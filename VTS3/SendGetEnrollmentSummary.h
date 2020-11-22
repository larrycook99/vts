#if !defined(AFX_SENDGETENROLLMENTSUMMARY_H__B4835A31_83AD_11D4_BEDC_00A0C95A9812__INCLUDED_)
#define AFX_SENDGETENROLLMENTSUMMARY_H__B4835A31_83AD_11D4_BEDC_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendGetEnrollmentSummary.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSRecipientProcess.h"

/////////////////////////////////////////////////////////////////////////////
// CSendGetEnrollmentSummary dialog

class CSendGetEnrollmentSummary : public CSendPage
{
	DECLARE_DYNCREATE(CSendGetEnrollmentSummary)

// Construction
public:
	CSendGetEnrollmentSummary( void );   // non-standard constructor

	VTSEnumeratedCtrl			m_AckFilterCombo;
	VTSRecipientProcess			m_RecipientProcess;
	VTSEnumeratedCtrl			m_EventStateCombo;
	VTSEnumeratedCtrl			m_EventTypeCombo;
	VTSUnsignedCtrl				m_MinPriority;
	VTSUnsignedCtrl				m_MaxPriority;
	VTSUnsignedCtrl				m_NotificationClass;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendGetEnrollmentSummary)
	enum { IDD = IDD_SENDGETENROLLMENTSUMMARY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendGetEnrollmentSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendGetEnrollmentSummary)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAckFilterCombo();
	afx_msg void OnRecipientProcess();
	afx_msg void OnSelchangeEventStateCombo();
	afx_msg void OnSelchangeEventTypeCombo();
	afx_msg void OnChangeMinPriority();
	afx_msg void OnChangeMaxPriority();
	afx_msg void OnChangeNotificationClass();
	//}}AFX_MSG
	afx_msg void OnChangeEventStateCombo();
	afx_msg void OnChangeEventTypeCombo();
	afx_msg void OnChangeAckFilterCombo();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDGETENROLLMENTSUMMARY_H__B4835A31_83AD_11D4_BEDC_00A0C95A9812__INCLUDED_)
