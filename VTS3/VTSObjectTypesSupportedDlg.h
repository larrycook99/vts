#if !defined(AFX_VTSOBJECTTYPESSUPPORTEDDLG_H__1E954EF0_AE8A_4190_BD83_750ADE56701C__INCLUDED_)
#define AFX_VTSOBJECTTYPESSUPPORTEDDLG_H__1E954EF0_AE8A_4190_BD83_750ADE56701C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSObjectTypesSupportedDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSObjectTypesSupportedDlg dialog

class VTSObjectTypesSupportedDlg : public CDialog
{
// Construction
public:
	VTSObjectTypesSupportedDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
// Dialog Data
	//{{AFX_DATA(VTSObjectTypesSupportedDlg)
	enum { IDD = IDD_OBJECTTYPESSUPPORTED };
	BOOL	m_aicheck;
	BOOL	m_aocheck;
	BOOL	m_avcheck;
	BOOL	m_avgcheck;
	BOOL	m_bicheck;
	BOOL	m_bocheck;
	BOOL	m_bvcheck;
	BOOL	m_cacheck;
	BOOL	m_cocheck;
	BOOL	m_decheck;
	BOOL	m_evcheck;
	BOOL	m_ficheck;
	BOOL	m_grcheck;
	BOOL	m_locheck;
	BOOL	m_lspcheck;
	BOOL	m_lszcheck;
	BOOL	m_msicheck;
	BOOL	m_msocheck;
	BOOL	m_msvcheck;
	BOOL	m_ntcheck;
	BOOL	m_prcheck;
	BOOL	m_sccheck;
	BOOL	m_trcheck;
	BOOL	m_SelectAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSObjectTypesSupportedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSObjectTypesSupportedDlg)
	afx_msg void OnSelectall();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSOBJECTTYPESSUPPORTEDDLG_H__1E954EF0_AE8A_4190_BD83_750ADE56701C__INCLUDED_)
