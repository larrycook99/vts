// SendConfirmedRequest.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "VTSPreferences.h"

#include "Send.h"
#include "SendConfirmedRequest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  Funny lookup table for Maximum APDU Length

char *MaxAPDULen[] =
		{ "50 (MinimumMessageSize)"		// 0
		, "128"							// 1
		, "206 (LonTalk)"				// 2
		, "480 (MS/TP, PTP, ARCNET)"	// 3
#define DEFAULT_MAX_APDU_LEN 3
		, "1024"						// 4
		, "1476 (ISO 8802-3)"			// 5
		, "Reserved (6)"
		, "Reserved (7)"
		, "Reserved (8)"
		, "Reserved (9)"
		, "Reserved (10)"
		, "Reserved (11)"
		, "Reserved (12)"
		, "Reserved (13)"
		, "Reserved (14)"
		, "Reserved (15)"
		};

/* Max_Segments_Accepted
000 unspecified
001 2 segments accepted
010 4 segments accepted
011 8 segments accepted
100 16 segments accepted
101 32 segments accepted
110 64 segments accepted
111 greater than 64 segments accepted
*/

char *MaxSegsAccepted[] =
{ "unspecified" // 0
, "2 segments"  // 1
, "4 segments"  // 2
, "8 segments"  // 3
, "16 segments"  // 4
, "32 segments"  // 5
, "64 segments"  // 6
, "greater than 64"  // 7
};

BACnetAPDUEncoder CSendConfirmedRequest::pageContents;

/////////////////////////////////////////////////////////////////////////////

extern VTSPreferences	gVTSPreferences;

/////////////////////////////////////////////////////////////////////////////
// CSendConfirmedRequest dialog

IMPLEMENT_DYNCREATE( CSendConfirmedRequest, CPropertyPage )

#pragma warning( disable : 4355 )
CSendConfirmedRequest::CSendConfirmedRequest( void )
	: CSendPage( CSendConfirmedRequest::IDD )
	, m_SegmentSize( this, IDC_SEGMENTSIZE )
	, m_SeqNumber( this, IDC_SEQNUMBER )
	, m_WindowSize( this, IDC_WINDOWSIZE )
	, m_MaxAPDULen( this, IDC_MAXAPDULEN, MaxAPDULen, 16, true )
	, m_InvokeID( this, IDC_INVOKEID )
	, m_Max_Segments_Accepted(this, IDC_MAX_SEGMENTS_ACCEPTED, MaxSegsAccepted, 8, true )
{
	//{{AFX_DATA_INIT(CSendConfirmedRequest)
	m_Segmented = FALSE;
	m_MoreFollows = FALSE;
	m_SegResponse = FALSE;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendConfirmedRequest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendConfirmedRequest)
	DDX_Check(pDX, IDC_SEGMENTED, m_Segmented);
	DDX_Check(pDX, IDC_MOREFOLLOWS, m_MoreFollows);
	DDX_Check(pDX, IDC_SEGRESPONSE, m_SegResponse);
	//}}AFX_DATA_MAP

	m_SegmentSize.UpdateData( pDX->m_bSaveAndValidate );
	m_SeqNumber.UpdateData( pDX->m_bSaveAndValidate );
	m_WindowSize.UpdateData( pDX->m_bSaveAndValidate );
	m_MaxAPDULen.UpdateData( pDX->m_bSaveAndValidate );
	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
	m_Max_Segments_Accepted.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendConfirmedRequest, CPropertyPage)
	//{{AFX_MSG_MAP(CSendConfirmedRequest)
	ON_EN_CHANGE(IDC_SEGMENTSIZE, OnChangeSegmentSize)
	ON_BN_CLICKED(IDC_SEGMENTED, OnSegmented)
	ON_BN_CLICKED(IDC_MOREFOLLOWS, OnMoreFollows)
	ON_BN_CLICKED(IDC_SEGRESPONSE, OnSegResponse)
	ON_EN_CHANGE(IDC_SEQNUMBER, OnChangeSeqNumber)
	ON_EN_CHANGE(IDC_WINDOWSIZE, OnChangeWindowSize)
	ON_EN_CHANGE(IDC_MAXAPDULEN, OnChangeMaxAPDULen)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	ON_CBN_SELCHANGE(IDC_MAXAPDULEN, OnSelchangeMaxAPDULen)
	ON_CBN_SELCHANGE(IDC_MAX_SEGMENTS_ACCEPTED, OnSelchangeMaxSegmentsAccepted)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendConfirmedRequest message handlers

