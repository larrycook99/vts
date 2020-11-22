// VTSLogStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSLogStatusDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSLogStatusDlg dialog


#pragma warning( disable : 4355 )
VTSLogStatusDlg::VTSLogStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSLogStatusDlg::IDD, pParent)
	, m_logdisabled(this, IDC_LOGDISABLED, true)
	, m_bufferpurged(this, IDC_BUFFERPURGED, true)
{
	//{{AFX_DATA_INIT(VTSLogStatusDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSLogStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSLogStatusDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSLogStatusDlg, CDialog)
	//{{AFX_MSG_MAP(VTSLogStatusDlg)
	ON_BN_CLICKED(IDC_BUFFERPURGED, OnBufferpurged)
	ON_BN_CLICKED(IDC_LOGDISABLED, OnLogdisabled)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSLogStatusDlg message handlers

void VTSLogStatusDlg::OnBufferpurged() 
{
  m_bufferpurged.UpdateData();	
}

void VTSLogStatusDlg::OnLogdisabled() 
{
  m_logdisabled.UpdateData();	
}

BOOL VTSLogStatusDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_logdisabled.ObjToCtrl();
	m_bufferpurged.ObjToCtrl();
	return TRUE;  
}
void VTSLogStatusDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	BACnetBitString		bits(2 );
	bits.SetBit( 0, m_logdisabled.boolValue );
	bits.SetBit( 1, m_bufferpurged.boolValue );
	bits.Encode( enc, context);
}
void VTSLogStatusDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength!=0)
	{
		BACnetBitString		bits( 2 );
		bits.Decode(dec);
		m_logdisabled.boolValue = (bits.GetBit(0)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
		m_bufferpurged.boolValue= (bits.GetBit(1)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
	}
}
