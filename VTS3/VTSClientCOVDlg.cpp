// VTSClientCOVDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSClientCOVDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSClientCOVDlg dialog


#pragma warning( disable : 4355 )
VTSClientCOVDlg::VTSClientCOVDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSClientCOVDlg::IDD, pParent)
	, m_realCtrl(this, IDC_REALINCR)
{
	//{{AFX_DATA_INIT(VTSClientCOVDlg)
	m_covchoice = 1;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSClientCOVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSClientCOVDlg)
	DDX_Radio(pDX, IDC_REALVALUE, m_covchoice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSClientCOVDlg, CDialog)
	//{{AFX_MSG_MAP(VTSClientCOVDlg)
	ON_BN_CLICKED(IDC_NULLVALUE, OnNullvalue)
	ON_BN_CLICKED(IDC_REALVALUE, OnRealvalue)
	ON_EN_CHANGE(IDC_REALINCR, OnChangeRealincr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSClientCOVDlg message handlers

void VTSClientCOVDlg::OnNullvalue() 
{
	this->GetDlgItem(IDC_REALINCR)->EnableWindow(false);
    m_realCtrl.ctrlNull = true;
}

void VTSClientCOVDlg::OnRealvalue() 
{
	this->GetDlgItem(IDC_REALINCR)->EnableWindow(true);
	if (m_realCtrl.ctrlNull)
	{
		m_realCtrl.ctrlNull = false;
		m_realCtrl.ObjToCtrl();
	}		
}

void VTSClientCOVDlg::OnChangeRealincr() 
{
	m_realCtrl.UpdateData();
}

BOOL VTSClientCOVDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	switch (m_covchoice) {
	case 0:
			if(!m_realCtrl.ctrlNull)
			m_realCtrl.ObjToCtrl();
			OnRealvalue();
			break;
	case 1:
			OnNullvalue();
			break;
	default:
			OnNullvalue();
			break;
	}	
	
	return TRUE;  
}

void VTSClientCOVDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	switch (m_covchoice) 
	{
	case 0:
			m_realCtrl.Encode(enc);
			break;
	case 1:
			BACnetNull().Encode(enc);
			break;
	default:
			BACnetNull().Encode(enc);
			break;
	}
}
void VTSClientCOVDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength!=0)
	{
		BACnetAPDUTag		tagTestType;
		dec.ExamineTag(tagTestType);
		if(tagTestType.tagClass == applicationTagClass)
		{
			if(tagTestType.tagLVT == 4)
			{
				m_realCtrl.Decode(dec);
				m_covchoice = 0;
			}
			else
			{
				BACnetNull().Decode(dec);
				m_covchoice = 1;
			}
		}
		else 
			throw "Decode Error";
	}
}
