// VTSRecipientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSRecipientDlg.h"
#include "VTSObjectIdentifierDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSRecipientDlg dialog


#pragma warning( disable : 4355 )
VTSRecipientDlg::VTSRecipientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSRecipientDlg::IDD, pParent)
	, m_DeviceID( this, IDC_DEVICEID )
	, m_Network( this, IDC_NETWORK )
	, m_MACAddress( this, IDC_MACADDRESS )
{
	//{{AFX_DATA_INIT(VTSRecipientDlg)
	m_choice = 0;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSRecipientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSRecipientDlg)
	DDX_Radio(pDX, IDC_RADIODEVICE, m_choice);
	//}}AFX_DATA_MAP
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_Network.UpdateData( pDX->m_bSaveAndValidate );
	m_MACAddress.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSRecipientDlg, CDialog)
	//{{AFX_MSG_MAP(VTSRecipientDlg)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceid)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceidbtn)
	ON_EN_CHANGE(IDC_MACADDRESS, OnChangeMacaddress)
	ON_EN_CHANGE(IDC_NETWORK, OnChangeNetwork)
	ON_BN_CLICKED(IDC_RADIOADDRESS, OnRadioaddress)
	ON_BN_CLICKED(IDC_RADIODEVICE, OnRadiodevice)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSRecipientDlg message handlers

void VTSRecipientDlg::OnChangeDeviceid() 
{
	m_DeviceID.UpdateData();
}

void VTSRecipientDlg::OnDeviceidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);
	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) 
	{
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}
}

void VTSRecipientDlg::OnChangeMacaddress() 
{
	m_MACAddress.UpdateData();
}

void VTSRecipientDlg::OnChangeNetwork() 
{
	m_MACAddress.UpdateData();
}

BOOL VTSRecipientDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_choice == 0)
		OnRadiodevice();
	if(m_choice == 1)
		OnRadioaddress();
	return TRUE;  
}
void VTSRecipientDlg::Encode( BACnetAPDUEncoder& enc, int context)
{

	if (m_choice == 0) 
		m_DeviceID.Encode( enc, 0 );
	if (m_choice == 1) 
	{		
		BACnetOpeningTag().Encode( enc, 1);		
		m_Network.Encode( enc );
		m_MACAddress.Encode( enc );
		BACnetClosingTag().Encode( enc, 1);
	}
}
void VTSRecipientDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)
	{
		
		BACnetAPDUTag tag;	
		dec.ExamineTag(tag);	
		switch (tag.tagNumber) 
		{
		case 0:
			m_DeviceID.Decode(dec);
			m_choice = 0;
			m_DeviceID.ctrlNull = false;
			m_Network.ctrlNull = true;
			m_MACAddress.ctrlNull = true;
			break;
		case 1:
			BACnetOpeningTag().Decode(dec);
			m_Network.Decode(dec);
			m_MACAddress.Decode(dec);
			BACnetClosingTag().Decode(dec);
			m_choice = 1;
			m_DeviceID.ctrlNull = true;
			m_Network.ctrlNull = false;
			m_MACAddress.ctrlNull = false;
			break;
		default:
			throw "Unknow choice.";
			break;
		}
	}
}

void VTSRecipientDlg::OnRadioaddress() 
{
	this->GetDlgItem(IDC_NETWORK)->EnableWindow(true);
	this->GetDlgItem(IDC_MACADDRESS)->EnableWindow(true);
	this->GetDlgItem(IDC_DEVICEID)->EnableWindow(false);
	this->GetDlgItem(IDC_DEVICEIDBTN)->EnableWindow(false);
	m_DeviceID.ctrlNull = true;
}

void VTSRecipientDlg::OnRadiodevice() 
{
	this->GetDlgItem(IDC_NETWORK)->EnableWindow(false);
	this->GetDlgItem(IDC_MACADDRESS)->EnableWindow(false);
	this->GetDlgItem(IDC_DEVICEID)->EnableWindow(true);
	this->GetDlgItem(IDC_DEVICEIDBTN)->EnableWindow(true);
	m_Network.ctrlNull = true;
	m_MACAddress.ctrlNull = true;
}
