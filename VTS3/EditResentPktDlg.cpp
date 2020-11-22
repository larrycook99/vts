// EditResentPktDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "EditResentPktDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditResentPktDlg dialog

CEditResentPktDlg::CEditResentPktDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditResentPktDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditResentPktDlg)
	m_dadr = _T("");
	m_dnet = 0;
	m_sadr = _T("");
	m_snet = 0;
	m_bSnet = FALSE;
	m_bDnet = FALSE;
	m_desIndex = -1;
	//}}AFX_DATA_INIT

	m_defaultDes = 0;
}


void CEditResentPktDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditResentPktDlg)
	DDX_Control(pDX, IDC_SADR, m_sadrCtrl);
	DDX_Control(pDX, IDC_SNET, m_snetCtrl);
	DDX_Control(pDX, IDC_DNET, m_dnetCtrl);
	DDX_Control(pDX, IDC_DADR, m_dadrCtrl);
	DDX_Control(pDX, IDC_DESCOMBO, m_desCombo);
	DDX_Text(pDX, IDC_DADR, m_dadr);
	DDX_Text(pDX, IDC_DNET, m_dnet);
	DDV_MinMaxUInt(pDX, m_dnet, 0, 65535);
	DDX_Text(pDX, IDC_SADR, m_sadr);
	DDX_Text(pDX, IDC_SNET, m_snet);
	DDV_MinMaxUInt(pDX, m_snet, 0, 65535);
	DDX_Check(pDX, IDC_SNETCHECK, m_bSnet);
	DDX_Check(pDX, IDC_DNETCHECK, m_bDnet);
	DDX_CBIndex(pDX, IDC_DESCOMBO, m_desIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditResentPktDlg, CDialog)
	//{{AFX_MSG_MAP(CEditResentPktDlg)
	ON_BN_CLICKED(IDC_DNETCHECK, OnDnetCheck)
	ON_BN_CLICKED(IDC_SNETCHECK, OnSnetCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditResentPktDlg message handlers

BOOL CEditResentPktDlg::OnInitDialog() 
{
	if (m_bDnet)
	{
		m_dnet = m_remoteDestination.addrNet;
		m_dadr = m_remoteDestination.MacAddress();
	}
	
	if (m_bSnet)
	{
		m_snet = m_remoteSource.addrNet;
		m_sadr = m_remoteSource.MacAddress();
	}

	CDialog::OnInitDialog();

	m_dnetCtrl.EnableWindow(m_bDnet);
	m_dadrCtrl.EnableWindow(m_bDnet);
	m_snetCtrl.EnableWindow(m_bSnet);
	m_sadrCtrl.EnableWindow(m_bSnet);

	POSITION pos = m_desArray.GetHeadPosition();
	while( pos )
	{
		m_desCombo.AddString(m_desArray.GetNext(pos).m_name);
	}

	m_desCombo.SetCurSel(m_defaultDes);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditResentPktDlg::OnDnetCheck() 
{
	UpdateData(TRUE);

	m_dnetCtrl.EnableWindow(m_bDnet);
	m_dadrCtrl.EnableWindow(m_bDnet);
}

void CEditResentPktDlg::OnSnetCheck() 
{
	UpdateData(TRUE);

	m_snetCtrl.EnableWindow(m_bSnet);
	m_sadrCtrl.EnableWindow(m_bSnet);	
}

void CEditResentPktDlg::OnOK() 
{
	UpdateData(TRUE);

	if (m_bSnet)
	{
		if (!m_remoteSource.SetMacAddress( m_sadr ))
		{
			MessageBox("Please input SADR in hex.", "Warning");
			return;
		}
		m_remoteSource.addrNet = m_snet;
	}

	if (m_bDnet)
	{
		if (!m_remoteDestination.SetMacAddress( m_dadr ))
		{
			MessageBox("Please input DADR in hex.", "Warning");
			return;
		}
		m_remoteDestination.addrNet = m_dnet;
	}

	CDialog::OnOK();
}
