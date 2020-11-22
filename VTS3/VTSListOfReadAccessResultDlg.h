#if !defined(AFX_VTSLISTOFREADACCESSRESULTDLG_H__29C82DFF_FED2_4029_9797_01FCE89651CD__INCLUDED_)
#define AFX_VTSLISTOFREADACCESSRESULTDLG_H__29C82DFF_FED2_4029_9797_01FCE89651CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSListOfReadAccessResultDlg.h : header file
//

#include "VTSCtrl.h"
#include "VTSAny.h"

class VTSListOfReadAccessResultDlg;
typedef VTSListOfReadAccessResultDlg *VTSListOfReadAccessResultDlgPtr;

//
//	ListOfResults
//

class ListOfResults {
	public:
		ListOfResults( VTSListOfReadAccessResultDlgPtr wp );

		VTSEnumeratedCtrl		rpaePropCombo;
		VTSUnsignedCtrl			rpaeArrayIndex;
		VTSAny					rpaeValue;
		VTSEnumeratedCtrl		rpaeClassCombo;
		VTSEnumeratedCtrl		rpaeCodeCombo;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
    	void Decode( BACnetAPDUDecoder& dec );
	};

typedef ListOfResults *ListOfResultsPtr;

//
//	ReadAccessResult
//

class ReadAccessResult : public CList<ListOfResultsPtr,ListOfResultsPtr> {
	public:
		VTSObjectIdentifierCtrl	rpalObjID;			// object

		VTSListOfReadAccessResultDlgPtr	rpalPagePtr;		// bound to specific page
		ListOfResultsPtr		rpalCurElem;		// current element
		int						rpalCurElemIndx;	// index of current element
		bool					rpalAddInProgress;	// adding an object

		ReadAccessResult( VTSListOfReadAccessResultDlgPtr pp );
		~ReadAccessResult( void );

		void Bind( void );						// this property list is current
		void Unbind( void );					// detach from window

		void AddButtonClick( void );			// add a new property
		void RemoveButtonClick( void );			// remove the selected 

		void UpButtonClick( void );				// shuffle up
		void DownButtonClick( void );			// shuffle downproperty

		void OnSelchangePropCombo( void );		// property combo has changed
		void OnChangeArrayIndex( void );		// array index has changed
		void OnValue( void );					// value button clicked on
		void OnSelchangeClassCombo( void );		// error class combo has changed
		void OnSelchangeCodeCombo( void );		// error code combo has changed
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected property changing

		void Encode( BACnetAPDUEncoder& enc );
    	void Decode( BACnetAPDUDecoder& dec );
	};

typedef ReadAccessResult *ReadAccessResultPtr;

//
//	ReadAccessResultList
//

class ReadAccessResultList : public CList<ReadAccessResultPtr,ReadAccessResultPtr> {
	public:
		VTSListOfReadAccessResultDlgPtr	rpallPagePtr;		// bound to specific page
		ReadAccessResultPtr		rpallCurElem;		// current element
		int						rpallCurElemIndx;	// index of current element
		bool					rpallAddInProgress;	// adding an object

		ReadAccessResultList( VTSListOfReadAccessResultDlgPtr pp );
		~ReadAccessResultList( void );

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
// VTSListOfReadAccessResultDlg dialog

class VTSListOfReadAccessResultDlg : public CDialog
{
	DECLARE_DYNCREATE(VTSListOfReadAccessResultDlg)

// Construction
public:
	VTSListOfReadAccessResultDlg(CWnd* pParent = NULL);   // non-standard constructor

	ReadAccessResultList		m_PropListList;		// list of lists

	CList<CString, CString> m_strList;
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	void Decode( BACnetAPDUDecoder& dec );
	static BACnetAPDUEncoder	pageContents;

// Dialog Data
	//{{AFX_DATA(VTSListOfReadAccessResultDlg)
	enum { IDD = IDD_LISTOFREADACCESSRESULT };
	CListCtrl	m_ObjList;
	CListCtrl	m_PropList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSListOfReadAccessResultDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSListOfReadAccessResultDlg)
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
	afx_msg void OnDropdownPropcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLISTOFREADACCESSRESULTDLG_H__29C82DFF_FED2_4029_9797_01FCE89651CD__INCLUDED_)
