#if !defined(AFX_SENDREADFDT_H__8B9C8FD3_515B_11D4_BEBB_00A0C95A9812__INCLUDED_)
#define AFX_SENDREADFDT_H__8B9C8FD3_515B_11D4_BEBB_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadFDT.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendReadFDT dialog

class CSendReadFDT : public CSendPage
{
	DECLARE_DYNCREATE(CSendReadFDT)

// Construction
public:
	CSendReadFDT( void );   // non-standard constructor

	void EncodePage( CByteArray* contents );	// encode the page

// Dialog Data
	//{{AFX_DATA(CSendReadFDT)
	enum { IDD = IDD_SENDREADFDT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendReadFDT)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendReadFDT)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADFDT_H__8B9C8FD3_515B_11D4_BEBB_00A0C95A9812__INCLUDED_)
