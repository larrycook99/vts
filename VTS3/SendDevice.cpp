// SendDevice.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendDevice::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendDevice dialog

IMPLEMENT_DYNCREATE( CSendDevice, CPropertyPage )

#pragma warning( disable : 4355 )
CSendDevice::CSendDevice( void )
	: CSendPage( CSendDevice::IDD )
	, m_DADR( this, IDC_DADRCOMBO, IDC_DADR )
	, m_Network( this, IDC_NETWORK )
	, m_Address( this, IDC_ADDRESS )
{
	//{{AFX_DATA_INIT(CSendDevice)
#if DEVICE_LOOPBACK
	m_AddrType = 0;
#else
	m_AddrType = 1;
#endif
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendDevice::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendDevice::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendDevice)
	DDX_Radio(pDX, IDC_NULLADDR, m_AddrType);
	//}}AFX_DATA_MAP

//	m_DADR.UpdateData( pDX->m_bSaveAndValidate );
	m_Network.UpdateData( pDX->m_bSaveAndValidate );
	m_Address.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendDevice, CPropertyPage)
	//{{AFX_MSG_MAP(CSendDevice)
	ON_CBN_SELCHANGE(IDC_DADRCOMBO, OnSelchangeDADRCombo)
	ON_BN_CLICKED(IDC_NULLADDR, OnAddrTypeChange)
	ON_BN_CLICKED(IDC_LOCALBROADCAST, OnAddrTypeChange)
	ON_BN_CLICKED(IDC_LOCALSTATION, OnAddrTypeChange)
	ON_BN_CLICKED(IDC_REMOTEBROADCAST, OnAddrTypeChange)
	ON_BN_CLICKED(IDC_REMOTESTATION, OnAddrTypeChange)
	ON_BN_CLICKED(IDC_GLOBALBROADCAST, OnAddrTypeChange)
	ON_EN_CHANGE(IDC_NETWORK, OnChangeNetwork)
	ON_EN_CHANGE(IDC_ADDRESS, OnChangeAddress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendDevice::OnInitDialog
//

BOOL CSendDevice::OnInitDialog() 
{
	TRACE0( "CSendDevice::OnInitDialog\n" );

	CPropertyPage::OnInitDialog();
	
	// load the address combo
//	m_DADR.LoadCombo( &pageParent->m_pPort->portDoc->m_Names, pageParent->m_pPort->portDescID );
//	m_DADR.FindName( "IUT" );
	
	return TRUE;
}

//
//	CSendDevice::InitPage
//

void CSendDevice::InitPage( void )
{
	TRACE0( "CSendDevice::InitPage()\n" );

#if DEVICE_LOOPBACK
	GetDlgItem( IDC_NULLADDR )->EnableWindow( true );
#endif
}

//
//	CSendDevice::EncodePage
//

void CSendDevice::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendDevice::EncodePage()\n" );

	switch (m_AddrType) {
		case -1:	// no selection
			throw "Select an address type";
			break;

		case 0:		// null address
#if !DEVICE_LOOPBACK
			throw "Invalid null address";
#endif
			break;

		case 1:		// local broadcast
		case 5:		// global broadcast
			break;

		case 2:		// local station
			if (m_Address.ctrlNull)
				throw "Address required";
			break;

		case 3:		// remote broadcast
			if (m_Network.ctrlNull)
				throw "Network required";
			if (m_Network.uintValue > 65534)
				throw "Network out of range 0..65534";
			break;

		case 4:		// remote station
			if (m_Network.ctrlNull)
				throw "Network required";
			if (m_Network.uintValue > 65534)
				throw "Network out of range 0..65534";
			if (m_Address.ctrlNull)
				throw "Address required";
			break;
	}

	// save the type
	header.Add( m_AddrType );

	// save the network number
	header.Add( (unsigned char)(m_Network.uintValue >> 8) );
	header.Add( (unsigned char)(m_Network.uintValue & 0xFF) );

	// save the address
	header.Add( m_Address.strLen );
	for (int i = 0; i < m_Address.strLen; i++)
		header.Add( m_Address.strBuff[i] );

	// stuff header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendDevice::SavePage
//

void CSendDevice::SavePage( void )
{
	TRACE0( "CSendDevice::SavePage\n" );

	pageContents.Flush();

//	m_DADR.SaveCtrl( pageContents );
	m_Network.SaveCtrl( pageContents );
	m_Address.SaveCtrl( pageContents );
}

//
//	CSendDevice::RestorePage
//

void CSendDevice::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendDevice::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

//	m_DADR.RestoreCtrl( dec );
	m_Network.RestoreCtrl( dec );
	m_Address.RestoreCtrl( dec );
}

/////////////////////////////////////////////////////////////////////////////
// CSendDevice message handlers

void CSendDevice::OnSelchangeDADRCombo() 
{
	TRACE0( "CSendDevice::OnSelchangeDADRCombo()\n" );
	SavePage();
	UpdateEncoded();
}

void CSendDevice::OnAddrTypeChange() 
{
	TRACE0( "CSendDevice::OnAddrTypeChange()\n" );

	// sync the member variables with the dialog controls
	UpdateData( true );

	switch (m_AddrType) {
		case -1:	// no selection
		case 0:		// null address
		case 1:		// local broadcast
		case 5:		// global broadcast
			GetDlgItem( IDC_NETWORK )->EnableWindow( false );
			GetDlgItem( IDC_ADDRESS )->EnableWindow( false );
			break;

		case 2:		// local station
			GetDlgItem( IDC_NETWORK )->EnableWindow( false );
			GetDlgItem( IDC_ADDRESS )->EnableWindow( true );
			break;

		case 3:		// remote broadcast
			GetDlgItem( IDC_NETWORK )->EnableWindow( true );
			GetDlgItem( IDC_ADDRESS )->EnableWindow( false );
			break;

		case 4:		// remote station
			GetDlgItem( IDC_NETWORK )->EnableWindow( true );
			GetDlgItem( IDC_ADDRESS )->EnableWindow( true );
			break;
	}

	SavePage();
	UpdateEncoded();
}

void CSendDevice::OnChangeNetwork() 
{
	TRACE0( "CSendDevice::OnChangeNetwork()\n" );

	m_Network.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendDevice::OnChangeAddress() 
{
	TRACE0( "CSendDevice::OnChangeAddress()\n" );

	m_Address.UpdateData();
	SavePage();
	UpdateEncoded();
}
