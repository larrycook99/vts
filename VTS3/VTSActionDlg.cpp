// VTSActionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSActionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSActionDlg dialog


#pragma warning( disable : 4355 )
VTSActionDlg::VTSActionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSActionDlg::IDD, pParent)
	, m_actcombo( this, IDC_ACTIONCOMBO, NetworkSniffer::BAC_STRTAB_BACnetAction, true )
{	
//	m_actcombo.ctrlNull = true;
	//{{AFX_DATA_INIT(VTSActionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSActionDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSActionDlg, CDialog)
	//{{AFX_MSG_MAP(VTSActionDlg)
	ON_CBN_DROPDOWN(IDC_ACTIONCOMBO, OnDropdownActioncombo)
	ON_CBN_SELCHANGE(IDC_ACTIONCOMBO, OnSelchangeActioncombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSActionDlg message handlers

void VTSActionDlg::OnDropdownActioncombo() 
{
	// TODO: Add your control notification handler code here
	
}
void VTSActionDlg::OnSelchangeActioncombo() 
{	
	m_actcombo.ctrlNull = false;
	m_actcombo.CtrlToObj();
		
}
void VTSActionDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_actcombo.Encode(enc, context);
}
void VTSActionDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_actcombo.Decode(dec);
}
BOOL VTSActionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_actcombo.LoadCombo();
	m_actcombo.ObjToCtrl();
	
	return TRUE;  
}
