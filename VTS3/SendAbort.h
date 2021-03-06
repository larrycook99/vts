#if !defined(AFX_SENDABORT_H__468353B2_5606_11D4_BEBF_00A0C95A9812__INCLUDED_)
#define AFX_SENDABORT_H__468353B2_5606_11D4_BEBF_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendAbort.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendAbort dialog

class CSendAbort : public CSendPage
{
	DECLARE_DYNCREATE(CSendAbort)

// Construction
public:
	CSendAbort( void );   // non-standard constructor

	VTSIntegerCtrl			m_InvokeID;
	VTSEnumeratedCtrl		m_AbortCombo;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(  int index = 0  );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendAbort)
	enum { IDD = IDD_SENDABORT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendAbort)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendAbort)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeInvokeID();
	afx_msg void OnSelchangeAbortCombo();
	afx_msg void OnClient();
	afx_msg void OnServer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDABORT_H__468353B2_5606_11D4_BEBF_00A0C95A9812__INCLUDED_)
