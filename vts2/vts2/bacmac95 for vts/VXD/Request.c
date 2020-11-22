/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1997,  Microsoft Corporation
	
module:		REQUEST.C
desc:		Win 95 NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	William Ingle, Jack Neyer
last edit:	24-Sep-99 [002] JN  add diagnostic stats
			12-Dec-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
// This library was taken from the MicroSoft Windows 95 packet driver.
//
// This module provides the some of the IOCTL functions for the driver and is meant to be 
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
#include "bacoid.h"							//							***002
#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

DWORD PacketRequest(	POPEN_INSTANCE		Open,
						DWORD  				FunctionCode,
						DWORD  				dwDDB,
						DWORD				hDevice,
						PDIOCPARAMETERS 	pDiocParms )
/*++

Routine Description:
	This is the request handler for the driver.

Arguments:
	Open
		The instance of the bound adapter
	FunctionCode
		The IOCTL function code
	dwDDB
		?
	hDevice
		The device handle
	pDiocParms
		The IOCTL params

Return Value:
    Status is returned.

--*/

{	PLIST_ENTRY			RequestListEntry;
	PINTERNAL_REQUEST	pRequest;
	PPACKET_RESERVED	pReserved;
	PPACKET_OID_DATA	OidData;
	NDIS_STATUS			Status;
				
	TRACE_ENTER( "Request Packet" );
// Acquire request element from list
	NdisAcquireSpinLock( &Open->RequestSpinLock );
	RequestListEntry = PacketRemoveHeadList(&Open->RequestList);
	NdisReleaseSpinLock( &Open->RequestSpinLock );
	if ( RequestListEntry == NULL ) 
	{	IF_TRACE( "Request List Error" );
// Place the error in the buffer
		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;
		TRACE_LEAVE( "Request Packet" );
		return NDIS_STATUS_SUCCESS;
	}
	pReserved = CONTAINING_RECORD( RequestListEntry, PACKET_RESERVED, ListElement );
	pRequest = CONTAINING_RECORD( pReserved, INTERNAL_REQUEST, Reserved );
	OidData = (PPACKET_OID_DATA)(pDiocParms->lpvInBuffer);
	if (( pDiocParms->cbInBuffer == pDiocParms->cbOutBuffer )&&
		( pDiocParms->cbInBuffer >= sizeof(PACKET_OID_DATA) - 1 + OidData->Length)) 
	{
// The buffer is valid
		pReserved->lpBuffer=(PVOID)PacketPageLock(pDiocParms->lpvInBuffer,pDiocParms->cbInBuffer);
		pReserved->lpcbBytesReturned=(PVOID)PacketPageLock((PVOID)pDiocParms->lpcbBytesReturned,sizeof(DWORD));
		pReserved->lpoOverlapped=(PVOID)PacketPageLock((PVOID)pDiocParms->lpoOverlapped,sizeof(OVERLAPPED));
		pReserved->cbBuffer=pDiocParms->cbInBuffer;
		pReserved->hDevice=pDiocParms->hDevice;
		pReserved->tagProcess=pDiocParms->tagProcess;
		if (FunctionCode == IOCTL_PROTOCOL_SET_OID) 
		{	pRequest->Request.RequestType = NdisRequestSetInformation;
			pRequest->Request.DATA.SET_INFORMATION.Oid=OidData->Oid;
			pRequest->Request.DATA.SET_INFORMATION.InformationBufferLength=OidData->Length;
			pRequest->Request.DATA.SET_INFORMATION.InformationBuffer=OidData->Data;
/*
Note: This must be removed or the nmake process fails after the compile and link process
for some unexplainable reason. The error code is a LNK1181 error!
			IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )
			{	IF_TRACE_MSG2( "Request Set: Oid=%08lx, Length=%08lx",
								OidData->Oid,OidData->Length );
			}
*/
			if (OidData->Oid==OID_GEN_DIRECTED_FRAMES_RCV)	//			***002 Begin
			{	Open->RxCount=0;
				Status=NDIS_STATUS_SUCCESS;
				goto alldone;
			}
			if (OidData->Oid==OID_GEN_DIRECTED_FRAMES_XMIT)
			{	Open->TxCount=0;
				Status=NDIS_STATUS_SUCCESS;
				goto alldone;
			}
			if (OidData->Oid==OID_BACMAC_GET_PERF_DATA)
			{	Open->RxCount=0;
				Open->TxCount=0;
				Open->RxNonBACnet=0;
				Open->RxNoUserBuff=0;
				Open->RxNoNDISBuff=0;
				Open->TxNoNDISBuff=0;
				Status=NDIS_STATUS_SUCCESS;
				goto alldone;
			}												//			***002 End
		} 
		else if (FunctionCode == IOCTL_PROTOCOL_QUERY_OID)
		{	pRequest->Request.RequestType=NdisRequestQueryInformation;
			pRequest->Request.DATA.QUERY_INFORMATION.Oid=OidData->Oid;
			pRequest->Request.DATA.QUERY_INFORMATION.InformationBufferLength=OidData->Length;
			pRequest->Request.DATA.QUERY_INFORMATION.InformationBuffer=OidData->Data;
//																		***002 Begin
/*
Note: This must be removed or the nmake process fails after the compile and link process
for some unexplainable reason. The error code is a LNK1181 error!
			IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )		
			{	IF_TRACE_MSG3( "Request Query: Type:%d Oid=%08lx, Length=%08lx",
								NdisRequestQueryInformation,OidData->Oid,
								OidData->Length );
			}
*/
			if (OidData->Oid==OID_GEN_DIRECTED_FRAMES_RCV)
			{	ULONG *dp=(ULONG *)(OidData->Data);
				*dp=Open->RxCount;				//NOTE: This includes "reflected" broadcast packets sent from us
				Status=NDIS_STATUS_SUCCESS;
				pRequest->Request.DATA.QUERY_INFORMATION.BytesWritten=4;
				goto alldone;
			}
			if (OidData->Oid==OID_GEN_DIRECTED_FRAMES_XMIT)
			{	ULONG *dp=(ULONG *)(OidData->Data);
				*dp=Open->TxCount;
				Status=NDIS_STATUS_SUCCESS;
				pRequest->Request.DATA.QUERY_INFORMATION.BytesWritten=4;
				goto alldone;
			}
			if (OidData->Oid==OID_BACMAC_GET_PERF_DATA)
			{	PPERF_DATA	pd=(PPERF_DATA)(OidData->Data);
				pd->RxCount=Open->RxCount;							//NOTE: This includes "reflected" broadcast packets sent from us
				pd->TxCount=Open->TxCount;
				pd->RxNonBACnet=Open->RxNonBACnet;
				pd->RxNoUserBuff=Open->RxNoUserBuff;
				pd->RxNoNDISBuff=Open->RxNoNDISBuff;
				pd->TxNoNDISBuff=Open->TxNoNDISBuff;
				Status=NDIS_STATUS_SUCCESS;
				pRequest->Request.DATA.QUERY_INFORMATION.BytesWritten=sizeof(PERF_DATA);
				goto alldone;
			}
//																		***002 End
		}
		else
		{	pRequest->Request.RequestType=NdisRequestGeneric1;
			pRequest->Request.DATA.QUERY_INFORMATION.Oid=OidData->Oid;
			pRequest->Request.DATA.QUERY_INFORMATION.InformationBufferLength=OidData->Length;
			pRequest->Request.DATA.QUERY_INFORMATION.InformationBuffer=OidData->Data;
			IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )		
			{	IF_TRACE_MSG3( "Request Statistic: Type:%d Oid=%08lx, Length=%08lx",
								NdisRequestGeneric1,OidData->Oid,OidData->Length);
			}
		}
// submit the request
		NdisRequest( &Status, Open->AdapterHandle, &pRequest->Request );
alldone:								//								***002
		if ( Status != NDIS_STATUS_PENDING )
		{	PacketRequestComplete( Open, &pRequest->Request, Status );
			TRACE_LEAVE( "Request Packet" );
			return NDIS_STATUS_SUCCESS;
		}
		TRACE_LEAVE( "Request Packet" );
		return(-1);		// This will make DeviceIOControl return ERROR_IO_PENDING
	}
	else
	{	IF_TRACE_MSG4( "Request Buffer Error: In=%lx Out=%lx Size=%lx Length=%lx",
						pDiocParms->cbInBuffer,
						pDiocParms->cbOutBuffer,
						sizeof( PACKET_OID_DATA ),
						OidData->Length);
		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;
	}
	TRACE_LEAVE( "Request Packet" );
	return NDIS_STATUS_SUCCESS;
}

