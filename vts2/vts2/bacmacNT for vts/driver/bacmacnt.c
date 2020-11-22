/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1990  Microsoft Corporation
	
module:		BACMACNT.C
desc:		NT NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	Jack Neyer
last edit:	14-Sep-99 [010] JN  changes per MS v3.0
			13-Sep-99 [005] JN  expand performance stats
			02-Sep-99 [004] JN  add RxCount, TxCount to OPEN_INSTANCE
			10-May-99 [003] JN  more Bug Fixes per Lew Perin:
								add cancel IRP routine (this is registered with IoSetCancelRoutine)
								(see NOTE: below)
			07-May-99 [002] JN  Bug Fixes per Lew Perin:
								do IoCompleteRequest when error makes I/O complete impossible
			25-Oct-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
//																		***003 Begin
//NOTE: The code for canceling is in this module. It should be activated by the 
//IoSetCancelRoutine in READ.C. I never could get this to work. It always did a blue 
//screen crash even when the cancel routine was called even if it only returned.
//																		***003 End

// This library was taken from the MicroSoft NT 4.0 DDK dated Jan '97 on the directory:
//               /src/network/packet/driver.
//
// It is an NT  ***Kernel mode only***  driver
//
// This module is the "main" module for the driver. It provides the DriverEntry procedure
// and basically starts up and shuts down the driver. It is linked with 
// three other modules that provide the open/close, read and write capabilities 
// of the driver.
//
// The primary changes to the driver are to rename it from "Packet" to "bacmacnt" and
// to rename the NT device names to "bacmac1, etc.".
//
// This is assummed that the linkages are set up in the registry at:
//					path\Linkage: \Device\macname1 \Device\macname2 ...
//					path\Export: \Device\bacmac1 \Device\bacmac2 ...
// and that those entries are made at install/setup time.
//
// Little else is changed in the driver, save some liberal comments to alleviate
// later confusion.

// This driver is meant to be built using the NT DDK Build utility.

#include "stdarg.h"
#include "ntddk.h"
#include "ntiologc.h"
#include "ndis.h"

#include "ntddpack.h"

#include "debug.h"
#include "bacmacnt.h"
#include "bacoid.h"										//			***005

// Some declarations:

NTSTATUS DriverEntry(IN PDRIVER_OBJECT	DriverObject,
					 IN PUNICODE_STRING RegistryPath);

NTSTATUS PacketReadRegistry(IN PWSTR *MacDriverName,
							IN PWSTR *PacketDriverName,
							IN PUNICODE_STRING RegistryPath);

NTSTATUS PacketCreateSymbolicLink(IN PUNICODE_STRING DeviceName,
								  IN BOOLEAN Create);

NTSTATUS PacketQueryRegistryRoutine(IN PWSTR ValueName,
									IN ULONG ValueType,
									IN PVOID ValueData,
									IN ULONG ValueLength,
									IN PVOID Context,
									IN PVOID EntryContext);
#if DBG
//
// Declare the global debug flag for this driver.
//
ULONG PacketDebugFlag = PACKET_DEBUG_LOUD;
#endif

PDEVICE_EXTENSION GlobalDeviceExtension;

//-------------------------------- Standard NT driver entry --------------------------------

///////////////////////////////////////////////////////////////////////
//	The Standard NT Driver Enrty. Called by the I/O Manager when it loads
//  the driver, either at boot time (if it loads automatically) or if loaded
//  from the control panel or from and application through the NT service
//  control manager.
//
//  in:		DriverObject	pointer to the driver object
//			RegistryPath	Registry Path string for the driver service key
//
//	out:	success/failure status	

