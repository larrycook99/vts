/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1997,  Microsoft Corporation
	
module:		BACMAC95.C
desc:		Win 95 NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	William Ingle, Jack Neyer
last edit:	21-Sep-99 [003] JN  bump up TRANSMIT_PACKETS (in bacmac95.h)
			20-Oct-98 [002] JN  support for multiple adapters
			12-Dec-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
// PSDBG commented out!!!
//
// This library was taken from the MicroSoft Windows 95 packet driver (formerly called packet.c)
//
// It is an	Ndis 3.1 Windows 95 VxD that allows apps to access packets from the MAC layer as
// if it were a file (eg. in the same way Serial I/O is virtualized). There is an accompanying
// DLL (PACKET32.DLL) that provides a higher level API interface. In our application however,
// PACKET32 is a library that gets linked with our main BACnet protocol stack (BACDOR95).
//
// This module is the "main" module for the driver. It provides the DriverEntry procedure
// which is called by the standard VxD wrapper code when the VxD is loaded It basically 
// starts up and shuts down the driver. It is linked with three other modules that provide
// the open/close, read and write capabilities, two utility modules for memory locking/
// unlocking and general requests of the driver.
//
// The primary changes to the driver are to rename it from "Packet" to "bacmac95" 
// (ie. module name) and VPACKET to BACMAC95 (ie. device/protocol name).
//
// Little else is changed in the driver, save some liberal comments to alleviate
// later confusion.

#include <basedef.h>
#include <vmm.h>
#include <ndis.h>
#include <vwin32.h>
#include "debug.h"
#include "bacmac95.h"						//							***001
#include "ntddpack.h"

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

PDEVICE_EXTENSION GlobalDeviceExtension = 0;

/*++
Routine Description:
   Our very own strlen so we don't have to include libraries.
--*/

ULONG strlen( BYTE *s )
{	ULONG len = 0;

	while ( *s++ ) len++;
	return len;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT	DriverObject,IN PUNICODE_STRING	RegistryPath)
/*++
Routine Description:
   This routine initializes the Packet driver. It is called from the VxD ASM shell
   when the VxD is installed.

Arguments:
   DriverObject - Pointer to driver object created by system.
   RegistryPath - Pointer to the Unicode name of the registry path for this driver. (not used)

Return Value:
   The function value is the final status from the initialization operation.
--*/

