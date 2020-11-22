// VTSStatisticsOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSStatisticsOptionsDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "VTSStatisticsCollector.h"

/////////////////////////////////////////////////////////////////////////////
// VTSStatisticsOptionsDlg dialog


extern VTSStatisticsCollector* gStatisticsCollector;

VTSStatisticsOptionsDlg::VTSStatisticsOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSStatisticsOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSStatisticsOptionsDlg)
	m_bArcnetReceive = FALSE;
	m_bArcnetSend = FALSE;
	m_bEthernetReceive = FALSE;
	m_bIPRecive = FALSE;
	m_bEthernetSend = FALSE;
	m_bIPSend = FALSE;
	m_bMSTPReceive = FALSE;
	m_bMSTPSend = FALSE;
	m_bOverallReceive = FALSE;
	m_bOverallSend = FALSE;
	m_bPTPReceive = FALSE;
	m_bPTPSend = FALSE;
	m_nSizeArray2 = 0;
	m_nSizeArray3 = 0;
	m_nSizeArray4 = 0;
	m_nSizeArray5 = 0;
	//}}AFX_DATA_INIT
}


void VTSStatisticsOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSStatisticsOptionsDlg)
	DDX_Check(pDX, IDC_ARCNET_RECEIVE_CHECK, m_bArcnetReceive);
	DDX_Check(pDX, IDC_ARCNET_SEND_CHECK, m_bArcnetSend);
	DDX_Check(pDX, IDC_ETHERNET_RECEIVE_CHECK, m_bEthernetReceive);
	DDX_Check(pDX, IDC_IP_RECEIVE_CHECK, m_bIPRecive);
	DDX_Check(pDX, IDC_ETHERNET_SEND_CHECK, m_bEthernetSend);
	DDX_Check(pDX, IDC_IP_SEND_CHECK, m_bIPSend);
	DDX_Check(pDX, IDC_MSTP_RECEIVE_CHECK, m_bMSTPReceive);
	DDX_Check(pDX, IDC_MSTP_SEND_CHECK, m_bMSTPSend);
	DDX_Check(pDX, IDC_OVERALL_RECEIVE_CHECK, m_bOverallReceive);
	DDX_Check(pDX, IDC_OVERALL_SEND_CHECK, m_bOverallSend);
	DDX_Check(pDX, IDC_PTP_RECEIVE_CHECK, m_bPTPReceive);
	DDX_Check(pDX, IDC_PTP_SEND_CHECK, m_bPTPSend);
	DDX_Text(pDX, IDC_RANGE_2, m_nSizeArray2);
	DDX_Text(pDX, IDC_RANGE_3, m_nSizeArray3);
	DDX_Text(pDX, IDC_RANGE_4, m_nSizeArray4);
	DDX_Text(pDX, IDC_RANGE_5, m_nSizeArray5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSStatisticsOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(VTSStatisticsOptionsDlg)
	ON_BN_CLICKED(IDC_SEL_ALL, OnSelAll)
	ON_BN_CLICKED(IDC_CLEAR_ALL, OnClearAll)
	ON_EN_CHANGE(IDC_RANGE_2, OnChangeRange2)
	ON_EN_CHANGE(IDC_RANGE_4, OnChangeRange4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSStatisticsOptionsDlg message handlers

BOOL VTSStatisticsOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bIPSend = gStatisticsCollector->m_bColumnStatus[0];
	m_bIPRecive = gStatisticsCollector->m_bColumnStatus[1];
	m_bEthernetReceive = gStatisticsCollector->m_bColumnStatus[2];
	m_bEthernetSend = gStatisticsCollector->m_bColumnStatus[3];
	m_bArcnetSend = gStatisticsCollector->m_bColumnStatus[4];
	m_bArcnetReceive = gStatisticsCollector->m_bColumnStatus[5];
	m_bMSTPSend = gStatisticsCollector->m_bColumnStatus[6];
	m_bMSTPReceive = gStatisticsCollector->m_bColumnStatus[7];
	m_bPTPSend = gStatisticsCollector->m_bColumnStatus[8];
	m_bPTPReceive = gStatisticsCollector->m_bColumnStatus[9];
	m_bOverallSend = gStatisticsCollector->m_bColumnStatus[10];
	m_bOverallReceive = gStatisticsCollector->m_bColumnStatus[11];
	
	m_nSizeArray1=gStatisticsCollector->m_nDistributionArray[0];
	m_nSizeArray2=gStatisticsCollector->m_nDistributionArray[1];
	m_nSizeArray3=gStatisticsCollector->m_nDistributionArray[2];
	m_nSizeArray4=gStatisticsCollector->m_nDistributionArray[3];
	m_nSizeArray5=gStatisticsCollector->m_nDistributionArray[4];

	UpdateData(false);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void VTSStatisticsOptionsDlg::OnSelAll() 
{
	// TODO: Add your control notification handler code here
	m_bArcnetReceive = TRUE;
	m_bArcnetSend = TRUE;
	m_bEthernetReceive = TRUE;
	m_bIPRecive = TRUE;
	m_bEthernetSend = TRUE;
	m_bIPSend = TRUE;
	m_bMSTPReceive = TRUE;
	m_bMSTPSend = TRUE;
	m_bOverallReceive = TRUE;
	m_bOverallSend = TRUE;
	m_bPTPReceive = TRUE;
	m_bPTPSend = TRUE;
	UpdateData(FALSE);
}

void VTSStatisticsOptionsDlg::OnClearAll() 
{
	// TODO: Add your control notification handler code here
	m_bArcnetReceive = FALSE;
	m_bArcnetSend = FALSE;
	m_bEthernetReceive = FALSE;
	m_bIPRecive = FALSE;
	m_bEthernetSend = FALSE;
	m_bIPSend = FALSE;
	m_bMSTPReceive = FALSE;
	m_bMSTPSend = FALSE;
	m_bOverallReceive = FALSE;
	m_bOverallSend = FALSE;
	m_bPTPReceive = FALSE;
	m_bPTPSend = FALSE;
	UpdateData(FALSE);
}


void VTSStatisticsOptionsDlg::OnChangeRange2() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(true);
	m_nSizeArray3=m_nSizeArray2+1;
	if(m_nSizeArray2>m_nSizeArray4)
	{
		m_nSizeArray4=m_nSizeArray3+1;
		m_nSizeArray5=m_nSizeArray4+1;
	}
	UpdateData(false);
}

void VTSStatisticsOptionsDlg::OnChangeRange4() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(true);
	m_nSizeArray5=m_nSizeArray4+1;
	UpdateData(false);
}

void VTSStatisticsOptionsDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	if(m_nSizeArray4<=m_nSizeArray3)
	{
		CString sub="";
		sub.Format("%s %d ","Please enter a number bigger than ",m_nSizeArray3);
		AfxMessageBox(sub);
		GetDlgItem(IDC_RANGE_4)->SetFocus();
		return;
	}
	else
		CDialog::OnOK();
}
