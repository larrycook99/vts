// SendGetEventInfoACK.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "SendGetEventInfoACK.h"
#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder	CSendGetEventInfoACK::pageContents;
IMPLEMENT_DYNCREATE( CSendGetEventInfoACK, CPropertyPage )
/////////////////////////////////////////////////////////////////////////////
// CSendGetEventInfoACK dialog

#pragma warning( disable : 4355 )
CSendGetEventInfoACK::CSendGetEventInfoACK(void)
	: CSendPage( CSendGetEventInfoACK::IDD )
	, m_EventSumList( this )
	, m_MoreEvent(this, IDC_MOREEVENT)
{
	//{{AFX_DATA_INIT(CSendGetEventInfoACK)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSendGetEventInfoACK::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendGetEventInfoACK)
	DDX_Control(pDX, IDC_EVENTINFOLIST, m_EventSumListCtrl);
	//}}AFX_DATA_MAP
	// if there is a selected property, allow the controls to update
	if (m_EventSumList.eslCurElem) {
		m_EventSumList.eslCurElem->m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
		m_EventSumList.eslCurElem->eseEventStateCombo.UpdateData( pDX->m_bSaveAndValidate );
		m_EventSumList.eslCurElem->eseNotifyTypeCombo.UpdateData( pDX->m_bSaveAndValidate );
		m_EventSumList.eslCurElem->esackOffnormal.UpdateData( pDX->m_bSaveAndValidate );
		m_EventSumList.eslCurElem->esackFault.UpdateData( pDX->m_bSaveAndValidate );
		m_EventSumList.eslCurElem->esackNormal.UpdateData( pDX->m_bSaveAndValidate );
		m_EventSumList.eslCurElem->esenaOffnormal.UpdateData( pDX->m_bSaveAndValidate );
		m_EventSumList.eslCurElem->esenaFault.UpdateData( pDX->m_bSaveAndValidate );
		m_EventSumList.eslCurElem->esenaNormal.UpdateData( pDX->m_bSaveAndValidate );
	}
	m_MoreEvent.UpdateData(pDX->m_bSaveAndValidate);
}
BOOL CSendGetEventInfoACK::OnInitDialog() 
{
	CComboBox	*cbp
	;
	
	TRACE0( "CSendGetEventInfoACK::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_EventSumListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_EventSumListCtrl.m_nFlags &= ~LBS_SORT;

	// set up the property list columns
	m_EventSumListCtrl.InsertColumn( 0, "Object ID", LVCFMT_LEFT, 126);
	m_EventSumListCtrl.InsertColumn( 1, "Event State", LVCFMT_LEFT, 82 );
	m_EventSumListCtrl.InsertColumn( 2, "Notify Type", LVCFMT_LEFT, 82 );
	m_EventSumListCtrl.InsertColumn( 3, "Ack Transitions", LVCFMT_LEFT, 82 );
	m_EventSumListCtrl.InsertColumn( 4, "Event Enable", LVCFMT_LEFT, 82 );
	// load the enumeration tables
	cbp = (CComboBox *)GetDlgItem( IDC_EVENTSTATECOMBO );
	NetworkSniffer::BAC_STRTAB_BACnetEventState.FillCombo( *cbp );

	cbp = (CComboBox *)GetDlgItem( IDC_NOTIFYTYPECOMBO );
	NetworkSniffer::BAC_STRTAB_BACnetNotifyType.FillCombo( *cbp );

	return TRUE;
}

void CSendGetEventInfoACK::InitPage( void )
{
	TRACE0( "CSendGetEventInfoACK::InitPage()\n" );
}					
void CSendGetEventInfoACK::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 29 );
	
	BACnetOpeningTag().Encode(enc, 0);
	m_EventSumList.Encode( enc);
	BACnetClosingTag().Encode(enc, 0);
	m_MoreEvent.Encode(enc, 1);
	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}
