#if !defined(AFX_SENDREJECTMTN_H__7DC66982_5224_11D4_BEBC_00A0C95A9812__INCLUDED_)
#define AFX_SENDREJECTMTN_H__7DC66982_5224_11D4_BEBC_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendRejectMTN.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendRejectMTN dialog

class CSendRejectMTN : public CSendPage
{
	DECLARE_DYNCREATE(CSendRejectMTN)

// Construction
public:
	CSendRejectMTN( void );   // non-standard constructor

	VTSIntegerCtrl	m_Reason;					// reject reason
	VTSIntegerCtrl	m_DNET;						// DNET

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendRejectMTN)
	enum { IDD = IDD_SENDREJECTMTN };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendRejectMTN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendRejectMTN)
	afx_msg void OnChangeReason();
	afx_msg void OnChangeDNET();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREJECTMTN_H__7DC66982_5224_11D4_BEBC_00A0C95A9812__INCLUDED_)
