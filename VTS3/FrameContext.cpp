// FrameContext.cpp: implementation of the CFrameContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VTS.h"
#include "FrameContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CFrameContext*	gNewFrameContext;

//
//	CFrameContext::CFrameContext
//

CFrameContext::CFrameContext()
	: m_pDoc(0), m_NextFrame(0)
	, m_Listeners(0)
	, m_PacketCount(0), m_CurrentPacket(-1)
	, m_DetailCount(0), m_CurrentDetail(-1)
	, m_HexStart(0), m_HexLen(0)
//	, m_Packet(), m_PacketInfo( false, true )		// no summary, do detail   MAD_DB
	, m_PacketInfo( false, true )					// no summary, do detail
{
}

//
//	CFrameContext::~CFrameContext
//
//	The only thing the destructor has to do is make sure that the 
//	document it is bound to will not have this soon to be invalid 
//	object in its list.
//

CFrameContext::~CFrameContext()
{
	if (m_pDoc)
		m_pDoc->UnbindFrameContext( this );
}

//
//	CFrameContext::AddListener
//
//	There is a new listener that needs to be added to this context.  In most 
//	cases it will be a view within a frame, but in the future it may be in 
//	its own frame but shares the context with other frames.  It would be nice 
//	to double-click on a packet and open up a window with just the detail 
//	and hex views of the packet (in which case it would keep its own context)
//	or ctrl-click on a packet to give it its own window but keep it bound to 
//	the window.
//

void CFrameContext::AddListener( CFrameContextListener* pListener )
{
	pListener->m_NextListener = m_Listeners;
	m_Listeners = pListener;
}


// The EPICS view creates and destroys so we want to erase before we add upon creation again

void CFrameContext::RemoveListener( CFrameContextListener * pListener )
{
	if ( m_Listeners == pListener )
		m_Listeners = pListener->m_NextListener;
	else
	{
		for (CFrameContextListener* cur = m_Listeners; cur  &&  cur->m_NextListener; cur = cur->m_NextListener)
			if ( cur->m_NextListener == pListener )
			{
				cur->m_NextListener = pListener->m_NextListener;
				break;
			}
	}
}



//
//	CFrameContext::NotifyListeners
//
//	Tell all the listeners that something about the frame context has changed.
//	See CFrameContext::Signal for a list of the defined signals.  Listeners are 
//	free to process or ignore the notification as they wish.
//

void CFrameContext::NotifyListeners( Signal s )
{
	for (CFrameContextListener* cur = m_Listeners; cur; cur = cur->m_NextListener)
		cur->ContextChange( s );
}


void CFrameContext::UpdatePrefs( void )
{
	// We've called this guy to tell our listeners that some settings in their prefs
	// might need to update... so go about it.

	NotifyListeners( eUpdatePrefs );
}

//
//	CFrameContext::SetPacketCount
//
//	The packet count is in the bound database, but rather than go through the 
//	overhead to get the count, just keep a local copy in the context.
//

void CFrameContext::SetPacketCount( int packetCnt )
{
	// skip if this context already has the correct packet count
	if (packetCnt == m_PacketCount)
		return;
	
	// update the local copy
	m_PacketCount = packetCnt;

	// tell the listeners
	NotifyListeners( eNewPacketCount );
}

//
//	CFrameContext::SetCurrentPacket
//

void CFrameContext::SetCurrentPacket( int packetNum )
{
	// skip it if this context is already pointing to the correct packet
	if (packetNum == m_CurrentPacket)
		return;
	
	// make sure it's valid, -1 means nothing is selected
	if (packetNum != -1)
		if ((packetNum < 0) || (packetNum >= m_PacketCount))
			return;
	
	// update the current packet
	m_CurrentPacket = packetNum;			// neccessary for GetCurrentPacket to work properly

	VTSPacketPtr ppkt = GetCurrentPacket();
	if (ppkt != NULL)
	{
		// set globals so name lookups have somewhere to go
		NetworkSniffer::SetLookupContext( ppkt->packetHdr.m_szPortName );

		m_PacketInfo.Interpret( (BACnetPIInfo::ProtocolType) ppkt->packetHdr.packetProtocolID, (char *) ppkt->packetData, ppkt->packetLen );
		if ((gCurrentInfo->detailLine != NULL) && (gCurrentInfo->detailLine[0] != NULL))
		{
			gCurrentInfo->detailLine[0]->piOffset = 0;
			gCurrentInfo->detailLine[0]->piLen = 0;
			gCurrentInfo->detailLine[0]->piNodeType = NT_NORMAL;
			sprintf(gCurrentInfo->detailLine[0]->piLine,"Timestamp : %s", ppkt->GetTimeStampString());
		}
	}

	// tell the listeners that the packet changed, come and get the new contents
	NotifyListeners( eNewCurrentPacket );
}


VTSPacketPtr CFrameContext::GetCurrentPacket()
{
	return m_pDoc->GetPacket(m_CurrentPacket);
}



//
//	CFrameContext::SetDetailCount
//

void CFrameContext::SetDetailCount( int detailCnt )
{
	if (detailCnt == m_DetailCount)
		return;
	
	m_DetailCount = detailCnt;
	NotifyListeners( eNewDetailCount );
}

//
//	CFrameContext::SetCurrentDetail
//

void CFrameContext::SetCurrentDetail( int detailNum )
{
	if (detailNum == m_CurrentDetail)
		return;
	
	if (detailNum != -1)
		if ((detailNum < 0) || (detailNum >= m_DetailCount))
			return;
	
	m_CurrentDetail = detailNum;
	NotifyListeners( eNewCurrentDetail );

	if (detailNum != -1)
		SetCurrentRange( m_PacketInfo.detailLine[detailNum]->piOffset
			, m_PacketInfo.detailLine[detailNum]->piLen
			);
}

//
//	CFrameContext::SetCurrentRange
//

void CFrameContext::SetCurrentRange( int start, int len )
{
	m_HexStart = start;
	m_HexLen = len;
	NotifyListeners( eNewCurrentRange );
}

//
//	CFrameContextListener::CFrameContextListener
//

CFrameContextListener::CFrameContextListener()
	: m_FrameContext(0), m_NextListener(0)
{
}

//
//	CFrameContextListener::SetContext
//

void CFrameContextListener::SetContext( CFrameContextPtr pContext )
{
	m_FrameContext = pContext;
	pContext->AddListener( this );
}


void CFrameContextListener::RemoveContext()
{
	if ( m_FrameContext != NULL )
		m_FrameContext->RemoveListener(this);
}
