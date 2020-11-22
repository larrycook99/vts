#if !defined(AFX_MSTP_NBLINKDLG_H__E7F2FFBF_6CB8_4E00_8021_D1423F556C77__INCLUDED_)
#define AFX_MSTP_NBLINKDLG_H__E7F2FFBF_6CB8_4E00_8021_D1423F556C77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MSTP_NBLinkDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMSTP_NBLinkDlg dialog

class CMSTP_NBLinkDlg : public CDialog
{
// Construction
public:
	CMSTP_NBLinkDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMSTP_NBLinkDlg)
	enum { IDD = IDD_MSTPIMP_NBLINK };
	CEdit	m_ctrlEditDisclaimer;
	CIPAddressCtrl	m_ctrlIPAddrGateway;
	CIPAddressCtrl	m_ctrlSubnetMask;
	CIPAddressCtrl	m_ctrlIPAddrReset;
	CIPAddressCtrl	m_ctrlIPAddr;
	int		m_nLocalPort;
	BOOL	m_fUseReset;
	int		m_nListenPort;
	BOOL	m_fInitOnStartup;
	//}}AFX_DATA

	CString * m_pstrConfig;
	CString m_strIPAddr;
	CString m_strIPMask;
	CString m_strIPGateway;
	CFont   m_fontFineprint;

	MSTPImp_NBLink * m_pnblink;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSTP_NBLinkDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMSTP_NBLinkDlg)
	afx_msg void OnReset();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnNblinkreset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void GetIPFromCtrl( CIPAddressCtrl * pIPCtrl, CString * pstr );
	void LoadIPCtrl( CIPAddressCtrl * pIPCtrl, CString * pstr );

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSTP_NBLINKDLG_H__E7F2FFBF_6CB8_4E00_8021_D1423F556C77__INCLUDED_)
