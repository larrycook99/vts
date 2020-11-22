/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1990  Microsoft Corporation
	
module:		READ.C
desc:		NT NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	Jack Neyer
last edit:	14-Sep-99 [010] JN  changes per MS v3.0
			13-Sep-99 [007] JN  expand performance stats
			02-Sep-99 [006] JN  add RxCount, TossCount
			10-May-99 [005] JN  more Bug Fixes per Lew Perin:
								add cancel pending reads IRP (registration)
			07-May-99 [004] JN  Bug Fixes per Lew Perin:
								do IoCompleteRequest when error makes I/O complete impossible
			31-Jul-98 [003] JN  support for ARCNET
								toss incoming packets properly per panix.com
			07-Jan-97 [002] JN  toss non-BACnet packets
			25-Oct-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
//																		***005 Begin
//NOTE: I never could get this to work. It always did a blue screen crash even when the cancel
//routine was called even if it only returned. This the places where the IoSetCancelRoutine 
//should be done are marked, but not activated in this version.
//																		***005 End

// This library was taken from the MicroSoft NT 4.0 DDK dated Jan '97 on the directory:
//               /src/network/packet/driver.
//
// It is an NT  ***Kernel mode only***  driver
//
// This module provides the read functions for the driver and is meant to be 
// linked with the other modules for the driver to form the whole.

// Little has changed in the driver, save some liberal comments to alleviate
// later confusion.


#include "stdarg.h"
#include "ntddk.h"
#include "ntiologc.h"
#include "ndis.h"

#include "debug.h"
#include "bacmacnt.h"

// Prototype declaration
VOID PacketCancelRoutine (IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp);		//			***010

///////////////////////////////////////////////////////////////////////
//	This is the dispatch routine for read requests.
//  These requests complete successfully.
//
//	*** Registered as an entry point with the Driver Object. ***
//
// in:	DeviceObject	pointer to a device object
//		Irp				pointer to the read request packet
//
// out:	Success/Failure

NTSTATUS PacketRead(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)

{	POPEN_INSTANCE      Open;
    PIO_STACK_LOCATION  IrpSp;
    PNDIS_PACKET        pPacket;
    PMDL                pMdl;
    NDIS_STATUS         Status;
	UINT				n;									//***003
//	KIRQL				Irql;								//***005

    IF_LOUD(DbgPrint("BACmac: Read\n");)
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    Open=IrpSp->FileObject->FsContext;
//  See if the buffer is at least big enough to hold the ethernet header
    if (IrpSp->Parameters.Read.Length < ETHERNET_HEADER_LENGTH)	//oops
	{   Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);	//complete the request	***004
        return STATUS_UNSUCCESSFUL;
    }

//  Allocate an MDL (memory descriptor list) to map the portion of 
//	the buffer following the header
    pMdl=IoAllocateMdl(
              MmGetMdlVirtualAddress(Irp->MdlAddress),	//the caller's buffer
              MmGetMdlByteCount(Irp->MdlAddress),
              FALSE,
              FALSE,
              NULL
              );

    if (pMdl==NULL)										//couldn't allocate mdl
	{	IF_LOUD(DbgPrint("BACmac: Read-Failed to allocate Mdl\n");)
        Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;	//			***010
		IoCompleteRequest(Irp, IO_NO_INCREMENT);	//complete the request	***004
        return STATUS_INSUFFICIENT_RESOURCES;		//						***010
    }

//  Build the mdl to point to the the portion of the buffer following
//  the header. This is done because the header and a look ahead buffer is
//	returned with the receive indicate, which must then ask the MAC layer to
//	get the rest of the packet.
//
//																		***003 Begin
	switch (Open->NdisMedium)					//do it this way for future expansion
	{
	case NdisMedium802_3:
		n=ETHERNET_HEADER_LENGTH;
		break;
	case NdisMediumArcnet878_2:
		n=ARCNET_HEADER_LENGTH;
		break;
	default:
		n=ETHERNET_HEADER_LENGTH;
	}
//																		***003 End
    IoBuildPartialMdl(
			Irp->MdlAddress,
			pMdl,
			((PUCHAR)MmGetMdlVirtualAddress(Irp->MdlAddress))+n,	//	***003
			0
			);

//  Clear the next link in the new MDL
    pMdl->Next=NULL;

