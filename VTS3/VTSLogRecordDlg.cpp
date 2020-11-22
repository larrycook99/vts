// VTSLogRecordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSLogRecordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSLogRecordDlg dialog


#pragma warning( disable : 4355 )
VTSLogRecordDlg::VTSLogRecordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSLogRecordDlg::IDD, pParent)
	, m_DateCtrl(this,IDC_DATETIME_DATE)
	, m_TimeCtrl(this,IDC_DATETIME_TIME)
	, m_LogDisabled(this, IDC_LOGDISABLED, true)
	, m_BufferPurged(this, IDC_BUFFERPURGED, true)
	, m_boolValue(this, IDC_BOOLVALUE)
	, m_realValue(this, IDC_REALVALUE)
	, m_enumValue(this, IDC_ENUMVALUE)
	, m_uintValue(this, IDC_UNSIGNEDVALUE)
	, m_intValue(this, IDC_INTERGERVALUE)
	, m_bitstringValue(this, IDC_BITSTRINGVALUE)
	, m_errorClass( this, IDC_ERRORCLASS, NetworkSniffer::BAC_STRTAB_BACnetErrorClass, true )
	, m_errorCode( this, IDC_ERRORCODE, NetworkSniffer::BAC_STRTAB_BACnetErrorCode, true )
	, m_TimeChange(this, IDC_TIMECHANGE)
	, m_StatusFlags( this )
{
	//{{AFX_DATA_INIT(VTSLogRecordDlg)
	m_hasstatusflags = FALSE;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSLogRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSLogRecordDlg)
	DDX_Control(pDX, IDC_LOGDATUM, m_DatumChoice);
	DDX_Check(pDX, IDC_HASSTATUSFLAGS, m_hasstatusflags);
	//}}AFX_DATA_MAP
	m_DateCtrl.UpdateData( pDX->m_bSaveAndValidate );
	m_TimeCtrl.UpdateData( pDX->m_bSaveAndValidate );
	m_LogDisabled.UpdateData( pDX->m_bSaveAndValidate );
	m_BufferPurged.UpdateData( pDX->m_bSaveAndValidate );
	m_boolValue.UpdateData( pDX->m_bSaveAndValidate );
	m_realValue.UpdateData( pDX->m_bSaveAndValidate );
	m_enumValue.UpdateData( pDX->m_bSaveAndValidate );
	m_uintValue.UpdateData( pDX->m_bSaveAndValidate );
	m_intValue.UpdateData( pDX->m_bSaveAndValidate );
	m_bitstringValue.UpdateData( pDX->m_bSaveAndValidate );
	m_errorClass.UpdateData( pDX->m_bSaveAndValidate );
	m_errorCode.UpdateData( pDX->m_bSaveAndValidate );
	m_TimeChange.UpdateData( pDX->m_bSaveAndValidate );
	m_StatusFlags.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSLogRecordDlg, CDialog)
	//{{AFX_MSG_MAP(VTSLogRecordDlg)
	ON_EN_CHANGE(IDC_DATETIME_DATE, OnChangeDatetimeDate)
	ON_EN_CHANGE(IDC_DATETIME_TIME, OnChangeDatetimeTime)
	ON_EN_CHANGE(IDC_UNSIGNEDVALUE, OnChangeUnsignedvalue)
	ON_EN_CHANGE(IDC_TIMECHANGE, OnChangeTimechange)
	ON_EN_CHANGE(IDC_REALVALUE, OnChangeRealvalue)
	ON_BN_CLICKED(IDC_OVERRIDDEN, OnOverridden)
	ON_BN_CLICKED(IDC_OUTOFSERVICE, OnOutofservice)
	ON_BN_CLICKED(IDC_LOGDISABLED, OnLogdisabled)
	ON_EN_CHANGE(IDC_INTERGERVALUE, OnChangeIntergervalue)
	ON_BN_CLICKED(IDC_INALARM, OnInalarm)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_CBN_SELCHANGE(IDC_ERRORCODE, OnSelchangeErrorcode)
	ON_CBN_SELCHANGE(IDC_ERRORCLASS, OnSelchangeErrorclass)
	ON_EN_CHANGE(IDC_ENUMVALUE, OnChangeEnumvalue)
	ON_BN_CLICKED(IDC_BUFFERPURGED, OnBufferpurged)
	ON_BN_CLICKED(IDC_BOOLVALUE, OnBoolvalue)
	ON_EN_CHANGE(IDC_BITSTRINGVALUE, OnChangeBitstringvalue)
	ON_BN_CLICKED(IDC_ANY, OnAny)
	ON_CBN_SELCHANGE(IDC_LOGDATUM, OnSelchangeLogdatum)
	ON_BN_CLICKED(IDC_HASSTATUSFLAGS, OnHasstatusflags)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSLogRecordDlg message handlers

void VTSLogRecordDlg::OnChangeDatetimeDate() 
{
	m_DateCtrl.UpdateData();
}

