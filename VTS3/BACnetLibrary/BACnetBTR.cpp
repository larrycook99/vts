
#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "BACnetBTR.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetBTR::BACnetBTR
//

BACnetBTR::BACnetBTR( void )
	: btrPeer(0), btrPeerLen(0)
{
}

//
//	BACnetBTR::~BACnetBTR
//

BACnetBTR::~BACnetBTR( void )
{
	if (btrPeer)
		delete[] btrPeer;
}

//
//	BACnetBTR::Indication
//
//	This is called when the router has a message to send out.  This function 
//	forwards directed messages to the peer BTR.  When a broadcast is being 
//	sent out, it is sent as a series of directed messages to the peers.
//

void BACnetBTR::Indication( const BACnetNPDU &npdu )
{
	int		i
	;

	switch (npdu.pduAddr.addrType) {
		case localStationAddr:
			Request( npdu );
			break;
			
		case localBroadcastAddr:
			for (i = 0; i < btrPeerLen; i++)
				Request(
					BACnetNPDU( btrPeer[i], npdu.pduData, npdu.pduLen
						, npdu.pduExpectingReply, npdu.pduNetworkPriority
						)
					);
			break;
        
        default:
            throw_(1); // should never get any other kind of address
	}
}

//
//	BACnetBTR::Confirmation
//
//	This is called when the endpoint has received a message.  It should 
//	simply be passed up to the router for processing.
//

void BACnetBTR::Confirmation( const BACnetNPDU &npdu )
{
	// check the length
	if (npdu.pduLen < 0)
		return;
	// check the version
	if (npdu.pduData[0] != 0x01)
		return;
	
	Response( npdu );
}

//
//	BACnetBTR::AddPeer
//

void BACnetBTR::AddPeer( const BACnetIPAddr &ipAddr )
{
	BACnetIPAddr	*newPeerList
	;
	
	newPeerList = new BACnetIPAddr[ btrPeerLen + 1 ];
	if (btrPeer)
		memcpy( newPeerList, btrPeer, (size_t)(btrPeerLen * sizeof(BACnetIPAddr)) );
	newPeerList[btrPeerLen++] = ipAddr;
	
	if (btrPeer)
		delete[] btrPeer;
	btrPeer = newPeerList;
}

//
//	BACnetBTR::DeletePeer
//

void BACnetBTR::DeletePeer( const BACnetIPAddr &ipAddr )
{
	int		i
	;
	BACnetIPAddr	*newPeerList
	;
	
	// find it in the list
	for (i = 0; i < btrPeerLen; i++)
		if (btrPeer[i] == ipAddr)
			break;
	if (i >= btrPeerLen)
		return;
	
	if (btrPeerLen == 1) {
		delete[] btrPeer;
		btrPeer = 0;
		btrPeerLen = 0;
	} else {
		// build a new list
		newPeerList = new BACnetIPAddr[ btrPeerLen - 1 ];
		
		if (i > 0)
			memcpy( newPeerList, btrPeer, (size_t)(i * sizeof(BACnetIPAddr)) );
		if (i <= (btrPeerLen - 1))
			memcpy( newPeerList+i, btrPeer + i + 1, (size_t)((btrPeerLen - i - 1) * sizeof(BACnetIPAddr)) );
		
		delete[] btrPeer;
		btrPeer = newPeerList;
		btrPeerLen -= 1;
	}
	
	// mock up an I-Am-Router-To-Network message with an empty list
	BACnetOctet		msg[] = { 0x01, 0x80, (unsigned char)IAmRouterToNetwork }
	;
	
	// send it to the router
	Response( BACnetNPDU( ipAddr, msg, 3 ) );
}

//
//	BACnetBTR::AddPeerNet
//
//	This function mocks up an I-Am-Router-To-Network message as if it came from an IP 
//	to tell the router to add this station as a path to the given network.  Note that the 
//	BTR object must already be bound to a router for this to do anything.
//

void BACnetBTR::AddPeerNet( const BACnetIPAddr &ipAddr, int net )
{
	// mock up an I-Am-Router-To-Network message
	BACnetOctet
		msg[] = { 0x01, 0x80, (unsigned char)IAmRouterToNetwork
				, (net >> 8) & 0xFF, (net & 0xFF)
				};
	
	// send it upstream to the router
	Response( BACnetNPDU( ipAddr, msg, 5 ) );
}