{	NDIS_PROTOCOL_CHARACTERISTICS	ProtocolChar;

// Name MUST match string used by ndisdev.asm to declare the driver name; case counts.
// This is necessary for binding to proceed properly.
	NDIS_STRING		ProtoName = NDIS_STRING_CONST("BACMAC95");		//	***001
 	NDIS_HANDLE		NdisProtocolHandle;
	NDIS_STATUS		Status;
	int				i;								//					***002

	INIT_ENTER( "DriverEntry" );					//debug
	NdisAllocateMemory((PVOID *)&GlobalDeviceExtension, sizeof( DEVICE_EXTENSION ), 0, -1 );
//	Initialize the NDIS characteristics and protocol entry points so we can 
//  register as a protocol
	if ( GlobalDeviceExtension != NULL )			//we got the memory we asked for
	{	NdisZeroMemory( (UCHAR*)GlobalDeviceExtension, sizeof(DEVICE_EXTENSION) );
		NdisZeroMemory( (UCHAR*)&ProtocolChar, sizeof(NDIS_PROTOCOL_CHARACTERISTICS) );
		ProtocolChar.MajorNdisVersion = 0x03;
		ProtocolChar.MinorNdisVersion = 0x0A;
		ProtocolChar.Reserved = 0;
		ProtocolChar.OpenAdapterCompleteHandler = PacketBindAdapterComplete;
		ProtocolChar.CloseAdapterCompleteHandler = PacketUnbindAdapterComplete;
		ProtocolChar.SendCompleteHandler = PacketSendComplete;
		ProtocolChar.TransferDataCompleteHandler = PacketTransferDataComplete;
		ProtocolChar.ResetCompleteHandler = PacketResetComplete;
		ProtocolChar.RequestCompleteHandler = PacketRequestComplete;
		ProtocolChar.ReceiveHandler = PacketReceiveIndicate;
		ProtocolChar.ReceiveCompleteHandler = PacketReceiveComplete;
		ProtocolChar.StatusHandler = PacketStatus;
		ProtocolChar.StatusCompleteHandler  = PacketStatusComplete;
		ProtocolChar.BindAdapterHandler = PacketBindAdapter;
		ProtocolChar.UnbindAdapterHandler = PacketUnbindAdapter;
		ProtocolChar.UnloadProtocolHandler = PacketUnload;
		ProtocolChar.Name = ProtoName;				//BACMAC95			***001

//	Register us with NDIS (as protocol=BACMAC95)						***001
	  	IF_TRACE( "registering protocol with NDIS" );				//	***001
		NdisRegisterProtocol(&Status,&GlobalDeviceExtension->NdisProtocolHandle,&ProtocolChar,sizeof(NDIS_PROTOCOL_CHARACTERISTICS));
		if (Status != NDIS_STATUS_SUCCESS) 
		{	NdisFreeMemory( GlobalDeviceExtension, sizeof( DEVICE_EXTENSION ) ,  0 );
//	  		IF_TRACE( "Failed to register protocol with NDIS" );	//??must be commented out?? ***001
			INIT_LEAVE( "DriverEntry" );
			return Status;
		}

// Initialize the adapter list
		InitializeListHead( &GlobalDeviceExtension->OpenList);
			
// Initialize global device extension driver object
		GlobalDeviceExtension->DriverObject = DriverObject;

// Initialize bound device handle list
		for (i=0;i<MAX_DEVICES;i++)					//				***002 Begin
		{	GlobalDeviceExtension->hDevice[i]=0;
			GlobalDeviceExtension->AdapterIndex[i]=0;
		}											//				***002 End

		IF_TRACE( "protocol registered with NDIS!!! :-)" );
		INIT_LEAVE( "DriverEntry" );
//		psdInit();									//				***PSDBG
//		psdPrint ("\r\nInit:Installed BACMAC95\r\n");	//			***PSDBG
		return Status;
	}

	IF_TRACE( "Memory Failure" );
	INIT_LEAVE( "DriverEntry" );

	return NDIS_STATUS_RESOURCES;
}

VOID NDIS_API PacketUnload()
/*++

Routine Description: 
	Unload the Protocol.

	*** Registered as an entry point with the Driver Object. ***

Arguments:

Return Value:

	none

--*/

{	NDIS_STATUS		Status;

	TRACE_ENTER( "Unload" );
// Deregister us
	if ( GlobalDeviceExtension )
	{	NdisDeregisterProtocol( &Status, GlobalDeviceExtension->NdisProtocolHandle );
		if ( Status == NDIS_STATUS_SUCCESS )
		{	NdisFreeMemory( GlobalDeviceExtension, sizeof( DEVICE_EXTENSION ) ,  0 );
			GlobalDeviceExtension = 0;
		}
	}
	TRACE_LEAVE( "Unload" );
	return;
}

POPEN_INSTANCE GetFirstAdapter( VOID )
/*++

Routine Description: 
	Return a pointer to the first instance of an adapter. This is obtained from
	GlobalDeviceExtension->OpenList.

Arguments:

Return Value:
	Pointer to a port device extension

--*/

{	PLIST_ENTRY pEntry = GlobalDeviceExtension->OpenList.Flink;			//get first adapter in the list
	POPEN_INSTANCE pOpen = CONTAINING_RECORD( pEntry, OPEN_INSTANCE, ListElement );	//point to it's name

	return pOpen;
}

