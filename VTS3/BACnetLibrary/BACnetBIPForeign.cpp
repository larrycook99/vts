
#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "BACnetBIPForeign.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetBIPForeign::BACnetBIPForeign
//

BACnetBIPForeign::BACnetBIPForeign( void )
	: BACnetTask( recurringTask )
	, foreignStatus(0), foreignTimeToLive(0)
{
}

//
//	BACnetBIPForeign::~BACnetBIPForeign
//

BACnetBIPForeign::~BACnetBIPForeign( void )
{
}

//
//	BACnetBIPForeign::BACnetBIPForeign
//

void BACnetBIPForeign::Register( unsigned long ipAddr, unsigned short port, int ttl )
{
	// we're dead until a response comes back
	foreignStatus = 0;
	foreignTimeToLive = ttl;
	
	// set up the BBMD address
	foreignBBMDAddr.Pack( ipAddr, port );
	
	// while we're alive, send registration requests at this interval
	InstallTask( recurringTask, foreignTimeToLive * 1000 );
	
	// send off one right now while we're at it
	ProcessTask();
}

//
//	BACnetBIPForeign::Indication
//
//	This is called when the appliction has a message to send out.  This function 
//	attaches the proper BIP header to the front and sends it down to the endpoint
//	for delivery.
//

void BACnetBIPForeign::Indication( const BACnetNPDU &npdu )
{
	int				len
	;
	BACnetOctet		*msg, *msgPtr
	;
	
	if (!foreignStatus) {
		return;
	}
	
	switch (npdu.pduAddr.addrType) {
		case localStationAddr:
			len = 4 + npdu.pduLen;
			msg = msgPtr = new BACnetOctet[ len ];
			
			*msgPtr++ = kBVLCType;						// BVLL message
			*msgPtr++ = bvlcOriginalUnicastNPDU;		// original unicast
			*msgPtr++ = (len >> 8) & 0xFF;				// length
			*msgPtr++ = (len & 0xFF);
			memcpy( msgPtr, npdu.pduData, npdu.pduLen );
			
			Request(
				BACnetNPDU( npdu.pduAddr, msg, len
					, npdu.pduExpectingReply, npdu.pduNetworkPriority
					)
				);
			delete[] msg;
			break;
			
		case localBroadcastAddr:
			len = 4 + npdu.pduLen;
			msg = msgPtr = new BACnetOctet[ len ];
			
			*msgPtr++ = kBVLCType;							// BVLL message
			*msgPtr++ = bvlcDistributeBroadcastToNetwork;	// distribute broadcast
			*msgPtr++ = (len >> 8) & 0xFF;					// length
			*msgPtr++ = (len & 0xFF);
			memcpy( msgPtr, npdu.pduData, npdu.pduLen );	// NPDU
			
			// send it to the BBMD for distribution
			Request(
				BACnetNPDU( foreignBBMDAddr, msg, len
					, npdu.pduExpectingReply, npdu.pduNetworkPriority
					)
				);
			delete[] msg;
			break;
			
		default:
			throw_(1); // no other address types allowed
	}
}

//
//	BACnetBIPForeign::Confirmation
//
//	This is called when the endpoint has received a message.  The BIP header is 
//	examined and the results passed up to the application.  If it is not one of 
//	ours, dump it.
//

void BACnetBIPForeign::Confirmation( const BACnetNPDU &npdu )
{
	int					msgType, msgLen, rsltCode
	;
	BACnetAddress		addr
	;
	const BACnetOctet	*msgPtr = npdu.pduData
	;
	
	// check for one of our headers
	if ((npdu.pduLen < 4) || (*msgPtr++ != kBVLCType))
		return;
	
	msgType = *msgPtr++;
	msgLen = *msgPtr++;
	msgLen = (msgLen << 8) + *msgPtr++;
	
	// make sure the length is correct
	if (npdu.pduLen != msgLen)
		return;
	
	switch (msgType) {
		case bvlcResult:
			rsltCode = *msgPtr++;
			rsltCode = (rsltCode << 8) + *msgPtr++;
			
			// check the result code
			if (rsltCode == 0)
				foreignStatus = 1;
			else
			if (rsltCode == 0x0030) {
				foreignStatus = 0;
				SuspendTask();
			}
			break;
			
		case blvcForwardedNPDU:
			// make sure there's enough data
			if (msgLen < 10)
				return;
			
			// extract the address from the header
			addr.LocalStation( msgPtr, 6 );
			
			// use result to forward up to application
			Response( BACnetNPDU( addr, msgPtr + 6, msgLen - 10 ) );
			break;
			
		case bvlcOriginalUnicastNPDU:
		case bvlcOriginalBroadcastNPDU:
			// keep the source that came up from the endpoint
			Response( BACnetNPDU( npdu.pduAddr, msgPtr, msgLen - 4 ) );
			break;
	}
}

//
//	BACnetBIPForeign::ProcessTask
//
//	This object sends a foreign device registration request to the BBMD.  It 
//	expects a confirmation (+) or (-) to maintain the foreignStatus.  If no 
//	response comes back in a reasonable amount of time the the device should 
//	send it again (more quickly than the TTL might otherwise specify).  If 
//	no confirmation comes back after a few attempts, this should go to an 
//	offline state.
//
//	Along the same lines, this object is expecting to go online soon after
//	Register() is called.  What happens when it can't?  How often (and how fast) 
//	should it continue the registration process before giving up (if ever)?
//
//	None of this behavior is specified in the standard...perhaps it should be.
//

void BACnetBIPForeign::ProcessTask( void )
{
	BACnetOctet		msg[6], *msgPtr = msg;
	
	*msgPtr++ = kBVLCType;							// type
	*msgPtr++ = bvlcRegisterForeignDevice;			// register foreign device
	*msgPtr++ = 0x00;								// length
	*msgPtr++ = 0x06;
	*msgPtr++ = (foreignTimeToLive >> 8) & 0xFF;	// time to live
	*msgPtr++ = (foreignTimeToLive & 0xFF);
	
	// send the request to the BBMD
	Request( BACnetNPDU( foreignBBMDAddr, msg, 6 ) );
}
