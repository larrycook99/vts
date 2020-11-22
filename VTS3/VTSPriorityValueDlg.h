#if !defined(AFX_VTSPRIORITYVALUEDLG_H__915DE05E_9CD6_4EEB_BF56_6D70C7B06A19__INCLUDED_)
#define AFX_VTSPRIORITYVALUEDLG_H__915DE05E_9CD6_4EEB_BF56_6D70C7B06A19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPriorityValueDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSAny.h"
/////////////////////////////////////////////////////////////////////////////
// VTSPriorityValueDlg dialog

class VTSPriorityValueDlg : public CDialog
{
// Construction
public:
	VTSPriorityValueDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSRealCtrl         m_RealCtrl;
	VTSEnumeratedCtrl   m_BinaryPV; 
	VTSUnsignedCtrl     m_IntCtrl;
	VTSAny				m_Value;
// Dialog Data
	//{{AFX_DATA(VTSPriorityValueDlg)
	enum { IDD = IDD_PRIORITYVALUE };
	int		m_nChoice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPriorityValueDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPriorityValueDlg)
	afx_msg void OnRadiovalue();
	afx_msg void OnRadioreal();
	afx_msg void OnRadionull();
	afx_msg void OnRadiointeger();
	afx_msg void OnRadiobinary();
	afx_msg void OnChangeInteger();
	afx_msg void OnSelchangeBinarypv();
	afx_msg void OnAnyvalue();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeRealvalue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPRIORITYVALUEDLG_H__915DE05E_9CD6_4EEB_BF56_6D70C7B06A19__INCLUDED_)
