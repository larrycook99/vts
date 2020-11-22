#if !defined(AFX_VTSVTCLASSDLG_H__D2E15CCD_035E_4C1E_B2E2_BC1BACD9D79D__INCLUDED_)
#define AFX_VTSVTCLASSDLG_H__D2E15CCD_035E_4C1E_B2E2_BC1BACD9D79D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSVTClassDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSVTClassDlg dialog

class VTSVTClassDlg : public CDialog
{
// Construction
public:
	VTSVTClassDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSVTClassDlg)
	enum { IDD = IDD_VTCLASS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSVTClassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSVTClassDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSVTCLASSDLG_H__D2E15CCD_035E_4C1E_B2E2_BC1BACD9D79D__INCLUDED_)
