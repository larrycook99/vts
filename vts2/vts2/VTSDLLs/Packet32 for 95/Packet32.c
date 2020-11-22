/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1990  Microsoft Corporation
	
module:		PACKET32.C
desc:		32-bit MAC Layer Packet Interface - WIN95
authors:	BrianL, Jack Neyer
last edit:	29-Oct-99 [014] JN  fix for multiprocessing
			28-Oct-99 [013] JN  add mysize
			18-Oct-99 [012] JN  fix getperformance stats
			07-Oct-99 [011] JN  reduce MAXADAPTERS to 2
			24-Sep-99 [010] JN  support for diagnostics
			23-Jun-99 [009] JN  fix media type checking back to GlobalAllocPtr and GlobalFreePtr
			16-Jun-99 [008] JN  use malloc and free in mymalloc, myfree
			31-Mar-99 [007] JN  implemented mymalloc and myfree as publics for all dlls
								(should have made a separate lib but I'm lazy so I put it
								 here cause everybody uses this lib module anyway)
								 changed GlobalAllocPtr/GlobalFree to mymalloc/myfree
			30-Sep-98 [006] JN  add support for multiple adapters
			29-Jul-98 [005] JN  add support for ARCNET
			12-Mar-98 [004] JN  add blocking option to Packet32PacketWait
								add auto/manual reset for CreateEvent in Packet32AllocatePacket
			06-Feb-98 [003] JN  add get GenMacOptions
			23-Jan-98 [002] JN  made changes to agree with NT version
								(i.e. to Packet32Wait) 
			16-Dec-97 [001] JN  first cut (changes from the template ARE marked)
								removed ASSERTE calls (not marked)
------------------------------------------------------------------------------*/
// This library was taken from the WIN95 Packet Driver packet.exe and was adapted from 
// a DLL to an include library for linking to BACDOR95. It provides a set of procedures 
// for accessing MAC layer packets using file model (i.e. file open, close, read, write, 
// create event,etc.)
//
// The basic changes are:
//	1) rename the functions from PacketXXX to Packet32XXX
//	2) add some clarifying comments.
//	3) add a "get adapter address" function
//	4) change the WaitPacketRoutine to NOT wait forever

// Use of this module requires the kernel-mode driver BACMAC95.VXD that maps file 
// functions to NDIS through the ioctl (DeviceIoControl) interface.
//
// PACKET32 loads BACMAC95.VXD ("TheAdapter") dynamically when Packet32OpenAdapter is called
// by doing a CreateFile ("BACMAC95"). The CreateFile specifies ASYNC operation through the
// FILE_FLAG_OVERLAPPED parameter. Packet32OpenAdapter also gets the MaxFrameSize and 
// MaxLookAheadSize for "TheAdapter" as well as the Current(MAC)Address and Permanent(MAC)
// Address. These stats are kept in lpTheAdapter which also contains a handle to the open
// file.

// PACKET32 unloads BACMAC95.VXD when Packet32CloseAdapter is called by doing a 
// CloseHandle(lpTheAdapter.hFile).
//
// Reads/Writes are done by the app first calling Packet32AllocatePacket/Packet32InitPacket 
// to allocate a packet and create an overlapped, non-signalled event through CreateEvent.
// The handle to the event is kept in the allocated packet structure. Packet32ReceivePacket/
// Packet32SendPacket is then called with the adapter object lpTheAdapter and the event
// handle specifying asynchronous operation. Packet32WaitPacket which calls 
// GetOverlappedResult with the file handle for "TheAdapter" and the overlapped buffer
// pointer for the event until the event completes. The event must be closed and the
// event packet freed by calling Packet32FreePacket which does a CloseHandle on the 
// event.
//  
#include <windows.h>
#include <windowsx.h>
//#include <crtdbg.h>									//				***001
//#include <malloc.h>										//				***013
#include "ntddndis.h"
#include "ntddpack.h"
#include "packet32.h"

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif /* __cplusplus */

typedef int NDIS_STATUS, *PNDIS_STATUS; // note default size

#define STATUS_NOT_SUPPORTED			(0xC00000BBL)
#define NDIS_STATUS_NOT_SUPPORTED		((NDIS_STATUS)STATUS_NOT_SUPPORTED)

#pragma data_seg("BDSHARE")								//										***148 Begin
TCHAR		szWindowTitle[]	= TEXT("PACKET32.LIB");		//					***014
LPADAPTER	lpTheAdapter[MAXADAPTERS]= {NULL,NULL};		//Our adapters		***006***011
ULONG		ulInstanceCount[MAXADAPTERS]= {0,0};		//					***006***011
ADAPTER		TheAdapter[MAXADAPTERS]={0,0};				//					***014
#pragma data_seg()										//					***014

