#if !defined(AFX_VTSEVENTCOMMANDFAILUREDLG_H__73A8DEAD_C6A7_473D_A01A_3AFDFCA71DB0__INCLUDED_)
#define AFX_VTSEVENTCOMMANDFAILUREDLG_H__73A8DEAD_C6A7_473D_A01A_3AFDFCA71DB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventCommandFailureDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventCommandFailureDlg dialog

class VTSEventCommandFailureDlg : public CDialog
{
// Construction
public:
	VTSEventCommandFailureDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSUnsignedCtrl					m_timeDelay;
	VTSObjectIdentifierCtrl					  m_DeviceID;
	VTSObjectIdentifierCtrl					  m_ObjectID;
	VTSEnumeratedCtrl						  m_propCombo;
	VTSUnsignedCtrl							  m_ArrayIndex;	
// Dialog Data
	//{{AFX_DATA(VTSEventCommandFailureDlg)
	enum { IDD = IDD_EVENT_CMDFAILURE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventCommandFailureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventCommandFailureDlg)
	afx_msg void OnChangeTimedelay();
	afx_msg void OnChangeObjectid();
	afx_msg void OnObjectidbtn();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnDeviceidbtn();
	afx_msg void OnChangeDeviceid();
	afx_msg void OnChangeArrayindex();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTCOMMANDFAILUREDLG_H__73A8DEAD_C6A7_473D_A01A_3AFDFCA71DB0__INCLUDED_)
