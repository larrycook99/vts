#ifndef _WinWinPcap
#define _WinWinPcap

#include "BACnet.hpp"

#define _WINSOCK2API_
#define _WS2TCPIP_H_

#define REMOTE	1
#define	WPCAP	1
#include "pcap.h"

/*
 * The number of bytes in an ethernet (MAC) address.
 */
#define ETHER_ADDR_LEN			6

/*
 * The number of bytes in the type field.
 */
#define ETHER_TYPE_LEN			2

/*
 * The number of bytes in the trailing CRC field.
 */
#define ETHER_CRC_LEN			4

/*
 * The length of the combined header.
 * Note: Due to padding, sizeof(ether_header) != ETHER_HDR_LEN.
 */
#define ETHER_HDR_LEN			(ETHER_ADDR_LEN * 2 + ETHER_TYPE_LEN)

/* 
 * Padding in struct in order to make it 32 bit aligned. 
 */
#define ETHER_PAD_LEN			(ETHER_HDR_LEN % sizeof(uint32_t))

/*
 * The minimum packet length.
 */
#define ETHER_MIN_LEN			64

/*
 * The maximum packet length.
 */
#define ETHER_MAX_LEN			1518

/*
 * A macro to validate a length with
 */
#define ETHER_IS_VALID_LEN(foo) \
		((foo) >= ETHER_MIN_LEN && (foo) <= ETHER_MAX_LEN)

#define ETHER_ARE_ADDR_EQUAL(a1, a2) (memcmp(a1, a2, ETHER_ADDR_LEN) == 0)

#define ETHER_IS_ADDR_MULTICAST(addr) ((addr)[0] & 0x1)

#define ETHER_IS_ADDR_BROADCAST(addr)                   \
     ((addr)[0] == 0xff && (addr)[1] == 0xff &&         \
      (addr)[2] == 0xff && (addr)[3] == 0xff &&         \
      (addr)[4] == 0xff && (addr)[5] == 0xff)

//
//	WinWinPcapAdapterDesc
//

enum WinWinPcapAdapterType { ethernetAdapter, arcnetAdapter };

struct WinWinPcapAdapterDesc {
	WinWinPcapAdapterType	AdapterType;				// Ethernet or ARCNET adapter
	char					AdapterName[256];			// the interface name
	char					AdapterDescription[256];	// the interface description
	BACnetAddress			AdapterLocalAddress;		// Ethernet address
	};

typedef WinWinPcapAdapterDesc *WinWinPcapAdapterDescPtr;

extern WinWinPcapAdapterDescPtr	gAdapterList;
extern int						gAdapterListLen;

void InitAdapterList( void );

//
//	WinWinPcap
//

class WinWinPcap : public BACnetPort {
		friend UINT WinWinPcapThreadFunc( LPVOID pParam );

	protected:
		short					m_Port;						// index into adapter list

		int						m_MaxFrameSize;

		CWinThread				*m_Thread;					// thread for reading packets
		pcap_t					*m_Interface;				// pointer to a caputure interface
		struct bpf_program		m_Filter;					// BACnet/Ethernet filter

		bool					m_Continue;

	public:
		WinWinPcapAdapterType	m_AdapterType;				// Ethernet or ARCNET adapter
		
		WinWinPcap( short port, bool ispromiscuous = false );
		WinWinPcap( const char *port, bool ispromiscuous = false );
		~WinWinPcap( void );
		
		void InitPort( short port, bool ispromiscuous );

		void Indication( const BACnetNPDU &pdu );			// client request
		void SendData( BACnetOctet *data, int len );		// raw data request
	};

typedef WinWinPcap *WinWinPcapPtr;

#endif
