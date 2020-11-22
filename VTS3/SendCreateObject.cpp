// SendCreateObject.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "propid.h"

#include "Send.h"
#include "SendCreateObject.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendCreateObject::pageContents;

void EncoderToHex( const BACnetAPDUEncoder &enc, CString &str );

/////////////////////////////////////////////////////////////////////////////
// CSendCreateObject dialog

IMPLEMENT_DYNCREATE( CSendCreateObject, CPropertyPage )

#pragma warning( disable : 4355 )
CSendCreateObject::CSendCreateObject( void )
	: CSendPage( CSendCreateObject::IDD )
	, m_ObjectTypeCombo( this, IDC_OBJECTTYPECOMBO, NetworkSniffer::BAC_STRTAB_BACnetObjectType, true, true )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_PropList( this )
{
	//{{AFX_DATA_INIT(CSendCreateObject)
	//}}AFX_DATA_INIT
	m_nChoice = 0;
}
#pragma warning( default : 4355 )

void CSendCreateObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CSendCreateObject)
	DDX_Control(pDX, IDC_PROPLIST, m_PropListCtrl);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_RADIO1, m_nChoice);

	m_ObjectTypeCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );

	// if there is a selected property, allow the controls to update
	if (m_PropList.colCurElem) {
		m_PropList.colCurElem->coePropCombo.UpdateData( pDX->m_bSaveAndValidate );
		m_PropList.colCurElem->coeArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
		m_PropList.colCurElem->coePriority.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(CSendCreateObject, CPropertyPage)
	//{{AFX_MSG_MAP(CSendCreateObject)
	ON_CBN_SELCHANGE(IDC_OBJECTTYPECOMBO, OnSelchangeObjectTypeCombo)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_BN_CLICKED(IDC_ADDPROP, OnAddProp)
	ON_BN_CLICKED(IDC_REMOVEPROP, OnRemoveProp)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_PROPLIST, OnItemchangingPropListCtrl)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_EN_CHANGE(IDC_PRIORITYX, OnChangePriority)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO1, OnSelectObjectType)
	ON_BN_CLICKED(IDC_RADIO2, OnSelectObjectID)

END_MESSAGE_MAP()

//
//	CSendCreateObject::InitPage
//

void CSendCreateObject::InitPage( void )
{
	TRACE0( "CSendCreateObject::InitPage()\n" );

	// flush the data
	m_ObjectTypeCombo.ctrlNull = true;
	m_ObjectID.ctrlNull = true;

	// LJT: added to control when controls should be active, ctrlNull can't be used because a filled combo box sets to false
	m_bObjectTypeActive = true;
}

//
//	CSendCreateObject::EncodePage
//

void CSendCreateObject::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 10 );

	// encode the object type or object identifier
	BACnetOpeningTag().Encode( enc, 0 );
  
	// LJT: Updated section
	if (m_bObjectTypeActive)
	{
		m_ObjectTypeCombo.Encode( enc, 0 );
	} else
	if (!m_ObjectID.ctrlNull) {
		m_ObjectID.Encode( enc, 1 );
	} else
		throw "Object type or identifier required";
	BACnetClosingTag().Encode( enc, 0 );

	// encode the contents
	m_PropList.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendCreateObject::SavePage
//

void CSendCreateObject::SavePage( void )
{
	TRACE0( "CSendCreateObject::SavePage\n" );

	pageContents.Flush();

	m_ObjectTypeCombo.SaveCtrl( pageContents );
	m_ObjectID.SaveCtrl( pageContents );
//	m_PropList.SaveList( pageContents );
}

//
//	CSendCreateObject::RestorePage
//

void CSendCreateObject::RestorePage(  int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendCreateObject::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ObjectTypeCombo.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );

//	m_PropList.RestoreList( dec );
}

//
//	CSendCreateObject::OnInitDialog
//

BOOL CSendCreateObject::OnInitDialog() 
{
	TRACE0( "CSendCreateObject::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// load the enumeration table
	m_ObjectTypeCombo.LoadCombo();
	
	m_ObjectID.Disable();

	// only allow one selection at a time, no sorting
	m_PropListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_PropListCtrl.m_nFlags &= ~LBS_SORT;

	// set up the property list columns
	m_PropListCtrl.InsertColumn( 0, "Property", LVCFMT_LEFT, 96 );
	m_PropListCtrl.InsertColumn( 1, "Index", LVCFMT_RIGHT, 48 );
	m_PropListCtrl.InsertColumn( 2, "Value", LVCFMT_LEFT, 96 );
	m_PropListCtrl.InsertColumn( 3, "Priority", LVCFMT_RIGHT, 48 );

	return TRUE;
}

void CSendCreateObject::OnSelchangeObjectTypeCombo()
{
	m_ObjectTypeCombo.UpdateData();

	SavePage();
	UpdateEncoded();
}

void CSendCreateObject::OnChangeObjectID()
{
	m_ObjectID.UpdateData();

	SavePage();
	UpdateEncoded();
}

void CSendCreateObject::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this);			// for proper parent control

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendCreateObject::OnAddProp()
{
	int type = (m_bObjectTypeActive) ? m_ObjectTypeCombo.m_enumValue : (m_ObjectID.objID >> 22);
	m_PropList.AddButtonClick(type);
	SavePage();
}

void CSendCreateObject::OnRemoveProp()
{
	m_PropList.RemoveButtonClick();
	SavePage();
}

