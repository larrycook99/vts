// VTSNotifyFloatLimit.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSNotifyFloatLimit.h"
#include "VTSNotificationParameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyFloatLimit property page

IMPLEMENT_DYNCREATE(VTSNotifyFloatLimit, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyFloatLimit::VTSNotifyFloatLimit()
	: CPropertyPage(VTSNotifyFloatLimit::IDD)
	, m_RefValue( this, IDC_REFVALUE )
	, m_StatusFlags( this )
	, m_SetpointValue( this, IDC_SETPOINTVALUE )
	, m_ErrorLimit( this, IDC_ERRORLIMIT )
{
	//{{AFX_DATA_INIT(VTSNotifyFloatLimit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyFloatLimit::~VTSNotifyFloatLimit()
{
}

void VTSNotifyFloatLimit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSNotifyFloatLimit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_RefValue.UpdateData( pDX->m_bSaveAndValidate );
	m_StatusFlags.UpdateData( pDX->m_bSaveAndValidate );
	m_SetpointValue.UpdateData( pDX->m_bSaveAndValidate );
	m_ErrorLimit.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSNotifyFloatLimit, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyFloatLimit)
	ON_EN_CHANGE(IDC_REFVALUE, OnChangeRefValue)
	ON_BN_CLICKED(IDC_INALARM, OnInAlarm)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_BN_CLICKED(IDC_OVERRIDDEN, OnOverridden)
	ON_BN_CLICKED(IDC_OUTOFSERVICE, OnOutOfService)
	ON_EN_CHANGE(IDC_SETPOINTVALUE, OnChangeSetpointValue)
	ON_EN_CHANGE(IDC_ERRORLIMIT, OnChangeErrorLimit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyFloatLimit::Encode
//

void VTSNotifyFloatLimit::Encode( BACnetAPDUEncoder& enc, int context )
{
	BACnetOpeningTag().Encode( enc, 4 );

	if (m_RefValue.ctrlNull)
		throw "Reference value required";
	m_RefValue.Encode( enc, 0 );

	m_StatusFlags.Encode( enc, 1 );

	if (m_SetpointValue.ctrlNull)
		throw "Setpoint value required";
	m_SetpointValue.Encode( enc, 2 );

	if (m_ErrorLimit.ctrlNull)
		throw "Error limit value required";
	m_ErrorLimit.Encode( enc, 3 );

	BACnetClosingTag().Encode( enc, 4 );
}

//
//	VTSNotifyFloatLimit::OnSetActive
//

BOOL VTSNotifyFloatLimit::OnSetActive() 
{
	pageParent->m_EventType = 4;
	return CPropertyPage::OnSetActive();
}

//
//	VTSNotifyFloatLimit::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void VTSNotifyFloatLimit::OnChangeRefValue()
{
	m_RefValue.UpdateData();
}

void VTSNotifyFloatLimit::OnInAlarm() 
{
	m_StatusFlags.InAlarmClick();
}

void VTSNotifyFloatLimit::OnFault() 
{
	m_StatusFlags.FaultClick();
}

void VTSNotifyFloatLimit::OnOverridden() 
{
	m_StatusFlags.OverriddenClick();
}

void VTSNotifyFloatLimit::OnOutOfService() 
{
	m_StatusFlags.OutOfServiceClick();
}

void VTSNotifyFloatLimit::OnChangeSetpointValue()
{
	m_SetpointValue.UpdateData();
}

void VTSNotifyFloatLimit::OnChangeErrorLimit()
{
	m_ErrorLimit.UpdateData();
}
