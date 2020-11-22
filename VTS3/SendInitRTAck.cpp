// SendInitRTAck.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendInitRTAck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendInitRTAck::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendInitRTAck dialog

IMPLEMENT_DYNCREATE( CSendInitRTAck, CPropertyPage )

CSendInitRTAck::CSendInitRTAck( void )
	: CSendPage( CSendInitRTAck::IDD )
{
	//{{AFX_DATA_INIT(CSendInitRTAck)
	//}}AFX_DATA_INIT
}

void CSendInitRTAck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendInitRTAck)
	DDX_Control(pDX, IDC_DNETLIST, m_DNETList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendInitRTAck, CPropertyPage)
	//{{AFX_MSG_MAP(CSendInitRTAck)
	ON_BN_CLICKED(IDC_ADDDNET, OnAddDNET)
	ON_BN_CLICKED(IDC_REMOVEDNET, OnRemoveDNET)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	ON_EN_CHANGE(IDC_PORTID, OnChangePortID)
	ON_EN_CHANGE(IDC_PORTINFO, OnChangePortInfo)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_DNETLIST, OnItemchangingDNETList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendInitRTAck::InitPage
//

void CSendInitRTAck::InitPage( void )
{
	TRACE0( "CSendInitRTAck::InitPage()\n" );

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendInitRTAck::EncodePage
//

void CSendInitRTAck::EncodePage( CByteArray* contents )
{
	int			len = m_DNETCtrl.GetItemCount()
	,			dnet, pid, infolen
	;
	const char	*txt
	;
	CByteArray	header
	;

	TRACE0( "CSendInitRTAck::EncodePage()\n" );

	// encode the message type
	header.Add( 0x07 );

	// encode the list length
	header.Add( len );

	// validate and encode the routing table information
	for (int i = 0; i < len; i++) {
		// make sure a DNET was provided
		txt = m_DNETCtrl.GetItemText( i, 0 );
		if (!txt || !*txt)
			throw "Invalid DNET";

		// convert to an integer and check the range
		dnet = atoi( txt );
		if ((dnet < 0) || (dnet > 65534))
			throw "DNET out of range 0..65534";

		// encode it
		header.Add( dnet >> 8 );
		header.Add( dnet & 0xFF );

		// make sure a port ID was provided
		txt = m_DNETCtrl.GetItemText( i, 1 );
		if (!txt || !*txt)
			throw "Invalid port ID";

		// convert to an integer and check the range
		pid = atoi( txt );
		if ((pid < 0) || (pid > 255))
			throw "Port ID out of range 0..255";

		// encode it
		header.Add( pid );

		// see if some info was provided
		txt = m_DNETCtrl.GetItemText( i, 2 );
		if (!txt || !*txt)
			header.Add( 0x00 );
		else {
			infolen = strlen( txt );
			if (infolen > 255)
				throw "Port information too long (255 max)";

			header.Add( infolen );
			for (int j = 0; j < infolen; j++)
				header.Add( (BYTE)txt[j] );
		}
	}

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendInitRTAck::SavePage
//

void CSendInitRTAck::SavePage( void )
{
	TRACE0( "CSendInitRTAck::SavePage\n" );

	pageContents.Flush();

	// ### save list contents
}

//
//	CSendInitRTAck::RestorePage
//

void CSendInitRTAck::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendInitRTAck::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore list contents
}

//
//	CSendInitRTAck::OnInitDialog
//

BOOL CSendInitRTAck::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "DNET", LVCFMT_RIGHT, 48, IDC_DNET }
		, { "ID", LVCFMT_RIGHT, 48, IDC_PORTID }
		, { "Info", LVCFMT_LEFT, 128, IDC_PORTINFO }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendInitRTAck::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_DNETList.m_nFlags |= LVS_SINGLESEL;
	m_DNETList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_DNETCtrl.Init( this, &m_DNETList, colDesc );

	return TRUE;
}

void CSendInitRTAck::OnAddDNET() 
{
	m_DNETCtrl.AddButtonClick();

	// madanner 9/3/02 - initialize defaults DNET=1, port=1 (0=remove)
	m_DNETCtrl.SetCtrlText(IDC_DNET, "1");
	m_DNETCtrl.SetCtrlText(IDC_PORTID, "1");
}

void CSendInitRTAck::OnRemoveDNET() 
{
	m_DNETCtrl.RemoveButtonClick();
}

void CSendInitRTAck::OnChangeDNET() 
{
	m_DNETCtrl.OnChangeItem( IDC_DNET );
}

void CSendInitRTAck::OnChangePortID() 
{
	m_DNETCtrl.OnChangeItem( IDC_PORTID );
}

void CSendInitRTAck::OnChangePortInfo() 
{
	m_DNETCtrl.OnChangeItem( IDC_PORTINFO );
}

void CSendInitRTAck::OnItemchangingDNETList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_DNETCtrl.OnItemChanging( pNMHDR, pResult );
}
