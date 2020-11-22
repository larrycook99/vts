#if !defined(AFX_VTSDEVICESTREEDEVPAGE_H__A27AE452_8FB0_4C79_AAC6_E3B58DD4F724__INCLUDED_)
#define AFX_VTSDEVICESTREEDEVPAGE_H__A27AE452_8FB0_4C79_AAC6_E3B58DD4F724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDevicesTreeDevPage.h : header file
//


#include "VTSPropertyPage.h"

class VTSDevice;

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeDevPage dialog

class VTSDevicesTreeDevPage : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSDevicesTreeDevPage)

	VTSDevice *			m_pdevice;

// Construction
public:
	VTSDevicesTreeDevPage( VTSPageCallbackInterface * pparent );
	VTSDevicesTreeDevPage();		// not used
	~VTSDevicesTreeDevPage();

// Dialog Data
	//{{AFX_DATA(VTSDevicesTreeDevPage)
	enum { IDD = IDD_DEVTREEDEV };
	CString	m_Name;
	int		m_nInstance;
	int		m_nSegSize;
	int		m_nMaxSegs;
	int		m_nWindowSize;
	int		m_nNextInvokeID;
	int		m_nMaxAPDUSize;
	int		m_nAPDUTimeout;
	int		m_nAPDUSegTimeout;
	int		m_nAPDURetries;
	int		m_nVendorID;
	int		m_Segmentation;
	int		m_nRouter;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSDevicesTreeDevPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSDevicesTreeDevPage)
	afx_msg void OnIAm();
	afx_msg void OnSaveNameChange();
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

#endif // !defined(AFX_VTSDEVICESTREEDEVPAGE_H__A27AE452_8FB0_4C79_AAC6_E3B58DD4F724__INCLUDED_)
