// SendReinitDevice.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendReinitDevice.h"
#include "bakrestoreexecutor.h"
#include ".\sendreinitdevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendReinitDevice::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendReinitDevice property page

IMPLEMENT_DYNCREATE( CSendReinitDevice, CPropertyPage )

#pragma warning( disable : 4355 )
CSendReinitDevice::CSendReinitDevice( void )
	: CSendPage( CSendReinitDevice::IDD )
	, m_Password( this, IDC_PASSWORD )
{
	m_State = 0;
}

#pragma warning( default : 4355 )

void CSendReinitDevice::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSendReinitDevice)
  //}}AFX_DATA_MAP

  m_Password.UpdateData( pDX->m_bSaveAndValidate );
  DDX_Control(pDX, IDC_REINITSTATE, ReinitStates);
}

BEGIN_MESSAGE_MAP(CSendReinitDevice, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReinitDevice)
	//}}AFX_MSG_MAP
  ON_CBN_SELCHANGE(IDC_REINITSTATE, OnCbnSelchangeReinitstate)
  ON_EN_CHANGE(IDC_PASSWORD, OnEnChangePassword)
END_MESSAGE_MAP()

//
//	CSendReinitDevice::OnInitDialog
//

BOOL CSendReinitDevice::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

  ReinitStates.AddString("Cold Start");
  ReinitStates.AddString("Warm Start");
  ReinitStates.AddString("Start Backup");
  ReinitStates.AddString("End Backup");
  ReinitStates.AddString("Start Restore");
  ReinitStates.AddString("End Restore");
  ReinitStates.AddString("Abort Restore");
  ReinitStates.SetCurSel(m_State);

	return TRUE;
}

//
//	CSendReinitDevice::InitPage
//

void CSendReinitDevice::InitPage( void )
{
	// flush the data
	m_Password.ctrlNull = true;
}

//
//	CSendReinitDevice::EncodePage
//

void CSendReinitDevice::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 20 );

	// encode the cold/warm start
	BACnetEnumerated( m_State ).Encode( enc, 0 );

	// encode the message
	if (!m_Password.ctrlNull)
		m_Password.Encode( enc, 1 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendReinitDevice::SavePage
//

void CSendReinitDevice::SavePage( void )
{
	TRACE0( "CSendReinitDevice::SavePage\n" );

	pageContents.Flush();

	BACnetInteger( m_State ).Encode( pageContents );
	m_Password.SaveCtrl( pageContents );
}

//
//	CSendReinitDevice::RestorePage
//

void CSendReinitDevice::RestorePage( int index )
{

	BACnetAPDUDecoder	dec( pageContents )
	;
	BACnetInteger		intValue
	;

	TRACE0( "CSendReinitDevice::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	intValue.Decode( dec );
	m_State = intValue.intValue;

	m_Password.RestoreCtrl( dec );
}

void CSendReinitDevice::OnCbnSelchangeReinitstate()
{
  m_State = ReinitStates.GetCurSel();
  SavePage();
	UpdateEncoded();
}

void CSendReinitDevice::OnEnChangePassword()
{
  m_Password.UpdateData();
  SavePage();
  UpdateEncoded();
}

