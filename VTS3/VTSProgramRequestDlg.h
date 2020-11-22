#if !defined(AFX_VTSPROGRAMREQUESTDLG_H__A3C9B7B1_8438_4602_9497_6BC2FA687F97__INCLUDED_)
#define AFX_VTSPROGRAMREQUESTDLG_H__A3C9B7B1_8438_4602_9497_6BC2FA687F97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSProgramRequestDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSProgramRequestDlg dialog

class VTSProgramRequestDlg : public CDialog
{
// Construction
public:
	VTSProgramRequestDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;	
// Dialog Data
	//{{AFX_DATA(VTSProgramRequestDlg)
	enum { IDD = IDD_PROGRAMREQUEST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSProgramRequestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSProgramRequestDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPROGRAMREQUESTDLG_H__A3C9B7B1_8438_4602_9497_6BC2FA687F97__INCLUDED_)