//
//	CSendConfirmedRequest::OnInitDialog
//

BOOL CSendConfirmedRequest::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// load the enumeration table
	m_MaxAPDULen.LoadCombo();
	m_Max_Segments_Accepted.LoadCombo();

	// humm
	SynchronizeControls();

	return TRUE;
}
//
//	CSendConfirmedRequest::SynchronizeControls
//

void CSendConfirmedRequest::SynchronizeControls()
{
	// segmentation controls
	GetDlgItem( IDC_MOREFOLLOWS )->EnableWindow( m_Segmented );
	GetDlgItem( IDC_SEQNUMBER )->EnableWindow( m_Segmented );
	GetDlgItem( IDC_WINDOWSIZE )->EnableWindow( m_Segmented );
// Not sure if we should reset the value of m_Max_Segments_Accepted here
	if ( !m_SegResponse )
		m_Max_Segments_Accepted.m_enumValue = 0;
	GetDlgItem( IDC_MAX_SEGMENTS_ACCEPTED )->EnableWindow( m_SegResponse );
}

//
//	CSendConfirmedRequest::InitPage
//

void CSendConfirmedRequest::InitPage( void )
{
	TRACE0( "CSendConfirmedRequest::InitPage()\n" );

	// flush the fields
	m_SegmentSize.ctrlNull = true;
	m_SeqNumber.ctrlNull = true;
	m_WindowSize.ctrlNull = true;
	m_MaxAPDULen.ctrlNull = false;
	m_MaxAPDULen.m_enumValue = DEFAULT_MAX_APDU_LEN;
	m_InvokeID.ctrlNull = false;
	m_InvokeID.intValue = gVTSPreferences.Send_GetInvokeID();
	m_Max_Segments_Accepted.m_enumValue = 0;
	// tell the NPCI this is expecting a reply
	pageParent->NPCIPage.m_ExpectingReply = true;
}

//
//	CSendConfirmedRequest::EncodePage
//

void CSendConfirmedRequest::EncodePage( CByteArray *contents )
{
	char		segCountMsg[64]
	;
	BACnetOctet	ctrl = 0
	;
	CByteArray	header
	;

	TRACE0( "CSendConfirmedRequest::EncodePage()\n" );

	// assume no segment count
	segCountMsg[0] = 0;

	// first octet
	header.Add( 0x00
		+ (m_Segmented ? 0x08 : 0)
		+ (m_MoreFollows ? 0x04 : 0)
		+ (m_SegResponse ? 0x02 : 0)
		);

	// maximum response accepted
	if (m_MaxAPDULen.ctrlNull)
		throw "Max APDU length required";
	if ((m_MaxAPDULen.m_enumValue < 0) || (m_MaxAPDULen.m_enumValue > 15))
		throw "Max APDU length out of range 0..15";

	int test = (m_Max_Segments_Accepted.m_enumValue <<4) + m_MaxAPDULen.m_enumValue;
	header.Add( test );

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
		if (::IsWindow(m_hWnd)) GetDlgItem( IDC_SEGCOUNTMSG )->SetWindowText( segCountMsg );

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
			throw "Uncheck 'more follows' for last segment";
		if ((m_SeqNumber.intValue < (segmentCount - 1)) && !m_MoreFollows)
			throw "Check 'more follows' except for last segment";

		// delete stuff from front
		contents->RemoveAt( 0, m_SeqNumber.intValue * segSize );

		// delete stuff off the end (if there is any)
		if (contents->GetSize() > segSize)
			contents->RemoveAt( segSize, contents->GetSize() - segSize );
	} else
		if (::IsWindow(m_hWnd)) GetDlgItem( IDC_SEGCOUNTMSG )->SetWindowText( "" );

	// stuff header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendConfirmedRequest::SavePage
