// SendConfEventNotification.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendConfEventNotification.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendConfEventNotification::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendConfEventNotification dialog

IMPLEMENT_DYNCREATE( CSendConfEventNotification, CPropertyPage )

#pragma warning( disable : 4355 )
CSendConfEventNotification::CSendConfEventNotification( void )
	: CSendPage( CSendConfEventNotification::IDD )
	, m_ProcessID( this, IDC_PROCESSID )
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_NotifyClass( this, IDC_NOTIFYCLASS )
	, m_Priority( this, IDC_PRIORITY )
	, m_MessageText( this, IDC_MESSAGETEXT )
	, m_NotifyTypeCombo( this, IDC_NOTIFYTYPECOMBO, NetworkSniffer::BAC_STRTAB_BACnetNotifyType, true )
	, m_AckRequired( this, IDC_ACKREQUIRED, true )
	, m_FromStateCombo( this, IDC_FROMSTATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetEventState, true )
	, m_ToStateCombo( this, IDC_TOSTATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetEventState, true )
	, m_EventValues( "Notification Parameters", this )			// added parent for proper send dlg function
	, m_TimeStamp(this)			// added parent for proper send dlg function
{
	//{{AFX_DATA_INIT(CSendConfEventNotification)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendConfEventNotification::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendConfEventNotification::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendConfEventNotification)
	//}}AFX_DATA_MAP

	m_ProcessID.UpdateData( pDX->m_bSaveAndValidate );
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_NotifyClass.UpdateData( pDX->m_bSaveAndValidate );
	m_Priority.UpdateData( pDX->m_bSaveAndValidate );
	m_MessageText.UpdateData( pDX->m_bSaveAndValidate );
	m_NotifyTypeCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_AckRequired.UpdateData( pDX->m_bSaveAndValidate );
	m_FromStateCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ToStateCombo.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendConfEventNotification, CPropertyPage)
	//{{AFX_MSG_MAP(CSendConfEventNotification)
	ON_EN_CHANGE(IDC_PROCESSID, OnChangeProcessID)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceID)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_BN_CLICKED(IDC_TIMESTAMP, OnTimeStamp)
	ON_EN_CHANGE(IDC_NOTIFYCLASS, OnChangeNotifyClass)
	ON_EN_CHANGE(IDC_PRIORITY, OnChangePriority)
	ON_EN_CHANGE(IDC_MESSAGETEXT, OnChangeMessageText)
	ON_CBN_SELCHANGE(IDC_NOTIFYTYPECOMBO, OnSelchangeNotifyTypeCombo)
	ON_BN_CLICKED(IDC_ACKREQUIRED, OnAckRequired)
	ON_CBN_SELCHANGE(IDC_FROMSTATECOMBO, OnSelchangeFromStateCombo)
	ON_CBN_SELCHANGE(IDC_TOSTATECOMBO, OnSelchangeToStateCombo)
	ON_BN_CLICKED(IDC_EVENTVALUES, OnEventValues)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceIDButton)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendConfEventNotification::OnInitDialog
//

BOOL CSendConfEventNotification::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the combos
	m_NotifyTypeCombo.LoadCombo();
	m_FromStateCombo.LoadCombo();
	m_ToStateCombo.LoadCombo();

	return TRUE;
}

//
//	CSendConfEventNotification::InitPage
//

void CSendConfEventNotification::InitPage( void )
{
	TRACE0( "CSendConfEventNotification::InitPage()\n" );

	// flush the data
	m_ProcessID.ctrlNull = true;
	m_DeviceID.ctrlNull = true;
	m_ObjectID.ctrlNull = true;
	m_NotifyClass.ctrlNull = true;
	m_Priority.ctrlNull = true;
	m_MessageText.ctrlNull = true;
	m_AckRequired.ctrlNull = true;
}

//
//	CSendConfEventNotification::EncodePage
//

