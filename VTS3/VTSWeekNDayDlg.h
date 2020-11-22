#if !defined(AFX_VTSWEEKNDAYDLG_H__36D66A38_C0F2_480E_B86B_E783C595ECB1__INCLUDED_)
#define AFX_VTSWEEKNDAYDLG_H__36D66A38_C0F2_480E_B86B_E783C595ECB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSWeekNDayDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSWeekNDayDlg dialog
class VTSWeekNDayDlg : public CDialog
{
// Construction
public:
	VTSWeekNDayDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_month;
	VTSEnumeratedCtrl	m_weekofmonth;
	VTSEnumeratedCtrl	m_dayofweek;	
// Dialog Data
	//{{AFX_DATA(VTSWeekNDayDlg)
	enum { IDD = IDD_WEEKNDAY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSWeekNDayDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSWeekNDayDlg)
	afx_msg void OnSelchangeMonth();
	afx_msg void OnSelchangeWeekofmonth();
	afx_msg void OnSelchangeDayofweek();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSWEEKNDAYDLG_H__36D66A38_C0F2_480E_B86B_E783C595ECB1__INCLUDED_)
