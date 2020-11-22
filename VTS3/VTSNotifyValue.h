#if !defined(AFX_VTSNOTIFYVALUE_H__C16F9189_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYVALUE_H__C16F9189_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyValue.h : header file
//

#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyValue dialog

class VTSNotifyValue : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyValue)

// Construction
public:
	VTSNotifyValue();
	~VTSNotifyValue();

	VTSBitStringCtrl		m_ChangedBits;
	VTSRealCtrl				m_ChangedValue;
	VTSStatusFlags			m_StatusFlags;

	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyValue)
	enum { IDD = IDD_NOTIFY_VALUE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyValue)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSNotifyValue)
	afx_msg void OnChangeChangedBits();
	afx_msg void OnChangeChangedValue();
	afx_msg void OnInAlarm();
	afx_msg void OnFault();
	afx_msg void OnOverridden();
	afx_msg void OnOutOfService();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYVALUE_H__C16F9189_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
