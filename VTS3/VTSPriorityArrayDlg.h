#if !defined(AFX_VTSPRIORITYARRAYDLG_H__D5EB8F66_CD84_4071_A279_11E47C379F44__INCLUDED_)
#define AFX_VTSPRIORITYARRAYDLG_H__D5EB8F66_CD84_4071_A279_11E47C379F44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPriorityArrayDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSPriorityValueDlg.h"
/////////////////////////////////////////////////////////////////////////////
// VTSPriorityArrayDlg dialog

class VTSPriorityArrayDlg : public CDialog
{
// Construction
public:
	VTSPriorityArrayDlg(CWnd* pParent = NULL);   // standard constructor
	~VTSPriorityArrayDlg();
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	CList<VTSPriorityValueDlg*,VTSPriorityValueDlg*> m_ListOfPriorityValue;
// Dialog Data
	//{{AFX_DATA(VTSPriorityArrayDlg)
	enum { IDD = IDD_PRIORITYARRAY };
	CListBox	m_listOfItem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPriorityArrayDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPriorityArrayDlg)
	afx_msg void OnListofAdd();
	afx_msg void OnListofDelete();
	afx_msg void OnListofModify();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPRIORITYARRAYDLG_H__D5EB8F66_CD84_4071_A279_11E47C379F44__INCLUDED_)
