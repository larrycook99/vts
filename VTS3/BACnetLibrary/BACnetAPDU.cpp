
#include "stdafx.h"

#include "BACnet.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetAPDU::BACnetAPDU
//

BACnetAPDU::BACnetAPDU( int initBuffSize )
	: BACnetAPDUEncoder( initBuffSize )
	, apduType(abortPDU)
	, apduSeg(0), apduMor(0), apduSA(0), apduSrv(0), apduNak(0)
	, apduSeq(0), apduWin(0)
	, apduService(0)
	, apduInvokeID(0), apduAbortRejectReason(0)
	, apduExpectingReply(0), apduNetworkPriority(0)
{
}

//
//	BACnetAPDU::BACnetAPDU
//

BACnetAPDU::BACnetAPDU( BACnetOctet *buffPtr, int buffLen )
	: BACnetAPDUEncoder( buffPtr, buffLen )
	, apduType(abortPDU)
	, apduSeg(0), apduMor(0), apduSA(0), apduSrv(0), apduNak(0)
	, apduSeq(0), apduWin(0)
	, apduService(0)
	, apduInvokeID(0), apduAbortRejectReason(0)
	, apduExpectingReply(0), apduNetworkPriority(0)
{
}

//
//	BACnetAPDU::BACnetAPDU
//

BACnetAPDU::BACnetAPDU( const BACnetAPDU &pdu )
	: BACnetAPDUEncoder( pdu.pktBuffer, pdu.pktLength )
	, apduType(pdu.apduType)
	, apduSeg(pdu.apduSeg), apduMor(pdu.apduMor), apduSA(pdu.apduSA)
	, apduSrv(pdu.apduSrv), apduNak(pdu.apduNak)
	, apduSeq(pdu.apduSeq), apduWin(pdu.apduWin)
	, apduService(pdu.apduService)
	, apduInvokeID(pdu.apduInvokeID)
	, apduAbortRejectReason(pdu.apduAbortRejectReason)
	, apduExpectingReply(pdu.apduExpectingReply)
	, apduNetworkPriority(pdu.apduNetworkPriority)
{
}

BACnetAPDU::~BACnetAPDU()
{
}

//
//	BACnetAPDU::Encode
//

void BACnetAPDU::Encode( BACnetAPDUEncoder& enc ) const
{
	// transform the APDU
	switch (apduType) {
		case confirmedRequestPDU:
			enc.Append( (((int)apduType) << 4)
					+ (apduSeg ? 0x08 : 0)
					+ (apduMor ? 0x04 : 0)
					+ (apduSA ? 0x02 : 0)
				);
			enc.Append( (MaxSegsEncode(apduMaxSegs) << 4) | MaxAPDUEncode(apduMaxResp) );
			enc.Append( apduInvokeID );
			if (apduSeg) {
				enc.Append( apduSeq );
				enc.Append( apduWin );
			}
			enc.Append( apduService );
			enc.Append( pktBuffer, pktLength );
			break;
			
		case unconfirmedRequestPDU:
			enc.Append( ((int)apduType) << 4 );
			enc.Append( apduService );
			enc.Append( pktBuffer, pktLength );
			break;
			
		case simpleAckPDU:
			enc.Append( ((int)apduType) << 4 );
			enc.Append( apduInvokeID );
			enc.Append( apduService );
			break;
			
		case complexAckPDU:
			enc.Append( (((int)apduType) << 4)
					+ (apduSeg ? 0x08 : 0)
					+ (apduMor ? 0x04 : 0)
				);
			enc.Append( apduInvokeID );
			if (apduSeg) {
				enc.Append( apduSeq );
				enc.Append( apduWin );
			}
			enc.Append( apduService );
			enc.Append( pktBuffer, pktLength );
			break;
			
		case segmentAckPDU:
			enc.Append( (((int)apduType) << 4)
					+ (apduNak ? 0x02 : 0)
					+ (apduSrv ? 0x01 : 0)
				);
			enc.Append( apduInvokeID );
			enc.Append( apduSeq );
			enc.Append( apduWin );
			break;
			
		case errorPDU:
			enc.Append( ((int)apduType) << 4 );
			enc.Append( apduInvokeID );
			enc.Append( apduService );
			enc.Append( pktBuffer, pktLength );
			break;
			
		case rejectPDU:
			enc.Append( ((int)apduType) << 4 );
			enc.Append( apduInvokeID );
			enc.Append( apduAbortRejectReason );
			break;
			
		case abortPDU:
			enc.Append( (((int)apduType) << 4)
					+ (apduSrv ? 0x01 : 0)
				);
			enc.Append( apduInvokeID );
			enc.Append( apduAbortRejectReason );
			break;
	}
}

