#if !defined(AFX_SENDREADPROP_H__52D290C1_6369_11D4_BEC8_00A0C95A9812__INCLUDED_)
#define AFX_SENDREADPROP_H__52D290C1_6369_11D4_BEC8_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadProp.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendReadProp dialog

class CSendReadProp : public CSendPage
{
	DECLARE_DYNCREATE(CSendReadProp)

// Construction
public:
	CSendReadProp( void );   // non-standard constructor

	VTSObjectIdentifierCtrl		m_ObjectID;
	VTSEnumeratedCtrl			m_PropCombo;
	VTSUnsignedCtrl				m_ArrayIndex;

	static  int historyCount;           //Xiao Shiyuan 2002-12-5
	static  int curHistoryIndex;        //Xiao Shiyuan 2002-12-5

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents[glMaxHistoryCount];

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

	void ForceValues(BACnetObjectIdentifier * pObjectID, BACnetEnumerated * pPropID );

// Dialog Data
	//{{AFX_DATA(CSendReadProp)
	enum { IDD = IDD_SENDREADPROP };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendReadProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendReadProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeObjectID();
	afx_msg void OnSelchangePropCombo();
	afx_msg void OnChangeArrayIndex();
	afx_msg void OnObjectIDButton();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnDropdownPropcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADPROP_H__52D290C1_6369_11D4_BEC8_00A0C95A9812__INCLUDED_)
