#if !defined(AFX_VTSLISTOFUNSIGNEDDLG_H__A48C6CFE_AD40_4FD3_A378_B2EADE28690A__INCLUDED_)
#define AFX_VTSLISTOFUNSIGNEDDLG_H__A48C6CFE_AD40_4FD3_A378_B2EADE28690A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSListOfUnsignedDlg.h : header file
//
#include "VTSCtrl.h"
class VTSListOfUnsignedDlg;
class UnsignedElem {
	public:
		UnsignedElem( VTSListOfUnsignedDlg* wp );	
		VTSUnsignedCtrl			m_uintvalue;
		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls
		void Encode( BACnetAPDUEncoder& enc, int context = kAppContext);
		void Decode ( BACnetAPDUDecoder& dec);
	};

typedef UnsignedElem* UnsignedElemPtr;
class UnsignedList : public CList<UnsignedElemPtr,UnsignedElemPtr> {
	public:
		VTSListOfUnsignedDlg*			tsParentPtr;			// bound to specific page
		UnsignedElemPtr                 tscurElem;			// current element
		int								tscurElemIndx;		// index of current element
		bool							tsaddInProgress;	// adding an object

		UnsignedList(VTSListOfUnsignedDlg* pp );
		~UnsignedList( void );
		void AddButtonClick( void );				// add a new property
		void RemoveButtonClick( void );				// remove the selected property
	    void OnChangePriority();
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected summary element changing
		void Encode( BACnetAPDUEncoder& enc );
	};

typedef UnsignedList *UnsignedListPtr;
/////////////////////////////////////////////////////////////////////////////
// VTSListOfUnsignedDlg dialog

class VTSListOfUnsignedDlg : public CDialog
{
		DECLARE_DYNCREATE(VTSListOfUnsignedDlg)
// Construction
public:
	VTSListOfUnsignedDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	UnsignedList m_UnsignedList;
// Dialog Data
	//{{AFX_DATA(VTSListOfUnsignedDlg)
	enum { IDD = IDD_LISTOF_UNSIGNED };
	CListCtrl	m_UnsignedListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSListOfUnsignedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSListOfUnsignedDlg)
	afx_msg void OnItemchangingUnsignedlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRemovesumelem();
	afx_msg void OnAddsumelem();
	afx_msg void OnChangePriority();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLISTOFUNSIGNEDDLG_H__A48C6CFE_AD40_4FD3_A378_B2EADE28690A__INCLUDED_)
