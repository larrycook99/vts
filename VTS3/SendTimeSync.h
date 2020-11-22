#if !defined(AFX_SENDTIMESYNC_H__E40CE461_5C68_11D4_BEC3_00A0C95A9812__INCLUDED_)
#define AFX_SENDTIMESYNC_H__E40CE461_5C68_11D4_BEC3_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendTimeSync.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendTimeSync dialog

class CSendTimeSync : public CSendPage
{
	DECLARE_DYNCREATE(CSendTimeSync)

// Construction
public:
	CSendTimeSync( void );							// non-standard constructor

	VTSDateCtrl				m_Date;
	VTSTimeCtrl				m_Time;

	void SynchronizeControls( void );

	void InitPage( void );						// get read for input
	void EncodePage( CByteArray* contents );	// encode the page into contents

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendTimeSync)
	enum { IDD = IDD_SENDTIMESYNC };
	BOOL	m_AutoSync;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendTimeSync)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendTimeSync)
	afx_msg void OnAutoSync();
	afx_msg void OnChangeDate();
	afx_msg void OnChangeTime();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDTIMESYNC_H__E40CE461_5C68_11D4_BEC3_00A0C95A9812__INCLUDED_)
