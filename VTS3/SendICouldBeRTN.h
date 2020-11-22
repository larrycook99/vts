#if !defined(AFX_SENDICOULDBERTN_H__7DC66981_5224_11D4_BEBC_00A0C95A9812__INCLUDED_)
#define AFX_SENDICOULDBERTN_H__7DC66981_5224_11D4_BEBC_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendICouldBeRTN.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendICouldBeRTN dialog

class CSendICouldBeRTN : public CSendPage
{
	DECLARE_DYNCREATE(CSendICouldBeRTN)

// Construction
public:
	CSendICouldBeRTN( void );   // non-standard constructor

	VTSIntegerCtrl	m_DNET;						// DNET
	VTSIntegerCtrl	m_PerfIndx;					// performance index

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendICouldBeRTN)
	enum { IDD = IDD_SENDICOULDBERTN };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendICouldBeRTN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendICouldBeRTN)
	afx_msg void OnChangeDNET();
	afx_msg void OnChangePerfIndx();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDICOULDBERTN_H__7DC66981_5224_11D4_BEBC_00A0C95A9812__INCLUDED_)