NTSTATUS
DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{   NDIS_PROTOCOL_CHARACTERISTICS ProtocolChar;	//the NDIS characteristics for registering this protocol

	UNICODE_STRING		MacDriverName;				//the MacDriverName eg.\Device\DC21x41						
	UNICODE_STRING		UnicodeDeviceName;			//the device name eg.\Device\bacmac1
    PDEVICE_OBJECT		DeviceObject = NULL;		//a pointer to the device object
    PDEVICE_EXTENSION	DeviceExtension = NULL;		//a pointer to the device extension
    NTSTATUS			Status = STATUS_SUCCESS;
    NTSTATUS			ErrorCode = STATUS_SUCCESS;
    NDIS_STRING			ProtoName = NDIS_STRING_CONST("BacmacNT");	//the protocol name registered with NDIS
    ULONG				DevicesCreated=0;			//the number of devices created (always 1)

    PWSTR				BindString;					//for determining the mac names
    PWSTR				ExportString;				//for determining the driver names
    PWSTR				BindStringSave;				//ditto
    PWSTR				ExportStringSave;			//ditto

    NDIS_HANDLE			NdisProtocolHandle;			//the handle returned when the protocol is registered

 
	IF_LOUD(DbgPrint("\n\nBACmac: DriverEntry\n");)

//	Initialize the NDIS characteristics and protocol entry points so we can 
//  register as a protocol
	RtlZeroMemory(&ProtocolChar,sizeof(NDIS_PROTOCOL_CHARACTERISTICS));	
    ProtocolChar.MajorNdisVersion            = 3;
    ProtocolChar.MinorNdisVersion            = 0;
    ProtocolChar.Reserved                    = 0;
    ProtocolChar.OpenAdapterCompleteHandler  = PacketOpenAdapterComplete;
    ProtocolChar.CloseAdapterCompleteHandler = PacketCloseAdapterComplete;
    ProtocolChar.SendCompleteHandler         = PacketSendComplete;
    ProtocolChar.TransferDataCompleteHandler = PacketTransferDataComplete;
    ProtocolChar.ResetCompleteHandler        = PacketResetComplete;
    ProtocolChar.RequestCompleteHandler      = PacketRequestComplete;
    ProtocolChar.ReceiveHandler              = PacketReceiveIndicate;
    ProtocolChar.ReceiveCompleteHandler      = PacketReceiveComplete;
    ProtocolChar.StatusHandler               = PacketStatus;
    ProtocolChar.StatusCompleteHandler       = PacketStatusComplete;
    ProtocolChar.Name                        = ProtoName;

//	Register us with NDIS
    NdisRegisterProtocol(&Status,&NdisProtocolHandle,&ProtocolChar,sizeof(NDIS_PROTOCOL_CHARACTERISTICS));
    if (Status != NDIS_STATUS_SUCCESS)		//oops 
	{	IF_LOUD(DbgPrint("BACmac: Failed to register protocol with NDIS\n");)
		return Status;
	}

//	Set up the device driver entry points.
    DriverObject->MajorFunction[IRP_MJ_CREATE] = PacketOpen;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]  = PacketClose;
    DriverObject->MajorFunction[IRP_MJ_READ]   = PacketRead;
    DriverObject->MajorFunction[IRP_MJ_WRITE]  = PacketWrite;
//    DriverObject->MajorFunction[IRP_MJ_CLEANUP]  = PacketCleanup;						***010
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = PacketIoControl;

//	Setup the Unload point.
    DriverObject->DriverUnload = PacketUnload;

//  Get the name(s) of the Packet driver(s) and the name(s) of the MAC driver(s)
//  to bind to from the registry. The return strings are a set of null-terminated
//	strings terminated by a null (MULTI_SZ string).
    Status=PacketReadRegistry(&BindString,&ExportString,RegistryPath);
    if (Status != STATUS_SUCCESS)			//oops
	{	IF_LOUD(DbgPrint("Perf: Failed to read registry\n");)
		goto RegistryError;
    }
    BindStringSave=BindString;				//remember--these were dynamically
    ExportStringSave=ExportString;			// and must be freed later!

