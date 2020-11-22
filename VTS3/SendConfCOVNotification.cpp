// SendConfCOVNotification.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "Propid.h"

#include "Send.h"
#include "SendConfCOVNotification.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendConfCOVNotification::pageContents;

void EncoderToHex( const BACnetAPDUEncoder &enc, CString &str );

/////////////////////////////////////////////////////////////////////////////
// CSendConfCOVNotification dialog

IMPLEMENT_DYNCREATE( CSendConfCOVNotification, CPropertyPage )

#pragma warning( disable : 4355 )
CSendConfCOVNotification::CSendConfCOVNotification( void )
	: CSendPage( CSendConfCOVNotification::IDD )
	, m_ProcessID( this, IDC_PROCESSID )
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_TimeRemaining( this, IDC_TIMEREMAINING )
	, m_PropList( this )
{
	//{{AFX_DATA_INIT(CSendConfCOVNotification)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendConfCOVNotification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CSendConfCOVNotification)
	DDX_Control(pDX, IDC_PROPLIST, m_PropListCtrl);
	//}}AFX_DATA_MAP

	m_ProcessID.UpdateData( pDX->m_bSaveAndValidate );
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_TimeRemaining.UpdateData( pDX->m_bSaveAndValidate );

	// if there is a selected property, allow the controls to update
	if (m_PropList.cnlCurElem) {
		m_PropList.cnlCurElem->cnePropCombo.UpdateData( pDX->m_bSaveAndValidate );
		m_PropList.cnlCurElem->cneArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
		m_PropList.cnlCurElem->cnePriority.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(CSendConfCOVNotification, CPropertyPage)
	//{{AFX_MSG_MAP(CSendConfCOVNotification)
	ON_EN_CHANGE(IDC_PROCESSID, OnChangeProcessID)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceID)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceIDButton)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_EN_CHANGE(IDC_TIMEREMAINING, OnChangeTimeRemaining)
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
//	CSendConfCOVNotification::InitPage
//

void CSendConfCOVNotification::InitPage( void )
{
	TRACE0( "CSendConfCOVNotification::InitPage()\n" );

	// flush the data
	m_ProcessID.ctrlNull = true;
	m_DeviceID.ctrlNull = true;
	m_ObjectID.ctrlNull = true;
	m_TimeRemaining.ctrlNull = true;
}

//
//	CSendConfCOVNotification::EncodePage
//

void CSendConfCOVNotification::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 1 );

	// encode the subscriber process ID
	if (m_ProcessID.ctrlNull)
		throw "Subscriber process ID required";
	m_ProcessID.Encode( enc, 0 );

	// encode the initiating device ID
	if (m_DeviceID.ctrlNull)
		throw "Initiating device ID required";
	m_DeviceID.Encode( enc, 1 );

	// encode the monitored object ID
	if (m_ObjectID.ctrlNull)
		throw "Monitored object ID required";
	m_ObjectID.Encode( enc, 2 );

	// encode the time remaining
	if (m_TimeRemaining.ctrlNull)
		throw "Time remaining required";
	m_TimeRemaining.Encode( enc, 3 );

	// encode the contents
	m_PropList.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendConfCOVNotification::SavePage
//

void CSendConfCOVNotification::SavePage( void )
{
	TRACE0( "CSendConfCOVNotification::SavePage\n" );

	pageContents.Flush();

	m_ProcessID.SaveCtrl( pageContents );
	m_DeviceID.SaveCtrl( pageContents );
	m_ObjectID.SaveCtrl( pageContents );
	m_TimeRemaining.SaveCtrl( pageContents );
//	m_PropList.SaveList( pageContents );
}

//
//	CSendConfCOVNotification::RestorePage
//

void CSendConfCOVNotification::RestorePage(  int index  )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendConfCOVNotification::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ProcessID.RestoreCtrl( dec );
	m_DeviceID.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );
	m_TimeRemaining.RestoreCtrl( dec );
//	m_PropList.RestoreCtrl( dec );
}

//
//	CSendConfCOVNotification::OnInitDialog
//

