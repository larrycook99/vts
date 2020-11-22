#if !defined(AFX_SENDVTOPEN_H__BBCB7D41_7712_11D4_BED7_00A0C95A9812__INCLUDED_)
#define AFX_SENDVTOPEN_H__BBCB7D41_7712_11D4_BED7_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendVTOpen.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendVTOpen dialog

class CSendVTOpen : public CSendPage
{
	DECLARE_DYNCREATE(CSendVTOpen)

// Construction
public:
	CSendVTOpen( void );   // non-standard constructor

	VTSEnumeratedCtrl			m_VTClassCombo;
	VTSUnsignedCtrl				m_SessionID;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendVTOpen)
	enum { IDD = IDD_SENDVTOPEN };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendVTOpen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendVTOpen)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeVTClassCombo();
	afx_msg void OnChangeSessionID();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDVTOPEN_H__BBCB7D41_7712_11D4_BED7_00A0C95A9812__INCLUDED_)
