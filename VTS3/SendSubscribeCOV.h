#if !defined(AFX_SENDSUBSCRIBECOV_H__15608460_844E_11D4_BEDC_00A0C95A9812__INCLUDED_)
#define AFX_SENDSUBSCRIBECOV_H__15608460_844E_11D4_BEDC_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendSubscribeCOV.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendSubscribeCOV dialog

class CSendSubscribeCOV : public CSendPage
{
	DECLARE_DYNCREATE(CSendSubscribeCOV)

// Construction
public:
	CSendSubscribeCOV( void );   // non-standard constructor

	VTSUnsignedCtrl				m_ProcessID;
	VTSObjectIdentifierCtrl		m_ObjectID;
	VTSBooleanCtrl				m_IssueConfirmed;
	VTSUnsignedCtrl				m_Lifetime;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendSubscribeCOV)
	enum { IDD = IDD_SENDSUBSCRIBECOV };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendSubscribeCOV)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendSubscribeCOV)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeProcessID();
	afx_msg void OnChangeObjectID();
	afx_msg void OnChangeIssueConfirmed();
	afx_msg void OnChangeLifetime();
	afx_msg void OnObjectIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDSUBSCRIBECOV_H__15608460_844E_11D4_BEDC_00A0C95A9812__INCLUDED_)
