// VTSDateRangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDateRangeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDateRangeDlg dialog

#pragma warning( disable : 4355 )
VTSDateRangeDlg::VTSDateRangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSDateRangeDlg::IDD, pParent)
	, m_StartDateCtrl(this,IDC_DATERANGE_STARTDATE)
	, m_EndDateCtrl(this,IDC_DATERANGE_ENDDATE)
{
	//{{AFX_DATA_INIT(VTSDateRangeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void VTSDateRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDateRangeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDateRangeDlg, CDialog)
	//{{AFX_MSG_MAP(VTSDateRangeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDateRangeDlg message handlers

BOOL VTSDateRangeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	BACnetDate* p1=&m_StartDateCtrl;
	*p1=m_value.bacnetDateStart;
	m_StartDateCtrl.ctrlNull=false;
	m_StartDateCtrl.ObjToCtrl();
		
	BACnetDate* p2=&m_EndDateCtrl;
	*p2=m_value.bacnetDateEnd;
	m_EndDateCtrl.ctrlNull=false;
	m_EndDateCtrl.ObjToCtrl();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void VTSDateRangeDlg::Decode(BACnetAPDUDecoder& dec)
{
	if(dec.pktLength!=0)
		m_value.Decode(dec);
}

void VTSDateRangeDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
	m_value.Encode(enc,context);
}

void VTSDateRangeDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_StartDateCtrl.CtrlToObj();
	m_value.bacnetDateStart=m_StartDateCtrl;
	m_EndDateCtrl.CtrlToObj();
	m_value.bacnetDateEnd=m_EndDateCtrl;	
	CDialog::OnOK();
}
