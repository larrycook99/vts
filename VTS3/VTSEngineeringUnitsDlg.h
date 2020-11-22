#if !defined(AFX_VTSENGINEERINGUNITSDLG_H__64999123_3785_4C36_8A6E_A8EBDA5A0177__INCLUDED_)
#define AFX_VTSENGINEERINGUNITSDLG_H__64999123_3785_4C36_8A6E_A8EBDA5A0177__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEngineeringUnitsDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEngineeringUnitsDlg dialog

class VTSEngineeringUnitsDlg : public CDialog
{
// Construction
public:
	VTSEngineeringUnitsDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSEngineeringUnitsDlg)
	enum { IDD = IDD_ENGIEERINGUNITS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEngineeringUnitsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEngineeringUnitsDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSENGINEERINGUNITSDLG_H__64999123_3785_4C36_8A6E_A8EBDA5A0177__INCLUDED_)
