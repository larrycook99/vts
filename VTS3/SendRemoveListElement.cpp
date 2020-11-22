// SendRemoveListElement.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendRemoveListElement.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendRemoveListElement::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendRemoveListElement dialog

IMPLEMENT_DYNCREATE( CSendRemoveListElement, CPropertyPage )

#pragma warning( disable : 4355 )
CSendRemoveListElement::CSendRemoveListElement( void )
	: CSendPage( CSendRemoveListElement::IDD )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_PropCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_Value(this)			// for proper parent control
{
	//{{AFX_DATA_INIT(CSendRemoveListElement)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendRemoveListElement::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendRemoveListElement::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendRemoveListElement)
	//}}AFX_DATA_MAP

	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_PropCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendRemoveListElement, CPropertyPage)
	//{{AFX_MSG_MAP(CSendRemoveListElement)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendRemoveListElement::OnInitDialog
//

BOOL CSendRemoveListElement::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_PropCombo.LoadCombo();
	
	return TRUE;
}

//
//	CSendRemoveListElement::InitPage
//

void CSendRemoveListElement::InitPage( void )
{
	// flush the data
	m_ObjectID.ctrlNull = true;
	m_PropCombo.ctrlNull = true;
	m_ArrayIndex.ctrlNull = true;
}

//
//	CSendRemoveListElement::EncodePage
//

void CSendRemoveListElement::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 9 );

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
//	CSendRemoveListElement::SavePage
//

void CSendRemoveListElement::SavePage( void )
{
	TRACE0( "CSendRemoveListElement::SavePage\n" );

	pageContents.Flush();

	m_ObjectID.SaveCtrl( pageContents );
	m_PropCombo.SaveCtrl( pageContents );
	m_ArrayIndex.SaveCtrl( pageContents );
	m_Value.SaveCtrl( pageContents );
}

//
//	CSendRemoveListElement::RestorePage
//

void CSendRemoveListElement::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendRemoveListElement::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ObjectID.RestoreCtrl( dec );
	m_PropCombo.RestoreCtrl( dec );
	m_ArrayIndex.RestoreCtrl( dec );
	m_Value.RestoreCtrl( dec );
}

//
//	CSendRemoveListElement::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendRemoveListElement::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendRemoveListElement::OnObjectIDButton() 
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

void CSendRemoveListElement::OnSelchangePropCombo()
{
	m_PropCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendRemoveListElement::OnChangeArrayIndex()
{
	m_ArrayIndex.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendRemoveListElement::OnValue() 
{
	m_Value.DoModal();
	SavePage();
	UpdateEncoded();
}

void CSendRemoveListElement::OnDropdownPropcombo() 
{
//Added by Zhu Zhenhua , 2003-7-22 
//	to Load Standard property list for certain ObjectType
	m_PropCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_PropCombo.LoadCombo();
}
