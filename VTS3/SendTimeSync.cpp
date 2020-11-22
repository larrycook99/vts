// SendTimeSync.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendTimeSync.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendTimeSync::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendTimeSync property page

IMPLEMENT_DYNCREATE(CSendTimeSync, CPropertyPage)

#pragma warning( disable : 4355 )
CSendTimeSync::CSendTimeSync()
	: CSendPage( CSendTimeSync::IDD )
	, m_Date( this, IDC_DATE )
	, m_Time( this, IDC_TIME )
{
	//{{AFX_DATA_INIT(CSendTimeSync)
	m_AutoSync = FALSE;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendTimeSync::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendTimeSync)
	DDX_Check(pDX, IDC_AUTOSYNC, m_AutoSync);
	//}}AFX_DATA_MAP

	m_Date.UpdateData( pDX->m_bSaveAndValidate );
	m_Time.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendTimeSync, CPropertyPage)
	//{{AFX_MSG_MAP(CSendTimeSync)
	ON_BN_CLICKED(IDC_AUTOSYNC, OnAutoSync)
	ON_EN_CHANGE(IDC_DATE, OnChangeDate)
	ON_EN_CHANGE(IDC_TIME, OnChangeTime)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendTimeSync::InitPage
//

void CSendTimeSync::InitPage( void )
{
	TRACE0( "CSendTimeSync::InitPage()\n" );

	// flush the data, should really be current date and time
	m_Date.ctrlNull = false;
	m_Time.ctrlNull = false;
}

//
//	CSendTimeSync::EncodePage
//

void CSendTimeSync::EncodePage( CByteArray* contents )
{
	CByteArray          header;
	BACnetAPDUEncoder	enc
	;
	// encode the pdu type and service choice
	header.Add( 0x10 );
	header.Add( 0x06 );
	if (m_Date.ctrlNull)
		throw "Date required";
	m_Date.Encode( enc );

	if (m_Time.ctrlNull)
		throw "Time required";
	m_Time.Encode( enc );

	// copy the encoding into the byte array		was this 
//	for (int i = 0; i < enc.pktLength; i++)
//		contents->Add( enc.pktBuffer[i] );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );		 // new
	// stuff the header on the front
	contents->InsertAt( 0, &header );		 // new
}

//
//	CSendTimeSync::SavePage
//

void CSendTimeSync::SavePage( void )
{
	TRACE0( "CSendTimeSync::SavePage\n" );

	pageContents.Flush();

	m_Date.SaveCtrl( pageContents );
	m_Time.SaveCtrl( pageContents );
}

//
//	CSendTimeSync::RestorePage
//

void CSendTimeSync::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendTimeSync::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_Date.RestoreCtrl( dec );
	m_Time.RestoreCtrl( dec );
}

//
//	CSendTimeSync::SynchronizeControls
//
//	If m_AutoSync is true, the values of the date and time will reflect the current 
//	date and time in the PC.  In that case, don't allow the user to change the 
//	values.  If m_AutoSync is off, the user can enter anything they'd like, including 
//	invalid combinations of the day-of-week and date.
//

void CSendTimeSync::SynchronizeControls()
{
	GetDlgItem( IDC_DATE )->EnableWindow( !m_AutoSync );
	GetDlgItem( IDC_TIME )->EnableWindow( !m_AutoSync );
}

//
//	CSendTimeSync::OnAutoSync
//

void CSendTimeSync::OnAutoSync() 
{
	// make sure m_AutoSync reflects the control value
	UpdateData();

	SavePage();

	// update the encoding
	UpdateEncoded();

	// disable the date and time if autosync
	SynchronizeControls();

	// start (or kill) a timer
	if (m_AutoSync)
		SetTimer( 0, 1000, 0 );
	else
		KillTimer( 0 );
}

//
//	CSendTimeSync::OnChangeDate
//

void CSendTimeSync::OnChangeDate()
{
	m_Date.UpdateData();
	SavePage();
	UpdateEncoded();
}

//
//	CSendTimeSync::OnChangeTime
//

void CSendTimeSync::OnChangeTime()
{
	m_Time.UpdateData();
	SavePage();
	UpdateEncoded();
}

//
//	CSendTimeSync::OnInitDialog
//

BOOL CSendTimeSync::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

//	SetTimer( 0, 1000, 0 );

	return TRUE;
}

//
//	CSendTimeSync::OnDestroy
//

void CSendTimeSync::OnDestroy() 
{
	// kill the timer if its running
	if (m_AutoSync)
		KillTimer( 0 );

	// continue usual processing
	CPropertyPage::OnDestroy();
}

//
//	CSendTimeSync::OnTimer
//

void CSendTimeSync::OnTimer(UINT nIDEvent) 
{
	time_t		now
	;
	struct tm	*currtime
	;
	
	// get the current time
	time( &now );
	currtime = localtime( &now );
	
	m_Date.year = currtime->tm_year;
	m_Date.month = currtime->tm_mon + 1;
	m_Date.day = currtime->tm_mday;
	m_Date.CalcDayOfWeek();
	m_Date.ObjToCtrl();

	m_Time.hour = currtime->tm_hour;
	m_Time.minute = currtime->tm_min;
	m_Time.second = currtime->tm_sec;
	m_Time.hundredths = 0;
	m_Time.ObjToCtrl();

	// save the new contents (overkill?)
	SavePage();

	// update the message
	UpdateEncoded();

	// cascade the timer event
	CPropertyPage::OnTimer(nIDEvent);
}
