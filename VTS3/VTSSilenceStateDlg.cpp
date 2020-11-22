// VTSSilenceStateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSSilenceStateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSSilenceStateDlg dialog


#pragma warning( disable : 4355 )
VTSSilenceStateDlg::VTSSilenceStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSSilenceStateDlg::IDD, pParent)
	, m_enumcombo( this, IDC_ENUMRATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetSilencedState, true )
{
	//{{AFX_DATA_INIT(VTSSilenceStateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSSilenceStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSSilenceStateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSSilenceStateDlg, CDialog)
	//{{AFX_MSG_MAP(VTSSilenceStateDlg)
	ON_CBN_SELCHANGE(IDC_ENUMRATECOMBO, OnSelchangeEnumratecombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSSilenceStateDlg message handlers

void VTSSilenceStateDlg::OnSelchangeEnumratecombo() 
{	
	m_enumcombo.ctrlNull = false;
	m_enumcombo.CtrlToObj();
		
}
void VTSSilenceStateDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_enumcombo.Encode(enc, context);
}
void VTSSilenceStateDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_enumcombo.Decode(dec);
}
BOOL VTSSilenceStateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_enumcombo.LoadCombo();
	m_enumcombo.ObjToCtrl();
	
	return TRUE;  
}
