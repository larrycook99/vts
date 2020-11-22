#if !defined(AFX_VTSPORTPTPDIALOG_H__BF8F3289_760B_44F0_879F_1FCB264AB6E8__INCLUDED_)
#define AFX_VTSPORTPTPDIALOG_H__BF8F3289_760B_44F0_879F_1FCB264AB6E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPortPTPDialog.h : header file
//

#include "VTSPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSPortPTPDialog dialog

class VTSPortPTPDialog : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSPortPTPDialog)

	CString		* m_pstrConfigParms;

public:
	VTSPortPTPDialog( VTSPageCallbackInterface * pparent );
	VTSPortPTPDialog();
	~VTSPortPTPDialog();
	
// Dialog Data
	//{{AFX_DATA(VTSPortPTPDialog)
	enum { IDD = IDD_PORTPAGE_PTP };
	CString		m_strCOM;
	CString		m_strBaud;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPortPTPDialog)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPortPTPDialog)
	afx_msg void OnDataChange();
	//}}AFX_MSG

	void ObjToCtrl(void);
	void CtrlToObj(void);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPORTPTPDIALOG_H__BF8F3289_760B_44F0_879F_1FCB264AB6E8__INCLUDED_)
