// VTSDoc.h : interface of the VTSDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSDOC_H__BDE65088_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
#define AFX_VTSDOC_H__BDE65088_B82F_11D3_BE52_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include <afxtempl.h>

#include "BACnet.hpp"
#include "BACnetRouter.hpp"

#include "BACnetBTR.hpp"
#include "BACnetBBMD.hpp"
#include "BACnetBIPSimple.hpp"
#include "BACnetBIPForeign.hpp"

#include "WinIP.hpp"
#include "WinWinPcap.hpp"

#include "VTSPropValue.h"

#include "VTSPacketDB.h"

class VTSStatisticsCollector;
typedef VTSStatisticsCollector *VTSStatisticsCollectorPtr;

class VTSStatisticsDlg;
typedef VTSStatisticsDlg *VTSStatisticsDlgPtr;
// forward declarations

class VTSDoc;
typedef VTSDoc *VTSDocPtr;

class CFrameContext;
typedef CFrameContext *CFrameContextPtr;

class VTSPort;
typedef VTSPort *VTSPortPtr;

class VTSWinWinPcapPort;
typedef VTSWinWinPcapPort *VTSWinWinPcapPortPtr;

class VTSWinIPPort;
typedef VTSWinIPPort *VTSWinIPPortPtr;

class VTSPortDlg;
typedef VTSPortDlg *VTSPortDlgPtr;

class VTSClient;
typedef VTSClient *VTSClientPtr;

class VTSServer;
typedef VTSServer *VTSServerPtr;

class VTSDevice;
typedef VTSDevice *VTSDevicePtr;

class VTSDevicePort;
typedef VTSDevicePort *VTSDevicePortPtr;

struct CSendGroup;
typedef CSendGroup *CSendGroupPtr;
typedef CSendGroupPtr *CSendGroupList;

class ScriptNetFilter;
typedef ScriptNetFilter *ScriptNetFilterPtr;

// Version constants for storage schemas.
// See comments on VTSDoc::Serialize
#define VTS_DOC_SCHEMA_1 1    // Until October 4 2014 (last official release 3.6.2.0, July 9 2014)
#define VTS_DOC_SCHEMA_2 2    // After Until October 4 2014
                              // Adds properties to VTSDevice

#define VTS_DOC_SCHEMA   VTS_DOC_SCHEMA_2  // Current schema when storing


//
// VTSPort
//
// The VTSPort object sits between the VTSDoc and a derived class of BACnetPort (one 
// of the VTSxPort objects).  The port that it connects to depends on the contents of 
// the descriptor.
//

enum VTSPortType
   { nullPort
   , ipPort
   , ethernetPort
   , arcnetPort
   , mstpPort
   , ptpPort
   };

class VTSPort : public CObject
{
   private:
      static const char *m_pszPortTypes[6];

      bool                 m_fDirty;            // set if modifications (or new) have been made

   public:
      int                  portStatus;          // non-zero iff error
      char                 *portStatusDesc;     // status description

      VTSDocPtr            portDoc;             // doc for packets
      CSendGroupList       portSendGroup;       // send group to form packets

      BACnetPortPtr        portEndpoint;        // endpoint to get them
      ScriptNetFilterPtr   portFilter;          // way to process them in scripts

      BACnetBTRPtr         portBTR;             // BTR object in stream
      BACnetBBMDPtr        portBBMD;            // BBMD obj
      BACnetBIPSimplePtr   portBIPSimple;       // BIP Simple endpoint
      BACnetBIPForeignPtr  portBIPForeign;      // BIP Foreign
      BACnetNetServerPtr   portBindPoint;       // points to one of the above

      VTSDevicePtr         m_pdevice;           // pointer to bound device

      // Begin persistent data
      CString              m_strName;
      VTSPortType          m_nPortType;         // port type
      BOOL                 m_fEnabled;          // true iff IO should be enabled
      int                  m_nNet;              // network associated with device
      CString              m_strDevice;         // Name of bound device, fixed-up at load and save
      CString              m_strConfigParms;    // configuration string
      // End persistent data

      VTSPort( void );
      ~VTSPort( void );

      void Refresh( void );                     // reconnect to port
      void Deactivate( void );

