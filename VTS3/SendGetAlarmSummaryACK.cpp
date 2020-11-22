// SendGetAlarmSummaryACK.cpp : implementation file
//


#include "stdafx.h"
#include "VTS.h"
#include "Send.h"
#include "SendGetAlarmSummaryACK.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void EncoderToHex( const BACnetAPDUEncoder &enc, CString &str );

BACnetAPDUEncoder CSendGetAlarmSummaryACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendGetAlarmSummaryACK dialog

IMPLEMENT_DYNCREATE( CSendGetAlarmSummaryACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendGetAlarmSummaryACK::CSendGetAlarmSummaryACK( void )
	: CSendPage( CSendGetAlarmSummaryACK::IDD )
	, m_AlarmSumList( this )
{
	//{{AFX_DATA_INIT(CSendGetAlarmSummaryACK)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendGetAlarmSummaryACK::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CSendGetAlarmSummaryACK)
	DDX_Control(pDX, IDC_ALARMSUMLIST, m_AlarmSumListCtrl);
	//}}AFX_DATA_MAP

	// if there is a selected property, allow the controls to update
	if (m_AlarmSumList.aslCurElem) {
		m_AlarmSumList.aslCurElem->aseObjectID.UpdateData( pDX->m_bSaveAndValidate );
		m_AlarmSumList.aslCurElem->aseEventStateCombo.UpdateData( pDX->m_bSaveAndValidate );
		m_AlarmSumList.aslCurElem->aseToOffnormal.UpdateData( pDX->m_bSaveAndValidate );
		m_AlarmSumList.aslCurElem->aseToFault.UpdateData( pDX->m_bSaveAndValidate );
		m_AlarmSumList.aslCurElem->aseToNormal.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(CSendGetAlarmSummaryACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendGetAlarmSummaryACK)
	ON_BN_CLICKED(IDC_ADDSUMELEM, OnAddSumElem)
	ON_BN_CLICKED(IDC_REMOVESUMELEM, OnRemoveSumElem)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_ALARMSUMLIST, OnItemchangingAlarmSumListCtrl)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_CBN_SELCHANGE(IDC_EVENTSTATECOMBO, OnSelchangeEventStateCombo)
	ON_BN_CLICKED(IDC_TOOFFNORMAL, OnToOffnormal)
	ON_BN_CLICKED(IDC_TOFAULT, OnToFault)
	ON_BN_CLICKED(IDC_TONORMAL, OnToNormal)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendGetAlarmSummaryACK::InitPage
//

void CSendGetAlarmSummaryACK::InitPage( void )
{
	TRACE0( "CSendGetAlarmSummaryACK::InitPage()\n" );
}

//
//	CSendGetAlarmSummaryACK::EncodePage
//

void CSendGetAlarmSummaryACK::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 3 );

	// encode the contents
	m_AlarmSumList.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendGetAlarmSummaryACK::SavePage
//

void CSendGetAlarmSummaryACK::SavePage( void )
{
	TRACE0( "CSendGetAlarmSummaryACK::SavePage\n" );

	pageContents.Flush();

	// ### save the list contents
}

//
//	CSendGetAlarmSummaryACK::RestorePage
//

void CSendGetAlarmSummaryACK::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendGetAlarmSummaryACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore the list contents
}

//
//	CSendGetAlarmSummaryACK::OnInitDialog
//

BOOL CSendGetAlarmSummaryACK::OnInitDialog() 
{
	TRACE0( "CSendGetAlarmSummaryACK::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_AlarmSumListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_AlarmSumListCtrl.m_nFlags &= ~LBS_SORT;

	// set up the property list columns
	m_AlarmSumListCtrl.InsertColumn( 0, "Object ID", LVCFMT_LEFT, 126 );
	m_AlarmSumListCtrl.InsertColumn( 1, "Event State", LVCFMT_LEFT, 82 );
	m_AlarmSumListCtrl.InsertColumn( 2, "Acked Transitions", LVCFMT_LEFT, 116 );

	// load the enumeration table
	CComboBox	*cbp = (CComboBox *)GetDlgItem( IDC_EVENTSTATECOMBO );
	NetworkSniffer::BAC_STRTAB_BACnetEventState.FillCombo( *cbp );

	return TRUE;
}

void CSendGetAlarmSummaryACK::OnAddSumElem() 
{
	m_AlarmSumList.AddButtonClick();
}

void CSendGetAlarmSummaryACK::OnRemoveSumElem() 
{
	m_AlarmSumList.RemoveButtonClick();
}

void CSendGetAlarmSummaryACK::OnItemchangingAlarmSumListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_AlarmSumList.OnItemChanging( pNMHDR, pResult );
}

void CSendGetAlarmSummaryACK::OnChangeObjectID()
{
	m_AlarmSumList.OnChangeObjectID();
}

void CSendGetAlarmSummaryACK::OnObjectIDButton()
{
	m_AlarmSumList.OnObjectIDButton();
}

void CSendGetAlarmSummaryACK::OnSelchangeEventStateCombo()
{
	m_AlarmSumList.OnSelchangeEventStateCombo();
}

