// MSTP_NBLinkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "WinMSTP.h"
#include "MSTP_NBLinkDlg.h"
#include "nbsettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSTP_NBLinkDlg dialog

extern void CheckLastError( const char *pTheFormat );
extern const char* GetSystemErrorMsg( CString &theResult, const char *pTheFormat, int nError );

const LPCTSTR lpctstrDisclaimer = "The BACnet Testing Laboratories (BTL) has not tested the American Auto-Matrix NB-Link for conformance to the BACnet standard, nor does the BTL endorse the NB-Link as a product.";

CMSTP_NBLinkDlg::CMSTP_NBLinkDlg( CWnd* pParent /*=NULL*/ )
	: CDialog(CMSTP_NBLinkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMSTP_NBLinkDlg)
	m_nLocalPort = 0;
	m_fUseReset = FALSE;
	m_nListenPort = 0;
	m_fInitOnStartup = TRUE;
	//}}AFX_DATA_INIT

    m_fontFineprint.CreatePointFont(60, "Arial", NULL);	
}


void CMSTP_NBLinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMSTP_NBLinkDlg)
	DDX_Control(pDX, IDC_DISCLAIMER, m_ctrlEditDisclaimer);
	DDX_Control(pDX, IDC_NBLINKGATEWAY, m_ctrlIPAddrGateway);
	DDX_Control(pDX, IDC_NBLINKSUBNETMASK, m_ctrlSubnetMask);
	DDX_Control(pDX, IDC_NBLINKRESETADDR, m_ctrlIPAddrReset);
	DDX_Control(pDX, IDC_NBLINKADDR, m_ctrlIPAddr);
	DDX_Text(pDX, IDC_NBLINKLOCALPORT, m_nLocalPort);
	DDX_Check(pDX, IDC_NBLINKRESET, m_fUseReset);
	DDX_Text(pDX, IDC_NBLINKSOCKET, m_nListenPort);
	DDX_Check(pDX, IDC_NBLINKSTARTUP, m_fInitOnStartup);
	//}}AFX_DATA_MAP

	if ( pDX->m_bSaveAndValidate )
	{
		GetIPFromCtrl(&m_ctrlIPAddr, &m_strIPAddr);
		GetIPFromCtrl(&m_ctrlSubnetMask, &m_strIPMask);
		GetIPFromCtrl(&m_ctrlIPAddrGateway, &m_strIPGateway);
	}
	else
	{
		LoadIPCtrl(&m_ctrlIPAddr, &m_strIPAddr);
		LoadIPCtrl(&m_ctrlSubnetMask, &m_strIPMask);
		LoadIPCtrl(&m_ctrlIPAddrGateway, &m_strIPGateway);
	}
}



void CMSTP_NBLinkDlg::GetIPFromCtrl( CIPAddressCtrl * pIPCtrl, CString * pstr )
{
	BYTE b0, b1, b2, b3;
	b0 = b1 = b2 = b3 = 0;

	pIPCtrl->GetAddress(b0, b1, b2, b3);
	pstr->Format("%d.%d.%d.%d", b0, b1, b2, b3);
}



void CMSTP_NBLinkDlg::LoadIPCtrl( CIPAddressCtrl * pIPCtrl, CString * pstr )
{
	BYTE b[4];
	b[0] = b[1] = b[2] = b[3] = 0;

	CString str = *pstr;
	LPTSTR p = strtok(str.GetBuffer(1), ".");

	for ( int i = 0; i < sizeof(b); i++, p = strtok(NULL, ".") )
		if ( p != NULL )
			b[i] = atoi(p);

	pIPCtrl->SetAddress(b[0], b[1], b[2], b[3]); 
}



BEGIN_MESSAGE_MAP(CMSTP_NBLinkDlg, CDialog)
	//{{AFX_MSG_MAP(CMSTP_NBLinkDlg)
	ON_BN_CLICKED(ID_APPLY, OnReset)
	ON_BN_CLICKED(IDC_NBLINKRESET, OnNblinkreset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSTP_NBLinkDlg message handlers

void CMSTP_NBLinkDlg::OnReset() 
{
	// Get the data into the variables....
	CDataExchange ddx(this, TRUE);
	DoDataExchange(&ddx);

	CString strIPReset;
	CString * pstrIP = &m_strIPAddr;

	if ( m_fUseReset )
	{
		GetIPFromCtrl(&m_ctrlIPAddrReset, &strIPReset);
		pstrIP = &strIPReset;
	}

	// Link to NB on specified IP... could be the one used for reset or the one specified
	int nError = NBInit(*pstrIP);

	if ( nError == 0 )
	{
		// Now see if the parms are equal... kind of cheesy, eh?
		CString strNBLinkParms;
		CString strIPThis;
		MSTPImp_NBLink::GetLocalIPAddrString(&strIPThis);

		MSTPImp_NBLink::FormatNBLinkString(&strNBLinkParms, strIPThis, m_strIPAddr, m_strIPMask, m_strIPGateway,	m_pnblink->m_pmstpData->m_strBaud,
					m_nListenPort, m_nLocalPort, m_pnblink->m_pmstpData->m_nMAC, m_pnblink->m_pmstpData->m_nMaxMaster, m_pnblink->m_pmstpData->m_nMaxInfoFrame);

		nError = NBSetLinkConfiguration(strNBLinkParms);

		if ( nError != 0  &&  nError == INVALID_SUBNET )
		{
			AfxMessageBox("MS/TP NB-Link reconfiguration failure.  The subnet mask is invalid.");
		}
	}

	if ( nError > 0 )		// use -1 if error but not WSAE error
	{
		CString str;
		GetSystemErrorMsg( str, "MS/TP NBLink Reset Error:\n%s", nError );
		AfxMessageBox(str);
	}

	NBUnload();
}




void CMSTP_NBLinkDlg::OnOK() 
{
	CDialog::OnOK();
}


void CMSTP_NBLinkDlg::OnCancel() 
{
	CDialog::OnCancel();
}


BOOL CMSTP_NBLinkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlIPAddrReset.SetAddress(0, 0, 0, 0);
	OnNblinkreset();
	
	// TODO: Add extra initialization here

    m_ctrlEditDisclaimer.SetWindowText(lpctstrDisclaimer);
	m_ctrlEditDisclaimer.SetFont(&m_fontFineprint);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMSTP_NBLinkDlg::OnNblinkreset() 
{
	bool bChecked = ((CButton *) GetDlgItem(IDC_NBLINKRESET))->GetCheck() == 1;

	m_ctrlIPAddrReset.EnableWindow(bChecked);
}

BOOL CMSTP_NBLinkDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
    m_fontFineprint.DeleteObject();	

	return CDialog::DestroyWindow();
}
