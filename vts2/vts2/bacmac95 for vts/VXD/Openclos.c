/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1997,  Microsoft Corporation
	
module:		OPENCLOS.C
desc:		Win 95 NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	William Ingle, Jack Neyer
last edit:	24-Sep-99 [003] JN  add diagnostic stats
			30-Jul-98 [002] JN  support for ARCNET
			12-Dec-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
// This library was taken from the MicroSoft Windows 95 packet driver.
//
// This module provides the open/close functions for the driver and is meant to be 
// linked with the other modules for the driver to form the whole.
//
// Little else is changed in the driver, save some liberal comments to alleviate
// later confusion.

#include <basedef.h>
#include <vmm.h>
#include <ndis.h>
#include <vwin32.h>
#include "debug.h"
#include "bacmac95.h"

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

/*++

Routine Description:

	Yield Execution of the VxD for awhile.

--*/

void YieldExecution( void )
{	VMMCall(Release_Time_Slice);		//give up our time slice
	VMMCall(Begin_Nest_Exec);			//so we know where to restart
	VMMCall(Resume_Exec);				//so we restart
	VMMCall(End_Nest_Exec);				//indicate end
}

VOID NDIS_API PacketBindAdapter( OUT PNDIS_STATUS Status,
						 IN  NDIS_HANDLE  BindAdapterContext,
						 IN  PNDIS_STRING AdapterName,
						 IN  PVOID        SystemSpecific1,
						 IN  PVOID        SystemSpecific2 )

/*++

Routine Description:
	ProtocolBindAdapter is required on Win95.  This makes the driver 
    Plug-and-Play-ready and defers binding until the underlying NIC 
    driver(s) have initialized.

	*** Registered as an entry point with the Driver Object. ***

Arguments:
	Status
		Points to a variable in which ProtocolBindAdapter returns the status of its operation(s), 
		as one of the following:
		
		NDIS_STATUS_SUCCESS
			The driver completed initialization successfully if it was deferred from DriverEntry. 
			The driver bound itself to the given NIC driver specified at AdapterName.
		
		NDIS_STATUS_PENDING
			The protocol will complete the bind operation asynchronously with a call to 
			NdisCompleteBindAdapter when it is ready to accept receives from the underlying driver 
			and to send transmit, query, and set requests down to the underlying driver. 

		NDIS_STATUS_XXX or NTSTATUS_XXX
			The protocol's attempt to set up a binding failed or the protocol could not allocate 
			the resources it needed to carry out network I/O operations. Usually, such an error 
			status is propagated from an NdisXxx function or a kernel-mode support routine. 

	BindContext
		Specifies a handle, supplied by NDIS, that the protocol passes subsequently to 
		NdisCompleteBindAdapter.  

	AdapterName
		Points to a counted, zero-terminated Unicode string naming an underlying NIC driver or 
		virtual NIC driver to which ProtocolBindAdapter should bind. 

	SystemSpecific1 (not used?)
		Specifies a registry path pointer that is a required parameter to 
		NdisOpenProtocolConfiguration. 

	SystemSpecific2
		Reserved for system use.  

Return Value:

    Status is returned.

--*/

