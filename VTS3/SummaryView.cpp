// SummaryView.cpp : implementation file

This file is obsolete - replaced by CListSummaryView

#include "stdafx.h"
#include "VTS.h"

#include "VTSDoc.h"
#include "ChildFrm.h"
#include "SummaryView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSummaryView

IMPLEMENT_DYNCREATE(CSummaryView, CScrollView)

CSummaryView::CSummaryView()
{
}

CSummaryView::~CSummaryView()
{
}


BEGIN_MESSAGE_MAP(CSummaryView, CScrollView)
	//{{AFX_MSG_MAP(CSummaryView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_CANCELMODE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSummaryView drawing

void CSummaryView::OnInitialUpdate()
{
	// let everything initialize correctly
	CScrollLineView::OnInitialUpdate();
	
	// set the line count according to what is in the document
//	VTSDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);
//	SetLineCount( pDoc->m_nPacketCount );
	SetLineCount(m_FrameContext->m_PacketCount);

	// set the scroll position at the top
	ScrollToPosition( CPoint( 0, 0 ) );
}

void CSummaryView::OnDraw(CDC* pDC)
{
	CScrollLineView::OnDraw( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CSummaryView diagnostics

#ifdef _DEBUG
void CSummaryView::AssertValid() const
{
	CScrollLineView::AssertValid();
}

void CSummaryView::Dump(CDumpContext& dc) const
{
	CScrollLineView::Dump(dc);
}

VTSDoc* CSummaryView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(VTSDoc)));
	return (VTSDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSummaryView message handlers

int CSummaryView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollLineView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetContext( gNewFrameContext );

	return 0;
}

//
//	CSummaryView::OnUpdate
//
//	This function passes the update message along to the scroll view.  Note 
//	that CScrollLineView doesn't have an OnUpdate() function to call, so 
//	this hops over the class.
//

void CSummaryView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// pass update message to the view which invalidates the visible area
	CScrollView::OnUpdate( pSender, lHint, pHint );
}

void CSummaryView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int	lineNo = GetLineAtPoint( point );
	m_FrameContext->SetCurrentPacket( lineNo );
}

void CSummaryView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TRACE1( "CSummaryView::OnChar( %d, ...)\n", nChar );
	if ((nChar == 0x09) && (m_pTabRing))
		m_pTabRing->SetFocus();
}

void CSummaryView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	bool	shiftKeyDown	= (GetKeyState(VK_SHIFT) < 0);
	bool	controlKeyDown	= (GetKeyState(VK_CONTROL) < 0);

	int		nMapMode;
	SIZE	sizeTotal, sizePage, sizeLine;

	GetDeviceScrollSizes( nMapMode, sizeTotal, sizePage, sizeLine );

	switch (nChar) {
		case VK_UP:
			if ((m_FrameContext->m_PacketCount != 0) && (m_FrameContext->m_CurrentPacket > 0))
				m_FrameContext->SetCurrentPacket( m_FrameContext->m_CurrentPacket - 1);
			break;

		case VK_DOWN:
			if ((m_FrameContext->m_PacketCount != 0) && (m_FrameContext->m_CurrentPacket < (m_FrameContext->m_PacketCount - 1)))
				m_FrameContext->SetCurrentPacket( m_FrameContext->m_CurrentPacket + 1);
			break;

		case VK_PRIOR:
			if (m_FrameContext->m_PacketCount != 0)
				ScrollToPosition( CPoint( 0, GetScrollPosition().y - sizePage.cy ) );
			break;

		case VK_NEXT:
			if (m_FrameContext->m_PacketCount != 0)
				ScrollToPosition( CPoint( 0, GetScrollPosition().y + sizePage.cy ) );
			break;

		case VK_HOME:
			if (m_FrameContext->m_PacketCount != 0) {
				if (controlKeyDown)
					m_FrameContext->SetCurrentPacket( 0 );
				else
					ScrollToPosition( CPoint( 0, 0 ) );
			}
			break;

		case VK_END:
			if (m_FrameContext->m_PacketCount != 0) {
				if (controlKeyDown)
					m_FrameContext->SetCurrentPacket( m_FrameContext->m_PacketCount - 1 );
				else
					ScrollToPosition( CPoint( 0, m_FrameContext->m_PacketCount * m_cyScreen ) );
			}
			break;

		default:
			CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}

