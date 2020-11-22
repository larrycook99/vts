// SendRejectMTN.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendRejectMTN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendRejectMTN::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendRejectMTN dialog

IMPLEMENT_DYNCREATE( CSendRejectMTN, CPropertyPage )

#pragma warning( disable : 4355 )
CSendRejectMTN::CSendRejectMTN( void )
	: CSendPage( CSendRejectMTN::IDD )
	, m_Reason( this, IDC_REASON )
	, m_DNET( this, IDC_DNET )
{
	//{{AFX_DATA_INIT(CSendRejectMTN)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendRejectMTN::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendRejectMTN::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendRejectMTN)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_Reason.UpdateData( pDX->m_bSaveAndValidate );
	m_DNET.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(CSendRejectMTN, CPropertyPage)
	//{{AFX_MSG_MAP(CSendRejectMTN)
	ON_EN_CHANGE(IDC_REASON, OnChangeReason)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendRejectMTN::InitPage
//

void CSendRejectMTN::InitPage( void )
{
	TRACE0( "CSendRejectMTN::InitPage()\n" );

	// flush the data
	m_Reason.ctrlNull = true;
	m_DNET.ctrlNull = true;

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendRejectMTN::EncodePage
//

void CSendRejectMTN::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendRejectMTN::EncodePage()\n" );

	// encode the message type
	header.Add( 0x03 );

	// encode the performance index
	if (m_Reason.ctrlNull)
		throw "Reason required";
	if ((m_Reason.intValue < 0) || (m_Reason.intValue > 4))
		throw "Reason out of range 0..4";
	header.Add( m_Reason.intValue );

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
//	CSendRejectMTN::SavePage
//

void CSendRejectMTN::SavePage( void )
{
	TRACE0( "CSendRejectMTN::SavePage\n" );

	pageContents.Flush();

	m_Reason.SaveCtrl( pageContents );
	m_DNET.SaveCtrl( pageContents );
}

//
//	CSendRejectMTN::RestorePage
//

void CSendRejectMTN::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendRejectMTN::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_Reason.RestoreCtrl( dec );
	m_DNET.RestoreCtrl( dec );
}

//
//	CSendRejectMTN::OnChangeReason
//

void CSendRejectMTN::OnChangeReason() 
{
	TRACE0( "CSendRejectMTN::OnChangeReason()\n" );
	m_Reason.UpdateData();
	SavePage();
	UpdateEncoded();
}

//
//	CSendRejectMTN::OnChangeDNET
//

void CSendRejectMTN::OnChangeDNET() 
{
	TRACE0( "CSendRejectMTN::OnChangeDNET()\n" );
	m_DNET.UpdateData();
	SavePage();
	UpdateEncoded();
}
