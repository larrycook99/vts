#if !defined(AFX_SENDREADPROPMULT_H__E061BCD1_6B4C_11D4_BECD_00A0C95A9812__INCLUDED_)
#define AFX_SENDREADPROPMULT_H__E061BCD1_6B4C_11D4_BECD_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReadPropMult.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

class CSendReadPropMult;
typedef CSendReadPropMult *CSendReadPropMultPtr;

//
//	ReadPropElem
//

class ReadPropElem {
	public:
		ReadPropElem( CSendPagePtr wp );

		VTSEnumeratedCtrl		rpePropCombo;
		VTSUnsignedCtrl			rpeArrayIndex;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef ReadPropElem *ReadPropElemPtr;

//
//	ReadPropList
//

class ReadPropList : public CList<ReadPropElemPtr,ReadPropElemPtr> {
	public:
		VTSObjectIdentifierCtrl	rplObjID;			// object

		CSendReadPropMultPtr	rplPagePtr;			// bound to specific page
		ReadPropElemPtr			rplCurElem;			// current element
		int						rplCurElemIndx;		// index of current element
		bool					rplAddInProgress;	// adding an object

		ReadPropList( CSendReadPropMultPtr pp );
		~ReadPropList( void );

		void Bind( void );						// this property list is current
		void Unbind( void );					// detach from window

		void AddButtonClick( void );			// add a new property
		void RemoveButtonClick( void );			// remove the selected property

		void UpButtonClick( void );				// shuffle up
		void DownButtonClick( void );			// shuffle down

		void OnSelchangePropCombo( void );		// property combo has changed
		void OnChangeArrayIndex( void );		// array index has changed
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected property changing

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef ReadPropList *ReadPropListPtr;

//
//	ReadPropListList
//

class ReadPropListList : public CList<ReadPropListPtr,ReadPropListPtr> {
	public:
		CSendReadPropMultPtr	rpllPagePtr;		// bound to specific page
		ReadPropListPtr			rpllCurElem;		// current element
		int						rpllCurElemIndx;	// index of current element
		bool					rpllAddInProgress;	// adding an object

		ReadPropListList( CSendReadPropMultPtr pp );
		~ReadPropListList( void );

		void AddButtonClick( void );			// add a new object
		void RemoveButtonClick( void );			// remove the selected object

		void UpButtonClick( void );				// shuffle up
		void DownButtonClick( void );			// shuffle down

		void OnChangeObjID( void );				// object ID has changed
		void OnObjectIDButton( void );			// associated button

		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult ); // selected object changing

		void Encode( BACnetAPDUEncoder& enc );

		// madanner 9/04
		void ForceValues(BACnetObjectIdentifier * pObjectID, int apPropID[], int nSize );
	};

/////////////////////////////////////////////////////////////////////////////
// CSendReadPropMult dialog

class CSendReadPropMult : public CSendPage
{
	DECLARE_DYNCREATE(CSendReadPropMult)

// Construction
public:
	CSendReadPropMult( void );   // non-standard constructor

	ReadPropListList		m_PropListList;		// list of lists. Xiao Shiyuan 2002-12-2

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );			// restore contents to last saved values
	void ClearAll(void);

	void ForceValues(BACnetObjectIdentifier * pObjectID, int apPropID[], int nSize );

	// madanner, 9/04
	CStringArray m_strList;
//	CList<CString, CString> m_strList;

// Dialog Data
	//{{AFX_DATA(CSendReadPropMult)
	enum { IDD = IDD_SENDREADPROPMULT };
	CListCtrl	m_ObjList;
	CListCtrl	m_PropList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendReadPropMult)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendReadPropMult)
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
	afx_msg void OnObjectIDButton();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDropdownPropcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREADPROPMULT_H__E061BCD1_6B4C_11D4_BECD_00A0C95A9812__INCLUDED_)
