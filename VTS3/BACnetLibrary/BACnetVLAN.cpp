
#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "BACnet.hpp"
#include "BACnetVLAN.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetVLANMsg::BACnetVLANMsg
//

BACnetVLANMsg::BACnetVLANMsg( BACnetVLANPtr lan, const BACnetAddress &source, const BACnetNPDU &pdu )
	: BACnetTask( oneShotDeleteTask, 0 )
	, msgLAN(lan), msgSource(source), msgDestination(pdu.pduAddr)
{
	// copy the message data
	msgLen = pdu.pduLen;
	msgData = new BACnetOctet[ pdu.pduLen ];
	memcpy( msgData, pdu.pduData, pdu.pduLen );
	
	// ready to go
	InstallTask();
}

//
//	BACnetVLANMsg::~BACnetVLANMsg
//
//	The constructor made a local copy of the PDU data, so when this object gets 
//	deleted, the copy needs to go away as well.
//

BACnetVLANMsg::~BACnetVLANMsg( void )
{
	// dispose of the copy of the data
	delete[] msgData;
}

//
//	BACnetVLANMsg::ProcessTask
//

void BACnetVLANMsg::ProcessTask( void )
{
	// process it
	msgLAN->ProcessMessage( this );
}

//
//	BACnetVLANNode::Indication
//

void BACnetVLANNode::Indication( const BACnetNPDU &pdu )
{
	// create a new VLAN message, it will get queued behind all of the other BACnetTask objects
	new BACnetVLANMsg( nodeLAN, portLocalAddr, pdu );
}

//
//	BACnetVLANNode::SendData
//

void BACnetVLANNode::SendData( BACnetOctet *, int )
{
	throw_(1);
}

//
//	BACnetVLAN::BACnetVLAN
//

BACnetVLAN::BACnetVLAN( void )
	: nodeListSize(0)
{
}

//
//	BACnetVLAN::NewNode
//

void BACnetVLAN::NewNode( BACnetVLANNodePtr np )
{
	int					i
	;
	
	// find an empty slot
	for (i = 0; i < nodeListSize; i++)
		if (!nodeList[i])
			break;
	
	// no slot found
	if (i >= nodeListSize) {
		if (nodeListSize == kBACnetVLANNodeListSize)
			throw_(2); // no more node slots available
		
		i = nodeListSize++;
	}
	
	// set up the node
	nodeList[i] = np;
	
	// give it a default address
	BACnetOctet			addrByte = i
	;
	BACnetAddress		addr( &addrByte, 1 )
	;
	
	np->portLocalAddr = addr;
}

//
//	BACnetVLAN::DeleteNode
//

void BACnetVLAN::DeleteNode( BACnetVLANNodePtr np )
{
	int		i
	;
	
	// find the slot
	for (i = 0; i < nodeListSize; i++)
		if (nodeList[i] == np)
			break;
	
	// found it?
	if (i < nodeListSize)
		nodeList[i] = 0;
}

//
//	BACnetVLAN::ProcessMessage
//

void BACnetVLAN::ProcessMessage( BACnetVLANMsgPtr msg )
{
	int			i
	;
	BACnetNPDU	resp( msg->msgSource, msg->msgData, msg->msgLen )
	;
	
	switch (msg->msgDestination.addrType) {
		case localStationAddr:
			for (i = 0; i < nodeListSize; i++)
				if (nodeList[i]->portLocalAddr == msg->msgDestination) {
					nodeList[i]->Response( resp );
					break;
				}
			break;
			
		case localBroadcastAddr:
			for (i = 0; i < nodeListSize; i++)
				if (!(msg->msgSource == nodeList[i]->portLocalAddr))
					nodeList[i]->Response( resp );
			break;
			
		default:
			break;
	}
}
