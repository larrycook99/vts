// VTSDevicesTreeOptPage.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDoc.h"

#include "VTSPropValue.h"
#include "VTSDevicesTreeOptPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeOptPage property page

IMPLEMENT_DYNCREATE(VTSDevicesTreeOptPage, VTSPropertyPage)

VTSDevicesTreeOptPage::VTSDevicesTreeOptPage( VTSPageCallbackInterface * pparent )
                      :VTSPropertyPage(VTSDevicesTreeOptPage::IDD, pparent)
{
	//{{AFX_DATA_INIT(VTSDevicesTreeOptPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// duplicate the member initalization that the class 'wizard' adds

	ClearData();
	m_pdevice = NULL;
}


VTSDevicesTreeOptPage::VTSDevicesTreeOptPage()
                      :VTSPropertyPage()
{
	ASSERT(0);
	//Not used
}


VTSDevicesTreeOptPage::~VTSDevicesTreeOptPage()
{
}


void VTSDevicesTreeOptPage::ClearData(void)
{
	m_bAlarmSummary = TRUE;
	m_bEventSummary = FALSE;
	m_bEventInfo = FALSE;
	
	m_nEvents = 0;
}



void VTSDevicesTreeOptPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDevicesTreeOptPage)
	DDX_Text(pDX, IDC_EVENTS, m_nEvents);
	DDX_Check(pDX, IDC_SASUM, m_bAlarmSummary);
	DDX_Check(pDX, IDC_SESUM, m_bEventSummary);
	DDX_Check(pDX, IDC_SINFO, m_bEventInfo);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDevicesTreeOptPage, VTSPropertyPage)
	//{{AFX_MSG_MAP(VTSDevicesTreeOptPage)
	ON_EN_CHANGE(IDC_EVENTS, OnSaveChange)
	ON_BN_CLICKED(IDC_SASUM, OnSaveChange)
	ON_BN_CLICKED(IDC_SESUM, OnSaveChange)
	ON_BN_CLICKED(IDC_SINFO, OnSaveChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeOptPage message handlers



BOOL VTSDevicesTreeOptPage::OnSetActive() 
{
	m_pdevice = (VTSDevice *) RetrieveCurrentData();
	if ( m_pdevice != NULL  && !m_pdevice->IsKindOf(RUNTIME_CLASS(VTSDevice)) )
		m_pdevice = NULL;

	DeviceToCtrl(m_pdevice);

	// calls DDX
	VTSPropertyPage::OnSetActive();
	EnableControls( m_pdevice != NULL );	
	return m_pdevice != NULL;
}


BOOL VTSDevicesTreeOptPage::OnKillActive() 
{
	// calls DDX
	VTSPropertyPage::OnKillActive();
	EnableControls(false);	
	CtrlToDevice(m_pdevice);
	return TRUE;
}



void VTSDevicesTreeOptPage::DeviceToCtrl( VTSDevice * pdevice )
{

	if ( pdevice == NULL )
	{
		ClearData();
		return;
	}

	CString ss = pdevice->m_services_supported.ToString();

	m_bAlarmSummary = pdevice->m_services_supported.GetBit(3) ? TRUE : FALSE;
	m_bEventSummary = pdevice->m_services_supported.GetBit(4) ? TRUE : FALSE;
	m_bEventInfo = pdevice->m_services_supported.GetBit(39) ? TRUE : FALSE;
	m_nEvents = pdevice->m_nEvents;

}


void VTSDevicesTreeOptPage::CtrlToDevice( VTSDevice * pdevice )
{
	if ( pdevice == NULL )
		return;

	pdevice->m_nEvents = m_nEvents;
	if ( m_bAlarmSummary )
		pdevice->m_services_supported.SetBit(3);
	else
		pdevice->m_services_supported.ResetBit(3);

	if ( m_bEventSummary )
		pdevice->m_services_supported.SetBit(4);
	else
		pdevice->m_services_supported.ResetBit(4);

	if ( m_bEventInfo )
		pdevice->m_services_supported.SetBit(39);
	else
		pdevice->m_services_supported.ResetBit(39);
}


void VTSDevicesTreeOptPage::EnableControls( bool fEnable )
{
	GetDlgItem( IDC_SESUM )->EnableWindow( fEnable );
	GetDlgItem( IDC_SASUM )->EnableWindow( fEnable );
	GetDlgItem( IDC_SINFO )->EnableWindow( fEnable );
	GetDlgItem( IDC_EVENTS )->EnableWindow( fEnable );
}


void VTSDevicesTreeOptPage::OnSaveChange() 
{
	UpdateData(TRUE);
	CtrlToDevice(m_pdevice);

	CString ss = m_pdevice->m_services_supported.ToString();
}
