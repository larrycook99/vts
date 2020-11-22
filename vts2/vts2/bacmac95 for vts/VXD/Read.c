/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1997,  Microsoft Corporation
	
module:		READ.C
desc:		Win 95 NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	William Ingle, Jack Neyer
last edit:	24-Sep-99 [004] JN  add diagnostic stats
			31-Jul-98 [003] JN  support for ARCNET
								toss incoming packets properly per panix.com
			07-Jan-98 [002] JN  filter non-BACnet packets and toss them
			12-Dec-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
// This library was taken from the MicroSoft Windows 95 packet driver.
//
// This module provides the read functions for the driver and is meant to be 
// linked with the other modules for the driver to form the whole.
//
// Little else is changed in the driver, save some liberal comments to alleviate
// later confusion.

#include <basedef.h>
#include <vmm.h>
#include <vwin32.h>
#include <winerror.h>
#include <ndis.h>

#include "debug.h"
#include "bacmac95.h"
#include "ntddpack.h"

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG


DWORD _stdcall MyPageLock(DWORD, DWORD);
void  _stdcall MyPageUnlock(DWORD, DWORD);

VOID PacketAllocatePacketBuffer(PNDIS_STATUS	pStatus,
							POPEN_INSTANCE		pOpen,
							PNDIS_PACKET		*lplpPacket,
							PDIOCPARAMETERS		pDiocParms,
							DWORD				FunctionCode )
/*++

Routine Description:
	Allocate an NDIS buffer.  

Arguments:
	pStatus
		return status
	pOpen
		adapter open handle
	*lplpPacket
		pointer to the packet buffer list from which to get one
	pDiocParms
		pointer to IOCTL params
	FunctionCode
		IOCTL function

Return Value:
    Status is returned.

--*/

