
#ifndef _BACnetIP
#define _BACnetIP

#include "BACnet.hpp"

//
//	BVLC Function
//

const BACnetOctet	kBVLCType	= 0x81;

enum BVLCFunction
		{ bvlcResult							= 0x00
		, blvcWriteBroadcastDistributionTable	= 0x01
		, blvcReadBroadcastDistributionTable	= 0x02
		, blvcReadBroadcastDistributionTableAck	= 0x03
		, blvcForwardedNPDU						= 0x04
		, bvlcRegisterForeignDevice				= 0x05
		, bvlcReadForeignDeviceTable			= 0x06
		, bvlcReadForeignDeviceTableAck			= 0x07
		, bvlcDeleteForeignDeviceTableEntry		= 0x08
		, bvlcDistributeBroadcastToNetwork		= 0x09
		, bvlcOriginalUnicastNPDU				= 0x0A
		, bvlcOriginalBroadcastNPDU				= 0x0B
		};

//
//	BACnetIPAddr
//

const unsigned short kBACnetIPDefaultPort = 0xBAC0;

class BACnetIPAddr : public BACnetAddress {
	public:
		BACnetIPAddr( void );
		BACnetIPAddr( const char *spec );
		BACnetIPAddr( const BACnetAddress &addr );
		BACnetIPAddr( unsigned long host, unsigned short port = kBACnetIPDefaultPort );
		
		void Pack( unsigned long host, unsigned short port );
		void Unpack( unsigned long *hostp, unsigned short *portp ) const;
		
		static void Pack( BACnetOctet *rsltp, unsigned long host, unsigned short port );
		static void Unpack( const BACnetOctet *rsltp, unsigned long *hostp, unsigned short *portp );
		
		static void StringToHostPort( const char *str, unsigned long *hostp, unsigned long *maskp, unsigned short *portp );
		
		char* AddressToString( void ) const;
		static char * AddressToString( const unsigned char * paddr );
		static char * AddressToString( const BACnetAddress * paddr );
	};

typedef BACnetIPAddr *BACnetIPAddrPtr;

#endif
