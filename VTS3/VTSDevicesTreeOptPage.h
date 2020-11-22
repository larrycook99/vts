#if !defined(AFX_VTSDEVICESTREEOPTPAGE_H__A27AE452_8FB0_4C79_AAC6_E3B58DD4F724__INCLUDED_)
#define AFX_VTSDEVICESTREEOPTPAGE_H__A27AE452_8FB0_4C79_AAC6_E3B58DD4F724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDevicesTreeOptPage.h : header file
//


#include "VTSPropertyPage.h"

class VTSDevice;

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeOptPage dialog

class VTSDevicesTreeOptPage : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSDevicesTreeOptPage)

	VTSDevice *			m_pdevice;

// Construction
public:
	VTSDevicesTreeOptPage( VTSPageCallbackInterface * pparent );
	VTSDevicesTreeOptPage();		// not used
	~VTSDevicesTreeOptPage();

// Dialog Data
	//{{AFX_DATA(VTSDevicesTreeOptPage)
	enum { IDD = IDD_DEVTREEOPT };
	BOOL m_bAlarmSummary;
	BOOL m_bEventSummary;
	BOOL m_bEventInfo;
	int	m_nEvents;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSDevicesTreeOptPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSDevicesTreeOptPage)
	afx_msg void OnSaveChange();
	//}}AFX_MSG

	void DeviceToCtrl( VTSDevice * pdevice );
	void CtrlToDevice( VTSDevice * pdevice );
	void ClearData(void);
	void EnableControls( bool fEnable );

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDEVICESTREEOPTPAGE_H__A27AE452_8FB0_4C79_AAC6_E3B58DD4F724__INCLUDED_)
