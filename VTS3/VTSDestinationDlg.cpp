// VTSDestinationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDestinationDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDestinationDlg dialog

#pragma warning( disable : 4355 )
VTSDestinationDlg::VTSDestinationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSDestinationDlg::IDD, pParent)
	, m_DeviceID( this, IDC_DESTINATION_DEVICEID )
	, m_Network( this, IDC_DESTINATION_NETWORK )
	, m_MACAddress( this, IDC_DESTINATION__MACADDRESS )
	, m_FromTimeCtrl(this, IDC_DESTINATION_FROMTIME)
	, m_ToTimeCtrl(this, IDC_DESTINATION_TOTIME)
	, m_ProcessID(this, IDC_PROCESS_IDENTIFIER)
{
	//{{AFX_DATA_INIT(VTSDestinationDlg)
	m_bDestinationFault = FALSE;
	m_bDestinationConfirm = FALSE;
	m_bDestinationFriday = FALSE;
	m_bDestinationMonday = FALSE;
	m_bDestinationNormal = FALSE;
	m_bDestinationOffNormal = FALSE;
	m_bDestinationSaturday = FALSE;
	m_bDestinationSunday = FALSE;
	m_bDestinationThursday = FALSE;
	m_bDestinationTuesday = FALSE;
	m_bDestinationWednesday = FALSE;
	m_IsRecipientDevice = -1;
	//}}AFX_DATA_INIT
}


void VTSDestinationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDestinationDlg)
	DDX_Check(pDX, IDC_DESTINATION_FAULT, m_bDestinationFault);
	DDX_Check(pDX, IDC_DESTINATION_CONFIRM, m_bDestinationConfirm);
	DDX_Check(pDX, IDC_DESTINATION_FRIDAY, m_bDestinationFriday);
	DDX_Check(pDX, IDC_DESTINATION_MONDAY, m_bDestinationMonday);
	DDX_Check(pDX, IDC_DESTINATION_NORMAL, m_bDestinationNormal);
	DDX_Check(pDX, IDC_DESTINATION_OFFNORMAL, m_bDestinationOffNormal);
	DDX_Check(pDX, IDC_DESTINATION_SATURDAY, m_bDestinationSaturday);
	DDX_Check(pDX, IDC_DESTINATION_SUNDAY, m_bDestinationSunday);
	DDX_Check(pDX, IDC_DESTINATION_THURSDAY, m_bDestinationThursday);
	DDX_Check(pDX, IDC_DESTINATION_TUESDAY, m_bDestinationTuesday);
	DDX_Check(pDX, IDC_DESTINATION_WEDNESDAY, m_bDestinationWednesday);
	DDX_Radio(pDX, IDC_DESTINATION_DEVICE, m_IsRecipientDevice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDestinationDlg, CDialog)
	//{{AFX_MSG_MAP(VTSDestinationDlg)
	ON_BN_CLICKED(IDC_DESTINATION_ADDRESS, OnDestinationAddress)
	ON_BN_CLICKED(IDC_DESTINATION_DEVICE, OnDestinationDevice)
	ON_BN_CLICKED(IDC_DESTINATION_DEVICEIDBTN, OnDeviceIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDestinationDlg message handlers

void VTSDestinationDlg::OnDestinationAddress() 
{
	this->GetDlgItem(IDC_DESTINATION_DEVICEID)->EnableWindow(false);
	this->GetDlgItem(IDC_DESTINATION_NETWORK)->EnableWindow(true);
	this->GetDlgItem(IDC_DESTINATION__MACADDRESS)->EnableWindow(true);
	if (m_Network.ctrlNull)
	{
		m_Network.ctrlNull = false;
		m_Network.ObjToCtrl();
	}
	if (m_MACAddress.ctrlNull)
	{
		m_MACAddress.ctrlNull = false;
		m_MACAddress.ObjToCtrl();
	}
	
}

void VTSDestinationDlg::OnDestinationDevice() 
{
	this->GetDlgItem(IDC_DESTINATION_DEVICEID)->EnableWindow(true);
	this->GetDlgItem(IDC_DESTINATION_NETWORK)->EnableWindow(false);
	this->GetDlgItem(IDC_DESTINATION__MACADDRESS)->EnableWindow(false);
	if (m_DeviceID.ctrlNull)
	{
		m_DeviceID.ctrlNull = false;
		m_DeviceID.ObjToCtrl();
	}
}

BOOL VTSDestinationDlg::OnInitDialog() 
{
	char junk[20];  // MAG 06JUN06
	CDialog::OnInitDialog();	
	
	sprintf(junk,"%d",m_Value.m_processID.uintValue);  // MAG 06JUN06
	this->GetDlgItem(IDC_PROCESS_IDENTIFIER)->SetWindowText(junk);  // MAG 06JUN06

	m_bDestinationConfirm = m_Value.m_issueConfirmedNotifications.boolValue;
	m_bDestinationOffNormal = m_Value.m_transitions.GetBit(0);
	m_bDestinationFault = m_Value.m_transitions.GetBit(1);
	m_bDestinationNormal = m_Value.m_transitions.GetBit(2);
			
	m_bDestinationMonday = m_Value.m_validDays.GetBit(0);
	m_bDestinationTuesday = m_Value.m_validDays.GetBit(1);
	m_bDestinationWednesday = m_Value.m_validDays.GetBit(2);
	m_bDestinationThursday = m_Value.m_validDays.GetBit(3);
	m_bDestinationFriday = m_Value.m_validDays.GetBit(4);
	m_bDestinationSaturday = m_Value.m_validDays.GetBit(5);
	m_bDestinationSunday = m_Value.m_validDays.GetBit(6);
	
	BACnetTime* pTime1 = (BACnetTime*)(&m_Value.m_fromTime);
	BACnetTime* pCtrl1 = &m_FromTimeCtrl;
	*pCtrl1 = *pTime1;
	m_FromTimeCtrl.ctrlNull=false;
	m_FromTimeCtrl.ObjToCtrl();	
	
	BACnetTime* pTime2 = (BACnetTime*)(&m_Value.m_toTime);
	BACnetTime* pCtrl2 = &m_ToTimeCtrl;
	*pCtrl2 = *pTime2;
	m_ToTimeCtrl.ctrlNull=false;
	m_ToTimeCtrl.ObjToCtrl();
	if(m_Value.m_recipient.GetObject() != NULL)
	{
		if(m_Value.m_recipient.GetChoice()== 0)
		{
			m_IsRecipientDevice = 0;
			BACnetObjectIdentifier* pDeviceCtrl=&m_DeviceID;
			m_Network.ctrlNull=true;
			m_ProcessID.ctrlNull=true;  // MAG 06JUN06
			m_DeviceID.ctrlNull = false;	
			pDeviceCtrl->objID = ((BACnetObjectIdentifier*)(m_Value.m_recipient.GetObject()))->objID ;				
			m_DeviceID.ObjToCtrl();
			OnDestinationDevice();
		}
		else
		{
			m_MACAddress.ctrlNull=false;
			m_Network.ctrlNull=false;
			m_DeviceID.ctrlNull = true;
			m_IsRecipientDevice = 1;																		
			m_Network.uintValue = ((BACnetAddr*)m_Value.m_recipient.GetObject())->m_bacnetAddress.addrNet;
			m_ProcessID.uintValue = 0;  // MAG 06JUN06
			m_Network.ObjToCtrl();			
			m_MACAddress.Insert(((BACnetAddr*)m_Value.m_recipient.GetObject())->m_bacnetAddress.addrAddr, ((BACnetAddr*)m_Value.m_recipient.GetObject())->m_bacnetAddress.addrLen, 0);
			m_MACAddress.ObjToCtrl();			
			OnDestinationAddress();
		}
	}
	else
	{
		m_IsRecipientDevice = 0;
		m_MACAddress.ctrlNull=true;
		m_Network.ctrlNull=true;
		m_ProcessID.ctrlNull=true;    // MAG 06JUN06
		m_DeviceID.ctrlNull = false;
		OnDestinationDevice();
	}

	UpdateData(FALSE);		
				
	return TRUE;  
}

void VTSDestinationDlg::OnDeviceIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// added parent for proper dialog chain
	;

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID)
	{
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}
}
void VTSDestinationDlg::Encode(BACnetAPDUEncoder &enc, int context)
{
	m_Value.Encode(enc,context);
}

