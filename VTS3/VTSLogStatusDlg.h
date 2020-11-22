#if !defined(AFX_VTSLOGSTATUSDLG_H__BBCEE331_F00B_4F2F_908D_47F8CD764DED__INCLUDED_)
#define AFX_VTSLOGSTATUSDLG_H__BBCEE331_F00B_4F2F_908D_47F8CD764DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSLogStatusDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSLogStatusDlg dialog

class VTSLogStatusDlg : public CDialog
{
// Construction
public:
	VTSLogStatusDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode( BACnetAPDUDecoder& dec );
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	VTSBooleanCtrl   m_logdisabled;
	VTSBooleanCtrl   m_bufferpurged;
// Dialog Data
	//{{AFX_DATA(VTSLogStatusDlg)
	enum { IDD = IDD_LOGSTATUS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSLogStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSLogStatusDlg)
	afx_msg void OnBufferpurged();
	afx_msg void OnLogdisabled();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLOGSTATUSDLG_H__BBCEE331_F00B_4F2F_908D_47F8CD764DED__INCLUDED_)
