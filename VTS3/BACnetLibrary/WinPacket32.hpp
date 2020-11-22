#ifndef _WinPacket32
#define _WinPacket32

#include "BACnet.hpp"

extern "C" {
#include "Packet32.h"
}

//
//	WinPacket32AdapterDesc
//

#define	MAXADAPTERS			2			// maximum number of available adapters
#define	MAXREADREQS			16			// read requests per adapter/thread

#define	ListenTime			500L		// listener thread timeout (ms)

struct WinPacket32AdapterDesc {
	int			AdapterType;			// ETH_802_3_ADAPTER or ARCNET_ADAPTER
	wchar_t		AdapterName[64];		// the UNICODE NT adapter name	\Device\bacpac1
	wchar_t		MacDriverName[64];		// the UNICODE NT MAC name		\Device\DC21X41
	};

typedef WinPacket32AdapterDesc *WinPacket32AdapterDescPtr;

extern WinPacket32AdapterDescPtr	gAdapterList;
extern int							gAdapterListLen;

void InitAdapterList( void );

//
//	WinPacket32
//

class WinPacket32 : public BACnetPort {
		friend UINT WinPacket32ThreadFunc( LPVOID pParam );

	protected:
		short					m_Port;						// index into adapter list
		LPADAPTER				m_Adapter;					// adapter
		int						m_MaxFrameSize;				// maximum frame size

		HANDLE					m_rxEvents[ MAXREADREQS ];	// events signaled when IO completes
		LPPACKET				m_rxPacket[ MAXREADREQS ];	// list of packets
		HGLOBAL					m_rxhMem[ MAXREADREQS ];	// global handle for RX data
		LPBYTE					m_rxlpMem[ MAXREADREQS ];	// pointer to the memory block
		unsigned long			m_rxLen[ MAXREADREQS ];		// received length

		HANDLE					m_txEvent;					// events signaled when IO completes
		LPPACKET				m_txPacket;					// packet
		HGLOBAL					m_txhMem;					// global handle for TX data
		LPBYTE					m_txlpMem;					// pointer to the memory block
		unsigned long			m_txLen;					// transmit length

		CWinThread				*m_Thread;
		bool					m_Continue;

	public:
		int						m_AdapterType;				// ETH_802_3_ADAPTER or ARCNET_ADAPTER
		
		WinPacket32( short port, bool ispromiscuous = false );
		WinPacket32( const char *port, bool ispromiscuous = false );
		~WinPacket32( void );
		
		void InitPort( short port, bool ispromiscuous );

		void Indication( const BACnetNPDU &pdu );			// client request
		void SendData( BACnetOctet *data, int len );		// raw data request
	};

typedef WinPacket32 *WinPacket32Ptr;

#endif
