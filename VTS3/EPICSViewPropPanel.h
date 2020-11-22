#if !defined(AFX_EPICSVIEWPROPPANEL_H__181D8C8B_4487_426B_A121_8CEF39B2329A__INCLUDED_)
#define AFX_EPICSVIEWPROPPANEL_H__181D8C8B_4487_426B_A121_8CEF39B2329A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EPICSViewPropPanel.h : header file
//

#include <afxcview.h>

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewPropPanel view

class CEPICSViewNodeObject;

class CEPICSViewPropPanel : public CListView
{
protected:
	CEPICSViewPropPanel();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEPICSViewPropPanel)

	CSend * m_pSend;
	HWND m_hwndSend;
	bool m_fSendReuse;

// Attributes
public:
	CEPICSViewNodeObject * m_pObjectNode;

	void Reset( CEPICSViewNodeObject * pObjectNode, LPCSTR lpszColumnText );
	void SetColumnText( int nCol, LPCSTR lpszText );

private:
	POSITION m_curpos;

	BOOL IsPortSelected(void);
	int GetFirstSelectedIndex(void);
	int GetNextSelectedIndex(void);
	CSend * InvokeSendWindow( LPCSTR lpszAction, int * pSelectedIndex );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEPICSViewPropPanel)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEPICSViewPropPanel();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CEPICSViewPropPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnReadProperty();
	afx_msg void OnWriteProperty();
	afx_msg void OnReadPropertyMultiple();
	afx_msg void OnWritePropertyMultiple();
	afx_msg void OnReadAllProperty();
	afx_msg void OnDeviceValueClear();
	afx_msg void OnCSendReuse();
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EPICSVIEWPROPPANEL_H__181D8C8B_4487_426B_A121_8CEF39B2329A__INCLUDED_)
