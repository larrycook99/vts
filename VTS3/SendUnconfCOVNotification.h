#if !defined(AFX_SENDUNCONFCOVNOTIFICATION_H__74A41752_89FC_11D4_BEDD_00A0C95A9812__INCLUDED_)
#define AFX_SENDUNCONFCOVNOTIFICATION_H__74A41752_89FC_11D4_BEDD_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendUnconfCOVNotification.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

class CSendUnconfCOVNotification;
typedef CSendUnconfCOVNotification *CSendUnconfCOVNotificationPtr;

//
//	UnconfCOVNotificationElem
//

class UnconfCOVNotificationElem {
	public:
		UnconfCOVNotificationElem( CSendPagePtr wp );

		VTSEnumeratedCtrl		unePropCombo;
		VTSUnsignedCtrl			uneArrayIndex;
		VTSAny					uneValue;
		VTSUnsignedCtrl			unePriority;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef UnconfCOVNotificationElem *UnconfCOVNotificationElemPtr;

//
//	UnconfCOVNotificationList
//

class UnconfCOVNotificationList : public CList<UnconfCOVNotificationElemPtr,UnconfCOVNotificationElemPtr> {
	public:
		CSendUnconfCOVNotificationPtr	unlPagePtr;			// bound to specific page
		UnconfCOVNotificationElemPtr	unlCurElem;			// current element
		int							unlCurElemIndx;		// index of current element
		bool						unlAddInProgress;	// adding an object

		UnconfCOVNotificationList( CSendUnconfCOVNotificationPtr pp );
		~UnconfCOVNotificationList( void );

		void AddButtonClick( int theObjectType );			// add a new property
		void RemoveButtonClick( void );			// remove the selected property

		void OnSelchangePropCombo( void );		// property combo has changed
		void OnChangeArrayIndex( void );		// array index has changed
		void OnValue( void );					// value button clicked on
		void OnChangePriority( void );			// priority has changed
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected property changing

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef UnconfCOVNotificationList *UnconfCOVNotificationListPtr;

/////////////////////////////////////////////////////////////////////////////
// CSendUnconfCOVNotification dialog

class CSendUnconfCOVNotification : public CSendPage
{
	DECLARE_DYNCREATE(CSendUnconfCOVNotification)

// Construction
public:
	CSendUnconfCOVNotification( void );   // non-standard constructor

	VTSUnsignedCtrl				m_ProcessID;
	VTSObjectIdentifierCtrl		m_DeviceID;
	VTSObjectIdentifierCtrl		m_ObjectID;
	VTSUnsignedCtrl				m_TimeRemaining;

	UnconfCOVNotificationList	m_PropList;		// list of properties

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendUnconfCOVNotification)
	enum { IDD = IDD_SENDUNCONFCOVNOTIFICATION };
	CListCtrl	m_PropListCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendUnconfCOVNotification)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendUnconfCOVNotification)
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

#endif // !defined(AFX_SENDUNCONFCOVNOTIFICATION_H__74A41752_89FC_11D4_BEDD_00A0C95A9812__INCLUDED_)
