#if !defined(AFX_VTSANY_H__5E0A2331_6116_11D4_BEC6_00A0C95A9812__INCLUDED_)
#define AFX_VTSANY_H__5E0A2331_6116_11D4_BEC6_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSAny.h : header file
//

#include <Afxmt.h>
#include <afxtempl.h>

#include "BACnet.hpp"

void EncoderToHex( const BACnetAPDUEncoder &enc, CString &str );

//
//	VTSAnyElement
//

//madanner 9/04, changed to class for proper destructors
class VTSAnyElement
{
public:
	VTSAnyElement();
	VTSAnyElement( const VTSAnyElement &theElement );
	~VTSAnyElement();

	int					elemType;
	int					elemContext;
	BACnetAPDUEncoder	elemEncoder;

private:
	// Private to prevent copy
	VTSAnyElement& operator=( VTSAnyElement const & );
};

typedef VTSAnyElement *VTSAnyElementPtr;

//
//	VTSAnyList
//

class VTSAnyList : public CList<VTSAnyElementPtr,VTSAnyElementPtr> 
{
public:
	VTSAnyList( void );
	~VTSAnyList( void );

	void Add( void );								// add a new element on the end
	void Remove( int i );							// remove an element
	void KillAll(void);

	int Length( void );								// number of defined elements
	VTSAnyElementPtr operator []( int i );			// index into element list

private:
	// Private to prevent copy
	VTSAnyList& operator=( VTSAnyList const &);
};

typedef VTSAnyList *VTSAnyListPtr;

//
// VTSAny dialog
//

class VTSAny : public CDialog
{
// Construction
public:
	VTSAny(CWnd* pParent = NULL);   // standard constructor
	VTSAny(const VTSAny&); //Xiao Shiyuan 2002-12-3
	VTSAny& operator = (const VTSAny&);

	VTSAnyList			m_anyList;				// list of encoded elements
	int					m_iSelectedElem;		// index of selected element

	void SynchronizeControls( void );

	void SetSelection( int indx );
	void ResetSelection( void );

	void EncodeValue( void );
	void DecodeValue( int indx );

	void ElemToList( int indx );				// match list to element
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );

	void SaveCtrl( BACnetAPDUEncoder& enc );		// save the contents
	void RestoreCtrl( BACnetAPDUDecoder& dec );		// restore the contents

	//madanner 9/04
	int GetBACnetToAnyTypeMapping( int nBACnetEncodeableDataType );
	void AddValue( BACnetEncodeable * pbacnetValue );


// Dialog Data
	//{{AFX_DATA(VTSAny)
	enum { IDD = IDD_ANY };
	CListCtrl			m_ElemList;
	CComboBox			m_TypeCombo;
	CEdit				m_Context;
	CEdit				m_Value;
	CButton				m_ValueIDButton;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSAny)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSAny)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddElem();
	afx_msg void OnRemoveElem();
	afx_msg void OnSelchangeTypeCombo();
	afx_msg void OnChangeContext();
	afx_msg void OnChangeValue();
	afx_msg void OnItemchangingElemList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnValueIDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSANY_H__5E0A2331_6116_11D4_BEC6_00A0C95A9812__INCLUDED_)
