// SendVTData.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendVTData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendVTData::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendVTData dialog

IMPLEMENT_DYNCREATE( CSendVTData, CPropertyPage )

#pragma warning( disable : 4355 )
CSendVTData::CSendVTData( void )
	: CSendPage( CSendVTData::IDD )
	, m_ID( this, IDC_ID )
	, m_Data( this, IDC_DATA )
	, m_DataFlag( this, IDC_DATAFLAG, true )
{
	//{{AFX_DATA_INIT(CSendVTData)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendVTData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendVTData)
	//}}AFX_DATA_MAP

	m_ID.UpdateData( pDX->m_bSaveAndValidate );
	m_Data.UpdateData( pDX->m_bSaveAndValidate );
	m_DataFlag.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendVTData, CPropertyPage)
	//{{AFX_MSG_MAP(CSendVTData)
	ON_EN_CHANGE(IDC_ID, OnChangeID)
	ON_EN_CHANGE(IDC_DATA, OnChangeData)
	ON_BN_CLICKED(IDC_DATAFLAG, OnDataFlag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendVTData::InitPage
//

void CSendVTData::InitPage( void )
{
	TRACE0( "CSendVTData::InitPage()\n" );

	// init the usual fields
	m_ID.ctrlNull = true;
	m_Data.ctrlNull = false;
	m_DataFlag.ctrlNull = false;
	m_DataFlag.boolValue = BACnetBoolean::bFalse;
}

//
//	CSendVTData::EncodePage
//

void CSendVTData::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 23 );

	// check for access type
	if (m_ID.ctrlNull)
		throw "Session ID required";
	m_ID.Encode( enc );

	// encode the data
	m_Data.Encode( enc );

	// encode the end-of-file
	if (m_DataFlag.boolValue)
		BACnetUnsigned( 1 ).Encode( enc );
	else
		BACnetUnsigned( 0 ).Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendVTData::SavePage
//

void CSendVTData::SavePage( void )
{
	TRACE0( "CSendVTData::SavePage\n" );

	pageContents.Flush();

	m_ID.SaveCtrl( pageContents );
	m_Data.SaveCtrl( pageContents );
	m_DataFlag.SaveCtrl( pageContents );
}

//
//	CSendVTData::RestorePage
//

void CSendVTData::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendVTData::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ID.RestoreCtrl( dec );
	m_Data.RestoreCtrl( dec );
	m_DataFlag.RestoreCtrl( dec );
}

//
//	CSendVTData::OnInitDialog
//

BOOL CSendVTData::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;
}

//
//	CSendVTData::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendVTData::OnChangeID()
{
	m_ID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendVTData::OnChangeData()
{
	m_Data.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendVTData::OnDataFlag()
{
	m_DataFlag.UpdateData();
	SavePage();
	UpdateEncoded();
}
