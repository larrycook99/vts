// SendDisconnectCTN.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendDisconnectCTN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendDisconnectCTN::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendDisconnectCTN dialog

IMPLEMENT_DYNCREATE( CSendDisconnectCTN, CPropertyPage )

#pragma warning( disable : 4355 )
CSendDisconnectCTN::CSendDisconnectCTN( void )
	: CSendPage( CSendDisconnectCTN::IDD )
	, m_DNET( this, IDC_DNET )
{
	//{{AFX_DATA_INIT(CSendDisconnectCTN)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendDisconnectCTN::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendDisconnectCTN::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendDisconnectCTN)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_DNET.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendDisconnectCTN, CPropertyPage)
	//{{AFX_MSG_MAP(CSendDisconnectCTN)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendDisconnectCTN::InitPage
//

void CSendDisconnectCTN::InitPage( void )
{
	TRACE0( "CSendDisconnectCTN::InitPage()\n" );

	// flush the data
	m_DNET.ctrlNull = true;

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendDisconnectCTN::EncodePage
//

void CSendDisconnectCTN::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendDisconnectCTN::EncodePage()\n" );

	// encode the message type
	header.Add( 0x09 );

	// encode the DNET
	if (m_DNET.ctrlNull)
		throw "Destination network required";
	if ((m_DNET.intValue < 0) || (m_DNET.intValue > 65534))
		throw "Destination network out of range 0..65534";
	header.Add( m_DNET.intValue >> 8 );
	header.Add( m_DNET.intValue & 0xFF );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendDisconnectCTN::SavePage
//

void CSendDisconnectCTN::SavePage( void )
{
	TRACE0( "CSendDisconnectCTN::SavePage\n" );

	pageContents.Flush();

	m_DNET.SaveCtrl( pageContents );
}

//
//	CSendDisconnectCTN::RestorePage
//

void CSendDisconnectCTN::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendDisconnectCTN::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_DNET.RestoreCtrl( dec );
}

//
//	CSendDisconnectCTN::OnChangeDNET
//

void CSendDisconnectCTN::OnChangeDNET() 
{
	TRACE0( "CSendDisconnectCTN::OnChangeDNET()\n" );
	m_DNET.UpdateData();
	SavePage();
	UpdateEncoded();
}
