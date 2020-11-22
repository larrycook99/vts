#if !defined(AFX_VTSVTSESSIONDLG_H__1F8825DE_BD4C_4C37_9A9A_EF611FA73F49__INCLUDED_)
#define AFX_VTSVTSESSIONDLG_H__1F8825DE_BD4C_4C37_9A9A_EF611FA73F49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSVTSessionDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSVTSessionDlg dialog

class VTSVTSessionDlg : public CDialog
{
// Construction
public:
	VTSVTSessionDlg(CWnd* pParent = NULL);   // standard constructor
	VTSUnsignedCtrl				m_localSessionID;
	VTSUnsignedCtrl				m_remoteSessionID;
	VTSUnsignedCtrl				m_Network;
	VTSOctetStringCtrl			m_MACAddress;
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
// Dialog Data
	//{{AFX_DATA(VTSVTSessionDlg)
	enum { IDD = IDD_VTSESSION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSVTSessionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSVTSessionDlg)
	afx_msg void OnChangeNetwork();
	afx_msg void OnChangeMacaddress();
	afx_msg void OnChangeLocalsessionid();
	afx_msg void OnChangeRemotesessionid();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSVTSESSIONDLG_H__1F8825DE_BD4C_4C37_9A9A_EF611FA73F49__INCLUDED_)
