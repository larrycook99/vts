#if !defined(AFX_VTSNOTIFYOUTOFRANGE_H__C16F9187_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYOUTOFRANGE_H__C16F9187_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyOutOfRange.h : header file
//

#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyOutOfRange dialog

class VTSNotifyOutOfRange : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyOutOfRange)

// Construction
public:
	VTSNotifyOutOfRange();
	~VTSNotifyOutOfRange();

	VTSRealCtrl				m_ExceedingValue;
	VTSStatusFlags			m_StatusFlags;
	VTSRealCtrl				m_Deadband;
	VTSRealCtrl				m_ExceedingLimit;

	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyOutOfRange)
	enum { IDD = IDD_NOTIFY_OUTOFRANGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyOutOfRange)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSNotifyOutOfRange)
	afx_msg void OnChangeExceedingValue();
	afx_msg void OnInAlarm();
	afx_msg void OnFault();
	afx_msg void OnOverridden();
	afx_msg void OnOutOfService();
	afx_msg void OnChangeDeadband();
	afx_msg void OnChangeExceedingLimit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYOUTOFRANGE_H__C16F9187_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
