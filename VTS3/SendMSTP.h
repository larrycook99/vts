// SendMSTP.h: interface for the CSendMSTP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDMSTP_H__0942450E_DC84_44E8_9A42_67D83EB7A099__INCLUDED_)
#define AFX_SENDMSTP_H__0942450E_DC84_44E8_9A42_67D83EB7A099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendMSTP dialog

class CSendMSTP : public CSendPage
{
	DECLARE_DYNCREATE(CSendMSTP)

// Construction
public:
	CSendMSTP( void );   // non-standard constructor

	VTSMSTPAddrCtrl		m_SADR;
	VTSMSTPAddrCtrl		m_DADR;

	void InitPage( void );						// get read for input
	void EncodePage( CByteArray* contents );	// encode the page into contents

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendMSTP)
	enum { IDD = IDD_SENDMSTP };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendMSTP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendMSTP)
	afx_msg void OnChangeSADR();
	afx_msg void OnChangeDADR();
	afx_msg void OnSelchangeDADRCombo();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSADRCombo();
	afx_msg void OnSelchangeMstpfrtypecombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_SENDMSTP_H__0942450E_DC84_44E8_9A42_67D83EB7A099__INCLUDED_)

