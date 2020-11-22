// VTSPortARCNETDialog.h: interface for the VTSPortARCNETDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSPORTARCNETDIALOG_H__EFA02451_46CA_431E_B0B9_6C9B4401F0EE__INCLUDED_)
#define AFX_VTSPORTARCNETDIALOG_H__EFA02451_46CA_431E_B0B9_6C9B4401F0EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VTSPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSPortPTPDialog dialog

class VTSPortARCNETDialog : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSPortARCNETDialog)

	CString		* m_pstrConfigParms;

public:
	VTSPortARCNETDialog( VTSPageCallbackInterface * pparent );
	VTSPortARCNETDialog();
	~VTSPortARCNETDialog();
	
// Dialog Data
	//{{AFX_DATA(VTSPortARCNETDialog)
	enum { IDD = IDD_PORTPAGE_ARCNET };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPortARCNETDialog)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPortARCNETDialog)
	//}}AFX_MSG

	void ObjToCtrl(void);
	void CtrlToObj(void);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_VTSPORTARCNETDIALOG_H__EFA02451_46CA_431E_B0B9_6C9B4401F0EE__INCLUDED_)
