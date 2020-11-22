#if !defined(AFX_SENDGETENROLLMENTSUMMARYACK_H__1560845F_844E_11D4_BEDC_00A0C95A9812__INCLUDED_)
#define AFX_SENDGETENROLLMENTSUMMARYACK_H__1560845F_844E_11D4_BEDC_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendGetEnrollmentSummaryACK.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

class CSendGetEnrollmentSummaryACK;
typedef CSendGetEnrollmentSummaryACK *CSendGetEnrollmentSummaryACKPtr;

//
//	EnrollmentSummaryElem
//

class EnrollmentSummaryElem {
	public:
		EnrollmentSummaryElem( CSendPagePtr wp );

		VTSObjectIdentifierCtrl	eseObjectID;
		VTSEnumeratedCtrl		eseEventTypeCombo;
		VTSEnumeratedCtrl		eseEventStateCombo;
		VTSUnsignedCtrl			esePriority;
		VTSUnsignedCtrl			eseNotificationClass;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef EnrollmentSummaryElem *EnrollmentSummaryElemPtr;

//
//	EnrollmentSummaryList
//

class EnrollmentSummaryList : public CList<EnrollmentSummaryElemPtr,EnrollmentSummaryElemPtr> {
	public:
		CSendGetEnrollmentSummaryACKPtr	eslPagePtr;			// bound to specific page
		EnrollmentSummaryElemPtr		eslCurElem;			// current element
		int								eslCurElemIndx;		// index of current element
		bool							eslAddInProgress;	// adding an object

		EnrollmentSummaryList( CSendGetEnrollmentSummaryACKPtr pp );
		~EnrollmentSummaryList( void );

		void AddButtonClick( void );				// add a new property
		void RemoveButtonClick( void );				// remove the selected property

		void OnChangeObjectID( void );				// object ID has changed
		void OnObjectIDButton( void );				// button!
		void OnSelchangeEventTypeCombo( void );		// event type combo has changed
		void OnSelchangeEventStateCombo( void );	// event state combo has changed
		void OnChangePriority( void );				// priority has changed
		void OnChangeNotificationClass( void );		// notification class has changed

		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected summary element changing

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef EnrollmentSummaryList *EnrollmentSummaryListPtr;

/////////////////////////////////////////////////////////////////////////////
// CSendGetEnrollmentSummaryACK dialog

class CSendGetEnrollmentSummaryACK : public CSendPage
{
	DECLARE_DYNCREATE(CSendGetEnrollmentSummaryACK)

// Construction
public:
	CSendGetEnrollmentSummaryACK( void );   // non-standard constructor

	EnrollmentSummaryList			m_EnrollmentSumList;	// list of alarm summary elements

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendGetEnrollmentSummaryACK)
	enum { IDD = IDD_SENDGETENROLLMENTSUMMARYACK };
	CListCtrl	m_EnrollmentSumListCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendGetEnrollmentSummaryACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendGetEnrollmentSummaryACK)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddSumElem();
	afx_msg void OnRemoveSumElem();
	afx_msg void OnItemchangingEnrollmentSumListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeObjectID();
	afx_msg void OnSelchangeEventTypeCombo( void );	// event state combo has changed
	afx_msg void OnSelchangeEventStateCombo( void );	// event state combo has changed
	afx_msg void OnChangePriority();
	afx_msg void OnChangeNotificationClass();
	afx_msg void OnObjectIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDGETENROLLMENTSUMMARYACK_H__1560845F_844E_11D4_BEDC_00A0C95A9812__INCLUDED_)
