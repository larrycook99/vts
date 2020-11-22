// ScrollLineView.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "ScrollLineView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScrollLineView

IMPLEMENT_DYNCREATE(CScrollLineView, CScrollView)

CScrollLineView::CScrollLineView()
	: m_bScrollSizesSet(0)
	, m_LineCount(0), m_SelectedLine(-1)
	, m_HasFocus(false)
{
}

CScrollLineView::~CScrollLineView()
{
}


BEGIN_MESSAGE_MAP(CScrollLineView, CScrollView)
	//{{AFX_MSG_MAP(CScrollLineView)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollLineView drawing

void CScrollLineView::OnInitialUpdate()
{
	// let the scroll view get itself ready
	CScrollView::OnInitialUpdate();
	
	if (!m_bScrollSizesSet) {
		m_bScrollSizesSet = true;

		// initialize the scroll sizes
		SetScrollSizes( MM_TEXT, CSize( 0, 0 ) );
		ScrollToPosition( CPoint( 0, 0 ) );
	}
}

void CScrollLineView::OnDraw(CDC* pDC)
{
	if (m_LineCount != 0) {
		CRect viewRect, clipRect;
		
		GetClientRect( &viewRect );
		pDC->GetClipBox( &clipRect );
		
		int nStart = clipRect.top / m_cyScreen;
		int nEnd = min( m_LineCount - 1, (clipRect.bottom + m_cyScreen - 1) / m_cyScreen );

		CFont* pOldFont = pDC->SelectObject( &m_fontScreen );
		for (int i=nStart; i<=nEnd; i++) {
			CRect	r( 0, (i * m_cyScreen), viewRect.right, (i + 1) * m_cyScreen );
#if 0
			if ((i % 3) == 0) {
				static int x = 189;

				CBrush	br( RGB( x, x, x ) );
				pDC->SelectObject( br );
				pDC->SetBkColor( RGB( x, x, x ) );
				pDC->Rectangle( r );
			} else
				pDC->SetBkColor( RGB( 255, 255, 255 ) );
#endif
			DrawLine( pDC, i, r );
		}
		pDC->SelectObject( pOldFont );
	}
}

void CScrollLineView::DrawLine( CDC* pDC, int lineNo, CRect &r )
{
	// get the line data and draw it
	CString* pString = GetLineData( lineNo );
	pDC->TextOut( 2, r.top, *pString );
	delete pString;

	// if the line is selected, highlight it somehow
	if (lineNo == m_SelectedLine) {
		if (m_HasFocus) {
			pDC->InvertRect( &r );
		} else {
			CBrush	brush( RGB( 255, 0, 0 ) );
			pDC->FrameRect( &r, &brush );
		}
	}
}

CString* CScrollLineView::GetLineData(int lineNo)
{
	CString* pString = new CString();
	pString->Format( "%04X", lineNo );
	
	return pString;
}

/////////////////////////////////////////////////////////////////////////////
// CScrollLineView diagnostics

#ifdef _DEBUG
void CScrollLineView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CScrollLineView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScrollLineView message handlers

int CScrollLineView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//
	// Create a screen font.
	//
	m_fontScreen.CreatePointFont( 90, _T("Courier New") );
	
	//
	// Compute the height of one line in the screen font.
	//
	CClientDC	dc(this);
	TEXTMETRIC	tm;
	CFont		*pOldFont = dc.SelectObject( &m_fontScreen );
	
	dc.GetTextMetrics( &tm );
	m_cxScreen = tm.tmAveCharWidth;
	m_cyScreen = tm.tmHeight + tm.tmExternalLeading;
	dc.SelectObject( pOldFont );
	
	return 0;
}

void CScrollLineView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);
	
	m_HasFocus = true;
	InvalidateVisibleSelection();
}

void CScrollLineView::OnKillFocus(CWnd* pNewWnd) 
{
	CScrollView::OnKillFocus(pNewWnd);
	
	m_HasFocus = false;
	InvalidateVisibleSelection();
}

void CScrollLineView::ResetView()
{
	CRect		viewRect;
	
	// get the visible area
	GetClientRect( &viewRect );
	
	// invalidate it
	InvalidateRect( &viewRect );

	// scroll to the top
	ScrollToPosition( CPoint( 0, 0 ) );
}

void CScrollLineView::InvalidateVisibleSelection()
{
	CRect		viewRect, selRect, clipRect;
	
	// make sure something is selected
	if (m_SelectedLine < 0)
		return;
	
	// get the visible area
	GetClientRect( &viewRect );
	
	// offset it to compensate for scrolling
	viewRect.OffsetRect( 0, GetScrollPosition().y );
	
	// build a rect representing the selection
	selRect.left = 0;
	selRect.top = m_SelectedLine * m_cyScreen;
	selRect.right = viewRect.right;
	selRect.bottom = selRect.top + m_cyScreen;
	
	// see if the two intersect
	if (clipRect.IntersectRect( &viewRect, &selRect )) {
		// move the clipRect back to client coordinates
		clipRect.OffsetRect( 0, - GetScrollPosition().y );
		InvalidateRect( &clipRect );
	}
}

