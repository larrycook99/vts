// VTSNotifyCmdFailure.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSNotifyCmdFailure.h"
#include "VTSNotificationParameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyCmdFailure property page

IMPLEMENT_DYNCREATE(VTSNotifyCmdFailure, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyCmdFailure::VTSNotifyCmdFailure()
	: CPropertyPage(VTSNotifyCmdFailure::IDD)
	, m_CommandValue( this )
	, m_StatusFlags( this )
	, m_FeedbackValue( this )
{
	//{{AFX_DATA_INIT(VTSNotifyCmdFailure)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyCmdFailure::~VTSNotifyCmdFailure()
{
}

void VTSNotifyCmdFailure::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSNotifyCmdFailure)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_StatusFlags.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSNotifyCmdFailure, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyCmdFailure)
	ON_BN_CLICKED(IDC_COMMANDVALUE, OnCommandValue)
	ON_BN_CLICKED(IDC_INALARM, OnInAlarm)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_BN_CLICKED(IDC_OVERRIDDEN, OnOverridden)
	ON_BN_CLICKED(IDC_OUTOFSERVICE, OnOutOfService)
	ON_BN_CLICKED(IDC_FEEDBACKVALUE, OnFeedbackValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyCmdFailure::Encode
//

void VTSNotifyCmdFailure::Encode( BACnetAPDUEncoder& enc, int context )
{
	BACnetOpeningTag().Encode( enc, 3 );
	m_CommandValue.Encode( enc, 0 );
	m_StatusFlags.Encode( enc, 1 );
	m_FeedbackValue.Encode( enc, 2 );
	BACnetClosingTag().Encode( enc, 3 );
}

//
//	VTSNotifyCmdFailure::OnSetActive
//

BOOL VTSNotifyCmdFailure::OnSetActive() 
{
	pageParent->m_EventType = 3;
	return CPropertyPage::OnSetActive();
}

//
//	VTSNotifyCmdFailure::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void VTSNotifyCmdFailure::OnCommandValue()
{
	m_CommandValue.DoModal();
}

void VTSNotifyCmdFailure::OnInAlarm() 
{
	m_StatusFlags.InAlarmClick();
}

void VTSNotifyCmdFailure::OnFault() 
{
	m_StatusFlags.FaultClick();
}

void VTSNotifyCmdFailure::OnOverridden() 
{
	m_StatusFlags.OverriddenClick();
}

void VTSNotifyCmdFailure::OnOutOfService() 
{
	m_StatusFlags.OutOfServiceClick();
}

void VTSNotifyCmdFailure::OnFeedbackValue()
{
	m_FeedbackValue.DoModal();
}
