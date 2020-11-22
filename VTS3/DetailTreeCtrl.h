#if !defined(AFX_DETAILTREECTRL_H__AB736771_E082_4D43_80E8_597C4231CDFC__INCLUDED_)
#define AFX_DETAILTREECTRL_H__AB736771_E082_4D43_80E8_597C4231CDFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DetailTreeCtrl.h : header file
//

#include "FrameContext.h"
/////////////////////////////////////////////////////////////////////////////
// CDetailTreeCtrl window

class CDetailTreeCtrl : public CTreeCtrl,public CFrameContextListener
{
// Construction
public:
	CDetailTreeCtrl();

	// Attributes
public:
	CWnd* m_pTabRing;

// Operations
public:
	void ContextChange( CFrameContext::Signal s );
	void ShowDetail();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDetailTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDetailTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDetailTreeCtrl)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILTREECTRL_H__AB736771_E082_4D43_80E8_597C4231CDFC__INCLUDED_)
