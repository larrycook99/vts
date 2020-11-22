#if !defined(AFX_SENDWRITEFILE_H__52DCE7D0_6D3D_11D4_BECF_00A0C95A9812__INCLUDED_)
#define AFX_SENDWRITEFILE_H__52DCE7D0_6D3D_11D4_BECF_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendWriteFile.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendWriteFile dialog

class CSendWriteFile : public CSendPage
{
	DECLARE_DYNCREATE(CSendWriteFile)

// Construction
public:
	CSendWriteFile( void );   // non-standard constructor

	VTSObjectIdentifierCtrl		m_FileID;
	VTSIntegerCtrl				m_StartPosition;
	VTSOctetStringCtrl			m_Data;			// stream data
	VTSIntegerCtrl				m_StartRecord;
	VTSListCtrl					m_RecordCtrl;	// list of octet strings

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendWriteFile)
	enum { IDD = IDD_SENDWRITEFILE };
	CListCtrl	m_RecordList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendWriteFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendWriteFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeFileID();
	afx_msg void OnChangeStartPosition();
	afx_msg void OnChangeData();
	afx_msg void OnChangeStartRecord();
	afx_msg void OnAddRecord();
	afx_msg void OnRemoveRecord();
	afx_msg void OnChangeRecordData();
	afx_msg void OnItemchangingRecordList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDWRITEFILE_H__52DCE7D0_6D3D_11D4_BECF_00A0C95A9812__INCLUDED_)
