// SendGetAlarmSummary.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendGetAlarmSummary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendGetAlarmSummary dialog

IMPLEMENT_DYNCREATE( CSendGetAlarmSummary, CPropertyPage )

CSendGetAlarmSummary::CSendGetAlarmSummary( void )
	: CSendPage( CSendGetAlarmSummary::IDD )
{
	//{{AFX_DATA_INIT(CSendGetAlarmSummary)
	//}}AFX_DATA_INIT
}

void CSendGetAlarmSummary::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendGetAlarmSummary::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendGetAlarmSummary)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendGetAlarmSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CSendGetAlarmSummary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendGetAlarmSummary::OnInitDialog
//

BOOL CSendGetAlarmSummary::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendGetAlarmSummary::InitPage
//

void CSendGetAlarmSummary::InitPage( void )
{
}

//
//	CSendGetAlarmSummary::EncodePage
//

void CSendGetAlarmSummary::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;

	// encode the service choice
	header.Add( 3 );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}
