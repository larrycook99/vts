// SendComplexACK.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendComplexACK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendComplexACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendComplexACK dialog

IMPLEMENT_DYNCREATE( CSendComplexACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendComplexACK::CSendComplexACK( void )
	: CSendPage( CSendComplexACK::IDD )
	, m_SegmentSize( this, IDC_SEGMENTSIZE )
	, m_SeqNumber( this, IDC_SEQNUMBER )
	, m_WindowSize( this, IDC_WINDOWSIZE )
	, m_InvokeID( this, IDC_INVOKEID )
{
	//{{AFX_DATA_INIT(CSendComplexACK)
	m_Segmented = FALSE;
	m_MoreFollows = FALSE;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendComplexACK::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendComplexACK)
	DDX_Check(pDX, IDC_SEGMENTED, m_Segmented);
	DDX_Check(pDX, IDC_MOREFOLLOWS, m_MoreFollows);
	//}}AFX_DATA_MAP

	m_SegmentSize.UpdateData( pDX->m_bSaveAndValidate );
	m_SeqNumber.UpdateData( pDX->m_bSaveAndValidate );
	m_WindowSize.UpdateData( pDX->m_bSaveAndValidate );
	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendComplexACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendComplexACK)
	ON_EN_CHANGE(IDC_SEGMENTSIZE, OnChangeSegmentSize)
	ON_BN_CLICKED(IDC_SEGMENTED, OnSegmented)
	ON_BN_CLICKED(IDC_MOREFOLLOWS, OnMoreFollows)
	ON_EN_CHANGE(IDC_SEQNUMBER, OnChangeSeqNumber)
	ON_EN_CHANGE(IDC_WINDOWSIZE, OnChangeWindowSize)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendComplexACK message handlers

//
//	CSendComplexACK::OnInitDialog
//

BOOL CSendComplexACK::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// humm
	SynchronizeControls();

	return TRUE;
}
//
//	CSendComplexACK::SynchronizeControls
//

void CSendComplexACK::SynchronizeControls()
{
	// segmentation controls
	GetDlgItem( IDC_MOREFOLLOWS )->EnableWindow( m_Segmented );
	GetDlgItem( IDC_SEQNUMBER )->EnableWindow( m_Segmented );
	GetDlgItem( IDC_WINDOWSIZE )->EnableWindow( m_Segmented );
}

//
//	CSendComplexACK::InitPage
//

void CSendComplexACK::InitPage( void )
{
	TRACE0( "CSendComplexACK::InitPage()\n" );

	// flush the fields
	m_SegmentSize.ctrlNull = true;
	m_SeqNumber.ctrlNull = true;
	m_WindowSize.ctrlNull = true;
	m_InvokeID.ctrlNull = true;
}

//
//	CSendComplexACK::EncodePage
//

