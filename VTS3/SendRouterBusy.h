#if !defined(AFX_SENDROUTERBUSY_H__7DC66983_5224_11D4_BEBC_00A0C95A9812__INCLUDED_)
#define AFX_SENDROUTERBUSY_H__7DC66983_5224_11D4_BEBC_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendRouterBusy.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendRouterBusy dialog

class CSendRouterBusy : public CSendPage
{
	DECLARE_DYNCREATE(CSendRouterBusy)

// Construction
public:
	CSendRouterBusy( void );   // non-standard constructor

	VTSListCtrl		m_DNETCtrl;					// list of DNET's

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendRouterBusy)
	enum { IDD = IDD_SENDROUTERBUSY };
	CListCtrl	m_DNETList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendRouterBusy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendRouterBusy)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddDNET();
	afx_msg void OnRemoveDNET();
	afx_msg void OnChangeDNET();
	afx_msg void OnItemchangingDNETList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDROUTERBUSY_H__7DC66983_5224_11D4_BEBC_00A0C95A9812__INCLUDED_)
