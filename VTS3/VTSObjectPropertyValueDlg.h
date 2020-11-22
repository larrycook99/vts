#if !defined(AFX_VTSOBJECTPROPERTYVALUEDLG_H__97AACE66_3D65_4FC8_8D3F_938AB55D173D__INCLUDED_)
#define AFX_VTSOBJECTPROPERTYVALUEDLG_H__97AACE66_3D65_4FC8_8D3F_938AB55D173D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSObjectPropertyValueDlg.h : header file
//
#include "VTSAny.h"
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSObjectPropertyValueDlg dialog

class VTSObjectPropertyValueDlg : public CDialog
{
// Construction
public:
	VTSObjectPropertyValueDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
		VTSObjectIdentifierCtrl					  m_ObjectID;
		VTSEnumeratedCtrl						  m_propCombo;
		VTSUnsignedCtrl							  m_ArrayIndex;
		VTSAny									  m_Value;
		VTSUnsignedCtrl							  m_priority;
// Dialog Data
	//{{AFX_DATA(VTSObjectPropertyValueDlg)
	enum { IDD = IDD_OBJECTPROPERTYVALUE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSObjectPropertyValueDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSObjectPropertyValueDlg)
	afx_msg void OnChangePropvaluePriority();
	afx_msg void OnValue();
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnObjectidbtn();
	afx_msg void OnChangeObjectid();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeArrayindex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSOBJECTPROPERTYVALUEDLG_H__97AACE66_3D65_4FC8_8D3F_938AB55D173D__INCLUDED_)