void CSendGetEventInfoACK::SavePage( void )
{
	TRACE0( "CSendGetEventInfoACK::SavePage()\n" );
}					
void CSendGetEventInfoACK::RestorePage( int index)
{
	TRACE0( "CSendGetEventInfoACK::RestorePage()\n" );
}	
BEGIN_MESSAGE_MAP(CSendGetEventInfoACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendGetEventInfoACK)
	ON_BN_CLICKED(IDC_NORMAL, OnNormal)
	ON_BN_CLICKED(IDC_FAULT1, OnFault1)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_CBN_SELCHANGE(IDC_EVENTSTATECOMBO, OnSelChangeEventStatecombo)
	ON_CBN_SELCHANGE(IDC_NOTIFYTYPECOMBO, OnSelChangeNotifyTypecombo)
	ON_BN_CLICKED(IDC_ADDSUMELEM, OnAddsumelem)
	ON_BN_CLICKED(IDC_REMOVESUMELEM, OnRemovesumelem)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_EVENTINFOLIST, OnItemchangingEventinfolist)
	ON_BN_CLICKED(IDC_SETPRIORITIES, OnSetpriorities)
	ON_BN_CLICKED(IDC_SETTIMESTAMP, OnSettimestamp)
	ON_BN_CLICKED(IDC_OFFNORMAL, OnOffnormal)
	ON_BN_CLICKED(IDC_OFFNORMAL1, OnOffnormal1)
	ON_BN_CLICKED(IDC_NORMAL1, OnNormal1)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_BN_CLICKED(IDC_MOREEVENT, OnMoreevent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendGetEventInfoACK message handlers

void CSendGetEventInfoACK::OnNormal() 
{
	m_EventSumList.OnNormal();	
}

void CSendGetEventInfoACK::OnFault1() 
{
	m_EventSumList.OnFault1();		
}

void CSendGetEventInfoACK::OnFault() 
{
	m_EventSumList.OnFault();
}

void CSendGetEventInfoACK::OnSelChangeEventStatecombo() 
{
	m_EventSumList.OnSelchangeEventStateCombo();
}

void CSendGetEventInfoACK::OnSelChangeNotifyTypecombo() 
{
	m_EventSumList.OnSelchangeNotifyTypeCombo();
}

void CSendGetEventInfoACK::OnAddsumelem() 
{
	m_EventSumList.AddButtonClick();	
}

void CSendGetEventInfoACK::OnRemovesumelem() 
{
	m_EventSumList.RemoveButtonClick();	
}

void CSendGetEventInfoACK::OnItemchangingEventinfolist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_EventSumList.OnItemChanging( pNMHDR, pResult );
}

void CSendGetEventInfoACK::OnSetpriorities() 
{
	m_EventSumList.OnSetpriorities();
}

void CSendGetEventInfoACK::OnSettimestamp() 
{
	m_EventSumList.OnSettimestamp();
}

void CSendGetEventInfoACK::OnOffnormal() 
{
	m_EventSumList.OnOffnormal();
}

void CSendGetEventInfoACK::OnOffnormal1() 
{
	m_EventSumList.OnOffnormal1();
}

void CSendGetEventInfoACK::OnNormal1() 
{
	m_EventSumList.OnNormal1();
}
void CSendGetEventInfoACK::OnChangeObjectid() 
{
	m_EventSumList.OnChangeObjectID();
}

void CSendGetEventInfoACK::OnObjectidbtn() 
{
	m_EventSumList.OnObjectIDButton();
}
void CSendGetEventInfoACK::OnMoreevent() 
{
	m_MoreEvent.UpdateData();
	SavePage();
	UpdateEncoded();
}

//
//	EventSummaryElem::EventSummaryElem
//

EventSummaryElem::EventSummaryElem( CSendPagePtr wp )
	: m_ObjectID( wp, IDC_OBJECTID )
	, eseEventStateCombo( wp, IDC_EVENTSTATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetEventState, true )
	, eseNotifyTypeCombo( wp, IDC_NOTIFYTYPECOMBO, NetworkSniffer::BAC_STRTAB_BACnetNotifyType, true )
	, esackOffnormal( wp, IDC_OFFNORMAL, true )
	, esackFault( wp, IDC_FAULT, true )
	, esackNormal( wp, IDC_NORMAL, true )
	, esenaOffnormal( wp, IDC_OFFNORMAL1, true )
	, esenaFault( wp, IDC_FAULT1, true )
	, esenaNormal( wp, IDC_NORMAL1, true )
{
	// controls start out disabled
	m_ObjectID.ctrlEnabled = false;
	m_ObjectID.ctrlNull = false;
	m_ObjectID.objID = 0;
	eseEventStateCombo.ctrlEnabled = false;
	eseNotifyTypeCombo.ctrlEnabled = false;
	esackOffnormal.ctrlEnabled = false;
	esackFault.ctrlEnabled = false;
	esackNormal.ctrlEnabled = false;
	esenaOffnormal.ctrlEnabled = false;
	esenaFault.ctrlEnabled = false;
	esenaNormal.ctrlEnabled = false;
}

