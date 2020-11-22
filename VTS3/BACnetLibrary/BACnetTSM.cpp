
#include "stdafx.h"

#include "BACnet.hpp"
#include "BACnetIP.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetTSM::BACnetTSM
//

BACnetTSM::BACnetTSM( BACnetDevicePtr dp )
	: tsmState(tsmIdle)
	, tsmSeg(0)
	, tsmInvokeID(0)
	, tsmRetryCount(0)
	, tsmSegmentation(noSegmentation)
	, tsmSegmentSize(0)
	, tsmSegmentRetryCount(0)
	, tsmSegmentCount(0)
	, tsmSegmentsSent(0)
	, tsmMaxSegs(0)
	, tsmInitialSequenceNumber(0)
	, tsmLastSequenceNumber(0)
	, tsmActualWindowSize(0)
	, tsmProposedWindowSize(0)
	, tsmDevice(dp)
{
}

//
//	BACnetTSM::~BACnetTSM
//

BACnetTSM::~BACnetTSM( void )
{
	if (tsmSeg != NULL)
	{
		TRACE( "tsmSeg %p not NULL", tsmSeg );
	}
}

//
//	BACnetTSM::StartTimer
//

void BACnetTSM::StartTimer( int msecs )
{
	// now install this
	InstallTask( oneShotTask, msecs );
}

//
//	BACnetTSM::StopTimer
//

void BACnetTSM::StopTimer( void )
{
	SuspendTask();
}

//
//	BACnetTSM::RestartTimer
//

void BACnetTSM::RestartTimer( int msecs )
{
	InstallTask( oneShotTask, msecs );
}

//
//	BACnetTSM::FillWindow
//

void BACnetTSM::FillWindow( int seqNum )
{
	for (int i = 0; i < tsmActualWindowSize; i++ ) {
		const BACnetAPDU &apdu = (*tsmSeg)[ seqNum + i ];

		// send the message to the device
		tsmDevice->Indication( apdu );

		// check for no more follows
		if ((apdu.pktBuffer[0] & 0x04) == 0)
			return;
	}
}

//
//	BACnetTSM::InWindow
//

int BACnetTSM::InWindow( int seqNum, int initSeqNum )
{
	return ((((seqNum + 256 - initSeqNum) % 256) < tsmActualWindowSize) ? 1 : 0);
}

void BACnetTSM::SetState( BACnetTSMState newState )
{
#if _TSMDebug
	cout << (unsigned long)this
		 << " from " << tsmState << " to " << newState
		 << endl;
#endif
	if ((newState == tsmIdle) && (tsmSeg != NULL))
	{
		delete tsmSeg;
		tsmSeg = NULL;
	}
	
	tsmState = newState;
}
