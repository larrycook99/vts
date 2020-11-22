#if !defined(AFX_VTSBINARYPVDLG_H__757DC29C_BB45_4236_8EDD_042256DE099C__INCLUDED_)
#define AFX_VTSBINARYPVDLG_H__757DC29C_BB45_4236_8EDD_042256DE099C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSBinaryPVDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSBinaryPVDlg dialog

class VTSBinaryPVDlg : public CDialog
{
// Construction
public:
	VTSBinaryPVDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_BinaryPVmbo;
// Dialog Data
	//{{AFX_DATA(VTSBinaryPVDlg)
	enum { IDD = IDD_BINARYPV };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSBinaryPVDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSBinaryPVDlg)
	afx_msg void OnSelchangeBinarypvcombo();
	afx_msg void OnDropdownBinarypvcombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSBINARYPVDLG_H__757DC29C_BB45_4236_8EDD_042256DE099C__INCLUDED_)
