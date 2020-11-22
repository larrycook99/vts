#if !defined(AFX_VTSPORTIPDIALOG_H__9CB63CA1_8D00_11D4_BEDD_00A0C95A9812__INCLUDED_)
#define AFX_VTSPORTIPDIALOG_H__9CB63CA1_8D00_11D4_BEDD_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPortIPDialog.h : header file
//

#include "VTSPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSPortIPDialog dialog

class VTSPortIPDialog : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSPortIPDialog)

	CString		* m_pstrConfigParms;

// Construction
public:
	VTSPortIPDialog( VTSPageCallbackInterface * pparent );
	VTSPortIPDialog();
	~VTSPortIPDialog();

// Dialog Data
	//{{AFX_DATA(VTSPortIPDialog)
	enum { IDD = IDD_PORTPAGE_IP };
	CComboBox m_interfaceCombo;
	CString	m_UDP_Port;
	CString	m_HostAddr;
	CString	m_TTL;
	int		m_nPortType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPortIPDialog)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPortIPDialog)
	afx_msg void OnDataChange();
	afx_msg void OnBTRPeers();
	afx_msg void OnBBMDPeers();
	//}}AFX_MSG

	void ObjToCtrl(void);
	void CtrlToObj(void);
	void SynchControls( int fEnable = TRUE );

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPORTIPDIALOG_H__9CB63CA1_8D00_11D4_BEDD_00A0C95A9812__INCLUDED_)
