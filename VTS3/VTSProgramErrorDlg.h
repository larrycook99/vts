#if !defined(AFX_VTSPROGRAMERRORDLG_H__0B87A244_C8F2_4D10_A67C_38EFA3B7B356__INCLUDED_)
#define AFX_VTSPROGRAMERRORDLG_H__0B87A244_C8F2_4D10_A67C_38EFA3B7B356__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSProgramErrorDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSProgramErrorDlg dialog

class VTSProgramErrorDlg : public CDialog
{
// Construction
public:
	VTSProgramErrorDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;	
// Dialog Data
	//{{AFX_DATA(VTSProgramErrorDlg)
	enum { IDD = IDD_PROGRAMERROR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSProgramErrorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSProgramErrorDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPROGRAMERRORDLG_H__0B87A244_C8F2_4D10_A67C_38EFA3B7B356__INCLUDED_)
