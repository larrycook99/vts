/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1990  Microsoft Corporation
	
module:		WRITE.C
desc:		NT NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	Jack Neyer
last edit:	14-Sep-99 [010] JN  changes per MS v3.0
			13-Sep-99 [004] JN  expand performance stats
			02-Sep-99 [003] JN  add TxCount
			07-May-99 [002] JN  Bug Fixes per Lew Perin:
								do IoCompleteRequest when error makes I/O complete impossible
			25-Oct-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
// This library was taken from the MicroSoft NT 4.0 DDK dated Jan '97 on the directory:
//               /src/network/packet/driver.
//
// It is an NT  ***Kernel mode only***  driver
//
// This module provides the write functions for the driver and is meant to be 
// linked with the other modules for the driver to form the whole.

// Little has changed in the driver, save some liberal comments to alleviate
// later confusion.


#include "stdarg.h"
#include "ntddk.h"
#include "ntiologc.h"
#include "ndis.h"

#include "debug.h"
#include "bacmacnt.h"

///////////////////////////////////////////////////////////////////////
//	This is the dispatch routine for write requests.
//  These requests complete successfully.
//
//	*** Registered as an entry point with the Driver Object. ***
//
// in:	DeviceObject	pointer to a device object
//		Irp				pointer to the write request packet
//
// out:	Success/Failure

NTSTATUS PacketWrite(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)

{	POPEN_INSTANCE      Open;
    PIO_STACK_LOCATION IrpSp;
    PNDIS_PACKET       pPacket;
    NDIS_STATUS     Status;

    IF_LOUD(DbgPrint("BACmac: SendAdapter\n");)
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    Open=IrpSp->FileObject->FsContext;

//  Try to get a packet from our list of free ones

    NdisAllocatePacket(&Status,&pPacket,Open->PacketPool);

    if (Status != NDIS_STATUS_SUCCESS)		//no free packets
	{	Open->TxNoNDISBuff++;				//								***004
		Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;	//			***010
 		IoCompleteRequest(Irp, IO_NO_INCREMENT);	//complete the request	***002
        return STATUS_INSUFFICIENT_RESOURCES;		//						***010
    }

    RESERVED(pPacket)->Irp=Irp;

//  Attach the writes buffer to the packet
    NdisChainBufferAtFront(pPacket,Irp->MdlAddress);

    IoMarkIrpPending(Irp);
    Irp->IoStatus.Status = STATUS_PENDING;

//  Call the MAC
    NdisSend(&Status,Open->AdapterHandle,pPacket);

    if (Status != NDIS_STATUS_PENDING)		//we're complete already
	{	PacketSendComplete(Open,pPacket,Status);
		return Status;						//										***010
    }
    return(STATUS_PENDING);
}

///////////////////////////////////////////////////////////////////////
//	This is the routing indicating that a packet has been sent.
//
//	*** Registered as an entry point with the Driver Object. ***
//
// in:	ProtocolBindingContext	the open instance of the device
//		pPacket					pointer to an ndis packet
//		Status					incoming status of the request
//

VOID PacketSendComplete(
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN PNDIS_PACKET  pPacket,
    IN NDIS_STATUS   Status
    )

{	PIRP              Irp;
	POPEN_INSTANCE      Open;			//						***003
    PIO_STACK_LOCATION IrpSp;			//						***003

    IF_LOUD(DbgPrint("BACmac: SendComplete\n");)
    Irp=RESERVED(pPacket)->Irp;
	IrpSp = IoGetCurrentIrpStackLocation(Irp);		//			***010

    if (Status==STATUS_SUCCESS)			//						***003 Begin
	{	//IrpSp = IoGetCurrentIrpStackLocation(Irp);			***010
		Open=IrpSp->FileObject->FsContext;
		Open->TxCount++;						
	}									//						***003 End

//  Recyle the packet
    NdisReinitializePacket(pPacket);

//  Put the packet back on the free list
    NdisFreePacket(pPacket);

    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = IrpSp->Parameters.Write.Length;	//	***010
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return;
}
