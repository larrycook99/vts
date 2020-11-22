// SendSegmentACK.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendSegmentACK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendSegmentACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendSegmentACK property page

IMPLEMENT_DYNCREATE(CSendSegmentACK, CPropertyPage)

#pragma warning( disable : 4355 )
CSendSegmentACK::CSendSegmentACK( void )
	: CSendPage( CSendSegmentACK::IDD )
	, m_InvokeID( this, IDC_INVOKEID )
	, m_SeqNum( this, IDC_SEQNUM )
	, m_WindowSize( this, IDC_WINDOWSIZE )
{
	//{{AFX_DATA_INIT(CSendSegmentACK)
	m_NAK = FALSE;
	m_SRV = 0;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendSegmentACK::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendSegmentACK)
	DDX_Check(pDX, IDC_NAK, m_NAK);
	DDX_Radio(pDX, IDC_CLIENT, m_SRV);
	//}}AFX_DATA_MAP

	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
	m_SeqNum.UpdateData( pDX->m_bSaveAndValidate );
	m_WindowSize.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendSegmentACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendSegmentACK)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	ON_EN_CHANGE(IDC_SEQNUM, OnChangeSeqNum)
	ON_EN_CHANGE(IDC_WINDOWSIZE, OnChangeWindowSize)
	ON_BN_CLICKED(IDC_NAK, OnNAK)
	ON_BN_CLICKED(IDC_CLIENT, OnClient)
	ON_BN_CLICKED(IDC_SERVER, OnServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendSegmentACK::OnInitDialog
//

BOOL CSendSegmentACK::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendSegmentACK::InitPage
//

void CSendSegmentACK::InitPage( void )
{
	TRACE0( "CSendSegmentACK::InitPage()\n" );

	// flush the data
	m_InvokeID.ctrlNull = true;
	m_SeqNum.ctrlNull = true;
	m_WindowSize.ctrlNull = true;
}

//
//	CSendSegmentACK::EncodePage
//

void CSendSegmentACK::EncodePage( CByteArray* contents )
{
	BACnetOctet	pduType
	;
	CByteArray	header
	;

	TRACE0( "CSendSegmentACK::EncodePage()\n" );

	// encode the pdu type
	pduType = 0x40 + (m_NAK ? 0x02 : 0) + (m_SRV ? 0x01 : 0);
	header.Add( pduType );

	// encode the invoke ID
	if (m_InvokeID.ctrlNull)
		throw "Original invoke ID required";
	if ((m_InvokeID.intValue < 0) || (m_InvokeID.intValue > 255))
		throw "Original invoke ID out of range 0..255";
	header.Add( m_InvokeID.intValue );

	// encode the sequence number
	if (m_SeqNum.ctrlNull)
		throw "Sequence Number required";
	if ((m_SeqNum.intValue < 0) || (m_SeqNum.intValue > 255))
		throw "Sequence number out of range 0..255";
	header.Add( m_SeqNum.intValue );

	// encode the invoke ID
	if (m_WindowSize.ctrlNull)
		throw "Actual window size required";
	if ((m_WindowSize.intValue < 1) || (m_WindowSize.intValue > 127))
		throw "Actual window size out of range 1..127";
	header.Add( m_WindowSize.intValue );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendSegmentACK::SavePage
//

void CSendSegmentACK::SavePage( void )
{
	TRACE0( "CSendSegmentACK::SavePage\n" );

	pageContents.Flush();

	BACnetInteger( m_NAK ).Encode( pageContents );
	BACnetInteger( m_SRV ).Encode( pageContents );
	m_InvokeID.SaveCtrl( pageContents );
	m_SeqNum.SaveCtrl( pageContents );
	m_WindowSize.SaveCtrl( pageContents );
}

//
//	CSendSegmentACK::RestorePage
//

void CSendSegmentACK::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;
	BACnetInteger		intValue
	;

	TRACE0( "CSendSegmentACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	intValue.Decode( dec );
	m_NAK = intValue.intValue;

	intValue.Decode( dec );
	m_SRV = intValue.intValue;

	m_InvokeID.RestoreCtrl( dec );
	m_SeqNum.RestoreCtrl( dec );
	m_WindowSize.RestoreCtrl( dec );
}

//
//	CSendSegmentACK::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendSegmentACK::OnChangeInvokeID()
{
	TRACE0( "CSendSegmentACK::OnChangeInvokeID()\n" );
	m_InvokeID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSegmentACK::OnChangeSeqNum()
{
	TRACE0( "CSendSegmentACK::OnChangeSeqNum()\n" );
	m_SeqNum.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSegmentACK::OnChangeWindowSize()
{
	TRACE0( "CSendSegmentACK::OnChangeWindowSize()\n" );
	m_WindowSize.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSegmentACK::OnNAK() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSegmentACK::OnClient() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSegmentACK::OnServer() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}
