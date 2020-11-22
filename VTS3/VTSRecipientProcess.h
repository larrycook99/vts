#if !defined(AFX_VTSRECIPIENTPROCESS_H__B4835A32_83AD_11D4_BEDC_00A0C95A9812__INCLUDED_)
#define AFX_VTSRECIPIENTPROCESS_H__B4835A32_83AD_11D4_BEDC_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSRecipientProcess.h : header file
//

#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// VTSRecipientProcess dialog

class VTSRecipientProcess : public CDialog
{
// Construction
public:
	VTSRecipientProcess(CWnd* pParent = NULL);   // standard constructor

	bool						ctrlNull;		// iff nothing provided

	VTSObjectIdentifierCtrl		m_DeviceID;
	VTSUnsignedCtrl				m_Network;
	VTSOctetStringCtrl			m_MACAddress;
	VTSUnsignedCtrl				m_ProcessID;

	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );

	//Added by Zhu Zhenhua, 2004-6-14
	void EncodeValue( BACnetAPDUEncoder& enc, int context = kAppContext );
	void DecodeValue( BACnetAPDUDecoder& dec ); 

	void SaveCtrl( BACnetAPDUEncoder& enc );		// save the contents
	void RestoreCtrl( BACnetAPDUDecoder& dec );		// restore the contents

// Dialog Data
	//{{AFX_DATA(VTSRecipientProcess)
	enum { IDD = IDD_RECIPIENTPROCESS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSRecipientProcess)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSRecipientProcess)
	afx_msg void OnChangeDeviceID();
	afx_msg void OnChangeNetwork();
	afx_msg void OnChangeMACAddress();
	afx_msg void OnChangeProcessID();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeviceIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSRECIPIENTPROCESS_H__B4835A32_83AD_11D4_BEDC_00A0C95A9812__INCLUDED_)
