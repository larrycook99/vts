#if !defined(AFX_SENDCREATEOBJECTACK_H__8D124EB3_78A7_11D4_BED8_00A0C95A9812__INCLUDED_)
#define AFX_SENDCREATEOBJECTACK_H__8D124EB3_78A7_11D4_BED8_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendCreateObjectACK.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendCreateObjectACK dialog

class CSendCreateObjectACK : public CSendPage
{
	DECLARE_DYNCREATE(CSendCreateObjectACK)

// Construction
public:
	CSendCreateObjectACK( void );   // non-standard constructor

	VTSObjectIdentifierCtrl		m_ObjectID;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(  int index = 0  );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendCreateObjectACK)
	enum { IDD = IDD_SENDCREATEOBJECTACK };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendCreateObjectACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendCreateObjectACK)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeObjectID();
	afx_msg void OnObjectIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDCREATEOBJECTACK_H__8D124EB3_78A7_11D4_BED8_00A0C95A9812__INCLUDED_)