void CSendComplexACK::EncodePage( CByteArray *contents )
{
	char		segCountMsg[64]
	;
	BACnetOctet	ctrl = 0
	;
	CByteArray	header
	;

	TRACE0( "CSendComplexACK::EncodePage()\n" );

	// assume no segment count
	segCountMsg[0] = 0;

	// first octet
	header.Add( 0x30
		+ (m_Segmented ? 0x08 : 0)
		+ (m_MoreFollows ? 0x04 : 0)
		);

	// invoke ID
	if (m_InvokeID.ctrlNull)
		throw "Invoke ID required";
	if ((m_InvokeID.intValue < 0) || (m_InvokeID.intValue > 255))
		throw "Invoke ID out of range 0..255";
	header.Add( m_InvokeID.intValue );

	// segmentation information
	if (m_Segmented) {
		// sequence number
		if (m_SeqNumber.ctrlNull)
			throw "Sequence number required";
		if ((m_SeqNumber.intValue < 0) || (m_SeqNumber.intValue > 255))
			throw "Sequence number out of range 0..255";
		header.Add( m_SeqNumber.intValue );

		// proposed window size
		if (m_WindowSize.ctrlNull)
			throw "Proposed window size required";
		if ((m_WindowSize.intValue < 0) || (m_WindowSize.intValue > 255))
			throw "Proposed window size out of range 0..255";
		header.Add( m_WindowSize.intValue );
	}

	// check to segment message
	if (!m_SegmentSize.ctrlNull) {
		int segSize = m_SegmentSize.intValue;

		// make sure the user is hacking up into non-zero length chunks
		if (segSize <= 0)
			throw "Segment size must be greater than zero";

		// calc how many segments there are
		int segmentCount = (contents->GetSize() + segSize - 1) / segSize;
		if (segmentCount == 1)
			strcpy( segCountMsg, "1 segment" );
		else
			sprintf( segCountMsg, "%d segments", segmentCount );
		GetDlgItem( IDC_SEGCOUNTMSG )->SetWindowText( segCountMsg );

		// if the segment count is one, this doesn't need segmentation
		if ((segmentCount == 1) && m_Segmented)
			throw "APDU fits in one segment, segmentation not necessary";
		if ((segmentCount > 1) && !m_Segmented)
			throw "APDU requires more than one segment";

		// make sure the segment number isn't more than what is necessary
		if (m_SeqNumber.intValue >= segmentCount) {
			char msg[60];
			sprintf( msg, "Sequence number out of range 0..%d", segmentCount-1 );
			throw msg;
		}

		// check more follows
		if ((m_SeqNumber.intValue == (segmentCount - 1)) && m_MoreFollows)
			throw "No more follows for last segment";
		if ((m_SeqNumber.intValue < (segmentCount - 1)) && !m_MoreFollows)
			throw "More follows for non-last segment";

		// delete stuff from front
		contents->RemoveAt( 0, m_SeqNumber.intValue * segSize );

		// delete stuff off the end (if there is any)
		if (contents->GetSize() > segSize)
			contents->RemoveAt( segSize, contents->GetSize() - segSize );
	} else
		GetDlgItem( IDC_SEGCOUNTMSG )->SetWindowText( "" );

	// stuff header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendComplexACK::SavePage
//

void CSendComplexACK::SavePage( void )
{
	TRACE0( "CSendComplexACK::SavePage\n" );

	pageContents.Flush();

	m_SegmentSize.SaveCtrl( pageContents );
	m_SeqNumber.SaveCtrl( pageContents );
	m_WindowSize.SaveCtrl( pageContents );
	m_InvokeID.SaveCtrl( pageContents );

	BACnetBoolean( m_Segmented ).Encode( pageContents );
	BACnetBoolean( m_MoreFollows ).Encode( pageContents );
}

//
//	CSendComplexACK::RestorePage
//

void CSendComplexACK::RestorePage(  int index  )
{
	BACnetAPDUDecoder	dec( pageContents )
	;
	BACnetBoolean		boolValue
	;

	TRACE0( "CSendComplexACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_SegmentSize.RestoreCtrl( dec );
	m_SeqNumber.RestoreCtrl( dec );
	m_WindowSize.RestoreCtrl( dec );
	m_InvokeID.RestoreCtrl( dec );

	boolValue.Decode( dec );
	m_Segmented = boolValue.boolValue;

	boolValue.Decode( dec );
	m_MoreFollows = boolValue.boolValue;
}

void CSendComplexACK::OnChangeSegmentSize() 
{
	TRACE0( "CSendComplexACK::OnChangeSegmentSize()\n" );
	m_SegmentSize.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendComplexACK::OnSegmented()
{
	TRACE0( "CSendComplexACK::OnSegmented()\n" );
	UpdateData();
	SynchronizeControls();
	SavePage();
	UpdateEncoded();
}

void CSendComplexACK::OnMoreFollows()
{
	TRACE0( "CSendComplexACK::OnMoreFollows()\n" );
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendComplexACK::OnChangeSeqNumber() 
{
	TRACE0( "CSendComplexACK::OnChangeSeqNumber()\n" );
	m_SeqNumber.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendComplexACK::OnChangeWindowSize() 
{
	TRACE0( "CSendComplexACK::OnChangeWindowSize()\n" );
	m_WindowSize.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendComplexACK::OnChangeInvokeID() 
{
	TRACE0( "CSendComplexACK::OnChangeInvokeID()\n" );
	m_InvokeID.UpdateData();
	SavePage();
	UpdateEncoded();
}