{	PDEVICE_EXTENSION	pde;
	POPEN_INSTANCE		oiNew;
	NDIS_STATUS			nsErrorStatus, nsOpenStatus;
	UINT           		uiMedium;
//	NDIS_MEDIUM    		nmMediumArray=NdisMedium802_3;				//	***002
	NDIS_MEDIUM    		nmMediumArray[2]={							//	***002 Begin
											NdisMedium802_3,
											NdisMediumArcnet878_2};	//	***002 End
//		Note: It seems this should actually be NdisMediumArcnetRaw..
//				however, NDIS does not seem to support raw arcnet.
//				This will not work unless a modified NIC driver
//				jimmies around the 878.2 packets is used.
	UINT           		i;

	PWRAPPER_PROTOCOL_BLOCK			pWPBlock;
	PNDIS_PROTOCOL_CHARACTERISTICS	pNPChar;

	TRACE_ENTER( "BindAdapter" );
	pde = GlobalDeviceExtension;

//  Allocate some memory for the open structure
	NdisAllocateMemory( (PVOID *)&oiNew, sizeof( OPEN_INSTANCE ), 0, -1 );
	if ( oiNew == NULL ) 
	{
// not enough memory
		*Status = NDIS_STATUS_RESOURCES;
		return;
	}

	NdisZeroMemory( (PVOID)oiNew, sizeof( OPEN_INSTANCE ) );
// Allocate a packet pool for our xmit and receive packets
	NdisAllocatePacketPool( &nsErrorStatus,&(oiNew->PacketPool),TRANSMIT_PACKETS,sizeof(PACKET_RESERVED) );
	if ( nsErrorStatus != NDIS_STATUS_SUCCESS ) 
	{	IF_TRACE_MSG( "Failed to allocate packet pool AllocStatus=%x", nsErrorStatus );
// not enough memory
		NdisFreeMemory( oiNew, sizeof( OPEN_INSTANCE ) ,  0 );
		*Status = NDIS_STATUS_RESOURCES;
		TRACE_LEAVE( "BindAdapter" );
		return;
	}
// Allocate a buffer pool for our xmit and receive buffers
	NdisAllocateBufferPool( &nsErrorStatus,&(oiNew->BufferPool),TRANSMIT_PACKETS);
	if ( nsErrorStatus != NDIS_STATUS_SUCCESS )
	{	IF_TRACE_MSG( "Failed to allocate packet pool AllocStatus=%x", nsErrorStatus );
// not enough memory
		NdisFreePacketPool( oiNew->PacketPool );
		NdisFreeMemory( oiNew, sizeof( OPEN_INSTANCE ) ,  0 );
		*Status = NDIS_STATUS_RESOURCES;
		TRACE_LEAVE( "BindAdapter" );
		return;
	}

//  Initialize list to hold irp's want to reset the adapter
	NdisAllocateSpinLock( &(oiNew->ResetSpinLock));
	InitializeListHead( &(oiNew->ResetIrpList));

//  Initialize list for holding pending read requests
	NdisAllocateSpinLock( &(oiNew->RcvQSpinLock) )
	InitializeListHead( &(oiNew->RcvList) );

//  Initialize the request list
	NdisAllocateSpinLock( &(oiNew->RequestSpinLock) );
	InitializeListHead( &(oiNew->RequestList) );

//  link up the request stored in our open block
	for ( i=0;i<MAX_REQUESTS;i++ ) 
	{	InsertTailList( &(oiNew->RequestList), &(oiNew->Requests[i].Reserved.ListElement) );
	}
// Preset status
	oiNew->Status = NDIS_STATUS_PENDING;
// Save Binding Context
	oiNew->BindAdapterContext = BindAdapterContext;
//  Try to open the MAC
	NdisOpenAdapter( &nsOpenStatus,
					 &nsErrorStatus,
					 &oiNew->AdapterHandle,
					 &uiMedium,
//					 &nmMediumArray,										***002 Begin
//					 1,
					 &nmMediumArray[0],
					 2,							//							***002 End
					 pde->NdisProtocolHandle,
					 oiNew,
					 AdapterName,
					 0,
					 NULL );

	IF_TRACE_MSG( "Open Status                   : %lx", nsOpenStatus );
	IF_TRACE_MSG( "Error Status                  : %lx", nsErrorStatus );
	IF_TRACE_MSG( "Completion Status             : %lx", oiNew->Status );

	if ( nsOpenStatus == NDIS_STATUS_PENDING )
	{
// Wait for completion handler
		while ( oiNew->Status == NDIS_STATUS_PENDING )
			YieldExecution();
	}
	else
	{
// Complete synchronously.  Error returns are handled later.
		PacketBindAdapterComplete( oiNew, nsOpenStatus, nsErrorStatus );
	}
//  Test for protocol name and adapter name
	pWPBlock = ((PWRAPPER_OPEN_BLOCK)(oiNew->AdapterHandle))->ProtocolHandle;
	pNPChar  = &pWPBlock->ProtocolCharacteristics;

	IF_TRACE_MSG( "Protocol                      : %s",  pNPChar->Name.Buffer );
	IF_TRACE_MSG( "Protocol Handle               : %lx", pde->NdisProtocolHandle );
	IF_TRACE_MSG( "PWRAPPER_OPEN_BLOCK           : %lx", oiNew->AdapterHandle );
	IF_TRACE_MSG( "PWRAPPER_PROTOCOL_BLOCK       : %lx", pWPBlock );
	IF_TRACE_MSG( "NDIS_PROTOCOL_CHARACTERISTICS : %lx", pNPChar );
	IF_TRACE_MSG( "Name                          : %lx", &pNPChar->Name );
	IF_TRACE_MSG( "Adapter Name                  : %s",  AdapterName->Buffer );
//	Remember the medium								//				***002
	oiNew->NdisMedium=nmMediumArray[uiMedium];		//				***002
	oiNew->RxCount=0;								//				***003 Begin
	oiNew->TxCount=0;
	oiNew->RxNonBACnet=0;
	oiNew->RxNoUserBuff=0;
	oiNew->RxNoNDISBuff=0;
	oiNew->TxNoNDISBuff=0;							//				***003 End
//  Return the completion status.
	*Status = oiNew->Status;
	if ( *Status != NDIS_STATUS_SUCCESS ) 
	{
// The bind operation failed
		NdisFreeMemory( oiNew, sizeof( OPEN_INSTANCE ) ,  0 );
		IF_TRACE( "Bind Operation FAILED!" );
	}
	else
	{
// TODO:  insert binding specific queries and requests for configuration
// This is added here because we blocked (YieldExecution) until the open
// completed.  
	}
	TRACE_LEAVE( "BindAdapter" );
	return;
}

