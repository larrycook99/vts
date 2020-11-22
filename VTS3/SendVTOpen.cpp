// SendVTOpen.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendVTOpen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendVTOpen::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CCSendVTOpen dialog

IMPLEMENT_DYNCREATE( CSendVTOpen, CPropertyPage )

#pragma warning( disable : 4355 )
CSendVTOpen::CSendVTOpen( void )
	: CSendPage( CSendVTOpen::IDD )
	, m_VTClassCombo( this, IDC_VTCLASSCOMBO, NetworkSniffer::BAC_STRTAB_BACnetVTClass, true )
	, m_SessionID( this, IDC_SESSIONID )
{
	//{{AFX_DATA_INIT(CSendVTOpen)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendVTOpen::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendVTOpen::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendVTOpen)
	//}}AFX_DATA_MAP

	m_VTClassCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_SessionID.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendVTOpen, CPropertyPage)
	//{{AFX_MSG_MAP(CSendVTOpen)
	ON_CBN_SELCHANGE(IDC_VTCLASSCOMBO, OnSelchangeVTClassCombo)
	ON_EN_CHANGE(IDC_SESSIONID, OnChangeSessionID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendVTOpen::OnInitDialog
//

BOOL CSendVTOpen::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_VTClassCombo.LoadCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendVTOpen::InitPage
//

void CSendVTOpen::InitPage( void )
{
	TRACE0( "CSendVTOpen::InitPage()\n" );

	// flush the data
	m_VTClassCombo.ctrlNull = false;
	m_VTClassCombo.m_enumValue = 0;

	m_SessionID.ctrlNull = true;
}

//
//	CSendVTOpen::EncodePage
//

void CSendVTOpen::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendVTOpen::EncodePage()\n" );

	// encode the pdu type and service choice
	header.Add( 21 );

	// encode the virtual terminal class
	if (m_VTClassCombo.ctrlNull)
		throw "VT class selection required";
	m_VTClassCombo.Encode( enc );

	// encode the local VT session identifier
	if (m_SessionID.ctrlNull)
		throw "Local VT session ID required";
	m_SessionID.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendVTOpen::SavePage
//

void CSendVTOpen::SavePage( void )
{
	TRACE0( "CSendVTOpen::SavePage\n" );

	pageContents.Flush();

	m_VTClassCombo.SaveCtrl( pageContents );
	m_SessionID.SaveCtrl( pageContents );
}

//
//	CSendVTOpen::RestorePage
//

void CSendVTOpen::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendVTOpen::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_VTClassCombo.RestoreCtrl( dec );
	m_SessionID.RestoreCtrl( dec );
}

//
//	CSendVTOpen::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendVTOpen::OnSelchangeVTClassCombo() 
{
	m_VTClassCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendVTOpen::OnChangeSessionID()
{
	m_SessionID.UpdateData();
	SavePage();
	UpdateEncoded();
}
