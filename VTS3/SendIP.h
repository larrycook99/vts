#if !defined(AFX_SENDIP_H__5BBD51C2_3F35_11D4_BEAF_00A0C95A9812__INCLUDED_)
#define AFX_SENDIP_H__5BBD51C2_3F35_11D4_BEAF_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendIP.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendIP dialog

class CSendIP : public CSendPage
{
	DECLARE_DYNCREATE(CSendIP)

// Construction
public:
	CSendIP( void );   // non-standard constructor

	VTSIPAddrCtrl	m_DADR;						// destination address

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page into contents

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendIP)
	enum { IDD = IDD_SENDIP };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendIP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendIP)
	afx_msg void OnUpdateDadr();
	afx_msg void OnSelchangeDADRCombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDIP_H__5BBD51C2_3F35_11D4_BEAF_00A0C95A9812__INCLUDED_)