//  Create a device object for each entry..there may be more than one!
    while (*BindString!= UNICODE_NULL && *ExportString!= UNICODE_NULL)
	{
//  Create a counted unicode string for both null terminated strings
		RtlInitUnicodeString(&MacDriverName,BindString);		//next mac name (eg. \Device\DC21x41)
        RtlInitUnicodeString(&UnicodeDeviceName,ExportString);	//next device name (eg. \Device\bacmac1)
//  Advance to the next string of the MULTI_SZ strings
        BindString+=(MacDriverName.Length+sizeof(UNICODE_NULL))/sizeof(WCHAR);
        ExportString+=(UnicodeDeviceName.Length+sizeof(UNICODE_NULL))/sizeof(WCHAR);
        IF_LOUD(DbgPrint("BACmac: DeviceName=%ws  MacName=%ws\n",UnicodeDeviceName.Buffer,MacDriverName.Buffer);)
//  Create the device object (will be bound to an adapter later by NdisOpenAdapter)
        Status = IoCreateDevice(DriverObject,sizeof(DEVICE_EXTENSION),&UnicodeDeviceName,
								FILE_DEVICE_PROTOCOL,0,FALSE,&DeviceObject);
        if (Status != STATUS_SUCCESS)
		{	IF_LOUD(DbgPrint("Perf: IoCreateDevice() failed:\n");)
            break;
        }
        DevicesCreated++;
        DeviceObject->Flags|=DO_DIRECT_IO;									//(I accidently omitted the |)	***010
        DeviceExtension=(PDEVICE_EXTENSION) DeviceObject->DeviceExtension;	//device extension provided by I/O Mgr
        DeviceExtension->DeviceObject=DeviceObject;							//create a link
//  Save the the name of the MAC driver to bind to (open) in the Device Extension
        DeviceExtension->AdapterName=MacDriverName;
        if (DevicesCreated == 1)											//need to save this only once
		{	DeviceExtension->BindString=BindStringSave;
            DeviceExtension->ExportString=ExportStringSave;
        }
		DeviceExtension->NdisProtocolHandle=NdisProtocolHandle;				//probably this too but then what do I know?
    }
//  Managed to create at least one device.
    if (DevicesCreated > 0) return STATUS_SUCCESS;

//	No devices were saved..let's bail out
//	Free up dynamic strings
	ExFreePool(BindStringSave);
    ExFreePool(ExportStringSave);
//	Deregister the protocol..we had an error
RegistryError:
    NdisDeregisterProtocol(&Status,NdisProtocolHandle);
    Status=STATUS_UNSUCCESSFUL;
    return(Status);
}
///////////////////////////////////////////////////////////////////////
//	The Driver Unload Routine.
//
//	*** Registered as an entry point with the Driver Object. ***
//
//  in:		DriverObject	pointer to the driver object

VOID PacketUnload(IN PDRIVER_OBJECT DriverObject)
{	PDEVICE_OBJECT     DeviceObject;
    PDEVICE_OBJECT     OldDeviceObject;
    PDEVICE_EXTENSION  DeviceExtension;
    NDIS_HANDLE        NdisProtocolHandle;
    NDIS_STATUS        Status;


    IF_LOUD(DbgPrint("BACmac: Unload\n");)
    DeviceObject = DriverObject->DeviceObject;
    while (DeviceObject!=NULL)							//for every device object of this type
	{   DeviceExtension = DeviceObject->DeviceExtension;
        NdisProtocolHandle=DeviceExtension->NdisProtocolHandle;	//get the protocol handle we registered
//	Free any memory we might have allocated
        if (DeviceExtension->BindString!=NULL) ExFreePool(DeviceExtension->BindString);
        if (DeviceExtension->ExportString!=NULL) ExFreePool(DeviceExtension->ExportString);
        OldDeviceObject=DeviceObject;
        DeviceObject=DeviceObject->NextDevice;
        IoDeleteDevice(OldDeviceObject);
    }
    NdisDeregisterProtocol(&Status,NdisProtocolHandle);
}

///////////////////////////////////////////////////////////////////////
//	This is the dispatch routine for create/open and close requests.
//  These requests complete successfully.
//
//	*** Registered as an entry point with the Driver Object. ***
//
// in:	DeviceObject	pointer to a device object
//		Irp				pointer to a request packet
//
// out:	Success/Failure

NTSTATUS PacketIoControl(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)

