/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1990  Microsoft Corporation
	
module:		OPENCLOS.C
desc:		NT NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	Jack Neyer
last edit:	14-Sep-99 [010] JN  changes per MS v3.0
			13-Sep-99 [006] JN  expand performance stats
			02-Sep-99 [005] JN  add RxCount, TxCount, Tosscount to OPEN_INSTANCE
			07-May-99 [004] JN  Bug Fixes per Lew Perin:
								do IoCompleteRequest when error makes I/O complete impossible
								remove one of the IoMarkIrpPending calls in PacketCleanup
								call CONTAINING_RECORD twice in PacketCleanup
			30-Jul-98 [003] JN  support for ARCNET
			23-Feb-98 [002] JN  return NDIS_STATUS_SUCCESS if close adapter succeeds
			25-Oct-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
// This library was taken from the MicroSoft NT 4.0 DDK dated Jan '97 on the directory:
//               /src/network/packet/driver.
//
// It is an NT  ***Kernel mode only***  driver
//
// This module provides the open/close functions for the driver and is meant to be 
// linked with the other modules for the driver to form the whole.

// Little has changed in the driver, save some liberal comments to alleviate
// later confusion.

//#include "stdarg.h"
#include "ntddk.h"
#include "ntiologc.h"
#include "ndis.h"

#include "debug.h"
#include "bacmacnt.h"

///////////////////////////////////////////////////////////////////////
//	This is the dispatch routine for open requests.
//  These requests complete successfully.
//
//	*** Registered as an entry point with the Driver Object. ***
//
// in:	DeviceObject	pointer to a device object
//		Irp				pointer to the open request packet
//
// out:	Success/Failure

NTSTATUS PacketOpen(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)

{	PDEVICE_EXTENSION DeviceExtension;			//the device extension for this device
//	Note: The struct below is one that we define
    POPEN_INSTANCE    Open;						//the open instance
    PIO_STACK_LOCATION  IrpSp;
    NDIS_STATUS     Status;
    NDIS_STATUS     ErrorStatus;
    UINT            Medium;
//	NDIS_MEDIUM    		nmMediumArray=NdisMedium802_3;				//	***003
	NDIS_MEDIUM    		nmMediumArray[2]={							//	***003 Begin
											NdisMedium802_3,
											NdisMediumArcnet878_2};	//	***003 End
    UINT            i;

    IF_LOUD(DbgPrint("BACmac: OpenAdapter\n");)
    DeviceExtension = DeviceObject->DeviceExtension;	//our device extension
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
//  Allocate some memory for the open structure
    Open=ExAllocatePool(NonPagedPool,sizeof(OPEN_INSTANCE));
    if (Open==NULL)									//couldn't get any memory
	{	Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);	//complete the request	***004
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    RtlZeroMemory(Open,sizeof(OPEN_INSTANCE));
//  Save our open here
	IrpSp->FileObject->FsContext=Open;				//save pointer to it in IRP
    Open->DeviceExtension=DeviceExtension;			//save pointer to our device extension

//  Save the Irp here for the completion routine to retrieve
    Open->OpenCloseIrp=Irp;

//  Allocate a packet pool for our xmit and receive packets
    NdisAllocatePacketPool(&Status,&Open->PacketPool,TRANSMIT_PACKETS,sizeof(PACKET_RESERVED));	//16 of them right now
    if (Status != NDIS_STATUS_SUCCESS)
	{	IF_LOUD(DbgPrint("BACmac: Failed to allocate packet pool\n");)
        ExFreePool(Open);							//free up memory for our open struc
        Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);	//complete the request	***004
        return STATUS_INSUFFICIENT_RESOURCES;
    }

//  Initialize the list to hold irp's wanting to reset the adapter
    InitializeListHead(&Open->ResetIrpList);		

//  Initialize list for holding pending read requests
    KeInitializeSpinLock(&Open->RcvQSpinLock);
    InitializeListHead(&Open->RcvList);

//  Initialize the request list
    KeInitializeSpinLock(&Open->RequestSpinLock);
    InitializeListHead(&Open->RequestList);

//  link up the request stored in our open block
    for (i=0;i<MAX_REQUESTS;i++)				//16 of them
	{	ExInterlockedInsertTailList(&Open->RequestList,
									&Open->Requests[i].ListElement,
									&Open->RequestSpinLock);
    }

    IoMarkIrpPending(Irp);
    Irp->IoStatus.Status = STATUS_PENDING;

