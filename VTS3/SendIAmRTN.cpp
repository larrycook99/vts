// SendIAmRTN.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendIAmRTN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendIAmRTN::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendIAmRTN dialog

IMPLEMENT_DYNCREATE( CSendIAmRTN, CPropertyPage )

CSendIAmRTN::CSendIAmRTN( void )
	: CSendPage( CSendIAmRTN::IDD )
{
	//{{AFX_DATA_INIT(CSendIAmRTN)
	//}}AFX_DATA_INIT
}


void CSendIAmRTN::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendIAmRTN)
	DDX_Control(pDX, IDC_DNETLIST, m_DNETList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendIAmRTN, CPropertyPage)
	//{{AFX_MSG_MAP(CSendIAmRTN)
	ON_BN_CLICKED(IDC_ADDDNET, OnAddDNET)
	ON_BN_CLICKED(IDC_REMOVEDNET, OnRemoveDNET)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_DNETLIST, OnItemchangingDNETList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendIAmRTN::InitPage
//

void CSendIAmRTN::InitPage( void )
{
	TRACE0( "CSendIAmRTN::InitPage()\n" );

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendIAmRTN::EncodePage
//

void CSendIAmRTN::EncodePage( CByteArray* contents )
{
	int			len = m_DNETCtrl.GetItemCount()
	;
	CByteArray	header
	;

	TRACE0( "CSendIAmRTN::EncodePage()\n" );

	// encode the message type
	header.Add( 0x01 );

	// validate DNET list
	if (len == 0)
		throw "At least one DNET required";

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
//	CSendIAmRTN::OnInitDialog
//

BOOL CSendIAmRTN::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "DNET", LVCFMT_RIGHT, 96, IDC_DNET }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendIAmRTN::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_DNETList.m_nFlags |= LVS_SINGLESEL;
	m_DNETList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_DNETCtrl.Init( this, &m_DNETList, colDesc );

	return TRUE;
}

//
//	CSendIAmRTN::SavePage
//

void CSendIAmRTN::SavePage( void )
{
	TRACE0( "CSendIAmRTN::SavePage\n" );

	pageContents.Flush();

	// ### save the value list
}

//
//	CSendIAmRTN::RestorePage
//

void CSendIAmRTN::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendIAmRTN::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore the DNET list
}

void CSendIAmRTN::OnAddDNET() 
{
	m_DNETCtrl.AddButtonClick();

	// madanner 9/3/02, added default DNET of 1
	m_DNETCtrl.SetCtrlText(IDC_DNET, "1");
}

void CSendIAmRTN::OnRemoveDNET() 
{
	m_DNETCtrl.RemoveButtonClick();
}

void CSendIAmRTN::OnChangeDNET() 
{
	m_DNETCtrl.OnChangeItem( IDC_DNET );
}

void CSendIAmRTN::OnItemchangingDNETList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_DNETCtrl.OnItemChanging( pNMHDR, pResult );
}
