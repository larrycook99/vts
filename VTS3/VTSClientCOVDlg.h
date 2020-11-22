#if !defined(AFX_VTSCLIENTCOVDLG_H__08344996_AAA6_424F_AB4B_4BC15DEBBA83__INCLUDED_)
#define AFX_VTSCLIENTCOVDLG_H__08344996_AAA6_424F_AB4B_4BC15DEBBA83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSClientCOVDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSClientCOVDlg dialog

class VTSClientCOVDlg : public CDialog
{
// Construction
public:
	VTSClientCOVDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSRealCtrl	 m_realCtrl;
// Dialog Data
	//{{AFX_DATA(VTSClientCOVDlg)
	enum { IDD = IDD_CLIENTCOV };
	int		m_covchoice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSClientCOVDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSClientCOVDlg)
	afx_msg void OnNullvalue();
	afx_msg void OnRealvalue();
	afx_msg void OnChangeRealincr();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSCLIENTCOVDLG_H__08344996_AAA6_424F_AB4B_4BC15DEBBA83__INCLUDED_)
