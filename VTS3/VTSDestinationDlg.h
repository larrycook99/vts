#if !defined(AFX_VTSDESTINATIONDLG_H__228F9854_252E_476C_9AF0_310ADFDBD446__INCLUDED_)
#define AFX_VTSDESTINATIONDLG_H__228F9854_252E_476C_9AF0_310ADFDBD446__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDestinationDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSDestinationDlg dialog

class VTSDestinationDlg : public CDialog
{
// Construction
public:
	VTSDestinationDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode( BACnetAPDUDecoder& dec );
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	VTSObjectIdentifierCtrl		m_DeviceID;
	VTSUnsignedCtrl				m_Network;
	VTSOctetStringCtrl			m_MACAddress;
	VTSTimeCtrl					m_FromTimeCtrl;
	VTSTimeCtrl					m_ToTimeCtrl;
	BACnetDestination			m_Value;
	VTSUnsignedCtrl				m_ProcessID;	// MAG 06JUN06 add this control
// Dialog Data
	//{{AFX_DATA(VTSDestinationDlg)
	enum { IDD = IDD_DESTINATION };
	BOOL	m_bDestinationFault;
	BOOL	m_bDestinationConfirm;
	BOOL	m_bDestinationFriday;
	BOOL	m_bDestinationMonday;
	BOOL	m_bDestinationNormal;
	BOOL	m_bDestinationOffNormal;
	BOOL	m_bDestinationSaturday;
	BOOL	m_bDestinationSunday;
	BOOL	m_bDestinationThursday;
	BOOL	m_bDestinationTuesday;
	BOOL	m_bDestinationWednesday;
	int		m_IsRecipientDevice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDestinationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSDestinationDlg)
	afx_msg void OnDestinationAddress();
	afx_msg void OnDestinationDevice();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeviceIDButton();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDESTINATIONDLG_H__228F9854_252E_476C_9AF0_310ADFDBD446__INCLUDED_)
