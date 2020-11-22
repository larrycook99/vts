// SendVTCloseError.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendVTCloseError.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendVTCloseError::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendVTCloseError dialog

IMPLEMENT_DYNCREATE( CSendVTCloseError, CPropertyPage )

#pragma warning( disable : 4355 )
CSendVTCloseError::CSendVTCloseError( void )
	: CSendPage( CSendVTCloseError::IDD )
	, m_InvokeID( this, IDC_INVOKEID )
	, m_ServiceCombo( this, IDC_SERVICECOMBO, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice, true )
	, m_ErrorClassCombo( this, IDC_ERRORCLASSCOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorClass, true )
	, m_ErrorCodeCombo( this, IDC_ERRORCODECOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorCode, true )
{
	//{{AFX_DATA_INIT(CSendVTCloseError)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendVTCloseError::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendVTCloseError::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendVTCloseError)
	DDX_Control(pDX, IDC_IDLIST, m_IDList);
	//}}AFX_DATA_MAP

	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
	m_ServiceCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ErrorClassCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ErrorCodeCombo.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendVTCloseError, CPropertyPage)
	//{{AFX_MSG_MAP(CSendVTCloseError)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	ON_CBN_SELCHANGE(IDC_SERVICECOMBO, OnSelchangeServiceCombo)
	ON_CBN_SELCHANGE(IDC_ERRORCLASSCOMBO, OnSelchangeErrorClassCombo)
	ON_CBN_SELCHANGE(IDC_ERRORCODECOMBO, OnSelchangeErrorCodeCombo)
	ON_BN_CLICKED(IDC_ADDID, OnAddID)
	ON_BN_CLICKED(IDC_REMOVEID, OnRemoveID)
	ON_EN_CHANGE(IDC_ID, OnChangeID)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_IDLIST, OnItemchangingIDList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendVTCloseError::OnInitDialog
//

BOOL CSendVTCloseError::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "ID", LVCFMT_RIGHT, 96, IDC_ID }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendVTCloseError::OnInitDialog()\n" );

	CDialog::OnInitDialog();

	// only allow one selection at a time, no sorting
	m_IDList.m_nFlags |= LVS_SINGLESEL;
	m_IDList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_IDCtrl.Init( this, &m_IDList, colDesc );

	// load the enumeration tables
	m_ServiceCombo.LoadCombo();
	m_ErrorClassCombo.LoadCombo();
	m_ErrorCodeCombo.LoadCombo();

	return TRUE;
}

//
//	CSendVTCloseError::InitPage
//

void CSendVTCloseError::InitPage( void )
{
	TRACE0( "CSendVTCloseError::InitPage()\n" );

	// flush the data
	m_InvokeID.ctrlNull = true;

	m_ServiceCombo.ctrlNull = false;
	m_ServiceCombo.m_enumValue = 0;

	m_ErrorClassCombo.ctrlNull = false;
	m_ErrorClassCombo.m_enumValue = 0;

	m_ErrorCodeCombo.ctrlNull = false;
	m_ErrorCodeCombo.m_enumValue = 0;
}

//
//	CSendVTCloseError::EncodePage
//

void CSendVTCloseError::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendVTCloseError::EncodePage()\n" );

	// encode the pdu type
	header.Add( 0x50 );

	// encode the invoke ID
	if (m_InvokeID.ctrlNull)
		throw "Original invoke ID required";
	if ((m_InvokeID.intValue < 0) || (m_InvokeID.intValue > 255))
		throw "Original invoke ID out of range 0..255";
	header.Add( m_InvokeID.intValue );

	// encode the service choice
	if (m_ServiceCombo.ctrlNull)
		throw "Service ACK choice required";
	if (m_ServiceCombo.m_enumValue != 22)
		throw "Service must be vtClose";
	header.Add( m_ServiceCombo.m_enumValue );

	// opening tag
	BACnetOpeningTag().Encode( enc, 0 );

	// encode the error class
	if (m_ErrorClassCombo.ctrlNull)
		throw "Error class required";
	m_ErrorClassCombo.Encode( enc );

	// encode the error code
	if (m_ErrorCodeCombo.ctrlNull)
		throw "Error code required";
	m_ErrorCodeCombo.Encode( enc );

	// closing tag
	BACnetClosingTag().Encode( enc, 0 );

	// opening tag
	BACnetOpeningTag().Encode( enc, 1 );

	// validate and encode the DNET's
	int len = m_IDCtrl.GetItemCount();
	for (int i = 0; i < len; i++) {
		const char *txt = m_IDCtrl.GetItemText( i, 0 );

		// make sure something was provided
		if (!txt || !*txt)
			throw "Invalid ID";

		// convert to an integer and check the range
		int id = atoi( txt );
		if ((id < 0) || (id > 255))
			throw "ID out of range 0..255";

		// encode it
		BACnetUnsigned( id ).Encode( enc );
	}

	// closing tag
	BACnetClosingTag().Encode( enc, 1 );

	// copy the encoding into the byte array
	for (int j = 0; j < enc.pktLength; j++)
		header.Add( enc.pktBuffer[j] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendVTCloseError::SavePage
//

void CSendVTCloseError::SavePage( void )
{
	TRACE0( "CSendVTCloseError::SavePage\n" );

	pageContents.Flush();

	m_InvokeID.SaveCtrl( pageContents );
	m_ServiceCombo.SaveCtrl( pageContents );
	m_ErrorClassCombo.SaveCtrl( pageContents );
	m_ErrorCodeCombo.SaveCtrl( pageContents );
//	m_IDCtrl.SaveList( pageContents );
}

//
//	CSendVTCloseError::RestorePage
//

void CSendVTCloseError::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendVTCloseError::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_InvokeID.RestoreCtrl( dec );
	m_ServiceCombo.RestoreCtrl( dec );
	m_ErrorClassCombo.RestoreCtrl( dec );
	m_ErrorCodeCombo.RestoreCtrl( dec );
//	m_IDCtrl.RestoreList( dec );
}

//
//	CSendVTCloseError::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendVTCloseError::OnChangeInvokeID()
{
	TRACE0( "CSendVTCloseError::OnChangeInvokeID()\n" );
	m_InvokeID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendVTCloseError::OnSelchangeServiceCombo() 
{
	TRACE0( "CSendVTCloseError::OnSelchangeServiceCombo()\n" );
	m_ServiceCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendVTCloseError::OnSelchangeErrorClassCombo() 
{
	TRACE0( "CSendVTCloseError::OnSelchangeErrorClassCombo()\n" );
	m_ErrorClassCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendVTCloseError::OnSelchangeErrorCodeCombo() 
{
	TRACE0( "CSendVTCloseError::OnSelchangeErrorCodeCombo()\n" );
	m_ErrorCodeCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendVTCloseError::OnAddID() 
{
	m_IDCtrl.AddButtonClick();

	// madanner 9/3/02, added default ID of 1
	m_IDCtrl.SetCtrlText(IDC_ID, "1");
}

void CSendVTCloseError::OnRemoveID() 
{
	m_IDCtrl.RemoveButtonClick();
}

void CSendVTCloseError::OnChangeID() 
{
	m_IDCtrl.OnChangeItem( IDC_ID );
}

void CSendVTCloseError::OnItemchangingIDList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_IDCtrl.OnItemChanging( pNMHDR, pResult );
}
