#if !defined(AFX_SENDWHOISRTN_H__060822B2_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_)
#define AFX_SENDWHOISRTN_H__060822B2_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendWhoIsRTN.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendWhoIsRTN dialog

class CSendWhoIsRTN : public CSendPage
{
	DECLARE_DYNCREATE(CSendWhoIsRTN)

// Construction
public:
	CSendWhoIsRTN( void );   // non-standard constructor

	VTSIntegerCtrl	m_DNET;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendWhoIsRTN)
	enum { IDD = IDD_SENDWHOISRTN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendWhoIsRTN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendWhoIsRTN)
	afx_msg void OnChangeDNET();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDWHOISRTN_H__060822B2_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_)
