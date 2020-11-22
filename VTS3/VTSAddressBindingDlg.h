#if !defined(AFX_VTSADDRESSBINDINGDLG_H__45EB5247_DBEE_45DD_B3F6_09F515A904C5__INCLUDED_)
#define AFX_VTSADDRESSBINDINGDLG_H__45EB5247_DBEE_45DD_B3F6_09F515A904C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSAddressBindingDlg.h : header file
//
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// VTSAddressBindingDlg dialog

class VTSAddressBindingDlg : public CDialog
{
// Construction
public:
	VTSAddressBindingDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );


	BACnetAddressBinding* m_pValue;
	VTSUnsignedCtrl m_NetworkNumberCtrl;
	VTSOctetStringCtrl m_MACAddressCtrl;
	VTSObjectIdentifierCtrl m_ObjectIDCtrl;

// Dialog Data
	//{{AFX_DATA(VTSAddressBindingDlg)
	enum { IDD = IDD_ADDRESSBINDING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSAddressBindingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSAddressBindingDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnOid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSADDRESSBINDINGDLG_H__45EB5247_DBEE_45DD_B3F6_09F515A904C5__INCLUDED_)
