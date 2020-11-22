#if !defined(AFX_VTSEVENTCHANGEOFVALUEDLG_H__4C084F32_75EE_41BB_A063_FBB7C173D478__INCLUDED_)
#define AFX_VTSEVENTCHANGEOFVALUEDLG_H__4C084F32_75EE_41BB_A063_FBB7C173D478__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventChangeOfValueDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventChangeOfValueDlg dialog

class VTSEventChangeOfValueDlg : public CDialog
{
// Construction
public:
	VTSEventChangeOfValueDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSUnsignedCtrl				 	    m_timeDelay;
	VTSBitStringCtrl					m_bitmask;
	VTSRealCtrl							m_realvalue;
// Dialog Data
	//{{AFX_DATA(VTSEventChangeOfValueDlg)
	enum { IDD = IDD_EVENT_VALUE };
	int		m_nChoice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventChangeOfValueDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventChangeOfValueDlg)
	afx_msg void OnRadiobitstr();
	afx_msg void OnRadioreal();
	afx_msg void OnChangeBitmask();
	afx_msg void OnChangeRealvalue();
	afx_msg void OnChangeTimedelay();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTCHANGEOFVALUEDLG_H__4C084F32_75EE_41BB_A063_FBB7C173D478__INCLUDED_)
