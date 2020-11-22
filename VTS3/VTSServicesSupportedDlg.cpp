// VTSServicesSupportedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSServicesSupportedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSServicesSupportedDlg dialog


VTSServicesSupportedDlg::VTSServicesSupportedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSServicesSupportedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSServicesSupportedDlg)
	m_ackalarm = FALSE;
	m_addlist = FALSE;
	m_areadfile = FALSE;
	m_authen = FALSE;
	m_awritefile = FALSE;
	m_cfmcovn = FALSE;
	m_cfmeven = FALSE;
	m_cfmprvtrans = FALSE;
	m_cfmtextmsg = FALSE;
	m_creatobj = FALSE;
	m_delobj = FALSE;
	m_devcomctrl = FALSE;
	m_getalarmsum = FALSE;
	m_getenrsum = FALSE;
	m_getevninfo = FALSE;
	m_iam = FALSE;
	m_ihave = FALSE;
	m_lfsftyo = FALSE;
	m_readprop = FALSE;
	m_readpropc = FALSE;
	m_readpropm = FALSE;
	m_readrange = FALSE;
	m_reinitdev = FALSE;
	m_requestkey = FALSE;
	m_rmlist = FALSE;
	m_subscobprop = FALSE;
	m_subscov = FALSE;
	m_timesyn = FALSE;
	m_ucfmcovn = FALSE;
	m_ucfmeven = FALSE;
	m_ucfmprvtr = FALSE;
	m_ucfmtextmsg = FALSE;
	m_utctimesyn = FALSE;
	m_vtclose = FALSE;
	m_vtdata = FALSE;
	m_vtopen = FALSE;
	m_whohas = FALSE;
	m_whois = FALSE;
	m_writeprop = FALSE;
	m_writepropm = FALSE;
	m_SelectAll = FALSE;
	//}}AFX_DATA_INIT
}


void VTSServicesSupportedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSServicesSupportedDlg)
	DDX_Check(pDX, IDC_ACKALARM, m_ackalarm);
	DDX_Check(pDX, IDC_ADDLIST, m_addlist);
	DDX_Check(pDX, IDC_AREADFILE, m_areadfile);
	DDX_Check(pDX, IDC_AUTHEN, m_authen);
	DDX_Check(pDX, IDC_AWRITEFILE, m_awritefile);
	DDX_Check(pDX, IDC_CFMCOVN, m_cfmcovn);
	DDX_Check(pDX, IDC_CFMEVN, m_cfmeven);
	DDX_Check(pDX, IDC_CFMPRVTRANS, m_cfmprvtrans);
	DDX_Check(pDX, IDC_CFMTEXTMSG, m_cfmtextmsg);
	DDX_Check(pDX, IDC_CREATOBJ, m_creatobj);
	DDX_Check(pDX, IDC_DELOBJ, m_delobj);
	DDX_Check(pDX, IDC_DEVCOMCTRL, m_devcomctrl);
	DDX_Check(pDX, IDC_GETALARMSUM, m_getalarmsum);
	DDX_Check(pDX, IDC_GETENRSUM, m_getenrsum);
	DDX_Check(pDX, IDC_GETEVNINFO, m_getevninfo);
	DDX_Check(pDX, IDC_IAM, m_iam);
	DDX_Check(pDX, IDC_IHAVE, m_ihave);
	DDX_Check(pDX, IDC_LFSFTYO, m_lfsftyo);
	DDX_Check(pDX, IDC_READPROP, m_readprop);
	DDX_Check(pDX, IDC_READPROPC, m_readpropc);
	DDX_Check(pDX, IDC_READPROPM, m_readpropm);
	DDX_Check(pDX, IDC_READRANGE, m_readrange);
	DDX_Check(pDX, IDC_REINITDEV, m_reinitdev);
	DDX_Check(pDX, IDC_REQUESTKEY, m_requestkey);
	DDX_Check(pDX, IDC_RMLIST, m_rmlist);
	DDX_Check(pDX, IDC_SUBSCOBPROP, m_subscobprop);
	DDX_Check(pDX, IDC_SUBSCOV, m_subscov);
	DDX_Check(pDX, IDC_TIMESYN, m_timesyn);
	DDX_Check(pDX, IDC_UCFMCOVN, m_ucfmcovn);
	DDX_Check(pDX, IDC_UCFMEVEN, m_ucfmeven);
	DDX_Check(pDX, IDC_UCFMPRVTR, m_ucfmprvtr);
	DDX_Check(pDX, IDC_UCFMTEXTMSG, m_ucfmtextmsg);
	DDX_Check(pDX, IDC_UTCTIMESYN, m_utctimesyn);
	DDX_Check(pDX, IDC_VTCLOSE, m_vtclose);
	DDX_Check(pDX, IDC_VTDATA, m_vtdata);
	DDX_Check(pDX, IDC_VTOPEN, m_vtopen);
	DDX_Check(pDX, IDC_WHOHAS, m_whohas);
	DDX_Check(pDX, IDC_WHOIS, m_whois);
	DDX_Check(pDX, IDC_WRITEPROP, m_writeprop);
	DDX_Check(pDX, IDC_WRITEPROPM, m_writepropm);
	DDX_Check(pDX, IDC_SELECTALL, m_SelectAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSServicesSupportedDlg, CDialog)
	//{{AFX_MSG_MAP(VTSServicesSupportedDlg)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSServicesSupportedDlg message handlers

