// SendReceiveExecutor.cpp: BACnetTask with send and receive capabilities
//
// By John Hartman, May 2014
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSDoc.h"
#include "BACnet.hpp"
#include "ScriptExecutor.h"
#include "SendReceiveExecutor.h"
#include "PI.h"

namespace PICS {
#include "db.h"
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"
}

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SendReceiveExecutor::AnyValue::AnyValue()
{
}

SendReceiveExecutor::AnyValue::~AnyValue()
{
   pbacnetTypedValue = NULL;
}

SendReceiveExecutor::PropertyValue::PropertyValue()
{
}

SendReceiveExecutor::PropertyValue::PropertyValue(const BACnetEnumerated& propID, BACnetEncodeable& propValue)
{
   m_propID.m_enumValue = propID.m_enumValue;
   m_propValue.SetObject(&propValue);
}

SendReceiveExecutor::PropertyValue::PropertyValue(const PropertyValue& value)
{
   m_propID.m_enumValue = value.m_propID.m_enumValue;
   PropertyValue* p = const_cast<PropertyValue*>(&value);
   m_propValue.SetObject(p->m_propValue.GetObject());
}

SendReceiveExecutor::PropertyValue::~PropertyValue()
{
}

SendReceiveExecutor::SendReceiveExecutor( const char *pTheTitle )
: m_title(pTheTitle),
  m_pOutputDlg(NULL),
  m_execState(execIdle),
  m_pPort(NULL),
  m_pName(NULL),
  m_routerAddr(),
  m_IUTAddr(),
  m_nDeviceObjInst(0),
  m_maxAPDULen(0),
  m_pAPDU(NULL),
  m_invokeID(0),
  m_lastNetwork(~0),
  m_cs(),
  m_event(),
  m_bAbort(false),
  m_bUserCancelled(false),
  m_packetData(NULL),
  m_bExpectPacket(false),
  m_bExpectAPDU(true),
  m_nExpectAPDUType(simpleAckPDU),
  m_nExpectAPDUServiceChoice(0),
  m_errorClass(0),
  m_errorCode(0)
{
}

SendReceiveExecutor::~SendReceiveExecutor()
{
   delete m_pAPDU;
   delete[] m_packetData;
}

// This function returns a new invokeID for each call.
BYTE SendReceiveExecutor::InvokeID()
{
   // pre-increment means that AFTER a call, you can compare
   // response invokeID value to m_invokeID
   return ++m_invokeID;
}

void SendReceiveExecutor::Kill()
{
   // verify the executor state
   if (m_execState != execRunning)
   {
      TRACE0( "Error: invalid executor state\n" );
      return;
   }

   m_bUserCancelled = true;
   m_event.SetEvent();
}

// Start the test
void SendReceiveExecutor::StartTest()
{
   // Make a progress dialog
   if (m_pOutputDlg != NULL)
   {
      m_pOutputDlg->SetFocus();
   }
   else
   {
      m_pOutputDlg = new SendReceiveExecutorProgressDlg( this, m_title );
      m_pOutputDlg->Create(IDD_SEND_RECEIVE_PROGRESS);
      m_pOutputDlg->ShowWindow(SW_SHOW);
   }

   // install the task
   InstallTask( oneShotTask, 0 );
}

// Forget the progress dialog
void SendReceiveExecutor::DestroyOutputDlg()
{
   m_pOutputDlg = NULL;
}

// Wait the specified number of seconds
void SendReceiveExecutor::Delay( UINT delaySec )
{
   if (delaySec != 0)
   {
      CString str;
      str.Format("Waiting %d seconds", delaySec);
      m_pOutputDlg->OutMessage((LPCTSTR)str);

      Sleep(delaySec * 1000);
   }
}

