// VTSEngineeringUnitsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEngineeringUnitsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSEngineeringUnitsDlg dialog


#pragma warning( disable : 4355 )
VTSEngineeringUnitsDlg::VTSEngineeringUnitsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEngineeringUnitsDlg::IDD, pParent)
	, m_enumcombo( this, IDC_ENUMRATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetEngineeringUnits, true )
{
	//{{AFX_DATA_INIT(VTSEngineeringUnitsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEngineeringUnitsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEngineeringUnitsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSEngineeringUnitsDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEngineeringUnitsDlg)
	ON_CBN_SELCHANGE(IDC_ENUMRATECOMBO, OnSelchangeEnumratecombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEngineeringUnitsDlg message handlers

void VTSEngineeringUnitsDlg::OnSelchangeEnumratecombo() 
{	
	m_enumcombo.ctrlNull = false;
	m_enumcombo.CtrlToObj();
		
}
void VTSEngineeringUnitsDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_enumcombo.Encode(enc, context);
}
void VTSEngineeringUnitsDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_enumcombo.Decode(dec);
}
BOOL VTSEngineeringUnitsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_enumcombo.LoadCombo();
	m_enumcombo.ObjToCtrl();
	
	return TRUE;  
}