{	POPEN_INSTANCE      Open;				//the open instance of the device
    PIO_STACK_LOCATION  IrpSp;				//pointer to the current request stack location of the device
    PLIST_ENTRY         RequestListEntry;
    PINTERNAL_REQUEST   pRequest;			//pointer to the request
    ULONG               FunctionCode;		//IOCTL function to be performed
    NDIS_STATUS			Status;

    IF_LOUD(DbgPrint("BACmac: IoControl\n");)
    IrpSp=IoGetCurrentIrpStackLocation(Irp);
    FunctionCode=IrpSp->Parameters.DeviceIoControl.IoControlCode;
    Open=IrpSp->FileObject->FsContext;			//the open instance of the device				
    RequestListEntry=ExInterlockedRemoveHeadList(&Open->RequestList,&Open->RequestSpinLock);
    if (RequestListEntry == NULL)				//we couldn't dequeue a request
	{	Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
 		IoCompleteRequest(Irp, IO_NO_INCREMENT);	//complete the request	***002
		return STATUS_UNSUCCESSFUL;
    }
    pRequest=CONTAINING_RECORD(RequestListEntry,INTERNAL_REQUEST,ListElement);
    pRequest->Irp=Irp;

    IoMarkIrpPending(Irp);						//mark the request pending
    Irp->IoStatus.Status=STATUS_PENDING;
    IF_LOUD(DbgPrint("BACmac: Function code is %08lx  buff size=%08lx  %08lx\n",FunctionCode,IrpSp->Parameters.DeviceIoControl.InputBufferLength,IrpSp->Parameters.DeviceIoControl.OutputBufferLength);)

    if (FunctionCode == IOCTL_PROTOCOL_RESET)	//Reset
	{	IF_LOUD(DbgPrint("BACmac: IoControl - Reset request\n");)
        ExInterlockedInsertTailList(&Open->ResetIrpList,&Irp->Tail.Overlay.ListEntry,&Open->RequestSpinLock);
        NdisReset(&Status,Open->AdapterHandle);	//do the reset
        if (Status != NDIS_STATUS_PENDING)		//we've completed the request
		{	IF_LOUD(DbgPrint("BACmac: IoControl - ResetComplte being called\n");)
			PacketResetComplete(Open,Status);	//process the completion
		}
    } 
	else										//may be a set or query request
	{	PPACKET_OID_DATA OidData=Irp->AssociatedIrp.SystemBuffer;	//point to the oid data
// Note: I corrected two(?) errors in the following statement. JN.
		if (((FunctionCode==IOCTL_PROTOCOL_SET_OID)||(FunctionCode==IOCTL_PROTOCOL_QUERY_OID))
           &&(IrpSp->Parameters.DeviceIoControl.InputBufferLength==IrpSp->Parameters.DeviceIoControl.OutputBufferLength)
           &&(IrpSp->Parameters.DeviceIoControl.InputBufferLength>=sizeof(PACKET_OID_DATA))
           &&(IrpSp->Parameters.DeviceIoControl.InputBufferLength<=sizeof(PACKET_OID_DATA)-1+OidData->Length))
		{	IF_LOUD(DbgPrint("BACmac: IoControl: Request: Oid=%08lx, Length=%08lx\n",OidData->Oid,OidData->Length);)
//  The buffer is valid and we've got either a set or a query request
//  Set up the request data based on whether we want to set or query
//	ie. (IOCTL_PROTOCOL_SET_OID or IOCTL_PROTOCOL_QUERY_OID)
            if (FunctionCode==IOCTL_PROTOCOL_SET_OID)			//we want to set something
			{	pRequest->Request.RequestType=NdisRequestSetInformation;
                pRequest->Request.DATA.SET_INFORMATION.Oid=OidData->Oid;
                pRequest->Request.DATA.SET_INFORMATION.InformationBuffer=OidData->Data;
                pRequest->Request.DATA.SET_INFORMATION.InformationBufferLength=OidData->Length;

            }
			else												//we want to query something
			{	
				pRequest->Request.RequestType=NdisRequestQueryInformation;	
				pRequest->Request.DATA.QUERY_INFORMATION.Oid=OidData->Oid;
				pRequest->Request.DATA.QUERY_INFORMATION.InformationBuffer=OidData->Data;
				pRequest->Request.DATA.QUERY_INFORMATION.InformationBufferLength=OidData->Length;
//																***004 Begin				
				if (OidData->Oid==OID_GEN_DIRECTED_FRAMES_RCV)
				{	ULONG *dp=(ULONG *)(OidData->Data);
					*dp=Open->RxCount;							//NOTE: This includes "reflected" broadcast packets sent from us
					Status=NDIS_STATUS_SUCCESS;
					pRequest->Request.DATA.QUERY_INFORMATION.BytesWritten=4;
					goto done;
				}
				else if (OidData->Oid==OID_GEN_DIRECTED_FRAMES_XMIT)
				{	ULONG *dp=(ULONG *)(OidData->Data);
					*dp=Open->TxCount;
					Status=NDIS_STATUS_SUCCESS;
					pRequest->Request.DATA.QUERY_INFORMATION.BytesWritten=4;
					goto done;
				}
				else if(OidData->Oid==OID_BACMAC_GET_PERF_DATA)	//	***005 Begin
				{	PERF_DATA *pd=(PERF_DATA *)(OidData->Data);	//assume we have enough to save into
					pd->RxCount=Open->RxCount;							//NOTE: This includes "reflected" broadcast packets sent from us
					pd->TxCount=Open->TxCount;
					pd->RxNonBACnet=Open->RxNonBACnet;
					pd->RxNoUserBuff=Open->RxNoUserBuff;
					pd->RxNoNDISBuff=Open->RxNoNDISBuff;
					pd->TxNoNDISBuff=Open->TxNoNDISBuff;
					Status=NDIS_STATUS_SUCCESS;
					pRequest->Request.DATA.QUERY_INFORMATION.BytesWritten=sizeof(PERF_DATA);
					goto done;
				}												//	***005 End
//																	***004 End
            }
//  Submit the request to Ndis
            NdisRequest(&Status,Open->AdapterHandle,&pRequest->Request);
        }
		else
		{
//  Buffer too small
			Status=NDIS_STATUS_FAILURE;
            pRequest->Request.DATA.SET_INFORMATION.BytesRead=0;
            pRequest->Request.DATA.QUERY_INFORMATION.BytesWritten=0;

        }
done:												//			***005
        if (Status != NDIS_STATUS_PENDING)			//the NDIS request has completed
		{	IF_LOUD(DbgPrint("BACmac: Calling RequestCompleteHandler\n");)
            PacketRequestComplete(Open,&pRequest->Request,Status);
        }
    }
    return(STATUS_PENDING);
}