void VTSDestinationDlg::Decode(BACnetAPDUDecoder &dec)
{
	if(dec.pktLength!=0)
			m_Value.Decode(dec);
}

void VTSDestinationDlg::OnOK() 
{	
	char junk[12];
	UpdateData(TRUE);
	m_Value.m_issueConfirmedNotifications = *(new BACnetBoolean(m_bDestinationConfirm));
#pragma warning( disable : 4800 )
	m_Value.m_transitions = *(new BACnetEventTransitionBits(m_bDestinationOffNormal,m_bDestinationFault,m_bDestinationNormal));
	m_Value.m_validDays = *(new BACnetDaysOfWeek(m_bDestinationMonday,
												 m_bDestinationTuesday,
												 m_bDestinationWednesday,
												 m_bDestinationThursday,
												 m_bDestinationFriday,
												 m_bDestinationSaturday,
												 m_bDestinationSunday));								
#pragma warning( default : 4800 )
	m_FromTimeCtrl.CtrlToObj();
	m_Value.m_fromTime = m_FromTimeCtrl;
	m_ToTimeCtrl.CtrlToObj();
	m_Value.m_toTime = m_ToTimeCtrl;

	this->GetDlgItem(IDC_PROCESS_IDENTIFIER)->GetWindowText(junk,10);  // MAG 06JUN06
	m_Value.m_processID = atoi(junk);  // MAG 06JUN06

	if(!m_IsRecipientDevice)
	{
		m_MACAddress.ctrlNull=true;
		m_Network.ctrlNull=true;
		m_DeviceID.ctrlNull = false;
		m_DeviceID.CtrlToObj();
	}
	else
	{
		m_MACAddress.ctrlNull=false;
		m_Network.ctrlNull=false;
		m_DeviceID.ctrlNull = true;
		m_Network.CtrlToObj();
		m_MACAddress.CtrlToObj();
	}
	if(m_IsRecipientDevice== 0)
	{
		BACnetObjectIdentifier* pDevice = new BACnetObjectIdentifier;
		pDevice->objID = m_DeviceID.objID;
		m_Value.m_recipient.SetObject(pDevice);
	}
	else
	{
		m_Value.m_recipient.SetObject(new BACnetAddr(m_Network.uintValue, 
		m_MACAddress.strBuff, m_MACAddress.strLen));
	}				
	CDialog::OnOK();
}
