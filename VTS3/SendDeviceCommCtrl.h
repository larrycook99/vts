#if !defined(AFX_SENDDEVICECOMMCTRL_H__17BFA1F2_5F84_11D4_BEC5_00A0C95A9812__INCLUDED_)
#define AFX_SENDDEVICECOMMCTRL_H__17BFA1F2_5F84_11D4_BEC5_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendDeviceCommCtrl.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendDeviceCommCtrl dialog

class CSendDeviceCommCtrl : public CSendPage
{
	DECLARE_DYNCREATE(CSendDeviceCommCtrl)

// Construction
public:
	CSendDeviceCommCtrl( void );   // non-standard constructor

	VTSUnsignedCtrl				m_TimeDuration;
	VTSCharacterStringCtrl		m_Password;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendDeviceCommCtrl)
	enum { IDD = IDD_SENDDEVICECOMMCTRL };
	int		m_EnableDisable;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendDeviceCommCtrl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendDeviceCommCtrl)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeTimeDuration();
	afx_msg void OnChangePassword();
	afx_msg void OnEnable();
	afx_msg void OnDisable();
	afx_msg void OnDisableInit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDDEVICECOMMCTRL_H__17BFA1F2_5F84_11D4_BEC5_00A0C95A9812__INCLUDED_)
