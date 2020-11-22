#if !defined(AFX_VTSPORTETHERNETDIALOG_H__9CB63CA2_8D00_11D4_BEDD_00A0C95A9812__INCLUDED_)
#define AFX_VTSPORTETHERNETDIALOG_H__9CB63CA2_8D00_11D4_BEDD_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPortEthernetDialog.h : header file
//

#include "VTSPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSPortEthernetDialog dialog

class VTSPortEthernetDialog : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSPortEthernetDialog)

	CString		* m_pstrConfigParms;

	// Construction
public:
	VTSPortEthernetDialog( VTSPageCallbackInterface * pparent );
	VTSPortEthernetDialog();
	~VTSPortEthernetDialog();

// Dialog Data
	//{{AFX_DATA(VTSPortEthernetDialog)
	enum { IDD = IDD_PORTPAGE_ENET };
	CString		m_strAdapter;
	BOOL		m_fPromiscuous;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPortEthernetDialog)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPortEthernetDialog)
	afx_msg void OnDataChange();
	//}}AFX_MSG

	void ObjToCtrl(void);
	void CtrlToObj(void);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPORTETHERNETDIALOG_H__9CB63CA2_8D00_11D4_BEDD_00A0C95A9812__INCLUDED_)
