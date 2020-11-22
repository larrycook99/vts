#if !defined(AFX_SENDBVLCRESULT_H__B4370191_45AE_11D4_BEB3_00A0C95A9812__INCLUDED_)
#define AFX_SENDBVLCRESULT_H__B4370191_45AE_11D4_BEB3_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendBVLCResult.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendBVLCResult dialog

class CSendBVLCResult : public CSendPage
{
	DECLARE_DYNCREATE(CSendBVLCResult)

// Construction
public:
	CSendBVLCResult( void );   // non-standard constructor

	VTSIntegerCtrl	m_Result;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(  int index = 0  );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendBVLCResult)
	enum { IDD = IDD_SENDBVLCRESULT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendBVLCResult)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendBVLCResult)
	afx_msg void OnChangeResult();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDBVLCRESULT_H__B4370191_45AE_11D4_BEB3_00A0C95A9812__INCLUDED_)
