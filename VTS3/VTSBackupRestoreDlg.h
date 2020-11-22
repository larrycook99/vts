// VTSBakRestoreDlg.h: interface for the VTSBakRestoreDlg class.
// Jingbo Gao, Sep 20 2004
//////////////////////////////////////////////////////////////////////

#include "afxwin.h"
#if !defined(AFX_VTSBACKUPRESTOREDLG_H__82C2A51F_AD40_42AD_B7BB_336533F2279E__INCLUDED_)
#define AFX_VTSBACKUPRESTOREDLG_H__82C2A51F_AD40_42AD_B7BB_336533F2279E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSBackupRestoreDlg.h : header file
//

class VTSNames;
class VTSPorts;
/////////////////////////////////////////////////////////////////////////////
// VTSBackupRestoreDlg dialog

class VTSBackupRestoreDlg : public CDialog
{
public:
	const VTSNames& m_names;
	const VTSPorts& m_ports;
// Construction
public:
	VTSBackupRestoreDlg(const VTSNames& names, const VTSPorts& ports, CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(VTSBackupRestoreDlg)
	enum { IDD = IDD_BACKUP_RESTORE };
	CButton	m_funCtrl;
	CComboBox	m_portCtrl;
	CComboBox	m_deviceCtrl;
	int		m_nFunction;
  CEdit   m_Delay;
  CEdit	  m_backup_timeout;
	CString	m_strBackupFileName;
	CString	m_strDevice;
	CString	m_strPassword;
	CString	m_strPort;
	CString	m_strDevObjInst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSBackupRestoreDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSBackupRestoreDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	virtual void OnOK();
	afx_msg void OnSelendokDevicecombo();
	afx_msg void OnSelendokPortcombo();
  afx_msg void OnEnChangeDelay();
  afx_msg void OnEnChangeTimeout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  UINT Delay;
  UINT timeout;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSBACKUPRESTOREDLG_H__82C2A51F_AD40_42AD_B7BB_336533F2279E__INCLUDED_)