void CSummaryView::OnCancelMode() 
{
	CScrollView::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CSummaryView::OnSetFocus(CWnd* pOldWnd)
{
	TRACE0( "CSummaryView::OnSetFocus()\n" );
	CScrollLineView::OnSetFocus(pOldWnd);
}

void CSummaryView::OnKillFocus(CWnd* pNewWnd)
{
	TRACE0( "CSummaryView::OnKillFocus()\n" );
	CScrollLineView::OnKillFocus(pNewWnd);
}

//CChildFrame* CSummaryView::GetFrame()
//{
//	CWnd*	parent = GetParent();
//
//	while (parent) {
//		if (parent->IsKindOf(RUNTIME_CLASS(CChildFrame)))
//			return (CChildFrame*)parent;
//		parent = parent->GetParent();
//	}
//
//	// bad news, this view is not a child of CChildFrame
//	return NULL;


void CSummaryView::ContextChange( CFrameContext::Signal s )
{
	switch (s) {
		case CFrameContext::eNewPacketCount:
			SetLineCount( m_FrameContext->m_PacketCount );
			break;
		case CFrameContext::eNewCurrentPacket:
			SetSelectedLine( m_FrameContext->m_CurrentPacket );
			break;
	}
}

//
//	CSummaryView::GetLineData
//
//	This function is called to get the ASCII description of the packet to 
//	be displayed in the summary view.  Note that it cannot rely on the 
//	current packet, because it is independant of the current packet!
//


CString* CSummaryView::GetLineData(int lineNo)
{
	CString*	pString = new CString()
	;
	char		theTime[16]
	;

	//MAD_DB	VTSPacket	pkt;
	VTSPacketPtr	ppkt = m_FrameContext->m_pDoc->GetPacket(lineNo);

	if ( ppkt == NULL || pString == NULL )
		return NULL;

	// format the packet number
	pString->Format( "%5d ", lineNo );
	
	// read the packet, the frame context holds the information
//MAD_DB	m_FrameContext->m_pDoc->m_pDB->ReadPacket( lineNo, pkt );

	// format the time
	FILETIME	locFileTime
	;
    SYSTEMTIME	st
	;

//MAD_DB	::FileTimeToLocalFileTime( &pkt.packetHdr.packetTime, &locFileTime );
	::FileTimeToLocalFileTime( &(ppkt->packetHdr.packetTime), &locFileTime );
	::FileTimeToSystemTime( &locFileTime, &st );

	sprintf( theTime, "%02d:%02d:%02d.%03d "
		, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
		);
	*pString += theTime;

#if 1
	int			i, len = 9
	;
	char		nameBuff[32], addrBuff[32], *addr
	;
	const char	*name
	;

	// look up the source
//MAD_DB	name = m_FrameContext->m_pDoc->m_Names.AddrToName( pkt.packetHdr.packetSource, pkt.packetHdr.packetPortID );
	name = m_FrameContext->m_pDoc->AddrToName( ppkt->packetHdr.packetSource, ppkt->packetHdr.m_szPortName );

	if (name)
		sprintf( nameBuff, "%-*.*s", len+1, len, name );
	else {
		addrBuff[0] = 0;
		addr = addrBuff;

/* MAD_DB
		for (i = 0; i < pkt.packetHdr.packetSource.addrLen; i++) {
			sprintf( addr, "%02X", pkt.packetHdr.packetSource.addrAddr[i] );
			addr += 2;
		}
*/
		for (i = 0; i < ppkt->packetHdr.packetSource.addrLen; i++)
		{
			sprintf( addr, "%02X", ppkt->packetHdr.packetSource.addrAddr[i] );
			addr += 2;
		}

		sprintf( nameBuff, "%-*.*s", len+1, len, addrBuff );
	}
	*pString += nameBuff;

	// look up the destination
//MAD_DB	name = m_FrameContext->m_pDoc->m_Names.AddrToName( pkt.packetHdr.packetDestination, pkt.packetHdr.packetPortID );
	name = m_FrameContext->m_pDoc->AddrToName( ppkt->packetHdr.packetDestination, ppkt->packetHdr.m_szPortName );

	if (name)
		sprintf( nameBuff, "%-*.*s", len+1, len, name );
	else {
		addrBuff[0] = 0;
		addr = addrBuff;

/* MAD_DB
		for (i = 0; i < pkt.packetHdr.packetDestination.addrLen; i++) {
			sprintf( addr, "%02X", pkt.packetHdr.packetDestination.addrAddr[i] );
			addr += 2;
		}
*/
		for (i = 0; i < ppkt->packetHdr.packetDestination.addrLen; i++)
		{
			sprintf( addr, "%02X", ppkt->packetHdr.packetDestination.addrAddr[i] );
			addr += 2;
		}

		sprintf( nameBuff, "%-*.*s", len+1, len, addrBuff );
	}
	*pString += nameBuff;
#endif

#if 1
	// format the summary line
	BACnetPIInfo	summary( true, false );

/*MAD_DB
	summary.Interpret( (BACnetPIInfo::ProtocolType)pkt.packetHdr.packetProtocolID
		, (char *)pkt.packetData
		, pkt.packetLen
		);
*/

	summary.Interpret( (BACnetPIInfo::ProtocolType) ppkt->packetHdr.packetProtocolID, (char *) ppkt->packetData, ppkt->packetLen );

	*pString += summary.summaryLine;
#endif

	return pString;
}

BOOL CSummaryView::CreateView(CWnd* pParent,CCreateContext* pContext,RECT rect)
{
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW;
    // dwStyle &= ~WS_BORDER;

    // Create with the right size (wrong position)
    CRect rect1(0,0,300,300);

    if (!Create(NULL, NULL, dwStyle,
        rect1, pParent, AFX_IDW_PANE_FIRST,pContext))
    {
        TRACE0("Warning: couldn't create treeview pane!. \n");
        return FALSE;
    }

	return TRUE;
}

void CSummaryView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd*	parent = GetParentFrame();

	if(! ((CChildFrame*)parent)->m_pwndDetailViewBar->IsVisible())
			((CChildFrame*)parent)->ShowControlBar( ((CChildFrame*)parent)->m_pwndDetailViewBar, TRUE, FALSE);

	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CSummaryView::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CScrollView::OnCaptureChanged(pWnd);
}
