// SendNPCI.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendNPCI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Xiao Shiyuan 2002-12-5
BACnetAPDUEncoder CSendNPCI::pageContents[glMaxHistoryCount];
int CSendNPCI::curHistoryIndex = 0;
int CSendNPCI::historyCount = 0;
/////////////////////////////////////////////////////////////////////////////
// CSendNPCI dialog

IMPLEMENT_DYNCREATE( CSendNPCI, CPropertyPage )

#pragma warning( disable : 4355 )
CSendNPCI::CSendNPCI( void )
	: CSendPage( CSendNPCI::IDD )
	, m_Version( this, IDC_VERSION )
	, m_DNET( this, IDC_DNET )
	, m_DADR( this, &m_DNET, IDC_DADRCOMBO, IDC_DADR )
	, m_HopCount( this, IDC_HOPCOUNT )
	, m_SNET( this, IDC_SNET )
	, m_SADR( this, &m_SNET, IDC_SADRCOMBO, IDC_SADR )
	, m_Priority( this, IDC_PRIORITY )	
{
	//{{AFX_DATA_INIT(CSendNPCI)
	m_DestPresent = FALSE;
	m_SrcPresent = FALSE;
	m_ExpectingReply = FALSE;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendNPCI::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendNPCI)
	DDX_Check(pDX, IDC_DESTPRESENT, m_DestPresent);
	DDX_Check(pDX, IDC_SRCPRESENT, m_SrcPresent);
	DDX_Check(pDX, IDC_EXPECTINGREPLY, m_ExpectingReply);
	//}}AFX_DATA_MAP

	m_Version.UpdateData( pDX->m_bSaveAndValidate );

	if (m_DestPresent) {
		m_DNET.UpdateData( pDX->m_bSaveAndValidate );
		m_DADR.UpdateData( pDX->m_bSaveAndValidate );
		m_HopCount.UpdateData( pDX->m_bSaveAndValidate );
	}
	if (m_SrcPresent) {
		m_SNET.UpdateData( pDX->m_bSaveAndValidate );
		m_SADR.UpdateData( pDX->m_bSaveAndValidate );
	}

	m_Priority.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendNPCI, CPropertyPage)
	//{{AFX_MSG_MAP(CSendNPCI)
	ON_BN_CLICKED(IDC_DESTPRESENT, OnDestPresent)
	ON_BN_CLICKED(IDC_SRCPRESENT, OnSrcPresent)
	ON_EN_CHANGE(IDC_VERSION, OnChangeVersion)
	ON_EN_CHANGE(IDC_DNET, OnChangeDNET)
	ON_EN_CHANGE(IDC_DADR, OnChangeDADR)
	ON_EN_CHANGE(IDC_HOPCOUNT, OnChangeHopCount)
	ON_EN_CHANGE(IDC_SNET, OnChangeSNET)
	ON_EN_CHANGE(IDC_SADR, OnChangeSADR)
	ON_BN_CLICKED(IDC_EXPECTINGREPLY, OnExpectingReply)
	ON_EN_CHANGE(IDC_PRIORITY, OnChangePriority)
	ON_CBN_SELCHANGE(IDC_DADRCOMBO, OnSelchangeDADRCombo)
	ON_CBN_SELCHANGE(IDC_SADRCOMBO, OnSelchangeSADRCombo)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendNPCI::OnInitDialog
//

BOOL CSendNPCI::OnInitDialog() 
{
	TRACE0( "CSendNPCI::OnInitDialog\n" );

	CDialog::OnInitDialog();
	
//	m_Version.ObjToCtrl();

	VTSDoc * pdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();

	if ( pdoc )
	{
		// Allow broadcast destination
		m_DADR.LoadCombo( pdoc->GetNames(), pageParent->m_pPort, true );
		m_SADR.LoadCombo( pdoc->GetNames(), pageParent->m_pPort );
	}

	if (m_DestPresent) {
		m_DNET.ObjToCtrl();
		m_DADR.ObjToCtrl();
		m_HopCount.ObjToCtrl();
	}
	if (m_SrcPresent) {
		m_SNET.ObjToCtrl();
		m_SADR.ObjToCtrl();
	}

//	m_Priority.ObjToCtrl();

	// let the contorls show correct status
	SynchronizeControls();

	return TRUE;
}

//
//	CSendNPCI::OnDestPresent
//

