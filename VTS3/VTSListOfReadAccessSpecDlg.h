#if !defined(AFX_VTSLISTOFREADACCESSSPECDLG_H__BFB67D73_A98F_4955_8452_BA17124D7D13__INCLUDED_)
#define AFX_VTSLISTOFREADACCESSSPECDLG_H__BFB67D73_A98F_4955_8452_BA17124D7D13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSListOfReadAccessSpecDlg.h : header file
//
#include "VTSCtrl.h"
#include "VTSAny.h"

class VTSListOfReadAccessSpecDlg;
typedef VTSListOfReadAccessSpecDlg *VTSListOfReadAccessSpecDlgPtr;

//
//	PropRefElem
//

class PropRefElem {
	public:
		PropRefElem( VTSListOfReadAccessSpecDlgPtr wp );

		VTSEnumeratedCtrl		rpePropCombo;
		VTSUnsignedCtrl			rpeArrayIndex;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls
		void Encode( BACnetAPDUEncoder& enc );
    	void Decode( BACnetAPDUDecoder& dec );
	};

typedef PropRefElem *PropRefElemPtr;

//
//	ReadAccessSpec
//

class ReadAccessSpec : public CList<PropRefElemPtr,PropRefElemPtr> {
	public:
		VTSObjectIdentifierCtrl	rplObjID;			// object

		VTSListOfReadAccessSpecDlgPtr	rplPagePtr;			// bound to specific page
		PropRefElemPtr			rplCurElem;			// current element
		int						rplCurElemIndx;		// index of current element
		bool					rplAddInProgress;	// adding an object

		ReadAccessSpec( VTSListOfReadAccessSpecDlgPtr pp );
		~ReadAccessSpec( void );

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
    	void Decode( BACnetAPDUDecoder& dec );
	};

typedef ReadAccessSpec *ReadAccessSpecPtr;

//
//	ReadAccessSpecList
//

class ReadAccessSpecList : public CList<ReadAccessSpecPtr,ReadAccessSpecPtr> {
	public:
		VTSListOfReadAccessSpecDlgPtr	rpllPagePtr;		// bound to specific page
		ReadAccessSpecPtr			rpllCurElem;		// current element
		int						rpllCurElemIndx;	// index of current element
		bool					rpllAddInProgress;	// adding an object

		ReadAccessSpecList( VTSListOfReadAccessSpecDlgPtr pp );
		~ReadAccessSpecList( void );

		void AddButtonClick( void );			// add a new object
		void RemoveButtonClick( void );			// remove the selected object

		void UpButtonClick( void );				// shuffle up
		void DownButtonClick( void );			// shuffle down

		void OnChangeObjID( void );				// object ID has changed
		void OnObjectIDButton( void );			// associated button

		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult ); // selected object changing

		void Encode( BACnetAPDUEncoder& enc );
    	void Decode( BACnetAPDUDecoder& dec );
	};

/////////////////////////////////////////////////////////////////////////////
// VTSListOfReadAccessSpecDlg dialog

class VTSListOfReadAccessSpecDlg : public CDialog
{
	DECLARE_DYNCREATE(VTSListOfReadAccessSpecDlg)

// Construction
public:
	VTSListOfReadAccessSpecDlg(CWnd* pParent = NULL); 

	ReadAccessSpecList		m_PropListList;		// list of lists. Xiao Shiyuan 2002-12-2
	CList<CString, CString> m_strList;
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
// Dialog Data
	//{{AFX_DATA(VTSListOfReadAccessSpecDlg)
	enum { IDD = IDD_LISTOFREADACCESSSPECIFICATION };
	CListCtrl	m_ObjList;
	CListCtrl	m_PropList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSListOfReadAccessSpecDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSListOfReadAccessSpecDlg)
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
	afx_msg void OnDropdownPropcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLISTOFREADACCESSSPECDLG_H__BFB67D73_A98F_4955_8452_BA17124D7D13__INCLUDED_)
