// InconsistentParsExecutor.cpp: implementation of the InconsistentParsExecutor class.
// Lori Tribble - 10/17/2009
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSDoc.h"
#include "BACnet.hpp"
#include "VTSInconsistentParsDlg.h"
//#include "VTSInconsistentProgressDlg.h"
#include "ScriptExecutor.h"
#include "InconsistentParsExecutor.h"
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

// global defines
InconsistentParsExecutor gInconsistentParsExecutor;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


InconsistentParsExecutor::InconsistentParsExecutor()
: SendReceiveExecutor("Inconsistent Parameter Tests"),
  m_funToExe(INVALID_TAG_TEST)
{
}

InconsistentParsExecutor::~InconsistentParsExecutor()
{
}

void InconsistentParsExecutor::ExecuteTest()
{
   CSingleLock lock(&m_cs);
   lock.Lock();

   if (m_execState != execIdle) {
      TRACE0( "Error: invalid executor state\n" );
      return;
   }

   VTSDocPtr pVTSDoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
   VTSPorts* pPorts = pVTSDoc->GetPorts();
   VTSNames* pNames = pVTSDoc->GetNames();

   VTSInconsistentParsDlg dlg(*pNames, *pPorts);
   if ( dlg.DoModal() == IDOK )
   {
      for ( int i = 0; i < pPorts->GetSize(); i++ )
      {
         if ( ((VTSPort *) pPorts->GetAt(i))->m_strName.CompareNoCase(dlg.m_strPort) == 0 )
         {
            m_pPort = (VTSPort *) pPorts->GetAt(i);
            break;
         }
      }
      m_pName = pNames->GetAt( pNames->FindIndex(dlg.m_strDevice) );
      m_funToExe = (FunctionToExecute)dlg.m_nFunction;
      m_ObjID = dlg.m_ObjectID;
      m_propID = dlg.m_propCombo;
   }
   else
   {
      return;
   }

   StartTest();
   lock.Unlock();
}

void InconsistentParsExecutor::ProcessTask()
{
   CSingleLock lock(&m_cs);
   lock.Lock();

   BOOL bSuccess = TRUE;
   try {
      if ( !m_pPort || !m_pName )
      {
         throw("VTSPort or IUT not specified");
      }

      m_IUTAddr = m_pName->m_bacnetaddr;
      if (m_IUTAddr.addrType != localStationAddr &&
         m_IUTAddr.addrType != remoteStationAddr)
      {
         throw("The IUT's address can only be local station or remote station");
      }

      m_execState = execRunning;

      m_pOutputDlg->BeginTestProcess();

      FindRouterAddress();

      switch(m_funToExe) 
      {
      case INVALID_TAG_TEST:
         DoInvalidTagTest();
         Msg("13.4.3: Invalid Tag test has been sucessfully completed!");
         break;
      case MISSING_REQUIRED_TEST:
         DoMissingRequiredTest();
         Msg("13.4.4: Missing Required test has been sucessfully completed!");
         break;
      case TOO_MANY_ARGS_TEST:
         DoTooManyArgsTest();
         Msg("13.4.5: Too Many Args test has been sucessfully completed!");
         break;
      default:
         DoInvalidTagTest();
         DoMissingRequiredTest();
         DoTooManyArgsTest();
         break;
      }
   }
   catch (const char *errMsg) {
      bSuccess = FALSE;
      Msg(errMsg);
   }
   catch (...) {
      bSuccess = FALSE;
      Msg("Decoding error! Maybe received uncorrect packet!");
   }

   m_execState = execIdle;
   if (bSuccess)
   {
      m_pOutputDlg->OutMessage("Selected tests completed successfully");
      Msg("Selected tests completed successfully");
   }
   else if (m_bUserCancelled)
   {
      m_bUserCancelled = FALSE;  // reset
      m_pOutputDlg->OutMessage("", TRUE);    //begin a new line
      m_pOutputDlg->OutMessage("Canceled by user");
   }
   else
   {
      m_pOutputDlg->OutMessage("Failed",TRUE);  // begin a new line
      m_pOutputDlg->OutMessage("Error occured during the test");
   }

   m_pOutputDlg->EndTestProcess();
   lock.Unlock();
}


