#if !defined(AFX_SENDREADPROPMULTACK_H__E061BCD2_6B4C_11D4_BECD_00A0C95A9812__INCLUDED_)
#define AFX_SENDREADPROPMULTACK_H__E061BCD2_6B4C_11D4_BECD_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadPropMultACK.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

class CSendReadPropMultACK;
typedef CSendReadPropMultACK *CSendReadPropMultACKPtr;

//
//	ReadPropACKElem
//

class ReadPropACKElem {
	public:
		ReadPropACKElem( CSendPagePtr wp );

		VTSEnumeratedCtrl		rpaePropCombo;
		VTSUnsignedCtrl			rpaeArrayIndex;
		VTSAny					rpaeValue;
		VTSEnumeratedCtrl		rpaeClassCombo;
		VTSEnumeratedCtrl		rpaeCodeCombo;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef ReadPropACKElem *ReadPropACKElemPtr;

//
//	ReadPropACKList
//

class ReadPropACKList : public CList<ReadPropACKElemPtr,ReadPropACKElemPtr> {
	public:
		VTSObjectIdentifierCtrl	rpalObjID;			// object

		CSendReadPropMultACKPtr	rpalPagePtr;		// bound to specific page
		ReadPropACKElemPtr		rpalCurElem;		// current element
		int						rpalCurElemIndx;	// index of current element
		bool					rpalAddInProgress;	// adding an object

		ReadPropACKList( CSendReadPropMultACKPtr pp );
		~ReadPropACKList( void );

		void Bind( void );						// this property list is current
		void Unbind( void );					// detach from window

		void AddButtonClick( void );			// add a new property
		void RemoveButtonClick( void );			// remove the selected property

		void UpButtonClick( void );				// shuffle up
		void DownButtonClick( void );			// shuffle down

		void OnSelchangePropCombo( void );		// property combo has changed
		void OnChangeArrayIndex( void );		// array index has changed
		void OnValue( void );					// value button clicked on
		void OnSelchangeClassCombo( void );		// error class combo has changed
		void OnSelchangeCodeCombo( void );		// error code combo has changed
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected property changing

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef ReadPropACKList *ReadPropACKListPtr;

//
//	ReadPropACKListList
//

class ReadPropACKListList : public CList<ReadPropACKListPtr,ReadPropACKListPtr> {
	public:
		CSendReadPropMultACKPtr	rpallPagePtr;		// bound to specific page
		ReadPropACKListPtr		rpallCurElem;		// current element
		int						rpallCurElemIndx;	// index of current element
		bool					rpallAddInProgress;	// adding an object

		ReadPropACKListList( CSendReadPropMultACKPtr pp );
		~ReadPropACKListList( void );

		void AddButtonClick( void );			// add a new object
		void RemoveButtonClick( void );			// remove the selected object

		void UpButtonClick( void );				// shuffle up
		void DownButtonClick( void );			// shuffle down

		void OnChangeObjID( void );				// object ID has changed
		void OnObjectIDButton( void );			// associated button

		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult ); // selected object changing

		void Encode( BACnetAPDUEncoder& enc );
	};

/////////////////////////////////////////////////////////////////////////////
// CSendReadPropMultACK dialog

class CSendReadPropMultACK : public CSendPage
{
	DECLARE_DYNCREATE(CSendReadPropMultACK)

// Construction
public:
	CSendReadPropMultACK( void );   // non-standard constructor

	ReadPropACKListList		m_PropListList;		// list of lists

	CList<CString, CString> m_strList;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendReadPropMultACK)
	enum { IDD = IDD_SENDREADPROPMULTACK };
	CListCtrl	m_ObjList;
	CListCtrl	m_PropList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendReadPropMultACK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendReadPropMultACK)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddObj();
	afx_msg void OnRemoveObj();
	afx_msg void OnObjUp();
	afx_msg void OnObjDown();
	afx_msg void OnChangeObjID();
	afx_msg void OnItemchangingObjList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddProp();
	afx_msg void OnRemoveProp();
	afx_msg void OnPropUp();
	afx_msg void OnPropDown();
	afx_msg void OnItemchangingPropList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangePropCombo();
	afx_msg void OnChangeArrayIndex();
	afx_msg void OnValue();
	afx_msg void OnSelchangeClassCombo();
	afx_msg void OnSelchangeCodeCombo();
	afx_msg void OnObjectIDButton();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDropdownPropcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADPROPMULTACK_H__E061BCD2_6B4C_11D4_BECD_00A0C95A9812__INCLUDED_)
