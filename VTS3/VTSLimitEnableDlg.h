#if !defined(AFX_VTSLIMITENABLEDLG_H__493F310F_563D_47F7_84A2_8E48FDBF93D5__INCLUDED_)
#define AFX_VTSLIMITENABLEDLG_H__493F310F_563D_47F7_84A2_8E48FDBF93D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSLimitEnableDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSLimitEnableDlg dialog

class VTSLimitEnableDlg : public CDialog
{
// Construction
public:
	VTSLimitEnableDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode( BACnetAPDUDecoder& dec );
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	VTSBooleanCtrl   m_lowLimit;
	VTSBooleanCtrl   m_highLimit;
// Dialog Data
	//{{AFX_DATA(VTSLimitEnableDlg)
	enum { IDD = IDD_LIMITENABLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSLimitEnableDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSLimitEnableDlg)
	afx_msg void OnLowlimit();
	afx_msg void OnHighlimit();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLIMITENABLEDLG_H__493F310F_563D_47F7_84A2_8E48FDBF93D5__INCLUDED_)