{	PNDIS_BUFFER		pNdisBuffer;
	PPACKET_RESERVED	pReserved;

	TRACE_ENTER( "PacketAllocatePacket" );
//  Try to get a packet from our list of free ones
	NdisAllocatePacket( pStatus, lplpPacket, pOpen->PacketPool );
	if ( *pStatus != NDIS_STATUS_SUCCESS ) 
	{	IF_VERY_LOUD( "Read- No free packets" );
		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;
		if (FunctionCode==IOCTL_PROTOCOL_READ)	//						***004 Begin
			pOpen->RxNoNDISBuff++;				
		else if (FunctionCode==IOCTL_PROTOCOL_WRITE) 
			pOpen->TxNoNDISBuff++;				//						***004 End
		return;
	}
// Initialize Reserved LIST_ELEMENT
	InitializeListHead( &(RESERVED(*lplpPacket)->ListElement) );
	pReserved = RESERVED(*lplpPacket);
// Buffers used asynchronously must be page locked
	switch ( FunctionCode )
	{
	case IOCTL_PROTOCOL_READ:
		pReserved->lpBuffer = (PVOID)PacketPageLock((PVOID)pDiocParms->lpvOutBuffer,pDiocParms->cbOutBuffer );
		pReserved->cbBuffer = pDiocParms->cbOutBuffer;
		break;

	case IOCTL_PROTOCOL_WRITE:
		pReserved->lpBuffer = (PVOID)PacketPageLock(pDiocParms->lpvInBuffer,pDiocParms->cbInBuffer );
		pReserved->cbBuffer = pDiocParms->cbInBuffer;
		break;

	default:
		IF_TRACE_MSG( "Allocate- Invalid FunctionCode %x", FunctionCode );
// recycle the packet
		NdisReinitializePacket( *lplpPacket );
// Put the packet on the free queue
		NdisFreePacket( *lplpPacket );
		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;
		*pStatus = NDIS_STATUS_NOT_ACCEPTED;
		return;
	}

// Buffers used by pended I/O operations must be page locked
	pReserved->lpcbBytesReturned=(PVOID)PacketPageLock( (PVOID)pDiocParms->lpcbBytesReturned, sizeof(DWORD) );
	pReserved->lpoOverlapped=(PVOID)PacketPageLock( (PVOID)pDiocParms->lpoOverlapped, sizeof(OVERLAPPED) );
	pReserved->hDevice=pDiocParms->hDevice;
	pReserved->tagProcess= pDiocParms->tagProcess;
// Here we allocate a buffer descriptor for our page locked buffer given us by the
// client application.
	NdisAllocateBuffer(	pStatus, 
						&pNdisBuffer, 
						pOpen->BufferPool, 
						(PVOID)pReserved->lpBuffer,
						pDiocParms->cbOutBuffer );

	if ( *pStatus != NDIS_STATUS_SUCCESS )
	{	IF_TRACE( "Read- No free buffers" );
// recycle the packet
		NdisReinitializePacket(*lplpPacket);
// Put the packet on the free queue
		NdisFreePacket(*lplpPacket);
		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;
		return;
	}
// Attach buffer to Packet
	NdisChainBufferAtFront( *lplpPacket, pNdisBuffer );
// Let's only print this when PACKET_DEBUG_VERY_LOUD is set
	IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD ) 
	{	IF_TRACE_MSG( " lplpPacket : %lx", lplpPacket );		
		IF_TRACE_MSG( "   lpPacket : %lx", *lplpPacket );		
		IF_TRACE_MSG3( "pNdisBuffer : %lx  %lx  %lx", pNdisBuffer, (*lplpPacket)->Private.Head, (*lplpPacket)->Private.Tail );
		IF_TRACE_MSG( "   Reserved : %lx", pReserved );		
		IF_TRACE_MSG4( "   lpBuffer : %lx  %lx  %lx  %lx", pReserved->lpBuffer, pNdisBuffer->VirtualAddress, pDiocParms->lpvOutBuffer, pDiocParms->lpvInBuffer );
		IF_TRACE_MSG3( "   cbBuffer : %lx  %lx  %lx", pReserved->cbBuffer, pDiocParms->cbOutBuffer, pDiocParms->cbInBuffer );
		IF_TRACE_MSG2( " lpcbBytes  : %lx  %lx", pReserved->lpcbBytesReturned, pDiocParms->lpcbBytesReturned );
		IF_TRACE_MSG2( " lpoOverlap : %lx  %lx", pReserved->lpoOverlapped, pDiocParms->lpoOverlapped );
		IF_TRACE_MSG2( "    hDevice : %lx  %lx", pReserved->hDevice, pDiocParms->hDevice );
		IF_TRACE_MSG2( " tagProcess : %lx  %lx", pReserved->tagProcess, pDiocParms->tagProcess );
	}
	PACKETASSERT( pReserved->lpBuffer );
	PACKETASSERT( pReserved->cbBuffer );
	PACKETASSERT( pReserved->lpcbBytesReturned );
	PACKETASSERT( pReserved->lpoOverlapped );
	PACKETASSERT( pReserved->hDevice == pDiocParms->hDevice );
	PACKETASSERT( pReserved->tagProcess == pDiocParms->tagProcess );
	PACKETASSERT( pNdisBuffer == (*lplpPacket)->Private.Head );
	PACKETASSERT( pNdisBuffer->VirtualAddress == pReserved->lpBuffer );

	TRACE_LEAVE( "PacketAllocatePacket" );
	return;
}

DWORD PacketRead(POPEN_INSTANCE	Open,
				DWORD  			dwDDB,
				DWORD  			hDevice,
		  		PDIOCPARAMETERS pDiocParms)

/*++

Routine Description:

    This is the dispatch routine for read requests.
    These requests complete successfully.

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
    Status is returned.

--*/

