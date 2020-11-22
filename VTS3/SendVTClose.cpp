// SendVTClose.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendVTClose.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendVTClose::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendVTClose dialog

IMPLEMENT_DYNCREATE( CSendVTClose, CPropertyPage )

CSendVTClose::CSendVTClose( void )
	: CSendPage( CSendVTClose::IDD )
{
	//{{AFX_DATA_INIT(CSendVTClose)
	//}}AFX_DATA_INIT
}

void CSendVTClose::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendVTClose::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendVTClose)
	DDX_Control(pDX, IDC_IDLIST, m_IDList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendVTClose, CPropertyPage)
	//{{AFX_MSG_MAP(CSendVTClose)
	ON_BN_CLICKED(IDC_ADDID, OnAddID)
	ON_BN_CLICKED(IDC_REMOVEID, OnRemoveID)
	ON_EN_CHANGE(IDC_ID, OnChangeID)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_IDLIST, OnItemchangingIDList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendVTClose::OnInitDialog
//

BOOL CSendVTClose::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "ID", LVCFMT_RIGHT, 96, IDC_ID }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendVTClose::OnInitDialog()\n" );

	CDialog::OnInitDialog();

	// only allow one selection at a time, no sorting
	m_IDList.m_nFlags |= LVS_SINGLESEL;
	m_IDList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_IDCtrl.Init( this, &m_IDList, colDesc );

	return TRUE;
}

//
//	CSendVTClose::InitPage
//

void CSendVTClose::InitPage( void )
{
	TRACE0( "CSendVTClose::InitPage()\n" );

	// flush the data
}

//
//	CSendVTClose::EncodePage
//

void CSendVTClose::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendVTClose::EncodePage()\n" );

	// encode the pdu type
	header.Add( 22 );

	// validate and encode the session ID's
	int len = m_IDCtrl.GetItemCount();
	for (int i = 0; i < len; i++) {
		const char *txt = m_IDCtrl.GetItemText( i, 0 );

		// make sure something was provided
		if (!txt || !*txt)
			throw "Invalid session ID";

		// convert to an integer and check the range
		int id = atoi( txt );
		if ((id < 0) || (id > 255))
			throw "Session ID out of range 0..255";

		// encode it
		BACnetUnsigned( id ).Encode( enc );
	}

	// copy the encoding into the byte array
	for (int j = 0; j < enc.pktLength; j++)
		header.Add( enc.pktBuffer[j] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendVTClose::SavePage
//

void CSendVTClose::SavePage( void )
{
	TRACE0( "CSendVTClose::SavePage\n" );

	pageContents.Flush();

//	m_IDCtrl.SaveList( pageContents );
}

//
//	CSendVTClose::RestorePage
//

void CSendVTClose::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendVTClose::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

//	m_IDCtrl.RestoreList( dec );
}

//
//	CSendVTClose::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendVTClose::OnAddID() 
{
	m_IDCtrl.AddButtonClick();

	// madanner 9/3/02, added default ID of 1
	m_IDCtrl.SetCtrlText(IDC_ID, "1");
}

void CSendVTClose::OnRemoveID() 
{
	m_IDCtrl.RemoveButtonClick();
}

void CSendVTClose::OnChangeID() 
{
	m_IDCtrl.OnChangeItem( IDC_ID );
}

void CSendVTClose::OnItemchangingIDList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_IDCtrl.OnItemChanging( pNMHDR, pResult );
}
