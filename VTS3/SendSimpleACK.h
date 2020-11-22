#if !defined(AFX_SENDSIMPLEACK_H__46A827C1_52F5_11D4_BEBD_00A0C95A9812__INCLUDED_)
#define AFX_SENDSIMPLEACK_H__46A827C1_52F5_11D4_BEBD_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendSimpleACK.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendSimpleACK dialog

class CSendSimpleACK : public CSendPage
{
	DECLARE_DYNCREATE(CSendSimpleACK)

// Construction
public:
	CSendSimpleACK( void );   // non-standard constructor

	VTSIntegerCtrl			m_InvokeID;
	VTSEnumeratedCtrl		m_ServiceCombo;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendSimpleACK)
	enum { IDD = IDD_SENDSIMPLEACK };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendSimpleACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendSimpleACK)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeInvokeID();
	afx_msg void OnSelchangeServiceCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDSIMPLEACK_H__46A827C1_52F5_11D4_BEBD_00A0C95A9812__INCLUDED_)
