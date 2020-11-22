// SendReadBDTAck.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendReadBDTAck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendReadBDTAck::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendReadBDTAck property page

IMPLEMENT_DYNCREATE(CSendReadBDTAck, CPropertyPage)

CSendReadBDTAck::CSendReadBDTAck( void )
	: CSendPage( CSendReadBDTAck::IDD )
{
	//{{AFX_DATA_INIT(CSendReadBDTAck)
		// NOTE: the ClassWizard will add member initialization here
	m_portInt = 0;
	m_maskInt = 0;
	//}}AFX_DATA_INIT
}

void CSendReadBDTAck::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReadBDTAck)
	DDX_Control(pDX, IDC_MASK, m_mask);
	DDX_Control(pDX, IDC_BDTPORT, m_port);
	DDX_Control(pDX, IDC_IP, m_IP);
	DDX_Control(pDX, IDC_BDTLIST, m_BDTList);
	DDX_Text(pDX, IDC_BDTPORT, m_portInt);
	DDX_Text(pDX, IDC_MASK, m_maskInt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendReadBDTAck, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReadBDTAck)
	ON_BN_CLICKED(IDC_ADDBDT, OnAddBDT)
	ON_BN_CLICKED(IDC_REMOVEBDT, OnRemoveBDT)
	ON_EN_CHANGE(IDC_MASK, OnChangeMask)
	ON_EN_CHANGE(IDC_BDTPORT, OnChangePort)
	ON_EN_CHANGE(IDC_IP, OnChangeIP)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_BDTLIST, OnItemchangingBDTList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReadBDTAck::InitPage
//

void CSendReadBDTAck::InitPage( void )
{
	TRACE0( "CSendReadBDTAck::InitPage()\n" );
}

//
//	CSendReadBDTAck::EncodePage
//

void CSendReadBDTAck::EncodePage( CByteArray* contents )
{
	int				len = m_BDTCtrl.GetItemCount()
	,				msgLen = len * 10 + 4
	;
	unsigned long	host, mask
	;
	unsigned short	port
	;
	CByteArray	header
	;

	TRACE0( "CSendReadBDTAck::EncodePage()\n" );

	// encode the message type and function
	header.Add( 0x81 );
	header.Add( 0x03 );

	// encode the length
	header.Add( msgLen >> 8 );
	header.Add( msgLen & 0xFF );

	// validate and encode the BDT entries
	for (int i = 0; i < len; i++) {
		//madanner 9/3/02
		//format:192.168.1.88/24:47808
        CString str = m_BDTCtrl.GetItemText( i, 0 ); 
		str	= str + "/" + m_BDTCtrl.GetItemText(i, 2); 
		str	= str + ":" + m_BDTCtrl.GetItemText(i, 1);
		const char *txt = (LPCTSTR)str;
//		const char *txt = m_BDTCtrl.GetItemText( i, 0 );

		// make sure something was provided
		if (!txt || !*txt)
			throw "Invalid BDT entry";

		// convert to host, port and network mask
		BACnetIPAddr::StringToHostPort( txt, &host, &mask, &port );

		// encode the host
		for (int j = 3; j >= 0; j--)
			header.Add( (unsigned char)((host >> (j * 8)) & 0xFF) );

		// encode the port
		header.Add( port >> 8 );
		header.Add( port & 0xFF );

		// encode the mask
		for (int k = 3; k >= 0; k--)
			header.Add( (unsigned char)((mask >> (k * 8)) & 0xFF) );
	}

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendReadBDTAck::SavePage
//

void CSendReadBDTAck::SavePage( void )
{
	TRACE0( "CSendReadBDTAck::SavePage\n" );

	pageContents.Flush();

	// ### save list contents
}

//
//	CSendReadBDTAck::RestorePage
//

void CSendReadBDTAck::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendReadBDTAck::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore list contents
}

//
//	CSendReadBDTAck::OnInitDialog
//

BOOL CSendReadBDTAck::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
//		{ { "BDT Entry", LVCFMT_RIGHT, 128, IDC_BDTENTRY }
		{ { "IP Address", LVCFMT_RIGHT, 128, IDC_IP }
	    , { "UDP Port", LVCFMT_RIGHT, 84, IDC_BDTPORT }
		, { "Mask", LVCFMT_RIGHT, 60, IDC_MASK }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendReadBDTAck::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_BDTList.m_nFlags |= LVS_SINGLESEL;
	m_BDTList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_BDTCtrl.Init( this, &m_BDTList, colDesc );

	//madanner 9/3/02 default UDP port
	m_port.SetWindowText("47808"); 
	m_mask.SetWindowText("0"); 

	return TRUE;
}

void CSendReadBDTAck::OnAddBDT() 
{
	m_BDTCtrl.AddButtonClick();

	//madanner 9/3/02 default UDP port
	m_port.SetWindowText("47808"); 
	m_mask.SetWindowText("0");
}

// madanner 9/3/02 added IP address text box
void CSendReadBDTAck::OnChangeIP() 
{
	m_BDTCtrl.OnChangeItem( IDC_IP );
}

void CSendReadBDTAck::OnRemoveBDT() 
{
	m_BDTCtrl.RemoveButtonClick();
}

// madanner 9/3/02  Not needed... IP address broken out
//void CSendReadBDTAck::OnChangeBDTEntry() 
//{
//	m_BDTCtrl.OnChangeItem( IDC_BDTENTRY );
//}

void CSendReadBDTAck::OnItemchangingBDTList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_BDTCtrl.OnItemChanging( pNMHDR, pResult );
}

//madanner 9/3/02
void CSendReadBDTAck::OnChangeMask() 
{
    m_BDTCtrl.OnChangeItem( IDC_MASK );	
}

//madanner 9/3/02
void CSendReadBDTAck::OnChangePort() 
{	
	m_BDTCtrl.OnChangeItem( IDC_BDTPORT );	
}