//
//	BACnetAPDU::Decode
//

void BACnetAPDU::Decode( const BACnetAPDUDecoder& dec )
{
	// transform the NPDU into an APDU
	apduType = (BACnetAPDUType)((dec.pktBuffer[0] & 0xF0) >> 4);
	
	switch (apduType) {
		case confirmedRequestPDU:
			apduSeg = ((dec.pktBuffer[0] & 0x08) != 0);
			apduMor = ((dec.pktBuffer[0] & 0x04) != 0);
			apduSA = ((dec.pktBuffer[0] & 0x02) != 0);
			apduMaxResp = MaxAPDUDecode( dec.pktBuffer[1] & 0x0F );
			apduMaxSegs = MaxSegsDecode( (dec.pktBuffer[1] >> 4) & 0x07 );
			apduInvokeID = dec.pktBuffer[2];
			if (apduSeg) {
				apduSeq = dec.pktBuffer[3];
				apduWin = dec.pktBuffer[4];
				apduService = dec.pktBuffer[5];
				SetBuffer( dec.pktBuffer + 6, dec.pktLength - 6 );
			} else {
				apduService = dec.pktBuffer[3];
				SetBuffer( dec.pktBuffer + 4, dec.pktLength - 4 );
			}
			break;
			
		case unconfirmedRequestPDU:
			apduService = dec.pktBuffer[1];
			SetBuffer( dec.pktBuffer + 2, dec.pktLength - 2 );
			break;
			
		case simpleAckPDU:
			apduInvokeID = dec.pktBuffer[1];
			apduService = dec.pktBuffer[2];
			break;
			
		case complexAckPDU:
			apduSeg = ((dec.pktBuffer[0] & 0x08) != 0);
			apduMor = ((dec.pktBuffer[0] & 0x04) != 0);
			apduInvokeID = dec.pktBuffer[1];
			if (apduSeg) {
				apduSeq = dec.pktBuffer[2];
				apduWin = dec.pktBuffer[3];
				apduService = dec.pktBuffer[4];
				SetBuffer( dec.pktBuffer + 5, dec.pktLength - 5 );
			} else {
				apduService = dec.pktBuffer[2];
				SetBuffer( dec.pktBuffer + 3, dec.pktLength - 3 );
			}
			break;
			
		case segmentAckPDU:
			apduNak = ((dec.pktBuffer[0] & 0x02) != 0);
			apduSrv = ((dec.pktBuffer[0] & 0x01) != 0);
			apduInvokeID = dec.pktBuffer[1];
			apduSeq = dec.pktBuffer[2];
			apduWin = dec.pktBuffer[3];
			break;
			
		case errorPDU:
			apduInvokeID = dec.pktBuffer[1];
			apduService = dec.pktBuffer[2];
			SetBuffer( dec.pktBuffer + 3, dec.pktLength - 3 );
			break;
			
		case rejectPDU:
			apduInvokeID = dec.pktBuffer[1];
			apduAbortRejectReason = dec.pktBuffer[2];
			break;
			
		case abortPDU:
			apduSrv = ((dec.pktBuffer[0] & 0x01) != 0);
			apduInvokeID = dec.pktBuffer[1];
			apduAbortRejectReason = dec.pktBuffer[2]; // meaningless for segAck
			break;
	}
}

