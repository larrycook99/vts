#if !defined(AFX_HEXVIEW_H__5408F5F3_B832_11D3_BE53_00A0C95A9812__INCLUDED_)
#define AFX_HEXVIEW_H__5408F5F3_B832_11D3_BE53_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HexView.h : header file
//

#include "VTSDoc.h"
#include "FrameContext.h"
#include "ScrollLineView.h"

/////////////////////////////////////////////////////////////////////////////
// CHexView view

class CHexView : public CScrollLineView, public CFrameContextListener
{
protected:
	CHexView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHexView)

// Attributes
public:

// Operations
public:
	CWnd*		m_pTabRing;
	int			m_SelectedStart;
	int			m_SelectedLen;

	VTSDoc*		GetDocument();

	virtual void ContextChange( CFrameContext::Signal s );
	BOOL CreateView(CWnd* pParent,CCreateContext* pContext,RECT rect);

	CString* GetLineData(int lineNo);
	void SetSelectedRange( int start, int len );
	virtual void InvalidateVisibleSelection( void );
	virtual void DrawLine( CDC* pDC, int lineNo, CRect &r );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHexView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHexView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CHexView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in HexView.cpp
inline VTSDoc* CHexView::GetDocument()
   { return (VTSDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEXVIEW_H__5408F5F3_B832_11D3_BE53_00A0C95A9812__INCLUDED_)
