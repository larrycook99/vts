#if !defined(AFX_VTSOBJECTIDENTIFIERDIALOG_H__E84D5121_DCA7_11D4_BF18_00A0C95A9812__INCLUDED_)
#define AFX_VTSOBJECTIDENTIFIERDIALOG_H__E84D5121_DCA7_11D4_BF18_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSObjectIdentifierDialog.h : header file
//

#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// VTSObjectIdentifierDialog dialog

class VTSObjectIdentifierDialog : public CDialog
{
// Construction
public:
	VTSObjectIdentifierDialog(CWnd* pParent = NULL);   // standard constructor

	unsigned int			objID;							// result
	bool					validObjID;						// valid form and values

	int						objType;
	int						instance;

	void					UpdateEncoding( bool valid );	// display nice text

// Dialog Data
	//{{AFX_DATA(VTSObjectIdentifierDialog)
	enum { IDD = IDD_OBJID };
	CEdit		m_Vendor;
	CEdit		m_Reserved;
	CEdit		m_Instance;
	CComboBox	m_ObjTypeCombo;
	CString		m_Encoded;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSObjectIdentifierDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSObjectIdentifierDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeObjTypeCombo();
	afx_msg void OnChangeReserved();
	afx_msg void OnChangeVendor();
	afx_msg void OnChangeInstance();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSOBJECTIDENTIFIERDIALOG_H__E84D5121_DCA7_11D4_BF18_00A0C95A9812__INCLUDED_)
