#if !defined(AFX_SENDRAW_H__C285DEF1_3F51_11D4_BEAF_00A0C95A9812__INCLUDED_)
#define AFX_SENDRAW_H__C285DEF1_3F51_11D4_BEAF_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendRaw.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendRaw dialog

class CSendRaw : public CSendPage
{
	DECLARE_DYNCREATE(CSendRaw)

// Construction
public:
	CSendRaw( void );   // non-standard constructor

	VTSOctetStringCtrl	m_data;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(int index = 0);					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendRaw)
	enum { IDD = IDD_SENDRAW };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendRaw)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendRaw)
	afx_msg void OnUpdateData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDRAW_H__C285DEF1_3F51_11D4_BEAF_00A0C95A9812__INCLUDED_)
