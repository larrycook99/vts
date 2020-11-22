#if !defined(AFX_VTSNOTIFYCHGLS_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYCHGLS_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyChgLifeSafety.h : header file
//

#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyChgLifeSafety dialog

class VTSNotificationParameters;

class VTSNotifyChgLifeSafety : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyChgLifeSafety)

// Construction
public:
	VTSEnumeratedCtrl			m_LSState;
	VTSEnumeratedCtrl			m_LSMode;
	VTSStatusFlags				m_StatusFlags;
	VTSEnumeratedCtrl			m_LSOperation;

	VTSNotifyChgLifeSafety();
	~VTSNotifyChgLifeSafety();


	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyBufferReady)
	enum { IDD = IDD_NOTIFY_CHG_LS };
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
	afx_msg void OnDropdownLSState();
	afx_msg void OnSelchangeLSState();
	afx_msg void OnDropdownLSMode();
	afx_msg void OnSelchangeLSMode();
	afx_msg void OnDropdownLSOperation();
	afx_msg void OnSelchangeLSOperation();
	afx_msg void OnInAlarm();
	afx_msg void OnFault();
	afx_msg void OnOverridden();
	afx_msg void OnOutOfService();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYCHGLS_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
