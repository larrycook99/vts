/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1990  Microsoft Corporation
	
module:		PACKET32.C
desc:		32-bit MAC Layer Packet Interface
authors:	Jack Neyer
last edit:	02-Sep-99 [010]	JN  add Packet32GetRxCount, etc.
			14-Jun-99 [009] JN  add mysize
			14-May-99 [008] JN  change GlobalAlloc/Free -> malloc/free to eliminate shutdown crashing
			31-Mar-99 [007] JN  implemented mymalloc and myfree as publics for all dlls
								(should have made a separate lib but I'm lazy so I put it
								 here cause everybody uses this lib module anyway)
								 changed GlobalAllocPtr/GlobalFree to mymalloc/myfree
			13-Nov-98 [006] JN  multiple adapters
								Note: no change is required here..this already supports multiple
								adapters
			29-Jul-98 [004] JN  support for ARCNET
			07-May-98 [003] JN  bacpac->bacmac
			12-Mar-98 [002] JN  add blocking option to Packet32PacketWait
								add auto/manual reset for CreateEvent in Packet32AllocatePacket
			16-Oct-97 [001] JN  first cut (changes from the template ARE marked)
------------------------------------------------------------------------------*/
// This library was taken from the MicroSoft NT 4.0 DDK dated Jan '97 on the directory:
//               /src/network/packet/dll 
// and was adapted from a DLL to an include library for linking to BACDOR32. It provides 
// a set of procedures for accessing MAC layer packets using file model (i.e. file open,
// close, read, write, create event,etc.)
//
// The basic changes are:
//	1) rename the functions from PacketXXX to Packet32XXX
//	2) add some clarifying comments so that it can be adapted later for WIN 95.
//	3) add a "get adapter address" function
//	4) change the WaitPacketRoutine to NOT wait forever
//	5) change the AdapterGetName to also access the Bind values (needed for get adapter address)

// Use of this module requires a kernel-mode driver that maps file functions to NDIS. 

#define UNICODE 1								//so that TEXT("x")==L"x"

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>								//						***009
#include <winioctl.h>
#include "ntddndis.h"
#include "ntddpack.h"
#include "packet32.h"
#include "bacoid.h"  // added MAG 15 FEB 2001

TCHAR   szWindowTitle[] = TEXT("PACKET32.LIB");

#if DBG
#define ODS(_x) OutputDebugString(TEXT(_x))
#else
#define ODS(_x)
#endif
// Function Prototypes

BOOLEAN StartPacketDriver(LPTSTR ServiceName, IN PVOID DllHandle);

//-------------------------------- Begin API Functions --------------------------------

///////////////////////////////////////////////////////////////////////
//	Initialize the Packet Interface by trying to load/unload the associated 
//  kernel-mode packet driver (BACMACNT).
//
//	in:		DllHandle		the handle of the calling DLL
//			Reason			Attach or Detach
//			Context			Not Used
//	out:	SUCCESS - TRUE; FAILURE - FALSE

__declspec(dllexport) BOOL Packet32Init(IN PVOID DllHandle,IN ULONG Reason,IN PCONTEXT Context OPTIONAL)
{	BOOLEAN		Status=TRUE;

	ODS("Packet32: PacketInit\n");
    switch(Reason)
    {   case DLL_PROCESS_ATTACH:
			Status=StartPacketDriver(drivername,DllHandle);
            break;
		case DLL_PROCESS_DETACH:
			 break;
        default:
             break;
    }
    return Status;
}

///////////////////////////////////////////////////////////////////////
//	Attempt to start the kernel mode packet driver.
//
//	in:		ServiceName		the driver name to start
//	out:	SUCCESS	- TRUE, FAILURE - FALSE