//
//	EventSummaryElem::Bind
//

void EventSummaryElem::Bind( void )
{
	// set the control value to this element value
	m_ObjectID.ObjToCtrl();
	m_ObjectID.Enable();
	m_ObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( true );
	eseEventStateCombo.ObjToCtrl();
	eseEventStateCombo.Enable();
	eseNotifyTypeCombo.ObjToCtrl();
	eseNotifyTypeCombo.Enable();
	esackOffnormal.ObjToCtrl();
	esackOffnormal.Enable();
	esackFault.ObjToCtrl();
	esackFault.Enable();
	esackNormal.ObjToCtrl();
	esackNormal.Enable();
	esenaOffnormal.ObjToCtrl();
	esenaOffnormal.Enable();
	esenaFault.ObjToCtrl();
	esenaFault.Enable();
	esenaNormal.ObjToCtrl();
	esenaNormal.Enable();	
	m_ObjectID.ctrlWindow->GetDlgItem( IDC_SETTIMESTAMP )->EnableWindow( true );
	m_ObjectID.ctrlWindow->GetDlgItem( IDC_SETPRIORITIES )->EnableWindow( true );
}

//
//	EventSummaryElem::Unbind
//

void EventSummaryElem::Unbind( void )
{
	// clear out the contents of the controls
	m_ObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTID )->SetWindowText( "" );
	m_ObjectID.Disable();
	m_ObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );
	eseEventStateCombo.ctrlWindow->GetDlgItem( IDC_EVENTSTATECOMBO )->SetWindowText( "" );
	eseEventStateCombo.Disable();
	eseNotifyTypeCombo.ctrlWindow->GetDlgItem( IDC_EVENTSTATECOMBO )->SetWindowText( "" );
	eseNotifyTypeCombo.Disable();
	esackOffnormal.SetCheck(false);
	esackOffnormal.Disable();
	esackFault.SetCheck(false);
	esackFault.Disable();
	esackNormal.SetCheck(false);
	esackNormal.Disable();
	esenaOffnormal.SetCheck(false);
	esenaOffnormal.Disable();
	esenaFault.SetCheck(false);
	esenaFault.Disable();
	esenaNormal.SetCheck(false);
	esenaNormal.Disable();
}

/*
void EventSummaryElem::ReleaseList(void)
{
	for (POSITION pos = listofTimestamp.m_TimeStampList.GetHeadPosition(); pos != NULL; )
	delete  listofTimestamp.m_TimeStampList.GetNext( pos );
	for (pos = m_eventPriorities.GetHeadPosition(); pos != NULL; )
	delete  m_eventPriorities.GetNext( pos );
}*/

//
//	EventSummaryElem::Encode
//
void EventSummaryElem::Encode( BACnetAPDUEncoder& enc )
{
	BACnetBitString		bits( 3 )
	;

	// encode the object ID
	if (m_ObjectID.ctrlNull)
		throw "Object ID required";
	m_ObjectID.Encode( enc,0 );

	// encode the event state
	if (eseEventStateCombo.ctrlNull)
		throw "Event state required";
	eseEventStateCombo.Encode( enc, 1);

	// set up the event transition bit string
	bits.SetBit( 0, esackOffnormal.boolValue );
	bits.SetBit( 1, esackFault.boolValue );
	bits.SetBit( 2, esackNormal.boolValue );
	// now encode it
	bits.Encode( enc, 2);
	BACnetOpeningTag().Encode( enc, 3);
	listofTimestamp.Encode( enc );
	BACnetClosingTag().Encode( enc, 3);
	
	if (eseEventStateCombo.ctrlNull)
		throw "Event state required";
	eseEventStateCombo.Encode( enc, 4);	

	bits.SetBit( 0, esenaOffnormal.boolValue );
	bits.SetBit( 1, esenaFault.boolValue );
	bits.SetBit( 2, esenaNormal.boolValue );
	// now encode it
	bits.Encode( enc, 5);

	BACnetOpeningTag().Encode( enc, 6);
	m_eventPriorities.Encode( enc );
	BACnetClosingTag().Encode( enc, 6);
	
}

//////////////////////////////////////////////////////////////////////////
EventSummaryList::EventSummaryList(CSendGetEventInfoACKPtr pp)
	: eslPagePtr( pp )
	, eslCurElem(0), eslCurElemIndx(0)
{

}
EventSummaryList::~EventSummaryList(void)
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos);
/*
		{
			EventSummaryElemPtr item =  GetNext( pos );
			item->ReleaseList();
			delete item;
		}*/
	
}