      void SendData( BACnetOctet *data, int len ); // pass data to endpoint

      LPCSTR GetName(void) { return m_strName; }
      LPCSTR GetTypeDesc(int n) { return m_pszPortTypes[n]; }
      LPCSTR GetTypeDesc(void) { return GetTypeDesc(m_nPortType); }
      LPCSTR GetDeviceName(void) { return m_strDevice; }

      void SetName( LPCSTR lpszName );
      void SetEnabled( BOOL fEnabled = true );
      void SetConfig( LPCSTR lpszConfig );
      void SetPortType( VTSPortType nType );
      void SetNetwork( int nNetwork );
      void SetDevice( VTSDevice * pdevice );

      bool IsDirty() { return m_fDirty; }
      bool IsEnabled() { return m_fEnabled == TRUE; }
      void SetDirty( bool fDirty = true ) { m_fDirty = fDirty; }
      void BindDevice(void);
      void UnbindDevice(void);

      const VTSPort& operator=(const VTSPort& rportSrc);
      void Serialize( CArchive& archive, UINT version );

      DECLARE_SERIAL(VTSPort)
};


DECLARE_VTSPTRARRAY(VTSPorts, VTSPort)


typedef VTSPort *VTSPortPtr;

class VTSName : public CObject                     // serializable name element
{
   private:

   public:

      // Begin persistent data
      CString        m_strName;
      BACnetAddress  m_bacnetaddr;
      CString        m_strPortNameTemp;
      // End persistend data

      VTSPort *      m_pportLink;

   public:
      VTSName( void );
      VTSName( LPCSTR pszname );
      virtual ~VTSName( void );

      const VTSName& operator=(const VTSName& rnameSrc);

      void Serialize( CArchive& archive, UINT version );
      LPCSTR GetPortName(void) { return m_strPortNameTemp; }
      LPCSTR GetName(void) { return m_strName; }
      bool IsAddressMatch( const BACnetAddress &addr, VTSPort * pport );

      DECLARE_SERIAL(VTSName)
};


class VTSNames : public CTypedPtrArray<CPtrArray, VTSName *>
{
      DECLARE_VTSPTRARRAY_GUTS(VTSNames, VTSName)

   public:
      void Remove( int i );                     // remove a name
      int FindIndex( LPCSTR lpszName );
      void AddUniqueName( VTSName *pName );

      LPCSTR AddrToName( const BACnetAddress &addr, VTSPort * pport );
};

// Class to hold information parsed from a VTSPacket
// Used by filtering, and by segmented message assembly
class VTSFilterInfo
{
public:
   VTSFilterInfo();
   bool Parse( const VTSPacket &packet );

   bool           m_hasSegment;
   bool           m_moreFollows;
   int            m_fnGroup;
   int            m_cursor;
   int            m_length;
   BACnetAddress  m_srcAddr;
   BACnetAddress  m_destAddr;
   int            m_pduType;
   int            m_sequence;
   int            m_service;
   int            m_invokeID;
};

// Class to hold information about a series of VTSPacket segments
// Used to accumulate an entire message
class VTSSegmentAccumulator
{
public:
   VTSSegmentAccumulator( const VTSPacket &packet, VTSFilterInfo &theInfo );

   // Return true if the packet matches this accumulator
   bool IsMatch( const VTSPacket &packet, VTSFilterInfo &theInfo ) const;

   // Add the packet to the accumulator
   // Return true if the final segment has been added
   bool AddPacket( const VTSPacket &packet, VTSFilterInfo &theInfo );

   DWORD          m_startTime;
   VTSPacket      m_packet;
   BACnetAddress  m_srcAddr;
   BACnetAddress  m_destAddr;
   int            m_pduType;
   int            m_sequence;
   int            m_service;
   int            m_invokeID;
};

class VTSFilter : public CObject                   // serializable filter element
{
   private:

   public:

      // Begin persistent data
      int            m_type;              // 1=accept, 0=reject
      CString        m_strPortNameTemp;
      int            m_addr;              // none, source, destination, bidirectional
      int            m_addrType;          // LS, LN, LB, RS, RN, RB, GB
      BACnetAddress  m_filteraddr;        // address filter bits
      int            m_fnGroup;           // function group
      // End persistend data