void CSendGetAlarmSummaryACK::OnToOffnormal() 
{
	m_AlarmSumList.OnToOffnormal();
}

void CSendGetAlarmSummaryACK::OnToFault() 
{
	m_AlarmSumList.OnToFault();
}

void CSendGetAlarmSummaryACK::OnToNormal() 
{
	m_AlarmSumList.OnToNormal();
}

//
//	AlarmSummaryElem::AlarmSummaryElem
//

AlarmSummaryElem::AlarmSummaryElem( CSendPagePtr wp )
	: aseObjectID( wp, IDC_OBJECTID )
	, aseEventStateCombo( wp, IDC_EVENTSTATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetEventState, true )
	, aseToOffnormal( wp, IDC_TOOFFNORMAL, true )
	, aseToFault( wp, IDC_TOFAULT, true )
	, aseToNormal( wp, IDC_TONORMAL, true )
{
	// controls start out disabled
	aseObjectID.ctrlEnabled = false;
	aseObjectID.ctrlNull = false;
	aseObjectID.objID = 0;
	aseEventStateCombo.ctrlEnabled = false;
	aseToOffnormal.ctrlEnabled = false;
	aseToFault.ctrlEnabled = false;
	aseToNormal.ctrlEnabled = false;
}

//
//	AlarmSummaryElem::Bind
//

void AlarmSummaryElem::Bind( void )
{
	// set the control value to this element value
	aseObjectID.ObjToCtrl();
	aseObjectID.Enable();
	aseObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( true );
	aseEventStateCombo.ObjToCtrl();
	aseEventStateCombo.Enable();
	aseToOffnormal.ObjToCtrl();
	aseToOffnormal.Enable();
	aseToFault.ObjToCtrl();
	aseToFault.Enable();
	aseToNormal.ObjToCtrl();
	aseToNormal.Enable();
}

//
//	AlarmSummaryElem::Unbind
//

void AlarmSummaryElem::Unbind( void )
{
	// clear out the contents of the controls
	aseObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTID )->SetWindowText( "" );
	aseObjectID.Disable();
	aseObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );
	aseEventStateCombo.ctrlWindow->GetDlgItem( IDC_EVENTSTATECOMBO )->SetWindowText( "" );
	aseEventStateCombo.Disable();
	aseToOffnormal.SetCheck(false);
	aseToOffnormal.Disable();
	aseToFault.SetCheck(false);
	aseToFault.Disable();
	aseToNormal.SetCheck(false);
	aseToNormal.Disable();
}

//
//	AlarmSummaryElem::Encode
//

void AlarmSummaryElem::Encode( BACnetAPDUEncoder& enc )
{
	BACnetBitString		bits( 3 )
	;

	// encode the object ID
	if (aseObjectID.ctrlNull)
		throw "Object ID required";
	aseObjectID.Encode( enc );

	// encode the event state
	if (aseEventStateCombo.ctrlNull)
		throw "Event state required";
	aseEventStateCombo.Encode( enc );

	// set up the event transition bit string
	bits.SetBit( 0, aseToOffnormal.boolValue );
	bits.SetBit( 1, aseToFault.boolValue );
	bits.SetBit( 2, aseToNormal.boolValue );

	// now encode it
	bits.Encode( enc );
}

//
//	AlarmSummaryList::AlarmSummaryList
//

AlarmSummaryList::AlarmSummaryList( CSendGetAlarmSummaryACKPtr pp )
	: aslPagePtr( pp )
	, aslCurElem(0), aslCurElemIndx(0)
{
}

//
//	AlarmSummaryList::~AlarmSummaryList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

AlarmSummaryList::~AlarmSummaryList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	AlarmSummaryList::AddButtonClick
//

void AlarmSummaryList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = aslPagePtr->m_AlarmSumListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = aslPagePtr->m_AlarmSumListCtrl.GetNextSelectedItem( selPos );
		aslPagePtr->m_AlarmSumListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	aslAddInProgress = true;
	aslPagePtr->m_AlarmSumListCtrl.InsertItem( listLen, "!" );

	// create a new item, add to the end of the list
	aslCurElem = new AlarmSummaryElem( aslPagePtr );
	aslCurElemIndx = listLen;
	AddTail( aslCurElem );

	// bind the element to the controls
	aslCurElem->Bind();

	// update the encoding
	aslAddInProgress = false;
	aslPagePtr->UpdateEncoded();

	OnSelchangeEventStateCombo();		// madanner 9/4/02, throws state into list as well
	aslPagePtr->m_AlarmSumListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);

}

//
//	AlarmSummaryList::RemoveButtonClick
//

void AlarmSummaryList::RemoveButtonClick( void )
{
	int					curRow = aslCurElemIndx
	;
	AlarmSummaryElemPtr	curElem = aslCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	aslPagePtr->m_AlarmSumListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	aslPagePtr->m_AlarmSumListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}
	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	aslPagePtr->m_AlarmSumListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	aslPagePtr->UpdateEncoded();
}

