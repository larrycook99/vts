// SendDeleteObject.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendDeleteObject.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendDeleteObject::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendDeleteObject dialog

IMPLEMENT_DYNCREATE( CSendDeleteObject, CPropertyPage )

#pragma warning( disable : 4355 )
CSendDeleteObject::CSendDeleteObject( void )
	: CSendPage( CSendDeleteObject::IDD )
	, m_ObjectID( this, IDC_OBJECTID )
{
	//{{AFX_DATA_INIT(CSendDeleteObject)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendDeleteObject::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendDeleteObject)
	//}}AFX_DATA_MAP

	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendDeleteObject, CPropertyPage)
	//{{AFX_MSG_MAP(CSendDeleteObject)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendDeleteObject::OnInitDialog
//

BOOL CSendDeleteObject::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendDeleteObject::InitPage
//

void CSendDeleteObject::InitPage( void )
{
	// flush the data
	m_ObjectID.ctrlNull = true;
}

//
//	CSendDeleteObject::EncodePage
//

void CSendDeleteObject::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 11 );

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
//	CSendDeleteObject::SavePage
//

void CSendDeleteObject::SavePage( void )
{
	TRACE0( "CSendDeleteObject::SavePage\n" );

	pageContents.Flush();

	m_ObjectID.SaveCtrl( pageContents );
}

//
//	CSendDeleteObject::RestorePage
//

void CSendDeleteObject::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendDeleteObject::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ObjectID.RestoreCtrl( dec );
}

//
//	CSendDeleteObject::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendDeleteObject::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendDeleteObject::OnObjectIDButton() 
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
