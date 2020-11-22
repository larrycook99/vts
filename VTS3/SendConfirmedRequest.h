#if !defined(AFX_SENDCONFIRMEDREQUEST_H__AAAD4FC3_5EAD_11D4_BEC4_00A0C95A9812__INCLUDED_)
#define AFX_SENDCONFIRMEDREQUEST_H__AAAD4FC3_5EAD_11D4_BEC4_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendConfirmedRequest.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendConfirmedRequest dialog

class CSendConfirmedRequest : public CSendPage
{
	DECLARE_DYNCREATE(CSendConfirmedRequest)

// Construction
public:
	CSendConfirmedRequest( void );				// non-standard constructor

	VTSIntegerCtrl		m_SegmentSize;			// automatic segmenting chunk size

	VTSIntegerCtrl		m_SeqNumber;			// sequence number
	VTSIntegerCtrl		m_WindowSize;			// proposed window size
	VTSEnumeratedCtrl	m_MaxAPDULen;			// maximum APDU length accepted
	VTSIntegerCtrl		m_InvokeID;				// invoke ID
	VTSEnumeratedCtrl	m_Max_Segments_Accepted;// max segments accepted

	void SynchronizeControls( void );

	void InitPage( void );						// get read for input
	void EncodePage( CByteArray* contents );	// encode the page into contents

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(  int index = 0  );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendConfirmedRequest)
	enum { IDD = IDD_SENDCONFREQUEST };
	BOOL	m_Segmented;
	BOOL	m_MoreFollows;
	BOOL	m_SegResponse;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendConfirmedRequest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendConfirmedRequest)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSegmentSize();
	afx_msg void OnSegmented();
	afx_msg void OnMoreFollows();
	afx_msg void OnSegResponse();
	afx_msg void OnChangeSeqNumber();
	afx_msg void OnChangeWindowSize();
	afx_msg void OnChangeMaxAPDULen();
	afx_msg void OnChangeInvokeID();
	afx_msg void OnSelchangeMaxAPDULen();
	afx_msg void OnSelchangeMaxSegmentsAccepted();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDCONFIRMEDREQUEST_H__AAAD4FC3_5EAD_11D4_BEC4_00A0C95A9812__INCLUDED_)
