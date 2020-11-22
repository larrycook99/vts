#if !defined(AFX_SCRIPTLOADRESULTS_H__39A08271_272A_11D5_BF54_00A0C95A9812__INCLUDED_)
#define AFX_SCRIPTLOADRESULTS_H__39A08271_272A_11D5_BF54_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptLoadResults.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScriptLoadResults dialog

class ScriptLoadResults : public CDialog
{
// Construction
public:
	ScriptLoadResults(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ScriptLoadResults)
	enum { IDD = IDD_SCRIPTLOADRESULTS };
	CString	m_VendorName;
	CString	m_ProductName;
	CString	m_ProductModelNumber;
	CString	m_ProductDescription;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScriptLoadResults)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScriptLoadResults)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTLOADRESULTS_H__39A08271_272A_11D5_BF54_00A0C95A9812__INCLUDED_)