void VTSLogRecordDlg::OnChangeDatetimeTime() 
{
	m_TimeCtrl.UpdateData();	
}
void VTSLogRecordDlg::OnChangeUnsignedvalue() 
{
	m_uintValue.UpdateData();
}

void VTSLogRecordDlg::OnChangeTimechange() 
{
	m_TimeChange.UpdateData();
}

void VTSLogRecordDlg::OnChangeRealvalue() 
{
	m_realValue.UpdateData();	
}

void VTSLogRecordDlg::OnOverridden() 
{
	m_StatusFlags.OverriddenClick();
}

void VTSLogRecordDlg::OnOutofservice() 
{
  m_StatusFlags.OutOfServiceClick();	
}

void VTSLogRecordDlg::OnLogdisabled() 
{
	// TODO: Add your control notification handler code here
	
}

void VTSLogRecordDlg::OnChangeIntergervalue() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
void VTSLogRecordDlg::OnInalarm() 
{
 m_StatusFlags.InAlarmClick();	
}

void VTSLogRecordDlg::OnFault() 
{
	m_StatusFlags.FaultClick();
}

void VTSLogRecordDlg::OnSelchangeErrorcode() 
{
	m_errorCode.ctrlNull = false;
	m_errorCode.CtrlToObj();
}

void VTSLogRecordDlg::OnSelchangeErrorclass() 
{
	m_errorClass.ctrlNull = false;
	m_errorClass.CtrlToObj();	
}

void VTSLogRecordDlg::OnChangeEnumvalue() 
{
	m_enumValue.UpdateData();
}

void VTSLogRecordDlg::OnBufferpurged() 
{
	m_BufferPurged.UpdateData();	
}

void VTSLogRecordDlg::OnBoolvalue() 
{
	m_boolValue.UpdateData();
}

void VTSLogRecordDlg::OnChangeBitstringvalue() 
{
	m_bitstringValue.UpdateData();	
}

void VTSLogRecordDlg::OnAny() 
{
   m_anyValue.DoModal();
}

