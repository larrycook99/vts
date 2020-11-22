// HexView.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "HexView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHexView

IMPLEMENT_DYNCREATE(CHexView, CScrollView)

CHexView::CHexView()
{
}

CHexView::~CHexView()
{
}

BEGIN_MESSAGE_MAP(CHexView, CScrollView)
	//{{AFX_MSG_MAP(CHexView)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHexView drawing

void CHexView::OnInitialUpdate()
{
	CScrollLineView::OnInitialUpdate();
}

void CHexView::OnDraw(CDC* pDC)
{
	CScrollLineView::OnDraw( pDC );
}

void CHexView::SetSelectedRange( int start, int len )
{
	if ((start == m_SelectedStart) && (len == m_SelectedLen))
		return;

	CRect		viewRect, oldSelRect, newSelRect, oldClipRect, newClipRect;
	BOOL		oldSelVisible, newSelVisible;
	int			scrollAmount = GetScrollPosition().y;
	int			scrollDelta;
	int			lineStart, lineCount;

	// get the visible area
	GetClientRect( &viewRect );

	// build a rect representing the old selection
	lineStart = m_SelectedStart / 16;
	lineCount = (m_SelectedStart + m_SelectedLen - 1) / 16 - lineStart + 1;

	if (m_SelectedLen == 0)
		oldSelVisible = false;
	else {
		oldSelRect.left = 0;
		oldSelRect.top = lineStart * m_cyScreen - scrollAmount;
		oldSelRect.right = viewRect.right;
		oldSelRect.bottom = oldSelRect.top + lineCount * m_cyScreen;
		oldSelVisible = oldClipRect.IntersectRect( &viewRect, &oldSelRect );
	}

	// test for going to no selection
	if (len == 0) {
		// if it was visible, invalidate it
		if (oldSelVisible)
			InvalidateRect( &oldClipRect );
		m_SelectedStart = m_SelectedLen = 0;
		return;
	}

	// build a rect representing the new selection
	lineStart = start / 16;
	lineCount = (start + len) / 16 - lineStart + 1;

	newSelRect.left = 0;
	newSelRect.top = lineStart * m_cyScreen - scrollAmount;
	newSelRect.right = viewRect.right;
	newSelRect.bottom = newSelRect.top + lineCount * m_cyScreen;
	newSelVisible = newClipRect.IntersectRect( &viewRect, &newSelRect );

	// check for needed scrolling
	if (newSelRect.top < viewRect.top) {
		ScrollToPosition( CPoint( 0, lineStart * m_cyScreen ) );
		scrollDelta = viewRect.top - newSelRect.top;
	} else
	if (newSelRect.bottom > viewRect.bottom) {
		ScrollToPosition( CPoint( 0, lineStart * m_cyScreen - viewRect.Height() + lineCount * m_cyScreen ) );
		scrollDelta = viewRect.bottom - newSelRect.bottom;
	} else {
		// no scrolling necessary, new selection is visible
		if (oldSelVisible)
			InvalidateRect( &oldClipRect );
		InvalidateRect( &newSelRect );
		oldSelVisible = newSelVisible = false;
	}
	
	// if any portion of the old selection was visible, it might still be, make 
	// sure it gets erased.
	if (oldSelVisible) {
		oldClipRect.OffsetRect( 0, scrollDelta );
		InvalidateRect( &oldClipRect );
	}

	// if any portion of the new selection was visible, it still needs to
	// be re-drawn.
	if (newSelVisible) {
		newClipRect.OffsetRect( 0, scrollDelta );
		InvalidateRect( &newClipRect );
	}
	
	// set the new selected info
	m_SelectedStart = start;
	m_SelectedLen = len;
}

void CHexView::InvalidateVisibleSelection( void )
{
	int			lineStart, lineCount;
	CRect		viewRect, selRect, clipRect;
	
	// get the visible area
	GetClientRect( &viewRect );
	
	// offset it to compensate for scrolling
	viewRect.OffsetRect( 0, GetScrollPosition().y );
	
	// build a rect representing the selection
	lineStart = m_SelectedStart / 16;
	lineCount = (m_SelectedStart + m_SelectedLen - 1) / 16 - lineStart + 1;

	selRect.left = 0;
	selRect.top = lineStart * m_cyScreen;
	selRect.right = viewRect.right;
	selRect.bottom = selRect.top + lineCount * m_cyScreen;
	
	// see if the two intersect
	if (clipRect.IntersectRect( &viewRect, &selRect )) {
		// move the clipRect back to client coordinates
		clipRect.OffsetRect( 0, - GetScrollPosition().y );
		InvalidateRect( &clipRect );
	}
}

