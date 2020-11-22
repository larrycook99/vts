// SendReceiveExecutor.h: BACnetTask with send and receive capabilities
//
// By John Hartman, May 2014
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEND_RECEIVE_EXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_)
#define AFX_SEND_RECEIVE_EXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include "bacnet.hpp"
#include "vtsdoc.h"
#include "SendReceiveExecutorProgressDlg.h"

class VTSDoc;
typedef VTSDoc * VTSDocPtr;
struct BACnetNPDU;

// TODO: yes, we SHOULD have a full set of error enumeration values in some .h file.
// Go right ahead.  I'll wait here
#define ERR_CLASS_PROPERTY           2
#define ERR_CODE_READ_ACCESS_DENIED  27
#define ERR_CODE_UNKNOWN_PROPERTY    32
#define ERR_CODE_WRITE_ACCESS_DENIED 40


// This class is a fairly crude refactoring of BakRestoreExecutor and InconsistentParsExecutor,
// to share the common portions.  C++ offers better options than clipboard inheritance...
class SendReceiveExecutor : public BACnetTask
{
public:
   // Subclass BACnetAnyValue to prevent automatic deletion of contents
   class AnyValue : public BACnetAnyValue
   {
   public:
      AnyValue();
      virtual ~AnyValue();
      virtual void SetObject(BACnetEncodeable * pbacnetEncodeable);
   };

   // Create a new data type to be used in CreateObject Service
   // do not support sequence-of, array-of property, etc.
   class PropertyValue
   {
   public:
      BACnetEnumerated m_propID;
      AnyValue m_propValue;
      
      PropertyValue(const BACnetEnumerated& propID, BACnetEncodeable& propValue);
      PropertyValue();
      PropertyValue(const PropertyValue& propValue);
      virtual ~PropertyValue() ;

      void Encode(BACnetAPDUEncoder &enc);
      void Decode(BACnetAPDUDecoder &dec);
   };

   enum
   {
      DEVICE_OBJ_INST_NUM_UNSET = 4194304,
      MAX_NPDU_LENGTH = 1497
   };

   enum ExecutorState
   { 
      execIdle,
      execRunning, 
      execStopped
   };

   enum ReinitializedStateOfDevice
   {
      COLDSTART      = 0,
      WARMSTART      = 1,
      STARTBACKUP    = 2,
      ENDBACKUP      = 3,
      STARTRESTORE   = 4,
      ENDRESTORE     = 5,
      ABORTRESTORE   = 6
   };

   SendReceiveExecutor( const char *pTheTitle );
   virtual ~SendReceiveExecutor();

   bool IsRunning() const { return m_execState == execRunning; }
   void ReceiveNPDU(const BACnetNPDU& npdu);
   void DestroyOutputDlg();
   void Kill();

protected:
   void StartTest();
   void Delay( UINT delaySec );
   bool SendExpectPacket(CByteArray& contents, bool resp = true);
   void SendUnconfirmed(CByteArray& contents, bool isBroadcast);
   void InsertMaxAPDULenAccepted(CByteArray& contents);
   void FindRouterAddress(void);
   void Msg(const char* errMsg);
   BYTE InvokeID();

   bool SendExpectReadProperty( BACnetObjectIdentifier   &objID,
                                BACnetEnumerated         &propID,
                                BACnetAnyValue           &propValue,
                                int                      propIndex = -1 );
   bool SendReadPropertyOptional( BACnetObjectIdentifier   &objID,
                                  BACnetEnumerated         &propID,
                                  BACnetAnyValue           &propValue,
                                  int                      propIndex = -1 );
   bool SendExpectWriteProperty( BACnetObjectIdentifier  &objID,
                                 BACnetEnumerated        &propID,
                                 BACnetAnyValue          &propValue,
                                 int                     propIndex = -1 );
   bool SendWritePropertyOptional( BACnetObjectIdentifier  &objID,
                                   BACnetEnumerated        &propID,
                                   BACnetAnyValue          &propValue,
                                   int                     propIndex = -1 );
   bool SendExpectWhoIs( BACnetObjectIdentifier          &iAmDeviceID,
                         BACnetUnsigned                  &maxAPDULenAccepted );
   bool SendExpectReinitialize( ReinitializedStateOfDevice nReinitState, const char *pPassword );

   void GetDeviceMaxAPDU();
   void GetObjectList( BACnetArrayOf<BACnetObjectIdentifier> &objList );
   void ErrorString( CString &theErrorString ) const;

   virtual void ReceiveAPDU( const BACnetAPDU &theAPDU );

   CString        m_title;
   SendReceiveExecutorProgressDlg *m_pOutputDlg;
   ExecutorState  m_execState;
   VTSPort        *m_pPort;
   VTSName        *m_pName;
   BACnetAddress  m_routerAddr;     // Router's MAC address
   BACnetAddress  m_IUTAddr;        // the IUT's address
   UINT           m_nDeviceObjInst;
   UINT           m_maxAPDULen;
   BACnetAPDU     *m_pAPDU;
   BYTE           m_invokeID;
   UINT           m_lastNetwork;    // network number of the last discovered network

   CCriticalSection m_cs;
   CEvent         m_event;             // is uesed by the main thread that the packet has been correctly received
   bool           m_bAbort;            // indicate the reception of "Result(-)"
   bool           m_bUserCancelled;
   BACnetOctet    *m_packetData;       // buffer used to hold the received packet

   bool           m_bExpectPacket;     // tell the main thread to receive packet and write it to the buffer
   bool           m_bExpectAPDU;       // if expect a APDU or not
   BACnetAPDUType m_nExpectAPDUType;   // expected APDU type if expect a APDU
   int            m_nExpectAPDUServiceChoice; // expected Service Choice if expect a APDU
   int            m_errorClass;        // gets error-class if an Error is returned
   int            m_errorCode;         // gets error-code if an Error is returned
};

#endif // !defined(AFX_SEND_RECEIVE_EXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_)
