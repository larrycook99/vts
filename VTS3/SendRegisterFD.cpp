// SendRegisterFD.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendRegisterFD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendRegisterFD::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendRegisterFD property page

IMPLEMENT_DYNCREATE( CSendRegisterFD, CPropertyPage )

#pragma warning( disable : 4355 )
CSendRegisterFD::CSendRegisterFD( void )
	: CSendPage( CSendRegisterFD::IDD )
	, m_TTL( this, IDC_TTL )
{
	//{{AFX_DATA_INIT(CSendRegisterFD)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendRegisterFD::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendRegisterFD::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendRegisterFD)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_TTL.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(CSendRegisterFD, CPropertyPage)
	//{{AFX_MSG_MAP(CSendRegisterFD)
	ON_EN_CHANGE(IDC_TTL, OnChangeTTL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendRegisterFD::InitPage
//

void CSendRegisterFD::InitPage( void )
{
	TRACE0( "CSendRegisterFD::InitPage()\n" );

	// flush the data
	m_TTL.ctrlNull = true;
}

//
//	CSendRegisterFD::EncodePage
//

void CSendRegisterFD::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendRegisterFD::EncodePage()\n" );

	// encode the type and function
	header.Add( 0x81 );
	header.Add( 0x05 );

	// encode the length
	header.Add( 0x00 );
	header.Add( 0x06 );

	// encode the time-to-live
	if (m_TTL.ctrlNull)
		throw "Time-to-live required";
	if ((m_TTL.intValue < 0) || (m_TTL.intValue > 65535))
		throw "Time-to-live out of range 0..65535";
	header.Add( m_TTL.intValue >> 8 );
	header.Add( m_TTL.intValue & 0xFF );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendRegisterFD::SavePage
//

void CSendRegisterFD::SavePage( void )
{
	TRACE0( "CSendRegisterFD::SavePage\n" );

	pageContents.Flush();

	m_TTL.SaveCtrl( pageContents );
}

//
//	CSendRegisterFD::RestorePage
//

void CSendRegisterFD::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendRegisterFD::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_TTL.RestoreCtrl( dec );
}

//
//	CSendRegisterFD::OnChangeTTL
//

void CSendRegisterFD::OnChangeTTL() 
{
	TRACE0( "CSendRegisterFD::OnChangeTTL()\n" );
	m_TTL.UpdateData();
	SavePage();
	UpdateEncoded();
}