void SendReceiveExecutor::ReceiveNPDU(const BACnetNPDU& npdu)
{
   // if were not running, just toss the message
   if (m_execState != execRunning || !m_bExpectPacket)
      return;

   // make a copy of the data
   BACnetOctet packetData[MAX_NPDU_LENGTH];
   memcpy( packetData, npdu.pduData, npdu.pduLen );

   // Initially, assume source is who sent the packet
   BACnetAddress sourceAddr;
   sourceAddr = npdu.pduAddr;

   // the rest of this code will need a decoder
   BACnetAPDUDecoder dec( packetData, npdu.pduLen );
   // Decode the packet
   if (0x81 == *dec.pktBuffer)
   {
      (dec.pktLength--,*dec.pktBuffer++); // fix by Trevor from Delta 1/30/2008
      // decode BVLCI
      int nBVLCfunc = (dec.pktLength--,*dec.pktBuffer++);
      if ((nBVLCfunc != 4) && (nBVLCfunc != 9)
         && (nBVLCfunc != 10) && (nBVLCfunc != 11))
         return;     // does not contain NPDU, it is obvious that it isn't the packet which we want
      // verify the length
      int len;
      len = (dec.pktLength--,*dec.pktBuffer++);
      len = (len << 8) + (dec.pktLength--,*dec.pktBuffer++);
      if (len != npdu.pduLen)
         return;     // "BVLCI length incorrect";
      if (nBVLCfunc == 4)
      {
         // Forwarded NPDU.  Remember the actual source of the packet
         sourceAddr.LocalStation( dec.pktBuffer, 6 );
         dec.pktLength -= 6;
         dec.pktBuffer += 6;
      }
   }

   (dec.pktLength--, dec.pktBuffer++);    // version
   BACnetOctet ctrl =  (dec.pktLength--, *dec.pktBuffer++);
   if (m_bExpectAPDU && (ctrl & 0x80))
   {
      return;     // Not an application message and APDU is expect
   }

   if ((ctrl & 0x40) || (ctrl & 0x10))
      return;                 // 6th and 4th bit should be 0

   if (ctrl & 0x20)           // has DNET
   {
      // We (as a SendReceiveExecutor) aren't a router.
      // Thus we look only at
      // - packets without DNET/DADR (per clause 6.5.2)
      // - global broadcasts (per clause 6.5.4)
      UINT dnet = (dec.pktBuffer[0] << 8) | dec.pktBuffer[1];
      dec.pktLength -= 2;     // DNET
      dec.pktBuffer += 2;
      int len = (dec.pktLength--, *dec.pktBuffer++);  // DLEN
      if ((dnet != 65535) || (len != 0))
         return;              // Not for us
      dec.pktLength -= len;   // DADR
      dec.pktBuffer += len;
   }

   if (ctrl & 0x08)
   {
      // Parse out SNET and SADR, source of this message
      // We also know that npdu.pduAddr is a router to SNET
      UINT snet = (dec.pktBuffer[0] << 8) | dec.pktBuffer[1];
      dec.pktLength -= 2;
      dec.pktBuffer += 2;

      int len = (dec.pktLength--, *dec.pktBuffer++);  // SNET
      if (len == 0)
         return;              // No SADR in packet
      sourceAddr.RemoteStation( snet, dec.pktBuffer, len );
      dec.pktLength -= len;   // SADR
      dec.pktBuffer += len;
   }

   if (ctrl & 0x20)
   {
      (dec.pktLength--, dec.pktBuffer++);    // hop count
   }

   if (!m_bExpectAPDU && (ctrl & 0x80))      // Expect NPDU
   {
      // We expect only two kinds of network messages:
      // I-Am-Router-To-Network and I-Could-Be-Router-To-Network
      if (0x01 == *dec.pktBuffer || 0x02 == *dec.pktBuffer)
      {
         m_routerAddr = npdu.pduAddr;
         m_event.SetEvent();
      }
      return;     // It's OK
   }

   BACnetAPDU apdu;
   apdu.Decode(dec);

   // Is this a segmentAck for the pending request?
   if (apdu.apduType == segmentAckPDU)
   {
      // Check for a NAK
      if (apdu.apduNak)
      {
         m_bAbort = true;
      }

      // Only set the event if this the packet we were waiting for
      // (Wait for a complexAck instead of a segmentAck for the last segment)
      if ((m_nExpectAPDUType == segmentAckPDU) && (apdu.apduInvokeID == m_invokeID))
      {
         m_event.SetEvent();
      }
      return;
   }

   if ((apdu.apduType == errorPDU) && (apdu.apduInvokeID == m_invokeID) &&
       (apdu.apduService == m_nExpectAPDUServiceChoice) && (m_nExpectAPDUType != errorPDU))
   {
      m_bAbort = true;     // received a Result(-) response from the IUT, terminate the test

      // Parse out at least error class and error choice, so that clients can act on them.
      // Depending on the service, we have either
      //   error-class ENUMERATED
      //   error-code ENUMERATED
      // or the above wrapped in [0]

      // Bypass the fixed header
      dec.pktBuffer += 3;
      dec.pktLength -= 3;

      BACnetAPDUTag tag;
      dec.ExamineTag(tag);
      if ((tag.tagClass == openingTagClass) && (tag.tagNumber == 0))
      {
         // Wrapped Error.  Eat the opening wrapper
         tag.Decode( dec );
      }

      BACnetEnumerated val;
      val.Decode(dec);
      m_errorClass = val.m_enumValue;
      val.Decode(dec);
      m_errorCode = val.m_enumValue;

      m_event.SetEvent();
      return;
   }

   if ((apdu.apduType != m_nExpectAPDUType) || (apdu.apduService != m_nExpectAPDUServiceChoice))
   {
      return;        // according to the pdu type and service choice, this is not the packet which we expect
   }

   if ((apdu.apduType == simpleAckPDU) && (apdu.apduInvokeID == m_invokeID))
   {
      m_event.SetEvent();
      return;        // it's OK.
   }

   // Set the source address, in case ReceiveAPDU wants it (for I-Am etc)
   apdu.apduAddr = sourceAddr;

   // Process the APDU
   ReceiveAPDU( apdu );
}

