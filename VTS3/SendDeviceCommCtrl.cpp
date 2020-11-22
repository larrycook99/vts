// SendDeviceCommCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendDeviceCommCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendDeviceCommCtrl::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendDeviceCommCtrl property page

IMPLEMENT_DYNCREATE( CSendDeviceCommCtrl, CPropertyPage )

#pragma warning( disable : 4355 )
CSendDeviceCommCtrl::CSendDeviceCommCtrl( void )
	: CSendPage( CSendDeviceCommCtrl::IDD )
	, m_TimeDuration( this, IDC_TIMEDURATION )
	, m_Password( this, IDC_PASSWORD )
{
	//{{AFX_DATA_INIT(CSendDeviceCommCtrl)
	m_EnableDisable = 0;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendDeviceCommCtrl::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendDeviceCommCtrl)
	DDX_Radio(pDX, IDC_ENABLE, m_EnableDisable);
	//}}AFX_DATA_MAP

	m_TimeDuration.UpdateData( pDX->m_bSaveAndValidate );
	m_Password.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendDeviceCommCtrl, CPropertyPage)
	//{{AFX_MSG_MAP(CSendDeviceCommCtrl)
	ON_EN_CHANGE(IDC_TIMEDURATION, OnChangeTimeDuration)
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_BN_CLICKED(IDC_DISABLE, OnDisable)
	ON_BN_CLICKED(IDC_DISABLE_INIT, OnDisableInit)
	ON_EN_CHANGE(IDC_PASSWORD, OnChangePassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendDeviceCommCtrl::OnInitDialog
//

BOOL CSendDeviceCommCtrl::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendDeviceCommCtrl::InitPage
//

void CSendDeviceCommCtrl::InitPage( void )
{
	// flush the data
	m_TimeDuration.ctrlNull = true;
	m_Password.ctrlNull = true;
}

//
//	CSendDeviceCommCtrl::EncodePage
//

void CSendDeviceCommCtrl::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 17 );

	// encode the time duration
	if (!m_TimeDuration.ctrlNull) {
		if (m_TimeDuration.uintValue > 65535)
			throw "Time duration out of range 0..65535";

		m_TimeDuration.Encode( enc, 0 );
	}

	// encode the enable/disable
	BACnetEnumerated( m_EnableDisable ).Encode( enc, 1 );

	// encode the message
	if (!m_Password.ctrlNull)
		m_Password.Encode( enc, 2 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendDeviceCommCtrl::SavePage
//

void CSendDeviceCommCtrl::SavePage( void )
{
	TRACE0( "CSendDeviceCommCtrl::SavePage\n" );

	pageContents.Flush();

	m_TimeDuration.SaveCtrl( pageContents );
	m_Password.SaveCtrl( pageContents );
	BACnetInteger( m_EnableDisable ).Encode( pageContents );
}

//
//	CSendDeviceCommCtrl::RestorePage
//

void CSendDeviceCommCtrl::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;
	BACnetInteger		endis
	;

	TRACE0( "CSendDeviceCommCtrl::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_TimeDuration.RestoreCtrl( dec );
	m_Password.RestoreCtrl( dec );

	endis.Decode( dec );
	m_EnableDisable = endis.intValue;
}

//
//	CSendDeviceCommCtrl::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendDeviceCommCtrl::OnChangeTimeDuration()
{
	m_TimeDuration.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendDeviceCommCtrl::OnEnable() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendDeviceCommCtrl::OnDisable() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendDeviceCommCtrl::OnDisableInit() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendDeviceCommCtrl::OnChangePassword()
{
	m_Password.UpdateData();
	SavePage();
	UpdateEncoded();
}
