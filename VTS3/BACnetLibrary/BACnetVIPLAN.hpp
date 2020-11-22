#ifndef _BACnetVIPLAN
#define _BACnetVIPLAN

#include "BACnet.hpp"
#include "BACnetIP.hpp"

//
//	BACnetVIPLAN
//
//	A BACnet Virtual LAN consists of two different objects, one that represents the 
//	LAN and one for nodes on the LAN.  The BACnetVIPLAN object is a factory that creates 
//	VLAN nodes, so it can use whatever mechanism it wants to assign addresses and keep 
//	them LAN specific.
//

class BACnetVIPLAN;
typedef BACnetVIPLAN *BACnetVIPLANPtr;

class BACnetVIPLANNode;
typedef BACnetVIPLANNode *BACnetVIPLANNodePtr;

//
//	BACnetVIPLANMsg
//

class BACnetVIPLANMsg : public BACnetTask {
		friend class BACnetVIPLAN;
		
	protected:
		BACnetVIPLANPtr		msgLAN;
		BACnetVIPLANNodePtr	msgSource;
		BACnetAddress		msgDestination;
		BACnetOctet			*msgData;
		int					msgLen;
	
	public:
		BACnetVIPLANMsg( BACnetVIPLANPtr lan, BACnetVIPLANNodePtr source, const BACnetNPDU &pdu );
		~BACnetVIPLANMsg( void );
		
		void ProcessTask( void );
	};

typedef BACnetVIPLANMsg *BACnetVIPLANMsgPtr;

//
//	BACnetVIPLANNode
//

class BACnetVIPLANNode : public BACnetNetServer {
		friend class BACnetVIPLAN;
		
	public:
		BACnetAddress	ipAddress;
		
	protected:
		BACnetVIPLANPtr	nodeLAN;
		unsigned long	nodeIPAddr;
		unsigned long	nodeSubnet;
		unsigned long	nodeSubnetMask;
		unsigned short	nodePort;
		
		BACnetVIPLANNode( BACnetVIPLANPtr lan, unsigned long ipAddr, unsigned long subnetMask, unsigned short port )
			: nodeLAN(lan), nodeIPAddr(ipAddr), nodeSubnetMask(subnetMask), nodePort(port)
		{
			nodeSubnet = (nodeIPAddr & nodeSubnetMask);
			
			ipAddress.addrType = localStationAddr;
			ipAddress.addrLen = 6;
			memcpy( ipAddress.addrAddr, &nodeIPAddr, 4 );
			memcpy( ipAddress.addrAddr + 4, &nodePort, 2 );
		}
		virtual ~BACnetVIPLANNode( void ) {}

		virtual void Indication( const BACnetNPDU &pdu );
	};

//
//	BACnetVIPLAN
//

static const int	kBACnetVIPLANNodeListSize = 32;

class BACnetVIPLAN {
		friend class BACnetVIPLANMsg;
		
	protected:
		int					vlanID;
		BACnetVIPLANNodePtr	nodeList[kBACnetVIPLANNodeListSize];
		int					nodeListSize;
		
		void ProcessMessage( BACnetVIPLANMsgPtr msg );
		
	public:
		BACnetVIPLAN( int id );
		
		BACnetVIPLANNodePtr	NewNode( unsigned long ipAddr, unsigned long subnetMask, unsigned short port );
	};

#endif
