// SendSubscribeCOV.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendSubscribeCOV.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendSubscribeCOV::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendSubscribeCOV dialog

IMPLEMENT_DYNCREATE( CSendSubscribeCOV, CPropertyPage )

#pragma warning( disable : 4355 )
CSendSubscribeCOV::CSendSubscribeCOV( void )
	: CSendPage( CSendSubscribeCOV::IDD )
	, m_ProcessID( this, IDC_PROCESSID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_IssueConfirmed( this, IDC_ISSUECONFIRMED )
	, m_Lifetime( this, IDC_LIFETIME )
{
	//{{AFX_DATA_INIT(CSendSubscribeCOV)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendSubscribeCOV::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendSubscribeCOV::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendSubscribeCOV)
	//}}AFX_DATA_MAP

	m_ProcessID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_IssueConfirmed.UpdateData( pDX->m_bSaveAndValidate );
	m_Lifetime.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendSubscribeCOV, CPropertyPage)
	//{{AFX_MSG_MAP(CSendSubscribeCOV)
	ON_EN_CHANGE(IDC_PROCESSID, OnChangeProcessID)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_EN_CHANGE(IDC_ISSUECONFIRMED, OnChangeIssueConfirmed)
	ON_EN_CHANGE(IDC_LIFETIME, OnChangeLifetime)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendSubscribeCOV::OnInitDialog
//

BOOL CSendSubscribeCOV::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendSubscribeCOV::InitPage
//

void CSendSubscribeCOV::InitPage( void )
{
	// flush the data
	m_ProcessID.ctrlNull = true;
	m_ObjectID.ctrlNull = true;
	m_IssueConfirmed.ctrlNull = true;
	m_Lifetime.ctrlNull = true;
}

//
//	CSendSubscribeCOV::EncodePage
//

void CSendSubscribeCOV::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 5 );

	// encode the process ID
	if (m_ProcessID.ctrlNull)
		throw "Process ID required";
	m_ProcessID.Encode( enc, 0 );

	// encode the object ID
	if (m_ObjectID.ctrlNull)
		throw "Object ID required";
	m_ObjectID.Encode( enc, 1 );

	// encode the (optional) issue confirmed notifications flag
	if (!m_IssueConfirmed.ctrlNull)
		m_IssueConfirmed.Encode( enc, 2 );

	// encode the (optional) lifetime
	if (!m_Lifetime.ctrlNull)
		m_Lifetime.Encode( enc, 3 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendSubscribeCOV::SavePage
//

void CSendSubscribeCOV::SavePage( void )
{
	TRACE0( "CSendSubscribeCOV::SavePage\n" );

	pageContents.Flush();

	m_ProcessID.SaveCtrl( pageContents );
	m_ObjectID.SaveCtrl( pageContents );
	m_IssueConfirmed.SaveCtrl( pageContents );
	m_Lifetime.SaveCtrl( pageContents );
}

//
//	CSendSubscribeCOV::RestorePage
//

void CSendSubscribeCOV::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendSubscribeCOV::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ProcessID.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );
	m_IssueConfirmed.RestoreCtrl( dec );
	m_Lifetime.RestoreCtrl( dec );
}

//
//	CSendSubscribeCOV::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendSubscribeCOV::OnChangeProcessID()
{
	m_ProcessID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSubscribeCOV::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSubscribeCOV::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// added parent for proper dialog chain
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

void CSendSubscribeCOV::OnChangeIssueConfirmed()
{
	m_IssueConfirmed.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSubscribeCOV::OnChangeLifetime()
{
	m_Lifetime.UpdateData();
	SavePage();
	UpdateEncoded();
}