      VTSPort *      m_pportLink;

   public:
      VTSFilter( void );
      virtual ~VTSFilter( void );

      const VTSFilter& operator=(const VTSFilter& rfilterSrc);

      void Serialize( CArchive& archive, UINT version );
      LPCSTR GetPortName(void) { return m_strPortNameTemp; }

      bool TestAddress( const BACnetAddress &addr );

      DECLARE_SERIAL(VTSFilter)
};


class VTSFilters : public CTypedPtrArray<CPtrArray, VTSFilter *>
{
   public:
      VTSFilters();
      virtual ~VTSFilters( void );

      void DeepCopy( const VTSFilters * psrc );
      void KillContents( void );
      void Serialize( CArchive& archive, UINT version );

      void Remove( int i );                     // remove a filter

      bool TestPacket( const VTSPacket& packet );
      bool TestPacket( const VTSPacket& packet, VTSFilterInfo &theInfo );
      static int ConfirmedServiceFnGroup( int service );
      static int UnconfirmedServiceFnGroup( int service );

      DECLARE_SERIAL(VTSFilters)
};




namespace NetworkSniffer {

void SetLookupContext( const char * pszPortName );
const char* LookupName( int net, const BACnetOctet *addr, int len );
}

//
// VTSClient
//

class VTSClient : public BACnetClient {
   public:
      VTSDevicePtr      clientDev;

      VTSClient( VTSDevicePtr dp );

      void Confirmation( const BACnetAPDU &apdu );

      void IAm( void ); // initiate a global-broadcast I-Am
   };

typedef VTSClient *VTSClientPtr;

//
// VTSServer
//

class VTSServer : public BACnetServer {
   public:
      VTSDevicePtr      serverDev;

      VTSServer( VTSDevicePtr dp );

      void Indication( const BACnetAPDU &apdu );
      void Response( const BACnetAPDU &pdu );

      void WhoIs( const BACnetAPDU &apdu );
      void IAm( const BACnetAPDU &apdu );

      void ReadProperty( const BACnetAPDU &apdu );
      void WriteProperty( const BACnetAPDU &apdu );
      void CovNotification( const BACnetAPDU &apdu );

      void GetAlarmSummary( const BACnetAPDU &apdu );
      void GetEventInformation( const BACnetAPDU &apdu );
      void DeviceCommunicationControl( const BACnetAPDU &apdu );
      void AcknowledgeAlarm( const BACnetAPDU &apdu );
      void EventNotification( const BACnetAPDU &apdu );
      void ReinitializeDevice( const BACnetAPDU &apdu );
   };

typedef VTSServer *VTSServerPtr;

//
// VTSDevice
//
// The VTSDevice object wraps around a BACnetDevice, BACnetRouter, VTSServer and 
// VTSClient.
//

class VTSDevice : public CObject
{
      friend class VTSClient;
      friend class VTSServer;

   protected:
      BACnetDevice      devDevice;
      BACnetRouter      devRouter;
      
      VTSClient         devClient;
      VTSServer         devServer;

      VTSPortPtr        devPort;
      VTSDevicePortPtr  devPortEndpoint;

   public:
      VTSDocPtr         devDoc;                 // doc for packets

      // Begin persistent data
      CString     m_strName;                    // name
      int         m_nInstance;                  // instance number
      BOOL        m_fRouter;                    // true iff device should act like a router
      BACnetSegmentation   m_segmentation;      // supports segments requests
      int         m_nSegmentSize;               // how to divide up chunks
      int         m_nWindowSize;                // how many to send
      int         m_nMaxAPDUSize;               // maximum APDU size
      int         m_nNextInvokeID;              // next invoke ID for client
      int         m_nAPDUTimeout;               // how long to wait for ack
      int         m_nAPDUSegmentTimeout;        // how long to wait between segments
      int         m_nAPDURetries;               // how many retries are acceptable
      int         m_nVendorID;                  // which vendor is this?
      // End of VTS_DOC_SCHEMA_1 data

      // VTS_DOC_SCHEMA_2 data
      BACnetBitString   m_services_supported;
      int               m_nEvents;
      int               m_nMaxSegs;             // Maximum number of segments accepted
      // End of VTS_DOC_SCHEMA_2 data

