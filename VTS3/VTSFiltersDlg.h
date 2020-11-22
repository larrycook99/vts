#if !defined(AFX_VTSFILTERSDLG_H__7E39F604_3F39_4198_A4BB_185142FA6A88__INCLUDED_)
#define AFX_VTSFILTERSDLG_H__7E39F604_3F39_4198_A4BB_185142FA6A88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSFiltersDlg.h : header file
//

#include "VTSDoc.h"

/////////////////////////////////////////////////////////////////////////////
// VTSFiltersDlg dialog

class VTSFiltersDlg : public CDialog
{
// Construction
public:
	VTSFiltersDlg( VTSFilters * pfilters, VTSPorts * pports, CWnd* pParent = NULL  );	// ctor

	void InitPortList( void );			// init the port pop-up list
	void InitFilterList( void );		// init the CListCtrl of existing names

	void ResetSelection( void );		// nothing selected
	void SynchronizeControls( void );	// sync list and controls
	void SaveChanges( void );			// new values to update selected name

	void FilterToList( const VTSFilter * pfilter, int elem );
	void FilterToCtrl( const VTSFilter * pfilter );
	void CtrlToFilter( VTSFilter * pfilter );

// Dialog Data
	//{{AFX_DATA(VTSFiltersDlg)
	enum { IDD = IDD_FILTERS };
	CListCtrl	m_FilterList;
	int			m_FilterType;
	CComboBox	m_PortCombo;
	CComboBox	m_AddrCombo;
	CComboBox	m_AddrTypeCombo;
	CString		m_Address;
	CString		m_Network;
	CComboBox	m_FnGroupCombo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSFiltersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	VTSPorts *		m_pports;
	VTSFilters		m_filters;							// copy of names for dlg manipulation
	VTSFilters *	m_pfilters;							// pointer to original names list for copy OnOK

	int				m_iSelectedFilter;					// index of selected filter
	bool			m_fWarnedAlready;

	// Generated message map functions
	//{{AFX_MSG(VTSFiltersDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnNewFilter();
	afx_msg void OnDeleteFilter();
	afx_msg void OnUpButtonClick();
	afx_msg void OnDownButtonClick();
	afx_msg void OnImportFilters();
	afx_msg void OnExportFilters();
	afx_msg void OnDblclkFilterList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangingFilterList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSFILTERSDLG_H__7E39F604_3F39_4198_A4BB_185142FA6A88__INCLUDED_)
