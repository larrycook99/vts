#if !defined(AFX_SENDBVLCI_H__B4370192_45AE_11D4_BEB3_00A0C95A9812__INCLUDED_)
#define AFX_SENDBVLCI_H__B4370192_45AE_11D4_BEB3_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendBVLCI.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendBVLCI dialog

class CSendBVLCI : public CSendPage
{
	DECLARE_DYNCREATE(CSendBVLCI)

// Construction
public:
	CSendBVLCI( void );   // non-standard constructor

	VTSIPAddrCtrl	m_OADR;						// originating device address

	static  int historyCount;           //Xiao Shiyuan 2002-12-5
	static  int curHistoryIndex;        //Xiao Shiyuan 2002-12-5

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents[glMaxHistoryCount];

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendBVLCI)
	enum { IDD = IDD_SENDBVLCI };
	int		m_HeaderType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendBVLCI)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendBVLCI)
	afx_msg void OnNone();
	afx_msg void OnDistributeBroadcastToNetwork();
	afx_msg void OnOriginalUnicast();
	afx_msg void OnOriginalBroadcast();
	afx_msg void OnChangeOADR();
	afx_msg void OnForwardedNPDU();
	afx_msg void OnSelchangeOADRCombo();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDBVLCI_H__B4370192_45AE_11D4_BEB3_00A0C95A9812__INCLUDED_)