//  Try to open the MAC
    NdisOpenAdapter(&Status,					//return status
					&ErrorStatus,				//more error info if necessary
					&Open->AdapterHandle,		//put the binding handle here
					&Medium,					//return index into the medium array
//					 &nmMediumArray,										***003 Begin
//					 1,
					&nmMediumArray[0],
					2,							//							***003 End
					DeviceExtension->NdisProtocolHandle,	//the protocol handle we registered with
					Open,						//binding context info
					&DeviceExtension->AdapterName,	//adapter name to bind to
					0,							//open options
					NULL);						//addressing info


	Open->NdisMedium=nmMediumArray[Medium];		//we need this for reading	***003
	Open->RxCount=0;							//							***005							
	Open->TxCount=0;							//							***005							
	Open->RxNonBACnet=0;						//							***006 Begin
	Open->RxNoUserBuff=0;
	Open->RxNoNDISBuff=0;
	Open->TxNoNDISBuff=0;						//							***006 End
    if (Status!=NDIS_STATUS_PENDING)			//the open adapter request has already completed
	{	PacketOpenAdapterComplete(Open,Status,NDIS_STATUS_SUCCESS );
		return STATUS_SUCCESS;					//							***010
    }
    return(STATUS_PENDING);
}

///////////////////////////////////////////////////////////////////////
//	This is the completion routine for open adapter request.
//
//	*** Registered as an upper edge protocol entry point with NDIS. ***
//
//	in:	ProtocolBindingContext	the open instance for this request
//		Status					the incoming status of the requesu
//		OpenErrorStatus			the incoming error status

VOID PacketOpenAdapterComplete(
	IN NDIS_HANDLE ProtocolBindingContext,
    IN NDIS_STATUS  Status,
    IN NDIS_STATUS  OpenErrorStatus)

{	PIRP              Irp;
    POPEN_INSTANCE    Open;

    IF_LOUD(DbgPrint("BACmac: OpenAdapterComplete\n");)
    Open=(POPEN_INSTANCE)ProtocolBindingContext;
//  get the open irp
    Irp=Open->OpenCloseIrp;

    if (Status!=NDIS_STATUS_SUCCESS)			//open adapter failed
	{	IF_LOUD(DbgPrint("BACmac: OpenAdapterComplete-FAILURE\n");)
        NdisFreePacketPool(Open->PacketPool);	//free up the request packet pool we got
        ExFreePool(Open);						//free up the memory we got for the open object
    }
    Irp->IoStatus.Status=Status;
    Irp->IoStatus.Information = 0;				//									***010
    IoCompleteRequest(Irp, IO_NO_INCREMENT);	//complete the request
    return;
}

///////////////////////////////////////////////////////////////////////
//	This is the dispatch routine for close requests.
//  These requests complete successfully.
//
//	*** Registered as an entry point with the Driver Object. ***
//
// in:	DeviceObject	pointer to a device object
//		Irp				pointer to the close request packet
//
// out:	Success/Failure

NTSTATUS PacketClose(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)

{	POPEN_INSTANCE    Open;
    NDIS_STATUS     Status;
    PIO_STACK_LOCATION  IrpSp;

    IF_LOUD(DbgPrint("BACmac: CloseAdapter\n");)
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    Open=IrpSp->FileObject->FsContext;				//get open instance

//  Save the IRP
    Open->OpenCloseIrp=Irp;

    IoMarkIrpPending(Irp);
    Irp->IoStatus.Status = STATUS_PENDING;

//  Close the adapter
	NdisCloseAdapter(&Status,						//return status
					 Open->AdapterHandle);			//handle we got from open adapter

    if (Status!=NDIS_STATUS_PENDING)				//the close adapter request has completed already
	{	PacketCloseAdapterComplete(Open,Status);
		return STATUS_SUCCESS;						//					***002
    }
    return(STATUS_PENDING);
}

///////////////////////////////////////////////////////////////////////
//	This is the completion routine for close adapter request.
//
//	*** Registered as an upper edge protocol entry point with NDIS. ***
//
//	in:	ProtocolBindingContext	the open instance for this request
//		Status					the incoming status of the request

VOID PacketCloseAdapterComplete(IN NDIS_HANDLE  ProtocolBindingContext,IN NDIS_STATUS  Status)

