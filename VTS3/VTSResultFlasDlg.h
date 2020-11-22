#if !defined(AFX_VTSRESULTFLASDLG_H__AF8C9124_A1BA_4CD0_A7A7_D141F9B7198B__INCLUDED_)
#define AFX_VTSRESULTFLASDLG_H__AF8C9124_A1BA_4CD0_A7A7_D141F9B7198B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSResultFlasDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSResultFlasDlg dialog

class VTSResultFlasDlg : public CDialog
{
// Construction
public:
	VTSResultFlasDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode( BACnetAPDUDecoder& dec );
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	VTSBooleanCtrl   m_firstItem;
	VTSBooleanCtrl   m_lastItem;
	VTSBooleanCtrl   m_moreItem;
// Dialog Data
	//{{AFX_DATA(VTSResultFlasDlg)
	enum { IDD = IDD_RESULTFLAGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSResultFlasDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSResultFlasDlg)
	afx_msg void OnFirstitem();
	afx_msg void OnLastitem();
	afx_msg void OnMoreitem();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSRESULTFLASDLG_H__AF8C9124_A1BA_4CD0_A7A7_D141F9B7198B__INCLUDED_)
