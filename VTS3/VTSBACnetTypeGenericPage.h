#if !defined(AFX_VTSBACNETTYPEGENERICPAGE_H__335E4DD6_8BFA_4CBC_9870_593772FEA48F__INCLUDED_)
#define AFX_VTSBACNETTYPEGENERICPAGE_H__335E4DD6_8BFA_4CBC_9870_593772FEA48F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSBACnetTypeGenericPage.h : header file
//

#include "VTSPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// VTSBACnetTypeGenericPage dialog

class VTSDevValue;

class VTSBACnetTypeGenericPage : public VTSPropertyPage
{
	DECLARE_DYNCREATE(VTSBACnetTypeGenericPage)

	VTSDevValue *				m_pdevvalue;

// Construction
public:
	VTSBACnetTypeGenericPage( VTSPageCallbackInterface * pparent );
	VTSBACnetTypeGenericPage();		// not used.
	~VTSBACnetTypeGenericPage();

// Dialog Data
	//{{AFX_DATA(VTSBACnetTypeGenericPage)
	enum { IDD = IDD_BTYPEGENERIC };
	CString	m_strValue;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSBACnetTypeGenericPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSBACnetTypeGenericPage)
	afx_msg void OnChangeValue();
	//}}AFX_MSG

	void EnableControls( bool fEnable );
	void DecodeValue( void );
	void EncodeValue( void );

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSBACNETTYPEGENERICPAGE_H__335E4DD6_8BFA_4CBC_9870_593772FEA48F__INCLUDED_)
