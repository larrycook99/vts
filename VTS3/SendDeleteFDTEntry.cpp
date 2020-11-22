// SendDeleteFDTEntry.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendDeleteFDTEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendDeleteFDTEntry::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendDeleteFDTEntry dialog

IMPLEMENT_DYNCREATE( CSendDeleteFDTEntry, CPropertyPage )

#pragma warning( disable : 4355 )
CSendDeleteFDTEntry::CSendDeleteFDTEntry( void )
	: CSendPage( CSendDeleteFDTEntry::IDD )
	, m_ADDR( this, IDC_ADDRCOMBO, IDC_ADDR )
{
	//{{AFX_DATA_INIT(CSendDeleteFDTEntry)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendDeleteFDTEntry::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendDeleteFDTEntry)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_ADDR.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendDeleteFDTEntry, CPropertyPage)
	//{{AFX_MSG_MAP(CSendDeleteFDTEntry)
	ON_EN_CHANGE(IDC_ADDR, OnChangeADDR)
	ON_CBN_SELCHANGE(IDC_ADDRCOMBO, OnSelchangeAddrCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendDeleteFDTEntry::OnInitDialog
//

BOOL CSendDeleteFDTEntry::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the name list
	VTSDoc * pdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
	if ( pdoc )
		m_ADDR.LoadCombo( pdoc->GetNames(), pageParent->m_pPort );
	
	return TRUE;
}

//
//	CSendDeleteFDTEntry::InitPage
//

void CSendDeleteFDTEntry::InitPage( void )
{
	TRACE0( "CSendDeleteFDTEntry::InitPage()\n" );

	m_ADDR.ctrlNull = true;
}

//
//	CSendDeleteFDTEntry::EncodePage
//

void CSendDeleteFDTEntry::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendDeleteFDTEntry::EncodePage()\n" );

	// encode the message type and function
	header.Add( 0x81 );
	header.Add( 0x08 );

	// encode the length
	header.Add( 0x00 );
	header.Add( 0x0A );

	// encode the table entry
	if (m_ADDR.ctrlNull)
		throw "Foreign device address required";
	for (int i = 0; i < m_ADDR.addrLen; i++)
		header.Add( m_ADDR.addrAddr[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendDeleteFDTEntry::SavePage
//

void CSendDeleteFDTEntry::SavePage( void )
{
	TRACE0( "CSendDeleteFDTEntry::SavePage\n" );

	pageContents.Flush();

	m_ADDR.SaveCtrl( pageContents );
}

//
//	CSendDeleteFDTEntry::RestorePage
//

void CSendDeleteFDTEntry::RestorePage(  int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendDeleteFDTEntry::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ADDR.RestoreCtrl( dec );
}

//
//	CSendDeleteFDTEntry::OnChangeADDR
//

void CSendDeleteFDTEntry::OnChangeADDR() 
{
	m_ADDR.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendDeleteFDTEntry::OnSelchangeAddrCombo() 
{
	m_ADDR.Selchange();
	SavePage();
	UpdateEncoded();
}
