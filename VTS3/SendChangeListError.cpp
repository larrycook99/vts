// SendChangeListError.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendChangeListError.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendChangeListError::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendChangeListError dialog

IMPLEMENT_DYNCREATE( CSendChangeListError, CPropertyPage )

#pragma warning( disable : 4355 )
CSendChangeListError::CSendChangeListError( void )
	: CSendPage( CSendChangeListError::IDD )
	, m_InvokeID( this, IDC_INVOKEID )
	, m_ServiceCombo( this, IDC_SERVICECOMBO, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice, true )
	, m_ErrorClassCombo( this, IDC_ERRORCLASSCOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorClass, true )
	, m_ErrorCodeCombo( this, IDC_ERRORCODECOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorCode, true )
	, m_Element( this, IDC_ELEMENT )
{
	//{{AFX_DATA_INIT(CSendChangeListError)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendChangeListError::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendChangeListError::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendChangeListError)
	//}}AFX_DATA_MAP

	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
	m_ServiceCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ErrorClassCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ErrorCodeCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_Element.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendChangeListError, CPropertyPage)
	//{{AFX_MSG_MAP(CSendChangeListError)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	ON_CBN_SELCHANGE(IDC_SERVICECOMBO, OnSelchangeServiceCombo)
	ON_CBN_SELCHANGE(IDC_ERRORCLASSCOMBO, OnSelchangeErrorClassCombo)
	ON_CBN_SELCHANGE(IDC_ERRORCODECOMBO, OnSelchangeErrorCodeCombo)
	ON_EN_CHANGE(IDC_ELEMENT, OnChangeElement)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendChangeListError::OnInitDialog
//

BOOL CSendChangeListError::OnInitDialog() 
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
//	CSendChangeListError::InitPage
//

void CSendChangeListError::InitPage( void )
{
	TRACE0( "CSendChangeListError::InitPage()\n" );

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
//	CSendChangeListError::EncodePage
//

void CSendChangeListError::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendChangeListError::EncodePage()\n" );

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
	if ((m_ServiceCombo.m_enumValue != 8) && (m_ServiceCombo.m_enumValue != 9))
		throw "Service must be addListElement or removeListElement";
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

	// encode the first failed element number (context 1)
	if (m_Element.ctrlNull)
		throw "Element number required";
	m_Element.Encode( enc, 1 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendChangeListError::SavePage
//

void CSendChangeListError::SavePage( void )
{
	TRACE0( "CSendChangeListError::SavePage\n" );

	pageContents.Flush();

	m_InvokeID.SaveCtrl( pageContents );
	m_ServiceCombo.SaveCtrl( pageContents );
	m_ErrorClassCombo.SaveCtrl( pageContents );
	m_ErrorCodeCombo.SaveCtrl( pageContents );
	m_Element.SaveCtrl( pageContents );
}

//
//	CSendChangeListError::RestorePage
//

void CSendChangeListError::RestorePage(  int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendChangeListError::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_InvokeID.RestoreCtrl( dec );
	m_ServiceCombo.RestoreCtrl( dec );
	m_ErrorClassCombo.RestoreCtrl( dec );
	m_ErrorCodeCombo.RestoreCtrl( dec );
	m_Element.RestoreCtrl( dec );
}

//
//	CSendChangeListError::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendChangeListError::OnChangeInvokeID()
{
	TRACE0( "CSendChangeListError::OnChangeInvokeID()\n" );
	m_InvokeID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendChangeListError::OnSelchangeServiceCombo() 
{
	TRACE0( "CSendChangeListError::OnSelchangeServiceCombo()\n" );
	m_ServiceCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendChangeListError::OnSelchangeErrorClassCombo() 
{
	TRACE0( "CSendChangeListError::OnSelchangeErrorClassCombo()\n" );
	m_ErrorClassCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendChangeListError::OnSelchangeErrorCodeCombo() 
{
	TRACE0( "CSendChangeListError::OnSelchangeErrorCodeCombo()\n" );
	m_ErrorCodeCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendChangeListError::OnChangeElement()
{
	TRACE0( "CSendChangeListError::OnChangeElement()\n" );
	m_Element.UpdateData();
	SavePage();
	UpdateEncoded();
}
