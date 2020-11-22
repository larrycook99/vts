// SendUnconfCOVNotification.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "propid.h"

#include "Send.h"
#include "SendUnconfCOVNotification.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void EncoderToHex( const BACnetAPDUEncoder &enc, CString &str );

BACnetAPDUEncoder CSendUnconfCOVNotification::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendUnconfCOVNotification dialog

IMPLEMENT_DYNCREATE( CSendUnconfCOVNotification, CPropertyPage )

#pragma warning( disable : 4355 )
CSendUnconfCOVNotification::CSendUnconfCOVNotification( void )
	: CSendPage( CSendUnconfCOVNotification::IDD )
	, m_ProcessID( this, IDC_PROCESSID )
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_TimeRemaining( this, IDC_TIMEREMAINING )
	, m_PropList( this )
{
	//{{AFX_DATA_INIT(CSendUnconfCOVNotification)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendUnconfCOVNotification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CSendUnconfCOVNotification)
	DDX_Control(pDX, IDC_PROPLIST, m_PropListCtrl);
	//}}AFX_DATA_MAP

	m_ProcessID.UpdateData( pDX->m_bSaveAndValidate );
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_TimeRemaining.UpdateData( pDX->m_bSaveAndValidate );

	// if there is a selected property, allow the controls to update
	if (m_PropList.unlCurElem) {
		m_PropList.unlCurElem->unePropCombo.UpdateData( pDX->m_bSaveAndValidate );
		m_PropList.unlCurElem->uneArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
		m_PropList.unlCurElem->unePriority.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(CSendUnconfCOVNotification, CPropertyPage)
	//{{AFX_MSG_MAP(CSendUnconfCOVNotification)
	ON_EN_CHANGE(IDC_PROCESSID, OnChangeProcessID)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceID)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_EN_CHANGE(IDC_TIMEREMAINING, OnChangeTimeRemaining)
	ON_BN_CLICKED(IDC_ADDPROP, OnAddProp)
	ON_BN_CLICKED(IDC_REMOVEPROP, OnRemoveProp)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_PROPLIST, OnItemchangingPropListCtrl)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_EN_CHANGE(IDC_PRIORITYX, OnChangePriority)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceIDButton)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendUnconfCOVNotification::InitPage
//

void CSendUnconfCOVNotification::InitPage( void )
{
	TRACE0( "CSendUnconfCOVNotification::InitPage()\n" );

	// flush the data
	m_ProcessID.ctrlNull = true;
	m_DeviceID.ctrlNull = true;
	m_ObjectID.ctrlNull = true;
	m_TimeRemaining.ctrlNull = true;
}

//
//	CSendUnconfCOVNotification::EncodePage
//

void CSendUnconfCOVNotification::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the pdu type and service choice
	header.Add( 0x10 );
	header.Add( 0x02 );

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
//	CSendUnconfCOVNotification::SavePage
//

void CSendUnconfCOVNotification::SavePage( void )
{
	TRACE0( "CSendUnconfCOVNotification::SavePage\n" );

	pageContents.Flush();

	m_ProcessID.SaveCtrl( pageContents );
	m_DeviceID.SaveCtrl( pageContents );
	m_ObjectID.SaveCtrl( pageContents );
	m_TimeRemaining.SaveCtrl( pageContents );
//	m_PropList.SaveList();
}

//
//	CSendUnconfCOVNotification::RestorePage
//

void CSendUnconfCOVNotification::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendUnconfCOVNotification::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ProcessID.RestoreCtrl( dec );
	m_DeviceID.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );
	m_TimeRemaining.RestoreCtrl( dec );
//	m_PropList.RestoreList();
}

//
//	CSendUnconfCOVNotification::OnInitDialog
//

