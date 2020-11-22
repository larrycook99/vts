#if !defined(AFX_SENDENET_H__5BBD51C1_3F35_11D4_BEAF_00A0C95A9812__INCLUDED_)
#define AFX_SENDENET_H__5BBD51C1_3F35_11D4_BEAF_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendEnet.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendEnet dialog

class CSendEnet : public CSendPage
{
	DECLARE_DYNCREATE(CSendEnet)

// Construction
public:
	CSendEnet( void );   // non-standard constructor

	VTSEnetAddrCtrl		m_SADR;
	VTSEnetAddrCtrl		m_DADR;

	void InitPage( void );						// get read for input
	void EncodePage( CByteArray* contents );	// encode the page into contents

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendEnet)
	enum { IDD = IDD_SENDENET };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendEnet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendEnet)
	afx_msg void OnChangeSADR();
	afx_msg void OnChangeDADR();
	afx_msg void OnSelchangeDADRCombo();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSADRCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDENET_H__5BBD51C1_3F35_11D4_BEAF_00A0C95A9812__INCLUDED_)
