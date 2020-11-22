#if !defined(AFX_VTSSPECIALEVENTDLG_H__6BB07D71_1196_496C_96D5_1FCFAB1D5A95__INCLUDED_)
#define AFX_VTSSPECIALEVENTDLG_H__6BB07D71_1196_496C_96D5_1FCFAB1D5A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSSpecialEventDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSCalendarEntryDlg.h"
#include "VTSDailyScheduleDlg.h"
/////////////////////////////////////////////////////////////////////////////
// VTSSpecialEventDlg dialog

class VTSSpecialEventDlg : public CDialog
{
// Construction
public:
	VTSSpecialEventDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSCalendarEntryDlg			    m_calendarEntry;
	VTSObjectIdentifierCtrl			m_calendarRef;
	VTSDailyScheduleDlg				m_listofTimeValues;
	VTSUnsignedCtrl					m_eventPriority;
// Dialog Data
	//{{AFX_DATA(VTSSpecialEventDlg)
	enum { IDD = IDD_SPECIALEVENT };
	int		m_nChoice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSSpecialEventDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSSpecialEventDlg)
	afx_msg void OnChangeObjectid();
	afx_msg void OnObjectidbtn();
	afx_msg void OnListoftimevalues();
	afx_msg void OnChangeUnsignedvalue();
	afx_msg void OnRadiocalentry();
	afx_msg void OnRadioobjid();
	afx_msg void OnValue();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSSPECIALEVENTDLG_H__6BB07D71_1196_496C_96D5_1FCFAB1D5A95__INCLUDED_)
