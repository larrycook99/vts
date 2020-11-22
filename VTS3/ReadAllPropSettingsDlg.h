/*--------------------------------------------------------------------------------
last edit:	03-SEP-03[001] GJB  added the support two keywords: DNET and DADR
-----------------------------------------------------------------------------*/ 
#if !defined(AFX_READALLPROPSETTINGSDLG_H__0E2523E1_24D9_4C5D_9C83_293E1FD40574__INCLUDED_)
#define AFX_READALLPROPSETTINGSDLG_H__0E2523E1_24D9_4C5D_9C83_293E1FD40574__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReadAllPropSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReadAllPropSettingsDlg dialog

class CReadAllPropSettingsDlg : public CDialog
{
// Construction
public:
	CReadAllPropSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReadAllPropSettingsDlg)
	enum { IDD = IDD_READALLPROPSETTING };
	CComboBox	m_ctrlDnetDadr;                 //****001
	CString	m_strIUTIP;
	CString	m_strNetwork;
	BOOL	m_bDNET;							//****001
	BOOL	m_bUseRPM;
	CString	m_strDnetDadr;						//****001
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReadAllPropSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReadAllPropSettingsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDnetpresent();					//****001
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READALLPROPSETTINGSDLG_H__0E2523E1_24D9_4C5D_9C83_293E1FD40574__INCLUDED_)