void CSendConfEventNotification::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 2 );

	// encode the parameters
	if (m_ProcessID.ctrlNull)
		throw "Process ID required";
	m_ProcessID.Encode( enc, 0 );

	if (m_DeviceID.ctrlNull)
		throw "Initiating device ID required";
	m_DeviceID.Encode( enc, 1 );

	if (m_ObjectID.ctrlNull)
		throw "Object ID required";
	m_ObjectID.Encode( enc, 2 );

	m_TimeStamp.Encode( enc, 3 );

	if (m_NotifyClass.ctrlNull)
		throw "Notification class required";
	if (m_NotifyClass.uintValue > 4194303)
		throw "Notification class out of range (0..4194303)";
	m_NotifyClass.Encode( enc, 4 );

	if (m_Priority.ctrlNull)
		throw "Priority required";
	if (m_Priority.uintValue > 255)
		throw "Priority out of range (0..255)";
	m_Priority.Encode( enc, 5 );

	// the event type and values are specified together
	if (m_EventValues.ctrlNull)
		throw "Event type and values required";

	BACnetEnumerated(m_EventValues.m_EventType).Encode( enc, 6 );

	if (!m_MessageText.ctrlNull)
		m_MessageText.Encode( enc, 7 );

	if (m_NotifyTypeCombo.ctrlNull)
		throw "Notify type required";
	m_NotifyTypeCombo.Encode( enc, 8 );

	if (!m_AckRequired.ctrlNull)
		m_AckRequired.Encode( enc, 9 );

	if (!m_FromStateCombo.ctrlNull)
		m_FromStateCombo.Encode( enc, 10 );

	if (!m_ToStateCombo.ctrlNull)
		m_ToStateCombo.Encode( enc, 11 );

	// encode the event type specific parameters
	m_EventValues.Encode( enc, 12 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendConfEventNotification::SavePage
//

void CSendConfEventNotification::SavePage( void )
{
	TRACE0( "CSendConfEventNotification::SavePage\n" );

	pageContents.Flush();

	m_ProcessID.SaveCtrl( pageContents );
	m_DeviceID.SaveCtrl( pageContents );
	m_ObjectID.SaveCtrl( pageContents );
	m_TimeStamp.SaveCtrl( pageContents );
	m_NotifyClass.SaveCtrl( pageContents );
	m_Priority.SaveCtrl( pageContents );
	m_MessageText.SaveCtrl( pageContents );
	m_NotifyTypeCombo.SaveCtrl( pageContents );
	m_AckRequired.SaveCtrl( pageContents );
	m_FromStateCombo.SaveCtrl( pageContents );
	m_ToStateCombo.SaveCtrl( pageContents );
//	m_EventValues.SaveCtrl( pageContents );
}

//
//	CSendConfEventNotification::RestorePage
//

void CSendConfEventNotification::RestorePage(  int index  )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendConfEventNotification::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ProcessID.RestoreCtrl( dec );
	m_DeviceID.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );
	m_TimeStamp.RestoreCtrl( dec );
	m_NotifyClass.RestoreCtrl( dec );
	m_Priority.RestoreCtrl( dec );
	m_MessageText.RestoreCtrl( dec );
	m_NotifyTypeCombo.RestoreCtrl( dec );
	m_AckRequired.RestoreCtrl( dec );
	m_FromStateCombo.RestoreCtrl( dec );
	m_ToStateCombo.RestoreCtrl( dec );
//	m_EventValues.RestoreCtrl( dec );
}

//
//	CSendConfEventNotification::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

afx_msg void CSendConfEventNotification::OnChangeProcessID()
{
	m_ProcessID.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendConfEventNotification::OnChangeDeviceID()
{
	m_DeviceID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfEventNotification::OnDeviceIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// added parent for proper send dlg function
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

afx_msg void CSendConfEventNotification::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendConfEventNotification::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// added parent for proper send dlg function
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

afx_msg void CSendConfEventNotification::OnTimeStamp()
{
	m_TimeStamp.DoModal();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendConfEventNotification::OnChangeNotifyClass()
{
	m_NotifyClass.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendConfEventNotification::OnChangePriority()
{
	m_Priority.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendConfEventNotification::OnChangeMessageText()
{
	m_MessageText.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendConfEventNotification::OnSelchangeNotifyTypeCombo()
{
	m_NotifyTypeCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendConfEventNotification::OnAckRequired()
{
	m_AckRequired.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendConfEventNotification::OnSelchangeFromStateCombo()
{
	m_FromStateCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendConfEventNotification::OnSelchangeToStateCombo()
{
	m_ToStateCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendConfEventNotification::OnEventValues()
{
	m_EventValues.DoModal();
	SavePage();
	UpdateEncoded();
}
