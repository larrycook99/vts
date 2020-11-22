#if !defined(AFX_SENDGETEVENTINFO_H__10A87DC8_2E63_4DCA_95C6_25A81C08FCF9__INCLUDED_)
#define AFX_SENDGETEVENTINFO_H__10A87DC8_2E63_4DCA_95C6_25A81C08FCF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendGetEventInfo.h : header file
//
#include "SendPage.h"
#include "VTSCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CSendGetEventInfo dialog

class CSendGetEventInfo : public CSendPage
{
	DECLARE_DYNCREATE(CSendGetEventInfo)
// Construction
public:
	CSendGetEventInfo(void);   // standard constructor
	VTSObjectIdentifierCtrl		m_ObjectID;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page
	static BACnetAPDUEncoder	pageContents;
	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );			// restore contents to last saved values
// Dialog Data
	//{{AFX_DATA(CSendGetEventInfo)
	enum { IDD = IDD_SENDGETEVENTINFO };
	BOOL	m_hasObjID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendGetEventInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendGetEventInfo)
	afx_msg void OnObjectidbtn();
	afx_msg void OnHasobjid();
	afx_msg void OnChangeObjectid();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDGETEVENTINFO_H__10A87DC8_2E63_4DCA_95C6_25A81C08FCF9__INCLUDED_)
