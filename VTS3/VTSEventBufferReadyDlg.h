#if !defined(AFX_VTSEVENTBUFFERREADYDLG_H__58938842_3AF4_4F66_ACAA_9B0EE7509B92__INCLUDED_)
#define AFX_VTSEVENTBUFFERREADYDLG_H__58938842_3AF4_4F66_ACAA_9B0EE7509B92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventBufferReadyDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventBufferReadyDlg dialog

class VTSEventBufferReadyDlg : public CDialog
{
// Construction
public:
	VTSEventBufferReadyDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSUnsignedCtrl				              m_ntfthreshold;
	VTSUnsignedCtrl							  m_prevntfcount;

// Dialog Data
	//{{AFX_DATA(VTSEventBufferReadyDlg)
	enum { IDD = IDD_EVENT_BUFFERREADY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventBufferReadyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventBufferReadyDlg)
	afx_msg void OnChangeNtfthresh();
	afx_msg void OnChangePreviousnotf();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTBUFFERREADYDLG_H__58938842_3AF4_4F66_ACAA_9B0EE7509B92__INCLUDED_)
