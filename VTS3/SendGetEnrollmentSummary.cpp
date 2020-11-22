// SendGetEnrollmentSummary.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendGetEnrollmentSummary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder	CSendGetEnrollmentSummary::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendGetEnrollmentSummary dialog

IMPLEMENT_DYNCREATE( CSendGetEnrollmentSummary, CPropertyPage )

#pragma warning( disable : 4355 )
CSendGetEnrollmentSummary::CSendGetEnrollmentSummary( void )
	: CSendPage( CSendGetEnrollmentSummary::IDD )
	, m_AckFilterCombo( this, IDC_ACKFILTERCOMBO, NetworkSniffer::BAC_STRTAB_Acknowledgement_Filter, true )
	, m_RecipientProcess( this )
	, m_EventStateCombo( this, IDC_EVENTSTATECOMBO, NetworkSniffer::BAC_STRTAB_EventState_Filter, true )
	, m_EventTypeCombo( this, IDC_EVENTTYPECOMBO, NetworkSniffer::BAC_STRTAB_BACnetEventType, true )
	, m_MinPriority( this, IDC_MINPRIORITY )
	, m_MaxPriority( this, IDC_MAXPRIORITY )
	, m_NotificationClass( this, IDC_NOTIFICATIONCLASS )
{
	//{{AFX_DATA_INIT(CSendGetEnrollmentSummary)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendGetEnrollmentSummary::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendGetEnrollmentSummary::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendGetEnrollmentSummary)
	//}}AFX_DATA_MAP

	m_AckFilterCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_EventStateCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_EventTypeCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_MinPriority.UpdateData( pDX->m_bSaveAndValidate );
	m_MaxPriority.UpdateData( pDX->m_bSaveAndValidate );
	m_NotificationClass.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendGetEnrollmentSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CSendGetEnrollmentSummary)
	ON_CBN_SELCHANGE(IDC_ACKFILTERCOMBO, OnSelchangeAckFilterCombo)
	ON_BN_CLICKED(IDC_RECIPIENTPROCESS, OnRecipientProcess)
	ON_CBN_SELCHANGE(IDC_EVENTSTATECOMBO, OnSelchangeEventStateCombo)
	ON_CBN_SELCHANGE(IDC_EVENTTYPECOMBO, OnSelchangeEventTypeCombo)
	ON_EN_CHANGE(IDC_MINPRIORITY, OnChangeMinPriority)
	ON_EN_CHANGE(IDC_MAXPRIORITY, OnChangeMaxPriority)
	ON_EN_CHANGE(IDC_NOTIFICATIONCLASS, OnChangeNotificationClass)
	//}}AFX_MSG_MAP
	ON_CBN_EDITCHANGE(IDC_EVENTSTATECOMBO, OnChangeEventStateCombo)
	ON_CBN_EDITCHANGE(IDC_EVENTTYPECOMBO, OnChangeEventTypeCombo)
	ON_CBN_EDITCHANGE(IDC_ACKFILTERCOMBO, OnChangeAckFilterCombo)
END_MESSAGE_MAP()

//
//	CSendGetEnrollmentSummary::OnInitDialog
//

BOOL CSendGetEnrollmentSummary::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration tables
	m_AckFilterCombo.LoadCombo();
	m_EventStateCombo.LoadCombo();
	m_EventTypeCombo.LoadCombo();
	
	return TRUE;
}

//
//	CSendGetEnrollmentSummary::InitPage
//

void CSendGetEnrollmentSummary::InitPage( void )
{
	// flush the data
	m_MinPriority.ctrlNull = true;
	m_MaxPriority.ctrlNull = true;
	m_NotificationClass.ctrlNull = true;
}

//
//	CSendGetEnrollmentSummary::EncodePage
//

