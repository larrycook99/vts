// SendWhoIsRTN.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendWhoIsRTN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendWhoIsRTN::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendWhoIsRTN dialog

IMPLEMENT_DYNCREATE( CSendWhoIsRTN, CPropertyPage )

#pragma warning( disable : 4355 )
CSendWhoIsRTN::CSendWhoIsRTN( void )
	: CSendPage( CSendWhoIsRTN::IDD )
	, m_DNET( this, IDC_DNET )
{
	//{{AFX_DATA_INIT(CSendWhoIsRTN)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendWhoIsRTN::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendWhoIsRTN::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWhoIsRTN)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_DNET.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(CSendWhoIsRTN, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWhoIsRTN)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWhoIsRTN::OnInitDialog
//

BOOL CSendWhoIsRTN::OnInitDialog() 
{
	TRACE0( "CSendWhoIsRTN::OnInitDialog()\n" );

	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendWhoIsRTN::InitPage
//

void CSendWhoIsRTN::InitPage( void )
{
	TRACE0( "CSendWhoIsRTN::InitPage()\n" );

	// flush the data
	m_DNET.ctrlNull = true;

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendWhoIsRTN::EncodePage
//

void CSendWhoIsRTN::EncodePage( CByteArray* contents )
{
	TRACE0( "CSendWhoIsRTN::EncodePage()\n" );

	// encode the message type
	contents->InsertAt( 0, (BYTE)0x00 );

	// check and encode the contents
	if (!m_DNET.ctrlNull) {
		if ((m_DNET.intValue < 0) || (m_DNET.intValue > 65534))
			throw "Destination network out of range 0..65534";

		contents->InsertAt( 1, m_DNET.intValue >> 8 );
		contents->InsertAt( 2, m_DNET.intValue & 0xFF );
	}
}

//
//	CSendWhoIsRTN::SavePage
//

void CSendWhoIsRTN::SavePage( void )
{
	TRACE0( "CSendWhoIsRTN::SavePage\n" );

	pageContents.Flush();

	m_DNET.SaveCtrl( pageContents );
}

//
//	CSendWhoIsRTN::RestorePage
//

void CSendWhoIsRTN::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendWhoIsRTN::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_DNET.RestoreCtrl( dec );
}

//
//	CSendWhoIsRTN::OnChangeDNET
//

void CSendWhoIsRTN::OnChangeDNET() 
{
	m_DNET.UpdateData();
	SavePage();
	UpdateEncoded();
}
