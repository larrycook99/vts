// SendIP.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendIP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendIP::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendIP dialog

IMPLEMENT_DYNCREATE( CSendIP, CPropertyPage )

#pragma warning( disable : 4355 )
CSendIP::CSendIP( void )
	: CSendPage( CSendIP::IDD )
	, m_DADR( this, IDC_DADRCOMBO, IDC_DADR )
{
	//{{AFX_DATA_INIT(CSendIP)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendIP::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendIP::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendIP)
	//}}AFX_DATA_MAP

	m_DADR.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendIP, CPropertyPage)
	//{{AFX_MSG_MAP(CSendIP)
	ON_EN_UPDATE(IDC_DADR, OnUpdateDadr)
	ON_CBN_SELCHANGE(IDC_DADRCOMBO, OnSelchangeDADRCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendIP::OnInitDialog
//

BOOL CSendIP::OnInitDialog() 
{
	TRACE0( "CSendIP::OnInitDialog\n" );

	CPropertyPage::OnInitDialog();
	
	// load the address combo, include broadcast addresses
	VTSDoc * pdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
	if ( pdoc )
		m_DADR.LoadCombo( pdoc->GetNames(), pageParent->m_pPort, true );
	m_DADR.FindName( "IUT" );
	
	return TRUE;
}

//
//	CSendPage::InitPage
//

void CSendIP::InitPage( void )
{
	TRACE0( "CSendIP::InitPage()\n" );
}

//
//	CSendIP::EncodePage
//

void CSendIP::EncodePage( CByteArray* contents )
{
	TRACE0( "CSendIP::EncodePage()\n" );

	// validate
	if (m_DADR.ctrlNull || (m_DADR.addrLen != 6))
		throw "Destination address required";

	// stuff the octets on the front
	for (int i = 0; i < m_DADR.addrLen; i++)
		contents->InsertAt( i, m_DADR.addrAddr[i] );
}

//
//	CSendIP::SavePage
//

void CSendIP::SavePage( void )
{
	TRACE0( "CSendIP::SavePage\n" );

	pageContents.Flush();

	m_DADR.SaveCtrl( pageContents );
}

//
//	CSendIP::RestorePage
//

void CSendIP::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendIP::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_DADR.RestoreCtrl( dec );
}

/////////////////////////////////////////////////////////////////////////////
// CSendIP message handlers

void CSendIP::OnUpdateDadr() 
{
	TRACE0( "CSendIP::OnUpdateDadr()\n" );

	// transfer the control contents to the address
	m_DADR.UpdateData();

	SavePage();

	// update the encoding
	UpdateEncoded();
}

void CSendIP::OnSelchangeDADRCombo() 
{
	m_DADR.Selchange();
	SavePage();
	UpdateEncoded();
}
