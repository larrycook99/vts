#if !defined(AFX_SENDWRITEPROPMULT_H__46DD41C3_6695_11D4_BEC9_00A0C95A9812__INCLUDED_)
#define AFX_SENDWRITEPROPMULT_H__46DD41C3_6695_11D4_BEC9_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendWritePropMult.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"
#include "VTSAny.h"

class CSendWritePropMult;
typedef CSendWritePropMult *CSendWritePropMultPtr;

//
//	WritePropElem
//

class WritePropElem {
	public:
		WritePropElem( CSendPagePtr wp );

		VTSEnumeratedCtrl		wpePropCombo;
		VTSUnsignedCtrl			wpeArrayIndex;
		VTSAny					wpeValue;
		VTSUnsignedCtrl			wpePriority;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef WritePropElem *WritePropElemPtr;

//
//	WritePropList
//

class WritePropList : public CList<WritePropElemPtr,WritePropElemPtr> {
	public:
		VTSObjectIdentifierCtrl	wplObjID;			// object

		CSendWritePropMultPtr	wplPagePtr;			// bound to specific page
		WritePropElemPtr		wplCurElem;			// current element
		int						wplCurElemIndx;		// index of current element
		bool					wplAddInProgress;	// adding an object

		WritePropList( CSendWritePropMultPtr pp );
		~WritePropList( void );

		void Bind( void );						// this property list is current
		void Unbind( void );					// detach from window

		void AddButtonClick( void );			// add a new property
		void RemoveButtonClick( void );			// remove the selected property

		void UpButtonClick( void );				// shuffle up
		void DownButtonClick( void );			// shuffle down

		void OnSelchangePropCombo( void );		// property combo has changed
		void OnChangeArrayIndex( void );		// array index has changed
		void OnValue( void );					// value button clicked on
		void OnChangePriority( void );			// priority has changed
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected property changing

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef WritePropList *WritePropListPtr;

//
//	WritePropListList
//

class WritePropListList : public CList<WritePropListPtr,WritePropListPtr> {
	public:
		CSendWritePropMultPtr	wpllPagePtr;		// bound to specific page
		WritePropListPtr		wpllCurElem;		// current element
		int						wpllCurElemIndx;	// index of current element
		bool					wpllAddInProgress;	// adding an object

		WritePropListList( CSendWritePropMultPtr pp );
		~WritePropListList( void );

		void AddButtonClick( void );			// add a new object
		void RemoveButtonClick( void );			// remove the selected object

		void UpButtonClick( void );				// shuffle up
		void DownButtonClick( void );			// shuffle down

		void OnChangeObjID( void );				// object ID has changed
		void OnObjectIDButton( void );			// associated button

		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult ); // selected object changing

		void Encode( BACnetAPDUEncoder& enc );

		void ForceValues(BACnetObjectIdentifier * pObjectID, int apPropID[], CPtrArray * apbacnetValues, int nSize );
	};

/////////////////////////////////////////////////////////////////////////////
// CSendWritePropMult dialog

class CSendWritePropMult : public CSendPage
{
	DECLARE_DYNCREATE(CSendWritePropMult)

// Construction
public:
	CSendWritePropMult( void );   // non-standard constructor

	WritePropListList		m_PropListList;		// list of lists
	CList<CString, CString> m_strList;

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

	void ClearAll(void);
	void ForceValues(BACnetObjectIdentifier * pObjectID, int apPropID[], CPtrArray * apbacnetValues, int nSize );

// Dialog Data
	//{{AFX_DATA(CSendWritePropMult)
	enum { IDD = IDD_SENDWRITEPROPMULT };
	CListCtrl	m_ObjList;
	CListCtrl	m_PropList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendWritePropMult)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendWritePropMult)
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
	afx_msg void OnChangePriority();
	afx_msg void OnObjectIDButton();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDWRITEPROPMULT_H__46DD41C3_6695_11D4_BEC9_00A0C95A9812__INCLUDED_)
