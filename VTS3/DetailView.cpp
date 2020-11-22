// DetailView.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "ChildFrm.h"
#include "DetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDetailView

IMPLEMENT_DYNCREATE(CDetailView, CScrollView)

CDetailView::CDetailView()
{
}

CDetailView::~CDetailView()
{
}


BEGIN_MESSAGE_MAP(CDetailView, CScrollView)
	//{{AFX_MSG_MAP(CDetailView)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetailView drawing

void CDetailView::OnInitialUpdate()
{
	CScrollLineView::OnInitialUpdate();
}

void CDetailView::OnDraw(CDC* pDC)
{
	CScrollLineView::OnDraw( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CDetailView diagnostics

#ifdef _DEBUG
void CDetailView::AssertValid() const
{
	CScrollLineView::AssertValid();
}

void CDetailView::Dump(CDumpContext& dc) const
{
	CScrollLineView::Dump(dc);
}

VTSDoc* CDetailView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(VTSDoc)));
	return (VTSDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDetailView message handlers

int CDetailView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollLineView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetContext( gNewFrameContext );
	
	return 0;
}

void CDetailView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
/*
	VTSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	switch ((VTSDoc::Signal)lHint) {
		case VTSDoc::eInitialUpdate:
			break;
	}
*/
	CScrollView::OnUpdate( pSender, lHint, pHint );
}

void CDetailView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TRACE1( "CDetailView::OnChar( %d, ...)\n", nChar );
	if ((nChar == 0x09) && (m_pTabRing))
		m_pTabRing->SetFocus();
}

void CDetailView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollLineView::OnSetFocus(pOldWnd);
}

void CDetailView::OnKillFocus(CWnd* pNewWnd) 
{
	CScrollLineView::OnKillFocus(pNewWnd);
}

void CDetailView::ContextChange( CFrameContext::Signal s )
{
	switch (s) {
		case CFrameContext::eNewCurrentPacket:
			m_FrameContext->SetCurrentDetail( -1 );
			if (m_FrameContext->m_CurrentPacket == -1)
				m_FrameContext->SetDetailCount( 0 );
			else
				m_FrameContext->SetDetailCount( m_FrameContext->m_PacketInfo.detailCount );
			ResetView();
			break;
			
		case CFrameContext::eNewDetailCount:
			SetLineCount( m_FrameContext->m_DetailCount );
			break;
			
		case CFrameContext::eNewCurrentDetail:
			SetSelectedLine( m_FrameContext->m_CurrentDetail );
			break;
	}
}

CString* CDetailView::GetLineData(int lineNo)
{
	CString*	pString = new CString( m_FrameContext->m_PacketInfo.detailLine[lineNo]->piLine )
	;
	
	return pString;
}

void CDetailView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int	lineNo = GetLineAtPoint( point );
	m_FrameContext->SetCurrentDetail( lineNo );
}

void CDetailView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	bool	shiftKeyDown	= (GetKeyState(VK_SHIFT) < 0);
	bool	controlKeyDown	= (GetKeyState(VK_CONTROL) < 0);

	int		nMapMode;
	SIZE	sizeTotal, sizePage, sizeLine;

	GetDeviceScrollSizes( nMapMode, sizeTotal, sizePage, sizeLine );

	switch (nChar) {
		case VK_UP:
			if ((m_FrameContext->m_DetailCount != 0) && (m_FrameContext->m_CurrentDetail > 0))
				m_FrameContext->SetCurrentDetail( m_FrameContext->m_CurrentDetail - 1);
			break;

		case VK_DOWN:
			if ((m_FrameContext->m_DetailCount != 0) && (m_FrameContext->m_CurrentDetail < (m_FrameContext->m_DetailCount - 1)))
				m_FrameContext->SetCurrentDetail( m_FrameContext->m_CurrentDetail + 1);
			break;

		case VK_PRIOR:
			if (m_FrameContext->m_DetailCount != 0)
				ScrollToPosition( CPoint( 0, GetScrollPosition().y - sizePage.cy ) );
			break;

		case VK_NEXT:
			if (m_FrameContext->m_DetailCount != 0)
				ScrollToPosition( CPoint( 0, GetScrollPosition().y + sizePage.cy ) );
			break;

		case VK_HOME:
			if (m_FrameContext->m_DetailCount != 0) {
				if (controlKeyDown)
					m_FrameContext->SetCurrentDetail( 0 );
				else
					ScrollToPosition( CPoint( 0, 0 ) );
			}
			break;

		case VK_END:
			if (m_FrameContext->m_DetailCount != 0) {
				if (controlKeyDown)
					m_FrameContext->SetCurrentDetail( m_FrameContext->m_DetailCount - 1 );
				else
					ScrollToPosition( CPoint( 0, m_FrameContext->m_DetailCount * m_cyScreen ) );
			}
			break;

		default:
			CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}

BOOL CDetailView::CreateView(CWnd *pParent, CCreateContext *pContext, RECT rect)
{
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW|WS_HSCROLL;
    // dwStyle &= ~WS_BORDER;

    // Create with the right size (wrong position)
    CRect rect1(0,0,500,400);
	

    if (!Create(NULL, NULL, dwStyle,
        rect1, pParent, AFX_IDW_PANE_FIRST,pContext))
    {
        TRACE0("Warning: couldn't create treeview pane!. \n");
        return FALSE;
    }

	return TRUE;
}

void CDetailView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd*	parent = GetParentFrame();

	if(! ((CChildFrame*)parent)->m_pwndHexViewBar->IsVisible())
			((CChildFrame*)parent)->ShowControlBar( ((CChildFrame*)parent)->m_pwndHexViewBar, TRUE, FALSE);

	CScrollView::OnLButtonDblClk(nFlags, point);
}
