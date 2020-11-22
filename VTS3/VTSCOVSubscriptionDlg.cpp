// VTSCOVSubscriptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSCOVSubscriptionDlg.h"
#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSCOVSubscriptionDlg dialog


#pragma warning( disable : 4355 )
VTSCOVSubscriptionDlg::VTSCOVSubscriptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSCOVSubscriptionDlg::IDD, pParent)
	, m_DeviceID( this, IDC_DEVICEID )
	, m_Network( this, IDC_NETWORK )
	, m_MACAddress( this, IDC_MACADDRESS )
	, m_ProcessID( this, IDC_PROCESSID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_issuecfmntf( this, IDC_ISSUECFMNTF, true)
	, m_TimeRemaining( this, IDC_TIMEREMAIN )
	, m_COVIncr( this, IDC_INCREMENT )
{
	//{{AFX_DATA_INIT(VTSCOVSubscriptionDlg)
	m_choice = 0;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSCOVSubscriptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSCOVSubscriptionDlg)
	DDX_Radio(pDX, IDC_RADIODEVICE, m_choice);
	//}}AFX_DATA_MAP
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_Network.UpdateData( pDX->m_bSaveAndValidate );
	m_MACAddress.UpdateData( pDX->m_bSaveAndValidate );
	m_ProcessID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	m_issuecfmntf.UpdateData( pDX->m_bSaveAndValidate );
	m_TimeRemaining.UpdateData( pDX->m_bSaveAndValidate );
	m_COVIncr.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSCOVSubscriptionDlg, CDialog)
	//{{AFX_MSG_MAP(VTSCOVSubscriptionDlg)
	ON_BN_CLICKED(IDC_RADIODEVICE, OnRadiodevice)
	ON_BN_CLICKED(IDC_RADIOADDRESS, OnRadioaddress)
	ON_EN_CHANGE(IDC_TIMEREMAIN, OnChangeTimeremain)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_EN_CHANGE(IDC_PROCESSID, OnChangeProcessid)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_EN_CHANGE(IDC_NETWORK, OnChangeNetwork)
	ON_EN_CHANGE(IDC_MACADDRESS, OnChangeMacaddress)
	ON_EN_CHANGE(IDC_INCREMENT, OnChangeIncrement)
	ON_BN_CLICKED(IDC_ISSUECFMNTF, OnIssuecfmntf)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceid)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceidbtn)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSCOVSubscriptionDlg message handlers

void VTSCOVSubscriptionDlg::OnRadiodevice() 
{
	this->GetDlgItem(IDC_NETWORK)->EnableWindow(false);
	this->GetDlgItem(IDC_MACADDRESS)->EnableWindow(false);
	this->GetDlgItem(IDC_DEVICEID)->EnableWindow(true);
	this->GetDlgItem(IDC_DEVICEIDBTN)->EnableWindow(true);
	m_Network.ctrlNull = true;
	m_MACAddress.ctrlNull = true;
}

void VTSCOVSubscriptionDlg::OnRadioaddress() 
{
	this->GetDlgItem(IDC_NETWORK)->EnableWindow(true);
	this->GetDlgItem(IDC_MACADDRESS)->EnableWindow(true);
	this->GetDlgItem(IDC_DEVICEID)->EnableWindow(false);
	this->GetDlgItem(IDC_DEVICEIDBTN)->EnableWindow(false);
	m_DeviceID.ctrlNull = true;
}

void VTSCOVSubscriptionDlg::OnChangeTimeremain() 
{
	m_TimeRemaining.UpdateData();
}

void VTSCOVSubscriptionDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSCOVSubscriptionDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSCOVSubscriptionDlg::OnChangeProcessid() 
{
	m_ProcessID.UpdateData();
}

void VTSCOVSubscriptionDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSCOVSubscriptionDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}

void VTSCOVSubscriptionDlg::OnChangeNetwork() 
{
	m_Network.UpdateData();
}

void VTSCOVSubscriptionDlg::OnChangeMacaddress() 
{
	m_MACAddress.UpdateData();
}

