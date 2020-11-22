#if !defined(AFX_SEND_H__8707D031_2ADB_11D4_BE9D_00A0C95A9812__INCLUDED_)
#define AFX_SEND_H__8707D031_2ADB_11D4_BE9D_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Send.h : header file
//

#include "VTSDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CSend

#include "SendPage.h"

// Null Page
#include "SendNull.h"
#include "SendRaw.h"
#include "SendTest.h"

// BIP Pages
#include "SendIP.h"
#include "SendBVLCI.h"
#include "SendBVLCResult.h"
#include "SendWriteBDT.h"
#include "SendReadBDT.h"
#include "SendReadBDTAck.h"
#include "SendRegisterFD.h"
#include "SendReadFDT.h"
#include "SendReadFDTAck.h"
#include "SendDeleteFDTEntry.h"
#include "SendNetworkNumberIs.h"
#include "SendWhatIsNetworkNumber.h"

// Ethernet Pages
#include "SendEnet.h"

// ARCNET Pages

// MSTP Pages
#include "SendMSTP.h"

// PTP Pages

// Device Pages
#include "SendDevice.h"

// NPDU Pages
#include "SendNPCI.h"
#include "SendVendorNPDU.h"
#include "SendWhoIsRTN.h"
#include "SendIAmRTN.h"
#include "SendICouldBeRTN.h"
#include "SendRejectMTN.h"
#include "SendRouterBusy.h"
#include "SendRouterAvailable.h"
#include "SendInitRT.h"
#include "SendInitRTAck.h"
#include "SendEstablishCTN.h"
#include "SendDisconnectCTN.h"

// APDU Base Pages
#include "SendConfirmedRequest.h"
#include "SendComplexACK.h"

// Confirmed Alarm and Event Services
#include "SendAckAlarm.h"
#include "SendConfCOVNotification.h"
#include "SendUnconfCOVNotification.h"
#include "SendGetAlarmSummary.h"
#include "SendGetAlarmSummaryACK.h"
#include "SendConfEventNotification.h"
#include "SendUnconfEventNotification.h"
#include "SendGetEnrollmentSummary.h"
#include "SendGetEnrollmentSummaryACK.h"
#include "SendGetEventInfo.h"			//Added by Zhu Zhenhua, 5-25
#include "SendGetEventInfoACK.h"	   //Added by Zhu Zhenhua, 5-25
#include "SendSubscribeCOV.h"
#include "SendSubscribeCOVProperty.h"

// File Access Services
#include "SendReadFile.h"
#include "SendReadFileACK.h"
#include "SendWriteFile.h"
#include "SendWriteFileACK.h"

// Confirmed Object Access Pages
#include "SendAddListElement.h"
#include "SendCreateObject.h"
#include "SendCreateObjectACK.h"
#include "SendCreateObjectError.h"
#include "SendDeleteObject.h"
#include "SendReadProp.h"
#include "SendReadPropACK.h"
#include "SendReadPropMult.h"
#include "SendReadPropMultACK.h"
#include "SendRemoveListElement.h"
#include "SendWriteProp.h"
#include "SendWritePropMult.h"
#include "SendWritePropMultError.h"
#include "SendReadRange.h"

// APDU Pages
#include "SendSimpleACK.h"
#include "SendSegmentACK.h"
#include "SendError.h"
#include "SendReject.h"
#include "SendAbort.h"
#include "SendChangeListError.h"

// Remote Device Management Pages
#include "SendDeviceCommCtrl.h"
#include "SendConfTextMsg.h"
#include "SendIAm.h"
#include "SendIHave.h"
#include "SendReinitDevice.h"
#include "SendUnconfTextMsg.h"
#include "SendTimeSync.h"
#include "SendUTCTimeSync.h"
//#include "SendUTCTimeSync.h"
#include "SendWhoHas.h"
#include "SendWhoIs.h"

// Virtual Terminal Pages
#include "SendVTOpen.h"
#include "SendVTOpenACK.h"
#include "SendVTClose.h"
#include "SendVTCloseError.h"
#include "SendVTData.h"
#include "SendVTDataACK.h"

//
//	CSend
//

struct CSendGroup;
const int kCSendMaxPages = 8;					// maximum number of installed pages

const DWORD kCSendPageUnused = (DWORD)-1;		// unused slot