//
//	AlarmSummaryList::OnChangeObjectID
//

void AlarmSummaryList::OnChangeObjectID( void )
{
	if (aslCurElem) {
		CString		someText
		;

		aslCurElem->aseObjectID.UpdateData();
		aslPagePtr->UpdateEncoded();

		if (aslCurElem->aseObjectID.ctrlNull)
			aslPagePtr->m_AlarmSumListCtrl.SetItemText( aslCurElemIndx, 0, "Error" );
		else {
			int		objType = (aslCurElem->aseObjectID.objID >> 22)
			,		instanceNum = (aslCurElem->aseObjectID.objID & 0x003FFFFF)
			;
			char	buff[100];
			sprintf( buff, "%s, %d", NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString(objType), instanceNum );
			aslPagePtr->m_AlarmSumListCtrl.SetItemText( aslCurElemIndx, 0, buff );
		}
	}
}

void AlarmSummaryList::OnObjectIDButton() 
{
	if (aslCurElem) {
		VTSObjectIdentifierDialog	dlg
		;

		dlg.objID = aslCurElem->aseObjectID.objID;
		if (dlg.DoModal() && dlg.validObjID) {
			aslCurElem->aseObjectID.ctrlNull = false;
			aslCurElem->aseObjectID.objID = dlg.objID;
			aslCurElem->aseObjectID.ObjToCtrl();

			// copy the text from the control to the list
			CString text;
			aslCurElem->aseObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTID )->GetWindowText( text );
			aslPagePtr->m_AlarmSumListCtrl.SetItemText( aslCurElemIndx, 0, text );

			aslPagePtr->UpdateEncoded();
		}
	}
}

//
//	AlarmSummaryList::OnSelchangeEventStateCombo
//

void AlarmSummaryList::OnSelchangeEventStateCombo( void )
{
	if (aslCurElem) {
		aslCurElem->aseEventStateCombo.UpdateData();
		aslPagePtr->UpdateEncoded();

		if (aslCurElem->aseObjectID.ctrlNull)
			aslPagePtr->m_AlarmSumListCtrl.SetItemText( aslCurElemIndx, 1, "" );
		else
			aslPagePtr->m_AlarmSumListCtrl.SetItemText( aslCurElemIndx, 1
				, NetworkSniffer::BAC_STRTAB_BACnetEventState.m_pStrings[ aslCurElem->aseEventStateCombo.m_enumValue ]
				);
	}
}

//
//	AlarmSummaryList::OnToOffnormal
//

void AlarmSummaryList::OnToOffnormal( void )
{
	if (aslCurElem) {
		aslCurElem->aseToOffnormal.UpdateData();
		aslPagePtr->UpdateEncoded();

		UpdateBitsDisplay();
	}
}

//
//	AlarmSummaryList::OnToFault
//

void AlarmSummaryList::OnToFault( void )
{
	if (aslCurElem) {
		aslCurElem->aseToFault.UpdateData();
		aslPagePtr->UpdateEncoded();

		UpdateBitsDisplay();
	}
}

//
//	AlarmSummaryList::OnToNormal
//

void AlarmSummaryList::OnToNormal( void )
{
	if (aslCurElem) {
		aslCurElem->aseToNormal.UpdateData();
		aslPagePtr->UpdateEncoded();

		UpdateBitsDisplay();
	}
}

//
//
//

void AlarmSummaryList::UpdateBitsDisplay( void )
{
	bool	first = true
	;
	CString	rslt
	;

	if (aslCurElem->aseToOffnormal.boolValue == BACnetBoolean::bTrue) {
		rslt += "to-offnormal";
		first = false;
	}
	if (aslCurElem->aseToFault.boolValue == BACnetBoolean::bTrue) {
		if (!first) rslt += ", ";
		rslt += "to-fault";
		first = false;
	}
	if (aslCurElem->aseToNormal.boolValue == BACnetBoolean::bTrue) {
		if (!first) rslt += ", ";
		rslt += "to-normal";
	}

	// now set the display
	aslPagePtr->m_AlarmSumListCtrl.SetItemText( aslCurElemIndx, 2, rslt );
}

//
//	AlarmSummaryList::OnItemChanging
//

void AlarmSummaryList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = aslCurElemIndx
	;
	AlarmSummaryElemPtr	curElem = aslCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	// madanner 9/5/02, proper masking check?
	if ((pNMListView->uOldState && LVIS_SELECTED)  == (pNMListView->uNewState && LVIS_SELECTED) )
		return;

	// skip messages during new item creation
	if (aslAddInProgress)
		return;

	if ((pNMListView->uNewState && LVIS_SELECTED) != 0) {		// madanner 9/5/02, proper mask ??
		// item becoming selected
		aslCurElemIndx = pNMListView->iItem;
		aslCurElem = GetAt( FindIndex( aslCurElemIndx ) );

		// bind the new current element
		aslCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == aslCurElemIndx) {
			// set nothing selected
			aslCurElem = 0;
			aslCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	AlarmSummaryList::Encode
//

void AlarmSummaryList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound elements
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}