#if DBG
#define ODS(_x) OutputDebugString(TEXT(_x))
#else
#define ODS(_x)
#endif

// Function Prototypes
BOOLEAN	StartPacketDriver(LPTSTR ServiceName,int i);	//						***006
BOOLEAN	StopPacketDriver(int i);						//						***006

/*														//						***006 Begin
///////////////////////////////////////////////////////////////////////			***001 Begin
//	Initialize the Packet Interface by trying to load/unload the associated 
//  kernel-mode packet driver (BACMAC95). This entry is provided so that
//  an associated DLL to which it is linked can call it during its load/unload 
//  sequence.
//
//	in:		DllHandle - Not used
//			Reason - Attach or Detach
//			Context - Not Used
//	out:	SUCCESS - TRUE; FAILURE - FALSE

BOOL APIENTRY Packet32Init( HANDLE hModule,DWORD dwReason,LPVOID lpReserved)
//																				***001 End
{	BOOLEAN Status;
	ODS("Packet32: Init\n");							//						***001
	switch ( dwReason ) 
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		Status = StartPacketDriver(TEXT("BACMAC95"));	//						***001
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		Status = StopPacketDriver();
		break;

	default:
		Status = TRUE;
		break;
	}
	return Status;
}
*/														//					***006 End

///////////////////////////////////////////////////////////////////////
//	Obtain a response from the NIC to an OID through Packet32Requests which
//  does Packet32DeviceIoControl which does DeviceIoControl which is 
//  handled in BACMAC95 in its PacketIoControl procedure.
//
//	in:		lpAdapter - Adapter object returned from PacketOpenAdapter
//			ulOid - The OID request
//			ulLength - a length
//	out:	SUCCESS returns a pointer to a packet
//			FAILURE - NULL

