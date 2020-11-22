// SendGetEnrollmentSummaryACK.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendGetEnrollmentSummaryACK.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendGetEnrollmentSummaryACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendGetEnrollmentSummaryACK dialog

IMPLEMENT_DYNCREATE( CSendGetEnrollmentSummaryACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendGetEnrollmentSummaryACK::CSendGetEnrollmentSummaryACK( void )
	: CSendPage( CSendGetEnrollmentSummaryACK::IDD )
	, m_EnrollmentSumList( this )
{
	//{{AFX_DATA_INIT(CSendGetEnrollmentSummaryACK)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendGetEnrollmentSummaryACK::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CSendGetEnrollmentSummaryACK)
	DDX_Control(pDX, IDC_ENROLLMENTSUMLIST, m_EnrollmentSumListCtrl);
	//}}AFX_DATA_MAP

	// if there is a selected property, allow the controls to update
	if (m_EnrollmentSumList.eslCurElem) {
		m_EnrollmentSumList.eslCurElem->eseObjectID.UpdateData( pDX->m_bSaveAndValidate );
		m_EnrollmentSumList.eslCurElem->eseEventTypeCombo.UpdateData( pDX->m_bSaveAndValidate );
		m_EnrollmentSumList.eslCurElem->eseEventStateCombo.UpdateData( pDX->m_bSaveAndValidate );
		m_EnrollmentSumList.eslCurElem->esePriority.UpdateData( pDX->m_bSaveAndValidate );
		m_EnrollmentSumList.eslCurElem->eseNotificationClass.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(CSendGetEnrollmentSummaryACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendGetEnrollmentSummaryACK)
	ON_BN_CLICKED(IDC_ADDSUMELEM, OnAddSumElem)
	ON_BN_CLICKED(IDC_REMOVESUMELEM, OnRemoveSumElem)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_ENROLLMENTSUMLIST, OnItemchangingEnrollmentSumListCtrl)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_CBN_SELCHANGE(IDC_EVENTTYPECOMBO, OnSelchangeEventTypeCombo)
	ON_CBN_SELCHANGE(IDC_EVENTSTATECOMBO, OnSelchangeEventStateCombo)
	ON_EN_CHANGE(IDC_PRIORITY, OnChangePriority)
	ON_EN_CHANGE(IDC_NOTIFICATIONCLASS, OnChangeNotificationClass)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendGetEnrollmentSummaryACK::InitPage
//

void CSendGetEnrollmentSummaryACK::InitPage( void )
{
	TRACE0( "CSendGetEnrollmentSummaryACK::InitPage()\n" );
}

//
//	CSendGetEnrollmentSummaryACK::EncodePage
//

void CSendGetEnrollmentSummaryACK::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 4 );

	// encode the contents
	m_EnrollmentSumList.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendGetEnrollmentSummaryACK::SavePage
//

void CSendGetEnrollmentSummaryACK::SavePage( void )
{
	TRACE0( "CSendGetEnrollmentSummaryACK::SavePage\n" );

	pageContents.Flush();

	// ### save the list
}

//
//	CSendGetEnrollmentSummaryACK::RestorePage
//

void CSendGetEnrollmentSummaryACK::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendGetEnrollmentSummaryACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore the list
}

//
//	CSendGetEnrollmentSummaryACK::OnInitDialog
//

