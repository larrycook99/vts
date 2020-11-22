#if !defined(AFX_VTSOBJECTPROPERTYREFERENCEDLG_H__D66E3B50_88E9_4E92_9DCC_3E3DE3008B87__INCLUDED_)
#define AFX_VTSOBJECTPROPERTYREFERENCEDLG_H__D66E3B50_88E9_4E92_9DCC_3E3DE3008B87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSObjectPropertyReferenceDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSObjectPropertyReferenceDlg dialog

class VTSObjectPropertyReferenceDlg : public CDialog
{
// Construction
public:
	VTSObjectPropertyReferenceDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
		VTSObjectIdentifierCtrl					  m_ObjectID;
		VTSEnumeratedCtrl						  m_propCombo;
		VTSUnsignedCtrl							  m_ArrayIndex;
// Dialog Data
	//{{AFX_DATA(VTSObjectPropertyReferenceDlg)
	enum { IDD = IDD_OBJECTPROPERTYREFERENCE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSObjectPropertyReferenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSObjectPropertyReferenceDlg)
	afx_msg void OnChangeObjectid();
	afx_msg void OnObjectidbtn();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnChangeArrayindex();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSOBJECTPROPERTYREFERENCEDLG_H__D66E3B50_88E9_4E92_9DCC_3E3DE3008B87__INCLUDED_)
