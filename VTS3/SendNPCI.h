#if !defined(AFX_SENDNPCI_H__7BC0D0A1_4330_11D4_BEB1_00A0C95A9812__INCLUDED_)
#define AFX_SENDNDPU_H__7BC0D0A1_4330_11D4_BEB1_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendNPCI.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendNPCI dialog

class CSendNPCI : public CSendPage
{
	DECLARE_DYNCREATE(CSendNPCI)

// Construction
public:
	CSendNPCI( void );							// non-standard constructor

	bool	m_isNetMessage;						// iff network layer message

	VTSIntegerCtrl		m_Version;				// Version

	VTSIntegerCtrl		m_DNET;					// DNET
	VTSRemoteAddrCtrl	m_DADR;					// DADR
	VTSIntegerCtrl		m_HopCount;				// hop count
	VTSIntegerCtrl		m_SNET;					// SNET
	VTSRemoteAddrCtrl	m_SADR;					// SADR
	VTSIntegerCtrl		m_Priority;				// Network priority

	static  int historyCount;           //Xiao Shiyuan 2002-12-5
	static  int curHistoryIndex;        //Xiao Shiyuan 2002-12-5

	void SynchronizeControls( void );

	void InitPage( void );						// get read for input
	void EncodePage( CByteArray* contents );	// encode the page into contents

	static BACnetAPDUEncoder	pageContents[glMaxHistoryCount];

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendNPCI)
	enum { IDD = IDD_SENDNPCI };
	BOOL	m_DestPresent;
	BOOL	m_SrcPresent;
	BOOL	m_ExpectingReply;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendNPCI)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendNPCI)
	afx_msg void OnDestPresent();
	afx_msg void OnSrcPresent();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeVersion();
	afx_msg void OnChangeDNET();
	afx_msg void OnChangeDADR();
	afx_msg void OnChangeHopCount();
	afx_msg void OnChangeSNET();
	afx_msg void OnChangeSADR();
	afx_msg void OnExpectingReply();
	afx_msg void OnChangePriority();
	afx_msg void OnSelchangeDADRCombo();
	afx_msg void OnSelchangeSADRCombo();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDNPCI_H__7BC0D0A1_4330_11D4_BEB1_00A0C95A9812__INCLUDED_)
