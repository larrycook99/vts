#if !defined(AFX_VTSCOVSUBSCRIPTIONDLG_H__5CCEC49C_3ADD_4EB3_9844_154BAD93A619__INCLUDED_)
#define AFX_VTSCOVSUBSCRIPTIONDLG_H__5CCEC49C_3ADD_4EB3_9844_154BAD93A619__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSCOVSubscriptionDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSCOVSubscriptionDlg dialog

class VTSCOVSubscriptionDlg : public CDialog
{
// Construction
public:
	VTSCOVSubscriptionDlg(CWnd* pParent = NULL);   // standard constructor
	VTSObjectIdentifierCtrl		m_DeviceID;
	VTSUnsignedCtrl				m_Network;
	VTSOctetStringCtrl			m_MACAddress;
	VTSUnsignedCtrl				m_ProcessID;
	VTSObjectIdentifierCtrl		m_ObjectID;
	VTSEnumeratedCtrl			m_propCombo;
	VTSUnsignedCtrl				m_ArrayIndex;
	VTSBooleanCtrl				m_issuecfmntf;
	VTSUnsignedCtrl				m_TimeRemaining;
	VTSRealCtrl					m_COVIncr;
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
// Dialog Data
	//{{AFX_DATA(VTSCOVSubscriptionDlg)
	enum { IDD = IDD_COVSUBSCRIPTION };
	int		m_choice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSCOVSubscriptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSCOVSubscriptionDlg)
	afx_msg void OnRadiodevice();
	afx_msg void OnRadioaddress();
	afx_msg void OnChangeTimeremain();
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnChangeProcessid();
	afx_msg void OnObjectidbtn();
	afx_msg void OnChangeObjectid();
	afx_msg void OnChangeNetwork();
	afx_msg void OnChangeMacaddress();
	afx_msg void OnChangeIncrement();
	afx_msg void OnIssuecfmntf();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeDeviceid();
	afx_msg void OnDeviceidbtn();
	afx_msg void OnChangeArrayindex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSCOVSUBSCRIPTIONDLG_H__5CCEC49C_3ADD_4EB3_9844_154BAD93A619__INCLUDED_)
