// VTSStatisticsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSStatisticsDlg.h"

#include "VTSStatisticsCollector.h"
#include "VTSStatisticsOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "VTSDOc.h"
/////////////////////////////////////////////////////////////////////////////
// VTSStatisticsDlg dialog
VTSStatisticsCollector* gStatisticsCollector;

VTSStatisticsDlg::VTSStatisticsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSStatisticsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSStatisticsDlg)
	m_nArcnetR = 0;
	m_nArcnetS = 0;
	m_nEthernetR = 0;
	m_nEthernetS = 0;
	m_nIPR = 0;
	m_nIPS = 0;
	m_nMSTPR = 0;
	m_strPacketType = _T("");
	m_nMSTPS = 0;
	m_nPTPR = 0;
	m_nPTPS = 0;
	m_nTotalR = 0;
	m_nTotalS = 0;
	//}}AFX_DATA_INIT

}


void VTSStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSStatisticsDlg)
	DDX_Control(pDX, IDC_SERVICE_TREE, m_Frame);
	DDX_Text(pDX, IDC_ARCNETRECEIVE, m_nArcnetR);
	DDX_Text(pDX, IDC_ARCNETSEND, m_nArcnetS);
	DDX_Text(pDX, IDC_ETHERNETRECEIVE, m_nEthernetR);
	DDX_Text(pDX, IDC_ETHERNETSEND, m_nEthernetS);
	DDX_Text(pDX, IDC_IPRECEIVE, m_nIPR);
	DDX_Text(pDX, IDC_IPSEND, m_nIPS);
	DDX_Text(pDX, IDC_MSTPRECEIVE, m_nMSTPR);
	DDX_Text(pDX, IDC_PACKET_TYPE_TEXT, m_strPacketType);
	DDX_Text(pDX, IDC_MSTPSEND, m_nMSTPS);
	DDX_Text(pDX, IDC_PTPRECEIVE, m_nPTPR);
	DDX_Text(pDX, IDC_PTPSEND, m_nPTPS);
	DDX_Text(pDX, IDC_TOTALRECEIVE, m_nTotalR);
	DDX_Text(pDX, IDC_TOTALSEND, m_nTotalS);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSStatisticsDlg, CDialog)
	//{{AFX_MSG_MAP(VTSStatisticsDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSStatisticsDlg message handlers

BOOL VTSStatisticsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//	m_pDoc->m_pStatisticsCollector=m_pDoc->m_pStatisticsCollector;
	gStatisticsCollector=m_pDoc->m_pStatisticsCollector;
	// TODO: Add extra initialization here

	//create tree list and initialize
	m_pList=new CTreeListCtrl(this);
	m_pList->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,0,0),this,0x1001);	
	m_pList->SetExtendedStyle(LVS_EX_GRIDLINES);
	m_pList->InitializeGrid();
	CRect rc;
	m_Frame.GetWindowRect(rc);
	ScreenToClient(rc);
	m_Frame.ShowWindow(SW_HIDE);
	if(m_pList->GetSafeHwnd())
			m_pList->MoveWindow(rc);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/***************************************************
Called when new packet is sent or received
to notify the tree list to update the data
of corresponding items.
****************************************************/
void VTSStatisticsDlg::Update(char* service)
{
	char pServiceName[100];
	GetServiceName(service,pServiceName);

	CString ServiceName(pServiceName);
	m_pList->UpdateStatistics(ServiceName);
	
}

void VTSStatisticsDlg::GetServiceName(char *summary, char *service)
{
	char* temp1=summary;
	char* temp2=service;
	while(*(temp1)!=0&&*(temp1)!=',')
	{
		*temp2++=*temp1++;
	}
	*temp2=0;
}

int VTSStatisticsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	return 0;
}

void VTSStatisticsDlg::OnOptions() 
{
	// TODO: Add your control notification handler code here
	VTSStatisticsOptionsDlg dlg;
	//get the new option parameters and re-build the statistics tree
	if(dlg.DoModal()==IDOK)
	{
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[0]=dlg.m_bIPSend;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[1]=dlg.m_bIPRecive;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[2]=dlg.m_bEthernetReceive;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[3]=dlg.m_bEthernetSend;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[4]=dlg.m_bArcnetSend;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[5]=dlg.m_bArcnetReceive;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[6]=dlg.m_bMSTPSend;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[7]=dlg.m_bMSTPReceive;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[8]=dlg.m_bPTPSend;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[9]=dlg.m_bPTPReceive;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[10]=dlg.m_bOverallSend;
		m_pDoc->m_pStatisticsCollector->m_bColumnStatus[11]=dlg.m_bOverallReceive;

		m_pDoc->m_pStatisticsCollector->m_nDistributionArray[1]=dlg.m_nSizeArray2;
		m_pDoc->m_pStatisticsCollector->m_nDistributionArray[2]=dlg.m_nSizeArray3;
		m_pDoc->m_pStatisticsCollector->m_nDistributionArray[3]=dlg.m_nSizeArray4;
		m_pDoc->m_pStatisticsCollector->m_nDistributionArray[4]=dlg.m_nSizeArray5;
	
		delete m_pList;
		
		//re-create the tree list
		m_pList=new CTreeListCtrl(this);
		m_pList->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,0,0),this,0x1001);	
		m_pList->SetExtendedStyle(LVS_EX_GRIDLINES);
		m_pList->InitializeGrid();
		CRect rc;
		m_Frame.GetWindowRect(rc);
		ScreenToClient(rc);
		m_Frame.ShowWindow(SW_HIDE);
		if(m_pList->GetSafeHwnd())
			m_pList->MoveWindow(rc);
	}
}

void VTSStatisticsDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	delete m_pList;
}

/*******************************************************
show new data on the right of the dialog when the 
selection of the tree list is changed
*******************************************************/
void VTSStatisticsDlg::UpdateStatisticsView(CString type, int data[12])
{
	m_nIPS = data[0];
	m_nIPR = data[1];
	m_nEthernetS = data[2];
	m_nEthernetR = data[3];
	m_nArcnetS = data[4];
	m_nArcnetR = data[5];
	m_nMSTPS = data[6];
	m_nMSTPR = data[7];
	m_nPTPS = data[8];
	m_nPTPR = data[9];
	m_nTotalS = data[10];
	m_nTotalR = data[11];
	m_strPacketType = type;
	UpdateData(false);
}
