// VTSPropertyIdentifierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSPropertyIdentifierDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSPropertyIdentifierDlg dialog


#pragma warning( disable : 4355 )
VTSPropertyIdentifierDlg::VTSPropertyIdentifierDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSPropertyIdentifierDlg::IDD, pParent)
	, m_enumcombo( this, IDC_ENUMRATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
{
	//{{AFX_DATA_INIT(VTSPropertyIdentifierDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSPropertyIdentifierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPropertyIdentifierDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSPropertyIdentifierDlg, CDialog)
	//{{AFX_MSG_MAP(VTSPropertyIdentifierDlg)
	ON_CBN_SELCHANGE(IDC_ENUMRATECOMBO, OnSelchangeEnumratecombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPropertyIdentifierDlg message handlers

void VTSPropertyIdentifierDlg::OnSelchangeEnumratecombo() 
{	
	m_enumcombo.ctrlNull = false;
	m_enumcombo.CtrlToObj();
		
}
void VTSPropertyIdentifierDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_enumcombo.Encode(enc, context);
}
void VTSPropertyIdentifierDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_enumcombo.Decode(dec);
}
BOOL VTSPropertyIdentifierDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_enumcombo.LoadCombo();
	m_enumcombo.ObjToCtrl();
	
	return TRUE;  
}