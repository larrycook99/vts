#if !defined(AFX_VTSSETPOINTREFERENCEDLG_H__9A113938_92FC_4EC0_B325_F5240AD5CA12__INCLUDED_)
#define AFX_VTSSETPOINTREFERENCEDLG_H__9A113938_92FC_4EC0_B325_F5240AD5CA12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSSetPointReferenceDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSSetPointReferenceDlg dialog

class VTSSetPointReferenceDlg : public CDialog
{
// Construction
public:
	VTSSetPointReferenceDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
		VTSObjectIdentifierCtrl					  m_ObjectID;
		VTSEnumeratedCtrl						  m_propCombo;
		VTSUnsignedCtrl							  m_ArrayIndex;
// Dialog Data
	//{{AFX_DATA(VTSSetPointReferenceDlg)
	enum { IDD = IDD_SETPOINTREFERENCE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSSetPointReferenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSSetPointReferenceDlg)
	afx_msg void OnDropdownPropcombo();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnObjectidbtn();
	afx_msg void OnChangeObjectid();
	afx_msg void OnChangeArrayindex();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSSETPOINTREFERENCEDLG_H__9A113938_92FC_4EC0_B325_F5240AD5CA12__INCLUDED_)