int CScrollLineView::GetLineCount()
{
	return m_LineCount;
}

void CScrollLineView::SetLineCount(int lineCount)
{
	//	Check for initialized scroll bars.  If the map mode hasn't been set, 
	//	the app isn't far enough along in the construction of a new frame/view.
	int		nMapMode = 0;
	SIZE	sizeTotal, sizePage, sizeLine;

	//
	//	If the scroll bars haven't been set up correctly yet, do it now.
	//	Depending on when the line count is being set, the view might 
	//	not be completely initalized.  When a document is being opened, 
	//	the document object gets created first, then the views, then the 
	//	document is initalized, then the views.
	//
	if (!m_bScrollSizesSet) {
		m_bScrollSizesSet = true;

		SetScrollSizes( MM_TEXT, CSize( 0, 0 ) );
		ScrollToPosition( CPoint( 0, 0 ) );
	}

	GetDeviceScrollSizes( nMapMode, sizeTotal, sizePage, sizeLine );
	if (nMapMode == 0)
		return;
	
	// don't bother if the line count isn't changing
	if (m_LineCount == lineCount)
		return;
	
	int			scrollAmount = GetScrollPosition().y
	;
	CRect		viewRect		// visible area
	,			newSelRect		// new area
	,			newClipRect		// visible portion of new area
	;
	
	// get the visible area
	GetClientRect( &viewRect );
	
	// build a rect representing the new area
	newSelRect.left = 0;
	newSelRect.right = viewRect.right;
	if (lineCount < m_LineCount) {
		newSelRect.top = lineCount * m_cyScreen - scrollAmount;
		newSelRect.bottom = newSelRect.top + (m_LineCount - lineCount) * m_cyScreen;
	} else {
		newSelRect.top = m_LineCount * m_cyScreen - scrollAmount;
		newSelRect.bottom = newSelRect.top + (lineCount - m_LineCount) * m_cyScreen;
	}

	// if this is a visible area, invalidate it
	if (newClipRect.IntersectRect( &viewRect, &newSelRect ))
		InvalidateRect( &newClipRect );
	
	// set the new size
	m_LineCount = lineCount;

	// re-calculate the total size of this view
	SetScrollSizes( MM_TEXT, CSize( 0, m_LineCount * m_cyScreen ),
		CSize( 0, m_cyScreen * 10 ), CSize( 0, m_cyScreen ) );
}

int CScrollLineView::GetSelectedLine()
{
	return m_SelectedLine;
}

void CScrollLineView::SetSelectedLine(int lineNo)
{
	if (lineNo == m_SelectedLine)
		return;

	CRect		viewRect, oldSelRect, newSelRect, oldClipRect, newClipRect;
	BOOL		oldSelVisible, newSelVisible;
	int			scrollAmount = GetScrollPosition().y;
	int			scrollDelta;

	// get the visible area
	GetClientRect( &viewRect );

	// build a rect representing the old selection
	if (m_SelectedLine == -1)
		oldSelVisible = false;
	else {
		oldSelRect.left = 0;
		oldSelRect.top = m_SelectedLine * m_cyScreen - scrollAmount;
		oldSelRect.right = viewRect.right;
		oldSelRect.bottom = oldSelRect.top + m_cyScreen;
		oldSelVisible = oldClipRect.IntersectRect( &viewRect, &oldSelRect );
	}

	// test for going to no selection
	if (lineNo == -1) {
		// if it was visible, invalidate it
		if (oldSelVisible)
			InvalidateRect( &oldClipRect );
		m_SelectedLine = lineNo;
		return;
	}

	// build a rect representing the new selection
	newSelRect.left = 0;
	newSelRect.top = lineNo * m_cyScreen - scrollAmount;
	newSelRect.right = viewRect.right;
	newSelRect.bottom = newSelRect.top + m_cyScreen;
	newSelVisible = newClipRect.IntersectRect( &viewRect, &newSelRect );

	// check for needed scrolling
	if (newSelRect.top < viewRect.top) {
		ScrollToPosition( CPoint( 0, lineNo * m_cyScreen ) );
		scrollDelta = viewRect.top - newSelRect.top;
	} else
	if (newSelRect.bottom > viewRect.bottom) {
		ScrollToPosition( CPoint( 0, lineNo * m_cyScreen - viewRect.Height() + m_cyScreen ) );
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
	
	// set the new selected line
	m_SelectedLine = lineNo;
}

int CScrollLineView::GetLineAtPoint( CPoint point ) 
{
//	TRACE2( "Point at (%d,%d)\n", point.x, point.y );
	
	CPoint		pos = point;
    CClientDC	dc( this );
    OnPrepareDC( &dc );
    dc.DPtoLP( &pos );
	
//	TRACE2( "Really at (%d,%d)\n", pos.x, pos.y );
	
	pos.x = (pos.x / m_cxScreen);
	pos.y = (pos.y / m_cyScreen);
	
//	TRACE2( "Char/Line (%d,%d)\n", pos.x, pos.y );
	
	return pos.y;
}
