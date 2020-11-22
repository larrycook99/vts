
#include "stdafx.h"
#include "ntddndis.h"

#include "WinPacket32.hpp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	BACnetLSAP			0x82

#define	NT_Packet32Debug	0					// set this for debugging messages

UINT WinPacket32ThreadFunc( LPVOID pParam );
void CheckSocketError( char *func );

//
//	Packet32 adapter list
//

WinPacket32AdapterDescPtr	gAdapterList = 0;
int							gAdapterListLen = 0;

short ConvertUnicodeToAscii( char *ap, wchar_t *up, short max );

//
//	InitAdapterList
//

void InitAdapterList( void )
{
	unsigned long	nameLength, addrLen
	;
	unsigned char	addrAddr[kMaxAddressLen]
	;
	wchar_t			adapterNames[128], *ap
	,				macDriverNames[128], *dp
	;
	LPADAPTER		adapter
	;

	// see if the adapter list needs to be initialized
	if (!gAdapterList) {
		// allocate the list
		gAdapterList = new WinPacket32AdapterDesc[ MAXADAPTERS ];

		// get device names
		nameLength = sizeof(adapterNames);
		adapterNames[0] = 0;
		Packet32GetDeviceNames( (char *)adapterNames, &nameLength, export );	//this will be \Device\bacmac1...	

		// get MAC driver names
		nameLength = sizeof(macDriverNames);
		macDriverNames[0] = 0;
		Packet32GetDeviceNames( (char *)macDriverNames, &nameLength, xbind );	//this will be \Device\DC21X41...

		// determine the number of bound adapters from asciiz AdapterNames=name0 name1 ... namex 0
		ap = adapterNames;
		dp = macDriverNames;

		for (int i = 0; i < MAXADAPTERS; i++ ) {
			// check for end of list
			if ((*ap == 0) || (*dp == 0))
				break;

			// copy the unicode names
			wcscpy( gAdapterList[gAdapterListLen].AdapterName, ap );
			ap += wcslen(ap) + 1;
			wcscpy( gAdapterList[gAdapterListLen].MacDriverName, dp );
			dp += wcslen(dp) + 1;

			// open the adapter
			adapter = (LPADAPTER)Packet32OpenAdapter( (char *)gAdapterList[gAdapterListLen].AdapterName );
			if (!adapter)
				return;
			
			// get the local address
			addrLen = Packet32GetAddress( adapter, addrAddr );

			// set the adapter type based on the length of the address
			if (addrLen == 6) {
				gAdapterList[gAdapterListLen].AdapterType = ETH_802_3_ADAPTER;
			} else {
				gAdapterList[gAdapterListLen].AdapterType = ARCNET_ADAPTER;
			}

			// close the adapter
			Packet32CloseAdapter( adapter );

			// count it
			gAdapterListLen += 1;
		}
	}
}

//
//	WinPacket32::WinPacket32
//

WinPacket32::WinPacket32( short port, bool ispromiscuous )
	: m_Thread(0), m_Adapter(0)
{
	InitPort( port, ispromiscuous );
}

//
//	WinPacket32::WinPacket32
//

WinPacket32::WinPacket32( const char *port, bool ispromiscuous )
	: m_Thread(0), m_Adapter(0)
{
	int		i
	;
	char	nameBuff[64], config[64]
	,		*dst
	;

	// make sure the adapter list is intialized
	if (!gAdapterList)
		InitAdapterList();

	// copy the name over
	dst = config;
	while (*port && (*port != ','))
		*dst++ = *port++;
	*dst++ = 0;

	// look for promiscuous mode
	ispromiscuous |= ((*port++ == ',') && (*port == '1'));

	// look for a matching port
	for (i = 0; i < gAdapterListLen; i++) {
		// convert the name
		ConvertUnicodeToAscii( nameBuff, gAdapterList[i].MacDriverName, sizeof(nameBuff) );

		// got a match?
		if (strcmp(config,nameBuff) != 0)
			continue;

		// found a match!
		InitPort( i, ispromiscuous );
	}
}

//
//	WinPacket32::InitPort
//

