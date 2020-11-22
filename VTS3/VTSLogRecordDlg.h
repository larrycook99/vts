#if !defined(AFX_VTSLOGRECORDDLG_H__465E0939_A17C_4125_8440_D5ABD4FDC904__INCLUDED_)
#define AFX_VTSLOGRECORDDLG_H__465E0939_A17C_4125_8440_D5ABD4FDC904__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSLogRecordDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSAny.h"
/////////////////////////////////////////////////////////////////////////////
// VTSLogRecordDlg dialog

class VTSLogRecordDlg : public CDialog
{
// Construction
public:
	VTSLogRecordDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode( BACnetAPDUDecoder& dec );
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void EnableChoice(int nchoice);
	int m_nSelItem;
	VTSDateCtrl                 m_DateCtrl;
	VTSTimeCtrl                 m_TimeCtrl;
	VTSBooleanCtrl				m_LogDisabled;
	VTSBooleanCtrl				m_BufferPurged;
	VTSBooleanCtrl				m_boolValue;
	VTSRealCtrl					m_realValue;
	VTSUnsignedCtrl 			m_enumValue;
	VTSUnsignedCtrl				m_uintValue;
	VTSIntegerCtrl				m_intValue;
	VTSBitStringCtrl			m_bitstringValue;
	VTSEnumeratedCtrl			m_errorClass;
	VTSEnumeratedCtrl			m_errorCode;
	VTSRealCtrl					m_TimeChange;	
	VTSAny						m_anyValue;
	VTSStatusFlags				m_StatusFlags;
// Dialog Data
	//{{AFX_DATA(VTSLogRecordDlg)
	enum { IDD = IDD_LOGRECORD };
	CComboBox	m_DatumChoice;
	BOOL	m_hasstatusflags;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSLogRecordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSLogRecordDlg)
	afx_msg void OnChangeDatetimeDate();
	afx_msg void OnChangeDatetimeTime();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeUnsignedvalue();
	afx_msg void OnChangeTimechange();
	afx_msg void OnChangeRealvalue();
	afx_msg void OnOverridden();
	afx_msg void OnOutofservice();
	afx_msg void OnLogdisabled();
	afx_msg void OnChangeIntergervalue();
	afx_msg void OnInalarm();
	afx_msg void OnFault();
	afx_msg void OnSelchangeErrorcode();
	afx_msg void OnSelchangeErrorclass();
	afx_msg void OnChangeEnumvalue();
	afx_msg void OnBufferpurged();
	afx_msg void OnBoolvalue();
	afx_msg void OnChangeBitstringvalue();
	afx_msg void OnAny();
	afx_msg void OnSelchangeLogdatum();
	afx_msg void OnHasstatusflags();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLOGRECORDDLG_H__465E0939_A17C_4125_8440_D5ABD4FDC904__INCLUDED_)
