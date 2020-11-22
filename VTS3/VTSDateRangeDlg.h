#if !defined(AFX_VTSDATERANGEDLG_H__235E94B4_E9C9_4DDF_A6AA_C6D193CA5B2A__INCLUDED_)
#define AFX_VTSDATERANGEDLG_H__235E94B4_E9C9_4DDF_A6AA_C6D193CA5B2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDateRangeDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSDateRangeDlg dialog

class VTSDateRangeDlg : public CDialog
{
// Construction
public:
	VTSDateRangeDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );

	VTSDateCtrl m_StartDateCtrl;
	VTSDateCtrl m_EndDateCtrl;
	BACnetDateRange m_value;

// Dialog Data
	//{{AFX_DATA(VTSDateRangeDlg)
	enum { IDD = IDD_DATERANGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDateRangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSDateRangeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDATERANGEDLG_H__235E94B4_E9C9_4DDF_A6AA_C6D193CA5B2A__INCLUDED_)
