#if !defined(AFX_VTSSESSIONKEYDLG_H__77158901_4057_4E90_9922_25B60F96BCA4__INCLUDED_)
#define AFX_VTSSESSIONKEYDLG_H__77158901_4057_4E90_9922_25B60F96BCA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSSessionKeyDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSSessionKeyDlg dialog

class VTSSessionKeyDlg : public CDialog
{
// Construction
public:
	VTSSessionKeyDlg(CWnd* pParent = NULL);   // standard constructor
	VTSOctetStringCtrl			m_sessionKey;
	VTSUnsignedCtrl				m_Network;
	VTSOctetStringCtrl			m_MACAddress;
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
// Dialog Data
	//{{AFX_DATA(VTSSessionKeyDlg)
	enum { IDD = IDD_SESSIONKEY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSSessionKeyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSSessionKeyDlg)
	afx_msg void OnChangeNetwork();
	afx_msg void OnChangeSessionkey();
	afx_msg void OnChangeMacaddress();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSSESSIONKEYDLG_H__77158901_4057_4E90_9922_25B60F96BCA4__INCLUDED_)
