// SendAddListElement.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendAddListElement.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendAddListElement::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendAddListElement dialog

IMPLEMENT_DYNCREATE( CSendAddListElement, CPropertyPage )

#pragma warning( disable : 4355 )
CSendAddListElement::CSendAddListElement( void )
	: CSendPage( CSendAddListElement::IDD )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_PropCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_Value(this)			// for proper parent control
{
	//{{AFX_DATA_INIT(CSendAddListElement)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendAddListElement::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendAddListElement::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendAddListElement)
	//}}AFX_DATA_MAP

	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_PropCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendAddListElement, CPropertyPage)
	//{{AFX_MSG_MAP(CSendAddListElement)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendAddListElement::OnInitDialog
//

BOOL CSendAddListElement::OnInitDialog() 
{
	TRACE0( "CSendAddListElement::OnInitDialog()\n" );

	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_PropCombo.LoadCombo();
	
	return TRUE;
}

//
//	CSendAddListElement::InitPage
//

void CSendAddListElement::InitPage( void )
{
	TRACE0( "CSendAddListElement::InitPage()\n" );

	// flush the data
	m_ObjectID.ctrlNull = true;
	m_PropCombo.ctrlNull = true;
	m_ArrayIndex.ctrlNull = true;
}

//
//	CSendAddListElement::EncodePage
//

void CSendAddListElement::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 8 );

	// encode the object ID
	if (m_ObjectID.ctrlNull)
		throw "Object ID required";
	m_ObjectID.Encode( enc, 0 );

	// encode the property
	if (m_PropCombo.ctrlNull)
		throw "Property ID required";
	m_PropCombo.Encode( enc, 1 );

	// encode the (optional) array index
	if (!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode( enc, 2 );

	// do the value
	if (m_Value.m_anyList.Length() < 1)
		throw "Value required";
	m_Value.Encode( enc, 3 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendAddListElement::SavePage
//

void CSendAddListElement::SavePage( void )
{
	TRACE0( "CSendAddListElement::SavePage\n" );

	pageContents.Flush();

	m_ObjectID.SaveCtrl( pageContents );
	m_PropCombo.SaveCtrl( pageContents );
	m_ArrayIndex.SaveCtrl( pageContents );
	m_Value.SaveCtrl( pageContents );
}

//
//	CSendAddListElement::RestorePage
//

void CSendAddListElement::RestorePage(  int index  )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendAddListElement::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ObjectID.RestoreCtrl( dec );
	m_PropCombo.RestoreCtrl( dec );
	m_ArrayIndex.RestoreCtrl( dec );
	m_Value.RestoreCtrl( dec );
}

//
//	CSendAddListElement::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendAddListElement::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendAddListElement::OnObjectIDButton() 
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

void CSendAddListElement::OnSelchangePropCombo()
{
	m_PropCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendAddListElement::OnChangeArrayIndex()
{
	m_ArrayIndex.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendAddListElement::OnValue() 
{
	m_Value.DoModal();
	SavePage();
	UpdateEncoded();
}

void CSendAddListElement::OnDropdownPropcombo() 
{
//Added by Zhu Zhenhua , 2003-7-22 
//	to Load Standard property list for certain ObjectType
	m_PropCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_PropCombo.LoadCombo();
}
