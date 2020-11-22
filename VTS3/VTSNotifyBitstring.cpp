// VTSNotifyBitstring.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSNotifyBitstring.h"
#include "VTSNotificationParameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSNotifyBitstring property page

IMPLEMENT_DYNCREATE(VTSNotifyBitstring, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyBitstring::VTSNotifyBitstring()
	: CPropertyPage(VTSNotifyBitstring::IDD)
	, m_RefBitString( this, IDC_REFBITSTRING )
	, m_StatusFlags( this )
{
	//{{AFX_DATA_INIT(VTSNotifyBitstring)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyBitstring::~VTSNotifyBitstring()
{
}

void VTSNotifyBitstring::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSNotifyBitstring)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_RefBitString.UpdateData( pDX->m_bSaveAndValidate );
	m_StatusFlags.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSNotifyBitstring, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyBitstring)
	ON_EN_CHANGE(IDC_REFBITSTRING, OnChangeRefBitString)
	ON_BN_CLICKED(IDC_INALARM, OnInAlarm)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_BN_CLICKED(IDC_OVERRIDDEN, OnOverridden)
	ON_BN_CLICKED(IDC_OUTOFSERVICE, OnOutOfService)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyBitstring::Encode
//

void VTSNotifyBitstring::Encode( BACnetAPDUEncoder& enc, int context )
{
	BACnetOpeningTag().Encode( enc, 0 );
	m_RefBitString.Encode( enc, 0 );
	m_StatusFlags.Encode( enc, 1 );
	BACnetClosingTag().Encode( enc, 0 );
}

//
//	VTSNotifyBitstring::OnSetActive
//

BOOL VTSNotifyBitstring::OnSetActive() 
{
	pageParent->m_EventType = 0;
	return CPropertyPage::OnSetActive();
}

//
//	VTSNotifyBitstring::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void VTSNotifyBitstring::OnChangeRefBitString()
{
	m_RefBitString.UpdateData();
}

void VTSNotifyBitstring::OnInAlarm() 
{
	m_StatusFlags.InAlarmClick();
}

void VTSNotifyBitstring::OnFault() 
{
	m_StatusFlags.FaultClick();
}

void VTSNotifyBitstring::OnOverridden() 
{
	m_StatusFlags.OverriddenClick();
}

void VTSNotifyBitstring::OnOutOfService() 
{
	m_StatusFlags.OutOfServiceClick();
}