//  Try to get a packet from our list of free ones
    NdisAllocatePacket(&Status,&pPacket,Open->PacketPool);

    if (Status!=NDIS_STATUS_SUCCESS)		//oops Note: JN this never happens!
	{   IF_LOUD(DbgPrint("BACmac: Read- No free packets\n");)
		Open->RxNoNDISBuff++;				//								***007
		IoFreeMdl(pMdl);					//free the mdl
        Irp->IoStatus.Status=STATUS_INSUFFICIENT_RESOURCES;	//				***010
		IoCompleteRequest(Irp, IO_NO_INCREMENT);	//complete the request	***004
        return STATUS_INSUFFICIENT_RESOURCES;		//						***010
    }

//  Get a pointer to the packet itself
    RESERVED(pPacket)->Irp=Irp;
    RESERVED(pPacket)->pMdl=pMdl;				//this points to the partial mdl that holds the packet data after the header

    IoMarkIrpPending(Irp);
    Irp->IoStatus.Status = STATUS_PENDING;

//  Attach our new MDL to the packet
    NdisChainBufferAtFront(pPacket,pMdl);

//  Put this packet in a list of pending reads.
//  The receive indication handler will attemp to remove packets
//  from this list for use in transfer data calls
    ExInterlockedInsertTailList(
        &Open->RcvList,
        &RESERVED(pPacket)->ListElement,
        &Open->RcvQSpinLock);
/*	IoAcquireCancelSpinLock (&Irql);							//						***005 Begin
	IoSetCancelRoutine(Irp,PacketReadCancel);					// Now it's cancelable
	IoReleaseCancelSpinLock(Irql);								//						***005 End
*/
    IoSetCancelRoutine(Irp, PacketCancelRoutine);				//????Will this work????***010
    return(STATUS_PENDING);
}

///////////////////////////////////////////////////////////////////////
//	This is the routine indicating that a packet has been received.
//
//	*** Registered as an upper edge protocol entry point with NDIS. ***
//
// in:	ProtocolBindingContext	the open instance of the device
//		MacReceiveContext		the mac receive handle
//		HeaderBuffer			pointer to the packet header (14 bytes)
//		HeaderBufferSize		14
//		LookAheadBuffer			pointer to the look ahead buffer (we don't use this)
//		LookaheadBufferSize		size of the look ahead buffer (ditto)	
//		PacketSize				size of the remainder of the packet (sans the header)
//
// out:	Success/Failure

NDIS_STATUS PacketReceiveIndicate (
    IN NDIS_HANDLE ProtocolBindingContext,
    IN NDIS_HANDLE MacReceiveContext,
    IN PVOID       HeaderBuffer,
    IN UINT        HeaderBufferSize,
    IN PVOID       LookAheadBuffer,
    IN UINT        LookaheadBufferSize,
    IN UINT        PacketSize
    )

