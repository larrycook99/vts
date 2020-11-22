#if !defined(AFX_VTSDAYSOFWEEKDLG_H__96C88543_BFB4_4BFE_B8FC_E4E9BCF1D38D__INCLUDED_)
#define AFX_VTSDAYSOFWEEKDLG_H__96C88543_BFB4_4BFE_B8FC_E4E9BCF1D38D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDaysOfWeekDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSDaysOfWeekDlg dialog

class VTSDaysOfWeekDlg : public CDialog
{
// Construction
public:
	VTSDaysOfWeekDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode( BACnetAPDUDecoder& dec );
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	VTSBooleanCtrl   m_MondayCtrl;
	VTSBooleanCtrl   m_TuesdayCtrl;
	VTSBooleanCtrl   m_WednesCtrl;
	VTSBooleanCtrl   m_ThusdayCtrl;
	VTSBooleanCtrl   m_FridayCtrl;
	VTSBooleanCtrl   m_SaturdayCtrl;
	VTSBooleanCtrl   m_SundayCtrl;
// Dialog Data
	//{{AFX_DATA(VTSDaysOfWeekDlg)
	enum { IDD = IDD_DAYSOFWEEK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDaysOfWeekDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSDaysOfWeekDlg)
	afx_msg void OnDofweekSunday();
	afx_msg void OnDofweekSaturday();
	afx_msg void OnDofweekMonday();
	afx_msg void OnDofweekFriday();
	afx_msg void OnDofweekThursday();
	afx_msg void OnDofweekTuesday();
	afx_msg void OnDofweekWednesday();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDAYSOFWEEKDLG_H__96C88543_BFB4_4BFE_B8FC_E4E9BCF1D38D__INCLUDED_)
