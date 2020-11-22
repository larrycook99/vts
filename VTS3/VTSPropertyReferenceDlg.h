#if !defined(AFX_VTSPROPERTYREFERENCEDLG_H__9330DD8A_E564_4DA0_8DCA_20CACD22BBEB__INCLUDED_)
#define AFX_VTSPROPERTYREFERENCEDLG_H__9330DD8A_E564_4DA0_8DCA_20CACD22BBEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPropertyReferenceDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSPropertyReferenceDlg dialog

class VTSPropertyReferenceDlg : public CDialog
{
// Construction
public:
	VTSPropertyReferenceDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
		VTSEnumeratedCtrl						  m_propCombo;
		VTSUnsignedCtrl							  m_ArrayIndex;
// Dialog Data
	//{{AFX_DATA(VTSPropertyReferenceDlg)
	enum { IDD = IDD_PROPERTYREFERENCE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPropertyReferenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPropertyReferenceDlg)
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnChangeArrayindex();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPROPERTYREFERENCEDLG_H__9330DD8A_E564_4DA0_8DCA_20CACD22BBEB__INCLUDED_)