POPEN_INSTANCE GetAdapter( WORD aindex )		//							***002 Begin
/*++

Routine Description: 
	Return a pointer to an instance of an adapter. This is obtained from
	GlobalDeviceExtension->OpenList.

Arguments:
	aindex	the index of the adapter in OpenList

Return Value:
	Pointer to a port device extension

--*/
{	WORD	i;
	POPEN_INSTANCE pOpen;
	PLIST_ENTRY pHead = GlobalDeviceExtension->OpenList.Flink;		//get first adapter in the list
	PLIST_ENTRY pEntry = pHead;

	for (i=0;i<aindex;i++)											//walk the list
	{	pEntry=pEntry->Flink;
		if (pEntry==pHead) break;									//must be invalid index							
	}
	pOpen = CONTAINING_RECORD( pEntry, OPEN_INSTANCE, ListElement );	//point to it's name
	return pOpen;
}
//																				***002 End
DWORD PacketGetMacNameList(DWORD dwDDB,DWORD hDevice,PDIOCPARAMETERS pDiocParms) 
/*++

Routine Description: 
	Return a list of the adapters know to the system.
    This request completes successfully.


Arguments:
	dwDDB - From the IOCTL (not used)
	hDevice - From the IOCTL (not used)
	pDiocParms - place to put adapter names
 
Return Value: 
    Status is returned.

--*/
{	PLIST_ENTRY	pHead = &(GlobalDeviceExtension->OpenList);		//we'll get the names from here
																//This is setup when the protocol adapter binding is complete
	PLIST_ENTRY pEntry;
	DWORD dwBytes = 0;
	
	TRACE_ENTER( "GetMacNameList" );

	for ( pEntry=pHead->Flink; pEntry != pHead; pEntry=pEntry->Flink)	//get them all
	{	BYTE 	*lpzName;
		ULONG	uLength;
		POPEN_INSTANCE pOpen = CONTAINING_RECORD( pEntry, OPEN_INSTANCE, ListElement );

		PWRAPPER_MAC_BLOCK			pWMBlock;
		PNDIS_MAC_CHARACTERISTICS	pNMChar;

		pWMBlock = ((PWRAPPER_OPEN_BLOCK)(pOpen->AdapterHandle))->MacHandle;
		pNMChar  = &pWMBlock->MacCharacteristics;
		lpzName  = pNMChar->Name.Buffer;
		uLength  = strlen( lpzName );
	
		IF_TRACE_MSG2( "     %s  %lu",  lpzName, uLength );
// Build list of space-separated adapter names (name1 name2 name3 ...)	
		if ( uLength < pDiocParms->cbOutBuffer - dwBytes - 1 )
		{	strcat( (BYTE*)(pDiocParms->lpvOutBuffer), lpzName );	//concatinate name of next adapter
			strcat( (BYTE*)(pDiocParms->lpvOutBuffer), " " );
			dwBytes += (uLength + 1);
		}
		else
			break;
	}

	*(ULONG*)(pDiocParms->lpcbBytesReturned) = dwBytes;
	IF_TRACE_MSG( "     Bytes Returned: %lu", *(ULONG*)(pDiocParms->lpcbBytesReturned) );
	TRACE_LEAVE( "GetMacNameList" );
	return NDIS_STATUS_SUCCESS;
}

DWORD _stdcall PacketIOControl( DWORD dwService,
                                DWORD dwDDB,
                                DWORD hDevice,
                                PDIOCPARAMETERS pDiocParms ) 
/*++
Routine Description:
    This is the dispatch routine for create/open and close requests. It gets called
	whenever a W32_DEVICEIOCONTROL message is issued to the VxD. This is mapped in
	NDISDEV.ASM.
    
	These requests complete successfully.

Arguments:
    dwService - The requested service.
    dwDDB - VxD Device Data Block (not used)
	hDevice - the device handle
	pDiocParms - the device parameters struct for passing data

Return Value:
    Status is returned.
--*/

