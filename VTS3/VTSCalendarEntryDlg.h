#if !defined(AFX_VTSCALENDARENTRYDLG_H__3EE64E2D_C46E_487E_8497_30988E7119DB__INCLUDED_)
#define AFX_VTSCALENDARENTRYDLG_H__3EE64E2D_C46E_487E_8497_30988E7119DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSCalendarEntryDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSCalendarEntryDlg dialog

class VTSCalendarEntryDlg : public CDialog
{
// Construction
public:
	VTSCalendarEntryDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );

	int m_nChoice;
	VTSDateCtrl m_DateCtrl;
	VTSDateCtrl m_StartDateCtrl;
	VTSDateCtrl m_EndDateCtrl;
	VTSOctetStringCtrl m_WeekNDayCtrl;
	BACnetCalendarEntry m_value;

// Dialog Data
	//{{AFX_DATA(VTSCalendarEntryDlg)
	enum { IDD = IDD_CALENDARENTTRY };
	CButton	m_DateRadio;
	CButton	m_DateRangeRadio;
	CButton	m_WeekNDayRadio;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSCalendarEntryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSCalendarEntryDlg)
	afx_msg void OnCalendarentryDateradio();
	afx_msg void OnCalendarentryDaterangeradio();
	afx_msg void OnCalendarentryWeekndayradio();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSCALENDARENTRYDLG_H__3EE64E2D_C46E_487E_8497_30988E7119DB__INCLUDED_)