void CSendNPCI::OnDestPresent() 
{
	UpdateData();

	// if the hop count hasn't been provided, give it a default value
	if (m_DestPresent && m_HopCount.ctrlNull) {
		m_HopCount.ctrlNull = false;
		m_HopCount.intValue = 255;
		m_HopCount.ObjToCtrl();
	}

	UpdateEncoded();
	SynchronizeControls();
}

//
//	CSendNPCI::OnSrcPresent
//

void CSendNPCI::OnSrcPresent() 
{
	UpdateData();
	UpdateEncoded();
	SynchronizeControls();
}

//
//	CSendNPCI::SynchronizeControls
//

void CSendNPCI::SynchronizeControls()
{
	// destination windows
	GetDlgItem( IDC_DNET )->EnableWindow( m_DestPresent );
	GetDlgItem( IDC_DADR )->EnableWindow( m_DestPresent );
	GetDlgItem( IDC_DADRCOMBO )->EnableWindow( m_DestPresent );
	GetDlgItem( IDC_HOPCOUNT )->EnableWindow( m_DestPresent );

	// source windows
	GetDlgItem( IDC_SNET )->EnableWindow( m_SrcPresent );
	GetDlgItem( IDC_SADR )->EnableWindow( m_SrcPresent );
	GetDlgItem( IDC_SADRCOMBO )->EnableWindow( m_SrcPresent );
}

//
//	CSendNPCI::InitPage
//

void CSendNPCI::InitPage( void )
{
	TRACE0( "CSendNPCI::InitPage()\n" );

	// assume this is *NOT* a network layer header
	m_isNetMessage = false;

	// version defaults to 1
	m_Version.ctrlNull = false;
	m_Version.intValue = 1;

	// flush the address fields
	m_DNET.ctrlNull = true;
	m_DADR.ctrlNull = true;
	m_SNET.ctrlNull = true;
	m_SADR.ctrlNull = true;

	// hop count defaults to 255
	m_HopCount.ctrlNull = false;
	m_HopCount.intValue = 255;

	// assume nothing is coming back
	m_ExpectingReply = false;

	// priority defaults to zero
	m_Priority.ctrlNull = false;
	m_Priority.intValue = 0;
}

//
//	CSendNPCI::EncodePage
//

