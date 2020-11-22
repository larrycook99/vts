#if !defined(AFX_SENDCONFCOVNOTIFICATION_H__A5F46EB3_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
#define AFX_SENDCONFCOVNOTIFICATION_H__A5F46EB3_797F_11D4_BED9_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendConfCOVNotification.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

class CSendConfCOVNotification;
typedef CSendConfCOVNotification *CSendConfCOVNotificationPtr;

//
//	COVNotificationElem
//

class COVNotificationElem {
	public:
		COVNotificationElem( CSendPagePtr wp );

		VTSEnumeratedCtrl		cnePropCombo;
		VTSUnsignedCtrl			cneArrayIndex;
		VTSAny					cneValue;
		VTSUnsignedCtrl			cnePriority;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef COVNotificationElem *COVNotificationElemPtr;

//
//	COVNotificationList
//

class COVNotificationList : public CList<COVNotificationElemPtr,COVNotificationElemPtr> {
	public:
		CSendConfCOVNotificationPtr	cnlPagePtr;			// bound to specific page
		COVNotificationElemPtr		cnlCurElem;			// current element
		int							cnlCurElemIndx;		// index of current element
		bool						cnlAddInProgress;	// adding an object

		COVNotificationList( CSendConfCOVNotificationPtr pp );
		~COVNotificationList( void );

		void AddButtonClick( int theObjectType );			// add a new property
		void RemoveButtonClick( void );			// remove the selected property

		void OnSelchangePropCombo( void );		// property combo has changed
		void OnChangeArrayIndex( void );		// array index has changed
		void OnValue( void );					// value button clicked on
		void OnChangePriority( void );			// priority has changed
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected property changing

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef COVNotificationList *COVNotificationListPtr;

/////////////////////////////////////////////////////////////////////////////
// CSendConfCOVNotification dialog

class CSendConfCOVNotification : public CSendPage
{
	DECLARE_DYNCREATE(CSendConfCOVNotification)

// Construction
public:
	CSendConfCOVNotification( void );   // non-standard constructor

	VTSUnsignedCtrl				m_ProcessID;
	VTSObjectIdentifierCtrl		m_DeviceID;
	VTSObjectIdentifierCtrl		m_ObjectID;
	VTSUnsignedCtrl				m_TimeRemaining;

	COVNotificationList			m_PropList;		// list of properties

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(  int index = 0  );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendConfCOVNotification)
	enum { IDD = IDD_SENDCONFCOVNOTIFICATION };
	CListCtrl	m_PropListCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendConfCOVNotification)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendConfCOVNotification)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeProcessID();
	afx_msg void OnChangeDeviceID();
	afx_msg void OnChangeObjectID();
	afx_msg void OnChangeTimeRemaining();
	afx_msg void OnAddProp();
	afx_msg void OnRemoveProp();
	afx_msg void OnItemchangingPropListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangePropCombo();
	afx_msg void OnChangeArrayIndex();
	afx_msg void OnValue();
	afx_msg void OnChangePriority();
	afx_msg void OnDeviceIDButton();
	afx_msg void OnObjectIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDCONFCOVNOTIFICATION_H__A5F46EB3_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