BOOL CSendGetEnrollmentSummaryACK::OnInitDialog() 
{
	CComboBox	*cbp
	;
	
	TRACE0( "CSendGetEnrollmentSummaryACK::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_EnrollmentSumListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_EnrollmentSumListCtrl.m_nFlags &= ~LBS_SORT;

	// set up the property list columns
	m_EnrollmentSumListCtrl.InsertColumn( 0, "Object ID", LVCFMT_LEFT, 126);
	m_EnrollmentSumListCtrl.InsertColumn( 1, "Event Type", LVCFMT_LEFT, 82 );
	m_EnrollmentSumListCtrl.InsertColumn( 2, "Event State", LVCFMT_LEFT, 82 );

	// load the enumeration tables
	cbp = (CComboBox *)GetDlgItem( IDC_EVENTTYPECOMBO );
	NetworkSniffer::BAC_STRTAB_BACnetEventType.FillCombo( *cbp );

	cbp = (CComboBox *)GetDlgItem( IDC_EVENTSTATECOMBO );
	NetworkSniffer::BAC_STRTAB_BACnetEventState.FillCombo( *cbp );

	return TRUE;
}

void CSendGetEnrollmentSummaryACK::OnAddSumElem() 
{
	m_EnrollmentSumList.AddButtonClick();
}

void CSendGetEnrollmentSummaryACK::OnRemoveSumElem() 
{
	m_EnrollmentSumList.RemoveButtonClick();
}

void CSendGetEnrollmentSummaryACK::OnItemchangingEnrollmentSumListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_EnrollmentSumList.OnItemChanging( pNMHDR, pResult );
}

void CSendGetEnrollmentSummaryACK::OnChangeObjectID()
{
	m_EnrollmentSumList.OnChangeObjectID();
}

void CSendGetEnrollmentSummaryACK::OnObjectIDButton() 
{
	m_EnrollmentSumList.OnObjectIDButton();
}

void CSendGetEnrollmentSummaryACK::OnSelchangeEventTypeCombo()
{
	m_EnrollmentSumList.OnSelchangeEventTypeCombo();
}

void CSendGetEnrollmentSummaryACK::OnSelchangeEventStateCombo()
{
	m_EnrollmentSumList.OnSelchangeEventStateCombo();
}

void CSendGetEnrollmentSummaryACK::OnChangePriority()
{
	m_EnrollmentSumList.OnChangePriority();
}

void CSendGetEnrollmentSummaryACK::OnChangeNotificationClass()
{
	m_EnrollmentSumList.OnChangeNotificationClass();
}

//
//	EnrollmentSummaryElem::EnrollmentSummaryElem
//

EnrollmentSummaryElem::EnrollmentSummaryElem( CSendPagePtr wp )
	: eseObjectID( wp, IDC_OBJECTID )
	, eseEventTypeCombo( wp, IDC_EVENTTYPECOMBO, NetworkSniffer::BAC_STRTAB_BACnetEventState, true )
	, eseEventStateCombo( wp, IDC_EVENTSTATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetEventState, true )
	, esePriority( wp, IDC_PRIORITY )
	, eseNotificationClass( wp, IDC_NOTIFICATIONCLASS )
{
	// controls start out disabled
	eseObjectID.ctrlEnabled = false;
	eseObjectID.ctrlNull = false;					// madanner 9/5/02, required for default 
	eseObjectID.objID = 0;
	eseEventTypeCombo.ctrlEnabled = false;
	eseEventStateCombo.ctrlEnabled = false;
	esePriority.ctrlEnabled = false;
	esePriority.ctrlNull = false;
	esePriority.uintValue = 1;
	eseNotificationClass.ctrlEnabled = false;
}

//
//	EnrollmentSummaryElem::Bind
//

void EnrollmentSummaryElem::Bind( void )
{
	// set the control value to this element value
	eseObjectID.ObjToCtrl();
	eseObjectID.Enable();
	eseObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( true );
	eseEventTypeCombo.ObjToCtrl();
	eseEventTypeCombo.Enable();
	eseEventStateCombo.ObjToCtrl();
	eseEventStateCombo.Enable();
	esePriority.ObjToCtrl();
	esePriority.Enable();
	eseNotificationClass.ObjToCtrl();
	eseNotificationClass.Enable();
}

//
//	EnrollmentSummaryElem::Unbind
//

void EnrollmentSummaryElem::Unbind( void )
{
	// clear out the contents of the controls
	eseObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTID )->SetWindowText( "" );
	eseObjectID.Disable();
	eseObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );
	eseEventTypeCombo.ctrlWindow->GetDlgItem( IDC_EVENTTYPECOMBO )->SetWindowText( "" );
	eseEventTypeCombo.Disable();
	eseEventStateCombo.ctrlWindow->GetDlgItem( IDC_EVENTSTATECOMBO )->SetWindowText( "" );
	eseEventStateCombo.Disable();
	esePriority.ctrlWindow->GetDlgItem( IDC_PRIORITY )->SetWindowText( "" );
	esePriority.Disable();
	eseNotificationClass.ctrlWindow->GetDlgItem( IDC_NOTIFICATIONCLASS )->SetWindowText( "" );
	eseNotificationClass.Disable();
}

