#if !defined(AFX_VTSEVENTSTATEDLG_H__A726882B_88B0_45F0_9789_A1F5E085B3FB__INCLUDED_)
#define AFX_VTSEVENTSTATEDLG_H__A726882B_88B0_45F0_9789_A1F5E085B3FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventStateDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventStateDlg dialog

class VTSEventStateDlg : public CDialog
{
// Construction
public:
	VTSEventStateDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSEventStateDlg)
	enum { IDD = IDD_EVENTSTATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventStateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventStateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeEnumratecombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTSTATEDLG_H__A726882B_88B0_45F0_9789_A1F5E085B3FB__INCLUDED_)
