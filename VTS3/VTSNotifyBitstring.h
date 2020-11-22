#if !defined(AFX_VTSNOTIFYBITSTRING_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYBITSTRING_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyBitstring.h : header file
//

#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyBitstring dialog

class VTSNotifyBitstring : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyBitstring)

// Construction
public:
	VTSNotifyBitstring();
	~VTSNotifyBitstring();

	VTSBitStringCtrl		m_RefBitString;
	VTSStatusFlags			m_StatusFlags;

	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyBitstring)
	enum { IDD = IDD_NOTIFY_BITSTRING };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyBitstring)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSNotifyBitstring)
	afx_msg void OnChangeRefBitString();
	afx_msg void OnInAlarm();
	afx_msg void OnFault();
	afx_msg void OnOverridden();
	afx_msg void OnOutOfService();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYBITSTRING_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
