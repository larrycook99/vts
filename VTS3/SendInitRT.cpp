// SendInitRT.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendInitRT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendInitRT::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendInitRT dialog

IMPLEMENT_DYNCREATE( CSendInitRT, CPropertyPage )

CSendInitRT::CSendInitRT( void )
	: CSendPage( CSendInitRT::IDD )
{
	//{{AFX_DATA_INIT(CSendInitRT)
	//}}AFX_DATA_INIT
}

void CSendInitRT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendInitRT)
	DDX_Control(pDX, IDC_DNETLIST, m_DNETList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendInitRT, CPropertyPage)
	//{{AFX_MSG_MAP(CSendInitRT)
	ON_BN_CLICKED(IDC_ADDDNET, OnAddDNET)
	ON_BN_CLICKED(IDC_REMOVEDNET, OnRemoveDNET)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	ON_EN_CHANGE(IDC_PORTID, OnChangePortID)
	ON_EN_CHANGE(IDC_PORTINFO, OnChangePortInfo)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_DNETLIST, OnItemchangingDNETList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendInitRT::InitPage
//

void CSendInitRT::InitPage( void )
{
	TRACE0( "CSendInitRT::InitPage()\n" );

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendInitRT::EncodePage
//

void CSendInitRT::EncodePage( CByteArray* contents )
{
	int			len = m_DNETCtrl.GetItemCount()
	,			dnet, pid, infolen
	;
	const char	*txt
	;
	CByteArray	header
	;

	TRACE0( "CSendInitRT::EncodePage()\n" );

	// encode the message type
	header.Add( 0x06 );

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
//	CSendInitRT::SavePage
//

void CSendInitRT::SavePage( void )
{
	TRACE0( "CSendInitRT::SavePage\n" );

	pageContents.Flush();

	// ### save list contents
}

//
//	CSendInitRT::RestorePage
//

void CSendInitRT::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendInitRT::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore list contents
}

//
//	CSendInitRT::OnInitDialog
//

BOOL CSendInitRT::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "DNET", LVCFMT_RIGHT, 48, IDC_DNET }
		, { "ID", LVCFMT_RIGHT, 48, IDC_PORTID }
		, { "Info", LVCFMT_LEFT, 128, IDC_PORTINFO }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendInitRT::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_DNETList.m_nFlags |= LVS_SINGLESEL;
	m_DNETList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_DNETCtrl.Init( this, &m_DNETList, colDesc );

	return TRUE;
}

void CSendInitRT::OnAddDNET() 
{
	m_DNETCtrl.AddButtonClick();

	// madanner 9/3/02 - initialize defaults DNET=1, port=1 (0=remove)
	m_DNETCtrl.SetCtrlText(IDC_DNET, "1");
	m_DNETCtrl.SetCtrlText(IDC_PORTID, "1");
}

void CSendInitRT::OnRemoveDNET() 
{
	m_DNETCtrl.RemoveButtonClick();
}

void CSendInitRT::OnChangeDNET() 
{
	m_DNETCtrl.OnChangeItem( IDC_DNET );
}

void CSendInitRT::OnChangePortID() 
{
	m_DNETCtrl.OnChangeItem( IDC_PORTID );
}

void CSendInitRT::OnChangePortInfo() 
{
	m_DNETCtrl.OnChangeItem( IDC_PORTINFO );
}

void CSendInitRT::OnItemchangingDNETList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_DNETCtrl.OnItemChanging( pNMHDR, pResult );
}
