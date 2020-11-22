#if !defined(AFX_VTSOBJECTTYPEDLG_H__EF7F1C24_4A8B_4E1F_A6A8_CEC1CAD76C85__INCLUDED_)
#define AFX_VTSOBJECTTYPEDLG_H__EF7F1C24_4A8B_4E1F_A6A8_CEC1CAD76C85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSObjectTypeDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSObjectTypeDlg dialog

class VTSObjectTypeDlg : public CDialog
{
// Construction
public:
	VTSObjectTypeDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSObjectTypeDlg)
	enum { IDD = IDD_OBJECTTYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSObjectTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSObjectTypeDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSOBJECTTYPEDLG_H__EF7F1C24_4A8B_4E1F_A6A8_CEC1CAD76C85__INCLUDED_)
