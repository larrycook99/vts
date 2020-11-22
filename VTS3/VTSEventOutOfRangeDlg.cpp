// VTSEventOutOfRangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventOutOfRangeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSEventOutOfRangeDlg dialog


#pragma warning( disable : 4355 )
VTSEventOutOfRangeDlg::VTSEventOutOfRangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventOutOfRangeDlg::IDD, pParent)
	, m_timeDelay(this, IDC_TIMEDELAY)
	, m_LowLimit(this, IDC_LOWLIMIT)
	, m_HighLimit(this, IDC_HIGHLIMIT)
	, m_DeadBand(this, IDC_DEADBAND)
{
	//{{AFX_DATA_INIT(VTSEventOutOfRangeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEventOutOfRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventOutOfRangeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_timeDelay.UpdateData( pDX->m_bSaveAndValidate );
	m_LowLimit.UpdateData( pDX->m_bSaveAndValidate );
	m_HighLimit.UpdateData( pDX->m_bSaveAndValidate );
	m_DeadBand.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSEventOutOfRangeDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventOutOfRangeDlg)
	ON_EN_CHANGE(IDC_TIMEDELAY, OnChangeTimedelay)
	ON_EN_CHANGE(IDC_LOWLIMIT, OnChangeLowlimit)
	ON_EN_CHANGE(IDC_HIGHLIMIT, OnChangeHighlimit)
	ON_EN_CHANGE(IDC_DEADBAND, OnChangeDeadband)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventOutOfRangeDlg message handlers
void VTSEventOutOfRangeDlg::OnChangeTimedelay() 
{
	m_timeDelay.UpdateData();
}
void VTSEventOutOfRangeDlg::OnChangeHighlimit() 
{
	m_HighLimit.UpdateData();
}

void VTSEventOutOfRangeDlg::OnChangeLowlimit() 
{
	m_LowLimit.UpdateData();	
}
void VTSEventOutOfRangeDlg::OnChangeDeadband() 
{
	m_DeadBand.UpdateData();	
}
void VTSEventOutOfRangeDlg::Encode(BACnetAPDUEncoder& enc, int context)
{	
	m_timeDelay.Encode(enc, 0);	
	m_LowLimit.Encode(enc, 1);
	m_HighLimit.Encode(enc, 2);
	m_DeadBand.Encode(enc, 3);
	
}
void VTSEventOutOfRangeDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{	
		m_timeDelay.Decode(dec);
		m_timeDelay.ctrlNull = false;
		m_LowLimit.Decode(dec);
		m_LowLimit.ctrlNull = false;
		m_HighLimit.Decode(dec);
		m_HighLimit.ctrlNull = false;
		m_DeadBand.Decode(dec);	
		m_DeadBand.ctrlNull = false;		
	}
}



