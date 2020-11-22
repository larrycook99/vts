#if !defined(AFX_VTSPOLARITYDLG_H__369129E5_63AF_40B6_ADE0_E422BF0785B2__INCLUDED_)
#define AFX_VTSPOLARITYDLG_H__369129E5_63AF_40B6_ADE0_E422BF0785B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPolarityDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSPolarityDlg dialog

class VTSPolarityDlg : public CDialog
{
// Construction
public:
	VTSPolarityDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSPolarityDlg)
	enum { IDD = IDD_POLARITY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPolarityDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPolarityDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPOLARITYDLG_H__369129E5_63AF_40B6_ADE0_E422BF0785B2__INCLUDED_)