void EventSummaryList::AddButtonClick( void )
{

	int		listLen = GetCount();

	// deselect if something was selected
	POSITION selPos = eslPagePtr->m_EventSumListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = eslPagePtr->m_EventSumListCtrl.GetNextSelectedItem( selPos );
		eslPagePtr->m_EventSumListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	eslAddInProgress = true;
	eslPagePtr->m_EventSumListCtrl.InsertItem( listLen, "!" );

	// create a new item, add to the end of the list
	eslCurElem = new EventSummaryElem( eslPagePtr );
	eslCurElemIndx = listLen;
	AddTail( eslCurElem );

	// bind the element to the controls
	eslCurElem->Bind();

	// update the encoding
	eslAddInProgress = false;
	eslPagePtr->UpdateEncoded();

	OnSelchangeEventStateCombo();		
	OnSelchangeNotifyTypeCombo();
	eslPagePtr->m_EventSumListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
}
void EventSummaryList::RemoveButtonClick( void )
{
	
	int					curRow = eslCurElemIndx
	;
	EventSummaryElemPtr	curElem = eslCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	eslPagePtr->m_EventSumListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	eslPagePtr->m_EventSumListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );	
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	eslPagePtr->m_EventSumListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	eslPagePtr->UpdateEncoded();
}
//
//	EventSummaryList::OnChangeObjectID
//

void EventSummaryList::OnChangeObjectID( void )
{
	if (eslCurElem) {
		CString		someText
		;

		eslCurElem->m_ObjectID.UpdateData();
		eslPagePtr->UpdateEncoded();

		if (eslCurElem->m_ObjectID.ctrlNull)
			eslPagePtr->m_EventSumListCtrl.SetItemText( eslCurElemIndx, 0, "Error" );
		else {
			int		objType = (eslCurElem->m_ObjectID.objID >> 22)
			,		instanceNum = (eslCurElem->m_ObjectID.objID & 0x003FFFFF)
			;
			char	buff[100];
			sprintf( buff, "%s, %d", NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString(objType), instanceNum );
			eslPagePtr->m_EventSumListCtrl.SetItemText( eslCurElemIndx, 0, buff );
		}
	}
}
void EventSummaryList::OnObjectIDButton() 
{
	if (eslCurElem) {
		VTSObjectIdentifierDialog	dlg(eslPagePtr)			// for proper parent control
		;

		dlg.objID = eslCurElem->m_ObjectID.objID;
		if (dlg.DoModal() && dlg.validObjID) {
			eslCurElem->m_ObjectID.ctrlNull = false;
			eslCurElem->m_ObjectID.objID = dlg.objID;
			eslCurElem->m_ObjectID.ObjToCtrl();

			// copy the text from the control to the list
			CString text;
			eslCurElem->m_ObjectID.ctrlWindow->GetDlgItem( IDC_OBJECTID )->GetWindowText( text );
			eslPagePtr->m_EventSumListCtrl.SetItemText( eslCurElemIndx, 0, text );

			eslPagePtr->UpdateEncoded();
		}
	}
}

//
//	EventSummaryList::OnSelchangeEventStateCombo
//

void EventSummaryList::OnSelchangeEventStateCombo( void )
{
	if (eslCurElem) {
		eslCurElem->eseEventStateCombo.UpdateData();
		eslPagePtr->UpdateEncoded();

		if (eslCurElem->eseEventStateCombo.ctrlNull)
			eslPagePtr->m_EventSumListCtrl.SetItemText( eslCurElemIndx, 1, "" );
		else
			eslPagePtr->m_EventSumListCtrl.SetItemText( eslCurElemIndx, 1
				, NetworkSniffer::BAC_STRTAB_BACnetEventState.m_pStrings[ eslCurElem->eseEventStateCombo.m_enumValue ]
				);
	}
}

//
//	EnrollmentSummaryList::OnSelchangeNotifyTypeCombo
//

