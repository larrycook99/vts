// SendRaw.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendRaw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendRaw::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendRaw dialog

IMPLEMENT_DYNCREATE( CSendRaw, CPropertyPage )

#pragma warning( disable : 4355 )
CSendRaw::CSendRaw( void )
	: CSendPage( CSendRaw::IDD )
	, m_data( this, IDC_DATA )
{
	//{{AFX_DATA_INIT(CSendRaw)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void CSendRaw::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendRaw::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendRaw)
	//}}AFX_DATA_MAP

	m_data.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(CSendRaw, CPropertyPage)
	//{{AFX_MSG_MAP(CSendRaw)
	ON_EN_UPDATE(IDC_DATA, OnUpdateData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendRaw::InitPage
//

void CSendRaw::InitPage( void )
{
	TRACE0( "CSendRaw::InitPage()\n" );

	// flush the data
	m_data.Flush();
}

//
//	CSendRaw::EncodePage
//

void CSendRaw::EncodePage( CByteArray* contents )
{
	TRACE0( "CSendRaw::EncodePage()\n" );

	// encode the contents
	for (int i = 0; i < m_data.Length(); i++)
		contents->InsertAt( i, m_data[i] );
}

//
//	CSendRaw::SavePage
//

void CSendRaw::SavePage( void )
{
	TRACE0( "CSendRaw::SavePage\n" );

	pageContents.Flush();

	m_data.SaveCtrl( pageContents );
}

//
//	CSendRaw::RestorePage
//

void CSendRaw::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendRaw::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_data.RestoreCtrl( dec );
}

//
//	CSendRaw::OnUpdateData
//

void CSendRaw::OnUpdateData() 
{
	TRACE0( "CSendRaw::OnUpdateData()\n" );
	m_data.UpdateData();
	SavePage();
	UpdateEncoded();
}
