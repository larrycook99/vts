#if !defined(AFX_VTSPROPERTYVALUEDLG_H__F1552FBF_AC0B_462D_8FDE_E8B0FDFAA29F__INCLUDED_)
#define AFX_VTSPROPERTYVALUEDLG_H__F1552FBF_AC0B_462D_8FDE_E8B0FDFAA29F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPropertyValueDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSAny.h"
/////////////////////////////////////////////////////////////////////////////
// VTSPropertyValueDlg dialog

class VTSPropertyValueDlg : public CDialog
{
// Construction
public:
	VTSPropertyValueDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
		VTSEnumeratedCtrl						  m_propCombo;
		VTSUnsignedCtrl							  m_ArrayIndex;
		VTSAny									  m_Value;
		VTSUnsignedCtrl							  m_priority;
// Dialog Data
	//{{AFX_DATA(VTSPropertyValueDlg)
	enum { IDD = IDD_PROPERTYVALUE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPropertyValueDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPropertyValueDlg)
	afx_msg void OnValue();
	afx_msg void OnChangePropvaluePriority();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnChangeArrayindex();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPROPERTYVALUEDLG_H__F1552FBF_AC0B_462D_8FDE_E8B0FDFAA29F__INCLUDED_)
