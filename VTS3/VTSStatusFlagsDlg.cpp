// VTSStatusFlagsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSStatusFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSStatusFlagsDlg dialog


#pragma warning( disable : 4355 )
VTSStatusFlagsDlg::VTSStatusFlagsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSStatusFlagsDlg::IDD, pParent)
	, m_StatusFlags( this )
{
	//{{AFX_DATA_INIT(VTSStatusFlagsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSStatusFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSStatusFlagsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_StatusFlags.UpdateData( pDX->m_bSaveAndValidate );	
}


BEGIN_MESSAGE_MAP(VTSStatusFlagsDlg, CDialog)
	//{{AFX_MSG_MAP(VTSStatusFlagsDlg)
	ON_BN_CLICKED(IDC_OVERRIDDEN, OnOverridden)
	ON_BN_CLICKED(IDC_OUTOFSERVICE, OnOutofservice)
	ON_BN_CLICKED(IDC_INALARM, OnInalarm)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSStatusFlagsDlg message handlers

void VTSStatusFlagsDlg::OnOverridden() 
{
	m_StatusFlags.OverriddenClick();
}

void VTSStatusFlagsDlg::OnOutofservice() 
{
  m_StatusFlags.OutOfServiceClick();
}

void VTSStatusFlagsDlg::OnInalarm() 
{
  m_StatusFlags.InAlarmClick();
}

void VTSStatusFlagsDlg::OnFault() 
{
	m_StatusFlags.FaultClick();
}

BOOL VTSStatusFlagsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	return TRUE; 
}
void VTSStatusFlagsDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	m_StatusFlags.Encode(enc, context);
}
void VTSStatusFlagsDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength != 0)
	{
		m_StatusFlags.Decode(dec);
	}
}
