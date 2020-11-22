#if !defined(AFX_VTSTIMESTAMPDLG_H__6BC2913F_F383_4503_BF27_DF2D5188E053__INCLUDED_)
#define AFX_VTSTIMESTAMPDLG_H__6BC2913F_F383_4503_BF27_DF2D5188E053__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSTimeStampDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSTimeStampDlg dialog

class VTSTimeStampDlg : public CDialog
{
// Construction
public:
	VTSTimeStampDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );

	int m_nChoice;
	VTSTimeCtrl m_TimeCtrl;
	VTSUnsignedCtrl m_UnsignedCtrl;
	VTSDateCtrl m_DateTimeDateCtrl;
	VTSTimeCtrl m_DateTimeTimeCtrl;
	BACnetTimeStamp m_value;

// Dialog Data
	//{{AFX_DATA(VTSTimeStampDlg)
	enum { IDD = IDD_TIMESTAMP_CHOICE };
	CButton	m_TimeRadio;
	CButton	m_UnsignedRadio;
	CButton	m_DateTimeRadio;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSTimeStampDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSTimeStampDlg)
	afx_msg void OnTimestampTimeradio();
	afx_msg void OnTimestampUnsignedradio();
	afx_msg void OnTimestampDatetimeradio();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSTIMESTAMPDLG_H__6BC2913F_F383_4503_BF27_DF2D5188E053__INCLUDED_)