// virtual
// Process an APDU that matches our requested APDU type
// Default assumes a single reply, which is passed via m_pADPU and m_event.
void SendReceiveExecutor::ReceiveAPDU( const BACnetAPDU &theAPDU )
{
   delete m_pAPDU;
   m_pAPDU = new BACnetAPDU(theAPDU);
   if (theAPDU.pktLength > 0)
   {
      delete []m_packetData;
      m_packetData = new BACnetOctet[theAPDU.pktLength];
      memcpy(m_packetData, theAPDU.pktBuffer, theAPDU.pktLength);
      m_pAPDU->pktBuffer = m_packetData;
   }

   // trigger the event
   m_event.SetEvent();
}

bool SendReceiveExecutor::SendExpectPacket(CByteArray& contents, bool resp/* = true*/)
{
   // network layer
   CByteArray buf;
   buf.Add((BYTE)0x01);      // version
   if (m_IUTAddr.addrType == remoteStationAddr)
   {  // if the IUT is on the remote network
      BYTE control = 0;
      // TODO: DER should only be set for ConfirmedRequest and segments
      control |= 0x24;                    // Set control to include DNET, DLEN, DADR, Hop and DER
      buf.Add(control);
      buf.Add((m_IUTAddr.addrNet & 0xFF00) >> 8);  // DNET
      buf.Add(m_IUTAddr.addrNet & 0x00FF);
      buf.Add((BYTE)m_IUTAddr.addrLen);            // DLEN

      for (int index = 0; index < m_IUTAddr.addrLen; index++)
      {
         buf.Add(m_IUTAddr.addrAddr[index]);       // DADR
      }

      buf.Add((BYTE)0xFF);       // hop count
   }
   else
   {
      // TODO: DER should only be set for ConfirmedRequest and segments
      if(resp)
         buf.Add((BYTE)0x04);    // control expect resp
      else 
         buf.Add((BYTE)0x00);    //control no resp
   }

   contents.InsertAt(0, &buf);
   if (m_pPort->m_nPortType == ipPort)
   {
      int len = contents.GetSize() + 4;       // the size of npdu plus BVLC
      contents.InsertAt(0, (BYTE)(len & 0xFF));
      contents.InsertAt(0, (BYTE)(len >> 8));
      contents.InsertAt(0, (BYTE)0x0A);      // Original Unicast
      contents.InsertAt(0, (BYTE)0x81);
   }

   VTSDevicePtr pdevice = m_pPort->m_pdevice;
   int nNumOfAPDURetries;
   int nAPDUTimeOut;
   if (pdevice != NULL)
   {
      nNumOfAPDURetries = pdevice->m_nAPDURetries;
      nAPDUTimeOut = pdevice->m_nAPDUTimeout;
   }
   else
   {
      nNumOfAPDURetries = 3;
      nAPDUTimeOut = 10000;   // millisecond
   }

   BACnetAddress  addr;
   if (m_IUTAddr.addrType == remoteStationAddr)
   {
      addr = m_routerAddr;
   }
   else
   {
      addr = m_IUTAddr;
   }
   // Before sending a packet, modify the signal bit to notify the main thread to
   // prepare accept packet
   m_bExpectAPDU = true;
   m_bExpectPacket = true;
   m_errorClass = -1;
   m_errorCode = -1;

   bool bReceived = false;
   for (int i = 0; i < nNumOfAPDURetries; i++)
   {
      // send packet
      m_pPort->portFilter->Indication(
         BACnetNPDU(addr, contents.GetData(), contents.GetSize())
         );

      if (!resp)
      {
         //Not expecting a response. Leave
         bReceived = true;
         break;
      }

      CString str;
      if (m_bUserCancelled)
      {
         m_bUserCancelled = FALSE;  // reset
         str.Format("The %s process has been cancelled by the user", (LPCTSTR)m_title);
         throw((LPCTSTR)str);
      }
      if (m_event.Lock(nAPDUTimeOut))
      {
         if (m_bUserCancelled)
         {
            str.Format("The %s process has been cancelled by the user", (LPCTSTR)m_title);
            throw((LPCTSTR)str);
         }

         bReceived = true;
         if (m_bAbort)
         {
            m_bAbort = false;    // reset
            str.Format("%s Received a Result(-) response from IUT", (LPCTSTR)m_title);
            throw((LPCTSTR)str);
         }
         break;
      }
   }

   m_bExpectPacket = false;   // OK, we have gotten the packet
   return bReceived;
}

