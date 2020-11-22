#if !defined(AFX_VTSLISTOFLIFESAFETYSTATEDLG_H__59A5742C_A2CB_4884_A77C_675902584B09__INCLUDED_)
#define AFX_VTSLISTOFLIFESAFETYSTATEDLG_H__59A5742C_A2CB_4884_A77C_675902584B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSListOfLifeSafetyStateDlg.h : header file
//
#include "VTSCtrl.h"
class VTSListOfLifeSafetyStateDlg;
class EnumElem {
	public:
		EnumElem( VTSListOfLifeSafetyStateDlg* wp );	
		VTSEnumeratedCtrl			m_enumvalue;
		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls
		void Encode( BACnetAPDUEncoder& enc, int context = kAppContext);
		void Decode ( BACnetAPDUDecoder& dec);
	};

typedef EnumElem* EnumElemPtr;
class EnumList : public CList<EnumElemPtr,EnumElemPtr> {
	public:
		VTSListOfLifeSafetyStateDlg*		tsParentPtr;			// bound to specific page
		EnumElemPtr                      tscurElem;			// current element
		int								tscurElemIndx;		// index of current element
		bool							tsaddInProgress;	// adding an object

		EnumList(VTSListOfLifeSafetyStateDlg* pp );
		~EnumList( void );
		void AddButtonClick( void );				// add a new property
		void RemoveButtonClick( void );				// remove the selected property
	    void OnSelchangeEnumvalue();
		void OnDropdownEnumvalue();
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected summary element changing
		void Encode( BACnetAPDUEncoder& enc );
	};

typedef EnumList *EnumListPtr;
/////////////////////////////////////////////////////////////////////////////
// VTSListOfLifeSafetyStateDlg dialog

class VTSListOfLifeSafetyStateDlg : public CDialog
{
// Construction
public:
	VTSListOfLifeSafetyStateDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	EnumList m_EnumList;
// Dialog Data
	//{{AFX_DATA(VTSListOfLifeSafetyStateDlg)
	enum { IDD = IDD_LISTOF_LIFESAFETYSTATE };
	CListCtrl	m_EnumListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSListOfLifeSafetyStateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSListOfLifeSafetyStateDlg)
	afx_msg void OnSelchangeEnumvalue();
	afx_msg void OnRemovesumelem();
	afx_msg void OnAddsumelem();
	afx_msg void OnItemchangingEnumlist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnDropdownEnumvalue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLISTOFLIFESAFETYSTATEDLG_H__59A5742C_A2CB_4884_A77C_675902584B09__INCLUDED_)