VOID NDIS_API PacketBindAdapterComplete(IN NDIS_HANDLE  ProtocolBindingContext,
									IN NDIS_STATUS  Status,
									IN NDIS_STATUS  OpenErrorStatus )
/*++

Routine Description:
	ProtocolOpenAdapterComplete is a required driver function that completes processing of a 
	binding operation for which NdisOpenAdapter returned NDIS_STATUS_PENDING. 
	
	When NdisOpenAdapter returns NDIS_STATUS_PENDING to the DriverEntry or ProtocolBindAdapter 
	function of an NDIS protocol driver, NDIS calls that driver's ProtocolOpenAdapterComplete 
	function when the asynchronous binding operation is complete. If the input Status is 
	NDIS_STATUS_SUCCESS, ProtocolBindAdapterComplete carries out whatever driver-determined 
	per-binding operations its DriverEntry or ProtocolBindAdapter function would have done in 
	a synchronous binding operation. If the driver has a ProtocolBindAdapter function, 
	ProtocolOpenAdapterComplete calls NdisCompleteBindAdapter with NDIS_STATUS_SUCCESS for the 
	Status and OpenStatus arguments and with the BindContext handle stored at 
	ProtocolBindingContext by the ProtocolBindAdapter function. Then, ProtocolOpenAdapterComplete 
	returns control. Otherwise, ProtocolOpenAdapterComplete cleans up any per-binding state 
	already set up by DriverEntry or ProtocolBindAdapter. After releasing any state set up by 
	ProtocolBindAdapter except for the BindContext handle, it calls NdisCompleteBindAdapter 
	with an appropriate error for Status and the handle before it returns control. 

	*** Registered as an entry point with the Driver Object. ***
	(OpenAdapterCompleteHandler)

Arguments:

	ProtocolBindingContext
		Specifies the handle to a protocol-allocated context area in which the protocol driver 
		maintains per-binding runtime state. The driver supplied this handle when it called 
		NdisOpenAdapter. 
	
	Status
		Specifies the final status of the NIC driver open operation, either NDIS_STATUS_SUCCESS 
		if the binding was established or an error status determined by the underlying driver. 
		The underlying NIC driver returns NDIS_STATUS_ADAPTER_NOT_READY for any requests it 
		receives while the open operation is pending. After ProtocolOpenAdapterComplete returns 
		control, the underlying driver cannot return this status. 
	
	OpenErrorStatus
		Specifies additional information about the reason for a failure if the value at Status 
		is not NDIS_STATUS_SUCCESS and if the underlying driver supplied this information. For 
		example, the driver of a Token Ring NIC might return a ring error that NDIS forwards to 
		ProtocolOpenAdapterComplete. This parameter can be NULL. 

Return Value:

    Status is returned.

--*/

{	POPEN_INSTANCE	Open;

	TRACE_ENTER( "BindAdapterComplete" );
	IF_TRACE_MSG2( "ErrorStatus=%x Status=%x", OpenErrorStatus, Status );

	Open = (POPEN_INSTANCE)ProtocolBindingContext;
	if ( Status == NDIS_STATUS_SUCCESS ) 
	{
// Insert New Adapter into list
		InsertTailList( &GlobalDeviceExtension->OpenList, &Open->ListElement );
	}
	else
	{
// bind operation failed
		PacketFreeResources( Open );
	}
//  Report status
	Open->Status = Status;

//  NdisCompleteBindAdapter MUST be called
	NdisCompleteBindAdapter( Open->BindAdapterContext, Status, OpenErrorStatus );
	TRACE_LEAVE( "BindAdapterComplete" );
	return;
}

VOID NDIS_API PacketUnbindAdapter( OUT PNDIS_STATUS	Status,
					 IN  NDIS_HANDLE	ProtocolBindingContext,
					 IN  NDIS_HANDLE	UnbindContext )
