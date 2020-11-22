#if !defined(AFX_SENDCOMPLEXACK_H__17BFA1F1_5F84_11D4_BEC5_00A0C95A9812__INCLUDED_)
#define AFX_SENDCOMPLEXACK_H__17BFA1F1_5F84_11D4_BEC5_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendComplexACK.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendComplexACK dialog

class CSendComplexACK : public CSendPage
{
	DECLARE_DYNCREATE(CSendComplexACK)

// Construction
public:
	CSendComplexACK( void );				// non-standard constructor

	VTSIntegerCtrl	m_SegmentSize;				// automatic segmenting chunk size

	VTSIntegerCtrl	m_SeqNumber;				// sequence number
	VTSIntegerCtrl	m_WindowSize;				// proposed window size
	VTSIntegerCtrl	m_InvokeID;					// invoke ID

	void SynchronizeControls( void );

	void InitPage( void );						// get read for input
	void EncodePage( CByteArray* contents );	// encode the page into contents

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(  int index = 0  );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendComplexACK)
	enum { IDD = IDD_SENDCOMPLEXACK };
	BOOL	m_Segmented;
	BOOL	m_MoreFollows;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendComplexACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendComplexACK)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSegmentSize();
	afx_msg void OnSegmented();
	afx_msg void OnMoreFollows();
	afx_msg void OnChangeSeqNumber();
	afx_msg void OnChangeWindowSize();
	afx_msg void OnChangeInvokeID();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDCOMPLEXACK_H__17BFA1F1_5F84_11D4_BEC5_00A0C95A9812__INCLUDED_)
