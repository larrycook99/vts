#if !defined(AFX_VTSNOTIFYTYPEDLG_H__3A02879F_2FB5_42FF_9257_C03CFDBD6CC7__INCLUDED_)
#define AFX_VTSNOTIFYTYPEDLG_H__3A02879F_2FB5_42FF_9257_C03CFDBD6CC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyTypeDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSNotifyTypeDlg dialog

class VTSNotifyTypeDlg : public CDialog
{
// Construction
public:
	VTSNotifyTypeDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSNotifyTypeDlg)
	enum { IDD = IDD_NOTIFYTYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSNotifyTypeDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYTYPEDLG_H__3A02879F_2FB5_42FF_9257_C03CFDBD6CC7__INCLUDED_)