// Send an unconfirmed request, leaving m_bExpectPacket and m_bExpectAPDU true
void SendReceiveExecutor::SendUnconfirmed(CByteArray& contents, bool isBroadcast)
{
   // network layer
   CByteArray buf;
   buf.Add((BYTE)0x01);      // version
   if ((m_IUTAddr.addrType != localStationAddr) && (m_IUTAddr.addrType != localBroadcastAddr))
   {  // IUT is on a remote network
      buf.Add(0x20);                               // Set control to include DNET, DLEN, DADR, Hop

      // Annoyingly, BACnetAddress doesn't set addrNet to 0xFFFF for global broadcast
      UINT net = (m_IUTAddr.addrType == globalBroadcastAddr) ? 0xFFFF : m_IUTAddr.addrNet;
      buf.Add((net & 0xFF00) >> 8);                // DNET
      buf.Add(net & 0x00FF);

      buf.Add((BYTE)m_IUTAddr.addrLen);            // DLEN
      for (int index = 0; index < m_IUTAddr.addrLen; index++)
      {
         buf.Add(m_IUTAddr.addrAddr[index]);       // DADR
      }

      buf.Add((BYTE)0xFF);       // hop count
   }
   else
   {
      buf.Add(0x00);       //control no resp
   }

   contents.InsertAt(0, &buf);
   if (m_pPort->m_nPortType == ipPort)
   {
      int len = contents.GetSize() + 4;       // the size of npdu plus BVLC
      contents.InsertAt(0, (BYTE)(len & 0xFF));
      contents.InsertAt(0, (BYTE)(len >> 8));

      // Normally we broadcast.
      // But if we have a foreign BBMD connection, ask him to do it
      // TODO: If we are registered with a foreign device, shouldn't the DATALINK
      // do this for us on ALL broadcasts?  Seems to process VTS's I-ams
      BYTE bipFunction = 0x0B;   // Original-broadcast
      if ((m_pPort->portBIPForeign != NULL) && (m_pPort->portBIPForeign->foreignBBMDAddr == m_IUTAddr))
      {
         bipFunction = 0x09;  // Distribute-Broadcast-To-Network
      }
      contents.InsertAt(0, bipFunction);

      contents.InsertAt(0, (BYTE)0x81);
   }

   VTSDevicePtr pdevice = m_pPort->m_pdevice;

   BACnetAddress  addr;
   if (m_IUTAddr.addrType == remoteStationAddr)
   {
      addr = m_routerAddr;
   }
   else
   {
      addr = m_IUTAddr;
   }
   // Before sending a packet, modify the signal bit to notify the main thread to
   // prepare accept packet
   m_bExpectAPDU = true;
   m_bExpectPacket = true;
   m_errorClass = -1;
   m_errorCode = -1;

   // send packet
   m_pPort->portFilter->Indication(
         BACnetNPDU(addr, contents.GetData(), contents.GetSize())
         );
}

