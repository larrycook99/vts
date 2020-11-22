#if !defined(AFX_VTSNOTIFYCOMPLEX_H__C16F9185_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYCOMPLEX_H__C16F9185_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotifyComplex.h : header file
//

#include "SendPage.h"
#include "VTSAny.h"
#include "VTSCtrl.h"
#include "VTSNotifyPage.h"

class VTSNotifyComplex;
typedef VTSNotifyComplex *VTSNotifyComplexPtr;

//
//	ComplexObjectElem
//

class ComplexObjectElem {
	public:
		ComplexObjectElem( VTSNotifyComplexPtr wp );

		VTSEnumeratedCtrl		coePropCombo;
		VTSUnsignedCtrl			coeArrayIndex;
		VTSAny					coeValue;
		VTSUnsignedCtrl			coePriority;

		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef ComplexObjectElem *ComplexObjectElemPtr;

//
//	ComplexObjectList
//

class ComplexObjectList : public CList<ComplexObjectElemPtr,ComplexObjectElemPtr> {
	public:
		VTSNotifyComplexPtr		colPagePtr;			// bound to specific page
		ComplexObjectElemPtr	colCurElem;			// current element
		int						colCurElemIndx;		// index of current element
		bool					colAddInProgress;	// adding an object

		ComplexObjectList( VTSNotifyComplexPtr pp );
		~ComplexObjectList( void );

		void AddButtonClick( void );			// add a new property
		void RemoveButtonClick( void );			// remove the selected property

		void OnSelchangePropCombo( void );		// property combo has changed
		void OnChangeArrayIndex( void );		// array index has changed
		void OnValue( void );					// value button clicked on
		void OnChangePriority( void );			// priority has changed
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected property changing

		void Encode( BACnetAPDUEncoder& enc );
	};

typedef ComplexObjectList *ComplexObjectListPtr;

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyComplex dialog

class VTSNotifyComplex : public CPropertyPage, public VTSNotifyPage
{
	DECLARE_DYNCREATE(VTSNotifyComplex)

// Construction
public:
	VTSNotifyComplex();
	~VTSNotifyComplex();

	ComplexObjectList			m_PropList;		// list of properties

	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Dialog Data
	//{{AFX_DATA(VTSNotifyComplex)
	enum { IDD = IDD_NOTIFY_COMPLEX };
	CListCtrl	m_PropListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSNotifyComplex)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSNotifyComplex)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddProp();
	afx_msg void OnRemoveProp();
	afx_msg void OnItemchangingPropListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangePropCombo();
	afx_msg void OnChangeArrayIndex();
	afx_msg void OnValue();
	afx_msg void OnChangePriority();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFYCOMPLEX_H__C16F9185_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