/*													//							***010 Begin
///////////////////////////////////////////////////////////////////////			***003 Begin
//	This code is added to cancel outstanding read requests and eliminate
//  the hanging of the IO manager for 5 minutes appearing as if the NT 
//  system has hung. This is actually a combo of "Rick's" code and Lew's. 
//  (I used Lew's method of finding the open handle, but Rick's 
//  cancellation method.)
//
// in:	DeviceObject	pointer to a device object
//		Irp				pointer to a request packet
//
// out:	Nothing

void PacketReadCancel(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)

{	PLIST_ENTRY			ListEntry=NULL;
	PLIST_ENTRY			NextElement=NULL;
	PNDIS_PACKET		pPacket;
	PPACKET_RESERVED	Reserved;
	PIRP				CancelIrp=NULL;
	KIRQL				Irql;
	KIRQL				CancelIrql=Irp->CancelIrql;
	PIO_STACK_LOCATION	IrpSp=IoGetCurrentIrpStackLocation(Irp);	//this is Lew's method
	POPEN_INSTANCE      Open=IrpSp->FileObject->FsContext;			//ditto

	IoReleaseCancelSpinLock (CancelIrql);							//release cancel spin lock
	if (Open==NULL)
	{	IF_LOUD(DbgPrint("Open Instance Ptr NULL during cancel \n");)	//this should never happen
		return;
	}
// Find the first entry in our Queue with the IRP set to cancel,
// complete the IRP with Status=CANCELLED
	KeAcquireSpinLock(&Open->RcvQSpinLock,&Irql);					//so we can mess with the receive queue
	for (NextElement=Open->RcvList.Flink;NextElement!=&Open->RcvList;)
	{	Reserved=CONTAINING_RECORD(NextElement,PACKET_RESERVED,ListElement);
		pPacket=CONTAINING_RECORD(Reserved,NDIS_PACKET,ProtocolReserved);
		ListEntry=NextElement;										//this is the guy we may be canceling
		NextElement=NextElement->Flink;								//next one
		CancelIrp=RESERVED(pPacket)->Irp;
		if (CancelIrp->Cancel)										//this guy's IRP is set to cancel
		{	RemoveEntryList(ListEntry);								//out of our list
			IF_VERY_LOUD(DbgPrint("Cancel IRP 0x%x\n",(ULONG)CancelIrp);)
			break;
		}
		CancelIrp=NULL;												//there was nothing to cancel
	}
	KeReleaseSpinLock (&Open->RcvQSpinLock,Irql);
	if (CancelIrp)													//Complete the Irp
	{	PacketTransferDataComplete(Open,pPacket,STATUS_CANCELLED,0);
	}
	return;
}
//																				***003 End
*/																	//			***010 End

///////////////////////////////////////////////////////////////////////
//	This is the completion routine for create/open and close requests.
//
//	*** Registered as an upper edge protocol entry point with NDIS. ***
//
// in:	ProtocolBindingContext	the open instance of the device
//		NdisRequest				pointer to the ndis request
//		Status					the incoming status of the request
//
// out:	Success/Failure