void CHexView::DrawLine( CDC* pDC, int lineNo, CRect &r )
{
	CScrollLineView::DrawLine( pDC, lineNo, r );

	if (m_SelectedLen != 0) {
		int lineStart = m_SelectedStart / 16;
		int lineCount = (m_SelectedStart + m_SelectedLen - 1) / 16 - lineStart + 1;

		// can we skip this line?
		if ((lineNo < lineStart) || (lineNo >= (lineStart+lineCount)))
			return;

		int charStart = m_SelectedStart;
		int charEnd = charStart + m_SelectedLen;

		// clip the front
		if (charStart < (lineNo * 16))
			charStart = lineNo * 16;

		// clip the end
		if (charEnd > ((lineNo + 1) * 16))
			charEnd = (lineNo + 1) * 16;

		// normalize to the current line
		charStart -= lineNo * 16;
		charEnd -= lineNo * 16;

		CRect drawRect;

		// calc rect for part 1
		CRect r1( (6 + charStart * 3) * m_cxScreen, r.top
				, (6 + charEnd * 3) * m_cxScreen, r.bottom
				);
		if (drawRect.IntersectRect( &r, &r1 ))
			if (m_HasFocus) {
				pDC->InvertRect( &r1 );
			} else {
				CBrush	brush( RGB( 255, 0, 0 ) );
				pDC->FrameRect( &drawRect, &brush );
			}

		// calc rect for part 2
		CRect r2( (55 + charStart) * m_cxScreen, r.top
				, (55 + charEnd) * m_cxScreen + 4, r.bottom
				);
		if (drawRect.IntersectRect( &r, &r2 ))
			if (m_HasFocus) {
				pDC->InvertRect( &r2 );
			} else {
				CBrush	brush( RGB( 255, 0, 0 ) );
				pDC->FrameRect( &drawRect, &brush );
			}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHexView diagnostics

#ifdef _DEBUG
void CHexView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CHexView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

VTSDoc* CHexView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(VTSDoc)));
	return (VTSDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHexView message handlers

int CHexView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollLineView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetContext( gNewFrameContext );
	
	return 0;
}

void CHexView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TRACE1( "CHexView::OnChar( %d, ...)\n", nChar );
	if ((nChar == 0x09) && (m_pTabRing))
		m_pTabRing->SetFocus();
}

void CHexView::ContextChange( CFrameContext::Signal s )
{
	switch (s) {
		case CFrameContext::eNewCurrentPacket:
			if (m_FrameContext->m_CurrentPacket == -1)
				SetLineCount( 0 );
			else
//MAD_DB		SetLineCount( (m_FrameContext->m_Packet.packetLen + 15) / 16 );
				SetLineCount( (m_FrameContext->GetCurrentPacket()->packetLen + 15) / 16 );
			m_SelectedStart = m_SelectedLen = 0;
			ResetView();
			break;
		case CFrameContext::eNewCurrentRange:
			SetSelectedRange( m_FrameContext->m_HexStart, m_FrameContext->m_HexLen );
			break;
	}
}

void CHexView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	bool	shiftKeyDown	= (GetKeyState(VK_SHIFT) < 0);
	bool	controlKeyDown	= (GetKeyState(VK_CONTROL) < 0);

	int		nMapMode;
	SIZE	sizeTotal, sizePage, sizeLine;

	GetDeviceScrollSizes( nMapMode, sizeTotal, sizePage, sizeLine );

	switch (nChar) {
		case VK_UP:
			ScrollToPosition( CPoint( 0, GetScrollPosition().y - sizeLine.cy ) );
			break;

		case VK_DOWN:
			ScrollToPosition( CPoint( 0, GetScrollPosition().y + sizeLine.cy ) );
			break;

		case VK_PRIOR:
			ScrollToPosition( CPoint( 0, GetScrollPosition().y - sizePage.cy ) );
			break;

		case VK_NEXT:
			ScrollToPosition( CPoint( 0, GetScrollPosition().y + sizePage.cy ) );
			break;

		case VK_HOME:
			ScrollToPosition( CPoint( 0, 0 ) );
			break;

		case VK_END:
			ScrollToPosition( CPoint( 0, sizeTotal.cy ) );
			break;

		default:
			CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}

void CHexView::OnSetFocus(CWnd* pOldWnd) 
{
	TRACE0( "CHexView::OnSetFocus ->\n" );
	CScrollLineView::OnSetFocus(pOldWnd);
}

void CHexView::OnKillFocus(CWnd* pNewWnd) 
{
	TRACE0( "CHexView::OnKillFocus ->\n" );
	CScrollLineView::OnKillFocus(pNewWnd);
}

CString* CHexView::GetLineData(int lineNo)
{
	CString *pString = new CString();
	if (m_FrameContext->GetCurrentPacket() != NULL)
	{
		// figure out how many bytes are available
		int bCount = m_FrameContext->GetCurrentPacket()->packetLen - (lineNo * 16);
		if (bCount > 16) bCount = 16;

		if (bCount > 0)
		{
		    BYTE b[16];
			memcpy( b, m_FrameContext->GetCurrentPacket()->packetData + (lineNo * 16), bCount );

			// Output is 3 char per hex value, two spaces, characters, then null
			char buf[16*3 + 2 + 16 + 1];
			char *pStr = buf;
			int ix;
			for (ix = 0; ix < 16; ix++)
			{
				if (ix < bCount)
				{
					pStr += sprintf( pStr, " %02X", b[ix] );
				}
				else
				{
					pStr += sprintf( pStr, "   " );
				}
			}

			pStr += sprintf( pStr, "  " );
			for (ix = 0; ix < bCount; ix++)
			{
				*pStr++ = ((b[ix] >= ' ') && (b[ix] <= '~')) ? b[ix] : '.';
			}
			
			*pStr = 0;
			
			pString->Format( _T("%0.4X %s"), lineNo * 16, buf );
		}
	}
	
	return pString;
}

BOOL CHexView::CreateView(CWnd *pParent, CCreateContext *pContext, RECT rect)
{
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW;
    // dwStyle &= ~WS_BORDER;

    // Create with the right size (wrong position)
    CRect rect1(0,0,500,300);

    if (!Create(NULL, NULL, dwStyle,
        rect1, pParent, AFX_IDW_PANE_FIRST,pContext))
    {
        TRACE0("Warning: couldn't create treeview pane!. \n");
        return FALSE;
    }

	return TRUE;
}
