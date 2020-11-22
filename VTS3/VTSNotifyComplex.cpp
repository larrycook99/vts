// VTSNotifyComplex.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "propid.h"

#include "VTSNotifyComplex.h"
#include "VTSNotificationParameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void EncoderToHex( const BACnetAPDUEncoder &enc, CString &str );

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyComplex property page

IMPLEMENT_DYNCREATE(VTSNotifyComplex, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyComplex::VTSNotifyComplex()
	: CPropertyPage(VTSNotifyComplex::IDD)
	, m_PropList( this )
{
	//{{AFX_DATA_INIT(VTSNotifyComplex)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyComplex::~VTSNotifyComplex()
{
}

void VTSNotifyComplex::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(VTSNotifyComplex)
	DDX_Control(pDX, IDC_PROPLIST, m_PropListCtrl);
	//}}AFX_DATA_MAP

	// if there is a selected property, allow the controls to update
	if (m_PropList.colCurElem) {
		m_PropList.colCurElem->coePropCombo.UpdateData( pDX->m_bSaveAndValidate );
		m_PropList.colCurElem->coeArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
		m_PropList.colCurElem->coePriority.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(VTSNotifyComplex, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyComplex)
	ON_BN_CLICKED(IDC_ADDPROP, OnAddProp)
	ON_BN_CLICKED(IDC_REMOVEPROP, OnRemoveProp)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_PROPLIST, OnItemchangingPropListCtrl)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_EN_CHANGE(IDC_PRIORITYX, OnChangePriority)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyComplex::Encode
//

void VTSNotifyComplex::Encode( BACnetAPDUEncoder& enc, int context )
{
	// just pass it along to the list
	m_PropList.Encode( enc );
}

//
//	VTSNotifyComplex::OnSetActive
//

BOOL VTSNotifyComplex::OnSetActive() 
{
	TRACE0( "VTSNotifyComplex::OnSetActive()\n" );

	pageParent->m_EventType = 6;
	return CPropertyPage::OnSetActive();
}

//
//	VTSNotifyComplex::OnInitDialog
//
//	This dialog box initialization must account for the fact that it might not 
//	be the first time that the page was selected.  In fact, the whole dialog 
//	context could go away when the user selects 'OK', but come back again when 
//	the user clicks on the event values button in the CSendEventNotification 
//	page.
//
//	So the basic concept is simple, assume the m_PropList has a bunch of stuff 
//	in it and fill out the list accordingly.
//

BOOL VTSNotifyComplex::OnInitDialog() 
{
	int			i
	;
	CString		someText
	;

	TRACE0( "VTSNotifyComplex::OnInitDialog()\n" );

	CPropertyPage::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_PropListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_PropListCtrl.m_nFlags &= ~LBS_SORT;

	// set up the property list columns
	m_PropListCtrl.InsertColumn( 0, "Property", LVCFMT_LEFT, 96 );
	m_PropListCtrl.InsertColumn( 1, "Index", LVCFMT_RIGHT, 48 );
	m_PropListCtrl.InsertColumn( 2, "Value", LVCFMT_LEFT, 46 );
	m_PropListCtrl.InsertColumn( 3, "Priority", LVCFMT_RIGHT, 48 );

	// fill out the table with the current list of elements
	i = 0;
	for (POSITION pos = m_PropList.GetHeadPosition(); pos != NULL; i++ ) {
		ComplexObjectElemPtr	coep = m_PropList.GetNext( pos )
		;

		m_PropListCtrl.InsertItem( i
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( coep->coePropCombo.m_enumValue )
			);
		if (coep->coeArrayIndex.ctrlNull)
			m_PropListCtrl.SetItemText( i, 1, "" );
		else {
			someText.Format( "%d", coep->coeArrayIndex.uintValue );
			m_PropListCtrl.SetItemText( i, 1, someText );
		}

		BACnetAPDUEncoder enc;
		coep->coeValue.Encode( enc );
		EncoderToHex( enc, someText );
		m_PropListCtrl.SetItemText( i, 2, someText );

		if (coep->coePriority.ctrlNull)
			m_PropListCtrl.SetItemText( i, 3, "" );
		else {
			someText.Format( "%d", coep->coePriority.uintValue );
			m_PropListCtrl.SetItemText( i, 3, someText );
		}
	}

	return TRUE;
}

void VTSNotifyComplex::OnAddProp()
{
	m_PropList.AddButtonClick();
}

void VTSNotifyComplex::OnRemoveProp() 
{
	m_PropList.RemoveButtonClick();
}

void VTSNotifyComplex::OnItemchangingPropListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_PropList.OnItemChanging( pNMHDR, pResult );
}

void VTSNotifyComplex::OnSelchangePropCombo()
{
	m_PropList.OnSelchangePropCombo();
}

void VTSNotifyComplex::OnChangeArrayIndex()
{
	m_PropList.OnChangeArrayIndex();
}

void VTSNotifyComplex::OnValue() 
{
	m_PropList.OnValue();
}

void VTSNotifyComplex::OnChangePriority()
{
	m_PropList.OnChangePriority();
}

//
//	ComplexObjectElem::ComplexObjectElem
//

ComplexObjectElem::ComplexObjectElem( VTSNotifyComplexPtr wp )
	: coePropCombo( wp, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, coeArrayIndex( wp, IDC_ARRAYINDEX )
	, coePriority( wp, IDC_PRIORITYX )
	, coeValue(wp)		// added for proper parent dialog chain
{
	// controls start out disabled
	coePropCombo.ctrlEnabled = false;
	coeArrayIndex.ctrlEnabled = false;
	coePriority.ctrlEnabled = false;
}

//
//	ComplexObjectElem::Bind
//

void ComplexObjectElem::Bind( void )
{
	// set the control value to this element values
	coePropCombo.ObjToCtrl();
	coePropCombo.Enable();
	coeArrayIndex.ObjToCtrl();
	coeArrayIndex.Enable();

	coePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( true );

	coePriority.ObjToCtrl();
	coePriority.Enable();
}

//
//	ComplexObjectElem::Unbind
//

void ComplexObjectElem::Unbind( void )
{
	// clear out the contents of the controls
	coePropCombo.ctrlWindow->GetDlgItem( IDC_PROPCOMBO )->SetWindowText( "" );
	coePropCombo.Disable();
	coeArrayIndex.ctrlWindow->GetDlgItem( IDC_ARRAYINDEX )->SetWindowText( "" );
	coeArrayIndex.Disable();

	coePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( false );

	coePriority.ctrlWindow->GetDlgItem( IDC_PRIORITYX )->SetWindowText( "" );
	coePriority.Disable();
}

//
//	ComplexObjectElem::Encode
//

void ComplexObjectElem::Encode( BACnetAPDUEncoder& enc )
{
	// encode the property
	if (coePropCombo.ctrlNull)
		throw "Property ID required";
	coePropCombo.Encode( enc, 0 );

	// encode the (optional) array index
	if (!coeArrayIndex.ctrlNull)
		coeArrayIndex.Encode( enc, 1 );

	// do the value
	// Allow empty value: some properties are List-Of, with empty list allowed
//	if (coeValue.m_anyList.Length() < 1)
//		throw "Value required";
	coeValue.Encode( enc, 2 );

	// encode the (optional) priority
	if (!coePriority.ctrlNull) {
		if ((coePriority.uintValue < 1) || (coePriority.uintValue > 16))
			throw "Priority out of range 1..16";
		coePriority.Encode( enc, 3 );
	}
}

//
//	ComplexObjectList::ComplexObjectList
//

ComplexObjectList::ComplexObjectList( VTSNotifyComplexPtr pp )
	: colPagePtr( pp )
	, colCurElem(0), colCurElemIndx(0)
{
}

//
//	ComplexObjectList::~ComplexObjectList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

ComplexObjectList::~ComplexObjectList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	ComplexObjectList::AddButtonClick
//
//	Note that where colPagePtr->UpdateEncoded() was commented out, if there was a 
//	pointer from the VTSNotificationParameters object back to the 
//	CSendEventNotification object, then this line (and the others in the functions 
//	below) could be changed to 	colPagePtr->pageParent->###->UpdateExcoded() where 
//	### is the name of the pointer.
//

void ComplexObjectList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = colPagePtr->m_PropListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = colPagePtr->m_PropListCtrl.GetNextSelectedItem( selPos );
		colPagePtr->m_PropListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	colAddInProgress = true;
	colPagePtr->m_PropListCtrl.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	colCurElem = new ComplexObjectElem( colPagePtr );
	colCurElemIndx = listLen;

	colCurElem->coePropCombo.SetEnumValue( PRESENT_VALUE );

	AddTail( colCurElem );

	// bind the element to the controls
	colCurElem->Bind();

	// update the encoding
	colAddInProgress = false;

	OnSelchangePropCombo();				// madanner 9/5/02, refresh field values into list control
	colPagePtr->m_PropListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
//	colPagePtr->UpdateEncoded();
}

//
//	ComplexObjectList::RemoveButtonClick
//

void ComplexObjectList::RemoveButtonClick( void )
{
	int					curRow = colCurElemIndx
	;
	ComplexObjectElemPtr	curElem = colCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	colPagePtr->m_PropListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	colPagePtr->m_PropListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	colPagePtr->m_PropListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
//	colPagePtr->UpdateEncoded();
}

//
//	ComplexObjectList::OnSelchangePropCombo
//

void ComplexObjectList::OnSelchangePropCombo( void )
{
	if (colCurElem) {
		colCurElem->coePropCombo.UpdateData();
//		colPagePtr->UpdateEncoded();

		colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 0
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( colCurElem->coePropCombo.m_enumValue )
			);
	}
}

