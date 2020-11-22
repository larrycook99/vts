#if !defined(AFX_VTSDEVICESDLG_H__86B18060_B765_11D5_8886_00A0C9E370F1__INCLUDED_)
#define AFX_VTSDEVICESDLG_H__86B18060_B765_11D5_8886_00A0C9E370F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDevicesDlg.h : header file
//

#include "VTSDoc.h"

// NOTE: This file is no longer part of VTS.
// It has been superceded by VTSDevicesTreeDevPage.

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesDlg dialog

class VTSDevicesDlg : public CDialog
{
	VTSDevices *	m_pdevices;
	VTSDevices  	m_devicesLocal;

// Construction
public:
	VTSDevicesDlg(CWnd* pParent = NULL);	// standard constructor (not used)
//MAD_DB	VTSDevicesDlg( VTSDeviceListPtr dlp );	// ctor
//	VTSDevicesDlg( VTSDeviceListPtr dlp, VTSDevices * pdevices );	// ctor
	VTSDevicesDlg( VTSDevices * pdevices );	// ctor

	void InitDeviceList( void );		// init the device list

	void SetSelection( int indx );
	void ResetSelection( void );		// nothing selected
	void SynchronizeControls( void );	// sync list and controls

	void SaveChanges( void );			// new values to update selected name

	void DeviceToList( VTSDevice * pdevice, int elem );
	void DeviceToCtrl( VTSDevice * pdevice );
	void CtrlToDevice( VTSDevice * pdevice );
//	void CtrlToDevice( VTSDevicePtr dp );

// Dialog Data
	//{{AFX_DATA(VTSDevicesDlg)
	enum { IDD = IDD_DEVICES };
	CListCtrl	m_DeviceList;
	CString		m_Name;
/* MAD_DB
	CString		m_Instance;
	CString		m_SegSize;
	CString		m_WindowSize;
	CString		m_NextInvokeID;
	CString		m_MaxAPDUSize;
	CString		m_APDUTimeout;
	CString		m_APDUSegTimeout;
	CString		m_APDURetries;
	CString		m_VendorID;
*/
	int		m_nInstance;
	int		m_nSegSize;
	int		m_nWindowSize;
	int		m_nNextInvokeID;
	int		m_nMaxAPDUSize;
	int		m_nAPDUTimeout;
	int		m_nAPDUSegTimeout;
	int		m_nAPDURetries;
	int		m_nVendorID;
	int		m_Segmentation;
	int		m_nRouter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDevicesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//MAD_DB	VTSDeviceListPtr	m_pDeviceList;					// pointer to list of Devices
	int					m_iSelectedDevice;				// index of selected name

	// Generated message map functions
	//{{AFX_MSG(VTSDevicesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnItemchangingDevicelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDevicelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIAm();
	afx_msg void OnObjProp();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDEVICESDLG_H__86B18060_B765_11D5_8886_00A0C9E370F1__INCLUDED_)
