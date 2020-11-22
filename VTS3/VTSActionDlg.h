#if !defined(AFX_VTSACTIONDLG_H__44E31DF7_8693_460D_914F_617E4F8BC192__INCLUDED_)
#define AFX_VTSACTIONDLG_H__44E31DF7_8693_460D_914F_617E4F8BC192__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSActionDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSActionDlg dialog

class VTSActionDlg : public CDialog
{
// Construction
public:
	VTSActionDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_actcombo;
// Dialog Data
	//{{AFX_DATA(VTSActionDlg)
	enum { IDD = IDD_ACTION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSActionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSActionDlg)
	afx_msg void OnDropdownActioncombo();
	afx_msg void OnSelchangeActioncombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSACTIONDLG_H__44E31DF7_8693_460D_914F_617E4F8BC192__INCLUDED_)
