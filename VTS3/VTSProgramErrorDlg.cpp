// VTSProgramErrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSProgramErrorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSProgramErrorDlg dialog


#pragma warning( disable : 4355 )
VTSProgramErrorDlg::VTSProgramErrorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSProgramErrorDlg::IDD, pParent)
	, m_enumcombo( this, IDC_ENUMRATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetProgramError, true )
{
	//{{AFX_DATA_INIT(VTSProgramErrorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSProgramErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSProgramErrorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSProgramErrorDlg, CDialog)
	//{{AFX_MSG_MAP(VTSProgramErrorDlg)
	ON_CBN_SELCHANGE(IDC_ENUMRATECOMBO, OnSelchangeEnumratecombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSProgramErrorDlg message handlers

void VTSProgramErrorDlg::OnSelchangeEnumratecombo() 
{	
	m_enumcombo.ctrlNull = false;
	m_enumcombo.CtrlToObj();
		
}
void VTSProgramErrorDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_enumcombo.Encode(enc, context);
}
void VTSProgramErrorDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_enumcombo.Decode(dec);
}
BOOL VTSProgramErrorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_enumcombo.LoadCombo();
	m_enumcombo.ObjToCtrl();
	
	return TRUE;  
}