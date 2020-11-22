// VTSNotifyValue.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSNotifyValue.h"
#include "VTSNotificationParameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyValue property page

IMPLEMENT_DYNCREATE(VTSNotifyValue, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyValue::VTSNotifyValue()
	: CPropertyPage(VTSNotifyValue::IDD)
	, m_ChangedBits( this, IDC_CHANGEDBITS )
	, m_ChangedValue( this, IDC_CHANGEDVALUE )
	, m_StatusFlags( this )
{
	//{{AFX_DATA_INIT(VTSNotifyValue)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyValue::~VTSNotifyValue()
{
}

void VTSNotifyValue::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSNotifyValue)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_ChangedBits.UpdateData( pDX->m_bSaveAndValidate );
	m_ChangedValue.UpdateData( pDX->m_bSaveAndValidate );
	m_StatusFlags.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSNotifyValue, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyValue)
	ON_EN_CHANGE(IDC_CHANGEDBITS, OnChangeChangedBits)
	ON_EN_CHANGE(IDC_CHANGEDVALUE, OnChangeChangedValue)
	ON_BN_CLICKED(IDC_INALARM, OnInAlarm)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_BN_CLICKED(IDC_OVERRIDDEN, OnOverridden)
	ON_BN_CLICKED(IDC_OUTOFSERVICE, OnOutOfService)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyValue::Encode
//

void VTSNotifyValue::Encode( BACnetAPDUEncoder& enc, int context )
{
	BACnetOpeningTag().Encode( enc, 2 );

	// the new value is a choice between bits and value
	BACnetOpeningTag().Encode( enc, 0 );
	if (!m_ChangedBits.ctrlNull) {
		if (!m_ChangedValue.ctrlNull)
			throw "Specify changed bits or value, but not both";
		m_ChangedBits.Encode( enc, 0 );
	} else
	if (!m_ChangedValue.ctrlNull)
		m_ChangedValue.Encode( enc, 1 );
	else
		throw "Specify changed bits or value";
	BACnetClosingTag().Encode( enc, 0 );

	// now do the status flags
	m_StatusFlags.Encode( enc, 1 );

	BACnetClosingTag().Encode( enc, 2 );
}

//
//	VTSNotifyValue::OnSetActive
//

BOOL VTSNotifyValue::OnSetActive() 
{
	pageParent->m_EventType = 2;
	return CPropertyPage::OnSetActive();
}

//
//	VTSNotifyValue::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void VTSNotifyValue::OnChangeChangedBits()
{
	m_ChangedBits.UpdateData();
}

void VTSNotifyValue::OnChangeChangedValue()
{
	m_ChangedValue.UpdateData();
}

void VTSNotifyValue::OnInAlarm() 
{
	m_StatusFlags.InAlarmClick();
}

void VTSNotifyValue::OnFault() 
{
	m_StatusFlags.FaultClick();
}

void VTSNotifyValue::OnOverridden() 
{
	m_StatusFlags.OverriddenClick();
}

void VTSNotifyValue::OnOutOfService() 
{
	m_StatusFlags.OutOfServiceClick();
}