VOID PacketRequestComplete(IN NDIS_HANDLE ProtocolBindingContext,IN PNDIS_REQUEST NdisRequest,IN NDIS_STATUS Status)
{	POPEN_INSTANCE      Open;
    PIO_STACK_LOCATION  IrpSp;				
    PIRP                Irp;
    PINTERNAL_REQUEST   pRequest;
    UINT                FunctionCode;
    PPACKET_OID_DATA    OidData;

    IF_LOUD(DbgPrint("BACmac: RequestComplete\n");)
    Open=(POPEN_INSTANCE)ProtocolBindingContext;
	pRequest=CONTAINING_RECORD(NdisRequest,INTERNAL_REQUEST,Request);
    Irp=pRequest->Irp;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    FunctionCode=IrpSp->Parameters.DeviceIoControl.IoControlCode;	//either IOCTL_PROTOCOL_SET_OID or IOCTL_PROTOCOL_QUERY_OID
    OidData=Irp->AssociatedIrp.SystemBuffer;

    if (FunctionCode == IOCTL_PROTOCOL_SET_OID)
		OidData->Length=pRequest->Request.DATA.SET_INFORMATION.BytesRead;
	else 
	{	if (FunctionCode == IOCTL_PROTOCOL_QUERY_OID)
			OidData->Length=pRequest->Request.DATA.QUERY_INFORMATION.BytesWritten;
    }

    Irp->IoStatus.Information=IrpSp->Parameters.DeviceIoControl.InputBufferLength;
//	Insert the entry at the end of the request list using spin lock protection
    ExInterlockedInsertTailList(&Open->RequestList,&pRequest->ListElement,&Open->RequestSpinLock);

    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);			//complete the request!
    return;


}

///////////////////////////////////////////////////////////////////////
//	This routine is called by the underlying NIC when a status change 
//  begins. It does nothing to the incoming status and/or buffer.
//
//	*** Registered as an upper edge protocol entry point with NDIS. ***
//
// in:	ProtocolBindingContext	the open instance of the device
//		Status					the incoming status of the request
//		StatusBuffer			the incoming status buffer
//		StatusBufferSize		the incoming size of status of the request
//

VOID PacketStatus(IN NDIS_HANDLE ProtocolBindingContext,IN NDIS_STATUS Status,IN PVOID StatusBuffer,IN UINT StatusBufferSize)
{	IF_LOUD(DbgPrint("BACmac: Status Indication\n");)
    return;
}
///////////////////////////////////////////////////////////////////////
//	This routine is called by the underlying NIC when a status change 
//  is complete. It does nothing.
//
//	*** Registered as an upper edge protocol entry point with NDIS. ***
//
// in:	ProtocolBindingContext	the open instance of the device
//		Status					the incoming status of the request
//		StatusBuffer			the incoming status buffer
//		StatusBufferSize		the incoming size of status of the request
//

VOID PacketStatusComplete(IN NDIS_HANDLE ProtocolBindingContext)
{	IF_LOUD(DbgPrint("BACmac: StatusIndicationComplete\n");)
	return;
}

#if 0
///////////////////////////////////////////////////////////////////////
//	This routine is provided to create a Symbolic link between the NT device
//	and a DOS device for access from WIN32 apps. It is currnetly not called
//	and in fact is not included because of the #if 0.
//
//	in:		DeviceName	the unicode name of the device (eg.\Device\bacmac1)

