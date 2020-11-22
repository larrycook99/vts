#if !defined(AFX_SENDWRITEPROPMULTERROR_H__90AF9F82_56E3_11D4_BEC0_00A0C95A9812__INCLUDED_)
#define AFX_SENDWRITEPROPMULTERROR_H__90AF9F82_56E3_11D4_BEC0_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendWritePropMultError.h : header file
//
#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendWritePropMultError dialog

class CSendWritePropMultError : public CSendPage
{
	DECLARE_DYNCREATE(CSendWritePropMultError)

// Construction
public:
	CSendWritePropMultError( void );   // non-standard constructor

	VTSIntegerCtrl			m_InvokeID;
	VTSEnumeratedCtrl		m_ServiceCombo;
	VTSEnumeratedCtrl		m_ErrorClassCombo;
	VTSEnumeratedCtrl		m_ErrorCodeCombo;

	VTSObjectIdentifierCtrl	m_ObjectID;
	VTSEnumeratedCtrl		m_PropertyID;
	VTSUnsignedCtrl			m_Index;

	static  int historyCount;           //Xiao Shiyuan 2002-12-5
	static  int curHistoryIndex;        //Xiao Shiyuan 2002-12-5

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents[glMaxHistoryCount];

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendWritePropMultError)
	enum { IDD = IDD_SENDWRITEPROPMULTERROR };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendWritePropMultError)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendWritePropMultError)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeInvokeID();
	afx_msg void OnSelchangeServiceCombo();
	afx_msg void OnSelchangeErrorClassCombo();
	afx_msg void OnSelchangeErrorCodeCombo();
	afx_msg void OnChangeObjectID();
	afx_msg void OnChangePropertyID();
	afx_msg void OnChangeIndex();
	afx_msg void OnObjectIDButton();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDWRITEPROPMULTERROR_H__90AF9F82_56E3_11D4_BEC0_00A0C95A9812__INCLUDED_)
