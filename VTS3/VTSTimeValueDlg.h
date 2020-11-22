#if !defined(AFX_VTSTIMEVALUEDLG_H__40C17026_CBE4_42A1_A13B_1754F733BA92__INCLUDED_)
#define AFX_VTSTIMEVALUEDLG_H__40C17026_CBE4_42A1_A13B_1754F733BA92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSTimeValueDlg.h : header file
//
#include "VTSAny.h"
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSTimeValueDlg dialog

class VTSTimeValueDlg : public CDialog
{
// Construction
public:
	VTSTimeValueDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSTimeCtrl m_TimeCtrl;
	VTSAny		m_Value;
// Dialog Data
	//{{AFX_DATA(VTSTimeValueDlg)
	enum { IDD = IDD_TIMEVALUE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSTimeValueDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSTimeValueDlg)
	afx_msg void OnValue();
	afx_msg void OnChangeTime();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSTIMEVALUEDLG_H__40C17026_CBE4_42A1_A13B_1754F733BA92__INCLUDED_)
