// VTSPolarityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSPolarityDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSPolarityDlg dialog


#pragma warning( disable : 4355 )
VTSPolarityDlg::VTSPolarityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSPolarityDlg::IDD, pParent)
	, m_enumcombo( this, IDC_ENUMRATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetPolarity, true )
{
	//{{AFX_DATA_INIT(VTSPolarityDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSPolarityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPolarityDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSPolarityDlg, CDialog)
	//{{AFX_MSG_MAP(VTSPolarityDlg)
	ON_CBN_SELCHANGE(IDC_ENUMRATECOMBO, OnSelchangeEnumratecombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPolarityDlg message handlers


void VTSPolarityDlg::OnSelchangeEnumratecombo() 
{	
	m_enumcombo.ctrlNull = false;
	m_enumcombo.CtrlToObj();
		
}
void VTSPolarityDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_enumcombo.Encode(enc, context);
}
void VTSPolarityDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_enumcombo.Decode(dec);
}
BOOL VTSPolarityDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_enumcombo.LoadCombo();
	m_enumcombo.ObjToCtrl();
	
	return TRUE;  
}
