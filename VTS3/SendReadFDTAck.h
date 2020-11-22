#if !defined(AFX_SENDREADFDTACK_H__8B9C8FD5_515B_11D4_BEBB_00A0C95A9812__INCLUDED_)
#define AFX_SENDREADFDTACK_H__8B9C8FD5_515B_11D4_BEBB_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadFDTAck.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendReadFDTAck dialog

class CSendReadFDTAck : public CSendPage
{
	DECLARE_DYNCREATE(CSendReadFDTAck)

// Construction
public:
	CSendReadFDTAck( void );   // non-standard constructor

	VTSListCtrl		m_FDTCtrl;					// list of FDT entries

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendReadFDTAck)
	enum { IDD = IDD_SENDREADFDTACK };
	CListCtrl	m_FDTList;
	CEdit	m_port;
	CIPAddressCtrl	m_IP;
	int		m_portInt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendReadFDTAck)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendReadFDTAck)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddFDT();
	afx_msg void OnRemoveFDT();
	afx_msg void OnChangeIP();
	afx_msg void OnChangePort();
	afx_msg void OnChangeTTL();
	afx_msg void OnChangeTRemain();
	afx_msg void OnItemchangingFDTList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADFDTACK_H__8B9C8FD5_515B_11D4_BEBB_00A0C95A9812__INCLUDED_)
