#if !defined(AFX_SENDUNCONFTEXTMSG_H__AAAD4FC1_5EAD_11D4_BEC4_00A0C95A9812__INCLUDED_)
#define AFX_SENDUNCONFTEXTMSG_H__AAAD4FC1_5EAD_11D4_BEC4_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendUnconfTextMsg.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendUnconfTextMsg dialog

class CSendUnconfTextMsg : public CSendPage
{
	DECLARE_DYNCREATE(CSendUnconfTextMsg)

// Construction
public:
	CSendUnconfTextMsg( void );   // non-standard constructor

	VTSObjectIdentifierCtrl		m_SourceDevice;
	VTSUnsignedCtrl				m_NumClass;
	VTSCharacterStringCtrl		m_CharClass;
	VTSCharacterStringCtrl		m_Msg;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendUnconfTextMsg)
	enum { IDD = IDD_SENDUNCONFTEXTMSG };
	int		m_Priority;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendUnconfTextMsg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendUnconfTextMsg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSourceDevice();
	afx_msg void OnChangeNumClass();
	afx_msg void OnChangeCharClass();
	afx_msg void OnChangeMsg();
	afx_msg void OnNormal();
	afx_msg void OnUrgent();
	afx_msg void OnSourceDeviceButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDUNCONFTEXTMSG_H__AAAD4FC1_5EAD_11D4_BEC4_00A0C95A9812__INCLUDED_)
