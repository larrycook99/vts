// SendUTCTimeSync.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendUTCTimeSync.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




BACnetAPDUEncoder CSendUTCTimeSync::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendUTCTimeSync property page

IMPLEMENT_DYNCREATE(CSendUTCTimeSync, CPropertyPage)

#pragma warning( disable : 4355 )
CSendUTCTimeSync::CSendUTCTimeSync() : CSendPage(CSendUTCTimeSync::IDD)
	, m_Date( this, IDC_UTCDATE )
	, m_Time( this, IDC_UTCTIME )

{
	//{{AFX_DATA_INIT(CSendUTCTimeSync)
	m_AutoSync = FALSE;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

/*CSendUTCTimeSync::~CSendUTCTimeSync()
{
}*/

void CSendUTCTimeSync::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendUTCTimeSync)
	DDX_Check(pDX, IDC_AUTOSYNCTIME, m_AutoSync);
	//}}AFX_DATA_MAP
	m_Date.UpdateData( pDX->m_bSaveAndValidate );
	m_Time.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(CSendUTCTimeSync, CPropertyPage)
	//{{AFX_MSG_MAP(CSendUTCTimeSync)
	ON_BN_CLICKED(IDC_AUTOSYNCTIME, OnAutosynctime)
	ON_EN_CHANGE(IDC_UTCDATE, OnChangeUtcdate)
	ON_EN_CHANGE(IDC_UTCTIME, OnChangeUtctime)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////////////////////
//Modifyed by Zhu Zhenhua 2003-7-22 
//Remove the UTC offset and Daylight Savings status for UTCTimeSynchronization
//Remove the Value, the implementation Function and the Encode&Decode 

/////////////////////////////////////////////////////////////////////////////
// CSendUTCTimeSync message handlers

void CSendUTCTimeSync::OnAutosynctime( void ) 
{
	TRACE0( "CSendUTCTimeSync::OnAutosynctime()\n" );
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


void CSendUTCTimeSync::OnChangeUtcdate() 
{
	TRACE0( "CSendUTCTimeSync::OnChangeUtcdate()\n" );
	m_Date.UpdateData();
	SavePage();
	UpdateEncoded();
	
}

void CSendUTCTimeSync::InitPage( void )
{
	TRACE0( "CSendUTCTimeSync::InitPage()\n" );

	// flush the data, should really be current date and time
	m_Date.ctrlNull = false;
	m_Time.ctrlNull = false;

}

void CSendUTCTimeSync::OnChangeUtctime() 
{
	TRACE0( "CSendUTCTimeSync::OnChangeUtctime()\n" );
	m_Time.UpdateData();
	SavePage();
	UpdateEncoded();
	
}

//////////////////////////////////////////////////////////////////
//	CSendUTCTimeSync::SavePage
//
void CSendUTCTimeSync::SavePage( void )
{
	TRACE0( "CSendUTCTimeSync::SavePage\n" );

	pageContents.Flush();

	m_Date.SaveCtrl( pageContents );
	m_Time.SaveCtrl( pageContents );
}

//
//	CSendUTCTimeSync::RestorePage
//
void CSendUTCTimeSync::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendUTCTimeSync::RestorePage\n" );
	if (dec.pktLength == 0)
		return;
	m_Date.RestoreCtrl( dec );
	m_Time.RestoreCtrl( dec );

}

//
//	CSendUTCTimeSync::SynchronizeControls
//
//	If m_AutoSync is true, the values of the date and time will reflect the current 
//	date and time in the PC.  In that case, don't allow the user to change the 
//	values.  If m_AutoSync is off, the user can enter anything they'd like, including 
//	invalid combinations of the day-of-week and date.
//

void CSendUTCTimeSync::SynchronizeControls()
{
	TRACE0( "CSendUTCTimeSync::SynchronizeControls\n" );
	GetDlgItem( IDC_UTCDATE )->EnableWindow( !m_AutoSync );
	GetDlgItem( IDC_UTCTIME )->EnableWindow( !m_AutoSync );
}



//////////////////////////////////////////////////////////////////
//	CSendUTCTimeSync::EncodePage
//
void CSendUTCTimeSync::EncodePage( CByteArray* contents )
{
	CByteArray          header;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendUTCTimeSync::EncodePage\n" );
	// encode the pdu type and service choice
	header.Add( 0x10 );
	header.Add( 0x09 );
	if (m_Date.ctrlNull)
		throw "Date required";
	m_Date.Encode( enc );

	if (m_Time.ctrlNull)
		throw "Time required";
	m_Time.Encode( enc );
	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );		 // new
	// stuff the header on the front
	contents->InsertAt( 0, &header );		 // new

//		contents->Add( enc.pktBuffer[i] );	 was this 
}


void CSendUTCTimeSync::OnTimer(UINT nIDEvent) 
{
	time_t		now
	;
	struct tm	*currtime
	;
	
	// get the current time
	time( &now );
	currtime = gmtime( &now );
	
	m_Date.year = currtime->tm_year;
	m_Date.month = currtime->tm_mon + 1;
	m_Date.day = currtime->tm_mday;

	//Modified by Xu Yiping, 2003-09-03
	//m_Date.CalcDayOfWeek();
	m_Date.dayOfWeek = currtime->tm_wday;

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

BOOL CSendUTCTimeSync::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendUTCTimeSync::OnDestroy() 
{
	// kill the timer if its running
	if (m_AutoSync)
		KillTimer( 0 );

	CPropertyPage::OnDestroy();
		
}