      VTSDevObjects m_devobjects;

      // End persistent data

      BACnetBitString   m_objects_supported;

      VTSDevice( void );
      ~VTSDevice( void );

      void Bind( VTSPortPtr pp, int net );      // associate with a port and network
      void Unbind( VTSPortPtr pp );             // disassociate

      void SendAPDU( const BACnetAPDU &apdu );  // message from a script

      void IAm( void );                         // ask the client to send out an I-Am
      void Activate( void );
      void Deactivate( void );

      void CopyToLive();                        // copy data to live device

      LPCSTR GetName(void) { return m_strName; }
      CString GetDescription(void);
      VTSDevObjects * GetObjects(void) { return &m_devobjects; }

      int ReadProperty( BACnetObjectIdentifier * pbacnetobjectid, BACnetEnumerated * pbacnetpropid, int nIndex, BACnetAPDUEncoder * pAPDUEncoder );
      int InternalReadProperty( BACnetObjectIdentifier * pbacnetobjectid, BACnetEnumerated * pbacnetpropid, BACnetAPDUEncoder * pAPDUEncoder, int index );
      int WriteProperty( BACnetObjectIdentifier * pbacnetobjectid, BACnetEnumerated * pbacnetpropid, int nIndex, BACnetAPDUDecoder * pdec );
      int InternalWriteProperty( BACnetObjectIdentifier * pbacnetobjectid, BACnetEnumerated * pbacnetpropid, BACnetAPDUDecoder * pdec );

      VTSDevObject * FindObject( unsigned int nObjID );
      static VTSDevProperty * FindProperty( VTSDevObject * pobject, int nPropID );

      const VTSDevice& operator=(const VTSDevice& rdeviceSrc);
      void Serialize( CArchive& archive, UINT version );

      DECLARE_SERIAL(VTSDevice)
   };

typedef VTSDevice *VTSDevicePtr;


DECLARE_VTSPTRARRAY(VTSDevices, VTSDevice)



//
// VTSDoc
//

class CSend;

class VTSDoc : public CDocument
{
private:
   int      m_n1stPktFilePos;          // holds file position of first packet in list
                                       // useful for backward scrolling virtual window
   int      m_nPacketCount;            // Current packets in document... may be different from
                                       // size of packet array

   int      m_fLoadPackets;            // false if max packet load has been reached

   // ======== Persistent data start
   CString           m_strLogFilename;
   VTSNames          m_names;
   VTSDevices        m_devices;        // list of persistent devices defined for this doc
   VTSPorts          m_ports;          // list of persistent ports
   VTSFilters        m_captureFilters; // list of capture filters
   VTSFilters        m_displayFilters; // list of display filters
   // ======== Persistent data end (preserve order)

   CTypedPtrArray <CPtrArray, VTSPacketPtr> m_apPackets;    // array of ptrs to packets

   void DestroyPacketArray(void);
   BOOL DeletePacket(int id); //added: 2004/12/02 author:Xiao Shiyuan   purpose: delete selected packet
   int LoadPacketArray( void );
   void ScheduleForProcessing(void);
   LPCSTR StripToPath( CString * pstr );

public: // create from serialization only
   VTSDoc();
   virtual ~VTSDoc();

   DECLARE_DYNCREATE(VTSDoc)

// Attributes
public:
   enum Signal
      { eInitialUpdate = 0
      , eNewFrameCount = 1
      };

   static LPCSTR m_pszDefaultFilename;

   VTSPacketDB          m_PacketDB;
   bool              m_postMessages;      // OK to post messages about new packets
   
   VTSPortDlgPtr  m_pPortDlg;
   VTSStatisticsDlgPtr m_pStatitiscsDlg;// a pointer to the statistics dialog
   bool m_bStatisticsDlgInUse;// indicates whether the statistics dialog is shown
    VTSStatisticsCollectorPtr m_pStatisticsCollector;

// Operations
public:

   void BindFrameContext( CFrameContext *pfc );
   void UnbindFrameContext( CFrameContext *pfc );

   void SetPacketCount( int count );

   int WritePacket( VTSPacket & pkt );       // MAD_DB
   int GetPacketCount();
   VTSPacketPtr GetPacket( int nIndex );

