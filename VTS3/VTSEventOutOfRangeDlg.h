#if !defined(AFX_VTSEVENTOUTOFRANGEDLG_H__2FC7F1AD_29F1_4485_9BFC_9D9C7A75323E__INCLUDED_)
#define AFX_VTSEVENTOUTOFRANGEDLG_H__2FC7F1AD_29F1_4485_9BFC_9D9C7A75323E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventOutOfRangeDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventOutOfRangeDlg dialog

class VTSEventOutOfRangeDlg : public CDialog
{
// Construction
public:
	VTSEventOutOfRangeDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSUnsignedCtrl				              m_timeDelay;
	VTSRealCtrl								  m_LowLimit;
	VTSRealCtrl								  m_HighLimit;
	VTSRealCtrl								  m_DeadBand;
// Dialog Data
	//{{AFX_DATA(VTSEventOutOfRangeDlg)
	enum { IDD = IDD_EVENT_OUTOFRANGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventOutOfRangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventOutOfRangeDlg)
	afx_msg void OnChangeTimedelay();
	afx_msg void OnChangeLowlimit();
	afx_msg void OnChangeHighlimit();
	afx_msg void OnChangeDeadband();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTOUTOFRANGEDLG_H__2FC7F1AD_29F1_4485_9BFC_9D9C7A75323E__INCLUDED_)
