// SendIAm.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendIAm.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendIAm::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendIAm dialog

IMPLEMENT_DYNCREATE( CSendIAm, CPropertyPage )

#pragma warning( disable : 4355 )
CSendIAm::CSendIAm( void )
	: CSendPage( CSendIAm::IDD )
	, m_DeviceID( this, IDC_DEVICEID )
	, m_MaxAPDULength( this, IDC_MAXAPDULENGTH )
	, m_SegmentationCombo( this, IDC_SEGMENTATIONCOMBO, NetworkSniffer::BAC_STRTAB_BACnetSegmentation, true )
	, m_VendorID( this, IDC_VENDORID )
{
	//{{AFX_DATA_INIT(CSendIAm)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendIAm::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendIAm::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendIAm)
	//}}AFX_DATA_MAP

	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_MaxAPDULength.UpdateData( pDX->m_bSaveAndValidate );
	m_SegmentationCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_VendorID.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendIAm, CPropertyPage)
	//{{AFX_MSG_MAP(CSendIAm)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceID)
	ON_EN_CHANGE(IDC_MAXAPDULENGTH, OnChangeMaxAPDULength)
	ON_CBN_SELCHANGE(IDC_SEGMENTATIONCOMBO, OnSelchangeSegmentationCombo)
	ON_EN_CHANGE(IDC_VENDORID, OnChangeVendorID)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendIAm::OnInitDialog
//

BOOL CSendIAm::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_SegmentationCombo.LoadCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendIAm::InitPage
//

void CSendIAm::InitPage( void )
{
	TRACE0( "CSendIAm::InitPage()\n" );

	// flush the data
	m_DeviceID.ctrlNull = true;
	m_MaxAPDULength.ctrlNull = true;

	m_SegmentationCombo.ctrlNull = false;
	m_SegmentationCombo.m_enumValue = 0;

	m_VendorID.ctrlNull = true;

	// tell the NPCI this is a global broadcast
	pageParent->NPCIPage.m_DestPresent = true;

	pageParent->NPCIPage.m_DNET.ctrlNull = false;
	pageParent->NPCIPage.m_DNET.intValue = 65535;

	pageParent->NPCIPage.m_HopCount.ctrlNull = false;
	pageParent->NPCIPage.m_HopCount.intValue = 255;
}

//
//	CSendIAm::EncodePage
//

void CSendIAm::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendIAm::EncodePage()\n" );

	// encode the pdu type and service choice
	header.Add( 0x10 );
	header.Add( 0x00 );

	// encode the device ID
	if (m_DeviceID.ctrlNull)
		throw "Device ID required";
	m_DeviceID.Encode( enc );

	// encode the maximum APDU length accepted
	if (m_MaxAPDULength.ctrlNull)
		throw "Max APDU length required";
	m_MaxAPDULength.Encode( enc );

	// encode the segmentation support
	if (m_SegmentationCombo.ctrlNull)
		throw "Segmentation selection required";
	m_SegmentationCombo.Encode( enc );

	// encode the maximum APDU length accepted
	if (m_VendorID.ctrlNull)
		throw "Vendor ID required";
	m_VendorID.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendIAm::SavePage
//

void CSendIAm::SavePage( void )
{
	TRACE0( "CSendIAm::SavePage\n" );

	pageContents.Flush();

	m_DeviceID.SaveCtrl( pageContents );
	m_MaxAPDULength.SaveCtrl( pageContents );
	m_SegmentationCombo.SaveCtrl( pageContents );
	m_VendorID.SaveCtrl( pageContents );
}

//
//	CSendIAm::RestorePage
//

void CSendIAm::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendIAm::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_DeviceID.RestoreCtrl( dec );
	m_MaxAPDULength.RestoreCtrl( dec );
	m_SegmentationCombo.RestoreCtrl( dec );
	m_VendorID.RestoreCtrl( dec );
}

//
//	CSendIAm::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendIAm::OnChangeDeviceID()
{
	TRACE0( "CSendIAm::OnChangeDeviceID()\n" );
	m_DeviceID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendIAm::OnChangeMaxAPDULength()
{
	TRACE0( "CSendIAm::OnChangeMaxAPDULength()\n" );
	m_MaxAPDULength.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendIAm::OnSelchangeSegmentationCombo() 
{
	TRACE0( "CSendIAm::OnSelchangeSegmentationCombo()\n" );
	m_SegmentationCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendIAm::OnChangeVendorID()
{
	TRACE0( "CSendIAm::OnChangeVendorID()\n" );
	m_VendorID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendIAm::OnDeviceIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// for proper parent control
	;

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}
