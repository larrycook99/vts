// VTSNotifyChgLifeSafety.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSNotifyChgLifeSafety.h"
#include "VTSNotificationParameters.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSNotifyBufferReady property page

IMPLEMENT_DYNCREATE(VTSNotifyChgLifeSafety, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyChgLifeSafety::VTSNotifyChgLifeSafety()
	: CPropertyPage(VTSNotifyChgLifeSafety::IDD)
	, m_LSState( this, IDC_LS_STATE, NetworkSniffer::BAC_STRTAB_BACnetLifeSafetyState, true )
	, m_LSMode( this, IDC_LS_MODE, NetworkSniffer::BAC_STRTAB_BACnetLifeSafetyMode, true )
	, m_LSOperation( this, IDC_LS_OPERATION, NetworkSniffer::BAC_STRTAB_BACnetLifeSafetyOperation, true )
	, m_StatusFlags( this )
{
	//{{AFX_DATA_INIT(VTSNotifyBufferReady)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyChgLifeSafety::~VTSNotifyChgLifeSafety()
{
}

void VTSNotifyChgLifeSafety::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSNotifyChgLifeSafety)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_LSState.UpdateData( pDX->m_bSaveAndValidate );
	m_LSMode.UpdateData( pDX->m_bSaveAndValidate );
	m_StatusFlags.UpdateData( pDX->m_bSaveAndValidate );
	m_LSOperation.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSNotifyChgLifeSafety, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyChgLifeSafety)
	ON_CBN_DROPDOWN(IDC_LS_STATE, OnDropdownLSState)
	ON_CBN_SELCHANGE(IDC_LS_STATE, OnSelchangeLSState)
	ON_CBN_DROPDOWN(IDC_LS_MODE, OnDropdownLSMode)
	ON_CBN_SELCHANGE(IDC_LS_MODE, OnSelchangeLSMode)
	ON_CBN_DROPDOWN(IDC_LS_OPERATION, OnDropdownLSOperation)
	ON_CBN_SELCHANGE(IDC_LS_OPERATION, OnSelchangeLSOperation)

	ON_BN_CLICKED(IDC_INALARM, OnInAlarm)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_BN_CLICKED(IDC_OVERRIDDEN, OnOverridden)
	ON_BN_CLICKED(IDC_OUTOFSERVICE, OnOutOfService)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyChgLifeSafety::Encode
//

void VTSNotifyChgLifeSafety::Encode( BACnetAPDUEncoder& enc, int context )
{
	BACnetOpeningTag().Encode( enc, 8 );
	{
		if (m_LSState.ctrlNull)
			throw "Life Safety State required";
		m_LSState.Encode( enc, 0 );		
		if (m_LSMode.ctrlNull)
			throw "Life Safety Mode required";
		m_LSMode.Encode( enc, 1 );		
	
		m_StatusFlags.Encode( enc, 2 );

		if (m_LSOperation.ctrlNull)
			throw "Life Safety Operation required";
		m_LSMode.Encode( enc, 3 );		
	}
	BACnetClosingTag().Encode( enc, 8 );
}
BOOL VTSNotifyChgLifeSafety::OnSetActive() 
{
		TRACE0( "VTSNotifyChgLifeSafety::OnSetActive()\n" );

	pageParent->m_EventType = 8;
	return CPropertyPage::OnSetActive();
}

void VTSNotifyChgLifeSafety::OnDropdownLSState() 
{
	m_LSState.LoadCombo();
}

void VTSNotifyChgLifeSafety::OnSelchangeLSState() 
{
	m_LSState.UpdateData();	
}

void VTSNotifyChgLifeSafety::OnDropdownLSMode() 
{
	m_LSMode.LoadCombo();
}

void VTSNotifyChgLifeSafety::OnSelchangeLSMode() 
{
	m_LSMode.UpdateData();	
}

void VTSNotifyChgLifeSafety::OnDropdownLSOperation() 
{
	m_LSOperation.LoadCombo();
}

void VTSNotifyChgLifeSafety::OnSelchangeLSOperation() 
{
	m_LSOperation.UpdateData();	
}

void VTSNotifyChgLifeSafety::OnInAlarm() 
{
	m_StatusFlags.InAlarmClick();
}

void VTSNotifyChgLifeSafety::OnFault() 
{
	m_StatusFlags.FaultClick();
}

void VTSNotifyChgLifeSafety::OnOverridden() 
{
	m_StatusFlags.OverriddenClick();
}

void VTSNotifyChgLifeSafety::OnOutOfService() 
{
	m_StatusFlags.OutOfServiceClick();
}
