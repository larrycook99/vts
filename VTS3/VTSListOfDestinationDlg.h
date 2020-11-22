#if !defined(AFX_VTSListOfDlg_H__05C49D7E_9952_4027_BEE4_AF7E45A0F21E__INCLUDED_)
#define AFX_VTSListOfDlg_H__05C49D7E_9952_4027_BEE4_AF7E45A0F21E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSListOfDestinationDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSListOfDestinationDlg dialog
class VTSListOfDestinationDlg : public CDialog
{
// Construction
public:
	VTSListOfDestinationDlg(int ItemType = -1, CWnd* pParent = NULL);   // standard constructor
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	BACnetListOf<BACnetDestination> m_ListOfDestination;
	int m_nItemType;

// Dialog Data
	//{{AFX_DATA(VTSListOfDestinationDlg)
	enum { IDD = IDD_LISTOF_DESTINATION };
	CListBox	m_listOfItem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSListOfDestinationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSListOfDestinationDlg)
	afx_msg void OnListOfAdd();
	afx_msg void OnListOfDelete();
	afx_msg void OnListOfModify();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSListOfDlg_H__05C49D7E_9952_4027_BEE4_AF7E45A0F21E__INCLUDED_)
