#if !defined(AFX_SENDREINITDEVICE_H__17BFA1F3_5F84_11D4_BEC5_00A0C95A9812__INCLUDED_)
#define AFX_SENDREINITDEVICE_H__17BFA1F3_5F84_11D4_BEC5_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReinitDevice.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CSendReinitDevice dialog

class CSendReinitDevice : public CSendPage
{
	DECLARE_DYNCREATE(CSendReinitDevice)

// Construction
public:
	CSendReinitDevice( void );   // non-standard constructor

	VTSCharacterStringCtrl		m_Password;
  UINT                      m_State;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendReinitDevice)
	enum { IDD = IDD_SENDREINITDEVICE };
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
	//{{AFX_MSG(CSendReinitDevice)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  CComboBox ReinitStates;
  afx_msg void OnCbnSelchangeReinitstate();
  afx_msg void OnEnChangePassword();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREINITDEVICE_H__17BFA1F3_5F84_11D4_BEC5_00A0C95A9812__INCLUDED_)