void CSendNPCI::EncodePage( CByteArray *contents )
{
	BACnetOctet	ctrl = 0
	;
	CByteArray	header
	;

	TRACE0( "CSendNPCI::EncodePage()\n" );

	// validate version
	if (m_Version.ctrlNull)
		throw "Version required";
	if ((m_Version.intValue < 0) || (m_Version.intValue > 255))
		throw "Version out of range 0..255";

	if (m_DestPresent) {
		if (m_DNET.ctrlNull)
			throw "Destination network required (65535 = global broadcast)";
		if ((m_DNET.intValue < 0) || (m_DNET.intValue > 65535))
			throw "Destination network out of range 0..65535";
		if (m_HopCount.ctrlNull)
			throw "Hop count required (normally 255)";
		if ((m_HopCount.intValue < 0) || (m_HopCount.intValue > 255))
			throw "Hop count out of range 0..255";
	}
	if (m_SrcPresent) {
		if (m_SNET.ctrlNull)
			throw "Source network required";
		if ((m_SNET.intValue < 0) || (m_SNET.intValue > 65534))
			throw "Source network out of range 0..65534";
		if (m_SADR.ctrlNull)
			throw "Source address required";
	}

	// validate priority
	if (m_Priority.ctrlNull)
		throw "Priority required";
	if ((m_Priority.intValue < 0) || (m_Priority.intValue > 3))
		throw "Priority out of range 0..3";

	// start with the version
	header.Add( m_Version.intValue );

	// build the control field
	if (m_isNetMessage)
		ctrl |= 0x80;
	if (m_DestPresent)
		ctrl |= 0x20;
	if (m_SrcPresent)
		ctrl |= 0x08;
	if (m_ExpectingReply)
		ctrl |= 0x04;
	ctrl |= m_Priority.intValue;

	// add the control field
	header.Add( ctrl );

	// add the destination stuff
	if (m_DestPresent) {
		header.Add( m_DNET.intValue >> 8 );
		header.Add( m_DNET.intValue & 0x0FF );
		if (!m_DADR.ctrlNull) {
			header.Add( (unsigned char)m_DADR.addrLen );
			for (int i = 0; i < m_DADR.addrLen; i++)
				header.Add( m_DADR.addrAddr[i] );
		} else
			header.Add( 0x00 );
	}

	// add the source stuff
	if (m_SrcPresent) {
		header.Add( m_SNET.intValue >> 8 );
		header.Add( m_SNET.intValue & 0x0FF );
		header.Add( (unsigned char)m_SADR.addrLen );
		for (int i = 0; i < m_SADR.addrLen; i++)
			header.Add( m_SADR.addrAddr[i] );
	}

	// add the hop count
	if (m_DestPresent)
		header.Add( m_HopCount.intValue );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendNPCI::SavePage
//

void CSendNPCI::SavePage( void )
{
	TRACE0( "CSendNPCI::SavePage\n" );

	pageContents[curHistoryIndex].Flush();

	m_Version.SaveCtrl( pageContents[curHistoryIndex] );
	m_DNET.SaveCtrl( pageContents[curHistoryIndex] );
	m_DADR.SaveCtrl( pageContents[curHistoryIndex] );
	m_HopCount.SaveCtrl( pageContents[curHistoryIndex] );
	m_SNET.SaveCtrl( pageContents[curHistoryIndex] );
	m_SADR.SaveCtrl( pageContents[curHistoryIndex] );
	m_Priority.SaveCtrl( pageContents[curHistoryIndex] );
}

//
//	CSendNPCI::RestorePage
//

void CSendNPCI::RestorePage( int index )
{
	TRACE0( "CSendNPCI::RestorePage\n" );

	if(historyCount < 1)
		return;
	
	if(index > historyCount)
		return;

	index = curHistoryIndex - index - 1;
	if(index < 0)
		index = index + glMaxHistoryCount;

	BACnetAPDUDecoder	dec( pageContents[index] );

	

	if (dec.pktLength == 0)
		return;

	m_Version.RestoreCtrl( dec );		
	m_DNET.RestoreCtrl( dec );
	m_DADR.RestoreCtrl( dec );
	m_HopCount.RestoreCtrl( dec );
	m_SNET.RestoreCtrl( dec );
	m_SADR.RestoreCtrl( dec );
	m_Priority.RestoreCtrl( dec );

	if(isShown)
	{
		m_Version.ObjToCtrl();
		m_DNET.ObjToCtrl();
		m_DADR.ObjToCtrl();
		m_HopCount.ObjToCtrl();
		m_SNET.ObjToCtrl();
		m_SADR.ObjToCtrl();
		m_Priority.ObjToCtrl();
	}	 
}

void CSendNPCI::OnChangeVersion() 
{
	TRACE0( "CSendNPCI::OnChangeVersion()\n" );
	m_Version.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendNPCI::OnChangeDNET() 
{
	TRACE0( "CSendNPCI::OnChangeDNET()\n" );
	m_DNET.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendNPCI::OnChangeDADR() 
{
	TRACE0( "CSendNPCI::OnChangeDADR()\n" );
	m_DADR.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendNPCI::OnSelchangeDADRCombo() 
{
	m_DADR.Selchange();
	SavePage();
	UpdateEncoded();
}

void CSendNPCI::OnChangeHopCount() 
{
	TRACE0( "CSendNPCI::OnChangeHopCount()\n" );
	m_HopCount.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendNPCI::OnChangeSNET() 
{
	TRACE0( "CSendNPCI::OnChangeSNET()\n" );
	m_SNET.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendNPCI::OnChangeSADR() 
{
	TRACE0( "CSendNPCI::OnChangeSADR()\n" );
	m_SADR.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendNPCI::OnSelchangeSADRCombo() 
{
	m_SADR.Selchange();
	SavePage();
	UpdateEncoded();
}
void CSendNPCI::OnExpectingReply() 
{
	TRACE0( "CSendNPCI::OnExpectingReply()\n" );
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendNPCI::OnChangePriority() 
{
	TRACE0( "CSendNPCI::OnChangePriority()\n" );
	m_Priority.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendNPCI::OnShowWindow(BOOL bShow, UINT nStatus) 
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

void CSendNPCI::OnDestroy() 
{
	CPropertyPage::OnDestroy();		

	// TODO: Add your message handler code here
	if(historyCount < glMaxHistoryCount)
		historyCount++;

	curHistoryIndex++;	

	if(curHistoryIndex > glMaxHistoryCount - 1)
		curHistoryIndex = 0;					
}
