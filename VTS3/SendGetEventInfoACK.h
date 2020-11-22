#if !defined(AFX_SENDGETEVENTINFOACK_H__A1369230_E52E_47D6_ABA0_F741281D6FDC__INCLUDED_)
#define AFX_SENDGETEVENTINFOACK_H__A1369230_E52E_47D6_ABA0_F741281D6FDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendGetEventInfoACK.h : header file
//
#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSListOfTimeStampDlg.h"
#include "VTSListOfUnsignedDlg.h"
//#include "BACnet.hpp"

class CSendGetEventInfoACK;
typedef CSendGetEventInfoACK *CSendGetEventInfoACKPtr;

//
//	EnrollmentSummaryElem
//

class EventSummaryElem {
	public:
		EventSummaryElem( CSendPagePtr wp );

		VTSObjectIdentifierCtrl					  m_ObjectID;
		VTSEnumeratedCtrl						  eseEventStateCombo;
		VTSListOfTimeStampDlg					  listofTimestamp;
		VTSEnumeratedCtrl						  eseNotifyTypeCombo;
		VTSListOfUnsignedDlg    m_eventPriorities;
		VTSBooleanCtrl			esackOffnormal;
		VTSBooleanCtrl			esackFault;
		VTSBooleanCtrl			esackNormal;
		VTSBooleanCtrl			esenaOffnormal;
		VTSBooleanCtrl			esenaFault;
		VTSBooleanCtrl			esenaNormal;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls
//		void ReleaseList( void );
		void Encode( BACnetAPDUEncoder& enc );
	};

typedef EventSummaryElem *EventSummaryElemPtr;

//
//	EnrollmentSummaryList
//

class EventSummaryList : public CList<EventSummaryElemPtr,EventSummaryElemPtr> {
	public:
		CSendGetEventInfoACKPtr	eslPagePtr;			// bound to specific page
		EventSummaryElemPtr		eslCurElem;			// current element
		int								eslCurElemIndx;		// index of current element
		bool							eslAddInProgress;	// adding an object

		EventSummaryList( CSendGetEventInfoACKPtr pp );
		~EventSummaryList( void );

		void AddButtonClick( void );				// add a new property
		void RemoveButtonClick( void );				// remove the selected property
		void OnChangeObjectID( void );				// object ID has changed
		void OnObjectIDButton( void );				// button!
		void OnSelchangeEventStateCombo( void );		// event State combo has changed
		void OnOffnormal();
		void OnNormal();
		void OnFault();
		void OnSettimestamp();
		void OnSelchangeNotifyTypeCombo( void );	// event state combo has changed
		void OnOffnormal1();
		void OnNormal1();
		void OnFault1();
		void OnSetpriorities();
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected summary element changing
		void UpdateBitsDisplay( void );				// update the display to reflect the bits

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef EventSummaryList *EventSummaryListPtr;

/////////////////////////////////////////////////////////////////////////////
// CSendGetEventInfoACK dialog

class CSendGetEventInfoACK  : public CSendPage
{
	DECLARE_DYNCREATE(CSendGetEventInfoACK)
// Construction
public:
	CSendGetEventInfoACK(void);   // standard constructor
	EventSummaryList m_EventSumList;
	VTSBooleanCtrl   m_MoreEvent;
	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page
	static BACnetAPDUEncoder	pageContents;
	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );			// restore contents to last saved values
// Dialog Data
	//{{AFX_DATA(CSendGetEventInfoACK)
	enum { IDD = IDD_SENDGETEVENTINFOACK };
	CListCtrl	m_EventSumListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendGetEventInfoACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendGetEventInfoACK)
	afx_msg void OnNormal();
	afx_msg void OnFault1();
	afx_msg void OnFault();
	afx_msg void OnSelChangeEventStatecombo();
	afx_msg void OnSelChangeNotifyTypecombo();
	afx_msg void OnAddsumelem();
	afx_msg void OnRemovesumelem();
	afx_msg void OnItemchangingEventinfolist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetpriorities();
	afx_msg void OnSettimestamp();
	afx_msg void OnOffnormal();
	afx_msg void OnOffnormal1();
	afx_msg void OnNormal1();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeObjectid();
	afx_msg void OnObjectidbtn();
	afx_msg void OnMoreevent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDGETEVENTINFOACK_H__A1369230_E52E_47D6_ABA0_F741281D6FDC__INCLUDED_)
