#if !defined(AFX_SENDDELETEFDTENTRY_H__8B9C8FD4_515B_11D4_BEBB_00A0C95A9812__INCLUDED_)
#define AFX_SENDDELETEFDTENTRY_H__8B9C8FD4_515B_11D4_BEBB_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendDeleteFDTEntry.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendDeleteFDTEntry dialog

class CSendDeleteFDTEntry : public CSendPage
{
	DECLARE_DYNCREATE(CSendDeleteFDTEntry)

// Construction
public:
	CSendDeleteFDTEntry( void );				// non-standard constructor

	VTSIPAddrCtrl	m_ADDR;						// device address

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(  int index = 0  );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendDeleteFDTEntry)
	enum { IDD = IDD_SENDDELETEFDTENTRY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendDeleteFDTEntry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendDeleteFDTEntry)
	afx_msg void OnChangeADDR();
	afx_msg void OnSelchangeAddrCombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDDELETEFDTENTRY_H__8B9C8FD4_515B_11D4_BEBB_00A0C95A9812__INCLUDED_)
