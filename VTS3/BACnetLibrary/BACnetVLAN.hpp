#ifndef _BACnetVLAN
#define _BACnetVLAN

#include "BACnet.hpp"

//
//	BACnetVLAN
//
//	A BACnet Virtual LAN consists of two different objects, one that represents the 
//	LAN and one for nodes on the LAN.  The BACnetVLAN object is a factory that creates 
//	VLAN nodes, so it can use whatever mechanism it wants to assign addresses and keep 
//	them LAN specific.
//

class BACnetVLAN;
typedef BACnetVLAN *BACnetVLANPtr;

class BACnetVLANNode;
typedef BACnetVLANNode *BACnetVLANNodePtr;

//
//	BACnetVLANMsg
//

class BACnetVLANMsg : public BACnetTask {
		friend class BACnetVLAN;
		
	protected:
		BACnetVLANPtr		msgLAN;
		BACnetAddress		msgSource;
		BACnetAddress		msgDestination;
		BACnetOctet			*msgData;
		int					msgLen;
	
	public:
		BACnetVLANMsg( BACnetVLANPtr lan, const BACnetAddress &source, const BACnetNPDU &pdu );
		~BACnetVLANMsg( void );
		
		void ProcessTask( void );
	};

typedef BACnetVLANMsg *BACnetVLANMsgPtr;

//
//	BACnetVLAN
//

const int	kBACnetVLANNodeListSize = 8;
		
class BACnetVLAN {
		friend class BACnetVLANMsg;
		
	protected:
		int					vlanID;
		BACnetVLANNodePtr	nodeList[kBACnetVLANNodeListSize];
		int					nodeListSize;
		
		void ProcessMessage( BACnetVLANMsgPtr msg );
		
	public:
		BACnetVLAN( void );
		
		void NewNode( BACnetVLANNodePtr np );
		void DeleteNode( BACnetVLANNodePtr np );
	};

//
//	BACnetVLANNode
//

class BACnetVLANNode : public BACnetPort {
		friend class BACnetVLAN;
		
	protected:
		BACnetVLANPtr	nodeLAN;
		
		virtual void Indication( const BACnetNPDU &pdu );
		virtual void SendData( BACnetOctet *, int len );
		
	public:
		BACnetVLANNode( BACnetVLANPtr lan )
			: nodeLAN(lan)
		{
			nodeLAN->NewNode( this );
		}
		
		virtual ~BACnetVLANNode( void )
		{
			nodeLAN->DeleteNode( this  );
		}
	};

#endif