void WinPacket32::InitPort( short port, bool ispromiscuous )
{
	ULONG			filter
	;

	// init the port 
	m_Port = port;

	// make sure the adapter list is intialized
	if (!gAdapterList)
		InitAdapterList();

	// check for valid port number
	if ((port < 0) || (port >= gAdapterListLen))
		return;

	// open the adapter
	m_Adapter = (LPADAPTER)Packet32OpenAdapter( (char *)gAdapterList[port].AdapterName );
	if (!m_Adapter)
		return;
	
	// set the local address
	portLocalAddr.addrType = localStationAddr;
	portLocalAddr.addrLen = Packet32GetAddress( m_Adapter, portLocalAddr.addrAddr );

	// set the adapter type and maximum frame size based on the length of the address
	if (portLocalAddr.addrLen == 6) {
		m_AdapterType = ETH_802_3_ADAPTER;
		m_MaxFrameSize = 1514;

		// broadcast
		portBroadcastAddr.addrType = localStationAddr;
		portBroadcastAddr.addrLen = 6;
		portBroadcastAddr.addrAddr[0] = 0xFF;
		portBroadcastAddr.addrAddr[1] = 0xFF;
		portBroadcastAddr.addrAddr[2] = 0xFF;
		portBroadcastAddr.addrAddr[3] = 0xFF;
		portBroadcastAddr.addrAddr[4] = 0xFF;
		portBroadcastAddr.addrAddr[5] = 0xFF;
	} else {
		m_AdapterType = ARCNET_ADAPTER;
		m_MaxFrameSize = 514;

		// broadcast
		portBroadcastAddr.addrType = localStationAddr;
		portBroadcastAddr.addrLen = 1;
		portBroadcastAddr.addrAddr[0] = 0;
	}

	// set packet filter
	filter = ((ispromiscuous) && (m_AdapterType == ETH_802_3_ADAPTER))
		? NDIS_PACKET_TYPE_PROMISCUOUS
		: NDIS_PACKET_TYPE_DIRECTED | NDIS_PACKET_TYPE_BROADCAST
		;
//	filter = NDIS_PACKET_TYPE_DIRECTED | NDIS_PACKET_TYPE_BROADCAST;
	Packet32SetFilter( m_Adapter, filter );

	// allocate and lock tx buffer
	m_txPacket = 0;
	m_txhMem = GlobalAlloc( GMEM_MOVEABLE, m_MaxFrameSize );
	m_txlpMem = (unsigned char *)GlobalLock( m_txhMem );

	// now allocate some buffers and start some reads
	for (int j = 0; j < MAXREADREQS; j++ ) {
		// allocate and lock rx buffer
		m_rxhMem[j] = GlobalAlloc( GMEM_MOVEABLE, m_MaxFrameSize );
		m_rxlpMem[j] = (unsigned char *)GlobalLock( m_rxhMem[j] );

		//allocate and create packet
		m_rxPacket[j] = (LPPACKET)Packet32AllocatePacket( m_Adapter, true );
		if (!m_rxPacket[j])
			break;
		
		// initialize the packet
		Packet32InitPacket( m_rxPacket[j], m_rxlpMem[j], m_MaxFrameSize );
		
		// save event handle
		m_rxEvents[j] = m_rxPacket[j]->OverLapped.hEvent;

		// start the read..don't wait
		Packet32ReceivePacket( m_Adapter, m_rxPacket[j], false, &m_rxLen[j] );
	}

	// transmit not in progress
	m_txEvent = NULL;

	// allow our thread to run once it gets started
	m_Continue = true;

	// start it suspended
	m_Thread =
		AfxBeginThread( WinPacket32ThreadFunc, this
			, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED
			);

	// dont let windows delete the thread object
	m_Thread->m_bAutoDelete = FALSE;

	// now let it run
	m_Thread->ResumeThread();

	// clear the port status, all is well
	portStatus = 0;
}

//
//	WinPacket32::~WinPacket32
//

