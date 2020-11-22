// SendWhoHas.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendWhoHas.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendWhoHas::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendWhoHas dialog

IMPLEMENT_DYNCREATE( CSendWhoHas, CPropertyPage )

#pragma warning( disable : 4355 )
CSendWhoHas::CSendWhoHas( void )
	: CSendPage( CSendWhoHas::IDD )
	, m_LowLimit( this, IDC_LOWLIMIT )
	, m_HighLimit( this, IDC_HIGHLIMIT )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_ObjectName( this, IDC_OBJECTNAME )
{
	//{{AFX_DATA_INIT(CSendWhoHas)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendWhoHas::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWhoHas)
	//}}AFX_DATA_MAP

	m_LowLimit.UpdateData( pDX->m_bSaveAndValidate );
	m_HighLimit.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectName.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendWhoHas, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWhoHas)
	ON_EN_CHANGE(IDC_LOWLIMIT, OnChangeLowLimit)
	ON_EN_CHANGE(IDC_HIGHLIMIT, OnChangeHighLimit)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_EN_CHANGE(IDC_OBJECTNAME, OnChangeObjectName)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWhoHas::OnInitDialog
//

BOOL CSendWhoHas::OnInitDialog() 
{
	TRACE0( "CSendWhoHas::OnInitDialog\n" );

	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendWhoHas::InitPage
//

void CSendWhoHas::InitPage( void )
{
	TRACE0( "CSendWhoHas::InitPage\n" );

	// flush the data
	m_LowLimit.ctrlNull = true;
	m_HighLimit.ctrlNull = true;
	m_ObjectID.ctrlNull = true;
	m_ObjectName.ctrlNull = true;

	// tell the NPCI this is a global broadcast
	pageParent->NPCIPage.m_DestPresent = true;

	pageParent->NPCIPage.m_DNET.ctrlNull = false;
	pageParent->NPCIPage.m_DNET.intValue = 65535;

	pageParent->NPCIPage.m_HopCount.ctrlNull = false;
	pageParent->NPCIPage.m_HopCount.intValue = 255;
}

//
//	CSendWhoHas::EncodePage
//

void CSendWhoHas::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the pdu type and service choice
	header.Add( 0x10 );
	header.Add( 0x07 );

	// encode the device ID
	if (!m_LowLimit.ctrlNull || !m_HighLimit.ctrlNull) {
		if (m_LowLimit.ctrlNull)
			throw "Low limit required";
		if ((m_LowLimit.uintValue < 0) || (m_LowLimit.uintValue > 4194303))
			throw "Low limit out of range 0..4194303";
		if (m_HighLimit.ctrlNull)
			throw "High limit required";
		if ((m_HighLimit.uintValue < 0) || (m_HighLimit.uintValue > 4194303))
			throw "High limit out of range 0..4194303";

		m_LowLimit.Encode( enc, 0 );
		m_HighLimit.Encode( enc, 1 );
	}

	// encode the object ID and name
	if (!m_ObjectID.ctrlNull) {
		m_ObjectID.Encode( enc, 2 );
		if ((!m_ObjectName.ctrlNull) && (m_ObjectName.strLen != 0))
			throw "Both Object ID and name invalid";
	} else
	if (m_ObjectName.ctrlNull || (m_ObjectName.strLen == 0))
		throw "Object ID or name required";
	else
		m_ObjectName.Encode( enc, 3 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendWhoHas::SavePage
//

void CSendWhoHas::SavePage( void )
{
	TRACE0( "CSendWhoHas::SavePage\n" );

	pageContents.Flush();

	m_LowLimit.SaveCtrl( pageContents );
	m_HighLimit.SaveCtrl( pageContents );
	m_ObjectID.SaveCtrl( pageContents );
	m_ObjectName.SaveCtrl( pageContents );
}

//
//	CSendWhoHas::RestorePage
//

void CSendWhoHas::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendWhoHas::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_LowLimit.RestoreCtrl( dec );
	m_HighLimit.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );
	m_ObjectName.RestoreCtrl( dec );
}

//
//	CSendWhoHas::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendWhoHas::OnChangeLowLimit()
{
	m_LowLimit.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWhoHas::OnChangeHighLimit()
{
	m_HighLimit.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWhoHas::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWhoHas::OnObjectIDButton() 
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

void CSendWhoHas::OnChangeObjectName()
{
	m_ObjectName.UpdateData();
	SavePage();
	UpdateEncoded();
}