void CSendCreateObject::OnItemchangingPropListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_PropList.OnItemChanging( pNMHDR, pResult );
	SavePage();
}

void CSendCreateObject::OnSelchangePropCombo()
{
	m_PropList.OnSelchangePropCombo();
	SavePage();
}

void CSendCreateObject::OnChangeArrayIndex()
{
	m_PropList.OnChangeArrayIndex();
	SavePage();
}

void CSendCreateObject::OnValue() 
{
	m_PropList.OnValue();
	SavePage();
}

void CSendCreateObject::OnChangePriority()
{
	m_PropList.OnChangePriority();
	SavePage();
}

//
//	CreateObjectElem::CreateObjectElem
//

CreateObjectElem::CreateObjectElem( CSendPagePtr wp )
	: coePropCombo( wp, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, coeArrayIndex( wp, IDC_ARRAYINDEX )
	, coePriority( wp, IDC_PRIORITYX )
	, coeValue(wp)			// for proper parent control
{
	// controls start out disabled
	coePropCombo.ctrlEnabled = false;
	coeArrayIndex.ctrlEnabled = false;
	coePriority.ctrlEnabled = false;
}

//
//	CreateObjectElem::Bind
//

void CreateObjectElem::Bind( void )
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
//	CreateObjectElem::Unbind
//

void CreateObjectElem::Unbind( void )
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
//	CreateObjectElem::Encode
//

void CreateObjectElem::Encode( BACnetAPDUEncoder& enc )
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
//	CreateObjectList::CreateObjectList
//

CreateObjectList::CreateObjectList( CSendCreateObjectPtr pp )
	: colPagePtr( pp )
	, colCurElem(0), colCurElemIndx(0)
{
}

//
//	CreateObjectList::~CreateObjectList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

CreateObjectList::~CreateObjectList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	CreateObjectList::AddButtonClick
//

void CreateObjectList::AddButtonClick( int theObjectType )
{
	int		listLen = GetCount();

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
	colCurElem = new CreateObjectElem( colPagePtr );
	colCurElemIndx = listLen;

	// Make a property list of the appropriate type
	colCurElem->coePropCombo.m_nObjType = theObjectType;
	colCurElem->coePropCombo.LoadCombo();
	colCurElem->coePropCombo.SetEnumValue( PRESENT_VALUE );

	AddTail( colCurElem );

	// bind the element to the controls
	colCurElem->Bind();

	// update the encoding
	colAddInProgress = false;

	// madanner, 9/3/02.
	OnSelchangePropCombo();				// Insert new list text for Present_Value
	colPagePtr->m_PropListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
//	colPagePtr->UpdateEncoded();
}

//
//	CreateObjectList::RemoveButtonClick
//

void CreateObjectList::RemoveButtonClick( void )
{
	int					curRow = colCurElemIndx
	;
	CreateObjectElemPtr	curElem = colCurElem
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
	colPagePtr->UpdateEncoded();
}

//
//	CreateObjectList::OnSelchangePropCombo
//

void CreateObjectList::OnSelchangePropCombo( void )
{
	if (colCurElem) {
		colCurElem->coePropCombo.UpdateData();
		colPagePtr->UpdateEncoded();

		colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 0
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( colCurElem->coePropCombo.m_enumValue )
			);
	}
}

//
//	CreateObjectList::OnChangeArrayIndex
//

void CreateObjectList::OnChangeArrayIndex( void )
{
	if (colCurElem) {
		CString		someText
		;

		colCurElem->coeArrayIndex.UpdateData();
		colPagePtr->UpdateEncoded();

		if (colCurElem->coeArrayIndex.ctrlNull)
			colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 1, "" );
		else {
			someText.Format( "%d", colCurElem->coeArrayIndex.uintValue );
			colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 1, someText );
		}
	}
}

//
//	CreateObjectList::OnChangeValue
//

void CreateObjectList::OnValue( void )
{
	if (colCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		colCurElem->coeValue.DoModal();
		colPagePtr->UpdateEncoded();

		colCurElem->coeValue.Encode( enc );
		EncoderToHex( enc, someText );
		colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 2, someText );
	}
}

//
//	CreateObjectList::OnChangePriority
//

void CreateObjectList::OnChangePriority( void )
{
	if (colCurElem) {
		CString				someText
		;

		colCurElem->coePriority.UpdateData();
		colPagePtr->UpdateEncoded();

		if (colCurElem->coePriority.ctrlNull)
			colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 3, "" );
		else {
			someText.Format( "%d", colCurElem->coePriority.uintValue );
			colPagePtr->m_PropListCtrl.SetItemText( colCurElemIndx, 3, someText );
		}
	}
}

//
//	CreateObjectList::OnItemChanging
//

void CreateObjectList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = colCurElemIndx
	;
	CreateObjectElemPtr	curElem = colCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (colAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
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
//	CreateObjectList::Encode
//

void CreateObjectList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound properties
	BACnetOpeningTag().Encode( enc, 1 );
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
	BACnetClosingTag().Encode( enc, 1 );
}

void CSendCreateObject::OnSelectObjectType()
{
	UpdateData();

	m_ObjectTypeCombo.Enable();
	m_ObjectID.Disable();

	m_bObjectTypeActive = true;

	SavePage();
	UpdateEncoded();
}

void CSendCreateObject::OnSelectObjectID()
{
	UpdateData();

	m_ObjectID.Enable();
	m_ObjectTypeCombo.Disable();

	m_bObjectTypeActive = false;

	SavePage();
	UpdateEncoded();
}