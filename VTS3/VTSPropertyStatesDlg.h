#if !defined(AFX_VTSPROPERTYSTATESDLG_H__CD0D3586_32B0_463D_85CC_45454919D94F__INCLUDED_)
#define AFX_VTSPROPERTYSTATESDLG_H__CD0D3586_32B0_463D_85CC_45454919D94F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPropertyStatesDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSPropertyStatesDlg dialog

class VTSPropertyStatesDlg : public CDialog
{
// Construction
public:
	VTSPropertyStatesDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	int m_nSelItem;
	VTSUnsignedCtrl  m_unsignedValue;
	VTSBooleanCtrl	 m_boolValue;
	int				 m_enumValue;
// Dialog Data
	//{{AFX_DATA(VTSPropertyStatesDlg)
	enum { IDD = IDD_PROPERTYSTATES };
	CComboBox	m_statesCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPropertyStatesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPropertyStatesDlg)
	afx_msg void OnOnvalue();
	afx_msg void OnChangeUnsignedvalue();
	afx_msg void OnSelchangeChoice();
	afx_msg void OnBoolvalue();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPROPERTYSTATESDLG_H__CD0D3586_32B0_463D_85CC_45454919D94F__INCLUDED_)