//
//	ComplexObjectList::OnChangeArrayIndex
//

void ComplexObjectList::OnChangeArrayIndex( void )
{
	if (colCurElem) {
		CString		someText
		;

		colCurElem->coeArrayIndex.UpdateData();
//		colPagePtr->UpdateEncoded();

		if (colCurElem->coeArrayIndex.ctrlNull)
			colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 1, "" );
		else {
			someText.Format( "%d", colCurElem->coeArrayIndex.uintValue );
			colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 1, someText );
		}
	}
}

//
//	ComplexObjectList::OnChangeValue
//

void ComplexObjectList::OnValue( void )
{
	if (colCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		colCurElem->coeValue.DoModal();
//		colPagePtr->UpdateEncoded();

		colCurElem->coeValue.Encode( enc );
		EncoderToHex( enc, someText );
		colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 2, someText );
	}
}

//
//	ComplexObjectList::OnChangePriority
//

void ComplexObjectList::OnChangePriority( void )
{
	if (colCurElem) {
		CString				someText
		;

		colCurElem->coePriority.UpdateData();
//		colPagePtr->UpdateEncoded();

		if (colCurElem->coePriority.ctrlNull)
			colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 3, "" );
		else {
			someText.Format( "%d", colCurElem->coePriority.uintValue );
			colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 3, someText );
		}
	}
}

//
//	ComplexObjectList::OnItemChanging
//

void ComplexObjectList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int						curRow = colCurElemIndx
	;
	ComplexObjectElemPtr	curElem = colCurElem
	;
	NM_LISTVIEW*			pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	// madanner 9/5/02, proper masking check?
	if ((pNMListView->uOldState && LVIS_SELECTED) == (pNMListView->uNewState && LVIS_SELECTED))
		return;

	// skip messages during new item creation
	if (colAddInProgress)
		return;

	if ((pNMListView->uNewState && LVIS_SELECTED) != 0) {		// madanner 9/5/02, proper mask ??
		// item becoming selected
		colCurElemIndx = pNMListView->iItem;
		colCurElem = GetAt( FindIndex( colCurElemIndx ) );

		// bind the new current element
		colCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == colCurElemIndx) {
			// set nothing selected
			colCurElem = 0;
			colCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	ComplexObjectList::Encode
//

void ComplexObjectList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound properties
	BACnetOpeningTag().Encode( enc, 6 );
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
	BACnetClosingTag().Encode( enc, 6 );
}