WinPacket32::~WinPacket32()
{
	int		loopCount
	;
	DWORD	dwExitCode
	;

	if (m_Thread) {
		// see if our thread is still around
		::GetExitCodeThread( m_Thread->m_hThread, &dwExitCode );
		if (dwExitCode == STILL_ACTIVE) {
			// the thread is still running
			m_Continue = false;
			
			// wait for it to go away
			for (loopCount = 0; loopCount < 15; loopCount++) {
				if (::WaitForSingleObject( m_Thread->m_hThread, 1000 ) == WAIT_OBJECT_0)
					break;
	#if NT_Packet32Debug
				TRACE0( "(thread still running...)\n" );
	#endif
				::Sleep( 1000 );
			}
			if (loopCount == 15) {
				AfxMessageBox( "WARNING: WinPacket32ThreadFunc still running" );
				::TerminateThread( m_Thread->m_hThread, 0 );
			}
		}

		// the thread has terminated, delete the CWinThread object
		delete m_Thread;
	}
	
	if (m_Adapter) {
		// unlock and deallocate rx buffers
		for (int j = 0; j < MAXREADREQS; j++ ) {
			Packet32FreePacket( m_rxPacket[j] );
			GlobalUnlock( m_rxhMem[j] );
			GlobalFree( m_rxhMem[j] );
		}

		// we have a transmit in progress (may actually already be completed)
		if (m_txPacket) {
			// wait for previous attempt to complete
			int ret = Packet32WaitPacket( m_Adapter, m_txPacket, &m_txLen, true );
	#if NT_Packet32Debug
			TRACE0( "(send complete)\n" );
	#endif
			Packet32FreePacket( m_txPacket );
		}

		// unlock and deallocate tx buffer
		GlobalUnlock( m_txhMem );
		GlobalFree( m_txhMem );

		// close the adapter
		Packet32CloseAdapter( m_Adapter );
	}
}

//
//	WinPacket32::Indication
//
//	This function is called when the client wants to send some data out the
//	port.  The address must be a localStation or a localBroadcast.  It encodes 
//	the packet according to the port type, then calls SendData() to send it.
//

