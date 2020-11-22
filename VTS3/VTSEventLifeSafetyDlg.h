#if !defined(AFX_VTSEVENTLIFESAFETYDLG_H__EA0A90A5_ABB7_47EB_AA25_7C3066A37BA1__INCLUDED_)
#define AFX_VTSEVENTLIFESAFETYDLG_H__EA0A90A5_ABB7_47EB_AA25_7C3066A37BA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventLifeSafetyDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSListOfLifeSafetyStateDlg.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventLifeSafetyDlg dialog

class VTSEventLifeSafetyDlg : public CDialog
{
// Construction
public:
	VTSEventLifeSafetyDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSUnsignedCtrl						      m_timeDelay;
	VTSListOfLifeSafetyStateDlg				  m_listoflfsftav;
	VTSListOfLifeSafetyStateDlg				  m_listofav;		
	VTSObjectIdentifierCtrl					  m_DeviceID;
	VTSObjectIdentifierCtrl					  m_ObjectID;
	VTSEnumeratedCtrl						  m_propCombo;
	VTSUnsignedCtrl							  m_ArrayIndex;	
// Dialog Data
	//{{AFX_DATA(VTSEventLifeSafetyDlg)
	enum { IDD = IDD_EVENT_LIFESAFETY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventLifeSafetyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventLifeSafetyDlg)
	afx_msg void OnChangeArrayindex();
	afx_msg void OnChangeDeviceid();
	afx_msg void OnDeviceidbtn();
	afx_msg void OnListofav();
	afx_msg void OnListoflsfav();
	afx_msg void OnChangeObjectid();
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

#endif // !defined(AFX_VTSEVENTLIFESAFETYDLG_H__EA0A90A5_ABB7_47EB_AA25_7C3066A37BA1__INCLUDED_)
