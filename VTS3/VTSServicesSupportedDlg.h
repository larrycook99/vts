#if !defined(AFX_VTSSERVICESSUPPORTEDDLG_H__1BA6B606_476B_4D22_8614_48BE5FFF350C__INCLUDED_)
#define AFX_VTSSERVICESSUPPORTEDDLG_H__1BA6B606_476B_4D22_8614_48BE5FFF350C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSServicesSupportedDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSServicesSupportedDlg dialog

class VTSServicesSupportedDlg : public CDialog
{
// Construction
public:
	VTSServicesSupportedDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
// Dialog Data
	//{{AFX_DATA(VTSServicesSupportedDlg)
	enum { IDD = IDD_SERVICESSUPPORTED };
	BOOL	m_ackalarm;
	BOOL	m_addlist;
	BOOL	m_areadfile;
	BOOL	m_authen;
	BOOL	m_awritefile;
	BOOL	m_cfmcovn;
	BOOL	m_cfmeven;
	BOOL	m_cfmprvtrans;
	BOOL	m_cfmtextmsg;
	BOOL	m_creatobj;
	BOOL	m_delobj;
	BOOL	m_devcomctrl;
	BOOL	m_getalarmsum;
	BOOL	m_getenrsum;
	BOOL	m_getevninfo;
	BOOL	m_iam;
	BOOL	m_ihave;
	BOOL	m_lfsftyo;
	BOOL	m_readprop;
	BOOL	m_readpropc;
	BOOL	m_readpropm;
	BOOL	m_readrange;
	BOOL	m_reinitdev;
	BOOL	m_requestkey;
	BOOL	m_rmlist;
	BOOL	m_subscobprop;
	BOOL	m_subscov;
	BOOL	m_timesyn;
	BOOL	m_ucfmcovn;
	BOOL	m_ucfmeven;
	BOOL	m_ucfmprvtr;
	BOOL	m_ucfmtextmsg;
	BOOL	m_utctimesyn;
	BOOL	m_vtclose;
	BOOL	m_vtdata;
	BOOL	m_vtopen;
	BOOL	m_whohas;
	BOOL	m_whois;
	BOOL	m_writeprop;
	BOOL	m_writepropm;
	BOOL	m_SelectAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSServicesSupportedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSServicesSupportedDlg)
	afx_msg void OnSelectall();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSSERVICESSUPPORTEDDLG_H__1BA6B606_476B_4D22_8614_48BE5FFF350C__INCLUDED_)