//

void CSendConfirmedRequest::SavePage( void )
{
	TRACE0( "CSendConfirmedRequest::SavePage\n" );

	pageContents.Flush();

	m_SegmentSize.SaveCtrl( pageContents );
	m_SeqNumber.SaveCtrl( pageContents );
	m_WindowSize.SaveCtrl( pageContents );
	m_MaxAPDULen.SaveCtrl( pageContents );
	m_InvokeID.SaveCtrl( pageContents );
	m_Max_Segments_Accepted.SaveCtrl( pageContents );

	BACnetBoolean( m_Segmented ).Encode( pageContents );
	BACnetBoolean( m_MoreFollows ).Encode( pageContents );
	BACnetBoolean( m_SegResponse ).Encode( pageContents );
}

//
//	CSendConfirmedRequest::RestorePage
//

void CSendConfirmedRequest::RestorePage(  int index  )
{
	BACnetAPDUDecoder	dec( pageContents )
	;
	BACnetBoolean		boolValue
	;

	TRACE0( "CSendConfirmedRequest::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_SegmentSize.RestoreCtrl( dec );
	m_SeqNumber.RestoreCtrl( dec );
	m_WindowSize.RestoreCtrl( dec );
	m_MaxAPDULen.RestoreCtrl( dec );
	m_InvokeID.RestoreCtrl( dec );
	m_Max_Segments_Accepted.RestoreCtrl( dec );

	boolValue.Decode( dec );
	m_Segmented = boolValue.boolValue;

	boolValue.Decode( dec );
	m_MoreFollows = boolValue.boolValue;

	boolValue.Decode( dec );
	m_SegResponse = boolValue.boolValue;
}

void CSendConfirmedRequest::OnChangeSegmentSize() 
{
	TRACE0( "CSendConfirmedRequest::OnChangeSegmentSize()\n" );
	m_SegmentSize.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfirmedRequest::OnSegmented()
{
	TRACE0( "CSendConfirmedRequest::OnSegmented()\n" );
	UpdateData();
	SynchronizeControls();
	SavePage();
	UpdateEncoded();
}

void CSendConfirmedRequest::OnMoreFollows()
{
	TRACE0( "CSendConfirmedRequest::OnMoreFollows()\n" );
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfirmedRequest::OnSegResponse()
{
	TRACE0( "CSendConfirmedRequest::OnSegResponse()\n" );
	UpdateData();
	SynchronizeControls();
	SavePage();
	UpdateEncoded();
}

void CSendConfirmedRequest::OnChangeSeqNumber() 
{
	TRACE0( "CSendConfirmedRequest::OnChangeSeqNumber()\n" );
	m_SeqNumber.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfirmedRequest::OnChangeWindowSize() 
{
	TRACE0( "CSendConfirmedRequest::OnChangeWindowSize()\n" );
	m_WindowSize.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfirmedRequest::OnChangeMaxAPDULen() 
{
	TRACE0( "CSendConfirmedRequest::OnChangeMaxAPDULen()\n" );
	m_MaxAPDULen.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfirmedRequest::OnSelchangeMaxAPDULen() 
{
	m_MaxAPDULen.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfirmedRequest::OnChangeInvokeID() 
{
	TRACE0( "CSendConfirmedRequest::OnChangeInvokeID()\n" );
	m_InvokeID.UpdateData();

	// save the new invoke ID in the preferences
	gVTSPreferences.Send_SetInvokeID(m_InvokeID.intValue);

	SavePage();
	UpdateEncoded();
}


void CSendConfirmedRequest::OnSelchangeMaxSegmentsAccepted() 
{
	TRACE0( "CSendConfirmedRequest::OnSelchangeMaxSegmentsAccepted()\n" );
	m_Max_Segments_Accepted.UpdateData();
	SavePage();
	UpdateEncoded();
}
