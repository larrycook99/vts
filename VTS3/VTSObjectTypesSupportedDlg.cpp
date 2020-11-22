// VTSObjectTypesSupportedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSObjectTypesSupportedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSObjectTypesSupportedDlg dialog


VTSObjectTypesSupportedDlg::VTSObjectTypesSupportedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSObjectTypesSupportedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSObjectTypesSupportedDlg)
	m_aicheck = FALSE;
	m_aocheck = FALSE;
	m_avcheck = FALSE;
	m_avgcheck = FALSE;
	m_bicheck = FALSE;
	m_bocheck = FALSE;
	m_bvcheck = FALSE;
	m_cacheck = FALSE;
	m_cocheck = FALSE;
	m_decheck = FALSE;
	m_evcheck = FALSE;
	m_ficheck = FALSE;
	m_grcheck = FALSE;
	m_locheck = FALSE;
	m_lspcheck = FALSE;
	m_lszcheck = FALSE;
	m_msicheck = FALSE;
	m_msocheck = FALSE;
	m_msvcheck = FALSE;
	m_ntcheck = FALSE;
	m_prcheck = FALSE;
	m_sccheck = FALSE;
	m_trcheck = FALSE;
	m_SelectAll = FALSE;
	//}}AFX_DATA_INIT
}


void VTSObjectTypesSupportedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSObjectTypesSupportedDlg)
	DDX_Check(pDX, IDC_AICHECK, m_aicheck);
	DDX_Check(pDX, IDC_AOCHECK, m_aocheck);
	DDX_Check(pDX, IDC_AVCHECK, m_avcheck);
	DDX_Check(pDX, IDC_AVGCHECK, m_avgcheck);
	DDX_Check(pDX, IDC_BICHECK, m_bicheck);
	DDX_Check(pDX, IDC_BOCHECK, m_bocheck);
	DDX_Check(pDX, IDC_BVCHECK, m_bvcheck);
	DDX_Check(pDX, IDC_CACHECK, m_cacheck);
	DDX_Check(pDX, IDC_COCHECK, m_cocheck);
	DDX_Check(pDX, IDC_DECHECK, m_decheck);
	DDX_Check(pDX, IDC_EVCHECK, m_evcheck);
	DDX_Check(pDX, IDC_FICHECK, m_ficheck);
	DDX_Check(pDX, IDC_GRCHECK, m_grcheck);
	DDX_Check(pDX, IDC_LOCHECK, m_locheck);
	DDX_Check(pDX, IDC_LSPCHECK, m_lspcheck);
	DDX_Check(pDX, IDC_LSZCHECK, m_lszcheck);
	DDX_Check(pDX, IDC_MSICHECK, m_msicheck);
	DDX_Check(pDX, IDC_MSOCHECK, m_msocheck);
	DDX_Check(pDX, IDC_MSVCHECK, m_msvcheck);
	DDX_Check(pDX, IDC_NTCHECK, m_ntcheck);
	DDX_Check(pDX, IDC_PRCHECK, m_prcheck);
	DDX_Check(pDX, IDC_SCCHECK, m_sccheck);
	DDX_Check(pDX, IDC_TRCHECK, m_trcheck);
	DDX_Check(pDX, IDC_SELECTALL, m_SelectAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSObjectTypesSupportedDlg, CDialog)
	//{{AFX_MSG_MAP(VTSObjectTypesSupportedDlg)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSObjectTypesSupportedDlg message handlers

void VTSObjectTypesSupportedDlg::OnSelectall() 
{
	UpdateData();
	m_aicheck = m_SelectAll;
	m_aocheck = m_SelectAll;
	m_avcheck = m_SelectAll;
	m_avgcheck = m_SelectAll;
	m_bicheck = m_SelectAll;
	m_bocheck = m_SelectAll;
	m_bvcheck = m_SelectAll;
	m_cacheck = m_SelectAll;
	m_cocheck = m_SelectAll;
	m_decheck = m_SelectAll;
	m_evcheck = m_SelectAll;
	m_ficheck = m_SelectAll;
	m_grcheck = m_SelectAll;
	m_locheck = m_SelectAll;
	m_lspcheck = m_SelectAll;
	m_lszcheck = m_SelectAll;
	m_msicheck = m_SelectAll;
	m_msocheck = m_SelectAll;
	m_msvcheck = m_SelectAll;
	m_ntcheck = m_SelectAll;
	m_prcheck = m_SelectAll;
	m_sccheck = m_SelectAll;
	m_trcheck = m_SelectAll;
	UpdateData(FALSE);
}

