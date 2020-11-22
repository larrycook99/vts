/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1997,  Microsoft Corporation
	
module:		REQUEST.C
desc:		Win 95 NDIS MAC Layer Packet Driver for BACDOOR BACnet driver
authors:	William Ingle, Jack Neyer
last edit:	26-Feb-98 [002] JN  fix for page lock/unlock crossing two pages
			12-Dec-97 [001] JN  first cut (only major programming changes are marked ***001)
------------------------------------------------------------------------------*/
// This library was taken from the MicroSoft Windows 95 packet driver.
//
// This module provides the VMM page lock/unlock functions for the driver and is meant to be 
// linked with the other modules for the driver to form the whole.
//
// Little else is changed in the driver, save some liberal comments to alleviate
// later confusion.

#define WANTVXDWRAPS

#include <basedef.h>
#include <vmm.h>
#include <debug.h>
#include <vxdwraps.h>
#include <vwin32.h>
#include <winerror.h>
#include "debug.h"

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

DWORD _stdcall PacketPageLock(DWORD, DWORD);
void  _stdcall PacketPageUnlock(DWORD, DWORD);

DWORD VXDINLINE
/*
	Check if all pages in a given range are committed so they can
	be locked.

in:		Page	the base page to check
		nPages	the number of pages to check
		Flags	the operation flags

out:	the number of committed pages in the range
*/
_PageCheckLinRange( DWORD Page, DWORD nPages, DWORD Flags )
{
	DWORD dw;
	
	_asm push [Flags]	
	_asm push [nPages]
	_asm push [Page]
	VMMCall( _PageCheckLinRange )
	_asm add esp, 0Ch
	_asm mov [dw], eax

	return (dw);
}

/*
	Lock memory using VMM services

in:		lpMem	memory to lock
		cbSize	number of bytes

out:	pointer to locked memory (NULL=failed to lock)
*/

DWORD _stdcall PacketPageLock(DWORD lpMem, DWORD cbSize)
{	DWORD LinOffset, nCommittedPages, nPages;

	if ( lpMem && cbSize )
	{	LinOffset = lpMem & PAGEMASK;		// page offset of memory to map
// Return global mapping of passed in pointer, as this new pointer
// is how the memory must be accessed out of context.
//
// We re-use lpMem at this point to get the return address from
// _LinPageLock.  If _LinPageLock returns zero we know an error
// occurred.  If we get a nonzero return then we must add the
// computed offset to the page address for the address of the 
// locked buffer.  Remember the original buffer exists at some 
// offset into the original virtual page.
//		nPages=NPAGES( cbSize );										//***002 Begin
		nPages=NPAGES (cbSize + LinOffset);		//account for buffer spanning more than one page
// Note: an alternative to the above fix would be
//		nPages = (PAGE(lpMem+cbSize) - PAGE(lpMem)) + NPAGES( cbSize );	//***002 End
		nCommittedPages = _PageCheckLinRange( PAGE( lpMem ), nPages, 0 );
		if ( nCommittedPages >= nPages )
		{	lpMem  = _LinPageLock( PAGE( lpMem ), nPages, PAGEMAPGLOBAL );
			lpMem += (( lpMem ) ? LinOffset : 0);
			return lpMem;
		}
	}
	return NULL;
}

/*
	Unlock memory using VMM services

in:		lpMem	memory to unlock
		cbSize	number of bytes
*/
void _stdcall PacketPageUnlock( DWORD lpMem, DWORD cbSize )
{	DWORD LinOffset;														//	***002
	if ( lpMem && cbSize )
	{
// Free globally mapped memory
//		_LinPageUnlock( lpMem / PAGESIZE , NPAGES( cbSize ), PAGEMAPGLOBAL );	//***002 Begin
		LinOffset = lpMem & PAGEMASK;
		_LinPageUnlock( lpMem / PAGESIZE , NPAGES( cbSize + LinOffset), PAGEMAPGLOBAL );
// Note: an alternative to the above fix would be
//		_LinPageUnlock( PAGE(lpMem),(PAGE(lpMem+cbSize) - PAGE(lpMem)) + NPAGES( cbSize ), PAGEMAPGLOBAL );		//***002 End
	}
}

#if DEBUG

#include <ndis.h>
#include "bacmac95.h"
// Declare the global debug flag for this driver.
//
// Declare with PACKET_DEBUG_INIT if you want to hit an int 3 in the initialization routine.
//
// ULONG PacketTraceImpt = PACKET_TRACE_IMPT | PACKET_DEBUG_INIT;
//

ULONG PacketTraceImpt = PACKET_TRACE_IMPT;

VOID DumpReceiveEntries( PLIST_ENTRY pListHead )
{	PLIST_ENTRY			pListEntry;
	PPACKET_RESERVED	pReserved;

	if ( !IsListEmpty( pListHead ) )
	{	pListEntry = pListHead->Flink;
		while ( pListEntry != pListHead )
		{	pReserved = CONTAINING_RECORD( pListEntry, PACKET_RESERVED, ListElement );
			DbgPrint( "          Entry %lx\n\r", pListEntry );
			DbgPrint( "               lpBuffer          %lx\n\r", pReserved->lpBuffer );
			DbgPrint( "               cbBuffer          %ld\n\r", pReserved->cbBuffer );
			DbgPrint( "               lpcbBytesReturned %lx\n\r", pReserved->lpcbBytesReturned );
			DbgPrint( "               lpoOverlapped     %lx\n\r", pReserved->lpoOverlapped );
			DbgPrint( "               hDevice           %lx\n\r", pReserved->hDevice );
			DbgPrint( "               tagProcess        %lx\n\r", pReserved->tagProcess );
			pListEntry = pListEntry->Flink;
		}
	}
}

