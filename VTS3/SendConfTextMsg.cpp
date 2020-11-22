// SendConfTextMsg.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendConfTextMsg.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendConfTextMsg::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendConfTextMsg property page

IMPLEMENT_DYNCREATE( CSendConfTextMsg, CPropertyPage )

#pragma warning( disable : 4355 )
CSendConfTextMsg::CSendConfTextMsg( void )
	: CSendPage( CSendConfTextMsg::IDD )
	, m_SourceDevice( this, IDC_SOURCEDEVICE )
	, m_NumClass( this, IDC_NUMCLASS )
	, m_CharClass( this, IDC_CHARCLASS )
	, m_Msg( this, IDC_MSG )
{
	//{{AFX_DATA_INIT(CSendConfTextMsg)
	m_Priority = 0;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendConfTextMsg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendConfTextMsg)
	DDX_Radio(pDX, IDC_NORMAL, m_Priority);
	//}}AFX_DATA_MAP

	m_SourceDevice.UpdateData( pDX->m_bSaveAndValidate );
	m_NumClass.UpdateData( pDX->m_bSaveAndValidate );
	m_CharClass.UpdateData( pDX->m_bSaveAndValidate );
	m_Msg.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendConfTextMsg, CPropertyPage)
	//{{AFX_MSG_MAP(CSendConfTextMsg)
	ON_EN_CHANGE(IDC_SOURCEDEVICE, OnChangeSourceDevice)
	ON_BN_CLICKED(IDC_SOURCEDEVICEBTN, OnSourceDeviceButton)
	ON_EN_CHANGE(IDC_NUMCLASS, OnChangeNumClass)
	ON_EN_CHANGE(IDC_CHARCLASS, OnChangeCharClass)
	ON_EN_CHANGE(IDC_MSG, OnChangeMsg)
	ON_BN_CLICKED(IDC_NORMAL, OnNormal)
	ON_BN_CLICKED(IDC_URGENT, OnUrgent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendConfTextMsg::OnInitDialog
//

BOOL CSendConfTextMsg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendConfTextMsg::InitPage
//

void CSendConfTextMsg::InitPage( void )
{
	TRACE0( "CSendConfTextMsg::InitPage()\n" );

	// flush the data
	m_SourceDevice.ctrlNull = true;
	m_NumClass.ctrlNull = true;
	m_CharClass.ctrlNull = true;
	m_Msg.ctrlNull = true;
}

//
//	CSendConfTextMsg::EncodePage
//

void CSendConfTextMsg::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendConfTextMsg::EncodePage()\n" );

	// encode the service choice
	header.Add( 19 );

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
//	CSendConfTextMsg::SavePage
//

void CSendConfTextMsg::SavePage( void )
{
	TRACE0( "CSendConfTextMsg::SavePage\n" );

	pageContents.Flush();

	m_SourceDevice.SaveCtrl( pageContents );
	m_NumClass.SaveCtrl( pageContents );
	m_CharClass.SaveCtrl( pageContents );
	m_Msg.SaveCtrl( pageContents );

	BACnetEnumerated( m_Priority ).Encode( pageContents );
}

//
//	CSendConfTextMsg::RestorePage
//

void CSendConfTextMsg::RestorePage(  int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;
	BACnetEnumerated	enumValue
	;

	TRACE0( "CSendConfTextMsg::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_SourceDevice.RestoreCtrl( dec );
	m_NumClass.RestoreCtrl( dec );
	m_CharClass.RestoreCtrl( dec );
	m_Msg.RestoreCtrl( dec );

	enumValue.Decode( dec );
	m_Priority = enumValue.m_enumValue;
}

//
//	CSendConfTextMsg::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendConfTextMsg::OnChangeSourceDevice()
{
	TRACE0( "CSendIHave::OnChangeSourceDevice()\n" );
	m_SourceDevice.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfTextMsg::OnSourceDeviceButton() 
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

void CSendConfTextMsg::OnChangeNumClass()
{
	TRACE0( "CSendIHave::OnChangeNumClass()\n" );
	m_NumClass.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfTextMsg::OnChangeCharClass()
{
	TRACE0( "CSendIHave::OnChangeCharClass()\n" );
	m_CharClass.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfTextMsg::OnChangeMsg()
{
	TRACE0( "CSendIHave::OnChangeMsg()\n" );
	m_Msg.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfTextMsg::OnNormal() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfTextMsg::OnUrgent() 
{
	UpdateData();
	SavePage();
	UpdateEncoded();
}
