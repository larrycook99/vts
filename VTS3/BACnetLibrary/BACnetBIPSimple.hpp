#ifndef _BACnetBIPSimple
#define _BACnetBIPSimple

#include "BACnet.hpp"
#include "BACnetIP.hpp"

//
//	BACnetBIPSimple
//

class BACnetBIPSimple : public BACnetNetClient, public BACnetNetServer {
	public:
		virtual void Indication( const BACnetNPDU &ndpu );
		virtual void Confirmation( const BACnetNPDU &npdu );
	};

typedef BACnetBIPSimple *BACnetBIPSimplePtr;

#endif
