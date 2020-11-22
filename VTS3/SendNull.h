#if !defined(AFX_SENDNULL_H__8707D032_2ADB_11D4_BE9D_00A0C95A9812__INCLUDED_)
#define AFX_SENDNULL_H__8707D032_2ADB_11D4_BE9D_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendNull.h : header file
//

#include "SendPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSendNull dialog

class CSendNull : public CSendPage
{
	DECLARE_DYNCREATE(CSendNull)

// Construction
public:
	CSendNull( void );   // non-standard constructor

// Dialog Data
	//{{AFX_DATA(CSendNull)
	enum { IDD = IDD_SENDNULL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendNull)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendNull)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDNULL_H__8707D032_2ADB_11D4_BE9D_00A0C95A9812__INCLUDED_)
