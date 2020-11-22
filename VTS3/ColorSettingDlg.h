#if !defined(AFX_COLORSETTINGDLG_H__0786953A_5786_441E_A199_50D0F77C9AA9__INCLUDED_)
#define AFX_COLORSETTINGDLG_H__0786953A_5786_441E_A199_50D0F77C9AA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorSettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorSettingDlg dialog

#define COLOR_PDU_COUNT 6

class CColorSettingDlg : public CDialog
{
// Construction
public:
	CColorSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColorSettingDlg)
	enum { IDD = IDD_COLOR };	
	CComboBoxEx	m_ctrl[COLOR_PDU_COUNT];
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:	
	CImageList m_imageList;
	// Generated message map functions
	//{{AFX_MSG(CColorSettingDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSETTINGDLG_H__0786953A_5786_441E_A199_50D0F77C9AA9__INCLUDED_)
