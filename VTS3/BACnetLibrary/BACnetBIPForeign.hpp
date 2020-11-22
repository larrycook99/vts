#ifndef _BACnetBIPForeign
#define _BACnetBIPForeign

#include "BACnet.hpp"
#include "BACnetIP.hpp"

//
//	BACnetBIPForeign
//

class BACnetBIPForeign : public BACnetNetClient, public BACnetNetServer, public BACnetTask {
	public:
		int				foreignStatus;		// 0=dead, 1=alive
		int				foreignTimeToLive;	// time to live interval in seconds
		BACnetIPAddr	foreignBBMDAddr;	// address of BBMD to register with
		
		BACnetBIPForeign( void );
		virtual ~BACnetBIPForeign( void );
		
		void Register( unsigned long ipAddr, unsigned short port, int ttl );
		
		virtual void Indication( const BACnetNPDU &ndpu );
		virtual void Confirmation( const BACnetNPDU &npdu );
		
		virtual void ProcessTask( void );
	};

typedef BACnetBIPForeign *BACnetBIPForeignPtr;

#endif
