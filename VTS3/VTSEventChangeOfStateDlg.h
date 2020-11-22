#if !defined(AFX_VTSEVENTCHANGEOFSTATEDLG_H__A0623535_A28C_4FB1_8F5D_0AD591E7B766__INCLUDED_)
#define AFX_VTSEVENTCHANGEOFSTATEDLG_H__A0623535_A28C_4FB1_8F5D_0AD591E7B766__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventChangeOfStateDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSListOfPropertyStatesDlg.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventChangeOfStateDlg dialog

class VTSEventChangeOfStateDlg : public CDialog
{
// Construction
public:
	VTSEventChangeOfStateDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSUnsignedCtrl				 	    m_timeDelay;
	VTSListOfPropertyStatesDlg			m_listofpropstates;
// Dialog Data
	//{{AFX_DATA(VTSEventChangeOfStateDlg)
	enum { IDD = IDD_EVENT_STATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventChangeOfStateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventChangeOfStateDlg)
	afx_msg void OnChangeTimedelay();
	virtual BOOL OnInitDialog();
	afx_msg void OnListofpropstates();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTCHANGEOFSTATEDLG_H__A0623535_A28C_4FB1_8F5D_0AD591E7B766__INCLUDED_)