{	NDIS_STATUS		Status;
	PNDIS_PACKET	pPacket;

	TRACE_ENTER( "PacketRead" );
//  See if the buffer is at least big enough to hold the
//  ethernet header
	if ( pDiocParms->cbOutBuffer < ETHERNET_HEADER_LENGTH ) 
	{
// Need bigger buffer		
		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;
		IF_VERY_LOUD( "Read- Buffer too small" );
		TRACE_LEAVE( "ReadPacket" );
		return NDIS_STATUS_SUCCESS;
	}
	PacketAllocatePacketBuffer( &Status, Open, &pPacket, pDiocParms, IOCTL_PROTOCOL_READ );
	if ( Status == NDIS_STATUS_SUCCESS )
	{
//  Put this packet in a list of pending reads.
//  The receive indication handler will attemp to remove packets
//  from this list for use in transfer data calls
		PACKETASSERT( Open != NULL );
		PACKETASSERT( pPacket != NULL );
		NdisAcquireSpinLock( &Open->RcvQSpinLock );
		InsertTailList( &Open->RcvList, &RESERVED(pPacket)->ListElement );
		NdisReleaseSpinLock( &Open->RcvQSpinLock );
		IF_TRACE_MSG2( "RcvList Link : %lx  %lx", Open->RcvList.Blink, &RESERVED(pPacket)->ListElement );
		PACKETASSERT( Open->RcvList.Blink == &RESERVED(pPacket)->ListElement );
		PACKETASSERT( &(Open->RcvList) == RESERVED(pPacket)->ListElement.Flink );
	}
	TRACE_LEAVE( "PacketRead" );
	return(-1);		// This will make DeviceIOControl return ERROR_IO_PENDING
}

NDIS_STATUS NDIS_API PacketReceiveIndicate (
						IN NDIS_HANDLE ProtocolBindingContext,
						IN NDIS_HANDLE MacReceiveContext,
						IN PVOID       pvHeaderBuffer,
						IN UINT        uiHeaderBufferSize,
						IN PVOID       pvLookAheadBuffer,
						IN UINT        uiLookaheadBufferSize,
						IN UINT        uiPacketSize)
/*++

Routine Description:
	This is the protocol's receive packet entry. We're assurred we have
    at least look-ahead bytes at this point.

	*** Registered as an entry point with the Driver Object. ***
	(ReceiveHandler)

Arguments:
	ProtocolBindingContext
		The handle of the protocol binding
	MacReceiveContext
		The handle to a NIC
	pvHeaderBuffer
		The base address if the header buffer
	uiHeaderBufferSize
		The size of the header buffer
	pvLookAheadBuffer
		Pointer to the look-ahead buffer
	uiLookaheadBufferSize
		The look-ahead buffer size
	uiPacketSize
		The network packet size

Return Value:
    Status is returned.

--*/

#define pOpen ((POPEN_INSTANCE)ProtocolBindingContext)

