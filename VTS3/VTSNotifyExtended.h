#if !defined(AFX_VTSNOTIFYEXTENDED_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYEXTENDED_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyEXTENDED.h : header file
//

#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyExtended dialog

class VTSNotificationParameters;

class VTSNotifyExtended : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyExtended)

// Construction
public:
	VTSUnsignedCtrl				m_VendorID;
	VTSUnsignedCtrl				m_EventType;
	// TODO add parameters section here not sure what format it should be

	VTSNotifyExtended();
	~VTSNotifyExtended();


	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyExtended)
	enum { IDD = IDD_NOTIFY_EXTENDED };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyExtended)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSNotifyExtended)
	afx_msg void OnChangeVendorID();
	afx_msg void OnChangeEventType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYEXTENDED_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
