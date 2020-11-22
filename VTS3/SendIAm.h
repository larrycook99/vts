#if !defined(AFX_SENDIAM_H__468353B4_5606_11D4_BEBF_00A0C95A9812__INCLUDED_)
#define AFX_SENDIAM_H__468353B4_5606_11D4_BEBF_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendIAm.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendIAm dialog

class CSendIAm : public CSendPage
{
	DECLARE_DYNCREATE(CSendIAm)

// Construction
public:
	CSendIAm( void );   // non-standard constructor

	VTSObjectIdentifierCtrl		m_DeviceID;
	VTSUnsignedCtrl				m_MaxAPDULength;
	VTSEnumeratedCtrl			m_SegmentationCombo;
	VTSUnsignedCtrl				m_VendorID;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendIAm)
	enum { IDD = IDD_SENDIAM };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendIAm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendIAm)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeDeviceID();
	afx_msg void OnChangeMaxAPDULength();
	afx_msg void OnSelchangeSegmentationCombo();
	afx_msg void OnChangeVendorID();
	afx_msg void OnDeviceIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDIAM_H__468353B4_5606_11D4_BEBF_00A0C95A9812__INCLUDED_)
