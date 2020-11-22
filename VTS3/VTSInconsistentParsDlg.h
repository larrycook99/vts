// VTSInconsistentParsDlg.h: interface for the VTSInconsistentParsDlg class.
// Lori Tribble - 10/17/2009
//////////////////////////////////////////////////////////////////////

#include "afxwin.h"
#if !defined(AFX_VTSINCONSISTENTPARSDLG_H__82C2A51F_AD40_42AD_B7BB_336533F2279E__INCLUDED_)
#define AFX_VTSINCONSISTENTPARSDLG_H__82C2A51F_AD40_42AD_B7BB_336533F2279E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSInconsistentParsDlg.h : header file
//
#include "VTSCtrl.h"

class VTSNames;
class VTSPorts;
/////////////////////////////////////////////////////////////////////////////
// VTSInconsistentParsDlg dialog

class VTSInconsistentParsDlg : public CDialog
{
public:
	const VTSNames& m_names;
	const VTSPorts& m_ports;
// Construction
public:
	VTSInconsistentParsDlg(const VTSNames& names, const VTSPorts& ports, CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(VTSInconsistentParsDlg)
	enum { IDD = IDD_INCONSISTENT_PARS };
	CButton	m_funCtrl;
	CComboBox	m_portCtrl;
	CComboBox	m_deviceCtrl;
	int		m_nFunction;
	CString	m_strPort;
	CString	m_strDevice;
	//CString	m_strDevObjInst;
	//}}AFX_DATA

	VTSObjectIdentifierCtrl					  m_ObjectID;
	VTSEnumeratedCtrl						  m_propCombo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSInconsistentParsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSInconsistentParsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelendokDevicecombo();
	afx_msg void OnSelendokPortcombo();
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnChangeObjectid();
	afx_msg void OnObjectidbtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSINCONSISTENTPARSDLG_H__82C2A51F_AD40_42AD_B7BB_336533F2279E__INCLUDED_)
