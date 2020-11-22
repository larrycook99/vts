#if !defined(AFX_VTSRECIPIENTDLG_H__44C8C6F2_8D22_4EEC_895B_2D0CC127B7F7__INCLUDED_)
#define AFX_VTSRECIPIENTDLG_H__44C8C6F2_8D22_4EEC_895B_2D0CC127B7F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSRecipientDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSRecipientDlg dialog

class VTSRecipientDlg : public CDialog
{
// Construction
public:
	VTSRecipientDlg(CWnd* pParent = NULL);   // standard constructor				
	VTSObjectIdentifierCtrl		m_DeviceID;
	VTSUnsignedCtrl				m_Network;
	VTSOctetStringCtrl			m_MACAddress;
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );

// Dialog Data
	//{{AFX_DATA(VTSRecipientDlg)
	enum { IDD = IDD_RECIPIENT };
	int		m_choice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSRecipientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSRecipientDlg)
	afx_msg void OnChangeDeviceid();
	afx_msg void OnDeviceidbtn();
	afx_msg void OnChangeMacaddress();
	afx_msg void OnChangeNetwork();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioaddress();
	afx_msg void OnRadiodevice();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSRECIPIENTDLG_H__44C8C6F2_8D22_4EEC_895B_2D0CC127B7F7__INCLUDED_)
