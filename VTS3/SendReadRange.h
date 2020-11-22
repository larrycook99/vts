#if !defined(AFX_SENDREADRANGE_H__FF515791_1E69_11D6_B126_00A024B53CCB__INCLUDED_)
#define AFX_SENDREADRANGE_H__FF515791_1E69_11D6_B126_00A024B53CCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadRange.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

//class BACnetDateTime : public

//Modified by Zhu Zhenhua, 2004-5-22, remove timeRange choice, change the context
// of byTime, Add bySequenceNumber of context 6
/////////////////////////////////////////////////////////////////////////////
// SendReadRange dialog

class SendReadRange : public CSendPage
{
	DECLARE_DYNCREATE(SendReadRange)

// Construction
public:
	SendReadRange(void);

	VTSObjectIdentifierCtrl		m_ObjectID;
	VTSEnumeratedCtrl			m_PropCombo;
	VTSUnsignedCtrl				m_ArrayIndex, m_ReadRangePosRef;

	VTSDateCtrl	                m_ReadRangeRefDate;
	VTSTimeCtrl	                m_ReadRangeRefTime;
	VTSIntegerCtrl              m_ReadRangeCount;
	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values
//    unsigned int m_DatumChoice;


// Dialog Data
	//{{AFX_DATA(SendReadRange)
	enum { IDD = IDD_SENDREADRANGE };
	int		m_RadioChoice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SendReadRange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SendReadRange)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeArrayindex();
	afx_msg void OnChangeEnddatum();
	afx_msg void OnObjectidbtn();
	afx_msg void OnSelchangePropcombo();
	afx_msg void OnRadionone();
	afx_msg void OnRadioposition();
	afx_msg void OnRadiotime();
	afx_msg void OnRadioSequenceNum();
	afx_msg void OnChangeObjectid();
	afx_msg void OnSetfocusEnddatum();
	afx_msg void OnChangeStartdatum();
	afx_msg void OnSetfocusStartdatum();
	afx_msg void OnChangeTimestart();
	afx_msg void OnChangePosRef();
	afx_msg void OnChangeCount();
	afx_msg void OnDropdownPropcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADRANGE_H__FF515791_1E69_11D6_B126_00A024B53CCB__INCLUDED_)
