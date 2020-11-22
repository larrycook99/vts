#if !defined(AFX_VTSEVENTBITSTRINGDLG_H__976D7EA1_CC67_4705_8E8C_35AC6F9E53F2__INCLUDED_)
#define AFX_VTSEVENTBITSTRINGDLG_H__976D7EA1_CC67_4705_8E8C_35AC6F9E53F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSEventBitstringDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSListOfBitstringDlg.h"
/////////////////////////////////////////////////////////////////////////////
// VTSEventBitstringDlg dialog

class VTSEventBitstringDlg : public CDialog
{
// Construction
public:
	VTSEventBitstringDlg(CWnd* pParent = NULL);   // standard constructor
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	VTSUnsignedCtrl					m_timeDelay;
	VTSBitStringCtrl                m_bitmask;
	VTSListOfBitstringDlg			m_listofbstr;
// Dialog Data
	//{{AFX_DATA(VTSEventBitstringDlg)
	enum { IDD = IDD_EVENT_BITSTRING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSEventBitstringDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSEventBitstringDlg)
	afx_msg void OnChangeTimedelay();
	afx_msg void OnChangeBitmask();
	afx_msg void OnListofbstr();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSEVENTBITSTRINGDLG_H__976D7EA1_CC67_4705_8E8C_35AC6F9E53F2__INCLUDED_)
