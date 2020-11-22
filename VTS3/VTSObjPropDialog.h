#if !defined(AFX_VTSOBJPROPDIALOG_H__D24A3E81_CDFC_11D6_A613_00A0C9E370F1__INCLUDED_)
#define AFX_VTSOBJPROPDIALOG_H__D24A3E81_CDFC_11D6_A613_00A0C9E370F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSObjPropDialog.h : header file
//

#include "VTSValue.h"

/////////////////////////////////////////////////////////////////////////////
// VTSObjPropDialog dialog

class VTSObjPropDialog : public CDialog
{
// Construction
public:
	VTSObjPropDialog( VTSObjPropValueListPtr opvlp );

	VTSObjPropValueListPtr	m_OPVL;				// this is where everything is stored
	VTSObjectList			m_dObjectList;		// object list access
	VTSObjectPropList		m_dPropList;		// property list access

	int						m_iSelectedObj;		// index of selected object
	int						m_iSelectedProp;	// index of selected property

	void SetObjSelection( int indx );			// change the selected object
	void ResetObjSelection( void );

	void SetPropSelection( int indx );			// change the selection property
	void ResetPropSelection( void );

// Dialog Data
	//{{AFX_DATA(VTSObjPropDialog)
	enum { IDD = IDD_OBJPROPLIST };
	CListCtrl	m_ObjectList;
	CString		m_ObjectID;
	CListCtrl	m_PropList;
	CComboBox	m_PropCombo;
	CString	m_PropID;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSObjPropDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSObjPropDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddObject();
	afx_msg void OnRemoveObject();
	afx_msg void OnObjectIDButton();
	afx_msg void OnAddProperty();
	afx_msg void OnRemoveProperty();
	afx_msg void OnValue();
	afx_msg void OnArray();
	afx_msg void OnValueToArray();
	afx_msg void OnArrayToValue();
	afx_msg void OnChangeObjectID();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnItemChangingObjectList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangingPropList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangePropID();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSOBJPROPDIALOG_H__D24A3E81_CDFC_11D6_A613_00A0C9E370F1__INCLUDED_)
