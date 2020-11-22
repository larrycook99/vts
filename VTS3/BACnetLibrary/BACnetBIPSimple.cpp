
#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "BACnetBIPSimple.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetBIPSimple::Indication
//
//	This is called when the appliction has a message to send out.  This function 
//	attaches the proper BIP header to the front and sends it down to the endpoint
//	for delivery.
//

void BACnetBIPSimple::Indication( const BACnetNPDU &npdu )
{
	int				len
	;
	BACnetOctet		*msg, *msgPtr
	;
	
	switch (npdu.pduAddr.addrType) {
		case localStationAddr:
			len = 4 + npdu.pduLen;
			msg = msgPtr = new BACnetOctet[ len ];
			
			*msgPtr++ = kBVLCType;					// BVLL message
			*msgPtr++ = bvlcOriginalUnicastNPDU;	// original unicast
			*msgPtr++ = (len >> 8) & 0xFF;			// length
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
			
			*msgPtr++ = kBVLCType;					// BVLL message
			*msgPtr++ = bvlcOriginalBroadcastNPDU;	// original broadcast
			*msgPtr++ = (len >> 8) & 0xFF;			// length
			*msgPtr++ = (len & 0xFF);
			memcpy( msgPtr, npdu.pduData, npdu.pduLen );
			
			Request(
				BACnetNPDU( npdu.pduAddr, msg, len
					, npdu.pduExpectingReply, npdu.pduNetworkPriority
					)
				);
			delete[] msg;
			break;
        
        default:
            throw_(1); // should never get any other kind of address
	}
}

//
//	BACnetBIPSimple::Confirmation
//
//	This is called when the endpoint has received a message.  The BIP header is 
//	examined and the results passed up to the application.  If it is not one of 
//	ours, dump it.
//

void BACnetBIPSimple::Confirmation( const BACnetNPDU &npdu )
{
	int					msgType, msgLen
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
