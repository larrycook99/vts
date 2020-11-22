// VTSTimeStampDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSTimeStampDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSTimeStampDlg dialog

#pragma warning( disable : 4355 )
VTSTimeStampDlg::VTSTimeStampDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSTimeStampDlg::IDD, pParent)
    , m_TimeCtrl(this,IDC_TIMESTAMP_TIME)
	, m_UnsignedCtrl(this,IDC_TIMESTAMP_UNSIGNED)
	, m_DateTimeDateCtrl(this,IDC_TIMESTAMP_DATETIME_DATE)
	, m_DateTimeTimeCtrl(this,IDC_TIMESTAMP_DATETIME_TIME)
	, m_nChoice(-1){
	//{{AFX_DATA_INIT(VTSTimeStampDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void VTSTimeStampDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSTimeStampDlg)
	DDX_Control(pDX, IDC_TIMESTAMP_TIMERADIO, m_TimeRadio);
	DDX_Control(pDX, IDC_TIMESTAMP_UNSIGNEDRADIO, m_UnsignedRadio);
	DDX_Control(pDX, IDC_TIMESTAMP_DATETIMERADIO, m_DateTimeRadio);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSTimeStampDlg, CDialog)
	//{{AFX_MSG_MAP(VTSTimeStampDlg)
	ON_BN_CLICKED(IDC_TIMESTAMP_TIMERADIO, OnTimestampTimeradio)
	ON_BN_CLICKED(IDC_TIMESTAMP_UNSIGNEDRADIO, OnTimestampUnsignedradio)
	ON_BN_CLICKED(IDC_TIMESTAMP_DATETIMERADIO, OnTimestampDatetimeradio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSTimeStampDlg message handlers

void VTSTimeStampDlg::OnTimestampTimeradio() 
{
	// TODO: Add your control notification handler code here
	this->GetDlgItem(IDC_TIMESTAMP_TIME)->EnableWindow(true);
	this->GetDlgItem(IDC_TIMESTAMP_UNSIGNED)->EnableWindow(false);
	this->GetDlgItem(IDC_TIMESTAMP_DATETIME_DATE)->EnableWindow(false);
	this->GetDlgItem(IDC_TIMESTAMP_DATETIME_TIME)->EnableWindow(false);	
	if (m_TimeCtrl.ctrlNull)
	{
		m_TimeCtrl.ctrlNull = false;
		m_TimeCtrl.ObjToCtrl();
	}
	m_nChoice=0;
}

void VTSTimeStampDlg::OnTimestampUnsignedradio() 
{
	// TODO: Add your control notification handler code here
	this->GetDlgItem(IDC_TIMESTAMP_TIME)->EnableWindow(false);
	this->GetDlgItem(IDC_TIMESTAMP_UNSIGNED)->EnableWindow(true);
	this->GetDlgItem(IDC_TIMESTAMP_DATETIME_DATE)->EnableWindow(false);
	this->GetDlgItem(IDC_TIMESTAMP_DATETIME_TIME)->EnableWindow(false);		
	if (m_UnsignedCtrl.ctrlNull)
	{
		m_UnsignedCtrl.ctrlNull = false;
		m_UnsignedCtrl.ObjToCtrl();
	}
	m_nChoice=1;
}

void VTSTimeStampDlg::OnTimestampDatetimeradio() 
{
	// TODO: Add your control notification handler code here
	this->GetDlgItem(IDC_TIMESTAMP_TIME)->EnableWindow(false);
	this->GetDlgItem(IDC_TIMESTAMP_UNSIGNED)->EnableWindow(false);
	this->GetDlgItem(IDC_TIMESTAMP_DATETIME_DATE)->EnableWindow(true);
	this->GetDlgItem(IDC_TIMESTAMP_DATETIME_TIME)->EnableWindow(true);		
	if (m_DateTimeDateCtrl.ctrlNull)
	{
		m_DateTimeDateCtrl.ctrlNull = false;
		m_DateTimeDateCtrl.ObjToCtrl();
	}
	if (m_DateTimeTimeCtrl.ctrlNull)
	{
		m_DateTimeTimeCtrl.ctrlNull = false;
		m_DateTimeTimeCtrl.ObjToCtrl();
	}
	m_nChoice=2;
}

void VTSTimeStampDlg::OnOK() 
{
	// TODO: Add extra validation here
	switch (m_nChoice) {
	case 0:
		{
			m_TimeCtrl.CtrlToObj();
			BACnetTime* pTime = new BACnetTime();
			*pTime = m_TimeCtrl;
			m_value.SetObject(pTime);
			break;
		}
	case 1:
		{
			m_UnsignedCtrl.CtrlToObj();
			BACnetUnsigned* pUnsigned = new BACnetUnsigned();
			pUnsigned->uintValue = m_UnsignedCtrl.uintValue;
			m_value.SetObject(pUnsigned);
			break;
		}
	case 2:
		{
			m_DateTimeDateCtrl.CtrlToObj();
			m_DateTimeTimeCtrl.CtrlToObj();
			BACnetDateTime* pDateTime = new BACnetDateTime();
			pDateTime->bacnetDate = m_DateTimeDateCtrl;
			pDateTime->bacnetTime = m_DateTimeTimeCtrl;
			m_value.SetObject(pDateTime);
			break;
		}
	default:
		break;
	}	
	CDialog::OnOK();
}

BOOL VTSTimeStampDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	switch (m_nChoice) {
	case 0:
		{
			BACnetTime* pTime = (BACnetTime*)m_value.GetObject();
			BACnetTime* pCtrl = &m_TimeCtrl;
			*pCtrl = *pTime;
			m_TimeCtrl.ctrlNull=false;
			m_TimeCtrl.ObjToCtrl();
			m_TimeRadio.SetCheck(1);
			OnTimestampTimeradio();
			break;
		}
	case 1:
		{
			BACnetUnsigned* pUnsigned = (BACnetUnsigned* )m_value.GetObject();
			m_UnsignedCtrl.uintValue = pUnsigned->uintValue;
			m_UnsignedCtrl.ctrlNull=false;
			m_UnsignedCtrl.ObjToCtrl();

			m_UnsignedRadio.SetCheck(1);
			OnTimestampUnsignedradio();
			break;
		}
	case 2:
		{

			BACnetDateTime* pDateTime =(BACnetDateTime*) m_value.GetObject();
			BACnetDate* p1 = &m_DateTimeDateCtrl;
			*p1 = pDateTime->bacnetDate;
			m_DateTimeDateCtrl.ctrlNull=false;
			m_DateTimeDateCtrl.ObjToCtrl();
		
			BACnetTime* p2=&m_DateTimeTimeCtrl;
			*p2 = pDateTime->bacnetTime;
			m_DateTimeTimeCtrl.ctrlNull=false;
			m_DateTimeTimeCtrl.ObjToCtrl();
			
			m_DateTimeRadio.SetCheck(1);
			OnTimestampDatetimeradio();
			break;
		}
	default:
		{
			m_TimeRadio.SetCheck(1);
			OnTimestampTimeradio();
			break;
		}
	}		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void VTSTimeStampDlg::Decode(BACnetAPDUDecoder& dec)
{
	if(dec.pktLength!=0)
	{
		BACnetAPDUTag		tagTestType;
		dec.ExamineTag(tagTestType);
		if(tagTestType.tagClass == openingTagClass)
			m_nChoice = 2;	//datetime
		else
			m_nChoice=tagTestType.tagNumber;
		m_value.Decode(dec);
	}
}

void VTSTimeStampDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
	m_value.Encode(enc,context);
}