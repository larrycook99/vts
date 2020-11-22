// VTSDevicesTreeValuePage.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"

#include "BACnet.hpp"
#include "VTSPropValue.h"
#include "VTSDevicesTreeValuePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeValuePage property page

IMPLEMENT_DYNCREATE(VTSDevicesTreeValuePage, VTSPropertyPage)

#pragma warning(disable:4355)

VTSDevicesTreeValuePage::VTSDevicesTreeValuePage(  VTSPageCallbackInterface * pparent )
                        : VTSPropertyPage(VTSDevicesTreeValuePage::IDD, pparent),
						  m_pageGeneric(this),
						  m_pageObjectID(this)
{
	//{{AFX_DATA_INIT(VTSDevicesTreeValuePage)
	m_nDataType = -1;
	m_nContext = -1;
	//}}AFX_DATA_INIT

	m_pdevvalue = NULL;
	m_sheetValueContainer.AddPage(&m_pageGeneric);
}


VTSDevicesTreeValuePage::VTSDevicesTreeValuePage( )		// not used
                        : VTSPropertyPage()
{
	ASSERT(0);
}


VTSDevicesTreeValuePage::~VTSDevicesTreeValuePage()
{
}

void VTSDevicesTreeValuePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDevicesTreeValuePage)
	DDX_CBIndex(pDX, IDC_TYPECOMBO, m_nDataType);
	//}}AFX_DATA_MAP

	CString str;
	if ( pDX->m_bSaveAndValidate )
	{
		DDX_Text(pDX, IDC_CONTEXT, str);
		if ( str.IsEmpty() )
		{
			m_nContext = -1;
		}
		else
		{
			DDX_Text(pDX, IDC_CONTEXT, m_nContext);
			DDV_MinMaxInt(pDX, m_nContext, 0, 255);
		}
	}
	else
	{
		if ( m_nContext == -1 )
			DDX_Text(pDX, IDC_CONTEXT, str);
		else
			DDX_Text(pDX, IDC_CONTEXT, m_nContext);
	}
}


