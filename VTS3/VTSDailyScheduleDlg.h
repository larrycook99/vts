#if !defined(AFX_VTSDAILYSCHEDULEDLG_H__4C921158_0D7A_4AC9_8D28_AEAAEF4F6D8C__INCLUDED_)
#define AFX_VTSDAILYSCHEDULEDLG_H__4C921158_0D7A_4AC9_8D28_AEAAEF4F6D8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDailyScheduleDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSTimeValueDlg.h"
/////////////////////////////////////////////////////////////////////////////
// VTSDailyScheduleDlg dialog

class VTSDailyScheduleDlg : public CDialog
{
// Construction
public:
	VTSDailyScheduleDlg(CWnd* pParent = NULL);   // standard constructor
	~VTSDailyScheduleDlg();
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	CList<VTSTimeValueDlg*,VTSTimeValueDlg*> m_ListOfTimeValue;
	BOOL m_showAsListofTimeValue;
// Dialog Data
	//{{AFX_DATA(VTSDailyScheduleDlg)
	enum { IDD = IDD_DAILYSCHEDULE };
	CListBox	m_listOfItem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDailyScheduleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSDailyScheduleDlg)
	afx_msg void OnListofAdd();
	afx_msg void OnListofDelete();
	afx_msg void OnListofModify();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDAILYSCHEDULEDLG_H__4C921158_0D7A_4AC9_8D28_AEAAEF4F6D8C__INCLUDED_)
