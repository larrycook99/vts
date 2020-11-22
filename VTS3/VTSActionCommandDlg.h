#if !defined(AFX_VTSACTIONCOMMANDDLG_H__04A44B15_C19F_4496_8786_8CF52DB5BEA6__INCLUDED_)
#define AFX_VTSACTIONCOMMANDDLG_H__04A44B15_C19F_4496_8786_8CF52DB5BEA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSActionCommandDlg.h : header file
//
#include "VTSAny.h"
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSActionCommandDlg dialog

class VTSActionCommandDlg : public CDialog
{
// Construction
public:
	VTSActionCommandDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
		VTSObjectIdentifierCtrl					  m_DeviceID;
		VTSObjectIdentifierCtrl					  m_ObjectID;
		VTSEnumeratedCtrl						  m_propCombo;
		VTSUnsignedCtrl							  m_ArrayIndex;
		VTSAny									  m_propValue;
		VTSUnsignedCtrl							  m_Priority;
		VTSUnsignedCtrl							  m_PostDelay;
		VTSBooleanCtrl			quitOnFail;
		VTSBooleanCtrl			writeSuccess;
// Dialog Data
	//{{AFX_DATA(VTSActionCommandDlg)
	enum { IDD = IDD_ACTIONCOMMAND };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSActionCommandDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSActionCommandDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnObjectidbtn();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnChangeObjectid();
	afx_msg void OnDeviceidbtn();
	afx_msg void OnChangeDeviceid();
	afx_msg void OnValue();
	afx_msg void OnWritesuccess();
	afx_msg void OnQuitonfail();
	afx_msg void OnChangePrio();
	afx_msg void OnChangePostdelay();
	afx_msg void OnChangeArrayindex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSACTIONCOMMANDDLG_H__04A44B15_C19F_4496_8786_8CF52DB5BEA6__INCLUDED_)
