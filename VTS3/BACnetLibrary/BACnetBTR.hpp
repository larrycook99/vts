#ifndef _BACnetBTR
#define _BACnetBTR

#include "BACnet.hpp"
#include "BACnetIP.hpp"

//
//	BACnetBTR
//
//	This object is a client of an IP endpoint and a server to a BACnetRouter.
//	It provides a "shim" layer for providing BTR functionality.
//

class BACnetBTR : public BACnetNetClient, public BACnetNetServer {
	protected:
		BACnetIPAddr	*btrPeer;
		int				btrPeerLen;
		
	public:
		BACnetBTR( void );
		virtual ~BACnetBTR( void );
		
		virtual void Indication( const BACnetNPDU &ndpu );
		virtual void Confirmation( const BACnetNPDU &npdu );
		
		void AddPeer( const BACnetIPAddr &ipAddr );
		void DeletePeer( const BACnetIPAddr &ipAddr );

		void AddPeerNet( const BACnetIPAddr &ipAddr, int net );
	};

typedef BACnetBTR *BACnetBTRPtr;

#endif
