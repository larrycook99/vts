#if !defined(AFX_SENDREADFILEACK_H__E39D75D2_6CEB_11D4_BECE_00A0C95A9812__INCLUDED_)
#define AFX_SENDREADFILEACK_H__E39D75D2_6CEB_11D4_BECE_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadFileACK.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendReadFileACK dialog

class CSendReadFileACK : public CSendPage
{
	DECLARE_DYNCREATE(CSendReadFileACK)

// Construction
public:
	CSendReadFileACK( void );   // non-standard constructor

	VTSBooleanCtrl				m_EOF;
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
	//{{AFX_DATA(CSendReadFileACK)
	enum { IDD = IDD_SENDREADFILEACK };
	CListCtrl	m_RecordList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendReadFileACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendReadFileACK)
	virtual BOOL OnInitDialog();
	afx_msg void OnEOF();
	afx_msg void OnChangeStartPosition();
	afx_msg void OnChangeData();
	afx_msg void OnChangeStartRecord();
	afx_msg void OnAddRecord();
	afx_msg void OnRemoveRecord();
	afx_msg void OnChangeRecordData();
	afx_msg void OnItemchangingRecordList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADFILEACK_H__E39D75D2_6CEB_11D4_BECE_00A0C95A9812__INCLUDED_)
