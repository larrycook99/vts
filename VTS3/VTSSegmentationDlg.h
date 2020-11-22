#if !defined(AFX_VTSSEGMENTATIONDLG_H__F5BE7A90_F665_4F10_A8E0_D380404F1CEB__INCLUDED_)
#define AFX_VTSSEGMENTATIONDLG_H__F5BE7A90_F665_4F10_A8E0_D380404F1CEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSSegmentationDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSSegmentationDlg dialog

class VTSSegmentationDlg : public CDialog
{
// Construction
public:
	VTSSegmentationDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSSegmentationDlg)
	enum { IDD = IDD_SEGMENTATION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSSegmentationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSSegmentationDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSSEGMENTATIONDLG_H__F5BE7A90_F665_4F10_A8E0_D380404F1CEB__INCLUDED_)
