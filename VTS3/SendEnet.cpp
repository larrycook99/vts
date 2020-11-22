// SendEnet.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendEnet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BACnetLSAP		0x82

BACnetAPDUEncoder CSendEnet::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendEnet dialog

IMPLEMENT_DYNCREATE( CSendEnet, CPropertyPage )

#pragma warning( disable : 4355 )
CSendEnet::CSendEnet( void )
	: CSendPage( CSendEnet::IDD )
	, m_SADR( this, IDC_SADRCOMBO, IDC_SADR )
	, m_DADR( this, IDC_DADRCOMBO, IDC_DADR )
{
	//{{AFX_DATA_INIT(CSendEnet)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendEnet::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendEnet)
	//}}AFX_DATA_MAP

	m_SADR.UpdateData( pDX->m_bSaveAndValidate );
	m_DADR.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendEnet, CPropertyPage)
	//{{AFX_MSG_MAP(CSendEnet)
	ON_EN_CHANGE(IDC_SADR, OnChangeSADR)
	ON_EN_CHANGE(IDC_DADR, OnChangeDADR)
	ON_CBN_SELCHANGE(IDC_SADRCOMBO, OnSelchangeSADRCombo)
	ON_CBN_SELCHANGE(IDC_DADRCOMBO, OnSelchangeDADRCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendEnet::OnInitDialog
//

BOOL CSendEnet::OnInitDialog() 
{
	TRACE0( "CSendEnet::OnInitDialog()\n" );

	CPropertyPage::OnInitDialog();
	
	VTSDoc * pdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();

	// Load the combos.  Allow broadcast destination
	if ( pdoc )
		m_SADR.LoadCombo( pdoc->GetNames(), pageParent->m_pPort );
	m_SADR.FindName( "TD" );

	if ( pdoc )
		m_DADR.LoadCombo( pdoc->GetNames(), pageParent->m_pPort, true );
	m_DADR.FindName( "IUT" );

	return TRUE;
}

//
//	CSendEnet::InitPage
//
//	This function will be called when the page is installed (see SetPageList()).
//

void CSendEnet::InitPage( void )
{
	TRACE0( "CSendEnet::InitPage()\n" );
}

//
//	CSendEnet::EncodePage
//

void CSendEnet::EncodePage( CByteArray* contents )
{
	int			len
	;
	CByteArray	header
	;

	TRACE0( "CSendEnet::EncodePage()\n" );

	// validate
	if (m_SADR.ctrlNull)
		throw "Source address required";
	if (m_SADR.addrLen != 6)
		throw "Invalid source address length";

	if (m_DADR.ctrlNull)
		throw "Destination address required";
	if (m_DADR.addrLen != 6)
		throw "Invalid destination address length";

	// get the length of the stuff already encoded, add SAP overhead
	len = contents->GetSize() + 3;

	// stuff the octets on the front
	for (int i = 0; i < m_DADR.addrLen; i++)
		header.Add( m_DADR.addrAddr[i] );
	for (int j = 0; j < m_SADR.addrLen; j++)
		header.Add( m_SADR.addrAddr[j] );

	// encode the length
	header.Add( (len >> 8) & 0xFF );		// MS len
	header.Add( (len & 0xFF) );				// LS len

	// encode the SAP
	header.Add( BACnetLSAP );				// DSAP
	header.Add( BACnetLSAP );				// SSAP
	header.Add( 0x03 );						// LLC Control = UI

	len = header.GetSize();
	// stuff the header on the front
	contents->InsertAt( 0, &header );

	// pad the packet to the Ethernet minimum
	len = contents->GetSize();
	for (int k = len; k < 60; k++)
		contents->Add( 0x00 );
}

//
//	CSendEnet::SavePage
//

void CSendEnet::SavePage( void )
{
	TRACE0( "CSendEnet::SavePage\n" );

	pageContents.Flush();

	m_SADR.SaveCtrl( pageContents );
	m_DADR.SaveCtrl( pageContents );
}

//
//	CSendEnet::RestorePage
//

void CSendEnet::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendEnet::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_SADR.RestoreCtrl( dec );
	m_DADR.RestoreCtrl( dec );
}

/////////////////////////////////////////////////////////////////////////////
// CSendEnet message handlers


void CSendEnet::OnChangeSADR() 
{
	m_SADR.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendEnet::OnChangeDADR() 
{
	m_DADR.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendEnet::OnSelchangeDADRCombo() 
{
	m_DADR.Selchange();
	SavePage();
	UpdateEncoded();
}

void CSendEnet::OnSelchangeSADRCombo() 
{
	m_SADR.Selchange();
	SavePage();
	UpdateEncoded();
}