{	PLIST_ENTRY			PacketListEntry;
	PNDIS_PACKET   		pPacket;
	ULONG          		ulSizeToTransfer;
	NDIS_STATUS    		Status;
	UINT           		uiBytesTransferred;
	PPACKET_RESERVED	pReserved;
	PNDIS_BUFFER		pNdisBuffer;
	PVOID				pvActualVirtualAddress;
	UINT				uiActualLength;
	UINT				n;										//***003

// we wait to print the trace to see if we even have a buffer
	PACKETASSERT( (pOpen != NULL) );
//															***002 Begin
#define BACnetLSAP	0x82
//															***003 Begin
#define BACnetArcnetSC	0xCD
/*		if ((uiLookaheadBufferSize<3)||(uiHeaderBufferSize<14)) return NDIS_STATUS_SUCCESS;  //toss it
		if (((((UCHAR *)pvHeaderBuffer)[12]==0xFF)&&(((UCHAR *)pvHeaderBuffer)[13]==0xFF)) || //IPX?
			((((UCHAR *)pvLookAheadBuffer)[0]!=BACnetLSAP)||(((UCHAR *)pvLookAheadBuffer)[1]!=BACnetLSAP)||(((UCHAR *)pvLookAheadBuffer)[2]!=3)))	//not a BACnet LSAP?
		{	return NDIS_STATUS_SUCCESS;					//toss it
		}
*/
//															***002 End
//	if (uiHeaderBufferSize==14)							//Ethernet
	IF_TRACE_MSG2( "         Header : %lx  %lx", pvHeaderBuffer, uiHeaderBufferSize );
	IF_TRACE_MSG2( "      LookAhead : %lx  %lx", pvLookAheadBuffer, uiLookaheadBufferSize );
	IF_TRACE_MSG3( " Header[0][1][2]: %x  %x  %x", (BYTE)(((UCHAR *)pvHeaderBuffer)[0]), (BYTE)(((UCHAR *)pvHeaderBuffer)[1]), (BYTE)(((UCHAR *)pvHeaderBuffer)[2]) );
	IF_TRACE_MSG4( "LookAhead[0]-[3]: %x  %x  %x  %x", (BYTE)(((UCHAR *)pvLookAheadBuffer)[0]), (BYTE)(((UCHAR *)pvLookAheadBuffer)[1]), 
			                                      (BYTE)(((UCHAR *)pvLookAheadBuffer)[2]), (BYTE)(((UCHAR *)pvLookAheadBuffer)[3]) );
	if (pOpen->NdisMedium==NdisMedium802_3)				//Ethernet
	{	IF_TRACE ("  ReceiveIndicate: Ethernet Packet");
		if (uiLookaheadBufferSize<3) return NDIS_STATUS_NOT_ACCEPTED;	//per panix.com		//toss it
		n=0;											//index for LLC
	}
//	else if (uiHeaderBufferSize==3)						//ARCNET
	else if (pOpen->NdisMedium==NdisMediumArcnet878_2)	//ARCNET
//		Note: It seems this should actually be NdisMediumArcnetRaw..
//				however, NDIS does not seem to support raw arcnet.
//				This will not work unless a modified NIC driver
//				jimmies around the 878.2 packets is used.
	{	IF_TRACE ("  ReceiveIndicate: ARCNET Packet");
		if (((UCHAR *)pvHeaderBuffer)[2]!=BACnetArcnetSC)																	//not an ARCNET packet
		{	IF_TRACE ("  ReceiveIndicate: Packet Tossed..not BACnet");
			pOpen->RxNonBACnet++;						//					***004
			return NDIS_STATUS_NOT_ACCEPTED;			//per panix.com		//toss it
		}
		n=0;											//index for LLC
	}
	else
	{	pOpen->RxNonBACnet++;							//					***004
		return NDIS_STATUS_NOT_ACCEPTED;				//per panix.com		//toss it
	}
	if ((((UCHAR *)pvLookAheadBuffer)[n]!=BACnetLSAP)||(((UCHAR *)pvLookAheadBuffer)[n+1]!=BACnetLSAP)||(((UCHAR *)pvLookAheadBuffer)[n+2]!=3))	//not a BACnet LSAP?
	{	IF_TRACE ("  ReceiveIndicate: Packet Tossed..not BACnet");
		pOpen->RxNonBACnet++;							//					***004
		return NDIS_STATUS_NOT_ACCEPTED;				//per panix.com		//toss it
	}

//															***003 End
	//  See if there are any pending reads that we can satisfy
	NdisAcquireSpinLock( &pOpen->RcvQSpinLock );
	PacketListEntry = PacketRemoveHeadList( &pOpen->RcvList );
	NdisReleaseSpinLock( &pOpen->RcvQSpinLock );
	if ( PacketListEntry == NULL )
	{	pOpen->RxNoUserBuff++;							//								***004
		return NDIS_STATUS_NOT_ACCEPTED;				//per panix.com		//toss it	***003
	}
	TRACE_ENTER( "IndicateReceive" );
	pReserved = CONTAINING_RECORD( PacketListEntry, PACKET_RESERVED, ListElement );
	pPacket   = CONTAINING_RECORD( pReserved, NDIS_PACKET, ProtocolReserved );
// Let's only print this when PACKET_DEBUG_VERY_LOUD is set
/*	IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )						***003 Begin 
	{	IF_TRACE_MSG( "   Reserved : %l2", pReserved );
		IF_TRACE_MSG( "    pPacket : %1x", pPacket );
		IF_TRACE_MSG2( "     Header : %lx  %lx", pvHeaderBuffer, uiHeaderBufferSize );
		IF_TRACE_MSG2( "  LookAhead : %lx  %lx", pvLookAheadBuffer, uiLookaheadBufferSize );
		IF_TRACE_MSG( " PacketSize : %lx", uiPacketSize );
	}
*/
	IF_TRACE_MSG( "   Reserved : %l2", pReserved );
	IF_TRACE_MSG( "    pPacket : %lx", pPacket );
//																	***003 End
	PACKETASSERT( (pReserved != NULL) );
	PACKETASSERT( (pPacket != NULL) );
// Clear total
	uiBytesTransferred = 0;
// Get pointer to private buffer
	pNdisBuffer = pPacket->Private.Head;
// Save private buffer's start address
	pvActualVirtualAddress	= pNdisBuffer->VirtualAddress;
	uiActualLength=pNdisBuffer->Length;
// Copy header buffer into client buffer
	if ( uiHeaderBufferSize > 0 ) 
	{	if ( uiHeaderBufferSize > pNdisBuffer->Length )
			uiHeaderBufferSize = pNdisBuffer->Length;
// copy the header buffer
		NdisMoveMemory( pNdisBuffer->VirtualAddress, pvHeaderBuffer, uiHeaderBufferSize );
// update bytes transferred
		uiBytesTransferred += uiHeaderBufferSize;
// Advance the NDIS_BUFFER address to the end of the header
		(BYTE *)(pNdisBuffer->VirtualAddress) += uiHeaderBufferSize;
// Adjust the NDIS_BUFFER Length
		pNdisBuffer->Length -= uiHeaderBufferSize;
	}
// Copy look ahead buffer into client buffer
	if ( uiLookaheadBufferSize > 0 )
	{	if ( uiLookaheadBufferSize > pNdisBuffer->Length )
			uiLookaheadBufferSize = pNdisBuffer->Length;
		NdisMoveMemory( pNdisBuffer->VirtualAddress, pvLookAheadBuffer, uiLookaheadBufferSize );
// update bytes transferred
		uiBytesTransferred += uiLookaheadBufferSize;
// Advance the NDIS_BUFFER address to the end of the Lookahead data
		(BYTE *)(pNdisBuffer->VirtualAddress) += uiLookaheadBufferSize;
// Adjust the NDIS_BUFFER Length
		pNdisBuffer->Length -= uiLookaheadBufferSize;
	}
// Save bytes transferred to client buffer so far
	*(pReserved->lpcbBytesReturned) = uiBytesTransferred;
// Copy any remaining bytes in packet
	if ( uiLookaheadBufferSize < uiPacketSize )
	{	if ( uiPacketSize >= 1500 )
		{	_asm { int 3 }
		}
// Adjust packet size by the look ahead buffer
		ulSizeToTransfer = uiPacketSize - uiLookaheadBufferSize;
// Make sure we only copy as many bytes as the client buffer can handler
		if ( ulSizeToTransfer > pNdisBuffer->Length )
			ulSizeToTransfer = pNdisBuffer->Length;
//  Call the Mac to transfer the packet
		NdisTransferData(	&Status,					// ndis status
							pOpen->AdapterHandle,		// from NdisOpenAdapter
							MacReceiveContext,			// handle from NIC
							uiLookaheadBufferSize,		// offset from start of buffer
							ulSizeToTransfer,			// number of bytes to copy
							pPacket,					// packet descriptor pointer
							&uiBytesTransferred );		// number of bytes actually copied
// Restore the private buffer's address pointer
		pNdisBuffer->VirtualAddress = pvActualVirtualAddress;
		pNdisBuffer->Length			= uiActualLength;
		if ( Status != NDIS_STATUS_PENDING ) 
		{
//  If it didn't pend, call the completeion routine now
			PacketTransferDataComplete(	pOpen,				// protocol context
										pPacket,			// packet descriptor pointer
										Status,				// ndis status
										uiBytesTransferred );// number of bytes actually copied
		}
	}
	else
	{
// The entire packet was in the look ahead buffer
		Status = NDIS_STATUS_SUCCESS;
// Restore the private buffer's address pointer
		pNdisBuffer->VirtualAddress = pvActualVirtualAddress;
		pNdisBuffer->Length = uiActualLength;
		PacketTransferDataComplete(	pOpen,			// protocol context
									pPacket,		// packet descriptor pointer
									Status,			// ndis status
									0 );			// number of bytes copied by NdisTransferData
	}
	TRACE_LEAVE( "IndicateReceive" );
	return NDIS_STATUS_SUCCESS;
}

