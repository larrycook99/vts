
#include "stdafx.h"

#define _BACnetVIPLANDebug			0
#define _BACnetVIPLANMsgDebug		0

#if _BACnetVIPLANDebug || _BACnetVIPLANMsgDebug
#include <iostream>
#endif

#include <stdlib.h>
#include <string.h>

#include "BACnet.hpp"
#include "BACnetVIPLAN.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetVIPLANMsg::BACnetVIPLANMsg
//

BACnetVIPLANMsg::BACnetVIPLANMsg( BACnetVIPLANPtr lan, BACnetVIPLANNodePtr source, const BACnetNPDU &pdu )
	: BACnetTask( oneShotDeleteTask, 0 )
	, msgLAN(lan), msgSource(source), msgDestination(pdu.pduAddr)
{
#if _BACnetVIPLANMsgDebug
	cout << "(new msg " << (int)this << ")" << endl;
#endif
	// copy the message data
	msgLen = pdu.pduLen;
	msgData = new BACnetOctet[ pdu.pduLen ];
	memcpy( msgData, pdu.pduData, pdu.pduLen );
	
	// ready to go
	InstallTask();
}

//
//	BACnetVIPLANMsg::~BACnetVIPLANMsg
//
//	The constructor made a local copy of the PDU data, so when this object gets 
//	deleted, the copy needs to go away as well.
//

BACnetVIPLANMsg::~BACnetVIPLANMsg( void )
{
#if _BACnetVIPLANMsgDebug
	cout << "(delete msg " << (int)this << ")" << endl;
#endif
	// dispose of the copy of the data
	delete[] msgData;
}

//
//	BACnetVIPLANMsg::ProcessTask
//

void BACnetVIPLANMsg::ProcessTask( void )
{
	// process it
	msgLAN->ProcessMessage( this );
}

//
//	BACnetVIPLANNode::Indication
//

void BACnetVIPLANNode::Indication( const BACnetNPDU &pdu )
{
	// create a new VLAN message, it will get queued behind all of the other BACnetTask objects
	new BACnetVIPLANMsg( nodeLAN, this, pdu );
}

//
//	BACnetVIPLAN::BACnetVIPLAN
//

BACnetVIPLAN::BACnetVIPLAN( int id )
	: vlanID( id ), nodeListSize(0)
{
}

//
//	BACnetVIPLAN::NewNode
//

BACnetVIPLANNodePtr BACnetVIPLAN::NewNode( unsigned long ipAddr, unsigned long subnetMask, unsigned short port )
{
	BACnetVIPLANNodePtr	rslt
	;
	
	rslt = new BACnetVIPLANNode( this, ipAddr, subnetMask, port );
	nodeList[nodeListSize++] = rslt;
	
	return rslt;
}

//
//	BACnetVIPLAN::ProcessMessage
//

void BACnetVIPLAN::ProcessMessage( BACnetVIPLANMsgPtr msg )
{
	int				i
	;
	unsigned long	destAddr
	;
	unsigned short	destPort
	;
	BACnetNPDU	resp( msg->msgSource->ipAddress, msg->msgData, msg->msgLen )
	;
	
#if _BACnetVIPLANDebug
	const static char hex[] = "0123456789ABCDEF";
	cout << "VIPLAN " << vlanID << ": ";
	cout << "src = " << msg->msgSource->ipAddress;
	cout << ", dst = " << msg->msgDestination;
	cout << ", msg = ";
	for (int i = 0; i < msg->msgLen; i++) {
		cout << hex[ (msg->msgData[i] >> 4) & 0x0F ];
		cout << hex[ msg->msgData[i] & 0x0F ];
		cout << '.';
	}
	cout << endl;
#endif
	
	// extract the destination address
	destAddr = *(unsigned long*)msg->msgDestination.addrAddr;
	destPort = *(unsigned short*)(msg->msgDestination.addrAddr + 4);
	
	switch (msg->msgDestination.addrType) {
		case localStationAddr:
			for (i = 0; i < nodeListSize; i++) {
				// no message back to self
				if (nodeList[i] == msg->msgSource)
					continue;
				
				// check for directed broadcast
				if ((destAddr & (~nodeList[i]->nodeSubnetMask)) == (~nodeList[i]->nodeSubnetMask)) {
					if (nodeList[i]->nodeSubnet != (destAddr & nodeList[i]->nodeSubnetMask))
						continue;
				} else
				// station must match
				if (nodeList[i]->nodeIPAddr != destAddr)
					continue;
				
				// must have the correct port
				if (nodeList[i]->nodePort != destPort)
					continue;
				
				// send the request along
				nodeList[i]->Response( resp );
			}
			break;
			
		case localBroadcastAddr:
			for (i = 0; i < nodeListSize; i++) {
				// no message back to self
				if (nodeList[i] == msg->msgSource)
					continue;
				
				// must be on the same subnet
				if (nodeList[i]->nodeSubnet != msg->msgSource->nodeSubnet)
					continue;
				
				// source port and destination port must match
				if (nodeList[i]->nodePort != msg->msgSource->nodePort)
					continue;
				
				// send the request along
				nodeList[i]->Response( resp );
			}
			break;
			
		default:
#if _BACnetVIPLANDebug
			cout << "VIPLAN Addressing error" << endl;
#endif
			break;
	}
}