   void ProcessPacketStoreChange(void);
   LPCSTR AddrToName( const BACnetAddress &addr, const char * pszPortName = NULL );
   void DefineNamesForPort( VTSPort * pport, BACnetPort * pBACnetPort );

   bool LoadNamedAddress( BACnetAddress * pbacnetaddr, LPCSTR lpszNameLookup );
   void ReActivateAllPorts(void);
   void FixupPortToDeviceLinks( bool fCheckForExistingLink = true );
   void FixupNameToPortLinks( bool fCheckForExistingLink = true );
   void FixupFiltersToPortLinks( bool fCheckForExistingLink = true );
   void BindPortsToDevices(void);
   void UnbindPortsToDevices(void);
   void ChangeCWDToWorkspace( LPCSTR lpszWksFile = NULL );
   void SetNewCacheSize(void);
   void CacheHint(int nFrom, int nTo);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(VTSDoc)
   public:
   virtual void Serialize(CArchive& ar);
   virtual BOOL OnNewDocument();
   virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
   virtual void OnCloseDocument();
   //}}AFX_VIRTUAL

// Implementation
public:
   void DeleteSelPacket(int index); // 2/25/05 Shiyuan Xiao. Delete selected packet.
   void DeletePackets( void );
   void DoPortsDialog( void );
   void PortStatusChange( void );
   void DoNamesDialog( void );
   void DoCaptureFiltersDialog( void );
   void DoDisplayFiltersDialog( void );
   void DoDevicesDialog( void );
   bool DoPacketFileNameDialog( bool fReload = true );
   void SaveConfiguration(void);
   void ReloadPacketStore(void);

   // madanner 9/04, changed to return type from void
   CSend * DoSendWindow( int iGroup, int iItem );

   VTSDevices * GetDevices( void ) { return &m_devices; }
   VTSPorts * GetPorts( void ) { return &m_ports; }
   VTSNames * GetNames(void) { return &m_names; }
   VTSFilters * GetCaptureFilters(void) { return &m_captureFilters; }
   VTSFilters * GetDisplayFilters(void) { return &m_displayFilters; }

//MAD_DB void NewPacketCount(void);

   virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
   void ConvertPathToRelative( CString * pstr );

   void DoBackupRestore(void);
   void DoInconsistentPars(void);
   
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   // Process the packet as a possible message segment
   void ProcessMessageSegment( const VTSPacket &pkt, VTSFilterInfo &theInfo );

   // Segmentation data
   VTSSegmentAccumulator   *m_pSegmentAccumulator[ 10 ];

   CCriticalSection     m_FrameContextsCS;
   CFrameContextPtr     m_FrameContexts;

// Generated message map functions
protected:
   //{{AFX_MSG(VTSDoc)
   afx_msg void OnViewStatistics();
   afx_msg void OnEditQuickSave();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

typedef VTSDoc *VTSDocPtr;

//
// VTSWinWinPcapPort
//

class VTSWinWinPcapPort : public WinWinPcap {
   protected:
      VTSPortPtr        m_pPort;

   public:
      VTSWinWinPcapPort( VTSPortPtr pp );
      virtual ~VTSWinWinPcapPort( void );

      void FilterData( BACnetOctet *, int len, BACnetPortDirection dir );

      void PortStatusChange( void );
   };

//
// VTSWinIPPort
//

class VTSWinIPPort : public WinIP {
   protected:
      VTSPortPtr        m_pPort;

   public:
      VTSWinIPPort( VTSPortPtr pp );
      virtual ~VTSWinIPPort( void );

      void FilterData( BACnetOctet *, int len, BACnetPortDirection dir );

      void PortStatusChange( void );
   };

//
// VTSDevicePort
//

class VTSDevicePort : public BACnetPort {
   protected:
      VTSPortPtr        m_pPort;
      VTSDevicePtr      m_pDevice;

   public:
      VTSDevicePort( VTSPortPtr pp, VTSDevicePtr dp );
      virtual ~VTSDevicePort( void );

      void Indication( const BACnetNPDU &pdu );
      void SendData( BACnetOctet *data, int len );    // raw data request
   };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDOC_H__BDE65088_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