void VTSServicesSupportedDlg::OnSelectall() 
{
	UpdateData();
	m_ackalarm = m_SelectAll;
	m_addlist = m_SelectAll;
	m_areadfile = m_SelectAll;
	m_authen = m_SelectAll;
	m_awritefile = m_SelectAll;
	m_cfmcovn = m_SelectAll;
	m_cfmeven = m_SelectAll;
	m_cfmprvtrans = m_SelectAll;
	m_cfmtextmsg = m_SelectAll;
	m_creatobj = m_SelectAll;
	m_delobj = m_SelectAll;
	m_devcomctrl = m_SelectAll;
	m_getalarmsum = m_SelectAll;
	m_getenrsum = m_SelectAll;
	m_getevninfo = m_SelectAll;
	m_iam = m_SelectAll;
	m_ihave = m_SelectAll;
	m_lfsftyo = m_SelectAll;
	m_readprop = m_SelectAll;
	m_readpropc = m_SelectAll;
	m_readpropm = m_SelectAll;
	m_readrange = m_SelectAll;
	m_reinitdev = m_SelectAll;
	m_requestkey = m_SelectAll;
	m_rmlist = m_SelectAll;
	m_subscobprop = m_SelectAll;
	m_subscov = m_SelectAll;
	m_timesyn = m_SelectAll;
	m_ucfmcovn = m_SelectAll;
	m_ucfmeven = m_SelectAll;
	m_ucfmprvtr = m_SelectAll;
	m_ucfmtextmsg = m_SelectAll;
	m_utctimesyn = m_SelectAll;
	m_vtclose = m_SelectAll;
	m_vtdata = m_SelectAll;
	m_vtopen = m_SelectAll;
	m_whohas = m_SelectAll;
	m_whois = m_SelectAll;
	m_writeprop = m_SelectAll;
	m_writepropm = m_SelectAll;
 UpdateData(FALSE);
}

void VTSServicesSupportedDlg::OnCancel() 
{
    UpdateData();	
	CDialog::OnCancel();
}

void VTSServicesSupportedDlg::OnOK() 
{
    UpdateData();
	CDialog::OnOK();
}

