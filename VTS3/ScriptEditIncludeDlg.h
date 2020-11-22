#if !defined(AFX_SCRIPTEDITINCLUDEDLG_H__E0A831CC_0DCD_41D6_A854_3CF103FED885__INCLUDED_)
#define AFX_SCRIPTEDITINCLUDEDLG_H__E0A831CC_0DCD_41D6_A854_3CF103FED885__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScriptBase.h"


// ScriptEditIncludeDlg.h : header file
//


class CIncludeDoc : public CDocument
{
	DECLARE_DYNCREATE(CIncludeDoc)
	void Serialize(CArchive& ar);
	//{{AFX_MSG(CIncludeDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CScriptEditIncludeDlg dialog

class CScriptEditIncludeDlg : public CDialog
{
private:
	CEditView *		m_pEdit;
	CIncludeDoc *	m_pdocinclude;
	CString			m_strFileName;
	ScriptToken *	m_ptok;
	int				m_nLine;

	RECT			m_rClient, m_rText, m_rOK, m_rView;

// Construction
public:
	CScriptEditIncludeDlg( LPCSTR lpszFileName, ScriptToken * ptok, LPCSTR lpszErrorMsg, int nLine = -1, CWnd * pParent = NULL );
	~CScriptEditIncludeDlg();

// Dialog Data
	//{{AFX_DATA(CScriptEditIncludeDlg)
	enum { IDD = IDD_SCRIPTINCLUDE };
	CString	m_strError;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptEditIncludeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScriptEditIncludeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTEDITINCLUDEDLG_H__E0A831CC_0DCD_41D6_A854_3CF103FED885__INCLUDED_)
