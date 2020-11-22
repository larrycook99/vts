// SendRouterAvailable.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendRouterAvailable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendRouterAvailable::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendRouterAvailable dialog

IMPLEMENT_DYNCREATE( CSendRouterAvailable, CPropertyPage )

CSendRouterAvailable::CSendRouterAvailable( void )
	: CSendPage( CSendRouterAvailable::IDD )
{
	//{{AFX_DATA_INIT(CSendRouterAvailable)
	//}}AFX_DATA_INIT
}


void CSendRouterAvailable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendRouterAvailable)
	DDX_Control(pDX, IDC_DNETLIST, m_DNETList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendRouterAvailable, CPropertyPage)
	//{{AFX_MSG_MAP(CSendRouterAvailable)
	ON_BN_CLICKED(IDC_ADDDNET, OnAddDNET)
	ON_BN_CLICKED(IDC_REMOVEDNET, OnRemoveDNET)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_DNETLIST, OnItemchangingDNETList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendRouterAvailable::InitPage
//

void CSendRouterAvailable::InitPage( void )
{
	TRACE0( "CSendRouterAvailable::InitPage()\n" );

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendRouterAvailable::EncodePage
//

void CSendRouterAvailable::EncodePage( CByteArray* contents )
{
	int			len = m_DNETCtrl.GetItemCount()
	;
	CByteArray	header
	;

	TRACE0( "CSendRouterAvailable::EncodePage()\n" );

	// encode the message type
	header.Add( 0x05 );

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
//	CSendRouterAvailable::SavePage
//

void CSendRouterAvailable::SavePage( void )
{
	TRACE0( "CSendRouterAvailable::SavePage\n" );

	pageContents.Flush();

	// ### save the list
}

//
//	CSendRouterAvailable::RestorePage
//

void CSendRouterAvailable::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendRouterAvailable::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore the list
}

//
//	CSendRouterAvailable::OnInitDialog
//

BOOL CSendRouterAvailable::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "DNET", LVCFMT_RIGHT, 96, IDC_DNET }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendRouterAvailable::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_DNETList.m_nFlags |= LVS_SINGLESEL;
	m_DNETList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_DNETCtrl.Init( this, &m_DNETList, colDesc );

	return TRUE;
}

void CSendRouterAvailable::OnAddDNET() 
{
	m_DNETCtrl.AddButtonClick();

	// madanner 9/3/02, added default DNET of 1
	m_DNETCtrl.SetCtrlText(IDC_DNET, "1");
}

void CSendRouterAvailable::OnRemoveDNET() 
{
	m_DNETCtrl.RemoveButtonClick();
}

void CSendRouterAvailable::OnChangeDNET() 
{
	m_DNETCtrl.OnChangeItem( IDC_DNET );
}

void CSendRouterAvailable::OnItemchangingDNETList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_DNETCtrl.OnItemChanging( pNMHDR, pResult );
}
