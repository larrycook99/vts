#if !defined(AFX_SENDREADFILE_H__E39D75D1_6CEB_11D4_BECE_00A0C95A9812__INCLUDED_)
#define AFX_SENDREADFILE_H__E39D75D1_6CEB_11D4_BECE_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadFile.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

/////////////////////////////////////////////////////////////////////////////
// SendReadFile dialog

class CSendReadFile : public CSendPage
{
	DECLARE_DYNCREATE(CSendReadFile)

// Construction
public:
	CSendReadFile( void );   // non-standard constructor

	VTSObjectIdentifierCtrl		m_FileID;
	VTSIntegerCtrl				m_StartPosition;
	VTSUnsignedCtrl				m_OctetCount;
	VTSIntegerCtrl				m_StartRecord;
	VTSUnsignedCtrl				m_RecordCount;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendReadFile)
	enum { IDD = IDD_SENDREADFILE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendReadFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendReadFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeFileID();
	afx_msg void OnChangeStartPosition();
	afx_msg void OnChangeOctetCount();
	afx_msg void OnChangeStartRecord();
	afx_msg void OnChangeRecordCount();
	afx_msg void OnFileIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADFILE_H__E39D75D1_6CEB_11D4_BECE_00A0C95A9812__INCLUDED_)
