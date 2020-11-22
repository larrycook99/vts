// VTSDevicesTreeDevPage.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDoc.h"

#include "VTSPropValue.h"
#include "VTSDevicesTreeDevPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeDevPage property page

IMPLEMENT_DYNCREATE(VTSDevicesTreeDevPage, VTSPropertyPage)

VTSDevicesTreeDevPage::VTSDevicesTreeDevPage( VTSPageCallbackInterface * pparent )
                      :VTSPropertyPage(VTSDevicesTreeDevPage::IDD, pparent)
{
	//{{AFX_DATA_INIT(VTSDevicesTreeDevPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// duplicate the member initalization that the class 'wizard' adds

	ClearData();
	m_pdevice = NULL;
}


VTSDevicesTreeDevPage::VTSDevicesTreeDevPage()
                      :VTSPropertyPage()
{
	ASSERT(0);
	//Not used
}


VTSDevicesTreeDevPage::~VTSDevicesTreeDevPage()
{
}


void VTSDevicesTreeDevPage::ClearData(void)
{
	m_Name = _T("");
	m_nInstance = 0;
	m_nSegSize = 0;
	m_nMaxSegs = 0;
	m_nWindowSize = 0;
	m_nNextInvokeID = 0;
	m_nMaxAPDUSize = 0;
	m_nAPDUTimeout = 0;
	m_nAPDUSegTimeout = 0;
	m_nAPDURetries = 0;
	m_nVendorID = 0;
	m_Segmentation = -1;
	m_nRouter = 0;
}



void VTSDevicesTreeDevPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDevicesTreeDevPage)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_INSTANCE, m_nInstance);
	DDX_Text(pDX, IDC_SEGSIZE, m_nSegSize);
	DDX_Text(pDX, IDC_MAXSEGS, m_nMaxSegs);
	DDX_Text(pDX, IDC_WINDOWSIZE, m_nWindowSize);
	DDX_Text(pDX, IDC_NEXTINVOKEID, m_nNextInvokeID);
	DDX_Text(pDX, IDC_MAXAPDUSIZE, m_nMaxAPDUSize);
	DDX_Text(pDX, IDC_APDUTIMEOUT, m_nAPDUTimeout);
	DDX_Text(pDX, IDC_APDUSEGTIMEOUT, m_nAPDUSegTimeout);
	DDX_Text(pDX, IDC_APDURETRIES, m_nAPDURetries);
	DDX_Text(pDX, IDC_VENDORID, m_nVendorID);
	DDX_Radio(pDX, IDC_SEGBOTH, m_Segmentation);
	DDX_Check(pDX, IDC_ROUTER, m_nRouter);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDevicesTreeDevPage, VTSPropertyPage)
	//{{AFX_MSG_MAP(VTSDevicesTreeDevPage)
	ON_BN_CLICKED(IDC_IAM, OnIAm)
	ON_EN_CHANGE(IDC_NAME, OnSaveNameChange)
	ON_EN_CHANGE(IDC_INSTANCE, OnSaveNameChange)
	ON_EN_CHANGE(IDC_MAXAPDUSIZE, OnSaveChange)
	ON_EN_CHANGE(IDC_NEXTINVOKEID, OnSaveChange)
	ON_BN_CLICKED(IDC_SEGBOTH, OnSaveChange)
	ON_BN_CLICKED(IDC_SEGNONE, OnSaveChange)
	ON_BN_CLICKED(IDC_SEGRECEIVE, OnSaveChange)
	ON_EN_CHANGE(IDC_SEGSIZE, OnSaveChange)
	ON_EN_CHANGE(IDC_MAXSEGS, OnSaveChange)
	ON_BN_CLICKED(IDC_SEGTRANSMIT, OnSaveChange)
	ON_BN_CLICKED(IDC_SEGTYPE, OnSaveChange)
	ON_EN_CHANGE(IDC_VENDORID, OnSaveChange)
	ON_EN_CHANGE(IDC_WINDOWSIZE, OnSaveChange)
	ON_EN_CHANGE(IDC_APDUTIMEOUT, OnSaveChange)
	ON_EN_CHANGE(IDC_APDUSEGTIMEOUT, OnSaveChange)
	ON_EN_CHANGE(IDC_APDURETRIES, OnSaveChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeDevPage message handlers



BOOL VTSDevicesTreeDevPage::OnSetActive() 
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


BOOL VTSDevicesTreeDevPage::OnKillActive() 
{
	// calls DDX
	VTSPropertyPage::OnKillActive();
	EnableControls(false);	
	CtrlToDevice(m_pdevice);
	return TRUE;
}



void VTSDevicesTreeDevPage::DeviceToCtrl( VTSDevice * pdevice )
{
	if ( pdevice == NULL )
	{
		ClearData();
		return;
	}

	m_Name = pdevice->m_strName;
	m_nInstance = pdevice->m_nInstance;
	m_nSegSize = pdevice->m_nSegmentSize;
	m_nMaxSegs = pdevice->m_nMaxSegs;
	m_nWindowSize = pdevice->m_nWindowSize;
	m_nNextInvokeID = pdevice->m_nNextInvokeID;
	m_nMaxAPDUSize = pdevice->m_nMaxAPDUSize;
	m_nAPDUTimeout = pdevice->m_nAPDUTimeout;
	m_nAPDUSegTimeout = pdevice->m_nAPDUSegmentTimeout;
	m_nAPDURetries = pdevice->m_nAPDURetries;
	m_nVendorID = pdevice->m_nVendorID;
	m_Segmentation = pdevice->m_segmentation;
	m_nRouter = pdevice->m_fRouter ? 1 : 0;
}


void VTSDevicesTreeDevPage::CtrlToDevice( VTSDevice * pdevice )
{
	if ( pdevice == NULL )
		return;

	pdevice->m_strName = m_Name;
	pdevice->m_nInstance = m_nInstance;
	pdevice->m_fRouter = m_nRouter != 0;
	pdevice->m_nSegmentSize = m_nSegSize;
	pdevice->m_nMaxSegs = m_nMaxSegs;
	pdevice->m_nWindowSize = m_nWindowSize;
	pdevice->m_nMaxAPDUSize = m_nMaxAPDUSize;
	pdevice->m_nNextInvokeID = m_nNextInvokeID;
	pdevice->m_nAPDUTimeout = m_nAPDUTimeout;
	pdevice->m_nAPDUSegmentTimeout = m_nAPDUSegTimeout;
	pdevice->m_nAPDURetries = m_nAPDURetries;
	pdevice->m_nVendorID = m_nVendorID;
	pdevice->m_segmentation = (BACnetSegmentation) m_Segmentation;
}


void VTSDevicesTreeDevPage::EnableControls( bool fEnable )
{
	GetDlgItem( IDC_NAME )->EnableWindow( fEnable );
	GetDlgItem( IDC_INSTANCE )->EnableWindow( fEnable );
	GetDlgItem( IDC_SEGSIZE )->EnableWindow( fEnable );
	GetDlgItem( IDC_MAXSEGS )->EnableWindow( fEnable );
	GetDlgItem( IDC_WINDOWSIZE )->EnableWindow( fEnable );
	GetDlgItem( IDC_NEXTINVOKEID )->EnableWindow( fEnable );
	GetDlgItem( IDC_MAXAPDUSIZE )->EnableWindow( fEnable );
	GetDlgItem( IDC_APDUTIMEOUT )->EnableWindow( fEnable );
	GetDlgItem( IDC_APDUSEGTIMEOUT )->EnableWindow( fEnable );
	GetDlgItem( IDC_APDURETRIES )->EnableWindow( fEnable );
	GetDlgItem( IDC_VENDORID )->EnableWindow( fEnable );
	GetDlgItem( IDC_SEGBOTH )->EnableWindow( fEnable );
	GetDlgItem( IDC_SEGTRANSMIT )->EnableWindow( fEnable );
	GetDlgItem( IDC_SEGRECEIVE )->EnableWindow( fEnable );
	GetDlgItem( IDC_SEGNONE )->EnableWindow( fEnable );
	GetDlgItem( IDC_IAM )->EnableWindow( fEnable );
}


void VTSDevicesTreeDevPage::OnIAm()
{
	// Don't send the I_Am from the device being edited... it's not bound to anything
	// So find it's original device (given by name, hopefully unchanged) and 
	// tell THAT device to send IAm... it's already attached to a port and everyting.

	VTSDevice * pdeviceReal = ((VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace())->GetDevices()->Find(m_pdevice->GetName());

	if ( pdeviceReal == NULL )
	{
		CString str;
		str.Format(IDS_ERR_NOREALDEVICENAME, m_pdevice->GetName());
		AfxMessageBox(str);
	}
	else
	{
		pdeviceReal->IAm();
	}
}


void VTSDevicesTreeDevPage::OnSaveNameChange( void )
{
	UpdateData(TRUE);
	CtrlToDevice(m_pdevice);
	NotifyOfDataChange();
}


void VTSDevicesTreeDevPage::OnSaveChange() 
{
	UpdateData(TRUE);
	CtrlToDevice(m_pdevice);
}