/*++

Routine Description:

	ProtocolUnbindAdapter is a required function in any driver that registers a ProtocolBindAdapter 
	function with the NDIS library. ProtocolUnbindAdapter must call NdisCloseAdapter, which can, in 
	turn, call the driver's ProtocolCloseAdapterComplete function with the ProtocolBindingContext 
	handle, so ProtocolUnbindAdapter cannot free the memory at ProtocolBindingContext unless 
	NdisCloseAdapter returns NDIS_STATUS_SUCCESS. 

	*** Registered as an entry point with the Driver Object. ***

Arguments:
	Status
		Specifies the status of the unbinding operation as the value returned by NdisCloseAdapter, 
		which can be either of the following:
			NDIS_STATUS_SUCCESS
				The binding was closed and all resources pertaining to that binding have been 
				released. 
			NDIS_STATUS_PENDING
				The request is being handled asynchronously, and the ProtocolCloseAdapterComplete 
				function will complete the operation. 
	
	ProtocolBindingContext
		Specifies the handle to a protocol-allocated context area in which the protocol driver 
		maintains per-binding runtime state. The driver supplied this handle when it called 
		NdisOpenAdapter. 

	UnbindContext
		Specifies a handle, supplied by NDIS, that the protocol passes subsequently to 
		NdisCompleteUnbindAdapter.


Return Value:

    Status is returned.

--*/

{	POPEN_INSTANCE	Open;
	NDIS_STATUS		nsCloseStatus;

	TRACE_ENTER( "CloseAdapter" );
	Open = (POPEN_INSTANCE)ProtocolBindingContext;

//  save the Unbind Context
	Open->BindAdapterContext = UnbindContext;

//  clean up any pending I/O requests
	PacketCleanUp( Status, Open );

//  preset adapter status 
	Open->Status = NDIS_STATUS_PENDING;

//  close the adapter
	NdisCloseAdapter( &nsCloseStatus, Open->AdapterHandle );

	if ( nsCloseStatus == NDIS_STATUS_PENDING )
	{	while ( Open->Status == NDIS_STATUS_PENDING )
			YieldExecution();
	}
	else
	{
// Complete synchronously
		PacketUnbindAdapterComplete( Open, nsCloseStatus );
	}
//  Return completion status.
	*Status = Open->Status;
	if ( *Status == NDIS_STATUS_SUCCESS )
	{	NdisFreeMemory( Open, sizeof( OPEN_INSTANCE ) ,  0 );
	}
	else
	{	IF_TRACE( "Unbind Operation FAILED!" );
	}

	TRACE_LEAVE( "CloseAdapter" );
	return;
}

VOID NDIS_API PacketUnbindAdapterComplete( IN NDIS_HANDLE ProtocolBindingContext,
							 IN NDIS_STATUS Status	)
/*++

Routine Description:

	NdisCompleteUnbindAdapter completes an unbinding operation for which the caller's 
	ProtocolUnbindAdapter function previously returned NDIS_STATUS_PENDING.  

	*** Registered as an entry point with the Driver Object. ***
	(CloseAdapterCompleteHandler)

Arguments:

	UnbindAdapterContext
		Specifies the ProtocolBindingContext handle passed in to ProtocolUnbindAdapter. 
	
	Status
		Specifies the Status of the completed unbind operation, always NDIS_STATUS_SUCCESS. 

Return Value:

    Status is returned.

--*/

{	POPEN_INSTANCE Open;

	TRACE_ENTER( "UnbindAdapterComplete" );
	Open = (POPEN_INSTANCE)ProtocolBindingContext;
	if ( Status == NDIS_STATUS_SUCCESS )
	{	PacketFreeResources( Open );
	}
	Open->Status = Status;
// Always Complete unbind
	NdisCompleteUnbindAdapter( Open->BindAdapterContext, Status );
	TRACE_LEAVE( "UnbindAdapterComplete" );
	return;
}

VOID PacketFreeResources( POPEN_INSTANCE Open )
/*++

Routine Description:
	Frees NDIS resources from our OPEN_INSTANCE.  The instance itself is released by
	the calling routine.

Arguments:
	Open
		a pointer to the OPEN_INSTANCE 

Return Value:

--*/

{
// Remove Adapter from global list
	RemoveEntryList( &Open->ListElement );
// Free Memory
	NdisFreeSpinLock( &Open->RequestSpinLock );
	NdisFreeSpinLock( &Open->RcvQSpinLock );
	NdisFreeSpinLock( &Open->ResetSpinLock );
	NdisFreeBufferPool( Open->BufferPool );
	NdisFreePacketPool( Open->PacketPool );
}

