// SendICouldBeRTN.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendICouldBeRTN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendICouldBeRTN::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendICouldBeRTN dialog

IMPLEMENT_DYNCREATE( CSendICouldBeRTN, CPropertyPage )

#pragma warning( disable : 4355 )
CSendICouldBeRTN::CSendICouldBeRTN( void )
	: CSendPage( CSendICouldBeRTN::IDD )
	, m_DNET( this, IDC_DNET )
	, m_PerfIndx( this, IDC_PERFINDX )
{
	//{{AFX_DATA_INIT(CSendICouldBeRTN)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendICouldBeRTN::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendICouldBeRTN::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendICouldBeRTN)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_DNET.UpdateData( pDX->m_bSaveAndValidate );
	m_PerfIndx.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendICouldBeRTN, CPropertyPage)
	//{{AFX_MSG_MAP(CSendICouldBeRTN)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	ON_EN_CHANGE(IDC_PERFINDX, OnChangePerfIndx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendICouldBeRTN::InitPage
//

void CSendICouldBeRTN::InitPage( void )
{
	TRACE0( "CSendICouldBeRTN::InitPage()\n" );

	// flush the data
	m_DNET.ctrlNull = true;
	m_PerfIndx.ctrlNull = true;

	// tell the NPCI this is a network layer message
	pageParent->NPCIPage.m_isNetMessage = true;
}

//
//	CSendICouldBeRTN::EncodePage
//

void CSendICouldBeRTN::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendICouldBeRTN::EncodePage()\n" );

	// encode the message type
	header.Add( 0x02 );

	// encode the DNET
	if (m_DNET.ctrlNull)
		throw "Destination network required";
	if ((m_DNET.intValue < 0) || (m_DNET.intValue > 65534))
		throw "Destination network out of range 0..65534";
	header.Add( m_DNET.intValue >> 8 );
	header.Add( m_DNET.intValue & 0xFF );

	// encode the performance index
	if (m_PerfIndx.ctrlNull)
		throw "Performance index required";
	if ((m_PerfIndx.intValue < 0) || (m_PerfIndx.intValue > 255))
		throw "Performance index out of range 0..255";
	header.Add( m_PerfIndx.intValue );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendICouldBeRTN::SavePage
//

void CSendICouldBeRTN::SavePage( void )
{
	TRACE0( "CSendICouldBeRTN::SavePage\n" );

	pageContents.Flush();

	m_DNET.SaveCtrl( pageContents );
	m_PerfIndx.SaveCtrl( pageContents );
}

//
//	CSendICouldBeRTN::RestorePage
//

void CSendICouldBeRTN::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendICouldBeRTN::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_DNET.RestoreCtrl( dec );
	m_PerfIndx.RestoreCtrl( dec );
}

//
//	CSendICouldBeRTN::OnChangeDNET
//

void CSendICouldBeRTN::OnChangeDNET() 
{
	TRACE0( "CSendICouldBeRTN::OnChangeDNET()\n" );
	m_DNET.UpdateData();
	SavePage();
	UpdateEncoded();
}

//
//	CSendICouldBeRTN::OnChangePerfIndx
//

void CSendICouldBeRTN::OnChangePerfIndx() 
{
	TRACE0( "CSendICouldBeRTN::OnChangePerfIndx()\n" );
	m_PerfIndx.UpdateData();
	SavePage();
	UpdateEncoded();
}