NTSTATUS PacketCreateSymbolicLink(IN PUNICODE_STRING DeviceName,IN BOOLEAN Create)
{	UNICODE_STRING UnicodeDosDeviceName;
    NTSTATUS       Status;

    if (DeviceName->Length < sizeof(L"\\Device\\")) return STATUS_UNSUCCESSFUL;	//device name not good
    RtlInitUnicodeString(&UnicodeDosDeviceName,NULL);
    UnicodeDosDeviceName.MaximumLength=DeviceName->Length+sizeof(L"\\DosDevices")+sizeof(UNICODE_NULL);
//	Allocate some memory for the dos device name
    UnicodeDosDeviceName.Buffer=ExAllocatePool(NonPagedPool,UnicodeDosDeviceName.MaximumLength);
    if (UnicodeDosDeviceName.Buffer != NULL)				//we were successful in our allocation
	{	RtlZeroMemory(UnicodeDosDeviceName.Buffer,UnicodeDosDeviceName.MaximumLength);
		RtlAppendUnicodeToString(&UnicodeDosDeviceName,L"\\DosDevices\\");
        RtlAppendUnicodeToString(&UnicodeDosDeviceName,(DeviceName->Buffer+(sizeof("\\Device"))));
//	The dos device name is now "\DosDevices\bacmac1" for example
        IF_LOUD(DbgPrint("BACmac: DosDeviceName is %ws\n",UnicodeDosDeviceName.Buffer);)
        if (Create)
			Status=IoCreateSymbolicLink(&UnicodeDosDeviceName,DeviceName);
		else 
			Status=IoDeleteSymbolicLink(&UnicodeDosDeviceName);

        ExFreePool(UnicodeDosDeviceName.Buffer);			//free the memory we got
    }
	return Status;
}

#endif

///////////////////////////////////////////////////////////////////////
//	Read the Registry to obtain the names of the drivers and the adapters to bind to.
//  in:		MacDriverName		a place to put the MULTI_SZ packet names
//			PacketDriverName	a place to put the MULTI_SZ driver names
//			RegistryPath		Registry Path string for the driver service key
//								(path=HKEY_LOCAL_MACHINE\SYSTEM\
									CurrentControllerSet\Services\bacmacnt)
//	
//	The packet driver names are found on Path\Linkage\Export:
//	The mac driver names are found on Path\Linkage\Bind:
//
//	out:	success/failure status	

NTSTATUS PacketReadRegistry(IN PWSTR *MacDriverName,IN PWSTR *PacketDriverName,IN PUNICODE_STRING RegistryPath)
{	NTSTATUS   Status;
    RTL_QUERY_REGISTRY_TABLE ParamTable[5];
    PWCHAR Path;
    PWSTR Bind       = L"Bind";
    PWSTR Export     = L"Export";
    PWSTR Parameters = L"Parameters";
    PWSTR Linkage    = L"Linkage";
//																		***001 Begin
/*

// Note: all this has been added in case the macdriver names have not been added
//	     the directory whici isn't clear from reading the .INF file supplied!

	PWSTR Network	 = L"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\NetworkCards\\1"; //the key off of HKEY_LOCAL_MACHINE to get the value***001
	PWSTR ServiceName = L"ServiceName";							  //the value name		***001
	PWSTR		Buffer;
	ULONG		n;

//	For now lets return "\Device\bacmac1" and the mac adapter found in:
//	HKEY_LOCAL_MACHINE\Software\Microsoft\NetworkCards\1\ServiceName

    n=50;
	Buffer=ExAllocatePool(NonPagedPool,n);							//allocate some space to save the string
    if (Buffer==NULL) return STATUS_INSUFFICIENT_RESOURCES;
    RtlZeroMemory(Buffer,n);
    RtlCopyMemory(Buffer,NTdevicename,28);						//Buffer now has device name
	*PacketDriverName=Buffer;
	n=16;
    Buffer=ExAllocatePool(NonPagedPool,n);							//allocate some space to save the string
    if (Buffer==NULL) return STATUS_INSUFFICIENT_RESOURCES;
    RtlZeroMemory(Buffer,n);
    RtlCopyMemory(Buffer,L"DC21x41",14);						//Buffer now has device name
	*MacDriverName=Buffer;
	return STATUS_SUCCESS;
    RtlZeroMemory(ParamTable,sizeof(ParamTable));
//  Get the name of the mac driver we should bind to from the ServiceName Value
    ParamTable[0].QueryRoutine = PacketQueryRegistryRoutine;
    ParamTable[0].Flags = RTL_QUERY_REGISTRY_REQUIRED | RTL_QUERY_REGISTRY_NOEXPAND;
    ParamTable[0].Name = ServiceName;
    ParamTable[0].EntryContext = (PVOID)MacDriverName;
    ParamTable[0].DefaultType = REG_SZ;
//	Use ParamTable to do the query
    Status=RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE,Network,ParamTable,NULL,NULL);
	if (Status!=STATUS_SUCCESS) ExFreePool(Buffer);		//free the Buffer we got
    return Status;
*/
													//				***001 End

    Path=ExAllocatePool(PagedPool,RegistryPath->Length+sizeof(WCHAR));	//allocate memory fot the path
	if (Path == NULL) return STATUS_INSUFFICIENT_RESOURCES;				//oops
	RtlZeroMemory(Path,RegistryPath->Length+sizeof(WCHAR));
    RtlCopyMemory(Path,RegistryPath->Buffer,RegistryPath->Length);		//copy the path into allocated memory
    IF_LOUD(DbgPrint("BACmac: Reg path is %ws\n",RegistryPath->Buffer);)
    RtlZeroMemory(ParamTable,sizeof(ParamTable));
//  change to the parmeters key (!!NOTE: I don't think this is required!!!!)
    ParamTable[0].QueryRoutine = NULL;
    ParamTable[0].Flags = RTL_QUERY_REGISTRY_SUBKEY;
    ParamTable[0].Name = Parameters;
//  change to the linkage key (cancelling the parameters subkey)
    ParamTable[1].QueryRoutine = NULL;
    ParamTable[1].Flags = RTL_QUERY_REGISTRY_SUBKEY;
    ParamTable[1].Name = Linkage;
//  Get the name of the mac driver we should bind to from the bind value
    ParamTable[2].QueryRoutine = PacketQueryRegistryRoutine;
    ParamTable[2].Flags = RTL_QUERY_REGISTRY_REQUIRED | RTL_QUERY_REGISTRY_NOEXPAND;
    ParamTable[2].Name = Bind;
    ParamTable[2].EntryContext = (PVOID)MacDriverName;	//will get a multi-string of the form \Device\macdriver1 \Device\macdriver2 ...
    ParamTable[2].DefaultType = REG_MULTI_SZ;
//	Get the name that we should use for the driver object from the export value
    ParamTable[3].QueryRoutine = PacketQueryRegistryRoutine;
    ParamTable[3].Flags = RTL_QUERY_REGISTRY_REQUIRED | RTL_QUERY_REGISTRY_NOEXPAND;
    ParamTable[3].Name = Export;
    ParamTable[3].EntryContext = (PVOID)PacketDriverName;	//will get a multi-string of the form \Device\bacmac1 \Device\bacmac2 ...
    ParamTable[3].DefaultType = REG_MULTI_SZ;
//	Use ParamTable to do the query
    Status=RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE,Path,ParamTable,NULL,NULL);