void VTSLogRecordDlg::OnSelchangeLogdatum() 
{
	m_nSelItem = m_DatumChoice.GetCurSel();
	EnableChoice(m_nSelItem);
}
void VTSLogRecordDlg::OnHasstatusflags() 
{
	UpdateData();
	if(m_hasstatusflags)
	{
		GetDlgItem( IDC_INALARM )->EnableWindow( true );
		GetDlgItem( IDC_FAULT )->EnableWindow( true );
		GetDlgItem( IDC_OVERRIDDEN )->EnableWindow( true );
		GetDlgItem( IDC_OUTOFSERVICE )->EnableWindow( true );
	}
	else
	{
		GetDlgItem( IDC_INALARM )->EnableWindow( false );
		GetDlgItem( IDC_FAULT )->EnableWindow( false );
		GetDlgItem( IDC_OVERRIDDEN )->EnableWindow( false );
		GetDlgItem( IDC_OUTOFSERVICE )->EnableWindow( false );
	}
}
void VTSLogRecordDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	BACnetBitString		bits(2 );
	BACnetEnumerated enumsel;
	BACnetOpeningTag().Encode(enc, 0);
	m_DateCtrl.Encode(enc);
	m_TimeCtrl.Encode(enc);
	BACnetClosingTag().Encode(enc, 0);
	
	BACnetOpeningTag().Encode(enc, 1);
	switch(m_nSelItem)
	{
	case 0:
		bits.SetBit( 0, m_LogDisabled.boolValue );
		bits.SetBit( 1, m_BufferPurged.boolValue );
		bits.Encode( enc, 0);
		break;
	case 1:
		m_boolValue.Encode(enc, 1);
		break;
	case 2:
		m_realValue.Encode(enc, 2);
		break;
	case 3:
		enumsel.m_enumValue = m_enumValue.uintValue;
		enumsel.Encode(enc, 3);
		break;
	case 4:
		m_uintValue.Encode(enc, 4);
		break;
	case 5:
		m_intValue.Encode(enc, 5);
		break;
	case 6:
		m_bitstringValue.Encode(enc, 6);
		break;
	case 7:
		BACnetNull().Encode(enc, 7);
		break;
	case 8:
		BACnetOpeningTag().Encode(enc, 8);
		m_errorClass.Encode(enc);
		m_errorCode.Encode(enc);
		BACnetClosingTag().Encode(enc, 8);
		break;
	case 9:
		m_TimeChange.Encode(enc, 9);
		break;
	case 10:
		m_anyValue.Encode(enc, 10);
		break;
	default:
		break;
	}
	BACnetClosingTag().Encode(enc, 1);
	
	if(m_hasstatusflags)
		m_StatusFlags.Encode(enc, 2);
}
void VTSLogRecordDlg::Decode(BACnetAPDUDecoder& dec )
{
	BACnetBitString		bits( 2 );
	BACnetEnumerated enumsel;
	int tagmiss;
	if(dec.pktLength != 0)
	{
		BACnetOpeningTag().Decode(dec);
		m_DateCtrl.Decode(dec);
		m_DateCtrl.ctrlNull = false;
		m_TimeCtrl.Decode(dec);
		m_TimeCtrl.ctrlNull = false;
		BACnetClosingTag().Decode(dec);
		BACnetOpeningTag().Decode(dec);
		BACnetAPDUTag	tagTestType;
		dec.ExamineTag(tagTestType);
		m_nSelItem = tagTestType.tagNumber;
	switch(m_nSelItem)
	{
	case 0:
		bits.Decode(dec);
		m_LogDisabled.boolValue = (bits.GetBit(0)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
		m_BufferPurged.boolValue= (bits.GetBit(1)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
		m_LogDisabled.ctrlNull = false;
		m_BufferPurged.ctrlNull = false;
		break;
	case 1:
		m_boolValue.Decode(dec);
		m_boolValue.ctrlNull = false;
		break;
	case 2:
		m_realValue.Decode(dec);
		m_realValue.ctrlNull = false;
		break;
	case 3:
		enumsel.Decode(dec);
		m_enumValue.uintValue = enumsel.m_enumValue;
		m_enumValue.ctrlNull = false;
		break;
	case 4:
		m_uintValue.Decode(dec);
		m_uintValue.ctrlNull = false;
		break;
	case 5:
		m_intValue.Decode(dec);
		m_intValue.ctrlNull = false;
		break;
	case 6:
		m_bitstringValue.Decode(dec);
		m_bitstringValue.ctrlNull = false;
		break;
	case 7:
		BACnetNull().Decode(dec);
		m_boolValue.ctrlNull = false;
		break;
	case 8:
		BACnetOpeningTag().Decode(dec);
		m_errorClass.Decode(dec);
		m_errorCode.Decode(dec);
		m_errorClass.ctrlNull = false;
		m_errorCode.ctrlNull = false;
		BACnetClosingTag().Decode(dec);
		break;
	case 9:
		m_TimeChange.Decode(dec);
		m_TimeChange.ctrlNull = false;
		break;
	case 10:
		BACnetOpeningTag().Decode(dec);
		tagmiss = 1; 
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
		break;
	default:
		break;
	}		
	BACnetClosingTag().Decode(dec);
	
	if(dec.ExamineOption(2))
	{
		m_StatusFlags.Decode(dec);
		m_hasstatusflags = TRUE;
	}
	else
		m_hasstatusflags = FALSE;		
	}
}
BOOL VTSLogRecordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_errorClass.LoadCombo();
	m_errorCode.LoadCombo();
	if(m_hasstatusflags)
	{
		GetDlgItem( IDC_INALARM )->EnableWindow( true );
		GetDlgItem( IDC_FAULT )->EnableWindow( true );
		GetDlgItem( IDC_OVERRIDDEN )->EnableWindow( true );
		GetDlgItem( IDC_OUTOFSERVICE )->EnableWindow( true );
	}
	else
	{
		GetDlgItem( IDC_INALARM )->EnableWindow( false );
		GetDlgItem( IDC_FAULT )->EnableWindow( false );
		GetDlgItem( IDC_OVERRIDDEN )->EnableWindow( false );
		GetDlgItem( IDC_OUTOFSERVICE )->EnableWindow( false );
	}
	if(m_nSelItem >= 0)
	{
		m_DatumChoice.SetCurSel(m_nSelItem);
		EnableChoice(m_nSelItem);
	}
	else
		EnableChoice(-1);	
//	m_statusFlags.ObjToCtrl();
	return TRUE; 
}
void VTSLogRecordDlg::EnableChoice(int nchoice)
{
	switch(nchoice) {
	case 0:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( true );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( true );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
	case 1:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( true );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
	case 2:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( true );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
	case 3:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( true );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
	case 4:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( true );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
		case 5:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( true );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
		case 6:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( true );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
	case 7:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
	case 8:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( true );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( true );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
	case 9:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( true );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;
	case 10:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( true );
		break;
	default:
		GetDlgItem( IDC_LOGDISABLED )->EnableWindow( false );
		GetDlgItem( IDC_BUFFERPURGED )->EnableWindow( false );
		GetDlgItem( IDC_BOOLVALUE )->EnableWindow( false );
		GetDlgItem( IDC_REALVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ENUMVALUE )->EnableWindow( false );
		GetDlgItem( IDC_UNSIGNEDVALUE )->EnableWindow( false );
		GetDlgItem( IDC_INTERGERVALUE )->EnableWindow( false );
		GetDlgItem( IDC_BITSTRINGVALUE )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCLASS )->EnableWindow( false );
		GetDlgItem( IDC_ERRORCODE )->EnableWindow( false );
		GetDlgItem( IDC_TIMECHANGE )->EnableWindow( false );
		GetDlgItem( IDC_ANY )->EnableWindow( false );
		break;	
	}
}


