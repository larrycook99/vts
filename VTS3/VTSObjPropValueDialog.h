#if !defined(AFX_VTSOBJPROPVALUEDIALOG_H__67017BA1_E785_11D6_A613_00A0C9E370F1__INCLUDED_)
#define AFX_VTSOBJPROPVALUEDIALOG_H__67017BA1_E785_11D6_A613_00A0C9E370F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSObjPropValueDialog.h : header file
//

#include "VTSValue.h"

/////////////////////////////////////////////////////////////////////////////
// VTSObjPropValueDialog dialog

class VTSObjPropValueDialog : public CDialog
{
// Construction
public:
	VTSObjPropValueDialog(VTSObjPropValueListPtr lp);

	VTSValue			m_dValue;				// access to data
	int					m_iSelectedComp;		// index of selected component

	void SynchronizeControls( void );

	void SetSelection( int indx );
	void ResetSelection( void );

	void EncodeValue( void );
	void DecodeValue( void );

	void CompToList( int indx );				// match list to component

// Dialog Data
	//{{AFX_DATA(VTSObjPropValueDialog)
	enum { IDD = IDD_OBJPROPVALUE };
	CButton	m_ValueIDButton;
	CListCtrl	m_CompList;
	CComboBox	m_TypeCombo;
	CString	m_Value;
	CString	m_Context;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSObjPropValueDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSObjPropValueDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddComp();
	afx_msg void OnRemoveComp();
	afx_msg void OnValueIDButton();
	afx_msg void OnItemChangingCompList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeContext();
	afx_msg void OnSelchangeTypeCombo();
	afx_msg void OnChangeValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSOBJPROPVALUEDIALOG_H__67017BA1_E785_11D6_A613_00A0C9E370F1__INCLUDED_)