VOID NDIS_API PacketTransferDataComplete (	
								IN NDIS_HANDLE   ProtocolBindingContext,
								IN PNDIS_PACKET  pPacket,
								IN NDIS_STATUS   Status,
								IN UINT          uiBytesTransferred)
/*++

Routine Description:
	A transfer of data from the MAC has completed.

	*** Registered as an entry point with the Driver Object. ***

Arguments:
	ProtocolBindingContext
		The protocol handle
	pPacket
		A pointer to an NDIS Packet
	Status
		The incoming NDIS status
	uiBytesTransferred
		The number of bytes transferred

Return Value:
    Status is returned.

--*/

{	PPACKET_RESERVED	pReserved;
	OVERLAPPED*			pOverlap;
	PNDIS_BUFFER		pNdisBuffer;

	TRACE_ENTER( "TransferDataComplete" );
	pReserved = (PPACKET_RESERVED) pPacket->ProtocolReserved;
	pOverlap  = (OVERLAPPED *) pReserved->lpoOverlapped;
	if (Status==STATUS_SUCCESS) pOpen->RxCount++;//count BACnet packets	***004

	PACKETASSERT( (pOpen != NULL) );
	PACKETASSERT( (pReserved != NULL) );
	PACKETASSERT( (pOverlap != NULL) );
// Let's only print this when PACKET_DEBUG_VERY_LOUD is set
	IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD ) 
	{	IF_TRACE_MSG( "     Status : %lx", Status );
		IF_TRACE_MSG( "BytesXfered : %lx", uiBytesTransferred );
		IF_TRACE_MSG( "Byte Offset : %lx", *(pReserved->lpcbBytesReturned) );
	}
