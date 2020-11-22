// SendCreateObjectACK.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendCreateObjectACK.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendCreateObjectACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendCreateObjectACK dialog

IMPLEMENT_DYNCREATE( CSendCreateObjectACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendCreateObjectACK::CSendCreateObjectACK( void )
	: CSendPage( CSendCreateObjectACK::IDD )
	, m_ObjectID( this, IDC_OBJECTID )
{
	//{{AFX_DATA_INIT(CSendCreateObjectACK)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendCreateObjectACK::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendCreateObjectACK)
	//}}AFX_DATA_MAP

	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendCreateObjectACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendCreateObjectACK)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendCreateObjectACK::OnInitDialog
//

BOOL CSendCreateObjectACK::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendCreateObjectACK::InitPage
//

void CSendCreateObjectACK::InitPage( void )
{
	// flush the data
	m_ObjectID.ctrlNull = true;
}

//
//	CSendCreateObjectACK::EncodePage
//

void CSendCreateObjectACK::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 10 );

	// encode the object ID
	if (m_ObjectID.ctrlNull)
		throw "Object ID required";
	m_ObjectID.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendCreateObjectACK::SavePage
//

void CSendCreateObjectACK::SavePage( void )
{
	TRACE0( "CSendCreateObjectACK::SavePage\n" );

	pageContents.Flush();

	m_ObjectID.SaveCtrl( pageContents );
}

//
//	CSendCreateObjectACK::RestorePage
//

void CSendCreateObjectACK::RestorePage(  int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendCreateObjectACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ObjectID.RestoreCtrl( dec );
}

//
//	CSendCreateObjectACK::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendCreateObjectACK::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendCreateObjectACK::OnObjectIDButton() 
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