BOOL CSendConfCOVNotification::OnInitDialog() 
{
	TRACE0( "CSendConfCOVNotification::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_PropListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_PropListCtrl.m_nFlags &= ~LBS_SORT;

	// set up the property list cnlumns
	m_PropListCtrl.InsertColumn( 0, "Property", LVCFMT_LEFT, 96 );
	m_PropListCtrl.InsertColumn( 1, "Index", LVCFMT_RIGHT, 48 );
	m_PropListCtrl.InsertColumn( 2, "Value", LVCFMT_LEFT, 96 );
	m_PropListCtrl.InsertColumn( 3, "Priority", LVCFMT_RIGHT, 48 );

	return TRUE;
}

void CSendConfCOVNotification::OnChangeProcessID()
{
	m_ProcessID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfCOVNotification::OnChangeDeviceID()
{
	m_DeviceID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfCOVNotification::OnDeviceIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)		// added parent for send dialog
	;

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendConfCOVNotification::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfCOVNotification::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)		// added parent for send dialog
	;

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendConfCOVNotification::OnChangeTimeRemaining()
{
	m_TimeRemaining.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfCOVNotification::OnAddProp() 
{
	m_PropList.AddButtonClick( m_ObjectID.objID >> 22 );
	SavePage();
}

void CSendConfCOVNotification::OnRemoveProp() 
{
	m_PropList.RemoveButtonClick();
	SavePage();
}

void CSendConfCOVNotification::OnItemchangingPropListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_PropList.OnItemChanging( pNMHDR, pResult );
	SavePage();
}

void CSendConfCOVNotification::OnSelchangePropCombo()
{
	m_PropList.OnSelchangePropCombo();
	SavePage();
}

void CSendConfCOVNotification::OnChangeArrayIndex()
{
	m_PropList.OnChangeArrayIndex();
	SavePage();
}

void CSendConfCOVNotification::OnValue() 
{
	m_PropList.OnValue();
	SavePage();
}

void CSendConfCOVNotification::OnChangePriority()
{
	m_PropList.OnChangePriority();
	SavePage();
}

//
//	COVNotificationElem::COVNotificationElem
//

COVNotificationElem::COVNotificationElem( CSendPagePtr wp )
	: cnePropCombo( wp, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, cneArrayIndex( wp, IDC_ARRAYINDEX )
	, cnePriority( wp, IDC_PRIORITYX )
	, cneValue(wp)		// added parent for send page
{
	// controls start out disabled
	cnePropCombo.ctrlEnabled = false;
	cneArrayIndex.ctrlEnabled = false;
	cnePriority.ctrlEnabled = false;
}

//
//	COVNotificationElem::Bind
//

void COVNotificationElem::Bind( void )
{
	// set the control value to this element values
	cnePropCombo.ObjToCtrl();
	cnePropCombo.Enable();
	cneArrayIndex.ObjToCtrl();
	cneArrayIndex.Enable();

	cnePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( true );

	cnePriority.ObjToCtrl();
	cnePriority.Enable();
}

//
//	COVNotificationElem::Unbind
//

void COVNotificationElem::Unbind( void )
{
	// clear out the contents of the controls
	cnePropCombo.ctrlWindow->GetDlgItem( IDC_PROPCOMBO )->SetWindowText( "" );
	cnePropCombo.Disable();
	cneArrayIndex.ctrlWindow->GetDlgItem( IDC_ARRAYINDEX )->SetWindowText( "" );
	cneArrayIndex.Disable();

	cnePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( false );

	cnePriority.ctrlWindow->GetDlgItem( IDC_PRIORITYX )->SetWindowText( "" );
	cnePriority.Disable();
}

//
//	COVNotificationElem::Encode
//

void COVNotificationElem::Encode( BACnetAPDUEncoder& enc )
{
	// encode the property
	if (cnePropCombo.ctrlNull)
		throw "Property ID required";
	cnePropCombo.Encode( enc, 0 );

	// encode the (optional) array index
	if (!cneArrayIndex.ctrlNull)
		cneArrayIndex.Encode( enc, 1 );

	// do the value
	if (cneValue.m_anyList.Length() < 1)
		throw "Value required";
	cneValue.Encode( enc, 2 );

	// encode the (optional) priority
	if (!cnePriority.ctrlNull) {
		if ((cnePriority.uintValue < 1) || (cnePriority.uintValue > 16))
			throw "Priority out of range 1..16";
		cnePriority.Encode( enc, 3 );
	}
}

//
//	COVNotificationList::COVNotificationList
//

COVNotificationList::COVNotificationList( CSendConfCOVNotificationPtr pp )
	: cnlPagePtr( pp )
	, cnlCurElem(0), cnlCurElemIndx(0)
{
}

//
//	COVNotificationList::~COVNotificationList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

COVNotificationList::~COVNotificationList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	COVNotificationList::AddButtonClick
//

void COVNotificationList::AddButtonClick( int theObjectType )
{
	int		listLen = GetCount();

	// deselect if something was selected
	POSITION selPos = cnlPagePtr->m_PropListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = cnlPagePtr->m_PropListCtrl.GetNextSelectedItem( selPos );
		cnlPagePtr->m_PropListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	cnlAddInProgress = true;
	cnlPagePtr->m_PropListCtrl.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	cnlCurElem = new COVNotificationElem( cnlPagePtr );
	cnlCurElemIndx = listLen;

	// Make a property list of the appropriate type
	cnlCurElem->cnePropCombo.m_nObjType = theObjectType;
	cnlCurElem->cnePropCombo.LoadCombo();
	cnlCurElem->cnePropCombo.SetEnumValue( PRESENT_VALUE );

	AddTail( cnlCurElem );

	// bind the element to the controls
	cnlCurElem->Bind();

	// update the encoding
	cnlAddInProgress = false;

	// Enable the property control
	OnSelchangePropCombo();				// Insert new list text for Present_Value
	cnlPagePtr->m_PropListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
//	cnlPagePtr->UpdateEncoded();
}

//
//	COVNotificationList::RemoveButtonClick
//

void COVNotificationList::RemoveButtonClick( void )
{
	int					curRow = cnlCurElemIndx
	;
	COVNotificationElemPtr	curElem = cnlCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	cnlPagePtr->m_PropListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	cnlPagePtr->m_PropListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}
	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	cnlPagePtr->m_PropListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	cnlPagePtr->UpdateEncoded();
}

//
//	COVNotificationList::OnSelchangePropCombo
//

void COVNotificationList::OnSelchangePropCombo( void )
{
	if (cnlCurElem) {
		cnlCurElem->cnePropCombo.UpdateData();
		cnlPagePtr->UpdateEncoded();

		cnlPagePtr->m_PropListCtrl.SetItemText( cnlCurElemIndx, 0
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( cnlCurElem->cnePropCombo.m_enumValue )
			);
	}
}

//
//	COVNotificationList::OnChangeArrayIndex
//

void COVNotificationList::OnChangeArrayIndex( void )
{
	if (cnlCurElem) {
		CString		someText
		;

		cnlCurElem->cneArrayIndex.UpdateData();
		cnlPagePtr->UpdateEncoded();

		if (cnlCurElem->cneArrayIndex.ctrlNull)
			cnlPagePtr->m_PropListCtrl.SetItemText( cnlCurElemIndx, 1, "" );
		else {
			someText.Format( "%d", cnlCurElem->cneArrayIndex.uintValue );
			cnlPagePtr->m_PropListCtrl.SetItemText( cnlCurElemIndx, 1, someText );
		}
	}
}

//
//	COVNotificationList::OnChangeValue
//

void COVNotificationList::OnValue( void )
{
	if (cnlCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		cnlCurElem->cneValue.DoModal();
		cnlPagePtr->UpdateEncoded();

		cnlCurElem->cneValue.Encode( enc );
		EncoderToHex( enc, someText );
		cnlPagePtr->m_PropListCtrl.SetItemText( cnlCurElemIndx, 2, someText );
	}
}

//
//	COVNotificationList::OnChangePriority
//

void COVNotificationList::OnChangePriority( void )
{
	if (cnlCurElem) {
		CString				someText
		;

		cnlCurElem->cnePriority.UpdateData();
		cnlPagePtr->UpdateEncoded();

		if (cnlCurElem->cnePriority.ctrlNull)
			cnlPagePtr->m_PropListCtrl.SetItemText( cnlCurElemIndx, 3, "" );
		else {
			someText.Format( "%d", cnlCurElem->cnePriority.uintValue );
			cnlPagePtr->m_PropListCtrl.SetItemText( cnlCurElemIndx, 3, someText );
		}
	}
}

//
//	COVNotificationList::OnItemChanging
//

void COVNotificationList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = cnlCurElemIndx
	;
	COVNotificationElemPtr	curElem = cnlCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (cnlAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		// item becoming selected
		cnlCurElemIndx = pNMListView->iItem;
		cnlCurElem = GetAt( FindIndex( cnlCurElemIndx ) );

		// bind the new current element
		cnlCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == cnlCurElemIndx) {
			// set nothing selected
			cnlCurElem = 0;
			cnlCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	COVNotificationList::Encode
//

void COVNotificationList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound properties
	BACnetOpeningTag().Encode( enc, 4 );
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
	BACnetClosingTag().Encode( enc, 4 );
}
