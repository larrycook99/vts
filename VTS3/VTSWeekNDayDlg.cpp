// VTSWeekNDayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSWeekNDayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
char *MONTH[] =
{
	"Januay", "February", "March", "April", "May", "June", "July",
		"August", "September", "October", "November", "December",
		"any month"
};
char *WEEK_OF_MONTH[] = 
{
	"days 1-7", "days 8-14", "days 15-21", "days 22-28", "days 29-31",
		"last 7 days", "any week"
};
char *DAY_OF_WEEK[] = 
{
	"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday",
		"any day of week"
};
	
/////////////////////////////////////////////////////////////////////////////
// VTSWeekNDayDlg dialog

#pragma warning( disable : 4355 )
VTSWeekNDayDlg::VTSWeekNDayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSWeekNDayDlg::IDD, pParent)
	, m_month( this, IDC_MONTH, MONTH, 13, true )
	, m_weekofmonth( this, IDC_WEEKOFMONTH, WEEK_OF_MONTH, 7, true )
	, m_dayofweek( this, IDC_DAYOFWEEK, DAY_OF_WEEK, 8, true )
{
	//{{AFX_DATA_INIT(VTSWeekNDayDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSWeekNDayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSWeekNDayDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSWeekNDayDlg, CDialog)
	//{{AFX_MSG_MAP(VTSWeekNDayDlg)
	ON_CBN_SELCHANGE(IDC_MONTH, OnSelchangeMonth)
	ON_CBN_SELCHANGE(IDC_WEEKOFMONTH, OnSelchangeWeekofmonth)
	ON_CBN_SELCHANGE(IDC_DAYOFWEEK, OnSelchangeDayofweek)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSWeekNDayDlg message handlers

void VTSWeekNDayDlg::OnSelchangeMonth() 
{
	m_month.ctrlNull = false;
	m_month.CtrlToObj();
}

void VTSWeekNDayDlg::OnSelchangeWeekofmonth() 
{
	m_weekofmonth.ctrlNull = false;
	m_weekofmonth.CtrlToObj();
}

void VTSWeekNDayDlg::OnSelchangeDayofweek() 
{
	m_dayofweek.ctrlNull = false;
	m_dayofweek.CtrlToObj();
}

void VTSWeekNDayDlg::Encode(BACnetAPDUEncoder& enc, int context)
{	
	int month = (m_month.m_enumValue< 12) ? (m_month.m_enumValue+1) : 255;
	int weekofmonth = (m_weekofmonth.m_enumValue< 6) ? (m_weekofmonth.m_enumValue+1) : 255;
	int dayofweek = (m_dayofweek.m_enumValue< 7) ? (m_dayofweek.m_enumValue+1) : 255;
	BACnetOctetString ocstr(3);
	BACnetOctet temp[3];
	temp[0] = (BACnetOctet)month;
	temp[1] = (BACnetOctet)weekofmonth;
	temp[2] = (BACnetOctet)dayofweek;	
	for(int i=0; i<3; i++)
		ocstr.Append(temp[i]);
	ocstr.Encode( enc, context);
}
void VTSWeekNDayDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{
		BACnetOctetString ocstr(3);
		ocstr.Decode(dec);
		int Month = (int) ocstr.strBuff[0];
	    int WeekOfMonth = (int) ocstr.strBuff[1];
	    int DayOfWeek = (int) ocstr.strBuff[2];
		m_month.m_enumValue = (Month < 13)?(Month- 1):12;
		m_weekofmonth.m_enumValue = (WeekOfMonth < 7)? (WeekOfMonth- 1):6;
		m_dayofweek.m_enumValue = (DayOfWeek < 8)? (DayOfWeek- 1):7;
	}
}
BOOL VTSWeekNDayDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_month.LoadCombo();
	m_month.ObjToCtrl();
	m_weekofmonth.LoadCombo();
	m_weekofmonth.ObjToCtrl();
	m_dayofweek.LoadCombo();
	m_dayofweek.ObjToCtrl();	
	return TRUE;  
}
