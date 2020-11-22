#if !defined(AFX_GOTOLINEDLG_H__1E70EE77_0640_45A7_9A95_0AEB03339F57__INCLUDED_)
#define AFX_GOTOLINEDLG_H__1E70EE77_0640_45A7_9A95_0AEB03339F57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GoToLineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGoToLineDlg dialog

class CGoToLineDlg : public CDialog
{
// Construction
public:
	CGoToLineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGoToLineDlg)
	enum { IDD = IDD_GOTOLINE };
	int		m_nLineIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoToLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGoToLineDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTOLINEDLG_H__1E70EE77_0640_45A7_9A95_0AEB03339F57__INCLUDED_)
