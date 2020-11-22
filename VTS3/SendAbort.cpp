// SendAbort.cpp : implementation file
//
// MAG 16AUG05 add 'Client' and 'Server' buttons and related code for SF bug #1167286

#include "stdafx.h"
#include "VTS.h"
#include "SendAbort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder	CSendAbort::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendAbort dialog

IMPLEMENT_DYNCREATE( CSendAbort, CPropertyPage )

#pragma warning( disable : 4355 )
CSendAbort::CSendAbort( void )
	: CSendPage( CSendAbort::IDD )
	, m_InvokeID( this, IDC_INVOKEID )
	, m_AbortCombo( this, IDC_ABORTCOMBO, NetworkSniffer::BAC_STRTAB_BACnetAbort, true )
{
	//{{AFX_DATA_INIT(CSendAbort)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendAbort::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendAbort::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendAbort)
//	DDX_Radio(pDX, IDC_CLIENT, m_SRV);
	//}}AFX_DATA_MAP

	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
	m_AbortCombo.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendAbort, CPropertyPage)
	//{{AFX_MSG_MAP(CSendAbort)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	ON_CBN_SELCHANGE(IDC_ABORTCOMBO, OnSelchangeAbortCombo)
	ON_BN_CLICKED(IDC_CLIENT, OnClient)
	ON_BN_CLICKED(IDC_SERVER, OnServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendAbort::OnInitDialog
//

BOOL CSendAbort::OnInitDialog() 
{
	TRACE0( "CSendAbort::OnInitDialog()\n" );

	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_AbortCombo.LoadCombo();
	CButton *cb;
	cb = (CButton *) GetDlgItem(IDC_CLIENT);
	cb->SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendAbort::InitPage
//

void CSendAbort::InitPage( void )
{
	TRACE0( "CSendAbort::InitPage()\n" );

	// flush the data
	m_InvokeID.ctrlNull = true;
	m_AbortCombo.ctrlNull = true;
}

//
//	CSendAbort::EncodePage
//

void CSendAbort::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;
	CButton *cb;
	unsigned char uc;

	TRACE0( "CSendAbort::EncodePage()\n" );

	cb = (CButton *) GetDlgItem(IDC_CLIENT);
	if((cb == NULL)||(cb->GetCheck() )) uc = 0;
	else uc = 1;
	// encode the pdu type
	header.Add( 0x70 + uc);

	// encode the invoke ID
	if (m_InvokeID.ctrlNull)
		throw "Original invoke ID required";
	if ((m_InvokeID.intValue < 0) || (m_InvokeID.intValue > 255))
		throw "Original invoke ID out of range 0..255";
	header.Add( m_InvokeID.intValue );

	// encode the service choice
	if (m_AbortCombo.ctrlNull)
		throw "Abort reason required";
	header.Add( m_AbortCombo.m_enumValue );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendAbort::SavePage
//

void CSendAbort::SavePage( void )
{
	TRACE0( "CSendAbort::SavePage\n" );

	pageContents.Flush();

	m_InvokeID.SaveCtrl( pageContents );
	m_AbortCombo.SaveCtrl( pageContents );
}

//
//	CSendAbort::RestorePage
//

void CSendAbort::RestorePage(  int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendAbort::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_InvokeID.RestoreCtrl( dec );
	m_AbortCombo.RestoreCtrl( dec );
}

//
//	CSendAbort::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendAbort::OnChangeInvokeID()
{
	TRACE0( "CSendAbort::OnChangeInvokeID()\n" );
	m_InvokeID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendAbort::OnSelchangeAbortCombo() 
{
	TRACE0( "CSendAbort::OnSelchangeAbortCombo()\n" );
	m_AbortCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

// MAG 16AUG05 add client and server button subroutines
void CSendAbort::OnClient() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendAbort::OnServer() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}