VOID DumpRequestEntries( PLIST_ENTRY pList )
{	PLIST_ENTRY			pEntry;
	PPACKET_RESERVED	pReserved;
	PINTERNAL_REQUEST	pRequest;

	if ( !IsListEmpty( pList ) )
	{	pEntry = pList->Flink;
		while ( pEntry != pList )
		{	pReserved = CONTAINING_RECORD( pEntry, PACKET_RESERVED, ListElement );
			pRequest  = CONTAINING_RECORD( pReserved, INTERNAL_REQUEST, Reserved );
			DbgPrint( "          Entry %lx  Request %lx\n\r", pEntry, pRequest );
			pEntry = pEntry->Flink;
		}
	}
}

VOID DumpList( char n )
{	PLIST_ENTRY		pEntry, pList;
	POPEN_INSTANCE pOpen;

// At DriverEntry even GlobalDeviceExtension may not be valid
	if ( GlobalDeviceExtension )
	{	pList  = &GlobalDeviceExtension->OpenList;
		if ( !IsListEmpty( pList ) )
		{	pEntry = pList->Flink;
			while ( pEntry != pList )
			{	pOpen = CONTAINING_RECORD( pEntry, OPEN_INSTANCE, ListElement );
				DbgPrint( "     Adapter %lx\n\r", pOpen );
				switch ( n )
				{
				case '1':
					DumpReceiveEntries( &pOpen->RcvList );
					break;

				case '2':
					DumpRequestEntries( &pOpen->RequestList );
					break;

				case '3':
					DumpRequestEntries( &pOpen->ResetIrpList );
					break;
				}
				pEntry = pEntry->Flink;
			}
		}
	}
	return;
}


DWORD _stdcall PacketDebugQuery( void ) 
{	char outList = 1;

	Out_Debug_String( "Packet Debug Services:\n\r" );
	while ( 1 )
	{	char c;
		Out_Debug_String( "\n\r" );
		
		if ( outList )
		{
			outList = 0;
			Out_Debug_String( "[0] Adapter List\n\r" );
			Out_Debug_String( "[1] Receive List\n\r" );
			Out_Debug_String( "[2] Request List\n\r" );
			Out_Debug_String( "[3] Reset   List\n\r\n\r" );
			Out_Debug_String( "[4] Turn OFF Trace Information\n\r" );
			IF_PACKETDEBUG( PACKET_TRACE_IMPT )
				Out_Debug_String( "[5] TOGGLE Normal Trace OFF\n\r" );
			else
				Out_Debug_String( "[5] TOGGLE Normal Trace ON\n\r" );
			IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )
				Out_Debug_String( "[6] TOGGLE Trace ALL OFF\n\r" );
			else
				Out_Debug_String( "[6] TOGGLE Trace ALL ON\n\r" );
			IF_PACKETDEBUG( PACKET_DEBUG_INIT )
				Out_Debug_String( "[7] TOGGLE Trace Init OFF\n\r" );
			else
				Out_Debug_String( "[7] TOGGLE Trace Init ON\n\r" );
			IF_PACKETDEBUG( PACKET_DEBUG_BREAK )
				Out_Debug_String( "[8] TOGGLE Break on Trace OFF\n\r" );
			else
				Out_Debug_String( "[8] TOGGLE Break on Trace ON\n\r" );
			Out_Debug_String( "\r\n[h] Reprint Commands\r\n" );
		}
		Out_Debug_String( "Enter selection or Press ESC to Exit: " );
		
		VxDCall( In_Debug_Chr );
		_asm	jz		Debug_Exit
		_asm	mov		c, al
		Out_Debug_String( "\n\r" );

		switch ( c )
		{
		case '0':
			Out_Debug_String( "\n\rAdapter List:\n\r" );
			DumpList( c );
			break;

		case '1':
			Out_Debug_String( "\n\rRecieve List:\n\r" );
			DumpList( c );
			break;

		case '2':
			Out_Debug_String( "\n\rRequest List:\n\r" );
			DumpList( c );
			break;

		case '3':
			Out_Debug_String( "\n\rReset List:\n\r" );
			DumpList( c );
			break;

		case '4':
			PacketTraceImpt = 0;
			break;

		case '5':
			IF_PACKETDEBUG( PACKET_TRACE_IMPT )
			{	PacketTraceImpt &= ~(PACKET_TRACE_IMPT | PACKET_DEBUG_VERY_LOUD);
			}
			else
			{	PacketTraceImpt |= PACKET_TRACE_IMPT;
				PacketTraceImpt &= ~PACKET_DEBUG_VERY_LOUD;
			}
			break;

		case '6':
			IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )
				PacketTraceImpt &= ~PACKET_DEBUG_VERY_LOUD;
			else
				PacketTraceImpt |= PACKET_DEBUG_VERY_LOUD | PACKET_TRACE_IMPT;
			break;

		case '7':
			IF_PACKETDEBUG( PACKET_DEBUG_INIT )
				PacketTraceImpt &= ~PACKET_DEBUG_INIT;
			else
				PacketTraceImpt |= PACKET_DEBUG_INIT;
			break;

		case '8':
			IF_PACKETDEBUG( PACKET_DEBUG_BREAK )
				PacketTraceImpt &= ~PACKET_DEBUG_BREAK;
			else
				PacketTraceImpt |= PACKET_DEBUG_BREAK;
			break;

		default:
			outList = 1;
			break;
		}
	}

Debug_Exit:
	return( VXD_SUCCESS );
}
#endif

