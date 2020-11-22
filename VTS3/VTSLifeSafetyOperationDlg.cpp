// VTSLifeSafetyOperationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSLifeSafetyOperationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSLifeSafetyOperationDlg dialog


#pragma warning( disable : 4355 )
VTSLifeSafetyOperationDlg::VTSLifeSafetyOperationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSLifeSafetyOperationDlg::IDD, pParent)
	, m_enumcombo( this, IDC_ENUMRATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetLifeSafetyOperation, true )	
{
	//{{AFX_DATA_INIT(VTSLifeSafetyOperationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSLifeSafetyOperationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSLifeSafetyOperationDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSLifeSafetyOperationDlg, CDialog)
	//{{AFX_MSG_MAP(VTSLifeSafetyOperationDlg)
	ON_CBN_SELCHANGE(IDC_ENUMRATECOMBO, OnSelchangeEnumratecombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSLifeSafetyOperationDlg message handlers

void VTSLifeSafetyOperationDlg::OnSelchangeEnumratecombo() 
{	
	m_enumcombo.ctrlNull = false;
	m_enumcombo.CtrlToObj();
		
}
void VTSLifeSafetyOperationDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_enumcombo.Encode(enc, context);
}
void VTSLifeSafetyOperationDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_enumcombo.Decode(dec);
}
BOOL VTSLifeSafetyOperationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_enumcombo.LoadCombo();
	m_enumcombo.ObjToCtrl();
	
	return TRUE;  
}
