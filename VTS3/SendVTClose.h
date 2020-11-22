#if !defined(AFX_SENDVTCLOSE_H__E1F7BE52_7771_11D4_BED7_00A0C95A9812__INCLUDED_)
#define AFX_SENDVTCLOSE_H__E1F7BE52_7771_11D4_BED7_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendVTClose.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendVTClose dialog

class CSendVTClose : public CSendPage
{
	DECLARE_DYNCREATE(CSendVTClose)

// Construction
public:
	CSendVTClose( void );   // non-standard constructor

	VTSListCtrl		m_IDCtrl;					// list of session ID's

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendVTClose)
	enum { IDD = IDD_SENDVTCLOSE };
	CListCtrl	m_IDList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendVTClose)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendVTClose)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddID();
	afx_msg void OnRemoveID();
	afx_msg void OnChangeID();
	afx_msg void OnItemchangingIDList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDVTCLOSE_H__E1F7BE52_7771_11D4_BED7_00A0C95A9812__INCLUDED_)
