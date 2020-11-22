#if !defined(AFX_VTSLISTOFPROPERTYSTATESDLG_H__45C491EC_7A39_4B2C_B72D_DA9EDB56C5C8__INCLUDED_)
#define AFX_VTSLISTOFPROPERTYSTATESDLG_H__45C491EC_7A39_4B2C_B72D_DA9EDB56C5C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSListOfPropertyStatesDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSPropertyStatesDlg.h"
/////////////////////////////////////////////////////////////////////////////
// VTSListOfPropertyStatesDlg dialog

class VTSListOfPropertyStatesDlg : public CDialog
{
// Construction
public:
	VTSListOfPropertyStatesDlg(CWnd* pParent = NULL);   // standard constructor
	~VTSListOfPropertyStatesDlg();
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	CList<VTSPropertyStatesDlg*,VTSPropertyStatesDlg*> m_ListOfpropstates;
// Dialog Data
	//{{AFX_DATA(VTSListOfPropertyStatesDlg)
	enum { IDD = IDD_LISTOFPROPERTYSTATES };
	CListBox	m_listOfItem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSListOfPropertyStatesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSListOfPropertyStatesDlg)
	afx_msg void OnListofAdd();
	afx_msg void OnListofDelete();
	afx_msg void OnListofModify();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLISTOFPROPERTYSTATESDLG_H__45C491EC_7A39_4B2C_B72D_DA9EDB56C5C8__INCLUDED_)
