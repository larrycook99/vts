#if !defined(AFX_VTSEVENTPARAMETERDLG_H__DB22564A_8529_4DC0_9146_CC17AAB46872__INCLUDED_)
#define AFX_VTSEVENTPARAMETERDLG_H__DB22564A_8529_4DC0_9146_CC17AAB46872__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventParameterDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSEventBitstringDlg.h"
#include "VTSEventBufferReadyDlg.h"
#include "VTSEventChangeOfStateDlg.h"
#include "VTSEventChangeOfValueDlg.h"
#include "VTSEventCommandFailureDlg.h"
#include "VTSEventFloatLimitDlg.h"
#include "VTSEventLifeSafetyDlg.h"
#include "VTSEventOutOfRangeDlg.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventParameterDlg dialog

class VTSEventParameterDlg : public CDialog
{
// Construction
public:
	VTSEventParameterDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	int m_nSelItem;
		VTSEventBitstringDlg       m_bitstring;
		VTSEventBufferReadyDlg		m_bufferready;
		VTSEventChangeOfStateDlg	m_state;
		VTSEventChangeOfValueDlg	m_value;
		VTSEventCommandFailureDlg	m_commandfailure;
		VTSEventFloatLimitDlg		m_floatlimit;
		VTSEventLifeSafetyDlg		m_lifesafety;
		VTSEventOutOfRangeDlg		m_outofrange;
// Dialog Data
	//{{AFX_DATA(VTSEventParameterDlg)
	enum { IDD = IDD_EVENTPARAMETER };
	CComboBox	m_choiceCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventParameterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventParameterDlg)
	afx_msg void OnSelchangeChoice();
	afx_msg void OnData();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTPARAMETERDLG_H__DB22564A_8529_4DC0_9146_CC17AAB46872__INCLUDED_)