VOID NDIS_API PacketRequestComplete( 	IN NDIS_HANDLE		ProtocolBindingContext,
										IN PNDIS_REQUEST	NdisRequest,
										IN NDIS_STATUS		Status)
/*++

Routine Description:
	This is the request completion handler for the driver.

Arguments:
	ProtocolBindingContext
		The protocol handle
	NdisRequest
		The NDIS request
	Status
		The incoming NDIS status

Return Value:

    Status is returned.

--*/

{	POPEN_INSTANCE		Open;
	PINTERNAL_REQUEST	pRequest;
	PPACKET_RESERVED	pReserved;
	OVERLAPPED*			pOverlap;
	PPACKET_OID_DATA	oidData;

	TRACE_ENTER( "RequestComplete" );
	Open=(POPEN_INSTANCE)ProtocolBindingContext;
	pRequest=CONTAINING_RECORD( NdisRequest, INTERNAL_REQUEST, Request );
	pReserved=&pRequest->Reserved;
	pOverlap=(OVERLAPPED *) pReserved->lpoOverlapped;
	oidData=(PPACKET_OID_DATA)(pReserved->lpBuffer);
	#if DEBUG
		IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )		
		{	ULONG		i;
			UCHAR*	pcData = oidData->Data;

			DbgPrint( "Packet: OID=%lx Status=%lx Buffer Length=%ld Buffer=%lx", 
			oidData->Oid, Status, oidData->Length, pcData );
			for ( i=0; i<oidData->Length; i++, pcData++ )
			{	if ( i%16 == 0 )
					DbgPrint( "\r\nPacket: " );
				DbgPrint( "%02x ", *pcData );
			}
			DbgPrint( "\r\n" );
			IF_BREAK_SET;
		}
	#endif
	if ( Status == NDIS_STATUS_SUCCESS )
	{
// set total bytes returned
		*(pReserved->lpcbBytesReturned)	= oidData->Length + sizeof(PACKET_OID_DATA) - 1;
		pOverlap->O_InternalHigh		= *(pReserved->lpcbBytesReturned);
	}
	else
	{
// set total bytes returned
		*(pReserved->lpcbBytesReturned)	= 0;
		pOverlap->O_InternalHigh		= 0;
// return status in oidData if there is an error 
		oidData->Length = Status;
	}
// The internal member of overlapped structure contains
// a pointer to the event structure that will be signalled,
// resuming the execution of the waitng GetOverlappedResult
// call.
	VWIN32_DIOCCompletionRoutine( pOverlap->O_Internal );
// Unlock buffers	
	PacketPageUnlock( pReserved->lpBuffer, pReserved->cbBuffer );
	PacketPageUnlock( pReserved->lpcbBytesReturned, sizeof(DWORD) );
	PacketPageUnlock( pReserved->lpoOverlapped, sizeof(OVERLAPPED) );
// Return request element to list
	NdisAcquireSpinLock( &Open->RequestSpinLock );
	InsertTailList( &Open->RequestList, &pReserved->ListElement );
	NdisReleaseSpinLock( &Open->RequestSpinLock );
	TRACE_LEAVE( "RequestComplete" );
	return;
}