BEGIN_MESSAGE_MAP(VTSDevicesTreeValuePage, VTSPropertyPage)
	//{{AFX_MSG_MAP(VTSDevicesTreeValuePage)
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnSelChangeTypeCombo)
	ON_EN_CHANGE(IDC_CONTEXT, OnEditChangeContextTag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeValuePage message handlers


BOOL VTSDevicesTreeValuePage::OnKillActive() 
{
	VTSPropertyPage::OnKillActive();
	CtrlToValue(m_pdevvalue);
	EnableControls(false);

	if ( m_sheetValueContainer.GetPageCount() != 0 )
		m_sheetValueContainer.RemovePage(0);

	return TRUE;
}


BOOL VTSDevicesTreeValuePage::OnSetActive() 
{
	// combo is alreay loaded
	m_pdevvalue = (VTSDevValue *) RetrieveCurrentData();
	if ( m_pdevvalue != NULL &&  !m_pdevvalue->IsKindOf(RUNTIME_CLASS(VTSDevValue)) )
		m_pdevvalue = NULL;

	ValueToCtrl(m_pdevvalue);
	InitializeTypeSheet();

	// calls DDX
	VTSPropertyPage::OnSetActive();

	SwapTypePages();
	EnableControls( m_pdevvalue != NULL );	

	return m_pdevvalue != NULL;
}


void VTSDevicesTreeValuePage::InitializeTypeSheet()
{
	if ( m_sheetValueContainer.GetSafeHwnd() == NULL )
	{
		m_sheetValueContainer.Create(this, WS_CHILD | WS_VISIBLE, 0);
		m_sheetValueContainer.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
		m_sheetValueContainer.ModifyStyle( 0, WS_TABSTOP );

		//m_sheetValueContainer.GetTabControl()->ModifyStyle(0, TCS_BUTTONS | TCS_BOTTOM );
		m_sheetValueContainer.GetTabControl()->ModifyStyle(0, TCS_BOTTOM );

		CRect rcSheet;
		GetDlgItem(IDC_TYPESHEETHOLDER)->GetWindowRect( &rcSheet );
		ScreenToClient( &rcSheet );
		m_sheetValueContainer.SetWindowPos( NULL, rcSheet.left-7, rcSheet.top-7, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );
	}
}



void VTSDevicesTreeValuePage::ValueToCtrl( VTSDevValue * pdevvalue )
{
	if ( pdevvalue == NULL )
		return;

	m_nDataType = pdevvalue->m_nType;
	m_nContext = pdevvalue->m_nContext;

	// get the ObjectID out of the control (if it's in there) and put it into the 
	// structure the ObjectID page needs..

	if ( m_nDataType == 12 )		// object ID
	{
		try
		{
		BACnetAPDUDecoder	dec( m_pdevvalue->m_abContent, m_pdevvalue->m_nLength );
		BACnetObjectIdentifier	bacnetobjectID(dec);
		m_devobject.SetID(bacnetobjectID.objID);
		}
		catch(...) {}		// don't care about flubs...
	}
}


void VTSDevicesTreeValuePage::EnableControls( bool fEnable )
{
//	if ( m_sheetValueContainer.GetActivePage() == &m_pageGeneric )
//	{
		CString strCaption;
		((CComboBox *) GetDlgItem(IDC_TYPECOMBO))->GetLBText(((CComboBox *) GetDlgItem(IDC_TYPECOMBO))->GetCurSel(), strCaption);

		strCaption += _T(" Parameters");

		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = (LPTSTR)((LPCTSTR)strCaption);
		m_sheetValueContainer.GetTabControl()->SetItem(0, &tcItem );
//	}
}


void VTSDevicesTreeValuePage::CtrlToValue( VTSDevValue * pdevvalue )
{
	if ( pdevvalue == NULL )
		return;

	pdevvalue->m_nType = m_nDataType;
	pdevvalue->m_nContext = m_nContext;

	// take the value out of the dev object and encode it into the buffer... the other generic pages
	// already take care of this for us but the object ID page doesn't... this helps us avoid creating
	// a whole new page type just for this...

	if ( m_nDataType == 12 )
	{
		try
		{
		BACnetAPDUEncoder	compEncoder;
		BACnetObjectIdentifier  bacnetobjectID(m_devobject.GetID());
		bacnetobjectID.Encode( compEncoder, m_pdevvalue->m_nContext );

		m_pdevvalue->m_nLength = compEncoder.pktLength;
		memcpy( m_pdevvalue->m_abContent, compEncoder.pktBuffer, compEncoder.pktLength );
		}
		catch(...) {}		// don't care too much for flubs...
	}
}


void VTSDevicesTreeValuePage::SwapTypePages(void)
{
	VTSPropertyPage * ppageShouldBe;

	if ( m_nDataType == 12 )		// Object ID
		ppageShouldBe = &m_pageObjectID;
	else
		ppageShouldBe = &m_pageGeneric;

	if ( m_sheetValueContainer.GetPageCount() == 0 || m_sheetValueContainer.GetPage(0) != ppageShouldBe )
	{
		if ( m_sheetValueContainer.GetPageCount() != 0 )
			m_sheetValueContainer.RemovePage(0);

		m_sheetValueContainer.AddPage(ppageShouldBe);
	}

	m_sheetValueContainer.SetActivePage(0);
}


void VTSDevicesTreeValuePage::DataChangeNotification(void)
{
	// called by child property pages... we don't care about things here...
	CtrlToValue(m_pdevvalue);
	NotifyOfDataChange();
}


void * VTSDevicesTreeValuePage::GetActiveData(void)
{
	// Shouldn't use get active page... we're probably in that call to start with
	if ( m_nDataType == 12 )		// Object ID
		return (void *) &m_devobject;

	return (void *) m_pdevvalue;
}


// Change of datatype: change to the appropriate edit page
void VTSDevicesTreeValuePage::OnSelChangeTypeCombo() 
{
	UpdateData(TRUE);
	CtrlToValue(m_pdevvalue);
	SwapTypePages();
	EnableControls(true);
	NotifyOfDataChange();
}

// Change of context tag value: be sure it is saved and displayed
void VTSDevicesTreeValuePage::OnEditChangeContextTag()
{
	UpdateData(TRUE);
	CtrlToValue(m_pdevvalue);
	EnableControls(true);
	NotifyOfDataChange();
}
