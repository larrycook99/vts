#if !defined(AFX_SCROLLLINEVIEW_H__4372D3E1_C19F_11D3_BE58_00A0C95A9812__INCLUDED_)
#define AFX_SCROLLLINEVIEW_H__4372D3E1_C19F_11D3_BE58_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScrollLineView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScrollLineView view

class CScrollLineView : public CScrollView
{
protected:
	CScrollLineView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScrollLineView)

// Attributes
public:

// Operations
public:
	int GetLineCount( void );
	void SetLineCount( int lineCount );

	int GetSelectedLine( void );
	void SetSelectedLine( int lineNo );
	
	void ResetView( void );
	virtual void InvalidateVisibleSelection( void );

	bool	m_HasFocus;
	CFont	m_fontScreen;
	int		m_cyScreen;
	int		m_cxScreen;
	int		m_SelectedLine;
	int		m_LineCount;
	
	virtual CString* GetLineData( int lineNo );
	virtual void DrawLine( CDC* pDC, int lineNo, CRect &r );

	int		GetLineAtPoint( CPoint point );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrollLineView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScrollLineView();

	bool	m_bScrollSizesSet;			// iff scroll sizes set

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CScrollLineView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCROLLLINEVIEW_H__4372D3E1_C19F_11D3_BE58_00A0C95A9812__INCLUDED_)
