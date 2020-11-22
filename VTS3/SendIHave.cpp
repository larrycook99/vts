// SendIHave.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendIHave.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendIHave::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendIHave dialog

IMPLEMENT_DYNCREATE( CSendIHave, CPropertyPage )

#pragma warning( disable : 4355 )
CSendIHave::CSendIHave( void )
	: CSendPage( CSendIHave::IDD )
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_ObjectName( this, IDC_OBJECTNAME )
{
	//{{AFX_DATA_INIT(CSendIHave)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendIHave::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendIHave)
	//}}AFX_DATA_MAP

	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectName.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendIHave, CPropertyPage)
	//{{AFX_MSG_MAP(CSendIHave)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceID)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_EN_CHANGE(IDC_OBJECTNAME, OnChangeObjectName)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceIDButton)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendIHave::OnInitDialog
//

BOOL CSendIHave::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendIHave::InitPage
//

void CSendIHave::InitPage( void )
{
	// flush the data
	m_DeviceID.ctrlNull = true;
	m_ObjectID.ctrlNull = true;
	m_ObjectName.ctrlNull = true;

	// tell the NPCI this is a global broadcast
	pageParent->NPCIPage.m_DestPresent = true;

	pageParent->NPCIPage.m_DNET.ctrlNull = false;
	pageParent->NPCIPage.m_DNET.intValue = 65535;

	pageParent->NPCIPage.m_HopCount.ctrlNull = false;
	pageParent->NPCIPage.m_HopCount.intValue = 255;
}

//
//	CSendIHave::EncodePage
//

void CSendIHave::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the pdu type and service choice
	header.Add( 0x10 );
	header.Add( 0x01 );

	// encode the device ID
	if (m_DeviceID.ctrlNull)
		throw "Device ID required";
	m_DeviceID.Encode( enc );

	// encode the object ID
	if (m_ObjectID.ctrlNull)
		throw "Object ID required";
	m_ObjectID.Encode( enc );

	// encode the object name
	if (m_ObjectName.ctrlNull || (m_ObjectName.strLen == 0))
		throw "Object name required";
	m_ObjectName.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendIHave::SavePage
//

void CSendIHave::SavePage( void )
{
	TRACE0( "CSendIHave::SavePage\n" );

	pageContents.Flush();

	m_DeviceID.SaveCtrl( pageContents );
	m_ObjectID.SaveCtrl( pageContents );
	m_ObjectName.SaveCtrl( pageContents );
}

//
//	CSendIHave::RestorePage
//

void CSendIHave::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendIHave::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_DeviceID.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );
	m_ObjectName.RestoreCtrl( dec );
}

//
//	CSendIHave::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendIHave::OnChangeDeviceID()
{
	m_DeviceID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendIHave::OnDeviceIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// for proper parent control
	;

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendIHave::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendIHave::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// for proper parent control
	;

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendIHave::OnChangeObjectName()
{
	m_ObjectName.UpdateData();
	SavePage();
	UpdateEncoded();
}
