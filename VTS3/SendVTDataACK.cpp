// SendVTDataACK.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendVTDataACK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendVTDataACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendVTDataACK dialog

IMPLEMENT_DYNCREATE( CSendVTDataACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendVTDataACK::CSendVTDataACK( void )
	: CSendPage( CSendVTDataACK::IDD )
	, m_AllDataFlag( this, IDC_ALLDATAFLAG, true )
	, m_AcceptedCount( this, IDC_ACCEPTEDCOUNT )
{
	//{{AFX_DATA_INIT(CSendVTDataACK)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void CSendVTDataACK::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendVTDataACK)
	//}}AFX_DATA_MAP

	m_AllDataFlag.UpdateData( pDX->m_bSaveAndValidate );
	m_AcceptedCount.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendVTDataACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendVTDataACK)
	ON_BN_CLICKED(IDC_ALLDATAFLAG, OnAllDataFlag)
	ON_EN_CHANGE(IDC_ACCEPTEDCOUNT, OnChangeAcceptedCount)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendVTDataACK::InitPage
//

void CSendVTDataACK::InitPage( void )
{
	TRACE0( "CSendVTDataACK::InitPage()\n" );

	// init the usual fields
	m_AllDataFlag.ctrlNull = false;
	m_AllDataFlag.boolValue = BACnetBoolean::bTrue;
	m_AcceptedCount.ctrlNull = true;
	m_AcceptedCount.ctrlEnabled = false;
}

//
//	CSendVTDataACK::EncodePage
//

void CSendVTDataACK::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 23 );

	// encode the all new data accepted
	m_AllDataFlag.Encode( enc, 0 );

	// encode the end-of-file
	if (!m_AllDataFlag.boolValue) {
		// check for accepted count
		if (m_AcceptedCount.ctrlNull)
			throw "Accepted octet count required";
		m_AcceptedCount.Encode( enc, 1 );
	}

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendVTDataACK::SavePage
//

void CSendVTDataACK::SavePage( void )
{
	TRACE0( "CSendVTDataACK::SavePage\n" );

	pageContents.Flush();

	m_AllDataFlag.SaveCtrl( pageContents );
	m_AcceptedCount.SaveCtrl( pageContents );
}

//
//	CSendVTDataACK::RestorePage
//

void CSendVTDataACK::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendVTDataACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_AllDataFlag.RestoreCtrl( dec );
	m_AcceptedCount.RestoreCtrl( dec );
}

//
//	CSendVTDataACK::OnInitDialog
//

BOOL CSendVTDataACK::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;
}

//
//	CSendVTDataACK::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendVTDataACK::OnAllDataFlag()
{
	m_AllDataFlag.UpdateData();

	// change the state of the accepted count to match the flag
	if (m_AllDataFlag.boolValue) {
		m_AcceptedCount.ctrlNull = true;
		m_AcceptedCount.Disable();
	} else
		m_AcceptedCount.Enable();

	SavePage();
	UpdateEncoded();
}

void CSendVTDataACK::OnChangeAcceptedCount()
{
	m_AcceptedCount.UpdateData();
	SavePage();
	UpdateEncoded();
}
