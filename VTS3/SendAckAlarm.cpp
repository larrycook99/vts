// SendAckAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendAckAlarm.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder	CSendAckAlarm::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendAckAlarm dialog

IMPLEMENT_DYNCREATE( CSendAckAlarm, CPropertyPage )

#pragma warning( disable : 4355 )
CSendAckAlarm::CSendAckAlarm( void )
	: CSendPage( CSendAckAlarm::IDD )
	, m_AckProcessID( this, IDC_ACKPROCESSID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_EventStateCombo( this, IDC_EVENTSTATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetEventState, true )
	, m_AckSource( this, IDC_ACKSOURCE )
	, m_TimeStamp(this)			// added parent wnd for modeless send dlg
	, m_TimeOfAck(this)			// added parent wnd for modeless send dlg
{
	//{{AFX_DATA_INIT(CSendAckAlarm)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendAckAlarm::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendAckAlarm::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendAckAlarm)
	//}}AFX_DATA_MAP

	m_AckProcessID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_EventStateCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_AckSource.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendAckAlarm, CPropertyPage)
	//{{AFX_MSG_MAP(CSendAckAlarm)
	ON_EN_CHANGE(IDC_ACKPROCESSID, OnChangeAckProcessID)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_BN_CLICKED(IDC_OBJIDBTN, OnObjIDButton)
	ON_CBN_SELCHANGE(IDC_EVENTSTATECOMBO, OnSelchangeEventStateCombo)
	ON_BN_CLICKED(IDC_TIMESTAMP, OnTimeStamp)
	ON_EN_CHANGE(IDC_ACKSOURCE, OnChangeAckSource)
	ON_BN_CLICKED(IDC_TIMEOFACK, OnTimeOfAck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendAckAlarm::OnInitDialog
//

BOOL CSendAckAlarm::OnInitDialog() 
{
	TRACE0( "CSendAckAlarm::OnInitDialog()\n" );

	CPropertyPage::OnInitDialog();
	
	// load the combo
	m_EventStateCombo.LoadCombo();

	return TRUE;
}

//
//	CSendAckAlarm::InitPage
//

void CSendAckAlarm::InitPage( void )
{
	TRACE0( "CSendAckAlarm::InitPage()\n" );

	// flush the data
	m_AckProcessID.ctrlNull = true;
	m_ObjectID.ctrlNull = true;
//	m_EventStateCombo.ctrlNull = true;
	m_AckSource.ctrlNull = true;
}

//
//	CSendAckAlarm::EncodePage
//

void CSendAckAlarm::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 0 );

	// encode the parameters
	if (m_AckProcessID.ctrlNull)
		throw "Acknowledging process ID required";
	m_AckProcessID.Encode( enc, 0 );

	if (m_ObjectID.ctrlNull)
		throw "Event object ID required";
	m_ObjectID.Encode( enc, 1 );

	if (m_EventStateCombo.ctrlNull)
		throw "Event state acknowledged required";
	m_EventStateCombo.Encode( enc, 2 );

	m_TimeStamp.Encode( enc, 3 );

	if (m_AckSource.ctrlNull)
		throw "Acknowedgment source required";
	m_AckSource.Encode( enc, 4 );

	m_TimeOfAck.Encode( enc, 5 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendAckAlarm::SavePage
//

void CSendAckAlarm::SavePage( void )
{
	TRACE0( "CSendAckAlarm::SavePage\n" );

	pageContents.Flush();

	m_AckProcessID.SaveCtrl( pageContents );
	m_ObjectID.SaveCtrl( pageContents );
	m_EventStateCombo.SaveCtrl( pageContents );
	m_TimeStamp.SaveCtrl( pageContents );
	m_AckSource.SaveCtrl( pageContents );
	m_TimeOfAck.SaveCtrl( pageContents );
}

//
//	CSendAckAlarm::RestorePage
//

void CSendAckAlarm::RestorePage(  int index  )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendAckAlarm::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_AckProcessID.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );
	m_EventStateCombo.RestoreCtrl( dec );
	m_TimeStamp.RestoreCtrl( dec );
	m_AckSource.RestoreCtrl( dec );
	m_TimeOfAck.RestoreCtrl( dec );
}

//
//	CSendAckAlarm::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

afx_msg void CSendAckAlarm::OnChangeAckProcessID()
{
	m_AckProcessID.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendAckAlarm::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendAckAlarm::OnObjIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)	// added parent wnd for modeless send dlg
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

afx_msg void CSendAckAlarm::OnSelchangeEventStateCombo()
{
	m_EventStateCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendAckAlarm::OnTimeStamp()
{
	m_TimeStamp.DoModal();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendAckAlarm::OnChangeAckSource()
{
	m_AckSource.UpdateData();
	SavePage();
	UpdateEncoded();
}

afx_msg void CSendAckAlarm::OnTimeOfAck()
{
	m_TimeOfAck.DoModal();
	SavePage();
	UpdateEncoded();
}
