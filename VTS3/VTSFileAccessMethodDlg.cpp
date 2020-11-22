// VTSFileAccessMethodDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSFileAccessMethodDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSFileAccessMethodDlg dialog


#pragma warning( disable : 4355 )
VTSFileAccessMethodDlg::VTSFileAccessMethodDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSFileAccessMethodDlg::IDD, pParent)
	, m_enumcombo( this, IDC_ENUMRATECOMBO, NetworkSniffer::BAC_STRTAB_BACnetFileAccessMethod, true )	
{
	//{{AFX_DATA_INIT(VTSFileAccessMethodDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSFileAccessMethodDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSFileAccessMethodDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSFileAccessMethodDlg, CDialog)
	//{{AFX_MSG_MAP(VTSFileAccessMethodDlg)
	ON_CBN_SELCHANGE(IDC_ENUMRATECOMBO, OnSelchangeEnumratecombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSFileAccessMethodDlg message handlers
void VTSFileAccessMethodDlg::OnSelchangeEnumratecombo() 
{	
	m_enumcombo.ctrlNull = false;
	m_enumcombo.CtrlToObj();
		
}
void VTSFileAccessMethodDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_enumcombo.Encode(enc, context);
}
void VTSFileAccessMethodDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_enumcombo.Decode(dec);
}
BOOL VTSFileAccessMethodDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_enumcombo.LoadCombo();
	m_enumcombo.ObjToCtrl();
	
	return TRUE;  
}