#if !defined(AFX_VTSNOTIFYCMDFAILURE_H__C16F9184_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYCMDFAILURE_H__C16F9184_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyCmdFailure.h : header file
//

#include "VTSAny.h"
#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyCmdFailure dialog

class VTSNotifyCmdFailure : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyCmdFailure)

// Construction
public:
	VTSNotifyCmdFailure();
	~VTSNotifyCmdFailure();

	VTSAny					m_CommandValue;
	VTSStatusFlags			m_StatusFlags;
	VTSAny					m_FeedbackValue;

	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyCmdFailure)
	enum { IDD = IDD_NOTIFY_CMDFAILURE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyCmdFailure)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSNotifyCmdFailure)
	afx_msg void OnCommandValue();
	afx_msg void OnInAlarm();
	afx_msg void OnFault();
	afx_msg void OnOverridden();
	afx_msg void OnOutOfService();
	afx_msg void OnFeedbackValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYCMDFAILURE_H__C16F9184_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
