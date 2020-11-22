// VTSNotifyOutOfRange.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSNotifyOutOfRange.h"
#include "VTSNotificationParameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyOutOfRange property page

IMPLEMENT_DYNCREATE(VTSNotifyOutOfRange, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyOutOfRange::VTSNotifyOutOfRange()
	: CPropertyPage(VTSNotifyOutOfRange::IDD)
	, m_ExceedingValue( this, IDC_EXCEEDINGVALUE )
	, m_StatusFlags( this )
	, m_Deadband( this, IDC_DEADBAND )
	, m_ExceedingLimit( this, IDC_EXCEEDINGLIMIT )
{
	//{{AFX_DATA_INIT(VTSNotifyOutOfRange)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyOutOfRange::~VTSNotifyOutOfRange()
{
}

void VTSNotifyOutOfRange::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSNotifyOutOfRange)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_ExceedingValue.UpdateData( pDX->m_bSaveAndValidate );
	m_StatusFlags.UpdateData( pDX->m_bSaveAndValidate );
	m_Deadband.UpdateData( pDX->m_bSaveAndValidate );
	m_ExceedingLimit.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSNotifyOutOfRange, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyOutOfRange)
	ON_EN_CHANGE(IDC_EXCEEDINGVALUE, OnChangeExceedingValue)
	ON_BN_CLICKED(IDC_INALARM, OnInAlarm)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_BN_CLICKED(IDC_OVERRIDDEN, OnOverridden)
	ON_BN_CLICKED(IDC_OUTOFSERVICE, OnOutOfService)
	ON_EN_CHANGE(IDC_DEADBAND, OnChangeDeadband)
	ON_EN_CHANGE(IDC_EXCEEDINGLIMIT, OnChangeExceedingLimit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyOutOfRange::Encode
//

void VTSNotifyOutOfRange::Encode( BACnetAPDUEncoder& enc, int context )
{
	BACnetOpeningTag().Encode( enc, 5 );

	if (m_ExceedingValue.ctrlNull)
		throw "Exceeding value required";
	m_ExceedingValue.Encode( enc, 0 );

	m_StatusFlags.Encode( enc, 1 );

	if (m_Deadband.ctrlNull)
		throw "Deadband required";
	m_Deadband.Encode( enc, 2 );

	if (m_ExceedingLimit.ctrlNull)
		throw "Exceeding limit value required";
	m_ExceedingLimit.Encode( enc, 3 );

	BACnetClosingTag().Encode( enc, 5 );
}

//
//	VTSNotifyOutOfRange::OnSetActive
//

BOOL VTSNotifyOutOfRange::OnSetActive() 
{
	pageParent->m_EventType = 5;
	return CPropertyPage::OnSetActive();
}

//
//	VTSNotifyOutOfRange::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void VTSNotifyOutOfRange::OnChangeExceedingValue()
{
	m_ExceedingValue.UpdateData();
}

void VTSNotifyOutOfRange::OnInAlarm() 
{
	m_StatusFlags.InAlarmClick();
}

void VTSNotifyOutOfRange::OnFault() 
{
	m_StatusFlags.FaultClick();
}

void VTSNotifyOutOfRange::OnOverridden() 
{
	m_StatusFlags.OverriddenClick();
}

void VTSNotifyOutOfRange::OnOutOfService() 
{
	m_StatusFlags.OutOfServiceClick();
}

void VTSNotifyOutOfRange::OnChangeDeadband()
{
	m_Deadband.UpdateData();
}

void VTSNotifyOutOfRange::OnChangeExceedingLimit()
{
	m_ExceedingLimit.UpdateData();
}
