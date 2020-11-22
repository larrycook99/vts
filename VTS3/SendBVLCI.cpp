// SendBVLCI.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendBVLCI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendBVLCI::pageContents[glMaxHistoryCount];
int CSendBVLCI::historyCount = 0;           //Xiao Shiyuan 2002-12-5
int CSendBVLCI::curHistoryIndex = 0;        //Xiao Shiyuan 2002-12-5

/////////////////////////////////////////////////////////////////////////////
// CSendBVLCI dialog

IMPLEMENT_DYNCREATE( CSendBVLCI, CPropertyPage )

#pragma warning( disable : 4355 )
CSendBVLCI::CSendBVLCI( void )
	: CSendPage( CSendBVLCI::IDD )
	, m_HeaderType(2)
	, m_OADR( this, IDC_OADRCOMBO, IDC_OADR )
{
	//{{AFX_DATA_INIT(CSendBVLCI)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendBVLCI::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendBVLCI)
	DDX_Radio(pDX, IDC_NONE, m_HeaderType);
	//}}AFX_DATA_MAP

	m_OADR.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendBVLCI, CPropertyPage)
	//{{AFX_MSG_MAP(CSendBVLCI)
	ON_BN_CLICKED(IDC_NONE, OnNone)
	ON_BN_CLICKED(IDC_DBTN, OnDistributeBroadcastToNetwork)
	ON_BN_CLICKED(IDC_UNICAST, OnOriginalUnicast)
	ON_BN_CLICKED(IDC_BROADCAST, OnOriginalBroadcast)
	ON_EN_CHANGE(IDC_OADR, OnChangeOADR)
	ON_BN_CLICKED(IDC_FORWARD, OnForwardedNPDU)
	ON_CBN_SELCHANGE(IDC_OADRCOMBO, OnSelchangeOADRCombo)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendBVLCI::OnInitDialog
//

BOOL CSendBVLCI::OnInitDialog() 
{
	TRACE0( "CSendBVLCI::OnInitDialog\n" );

	CPropertyPage::OnInitDialog();
	
	// load the combo, include broadcast addresses
	VTSDoc * pdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
	if ( pdoc )
		m_OADR.LoadCombo( pdoc->GetNames(), pageParent->m_pPort, true );
	
	return TRUE;
}

//
//	CSendBVLCI::InitPage
//

void CSendBVLCI::InitPage( void )
{
	TRACE0( "CSendBVLCI::InitPage()\n" );

	m_OADR.ctrlNull = true;
}

//
//	CSendBVLCI::EncodePage
//

void CSendBVLCI::EncodePage( CByteArray* contents )
{
	int			len = contents->GetSize() + 4
	;
	CByteArray	header
	;

	TRACE0( "CSendBVLCI::EncodePage()\n" );

	// don't bother if there's no header
	if (m_HeaderType <= 0)
		return;

	// encode the message header
	header.Add( 0x81 );
	switch (m_HeaderType) {
		case 1:						// Distribute-Broadcast-To-Network
			header.Add( 0x09 );
			header.Add( len >> 8 );
			header.Add( len & 0xFF );
			break;
		case 2:						// Original-Unicast-NPDU
			header.Add( 0x0A );
			header.Add( len >> 8 );
			header.Add( len & 0xFF );
			break;
		case 3:						// Original-Broadcast-NPDU
			header.Add( 0x0B );
			header.Add( len >> 8 );
			header.Add( len & 0xFF );
			break;
		case 4:						// Forwarded-NPDU
			// validate
			if (m_OADR.ctrlNull || (m_OADR.addrLen != 6))
				throw "Originating device address required";

			// this is a little longer
			len += 6;

			// encode the header
			header.Add( 0x04 );
			header.Add( len >> 8 );
			header.Add( len & 0xFF );
			for (int i = 0; i < m_OADR.addrLen; i++)
				header.Add( m_OADR.addrAddr[i] );
			break;
	}

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendBVLCI::SavePage
//

void CSendBVLCI::SavePage( void )
{
	TRACE0( "CSendBVLCI::SavePage\n" );

	pageContents[curHistoryIndex].Flush();

	m_OADR.SaveCtrl( pageContents[curHistoryIndex] );
	BACnetInteger( m_HeaderType ).Encode( pageContents[curHistoryIndex] );
}

//
//	CSendBVLCI::RestorePage
//

void CSendBVLCI::RestorePage( int index )
{
	TRACE0( "CSendBVLCI::RestorePage\n" );

	if(historyCount < 1)
		return;
	
	if(index > historyCount)
		return;

	index = curHistoryIndex - index - 1;
	if(index < 0)
		index = index + glMaxHistoryCount;
	
	BACnetAPDUDecoder	dec( pageContents[index] );
	BACnetInteger		hdrType;	

	if (dec.pktLength == 0)
		return;

	m_OADR.RestoreCtrl( dec );
	hdrType.Decode( dec );
	m_HeaderType = hdrType.intValue;

	if(isShown)
	{
		m_OADR.ObjToCtrl();
		UpdateData(FALSE);
	}		
}

void CSendBVLCI::OnNone() 
{
	UpdateData();
	m_OADR.Disable();

	SavePage();
	UpdateEncoded();
}

void CSendBVLCI::OnDistributeBroadcastToNetwork() 
{
	UpdateData();
	m_OADR.Disable();

	SavePage();
	UpdateEncoded();
}

void CSendBVLCI::OnOriginalUnicast() 
{
	UpdateData();
	m_OADR.Disable();

	SavePage();
	UpdateEncoded();
}

void CSendBVLCI::OnOriginalBroadcast() 
{
	UpdateData();
	m_OADR.Disable();

	SavePage();
	UpdateEncoded();
}

void CSendBVLCI::OnForwardedNPDU() 
{
	UpdateData();
	m_OADR.Enable();

	SavePage();
	UpdateEncoded();
}

void CSendBVLCI::OnChangeOADR() 
{
	m_OADR.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendBVLCI::OnSelchangeOADRCombo() 
{
	m_OADR.Selchange();
	SavePage();
	UpdateEncoded();
}

//Xiao Shiyuan 2002-12-5
void CSendBVLCI::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	isShown = bShow;

	if(bShow)
	{
		pageParent->SetHistoryComboBox(historyCount);
		pageParent->curPagePtr = this;
	}
}

//Xiao Shiyuan 2002-12-5
void CSendBVLCI::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	if(historyCount < glMaxHistoryCount)
		historyCount++;

	curHistoryIndex++;

	if(curHistoryIndex > glMaxHistoryCount - 1)
		curHistoryIndex = 0;	
}