BOOL CSendUnconfCOVNotification::OnInitDialog() 
{
	TRACE0( "CSendUnconfCOVNotification::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_PropListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_PropListCtrl.m_nFlags &= ~LBS_SORT;

	// set up the property list unlumns
	m_PropListCtrl.InsertColumn( 0, "Property", LVCFMT_LEFT, 96 );
	m_PropListCtrl.InsertColumn( 1, "Index", LVCFMT_RIGHT, 48 );
	m_PropListCtrl.InsertColumn( 2, "Value", LVCFMT_LEFT, 96 );
	m_PropListCtrl.InsertColumn( 3, "Priority", LVCFMT_RIGHT, 48 );

	return TRUE;
}

void CSendUnconfCOVNotification::OnChangeProcessID()
{
	m_ProcessID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendUnconfCOVNotification::OnChangeDeviceID()
{
	m_DeviceID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendUnconfCOVNotification::OnDeviceIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// added parent for proper send dlg function
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

void CSendUnconfCOVNotification::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendUnconfCOVNotification::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// added parent for proper send dlg function
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

void CSendUnconfCOVNotification::OnChangeTimeRemaining()
{
	m_TimeRemaining.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendUnconfCOVNotification::OnAddProp() 
{
	m_PropList.AddButtonClick( m_ObjectID.objID >> 22 );
}

void CSendUnconfCOVNotification::OnRemoveProp() 
{
	m_PropList.RemoveButtonClick();
}

void CSendUnconfCOVNotification::OnItemchangingPropListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_PropList.OnItemChanging( pNMHDR, pResult );
}

void CSendUnconfCOVNotification::OnSelchangePropCombo()
{
	m_PropList.OnSelchangePropCombo();
}

void CSendUnconfCOVNotification::OnChangeArrayIndex()
{
	m_PropList.OnChangeArrayIndex();
}

void CSendUnconfCOVNotification::OnValue() 
{
	m_PropList.OnValue();
}

void CSendUnconfCOVNotification::OnChangePriority()
{
	m_PropList.OnChangePriority();
}

//
//	UnconfCOVNotificationElem::UnconfCOVNotificationElem
//

UnconfCOVNotificationElem::UnconfCOVNotificationElem( CSendPagePtr wp )
	: unePropCombo( wp, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, uneArrayIndex( wp, IDC_ARRAYINDEX )
	, unePriority( wp, IDC_PRIORITYX )
	, uneValue(wp)				// added parent for proper send dlg function
{
	// controls start out disabled
	unePropCombo.ctrlEnabled = false;
	uneArrayIndex.ctrlEnabled = false;
	unePriority.ctrlEnabled = false;
}

//
//	UnconfCOVNotificationElem::Bind
//

void UnconfCOVNotificationElem::Bind( void )
{
	// set the control value to this element values
	unePropCombo.ObjToCtrl();
	unePropCombo.Enable();
	uneArrayIndex.ObjToCtrl();
	uneArrayIndex.Enable();

	unePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( true );

	unePriority.ObjToCtrl();
	unePriority.Enable();
}

//
//	UnconfCOVNotificationElem::Unbind
//

void UnconfCOVNotificationElem::Unbind( void )
{
	// clear out the contents of the controls
	unePropCombo.ctrlWindow->GetDlgItem( IDC_PROPCOMBO )->SetWindowText( "" );
	unePropCombo.Disable();
	uneArrayIndex.ctrlWindow->GetDlgItem( IDC_ARRAYINDEX )->SetWindowText( "" );
	uneArrayIndex.Disable();

	unePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( false );

	unePriority.ctrlWindow->GetDlgItem( IDC_PRIORITYX )->SetWindowText( "" );
	unePriority.Disable();
}

//
//	UnconfCOVNotificationElem::Encode
//

void UnconfCOVNotificationElem::Encode( BACnetAPDUEncoder& enc )
{
	// encode the property
	if (unePropCombo.ctrlNull)
		throw "Property ID required";
	unePropCombo.Encode( enc, 0 );

	// encode the (optional) array index
	if (!uneArrayIndex.ctrlNull)
		uneArrayIndex.Encode( enc, 1 );

	// do the value
	if (uneValue.m_anyList.Length() < 1)
		throw "Value required";
	uneValue.Encode( enc, 2 );

	// encode the (optional) priority
	if (!unePriority.ctrlNull) {
		if ((unePriority.uintValue < 1) || (unePriority.uintValue > 16))
			throw "Priority out of range 1..16";
		unePriority.Encode( enc, 3 );
	}
}

//
//	UnconfCOVNotificationList::UnconfCOVNotificationList
//

UnconfCOVNotificationList::UnconfCOVNotificationList( CSendUnconfCOVNotificationPtr pp )
	: unlPagePtr( pp )
	, unlCurElem(0), unlCurElemIndx(0)
{
}

//
//	UnconfCOVNotificationList::~UnconfCOVNotificationList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

UnconfCOVNotificationList::~UnconfCOVNotificationList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	UnconfCOVNotificationList::AddButtonClick
//

void UnconfCOVNotificationList::AddButtonClick( int theObjectType )
{
	int		listLen = GetCount();

	// deselect if something was selected
	POSITION selPos = unlPagePtr->m_PropListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = unlPagePtr->m_PropListCtrl.GetNextSelectedItem( selPos );
		unlPagePtr->m_PropListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	unlAddInProgress = true;
	unlPagePtr->m_PropListCtrl.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	unlCurElem = new UnconfCOVNotificationElem( unlPagePtr );
	unlCurElemIndx = listLen;

	// Make a property list of the appropriate type
	unlCurElem->unePropCombo.m_nObjType = theObjectType;
	unlCurElem->unePropCombo.LoadCombo();
	unlCurElem->unePropCombo.SetEnumValue( PRESENT_VALUE );

	AddTail( unlCurElem );

	// bind the element to the controls
	unlCurElem->Bind();

	// update the encoding
	unlAddInProgress = false;

	// madanner, 9/3/02.
	OnSelchangePropCombo();				// Insert new list text for Present_Value
	unlPagePtr->m_PropListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
//	unlPagePtr->UpdateEncoded();
}

//
//	UnconfCOVNotificationList::RemoveButtonClick
//

void UnconfCOVNotificationList::RemoveButtonClick( void )
{
	int					curRow = unlCurElemIndx
	;
	UnconfCOVNotificationElemPtr	curElem = unlCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	unlPagePtr->m_PropListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	unlPagePtr->m_PropListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	unlPagePtr->m_PropListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	unlPagePtr->UpdateEncoded();
}

//
//	UnconfCOVNotificationList::OnSelchangePropCombo
//

void UnconfCOVNotificationList::OnSelchangePropCombo( void )
{
	if (unlCurElem) {
		unlCurElem->unePropCombo.UpdateData();
		unlPagePtr->UpdateEncoded();

		unlPagePtr->m_PropListCtrl.SetItemText( unlCurElemIndx, 0
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( unlCurElem->unePropCombo.m_enumValue )
			);
	}
}

//
//	UnconfCOVNotificationList::OnChangeArrayIndex
//

void UnconfCOVNotificationList::OnChangeArrayIndex( void )
{
	if (unlCurElem) {
		CString		someText
		;

		unlCurElem->uneArrayIndex.UpdateData();
		unlPagePtr->UpdateEncoded();

		if (unlCurElem->uneArrayIndex.ctrlNull)
			unlPagePtr->m_PropListCtrl.SetItemText( unlCurElemIndx, 1, "" );
		else {
			someText.Format( "%d", unlCurElem->uneArrayIndex.uintValue );
			unlPagePtr->m_PropListCtrl.SetItemText( unlCurElemIndx, 1, someText );
		}
	}
}

//
//	UnconfCOVNotificationList::OnChangeValue
//

void UnconfCOVNotificationList::OnValue( void )
{
	if (unlCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		unlCurElem->uneValue.DoModal();
		unlPagePtr->UpdateEncoded();

		unlCurElem->uneValue.Encode( enc );
		EncoderToHex( enc, someText );
		unlPagePtr->m_PropListCtrl.SetItemText( unlCurElemIndx, 2, someText );
	}
}

//
//	UnconfCOVNotificationList::OnChangePriority
//

void UnconfCOVNotificationList::OnChangePriority( void )
{
	if (unlCurElem) {
		CString				someText
		;

		unlCurElem->unePriority.UpdateData();
		unlPagePtr->UpdateEncoded();

		if (unlCurElem->unePriority.ctrlNull)
			unlPagePtr->m_PropListCtrl.SetItemText( unlCurElemIndx, 3, "" );
		else {
			someText.Format( "%d", unlCurElem->unePriority.uintValue );
			unlPagePtr->m_PropListCtrl.SetItemText( unlCurElemIndx, 3, someText );
		}
	}
}

//
//	UnconfCOVNotificationList::OnItemChanging
//

void UnconfCOVNotificationList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = unlCurElemIndx
	;
	UnconfCOVNotificationElemPtr	curElem = unlCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (unlAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		// item becoming selected
		unlCurElemIndx = pNMListView->iItem;
		unlCurElem = GetAt( FindIndex( unlCurElemIndx ) );

		// bind the new current element
		unlCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == unlCurElemIndx) {
			// set nothing selected
			unlCurElem = 0;
			unlCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	UnconfCOVNotificationList::Encode
//

void UnconfCOVNotificationList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound properties
	BACnetOpeningTag().Encode( enc, 4 );
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
	BACnetClosingTag().Encode( enc, 4 );
}
