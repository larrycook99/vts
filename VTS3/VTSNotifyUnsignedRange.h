#if !defined(AFX_VTSNOTIFYUNSRANGE_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFUNSRANGE_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyUnsignedRange.h : header file
//

#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyUnsignedRnage dialog

class VTSNotificationParameters;

class VTSNotifyUnsignedRange : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyUnsignedRange)

// Construction
public:
	VTSUnsignedCtrl				m_ExValue;
	VTSStatusFlags				m_StatusFlags;
	VTSUnsignedCtrl				m_ExLimit;

	VTSNotifyUnsignedRange();
	~VTSNotifyUnsignedRange();


	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyBufferReady)
	enum { IDD = IDD_NOTIFY_UNS_RANGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyBufferReady)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSNotifyBufferReady)
	afx_msg void OnChangeExValue();
	afx_msg void OnChangeExLimit();
	afx_msg void OnInAlarm();
	afx_msg void OnFault();
	afx_msg void OnOverridden();
	afx_msg void OnOutOfService();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYUNSRANGE_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
