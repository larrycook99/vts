#if !defined(AFX_VTSDEVICESTREEPROPPAGE_H__B4170031_0D75_4455_BC47_4EE65FF96473__INCLUDED_)
#define AFX_VTSDEVICESTREEPROPPAGE_H__B4170031_0D75_4455_BC47_4EE65FF96473__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDevicesTreePropPage.h : header file
//

#include "VTSPropertyPage.h"

class VTSDevProperty;

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreePropPage dialog

class VTSDevicesTreePropPage : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSDevicesTreePropPage)

	VTSDevProperty *	m_pdevproperty;

// Construction
public:
	VTSDevicesTreePropPage( VTSPageCallbackInterface * pparent );
	VTSDevicesTreePropPage();		// not used
	~VTSDevicesTreePropPage();

// Dialog Data
	//{{AFX_DATA(VTSDevicesTreePropPage)
	enum { IDD = IDD_DEVTREEPROP };
	CComboBox	m_propCombo;
	int		m_nPropID;
	BOOL	m_fIsArray;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSDevicesTreePropPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSDevicesTreePropPage)
	afx_msg void OnSelChangePropCombo();
	afx_msg void OnChangePropID();
	afx_msg void OnPropIsArray();
	//}}AFX_MSG

	void PropToCtrl( VTSDevProperty * pdevproperty );
	void CtrlToProp( VTSDevProperty * pdevproperty );
	
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDEVICESTREEPROPPAGE_H__B4170031_0D75_4455_BC47_4EE65FF96473__INCLUDED_)