void SendReceiveExecutor::InsertMaxAPDULenAccepted(CByteArray& contents)
{
   VTSDevicePtr   pdevice = m_pPort->m_pdevice;
   int nMaxAPDUSize;
   if (pdevice)
   {
      nMaxAPDUSize = pdevice->m_nMaxAPDUSize;
   }
   else
   {
      nMaxAPDUSize = 1024;
   }

   if (nMaxAPDUSize <= 50)
   {
      contents.InsertAt(0, (BYTE)0x00);
   }else if (nMaxAPDUSize <= 128)
   {
      contents.InsertAt(0, (BYTE)0x01);
   }else if (nMaxAPDUSize <= 206)
   {
      contents.InsertAt(0, (BYTE)0x02);
   }else if (nMaxAPDUSize <= 480)
   {
      contents.InsertAt(0, (BYTE)0x03);
   }else if (nMaxAPDUSize <= 1024)
   {
      contents.InsertAt(0, (BYTE)0x04);
   }else
   {
      contents.InsertAt(0, (BYTE)0x05);
   }
}

void SendReceiveExecutor::FindRouterAddress()
{
   // Router is only needed for remote destinations
   if ((m_IUTAddr.addrType == remoteStationAddr) || (m_IUTAddr.addrType == remoteBroadcastAddr))
   {
      // We need a route to m_IUTAddr.addrNet
      unsigned short addrNet = m_IUTAddr.addrNet;
      if (addrNet != m_lastNetwork)
      {
         // Send Who-Is-Router-To-Network message
         CByteArray contents;
         contents.InsertAt( 0, (BYTE)(0x00FF & addrNet) );      // DNET, 2 octets
         contents.InsertAt( 0, (BYTE)(addrNet >> 8) );
         contents.InsertAt( 0, (BYTE)0x00 );       // Message Type = X'00' Who-Is-Router-To-Network
         contents.InsertAt( 0, (BYTE)0x80 );       // control
         contents.InsertAt( 0, (BYTE)0x01 );       // version
         if (m_pPort->m_nPortType == ipPort)
         {
            int len = contents.GetSize() + 4;      // the size of npdu plus BVLC
            contents.InsertAt( 0, (BYTE)(len & 0x00FF) );
            contents.InsertAt( 0, (BYTE)(len >> 8) );
            contents.InsertAt( 0, (BYTE)0x0B );    // Original Broadcast message
            contents.InsertAt( 0, (BYTE)0x81 );
         }

         BACnetAddress  addr(localBroadcastAddr);
         m_bExpectAPDU = false;     // expect NPDU
         m_bExpectPacket = true;
         m_pPort->portFilter->Indication(
            BACnetNPDU(addr, contents.GetData(), contents.GetSize())
            );
         if (!m_event.Lock(30000))  // timeout setting: 30 seconds
         {
            CString str;
            str.Format("Cannot find a router to network %u", addrNet);
            throw( (LPCTSTR)str );
         }
         m_bExpectPacket = false;
         m_bExpectAPDU = true;      // reset

         // We don't parse the I-Am-Router here.
         // Instead, we rely on the infrastructure to make note of this
         // router, and use it when we use a remoteStation or remoteBroadcast
         // to that network.
         m_lastNetwork = addrNet;
      }
   }
}

void SendReceiveExecutor::Msg(const char* errMsg)
{
   VTSPacket pkt;
   pkt.packetHdr.packetProtocolID = (int)BACnetPIInfo::textMsgProtocol;
   pkt.packetHdr.packetFlags = 0;
   pkt.packetHdr.packetType = msgData;
   BACnetOctet* buff = new BACnetOctet[strlen(errMsg)+1];
   memcpy(buff, errMsg, strlen(errMsg)+1);
   pkt.NewDataRef(buff, strlen(errMsg)+1);

   VTSDocPtr   vdp = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
   vdp->WritePacket( pkt );

   delete []buff;
}

