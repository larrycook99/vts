// SendEstablishCTN.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendEstablishCTN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendEstablishCTN::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendEstablishCTN dialog

IMPLEMENT_DYNCREATE( CSendEstablishCTN, CPropertyPage )

#pragma warning( disable : 4355 )
CSendEstablishCTN::CSendEstablishCTN( void )
	: CSendPage( CSendEstablishCTN::IDD )
	, m_DNET( this, IDC_DNET )
	, m_TermTime( this, IDC_TERMTIME )
{
	//{{AFX_DATA_INIT(CSendEstablishCTN)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendEstablishCTN::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendEstablishCTN::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendEstablishCTN)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_DNET.UpdateData( pDX->m_bSaveAndValidate );
	m_TermTime.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendEstablishCTN, CPropertyPage)
	//{{AFX_MSG_MAP(CSendEstablishCTN)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	ON_EN_CHANGE(IDC_TERMTIME, OnChangeTermTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendEstablishCTN::InitPage
//

void CSendEstablishCTN::InitPage( void )
{
	TRACE0( "CSendEstablishCTN::InitPage()\n" );

	// flush the data
	m_DNET.ctrlNull = true;
	m_TermTime.ctrlNull = true;

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendEstablishCTN::EncodePage
//

void CSendEstablishCTN::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendEstablishCTN::EncodePage()\n" );

	// encode the message type
	header.Add( 0x08 );

	// encode the DNET
	if (m_DNET.ctrlNull)
		throw "Destination network required";
	if ((m_DNET.intValue < 0) || (m_DNET.intValue > 65534))
		throw "Destination network out of range 0..65534";
	header.Add( m_DNET.intValue >> 8 );
	header.Add( m_DNET.intValue & 0xFF );

	// encode the termination time
	if (m_TermTime.ctrlNull)
		throw "Termination time required";
	if ((m_TermTime.intValue < 0) || (m_TermTime.intValue > 255))
		throw "Termination time out of range 0..255";
	header.Add( m_TermTime.intValue );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendEstablishCTN::SavePage
//

void CSendEstablishCTN::SavePage( void )
{
	TRACE0( "CSendEstablishCTN::SavePage\n" );

	pageContents.Flush();

	m_DNET.SaveCtrl( pageContents );
	m_TermTime.SaveCtrl( pageContents );
}

//
//	CSendEstablishCTN::RestorePage
//

void CSendEstablishCTN::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendEstablishCTN::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_DNET.RestoreCtrl( dec );
	m_TermTime.RestoreCtrl( dec );
}

//
//	CSendEstablishCTN::OnChangeDNET
//

void CSendEstablishCTN::OnChangeDNET() 
{
	TRACE0( "CSendEstablishCTN::OnChangeDNET()\n" );
	m_DNET.UpdateData();
	SavePage();
	UpdateEncoded();
}

//
//	CSendEstablishCTN::OnChangeTermTime
//

void CSendEstablishCTN::OnChangeTermTime() 
{
	TRACE0( "CSendEstablishCTN::OnChangeTermTime()\n" );
	m_TermTime.UpdateData();
	SavePage();
	UpdateEncoded();
}
