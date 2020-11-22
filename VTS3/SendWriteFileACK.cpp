// SendWriteFileACK.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendWriteFileACK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendWriteFileACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendWriteFileACK dialog

IMPLEMENT_DYNCREATE( CSendWriteFileACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendWriteFileACK::CSendWriteFileACK( void )
	: CSendPage( CSendWriteFileACK::IDD )
	, m_StartPosition( this, IDC_STARTPOSITION )
	, m_StartRecord( this, IDC_STARTRECORD )
{
	//{{AFX_DATA_INIT(CSendWriteFileACK)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendWriteFileACK::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendWriteFileACK::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWriteFileACK)
	//}}AFX_DATA_MAP

	m_StartPosition.UpdateData( pDX->m_bSaveAndValidate );
	m_StartRecord.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendWriteFileACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWriteFileACK)
	ON_EN_CHANGE(IDC_STARTPOSITION, OnChangeStartPosition)
	ON_EN_CHANGE(IDC_STARTRECORD, OnChangeStartRecord)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWriteFileACK::OnInitDialog
//

BOOL CSendWriteFileACK::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendWriteFileACK::InitPage
//

void CSendWriteFileACK::InitPage( void )
{
	// flush the data
	m_StartPosition.ctrlNull = true;
	m_StartRecord.ctrlNull = true;
}

//
//	CSendWriteFileACK::EncodePage
//

void CSendWriteFileACK::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 7 );

	// check for access type
	if (!m_StartPosition.ctrlNull) {
		if (!m_StartRecord.ctrlNull)
			throw "Omit start record for stream access";

		m_StartPosition.Encode( enc, 0 );
	} else
	if (!m_StartRecord.ctrlNull) {
		if (!m_StartPosition.ctrlNull)
			throw "Omit start position for record access";

		m_StartRecord.Encode( enc, 1 );
	} else
		throw "Stream or record access required";

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendWriteFileACK::SavePage
//

void CSendWriteFileACK::SavePage( void )
{
	TRACE0( "CSendWriteFileACK::SavePage\n" );

	pageContents.Flush();

	m_StartPosition.SaveCtrl( pageContents );
	m_StartRecord.SaveCtrl( pageContents );
}

//
//	CSendWriteFileACK::RestorePage
//

void CSendWriteFileACK::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendWriteFileACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_StartPosition.RestoreCtrl( dec );
	m_StartRecord.RestoreCtrl( dec );
}

//
//	CSendWriteFileACK::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendWriteFileACK::OnChangeStartPosition()
{
	m_StartPosition.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteFileACK::OnChangeStartRecord()
{
	m_StartRecord.UpdateData();
	SavePage();
	UpdateEncoded();
}
