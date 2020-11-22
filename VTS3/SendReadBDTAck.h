#if !defined(AFX_SENDREADBDTACK_H__8B9C8FD1_515B_11D4_BEBB_00A0C95A9812__INCLUDED_)
#define AFX_SENDREADBDTACK_H__8B9C8FD1_515B_11D4_BEBB_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadBDTAck.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendReadBDTAck dialog

class CSendReadBDTAck : public CSendPage
{
	DECLARE_DYNCREATE(CSendReadBDTAck)

// Construction
public:
	CSendReadBDTAck( void );   // non-standard constructor

	VTSListCtrl		m_BDTCtrl;					// list of BDT entries

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendReadBDTAck)
	enum { IDD = IDD_SENDREADBDTACK };
	CListCtrl	m_BDTList;
	CEdit	m_mask;
	CEdit	m_port;
	CIPAddressCtrl	m_IP;
	int		m_portInt;
	int		m_maskInt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendReadBDTAck)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendReadBDTAck)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddBDT();
	afx_msg void OnRemoveBDT();
	afx_msg void OnChangeBDTEntry();
	afx_msg void OnItemchangingBDTList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeIP();
	afx_msg void OnChangeMask();
	afx_msg void OnChangePort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADBDTACK_H__8B9C8FD1_515B_11D4_BEBB_00A0C95A9812__INCLUDED_)
