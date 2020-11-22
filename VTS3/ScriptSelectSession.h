#if !defined(AFX_SCRIPTSELECTSESSION_H__18B81821_BE98_11D4_BEF8_00A0C95A9812__INCLUDED_)
#define AFX_SCRIPTSELECTSESSION_H__18B81821_BE98_11D4_BEF8_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptSelectSession.h : header file
//

#include "VTSDoc.h"

/////////////////////////////////////////////////////////////////////////////
// ScriptSelectSession dialog

class ScriptSelectSession : public CDialog
{
// Construction
public:
	ScriptSelectSession(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ScriptSelectSession)
	enum { IDD = IDD_SELECT_SESSION };
	CButton	m_OK;
	CListCtrl	m_DocList;
	//}}AFX_DATA

	VTSDocPtr		m_SelectedDoc;						// selected document

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScriptSelectSession)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScriptSelectSession)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemChangingDocList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTSELECTSESSION_H__18B81821_BE98_11D4_BEF8_00A0C95A9812__INCLUDED_)
