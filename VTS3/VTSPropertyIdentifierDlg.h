#if !defined(AFX_VTSPROPERTYIDENTIFIERDLG_H__430B8553_D175_4F39_A83B_2BB0A4F19F85__INCLUDED_)
#define AFX_VTSPROPERTYIDENTIFIERDLG_H__430B8553_D175_4F39_A83B_2BB0A4F19F85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPropertyIdentifierDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSPropertyIdentifierDlg dialog

class VTSPropertyIdentifierDlg : public CDialog
{
// Construction
public:
	VTSPropertyIdentifierDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSEnumeratedCtrl	m_enumcombo;
// Dialog Data
	//{{AFX_DATA(VTSPropertyIdentifierDlg)
	enum { IDD = IDD_PROPERTYIDENTIFIER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPropertyIdentifierDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPropertyIdentifierDlg)
	afx_msg void OnSelchangeEnumratecombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPROPERTYIDENTIFIERDLG_H__430B8553_D175_4F39_A83B_2BB0A4F19F85__INCLUDED_)
