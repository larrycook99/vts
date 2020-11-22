// DiscoveryExecutor.h  - Device and object discovery and EPICS generation
// John Hartman 1 August 2014
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISCOVERY_SEXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_)
#define AFX_DISCOVERY_SEXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include "bacnet.hpp"
#include "vtsdoc.h"
#include "SendReceiveExecutor.h"

class VTSDoc;
typedef VTSDoc * VTSDocPtr;
struct BACnetNPDU;
class VTSInconsistentParsProgressDlg;

class DiscoveryExecutor : public SendReceiveExecutor
{
public:
   class DiscoAnyValue : public BACnetAnyValue
   {
   public:
      DiscoAnyValue( UINT a_parseType, UINT a_propertyID );
      virtual ~DiscoAnyValue();
      virtual void Decode( BACnetAPDUDecoder& dec );
   protected:
      UINT     m_parseType;
      UINT     m_propertyID;
   };

   DiscoveryExecutor();
   virtual ~DiscoveryExecutor();
   virtual void ProcessTask();

   enum FunctionToExecute
   {
      FN_IDLE,
      FN_DISCOVER_DEVICES,
      FN_CREATE_EPICS
   };

   void ExecuteTest( FunctionToExecute theFunction );

protected:
   void FindDeviceInstance();
   bool ReadObjects();
   bool ReadProtocolRevision();
   bool ReadObjectProperties( BACnetObjectIdentifier &objID );
   void ShowPropertyError( const char *pTitle, const char *pPropName );
   void OpenEpicsFile();
   virtual void ReceiveAPDU( const BACnetAPDU &theAPDU );

   void DoDiscovery();
   void ClearDiscoveryList();
   void SendWhoIs( int theLowInstance, int theHightInstance );
   int  InstanceAfter( int theInstance );
   void DumpDeviceProperties();
   void Show( const char *pTheString ) const;
   int* DumpPropertyList( const BACnetGenericArray &thePropList ) const;
   void DumpSpecialProperties( const BACnetObjectIdentifier &objID,
                               const int                    *pTheProperties ) const;
   void DumpBitString( const char                              *pPropName,
                       const BACnetBitString                   &theBitString,
                       const NetworkSniffer::BACnetStringTable &theStringTable ) const;

   FunctionToExecute    m_funToExe;
   CString              m_fileName;
   FILE                 *m_pFile;
   UINT                 m_protocolRevision;
   bool                 m_includeAllValues;
   bool                 m_includeUnsupported;
   bool                 m_writeBack;

   // Information about a discovered device
   struct DiscoveryInfo
   {
      UINT              m_instance;
      BACnetAddress     m_address;
      UINT              m_mxaAPDU;
      UINT              m_segmentationSupport;
      UINT              m_vendorID;
   };

   // Array of discovered devices
   CArray<DiscoveryInfo*,DiscoveryInfo*> m_discoveredDevices;
};

#endif // !defined(AFX_DISCOVERY_SEXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_)
