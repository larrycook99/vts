#if !defined(AFX_VTSLIMITENABLE_H__142420A5_C0EB_46CB_82B9_331264573F49__INCLUDED_)
#define AFX_VTSLIMITENABLE_H__142420A5_C0EB_46CB_82B9_331264573F49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSLimitEnable.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VTSLimitEnable dialog

class VTSLimitEnable : public CDialog
{
// Construction
public:
	VTSLimitEnable(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(VTSLimitEnable)
	enum { IDD = IDD_LIMITENABLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSLimitEnable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSLimitEnable)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLIMITENABLE_H__142420A5_C0EB_46CB_82B9_331264573F49__INCLUDED_)