bool SendReceiveExecutor::SendExpectReadProperty( BACnetObjectIdentifier &objID,
                                                  BACnetEnumerated       &propID,
                                                  BACnetAnyValue         &propValue,
                                                  int                    propIndex /*= -1*/)
{
   m_bExpectAPDU = true;
   m_nExpectAPDUType = complexAckPDU;
   m_nExpectAPDUServiceChoice = readProperty;

   // encode the packet
   CByteArray contents;
   BACnetAPDUEncoder enc;
   objID.Encode(enc,0);
   propID.Encode(enc,1);
   if (propIndex != -1)
   {
      BACnetUnsigned propertyArrayIndex(propIndex);
      propertyArrayIndex.Encode(enc,2);
   }

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
   {
      contents.Add( enc.pktBuffer[i] );
   }
   contents.InsertAt(0, (BYTE)0x0C);      // Service Choice = 12(ReadProperty-Request)
   contents.InsertAt(0, InvokeID());      // Invoke ID
   InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
   contents.InsertAt(0, (BYTE)0x00);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=0)

   if (!SendExpectPacket(contents))
   {
      return false;
   }

   BACnetAPDUDecoder dec(m_pAPDU->pktBuffer, m_pAPDU->pktLength);
   objID.Decode(dec);
   propID.Decode(dec);
   if (propIndex != -1)
   {
      BACnetUnsigned index;
      index.Decode(dec);
   }
   BACnetOpeningTag().Decode(dec);
   propValue.Decode(dec);
   BACnetClosingTag().Decode(dec);

   if (dec.pktLength != 0)
   {
      Msg("Decoding hasn't finished!");
   }

   return true;
}

// As SendExpectReadProperty, but never throw: log any error except "unknown property" and return false
bool SendReceiveExecutor::SendReadPropertyOptional( BACnetObjectIdentifier   &objID,
                                                    BACnetEnumerated         &propID,
                                                    BACnetAnyValue           &propValue,
                                                    int                      propIndex )
{
   bool retval = false;
   try
   {
      retval = SendExpectReadProperty( objID, propID, propValue, propIndex);
   }
   catch (...)
   {
      // If the error is "unknown property", say nothing.  Else log the error
      if ((m_errorClass != ERR_CLASS_PROPERTY) || (m_errorCode != ERR_CODE_UNKNOWN_PROPERTY))
      {
         CString errType;
         ErrorString(errType);

         CString msg;
         msg.Format( "Failed to read %s.  %s",
                     NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( propID.m_enumValue ),
                     (LPCTSTR)errType );
         m_pOutputDlg->OutMessage( msg );
      }
      retval = false;
   }

   return retval;
}

bool SendReceiveExecutor::SendExpectWriteProperty( BACnetObjectIdentifier &objID,
                                                   BACnetEnumerated       &propID,
                                                   BACnetAnyValue         &propValue,
                                                   int                    propIndex )
{
   m_bExpectAPDU = true;
   m_nExpectAPDUType = simpleAckPDU;
   m_nExpectAPDUServiceChoice = writeProperty;
   CByteArray contents;
   BACnetAPDUEncoder enc;
   objID.Encode(enc, 0);
   propID.Encode(enc, 1);
   BACnetOpeningTag().Encode(enc,3);
   propValue.Encode(enc);
   BACnetClosingTag().Encode(enc,3);

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
   {
      contents.Add( enc.pktBuffer[i] );
   }
   contents.InsertAt(0, (BYTE)0x0F);      // Service Choice = 15(WriteProperty-Request)
   contents.InsertAt(0, InvokeID());      // Invoke ID = 1;
   InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
   contents.InsertAt(0, (BYTE)0x00);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=0)

   return SendExpectPacket(contents);
}

// As SendExpectWriteProperty, but never throw: log any error except write-access-denied and return false
bool SendReceiveExecutor::SendWritePropertyOptional( BACnetObjectIdentifier &objID,
                                                     BACnetEnumerated       &propID,
                                                     BACnetAnyValue         &propValue,
                                                     int                    propIndex )
{
   bool retval = false;
   try
   {
      retval = SendExpectWriteProperty( objID, propID, propValue, propIndex);
   }
   catch (...)
   {
      // If the error is "write access denied", say nothing.  Else log the error
      if ((m_errorClass != ERR_CLASS_PROPERTY) || (m_errorCode != ERR_CODE_WRITE_ACCESS_DENIED))
      {
         CString errType;
         ErrorString(errType);

         CString msg;
         msg.Format( "Failed to write %s.  %s",
                     NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( propID.m_enumValue ),
                     (LPCTSTR)errType );
         m_pOutputDlg->OutMessage( msg );
      }
      retval = false;
   }

   return retval;
}

