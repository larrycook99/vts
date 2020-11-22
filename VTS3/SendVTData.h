#if !defined(AFX_SENDVTDATA_H__E1F7BE53_7771_11D4_BED7_00A0C95A9812__INCLUDED_)
#define AFX_SENDVTDATA_H__E1F7BE53_7771_11D4_BED7_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendVTData.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendVTData dialog

class CSendVTData : public CSendPage
{
	DECLARE_DYNCREATE(CSendVTData)

// Construction
public:
	CSendVTData( void );   // non-standard constructor

	VTSUnsignedCtrl				m_ID;
	VTSOctetStringCtrl			m_Data;			// stream data
	VTSBooleanCtrl				m_DataFlag;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendVTData)
	enum { IDD = IDD_SENDVTDATA };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendVTData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendVTData)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeID();
	afx_msg void OnChangeData();
	afx_msg void OnDataFlag();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDVTDATA_H__E1F7BE53_7771_11D4_BED7_00A0C95A9812__INCLUDED_)