{  POPEN_INSTANCE	Open;
   NDIS_STATUS	Status;
   int			*pai,ai,i;				//							***002

   TRACE_ENTER( "DeviceIoControl" );

//For now we utilize only the FIRST adapter bound to the protocol
    Open = GetFirstAdapter();			//default
// --- Not any more ---													***002 Begin
	if (hDevice!=0)
	{	for (i=0;i<MAX_DEVICES;i++)
		{	if (GlobalDeviceExtension->hDevice[i]==hDevice)
			{	Open = GetAdapter(GlobalDeviceExtension->AdapterIndex[i]);
				break;
			}
		}
	}
//																	***002 End
   IF_TRACE_MSG3( "    Function code is %08lx  buff size=%08lx  %08lx",
						dwService,
						pDiocParms->cbInBuffer,
						pDiocParms->cbOutBuffer );
   IF_TRACE_MSG3( "    Device Handle is %08lx  Open Instance=%08lx DDB=%08lx",
						hDevice,
						Open,
						dwDDB );			//							***002

	switch ( dwService )
	{
	case DIOC_OPEN:
// Just return success,  This is required for Win95
		break;

	case DIOC_CLOSEHANDLE:
// Make sure there are no pending i/o
		Status = NDIS_STATUS_SUCCESS;
//		PacketCleanUp( &Status, GetFirstAdapter() );		//				***002 Begin
		for (i=0;i<MAX_DEVICES;i++)
		{	if (GlobalDeviceExtension->hDevice[i]==hDevice)
			{	GlobalDeviceExtension->hDevice[i]=0;		//free up this handle
				GlobalDeviceExtension->AdapterIndex[i]=0;
				break;
			}
		}
		PacketCleanUp (&Status, Open);						//				***002 End
		break;

	case IOCTL_PROTOCOL_RESET:
		PacketReset( &Status, Open );
		break;

	case IOCTL_PROTOCOL_SET_OID:
	case IOCTL_PROTOCOL_QUERY_OID:
	case IOCTL_PROTOCOL_STATISTICS:
		return PacketRequest( Open, dwService, dwDDB, hDevice, pDiocParms );

	case IOCTL_PROTOCOL_READ:
		return PacketRead( Open, dwDDB, hDevice, pDiocParms );

	case IOCTL_PROTOCOL_WRITE:
		return PacketWrite( Open, dwDDB, hDevice, pDiocParms );

	case IOCTL_PROTOCOL_MACNAME:
		PacketGetMacNameList( dwDDB, hDevice, pDiocParms );
		break;
      
	case IOCTL_PROTOCOL_BIND_DEVICE:					//					***002 Begin
//	This stuff has been added so that after a create has been done, we can bind the
//  resulting handle to some adapter to which this protocol has been bound. If WIN passed
//  this handle (which it doesn't) along to the DIOC_OPEN we could have done it there!

		pai =(int *)pDiocParms->lpvInBuffer;
		ai=*pai;										//relative adapter to bind to
		for (i=0;i<MAX_DEVICES;i++)
		{	if (GlobalDeviceExtension->hDevice[i]==0)	//we found an empty slot
			{	GlobalDeviceExtension->hDevice[i]=hDevice;			//save the device handle
				GlobalDeviceExtension->AdapterIndex[i]=(WORD)ai;	//matching adapter index
				IF_TRACE_MSG3( "    Device Handle %08lx  bound to adapter %u at %u",
									hDevice,
									(WORD)ai,
									(WORD)i );
				break;
			}
		}
		break;
//												//					***002 End
	default: 
// Error, Unrecognized IOCTL
		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;
		break;
	}

   TRACE_LEAVE( "DeviceIoControl" );
   
   return NDIS_STATUS_SUCCESS;
}

VOID PacketStatus(
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN NDIS_STATUS   Status,
    IN PVOID         StatusBuffer,
    IN UINT          StatusBufferSize
    )
/*++
Routine Description:
	Return status. (A void routine)

	*** Registered as an entry point with the Driver Object. ***

Arguments:

Return Value:
--*/

{	TRACE_ENTER( "Status Indication" );
	TRACE_LEAVE( "Status Indication" );

	return;
}