class CSend : public CPropertySheet
{
	DECLARE_DYNAMIC(CSend)

// Construction
public:
	CSend(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSend(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	void InitPages( void );

// Attributes
public:
	CSendNull				NullPage;				// no fields, does nothing

	CSendRaw				RawPage;				// good for raw octets
	CSendTest				TestPage;				// very useful for debugging

	CSendIP					IPPage;					// IP base page
	CSendEnet				EnetPage;				// Ethernet base page
	CSendNull				ARCNETPage;				// ARCNET base page
	CSendMSTP				MSTPPage;				// MS/TP base page
	CSendNull				PTPPage;				// PTP base page
	CSendDevice				DevicePage;				// Device base page

	CSendBVLCI				BVLCIPage;				// BVLCI base page
	CSendBVLCResult			BVLCResultPage;			// BVLC-Result
	CSendWriteBDT			WriteBDTPage;			// Write-Broadcast-Distribution-Table
	CSendReadBDT			ReadBDTPage;			// Read-Broadcast-Distribution-Table
	CSendReadBDTAck			ReadBDTAckPage;			// Read-Broadcast-Distribution-Table-Ack
	CSendRegisterFD			RegisterFDPage;			// Register-Foreign-Device
	CSendReadFDT			ReadFDTPage;			// Read-Foreign-Device-Table
	CSendReadFDTAck			ReadFDTAckPage;			// Read-Foreign-Device-Table-Ack
	CSendDeleteFDTEntry		DeleteFDTEntryPage;		// Delete-Foreign-Device-Table-Entry

	CSendNPCI				NPCIPage;				// NPDU base page
	CSendVendorNPDU			VendorNPDUPage;			// vendor propietary network message
	CSendWhoIsRTN			WhoIsRTNPage;			// Who-Is-Router-To-Network
	CSendIAmRTN				IAmRTNPage;				// I-Am-Router-To-Network
	CSendICouldBeRTN		ICouldBeRTNPage;		// I-Could-Be-Router-To-Network
	CSendRejectMTN			RejectMTNPage;			// Reject-Message-To-Network
	CSendRouterBusy			RouterBusyPage;			// Router-Busy-To-Network
	CSendRouterAvailable	RouterAvailablePage;	// Router-Available-To-Network
	CSendInitRT				InitRTPage;				// Initialize-Routing-Table
	CSendInitRTAck			InitRTAckPage;			// Initialize-Routing-Table-Ack
	CSendEstablishCTN		EstablishCTNPage;		// Establish-Connection-To-Network
	CSendDisconnectCTN		DisconnectCTNPage;		// Disconnect-Connection-To-Network
	CSendNetworkNumberIs	SendNetworkNumberIsPage;	// Network-Number-Is
	CSendWhatIsNetworkNumber	SendWhatIsNetworkNumberPage;	// What-Is-Network-Number

	CSendConfirmedRequest	ConfirmedRequestPage;	// Confirmed Request base page
	CSendComplexACK			ComplexACKPage;			// Complex-ACK base page

	CSendAckAlarm			AckAlarmPage;			// AcknowledgeAlarm
	CSendConfCOVNotification ConfCOVNotificationPage;	// ConfirmedCOVNotification
	CSendUnconfCOVNotification UnconfCOVNotificationPage;	// UnconfirmedCOVNotification
	CSendGetAlarmSummary	GetAlarmSummaryPage;	// GetAlarmSummary
	CSendGetAlarmSummaryACK	GetAlarmSummaryACKPage;	// GetAlarmSummary-ACK
	CSendConfEventNotification ConfEventNotificationPage; // ConfirmedEventNotification
	CSendUnconfEventNotification UnconfEventNotificationPage; // UnconfirmedEventNotification
	CSendGetEnrollmentSummary GetEnrollmentSummaryPage; // GetEnrollmentSummary
	CSendGetEnrollmentSummaryACK GetEnrollmentSummaryACKPage; // GetEnrollmentSummary-ACK
	CSendGetEventInfo		 GetEventInformationPage;		//GetEventInformation        ,Added by Zhu Zhenhua, 2004-5-25
	CSendGetEventInfoACK	 GetEventInformationACKPage;	//GetEventInformation-ACK    ,Added by Zhu Zhenhua, 2004-5-25
	CSendSubscribeCOV		SubscribeCOVPage;		// SubscribeCOV
	CSendSubscribeCOVProperty SubscribeCOVPropertyPage;  //SubscribeCOVProperty

	CSendReadFile			ReadFilePage;			// AtomicReadFile
	CSendReadFileACK		ReadFileACKPage;		// AtomicReadFile-ACK
	CSendWriteFile			WriteFilePage;			// AtomicWriteFile
	CSendWriteFileACK		WriteFileACKPage;		// AtomicWriteFile-ACK

	CSendAddListElement		AddListElementPage;		// AddListElement
	CSendCreateObject		CreateObjectPage;		// CreateObject
	CSendCreateObjectACK	CreateObjectACKPage;	// CreateObject-ACK
	CSendCreateObjectError	CreateObjectErrorPage;	// CreateObject-Error
	CSendDeleteObject		DeleteObjectPage;		// DeleteObject
	CSendReadProp			ReadPropPage;			// ReadProperty
	CSendReadPropACK		ReadPropACKPage;		// ReadProperty-ACK
	CSendReadPropMult		ReadPropMultPage;		// ReadPropertyMultiple
	CSendReadPropMultACK	ReadPropMultACKPage;	// ReadPropertyMultiple-ACK
	SendReadRange			ReadRangePage;		    // ReadRange
	CSendRemoveListElement	RemoveListElementPage;	// RemoveListElement
	CSendWriteProp			WritePropPage;			// WriteProperty
	CSendWritePropMult		WritePropMultPage;		// WritePropertyMultiple
	CSendWritePropMultError	WritePropMultErrorPage;	// WritePropertyMultiple-Error

	CSendDeviceCommCtrl		DeviceCommCtrlPage;		// DeviceCommunicationControl
	CSendConfTextMsg		ConfTextMsgPage;		// ConfirmedTextMessage
	CSendIAm				IAmPage;				// I-Am
	CSendIHave				IHavePage;				// I-Have
	CSendReinitDevice		ReinitDevicePage;		// ReinitializeDevice
	CSendUnconfTextMsg		UnconfTextMsgPage;		// UnconfirmedTextMessage
	CSendTimeSync			TimeSyncPage;			// TimeSynchronization
	CSendUTCTimeSync	    UTCTimeSyncPage;		// UTCTimeSynchronization
	CSendWhoHas				WhoHasPage;				// Who-Has
	CSendWhoIs				WhoIsPage;				// Who-Is

	CSendVTOpen				VTOpenPage;				// VT-Open
	CSendVTOpenACK			VTOpenACKPage;			// VT-Open-ACK
	CSendVTClose			VTClosePage;			// VT-Close
	CSendVTCloseError		VTCloseErrorPage;		// VT-Close-Error
	CSendVTData				VTDataPage;				// VT-Data
	CSendVTDataACK			VTDataACKPage;			// VT-Data-ACK

	CSendSimpleACK			SimpleACKPage;			// Simple-ACK
	CSendSegmentACK			SegmentACKPage;			// Segment-ACK

	CSendError				ErrorPage;				// Error
	CSendReject				RejectPage;				// Reject
	CSendAbort				AbortPage;				// Abort
	CSendChangeListError	ChangeListErrorPage;	// ChangeList-Error

	CComboBox		m_port;						// which port to send contents
	CTreeCtrl		m_packetTree;				// tree of packet types
	CImageList		m_packetTreeImageList;		// images to use for packetTree
	CEdit			m_packetData;				// packet data in hex

	VTSPortPtr		m_pPort;					// current port
	CSendGroupList	m_groupList;				// current list of page groups
	CSendPagePtr	m_pages[kCSendMaxPages];	// list of installed pages
	HTREEITEM		m_currentItem;				// current selected item
	bool			m_isConfirmedRequest;		// true iff ConfirmedRequestPage loaded

	CButton			m_send;						// send button
	CButton		 	m_transmit_close;           // transmit and close window button  MAG 27 OCT 03
	CButton			m_close;					// just close the window, without sending MAG 15AUG05
	CComboBox       m_history;                  //choose history data. Xiao Shiyuan 2002-12-5  
	CSendPagePtr    curPagePtr;                 //current shown page. Xiao Shiyuan 2002-12-5 

// Operations
public:
	void SetHistoryComboBox(int count);
	void UpdateEncoded( void );					// update the encoded contents
	void DoSend();								// send the contents to the port

	void SetPageList( CSendPageMList lp );		// install a new list of pages
	void SetNullPageList( void );				// install just the null page

	void SetSendGroup( CSendGroupList lst );	// change to new send group

	void ChangePort( int indx );				// change the port selection
	void ChangePacketTree( int iGroup, int iItem );	// change the tree selection

	// static
	static void FindActionIndicies( LPCSTR lpszCommand, int * pIndexGroup, int * pIndexItem );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSend)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSend();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSend)
	void OnSelchangePort();
	void OnSelchangeHistory();
	void OnSelchangePacketTree( NMHDR* pNotifyStruct, LRESULT* result );
	void OnItemExpandedPacketTree( NMHDR* pNotifyStruct, LRESULT* result );
	void OnSend();
	void OnTransmitClose();
	void OnOnlyClose();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
// MAG 15AUG05 add function declaration OnOnlyClose()

/////////////////////////////////////////////////////////////////////////////

extern CSendGroup gIPGroup;
extern CSendGroupPtr gIPGroupList[];

extern CSendGroup gEthernetGroup;
extern CSendGroupPtr gEthernetGroupList[];

extern CSendGroup gARCNETGroup;
extern CSendGroupPtr gARCNETGroupList[];

extern CSendGroup gMSTPGroup;
extern CSendGroupPtr gMSTPGroupList[];

extern CSendGroup gPTPGroup;
extern CSendGroupPtr gPTPGroupList[];

extern CSendGroup gDeviceGroup;
extern CSendGroupPtr gDeviceGroupList[];

/////////////////////////////////////////////////////////////////////////////

extern CSendGroup gBVLLGroup;

extern CSendGroup gBVLLGroup;
extern CSendGroup gNetworkGroup;
extern CSendGroup gAlarmEventAccessGroup;
extern CSendGroup gFileAccessGroup;
extern CSendGroup gObjectAccessGroup;
extern CSendGroup gRemoteDevMgmtGroup;
extern CSendGroup gVirtualTerminalGroup;
extern CSendGroup gSimpleACKGroup;
extern CSendGroup gApplicationGroup;
extern CSendGroup gErrorGroup;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEND_H__8707D031_2ADB_11D4_BE9D_00A0C95A9812__INCLUDED_)