BOOLEAN StartPacketDriver(LPTSTR ServiceName,PVOID DllHandle)
{	BOOLEAN		Status;
    SC_HANDLE	SCManagerHandle;
    SC_HANDLE	SCServiceHandle;

	SCManagerHandle = OpenSCManager(		// Open a handle to the SC Manager database.
                      NULL,                   // local machine
                      NULL,                   // ServicesActive database
                      SC_MANAGER_ALL_ACCESS); // full access rights

    if (SCManagerHandle==NULL)
	{	MessageBox(DllHandle,TEXT("Could not open SC"),szWindowTitle,MB_OK);
        return FALSE;
	}
	else
	{	SCServiceHandle=OpenService(SCManagerHandle,ServiceName,SERVICE_START); //open a handle for the driver
        if (SCServiceHandle == NULL)
			MessageBox(NULL,TEXT("Could not open service"),szWindowTitle,MB_OK);
		Status=StartService(SCServiceHandle,0,NULL);		//start the driver
        if (!Status)
		{	if (GetLastError()==ERROR_SERVICE_ALREADY_RUNNING)
			{	ODS("Packet32: Packet service already started\n");
				return TRUE;
            }
        }
        return Status;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////
//	Open an Instance of the adapter. Allocate memory for an ADAPTER object,
//	define a WIN32 device object name for the NT device and create a file handle
//	for the device. 
//
//	in:		AdapterName		unicode name of the adapter to open
//							of the form: \Device\bacmac1
//	out:	SUCCESS - Pointer to an Adapter; FAILURE - NULL

__declspec(dllexport) PVOID Packet32OpenAdapter(LPTSTR AdapterName)
{	LPADAPTER	lpAdapter;
    BOOLEAN		Result;

    ODS("Packet32: PacketOpenAdapter\n");
    lpAdapter=(LPADAPTER)mymalloc(sizeof(ADAPTER));	//allocate and lock a adapter object***007
    if (lpAdapter==NULL)
	{	ODS("Packet32: PacketOpenAdapter GlobalAlloc Failed\n");
        return NULL;
    }
//	I believe there are errors in the next two lines that need to be corrected!
//	In order for us to do a CreateFile, we must have a dos device name of the form:
//	"////.//dosname. This can be accomplished in two ways:
//		1) By IoCreateDeviceSymbolicLink of unique(?)DosDevice and (NT)Device names in
//			the DriverEntry init (see the "NT Driver" Book example in chapter 9) where:
//
//			IoCreateSymbolicLink ("\\DosDevices\\XX1","\\Device\\Xx0");	//in the driver
//			CreateFile (""\\\\.\\XX1,etc.);								//in the api
//
//		2) By defining a dos device for the NT device in the api and no IoCreateSymbolicLink
//			is required! (see sample driver on NT DDK \General\dosdev\dosdev.c) where:
//			
//			For the standard NT device \Device\PointerClass0"
//			DefineDosDevice(DDD_RAW_TARGET_PATH,"MOU","\\Device\\PointerClass0");	//in the api (make device visible to WIN32 as "MOU")
//			CreateFile("\\\\.\\MOU",etc.)		//in the api
//
//	In our case if NT device is \Device\bacmac1 and the DOSNAMEPREFIX = Packet_
//	we want to make a WIN32 device \DosDevices\Packet_bacmac1 and create a 
//	file \\\\.\\Packet_bacmac1:

	wsprintf(lpAdapter->SymbolicLink,TEXT("\\\\.\\%s%s"),DOSNAMEPREFIX,&AdapterName[8]);	//AdapterName[8]=bacmac1 SymbolicLink=\\.\Packet_bacmac1
	Result=DefineDosDevice(DDD_RAW_TARGET_PATH,&lpAdapter->SymbolicLink[4],AdapterName);	//SymbolicLink{4]=Packet_bacmac1
    if (Result)													//definition succeeded
	{	lpAdapter->hFile=CreateFile(lpAdapter->SymbolicLink,	//now create the file "\\\\.\\BACMAC"
                             GENERIC_WRITE | GENERIC_READ,
                             0,
                             NULL,
                             CREATE_ALWAYS,
                             FILE_FLAG_OVERLAPPED,
                             0
                             );
        if (lpAdapter->hFile != INVALID_HANDLE_VALUE)
			return lpAdapter;									//SUCCESS
    }
//	Some Error Occurred!
    ODS("Packet32: PacketOpenAdapter Could not open adapter\n");
    myfree(lpAdapter);											//Unlock and free the object we got	***007
    return NULL;												//FAILURE
}
///////////////////////////////////////////////////////////////////////
//	Close a previously opened instance of the adapter. Close the handle
//	for the adapter and free the ADAPTER object.
//
//	in:		lpAdapter	pointer to an adapter object

__declspec(dllexport) VOID Packet32CloseAdapter(LPADAPTER lpAdapter)

{	ODS("Packet32: PacketCloseAdapter\n");
	CloseHandle(lpAdapter->hFile);								//close the file
    myfree(lpAdapter);											//Unlock and free the adapter object***007
}

///////////////////////////////////////////////////////////////////////
//	Allocate a packet object for use with read/write. Allocate memory for
//	a PACKET object (does not store data) and Create an (overlapped) event.
//
//	in:		AdapterObject	pointer to an Adapter object
//	out:	SUCCESS - returns packet object; FAILURE - NULL

__declspec(dllexport) PVOID Packet32AllocatePacket(LPADAPTER AdapterObject,BOOLEAN manreset)	//	***002
{	LPPACKET    lpPacket;

    lpPacket=(LPPACKET)mymalloc(sizeof(PACKET));			//allocate and lock a packet object		***007
	if (lpPacket==NULL)
	{   ODS("Packet32: PacketAllocatePacket: GlobalAlloc Failed\n");
        return NULL;										//FAILURE
    }
    lpPacket->OverLapped.hEvent=CreateEvent(NULL,manreset,FALSE,NULL);		//default/no security, auto/manual-reset, non-signalled initial state, no event name	***002
	if (lpPacket->OverLapped.hEvent==NULL)								//could not create the event
	{	ODS("Packet32: PacketAllocateSendPacket: CreateEvent Failed\n");
		myfree(lpPacket);									//unlock and free our packet object***007
        return NULL;
    }
    return lpPacket;										//SUCCESS
}

///////////////////////////////////////////////////////////////////////
//	Free a packet object. Close the handle for the previously created 
//	event and free the PACKET object.
//
//	in:		lpPacket		pointer to a Packet object

__declspec(dllexport)VOID Packet32FreePacket(LPPACKET lpPacket)
{
    CloseHandle(lpPacket->OverLapped.hEvent);				//close the event
    myfree(lpPacket);								//unlock and free the packet***007
}
///////////////////////////////////////////////////////////////////////
//	Initialize a packet object to point to a memory buffer of known length.
//	The memory buffer us used to send/receive data.
//
//	in:		lpPacket	Packet object
//			Buffer		Begining address of a memory buffer
//			Length		Length of memory buffer

__declspec(dllexport) VOID Packet32InitPacket(LPPACKET lpPacket,PVOID Buffer,UINT Length)
{	lpPacket->Buffer=Buffer;
    lpPacket->Length=Length;
}
///////////////////////////////////////////////////////////////////////
//	Send a packet. Either sync or async operation using a write file.
//	If synchronous, then we wait for completion using GetOverlappedResult.
//
//	in:		AdapterObject	an adapter object
//			lpPacket		Packet object to be sent
//			Sync			TRUE if service should wait for packet to transmit
//	out:	SUCCESS - TRUE; FAILURE - FALSE

__declspec(dllexport) BOOLEAN Packet32SendPacket(LPADAPTER AdapterObject,LPPACKET lpPacket,BOOLEAN Sync)
{	BOOLEAN		Result;
    DWORD		BytesTransfered;

	lpPacket->OverLapped.Offset=0;
    lpPacket->OverLapped.OffsetHigh=0;
    if (!ResetEvent(lpPacket->OverLapped.hEvent))		//set the event to non-signalled
		return FALSE;									//FAILURE
    Result=WriteFile(AdapterObject->hFile,lpPacket->Buffer,lpPacket->Length,&BytesTransfered,&lpPacket->OverLapped);	//send it
    if (Sync)											//wait for completion
		Result=GetOverlappedResult(AdapterObject->hFile,&lpPacket->OverLapped,&BytesTransfered,TRUE);
	else												//must call Packet32WaitPacket
		Result=TRUE;									//SUCCESS
    return Result;
}
///////////////////////////////////////////////////////////////////////
//	Issue a receive request to the adapter. Either sync or async operation 
//	using a read file. If synchronous, then we wait for completion using 
//	GetOverlappedResult.
//
//	in:		AdapterObject	an adapter object
//			lpPacket		Packet object to be sent
//			Sync			TRUE if service should wait for packet to be received
//			BytesReceived	A place to put the byte count if sync == TRUE
//	out:	SUCCESS - TRUE and sync==true; FAILURE - FALSE


__declspec(dllexport) BOOLEAN Packet32ReceivePacket(LPADAPTER AdapterObject,LPPACKET lpPacket,BOOLEAN Sync,PULONG BytesReceived)
{	BOOLEAN      Result;

    lpPacket->OverLapped.Offset=0;
    lpPacket->OverLapped.OffsetHigh=0;
    if (!ResetEvent(lpPacket->OverLapped.hEvent))		//set the event to non-signalled
        return FALSE;									//FAILURE
	Result=ReadFile(AdapterObject->hFile,lpPacket->Buffer,lpPacket->Length,BytesReceived,&lpPacket->OverLapped);	//make a read request

    if (Sync)											//wait until something's there
		Result=GetOverlappedResult(AdapterObject->hFile,&lpPacket->OverLapped,BytesReceived,TRUE);
	else												//must call Packet32WaitPacket to get real result
		Result=TRUE;									//SUCCESS
    return Result;

}
//																	***001 Begin
///////////////////////////////////////////////////////////////////////
//	Checks if an overlapped IO on a packet is complete.
//  Must be Called if the send or receive call specified FALSE for the Sync parmeter
//
//	in:		AdapterObject	an adapter object
//			lpPacket		Packet object to be sent
//			BytesReceived	to be filled in with the bytes received
//			bWait			wait for completion						***002			
//	out:	ERROR_SUCCESS or ERROR_IO_INCOMPLETE or ERROR_xxx

__declspec(dllexport) DWORD Packet32WaitPacket(LPADAPTER AdapterObject,LPPACKET lpPacket,PULONG BytesReceived,BOOLEAN bWait)	//***002
{
	if (bWait)										//				***002 Begin
		GetOverlappedResult(AdapterObject->hFile,&lpPacket->OverLapped,BytesReceived,FALSE);
	else											//				***002 End
	if (GetOverlappedResult(AdapterObject->hFile,&lpPacket->OverLapped,BytesReceived,FALSE)) return ERROR_SUCCESS;	//Don't wait
	return GetLastError();							//either we're done or we have an error
//																	***001 End
}

///////////////////////////////////////////////////////////////////////
//	Reset an adapter thereby completing all pending sends and receives.
//	Issues an IOCTL_PROTOCOL_RESET thru DeviceIoControl.
//
//	in:		AdapterObject	an adapter object
//	out:	always SUCCESS

__declspec(dllexport) BOOLEAN Packet32ResetAdapter(LPADAPTER AdapterObject)
{	UINT	BytesReturned;

	DeviceIoControl(
		AdapterObject->hFile,
        (DWORD)IOCTL_PROTOCOL_RESET,
        NULL,
        0,
        NULL,
        0,
        &BytesReturned,
        NULL
        );

    return TRUE;
}

///////////////////////////////////////////////////////////////////////
//	Issue a request to an adapter. Issues a IOCTL request thru DeviceIoControl.
//
//	in:		AdapterObject	an adapter object
//			Set				True if the request is a set, else it's a query
//		    OidData			Structure containing the details of the OID
//	out:	SUCCESS	- TRUE, FAILURE - FALSE

__declspec(dllexport) BOOLEAN Packet32Request(LPADAPTER AdapterObject,BOOLEAN Set,PPACKET_OID_DATA OidData)
{
    UINT       BytesReturned;
    BOOLEAN    Result;

    Result=DeviceIoControl(
        AdapterObject->hFile,
        (DWORD) Set ? IOCTL_PROTOCOL_SET_OID : IOCTL_PROTOCOL_QUERY_OID,
        OidData,
        sizeof(PACKET_OID_DATA)-1+OidData->Length,
        OidData,
        sizeof(PACKET_OID_DATA)-1+OidData->Length,
        &BytesReturned,
        NULL
        );

    return Result;
}
///////////////////////////////////////////////////////////////////////
//	Issue a request to an adapter. Issues arequest thru DeviceIoControl.
//
//	in:		AdapterObject	an adapter object
//			Filter			a packet filter to be set
//	out:	SUCCESS	- TRUE, FAILURE - FALSE

__declspec(dllexport) BOOLEAN Packet32SetFilter(LPADAPTER AdapterObject,ULONG Filter)
{	BOOLEAN		Status;
	ULONG		IoCtlBufferLength=(sizeof(PACKET_OID_DATA)+sizeof(ULONG)-1);
    PPACKET_OID_DATA  OidData;

    OidData=mymalloc(IoCtlBufferLength);					//allocate some memory	***007
	if (OidData == NULL) return FALSE;						//allocation failed
    OidData->Oid=OID_GEN_CURRENT_PACKET_FILTER;				//set filter command
    OidData->Length=sizeof(ULONG);
    *((PULONG)OidData->Data)=Filter;						//the filter
    Status=Packet32Request(AdapterObject,TRUE,OidData);		//set it
    myfree(OidData);										//						***007
    return Status;
}

///////////////////////////////////////////////////////////////////////
//	Returns the names all Device/MAC Drivers available by reading the registry.
//  This was adapted from PacketGetAdapterNames
//
//	in:		pStr		Pointer to a buffer which receives the UNICODE names
//						Each name is NULL terminated with a second NULL at the end
//						of the list.
//			BufferSize	Size of the buffer passed in
//			type		Type of device to get (Bind or Export)
//	out:	SUCCESS - TRUE

__declspec(dllexport) ULONG Packet32GetDeviceNames(PTSTR pStr,PULONG BufferSize, enum DEVICETYPES type)
{	HKEY		SystemKey;
    HKEY		ControlSetKey;
    HKEY		ServicesKey;
    HKEY		NdisPerfKey;
    HKEY		LinkageKey;
    LONG		Status;
    DWORD		RegType;

	Status=RegOpenKeyEx(
				HKEY_LOCAL_MACHINE,
				TEXT("SYSTEM"),
				0,
				KEY_READ,
				&SystemKey);		//HKEY_LOCAL_MACHINE\SYSTEM
    if (Status == ERROR_SUCCESS)
	{	Status=RegOpenKeyEx(
				SystemKey,
				TEXT("CurrentControlSet"),
				0,
				KEY_READ,
				&ControlSetKey);	//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet
        if (Status == ERROR_SUCCESS)
		{	Status=RegOpenKeyEx(
					ControlSetKey,
					TEXT("Services"),
					0,
					KEY_READ,
					&ServicesKey);	//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services
            if (Status == ERROR_SUCCESS)
			{	Status=RegOpenKeyEx(
						ServicesKey,
						drivername,
						0,
						KEY_READ,
						&NdisPerfKey);	//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\bacmacnt
 				if (Status == ERROR_SUCCESS)
				{	Status=RegOpenKeyEx(
							NdisPerfKey,
							TEXT("Linkage"),
							0,
							KEY_READ,
							&LinkageKey);	//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\bacmacnt\Linkage
					if (Status == ERROR_SUCCESS)
					{	if (type==export)				//						***001
						{	Status=RegQueryValueEx(
								LinkageKey,
								TEXT("Export"),
								NULL,
								&RegType,
								(LPBYTE)pStr,
								BufferSize);
							RegCloseKey(LinkageKey);	//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\BACMACNT\Linkage\Export
						}
						else	//(bind)				//						***001 Begin
						{	Status=RegQueryValueEx(
								LinkageKey,
								TEXT("Bind"),
								NULL,
								&RegType,
								(LPBYTE)pStr,
								BufferSize);
							RegCloseKey(LinkageKey);	//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\BACMACNT\Linkage\Export
						}								//						***001 End
					}
					RegCloseKey(NdisPerfKey);
                }
                RegCloseKey(ServicesKey);
            }
            RegCloseKey(ControlSetKey);
        }
        RegCloseKey(SystemKey);
    }
    return Status;
}

///////////////////////////////////////////////////////////////////////		***004 Begin
//	Returns the node address of an adapter.
//
//
//	in:		AdapterObject	an adapter object
//			paddr			pointer to the array in which to store the address
//	out:	the number of bytes in the address

__declspec(dllexport) WORD Packet32GetAddress(LPADAPTER AdapterObject,PBYTE paddr)
{	BOOLEAN		Status;
	ULONG		IoCtlBufferLength=(sizeof(PACKET_OID_DATA)+127);	//let's make sure we have enough to get an address
    PPACKET_OID_DATA  OidData;
	WORD		n=0;
	ULONG		*mediatype;	

    OidData=mymalloc(IoCtlBufferLength);					//allocate some memory***007
	if (OidData == NULL) return 0;							//allocation failed
	OidData->Oid=OID_GEN_MEDIA_IN_USE;
	OidData->Length=4*sizeof(ULONG);
	Status=Packet32Request(AdapterObject,FALSE,OidData);
	if (Status)
	{	mediatype=(ULONG *)(OidData->Data);
		switch (*mediatype)
		{
		case NdisMedium802_3:
			OidData->Oid=OID_802_3_CURRENT_ADDRESS;					//get ethernet node address
			OidData->Length=ETH_802_3_ADDRESS_LENGTH;
			Status=Packet32Request(AdapterObject,FALSE,OidData);	//get it
			if (Status)
			{	n=ETH_802_3_ADDRESS_LENGTH;
				memcpy (paddr,OidData->Data,n);
			}
			break;
		case NdisMediumArcnet878_2:
		case NdisMediumArcnetRaw:
			OidData->Oid=OID_ARCNET_CURRENT_ADDRESS;				//get ARCNET node address
			OidData->Length=ARCNET_ADDRESS_LENGTH;
			Status=Packet32Request(AdapterObject,FALSE,OidData);	//get it
			if (Status)
			{	n=ARCNET_ADDRESS_LENGTH;
				memcpy (paddr,OidData->Data,n);
			}
			break;
		}
 	}
    myfree(OidData);												//		***007
	return n;
}

///////////////////////////////////////////////////////////////////////		***004 Begin
//	Returns the maximum frame size of an adapter.
//
//
//	in:		AdapterObject	an adapter object
//	out:	the maximum number of bytes in the frame

__declspec(dllexport) WORD Packet32GetMaxFrameSize(LPADAPTER AdapterObject)
{	BOOLEAN		Status;
	ULONG		IoCtlBufferLength=(sizeof(PACKET_OID_DATA)+15);	//let's make sure we have enough to get an address
    PPACKET_OID_DATA  OidData;
	WORD		n=0;
	ULONG		*maxsize;	

    OidData=mymalloc(IoCtlBufferLength);					//allocate some memory	***007
	if (OidData == NULL) return 0;							//allocation failed
	OidData->Oid=OID_GEN_MAXIMUM_FRAME_SIZE;
	OidData->Length=sizeof(ULONG);
	Status=Packet32Request(AdapterObject,FALSE,OidData);
	if (Status)
	{	maxsize=(ULONG *)(OidData->Data);
		n=(WORD)(*maxsize);
 	}
    myfree(OidData);										//				***007
	return n;
}																	//		***004 End

/*
///////////////////////////////////////////////////////////////////////		***001 Begin
//	Returns the node address of an adapter.
//
//	Adapted from macaddr on the NT4.0 DDK
//
//	in:		MacDeviceName	in the form /Device/DC21X41 (WCHAR)
//			paddr			pointer to the array in which to store the address
//	out:	the number of bytes in the address (always 6)

__declspec(dllexport) WORD Packet32GetAddress(LPTSTR MacDeviceName,PBYTE paddr)

{	TCHAR       LinkName[512];					//changed all from UCHAR to TCHAR
    TCHAR       szMACFileName[80];
    UCHAR       OidData[4096];
    NDIS_OID    OidCode;
    BOOLEAN     bCreatedDevice;
    DWORD       ErrorNumber;
    DWORD       ReturnedCount;
    HANDLE      hMAC;

//	Check to see if the DOS name for the MAC driver already exists.
//	It's not created automatically in version 3.1 but may be later.
//
    memset(paddr,0,6);						//in case we can't find the address
	if (QueryDosDevice(&MacDeviceName[8],LinkName,sizeof(LinkName))==0)
    {	if ((ErrorNumber=GetLastError())==ERROR_FILE_NOT_FOUND)
        {
//	It doesn't exist so create it.
            if (!DefineDosDevice(DDD_RAW_TARGET_PATH,&MacDeviceName[8],MacDeviceName))
				return 6;									//oops
            else
				bCreatedDevice=TRUE;
        }
//	QueryDosDevice returned an error
		else return 6;
    }
//	It already existed
    else
    {
        bCreatedDevice=FALSE;
    }

//	Construct a device name to pass to CreateFile
	wsprintf(szMACFileName,TEXT("\\\\.\\%s"),&MacDeviceName[8]);	//becomes \\.\DC21X41
    hMAC=CreateFile(
					szMACFileName,
					GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					0,
					INVALID_HANDLE_VALUE
					);
    if (hMAC!=INVALID_HANDLE_VALUE)
    {
//	We successfully opened the driver, format the IOCTL to pass the driver.
        //
		OidCode=OID_802_3_CURRENT_ADDRESS;

        if (DeviceIoControl(
							hMAC,
							IOCTL_NDIS_QUERY_GLOBAL_STATS,
							&OidCode,
							sizeof(OidCode),
							OidData,
							sizeof(OidData),
							&ReturnedCount,
							NULL
							))
        {	if (ReturnedCount==6) memcpy(paddr,OidData,6);
        }
    }

    if (bCreatedDevice)
    {
//	The MAC driver wasn't visible in the Win32 name space so we created
//	a link. Now we have to delete it.
        DefineDosDevice(DDD_RAW_TARGET_PATH|DDD_REMOVE_DEFINITION|DDD_EXACT_MATCH_ON_REMOVE,
						&MacDeviceName[8],MacDeviceName);
	}
    return 6;
}																//		***001 End
*/																//		***004 End

//																		***010 Begin
///////////////////////////////////////////////////////////////////////
//	Issue a IoCtr request to the driver to return the number of BACnet 
//  packets received or transmitted.
//
//	in:		AdapterObject	an adapter object
//			oidreq			ther request to get rx ot tx frames
//  out:	count of packets received

ULONG GetCount(LPADAPTER AdapterObject,ULONG oidreq)
{	byte		oid[sizeof(PACKET_OID_DATA)+16];
    PPACKET_OID_DATA  OidData=(PACKET_OID_DATA *)oid;
	ULONG		*count,n=0;
	BOOLEAN		Status;

	OidData->Oid=oidreq;
	OidData->Length=(sizeof(ULONG))*4;
	count=(ULONG *)(OidData->Data);
	*count=0;
	Status=Packet32Request(AdapterObject,FALSE,OidData);
	if (Status) n=*count;
	return n;
}
///////////////////////////////////////////////////////////////////////
//	Issue a IoCtl request to the driver to return the number of BACnet 
//  packets received.
//
//	in:		AdapterObject	an adapter object
//  out:	count of packets received

__declspec(dllexport) ULONG Packet32GetRxCount(LPADAPTER AdapterObject)
{	
	return GetCount(AdapterObject,OID_GEN_DIRECTED_FRAMES_RCV);
}

///////////////////////////////////////////////////////////////////////
//	Issue a IoCtl request to the driver to return the number of BACnet 
//  packets transmitted.
//
//	in:		AdapterObject	an adapter object
//  out:	count of packets transmitted

__declspec(dllexport) ULONG Packet32GetTxCount(LPADAPTER AdapterObject)
{	
	return GetCount(AdapterObject,OID_GEN_DIRECTED_FRAMES_XMIT);
}

///////////////////////////////////////////////////////////////////////
//	Issue a IoCtl request to the driver to return performance counts from 
//  the driver.
//
//	in:		AdapterObject	an adapter object
//			PData			place to put stats

__declspec(dllexport) VOID Packet32GetPerfStats(LPADAPTER AdapterObject,PERF_DATA *PData)
{	byte		oid[sizeof(PACKET_OID_DATA)+sizeof(PERF_DATA)];
    PPACKET_OID_DATA  OidData=(PACKET_OID_DATA *)oid;
	PERF_DATA	*pd;
	BOOLEAN		Status;

	OidData->Oid=OID_BACMAC_GET_PERF_DATA;
	OidData->Length=sizeof(PERF_DATA);
	pd=(PERF_DATA *)(OidData->Data);
	Status=Packet32Request(AdapterObject,FALSE,OidData);
	if (Status)
	{	PData->RxCount=pd->RxCount;
		PData->TxCount=pd->TxCount;
		PData->RxNonBACnet=pd->RxNonBACnet;
		PData->RxNoUserBuff=pd->RxNoUserBuff;
		PData->RxNoNDISBuff=pd->RxNoNDISBuff;
		PData->TxNoNDISBuff=pd->TxNoNDISBuff;
	}
	else
	{	PData->RxCount=0;
		PData->TxCount=0;
		PData->RxNonBACnet=0;
		PData->RxNoUserBuff=0;
		PData->RxNoNDISBuff=0;
		PData->TxNoNDISBuff=0;
	}
}
//																		***010 End
//																		***007 Begin
///////////////////////////////////////////////////////////////////////
// My very own malloc -- since Microsoft oscillates between using 
// malloc free and not!

__declspec(dllexport) void *mymalloc(size_t n)
{
//	return GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT,n);				***008
	return malloc(n);						//							***008
}

///////////////////////////////////////////////////////////////////////
// My very own free -- since Microsoft oscillates between using 
// malloc free and not!

__declspec(dllexport) void myfree(void *p)
{	
/*	char	b[256];			NOTE: This is example of OutputDebugString

	wsprintf((LPTSTR)b,TEXT("\nMyfree->0x%.8X"),p);
	OutputDebugString((LPCTSTR)b);
*/

//	GlobalFreePtr(p);													***008
	free(p);								//							***008
}
//																		***007 End
///////////////////////////////////////////////////////////////////////	***009 Begin
// My very own free -- since Microsoft oscillates between using 
// malloc free and not!

__declspec(dllexport) size_t mysize(void *p)
{	//size_t	size;	
//	size=GlobalSize(GlobalPtrHandle(p));
//	return size;
	return _msize(p);
}											//							***009 End
