// VTSRecipientProcess.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSRecipientProcess.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSRecipientProcess dialog

#pragma warning( disable : 4355 )
VTSRecipientProcess::VTSRecipientProcess(CWnd* pParent /*=NULL*/)
	: CDialog(VTSRecipientProcess::IDD, pParent)
	, m_DeviceID( this, IDC_DEVICEID )
	, m_Network( this, IDC_NETWORK )
	, m_MACAddress( this, IDC_MACADDRESS )
	, m_ProcessID( this, IDC_PROCESSID )
{
	//{{AFX_DATA_INIT(VTSRecipientProcess)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// everything starts out empty
	ctrlNull = true;
}
#pragma warning( default : 4355 )

void VTSRecipientProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSRecipientProcess)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_Network.UpdateData( pDX->m_bSaveAndValidate );
	m_MACAddress.UpdateData( pDX->m_bSaveAndValidate );
	m_ProcessID.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSRecipientProcess, CDialog)
	//{{AFX_MSG_MAP(VTSRecipientProcess)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceID)
	ON_EN_CHANGE(IDC_NETWORK, OnChangeNetwork)
	ON_EN_CHANGE(IDC_MACADDRESS, OnChangeMACAddress)
	ON_EN_CHANGE(IDC_PROCESSID, OnChangeProcessID)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSRecipientProcess::Encode
//
//	A simplified way of copying the contents of the list elements into 
//	the encoder.  This simplifies EncodePage() functions.
//

void VTSRecipientProcess::Encode( BACnetAPDUEncoder& enc, int context )
{
	if (context != kAppContext)
		BACnetOpeningTag().Encode( enc, context );

	// start the recipient
	BACnetOpeningTag().Encode( enc, 0 );

	if (!m_DeviceID.ctrlNull) {
		if (!m_Network.ctrlNull || !m_MACAddress.ctrlNull)
			throw "Exclude network and MAC address when providing a device";
		m_DeviceID.Encode( enc, 0 );
	} else {
		if (m_Network.ctrlNull && m_MACAddress.ctrlNull)
			throw "Specify a device or network and MAC address";

		// start the address
		BACnetOpeningTag().Encode( enc, 1 ); // Modified by Zhu Zhenhua, 2004-6-14, it should be context 1

		if (m_Network.ctrlNull)
			throw "Network required";
		m_Network.Encode( enc );
		if (m_MACAddress.ctrlNull)
			throw "MAC address required";
		m_MACAddress.Encode( enc );

		// end the address
		BACnetClosingTag().Encode( enc, 1 ); // Modified by Zhu Zhenhua, 2004-6-14, it should be context 1
	}

	// end the recipient
	BACnetClosingTag().Encode( enc, 0 );

	// do the process ID
	if (m_ProcessID.ctrlNull)
		throw "Process ID required";
	m_ProcessID.Encode( enc, 1 );

	if (context != kAppContext)
		BACnetClosingTag().Encode( enc, context );
}

//
//	VTSRecipientProcess::SaveCtrl
//

void VTSRecipientProcess::SaveCtrl( BACnetAPDUEncoder& enc )
{
//	TRACE0( "VTSRecipientProcess::SaveCtrl\n" );

	m_DeviceID.SaveCtrl( enc );
	m_Network.SaveCtrl( enc );
	m_MACAddress.SaveCtrl( enc );
	m_ProcessID.SaveCtrl( enc );
}

//
//	VTSRecipientProcess::RestoreCtrl
//

void VTSRecipientProcess::RestoreCtrl( BACnetAPDUDecoder& dec )
{
//	TRACE0( "VTSRecipientProcess::RestoreCtrl\n" );

	m_DeviceID.RestoreCtrl( dec );
	m_Network.RestoreCtrl( dec );
	m_MACAddress.RestoreCtrl( dec );
	m_ProcessID.RestoreCtrl( dec );
}

//
//	VTSRecipientProcess::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.  It would be nice if this object had a pointer back to the page 
//	that included this dialog box so the UpdateEncoded() processing could continue 
//	to work rather than be suspended until after it is complete.  See ###.
//

void VTSRecipientProcess::OnChangeDeviceID()
{
	m_DeviceID.UpdateData();

	ctrlNull = m_DeviceID.ctrlNull && m_Network.ctrlNull
		&& m_MACAddress.ctrlNull
		&& m_ProcessID.ctrlNull
		;
//	UpdateEncoded();
}

void VTSRecipientProcess::OnDeviceIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// added parent for proper dialog chain
	;

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();

		ctrlNull = false;

//		UpdateEncoded();
	}
}

void VTSRecipientProcess::OnChangeNetwork()
{
	m_Network.UpdateData();

	ctrlNull = m_DeviceID.ctrlNull && m_Network.ctrlNull
		&& m_MACAddress.ctrlNull
		&& m_ProcessID.ctrlNull
		;
//	UpdateEncoded();
}

void VTSRecipientProcess::OnChangeMACAddress()
{
	m_MACAddress.UpdateData();

	ctrlNull = m_DeviceID.ctrlNull && m_Network.ctrlNull
		&& m_MACAddress.ctrlNull
		&& m_ProcessID.ctrlNull
		;
//	UpdateEncoded();
}

void VTSRecipientProcess::OnChangeProcessID()
{
	m_ProcessID.UpdateData();

	ctrlNull = m_DeviceID.ctrlNull && m_Network.ctrlNull
		&& m_MACAddress.ctrlNull
		&& m_ProcessID.ctrlNull
		;
//	UpdateEncoded();
}

BOOL VTSRecipientProcess::OnInitDialog() 
{
	TRACE0( "VTSRecipientProcess::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//Added by Zhu Zhenhua, 2004-6-14
void VTSRecipientProcess::EncodeValue( BACnetAPDUEncoder& enc, int context )
{
	// start the recipient
	BACnetOpeningTag().Encode( enc, 0 );

	if (!m_DeviceID.ctrlNull) {
		if (!m_Network.ctrlNull || !m_MACAddress.ctrlNull)
			throw "Exclude network and MAC address when providing a device";
		m_DeviceID.Encode( enc, 0 );
	} else {
		if (m_Network.ctrlNull && m_MACAddress.ctrlNull)
			throw "Specify a device or network and MAC address";

		// start the address
		BACnetOpeningTag().Encode( enc, 1 );  

		if (m_Network.ctrlNull)
			throw "Network required";
		m_Network.Encode( enc );
		if (m_MACAddress.ctrlNull)
			throw "MAC address required";
		m_MACAddress.Encode( enc );

		// end the address
		BACnetClosingTag().Encode( enc, 1 );
	}

	// end the recipient
	BACnetClosingTag().Encode( enc, 0 );

	// do the process ID
	if (m_ProcessID.ctrlNull)
		throw "Process ID required";
	m_ProcessID.Encode( enc, 1 );
}
void VTSRecipientProcess::DecodeValue(BACnetAPDUDecoder& dec )
{
	if (dec.pktLength != 0)
	{
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
			break;
		case 1:
			BACnetOpeningTag().Decode(dec);
			m_Network.Decode(dec);
			m_MACAddress.Decode(dec);
			BACnetClosingTag().Decode(dec);
			m_DeviceID.ctrlNull = true;
			m_Network.ctrlNull = false;
			m_MACAddress.ctrlNull = false;
			break;
		default:
			throw "Unknow choice.";
			break;
		}
		BACnetClosingTag().Decode(dec);
		m_ProcessID.Decode(dec);
		m_ProcessID.ctrlNull = false;
	}
}