void VTSObjectTypesSupportedDlg::OnCancel() 
{
	UpdateData();
	CDialog::OnCancel();
}

void VTSObjectTypesSupportedDlg::OnOK() 
{
	UpdateData();	
	CDialog::OnOK();
}

BOOL VTSObjectTypesSupportedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();		
	return TRUE;  
}
void VTSObjectTypesSupportedDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	BACnetBitString		bits( 23 );
	bits.SetBit( 0, (int)m_aicheck );
	bits.SetBit( 1, (int)m_aocheck );
	bits.SetBit( 2, (int)m_avcheck );
	bits.SetBit( 18, (int)m_avgcheck);
	bits.SetBit( 3, (int)m_bicheck );
	bits.SetBit( 4, (int)m_bocheck );
	bits.SetBit( 5, (int)m_bvcheck );
	bits.SetBit( 6, (int)m_cacheck );
	bits.SetBit( 7, (int)m_cocheck );
	bits.SetBit( 8, (int)m_decheck );
	bits.SetBit( 9, (int)m_evcheck );
	bits.SetBit( 10, (int)m_ficheck );
	bits.SetBit( 11, (int)m_grcheck );
	bits.SetBit( 12, (int)m_locheck );
	bits.SetBit( 21, (int)m_lspcheck );
	bits.SetBit( 22, (int)m_lszcheck );
	bits.SetBit( 13, (int)m_msicheck );
	bits.SetBit( 14, (int)m_msocheck );
	bits.SetBit( 19, (int)m_msvcheck );
	bits.SetBit( 15, (int)m_ntcheck );
	bits.SetBit( 16, (int)m_prcheck );
	bits.SetBit( 17, (int)m_sccheck );
	bits.SetBit( 20, (int)m_trcheck );
	bits.Encode( enc, context);	
}
void VTSObjectTypesSupportedDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength != 0)
	{
	BACnetBitString		bits( 23 );
	bits.Decode(dec);	
	m_aicheck = (bits.GetBit(0)!=0)?TRUE:FALSE;
	m_aocheck = (bits.GetBit(1)!=0)?TRUE:FALSE;
	m_avcheck = (bits.GetBit(2)!=0)?TRUE:FALSE;
	m_avgcheck = (bits.GetBit(18)!=0)?TRUE:FALSE;
	m_bicheck = (bits.GetBit(3)!=0)?TRUE:FALSE;
	m_bocheck = (bits.GetBit(4)!=0)?TRUE:FALSE;
	m_bvcheck = (bits.GetBit(5)!=0)?TRUE:FALSE;
	m_cacheck = (bits.GetBit(6)!=0)?TRUE:FALSE;
	m_cocheck = (bits.GetBit(7)!=0)?TRUE:FALSE;
	m_decheck = (bits.GetBit(8)!=0)?TRUE:FALSE;
	m_evcheck = (bits.GetBit(9)!=0)?TRUE:FALSE;
	m_ficheck = (bits.GetBit(10)!=0)?TRUE:FALSE;
	m_grcheck = (bits.GetBit(11)!=0)?TRUE:FALSE;
	m_locheck = (bits.GetBit(12)!=0)?TRUE:FALSE;
	m_lspcheck = (bits.GetBit(21)!=0)?TRUE:FALSE;
	m_lszcheck = (bits.GetBit(22)!=0)?TRUE:FALSE;
	m_msicheck = (bits.GetBit(13)!=0)?TRUE:FALSE;
	m_msocheck = (bits.GetBit(14)!=0)?TRUE:FALSE;
	m_msvcheck = (bits.GetBit(19)!=0)?TRUE:FALSE;
	m_ntcheck = (bits.GetBit(15)!=0)?TRUE:FALSE;
	m_prcheck = (bits.GetBit(16)!=0)?TRUE:FALSE;
	m_sccheck = (bits.GetBit(17)!=0)?TRUE:FALSE;
	m_trcheck = (bits.GetBit(20)!=0)?TRUE:FALSE;
	}
}
