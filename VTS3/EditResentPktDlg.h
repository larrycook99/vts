#if !defined(AFX_EDITRESENTPKTDLG_H__8769E5E0_2552_40D8_B242_8E3343ADF6AE__INCLUDED_)
#define AFX_EDITRESENTPKTDLG_H__8769E5E0_2552_40D8_B242_8E3343ADF6AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditResentPktDlg.h : header file
//
#include "BACnetLibrary/BACnet.hpp"

/////////////////////////////////////////////////////////////////////////////
class DesName
{
public:
	CString m_name;
	int     m_index;
};
// CEditResentPktDlg dialog

class CEditResentPktDlg : public CDialog
{
// Construction
public:
	CEditResentPktDlg(CWnd* pParent = NULL);   // standard constructor

	BACnetAddress m_remoteDestination;
	BACnetAddress m_remoteSource;

// Dialog Data
	//{{AFX_DATA(CEditResentPktDlg)
	enum { IDD = IDD_EDITRESENTPKT };
	CEdit	m_sadrCtrl;
	CEdit	m_snetCtrl;
	CEdit	m_dnetCtrl;
	CEdit	m_dadrCtrl;
	CComboBox	m_desCombo;
	CString	m_dadr;
	UINT	m_dnet;
	CString	m_sadr;
	UINT	m_snet;
	BOOL	m_bSnet;
	BOOL	m_bDnet;
	int		m_desIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditResentPktDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	CList<DesName, DesName> m_desArray;
	int m_defaultDes;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditResentPktDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDnetCheck();
	afx_msg void OnSnetCheck();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITRESENTPKTDLG_H__8769E5E0_2552_40D8_B242_8E3343ADF6AE__INCLUDED_)