bool SendReceiveExecutor::SendExpectReinitialize( ReinitializedStateOfDevice nRreinitState, const char *pPassword )
{
   m_bExpectAPDU = true;
   m_nExpectAPDUType = simpleAckPDU;
   m_nExpectAPDUServiceChoice = reinitializeDevice;
   CByteArray contents;
   BACnetAPDUEncoder enc;
   BACnetEnumerated  reinitState(nRreinitState);
   reinitState.Encode(enc, 0);
   if (pPassword && *pPassword)
   {
      BACnetCharacterString password(pPassword);
      password.Encode(enc, 1);
   }
   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
   {
      contents.Add( enc.pktBuffer[i] );
   }
   contents.InsertAt(0, (BYTE)0x14);      // Service Choice = 20(ReinitializeDevice-Request)
   contents.InsertAt(0, InvokeID());      // Invoke ID
   InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
   contents.InsertAt(0, (BYTE)0x00);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=0)

   return SendExpectPacket(contents);
}


bool SendReceiveExecutor::SendExpectWhoIs(BACnetObjectIdentifier& iAmDeviceID, BACnetUnsigned& maxAPDULenAccepted)
{
   m_bExpectAPDU = true;
   m_nExpectAPDUType = unconfirmedRequestPDU;
   m_nExpectAPDUServiceChoice = iAm;

   // encode the packet
   CByteArray contents;
   contents.Add((BYTE)0x10);
   contents.Add((BYTE)whoIs);

   if (!SendExpectPacket(contents))
   {
      return false;
   }
   // set the contents of the following variables from the parameters contained
   // in the I-Am message
   BACnetAPDUDecoder dec(m_pAPDU->pktBuffer, m_pAPDU->pktLength);
   iAmDeviceID.Decode(dec);
   maxAPDULenAccepted.Decode(dec);

   BACnetEnumerated().Decode(dec);     // segSupported
   BACnetUnsigned().Decode(dec);       // vendorID

   if (dec.pktLength != 0)
   {
      Msg("Decoding hasn't finished!");
   }

   return true;
}

// Get m_maxAPDULen for the device specified by m_nDeviceObjInst.
// If m_nDeviceObjInst is unknown, use WhoIs to find it.
// Throws on error
void SendReceiveExecutor::GetDeviceMaxAPDU()
{
   BACnetObjectIdentifier devObjID;
   BACnetUnsigned         maxAPDULenAccepted;
   BACnetEnumerated       propID;
   AnyValue               propValue;

   if (m_nDeviceObjInst != DEVICE_OBJ_INST_NUM_UNSET)
   {
      m_pOutputDlg->OutMessage("Use ReadProperty to find MAX_APDU_LENGTH_ACCEPTED...", FALSE);
      devObjID.SetValue(OBJ_DEVICE, m_nDeviceObjInst);
      propID.m_enumValue = PICS::MAX_APDU_LENGTH_ACCEPTED;
      propValue.SetObject(&maxAPDULenAccepted);
      if (!SendExpectReadProperty(devObjID, propID, propValue))
      {
         throw("Cannot read MAX_APDU_LENGTH_ACCEPTED value from IUT device object");
      }
   }
   else
   {
      // Transmit a unicast Who-Is message to the IUT's address,
      // and get deviceID, Max APDU Length Accepted
      m_pOutputDlg->OutMessage("Transmit a unicast Who-Is message to the IUT's address...", FALSE);
      if (SendExpectWhoIs(devObjID, maxAPDULenAccepted))
      {
         m_nDeviceObjInst = devObjID.GetObjectInstance();
      }
      else
      {
         m_pOutputDlg->OutMessage("Failed");
         m_pOutputDlg->OutMessage("Transmit a ReadProperty to read the Device ID by wildcard...", FALSE);
         // Transmit a ReadProperty request to the IUT, using the wildcard Device ID to read
         // the real Device Object ID. Store the returned Device Object ID into the variable
         // DEVICE_ID, then use ReadProperty to set M2 = Max_APDU_Length_Accepted
         BACnetObjectIdentifier objID(OBJ_DEVICE, 4194303);
         BACnetEnumerated propID(PICS::OBJECT_IDENTIFIER);
         propValue.SetObject(&devObjID);
         if (!SendExpectReadProperty(objID, propID, propValue))
         {
            // if failed received packet, the test failed
            throw("Dynamic discovery of the Device ID has failed!\n"
                 "The user must specify a Device Instance number");
         }

         m_nDeviceObjInst = devObjID.GetObjectInstance();
         m_pOutputDlg->OutMessage("OK");
         m_pOutputDlg->OutMessage("Use ReadProperty to set MAX_APDU_LENGTH_ACCEPTED...", FALSE);
         propID.m_enumValue = PICS::MAX_APDU_LENGTH_ACCEPTED;
         propValue.SetObject(&maxAPDULenAccepted);
         if (!SendExpectReadProperty(devObjID, propID, propValue))
         {
            throw("Cannot read MAX_APDU_LENGTH_ACCEPTED");
         }
      }
   }

   m_maxAPDULen = maxAPDULenAccepted.uintValue;

   CString str;
   str.Format("OK.\r\nDevice, %u has MAX_APDU_LENGTH_ACCEPTED %u.", m_nDeviceObjInst, m_maxAPDULen );
   m_pOutputDlg->OutMessage(str);
}

