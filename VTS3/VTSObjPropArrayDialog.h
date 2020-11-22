#if !defined(AFX_VTSOBJPROPARRAYDIALOG_H__92DAC6F1_EC49_11D6_A613_00A0C9E370F1__INCLUDED_)
#define AFX_VTSOBJPROPARRAYDIALOG_H__92DAC6F1_EC49_11D6_A613_00A0C9E370F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSObjPropArrayDialog.h : header file
//

#include "VTSValue.h"

/////////////////////////////////////////////////////////////////////////////
// VTSObjPropArrayDialog dialog

class VTSObjPropArrayDialog : public CDialog
{
// Construction
public:
	VTSObjPropArrayDialog(VTSObjPropValueListPtr lp);

	VTSObjPropValueListPtr	m_OPVL;				// this is where everything is stored
	VTSValueArray			m_dArray;			// access to data
	int						m_iSelectedElem;	// index of selected element

	void SetSelection( int indx );
	void ResetSelection( void );

// Dialog Data
	//{{AFX_DATA(VTSObjPropArrayDialog)
	enum { IDD = IDD_OBJPROPARRAY };
	CListCtrl	m_ElemList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSObjPropArrayDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSObjPropArrayDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddElem();
	afx_msg void OnRemoveElem();
	afx_msg void OnItemchangingElemList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSOBJPROPARRAYDIALOG_H__92DAC6F1_EC49_11D6_A613_00A0C9E370F1__INCLUDED_)
