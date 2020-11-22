#if !defined(AFX_SENDTEST_H__4ACEC6C1_4CAB_11D4_BEB8_00A0C95A9812__INCLUDED_)
#define AFX_SENDTEST_H__4ACEC6C1_4CAB_11D4_BEB8_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendTest.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

/////////////////////////////////////////////////////////////////////////////
// CSendTest dialog

class CSendTest : public CSendPage
{
	DECLARE_DYNCREATE(CSendTest)

// Construction
public:
	CSendTest( void );   // non-standard constructor

	VTSBooleanCtrl			m_Boolean;
	VTSBooleanCtrl			m_BooleanCheck;
	VTSEnumeratedCtrl		m_Enumerated;
	VTSEnumeratedCtrl		m_EnumeratedCombo;
	VTSUnsignedCtrl			m_Unsigned;
	VTSIntegerCtrl			m_Integer;
	VTSRealCtrl				m_Real;
	VTSDoubleCtrl			m_Double;
	VTSCharacterStringCtrl	m_CharStr;
	VTSOctetStringCtrl		m_OctetStr;
	VTSBitStringCtrl		m_BitStr;
	VTSDateCtrl				m_Date;
	VTSTimeCtrl				m_Time;
	VTSObjectIdentifierCtrl	m_ObjID;

	VTSAny					m_Any;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

// Dialog Data
	//{{AFX_DATA(CSendTest)
	enum { IDD = IDD_SENDTEST };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendTest)
	afx_msg void OnChangeBoolean();
	afx_msg void OnBooleanCheck();
	afx_msg void OnChangeEnumerated();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeEnumeratedCombo();
	afx_msg void OnChangeUnsigned();
	afx_msg void OnChangeInteger();
	afx_msg void OnChangeReal();
	afx_msg void OnChangeDouble();
	afx_msg void OnChangeCharStr();
	afx_msg void OnChangeOctetStr();
	afx_msg void OnChangeBitStr();
	afx_msg void OnChangeDate();
	afx_msg void OnChangeTime();
	afx_msg void OnChangeObjID();
	afx_msg void OnAny();
	afx_msg void OnObjectIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDTEST_H__4ACEC6C1_4CAB_11D4_BEB8_00A0C95A9812__INCLUDED_)
