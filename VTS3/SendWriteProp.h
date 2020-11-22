#if !defined(AFX_SENDWRITEPROP_H__46DD41C2_6695_11D4_BEC9_00A0C95A9812__INCLUDED_)
#define AFX_SENDWRITEPROP_H__46DD41C2_6695_11D4_BEC9_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendWriteProp.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

/////////////////////////////////////////////////////////////////////////////
// SendWriteProp dialog

class CSendWriteProp : public CSendPage
{
	DECLARE_DYNCREATE(CSendWriteProp)

// Construction
public:
	CSendWriteProp( void );   // non-standard constructor

	VTSObjectIdentifierCtrl		m_ObjectID;
	VTSEnumeratedCtrl			m_PropCombo;
	VTSUnsignedCtrl				m_ArrayIndex;
	VTSAny						m_Value;
	VTSUnsignedCtrl				m_Priority;

	static  int historyCount;           //Xiao Shiyuan 2002-12-5
	static  int curHistoryIndex;        //Xiao Shiyuan 2002-12-5

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents[glMaxHistoryCount];

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

	void ForceValues(BACnetObjectIdentifier * pObjectID, BACnetEnumerated * pPropID, BACnetEncodeable * pbacnetEncodeable );

// Dialog Data
	//{{AFX_DATA(CSendWriteProp)
	enum { IDD = IDD_SENDWRITEPROP };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendWriteProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendWriteProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeObjectID();
	afx_msg void OnSelchangePropCombo();
	afx_msg void OnChangeArrayIndex();
	afx_msg void OnValue();
	afx_msg void OnChangePriority();
	afx_msg void OnObjectIDButton();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDropdownPropcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDWRITEPROP_H__46DD41C2_6695_11D4_BEC9_00A0C95A9812__INCLUDED_)
