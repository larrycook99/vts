#if !defined(AFX_VTSNOTIFYBUFFERREADY_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYBUFFERREADY_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyBufferReady.h : header file
//

#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyBufferReady dialog

class VTSNotifyBufferReady : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyBufferReady)

// Construction
public:
	VTSObjectIdentifierCtrl		m_ObjectID;
	VTSEnumeratedCtrl			m_PropCombo;
	VTSUnsignedCtrl				m_ArrayIndex;
	VTSObjectIdentifierCtrl		m_DeviceID;
	VTSUnsignedCtrl				m_Previousnotification;
	VTSUnsignedCtrl				m_Currentnotification;

	VTSNotifyBufferReady();
	~VTSNotifyBufferReady();


	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyBufferReady)
	enum { IDD = IDD_NOTIFY_BUFFERREADY };
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
	afx_msg void OnDeviceidbtn();
	afx_msg void OnObjectidbtn();
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnChangeArrayindex();
	afx_msg void OnChangeCurnotif();
	afx_msg void OnChangePrenotif();
	afx_msg void OnChangeObjID();
	afx_msg void OnChangeDevID();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYBITSTRING_H__C16F9183_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
