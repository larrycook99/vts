// VTSCalendarEntryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSCalendarEntryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSCalendarEntryDlg dialog

#pragma warning( disable : 4355 )
VTSCalendarEntryDlg::VTSCalendarEntryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSCalendarEntryDlg::IDD, pParent)
    , m_DateCtrl(this,IDC_CALENDARENTRY_DATE)
	, m_StartDateCtrl(this,IDC_CALENDARENTRY_STARTDATE)
	, m_EndDateCtrl(this,IDC_CALENDARENTRY_ENDDATE)
	, m_WeekNDayCtrl(this,IDC_CALENDARENTRY_WEEKNDAY)
	, m_nChoice(-1)
{
	//{{AFX_DATA_INIT(VTSCalendarEntryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void VTSCalendarEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSCalendarEntryDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_CALENDARENTRY_DATERADIO, m_DateRadio);
	DDX_Control(pDX, IDC_CALENDARENTRY_DATERANGERADIO, m_DateRangeRadio);
	DDX_Control(pDX, IDC_CALENDARENTRY_WEEKNDAYRADIO, m_WeekNDayRadio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSCalendarEntryDlg, CDialog)
	//{{AFX_MSG_MAP(VTSCalendarEntryDlg)
	ON_BN_CLICKED(IDC_CALENDARENTRY_DATERADIO, OnCalendarentryDateradio)
	ON_BN_CLICKED(IDC_CALENDARENTRY_DATERANGERADIO, OnCalendarentryDaterangeradio)
	ON_BN_CLICKED(IDC_CALENDARENTRY_WEEKNDAYRADIO, OnCalendarentryWeekndayradio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSCalendarEntryDlg message handlers

void VTSCalendarEntryDlg::OnCalendarentryDateradio() 
{
	// TODO: Add your control notification handler code here
	this->GetDlgItem(IDC_CALENDARENTRY_DATE)->EnableWindow(true);
	this->GetDlgItem(IDC_CALENDARENTRY_STARTDATE)->EnableWindow(false);
	this->GetDlgItem(IDC_CALENDARENTRY_ENDDATE)->EnableWindow(false);
	this->GetDlgItem(IDC_CALENDARENTRY_WEEKNDAY)->EnableWindow(false);
	if (m_DateCtrl.ctrlNull)
	{
		m_DateCtrl.ctrlNull = false;
		m_DateCtrl.ObjToCtrl();
	}
	m_nChoice=0;	
}

void VTSCalendarEntryDlg::OnCalendarentryDaterangeradio() 
{
	// TODO: Add your control notification handler code here
	this->GetDlgItem(IDC_CALENDARENTRY_DATE)->EnableWindow(false);
	this->GetDlgItem(IDC_CALENDARENTRY_STARTDATE)->EnableWindow(true);
	this->GetDlgItem(IDC_CALENDARENTRY_ENDDATE)->EnableWindow(true);
	this->GetDlgItem(IDC_CALENDARENTRY_WEEKNDAY)->EnableWindow(false);
	if (m_StartDateCtrl.ctrlNull)
	{
		m_StartDateCtrl.ctrlNull = false;
		m_StartDateCtrl.ObjToCtrl();
	}	
	if (m_EndDateCtrl.ctrlNull)
	{
		m_EndDateCtrl.ctrlNull = false;
		m_EndDateCtrl.ObjToCtrl();
	}	
	m_nChoice=1;	
}

void VTSCalendarEntryDlg::OnCalendarentryWeekndayradio() 
{
	// TODO: Add your control notification handler code here
	this->GetDlgItem(IDC_CALENDARENTRY_DATE)->EnableWindow(false);
	this->GetDlgItem(IDC_CALENDARENTRY_STARTDATE)->EnableWindow(false);
	this->GetDlgItem(IDC_CALENDARENTRY_ENDDATE)->EnableWindow(false);
	this->GetDlgItem(IDC_CALENDARENTRY_WEEKNDAY)->EnableWindow(true);
	if (m_WeekNDayCtrl.ctrlNull)
	{
		m_WeekNDayCtrl.ctrlNull = false;
		m_WeekNDayCtrl.ObjToCtrl();
	}
	m_nChoice=2;	
}

void VTSCalendarEntryDlg::Decode(BACnetAPDUDecoder& dec)
{
	if(dec.pktLength!=0)
	{
		BACnetAPDUTag		tagTestType;
		dec.ExamineTag(tagTestType);
		if(tagTestType.tagClass == openingTagClass)
			m_nChoice = 1;	//date range
		else
			m_nChoice=tagTestType.tagNumber;
		m_value.Decode(dec);
	}
}

void VTSCalendarEntryDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
	m_value.Encode(enc,context);
}

void VTSCalendarEntryDlg::OnOK() 
{
//Modified by Zhu Zhenhua, 2004-6-14
	// TODO: Add extra validation here
	switch (m_nChoice) {
	case 0:
		{
			m_DateCtrl.CtrlToObj();
			BACnetDate* pDate = new BACnetDate();
			*pDate = m_DateCtrl;
			m_value.SetObject(pDate);
			m_value.m_nChoice = m_nChoice;
			break;
		}
	case 1:
		{
			m_StartDateCtrl.CtrlToObj();
			m_EndDateCtrl.CtrlToObj();
			BACnetDateRange* pDateRange = new BACnetDateRange();
			pDateRange->bacnetDateStart = m_StartDateCtrl;
			pDateRange->bacnetDateEnd = m_EndDateCtrl;
			m_value.SetObject(pDateRange);
			m_value.m_nChoice = m_nChoice;
			break;
		}
	case 2:
		{
			m_WeekNDayCtrl.CtrlToObj();
			BACnetOctetString *pWeekNDay=new BACnetOctetString(m_WeekNDayCtrl);
	//		*pWeekNDay = (BACnetOctetString)m_WeekNDayCtrl;
			m_value.SetObject(pWeekNDay);
			m_value.m_nChoice = m_nChoice;
			break;
		}
	default:
		break;
	}	
	CDialog::OnOK();
}

BOOL VTSCalendarEntryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	switch (m_nChoice) {
	case 0:
		{
			BACnetDate* pDate = (BACnetDate*)m_value.GetObject();
			BACnetDate* pCtrl = &m_DateCtrl;
			*pCtrl = *pDate;
			m_DateCtrl.ctrlNull=false;
			m_DateCtrl.ObjToCtrl();
			m_DateRadio.SetCheck(1);
			OnCalendarentryDateradio();
			break;
		}
	case 1:
		{
			BACnetDate* pStartCtrl = &m_StartDateCtrl;
			BACnetDate* pEndCtrl = &m_EndDateCtrl;
			BACnetDateRange* pDateRange = (BACnetDateRange*)m_value.GetObject();
			*pStartCtrl = pDateRange->bacnetDateStart;
			*pEndCtrl = pDateRange->bacnetDateEnd;
			
			m_StartDateCtrl.ctrlNull=false;
			m_StartDateCtrl.ObjToCtrl();
			m_EndDateCtrl.ctrlNull=false;
			m_EndDateCtrl.ObjToCtrl();

			m_DateRangeRadio.SetCheck(1);
			OnCalendarentryDaterangeradio();
			break;
		}
	case 2:
		{
			BACnetOctetString* pOctetCtrl=&m_WeekNDayCtrl;
			BACnetOctetString* pWeekNDay = (BACnetOctetString*)m_value.GetObject();
			pOctetCtrl->Insert(*pWeekNDay,0);
			m_WeekNDayCtrl.ctrlNull=false;
			m_WeekNDayCtrl.ObjToCtrl();
			m_WeekNDayRadio.SetCheck(1);
			OnCalendarentryWeekndayradio();
			break;
		}
	default:
		{
			m_DateRadio.SetCheck(1);
			OnCalendarentryDateradio();
			break;
		}
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
