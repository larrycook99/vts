#if !defined(AFX_VTSDEVICESTREEDLG_H__0772AA3F_8213_4214_B861_E241FA6A508B__INCLUDED_)
#define AFX_VTSDEVICESTREEDLG_H__0772AA3F_8213_4214_B861_E241FA6A508B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDevicesTreeDlg.h : header file
//

#include "VTSDoc.h"
#include "VTSDevicesTreeDevPage.h"
#include "VTSDevicesTreeOptPage.h"
#include "VTSDevicesTreeObjPage.h"
#include "VTSDevicesTreePropPage.h"
#include "VTSDevicesTreeValuePage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeDlg dialog

//class VTSDevicesTreeDlg : public CPropertySheet
class VTSDevicesTreeDlg : public CDialog, public VTSPageCallbackInterface
{
	VTSDevices *	m_pdevices;
	VTSDevices  	m_devicesLocal;

	HTREEITEM		m_htreeitemSelected;
	CObject	*		m_pnodeSelected;
	CImageList		m_imagelist;

	DECLARE_DYNAMIC(VTSDevicesTreeDlg)

// Construction
public:
	VTSDevicesTreeDlg(VTSDevices * pdevices, CWnd* pParent = NULL);   // standard constructor

	// Define all pages...
	CPropertySheet			m_sheet;
	CPropertyPage			m_pageNull;
	VTSDevicesTreeDevPage	m_pageDev;
	VTSDevicesTreeOptPage	m_pageOpt;
	VTSDevicesTreeObjPage	m_pageObj;
	VTSDevicesTreePropPage	m_pageProp;
	VTSDevicesTreeValuePage	m_pageValue;

	virtual void DataChangeNotification(void);		// call back from pages
	virtual void * GetActiveData(void);

// Dialog Data
	//{{AFX_DATA(VTSDevicesTreeDlg)
	enum { IDD = IDD_DEVICESTREE };
	CTreeCtrl	m_treeDevices;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDevicesTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int GetSelectedNodeLevel(void);
	int FindDataInsertionIndex( CObArray * pobjarray, LPARAM lparam );
	HTREEITEM InsertNode(  int nAddLevel, CObject * pobjectNew );

	void InitTree(void);
	void LoadDevices( VTSDevices * pdevices, HTREEITEM htreeitemParent );
	void LoadObjects( VTSDevObjects * pdevobjects, HTREEITEM htreeitemParent );
	void LoadProperties( VTSDevProperties * pdevproperties, HTREEITEM htreeitemParent );
	void LoadValues( VTSDevValues * pdevvalues, HTREEITEM htreeitemParent, int nLevel );
	int MoveAllChildWindows(HDWP hdwp, HWND hWnd, int x, int y);

	void UpdateNodeName( LPCSTR lpszNodeName );
	CString GetNodeTitle( int nLevel, CObject * pobjNode );

	// Generated message map functions
	//{{AFX_MSG(VTSDevicesTreeDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateNewDevice(CCmdUI* pCmdUI);
	afx_msg void OnNewDevice();
	afx_msg void OnCopyObject();
	afx_msg void OnUpdateCopyObject(CCmdUI* pCmdUI);
	afx_msg void OnNewObject();
	afx_msg void OnUpdateNewObject(CCmdUI* pCmdUI);
	afx_msg void OnNewProperty();
	afx_msg void OnUpdateNewProperty(CCmdUI* pCmdUI);
	afx_msg void OnNewValue();
	afx_msg void OnUpdateNewValue(CCmdUI* pCmdUI);
	afx_msg void OnNewValueComponent();
	afx_msg void OnUpdateNewValueComponent(CCmdUI* pCmdUI);
	afx_msg void OnSelchangedDeviceTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnKeydownDevicetree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExpandItems();
	afx_msg void OnCollapseItems();
	afx_msg void OnSortItems();
	//}}AFX_MSG

	afx_msg void OnExport();
	afx_msg void OnImport();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDEVICESTREEDLG_H__0772AA3F_8213_4214_B861_E241FA6A508B__INCLUDED_)
