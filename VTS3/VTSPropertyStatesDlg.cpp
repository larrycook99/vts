// VTSPropertyStatesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSPropertyStatesDlg.h"
#include "VTSBinaryPVDlg.h"
#include "VTSEventTypeDlg.h"
#include "VTSPolarityDlg.h"
#include "VTSProgramErrorDlg.h"
#include "VTSProgramRequestDlg.h"
#include "VTSProgramStateDlg.h"
#include "VTSReliabilityDlg.h"
#include "VTSEventStateDlg.h"
#include "VTSDeviceStatusDlg.h"
#include "VTSEngineeringUnitsDlg.h"
#include "VTSLifeSafetyModeDlg.h"
#include "VTSLifeSafetyStateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSPropertyStatesDlg dialog


#pragma warning( disable : 4355 )
VTSPropertyStatesDlg::VTSPropertyStatesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSPropertyStatesDlg::IDD, pParent)
	, m_nSelItem(-1)
	, m_boolValue(this, IDC_BOOLVALUE, true)
	, m_unsignedValue(this, IDC_UNSIGNEDVALUE)
{
	//{{AFX_DATA_INIT(VTSPropertyStatesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSPropertyStatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPropertyStatesDlg)
	DDX_Control(pDX, IDC_CHOICE, m_statesCombo);
	//}}AFX_DATA_MAP
	m_boolValue.UpdateData( pDX->m_bSaveAndValidate );
	m_unsignedValue.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSPropertyStatesDlg, CDialog)
	//{{AFX_MSG_MAP(VTSPropertyStatesDlg)
	ON_BN_CLICKED(IDC_ONVALUE, OnOnvalue)
	ON_EN_CHANGE(IDC_UNSIGNEDVALUE, OnChangeUnsignedvalue)
	ON_CBN_SELCHANGE(IDC_CHOICE, OnSelchangeChoice)
	ON_BN_CLICKED(IDC_BOOLVALUE, OnBoolvalue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPropertyStatesDlg message handlers

void VTSPropertyStatesDlg::OnOnvalue() 
{
	UpdateData(TRUE);
	BACnetAPDUEncoder enctemp;
	BACnetAPDUEncoder enc;	
	if(m_enumValue >= 0)
	{	
		BACnetEnumerated enumtemp;
		enumtemp.m_enumValue = m_enumValue;
		enumtemp.Encode(enctemp);		
	}
	switch(m_nSelItem)
	{
	case 1:
		{
			VTSBinaryPVDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 1);
		}
		break;
	case 2:
		{
			VTSEventTypeDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 2);
		}
		break;
	case 3:
		{
			VTSPolarityDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 3);
		}
		break;
	case 4:
		{
			VTSProgramRequestDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);	
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc,4);
		}
		break;
	case 5:
		{
			VTSProgramStateDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 5);
		}
		break;
	case 6:
		{
			VTSProgramErrorDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 6);
		}
		break;
	case 7:
		{
			VTSReliabilityDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 7);
		}
		break;
	case 8:
		{
			VTSEventStateDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 8);
		}
		break;
	case 9:
		{
			VTSBinaryPVDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 9);
		}
		break;
	case 10:
		{
			VTSDeviceStatusDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 10);
		}
		break;
	case 12:
		{
			VTSLifeSafetyModeDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 12);
		}
		break;
	case 13:
		{
			VTSLifeSafetyStateDlg dlg(this);
			dlg.Decode((BACnetAPDUDecoder)enctemp);
			if(dlg.DoModal() == IDCANCEL)
				return;
			dlg.Encode(enc, 13);
		}
		break;
	default:
		break;
	}
	if(enc.pktLength != 0)
	{
		BACnetEnumerated enumsel;
		enumsel.Decode((BACnetAPDUDecoder)enc);
		m_enumValue = enumsel.m_enumValue;
	}
}

void VTSPropertyStatesDlg::OnChangeUnsignedvalue() 
{
	m_unsignedValue.UpdateData();
}

void VTSPropertyStatesDlg::OnSelchangeChoice() 
{
	m_nSelItem = m_statesCombo.GetCurSel();
	switch(m_nSelItem) {
	case 0:
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( true );
		GetDlgItem(IDC_BOOLVALUE)->ShowWindow(SW_SHOW);
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem(IDC_UNSIGNEDVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_ONVALUE )->EnableWindow( false );
		GetDlgItem(IDC_ONVALUE)->ShowWindow(SW_HIDE);
		break;
	case 11:
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem(IDC_BOOLVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( true );
		GetDlgItem(IDC_UNSIGNEDVALUE)->ShowWindow(SW_SHOW);
		GetDlgItem( IDC_ONVALUE )->EnableWindow( false );
		GetDlgItem(IDC_ONVALUE)->ShowWindow(SW_HIDE);
		break;
	default:
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem(IDC_BOOLVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem(IDC_UNSIGNEDVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_ONVALUE )->EnableWindow( true );
		GetDlgItem(IDC_ONVALUE)->ShowWindow(SW_SHOW);
		break;
	}
}
void VTSPropertyStatesDlg::OnBoolvalue() 
{
  m_boolValue.UpdateData();
}
void VTSPropertyStatesDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	switch(m_nSelItem) {
	case 0:
		m_boolValue.Encode(enc, 0);
		break;
	case 11:
		m_unsignedValue.Encode(enc, 11);
		break;
	default:
		BACnetEnumerated enumsel;
		enumsel.m_enumValue = m_enumValue;
		enumsel.Encode(enc, m_nSelItem);
		break;
	}
}
void VTSPropertyStatesDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength != 0)
	{
		BACnetAPDUTag	tagTestType;
		dec.ExamineTag(tagTestType);
		m_nSelItem = tagTestType.tagNumber;
	switch(m_nSelItem) {
	case 0:
		m_boolValue.Decode(dec);
		m_boolValue.ctrlNull = false;
		break;
	case 11:
		m_unsignedValue.Decode(dec);
		m_unsignedValue.ctrlNull = false;
		break;
	default:
		BACnetEnumerated enumsel;
		enumsel.Decode(dec);
		m_enumValue = enumsel.m_enumValue;
		break;
	}
	}
}

BOOL VTSPropertyStatesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_nSelItem >= 0)
	{
	m_statesCombo.SetCurSel(m_nSelItem);
	switch(m_nSelItem) {
	case 0:
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( true );
		GetDlgItem(IDC_BOOLVALUE)->ShowWindow(SW_SHOW);
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem(IDC_UNSIGNEDVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_ONVALUE )->EnableWindow( false );
		GetDlgItem(IDC_ONVALUE)->ShowWindow(SW_HIDE);
		break;
	case 11:
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem(IDC_BOOLVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( true );
		GetDlgItem(IDC_UNSIGNEDVALUE)->ShowWindow(SW_SHOW);
		GetDlgItem( IDC_ONVALUE )->EnableWindow( false );
		GetDlgItem(IDC_ONVALUE)->ShowWindow(SW_HIDE);
		break;
	default:
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem(IDC_BOOLVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem(IDC_UNSIGNEDVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_ONVALUE )->EnableWindow( true );
		GetDlgItem(IDC_ONVALUE)->ShowWindow(SW_SHOW);
		break;
	}
	}
	else
	{
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem(IDC_BOOLVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem(IDC_UNSIGNEDVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_ONVALUE )->EnableWindow( false );
		GetDlgItem(IDC_ONVALUE)->ShowWindow(SW_HIDE);
	}
	return TRUE; 
}
