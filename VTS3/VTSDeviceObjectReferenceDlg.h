#if !defined(AFX_VTSDEVICEOBJECTREFERENCEDLG_H__EC23D6FF_1449_48AD_9D3E_EB72D1BD95F7__INCLUDED_)
#define AFX_VTSDEVICEOBJECTREFERENCEDLG_H__EC23D6FF_1449_48AD_9D3E_EB72D1BD95F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDeviceObjectReferenceDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSDeviceObjectReferenceDlg dialog

class VTSDeviceObjectReferenceDlg : public CDialog
{
// Construction
public:
	VTSDeviceObjectReferenceDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
		VTSObjectIdentifierCtrl					  m_DeviceID;
		VTSObjectIdentifierCtrl					  m_ObjectID;
// Dialog Data
	//{{AFX_DATA(VTSDeviceObjectReferenceDlg)
	enum { IDD = IDD_DEVICEOBJECTREFERENCE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDeviceObjectReferenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSDeviceObjectReferenceDlg)
	afx_msg void OnChangeObjectid();
	afx_msg void OnObjectidbtn();
	afx_msg void OnDeviceidbtn();
	afx_msg void OnChangeDeviceid();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDEVICEOBJECTREFERENCEDLG_H__EC23D6FF_1449_48AD_9D3E_EB72D1BD95F7__INCLUDED_)
