// SendSimpleACK.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendSimpleACK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendSimpleACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendSimpleACK dialog

IMPLEMENT_DYNCREATE( CSendSimpleACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendSimpleACK::CSendSimpleACK( void )
	: CSendPage( CSendSimpleACK::IDD )
	, m_InvokeID( this, IDC_INVOKEID )
	, m_ServiceCombo( this, IDC_SERVICECOMBO, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice, true )
{
	//{{AFX_DATA_INIT(CSendSimpleACK)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendSimpleACK::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendSimpleACK::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendSimpleACK)
	//}}AFX_DATA_MAP

	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
	m_ServiceCombo.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendSimpleACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendSimpleACK)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	ON_CBN_SELCHANGE(IDC_SERVICECOMBO, OnSelchangeServiceCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendSimpleACK::OnInitDialog
//

BOOL CSendSimpleACK::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_ServiceCombo.LoadCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendSimpleACK::InitPage
//

void CSendSimpleACK::InitPage( void )
{
	TRACE0( "CSendSimpleACK::InitPage()\n" );

	// flush the data
	m_InvokeID.ctrlNull = true;

	m_ServiceCombo.ctrlNull = false;
	m_ServiceCombo.m_enumValue = 0;
}

//
//	CSendSimpleACK::EncodePage
//

void CSendSimpleACK::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendSimpleACK::EncodePage()\n" );

	// encode the pdu type
	header.Add( 0x20 );

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

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendSimpleACK::SavePage
//

void CSendSimpleACK::SavePage( void )
{
	TRACE0( "CSendSimpleACK::SavePage\n" );

	pageContents.Flush();

	m_InvokeID.SaveCtrl( pageContents );
	m_ServiceCombo.SaveCtrl( pageContents );
}

//
//	CSendSimpleACK::RestorePage
//

void CSendSimpleACK::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendSimpleACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_InvokeID.RestoreCtrl( dec );
	m_ServiceCombo.RestoreCtrl( dec );
}

//
//	CSendSimpleACK::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendSimpleACK::OnChangeInvokeID()
{
	TRACE0( "CSendSimpleACK::OnChangeInvokeID()\n" );
	m_InvokeID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSimpleACK::OnSelchangeServiceCombo() 
{
	TRACE0( "CSendSimpleACK::OnSelchangeServiceCombo()\n" );
	m_ServiceCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}