void CSendGetEnrollmentSummary::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 4 );

	if (m_AckFilterCombo.ctrlNull)
		throw "Acknowledgement filter required";
	m_AckFilterCombo.Encode( enc, 0 );

	if (!m_RecipientProcess.ctrlNull)
		m_RecipientProcess.Encode( enc, 1 );

	if (!m_EventStateCombo.ctrlNull)
		m_EventStateCombo.Encode( enc, 2 );

	if (!m_EventTypeCombo.ctrlNull)
		m_EventTypeCombo.Encode( enc, 3 );

	if (!m_MinPriority.ctrlNull || !m_MaxPriority.ctrlNull) {
		if (m_MinPriority.ctrlNull || m_MaxPriority.ctrlNull)
			throw "Minimum and maximum priority must be specified together";

		if (m_MinPriority.uintValue > 255)
			throw "Minimum priority out of range (0..255)";
		if (m_MaxPriority.uintValue > 255)
			throw "Maximum priority out of range (0..255)";

		if (m_MinPriority.uintValue > m_MinPriority.uintValue)
			throw "Minimum priority must be less than or equal to the maximum";

		BACnetOpeningTag().Encode( enc, 4 );
		m_MinPriority.Encode( enc, 0 );
		m_MaxPriority.Encode( enc, 1 );
		BACnetClosingTag().Encode( enc, 4 );
	}

	if (!m_NotificationClass.ctrlNull)
		m_NotificationClass.Encode( enc, 5 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendGetEnrollmentSummary::SavePage
//

void CSendGetEnrollmentSummary::SavePage( void )
{
	TRACE0( "CSendGetEnrollmentSummary::SavePage\n" );

	pageContents.Flush();

	m_AckFilterCombo.SaveCtrl( pageContents );
//	m_RecipientProcess.SaveCtrl( pageContents );
	m_EventStateCombo.SaveCtrl( pageContents );
	m_EventTypeCombo.SaveCtrl( pageContents );
	m_MinPriority.SaveCtrl( pageContents );
	m_MaxPriority.SaveCtrl( pageContents );
	m_NotificationClass.SaveCtrl( pageContents );
}

//
//	CSendGetEnrollmentSummary::RestorePage
//

void CSendGetEnrollmentSummary::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendGetEnrollmentSummary::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_AckFilterCombo.RestoreCtrl( dec );
//	m_RecipientProcess.RestoreCtrl( dec );
	m_EventStateCombo.RestoreCtrl( dec );
	m_EventTypeCombo.RestoreCtrl( dec );
	m_MinPriority.RestoreCtrl( dec );
	m_MaxPriority.RestoreCtrl( dec );
	m_NotificationClass.RestoreCtrl( dec );
}

//
//	CSendGetEnrollmentSummary::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//
void CSendGetEnrollmentSummary::OnChangeAckFilterCombo()
{
	// any change resets the selected value to none selected
	m_AckFilterCombo.m_enumValue = -1;
	m_AckFilterCombo.UpdateData(false);
	SavePage();
	UpdateEncoded();

}

void CSendGetEnrollmentSummary::OnSelchangeAckFilterCombo()
{
	m_AckFilterCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendGetEnrollmentSummary::OnRecipientProcess()
{
	m_RecipientProcess.DoModal();
	SavePage();
	UpdateEncoded();
}

void CSendGetEnrollmentSummary::OnChangeEventStateCombo()
{
	// any change resets the selected value to none selected
	m_EventStateCombo.m_enumValue = -1;
	m_EventStateCombo.UpdateData(false);
	SavePage();
	UpdateEncoded();

}

void CSendGetEnrollmentSummary::OnSelchangeEventStateCombo()
{
	m_EventStateCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendGetEnrollmentSummary::OnChangeEventTypeCombo()
{
	// any change resets the selected value to none selected
	m_EventTypeCombo.m_enumValue = -1;
	m_EventTypeCombo.UpdateData(false);
	SavePage();
	UpdateEncoded();

}

void CSendGetEnrollmentSummary::OnSelchangeEventTypeCombo()
{
	m_EventTypeCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendGetEnrollmentSummary::OnChangeMinPriority()
{
	m_MinPriority.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendGetEnrollmentSummary::OnChangeMaxPriority()
{
	m_MaxPriority.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendGetEnrollmentSummary::OnChangeNotificationClass()
{
	m_NotificationClass.UpdateData();
	SavePage();
	UpdateEncoded();
}