// free buffer descriptor
	NdisUnchainBufferAtFront( pPacket, &pNdisBuffer );
	PACKETASSERT( (pNdisBuffer != NULL) );
	if ( pNdisBuffer )
		NdisFreeBuffer( pNdisBuffer );
// increment total bytes returned and set in overlapped structure
	*(pReserved->lpcbBytesReturned) += uiBytesTransferred;
	pOverlap->O_InternalHigh = *(pReserved->lpcbBytesReturned);
// The internal member of overlapped structure contains
// a pointer to the event structure that will be signalled,
// resuming the execution of the waitng GetOverlappedResult
// call.
	VWIN32_DIOCCompletionRoutine( pOverlap->O_Internal );
// Unlock buffers	

	PacketPageUnlock( pReserved->lpBuffer, pReserved->cbBuffer );
	PacketPageUnlock( pReserved->lpcbBytesReturned, sizeof(DWORD) );
	PacketPageUnlock( pReserved->lpoOverlapped, sizeof(OVERLAPPED) );
// recycle the packet
	NdisReinitializePacket( pPacket );
// Put the packet on the free queue
	NdisFreePacket( pPacket );
	TRACE_LEAVE( "TransferDataComplete" );
	return;
}

VOID NDIS_API PacketReceiveComplete( IN NDIS_HANDLE ProtocolBindingContext )
/*++

Routine Description:
	This is the receive complete handler for the protocol.

	*** Registered as an entry point with the Driver Object. ***

Arguments:
	ProtocolBindingContext
		The protocol handle.

Return Value:

--*/

{	IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD ) 
	{	TRACE_ENTER( "ReceiveComplete" );
		TRACE_LEAVE( "ReceiveComplete" );
	}
	return;
}

