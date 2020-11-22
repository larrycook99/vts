#if !defined(AFX_VTSNOTIFYSTATE_H__C16F9188_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYSTATE_H__C16F9188_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyState.h : header file
//

#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyState dialog

class VTSNotifyState : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyState)

// Construction
public:
	VTSNotifyState();
	~VTSNotifyState();

	int						m_Type;
	int						m_Value;
	VTSStatusFlags			m_StatusFlags;

	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyState)
	enum { IDD = IDD_NOTIFY_STATE };
	CComboBox	m_ValueCombo;
	CComboBox	m_TypeCombo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyState)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSNotifyState)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTypeCombo();
	afx_msg void OnSelchangeValueCombo();
	afx_msg void OnInAlarm();
	afx_msg void OnFault();
	afx_msg void OnOverridden();
	afx_msg void OnOutOfService();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYSTATE_H__C16F9188_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