{
    POPEN_INSTANCE      Open;
    PIO_STACK_LOCATION  IrpSp;
    PIRP                Irp;
    PLIST_ENTRY         PacketListEntry;	//pointer to read requests
    PNDIS_PACKET        pPacket;			//pointer to user's packet buffer
    ULONG               SizeToTransfer;
    NDIS_STATUS         Status;
    UINT                BytesTransfered;
    ULONG               BufferLength;
    PPACKET_RESERVED    Reserved;
	UINT				n;										//***003
//	KIRQL				Irql;									//***005

    IF_LOUD(DbgPrint("BACmac: ReceiveIndicate\n");)
    Open= (POPEN_INSTANCE)ProtocolBindingContext;
//																			***002 Begin
#define		BACnetLSAP	0x82
//																			***003 Begin
#define BACnetArcnetSC	0xCD
/*		if ((LookaheadBufferSize<3)||(HeaderBufferSize<14)) return NDIS_STATUS_SUCCESS;  //toss it
		if (((((UCHAR *)HeaderBuffer)[12]==0xFF)&&(((UCHAR *)HeaderBuffer)[13]==0xFF)) || //IPX?
			((((UCHAR *)LookAheadBuffer)[0]!=BACnetLSAP)||(((UCHAR *)LookAheadBuffer)[1]!=BACnetLSAP)||(((UCHAR *)LookAheadBuffer)[2]!=3)))	//not a BACnet LSAP?
		{	return NDIS_STATUS_SUCCESS;					//toss it
		}
*/
//																			***002 End	
//	if (HeaderBufferSize==14)							//Ethernet
	if (Open->NdisMedium==NdisMedium802_3)				//Ethernet
	{	if (LookaheadBufferSize<3) return NDIS_STATUS_NOT_ACCEPTED;	//per panix.com		//toss it
		n=0;											//index for LLC
	}
//	else if (HeaderBufferSize==3)						//ARCNET
	else if (Open->NdisMedium==NdisMediumArcnet878_2)	//ARCNET
	{	if (((UCHAR *)HeaderBuffer)[2]!=BACnetArcnetSC)																	//not an ARCNET packet
		{	Open->RxNonBACnet++;						//					***007
			return NDIS_STATUS_NOT_ACCEPTED;			//per panix.com		//toss it
		}
		n=0;											//index for LLC
	}
	else
	{	Open->RxNonBACnet++;							//					***007
		return NDIS_STATUS_NOT_ACCEPTED;				//per panix.com		//toss it
	}

	if ((((UCHAR *)LookAheadBuffer)[n]!=BACnetLSAP)||(((UCHAR *)LookAheadBuffer)[n+1]!=BACnetLSAP)||(((UCHAR *)LookAheadBuffer)[n+2]!=3))	//not a BACnet LSAP?
	{	Open->RxNonBACnet++;							//					***007
		return NDIS_STATUS_NOT_ACCEPTED;				//per panix.com		//toss it
	}

//															***003 End

//  See if there are any pending read that we can satisfy
    PacketListEntry=ExInterlockedRemoveHeadList(
                        &Open->RcvList,
                        &Open->RcvQSpinLock
                        );
    if (PacketListEntry == NULL)								//none to satisfy (toss it???)
	{	//JN--Note: In my testing I found that this does happen sometimes but not enough to explain dropped packets
		Open->RxNoUserBuff++;							//								***007
		return NDIS_STATUS_NOT_ACCEPTED;				//(changed from STATUS_SUCCESS)***006
    }

    Reserved=CONTAINING_RECORD(PacketListEntry,PACKET_RESERVED,ListElement);	//base address of PacketListEntry->ListElement
    pPacket=CONTAINING_RECORD(Reserved,NDIS_PACKET,ProtocolReserved);	//base address of PacketListEntry->ListElement->ProtocolReserved
//	how does pPacket point to the user buffer(?)

    Irp=RESERVED(pPacket)->Irp;
/*	IoAcquireCancelSpinLock (&Irql);							//						***005 Begin
	IoSetCancelRoutine(Irp,NULL);								// Now it's not cancelable
	IoReleaseCancelSpinLock (Irql);								//						***005 End
*/
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
	IoSetCancelRoutine(Irp, NULL);								//??????				***010

//  This is the length of our partial MDL
//  BufferLength=IrpSp->Parameters.Read.Length-ETHERNET_HEADER_LENGTH;			***003
    BufferLength=IrpSp->Parameters.Read.Length-HeaderBufferSize;		//		***003

//  Find out how much to transfer
    SizeToTransfer=(PacketSize<BufferLength)?PacketSize:BufferLength;

//  copy the header into the actual readbuffer
	NdisMoveMappedMemory(
		MmGetSystemAddressForMdl(Irp->MdlAddress),
		HeaderBuffer,
		HeaderBufferSize
		);

//  Call the Mac to transfer the packet (sans the header)
	NdisTransferData(
					&Status,
					Open->AdapterHandle,
					MacReceiveContext,
					0,
					SizeToTransfer,
					pPacket,					//how does this point to our packet data buffer???
					&BytesTransfered);

	if (Status!=NDIS_STATUS_PENDING)	//we're complete already
	{	PacketTransferDataComplete(Open,pPacket,Status,BytesTransfered);
    }
    return NDIS_STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
//	This is the completion routine for read NdisTransferData requests.
//
//	*** Registered as an upper edge protocol entry point with NDIS. ***
//
// in:	ProtocolBindingContext	the open instance of the device
//		pPacket					pointer to an Ndis packet
//		Status					the incoming status of the request
//		BytesTransfered			the number of bytes transferred
//
// out:	Success/Failure

VOID PacketTransferDataComplete (
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN PNDIS_PACKET  pPacket,
    IN NDIS_STATUS   Status,
    IN UINT          BytesTransfered
    )

{	PIO_STACK_LOCATION   IrpSp;
    POPEN_INSTANCE       Open;
    PIRP                 Irp;
    PMDL                 pMdl;
	UINT				 n;									//			***003

    IF_LOUD(DbgPrint("BACmac: TransferDataComplete\n");)
    Open= (POPEN_INSTANCE)ProtocolBindingContext;
    Irp=RESERVED(pPacket)->Irp;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);	//Note: IrpSp is never really used
    pMdl=RESERVED(pPacket)->pMdl;
	if (Status==STATUS_SUCCESS) Open->RxCount++;//count BACnet packets	***006

//  Free the MDL that we allocated
    IoFreeMdl(pMdl);

//  Recylcle the packet
	NdisReinitializePacket(pPacket);

//  Put the packet on the free queue
    NdisFreePacket(pPacket);

    Irp->IoStatus.Status = Status;
//																		***003 Begin
	switch (Open->NdisMedium)					//do it this way for future expansion
	{
	case NdisMedium802_3:
		n=ETHERNET_HEADER_LENGTH;
		break;
	case NdisMediumArcnet878_2:
		n=ARCNET_HEADER_LENGTH;
		break;
	default:
		n=ETHERNET_HEADER_LENGTH;
	}
//																		***003 End
    Irp->IoStatus.Information = BytesTransfered+n;			//			***003
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return;
}

