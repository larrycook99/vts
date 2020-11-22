// SendReadFDTAck.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendReadFDTAck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendReadFDTAck::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendReadFDTAck property page

IMPLEMENT_DYNCREATE(CSendReadFDTAck, CPropertyPage)

CSendReadFDTAck::CSendReadFDTAck( void )
	: CSendPage( CSendReadFDTAck::IDD )
{
	//{{AFX_DATA_INIT(CSendReadFDTAck)
		// NOTE: the ClassWizard will add member initialization here
	m_portInt = 0;
	//}}AFX_DATA_INIT
}

void CSendReadFDTAck::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReadFDTAck)
	DDX_Control(pDX, IDC_BDTPORT, m_port);
	DDX_Control(pDX, IDC_IP, m_IP);
	DDX_Control(pDX, IDC_FDTLIST, m_FDTList);
	DDX_Text(pDX, IDC_BDTPORT, m_portInt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendReadFDTAck, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReadFDTAck)
	ON_BN_CLICKED(IDC_ADDFDT, OnAddFDT)
	ON_BN_CLICKED(IDC_REMOVEFDT, OnRemoveFDT)
	ON_EN_CHANGE(IDC_BDTPORT, OnChangePort)
	ON_EN_CHANGE(IDC_IP, OnChangeIP)
	ON_EN_CHANGE(IDC_TTL, OnChangeTTL)
	ON_EN_CHANGE(IDC_TREMAIN, OnChangeTRemain)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_FDTLIST, OnItemchangingFDTList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReadFDTAck::InitPage
//

void CSendReadFDTAck::InitPage( void )
{
	TRACE0( "CSendReadFDTAck::InitPage()\n" );
}

//
//	CSendReadFDTAck::EncodePage
//

void CSendReadFDTAck::EncodePage( CByteArray* contents )
{
	int				len = m_FDTCtrl.GetItemCount()
	,				msgLen = len * 10 + 4
	;
	unsigned long	host, mask
	;
	unsigned short	port, ttl, rem
	;
	CByteArray	header
	;

	TRACE0( "CSendReadFDTAck::EncodePage()\n" );

	// encode the message type and function
	header.Add( 0x81 );
	header.Add( 0x07 );

	// encode the length
	header.Add( msgLen >> 8 );
	header.Add( msgLen & 0xFF );

	// validate and encode the BDT entries
	for (int i = 0; i < len; i++) {
		//madanner 9/3/02
		//format:192.168.1.88:47808
        CString str = m_FDTCtrl.GetItemText( i, 0 );
		str = str + ":" + m_FDTCtrl.GetItemText(i, 1);
		const char *entry = (LPCTSTR)str;
//		const char *entry = m_FDTCtrl.GetItemText( i, 0 );

		// make sure something was provided
		if (!entry || !*entry)
			throw "Invalid BDT entry";

		// convert to host, port and network mask
		BACnetIPAddr::StringToHostPort( entry, &host, &mask, &port );

		// encode the host
		for (int j = 3; j >= 0; j--)
			header.Add( (unsigned char)((host >> (j * 8)) & 0xFF) );

		// encode the port
		header.Add( port >> 8 );
		header.Add( port & 0xFF );

		const char *ttltxt = m_FDTCtrl.GetItemText( i, 2 );

		// make sure something was provided
		if (!ttltxt || !*ttltxt)
			throw "Invalid TTL";
		if (sscanf( ttltxt, "%d", &ttl ) != 1)
			throw "Invalid TTL";

		// encode the time-to-live
		header.Add( ttl >> 8 );
		header.Add( ttl & 0xFF );

		const char *remtxt = m_FDTCtrl.GetItemText( i, 3 );

		// make sure something was provided
		if (!remtxt || !*remtxt)
			throw "Invalid time remaining";
		if (sscanf( remtxt, "%d", &rem ) != 1)
			throw "Invalid time remaining";

		// encode the time remaining
		header.Add( rem >> 8 );
		header.Add( rem & 0xFF );
	}

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendReadFDTAck::SavePage
//

void CSendReadFDTAck::SavePage( void )
{
	TRACE0( "CSendReadFDTAck::SavePage\n" );

	pageContents.Flush();

	// ### save list contents
}

//
//	CSendReadFDTAck::RestorePage
//

void CSendReadFDTAck::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendReadFDTAck::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore list contents
}

//
//	CSendReadFDTAck::OnInitDialog
//

BOOL CSendReadFDTAck::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "IP Address", LVCFMT_RIGHT, 104, IDC_IP }
		, { "UDP Port", LVCFMT_RIGHT, 64, IDC_BDTPORT }
		, { "TTL", LVCFMT_RIGHT, 54, IDC_TTL }
		, { "Remaining", LVCFMT_RIGHT, 64, IDC_TREMAIN }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendReadFDTAck::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_FDTList.m_nFlags |= LVS_SINGLESEL;
	m_FDTList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_FDTCtrl.Init( this, &m_FDTList, colDesc );

	//madanner 9/3/02 default UDP port
	m_port.SetWindowText("47808"); 
	GetDlgItem(IDC_TTL)->SetWindowText("0");
	GetDlgItem(IDC_TREMAIN)->SetWindowText("0");

	return TRUE;
}

void CSendReadFDTAck::OnAddFDT() 
{
	m_FDTCtrl.AddButtonClick();
	//madanner 9/3/02 default UDP port
	m_port.SetWindowText("47808"); 
	GetDlgItem(IDC_TTL)->SetWindowText("0");
	GetDlgItem(IDC_TREMAIN)->SetWindowText("0");
}

// madanner 9/3/02 added IP address text box
void CSendReadFDTAck::OnChangeIP() 
{
	m_FDTCtrl.OnChangeItem( IDC_IP );
}

void CSendReadFDTAck::OnRemoveFDT() 
{
	m_FDTCtrl.RemoveButtonClick();
}

// madanner 9/3/02  Not needed... IP address broken out
//void CSendReadFDTAck::OnChangeFDTEntry() 
//{
//	m_FDTCtrl.OnChangeItem( IDC_FDTENTRY );
//}

void CSendReadFDTAck::OnChangeTTL() 
{
	m_FDTCtrl.OnChangeItem( IDC_TTL );
}

void CSendReadFDTAck::OnChangeTRemain() 
{
	m_FDTCtrl.OnChangeItem( IDC_TREMAIN );
}

void CSendReadFDTAck::OnItemchangingFDTList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_FDTCtrl.OnItemChanging( pNMHDR, pResult );
}

//madanner 9/3/02
void CSendReadFDTAck::OnChangePort() 
{	
	m_FDTCtrl.OnChangeItem( IDC_BDTPORT );	
}

