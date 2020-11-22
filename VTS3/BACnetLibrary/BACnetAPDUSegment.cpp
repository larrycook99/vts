
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "BACnet.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetAPDUSegment::BACnetAPDUSegment
//
//	This constructor is called when a client or a server TSM has an outbound message
//	that needs segmentation.  It must be a confirmed request or a complex ack, those 
//	are the only ones that can be segmented.
//
//	The lifetime of the buffer is longer than the lifetime of the APDU parameter (which 
//	probably belongs to a client or the workstation object) so a copy is necessary.
//

BACnetAPDUSegment::BACnetAPDUSegment( const BACnetAPDU &apdu, BACnetTSMPtr tp )
: segAPDU(0)	// no apdu buffer yet
, segTSMPtr(tp)
{
	segAPDU.apduAddr				= apdu.apduAddr;
	segAPDU.apduType				= apdu.apduType;
	segAPDU.apduSeg					= apdu.apduSeg;
	segAPDU.apduMor					= apdu.apduMor;
	segAPDU.apduSA					= apdu.apduSA;
	segAPDU.apduSrv					= apdu.apduSrv;
	segAPDU.apduNak					= apdu.apduNak;
	segAPDU.apduSeq					= apdu.apduSeq;
	segAPDU.apduWin					= apdu.apduWin;
	segAPDU.apduMaxResp				= apdu.apduMaxResp;
	segAPDU.apduMaxSegs				= apdu.apduMaxSegs;
	segAPDU.apduService				= apdu.apduService;
	segAPDU.apduInvokeID			= apdu.apduInvokeID;
	segAPDU.apduAbortRejectReason	= apdu.apduAbortRejectReason;
	segAPDU.apduNetworkPriority		= apdu.apduNetworkPriority;
	segAPDU.apduExpectingReply		= apdu.apduExpectingReply;
	
	// make a copy of the variable encoded portion
	if (apdu.pktLength != 0) {
		segBuff = new BACnetOctet[apdu.pktLength];
		segBuffSize = apdu.pktLength;
		segLen = apdu.pktLength;
		
		memcpy( segBuff, apdu.pktBuffer, (size_t)apdu.pktLength );
	} else {
		segBuff = 0;
		segBuffSize = 0;
		segLen = 0;
	}
}

//
//	BACnetAPDUSegment::BACnetAPDUSegment
//

BACnetAPDUSegment::BACnetAPDUSegment( int ssize, BACnetTSMPtr tp )
: segAPDU(0)	// no apdu buffer yet
, segTSMPtr(tp)
{
	segBuff = new BACnetOctet[ssize];
	segBuffSize = ssize;
	segLen = 0;
}

//
//	BACnetAPDUSegment::~BACnetAPDUSegment
//

BACnetAPDUSegment::~BACnetAPDUSegment( void )
{
	delete[] segBuff;
}

//
//	BACnetAPDUSegment::operator[]
//

