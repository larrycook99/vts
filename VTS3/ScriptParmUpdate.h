#if !defined(AFX_SCRIPTPARMUPDATE_H__CBE193B6_B97C_11D4_BEF2_00A0C95A9812__INCLUDED_)
#define AFX_SCRIPTPARMUPDATE_H__CBE193B6_B97C_11D4_BEF2_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptParmUpdate.h : header file
//

#include "ScriptParmList.h"

/////////////////////////////////////////////////////////////////////////////
// ScriptParmUpdate dialog

class ScriptParmUpdate : public CDialog
{
// Construction
public:
	ScriptParmUpdate( CWnd* pParent = NULL );	// standard constructor
	ScriptParmUpdate( ScriptParmPtr pp, CWnd * pparent = NULL );		// extended constructor

// Dialog Data
	bool		m_ParmValueOK;
	CString		m_ParmValueNormalized;

	//{{AFX_DATA(ScriptParmUpdate)
	enum { IDD = IDD_SCRIPTPARMUPDATE };
	CString	m_ParmDesc;
	CString	m_ParmName;
	CString	m_ParmScriptValue;
	CString	m_ParmValue;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScriptParmUpdate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScriptParmUpdate)
	afx_msg void OnUpdateParmValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTPARMUPDATE_H__CBE193B6_B97C_11D4_BEF2_00A0C95A9812__INCLUDED_)
