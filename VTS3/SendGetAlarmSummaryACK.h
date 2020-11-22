#if !defined(AFX_SENDGETALARMSUMMARYACK_H__A5F46EB5_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
#define AFX_SENDGETALARMSUMMARYACK_H__A5F46EB5_797F_11D4_BED9_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendGetAlarmSummaryACK.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

class CSendGetAlarmSummaryACK;
typedef CSendGetAlarmSummaryACK *CSendGetAlarmSummaryACKPtr;

//
//	AlarmSummaryElem
//

class AlarmSummaryElem {
	public:
		AlarmSummaryElem( CSendPagePtr wp );

		VTSObjectIdentifierCtrl	aseObjectID;
		VTSEnumeratedCtrl		aseEventStateCombo;
		VTSBooleanCtrl			aseToOffnormal;
		VTSBooleanCtrl			aseToFault;
		VTSBooleanCtrl			aseToNormal;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef AlarmSummaryElem *AlarmSummaryElemPtr;

//
//	AlarmSummaryList
//

class AlarmSummaryList : public CList<AlarmSummaryElemPtr,AlarmSummaryElemPtr> {
	public:
		CSendGetAlarmSummaryACKPtr	aslPagePtr;			// bound to specific page
		AlarmSummaryElemPtr			aslCurElem;			// current element
		int							aslCurElemIndx;		// index of current element
		bool						aslAddInProgress;	// adding an object

		AlarmSummaryList( CSendGetAlarmSummaryACKPtr pp );
		~AlarmSummaryList( void );

		void AddButtonClick( void );				// add a new property
		void RemoveButtonClick( void );				// remove the selected property

		void OnChangeObjectID( void );				// object ID has changed
		void OnObjectIDButton( void );				// button associated with object ID
		void OnSelchangeEventStateCombo( void );	// event state combo has changed
		void OnToOffnormal( void );					// checkbox changed
		void OnToFault( void );						// checkbox changed
		void OnToNormal( void );					// checkbox changed
		void UpdateBitsDisplay( void );				// update the display to reflect the bits

		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected summary element changing

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef AlarmSummaryList *AlarmSummaryListPtr;

/////////////////////////////////////////////////////////////////////////////
// CSendGetAlarmSummaryACK dialog

class CSendGetAlarmSummaryACK : public CSendPage
{
	DECLARE_DYNCREATE(CSendGetAlarmSummaryACK)

// Construction
public:
	CSendGetAlarmSummaryACK( void );   // non-standard constructor

	AlarmSummaryList			m_AlarmSumList;	// list of alarm summary elements

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendGetAlarmSummaryACK)
	enum { IDD = IDD_SENDGETALARMSUMMARYACK };
	CListCtrl	m_AlarmSumListCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendGetAlarmSummaryACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendGetAlarmSummaryACK)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddSumElem();
	afx_msg void OnRemoveSumElem();
	afx_msg void OnItemchangingAlarmSumListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeObjectID();
	afx_msg void OnSelchangeEventStateCombo( void );	// event state combo has changed
	afx_msg void OnToOffnormal();
	afx_msg void OnToFault();
	afx_msg void OnToNormal();
	afx_msg void OnObjectIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDGETALARMSUMMARYACK_H__A5F46EB5_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
