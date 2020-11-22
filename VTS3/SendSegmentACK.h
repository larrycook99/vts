#if !defined(AFX_SENDSEGMENTACK_H__B18767C1_5558_11D4_BEBE_00A0C95A9812__INCLUDED_)
#define AFX_SENDSEGMENTACK_H__B18767C1_5558_11D4_BEBE_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendSegmentACK.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendSegmentACK dialog

class CSendSegmentACK : public CSendPage
{
	DECLARE_DYNCREATE(CSendSegmentACK)

// Construction
public:
	CSendSegmentACK( void );   // non-standard constructor

	VTSIntegerCtrl			m_InvokeID;
	VTSIntegerCtrl			m_SeqNum;
	VTSIntegerCtrl			m_WindowSize;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendSegmentACK)
	enum { IDD = IDD_SENDSEGMENTACK };
	BOOL	m_NAK;
	int		m_SRV;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendSegmentACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendSegmentACK)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeInvokeID();
	afx_msg void OnChangeSeqNum();
	afx_msg void OnChangeWindowSize();
	afx_msg void OnNAK();
	afx_msg void OnClient();
	afx_msg void OnServer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDSEGMENTACK_H__B18767C1_5558_11D4_BEBE_00A0C95A9812__INCLUDED_)
