#if !defined(AFX_SENDWHOIS_H__468353B5_5606_11D4_BEBF_00A0C95A9812__INCLUDED_)
#define AFX_SENDWHOIS_H__468353B5_5606_11D4_BEBF_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendWhoIs.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendWhoIs dialog

class CSendWhoIs : public CSendPage
{
	DECLARE_DYNCREATE(CSendWhoIs)

// Construction
public:
	CSendWhoIs( void );   // non-standard constructor

	VTSUnsignedCtrl				m_LowLimit;
	VTSUnsignedCtrl				m_HighLimit;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendWhoIs)
	enum { IDD = IDD_SENDWHOIS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendWhoIs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendWhoIs)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeLowLimit();
	afx_msg void OnChangeHighLimit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDWHOIS_H__468353B5_5606_11D4_BEBF_00A0C95A9812__INCLUDED_)
