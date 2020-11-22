#if !defined(AFX_SENDDEVICE_H__AE31D331_BA2B_11D6_A613_00A0C9E370F1__INCLUDED_)
#define AFX_SENDDEVICE_H__AE31D331_BA2B_11D6_A613_00A0C9E370F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendDevice.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendDevice dialog

class CSendDevice : public CSendPage
{
	DECLARE_DYNCREATE(CSendDevice)

// Construction
public:
	CSendDevice( void );   // non-standard constructor

	VTSIPAddrCtrl	m_DADR;						// destination address
	VTSUnsignedCtrl	m_Network;
	VTSAddrCtrl		m_Address;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page into contents

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendDevice)
	enum { IDD = IDD_SENDDEVICE };
	int m_AddrType;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendDevice)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendDevice)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDADRCombo();
	afx_msg void OnAddrTypeChange();
	afx_msg void OnChangeNetwork();
	afx_msg void OnChangeAddress();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDDEVICE_H__AE31D331_BA2B_11D6_A613_00A0C9E370F1__INCLUDED_)
