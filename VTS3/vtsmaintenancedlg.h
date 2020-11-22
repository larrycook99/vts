#if !defined(AFX_VTSMAINTENANCEDLG_H__05A0CB6D_EB46_4F11_97E8_FA130020A58F__INCLUDED_)
#define AFX_VTSMAINTENANCEDLG_H__05A0CB6D_EB46_4F11_97E8_FA130020A58F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// vtsmaintenancedlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSMaintenanceDlg dialog

class VTSMaintenanceDlg : public CDialog
{
// Construction
public:
	VTSMaintenanceDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSMaintenanceDlg)
	enum { IDD = IDD_MAINTENANCE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSMaintenanceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSMaintenanceDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSMAINTENANCEDLG_H__05A0CB6D_EB46_4F11_97E8_FA130020A58F__INCLUDED_)
