// VTSPortMSTPDialog.h: interface for the VTSPortMSTPDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSPORTMSTPDIALOG_H__6339FBE7_7A94_47B2_9497_FC2389E80DB2__INCLUDED_)
#define AFX_VTSPORTMSTPDIALOG_H__6339FBE7_7A94_47B2_9497_FC2389E80DB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VTSPropertyPage.h"


class VTSPortMSTPDialog : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSPortMSTPDialog)

	CString		* m_pstrConfigParms;

public:
	VTSPortMSTPDialog( VTSPageCallbackInterface * pparent );
	VTSPortMSTPDialog();
	~VTSPortMSTPDialog();
	
// Dialog Data
	//{{AFX_DATA(VTSPortMSTPDialog)
	enum { IDD = IDD_PORTPAGE_MSTP };
	int 	m_nMaxMaster;
	int		m_nMaxInfoFrame;
	int 	m_nMAC;
	CString	m_strBaud;
	CString	m_strImpName;
	//}}AFX_DATA

	CString m_strImpParms;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPortMSTPDialog)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPortMSTPDialog)
	afx_msg void OnChangeNblinksocket();
	afx_msg void OnSelchangeMstpimp();
	afx_msg void OnChangeMstpmac();
	afx_msg void OnSelchangeMstpbaud();
	afx_msg void OnChangeMstpmaxinfo();
	afx_msg void OnChangeMstpmaxmaster();
	afx_msg void OnMstpconfigimp();
	//}}AFX_MSG

	void ObjToCtrl(void);
	void CtrlToObj(void);

	DECLARE_MESSAGE_MAP()
};


class VTSPortNullDialog : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSPortNullDialog)

public:
	VTSPortNullDialog( VTSPageCallbackInterface * pparent );
	VTSPortNullDialog() {};
	
// Dialog Data
	//{{AFX_DATA(VTSPortNullDialog)
	enum { IDD = IDD_PORTPAGE_NULL };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPortNullDialog)
	public:
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPortNullDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif // !defined(AFX_VTSPORTMSTPDIALOG_H__6339FBE7_7A94_47B2_9497_FC2389E80DB2__INCLUDED_)