void VTSCOVSubscriptionDlg::OnChangeIncrement() 
{
	m_COVIncr.UpdateData();
}

void VTSCOVSubscriptionDlg::OnIssuecfmntf() 
{
	m_issuecfmntf.UpdateData();
}

BOOL VTSCOVSubscriptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_propCombo.LoadCombo();
	m_propCombo.ObjToCtrl();
	if(m_choice == 0)
		OnRadiodevice();
	if(m_choice == 1)
		OnRadioaddress();	
	return TRUE;  
}

void VTSCOVSubscriptionDlg::OnChangeDeviceid() 
{
	m_DeviceID.UpdateData();
}

void VTSCOVSubscriptionDlg::OnDeviceidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}	
}

void VTSCOVSubscriptionDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}
void VTSCOVSubscriptionDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	BACnetOpeningTag().Encode(enc, 0);
		BACnetOpeningTag().Encode( enc, 0 );
		if (m_choice == 0) 
			m_DeviceID.Encode( enc, 0 );
		if (m_choice == 1) 
		{		
			BACnetOpeningTag().Encode( enc, 1);		
			m_Network.Encode( enc );
			m_MACAddress.Encode( enc );
			BACnetClosingTag().Encode( enc, 1);
		}
		BACnetClosingTag().Encode(enc, 0);
		m_ProcessID.Encode( enc, 1 );
	BACnetClosingTag().Encode(enc, 0);

	BACnetOpeningTag().Encode(enc, 1);
	m_ObjectID.Encode(enc, 0);
	m_propCombo.Encode(enc, 1);
	
	if(!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode(enc, 2);
	BACnetClosingTag().Encode(enc, 1);
	
	m_issuecfmntf.Encode(enc, 2);
	m_TimeRemaining.Encode(enc, 3);
	
	if(!m_COVIncr.ctrlNull)
		m_COVIncr.Encode(enc, 4);
}
void VTSCOVSubscriptionDlg::Decode(BACnetAPDUDecoder& dec )
{
	if (dec.pktLength != 0)
	{
		BACnetOpeningTag().Decode(dec);		
		BACnetOpeningTag().Decode(dec);
		BACnetAPDUTag tag;	
		dec.ExamineTag(tag);	
		switch (tag.tagNumber) 
		{
		case 0:
			m_DeviceID.Decode(dec);
			m_DeviceID.ctrlNull = false;
			m_Network.ctrlNull = true;
			m_MACAddress.ctrlNull = true;
			m_choice = 0;
			break;
		case 1:
			BACnetOpeningTag().Decode(dec);
			m_Network.Decode(dec);
			m_MACAddress.Decode(dec);
			BACnetClosingTag().Decode(dec);
			m_DeviceID.ctrlNull = true;
			m_Network.ctrlNull = false;
			m_MACAddress.ctrlNull = false;
			m_choice = 1;
			break;
		default:
			throw "Unknow choice.";
			break;
		}
		BACnetClosingTag().Decode(dec);
		m_ProcessID.Decode(dec);
		m_ProcessID.ctrlNull = false;
		BACnetClosingTag().Decode(dec);

	BACnetOpeningTag().Decode(dec);
	m_ObjectID.Decode(dec);
	m_ObjectID.ctrlNull = false;
	
	m_propCombo.Decode(dec);
	m_propCombo.ctrlNull = false;		
	if(dec.ExamineOption(2))
	{
		m_ArrayIndex.Decode(dec);
		m_ArrayIndex.ctrlNull = false;
	}
	else
		m_ArrayIndex.ctrlNull = true;		
	BACnetClosingTag().Decode(dec);
	
	m_issuecfmntf.Decode(dec);
	m_issuecfmntf.ctrlNull = false;
	
	m_TimeRemaining.Decode(dec);
	m_TimeRemaining.ctrlNull = false;		
	if(dec.ExamineOption(4))
	{
		m_COVIncr.Decode(dec);
		m_COVIncr.ctrlNull = false;
	}
	else
		m_COVIncr.ctrlNull = true;		
	
	}	
}
