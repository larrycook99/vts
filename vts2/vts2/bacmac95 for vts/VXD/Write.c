/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1997,  Microsoft Corporation
	
module:		WRITE.C
desc:		Win 95 NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	William Ingle, Jack Neyer
last edit:	24-Sep-99 [003] JN  add diagnostic stats
			15-Oct-98 [002] JN  add more debug info for send
			12-Dec-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
// This library was taken from the MicroSoft Windows 95 packet driver.
//
// This module provides the write functions for the driver and is meant to be 
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
#include "ntddpack.h"

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

DWORD _stdcall MyPageLock(DWORD, DWORD);
void  _stdcall MyPageUnlock(DWORD, DWORD);

DWORD PacketWrite(
			POPEN_INSTANCE	Open,
			DWORD  			dwDDB,
			DWORD  			hDevice,
			PDIOCPARAMETERS	pDiocParms
	)


/*++

Routine Description:
    This is the dispatch routine for write requests.

	*** Registered as an entry point with the Driver Object. ***

Arguments:
	Open
		an instance of an adapter
	dwDDB
		?
	hDevice
		the device handle
	pDiocParms
		IOCTL parameters

Return Value:

--*/

{	PNDIS_PACKET	pPacket;
	PNDIS_BUFFER 	pNdisBuffer;
	NDIS_STATUS		Status;
	PPACKET_RESERVED	Reserved;						//					***002
	UCHAR			*pReserved;							//					***002


	TRACE_ENTER( "SendPacket" );
	PacketAllocatePacketBuffer( &Status, Open, &pPacket, pDiocParms, IOCTL_PROTOCOL_WRITE );
	if ( Status != NDIS_STATUS_SUCCESS )
	{	return 0;	// This will return immediately with no data written
	}
//																			***002 Begin
	Reserved = (PPACKET_RESERVED) pPacket->ProtocolReserved;
	pReserved=(UCHAR *)Reserved->lpBuffer;
	IF_TRACE_MSG2( "   lpBuffer [cbBuffer]: %lx [%lx]", Reserved->lpBuffer, Reserved->cbBuffer);
	IF_TRACE_MSG4( "       Transmit[0]-[3]: %x  %x  %x  %x", 
						(BYTE)(pReserved[0]), 
						(BYTE)(pReserved[1]), 
			            (BYTE)(pReserved[2]), 
						(BYTE)(pReserved[3]));
	IF_TRACE_MSG3( "       Transmit[4]-[6]: %x  %x  %x", 
						(BYTE)(pReserved[4]), 
						(BYTE)(pReserved[5]), 
			            (BYTE)(pReserved[6]));
//																			***002 End
//  Call the MAC
	NdisSend( &Status, Open->AdapterHandle, pPacket );
	if ( Status != NDIS_STATUS_PENDING ) 
	{
		//  The send didn't pend so call the completion handler now
		PacketSendComplete( Open, pPacket, Status );
	}
	TRACE_LEAVE( "SendPacket" );
	return(-1);		// This will make DeviceIOControl return ERROR_IO_PENDING
}

VOID NDIS_API PacketSendComplete(
					IN NDIS_HANDLE	ProtocolBindingContext,
					IN PNDIS_PACKET	pPacket,
					IN NDIS_STATUS	Status)
/*++

Routine Description:

Arguments:
	ProtocolBindingContext
		The protocol handle
	pPacket
		Pointer to the Packet
	Status
		NDIS Status

Return Value:

--*/
#define pOpen ((POPEN_INSTANCE)ProtocolBindingContext)		//				***003

{	PNDIS_BUFFER 		pNdisBuffer;
	PPACKET_RESERVED	Reserved = (PPACKET_RESERVED) pPacket->ProtocolReserved;

	TRACE_ENTER( "SendComplete" );
// free buffer descriptor
	
    if (Status==STATUS_SUCCESS)			//						***003 Begin
	{	pOpen->TxCount++;						
	}									//						***003 End

	NdisUnchainBufferAtFront( pPacket, &pNdisBuffer );
	if ( pNdisBuffer )
		NdisFreeBuffer( pNdisBuffer );
// The internal member of overlapped structure contains
// a pointer to the event structure that will be signalled,
// resuming the execution of the waiting GetOverlappedResult
// call.
	VWIN32_DIOCCompletionRoutine( Reserved->lpoOverlapped->O_Internal );
// Unlock buffers	
	PacketPageUnlock( Reserved->lpBuffer, Reserved->cbBuffer );
	PacketPageUnlock( Reserved->lpcbBytesReturned, sizeof(DWORD) );
	PacketPageUnlock( Reserved->lpoOverlapped, sizeof(OVERLAPPED) );
// recycle the packet
	NdisReinitializePacket(pPacket);
// Put the packet back in the free list
	NdisFreePacket(pPacket);
	TRACE_LEAVE( "SendComplete" );
	return;
}
