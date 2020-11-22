#include "afxwin.h"
#if !defined(AFX_VTSVENDORPROPIDDLG_H__8E8E5927_C06B_48DB_912B_5A7F2CC3B73B__INCLUDED_)
#define AFX_VTSVENDORPROPIDDLG_H__8E8E5927_C06B_48DB_912B_5A7F2CC3B73B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSVendorPropIDDlg.h : header file
//

///////////////////////////////////////////////////////////////////////
//Added by Zhu Zhenhua 2003-7-22, Input Dlg for Vendor Property


/////////////////////////////////////////////////////////////////////////////
// VTSVendorPropIDDlg dialog


class VTSVendorPropIDDlg : public CDialog
{
// Construction
public:
	int m_PropID;
	VTSVendorPropIDDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(VTSVendorPropIDDlg)
	enum { IDD = IDD_VENDORPROPID };
	int		m_nEditPropID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSVendorPropIDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSVendorPropIDDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_captionStatic;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSVENDORPROPIDDLG_H__8E8E5927_C06B_48DB_912B_5A7F2CC3B73B__INCLUDED_)
