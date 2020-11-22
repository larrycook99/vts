#if !defined(AFX_VTSEVENTTRANSITIONBITSDLG_H__6A451CA0_A3B6_41CC_A275_34706531C18C__INCLUDED_)
#define AFX_VTSEVENTTRANSITIONBITSDLG_H__6A451CA0_A3B6_41CC_A275_34706531C18C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventTransitionBitsDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventTransitionBitsDlg dialog

class VTSEventTransitionBitsDlg : public CDialog
{
// Construction
public:
	VTSEventTransitionBitsDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode( BACnetAPDUDecoder& dec );
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	VTSBooleanCtrl   m_offNormal;
	VTSBooleanCtrl   m_fault;
	VTSBooleanCtrl   m_normal;
// Dialog Data
	//{{AFX_DATA(VTSEventTransitionBitsDlg)
	enum { IDD = IDD_EVENTTRANSITIONBITS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventTransitionBitsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventTransitionBitsDlg)
	afx_msg void OnFault();
	afx_msg void OnNormal();
	afx_msg void OnOffnormal();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTTRANSITIONBITSDLG_H__6A451CA0_A3B6_41CC_A275_34706531C18C__INCLUDED_)
