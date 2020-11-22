// SendBVLCResult.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendBVLCResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendBVLCResult::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendBVLCResult dialog

IMPLEMENT_DYNCREATE( CSendBVLCResult, CPropertyPage )

#pragma warning( disable : 4355 )
CSendBVLCResult::CSendBVLCResult( void )
	: CSendPage( CSendBVLCResult::IDD )
	, m_Result( this, IDC_RESULT )
{
	//{{AFX_DATA_INIT(CSendBVLCResult)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendBVLCResult::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendBVLCResult::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendBVLCResult)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_Result.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(CSendBVLCResult, CPropertyPage)
	//{{AFX_MSG_MAP(CSendBVLCResult)
	ON_EN_CHANGE(IDC_RESULT, OnChangeResult)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendBVLCResult message handlers

//
//	CSendBVLCResult::OnInitDialog
//

BOOL CSendBVLCResult::OnInitDialog() 
{
	TRACE0( "CSendBVLCResult::OnInitDialog()\n" );

	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendBVLCResult::InitPage
//

void CSendBVLCResult::InitPage( void )
{
	TRACE0( "CSendBVLCResult::InitPage()\n" );

	// flush the data
	m_Result.ctrlNull = true;
}

//
//	CSendBVLCResult::EncodePage
//

void CSendBVLCResult::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendBVLCResult::EncodePage()\n" );

	// encode the message header
	header.Add( 0x81 );
	header.Add( 0x00 );

	// encode the length
	header.Add( 0 );
	header.Add( 6 );

	// check and encode the result code
	if (m_Result.ctrlNull)
		throw "Result code required";
	if ((m_Result.intValue < 0) || (m_Result.intValue > 65535))
		throw "Result code out of range 0..65535";

	// encode the result code
	header.Add( m_Result.intValue >> 8 );
	header.Add( m_Result.intValue & 0xFF );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendBVLCResult::SavePage
//

void CSendBVLCResult::SavePage( void )
{
	TRACE0( "CSendBVLCResult::SavePage\n" );

	pageContents.Flush();

	m_Result.SaveCtrl( pageContents );
}

//
//	CSendBVLCResult::RestorePage
//

void CSendBVLCResult::RestorePage(  int index  )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendBVLCResult::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_Result.RestoreCtrl( dec );
}

//
//	CSendBVLCResult::OnChangeResult
//

void CSendBVLCResult::OnChangeResult() 
{
	// transfer the control contents to the address
	m_Result.UpdateData();

	// save the value for another instance of the dialog box
	SavePage();

	// update the encoding
	UpdateEncoded();
}
