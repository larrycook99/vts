#if !defined(AFX_SENDREADBDT_H__AA88AB52_473B_11D4_BEB4_00A0C95A9812__INCLUDED_)
#define AFX_SENDREADBDT_H__AA88AB52_473B_11D4_BEB4_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadBDT.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendReadBDT dialog

class CSendReadBDT : public CSendPage
{
	DECLARE_DYNCREATE(CSendReadBDT)

// Construction
public:
	CSendReadBDT( void );   // non-standard constructor

	void EncodePage( CByteArray* contents );	// encode the page

// Dialog Data
	//{{AFX_DATA(CSendReadBDT)
	enum { IDD = IDD_SENDREADBDT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendReadBDT)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendReadBDT)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADBDT_H__AA88AB52_473B_11D4_BEB4_00A0C95A9812__INCLUDED_)