//
//	EnrollmentSummaryElem::Encode
//

void EnrollmentSummaryElem::Encode( BACnetAPDUEncoder& enc )
{
	// encode the object ID
	if (eseObjectID.ctrlNull)
		throw "Object ID required";
	eseObjectID.Encode( enc );

	// encode the event type
	if (eseEventTypeCombo.ctrlNull)
		throw "Event type required";
	eseEventTypeCombo.Encode( enc );

	// encode the event state
	if (eseEventStateCombo.ctrlNull)
		throw "Event state required";
	eseEventStateCombo.Encode( enc );

	// encode the priority
	if (esePriority.ctrlNull)
		throw "Priority required";
	if ((esePriority.uintValue < 0) || (esePriority.uintValue > 255))
		throw "Priority out of range (0..255)";
	esePriority.Encode( enc );

	// encode the optional notification class
	if (!eseNotificationClass.ctrlNull)
		eseNotificationClass.Encode( enc );
}

//
//	EnrollmentSummaryList::EnrollmentSummaryList
//

EnrollmentSummaryList::EnrollmentSummaryList( CSendGetEnrollmentSummaryACKPtr pp )
	: eslPagePtr( pp )
	, eslCurElem(0), eslCurElemIndx(0)
{
}

//
//	EnrollmentSummaryList::~EnrollmentSummaryList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

EnrollmentSummaryList::~EnrollmentSummaryList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	EnrollmentSummaryList::AddButtonClick
//

void EnrollmentSummaryList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = eslPagePtr->m_EnrollmentSumListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = eslPagePtr->m_EnrollmentSumListCtrl.GetNextSelectedItem( selPos );
		eslPagePtr->m_EnrollmentSumListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	eslAddInProgress = true;
	eslPagePtr->m_EnrollmentSumListCtrl.InsertItem( listLen, "!" );

	// create a new item, add to the end of the list
	eslCurElem = new EnrollmentSummaryElem( eslPagePtr );
	eslCurElemIndx = listLen;
	AddTail( eslCurElem );

	// bind the element to the controls
	eslCurElem->Bind();

	// update the encoding
	eslAddInProgress = false;
	eslPagePtr->UpdateEncoded();

	OnSelchangeEventStateCombo();		// madanner 9/4/02, throws state into list as well
	OnSelchangeEventTypeCombo();
	eslPagePtr->m_EnrollmentSumListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
}

//
//	EnrollmentSummaryList::RemoveButtonClick
//

void EnrollmentSummaryList::RemoveButtonClick( void )
{
	int					curRow = eslCurElemIndx
	;
	EnrollmentSummaryElemPtr	curElem = eslCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	eslPagePtr->m_EnrollmentSumListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	eslPagePtr->m_EnrollmentSumListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	eslPagePtr->m_EnrollmentSumListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	eslPagePtr->UpdateEncoded();
}

//
//	EnrollmentSummaryList::OnChangeObjectID
//

void EnrollmentSummaryList::OnChangeObjectID( void )
{
	if (eslCurElem) {
		CString		someText
		;

		eslCurElem->eseObjectID.UpdateData();
		eslPagePtr->UpdateEncoded();

		if (eslCurElem->eseObjectID.ctrlNull)
			eslPagePtr->m_EnrollmentSumListCtrl.SetItemText( eslCurElemIndx, 0, "Error" );
		else {
			int		objType = (eslCurElem->eseObjectID.objID >> 22)
			,		instanceNum = (eslCurElem->eseObjectID.objID & 0x003FFFFF)
			;
			char	buff[100];
			sprintf( buff, "%s, %d", NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString(objType), instanceNum );
			eslPagePtr->m_EnrollmentSumListCtrl.SetItemText( eslCurElemIndx, 0, buff );
		}
	}
}