//
//	BACnetConfirmedServiceAPDU::BACnetConfirmedServiceAPDU
//
//	Most likely a confirmed service will be built without easy access to the device information record 
//	where information about segmentation support is stored, so build the simplest kind of message.  It 
//	may get modified (the SA bit in the first octet).  If it needs to be segmented, a new header will be 
//	built (see BACnetAPDUSegment::operator[]).
//
//	In addition to SA, the device will also have to modify the max-APDU-length-accepted octet based on 
//	its configuration.
//

BACnetConfirmedServiceAPDU::BACnetConfirmedServiceAPDU( BACnetConfirmedServiceChoice ch )
{
	apduType = confirmedRequestPDU;
	apduService = (int)ch;
	apduExpectingReply = 1;
}

//
//	BACnetUnconfirmedServiceAPDU::BACnetUnconfirmedServiceAPDU
//

BACnetUnconfirmedServiceAPDU::BACnetUnconfirmedServiceAPDU( BACnetUnconfirmedServiceChoice ch )
{
	apduType = unconfirmedRequestPDU;
	apduService = (int)ch;
}

//
//	BACnetSimpleAckAPDU::BACnetSimpleAckAPDU
//

BACnetSimpleAckAPDU::BACnetSimpleAckAPDU( BACnetConfirmedServiceChoice ch, BACnetOctet invID )
	: BACnetAPDU( 0 )
{
	apduType = simpleAckPDU;
	apduService = (int)ch;
	apduInvokeID = invID;
}

//
//	BACnetComplexAckAPDU::BACnetComplexAckAPDU
//
//	A complex ACK suffers from the same problems that a confirmed request does: at the time the message
//	is being built it's not known if segmentation is supported or if it needs to be segmented.  This 
//	may be rebuilt by a state machine.
//

BACnetComplexAckAPDU::BACnetComplexAckAPDU( BACnetConfirmedServiceChoice ch, BACnetOctet invID )
{
	apduType = complexAckPDU;
	apduService = (int)ch;
	apduInvokeID = invID;
}

//
//	BACnetComplexAckAPDU::SetInvokeID
//

void BACnetComplexAckAPDU::SetInvokeID( BACnetOctet invID )
{
	apduInvokeID = invID;
	pktBuffer[1] = invID;
}

//
//	BACnetSegmentAckAPDU::BACnetSegmentAckAPDU
//

BACnetSegmentAckAPDU::BACnetSegmentAckAPDU( const BACnetAddress &dest, BACnetOctet invID, int nak, int srv, BACnetOctet seq, BACnetOctet win )
	: BACnetAPDU( 0 )
{
	apduType = segmentAckPDU;
	apduAddr = dest;
	apduNak = (nak != 0);
	apduSrv = (srv != 0);
	apduSeq = seq;
	apduWin = win;
	apduInvokeID = invID;
}

//
//	BACnetErrorAPDU::BACnetErrorAPDU
//

BACnetErrorAPDU::BACnetErrorAPDU( BACnetConfirmedServiceChoice ch, BACnetOctet invID )
{
	apduType = errorPDU;
	apduService = (int)ch;
	apduInvokeID = invID;
}

//
//	BACnetRejectAPDU::BACnetRejectAPDU
//

BACnetRejectAPDU::BACnetRejectAPDU( BACnetOctet invID, BACnetRejectReason reason )
	: BACnetAPDU( 0 )
{
	apduType = rejectPDU;
	apduInvokeID = invID;
	apduAbortRejectReason = (int)reason;
}

//
//	BACnetAbortAPDU::BACnetAbortAPDU
//

BACnetAbortAPDU::BACnetAbortAPDU( const BACnetAddress &dest, int srv, BACnetOctet invID, BACnetAbortReason reason )
	: BACnetAPDU( 0 )
{
	apduType = abortPDU;
	apduAddr = dest;
	apduSrv = (srv != 0);
	apduInvokeID = invID;
	apduAbortRejectReason = (int)reason;
}
