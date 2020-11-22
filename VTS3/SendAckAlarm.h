#if !defined(AFX_SENDACKALARM_H__A5F46EB2_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
#define AFX_SENDACKALARM_H__A5F46EB2_797F_11D4_BED9_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendAckAlarm.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSTimeStamp.h"

/////////////////////////////////////////////////////////////////////////////
// CSendAckAlarm dialog

class CSendAckAlarm : public CSendPage
{
	DECLARE_DYNCREATE(CSendAckAlarm)

// Construction
public:
	CSendAckAlarm( void );   // non-standard constructor

	VTSUnsignedCtrl			m_AckProcessID;
	VTSObjectIdentifierCtrl	m_ObjectID;
	VTSEnumeratedCtrl		m_EventStateCombo;
	VTSTimeStamp			m_TimeStamp;
	VTSCharacterStringCtrl	m_AckSource;
	VTSTimeStamp			m_TimeOfAck;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(  int index = 0  );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendAckAlarm)
	enum { IDD = IDD_SENDACKALARM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendAckAlarm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendAckAlarm)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAckProcessID();
	afx_msg void OnChangeObjectID();
	afx_msg void OnSelchangeEventStateCombo();
	afx_msg void OnTimeStamp();
	afx_msg void OnChangeAckSource();
	afx_msg void OnTimeOfAck();
	afx_msg void OnObjIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDACKALARM_H__A5F46EB2_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
