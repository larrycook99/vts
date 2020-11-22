// VTSPriorityValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSPriorityValueDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSPriorityValueDlg dialog


#pragma warning( disable : 4355 )
VTSPriorityValueDlg::VTSPriorityValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSPriorityValueDlg::IDD, pParent)
	, m_BinaryPV( this, IDC_BINARYPV, NetworkSniffer::BAC_STRTAB_BACnetBinaryPV, true )
    , m_RealCtrl(this,IDC_REALVALUE)
	, m_IntCtrl(this,IDC_INTEGER)
{
	//{{AFX_DATA_INIT(VTSPriorityValueDlg)
	m_nChoice = -1;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSPriorityValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPriorityValueDlg)
	DDX_Radio(pDX, IDC_RADIONULL, m_nChoice);
	//}}AFX_DATA_MAP
	m_BinaryPV.UpdateData( pDX->m_bSaveAndValidate );
	m_RealCtrl.UpdateData( pDX->m_bSaveAndValidate );
	m_IntCtrl.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSPriorityValueDlg, CDialog)
	//{{AFX_MSG_MAP(VTSPriorityValueDlg)
	ON_BN_CLICKED(IDC_RADIOVALUE, OnRadiovalue)
	ON_BN_CLICKED(IDC_RADIOREAL, OnRadioreal)
	ON_BN_CLICKED(IDC_RADIONULL, OnRadionull)
	ON_BN_CLICKED(IDC_RADIOINTEGER, OnRadiointeger)
	ON_BN_CLICKED(IDC_RADIOBINARY, OnRadiobinary)
	ON_EN_CHANGE(IDC_INTEGER, OnChangeInteger)
	ON_CBN_SELCHANGE(IDC_BINARYPV, OnSelchangeBinarypv)
	ON_BN_CLICKED(IDC_ANYVALUE, OnAnyvalue)
	ON_EN_CHANGE(IDC_REALVALUE, OnChangeRealvalue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPriorityValueDlg message handlers

void VTSPriorityValueDlg::OnRadiovalue() 
{
	this->GetDlgItem(IDC_BINARYPV)->EnableWindow(false);
	this->GetDlgItem(IDC_REALVALUE)->EnableWindow(false);
	this->GetDlgItem(IDC_INTEGER)->EnableWindow(false);
	this->GetDlgItem(IDC_ANYVALUE)->EnableWindow(true);		
}

void VTSPriorityValueDlg::OnRadioreal() 
{
	this->GetDlgItem(IDC_BINARYPV)->EnableWindow(false);
	this->GetDlgItem(IDC_REALVALUE)->EnableWindow(true);
	this->GetDlgItem(IDC_INTEGER)->EnableWindow(false);
	this->GetDlgItem(IDC_ANYVALUE)->EnableWindow(false);			
}

void VTSPriorityValueDlg::OnRadionull() 
{
	this->GetDlgItem(IDC_BINARYPV)->EnableWindow(false);
	this->GetDlgItem(IDC_REALVALUE)->EnableWindow(false);
	this->GetDlgItem(IDC_INTEGER)->EnableWindow(false);
	this->GetDlgItem(IDC_ANYVALUE)->EnableWindow(false);	
}

void VTSPriorityValueDlg::OnRadiointeger() 
{
	this->GetDlgItem(IDC_BINARYPV)->EnableWindow(false);
	this->GetDlgItem(IDC_REALVALUE)->EnableWindow(false);
	this->GetDlgItem(IDC_INTEGER)->EnableWindow(true);
	this->GetDlgItem(IDC_ANYVALUE)->EnableWindow(false);
}

void VTSPriorityValueDlg::OnRadiobinary() 
{
	this->GetDlgItem(IDC_BINARYPV)->EnableWindow(true);
	this->GetDlgItem(IDC_REALVALUE)->EnableWindow(false);
	this->GetDlgItem(IDC_INTEGER)->EnableWindow(false);
	this->GetDlgItem(IDC_ANYVALUE)->EnableWindow(false);
	m_BinaryPV.LoadCombo();
}

void VTSPriorityValueDlg::OnChangeInteger() 
{
	m_IntCtrl.UpdateData();
}

void VTSPriorityValueDlg::OnSelchangeBinarypv() 
{
	m_BinaryPV.ctrlNull = false;
	m_BinaryPV.CtrlToObj();
}

void VTSPriorityValueDlg::OnAnyvalue() 
{
	m_Value.DoModal();
}
void VTSPriorityValueDlg::Decode(BACnetAPDUDecoder& dec)
{
	if(dec.pktLength!=0)
	{
		BACnetAPDUTag		tagTestType;
		dec.ExamineTag(tagTestType);
		if(tagTestType.tagClass == openingTagClass)
		{
			m_nChoice = 4;	//constructed Value
			BACnetOpeningTag().Decode(dec);
			BACnetAPDUTag tag;
			int tagmiss = 1; 
			while(tagmiss != 0)
			{
				BACnetOctet	tag;			
				if (dec.pktLength < 1)
					break;				
				tag = (dec.pktLength--,*dec.pktBuffer++);
				
				if ((tag & 0x0F) == 0x0E)
					tagmiss++;
				if ((tag & 0x0F) == 0x0F)
					tagmiss--;
			}
		}
		else
		{
			if(tagTestType.tagClass == applicationTagClass)
			{
				switch(tagTestType.tagNumber)
				{
				case nullAppTag:
					BACnetNull().Decode(dec);
					m_nChoice = 0;
					break;
				case realAppTag:
					m_RealCtrl.Decode(dec);
					m_RealCtrl.ctrlNull = false;
					m_nChoice = 1;
					break;
				case enumeratedAppTag:
					m_BinaryPV.Decode(dec);
					m_BinaryPV.ctrlNull = false;
					m_nChoice = 2;
					break;
				case unsignedIntAppTag:
					m_IntCtrl.Decode(dec);
					m_IntCtrl.ctrlNull = false;
					m_nChoice = 3;
					break;
				default:
					break;
				}
			}
			else 
				throw "Decode Error";
		}
	
	}
}

void VTSPriorityValueDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
		switch(m_nChoice)
		{
		case 0:
			BACnetNull().Encode(enc);
			break;
		case 1:
			m_RealCtrl.Encode(enc);
			break;
		case 2:
			m_BinaryPV.Encode(enc);
			break;
		case 3:
			m_IntCtrl.Encode(enc);
			break;
		case 4:
			m_Value.Encode(enc,0);
		default:
			break;
		}
}

BOOL VTSPriorityValueDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_BinaryPV.LoadCombo();	
	switch (m_nChoice) {
	case 0:
			OnRadionull();
			break;
	case 1:
			OnRadioreal();
			break;
	case 2:
			OnRadiobinary();
			break;
	case 3:
			OnRadiointeger();
			break;
	case 4:
			OnRadiovalue();
			break;
	default:
	this->GetDlgItem(IDC_BINARYPV)->EnableWindow(false);
	this->GetDlgItem(IDC_REALVALUE)->EnableWindow(false);
	this->GetDlgItem(IDC_INTEGER)->EnableWindow(false);
	this->GetDlgItem(IDC_ANYVALUE)->EnableWindow(false);
			break;
	}		
	return TRUE;  
}

void VTSPriorityValueDlg::OnChangeRealvalue() 
{
	m_RealCtrl.UpdateData();
}
