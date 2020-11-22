#if !defined(AFX_VTSLIFESAFETYOPERATIONDLG_H__1FBDC7BB_8B07_4B6E_8122_F951E8710373__INCLUDED_)
#define AFX_VTSLIFESAFETYOPERATIONDLG_H__1FBDC7BB_8B07_4B6E_8122_F951E8710373__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSLifeSafetyOperationDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSLifeSafetyOperationDlg dialog

class VTSLifeSafetyOperationDlg : public CDialog
{
// Construction
public:
	VTSLifeSafetyOperationDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSLifeSafetyOperationDlg)
	enum { IDD = IDD_LIFESAFETYOPERATION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSLifeSafetyOperationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSLifeSafetyOperationDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLIFESAFETYOPERATIONDLG_H__1FBDC7BB_8B07_4B6E_8122_F951E8710373__INCLUDED_)