const BACnetAPDU& BACnetAPDUSegment::operator[](const int indx)
{
	int	 segSize = segTSMPtr->tsmSegmentSize;
	
#if _TSMDebug
	cout << "[BACnetAPDUSegment: building segment " << indx << "]" << endl;
#endif
	
	// clear the contents
	segAPDU.pktLength = 0;
	int len;
	
	switch (segAPDU.apduType) {
		case confirmedRequestPDU:
			segAPDU.apduMaxResp = segTSMPtr->tsmDevice->deviceMaxAPDUSize;
			segAPDU.apduMaxSegs = segTSMPtr->tsmDevice->deviceMaxSegs;
			segAPDU.apduInvokeID = segTSMPtr->tsmInvokeID;
			
			// segmented message?
			if (segTSMPtr->tsmSegmentCount != 1) {
				segAPDU.apduSeg = true;
				segAPDU.apduMor = (indx < (segTSMPtr->tsmSegmentCount - 1)); // more follows
				segAPDU.apduSeq = indx % 256;						// sequence number
				segAPDU.apduWin = segTSMPtr->tsmActualWindowSize;	// window size
				len = (segAPDU.apduMor) ? segSize : (segLen % segSize);
			} else {
				segAPDU.apduSeg = false;
				segAPDU.apduMor = false;
				len = segLen;
			}
			
			// segmented response accepted?
			segAPDU.apduSA = ( (segTSMPtr->tsmSegmentation == segmentedBoth)
					|| (segTSMPtr->tsmSegmentation == segmentedReceive)
					);
			
			// reference the content
			segAPDU.SetBuffer( segBuff + (indx * segSize), len );
			break;
		
		case complexAckPDU:
			segAPDU.apduInvokeID = segTSMPtr->tsmInvokeID;
			
			// segmented message?
			if (segTSMPtr->tsmSegmentCount != 1) {
				segAPDU.apduSeg = true;
				segAPDU.apduMor = (indx < (segTSMPtr->tsmSegmentCount - 1)); // more follows
				segAPDU.apduSeq = indx % 256;						// sequence number
				segAPDU.apduWin = segTSMPtr->tsmActualWindowSize;	// window size
				len = (segAPDU.apduMor) ? segSize : (segLen % segSize);
			} else {
				segAPDU.apduSeg = false;
				segAPDU.apduMor = false;
				len = segLen;
			}
			
			// reference the content
			segAPDU.SetBuffer( segBuff + (indx * segSize), len );
			break;
		
		default:
			throw_(1); // other PDU types invalid
	}
	
	return segAPDU;
}

//
//	BACnetAPDUSegment::AddSegment
//

int BACnetAPDUSegment::AddSegment( const BACnetAPDU &apdu )
{
	// check for first packet
	if (segLen == 0) {
		segAPDU.apduAddr				= apdu.apduAddr;
		segAPDU.apduType				= apdu.apduType;
		segAPDU.apduSeg					= apdu.apduSeg;
		segAPDU.apduMor					= apdu.apduMor;
		segAPDU.apduSA					= apdu.apduSA;
		segAPDU.apduSrv					= apdu.apduSrv;
		segAPDU.apduNak					= apdu.apduNak;
		segAPDU.apduSeq					= apdu.apduSeq;
		segAPDU.apduWin					= apdu.apduWin;
		segAPDU.apduMaxResp				= apdu.apduMaxResp;
		segAPDU.apduMaxSegs				= apdu.apduMaxSegs;
		segAPDU.apduService				= apdu.apduService;
		segAPDU.apduInvokeID			= apdu.apduInvokeID;
		segAPDU.apduAbortRejectReason	= apdu.apduAbortRejectReason;
		segAPDU.apduNetworkPriority		= apdu.apduNetworkPriority;
		segAPDU.apduExpectingReply		= apdu.apduExpectingReply;

		// DO NOT set these here: let operator[] or ResultAPDU do it
//		segAPDU.pktBuffer = segBuff;
//		segAPDU.pktLength = 0;
	}

	if ((segLen + apdu.pktLength) > segBuffSize)
	{
		// Allocate extra space: twice as much as we need
		segBuffSize = 2*(segLen + apdu.pktLength);
		if (segBuffSize < 1024) 
			segBuffSize = 1024;
		
		BACnetOctet *pNew = new BACnetOctet[ segBuffSize ];
		memcpy( pNew, segBuff, segLen );
		delete[] segBuff;
		segBuff = pNew;
	}
	
	// copy variable part of APDU into buffer
	memcpy( segBuff + segLen, apdu.pktBuffer, (size_t)apdu.pktLength );
	segLen += apdu.pktLength;
	
	// DO NOT set this: use segLen (as from BACnetClientTSM::AwaitConfirmationTimeout)
	// or let operator[] or ResultAPDU set the APDU when needed
//	segAPDU.pktLength = segLen;
	
	return 0;
}

//
//	BACnetAPDUSegment::ResultAPDU
//

const BACnetAPDU& BACnetAPDUSegment::ResultAPDU( void )
{
	// Set buffer to the entire multi-segment data
	segAPDU.SetBuffer( segBuff, segLen );
	return segAPDU;
}
