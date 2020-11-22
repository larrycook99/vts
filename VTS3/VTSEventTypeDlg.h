#if !defined(AFX_VTSEVENTTYPEDLG_H__5EEE3F41_5228_4DC2_92ED_BE7EC3066FDF__INCLUDED_)
#define AFX_VTSEVENTTYPEDLG_H__5EEE3F41_5228_4DC2_92ED_BE7EC3066FDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventTypeDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventTypeDlg dialog

class VTSEventTypeDlg : public CDialog
{
// Construction
public:
	VTSEventTypeDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSEventTypeDlg)
	enum { IDD = IDD_EVENTTYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventTypeDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTTYPEDLG_H__5EEE3F41_5228_4DC2_92ED_BE7EC3066FDF__INCLUDED_)
