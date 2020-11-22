#if !defined(AFX_DETAILVIEW_H__5408F5F2_B832_11D3_BE53_00A0C95A9812__INCLUDED_)
#define AFX_DETAILVIEW_H__5408F5F2_B832_11D3_BE53_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DetailView.h : header file
//

#include "VTSDoc.h"
#include "FrameContext.h"
#include "ScrollLineView.h"

/////////////////////////////////////////////////////////////////////////////
// CDetailView view

class CDetailView : public CScrollLineView, public CFrameContextListener
{
protected:
	CDetailView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDetailView)

// Attributes
public:
	int		m_DetailLines;				// number of detail lines

// Operations
public:
	BOOL CreateView(CWnd* pParent,CCreateContext* pContext,RECT rect);
	CWnd* m_pTabRing;
	VTSDoc* GetDocument();

	virtual void ContextChange( CFrameContext::Signal s );

	CString* GetLineData(int lineNo);
	virtual ~CDetailView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDetailView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDetailView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DetailView.cpp
inline VTSDoc* CDetailView::GetDocument()
   { return (VTSDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILVIEW_H__5408F5F2_B832_11D3_BE53_00A0C95A9812__INCLUDED_)
