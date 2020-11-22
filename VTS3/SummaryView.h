#if !defined(AFX_SUMMARYVIEW_H__5408F5F1_B832_11D3_BE53_00A0C95A9812__INCLUDED_)
#define AFX_SUMMARYVIEW_H__5408F5F1_B832_11D3_BE53_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SummaryView.h : header file

This file is obsolete - replaced by CListSummaryView


#include "VTSDoc.h"
#include "FrameContext.h"
#include "ScrollLineView.h"

/////////////////////////////////////////////////////////////////////////////
// CSummaryView view

class CSummaryView : public CScrollLineView, public CFrameContextListener
{
protected:
	CSummaryView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSummaryView)

// Attributes
public:

// Operations
public:
	BOOL CreateView(CWnd* pParent,CCreateContext* pContext,RECT rect);
	CString* GetLineData(int lineNo);
	CWnd* m_pTabRing;
	//CChildFrame* GetFrame( void );
	VTSDoc* GetDocument();
	
	virtual void ContextChange( CFrameContext::Signal s );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSummaryView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSummaryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CSummaryView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCancelMode();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in HexView.cpp
inline VTSDoc* CSummaryView::GetDocument()
   { return (VTSDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUMMARYVIEW_H__5408F5F1_B832_11D3_BE53_00A0C95A9812__INCLUDED_)
