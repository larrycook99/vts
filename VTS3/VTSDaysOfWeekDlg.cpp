// VTSDaysOfWeekDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDaysOfWeekDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDaysOfWeekDlg dialog


#pragma warning( disable : 4355 )
VTSDaysOfWeekDlg::VTSDaysOfWeekDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSDaysOfWeekDlg::IDD, pParent)
	,m_MondayCtrl(this, IDC_DOFWEEK_MONDAY, true)
	,m_TuesdayCtrl(this, IDC_DOFWEEK_TUESDAY, true)
	,m_WednesCtrl(this, IDC_DOFWEEK_WEDNESDAY, true)
	,m_ThusdayCtrl(this, IDC_DOFWEEK_THURSDAY, true)
	,m_FridayCtrl(this, IDC_DOFWEEK_FRIDAY, true)
	,m_SaturdayCtrl(this, IDC_DOFWEEK_SATURDAY, true)
	,m_SundayCtrl(this, IDC_DOFWEEK_SUNDAY, true)
{
	//{{AFX_DATA_INIT(VTSDaysOfWeekDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSDaysOfWeekDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDaysOfWeekDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDaysOfWeekDlg, CDialog)
	//{{AFX_MSG_MAP(VTSDaysOfWeekDlg)
	ON_BN_CLICKED(IDC_DOFWEEK_SUNDAY, OnDofweekSunday)
	ON_BN_CLICKED(IDC_DOFWEEK_SATURDAY, OnDofweekSaturday)
	ON_BN_CLICKED(IDC_DOFWEEK_MONDAY, OnDofweekMonday)
	ON_BN_CLICKED(IDC_DOFWEEK_FRIDAY, OnDofweekFriday)
	ON_BN_CLICKED(IDC_DOFWEEK_THURSDAY, OnDofweekThursday)
	ON_BN_CLICKED(IDC_DOFWEEK_TUESDAY, OnDofweekTuesday)
	ON_BN_CLICKED(IDC_DOFWEEK_WEDNESDAY, OnDofweekWednesday)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDaysOfWeekDlg message handlers

void VTSDaysOfWeekDlg::OnDofweekSunday() 
{
	m_SundayCtrl.UpdateData();
}

void VTSDaysOfWeekDlg::OnDofweekSaturday() 
{
	m_SaturdayCtrl.UpdateData();
}

void VTSDaysOfWeekDlg::OnDofweekMonday() 
{
	m_MondayCtrl.UpdateData();
}

void VTSDaysOfWeekDlg::OnDofweekFriday() 
{
	m_FridayCtrl.UpdateData();
}

void VTSDaysOfWeekDlg::OnDofweekThursday() 
{
	m_ThusdayCtrl.UpdateData();
}

void VTSDaysOfWeekDlg::OnDofweekTuesday() 
{
	m_TuesdayCtrl.UpdateData();
}

void VTSDaysOfWeekDlg::OnDofweekWednesday() 
{
	m_WednesCtrl.UpdateData();
}

BOOL VTSDaysOfWeekDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_MondayCtrl.ObjToCtrl();
	m_TuesdayCtrl.ObjToCtrl();
	m_WednesCtrl.ObjToCtrl();
	m_ThusdayCtrl.ObjToCtrl();
	m_FridayCtrl.ObjToCtrl();
	m_SaturdayCtrl.ObjToCtrl();
	m_SundayCtrl.ObjToCtrl();
	return TRUE;  
}

void VTSDaysOfWeekDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	BACnetBitString		bits( 7 );
	bits.SetBit( 0, m_MondayCtrl.boolValue );
	bits.SetBit( 1, m_TuesdayCtrl.boolValue );
	bits.SetBit( 2, m_WednesCtrl.boolValue );
	bits.SetBit( 3, m_ThusdayCtrl.boolValue );
	bits.SetBit( 4, m_FridayCtrl.boolValue );
	bits.SetBit( 5, m_SaturdayCtrl.boolValue );
	bits.SetBit( 6, m_SundayCtrl.boolValue );
	bits.Encode( enc, context);
}
void VTSDaysOfWeekDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength!=0)
	{
		BACnetBitString		bits( 7 );
		bits.Decode(dec);
m_MondayCtrl.boolValue = (bits.GetBit(0)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
m_TuesdayCtrl.boolValue= (bits.GetBit(1)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
m_WednesCtrl.boolValue =  (bits.GetBit(2)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
m_ThusdayCtrl.boolValue = (bits.GetBit(3)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
m_FridayCtrl.boolValue =  (bits.GetBit(4)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
m_SaturdayCtrl.boolValue =  (bits.GetBit(5)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
m_SundayCtrl.boolValue =  (bits.GetBit(6)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
	}
}
