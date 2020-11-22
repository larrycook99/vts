// SendRouterBusy.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendRouterBusy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendRouterBusy::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendRouterBusy dialog

IMPLEMENT_DYNCREATE( CSendRouterBusy, CPropertyPage )

CSendRouterBusy::CSendRouterBusy( void )
	: CSendPage( CSendRouterBusy::IDD )
{
	//{{AFX_DATA_INIT(CSendRouterBusy)
	//}}AFX_DATA_INIT
}


void CSendRouterBusy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendRouterBusy)
	DDX_Control(pDX, IDC_DNETLIST, m_DNETList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendRouterBusy, CPropertyPage)
	//{{AFX_MSG_MAP(CSendRouterBusy)
	ON_BN_CLICKED(IDC_ADDDNET, OnAddDNET)
	ON_BN_CLICKED(IDC_REMOVEDNET, OnRemoveDNET)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_DNETLIST, OnItemchangingDNETList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendRouterBusy::InitPage
//

void CSendRouterBusy::InitPage( void )
{
	TRACE0( "CSendRouterBusy::InitPage()\n" );

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendRouterBusy::EncodePage
//

void CSendRouterBusy::EncodePage( CByteArray* contents )
{
	int			len = m_DNETCtrl.GetItemCount()
	;
	CByteArray	header
	;

	TRACE0( "CSendRouterBusy::EncodePage()\n" );

	// encode the message type
	header.Add( 0x04 );

	// validate and encode the DNET's
	for (int i = 0; i < len; i++) {
		const char *txt = m_DNETCtrl.GetItemText( i, 0 );

		// make sure something was provided
		if (!txt || !*txt)
			throw "Invalid DNET";

		// convert to an integer and check the range
		int dnet = atoi( txt );
		if ((dnet < 0) || (dnet > 65534))
			throw "DNET out of range 0..65534";

		// encode it
		header.Add( dnet >> 8 );
		header.Add( dnet & 0xFF );
	}

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendRouterBusy::SavePage
//

void CSendRouterBusy::SavePage( void )
{
	TRACE0( "CSendRouterBusy::SavePage\n" );

	pageContents.Flush();

	// ### save the list
}

//
//	CSendRouterBusy::RestorePage
//

void CSendRouterBusy::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendRouterBusy::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore the list
}

//
//	CSendRouterBusy::OnInitDialog
//

BOOL CSendRouterBusy::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "DNET", LVCFMT_RIGHT, 96, IDC_DNET }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendRouterBusy::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_DNETList.m_nFlags |= LVS_SINGLESEL;
	m_DNETList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_DNETCtrl.Init( this, &m_DNETList, colDesc );

	return TRUE;
}

void CSendRouterBusy::OnAddDNET() 
{
	m_DNETCtrl.AddButtonClick();

	// madanner 9/3/02, added default DNET of 1
	m_DNETCtrl.SetCtrlText(IDC_DNET, "1");
}

void CSendRouterBusy::OnRemoveDNET() 
{
	m_DNETCtrl.RemoveButtonClick();
}

void CSendRouterBusy::OnChangeDNET() 
{
	m_DNETCtrl.OnChangeItem( IDC_DNET );
}

void CSendRouterBusy::OnItemchangingDNETList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_DNETCtrl.OnItemChanging( pNMHDR, pResult );
}
