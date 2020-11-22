#if !defined(AFX_VTSDEVICEOBJECTPROPERTYREFERENCEDLG_H__2366AE50_82FC_47F2_99FA_58BFCF22B0A3__INCLUDED_)
#define AFX_VTSDEVICEOBJECTPROPERTYREFERENCEDLG_H__2366AE50_82FC_47F2_99FA_58BFCF22B0A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDeviceObjectPropertyReferenceDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSDeviceObjectPropertyReferenceDlg dialog

class VTSDeviceObjectPropertyReferenceDlg : public CDialog
{
// Construction
public:
	VTSDeviceObjectPropertyReferenceDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
		VTSObjectIdentifierCtrl					  m_DeviceID;
		VTSObjectIdentifierCtrl					  m_ObjectID;
		VTSEnumeratedCtrl						  m_propCombo;
		VTSUnsignedCtrl							  m_ArrayIndex;
// Dialog Data
	//{{AFX_DATA(VTSDeviceObjectPropertyReferenceDlg)
	enum { IDD = IDD_DEVICEOBJECTPROPREFERENCE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDeviceObjectPropertyReferenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSDeviceObjectPropertyReferenceDlg)
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnChangeObjectid();
	afx_msg void OnObjectidbtn();
	afx_msg void OnDeviceidbtn();
	afx_msg void OnChangeDeviceid();
	afx_msg void OnChangeArrayindex();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDEVICEOBJECTPROPERTYREFERENCEDLG_H__2366AE50_82FC_47F2_99FA_58BFCF22B0A3__INCLUDED_)