VOID PacketCleanUp(	PNDIS_STATUS Status,
				POPEN_INSTANCE	Open )
/*++

Routine Description:
    This is the dispatch routine for cleaning up create/open and close requests.

Arguments:
	Status
		the status of the request that prompts the cleanup
	Open
		a pointer to the OPEN_INSTANCE 

Return Value:

--*/

{	PLIST_ENTRY			PacketListEntry;
	PNDIS_PACKET   		pPacket;
	PPACKET_RESERVED	Reserved;

	TRACE_ENTER( "Cleanup" );

//  The open instance of the device is about to close
//  We need to complete all pending I/O requests
//  First we complete any pending read requests
	NdisAcquireSpinLock( &(Open->RcvQSpinLock) );
	while( (PacketListEntry = PacketRemoveHeadList( &(Open->RcvList) )) != NULL )
	{	IF_VERY_LOUD( "CleanUp - Completing read" );
		Reserved = CONTAINING_RECORD( PacketListEntry, PACKET_RESERVED, ListElement );
		pPacket  = CONTAINING_RECORD( Reserved, NDIS_PACKET, ProtocolReserved );
//  complete normally
		PacketTransferDataComplete( Open, pPacket, NDIS_STATUS_SUCCESS, 0 );
	}
	NdisReleaseSpinLock( &(Open->RcvQSpinLock) );
	PacketReset( Status, Open );
	TRACE_LEAVE( "Cleanup" );
	return;
}

VOID PacketReset( PNDIS_STATUS	pStatus,
			 POPEN_INSTANCE	pOpen )
/*++

Routine Description:
    This resets the adapter.

Arguments:
	Status
		the status of the request that prompts the cleanup
	Open
		a pointer to the OPEN_INSTANCE 

Return Value:

--*/

{	PLIST_ENTRY	ResetListEntry;

	TRACE_ENTER( "PacketReset" );
// Acquire request element from list
	NdisAcquireSpinLock( &pOpen->RequestSpinLock );
	ResetListEntry = PacketRemoveHeadList( &pOpen->RequestList );
	NdisReleaseSpinLock( &pOpen->RequestSpinLock );
	if ( ResetListEntry == NULL ) 
	{
// Place the error in the buffer
		*pStatus = NDIS_STATUS_RESOURCES;
		TRACE_LEAVE( "PacketReset" );
		return;
	}
// Insert Reset IRP into Request Queue
	NdisAcquireSpinLock( &pOpen->ResetSpinLock );
	InsertTailList( &pOpen->ResetIrpList, ResetListEntry );
	NdisReleaseSpinLock( &pOpen->ResetSpinLock );
// Reset the adapter
	NdisReset( pStatus, pOpen->AdapterHandle );
	if ( *pStatus != NDIS_STATUS_PENDING ) 
	{	PacketResetComplete( pOpen, *pStatus );
	}

	TRACE_LEAVE( "PacketReset" );
	return;
}
	
VOID NDIS_API PacketResetComplete( IN NDIS_HANDLE ProtocolBindingContext,
					 IN NDIS_STATUS	Status   )
/*++

Routine Description:
    This is the completion routine for an adapter reset.

Arguments:
	ProtocolBindingContext
		Specifies the ProtocolBindingContext handle passed in to ProtocolUnbindAdapter. 
	Status
		the status of the request that prompts the cleanup

Return Value:

--*/
{	POPEN_INSTANCE		Open;
	PLIST_ENTRY			ResetListEntry;

	TRACE_ENTER( "PacketResetComplete" );
	Open = (POPEN_INSTANCE)ProtocolBindingContext;
//  remove the reset request from the list
	NdisAcquireSpinLock( &Open->ResetSpinLock );

	ResetListEntry = PacketRemoveHeadList( &Open->ResetIrpList );

	NdisReleaseSpinLock( &Open->ResetSpinLock );

	if ( ResetListEntry == NULL ) 
	{
		IF_VERY_LOUD( "Reset List Empty Error" );
		TRACE_LEAVE( "PacketResetComplete" );
		return;
	}
// Acquire request element from list
	NdisAcquireSpinLock( &Open->RequestSpinLock );
	InsertTailList( &Open->RequestList, ResetListEntry );
	NdisReleaseSpinLock( &Open->RequestSpinLock );
	TRACE_LEAVE( "PacketResetComplete" );
	return;
}
