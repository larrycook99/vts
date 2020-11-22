#if !defined(AFX_VTSPROGRAMSTATEDLG_H__DC714317_0248_4B78_98F1_2377B48A30CB__INCLUDED_)
#define AFX_VTSPROGRAMSTATEDLG_H__DC714317_0248_4B78_98F1_2377B48A30CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSProgramStateDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSProgramStateDlg dialog

class VTSProgramStateDlg : public CDialog
{
// Construction
public:
	VTSProgramStateDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSProgramStateDlg)
	enum { IDD = IDD_PROGRAMSTATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSProgramStateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSProgramStateDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPROGRAMSTATEDLG_H__DC714317_0248_4B78_98F1_2377B48A30CB__INCLUDED_)
