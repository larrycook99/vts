
#include "stdafx.h"

#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")

#include "WinWinPcap.hpp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	BACnetLSAP			0x82

UINT WinWinPcapThreadFunc( LPVOID pParam );

//
//	WinWinPcap adapter list
//

#define MAXADAPTERS			16

WinWinPcapAdapterDescPtr	gAdapterList = 0;
int							gAdapterListLen = 0;

//
//	InitAdapterList
//

static void GetMACaddress(void);

void InitAdapterList( void )
{
	pcap_if_t	*alldevs
	;
	char		errbuf[PCAP_ERRBUF_SIZE+1]
	;

	// see if the adapter list needs to be initialized
	if (!gAdapterList) {
		// allocate the list
		gAdapterList = new WinWinPcapAdapterDesc[ MAXADAPTERS ];
		gAdapterListLen = 0;

		/* Retrieve the interfaces list */
//		if (pcap_findalldevs_ex(NULL, NULL, 0, NULL, &alldevs, errbuf) == -1) 
		if (pcap_findalldevs(&alldevs, errbuf) == -1)		// changed for WinPCap 4.0.1
		{
			AfxMessageBox( errbuf );
			exit(1);
		}

		// scan through the list
		for(pcap_if_t *d=alldevs; d; d=d->next ) {
			// copy the name and description
			strcpy( gAdapterList[gAdapterListLen].AdapterName, d->name );
			strcpy( gAdapterList[gAdapterListLen].AdapterDescription, d->description );

			// assume all Ethernet adapters for now
			gAdapterList[gAdapterListLen].AdapterType = ethernetAdapter;

			gAdapterListLen += 1;
		}

		// done with the device list
		pcap_freealldevs(alldevs);

		GetMACaddress();
	}
}

//
//	GetMACaddress
//

static void GetMACaddress(void)
{
	char				name[256]
	;
	IP_ADAPTER_INFO		AdapterInfo[16]					// Allocate information for up to 16 NICs
	;
	DWORD				dwBufLen = sizeof(AdapterInfo)	// Save the memory size of buffer
	,					dwStatus
	;

	dwStatus = GetAdaptersInfo( AdapterInfo, &dwBufLen );
	if (dwStatus != ERROR_SUCCESS) {
		char errmsg[64];

		sprintf( errmsg, "GetAdaptersInfo() error: %d", dwStatus );
		AfxMessageBox( errmsg );
		return;
	}

	// go through the adapters
	for (PIP_ADAPTER_INFO pai = AdapterInfo; pai; pai = pai->Next) {
		// build a name that looks like the WinPcap names
		sprintf( name, "\\Device\\NPF_%s", pai->AdapterName );

		// look for a matching name
		for (int i = 0; i < gAdapterListLen; i++)
			if (strcmp(name,gAdapterList[i].AdapterName) == 0) {
				// save the address
				gAdapterList[i].AdapterLocalAddress.LocalStation( pai->Address, 6 );
				break;
			}
	}
}

//
//	WinWinPcap::WinWinPcap
//

WinWinPcap::WinWinPcap( short port, bool ispromiscuous )
	: m_Thread(0)
{
	InitPort( port, ispromiscuous );
}

//
//	WinWinPcap::WinWinPcap
//

WinWinPcap::WinWinPcap( const char *port, bool ispromiscuous )
	: m_Thread(0)
{
	int		i
	;
	char	config[256]
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
		// got a match?
		if (strcmp(config,gAdapterList[i].AdapterDescription) != 0)
			continue;

		// found a match!
		InitPort( i, ispromiscuous );
	}
}

//
//	WinWinPcap::InitPort
//

