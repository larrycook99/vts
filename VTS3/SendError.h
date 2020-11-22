#if !defined(AFX_SENDERROR_H__B18767C2_5558_11D4_BEBE_00A0C95A9812__INCLUDED_)
#define AFX_SENDERROR_H__B18767C2_5558_11D4_BEBE_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendError.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendError dialog

class CSendError : public CSendPage
{
	DECLARE_DYNCREATE(CSendError)

// Construction
public:
	CSendError( void );   // non-standard constructor

	VTSIntegerCtrl			m_InvokeID;
	VTSEnumeratedCtrl		m_ServiceCombo;
	VTSEnumeratedCtrl		m_ErrorClassCombo;
	VTSEnumeratedCtrl		m_ErrorCodeCombo;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendError)
	enum { IDD = IDD_SENDERROR };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendError)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendError)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeInvokeID();
	afx_msg void OnSelchangeServiceCombo();
	afx_msg void OnSelchangeErrorClassCombo();
	afx_msg void OnSelchangeErrorCodeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDERROR_H__B18767C2_5558_11D4_BEBE_00A0C95A9812__INCLUDED_)
