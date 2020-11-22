#if !defined(AFX_SENDWRITEFILEACK_H__52DCE7D2_6D3D_11D4_BECF_00A0C95A9812__INCLUDED_)
#define AFX_SENDWRITEFILEACK_H__52DCE7D2_6D3D_11D4_BECF_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendWriteFileACK.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

/////////////////////////////////////////////////////////////////////////////
// SendWriteFileACK dialog

class CSendWriteFileACK : public CSendPage
{
	DECLARE_DYNCREATE(CSendWriteFileACK)

// Construction
public:
	CSendWriteFileACK( void );   // non-standard constructor

	VTSIntegerCtrl				m_StartPosition;
	VTSIntegerCtrl				m_StartRecord;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendWriteFileACK)
	enum { IDD = IDD_SENDWRITEFILEACK };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendWriteFileACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendWriteFileACK)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeStartPosition();
	afx_msg void OnChangeStartRecord();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDWRITEFILEACK_H__52DCE7D2_6D3D_11D4_BECF_00A0C95A9812__INCLUDED_)