//	Free the memory we got
    ExFreePool(Path);
    return Status;
}

///////////////////////////////////////////////////////////////////////
//	Routine used to Query the Registry. Called from RtlQueryRegistryValues for each query.
//  in:		args provided by RtlQueryRegistryValues (see below)
//	out:	success/failure

NTSTATUS PacketQueryRegistryRoutine(
	IN PWSTR	ValueName,						//the key
	IN ULONG	ValueType,						//eg. REG_MULTI_SZ, REG_SIZE
    IN PVOID	ValueData,						//the string
    IN ULONG	ValueLength,					//length of the string found
    IN PVOID	Context,
    IN PVOID	EntryContext					//place to put pointer to string
    )
{	PUCHAR		Buffer;

/*    IF_LOUD(DbgPrint("Perf: QueryRegistryRoutine\n");)				//***001 Begin
    if (ValueType != REG_SZ) return STATUS_OBJECT_NAME_NOT_FOUND;
    Buffer=ExAllocatePool(NonPagedPool,ValueLength+sizeof(WCHAR));	//allocate some space to save the string
    if (Buffer==NULL) return STATUS_INSUFFICIENT_RESOURCES;
    RtlCopyMemory(Buffer,ValueData,ValueLength);
	RtlStoreUshort((PUSHORT)Buffer[ValueLength],UNICODE_NULL);		//add a unicode 0 to make it MULTI_REG_SZ
   *((PUCHAR *)EntryContext)=Buffer;
    return STATUS_SUCCESS;
*/
																	//***001 End

	IF_LOUD(DbgPrint("Perf: QueryRegistryRoutine\n");)
    if (ValueType != REG_MULTI_SZ) return STATUS_OBJECT_NAME_NOT_FOUND;
    Buffer=ExAllocatePool(NonPagedPool,ValueLength);	//allocate some space to save the string
    if (Buffer==NULL) return STATUS_INSUFFICIENT_RESOURCES;
    RtlCopyMemory(Buffer,ValueData,ValueLength);
    *((PUCHAR *)EntryContext)=Buffer;
    return STATUS_SUCCESS;
}