void InconsistentParsExecutor::DoInvalidTagTest()
{
   m_pOutputDlg->OutMessage("Begin Invalid Tag test...");

   BACnetObjectIdentifier objObjID;
   BACnetEnumerated propID;
   RejectReason expectedErrors[] = {INVALID_TAG, INCONSISTENT_PARAMETERS, INVALID_PARAMETER_DATA_TYPE,
      MISSING_REQUIRED_PARAMETER, TOO_MANY_ARGUMENTS};

   m_pOutputDlg->OutMessage("Send ReadProperty...",   FALSE);
   // TODO: get prop and obj from DLG and then send special packet
   //propID.m_enumValue = PICS::OBJECT_IDENTIFIER;
   //objObjID.SetValue( OBJ_DEVICE, m_nDeviceObjInst);
   objObjID = m_ObjID;
   propID = m_propID;

   // encoder with objObjID, PropID with invalid tag
   // encode the packet
   BACnetAPDUEncoder enc;
   objObjID.Encode(enc,0);
   propID.Encode(enc,5);  // note incorrect tag specified here on purpose

   if (!SendReadPropertyExpectReject(&enc, expectedErrors, 5))
   {
      throw("Incorrect Error Code returned by IUT");
   }
   m_pOutputDlg->OutMessage("OK");

}


void InconsistentParsExecutor::DoMissingRequiredTest()
{
   m_pOutputDlg->OutMessage("Begin Missing Required Test...");

   BACnetObjectIdentifier objObjID;
   RejectReason expectedErrors[] = {INVALID_TAG, MISSING_REQUIRED_PARAMETER};

   m_pOutputDlg->OutMessage("Send ReadProperty...",   FALSE);
   // TODO: get obj from DLG and then send special packet
   //objObjID.SetValue( OBJ_DEVICE, m_nDeviceObjInst);
   objObjID = m_ObjID;

   BACnetAPDUEncoder enc;
   objObjID.Encode(enc,0);

   if (!SendReadPropertyExpectReject(&enc, expectedErrors, 2))
   {
      throw("Incorrect Error Code returned by IUT");
   }
   m_pOutputDlg->OutMessage("OK");

}


void InconsistentParsExecutor::DoTooManyArgsTest()
{
   m_pOutputDlg->OutMessage("Begin Too Many Args Test...");

   BACnetObjectIdentifier objObjID;
   BACnetEnumerated propID;

   RejectReason expectedErrors[] = {INVALID_TAG, TOO_MANY_ARGUMENTS};

   m_pOutputDlg->OutMessage("Send ReadProperty...",   FALSE);
   // TODO: get prop and obj from DLG and then send special packet
   //propID.m_enumValue = PICS::OBJECT_IDENTIFIER;
   //objObjID.SetValue( OBJ_DEVICE, m_nDeviceObjInst);
   objObjID = m_ObjID;
   propID = m_propID;

   BACnetAPDUEncoder enc;
   objObjID.Encode(enc,0);
   propID.Encode(enc, 1);
   propID.Encode(enc, 1);  // add second property ID to encoding

   if (!SendReadPropertyExpectReject(&enc, expectedErrors, 2))
   {
      throw("Incorrect Error Code returned by IUT");
   }
   m_pOutputDlg->OutMessage("OK");

}

BOOL InconsistentParsExecutor::SendReadPropertyExpectReject(BACnetAPDUEncoder *enc, RejectReason Allowed_Errors[], int len)
{
   m_bExpectAPDU = TRUE;
   m_nExpectAPDUType = rejectPDU;
   //m_nExpectAPDUServiceChoice = readProperty;
   // encode the packet
   CByteArray contents;
   int i = 0;

   // copy the encoding into the byte array
   for (i = 0; i < enc->pktLength; i++)
   {
      contents.Add( enc->pktBuffer[i] );
   }
   contents.InsertAt(0, (BYTE)0x0C);      // Service Choice = 12(ReadProperty-Request)
   contents.InsertAt(0, (BYTE)0x01);      // Invoke ID = 1;
   InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
   contents.InsertAt(0, (BYTE)0x00);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=0)

   if (!SendExpectPacket(contents))
   {
      return FALSE;
   }

   BACnetEnumerated rejectReason;

   rejectReason.m_enumValue = m_pAPDU->apduAbortRejectReason;

   // see if we received one of the expected error codes
   for (i = 0; i < len; i++ )
   {
      if ( rejectReason.m_enumValue == Allowed_Errors[i] )
         return TRUE;
   }

   return FALSE;

}