LPPACKET APIENTRY Packet32QueryOid(LPADAPTER lpAdapter,ULONG ulOid,ULONG ulLength)
{	ULONG		ioctl;
	LPPACKET	lpPacket;
	#define pOidData ((PPACKET_OID_DATA)(lpPacket->Buffer))

	lpPacket = Packet32AllocatePacket(lpAdapter,FALSE);		//						***004
	if (lpPacket)
	{	if (Packet32InitPacket( lpPacket,sizeof(PACKET_OID_DATA)-1+ulLength ))
//	It looks as if ioctl is NOT used
			ioctl=(ulOid >= OID_802_3_PERMANENT_ADDRESS) ? IOCTL_PROTOCOL_QUERY_OID : IOCTL_PROTOCOL_STATISTICS;
			pOidData->Oid=ulOid;
			pOidData->Length=ulLength;
			if ( Packet32Request( lpAdapter, lpPacket, FALSE ) )		//DeviceIoControl
			{	return lpPacket;
			}
		Packet32FreePacket( lpPacket );
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////
//	Obtain a statistic about the adapter through an OID
//
//	in:		lpAdapter - Adapter object returned from PacketOpenAdapter
//			ulOid - The OID request
//	out:	SUCCESS returns the requested statistic
//			FAILURE - 0L

ULONG APIENTRY Packet32QueryULStatistic(LPADAPTER lpAdapter,ULONG ulOid)

{	ULONG		ulStatistic = 0L;
	LPPACKET	lpPacket = Packet32QueryOid( lpAdapter, ulOid, 4L );	//Get the statistic

	if ( lpPacket )
	{	ulStatistic = *(ULONG *)((BYTE *)(lpPacket->Buffer) + sizeof(PACKET_OID_DATA) - 1);
		Packet32FreePacket( lpPacket );
	}
	return ulStatistic;
}


///////////////////////////////////////////////////////////////////////
//	Open an instance of the adapter by loading the BACMAC95.VXD through
//  StartPacketDriver which does a CreateFile.
//
//	in:		lpstrAdapterName - unicode name of the adapter to open
//
// NOTE: lpstrAdapterNMame is NOT used!! This is because Win95 doen NOT
//       support symbolic links!?!?!
//
//	out:	SUCCESS - returns a pointer to an adapter object
//			FAILURE - NULL

LPADAPTER APIENTRY Packet32OpenAdapter(LPTSTR lpstrAdapterName)
{	LPPACKET lpPacket;
	ULONG		*mtp,mediatype;				//											***005***009
	int			i;							//											***006
	LPTSTR		lpstr;						//											***006
#define lpOidData	((PPACKET_OID_DATA)(lpPacket->Buffer))		//						***010
//											//Lets find the adapter number to open		***006 Begin
											//lpstrAdapterName = "BACMAC95_x" where x is the adapter number
	lpstr=lpstrAdapterName;
	i=strlen(lpstrAdapterName);
	i=(int)(lpstr[i-1]-'0');
	if (i>MAXADAPTERS-1) return NULL;
//											//											***006 End	
	ODS("Packet32: PacketOpenAdapter\n");
// Make sure packet driver is started
	StartPacketDriver(TEXT("BACMAC95"),i);	//this loads the VXD and returns a handle	***001***006

	if ( lpTheAdapter[i] && ( lpTheAdapter[i]->hFile != INVALID_HANDLE_VALUE ) )	;	//***006
	{
// Get some adapter statistics to use later
		lpTheAdapter[i]->ulMaxFrameSize=Packet32QueryULStatistic( lpTheAdapter[i], OID_GEN_MAXIMUM_FRAME_SIZE );	//	***006
		lpTheAdapter[i]->ulMaxLookAheadSize=Packet32QueryULStatistic( lpTheAdapter[i], OID_GEN_MAXIMUM_LOOKAHEAD );	//	***006
		lpTheAdapter[i]->ulGenMacOptions=0x12345678;			//so we know if it changes						//***003***006
		lpTheAdapter[i]->ulGenMacOptions=Packet32QueryULStatistic( lpTheAdapter[i], OID_GEN_MAC_OPTIONS );	//***003	***006
//																					***005 Begin
		lpPacket = Packet32QueryOid(lpTheAdapter[i],OID_GEN_MEDIA_IN_USE,4*sizeof(ULONG));	//***006
		if (lpPacket==NULL) return NULL;
		mtp=(ULONG *)(((BYTE *)(lpPacket->Buffer))+sizeof(PACKET_OID_DATA)-1);	//		***009 Begin
		mediatype=*mtp;
		lpOidData->Oid=OID_BACMAC_GET_PERF_DATA;				//						***010
		Packet32Request(lpTheAdapter[i],lpPacket,TRUE);			//						***010
		Packet32FreePacket(lpPacket);
		switch (mediatype)														//		***009 End
		{
		case NdisMedium802_3:											//			***005 End
// Get Current Address
			lpPacket = Packet32QueryOid( lpTheAdapter[i],OID_802_3_CURRENT_ADDRESS,ETH_802_3_ADDRESS_LENGTH );	//***006
			if ( lpPacket )
			{	memcpy(lpTheAdapter[i]->CurrentAddress,
						((BYTE *)(lpPacket->Buffer))+sizeof(PACKET_OID_DATA)-1, 
						ETH_802_3_ADDRESS_LENGTH);						//***006
				lpTheAdapter[i]->AddressLength=ETH_802_3_ADDRESS_LENGTH;	//***006
				Packet32FreePacket(lpPacket);
				lpTheAdapter[i]->ulAdapterType=ETH_802_3_ADAPTER;		//			***005***006
			}
			else return NULL;											//			***005
// Get Permanent Address
			lpPacket = Packet32QueryOid( lpTheAdapter[i],OID_802_3_PERMANENT_ADDRESS, ETH_802_3_ADDRESS_LENGTH );	//***006
			if ( lpPacket )
			{	memcpy(lpTheAdapter[i]->PermanentAddress,
						((BYTE *)lpPacket->Buffer)+sizeof(PACKET_OID_DATA)-1, 
						ETH_802_3_ADDRESS_LENGTH);						//			***006
				Packet32FreePacket( lpPacket );
			}
			else return NULL;											//			***005
			break;														//			***005
		case NdisMediumArcnetRaw:										//			***005 Begin
		case NdisMediumArcnet878_2:
// Get Current Address
			lpPacket = Packet32QueryOid( lpTheAdapter[i],OID_ARCNET_CURRENT_ADDRESS,ARCNET_ADDRESS_LENGTH );	//***006
			if ( lpPacket )
			{	memcpy(lpTheAdapter[i]->CurrentAddress, 
						((BYTE *)(lpPacket->Buffer))+sizeof(PACKET_OID_DATA)-1, 
						ARCNET_ADDRESS_LENGTH);							//			***006
				lpTheAdapter[i]->AddressLength=ARCNET_ADDRESS_LENGTH;	//***006
				lpTheAdapter[i]->ulAdapterType=ARCNET_ADAPTER;			//			***006
				Packet32FreePacket(lpPacket);
			}
			else return NULL;
			lpPacket = Packet32QueryOid( lpTheAdapter[i],OID_ARCNET_PERMANENT_ADDRESS, ARCNET_ADDRESS_LENGTH );	//	***006
			if ( lpPacket )
			{	memcpy(lpTheAdapter[i]->PermanentAddress, 
						((BYTE *)lpPacket->Buffer)+sizeof(PACKET_OID_DATA)-1, 
						ARCNET_ADDRESS_LENGTH);							//			***006
				Packet32FreePacket( lpPacket );
			}
			else return NULL;
			break;
		default:
			return NULL;
		}															//				***005 End
		return lpTheAdapter[i];										//				***006
	}
	ODS("Packet32: PacketOpenAdapter Could not open adapter\n");
	return NULL;
}

///////////////////////////////////////////////////////////////////////
//	Close an instance of a previosuly opened adapter by unloading 
//  BACMAC95.VXD through StopPacketDriver which does a CloseHandle.
//
//	in:		lpAdapter-Adapter object returned from PacketOpenAdapter

VOID APIENTRY Packet32CloseAdapter( LPADAPTER lpAdapter )
{	int		i;														//			***006
	ODS("Packet32: PacketCloseAdapter\n");
/*
	CloseHandle(lpAdapter->hFile);
	myfree(lpAdapter);												//			***007
*/
// ?????? Don't We Need This ???????
	for (i=0;i<MAXADAPTERS-1;i++)										//			***006 Begin
	{	if ((lpTheAdapter[i]!=NULL) && (lpTheAdapter[i]->hFile==lpAdapter->hFile)) break;
	}
	if (i<MAXADAPTERS) StopPacketDriver(i);	//Unload the Packet Driver		***001	***006 End
}

///////////////////////////////////////////////////////////////////////
//	Allocate a packet object for use in sending and receiving packets.
//  This also creates an overlapped (async), non-signalled event for the 
//  packet using CreateEvent.
//
// in:		AdapterObject - Adapter object returned from PacketOpenAdapter
//
// NOTE: AdapterObject is NOT used!!
//
// out:		SUCCESS - returns packet object
//			FAILURE - NULL
LPPACKET APIENTRY Packet32AllocatePacket( LPADAPTER AdapterObject,BOOLEAN manreset)	//	***004
{	LPPACKET lpPacket;

	lpPacket=(LPPACKET)mymalloc(sizeof(PACKET));										//	***007
	if ( lpPacket == NULL ) 
	{	ODS("Packet32: PacketAllocateSendPacket: GlobalAlloc Failed\n");
		return NULL;
	}
	lpPacket->OverLapped.hEvent=CreateEvent(NULL,manreset,FALSE,NULL);		// auto/manual-reset,non-signalled, overlapped***004
	if (lpPacket->OverLapped.hEvent==NULL)
	{	ODS("Packet32: PacketAllocateSendPacket: CreateEvent Failed\n");
		myfree(lpPacket);					//						***007
		return NULL;
	}
	lpPacket->Buffer=NULL;
	lpPacket->Length=0;
	lpPacket->ulBytesReceived=0;
	lpPacket->bIoComplete=FALSE;
	lpPacket->OverLapped.Offset=0;			//						***002
	lpPacket->OverLapped.OffsetHigh=0;		//						***002
	return lpPacket;
}

///////////////////////////////////////////////////////////////////////
//	Free a packet object used in sending and receiving packets.
//  This also closes the event for that packet object.
//
// in:	lpPacket - packet returned from PacketAllocatePacket

VOID APIENTRY Packet32FreePacket( LPPACKET lpPacket )
{	
	if (lpPacket->Buffer) myfree(lpPacket->Buffer);			//		***007
	CloseHandle(lpPacket->OverLapped.hEvent);
	myfree(lpPacket);										//		***007
}

///////////////////////////////////////////////////////////////////////
//	Initialize a packet object to point to a memory buffer described by
//	Start address and length
//
//	in:		lpPacket - Packet object returned by PacketAllocatePacket
//			Length - Length of memory buffer

BOOLEAN APIENTRY Packet32InitPacket(LPPACKET lpPacket,UINT Length)
{	
	if (lpPacket->Buffer) myfree( lpPacket->Buffer );				//					***007
	lpPacket->Buffer=(LPPACKET) mymalloc(Length);					//					***007
	if (lpPacket->Buffer) 																
	{	lpPacket->Length=Length;
		memset(lpPacket->Buffer,0,Length);
	}
	else
		lpPacket->Length = 0;

	return (BOOLEAN) (lpPacket->Length != 0);
}

///////////////////////////////////////////////////////////////////////
//	Send an ioctl packet to the adapter. Used to start read, write, etc.
//  This is passed along the BACMAC95.VXD which manages the DeviceIoControl
//  functions.
//
//	in:		lpAdapterObject  - AdapterObject return by PacketOpenAdapter
//			lpPacket         - Packet object returned by PacketAllocatePacket and initialized
//								by PacketInitPacket
//			ulIoctl			 - the ioctl command
//			bSync            - TRUE if service should wait for packet to transmit
//
//	out:	SUCCESS - TRUE if succeeded and SYNC==TRUE
//			FAILURE -

BOOLEAN APIENTRY Packet32DeviceIoControl( LPADAPTER lpAdapterObject,LPPACKET lpPacket,ULONG ulIoctl,BOOLEAN bSync)
{	BOOLEAN Result;

//  Too much !!!
//	ODS( "Packet32: PacketDeviceIoControl\n" );
//	_ASSERTE(lpAdapterObject!=NULL);
//	_ASSERTE(lpPacket!=NULL);

	lpPacket->OverLapped.Offset=0;
	lpPacket->OverLapped.OffsetHigh=0;
	lpPacket->ulBytesReceived=0;

	if (!ResetEvent(lpPacket->OverLapped.hEvent)) 
	{	lpPacket->bIoComplete = FALSE;
		return FALSE;
	}
	Result=DeviceIoControl(lpAdapterObject->hFile,
								ulIoctl, 
								lpPacket->Buffer, 
								lpPacket->Length, 
								lpPacket->Buffer, 
								lpPacket->Length, 
								&(lpPacket->ulBytesReceived), 
								&(lpPacket->OverLapped) );

	if (!Result && bSync)
	{
//  They want to wait
		if (GetLastError()==ERROR_IO_PENDING)
		{	Result=GetOverlappedResult (lpAdapterObject->hFile,
										&(lpPacket->OverLapped),
										&(lpPacket->ulBytesReceived),
										TRUE);
		}
		else
			ODS( "Packet32: unsupported API call return error!\n" );
	} 
	lpPacket->bIoComplete = Result;
	return Result;
}

///////////////////////////////////////////////////////////////////////
//    Send a packet to the adapter through ioctl.
//
//	in:		AdapterObject - AdapterObject return by PacketOpenAdapter
//			lpPacket - Packet object returned by PacketAllocatePacket and initialized
//						by PacketInitPacket
//			Sync- TRUE if service should wait for packet to transmit
//
// out:		SUCCESS - TRUE if succeeded and SYNC==TRUE
//			FAILURE -

BOOLEAN APIENTRY Packet32SendPacket(LPADAPTER AdapterObject,LPPACKET lpPacket, BOOLEAN Sync)

{	return Packet32DeviceIoControl( AdapterObject,
								  lpPacket,
								  IOCTL_PROTOCOL_WRITE,
								  Sync);
}

///////////////////////////////////////////////////////////////////////
//	Issue a receive request from the adapter through ioctl.
//
// in:		AdapterObject - AdapterObject return by PacketOpenAdapter
//			lpPacket - Packet object returned by PacketAllocatePacket and initialized
//			Sync - TRUE if service should wait for packet to transmit
//
// out:		SUCCESS - TRUE if succeeded and SYNC==TRUE
//			FAILURE -

BOOLEAN APIENTRY Packet32ReceivePacket(LPADAPTER AdapterObject, LPPACKET lpPacket, BOOLEAN Sync)

{	return Packet32DeviceIoControl(	AdapterObject,
									lpPacket,
									IOCTL_PROTOCOL_READ,
									Sync );
}

//																***002 Begin
///////////////////////////////////////////////////////////////////////
//	Wait for an overlapped IO (async) on a packet to complete.
//	Called if the send or receive call specified FALSE for the Sync parmeter
//	in:		AdapterObject - AdapterObject return by PacketOpenAdapter
//			lpPacket - Packet object returned by PacketAllocatePacket and 
//						initialized	by PacketInitPacket
//			bWait			wait for completion						***004			
//	out:    ERROR_SUCCESS or ERROR_INCOMPLETE or ERROR_xxx
//			FAILURE -

DWORD APIENTRY Packet32WaitPacket(LPADAPTER AdapterObject,LPPACKET lpPacket,PULONG BytesReceived,BOOLEAN bWait)	//***004
{
	lpPacket->bIoComplete=GetOverlappedResult(	AdapterObject->hFile,
												&lpPacket->OverLapped,
												&lpPacket->ulBytesReceived,
												bWait);				//				***004
//												FALSE);				//don't wait	***004
	if (lpPacket->bIoComplete)
	{	*BytesReceived=lpPacket->ulBytesReceived;
		return ERROR_SUCCESS;
	}
	return GetLastError();											//		***002 End
}

///////////////////////////////////////////////////////////////////////
//	Resets the adapter. This will complete all pending sends receives and requests
//
//	in:		AdapterObject - AdapterObject return by PacketOpenAdapter
//	out:	SUCCESS - TRUE if overlapped call succeeded
//			FAILURE -

BOOLEAN APIENTRY Packet32ResetAdapter(LPADAPTER AdapterObject)
{	UINT BytesReturned;

	DeviceIoControl(AdapterObject->hFile,
					  (DWORD)IOCTL_PROTOCOL_RESET,
					  NULL,
					  0,
					  NULL,
					  0,
					  &BytesReturned,
					  NULL );
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
//	Issues a request to an adapter
//
//	in:		lpAdapterObject - AdapterObject return by PacketOpenAdapter
//			lpPacket - Packet object returned by PacketAllocatePacket and initialized
//			bSet - True means that the request is SET otherwise it is a query
//	out:	SUCCESS -
//			FAILURE -

BOOLEAN APIENTRY Packet32Request(LPADAPTER lpAdapterObject,LPPACKET lpPacket,BOOLEAN bSet)

{	BOOLEAN	Result = FALSE;

	ODS( "Packet32: PacketRequest\n" );
//	_ASSERTE( lpAdapterObject != NULL );
//	_ASSERTE( lpPacket != NULL );
//	_ASSERTE( lpPacket->Buffer != NULL );
	Result = Packet32DeviceIoControl( lpAdapterObject,
									lpPacket,
									(ULONG) ((bSet) ? IOCTL_PROTOCOL_SET_OID : IOCTL_PROTOCOL_QUERY_OID),
									TRUE );

	if ( lpPacket->ulBytesReceived == 0 )
	{
// There was an ndis error
		ODS( "Packet32: Ndis returned error to OID\n" );
		Result = FALSE;
	}
	return Result;
}

///////////////////////////////////////////////////////////////////////
//	Set the adapter's packet filter
//
//	in:		AdapterObject - AdapterObject return by PacketOpenAdapter
//			Filter - filter to be set
//	out:	SUCCESS -
//			FAILURE -

BOOLEAN APIENTRY Packet32SetFilter(LPADAPTER AdapterObject,ULONG Filter)
{	BOOLEAN	Status;
	ULONG		IoCtlBufferLength=(sizeof(PACKET_OID_DATA)+sizeof(ULONG)-1);
	LPPACKET	lpPacket;
//#define lpOidData	((PPACKET_OID_DATA)(lpPacket->Buffer))		//		***002***010

	ODS( "Packet32: PacketSetFilter\n" );
//	_ASSERTE( AdapterObject != NULL );

	lpPacket=Packet32AllocatePacket(AdapterObject,FALSE);		//		***004
	if (lpPacket && Packet32InitPacket( lpPacket, IoCtlBufferLength ))
	{	lpOidData->Oid					= OID_GEN_CURRENT_PACKET_FILTER;
		lpOidData->Length				= sizeof(ULONG);
		*((PULONG)lpOidData->Data)	= Filter;

		Status = Packet32Request( AdapterObject,lpPacket,TRUE );
		Packet32FreePacket( lpPacket );									
	}
	else
		Status = FALSE;

	return Status;
}

///////////////////////////////////////////////////////////////////////
//	Attempt to load the kernel mode packet driver BACMAC95.VXD.
//
//	in:		lpstrServiceName  - Name of packet driver
//			i				  - the relative adapter number (0-MAXADAPTERS-1)
//	out:	SUCCESS	- TRUE, FAILURE - FALSE

BOOLEAN StartPacketDriver(LPTSTR lpstrServiceName, int i)	//						***006
{	BOOLEAN Result;
	DWORD	lasterr;							//for debugging	***001
	LPPACKET	lpPacket;						//									***006
	int			*aptr;							//									***006

	ODS( "Packet32: StartPacketDriver\n" );

	if (lpTheAdapter[i])						//									***006
//	Driver is already started, bump the counter up, return
	{	ulInstanceCount[i]++;					//									***006		
		Result = TRUE;
	}
	else
	{	lpPacket=Packet32AllocatePacket(lpTheAdapter[i],FALSE);			//			***006
		if (lpPacket && Packet32InitPacket( lpPacket, sizeof (int) ))	//			***006
		{	//lpTheAdapter[i]=(LPADAPTER)mymalloc(sizeof(ADAPTER));	//				***006***007***014
			lpTheAdapter[i]=&TheAdapter[i];			//								***014
			if ( lpTheAdapter[i] == NULL )			//								***006 
			{	ODS( "Packet32: StartPacketDriver GlobalAlloc Failed\n" );
				Packet32FreePacket( lpPacket );			//							***006								
				return FALSE;
			}
		}
		else
		{	ODS( "Packet32: StartPacketDriver Get Packet Failed\n" );
			Packet32FreePacket( lpPacket );			//								***006								
			return FALSE;
		}
//	Try to load the VXD BACMAC95 by doing a CreateFile which in this case has the
//	name \\.\BACMAC95																***001
		wsprintf(lpTheAdapter[i]->SymbolicLink,TEXT("\\\\.\\%s"),lpstrServiceName);	//***006
		lpTheAdapter[i]->hFile = CreateFile(lpTheAdapter[i]->SymbolicLink,
										  GENERIC_WRITE | GENERIC_READ,
										  0,
										  NULL,
										  OPEN_EXISTING,
										  FILE_FLAG_OVERLAPPED,
										  0 );				//						***006

		if (lpTheAdapter[i]->hFile!=INVALID_HANDLE_VALUE)	//						***006
		{	aptr=(int *)lpPacket->Buffer;					//						***006 Begin
			*aptr=i;
// NOTE: We have to do this lame stuff because the Create does not pass the file handle on
//		  to BACMAC95 so that an association between the file handle and the adapter can be made
			Result = Packet32DeviceIoControl(lpTheAdapter[i],
									lpPacket,
									IOCTL_PROTOCOL_BIND_DEVICE,
									TRUE );					//						***006 End
//	We successfully opened a device. Keep track of the number of opens
			ulInstanceCount[i]++;		//											***006
			Result=TRUE;
		}
		else
		{	ODS("Packet32: StartPacketDriver Could not open adapter\n");
			//myfree(lpTheAdapter[i]);					//						***006***007***014
			lpTheAdapter[i] = NULL;			//										***006
			Result = FALSE;
			lasterr=GetLastError();			//				***001
		}
	}
	Packet32FreePacket( lpPacket );			//										***006								
	return Result;
}

///////////////////////////////////////////////////////////////////////
//	Attempt to unload the kernel mode packet driver BACMAC95.VXD.
//
//	in:		i  - adapter number										***006
//	out:	SUCCESS	- TRUE, FAILURE - FALSE

BOOLEAN StopPacketDriver( int i )						//			***006
{
	ODS( "Packet32: StopPacketDriver\n" );
	if (lpTheAdapter[i])								//			***006
	{	if ( --ulInstanceCount[i] == 0 )				//			***006
//	No more instances are open, close the device
		{	CloseHandle(lpTheAdapter[i]->hFile);		//			***006
//			myfree(lpTheAdapter[i]);				//			***006***007***014
			lpTheAdapter[i] = NULL;						//			***006
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
//	Return the names all adapters availible
//
//	in:		AdapterObject  - AdapterObject return by PacketOpenAdapter
//			lpPacket - Packet object returned by PacketAllocatePacket and initialized
// out:		SUCCESS/FAILURE

BOOLEAN APIENTRY Packet32GetAdapterNames(
						LPADAPTER lpAdapterObject,
						LPPACKET  lpPacket)
{
	ODS( "Packet32: PacketGetAdapterNames\n" );

//	_ASSERTE( lpAdapterObject != NULL );
//	_ASSERTE( lpPacket != NULL );
// Make Synchronous VxD call here
	return Packet32DeviceIoControl( lpAdapterObject,
								  lpPacket,
								  (ULONG)IOCTL_PROTOCOL_MACNAME,
								  TRUE );
}


///////////////////////////////////////////////////////////////////////		***001 Begin
//	Returns the node address of an adapter.
//
//	in:		paddr			pointer to the array in which to store the address
//	out:	the number of bytes in the address; 0=unknown adapter

WORD APIENTRY Packet32GetAddress(LPADAPTER lpAdapter,PBYTE paddr)	//		***006
{	int		i;														//		***006
	for (i=0;i<MAXADAPTERS-1;i++)									//		***006 Begin
	{	if ((lpTheAdapter[i]!=NULL) && (lpTheAdapter[i]->hFile==lpAdapter->hFile)) break;
	}																//		***006 End
	if (lpTheAdapter[i])											//		***006
	{	memcpy(paddr,lpTheAdapter[i]->CurrentAddress,ETH_802_3_ADDRESS_LENGTH);	//***006
		if (lpTheAdapter[i]->ulAdapterType==ETH_802_3_ADAPTER)		//			***005***006
			return ETH_802_3_ADDRESS_LENGTH;
		else													//			***005
			return ARCNET_ADDRESS_LENGTH;						//			***005
	}
	else return 0;
}																	//		***001 End

//																		***010 Begin
///////////////////////////////////////////////////////////////////////
//	Issue a IoCtr request to the driver to return the number of BACnet 
//  packets received or transmitted.
//
//	in:		AdapterObject	an adapter object
//			oidreq			ther request to get rx ot tx frames
//  out:	count of packets received

ULONG GetCount(LPADAPTER AdapterObject,ULONG oidreq)
{	ULONG		IoCtlBufferLength=(sizeof(PACKET_OID_DATA)+16);
	LPPACKET	lpPacket;
	ULONG		*count,n=0;
	BOOLEAN		Status;
//#define lpOidData	((PPACKET_OID_DATA)(lpPacket->Buffer))

	lpPacket=Packet32AllocatePacket(AdapterObject,FALSE);
	if (lpPacket && Packet32InitPacket(lpPacket, IoCtlBufferLength))
	{	lpOidData->Oid=oidreq;
		lpOidData->Length=sizeof(ULONG);
		count=(ULONG *)(lpOidData->Data);
		*count=0;
		Status = Packet32Request(AdapterObject,lpPacket,FALSE);
		Packet32FreePacket(lpPacket);									
		if (Status) n=*count;
	}
	return n;
}
///////////////////////////////////////////////////////////////////////
//	Issue a IoCtl request to the driver to return the number of BACnet 
//  packets received.
//
//	in:		AdapterObject	an adapter object
//  out:	count of packets received

ULONG APIENTRY Packet32GetRxCount(LPADAPTER AdapterObject)
{	
	return GetCount(AdapterObject,OID_GEN_DIRECTED_FRAMES_RCV);
}

///////////////////////////////////////////////////////////////////////
//	Issue a IoCtl request to the driver to return the number of BACnet 
//  packets transmitted.
//
//	in:		AdapterObject	an adapter object
//  out:	count of packets transmitted

ULONG APIENTRY Packet32GetTxCount(LPADAPTER AdapterObject)
{	
	return GetCount(AdapterObject,OID_GEN_DIRECTED_FRAMES_XMIT);
}

///////////////////////////////////////////////////////////////////////
//	Issue a IoCtl request to the driver to return performance counts from 
//  the driver.
//
//	in:		AdapterObject	an adapter object
//			PData			place to put stats

VOID APIENTRY Packet32GetPerfStats(LPADAPTER AdapterObject,PERF_DATA *PData)
{	ULONG		IoCtlBufferLength=(sizeof(PACKET_OID_DATA)+sizeof(PERF_DATA));
	LPPACKET	lpPacket;
	PERF_DATA	*pd;
	BOOLEAN		Status;

	PData->RxCount=0;
	PData->TxCount=0;
	PData->RxNonBACnet=0;
	PData->RxNoUserBuff=0;
	PData->RxNoNDISBuff=0;
	PData->TxNoNDISBuff=0;
	lpPacket=Packet32AllocatePacket(AdapterObject,FALSE);
	if (lpPacket && Packet32InitPacket(lpPacket, IoCtlBufferLength))
	{	lpOidData->Oid=OID_BACMAC_GET_PERF_DATA;
		lpOidData->Length=sizeof(PERF_DATA);
		Status = Packet32Request(AdapterObject,lpPacket,FALSE);
		if (Status)
		{	pd=(PERF_DATA *)lpOidData->Data;
			PData->RxCount=pd->RxCount;
			PData->TxCount=pd->TxCount;
			PData->RxNonBACnet=pd->RxNonBACnet;
			PData->RxNoUserBuff=pd->RxNoUserBuff;
			PData->RxNoNDISBuff=pd->RxNoNDISBuff;
			PData->TxNoNDISBuff=pd->TxNoNDISBuff;
		}
		Packet32FreePacket(lpPacket);					//				***012								
	}
}
//																		***010 End
//																		***007 Begin
///////////////////////////////////////////////////////////////////////
// My very own malloc -- since Microsoft oscillates between using 
// malloc free and not!

VOID * APIENTRY mymalloc(size_t n)
{	//void	*p;
	return GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT,n);		//***008 Begin***009
/*	p=malloc(n);
	ZeroMemory(p,n);
	return p;						//								***008 End
*/
}

///////////////////////////////////////////////////////////////////////
// My very own free -- since Microsoft oscillates between using 
// malloc free and not!

VOID APIENTRY myfree(void *p)
{	GlobalFreePtr(p);					//								***008***009
//	free (p);							//								***008
}
//																		***007 End
///////////////////////////////////////////////////////////////////////	***013 Begin
// My very own free -- since Microsoft oscillates between using 
// malloc free and not!

 size_t APIENTRY mysize(void *p)
{	size_t	size;	
	size=GlobalSize(GlobalPtrHandle(p));
	return size;
//	return _msize(p);
}											//							***013 End

#ifdef __cplusplus
}
#endif /* __cplusplus */
