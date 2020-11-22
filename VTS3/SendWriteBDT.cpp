// SendWriteBDT.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendWriteBDT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendWriteBDT::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendWriteBDT property page

IMPLEMENT_DYNCREATE(CSendWriteBDT, CPropertyPage)

CSendWriteBDT::CSendWriteBDT( void )
	: CSendPage( CSendWriteBDT::IDD )
{
	//{{AFX_DATA_INIT(CSendWriteBDT)
	// NOTE: the ClassWizard will add member initialization here
	m_portInt = 0;
	m_maskInt = 0;
	//}}AFX_DATA_INIT
}


void CSendWriteBDT::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWriteBDT)
	DDX_Control(pDX, IDC_MASK, m_mask);
	DDX_Control(pDX, IDC_BDTPORT, m_port);
	DDX_Control(pDX, IDC_IP, m_IP);
	DDX_Control(pDX, IDC_BDTLIST, m_BDTList);	
	DDX_Text(pDX, IDC_BDTPORT, m_portInt);
	DDX_Text(pDX, IDC_MASK, m_maskInt);
	//}}AFX_DATA_MAP
	
	if(m_port.IsWindowEnabled())
		DDV_MinMaxInt(pDX, m_portInt, 0, 0xFFFF);
	if(m_mask.IsWindowEnabled())
		DDV_MinMaxInt(pDX, m_maskInt, 1, 32);
}

BEGIN_MESSAGE_MAP(CSendWriteBDT, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWriteBDT)
	ON_BN_CLICKED(IDC_ADDBDT, OnAddBDT)
	ON_BN_CLICKED(IDC_REMOVEBDT, OnRemoveBDT)
	ON_EN_CHANGE(IDC_IP, OnChangeIP)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_BDTLIST, OnItemchangingBDTList)
	ON_EN_CHANGE(IDC_MASK, OnChangeMask)
	ON_EN_CHANGE(IDC_BDTPORT, OnChangePort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWriteBDT::InitPage
//

void CSendWriteBDT::InitPage( void )
{
	TRACE0( "CSendWriteBDT::InitPage()\n" );
}

//
//	CSendWriteBDT::EncodePage
//

void CSendWriteBDT::EncodePage( CByteArray* contents )
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

	TRACE0( "CSendWriteBDT::EncodePage()\n" );

	// encode the message type and function
	header.Add( 0x81 );
	header.Add( 0x01 );

	// encode the length
	header.Add( msgLen >> 8 );
	header.Add( msgLen & 0xFF );

	// validate and encode the BDT entries
	for (int i = 0; i < len; i++) {
		//Xiao Shiyuan 2002-6-18
		//format:192.168.1.88/24:47808
        CString str = m_BDTCtrl.GetItemText( i, 0 ); 
		str	= str + "/" + m_BDTCtrl.GetItemText(i, 2); 
		str	= str + ":" + m_BDTCtrl.GetItemText(i, 1);
		const char *txt = (LPCTSTR)str;

		// make sure something was provided
		if (!txt || !*txt)
			throw "Invalid BDT entry";

		// convert to host, port and network mask
		BACnetIPAddr::StringToHostPort( txt, &host, &mask, &port );		

		// encode the host
		for (int i = 3; i >= 0; i--)
			header.Add( (unsigned char)((host >> (i * 8)) & 0xFF) );

		// encode the port
		header.Add( port >> 8 );
		header.Add( port & 0xFF );

		// encode the mask
		for (int j = 3; j >= 0; j--)
			header.Add( (unsigned char)((mask >> (j * 8)) & 0xFF) );
	}

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendWriteBDT::SavePage
//

void CSendWriteBDT::SavePage( void )
{
	TRACE0( "CSendWriteBDT::SavePage\n" );

	pageContents.Flush();

//	m_BDTCtrl.SaveList( pageContents );
}

//
//	CSendWriteBDT::RestorePage
//

void CSendWriteBDT::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendWriteBDT::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

//	m_BDTCtrl.RestoreCtrl( dec );
}

//
//	CSendWriteBDT::OnInitDialog
//

BOOL CSendWriteBDT::OnInitDialog() 
{
	//Xiao Shiyuan 2002-6-18
	static VTSListColDesc colDesc[] =
		{ { "IP Address", LVCFMT_RIGHT, 128, IDC_IP }
	    , { "UDP Port", LVCFMT_RIGHT, 84, IDC_BDTPORT }
		, { "Mask", LVCFMT_RIGHT, 60, IDC_MASK }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendWriteBDT::OnInitDialog()\n" );	

	CDialog::OnInitDialog();			
	
	// only allow one selection at a time, no sorting
	m_BDTList.m_nFlags |= LVS_SINGLESEL;
	m_BDTList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_BDTCtrl.Init( this, &m_BDTList, colDesc );	

	//Xiao Shiyuan 2002-6-18 default UDP port
	m_port.SetWindowText("47808"); 
	m_mask.SetWindowText("0"); 

	return TRUE;
}

void CSendWriteBDT::OnAddBDT() 
{
	m_BDTCtrl.AddButtonClick();	

	//Xiao Shiyuan 2002-6-18 default UDP port
	m_port.SetWindowText("47808"); 
	m_mask.SetWindowText("0");
}

void CSendWriteBDT::OnRemoveBDT() 
{
	m_BDTCtrl.RemoveButtonClick();
}

void CSendWriteBDT::OnChangeIP() 
{
	m_BDTCtrl.OnChangeItem( IDC_IP );
}

void CSendWriteBDT::OnItemchangingBDTList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_BDTCtrl.OnItemChanging( pNMHDR, pResult );
}

//Xiao Shiyuan 2002-6-18
void CSendWriteBDT::OnChangeMask() 
{
    m_BDTCtrl.OnChangeItem( IDC_MASK );	
}

//Xiao Shiyuan 2002-6-18
void CSendWriteBDT::OnChangePort() 
{	
	m_BDTCtrl.OnChangeItem( IDC_BDTPORT );	
}
