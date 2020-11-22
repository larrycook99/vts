#if !defined(AFX_VTSNAMESDLG_H__BBCF24E1_6DA8_11D4_BED0_00A0C95A9812__INCLUDED_)
#define AFX_VTSNAMESDLG_H__BBCF24E1_6DA8_11D4_BED0_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNamesDlg.h : header file
//

#include "VTSDoc.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNamesDlg dialog

class VTSNamesDlg : public CDialog
{
// Construction
public:
//MAD_DB	VTSNamesDlg(CWnd* pParent = NULL);	// standard constructor (not used)
//MAD_DB	VTSNamesDlg( VTSNameListPtr nlp, VTSPortListPtr plp );	// ctor
	VTSNamesDlg( VTSNames * pnames, VTSPorts * pports, CWnd* pParent = NULL  );	// ctor

	void InitPortList( void );			// init the port pop-up list
	void InitNameList( void );			// init the CListCtrl of existing names

	void ResetSelection( void );		// nothing selected
	void SynchronizeControls( void );	// sync list and controls
	void SaveChanges( void );			// new values to update selected name

//MAD_DB
//	void NameToList( const VTSNameDesc &name, int elem );
	void NameToList( const VTSName * pname, int elem );
//	void NameToCtrl( const VTSNameDesc &name );
	void NameToCtrl( const VTSName * pname );
//	void CtrlToName( VTSNameDesc &name );
	void CtrlToName( VTSName * pname );

// Dialog Data
	//{{AFX_DATA(VTSNamesDlg)
	enum { IDD = IDD_NAMES };
	CListCtrl	m_NameList;
    CEdit       m_AddressCtrl;  
	CString		m_Name;
	CString		m_Address;
	CComboBox	m_PortCombo;
	CString		m_Network;
	int			m_AddrType;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSNamesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//MAD_DB	VTSNameListPtr	m_pNameList;						// pointer to list of names
//MAD_DB	VTSPortListPtr	m_pPortList;						// pointer to list of ports
	VTSPorts *		m_pports;
	VTSNames		m_names;							// copy of names for dlg manipulation
	VTSNames *		m_pnames;							// pointer to original names list for copy OnOK

	int				m_iSelectedName;					// index of selected name
	bool			m_fWarnedAlready;

	// Generated message map functions
	//{{AFX_MSG(VTSNamesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnDelete();
	afx_msg void OnImportNames();
	afx_msg void OnExportNames();
	afx_msg void OnItemchangingNamelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkNamelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusAddress();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNAMESDLG_H__BBCF24E1_6DA8_11D4_BED0_00A0C95A9812__INCLUDED_)
