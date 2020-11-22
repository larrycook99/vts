// VTSNotifyUnsignedRange.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSNotifyUnsignedRange.h"
#include "VTSNotificationParameters.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace NetworkSniffer {
	extern char *BACnetPropertyIdentifier[];
}
/////////////////////////////////////////////////////////////////////////////
// VTSNotifyUnsignedRange property page

IMPLEMENT_DYNCREATE(VTSNotifyUnsignedRange, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyUnsignedRange::VTSNotifyUnsignedRange()
	: CPropertyPage(VTSNotifyUnsignedRange::IDD)
	, m_ExValue( this, IDC_EXVALUE )
	, m_ExLimit( this, IDC_EXLIMIT )
	, m_StatusFlags( this )

{
	//{{AFX_DATA_INIT(VTSNotifyUnsignedRange)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyUnsignedRange::~VTSNotifyUnsignedRange()
{
}

void VTSNotifyUnsignedRange::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSNotifyUnsignedRange)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_ExValue.UpdateData( pDX->m_bSaveAndValidate );
	m_ExLimit.UpdateData( pDX->m_bSaveAndValidate );
	m_StatusFlags.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSNotifyUnsignedRange, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyUnsignedRange)
	ON_EN_CHANGE(IDC_EXVALUE, OnChangeExValue)
	ON_EN_CHANGE(IDC_EXLIMIT, OnChangeExLimit)

	ON_BN_CLICKED(IDC_INALARM, OnInAlarm)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_BN_CLICKED(IDC_OVERRIDDEN, OnOverridden)
	ON_BN_CLICKED(IDC_OUTOFSERVICE, OnOutOfService)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyUnsignedRange::Encode
//

void VTSNotifyUnsignedRange::Encode( BACnetAPDUEncoder& enc, int context )
{
	BACnetOpeningTag().Encode( enc, 11 );
	{
		if (m_ExValue.ctrlNull)
			throw "Exceeding Value required";
		m_ExValue.Encode( enc, 0 );		
		m_StatusFlags.Encode( enc, 1 );		
		if (m_ExLimit.ctrlNull)
			throw "Exceeded Limit required";
		m_ExLimit.Encode( enc, 2 );		
	}
	BACnetClosingTag().Encode( enc, 11 );
}
BOOL VTSNotifyUnsignedRange::OnSetActive() 
{
		TRACE0( "VTSNotifyUnsignedRange::OnSetActive()\n" );

	pageParent->m_EventType = 11;
	return CPropertyPage::OnSetActive();
}
void VTSNotifyUnsignedRange::OnChangeExValue() 
{
	m_ExValue.UpdateData();		
}
void VTSNotifyUnsignedRange::OnChangeExLimit() 
{
	m_ExLimit.UpdateData();		
}

void VTSNotifyUnsignedRange::OnInAlarm() 
{
	m_StatusFlags.InAlarmClick();
}

void VTSNotifyUnsignedRange::OnFault() 
{
	m_StatusFlags.FaultClick();
}

void VTSNotifyUnsignedRange::OnOverridden() 
{
	m_StatusFlags.OverriddenClick();
}

void VTSNotifyUnsignedRange::OnOutOfService() 
{
	m_StatusFlags.OutOfServiceClick();
}
