// SendSubscribeCOVProperty.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "Send.h"
#include "SendSubscribeCOVProperty.h"
#include "VTSObjectIdentifierDialog.h"

namespace NetworkSniffer {
	extern char *BACnetPropertyIdentifier[];
}

BACnetAPDUEncoder CSendSubscribeCOVProperty::pageContents;

// CSendSubscribeCOVProperty dialog

IMPLEMENT_DYNAMIC(CSendSubscribeCOVProperty, CPropertyPage)

#pragma warning( disable : 4355 )
CSendSubscribeCOVProperty::CSendSubscribeCOVProperty()
: CSendPage( CSendSubscribeCOVProperty::IDD )
, m_ProcessID( this, IDC_PROCESSID )
, m_ObjectID( this, IDC_OBJECTID )
, m_IssueConfirmed( this, IDC_ISSUECONFIRMED )
, m_Lifetime( this, IDC_LIFETIME )
, m_ArrayIndex( this, IDC_ARRAYINDEX )
, m_COVIncrement( this, IDC_REAL )
, m_property( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )

{
}

#pragma warning( default : 4355 )

CSendSubscribeCOVProperty::~CSendSubscribeCOVProperty()
{
}

//
//	CSendSubscribeCOVProperty::OnInitDialog
//

BOOL CSendSubscribeCOVProperty::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_property.LoadCombo();   // needed to show text when refreshing the dlg
	
	return TRUE;
}

void CSendSubscribeCOVProperty::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);

  m_ProcessID.UpdateData( pDX->m_bSaveAndValidate );
  m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
  m_IssueConfirmed.UpdateData( pDX->m_bSaveAndValidate );
  m_Lifetime.UpdateData( pDX->m_bSaveAndValidate );
  m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
  m_property.UpdateData( pDX->m_bSaveAndValidate );
  m_COVIncrement.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(CSendSubscribeCOVProperty, CPropertyPage)
  ON_EN_CHANGE(IDC_PROCESSID, OnEnChangeProcessid)
  ON_BN_CLICKED(IDC_OBJECTIDBTN, OnBnClickedObjectidbtn)
  ON_EN_CHANGE(IDC_ISSUECONFIRMED, OnEnChangeIssueconfirmed)
  ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnCbnDropdownPropcombo)
  ON_EN_CHANGE(IDC_REAL, OnEnChangeReal)
  ON_EN_UPDATE(IDC_OBJECTID, OnEnUpdateObjectid)
  ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnCbnSelchangePropcombo)
  ON_EN_CHANGE(IDC_ARRAYINDEX, OnEnChangeArrayindex)
  ON_EN_CHANGE(IDC_LIFETIME, OnEnChangeLifetime)
END_MESSAGE_MAP()


void CSendSubscribeCOVProperty::InitPage( void )
{
	// flush the data
	m_ProcessID.ctrlNull = true;
	m_ObjectID.ctrlNull = true;
	m_IssueConfirmed.ctrlNull = true;
	m_Lifetime.ctrlNull = true;
  m_property.ctrlNull = true;
  m_ArrayIndex.ctrlNull = true;
  m_COVIncrement.ctrlNull = true;
}

//
//	CSendSubscribeCOV::EncodePage
//

void CSendSubscribeCOVProperty::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
  unsigned char oct;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 28 );

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

  // encode the Property Identifier
  if (!m_property.ctrlNull)
  {
    BACnetOpeningTag().Encode(enc, 4);
    m_property.Encode( enc, 0 );
    if(!m_ArrayIndex.ctrlNull)
      m_ArrayIndex.Encode( enc, 1 );
    oct = 0x4f;
    BACnetClosingTag().Encode(enc, 4);
  }

  if (!m_COVIncrement.ctrlNull)
    m_COVIncrement.Encode(enc, 5);

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendSubscribeCOV::SavePage
//

void CSendSubscribeCOVProperty::SavePage( void )
{
	TRACE0( "CSendSubscribeCOVProperty::SavePage\n" );

	pageContents.Flush();

	m_ProcessID.SaveCtrl( pageContents );
	m_ObjectID.SaveCtrl( pageContents );
	m_IssueConfirmed.SaveCtrl( pageContents );
	m_Lifetime.SaveCtrl( pageContents );
  m_property.SaveCtrl( pageContents );
  m_ArrayIndex.SaveCtrl( pageContents );
  m_COVIncrement.SaveCtrl( pageContents );
}

//
//	CSendSubscribeCOV::RestorePage
//

void CSendSubscribeCOVProperty::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents );

	TRACE0( "CSendSubscribeCOV::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ProcessID.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );
	m_IssueConfirmed.RestoreCtrl( dec );
	m_Lifetime.RestoreCtrl( dec );
  m_property.RestoreCtrl( dec );
  m_ArrayIndex.RestoreCtrl( dec );
  m_COVIncrement.RestoreCtrl( dec );

  if ( isShown )
	  m_property.ObjToCtrl();

}
// CSendSubscribeCOVProperty message handlers

void CSendSubscribeCOVProperty::OnEnChangeProcessid()
{
  m_ProcessID.UpdateData();
	SavePage();
	UpdateEncoded();

}

void CSendSubscribeCOVProperty::OnBnClickedObjectidbtn()
{
  VTSObjectIdentifierDialog	dlg(this);			// added parent for proper dialog chain

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) 
  {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendSubscribeCOVProperty::OnEnChangeIssueconfirmed()
{
  m_IssueConfirmed.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendSubscribeCOVProperty::OnCbnDropdownPropcombo()
{
  //Added by Zhu Zhenhua , 2003-7-22 
//	to Load Standard property list for certain ObjectType
	m_property.m_nObjType = m_ObjectID.GetObjectType();
	m_property.LoadCombo();
}

void CSendSubscribeCOVProperty::OnEnChangeReal()
{
  m_COVIncrement.UpdateData();
	SavePage();
	UpdateEncoded();

}

void CSendSubscribeCOVProperty::OnEnUpdateObjectid()
{
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CSendPage::OnInitDialog()
  // function to send the EM_SETEVENTMASK message to the control
  // with the ENM_UPDATE flag ORed into the lParam mask.

  // TODO:  Add your control notification handler code here
}

void CSendSubscribeCOVProperty::OnCbnSelchangePropcombo()
{
  m_property.UpdateData();
    SavePage();
	UpdateEncoded();
}

void CSendSubscribeCOVProperty::OnEnChangeArrayindex()
{
  m_ArrayIndex.UpdateData();
  SavePage();
  UpdateEncoded();
}

void CSendSubscribeCOVProperty::OnEnChangeLifetime()
{
  m_Lifetime.UpdateData();
  SavePage();
  UpdateEncoded();
}
