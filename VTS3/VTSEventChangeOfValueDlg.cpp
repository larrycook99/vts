// VTSEventChangeOfValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventChangeOfValueDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSEventChangeOfValueDlg dialog


#pragma warning( disable : 4355 )
VTSEventChangeOfValueDlg::VTSEventChangeOfValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventChangeOfValueDlg::IDD, pParent)
	,m_timeDelay(this, IDC_TIMEDELAY)
	,m_bitmask(this, IDC_BITMASK)
	,m_realvalue(this, IDC_REALVALUE)
{
	//{{AFX_DATA_INIT(VTSEventChangeOfValueDlg)
	m_nChoice = -1;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEventChangeOfValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventChangeOfValueDlg)
	DDX_Radio(pDX, IDC_RADIOBITSTR, m_nChoice);
	//}}AFX_DATA_MAP
	m_timeDelay.UpdateData( pDX->m_bSaveAndValidate );
	m_bitmask.UpdateData( pDX->m_bSaveAndValidate );
	m_realvalue.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSEventChangeOfValueDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventChangeOfValueDlg)
	ON_BN_CLICKED(IDC_RADIOBITSTR, OnRadiobitstr)
	ON_BN_CLICKED(IDC_RADIOREAL, OnRadioreal)
	ON_EN_CHANGE(IDC_BITMASK, OnChangeBitmask)
	ON_EN_CHANGE(IDC_REALVALUE, OnChangeRealvalue)
	ON_EN_CHANGE(IDC_TIMEDELAY, OnChangeTimedelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventChangeOfValueDlg message handlers

void VTSEventChangeOfValueDlg::OnRadiobitstr() 
{
	UpdateData();
	this->GetDlgItem(IDC_BITMASK)->EnableWindow(true);
	this->GetDlgItem(IDC_REALVALUE)->EnableWindow(false);
}

void VTSEventChangeOfValueDlg::OnRadioreal() 
{
	UpdateData();
	this->GetDlgItem(IDC_BITMASK)->EnableWindow(false);
	this->GetDlgItem(IDC_REALVALUE)->EnableWindow(true);	
}

void VTSEventChangeOfValueDlg::OnChangeBitmask() 
{
	m_bitmask.UpdateData();
}

void VTSEventChangeOfValueDlg::OnChangeRealvalue() 
{
	m_realvalue.UpdateData();
}

void VTSEventChangeOfValueDlg::OnChangeTimedelay() 
{
	m_timeDelay.UpdateData();	
}
BOOL VTSEventChangeOfValueDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	switch (m_nChoice) {
	case 0:
			OnRadiobitstr();
			break;
	case 1:
			OnRadioreal();
			break;
	default:
			break;
	}			
	return TRUE;  
}

void VTSEventChangeOfValueDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{	
	m_timeDelay.Encode(enc, 0);
		BACnetOpeningTag().Encode(enc, 1);
	switch(m_nChoice)
	{
	case 0:
		m_bitmask.Encode(enc, 0);
		break;
	case 1:
		m_realvalue.Encode(enc, 1);
		break;
	default:
		break;
	}
	BACnetClosingTag().Encode(enc, 1);	
}
void VTSEventChangeOfValueDlg::Decode(BACnetAPDUDecoder& dec )
{
  if(dec.pktLength != 0)
  {	  
	  m_timeDelay.Decode(dec);
	  BACnetOpeningTag().Decode(dec);
	  m_timeDelay.ctrlNull = false;
	  BACnetAPDUTag		tagTestType;
	  dec.ExamineTag(tagTestType);
	  m_nChoice=tagTestType.tagNumber;	  
	  switch(m_nChoice)
	  {
	  case 0:
		  m_bitmask.Decode(dec);
		  m_bitmask.ctrlNull = false;
		  break;
	  case 1:
		  m_realvalue.Decode(dec);
		  m_realvalue.ctrlNull = false;
		  break;
	  default:
		  break;
	  }		
	  BACnetClosingTag().Decode(dec);
  }
}
