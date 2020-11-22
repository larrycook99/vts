#if !defined(AFX_VTSLIFESAFETYMODEDLG_H__5A79DDEC_F87A_4B89_B7C3_9E59232C734B__INCLUDED_)
#define AFX_VTSLIFESAFETYMODEDLG_H__5A79DDEC_F87A_4B89_B7C3_9E59232C734B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSLifeSafetyModeDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSLifeSafetyModeDlg dialog

class VTSLifeSafetyModeDlg : public CDialog
{
// Construction
public:
	VTSLifeSafetyModeDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSLifeSafetyModeDlg)
	enum { IDD = IDD_LIFESAFETYMODE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSLifeSafetyModeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSLifeSafetyModeDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLIFESAFETYMODEDLG_H__5A79DDEC_F87A_4B89_B7C3_9E59232C734B__INCLUDED_)
