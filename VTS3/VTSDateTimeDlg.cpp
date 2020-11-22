// VTSDateTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDateTimeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDateTimeDlg dialog

#pragma warning( disable : 4355 )
VTSDateTimeDlg::VTSDateTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSDateTimeDlg::IDD, pParent)
	, m_DateCtrl(this,IDC_DATETIME_DATE)
	, m_TimeCtrl(this,IDC_DATETIME_TIME)
{
	//{{AFX_DATA_INIT(VTSDateTimeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void VTSDateTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDateTimeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDateTimeDlg, CDialog)
	//{{AFX_MSG_MAP(VTSDateTimeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDateTimeDlg message handlers

void VTSDateTimeDlg::Encode(BACnetAPDUEncoder &enc, int context)
{
	m_value.Encode(enc,context);
}

void VTSDateTimeDlg::Decode(BACnetAPDUDecoder &dec)
{
	if(dec.pktLength!=0)
			m_value.Decode(dec);
}

BOOL VTSDateTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	BACnetDate* p1=&m_DateCtrl;
	*p1=m_value.bacnetDate;
	m_DateCtrl.ctrlNull=false;
	m_DateCtrl.ObjToCtrl();
		
	BACnetTime* p2=&m_TimeCtrl;
	*p2=m_value.bacnetTime;
	m_TimeCtrl.ctrlNull=false;
	m_TimeCtrl.ObjToCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void VTSDateTimeDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_DateCtrl.CtrlToObj();
	m_value.bacnetDate=m_DateCtrl;
	m_TimeCtrl.CtrlToObj();
	m_value.bacnetTime=m_TimeCtrl;
	CDialog::OnOK();
}
