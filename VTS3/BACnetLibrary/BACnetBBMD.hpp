#ifndef _BACnetBBMD
#define _BACnetBBMD

#include "BACnet.hpp"
#include "BACnetIP.hpp"

//
//	BACnet Broadcast Distribution Table Entry
//

struct BACnetBDTEntry {
	BACnetIPAddr	bdtAddress;
	unsigned long	bdtIPAddr;
	unsigned short	bdtPort;
	unsigned long	bdtMask;
	};

//
//	BACnet Broadcast Distribution Table Entry
//

struct BACnetFDTEntry {
	BACnetIPAddr	fdAddress;
	unsigned long	fdIPAddr;
	unsigned short	fdPort;
	unsigned short	fdTTL;
	unsigned short	fdRemain;
	};

//
//	BACnetBBMD
//

const int		kBACnetBBMDMaxBDTSize = 50;
const int		kBACnetBBMDMaxFDTSize = 50;
const int		kBACnetBBMDForeignTTLExt = 5;

class BACnetBBMD : public BACnetNetClient, public BACnetNetServer, BACnetTask {
	public:
		BACnetBBMD( const BACnetAddress &addr );
		virtual ~BACnetBBMD( void );
		
		void AddForeignDevice( char *spec, int ttl );
		void DeleteForeignDevice( char *spec );
		
		void AddPeer( char *spec );
		void DeletePeer( char *spec );
		
	protected:
		BACnetAddress	bbmdAddress;
		int				bbmdLocalIndex;
		
		BACnetBDTEntry	bbmdBDT[kBACnetBBMDMaxBDTSize];
		int				bbmdBDTSize;
		BACnetFDTEntry	bbmdFDT[kBACnetBBMDMaxFDTSize];
		int				bbmdFDTSize;
		
		int RegisterForeignDevice( const BACnetIPAddr &addr, int ttl );
		
		virtual void Indication( const BACnetNPDU &ndpu );
		virtual void Confirmation( const BACnetNPDU &npdu );
		
		virtual void ProcessTask( void );
	};

typedef BACnetBBMD *BACnetBBMDPtr;

#endif
