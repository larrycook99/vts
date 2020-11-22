#if !defined(AFX_VTSEVENTFLOATLIMITDLG_H__95A6765E_5B4A_4FBB_A829_5039B17EC3DB__INCLUDED_)
#define AFX_VTSEVENTFLOATLIMITDLG_H__95A6765E_5B4A_4FBB_A829_5039B17EC3DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventFloatLimitDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventFloatLimitDlg dialog

class VTSEventFloatLimitDlg : public CDialog
{
// Construction
public:
	VTSEventFloatLimitDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSUnsignedCtrl				              m_timeDelay;
	VTSObjectIdentifierCtrl					  m_DeviceID;
	VTSObjectIdentifierCtrl					  m_ObjectID;
	VTSEnumeratedCtrl						  m_propCombo;
	VTSUnsignedCtrl							  m_ArrayIndex;	
	VTSRealCtrl								  m_LowLimit;
	VTSRealCtrl								  m_HighLimit;
	VTSRealCtrl								  m_DeadBand;
// Dialog Data
	//{{AFX_DATA(VTSEventFloatLimitDlg)
	enum { IDD = IDD_EVENT_FLOATLIMIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventFloatLimitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventFloatLimitDlg)
	afx_msg void OnChangeHighlimit();
	afx_msg void OnChangeLowlimit();
	afx_msg void OnChangeObjectid();
	afx_msg void OnDeviceidbtn();
	afx_msg void OnChangeDeviceid();
	afx_msg void OnChangeDeadband();
	afx_msg void OnChangeArrayindex();
	afx_msg void OnObjectidbtn();
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnChangeTimedelay();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTFLOATLIMITDLG_H__95A6765E_5B4A_4FBB_A829_5039B17EC3DB__INCLUDED_)
