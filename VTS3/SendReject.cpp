// SendReject.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendReject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendReject::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendReject dialog

IMPLEMENT_DYNCREATE( CSendReject, CPropertyPage )

#pragma warning( disable : 4355 )
CSendReject::CSendReject( void )
	: CSendPage( CSendReject::IDD )
	, m_InvokeID( this, IDC_INVOKEID )
	, m_RejectCombo( this, IDC_REJECTCOMBO, NetworkSniffer::BAC_STRTAB_BACnetReject, true )
{
	//{{AFX_DATA_INIT(CSendReject)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendReject::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendReject::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReject)
	//}}AFX_DATA_MAP

	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
	m_RejectCombo.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendReject, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReject)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	ON_CBN_SELCHANGE(IDC_REJECTCOMBO, OnSelchangeRejectCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReject::OnInitDialog
//

BOOL CSendReject::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_RejectCombo.LoadCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendReject::InitPage
//

void CSendReject::InitPage( void )
{
	TRACE0( "CSendReject::InitPage()\n" );

	// flush the data
	m_InvokeID.ctrlNull = true;

	m_RejectCombo.ctrlNull = false;
	m_RejectCombo.m_enumValue = 0;
}

//
//	CSendReject::EncodePage
//

void CSendReject::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendReject::EncodePage()\n" );

	// encode the pdu type
	header.Add( 0x60 );

	// encode the invoke ID
	if (m_InvokeID.ctrlNull)
		throw "Original invoke ID required";
	if ((m_InvokeID.intValue < 0) || (m_InvokeID.intValue > 255))
		throw "Original invoke ID out of range 0..255";
	header.Add( m_InvokeID.intValue );

	// encode the service choice
	if (m_RejectCombo.ctrlNull)
		throw "Reject reason required";
	header.Add( m_RejectCombo.m_enumValue );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendReject::SavePage
//

void CSendReject::SavePage( void )
{
	TRACE0( "CSendReject::SavePage\n" );

	pageContents.Flush();

	m_InvokeID.SaveCtrl( pageContents );
	m_RejectCombo.SaveCtrl( pageContents );
}

//
//	CSendReject::RestorePage
//

void CSendReject::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendReject::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_InvokeID.RestoreCtrl( dec );
	m_RejectCombo.RestoreCtrl( dec );
}

//
//	CSendReject::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendReject::OnChangeInvokeID()
{
	TRACE0( "CSendReject::OnChangeInvokeID()\n" );
	m_InvokeID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReject::OnSelchangeRejectCombo() 
{
	TRACE0( "CSendReject::OnSelchangeRejectCombo()\n" );
	m_RejectCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}
