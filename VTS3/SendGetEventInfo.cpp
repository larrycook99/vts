// SendGetEventInfo.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "Send.h"
#include "SendGetEventInfo.h"

#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BACnetAPDUEncoder	CSendGetEventInfo::pageContents;
IMPLEMENT_DYNCREATE( CSendGetEventInfo, CPropertyPage )
/////////////////////////////////////////////////////////////////////////////
// CSendGetEventInfo dialog

#pragma warning( disable : 4355 )
CSendGetEventInfo::CSendGetEventInfo(void)
	: CSendPage( CSendGetEventInfo::IDD )
	, m_ObjectID( this, IDC_OBJECTID )
{
	//{{AFX_DATA_INIT(CSendGetEventInfo)
	m_hasObjID = FALSE;
	//}}AFX_DATA_INIT
}


void CSendGetEventInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendGetEventInfo)
	DDX_Check(pDX, IDC_HASOBJID, m_hasObjID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendGetEventInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CSendGetEventInfo)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_BN_CLICKED(IDC_HASOBJID, OnHasobjid)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendGetEventInfo message handlers

void CSendGetEventInfo::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this)		// added parent wnd to bind to modeless Send dialog
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

void CSendGetEventInfo::OnHasobjid() 
{
	UpdateData(TRUE);
	GetDlgItem( IDC_OBJECTID )->EnableWindow( m_hasObjID );
	GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( m_hasObjID );
	UpdateEncoded();
}

void CSendGetEventInfo::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

BOOL CSendGetEventInfo::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	GetDlgItem( IDC_OBJECTID )->EnableWindow( m_hasObjID );
	GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( m_hasObjID );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CSendGetEventInfo::InitPage( void )
{
	m_ObjectID.ctrlNull = true;
}					
void CSendGetEventInfo::EncodePage( CByteArray* contents )
{
		CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 29 );
	
	if(m_hasObjID)
	{
		if (m_ObjectID.ctrlNull)
			throw "If Has Last Received Object Identifier, fill it";
		m_ObjectID.Encode( enc, 0 );
		
		// copy the encoding into the byte array
		for (int i = 0; i < enc.pktLength; i++)
			header.Add( enc.pktBuffer[i] );
	}
	// stuff the header on the front
	contents->InsertAt( 0, &header );
}
void CSendGetEventInfo::SavePage( void )
{

	TRACE0( "CSendGetEventInformation::SavePage\n" );
	pageContents.Flush();
	m_ObjectID.SaveCtrl( pageContents );
}					
void CSendGetEventInfo::RestorePage( int index)
{
	BACnetAPDUDecoder	dec( pageContents );
	TRACE0( "CSendGetEventInformation::RestorePage\n" );
	if (dec.pktLength == 0)
		return;
	m_ObjectID.RestoreCtrl( dec );
}	