{	POPEN_INSTANCE    Open;
    PIRP              Irp;

    IF_LOUD(DbgPrint("BACmac: CloseAdapterComplete\n");)
    Open= (POPEN_INSTANCE)ProtocolBindingContext;
    Irp=Open->OpenCloseIrp;
    NdisFreePacketPool(Open->PacketPool);			//free up the request packet pool we got
    ExFreePool(Open);								//free up the memory we got for the open object

    Irp->IoStatus.Status=STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;					//								***010
    IoCompleteRequest(Irp,IO_NO_INCREMENT);
    return;
}
/*																					***010 Begin
///////////////////////////////////////////////////////////////////////
//	This is the dispatch routine for cleaning up IRP requests when a handle closes.
//  These requests complete successfully.
//
//	*** Registered as an entry point with the Driver Object. ***
//
// in:	DeviceObject	pointer to a device object
//		FlushIrp		pointer to the cleanup request packet
//
// out:	success/failure

NTSTATUS PacketCleanup(IN PDEVICE_OBJECT DeviceObject,IN PIRP FlushIrp)

{	POPEN_INSTANCE      Open;
    PIO_STACK_LOCATION  IrpSp;
    PLIST_ENTRY         PacketListEntry;
    PNDIS_PACKET        pPacket;
    NDIS_STATUS         Status;
    PPACKET_RESERVED    Reserved;									//						***004

    IF_LOUD(DbgPrint("BACmac: Cleanup\n");)
    IrpSp = IoGetCurrentIrpStackLocation(FlushIrp);

    Open=IrpSp->FileObject->FsContext;

    IoMarkIrpPending(FlushIrp);
    FlushIrp->IoStatus.Status = STATUS_PENDING;
//
//  The open instance of the device is about to close
//  We need to complete all pending Irp's
//  First we complete any pending read requests
//
    while ((PacketListEntry=ExInterlockedRemoveHeadList(
								&Open->RcvList,
                                &Open->RcvQSpinLock)) != NULL)
	{	IF_LOUD(DbgPrint("BACmac: CleanUp - Completeing read\n");)
//																						***004 Begin
//	    pPacket=CONTAINING_RECORD(PacketListEntry,NDIS_PACKET,ProtocolReserved); //get base address of packet list
		Reserved=CONTAINING_RECORD(PacketListEntry,PACKET_RESERVED,ListElement);	//base address of PacketListEntry->ListElement
		pPacket=CONTAINING_RECORD(Reserved,NDIS_PACKET,ProtocolReserved);	//base address of PacketListEntry->ListElement->ProtocolReserved
//																						***004 End

//  Complete normally
		PacketTransferDataComplete(Open,pPacket,NDIS_STATUS_SUCCESS,0);		
	}

//    IoMarkIrpPending(FlushIrp);									***004
//    FlushIrp->IoStatus.Status = STATUS_PENDING;					***004

//  We now place the Irp on the Reset list
    ExInterlockedInsertTailList(
			&Open->ResetIrpList,
			&FlushIrp->Tail.Overlay.ListEntry,
            &Open->RequestSpinLock);

//  Now reset the adapter, the mac driver will complete any
//	pending requests we have made to it.

    NdisReset(&Status,Open->AdapterHandle);
    if (Status != NDIS_STATUS_PENDING)			//reset request has completed already
	{	IF_LOUD(DbgPrint("BACmac: Cleanup - ResetComplte being called\n");)
        PacketResetComplete(Open,Status);
    }
    return(STATUS_PENDING);						//why this status???
}
*/												//									***010 End
///////////////////////////////////////////////////////////////////////
//	This is the completion routine for reset adapter request.
//
//	*** Registered as an upper edge protocol entry point with NDIS. ***
//
//	in:	ProtocolBindingContext	the open instance for this request
//		Status					the incoming status of the request

VOID PacketResetComplete(IN NDIS_HANDLE ProtocolBindingContext,IN NDIS_STATUS Status)

{	POPEN_INSTANCE      Open;
    PIRP                Irp;
    PLIST_ENTRY         ResetListEntry;

    IF_LOUD(DbgPrint("BACmac: PacketResetComplete\n");)
    Open= (POPEN_INSTANCE)ProtocolBindingContext;

//  Remove the reset IRP from the list
    ResetListEntry=ExInterlockedRemoveHeadList(
                       &Open->ResetIrpList,
                       &Open->RequestSpinLock
                       );

#if DBG
    if (ResetListEntry==NULL)				//error
	{   DbgBreakPoint();
        return;
    }
#endif

    Irp=CONTAINING_RECORD(ResetListEntry,IRP,Tail.Overlay.ListEntry);	//get base address
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IF_LOUD(DbgPrint("BACmac: PacketResetComplete exit\n");)
    return;
}