void EventSummaryList::OnSelchangeNotifyTypeCombo( void )
{
	if (eslCurElem) {
		eslCurElem->eseNotifyTypeCombo.UpdateData();
		eslPagePtr->UpdateEncoded();

		if (eslCurElem->eseNotifyTypeCombo.ctrlNull)
			eslPagePtr->m_EventSumListCtrl.SetItemText( eslCurElemIndx, 2, "" );
		else
			eslPagePtr->m_EventSumListCtrl.SetItemText( eslCurElemIndx, 2
				, NetworkSniffer::BAC_STRTAB_BACnetNotifyType.m_pStrings[ eslCurElem->eseNotifyTypeCombo.m_enumValue ]
				);
	}
}
void EventSummaryList::OnOffnormal()
{
	if (eslCurElem) {
		eslCurElem->esackOffnormal.UpdateData();
		eslPagePtr->UpdateEncoded();
		UpdateBitsDisplay();
	}
}
void EventSummaryList::OnNormal()
{
	if (eslCurElem) {
		eslCurElem->esackNormal.UpdateData();
		eslPagePtr->UpdateEncoded();
		UpdateBitsDisplay();
	}
}
void EventSummaryList::OnFault()
{
	if (eslCurElem) {
		eslCurElem->esackFault.UpdateData();
		eslPagePtr->UpdateEncoded();
		UpdateBitsDisplay();
	}
}
void EventSummaryList::OnOffnormal1()
{
	if (eslCurElem) {
		eslCurElem->esenaOffnormal.UpdateData();
		eslPagePtr->UpdateEncoded();
		UpdateBitsDisplay();
	}
}
void EventSummaryList::OnNormal1()
{
	if (eslCurElem) {
		eslCurElem->esenaNormal.UpdateData();
		eslPagePtr->UpdateEncoded();
		UpdateBitsDisplay();
	}
}
void EventSummaryList::OnFault1()
{
	if (eslCurElem) {
		eslCurElem->esenaFault.UpdateData();
		eslPagePtr->UpdateEncoded();
		UpdateBitsDisplay();
	}
}
void EventSummaryList::OnSettimestamp()
{
	if (eslCurElem) {
		eslCurElem->listofTimestamp.DoModal();
		while(eslCurElem->listofTimestamp.m_TimeStampList.GetCount() != 3)
		{
			AfxMessageBox("Event Priorities are BACnetArray[3] of BACnetTimeStamp");
			eslCurElem->listofTimestamp.DoModal();
		}
		eslPagePtr->UpdateEncoded();
		UpdateBitsDisplay();
	}
}
void EventSummaryList::OnSetpriorities()
{
	if (eslCurElem) {
		eslCurElem->m_eventPriorities.DoModal();
		while(eslCurElem->m_eventPriorities.m_UnsignedList.GetCount() != 3)
		{
			AfxMessageBox("Event Priorities are BACnetArray[3] of Unsigned");
			eslCurElem->m_eventPriorities.DoModal();
		}
		eslPagePtr->UpdateEncoded();
		UpdateBitsDisplay();
	}
}
void EventSummaryList::UpdateBitsDisplay( void )
{
	bool	first = true
	;
	CString	rslt
	;
	CString rslt1;
	if (eslCurElem->esackOffnormal.boolValue == BACnetBoolean::bTrue) {
		rslt += "to-offnormal";
		first = false;
	}
	if (eslCurElem->esackFault.boolValue == BACnetBoolean::bTrue) {
		if (!first) rslt += ", ";
		rslt += "to-fault";
		first = false;
	}
	if (eslCurElem->esackNormal.boolValue == BACnetBoolean::bTrue) {
		if (!first) rslt += ", ";
		rslt += "to-normal";
	}
	// now set the display
	eslPagePtr->m_EventSumListCtrl.SetItemText( eslCurElemIndx, 3, rslt );

	first = true;
	if (eslCurElem->esenaOffnormal.boolValue == BACnetBoolean::bTrue) {
		rslt1 += "to-offnormal";
		first = false;
	}
	if (eslCurElem->esenaFault.boolValue == BACnetBoolean::bTrue) {
		if (!first) rslt += ", ";
		rslt1 += "to-fault";
		first = false;
	}
	if (eslCurElem->esenaNormal.boolValue == BACnetBoolean::bTrue) {
		if (!first) rslt += ", ";
		rslt1 += "to-normal";
	}
	// now set the display
	eslPagePtr->m_EventSumListCtrl.SetItemText( eslCurElemIndx, 4, rslt1 );
}
void EventSummaryList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = eslCurElemIndx
	;
	EventSummaryElemPtr	curElem = eslCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	// madanner 9/5/02, proper masking check?
	if ((pNMListView->uOldState && LVIS_SELECTED)  == (pNMListView->uNewState && LVIS_SELECTED) )
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
void EventSummaryList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound elements
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}




