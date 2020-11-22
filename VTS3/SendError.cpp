// SendError.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendError.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendError::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendError dialog

IMPLEMENT_DYNCREATE( CSendError, CPropertyPage )

#pragma warning( disable : 4355 )
CSendError::CSendError( void )
	: CSendPage( CSendError::IDD )
	, m_InvokeID( this, IDC_INVOKEID )
	, m_ServiceCombo( this, IDC_SERVICECOMBO, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice, true )
	, m_ErrorClassCombo( this, IDC_ERRORCLASSCOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorClass, true )
	, m_ErrorCodeCombo( this, IDC_ERRORCODECOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorCode, true )
{
	//{{AFX_DATA_INIT(CSendError)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendError::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendError::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendError)
	//}}AFX_DATA_MAP

	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
	m_ServiceCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ErrorClassCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ErrorCodeCombo.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendError, CPropertyPage)
	//{{AFX_MSG_MAP(CSendError)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	ON_CBN_SELCHANGE(IDC_SERVICECOMBO, OnSelchangeServiceCombo)
	ON_CBN_SELCHANGE(IDC_ERRORCLASSCOMBO, OnSelchangeErrorClassCombo)
	ON_CBN_SELCHANGE(IDC_ERRORCODECOMBO, OnSelchangeErrorCodeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendError::OnInitDialog
//

BOOL CSendError::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration tables
	m_ServiceCombo.LoadCombo();
	m_ErrorClassCombo.LoadCombo();
	m_ErrorCodeCombo.LoadCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendError::InitPage
//

void CSendError::InitPage( void )
{
	TRACE0( "CSendError::InitPage()\n" );

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
//	CSendError::EncodePage
//

void CSendError::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendError::EncodePage()\n" );

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
	header.Add( m_ServiceCombo.m_enumValue );

	// encode the error class
	if (m_ErrorClassCombo.ctrlNull)
		throw "Error class required";
	m_ErrorClassCombo.Encode( enc );

	// encode the error code
	if (m_ErrorCodeCombo.ctrlNull)
		throw "Error code required";
	m_ErrorCodeCombo.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendError::SavePage
//

void CSendError::SavePage( void )
{
	TRACE0( "CSendError::SavePage\n" );

	pageContents.Flush();

	m_InvokeID.SaveCtrl( pageContents );
	m_ServiceCombo.SaveCtrl( pageContents );
	m_ErrorClassCombo.SaveCtrl( pageContents );
	m_ErrorCodeCombo.SaveCtrl( pageContents );
}

//
//	CSendError::RestorePage
//

void CSendError::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendError::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_InvokeID.RestoreCtrl( dec );
	m_ServiceCombo.RestoreCtrl( dec );
	m_ErrorClassCombo.RestoreCtrl( dec );
	m_ErrorCodeCombo.RestoreCtrl( dec );
}

//
//	CSendError::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendError::OnChangeInvokeID()
{
	TRACE0( "CSendError::OnChangeInvokeID()\n" );
	m_InvokeID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendError::OnSelchangeServiceCombo() 
{
	TRACE0( "CSendError::OnSelchangeServiceCombo()\n" );
	m_ServiceCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendError::OnSelchangeErrorClassCombo() 
{
	TRACE0( "CSendError::OnSelchangeErrorClassCombo()\n" );
	m_ErrorClassCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendError::OnSelchangeErrorCodeCombo() 
{
	TRACE0( "CSendError::OnSelchangeErrorCodeCombo()\n" );
	m_ErrorCodeCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}
