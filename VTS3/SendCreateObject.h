#if !defined(AFX_SENDCREATEOBJECT_H__8D124EB2_78A7_11D4_BED8_00A0C95A9812__INCLUDED_)
#define AFX_SENDCREATEOBJECT_H__8D124EB2_78A7_11D4_BED8_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendCreateObject.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

class CSendCreateObject;
typedef CSendCreateObject *CSendCreateObjectPtr;

//
//	CreateObjectElem
//

class CreateObjectElem {
	public:
		CreateObjectElem( CSendPagePtr wp );

		VTSEnumeratedCtrl		coePropCombo;
		VTSUnsignedCtrl			coeArrayIndex;
		VTSAny					coeValue;
		VTSUnsignedCtrl			coePriority;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef CreateObjectElem *CreateObjectElemPtr;

//
//	CreateObjectList
//

class CreateObjectList : public CList<CreateObjectElemPtr,CreateObjectElemPtr> {
	public:
		CSendCreateObjectPtr	colPagePtr;			// bound to specific page
		CreateObjectElemPtr		colCurElem;			// current element
		int						colCurElemIndx;		// index of current element
		bool					colAddInProgress;	// adding an object

		CreateObjectList( CSendCreateObjectPtr pp );
		~CreateObjectList( void );

		void AddButtonClick( int theObjectType );			// add a new property
		void RemoveButtonClick( void );			// remove the selected property

		void OnSelchangePropCombo( void );		// property combo has changed
		void OnChangeArrayIndex( void );		// array index has changed
		void OnValue( void );					// value button clicked on
		void OnChangePriority( void );			// priority has changed
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected property changing

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef CreateObjectList *CreateObjectListPtr;

/////////////////////////////////////////////////////////////////////////////
// CSendCreateObject dialog

class CSendCreateObject : public CSendPage
{
	DECLARE_DYNCREATE(CSendCreateObject)

// Construction
public:
	CSendCreateObject( void );   // non-standard constructor

	VTSEnumeratedCtrl			m_ObjectTypeCombo;
	VTSObjectIdentifierCtrl		m_ObjectID;

	bool	m_bObjectTypeActive;
	int		m_nChoice;

	CreateObjectList			m_PropList;		// list of properties

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage(  int index = 0  );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendCreateObject)
	enum { IDD = IDD_SENDCREATEOBJECT };
	CListCtrl	m_PropListCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendCreateObject)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendCreateObject)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeObjectTypeCombo();
	afx_msg void OnChangeObjectID();
	afx_msg void OnAddProp();
	afx_msg void OnRemoveProp();
	afx_msg void OnItemchangingPropListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangePropCombo();
	afx_msg void OnChangeArrayIndex();
	afx_msg void OnValue();
	afx_msg void OnChangePriority();
	afx_msg void OnObjectIDButton();
	//}}AFX_MSG
	afx_msg void OnSelectObjectType();
	afx_msg void OnSelectObjectID();


	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDCREATEOBJECT_H__8D124EB2_78A7_11D4_BED8_00A0C95A9812__INCLUDED_)
