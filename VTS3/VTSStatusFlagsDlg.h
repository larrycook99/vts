#if !defined(AFX_VTSSTATUSFLAGSDLG_H__7BCA5D1A_118B_418E_9099_AC31BE73CE53__INCLUDED_)
#define AFX_VTSSTATUSFLAGSDLG_H__7BCA5D1A_118B_418E_9099_AC31BE73CE53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSStatusFlagsDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSStatusFlagsDlg dialog

class VTSStatusFlagsDlg : public CDialog
{
// Construction
public:
	VTSStatusFlagsDlg(CWnd* pParent = NULL);   // standard constructor
	VTSStatusFlags			m_StatusFlags;
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
// Dialog Data
	//{{AFX_DATA(VTSStatusFlagsDlg)
	enum { IDD = IDD_STATUSFLAGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSStatusFlagsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSStatusFlagsDlg)
	afx_msg void OnOverridden();
	afx_msg void OnOutofservice();
	afx_msg void OnInalarm();
	afx_msg void OnFault();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSSTATUSFLAGSDLG_H__7BCA5D1A_118B_418E_9099_AC31BE73CE53__INCLUDED_)