void EnrollmentSummaryList::OnObjectIDButton() 
{
	if (eslCurElem) {
		VTSObjectIdentifierDialog	dlg(eslPagePtr)			// for proper parent control
		;

		dlg.objID = eslCurElem->eseObjectID.objID;
		if (dlg.DoModal() && dlg.validObjID) {
			eslCurElem->eseObjectID.ctrlNull = false;
			eslCurElem->eseObjectID.objID = dlg.objID;
			eslCurElem->eseObjectID.ObjToCtrl();

			// copy the text from the control to the list
			CString text;
			eslCurElem->eseObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTID )->GetWindowText( text );
			eslPagePtr->m_EnrollmentSumListCtrl.SetItemText( eslCurElemIndx, 0, text );

			eslPagePtr->UpdateEncoded();
		}
	}
}

//
//	EnrollmentSummaryList::OnSelchangeEventTypeCombo
//

void EnrollmentSummaryList::OnSelchangeEventTypeCombo( void )
{
	if (eslCurElem) {
		eslCurElem->eseEventTypeCombo.UpdateData();
		eslPagePtr->UpdateEncoded();

		if (eslCurElem->eseEventTypeCombo.ctrlNull)
			eslPagePtr->m_EnrollmentSumListCtrl.SetItemText( eslCurElemIndx, 1, "" );
		else
			eslPagePtr->m_EnrollmentSumListCtrl.SetItemText( eslCurElemIndx, 1
				, NetworkSniffer::BAC_STRTAB_BACnetEventType.m_pStrings[ eslCurElem->eseEventTypeCombo.m_enumValue ]
				);
	}
}

//
//	EnrollmentSummaryList::OnSelchangeEventStateCombo
//

void EnrollmentSummaryList::OnSelchangeEventStateCombo( void )
{
	if (eslCurElem) {
		eslCurElem->eseEventStateCombo.UpdateData();
		eslPagePtr->UpdateEncoded();

		if (eslCurElem->eseEventStateCombo.ctrlNull)
			eslPagePtr->m_EnrollmentSumListCtrl.SetItemText( eslCurElemIndx, 2, "" );
		else
			eslPagePtr->m_EnrollmentSumListCtrl.SetItemText( eslCurElemIndx, 2
				, NetworkSniffer::BAC_STRTAB_BACnetEventState.m_pStrings[ eslCurElem->eseEventStateCombo.m_enumValue ]
				);
	}
}

//
//	EnrollmentSummaryList::OnChangePriority
//

void EnrollmentSummaryList::OnChangePriority( void )
{
	if (eslCurElem) {
		eslCurElem->esePriority.UpdateData();
		eslPagePtr->UpdateEncoded();
	}
}

//
//	EnrollmentSummaryList::OnChangeNotificationClass
//

void EnrollmentSummaryList::OnChangeNotificationClass( void )
{
	if (eslCurElem) {
		eslCurElem->eseNotificationClass.UpdateData();
		eslPagePtr->UpdateEncoded();
	}
}

//
//	EnrollmentSummaryList::OnItemChanging
//

void EnrollmentSummaryList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = eslCurElemIndx
	;
	EnrollmentSummaryElemPtr	curElem = eslCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	// madanner 9/5/02, proper masking check?
	if ((pNMListView->uOldState && LVIS_SELECTED) == (pNMListView->uNewState && LVIS_SELECTED))
		return;

	// skip messages during new item creation
	if (eslAddInProgress)
		return;

	if ((pNMListView->uNewState && LVIS_SELECTED) != 0) {		// madanner 9/5/02, proper mask ??
		// item becoming selected
		eslCurElemIndx = pNMListView->iItem;
		eslCurElem = GetAt( FindIndex( eslCurElemIndx ) );

		// bind the new current element
		eslCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == eslCurElemIndx) {
			// set nothing selected
			eslCurElem = 0;
			eslCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	EnrollmentSummaryList::Encode
//

void EnrollmentSummaryList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound elements
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}
