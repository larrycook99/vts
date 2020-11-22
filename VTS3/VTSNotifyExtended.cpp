// VTSNotifyExtended.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSNotifyExtended.h"
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
// VTSNotifyExtended property page

IMPLEMENT_DYNCREATE(VTSNotifyExtended, CPropertyPage)

#pragma warning( disable : 4355 )
VTSNotifyExtended::VTSNotifyExtended()
	: CPropertyPage(VTSNotifyExtended::IDD)
	, m_VendorID( this, IDC_VENDORID )
	, m_EventType( this, IDC_EVENTTYPE )
{
	//{{AFX_DATA_INIT(VTSNotifyExtended)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

VTSNotifyExtended::~VTSNotifyExtended()
{
}

void VTSNotifyExtended::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSNotifyExtended)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_VendorID.UpdateData( pDX->m_bSaveAndValidate );
	m_EventType.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSNotifyExtended, CPropertyPage)
	//{{AFX_MSG_MAP(VTSNotifyBufferReady)
	ON_EN_CHANGE(IDC_VENDORID, OnChangeVendorID)
	ON_EN_CHANGE(IDC_EVENTTYPE, OnChangeEventType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotifyExtended::Encode
//

void VTSNotifyExtended::Encode( BACnetAPDUEncoder& enc, int context )
{
	BACnetOpeningTag().Encode( enc, 9 );
	{
		if (m_VendorID.ctrlNull)
			throw "Vendor ID required";
		m_VendorID.Encode( enc, 0 );		
		if(m_EventType.ctrlNull)
			throw "Vendor ID required";
		m_EventType.Encode(enc, 1);
		BACnetOpeningTag().Encode(enc,2);
		// todo: enter parameters here
		BACnetClosingTag().Encode(enc,2);
	}
	BACnetClosingTag().Encode( enc, 9 );
}
BOOL VTSNotifyExtended::OnSetActive() 
{
		TRACE0( "VTSNotifyExtended::OnSetActive()\n" );

	pageParent->m_EventType = 9;
	return CPropertyPage::OnSetActive();
}
void VTSNotifyExtended::OnChangeVendorID() 
{
	m_VendorID.UpdateData();		
}
void VTSNotifyExtended::OnChangeEventType() 
{
	m_EventType.UpdateData();		
}