// Get the list of objects contained by the device specified by m_nDeviceObjInst.
// Throws on error
void SendReceiveExecutor::GetObjectList( BACnetArrayOf<BACnetObjectIdentifier> &objList )
{
   CString str;
   BACnetObjectIdentifier devObjID(OBJ_DEVICE, m_nDeviceObjInst);
   BACnetEnumerated       propID;
   AnyValue               propValue;

   m_pOutputDlg->OutMessage("Use ReadProperty to read the Device's Object_List...", FALSE);
   propID.m_enumValue = PICS::OBJECT_LIST;

   // Simple version to handle even devices without segmentation.
   // First to determine how many elements are in the list
   BACnetUnsigned numOfObjects;
   propValue.SetObject(&numOfObjects);
   if (!SendExpectReadProperty(devObjID, propID, propValue, 0))
   {
      throw("Cannot determine the number of objects in Device's Object_List");
   }

   for (UINT ix = 0; ix < numOfObjects.uintValue; ix++)
   {
      BACnetObjectIdentifier objID;
      propValue.SetObject(&objID);
      if (!SendExpectReadProperty(devObjID, propID, propValue, ix+1))
      {
         str.Format("Cannot read object %u in Device's Object_List", ix);
         throw(str);
      }
      objList.AddElement(&objID);
   }

   str.Format( "%u objects.  OK", numOfObjects.uintValue);
   m_pOutputDlg->OutMessage(str);
}

// Format error-class and error-code into a string
void SendReceiveExecutor::ErrorString( CString &theErrorString ) const
{
   if (m_errorClass >= 0)
   {
      // Got an Error response
      const char *pClass = NetworkSniffer::BAC_STRTAB_BACnetErrorClass.EnumString(m_errorClass);
      const char *pCode  = NetworkSniffer::BAC_STRTAB_BACnetErrorCode.EnumString(m_errorCode);
      theErrorString.Format( "Error %s:%s", pClass, pCode );
   }
   else
   {
      theErrorString.Empty();
   }
}

void SendReceiveExecutor::AnyValue::SetObject(BACnetEncodeable * pbacnetEncodeable)
{
   pbacnetTypedValue = NULL;
   BACnetAnyValue::SetObject(pbacnetEncodeable);
}

void SendReceiveExecutor::PropertyValue::Encode(BACnetAPDUEncoder& enc)
{
   m_propID.Encode(enc, 0);
   BACnetOpeningTag().Encode(enc, 2);
   m_propValue.Encode(enc);
   BACnetClosingTag().Encode(enc, 2);
}

void SendReceiveExecutor::PropertyValue::Decode(BACnetAPDUDecoder &dec)
{
   // can not decode.
   ASSERT(FALSE);
}

