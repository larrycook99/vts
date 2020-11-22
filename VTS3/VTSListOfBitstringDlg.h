#if !defined(AFX_VTSLISTOFBITSTRINGDLG_H__9E451DEC_16A0_43AF_A760_A92388AB8321__INCLUDED_)
#define AFX_VTSLISTOFBITSTRINGDLG_H__9E451DEC_16A0_43AF_A760_A92388AB8321__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSListOfBitstringDlg.h : header file
//
#include "VTSCtrl.h"
class VTSListOfBitstringDlg;
class BitstringElem {
	public:
		BitstringElem( VTSListOfBitstringDlg* wp );	
		VTSBitStringCtrl			m_bstrvalue;
		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls
		void Encode( BACnetAPDUEncoder& enc, int context = kAppContext);
		void Decode ( BACnetAPDUDecoder& dec);
	};

typedef BitstringElem* BitstringElemPtr;
class BitstringList : public CList<BitstringElemPtr,BitstringElemPtr> {
	public:
		VTSListOfBitstringDlg*			tsParentPtr;			// bound to specific page
		BitstringElemPtr                 tscurElem;			// current element
		int								tscurElemIndx;		// index of current element
		bool							tsaddInProgress;	// adding an object

		BitstringList(VTSListOfBitstringDlg* pp );
		~BitstringList( void );
		void AddButtonClick( void );				// add a new property
		void RemoveButtonClick( void );				// remove the selected property
	    void OnChangeBitstring();
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected summary element changing
		void Encode( BACnetAPDUEncoder& enc );
	};

typedef BitstringList *BitstringListPtr;
/////////////////////////////////////////////////////////////////////////////
// VTSListOfBitstringDlg dialog

class VTSListOfBitstringDlg : public CDialog
{
// Construction
public:
	VTSListOfBitstringDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	BitstringList m_BitstringList;
// Dialog Data
	//{{AFX_DATA(VTSListOfBitstringDlg)
	enum { IDD = IDD_LISTOF_BITSTRING };
	CListCtrl	m_BitstringListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSListOfBitstringDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSListOfBitstringDlg)
	afx_msg void OnAddsumelem();
	afx_msg void OnChangeBitstring();
	afx_msg void OnItemchangingBitstringlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRemovesumelem();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLISTOFBITSTRINGDLG_H__9E451DEC_16A0_43AF_A760_A92388AB8321__INCLUDED_)
