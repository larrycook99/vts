// VTSNotifyBufferReady.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSNotifyBufferReady.h"
#include "VTSNotificationParameters.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSNotifyBufferReady property page

IMPLEMENT_DYNCREATE(VTSNotifyBufferReady, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyBufferReady::VTSNotifyBufferReady()
	: CPropertyPage(VTSNotifyBufferReady::IDD)
	, m_ObjectID( this, IDC_OBJECTID )
	, m_PropCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_DeviceID(  this, IDC_DEVICEID)
	, m_Previousnotification(this, IDC_PRENOTIF)
	, m_Currentnotification(this, IDC_CURNOTIF)
{
	//{{AFX_DATA_INIT(VTSNotifyBufferReady)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyBufferReady::~VTSNotifyBufferReady()
{
}

void VTSNotifyBufferReady::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSNotifyBufferReady)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_PropCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_Previousnotification.UpdateData( pDX->m_bSaveAndValidate );
	m_Currentnotification.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSNotifyBufferReady, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyBufferReady)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceidbtn)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	ON_EN_CHANGE(IDC_CURNOTIF, OnChangeCurnotif)
	ON_EN_CHANGE(IDC_PRENOTIF, OnChangePrenotif)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjID)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDevID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyBufferReady::Encode
//

void VTSNotifyBufferReady::Encode( BACnetAPDUEncoder& enc, int context )
{
	BACnetOpeningTag().Encode( enc, 10 );
	{
	BACnetOpeningTag().Encode( enc, 0 );
		if (m_ObjectID.ctrlNull)
			throw "Object ID required";
		m_ObjectID.Encode( enc, 0 );		
		if (m_PropCombo.ctrlNull)
			throw "Property ID required";
		m_PropCombo.Encode( enc, 1 );		
		if(!m_ArrayIndex.ctrlNull)
			m_ArrayIndex.Encode(enc, 2);
		if(!m_DeviceID.ctrlNull)
			m_DeviceID.Encode(enc, 3);
	BACnetClosingTag().Encode( enc, 0 );
	}
	if (m_Previousnotification.ctrlNull)
		throw "previous-notification value required";
	m_Previousnotification.Encode( enc, 1 );
	if (m_Currentnotification.ctrlNull)
		throw "current-notification value required";
	m_Currentnotification.Encode( enc, 2 );

	BACnetClosingTag().Encode( enc, 10 );
}
BOOL VTSNotifyBufferReady::OnSetActive() 
{
		TRACE0( "VTSNotifyBufferReady::OnSetActive()\n" );

	pageParent->m_EventType = 10;
	return CPropertyPage::OnSetActive();
}
void VTSNotifyBufferReady::OnDeviceidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}
	
}

void VTSNotifyBufferReady::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}
	
}

void VTSNotifyBufferReady::OnDropdownPropcombo() 
{
	m_PropCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_PropCombo.LoadCombo();
	
}

void VTSNotifyBufferReady::OnSelchangePropcombo() 
{
	m_PropCombo.UpdateData();	
}

void VTSNotifyBufferReady::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();		
}

void VTSNotifyBufferReady::OnChangeCurnotif() 
{
	m_Currentnotification.UpdateData();	
}

void VTSNotifyBufferReady::OnChangePrenotif() 
{
	m_Previousnotification.UpdateData();	
}
void VTSNotifyBufferReady::OnChangeObjID() 
{
	m_ObjectID.UpdateData();	
}

void VTSNotifyBufferReady::OnChangeDevID() 
{
	m_DeviceID.UpdateData();	
}