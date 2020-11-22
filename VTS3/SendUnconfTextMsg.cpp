// SendUnconfTextMsg.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendUnconfTextMsg.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendUnconfTextMsg::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendUnconfTextMsg property page

IMPLEMENT_DYNCREATE( CSendUnconfTextMsg, CPropertyPage )

#pragma warning( disable : 4355 )
CSendUnconfTextMsg::CSendUnconfTextMsg( void )
	: CSendPage( CSendUnconfTextMsg::IDD )
	, m_SourceDevice( this, IDC_SOURCEDEVICE )
	, m_NumClass( this, IDC_NUMCLASS )
	, m_CharClass( this, IDC_CHARCLASS )
	, m_Msg( this, IDC_MSG )
{
	//{{AFX_DATA_INIT(CSendUnconfTextMsg)
	m_Priority = 0;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendUnconfTextMsg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendUnconfTextMsg)
	DDX_Radio(pDX, IDC_NORMAL, m_Priority);
	//}}AFX_DATA_MAP

	m_SourceDevice.UpdateData( pDX->m_bSaveAndValidate );
	m_NumClass.UpdateData( pDX->m_bSaveAndValidate );
	m_CharClass.UpdateData( pDX->m_bSaveAndValidate );
	m_Msg.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendUnconfTextMsg, CPropertyPage)
	//{{AFX_MSG_MAP(CSendUnconfTextMsg)
	ON_EN_CHANGE(IDC_SOURCEDEVICE, OnChangeSourceDevice)
	ON_EN_CHANGE(IDC_NUMCLASS, OnChangeNumClass)
	ON_EN_CHANGE(IDC_CHARCLASS, OnChangeCharClass)
	ON_EN_CHANGE(IDC_MSG, OnChangeMsg)
	ON_BN_CLICKED(IDC_NORMAL, OnNormal)
	ON_BN_CLICKED(IDC_URGENT, OnUrgent)
	ON_BN_CLICKED(IDC_SOURCEDEVICEBTN, OnSourceDeviceButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendUnconfTextMsg::OnInitDialog
//

BOOL CSendUnconfTextMsg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendUnconfTextMsg::InitPage
//

void CSendUnconfTextMsg::InitPage( void )
{
	TRACE0( "CSendUnconfTextMsg::InitPage()\n" );

	// flush the data
	m_SourceDevice.ctrlNull = true;
	m_NumClass.ctrlNull = true;
	m_CharClass.ctrlNull = true;
	m_Msg.ctrlNull = true;
}

//
//	CSendUnconfTextMsg::EncodePage
//

void CSendUnconfTextMsg::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendUnconfTextMsg::EncodePage()\n" );

	// encode the pdu type and service choice
	header.Add( 0x10 );
	header.Add( 0x05 );

	// encode the device ID
	if (m_SourceDevice.ctrlNull)
		throw "Source device required";
	m_SourceDevice.Encode( enc, 0 );

	// encode the object ID
	if (!m_NumClass.ctrlNull) {
		if (!m_CharClass.ctrlNull)
			throw "Numeric or character message class";

		// context tag wrapper
		BACnetOpeningTag().Encode( enc, 1 );
		m_NumClass.Encode( enc, 0 );
		BACnetClosingTag().Encode( enc, 1 );
	} else
	if (!m_CharClass.ctrlNull) {
		// context tag wrapper
		BACnetOpeningTag().Encode( enc, 1 );
		m_CharClass.Encode( enc, 1 );
		BACnetClosingTag().Encode( enc, 1 );
	} else
		;

	// encode the priority
	BACnetEnumerated( m_Priority ).Encode( enc, 2 );

	// encode the message
	if (m_Msg.ctrlNull || (m_Msg.strLen == 0))
		throw "Message required";
	m_Msg.Encode( enc, 3 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendUnconfTextMsg::SavePage
//

void CSendUnconfTextMsg::SavePage( void )
{
	TRACE0( "CSendUnconfTextMsg::SavePage\n" );

	pageContents.Flush();

	BACnetInteger( m_Priority ).Encode( pageContents );
	m_SourceDevice.SaveCtrl( pageContents );
	m_NumClass.SaveCtrl( pageContents );
	m_CharClass.SaveCtrl( pageContents );
	m_Msg.SaveCtrl( pageContents );
}

//
//	CSendUnconfTextMsg::RestorePage
//

void CSendUnconfTextMsg::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;
	BACnetInteger		intValue
	;

	TRACE0( "CSendUnconfTextMsg::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	intValue.Decode( dec );
	m_Priority = intValue.intValue;

	m_SourceDevice.RestoreCtrl( dec );
	m_NumClass.RestoreCtrl( dec );
	m_CharClass.RestoreCtrl( dec );
	m_Msg.RestoreCtrl( dec );
}

//
//	CSendUnconfTextMsg::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendUnconfTextMsg::OnChangeSourceDevice()
{
	TRACE0( "CSendIHave::OnChangeSourceDevice()\n" );
	m_SourceDevice.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendUnconfTextMsg::OnSourceDeviceButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// for proper parent control
	;

	dlg.objID = m_SourceDevice.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_SourceDevice.ctrlNull = false;
		m_SourceDevice.objID = dlg.objID;
		m_SourceDevice.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendUnconfTextMsg::OnChangeNumClass()
{
	TRACE0( "CSendIHave::OnChangeNumClass()\n" );
	m_NumClass.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendUnconfTextMsg::OnChangeCharClass()
{
	TRACE0( "CSendIHave::OnChangeCharClass()\n" );
	m_CharClass.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendUnconfTextMsg::OnChangeMsg()
{
	TRACE0( "CSendIHave::OnChangeMsg()\n" );
	m_Msg.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendUnconfTextMsg::OnNormal() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendUnconfTextMsg::OnUrgent() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}