void WinWinPcap::InitPort( short port, bool ispromiscuous )
{
	char		errbuf[PCAP_ERRBUF_SIZE+1]
	;

	// init the port 
	m_Port = port;

	// make sure the adapter list is intialized
	if (!gAdapterList)
		InitAdapterList();

	// check for valid port number
	if ((port < 0) || (port >= gAdapterListLen))
		return;

	// no thread yet, until the interface is open
	m_Thread = 0;

	// open the capture interface
//	m_Interface = pcap_open( gAdapterList[port].AdapterName
//		, 1500														// snaplen
//		, (ispromiscuous ? PCAP_OPENFLAG_PROMISCUOUS : 0)			// flags
//		, 500														// read timeout
//		, NULL														// authentication
//		, errbuf
//		);
	// Changed for WinPCap 4.0.1
	m_Interface = pcap_open_live(gAdapterList[port].AdapterName, 1500, 1, 500, errbuf);
	if (!m_Interface) {
		AfxMessageBox( errbuf );
		return;
	}

	// verify that it's Ethernet
	if (pcap_datalink(m_Interface) != DLT_EN10MB) {
		AfxMessageBox( "Not an Ethernet adapter." );
		return;
	}

    // compile the filter, 802.3 packets, BACnet LSAP
    if (pcap_compile(m_Interface, &m_Filter, "(ether[12:2] < 0x0600) and (ether[14] == 0x82) and (ether[15] == 0x82) and (ether[16] == 0x03)", 1, 0) < 0) {
        AfxMessageBox("Unable to compile the packet filter.");
		return;
	}

    // set the filter
	if (pcap_setfilter(m_Interface, &m_Filter) < 0) {
		AfxMessageBox("Unable to set the packet filter.");
		return;
	}

	// set the local address
	portLocalAddr = gAdapterList[port].AdapterLocalAddress;

	// set the adapter type and maximum frame size based on the length of the address
	m_AdapterType = ethernetAdapter;
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

	// allow our thread to run once it gets started
	m_Continue = true;

	// start it suspended
	m_Thread =
		AfxBeginThread( WinWinPcapThreadFunc, this
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
//	WinWinPcap::~WinWinPcap
//

WinWinPcap::~WinWinPcap()
{
	if (m_Thread) {
		DWORD	dwExitCode
		;

		// see if our thread is still around
		::GetExitCodeThread( m_Thread->m_hThread, &dwExitCode );
		if (dwExitCode == STILL_ACTIVE) {
			int		loopCount
			;

			// the thread is still running
			m_Continue = false;
			
			// wait for it to go away
			for (loopCount = 0; loopCount < 15; loopCount++) {
				if (::WaitForSingleObject( m_Thread->m_hThread, 1000 ) == WAIT_OBJECT_0)
					break;
				::Sleep( 1000 );
			}
			if (loopCount == 15) {
				AfxMessageBox( "WARNING: WinWinPcapThreadFunc still running" );
				::TerminateThread( m_Thread->m_hThread, 0 );
			}
		}

		// the thread has terminated, delete the CWinThread object
		delete m_Thread;
	}
	
	if (m_Interface)
		pcap_close(m_Interface);
}

//
//	WinWinPcap::Indication
//
//	This function is called when the client wants to send some data out the
//	port.  The address must be a localStation or a localBroadcast.  It encodes 
//	the packet then calls SendData() to send it.
//

void WinWinPcap::Indication( const BACnetNPDU &pdu )
{
	static BACnetOctet	enetBroadcast[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
	;
	int				n
	;
	BACnetOctet		packetBuffer[1514]
	,				*bp
	;

	// build the packet to send
	bp = packetBuffer;
	memset( bp, 0xCD, m_MaxFrameSize );					// ### debug

	if (pdu.pduAddr.addrType == localBroadcastAddr)
		memcpy( bp, enetBroadcast, 6 );				// broadcast addr
	else
		memcpy( bp, pdu.pduAddr.addrAddr, 6 );		// dest addr
	bp += 6;
	memcpy( bp, portLocalAddr.addrAddr, 6 );		// source addr
	bp += 6;

	n = pdu.pduLen + 3;								// must account for LLC
	*bp++ = (n >> 8) & 0xFF;						// MS len
	*bp++ = (n & 0xFF);								// LS len
	*bp++ = BACnetLSAP;								// DSAP
	*bp++ = BACnetLSAP;								// SSAP
	*bp++ = 0x03;									// LLC Control = UI

	memcpy( bp, pdu.pduData, pdu.pduLen );			// PDU
	bp += pdu.pduLen;

	// see if we're smaller than the Ethernet minimum
	n = 60 - (bp - packetBuffer);
	if (n > 0) {									// we must pad
		memset( bp, 0, n );
		bp += n;
	}

	// send it
	SendData( packetBuffer, bp - packetBuffer );
}

//
//	WinWinPcap::SendData
//
//	This function is called with the data to send out the port.
//

void WinWinPcap::SendData( BACnetOctet *data, int len )
{
	// let the application filter it
	FilterData( data, len, portSending );

	// send it
	pcap_sendpacket( m_Interface, data, len );
}

//
//	WinWinPcapThreadFunc
//

UINT WinWinPcapThreadFunc( LPVOID pParam )
{
	int					res, len
	;
	struct pcap_pkthdr	*header
	;
	u_char				*pkt_data
	;
	WinWinPcapPtr		pServer = (WinWinPcapPtr)pParam
	;
	BACnetAddress	srcAddr
	;

	// read packets
	while (pServer->m_Continue) {
		res = pcap_next_ex( pServer->m_Interface, &header, (const unsigned char **)&pkt_data);

		// check for problems
		if (res < 0) break;

		// no packet, try again
		if (res == 0) continue;

		// extract the length from the packet contents
		len = (pkt_data[12] << 8) + pkt_data[13];

		// let the application filter just the data portion
		pServer->FilterData( pkt_data, len + 14, BACnetPort::portReceiving );

		// extract the source address
		srcAddr.LocalStation( pkt_data + 6, 6 );

		pkt_data += 17;		// skip over dest,src,len,lsap
		len -= 3;			// do not pass LSAP

		// send it upstream if it is not from us
		if (!(srcAddr == pServer->portLocalAddr))
			pServer->Response( BACnetNPDU( srcAddr, pkt_data, len ) );
	}

	// finished
    return 0;
}