VOID NDIS_API PacketStatusComplete(
    IN NDIS_HANDLE  ProtocolBindingContext
    )
/*++

Routine Description:
	Return status complete. (A void routine)

	*** Registered as an entry point with the Driver Object. ***

Arguments:

Return Value:

--*/

{	TRACE_ENTER( "StatusIndicationComplete" );
	TRACE_LEAVE( "StatusIndicationComplete" );

	return;
}

PLIST_ENTRY PacketRemoveHeadList(IN PLIST_ENTRY pListHead)
/*++

Routine Description:
	Given a pointer to a list, return a pointer to the first 
	entry in the list.

Arguments:

Return Value:

    First PLIST_ENTRY from list is returned
	 Returns NULL if list is empty
--*/

{	if (!IsListEmpty( pListHead))
	{	PLIST_ENTRY pLE = RemoveHeadList( pListHead );
		return pLE;
	}
	return NULL;
}

//															***PSDBG Begin
/*
void psdInit()
{
_asm{
	push	edx
	push	eax
	mov		dx,LCR		//clear DLAB, no parity, 1 stop, 8 bit word
	mov		al,03         
	out		dx,al
	mov		dx,IER		//disable all interrupts (use polling)
	mov		al,00         
	out		dx,al
	mov		dx,FCR		//disable FIFOs (if 16550)
	mov		al,00         
	out		dx,al
	mov		dx,MCR		//set DTR and RTS and out1,out2
	mov		al,0x0F         
	out		dx,al
//assume baudrate is already setup
	pop	eax
	pop	edx
	}
}

//display a character on the debug port

void psdOutc(int c)
{
_asm{
    push	edx
loutc1:
	mov		dx,LSR		//read line status to see if uart is busy
	in		al, dx		//input byte
    and		al,0x20		//THRE bit
    jz		loutc1
    mov		eax,c
    mov     dx,THR
    out     dx,al
    pop     edx
	}
}
//display a message

void psdPrint(char *msg)
{	
	while(*msg)
		psdOutc(*msg++);
}

//display a message with an argument
// msg is of form "message %dn%" or "message %xn"

void psdPrintf(char *msg,DWORD arg)
{	int		n=0,nchr,i,j;
	char	*mmsg=msg,nmsg[256],c;
	DWORD	x=arg,y;

	i=0;
	while (*mmsg!=0)
	{	if (*mmsg=='%')								//formatted msg??
		{	mmsg++;
			if (*mmsg=='d')							//%dn format
			{	mmsg++;
				while ((*mmsg>='0')&&(*mmsg<='9'))	//determine n
				{	n*=10;
					n+=((*mmsg++)&0x0f);
				}
				if (n!=0)
				{	i+=(n-1);
					j=i+1;							//remember where we were in nmsg
					while ((n--)!=0)				//format decimal argument
					{	y=x%10;
						nchr=(int)(0x30+y);
						x/=10;
						nmsg[i--]=nchr;
					}
					i=j;							//restore i
				}
			}
			else if (*mmsg=='x')					//%xn format
			{	mmsg++;
				while ((*mmsg>='0')&&(*mmsg<='9'))	//determine n
				{	n*=10;
					n+=((*mmsg++)&0x0f);
				}
				if (n!=0)
				{	nmsg[i++]='0';
					nmsg[i++]='x';
					i+=(n-1);
					j=i+1;							//remember where we were in nmsg
					while ((n--)!=0)				//format hex argument
					{	y=x%16;
						if ((y>=0)&&(y<=9)) nchr=(int)(0x30+y);		//0-9
						else nchr=(int)(0x41-10+y);					//A-F
						x/=16;
						nmsg[i--]=nchr;
					}
					i=j;							//restore i
				}
			}
		}
		nmsg[i++]=*mmsg++;
	}
	nmsg[i]=0;										//make sure it's 0 terminated
	mmsg=nmsg;
	while(*mmsg)
		psdOutc(*mmsg++);
}
*/
//															***PSDBG End

