#if !defined(AFX_VTSACTIONLISTDLG_H__2C3A4A09_C873_427C_925E_F41096D69306__INCLUDED_)
#define AFX_VTSACTIONLISTDLG_H__2C3A4A09_C873_427C_925E_F41096D69306__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSActionListDlg.h : header file
//

#include "VTSCtrl.h"
#include "VTSActionCommandDlg.h"
/////////////////////////////////////////////////////////////////////////////
// VTSListOfDestinationDlg dialog
class VTSActionListDlg : public CDialog
{
// Construction
public:
	VTSActionListDlg(CWnd* pParent = NULL);   // standard constructor
	~VTSActionListDlg();
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	CList<VTSActionCommandDlg*,VTSActionCommandDlg*> m_ListOfactioncommand;
// Dialog Data
	//{{AFX_DATA(VTSListOfDestinationDlg)
	enum { IDD = IDD_ACTIONLIST };
	CListBox	m_listOfItem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSActionListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSActionListDlg)
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

#endif // !defined(AFX_VTSACTIONLISTDLG_H__2C3A4A09_C873_427C_925E_F41096D69306__INCLUDED_)
