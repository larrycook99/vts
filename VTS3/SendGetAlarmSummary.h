#if !defined(AFX_SENDGETALARMSUMMARY_H__A5F46EB4_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
#define AFX_SENDGETALARMSUMMARY_H__A5F46EB4_797F_11D4_BED9_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendGetAlarmSummary.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendGetAlarmSummary dialog

class CSendGetAlarmSummary : public CSendPage
{
	DECLARE_DYNCREATE(CSendGetAlarmSummary)

// Construction
public:
	CSendGetAlarmSummary( void );   // non-standard constructor

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

// Dialog Data
	//{{AFX_DATA(CSendGetAlarmSummary)
	enum { IDD = IDD_SENDGETALARMSUMMARY };
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendGetAlarmSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendGetAlarmSummary)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDGETALARMSUMMARY_H__A5F46EB4_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