void WinPacket32::Indication( const BACnetNPDU &pdu )
{
	static BACnetOctet	enetBroadcast[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
	;
	int				n
	;
	BACnetOctet		*bp
	;

	// we have a transmit in progress (may actually already be completed)
	if (m_txPacket) {
		// wait for previous attempt to complete
		int ret = Packet32WaitPacket( m_Adapter, m_txPacket, &m_txLen, true );
#if NT_Packet32Debug
		TRACE0( "(send complete)\n" );
#endif
		Packet32FreePacket( m_txPacket );
	}

	// build the packet to send
	bp = m_txlpMem;
	memset( bp, 0xCD, m_MaxFrameSize );					// ### debug
	if (m_AdapterType == ETH_802_3_ADAPTER) {
		n = ETH_802_3_ADDRESS_LENGTH;
		if (pdu.pduAddr.addrType == localBroadcastAddr)
			memcpy( bp, enetBroadcast, n );				// broadcast addr
		else
			memcpy( bp, pdu.pduAddr.addrAddr, n );		// dest addr
		bp += n;
		memcpy( bp, portLocalAddr.addrAddr, n );		// source addr
		bp += n;

		n = pdu.pduLen + 3;								// must account for LLC
		*bp++ = (n >> 8) & 0xFF;						// MS len
		*bp++ = (n & 0xFF);								// LS len
		*bp++ = BACnetLSAP;								// DSAP
		*bp++ = BACnetLSAP;								// SSAP
		*bp++ = 0x03;									// LLC Control = UI

		memcpy( bp, pdu.pduData, pdu.pduLen );			// PDU
		bp += pdu.pduLen;

		// see if we're smaller than ethernet minimum
		n = 60 - (bp - m_txlpMem);
		if (n > 0) {									// we must pad
			memset( bp, 0, n );
			bp += n;
		}
	} else
	if (m_AdapterType == ARCNET_ADAPTER) {
		*bp++ = portLocalAddr.addrAddr[0];				// source address
		if (pdu.pduAddr.addrType == localBroadcastAddr)
			*bp++ = 0;									// broadcast address (0=ARCnet Broadcast)
		else
			*bp++ = pdu.pduAddr.addrAddr[0];			// dest addr
		*bp++ = 0xCD;									// BACnet SC
		*bp++ = BACnetLSAP;								// DSAP
		*bp++ = BACnetLSAP;								// SSAP
		*bp++ = 0x03;									// LLC Control = UI

		memcpy( bp, pdu.pduData, pdu.pduLen );			// PDU
		bp += pdu.pduLen;
	} else
		return;

	// send it
	SendData( m_txlpMem, bp - m_txlpMem );
}

//
//	WinPacket32::SendData
//
//	This function is called with the data to send out the port.
//

void WinPacket32::SendData( BACnetOctet *data, int len )
{
	// let the application filter it
	FilterData( data, len, portSending );

	// allocate a packet
	LPPACKET newPacket = (LPPACKET)Packet32AllocatePacket( m_Adapter, false );
	if (!newPacket)
		return;

	// init pointer and size
	Packet32InitPacket( newPacket, data, len );

	// send it, do not wait for completion
	m_txPacket = newPacket;
#if NT_Packet32Debug
	TRACE0( "(send starting)\n" );
#endif
	Packet32SendPacket( m_Adapter, m_txPacket, false );
}

//
//	WinPacket32ThreadFunc
//

#define MAX_MSGLEN	65536

UINT WinPacket32ThreadFunc( LPVOID pParam )
{
	int				err, eventIndx, len
	;
	BACnetAddress	srcAddr
	;
	BACnetOctet		*bp
	;
	DWORD			dw
	;
	WinPacket32Ptr	pServer = (WinPacket32Ptr)pParam
	;

#if NT_Packet32Debug
	TRACE0( "(WinPacket32ThreadFunc enter)\n" );
#endif

	while (pServer->m_Continue) {
		// block until one of our ports gets something (but no longer than ListenTime)
		dw = ::WaitForMultipleObjects( MAXREADREQS, pServer->m_rxEvents, false, ListenTime );
#if NT_Packet32Debug
		TRACE0( "(tick)\n" );
#endif

		// see if there was some reason we couldn't do the wait
		if (dw == WAIT_FAILED)
			break;
		
		// see if something arrived
		if ((dw < WAIT_OBJECT_0) || (dw >= (WAIT_OBJECT_0 + MAXREADREQS)))
			continue;
		
		// start with the index that was signaled
		eventIndx = dw - WAIT_OBJECT_0;
#if NT_Packet32Debug
		TRACE1( "(eventIndx %d)\n", eventIndx );
#endif

		// loop through them all
		for (int i = 0; i < MAXREADREQS; i++, eventIndx = (eventIndx + 1) % MAXREADREQS ) {
			// skip it if the event hasn't signaled
			if (pServer->m_rxPacket[eventIndx] == NULL)
				continue;

			// grab ourselves a packet
			err = Packet32WaitPacket( pServer->m_Adapter
				, pServer->m_rxPacket[eventIndx]
				, &pServer->m_rxLen[eventIndx], false
				);
			if (err == ERROR_IO_INCOMPLETE)
				continue;

#if NT_Packet32Debug
			TRACE0( "(got a packet)\n" );
#endif

			// reset the event signal manually
			ResetEvent( pServer->m_rxEvents[eventIndx] );				

			// point to the packet
			bp = pServer->m_rxlpMem[eventIndx];
			len = pServer->m_rxLen[eventIndx];

			// let the application filter it
			pServer->FilterData( bp, len, BACnetPort::portReceiving );

			if (pServer->m_AdapterType == ETH_802_3_ADAPTER) {
				srcAddr.LocalStation( bp + 6, 6 );

				// extract the length from the packet contents
				len = (bp[12] << 8) + bp[13];

				bp += 17;		// skip over dest,src,...
				len -= 3;		// do not pass LSAP
			} else
			if (pServer->m_AdapterType == ARCNET_ADAPTER) {
				srcAddr.LocalStation( bp, 1 );
				bp += 6;
				len -= 6;
			} else
				;

			// send it upstream if it is not from us
			if (!(srcAddr == pServer->portLocalAddr))
				pServer->Response( BACnetNPDU( srcAddr, bp, len ) );

			// free the old packet (but not the memory)
			Packet32FreePacket( pServer->m_rxPacket[eventIndx] );

			// allocate a new one
			pServer->m_rxPacket[eventIndx] =
				(LPPACKET)Packet32AllocatePacket( pServer->m_Adapter
					, true
					);
			if (pServer->m_rxPacket[eventIndx] == NULL)
				continue;

			// initialize the new packet
			Packet32InitPacket( pServer->m_rxPacket[eventIndx]
				, pServer->m_rxlpMem[eventIndx]
				, pServer->m_MaxFrameSize
				);
		
			// save event handle
			pServer->m_rxEvents[eventIndx] = pServer->m_rxPacket[eventIndx]->OverLapped.hEvent;

			// start the read, don't wait
			Packet32ReceivePacket( pServer->m_Adapter
				, pServer->m_rxPacket[eventIndx], false
				, &pServer->m_rxLen[eventIndx]
				);
		}
	}

#if NT_Packet32Debug
	TRACE0( "(WinPacket32ThreadFunc exit)\n" );
#endif

	// finished
    return 0;
}

//
//	ConvertUnicodeToAscii
//

short ConvertUnicodeToAscii( char *ap, wchar_t *up, short max )
{
	short	n = 0
	;
	
	while ((*up != 0) && (n < max-1)) {
		*ap++=(char)(*up++);
		n++;
	}
	*ap=0;
	return n;
}
