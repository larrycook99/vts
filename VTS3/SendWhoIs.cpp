// SendWhoIs.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendWhoIs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder  CSendWhoIs::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendWhoIs dialog

IMPLEMENT_DYNCREATE( CSendWhoIs, CPropertyPage )

#pragma warning( disable : 4355 )
CSendWhoIs::CSendWhoIs( void )
	: CSendPage( CSendWhoIs::IDD )
	, m_LowLimit( this, IDC_LOWLIMIT )
	, m_HighLimit( this, IDC_HIGHLIMIT )
{
	//{{AFX_DATA_INIT(CSendWhoIs)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendWhoIs::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendWhoIs::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWhoIs)
	//}}AFX_DATA_MAP

	m_LowLimit.UpdateData( pDX->m_bSaveAndValidate );
	m_HighLimit.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendWhoIs, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWhoIs)
	ON_EN_CHANGE(IDC_LOWLIMIT, OnChangeLowLimit)
	ON_EN_CHANGE(IDC_HIGHLIMIT, OnChangeHighLimit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWhoIs::OnInitDialog
//

BOOL CSendWhoIs::OnInitDialog() 
{
	TRACE0( "CSendWhoIs::OnInitDialog()\n" );

	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendWhoIs::InitPage
//

void CSendWhoIs::InitPage( void )
{
	TRACE0( "CSendWhoIs::InitPage()\n" );

	// flush the data
	m_LowLimit.ctrlNull = true;
	m_HighLimit.ctrlNull = true;

	// tell the NPCI this is a global broadcast
	pageParent->NPCIPage.m_DestPresent = true;

	pageParent->NPCIPage.m_DNET.ctrlNull = false;
	pageParent->NPCIPage.m_DNET.intValue = 65535;

	pageParent->NPCIPage.m_HopCount.ctrlNull = false;
	pageParent->NPCIPage.m_HopCount.intValue = 255;
}

//
//	CSendWhoIs::EncodePage
//

void CSendWhoIs::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendWhoIs::EncodePage()\n" );

	// encode the pdu type and service choice
	header.Add( 0x10 );
	header.Add( 0x08 );

	// encode the device ID
	if (!m_LowLimit.ctrlNull || !m_HighLimit.ctrlNull) {
		if (m_LowLimit.ctrlNull)
			throw "Low limit required";
		if ((m_LowLimit.uintValue < 0) || (m_LowLimit.uintValue > 4194303))
			throw "Low limit out of range 0..4194303";
		if (m_HighLimit.ctrlNull)
			throw "High limit required";
		if ((m_HighLimit.uintValue < 0) || (m_HighLimit.uintValue > 4194303))
			throw "High limit out of range 0..4194303";

		m_LowLimit.Encode( enc, 0 );
		m_HighLimit.Encode( enc, 1 );
	}

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendWhoIs::SavePage
//

void CSendWhoIs::SavePage( void )
{
	TRACE0( "CSendWhoIs::SavePage\n" );

	pageContents.Flush();

	m_LowLimit.SaveCtrl( pageContents );
	m_HighLimit.SaveCtrl( pageContents );
}

//
//	CSendWhoIs::RestorePage
//

void CSendWhoIs::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendWhoIs::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_LowLimit.RestoreCtrl( dec );
	m_HighLimit.RestoreCtrl( dec );
}

//
//	CSendWhoIs::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendWhoIs::OnChangeLowLimit()
{
	TRACE0( "CSendWhoIs::OnChangeLowLimit()\n" );
	m_LowLimit.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWhoIs::OnChangeHighLimit()
{
	TRACE0( "CSendWhoIs::OnChangeHighLimit()\n" );
	m_HighLimit.UpdateData();
	SavePage();
	UpdateEncoded();
}
