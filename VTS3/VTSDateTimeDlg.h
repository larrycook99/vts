#if !defined(AFX_VTSDATETIMEDLG_H__BB49F170_4497_4D3A_BDF1_45415203B864__INCLUDED_)
#define AFX_VTSDATETIMEDLG_H__BB49F170_4497_4D3A_BDF1_45415203B864__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDateTimeDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSDateTimeDlg dialog

class VTSDateTimeDlg : public CDialog
{
// Construction
public:
	void Decode( BACnetAPDUDecoder& dec );
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	VTSDateTimeDlg(CWnd* pParent = NULL);   // standard constructor
	VTSDateCtrl m_DateCtrl;
	VTSTimeCtrl m_TimeCtrl;
	BACnetDateTime m_value;
// Dialog Data
	//{{AFX_DATA(VTSDateTimeDlg)
	enum { IDD = IDD_DATETIME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDateTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSDateTimeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDATETIMEDLG_H__BB49F170_4497_4D3A_BDF1_45415203B864__INCLUDED_)
