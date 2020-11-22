#if !defined(AFX_SCRIPTMAKEDLG_H__DE1EA86E_2B9F_4253_AFB2_3F09FC2447C7__INCLUDED_)
#define AFX_SCRIPTMAKEDLG_H__DE1EA86E_2B9F_4253_AFB2_3F09FC2447C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptMakeDlg.h : header file
//

#include "resource.h"
#include "ScriptExecMsg.h"



/////////////////////////////////////////////////////////////////////////////
// CScriptMakeDlg dialog

class CScriptMakeDlg : public CDialog
{
// Construction
public:
	CScriptMakeDlg(bool fMakeDlg, LPCSTR lpszText, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScriptMakeDlg)
	enum { IDDMAKE = IDD_SCRIPTMAKE, IDDCHECK = IDD_SCRIPTCHECK };
	CString	m_strText;
	//}}AFX_DATA

	void DoModeless();
	void SetText(LPCSTR lpszText);
	bool IsUp();
	bool IsSuccess();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptMakeDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int m_nDestroyCode;
	bool m_fModal;

	// Generated message map functions
	//{{AFX_MSG(CScriptMakeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTMAKEDLG_H__DE1EA86E_2B9F_4253_AFB2_3F09FC2447C7__INCLUDED_)
