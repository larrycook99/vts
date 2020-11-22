#if !defined(AFX_VTSLIFESAFETYSTATEDLG_H__D59729BC_97DB_4812_9068_F770362F6380__INCLUDED_)
#define AFX_VTSLIFESAFETYSTATEDLG_H__D59729BC_97DB_4812_9068_F770362F6380__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSLifeSafetyStateDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSLifeSafetyStateDlg dialog

class VTSLifeSafetyStateDlg : public CDialog
{
// Construction
public:
	VTSLifeSafetyStateDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSLifeSafetyStateDlg)
	enum { IDD = IDD_LIFESAFETYSTATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSLifeSafetyStateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSLifeSafetyStateDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLIFESAFETYSTATEDLG_H__D59729BC_97DB_4812_9068_F770362F6380__INCLUDED_)