///////////////////////////////////////////////////////////////////////
//	This is the completion routine for read requests.
//
//	*** Registered as an upper edge protocol entry point with NDIS. ***
//
// in:	ProtocolBindingContext	the open instance of the device
//
// out:	Success/Failure

VOID PacketReceiveComplete(IN NDIS_HANDLE  ProtocolBindingContext)

{	return;
}

//																				***010 Begin
///////////////////////////////////////////////////////////////////////
// This is the read cancel routine. It will remove the IRP from the queue 
// and will complete it. The cancel spin lock is already acquired 
// when this routine is called.

// in:	DeviceObject	pointer to a device object
//		Irp				pointer to the IRP to be cancelled
//
// out:	Nothing
VOID PacketCancelRoutine (IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
{	POPEN_INSTANCE      open; 
	KIRQL               oldIrql;
    PIRP                irpToComplete = NULL;
    PLIST_ENTRY         thisEntry, nextEntry, listHead;
    PIRP                pendingIrp;
    PNDIS_PACKET        myPacket;
    PPACKET_RESERVED    reserved;
    PMDL                mdl;
    PIO_STACK_LOCATION  irpSp;

    irpSp = IoGetCurrentIrpStackLocation(Irp);
    open=irpSp->FileObject->FsContext;

// Don't assume that the IRP being cancelled is in the queue.
// Only complete the IRP if it IS in the queue.

// Must acquire the local spinlock before releasing 
// the global cancel spinlock
    oldIrql = Irp->CancelIrql;
    KeAcquireSpinLockAtDpcLevel(&open->RcvQSpinLock);
    IoReleaseCancelSpinLock(KeGetCurrentIrql());

// Remove the IRP from the queue
    listHead = &open->RcvList;
    for(thisEntry=listHead->Flink;thisEntry!=listHead;thisEntry=thisEntry->Flink)
    {	reserved=CONTAINING_RECORD(thisEntry,PACKET_RESERVED,ListElement);
        myPacket=CONTAINING_RECORD(reserved,NDIS_PACKET,ProtocolReserved);
        pendingIrp = RESERVED(myPacket)->Irp;
        if (pendingIrp == Irp) 
        {	RemoveEntryList(thisEntry);
            irpToComplete = pendingIrp;
            break;
        }
    }
    
// Release the queue spinlock
    KeReleaseSpinLock(&open->RcvQSpinLock, oldIrql);

// Complete the IRP with status canclled
    
    if(irpToComplete) 
	{	mdl=RESERVED(myPacket)->pMdl;
// Free the MDL that we allocated
        IoFreeMdl(mdl);
// Recylcle the packet
        NdisReinitializePacket(myPacket);
// Put the packet on the free queue
        NdisFreePacket(myPacket);
        irpToComplete->IoStatus.Status = STATUS_CANCELLED;
        irpToComplete->IoStatus.Information = 0;
        IoCompleteRequest(irpToComplete, IO_NO_INCREMENT);
    }
    return;
 
}

//																				***010 End