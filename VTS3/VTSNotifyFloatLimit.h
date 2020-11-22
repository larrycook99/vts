#if !defined(AFX_VTSNOTIFYFLOATLIMIT_H__C16F9186_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYFLOATLIMIT_H__C16F9186_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyFloatLimit.h : header file
//

#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyFloatLimit dialog

class VTSNotifyFloatLimit : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyFloatLimit)

// Construction
public:
	VTSNotifyFloatLimit();
	~VTSNotifyFloatLimit();

	VTSRealCtrl				m_RefValue;
	VTSStatusFlags			m_StatusFlags;
	VTSRealCtrl				m_SetpointValue;
	VTSRealCtrl				m_ErrorLimit;

	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyFloatLimit)
	enum { IDD = IDD_NOTIFY_FLOATLIMIT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyFloatLimit)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSNotifyFloatLimit)
	afx_msg void OnChangeRefValue();
	afx_msg void OnInAlarm();
	afx_msg void OnFault();
	afx_msg void OnOverridden();
	afx_msg void OnOutOfService();
	afx_msg void OnChangeSetpointValue();
	afx_msg void OnChangeErrorLimit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYFLOATLIMIT_H__C16F9186_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
