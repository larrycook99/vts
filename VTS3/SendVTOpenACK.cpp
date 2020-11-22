// SendVTOpenACK.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendVTOpenACK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendVTOpenACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendVTOpenACK dialog

IMPLEMENT_DYNCREATE( CSendVTOpenACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendVTOpenACK::CSendVTOpenACK( void )
	: CSendPage( CSendVTOpenACK::IDD )
	, m_SessionID( this, IDC_SESSIONID )
{
	//{{AFX_DATA_INIT(CSendVTOpenACK)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendVTOpenACK::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendVTOpenACK::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendVTOpenACK)
	//}}AFX_DATA_MAP

	m_SessionID.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendVTOpenACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendVTOpenACK)
	ON_EN_CHANGE(IDC_SESSIONID, OnChangeSessionID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendVTOpenACK::OnInitDialog
//

BOOL CSendVTOpenACK::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendVTOpenACK::InitPage
//

void CSendVTOpenACK::InitPage( void )
{
	// flush the data
	m_SessionID.ctrlNull = true;
}

//
//	CSendVTOpenACK::EncodePage
//

void CSendVTOpenACK::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the pdu type and service choice
	header.Add( 21 );

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
//	CSendVTOpenACK::SavePage
//

void CSendVTOpenACK::SavePage( void )
{
	TRACE0( "CSendVTOpenACK::SavePage\n" );

	pageContents.Flush();

	m_SessionID.SaveCtrl( pageContents );
}

//
//	CSendVTOpenACK::RestorePage
//

void CSendVTOpenACK::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendVTOpenACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_SessionID.RestoreCtrl( dec );
}

//
//	CSendVTOpenACK::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendVTOpenACK::OnChangeSessionID()
{
	m_SessionID.UpdateData();
	SavePage();
	UpdateEncoded();
}