BOOL VTSServicesSupportedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();		
	return TRUE; 
}
void VTSServicesSupportedDlg::Encode( BACnetAPDUEncoder& enc, int context)
{
	BACnetBitString		bits( 40 );
	bits.SetBit( 0, (int)m_ackalarm);
	bits.SetBit( 8, (int)m_addlist);
	bits.SetBit( 6, (int)m_areadfile);
	bits.SetBit( 24, (int)m_authen);
	bits.SetBit( 7, (int)m_awritefile);
	bits.SetBit( 1, (int)m_cfmcovn);
	bits.SetBit( 2, (int)m_cfmeven);
	bits.SetBit( 18, (int)m_cfmprvtrans);
	bits.SetBit(19, (int)m_cfmtextmsg);
	bits.SetBit( 10, (int)m_creatobj);
	bits.SetBit( 11, (int)m_delobj);
	bits.SetBit( 17, (int)m_devcomctrl);
	bits.SetBit( 3, (int)m_getalarmsum);
	bits.SetBit( 4, (int)m_getenrsum);
	bits.SetBit( 39, (int)m_getevninfo);
	bits.SetBit( 26, (int)m_iam);
	bits.SetBit( 27, (int)m_ihave);
	bits.SetBit( 37, (int)m_lfsftyo);
	bits.SetBit( 12, (int)m_readprop);
	bits.SetBit( 13, (int)m_readpropc);
	bits.SetBit( 14, (int)m_readpropm);
	bits.SetBit( 35, (int)m_readrange);
	bits.SetBit( 20, (int)m_reinitdev);
	bits.SetBit( 25, (int)m_requestkey);
	bits.SetBit( 9, (int)m_rmlist);
	bits.SetBit( 38, (int)m_subscobprop);
	bits.SetBit( 5, (int)m_subscov);
	bits.SetBit( 32, (int)m_timesyn);
	bits.SetBit( 28, (int)m_ucfmcovn);
	bits.SetBit( 29, (int)m_ucfmeven);
	bits.SetBit( 30, (int)m_ucfmprvtr);
	bits.SetBit( 31, (int)m_ucfmtextmsg);
	bits.SetBit( 36, (int)m_utctimesyn);
	bits.SetBit( 22, (int)m_vtclose);
	bits.SetBit( 23, (int)m_vtdata);
	bits.SetBit( 21, (int)m_vtopen);
	bits.SetBit( 33, (int)m_whohas);
	bits.SetBit( 34, (int)m_whois);
	bits.SetBit( 15, (int)m_writeprop);
	bits.SetBit( 16, (int)m_writepropm);
	bits.Encode(enc, context);
}
void VTSServicesSupportedDlg::Decode( BACnetAPDUDecoder& dec )
{
	if(dec.pktLength != 0)
	{
		BACnetBitString		bits( 40 );
		bits.Decode(dec);
		m_ackalarm= (bits.GetBit(0)!=0)?TRUE:FALSE;
		m_addlist= (bits.GetBit(8)!=0)?TRUE:FALSE;
		m_areadfile= (bits.GetBit(6)!=0)?TRUE:FALSE;
		m_authen= (bits.GetBit(24)!=0)?TRUE:FALSE;
		m_awritefile= (bits.GetBit(7)!=0)?TRUE:FALSE;
		m_cfmcovn= (bits.GetBit(1)!=0)?TRUE:FALSE;
		m_cfmeven= (bits.GetBit(2)!=0)?TRUE:FALSE;
		m_cfmprvtrans= (bits.GetBit(18)!=0)?TRUE:FALSE;
		m_cfmtextmsg= (bits.GetBit(19)!=0)?TRUE:FALSE;
		m_creatobj= (bits.GetBit(10)!=0)?TRUE:FALSE;
		m_delobj= (bits.GetBit(11)!=0)?TRUE:FALSE;
		m_devcomctrl= (bits.GetBit(17)!=0)?TRUE:FALSE;
		m_getalarmsum= (bits.GetBit(3)!=0)?TRUE:FALSE;
		m_getenrsum= (bits.GetBit(4)!=0)?TRUE:FALSE;
		m_getevninfo= (bits.GetBit(39)!=0)?TRUE:FALSE;
		m_iam= (bits.GetBit(26)!=0)?TRUE:FALSE;
		m_ihave= (bits.GetBit(27)!=0)?TRUE:FALSE;
		m_lfsftyo= (bits.GetBit(37)!=0)?TRUE:FALSE;
		m_readprop= (bits.GetBit(12)!=0)?TRUE:FALSE;
		m_readpropc= (bits.GetBit(13)!=0)?TRUE:FALSE;
		m_readpropm= (bits.GetBit(14)!=0)?TRUE:FALSE;
		m_readrange= (bits.GetBit(35)!=0)?TRUE:FALSE;
		m_reinitdev= (bits.GetBit(20)!=0)?TRUE:FALSE;
		m_requestkey= (bits.GetBit(25)!=0)?TRUE:FALSE;
		m_rmlist= (bits.GetBit(9)!=0)?TRUE:FALSE;
		m_subscobprop= (bits.GetBit(38)!=0)?TRUE:FALSE;
		m_subscov= (bits.GetBit(5)!=0)?TRUE:FALSE;
		m_timesyn= (bits.GetBit(32)!=0)?TRUE:FALSE;
		m_ucfmcovn= (bits.GetBit(28)!=0)?TRUE:FALSE;
		m_ucfmeven= (bits.GetBit(29)!=0)?TRUE:FALSE;
		m_ucfmprvtr= (bits.GetBit(30)!=0)?TRUE:FALSE;
		m_ucfmtextmsg= (bits.GetBit(31)!=0)?TRUE:FALSE;
		m_utctimesyn= (bits.GetBit(36)!=0)?TRUE:FALSE;
		m_vtclose= (bits.GetBit(22)!=0)?TRUE:FALSE;
		m_vtdata= (bits.GetBit(23)!=0)?TRUE:FALSE;
		m_vtopen= (bits.GetBit(21)!=0)?TRUE:FALSE;
		m_whohas= (bits.GetBit(33)!=0)?TRUE:FALSE;
		m_whois= (bits.GetBit(34)!=0)?TRUE:FALSE;
		m_writeprop= (bits.GetBit(15)!=0)?TRUE:FALSE;
		m_writepropm= (bits.GetBit(16)!=0)?TRUE:FALSE;
	}
}
