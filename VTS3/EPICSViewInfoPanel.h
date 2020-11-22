#if !defined(AFX_EPICSVIEWINFOPANEL_H__BC3848AD_A732_4CFC_8AB8_7EA337121943__INCLUDED_)
#define AFX_EPICSVIEWINFOPANEL_H__BC3848AD_A732_4CFC_8AB8_7EA337121943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EPICSViewInfoPanel.h : header file
//

#include <afxrich.h>
#include <afxcview.h>

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewInfoPanel window

class CEPICSViewInfoPanel : public CRichEditView
{
// Construction
public:
	CEPICSViewInfoPanel();
	DECLARE_DYNCREATE(CEPICSViewInfoPanel)

// Attributes
public:


// Operations
public:

	void ResetPanel();
	void AddText(LPCSTR lpsz);
	void AddLine(LPCSTR lpsz);
	void SetParagraphStyle(WORD wNumbering, LONG lIndent, LONG lOffset, WORD wAlignment);
	void SetCharStyle(int nPoints, COLORREF clr = 0xFFFFFFFF );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEPICSViewInfoPanel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEPICSViewInfoPanel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEPICSViewInfoPanel)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CEPICSViewListPanel view

class CEPICSViewListPanel : public CListView
{
protected:
	CEPICSViewListPanel();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEPICSViewListPanel)

// Attributes
public:
	void Reset(void);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEPICSViewListPanel)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEPICSViewListPanel();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CEPICSViewListPanel)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EPICSVIEWINFOPANEL_H__BC3848AD_A732_4CFC_8AB8_7EA337121943__INCLUDED_)
