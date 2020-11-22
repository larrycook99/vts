// VTSBinaryPVDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSBinaryPVDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace NetworkSniffer {
	extern char *BACnetBinaryPV[];
}
/////////////////////////////////////////////////////////////////////////////
// VTSBinaryPVDlg dialog


#pragma warning( disable : 4355 )
VTSBinaryPVDlg::VTSBinaryPVDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSBinaryPVDlg::IDD, pParent)
	, m_BinaryPVmbo( this, IDC_BINARYPVCOMBO, NetworkSniffer::BAC_STRTAB_BACnetBinaryPV, true )
{
	//{{AFX_DATA_INIT(VTSBinaryPVDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSBinaryPVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSBinaryPVDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSBinaryPVDlg, CDialog)
	//{{AFX_MSG_MAP(VTSBinaryPVDlg)
	ON_CBN_SELCHANGE(IDC_BINARYPVCOMBO, OnSelchangeBinarypvcombo)
	ON_CBN_DROPDOWN(IDC_BINARYPVCOMBO, OnDropdownBinarypvcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSBinaryPVDlg message handlers
void VTSBinaryPVDlg::OnDropdownBinarypvcombo() 
{
	// TODO: Add your control notification handler code here
	
}
void VTSBinaryPVDlg::OnSelchangeBinarypvcombo() 
{	
	m_BinaryPVmbo.ctrlNull = false;
	m_BinaryPVmbo.CtrlToObj();
		
}
void VTSBinaryPVDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_BinaryPVmbo.Encode(enc);
}
void VTSBinaryPVDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_BinaryPVmbo.Decode(dec);
}
BOOL VTSBinaryPVDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_BinaryPVmbo.LoadCombo();
	m_BinaryPVmbo.ObjToCtrl();
	return TRUE;  
}
