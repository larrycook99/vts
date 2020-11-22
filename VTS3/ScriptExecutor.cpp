// ScriptExecutor.cpp: implementation of the ScriptExecutor class.
//
//////////////////////////////////////////////////////////////////////

// I know it is generally poor form to reformat source code to meet one's
// personal preferences, but some of these function were so mangled with random tab/space
// indenting and 50-line if-following-if-following-for that I found
// it both incomprehensible and dangerous to modify.  Sorry if you are offended...

#include "stdafx.h"
#include "VTS.h"
#include "VTSDoc.h"
#include "VTSQueue.h"

#include "PI.h"
#include "StringTables.h"

#include "BACnetIP.hpp"

#include "ScriptExecutor.h"
#include "ScriptSelectSession.h"
#include "ScriptParmList.h"
#include "ScriptCommand.h"

#include "ScriptKeywords.h"

#include "BakRestoreExecutor.h"
#include "InconsistentParsExecutor.h"
#include "DiscoveryExecutor.h"

namespace PICS {

#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"

//extern struct generic_object;
//class BACnetAnyValue;
extern "C" void CreatePropertyFromEPICS( PICS::generic_object * pObj, int PropId, BACnetAnyValue * pbacnetAnyValue );

}

extern PICS::PICSdb *gPICSdb;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// global defines

ScriptExecutor    gExecutor;
ScriptFilterList  gMasterFilterList;

#include "math.h"
const float FLOAT_EPSILON = (float)1e-005;
const double DOUBLE_EPSILON = 1e-010;

// VTSQueue<ScriptExecMsg>

// Why is this CPP file here?  Should be able to do all from header file. - STK
//#include "VTSQueue.cpp"
// I moved all the code from VTSQueue.CPP to VTSQueue.H (okay for templates). - STK
 
// This declaration below instantiates a version of the VTSQueue that contains
// executor messages.  This will generate a warning that the template has already 
// been instantiated, but don't believe it.  I wouldn't be suprised if the 
// template code is defined someplace but not accessable to the linker. - Joel?

// This is instantiated in the class ScriptExecutor as execMsgQueue - STK
// I wouldn't have typedef'd it - that really confuses the programmer. - STK
//template class VTSQueue<ScriptExecMsg>;


ScriptMsgStatus::ScriptMsgStatus( ScriptDocumentPtr pDoc, ScriptBasePtr pbase, int nStatus )
            :ScriptExecMsg(ScriptExecMsg::msgStatus)
{
   m_pdoc = pDoc;
   m_pbase = pbase;
   m_nStatus = nStatus;
}

ScriptMsgStatus::~ScriptMsgStatus()
{
   // Invalidate all out pointers: we think someone is using us post mortem
   m_pdoc = NULL;
   m_pbase = NULL;
}

//
// ScriptFilter::ScriptFilter
//

ScriptFilter::ScriptFilter( ScriptFilterType typ, const char *name )
   : filterType(typ)
{
   strcpy( filterName, name );
   gMasterFilterList.AddFilter( this );
}

//
// ScriptFilter::~ScriptFilter
//

ScriptFilter::~ScriptFilter( void )
{
   gMasterFilterList.RemoveFilter( this );
}

//
// ScriptFilterList::ScriptFilterList
//

ScriptFilterList::ScriptFilterList( void )
{
}

//
// ScriptFilterList::~ScriptFilterList
//
// If the list goes away, make sure all of the filters go away.
//

ScriptFilterList::~ScriptFilterList( void )
{
   for (POSITION xpos = GetHeadPosition(); xpos; )
      delete GetNext( xpos );
}

//
// ScriptFilterList::AddFilter
//

void ScriptFilterList::AddFilter( ScriptFilterPtr fp )
{
   // add it to our list of ports
   AddTail( fp );
}

//
// ScriptFilterList::RemoveFilter
//

void ScriptFilterList::RemoveFilter( ScriptFilterPtr fp )
{
   for (POSITION xpos = GetHeadPosition(); xpos; ) {
      POSITION    cur = xpos;
      ScriptFilterPtr curfp = (ScriptFilterPtr)GetNext( xpos );

      if (curfp == fp) {
         RemoveAt( cur );
         return;
      }
   }
}

//
// ScriptFilterList::FindFilter
//

ScriptFilterPtr ScriptFilterList::FindFilter( const char *name )
{
   for (POSITION xpos = GetHeadPosition(); xpos; ) {
      ScriptFilterPtr cur = (ScriptFilterPtr)GetNext( xpos );

      if (strcmp(name,cur->filterName) == 0)
         return cur;
   }

   // failed to find it
   return 0;
}

//
// ScriptFilterList::Length
//

int ScriptFilterList::Length( void )
{
   return CList<ScriptFilterPtr,ScriptFilterPtr>::GetCount();
}

//
// ScriptFilterList::operator []
//

ScriptFilterPtr ScriptFilterList::operator []( int i )
{
   POSITION xpos = FindIndex( i );

   ASSERT( xpos != NULL );
   return GetAt( xpos );
}

//
// ScriptNetFilter::ScriptNetFilter
//
// The only thing that the filter ctor must do is save its name.
//

ScriptNetFilter::ScriptNetFilter( const char *name )
   : ScriptFilter( scriptNPDUFilter, name )
{
}

//
// ScriptNetFilter::~ScriptNetFilter
//
// When the configuration of VTS changes, these filter objects will be deleted.  These
// filter objects are private to the filter list, so there's nothing special to do 
// in the dtor.
//

ScriptNetFilter::~ScriptNetFilter( void )
{
}

//
// ScriptNetFilter::Indication
//
// When a filter gets an indication is acting as a server to some higher level object.
// This function is also called when the executor has some NPDU that needs to be sent 
// down to the port.  This server is simple, it just passes it to the bound client.
//

void ScriptNetFilter::Indication( const BACnetNPDU &npdu )
{
   // pass down to server
   Request( npdu );
}

//
// ScriptNetFilter::Confirmation
//
// When a filter gets a confirmation it is bound to some object that wants this message 
// passed up the chain.  Before doing that, make sure the executor gets a chance to look 
// at it and see if it passes any EXPECT conditions that a running script established.
// If the executor isn't running, don't bother.
//
extern BakRestoreExecutor gBakRestoreExecutor;
extern InconsistentParsExecutor gInconsistentParsExecutor;
extern DiscoveryExecutor gDiscoveryExecutor;

void ScriptNetFilter::Confirmation( const BACnetNPDU &npdu )
{
   // make a copy for the executor thread
   // (Yes, new without storing a pointer: the constructor queues "thi"s...
   if (gExecutor.IsRunning())
      new ScriptNetPacket( this, npdu );
   
   // send this packet to BackupRestore executor
   if (gBakRestoreExecutor.IsRunning()) {
      gBakRestoreExecutor.ReceiveNPDU(npdu);
   }

   if (gInconsistentParsExecutor.IsRunning()) {
      gInconsistentParsExecutor.ReceiveNPDU(npdu);
   }

   if (gDiscoveryExecutor.IsRunning()) {
      gDiscoveryExecutor.ReceiveNPDU(npdu);
   }
   // pass up to client
   Response( npdu );
}

//
// ScriptNetPacket::ScriptNetPacket
//

ScriptNetPacket::ScriptNetPacket( ScriptFilterPtr fp, const BACnetNPDU &npdu )
   : BACnetTask( BACnetTask::oneShotDeleteTask )
   , packetFilter(fp)
   , packetNPDU( npdu.pduAddr
      , 0, npdu.pduLen
      , npdu.pduExpectingReply, npdu.pduNetworkPriority
      )
{
   // timestamp the delivery
   ::GetSystemTimeAsFileTime( &packetTime );

   // make a copy of the data
   packetData = new BACnetOctet[ npdu.pduLen ];
   memcpy( packetData, npdu.pduData, npdu.pduLen );

   // point to the new copy
   packetNPDU.pduData = packetData;

   // let it run when it gets a chance
   InstallTask();
}

//
// ScriptNetPacket::~ScriptNetPacket
//

ScriptNetPacket::~ScriptNetPacket( void )
{
   // delete the content
   delete[] packetData;
}

//
// ScriptNetPacket::ProcessTask
//

void ScriptNetPacket::ProcessTask( void )
{
   // pass this along
   gExecutor.ReceiveNPDU( (ScriptNetFilterPtr)packetFilter, packetNPDU );
}

//
// ScriptDebugNetFilter::ScriptDebugNetFilter
//

ScriptDebugNetFilter::ScriptDebugNetFilter( char *name )
   : ScriptFilter( scriptNPDUFilter, name ), filterPeer(0)
{
}

//
// ScriptDebugNetFilter::~ScriptDebugNetFilter
//

ScriptDebugNetFilter::~ScriptDebugNetFilter( void )
{
}

//
// ScriptDebugNetFilter::Indication
//

void ScriptDebugNetFilter::Indication( const BACnetNPDU &npdu )
{
   // make a copy for the executor thread
   if (filterPeer && gExecutor.IsRunning())
      new ScriptNetPacket( filterPeer, npdu );
}

//
// ScriptDebugNetFilter Bind
//

void Bind( ScriptDebugNetFilterPtr fp1, ScriptDebugNetFilterPtr fp2 )
{
   fp1->filterPeer = fp2;
   fp2->filterPeer = fp1;
}

//--------------------------------------------------

//
// ScriptAppFilter::ScriptAppFilter
//
// The only thing that the filter ctor must do is save its name.
//

ScriptAppFilter::ScriptAppFilter( char *name )
   : ScriptFilter( scriptAPDUFilter, name )
{
}

//
// ScriptAppFilter::~ScriptAppFilter
//

ScriptAppFilter::~ScriptAppFilter( void )
{
}

//
// ScriptAppFilter::Indication
//

void ScriptAppFilter::Indication( const BACnetAPDU &adpu )
{
   // pass down to server
   Request( adpu );
}

//
// ScriptAppFilter::Confirmation
//

void ScriptAppFilter::Confirmation( const BACnetAPDU &apdu )
{
#if 0
   // give it to the executor
   if (gExecutor.IsRunning())
      gExecutor.ReceiveAPDU( this, apdu );
#endif

   // pass up to client
   Response( apdu );
}

//
// ScriptExecutor::ExecError::ExecError
//
// Error message with a script line number
//

ScriptExecutor::ExecError::ExecError( const char *msg, int lineNo )
: errMsg(msg), errLineNo(lineNo)
{
}

//
// ScriptExecutor::ScriptExecutor
//

ScriptExecutor::ScriptExecutor( void )
   : execState(execIdle)
   , execAllTests(false), execSingleStep(false), execStepForced(0)
   , execFailContinue(false)
   , execDB(0), execDoc(0), execTest(0), execPacket(0), execCommand(0)
{
   // Can't execute these here, since ScriptExecutor is statically constructed,
   // and we may get run before all our dependencies do.
//   BACnetTime::TestTimeComps();
//   BACnetDate::TestDateComps();
}

//
// ScriptExecutor::~ScriptExecutor
//

ScriptExecutor::~ScriptExecutor()
{
}

//
// ScriptExecutor::Setup
//
// This function is used by the application to initialize the executor
// for running.  It does not change the execState, that will be done when 
// the application calls Run() or Step().
//
// By default if a specific test is being run, then only that test will 
// run and execSingleTest will be set.  The application can reset it at any 
// before it completes and the executor will continue to the end of the 
// script.
//

void ScriptExecutor::Setup( VTSDocPtr vdp, ScriptDocumentPtr sdp, ScriptTestPtr stp )
{
   // if the executor is already initialized, exit
   if (execState != execIdle) {
      TRACE( "ScriptExecutor::Setup() error, not idle\n" );
      return;
   }

   // initialize
   execDB = vdp;
   execDoc = sdp;
   execTest = stp;

   // see if all tests should execute, default to run 
   execAllTests = (stp == 0);
   execSingleStep = false;
   execPending = false;

   // there is no current packet
   execPacket = 0;
   execCommand = NULL;

   // let the document know that a script will be writing messages
   execDoc->BindExecutor();
}

//
// ScriptExecutor::Cleanup
//
// This member function is called when the executor has completed the 
// testing process for some reason (success or failure).  It will let 
// the bound document know that it will no longer receive messages.
//

void ScriptExecutor::Cleanup( void )
{
   TRACE( "Executor::Cleanup()\n" );

   // make sure we don't get rescheduled
   SuspendTask();

   // check to run the next test
   if (execAllTests && execTest->testNext)
   {
      // move to the next test
      execTest = execTest->testNext;
      execPacket = 0;
      execCommand = NULL;

      // install the task
      InstallTask( oneShotTask, 0 );
   }
   else
   {
      // Added by Zhu Zhenhua, 2003-12-18, to run all sections
      bool bOtherSectionTest = false;
      if (execAllTests && (execDoc->SelectedSection() == NULL))
      {
         ScriptBasePtr sbp = execDoc->m_pContentTree->m_pScriptContent;
         for (int i = 0; i < sbp->Length(); i++)
         {
            ScriptSectionPtr ssp = (ScriptSectionPtr)sbp->Child( i );
            if (execTest == (ScriptTestPtr)( ssp->Child( ssp->Length() -1)) && (i < sbp->Length()-1))
            {
               if (sbp->Child( i+1 )->Length() != 0)
               {
                  bOtherSectionTest = true;
                  execTest = (ScriptTestPtr)(sbp->Child( i+1 )->Child(0));
                  execPacket = 0;
                  execCommand = NULL;
                  // install the task
                  InstallTask( oneShotTask, 0 );
                  break;
               }
            }
         }
      }

      if (!bOtherSectionTest)
      {
         // release from the document
         execDoc->UnbindExecutor();

         if (execCommand && (execCommand->baseType == ScriptBase::scriptPacket) && 
            (((ScriptPacketPtr) execCommand)->packetType == ScriptPacket::expectPacket) &&
            (((ScriptPacketPtr) execCommand)->m_pcmdMake != NULL) &&
            ((ScriptPacketPtr) execCommand)->m_pcmdMake->IsUp())
         {
            // It's still up... Kill it with a success regardless of whether or not this one failed...
            ((ScriptPacketPtr) execCommand)->m_pcmdMake->Kill();
            SetPacketStatus( ((ScriptPacketPtr) execCommand)->m_pcmdMake, SCRIPT_STATUS_OK );
         }

         // clear all the rest of the execution vars
         execState = execIdle;
         execDB = 0;
         execDoc = 0;
         execTest = 0;
         execPacket = 0;
         execCommand = NULL;
      }
   }
}

//
// ScriptExecutor::IsBound
//
// This function returns true iff the executor is bound to a specific 
// document.  Unlike the other simpler functions IsIdle(), IsRunning()
// and IsStopped(), this one has two things to check and just to make 
// sure the executor state doesn't change in the middle of the call, 
// this is locked (in the main thread context).
//

bool ScriptExecutor::IsBound( ScriptDocumentPtr sdp )
{
// execCS.Lock();
   bool rslt = ((execState != execIdle) && (execDoc == sdp));
// execCS.Unlock();

   return rslt;
}

//
// ScriptExecutor::IsBound
//
// This function returns true iff the executor is bound to a specific 
// database.  This is used when the database is closed, the test will be 
// killed if it is.
//

bool ScriptExecutor::IsBound( VTSDocPtr vdp )
{
// execCS.Lock();
   bool rslt = ((execState != execIdle) && (execDB == vdp));
// execCS.Unlock();

   return rslt;
}

//
// ScriptExecutor::Msg
//
// Save a messsage related to script execution.  There are three "severity code" levels:
//
//    1  - test start/halt/pass/fail
//    2  - operator action (step, step/pass/fail)
//    3  - script info (delay expired, parm mismatch)
//

void ScriptExecutor::Msg( int sc, int line, const char *msg, int testStatus /*=SCRIPT_STATUS_OK*/ )
{
   VTSPacket      pkt;
   TRACE( "[%d:%d] %s stat=%d\n", sc, line, msg, testStatus );

   int len = strlen( msg );
   int failLen = ((sc == 1) && (testStatus == SCRIPT_STATUS_FAIL)) ? 6 : 0; // strlen("FAIL: ")

   // Header, plus test name, plus msg
   unsigned char *buff = new unsigned char[ 21 + failLen + 250+3 + len+1 ];

   // save the severity code
   buff[0] = sc;

   // save the line number
   buff[1] = (line >> 24) & 0xFF;
   buff[2] = (line >> 16) & 0xFF;
   buff[3] = (line >> 8) & 0xFF;
   buff[4] = (line & 0xFF);

   // save the digest
   memcpy( buff+5, execDoc->m_digest, 16 );
   unsigned char *dst = buff + 21;

   // for test level messages start with test name (up to 250 characters)
   if (sc == 1) {
      if (failLen)
      {
         // This is used as a flag by ListSummary to change the color of failure lines
         strcpy( (char*)dst, "FAIL: " );
         dst += failLen;
      }
      dst += sprintf( (char*)dst, "Test \"%.250s\" ", (LPCTSTR)execTest->baseLabel );
   }

   strcpy( (char*)dst, msg );
   dst += len;

   // fill in the packet header
   pkt.packetHdr.packetProtocolID = (int)BACnetPIInfo::msgProtocol;
   pkt.packetHdr.packetFlags = 0;
   pkt.packetHdr.packetType = msgData;

   // let the packet refer to the pdu contents, cast away const
   pkt.NewDataRef( buff, (dst - buff) + 1 );

   // save it in the database;
   execDB->WritePacket( pkt );

   delete[] buff;
}

//
// ScriptExecutor::Run
//
// This member function is called by the main thread in response to a run 
// request (as selected in the menu).  The application has called Setup() 
// to bind the executor to the correct document.
//

void ScriptExecutor::Run( void )
{
   CSingleLock    lock( &execCS );

   // lock to prevent multiple thread access
   lock.Lock();

   // verify the correct state
   if (execState != execIdle) {
      TRACE( "Error: invalid executor state\n" );
      return;
   }

   // clear the single step flag
   execSingleStep = false;

   // install the task
   InstallTask( oneShotTask, 0 );

   // unlock
   lock.Unlock();
}

//
// ScriptExecutor::Halt
//

void ScriptExecutor::Halt( void )
{
   CSingleLock    lock( &execCS );

   // lock to prevent multiple thread access
   lock.Lock();

   // verify the correct state
   if (execState != execRunning) {
      TRACE( "Error: invalid executor state\n" );
      return;
   }

   // "Halt" might sound like "halt forever".
   // "Suspend" is clearly temporary
   // I'm too lazy to change the method names...
   Msg( 1, execTest->baseLineStart, "Suspended" );

   // change the state to stopped
   execState = execStopped;

   // pull the task from the installed list (effecively canceling a timer)
   SuspendTask();
   execPending = false;

   // unlock
   lock.Unlock();
}

//
// ScriptExecutor::Step
//

void ScriptExecutor::Step( void )
{
   CSingleLock    lock( &execCS );

   // lock to prevent multiple thread access
   lock.Lock();

   // verify the correct state
   if (execState == execRunning) {
      TRACE( "Error: invalid executor state\n" );
      return;
   }

   Msg( 2, execTest->baseLineStart, "Step" );

   // set the single step flag
   execSingleStep = true;
   execPending = false;

   // install the task
   InstallTask( oneShotTask, 0 );

   // unlock
   lock.Unlock();
}

//
// ScriptExecutor::Step
//

void ScriptExecutor::Step( bool pass )
{
   CSingleLock    lock( &execCS );

   // lock to prevent multiple thread access
   lock.Lock();

   // verify the correct state
   if (execState != execStopped) {
      TRACE( "Error: invalid executor state\n" );
      return;
   }

   if (pass)
      Msg( 2, execTest->baseLineStart, "Step Pass" );
   else
      Msg( 2, execTest->baseLineStart, "Step Fail" );

   // set the single step flag
   execSingleStep = true;
   execPending = false;
   execStepForced = (pass ? 1 : 2);

   // install the task
   InstallTask( oneShotTask, 0 );

   // unlock
   lock.Unlock();
}

//
// ScriptExecutor::Resume
//

void ScriptExecutor::Resume( void )
{
   CSingleLock    lock( &execCS );

   // lock to prevent multiple thread access
   lock.Lock();

   // verify the correct state
   if (execState != execStopped) {
      TRACE( "Error: invalid executor state\n" );
      return;
   }

   Msg( 2, execTest->baseLineStart, "Resume" );

   // clear the single step flag
   execSingleStep = false;
   execPending = false;

   // install the task
   InstallTask( oneShotTask, 0 );

   // unlock
   lock.Unlock();
}

//
// ScriptExecutor::Kill
//

void ScriptExecutor::Kill( bool closingDoc )
{
   CSingleLock    lock( &execCS );

   // lock to prevent multiple thread access
   lock.Lock();

   // verify the correct state
   if (execState == execIdle) {
      TRACE( "Error: invalid executor state\n" );
      return;
   }

   // tell the database the test was killed
   Msg( 1, execTest->baseLineStart, "Terminated", SCRIPT_STATUS_FAIL );

   // pull the task from the installed list (effecively canceling a timer)
   SuspendTask();

   // set the status
// SetPacketStatus( execPacket, SCRIPT_STATUS_FAIL );
   SetPacketStatus( execCommand, SCRIPT_STATUS_FAIL );
   SetTestStatus( execTest, SCRIPT_STATUS_FAIL );

   // Clean up left over popups...

   if ( execCommand->baseType == ScriptBase::scriptPacket && ((ScriptPacketPtr) execCommand)->m_pcmdMake != NULL )
      ((ScriptPacketPtr) execCommand)->m_pcmdMake->Kill();

   // If we don't do this, "Kill" just aborts one test and goes on to the
   // next one.  We have FAIL and PASS for that.
   // Force the entire suite (if any) to fail.
   execAllTests = false;

   // make sure the doc knows we're done
   Cleanup();

   if (closingDoc)
   {
      // SetTestStatus sends WM_VTS_EXECMSG to the script window, but
      // it uses the document, which may be closed before ther message is
      // processed when Kill is called from close processing.
      // We need to ensure that gExecutor.ReadMsg() will return NULL
      //
      ScriptExecMsg *pMsg;
      while ((pMsg = ReadMsg()) != NULL)
      {
         delete pMsg;
      }
   }

   // unlock
   lock.Unlock();
}

//
// ScriptExecutor::ReadMsg
//
// This function provides the application access to the executor message queue 
// which is protected.
//

ScriptExecMsg * ScriptExecutor::ReadMsg( void )
{
   return execMsgQueue.Read();
}

//
// ScriptExecutor::ProcessTask
//
// This member function is called by the task manager when the task 
// should do something that it was scheduled to do.  In the case of 
// the executor, this means that a timer has expired and a receive 
// packet attempt has failed.

void ScriptExecutor::ProcessTask( void )
{
   int now = ::GetTickCount();
   CSingleLock lock(&execCS);

   // lock to prevent multiple thread access
   lock.Lock();

   // if no test, get the first one
   if (!execTest)
   {
      // must be bound to a document
      if (!execDoc)
      {
         // alert the user
         TRACE( "***** No document bound" );
         return;
      }

      // make sure the document has content
      if (!execDoc->m_pContentTree || !execDoc->m_pContentTree->m_pScriptContent)
      {
         // alert the user
         Msg( 2, 0, "No test to run, check syntax", SCRIPT_STATUS_FAIL );

         // go back to idle
         Cleanup();
         return;
      }

      // find the first test of section that has one
      ScriptBasePtr sbp = execDoc->m_pContentTree->m_pScriptContent;

      // Added by Zhu Zhenhua, 2003-12-18, to run select section
      ScriptSectionPtr ssp = execDoc->SelectedSection();
      if (ssp != NULL)
      {
         if (ssp->Length() != 0)
         {
            execTest = (ScriptTestPtr)ssp->Child( 0 );
         }
      }
      else
      {
         for (int i = 0; i < sbp->Length(); i++)
         {
            ScriptSectionPtr ssp = (ScriptSectionPtr)sbp->Child( i );
            if (ssp->Length() != 0) 
            {
               execTest = (ScriptTestPtr)ssp->Child( 0 );
               break;
            }
         }
      }

      if (!execTest) 
      {
         // alert the user
         Msg( 2, 0, "No test to run, check syntax", SCRIPT_STATUS_FAIL );

         // go back to idle
         Cleanup();
         return;
      }
   }

   // if no packet, get first one
   if (!execCommand)
   {
      // reset the test
      ResetTest( execTest );

      // calculate the new status
      int newStatus = CalcTestStatus( execTest );
      if (newStatus) 
      {
         // alert the user
         Msg( 1, execTest->baseLineStart, "failed, check dependencies", newStatus );

         // set the status
         SetTestStatus( execTest, newStatus );

         // go back to idle
         Cleanup();
         return;
      }

      // extract the first packet
      execCommand = execTest->testFirstCommand;

      // if no packets in test, it succeeds easily
      if (!execCommand)
      {
         // alert the user
         Msg( 1, execTest->baseLineStart, "trivial test successful" );

         // set the status
         SetTestStatus( execTest, SCRIPT_STATUS_OK );

         // go back to idle
         Cleanup();
         return;
      }

      // message to the database
      Msg( 1, execTest->baseLineStart, "started" );
      Msg( 3, execTest->baseLineStart, execDoc->GetPathName() );

      // set the test status to running
      SetTestStatus( execTest, SCRIPT_STATUS_WARN );

      // if single stepping, go to stopped
      if (execSingleStep) 
      {
         // set the packet status to running
         SetPacketStatus( execCommand, SCRIPT_STATUS_WARN );

         execState = execStopped;
         return;
      }
   }

   // executor running
   execState = execRunning;

keepGoing:
   // execute the current packet
   if (execStepForced == 1)
   {
      execStepForced = 0;
      NextPacket( true );
   }
   else if (execStepForced == 2)
   {
      execStepForced = 0;
      NextPacket( false );
   }
   else if ((execCommand->baseType == ScriptBase::scriptPacket) && 
          (((ScriptPacketPtr)execCommand)->packetType == ScriptPacket::sendPacket))
   {
      execPacket = (ScriptPacketPtr) execCommand;
      if (execPending || (execPacket->packetDelay == 0))
      {
         execPending = false;

         // do the packet
         NextPacket( SendPacket() );

         // might be more to do.  The goto sucks, but rather than reschedule with
         // a zero delay, keep the executor locked down until the transition to
         // the next packet can be completed.  This prevents packets that come in
         // faster than the reschedule from being processed by ReceiveNPDU.
         if (execCommand)
            goto keepGoing;
      }
      else 
      {
         if (execPacket->packetSubtype == ScriptPacket::rootPacket)
         {
            TRACE( "ProcessTask %p Send %08X root time %d\n", this, execPacket, now );
            execRootTime = now;
         }

         int delay = execPacket->packetDelay - (now - execRootTime);

         // proposed delay may have expired
         if (delay < 0) 
         {
            TRACE( "Send delay expired %d (2) %d\n", delay, now );
            Msg( 3, execPacket->baseLineStart, "Delay expired" );
            NextPacket( false );
         }
         else 
         {
            // set the status pending
            SetPacketStatus( execPacket, SCRIPT_STATUS_WARN );

            // come back later
            TRACE( "Send delay %d (2)\n", delay );
            execPending = true;
            InstallTask( oneShotTask, delay );
         }
      }
   }
   else if ((execCommand->baseType == ScriptBase::scriptPacket) &&
          (((ScriptPacketPtr)execCommand)->packetType == ScriptPacket::expectPacket))
   {
      execPacket = (ScriptPacketPtr) execCommand;
      TRACE( "ProcessTask expectPacket execPending=%d bpacketNotExpect=%d root time %d\n",
            execPending, execPacket->bpacketNotExpect, now );
      if (execPending) 
      {
         if ((execPacket->packetType == ScriptPacket::expectPacket) && execPacket->bpacketNotExpect)
         {
            //Modified by Zhu Zhenhua, 2003-11-25
            //add to meet "EXPECT NOT" or "EXPECT BEFORE (value) NOT"instead of "CHECK" sometimes
            NextPacket(true);
         }
         else
         {
            bool gotOne = false;
            // delay timer if necessary
            if (execPacket->m_pcmdMake != NULL)
            {
               if (execPacket->m_pcmdMake->IsUp())
               {
                  // if dialog still pending... don't start timers yet
                  execRootTime = now;
               }
               else if (!execPacket->m_pcmdMake->IsSuccess())
               {
                  CString strError;

                  SetPacketStatus(execPacket->m_pcmdMake, SCRIPT_STATUS_FAIL);
                  strError.Format("MAKE \"%s\" aborted by user", execPacket->m_pcmdMake->baseLabel );
                  Msg( 3, execPacket->m_pcmdMake->baseLineStart, strError, SCRIPT_STATUS_FAIL );

                  // shouldn't chain to next packet... already did.  Fail all these guys.
                  // This will short circuit all th timer stuff and fail all of the pending
                  // expect chains.

                  execRootTime = 0;
               }
               else
               {
                  SetPacketStatus(execPacket->m_pcmdMake, SCRIPT_STATUS_OK);
               }
            }

            // fail all the packets that have expired and min delay of rest
            int minDelay1 = kMaxPacketDelay + 1;

            for (ScriptPacketPtr pp1 = execPacket; pp1; pp1 = (ScriptPacketPtr) pp1->m_pcmdFail)
            {
               if ((pp1->baseStatus == SCRIPT_STATUS_WARN) && (pp1->baseType == ScriptBase::scriptPacket))
               {
                  if (pp1->packetDelay <= (now - execRootTime))
                  {
                     SetPacketStatus( pp1, SCRIPT_STATUS_FAIL );
                  }
                  else
                  {
                     gotOne = true;
                     minDelay1 = (pp1->packetDelay < minDelay1 ? pp1->packetDelay : minDelay1);
                  }
               }
            }

            // still one that hasn't failed?
            if (!gotOne)
            {
               Msg( 1, execTest->baseLineStart, "failed", SCRIPT_STATUS_FAIL );
               SetTestStatus( execTest, SCRIPT_STATUS_FAIL );

               // go back to idle
               Cleanup();
            }
            else 
            {
               // subtract time already expired
               minDelay1 -= (now - execRootTime);

               // If there is a modeless MAKE dialog UP... come back as soon as possible and keep checking
               // it's status to delay the timer...
               if ((execPacket->m_pcmdMake != NULL) && execPacket->m_pcmdMake->IsUp())
                  minDelay1 = 180;

               // schedule for the next delay
               TRACE( "ProcessTask %p Expect more delay %d %d\n", this, minDelay1, now );
               execPending = true;
               InstallTask( oneShotTask, minDelay1 );
            }
         } 
      }
      else
      {
         // set the root time, always a root packet here
         TRACE( "Expect %08X root time %d\n", execPacket, now );
         execRootTime = now;

         // set all the packets pending and find min delay
         int minDelay2 = kMaxPacketDelay + 1;
         for (ScriptPacketPtr pp2 = execPacket; pp2; pp2 = (ScriptPacketPtr) pp2->m_pcmdFail)
         {
            if (pp2->baseType == ScriptBase::scriptPacket)
            {
               SetPacketStatus( pp2, SCRIPT_STATUS_WARN );
               minDelay2 = (pp2->packetDelay < minDelay2 ? pp2->packetDelay : minDelay2);
            }
         }

         // If there is a modeless MAKE dialog up... come back as soon as possible and keep checking
         // it's status to delay the timer...

         if (execPacket->m_pcmdMake != NULL)
            minDelay2 = 180;

         // schedule for the next delay
         TRACE( "ProcessTask %p Expect delay %d %d\n", this, minDelay2, now );
         execPending = true;
         InstallTask( oneShotTask, minDelay2 );
      }
   }
   else if (execCommand->baseType == ScriptBase::scriptMake)
   {
      execPending = false;
      SetPacketStatus( execCommand, SCRIPT_STATUS_WARN );

      CString strError;
      // Set up the queue needed by this guy to fire off message for handling
      // modeless dialogs to main app.  Sorry.  It's weird, I know.

      ((ScriptMAKECommand *) execCommand)->SetQueue(&execMsgQueue);

      bool fCmdResult = execCommand->Execute(&strError);

      if ( !fCmdResult && !((ScriptMAKECommand *) execCommand)->m_fHanging )
         Msg( 3, execCommand->baseLineStart, strError, SCRIPT_STATUS_WARN );

      NextPacket(fCmdResult, ((ScriptMAKECommand *) execCommand)->m_fHanging );
   }
   else if (execCommand->baseType == ScriptBase::scriptCheck)
   {
      execPending = false;
      SetPacketStatus( execCommand, SCRIPT_STATUS_WARN );
      CString strError;
      bool fCmdResult = execCommand->Execute(&strError);
      if (!fCmdResult)
         Msg( 3, execCommand->baseLineStart, strError, SCRIPT_STATUS_WARN );

      NextPacket(fCmdResult);
   }
   else if (execCommand->baseType == ScriptBase::scriptAssign)
   {
      // TODO: same code as scriptWait and scriptAssign...
      //Added by Zhu Zhenhua, 2003-12-24, to ASSIGN statement
      execPending = false;
      SetPacketStatus( execCommand, SCRIPT_STATUS_WARN );
      CString strError;
      bool fCmdResult = execCommand->Execute(&strError);
      if (!fCmdResult)
         Msg( 3, execCommand->baseLineStart, strError, SCRIPT_STATUS_WARN );

      NextPacket(fCmdResult);
   }
   else if (execCommand->baseType == ScriptBase::scriptWait)
   {
      // TODO: same code as scriptCheck and scriptAssign...
      //Added by Zhu Zhenhua, 2003-12-31, to WAIT statement
      execPending = false;
      SetPacketStatus( execCommand, SCRIPT_STATUS_WARN );
      CString strError;
      bool fCmdResult = execCommand->Execute(&strError);
      if (!fCmdResult)
         Msg( 3, execCommand->baseLineStart, strError, SCRIPT_STATUS_WARN );

      NextPacket(fCmdResult);
   }
   else
   {
      TRACE( "ProcessTask %p unknown baseType = %d %d\n", this, execCommand->baseType, now );
   }

   // Since the whole point os CSingleLock is to unlock
   // in its destructor, and since we have a half-dozen return
   // statements prior to here, this seems a bit silly...
   //
   // unlock
   lock.Unlock();
}

//
// ScriptExecutor::NextPacket
//

void ScriptExecutor::NextPacket( bool okPacket, bool fHanging /* = false */ )
{
   TRACE( "NextPacket %p %08X %s %d\n", this, execCommand, 
         (fHanging ? "hanging" : (okPacket ? "succeeded" : "failed")),
         GetTickCount() );

   // set the current packet status
   if ( !fHanging )
      SetPacketStatus( execCommand, okPacket ? SCRIPT_STATUS_OK : SCRIPT_STATUS_FAIL );

   // Test to see if the one we're finished with is an EXPECT with a MAKE in front of it that is still up

   if ( (execCommand->baseType == ScriptBase::scriptPacket) && 
       (((ScriptPacketPtr) execCommand)->packetType == ScriptPacket::expectPacket) &&
       (((ScriptPacketPtr) execCommand)->m_pcmdMake != NULL) && 
       (((ScriptPacketPtr) execCommand)->m_pcmdMake->IsUp()) )
   {
      // It's still up... Kill it with a success regardless of whether or not this one failed...

      ((ScriptPacketPtr) execCommand)->m_pcmdMake->Kill();
      SetPacketStatus(((ScriptPacketPtr) execCommand)->m_pcmdMake, SCRIPT_STATUS_OK);
   }

   // move to next packet
   execCommand = (okPacket ? execCommand->m_pcmdPass : execCommand->m_pcmdFail);

   // if no next packet, set test status
   if (!execCommand) {
      // set the test status
      if (okPacket) {
         Msg( 1, execTest->baseLineStart, "passed", SCRIPT_STATUS_OK );
         SetTestStatus( execTest, SCRIPT_STATUS_OK );
      } else {
         Msg( 1, execTest->baseLineStart, "failed", SCRIPT_STATUS_FAIL );
         SetTestStatus( execTest, SCRIPT_STATUS_FAIL );
      }

      // go back to idle
      Cleanup();
   } else {
      // set the current packet status to running
      SetPacketStatus( execCommand, SCRIPT_STATUS_WARN );

      // if the user is single stepping, stop
      if (execSingleStep) {
         execState = execStopped;
      } else {
         // schedule it to keep running
         InstallTask( oneShotTask, 0 );
      }
   }
}

//
// ScriptExecutor::ResolveExpr
//
// Given a comma separated list of values and paramter names, this function 
// turns it into an array of tokens.  If a parameter name is found, the 
// parameter value is scanned recursively.
//

void ScriptExecutor::ResolveExpr( const char *expr, int exprLine, ScriptTokenList &lst, ScriptParmPtr * ppScriptParm /* = NULL */ )
{
   ScriptScanner  scan( expr );
   ScriptToken    tok;
   ScriptParmPtr  pp;

   // start at the beginning
   scan.Next( tok );
   if (tok.tokenType == scriptEOL)
      return;

   for (;;) {
      // check the value
      switch (tok.tokenType) {
         case scriptKeyword:
            // resolve the keyword as a parameter?
            if ((pp = execDoc->m_pParmList->LookupParm( tok.tokenSymbol )) != 0) {

               // We've found a variable... use it as the first.  All other variables don't matter
               // for the purposes of stuffing values INTO them.  Don't resolve parameter, it might
               // contain junk and that would confuse the assignment.  Just drop through as keyword.

               if ( ppScriptParm != NULL )
                  *ppScriptParm = pp;
               else
               {
                  // They haven't supplied a parm pointer so this must not be an assignment...
                  // we should resolve it and go through.
                  ResolveExpr( pp->parmValue, pp->parmLine, lst );
                  break;
               }
            }
            // Else, fall through?
            // TODO: Fall through here on purpose?
            // FIXME: If so, say so; otherwise, looks like a coding error.

         case scriptValue:
            // save the value
            lst.Append( tok );
            break;

         case scriptReference:
            // look up reference in EPICS database

            lst.Append( tok );
            break;

         default:
            throw ExecError( "Value expected", exprLine );
      }

      // check for EOL
      scan.Next( tok );
      if (tok.tokenType == scriptEOL)
         break;

      // check for more
      if ((tok.tokenType != scriptSymbol) || (tok.tokenSymbol != ','))
         throw ExecError( "End-of-line or comma expected in value list", exprLine );

      // get ready for next value
      scan.Next( tok );
   }

   // token list is built.  Now go through and resolve all of the index items on each token and
   // set that final index value into the top level token.

   // allow cases of {property[int]}, {property[var]}, {property[{property}]}, {property[{property[int]}]}, etc.
   // Don't allow cases (yet) of var[var], var[int], var[etc.]

   for ( int i = 0; i < lst.GetCount(); i++ )
      lst[i].ResolveIndex(execDoc->m_pParmList);
}




//
// ScriptExecutor::GetEPICSProperty
//
// Given a property keyword, this function will attempt to get the value of the 
// property from the EPICS database.
//  Throw a string exception if there is a problem

void ScriptExecutor::GetEPICSProperty( int prop, BACnetAnyValue * pbacnetAny, int nIndex /* = -1 */ )
{
   int                  pid, pindx;
   PICS::generic_object *pobj;

   // find the property
   pid = ScriptToken::Lookup( prop, ScriptPropertyMap );
   if (pid < 0)
      throw "No such property";

   // verify the context
   if (execObjID == 0xFFFFFFFF)
      throw "No object reference";

   // make sure there's something to search
   if (!gPICSdb)
      throw "No EPICS information loaded";

   // get a pointer to the object database record
   pobj = PICS::GetpObj( gPICSdb->Database, execObjID );
   if (!pobj)
      throw "Object not defined in EPICS";

   // get the index of the property flags
   pindx = PICS::GetPropIndex( pobj->object_type, pid );
   if (pindx < 0)
      throw "Property not defined for this object type";

   // see if the value is not available
   // madanner 11/6/02, commented.  Not sure the purpose of unknown values.  They're a good thing, no?

// if (pobj->propflags[pindx] & ValueUnknown)            
//    throw "EPICS property is not specified";

   // Access DUDAPI call to point to yucky part of EPICS internal data structures
   PICS::CreatePropertyFromEPICS( pobj, pid, pbacnetAny );

   // assign real value to this array pointer... doesn't matter here if it's an actual
   // array or not... just pretent.  Sure.  I knew you could.

   BACnetGenericArray * pbacnetarray = (BACnetGenericArray *) pbacnetAny->GetObject();

   if ( pbacnetarray == NULL )
      throw "Property value not known or comparison for this type not implemented";

   // see if caller really wants just an element here.  If so, (won't be -1), then
   // grab a copy of the element and destroy the rest (taken care of by resetting object).
   // only do this for list and array types.
   // ALL must decend from GenericArray

   if ( nIndex != -1  &&  pbacnetarray->IsKindOf(RUNTIME_CLASS(BACnetGenericArray)) )
   {
      if ( nIndex == 0 )            // return number of elements only
      {
         pbacnetAny->SetObject(ud, new BACnetUnsigned(pbacnetarray->GetSize()) );
      }
      else
      {
         if ( nIndex > pbacnetarray->GetSize() )         // out of bounds?  Remember index is base 1
         {
            CString str;
            str.Format(IDS_SCREX_INDEXBOUNDS, nIndex, pbacnetarray->GetSize());
            throw CString(str);
         }

         pbacnetAny->SetObject((*pbacnetarray)[nIndex-1].clone());
      }
   }
}

// GJB 24/12/2003
// get a device object's property value from EPICS database
void ScriptExecutor::GetEPICSDeviceProperty( int prop, BACnetAnyValue * pbacnetAny, int nIndex /* = -1 */ )
{
   //get device object ID, and execObjID is temporarily set to this deivceId, after Get the property, rollback
   unsigned long        objID;
   PICS::generic_object *pobj;
   
   objID = execObjID;
   // make sure there's something to search
   if (!gPICSdb)
      throw "No EPICS information loaded";
   pobj = PICS::GetpDeviceObj( gPICSdb->Database );
   if (!pobj)
      throw "Device Object not defined in EPICS";
   execObjID = pobj->object_id;
   GetEPICSProperty(prop, pbacnetAny, nIndex);
   execObjID = objID;
   
}

//
// ScriptExecutor::SendPacket
//
// Construct a packet out of the information in execPacket and send it.  Return true 
// if the send was successful.  The only time it should fail is if there is something 
// wrong with its construction (or something like the network missing).
//

bool ScriptExecutor::SendPacket( void )
{
   int                  bipMsgID = -1, nlMsgID = -1, alMsgID = -1;
   BACnetOctet          ctrl = 0;
   CByteArray           packet;
   ScriptFilterPtr      sfp;
   ScriptNetFilterPtr   nfp;
   ScriptPacketExprPtr  pNet, bipMsg, nlMsg, nlDA, alMsg
   ,                    pVersion, pDNET, pDADR, pHopCount, pSNET, pSADR
   ,                    pDER, pPriority;
   BACnetAddress        nlDestAddr;
   BACnetBoolean        nlDER;
   BACnetInteger        nlVersion, nlDNET, nlSNET
   ,                    nlPriority, nlHopCount;
   BACnetOctetString    nlDest, nlDADR, nlSADR, nlData, alData;
   BACnetCharacterString nlNetwork;

   try {
      // see if the network is provided
      pNet = GetKeywordValue( NULL, kwNETWORK, nlNetwork );

      // see if this has a BVLCI header
      bipMsg = execPacket->packetExprList.Find( kwBVLCI );

      // see if this is a network layer message
      nlMsg = execPacket->packetExprList.Find( kwMESSAGE );
      if (!nlMsg)
         nlMsg = execPacket->packetExprList.Find( kwMSG );

      // see if this is an application layer message
      alMsg = execPacket->packetExprList.Find( kwPDU );

      // see if this should be sent in the context of a device
      if (!pNet && !bipMsg && !nlMsg && alMsg) {
         SendDevPacket();
         return true;
      }

      // look for the network (actually the filter name)
      if (gMasterFilterList.Length() == 1) {
         sfp = gMasterFilterList[0];
         if (pNet && !nlNetwork.Equals(sfp->filterName))
            throw ExecError( "Port not found", pNet->exprLine );
      }
      else
      {
         int i;
         if (!pNet)
            throw ExecError( "Network required", execPacket->baseLineStart );

         for (i = 0; i < gMasterFilterList.Length(); i++) {
            sfp = gMasterFilterList[i];
            if (nlNetwork.Equals(sfp->filterName))
               break;
         }
         if (i >= gMasterFilterList.Length())
            throw ExecError( "Port not found", pNet->exprLine );
      }

      // make sure its a network filter for now
      if (sfp->filterType != scriptNPDUFilter)
         throw ExecError( "NPDU filter only", pNet->exprLine );
      nfp = (ScriptNetFilterPtr)sfp;

      // get the destination
      nlDA = execPacket->packetExprList.Find( kwDA );
      if (!nlDA) {
         if ( !execDB->LoadNamedAddress(&nlDestAddr, "IUT") )
            throw ExecError( "Default destination address IUT not found", execPacket->baseLineStart );
      } else {
         ScriptTokenList  daList;

         // resolve the expressions
         ResolveExpr( nlDA->exprValue, nlDA->exprLine, daList );
         if (daList.Length() < 1)
            throw ExecError( "Address, name or keyword expected", nlDA->exprLine );

         // get a reference to the first parameter
         const ScriptToken &t = daList[0];

         // check to see if this is a keyword
         if (t.tokenType == scriptKeyword)
         {
            if ((t.tokenSymbol == kwBROADCAST) || (t.tokenSymbol == kwLOCALBROADCAST))
            {
               nlDestAddr.LocalBroadcast();
            }
            else if ((t.tokenSymbol == kwGLOBALBROADCAST) || (t.tokenSymbol == kwGLOBAL_BROADCAST))
            {
               nlDestAddr.GlobalBroadcast();
            }
            else if (t.tokenSymbol == kwREMOTEBROADCAST) 
            {
               if (daList.Length() == 1)
                  throw ExecError( "DNET expected", nlDA->exprLine );

               const ScriptToken &dnet = daList[1];

               int valu;
               if (dnet.tokenType != scriptValue)
                  throw "DNET expected";
               if (!dnet.IsInteger(valu))
                  throw "DNET invalid format, integer required";
               if ((valu < 0) || (valu > 65534))
                  throw "DNET out of range (0..65534)";

               nlDestAddr.RemoteBroadcast( valu );
            } 
            else
            {
               throw ExecError( "Unrecognized keyword", nlDA->exprLine );
            }
         } else
         // it might be a name
         if ((t.tokenType == scriptValue) && (t.tokenEnc == scriptASCIIEnc)) {
            if ( !execDB->LoadNamedAddress(&nlDestAddr, t.RemoveQuotes()) )
               throw ExecError( "Destination address name not found", nlDA->exprLine );
         } else
         // it might be an IP address
         if ((t.tokenType == scriptValue) && (t.tokenEnc == scriptIPEnc)) {
            BACnetIPAddr nlIPAddr( t.tokenValue );
            nlDestAddr = nlIPAddr;
         } else
         // it might be an explicit octet string
         if (t.IsEncodeable( nlDest )) {
            nlDestAddr.LocalStation( nlDest.strBuff, nlDest.strLen );
         } else
            throw ExecError( "Destination address expected", nlDA->exprLine );
      }

      // get some interesting keywords in case they should be used in BIP messages
      pDER     = GetKeywordValue( NULL, kwDER, nlDER );
      pPriority   = GetKeywordValue( NULL, kwPRIORITY, nlPriority );
      if (!pPriority)
         pPriority = GetKeywordValue( NULL, kwPRIO, nlPriority );
      if (!pPriority)
         nlPriority.intValue = 0;
      else
      if ((nlPriority.intValue < 0) || (nlPriority.intValue > 3))
         throw ExecError( "Priority out of range 0..3", pPriority->exprLine );

      // if this is a BVLL specific message, encode and send it
      if (bipMsg) {
         ScriptTokenList   bvllList;

         // resolve the expressions
         ResolveExpr( bipMsg->exprValue, bipMsg->exprLine, bvllList );
         if (bvllList.Length() < 1)
            throw ExecError( "BIP message keyword expected", bipMsg->exprLine );

         // get a reference to the first parameter
         const ScriptToken &t = bvllList[0];

         // check to see if this is a keyword
         if (t.tokenType == scriptKeyword) {
            bipMsgID = t.Lookup( t.tokenSymbol, ScriptBIPMsgTypeMap );
            if (bipMsgID < 0)
               throw ExecError( "Unrecognized keyword", bipMsg->exprLine );
         } else
            throw ExecError( "Keyword expected", bipMsg->exprLine );

         // based on the number, check for other parameters
         try {
            // encode the message type and function
            packet.Add( 0x81 );
            packet.Add( bipMsgID );

            // hold space for the length
            packet.Add( 0x00 );
            packet.Add( 0x00 );

            switch (bipMsgID) {
               case 0:                 // BVLC-RESULT, rslt
                  SendBVLCResult( bvllList, packet );
                  break;
               case 1:                 // WRITE-BROADCAST-DISTRIBUTION-TABLE {,host/net:port}*
                  SendWriteBDT( bvllList, packet );
                  break;
               case 2:                 // READ-BROADCAST-DISTRIBUTION-TABLE
                  if (bvllList.Length() != 1)
                     throw "BVLC function takes no parameters";
                  break;
               case 3:                 // READ-BROADCAST-DISTRIBUTION-TABLE-ACK {,host/net:port}*
                  SendReadBDTAck( bvllList, packet );
                  break;
               case 4:                 // FORWARDED-NPDU, host:port
                  SendForwardedNPDU( bvllList, packet );
                  break;
               case 5:                 // REGISTER-FOREIGN-DEVICE, ttl
                  SendRegisterFD( bvllList, packet );
                  break;
               case 6:                 // READ-FOREIGN-DEVICE-TABLE
                  if (bvllList.Length() != 1)
                     throw "BVLC function takes no parameters";
                  break;
               case 7:                 // READ-FOREIGN-DEVICE-TABLE-ACK {,host:port,ttl,tr}*
                  SendReadFDTAck( bvllList, packet );
                  break;
               case 8:                 // DELETE-FOREIGN-DEVICE-TABLE-ENTRY
                  SendDeleteFDTEntry( bvllList, packet );
                  break;
               case 9:                 // DISTRIBUTE-BROADCAST-TO-NETWORK
               case 10:                // ORIGINAL-UNICAST-NPDU
               case 11:                // ORIGINAL-BROADCAST-NPDU
                  if (bvllList.Length() != 1)
                     throw "BVLC function takes no parameters";
                  break;
            }

            // clean up the length
            int len = packet.GetSize();
            packet[2] = (len >> 8);
            packet[3] = (len & 0xFF);
         }
         catch (const char *errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, bipMsg->exprLine );
         }
         catch (const CString &errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, bipMsg->exprLine );
         }

         // if this doesn't have an NPDU that follows, send it
         if ((bipMsgID != 4) && (bipMsgID != 9) && (bipMsgID != 10) && (bipMsgID != 11)) {
            nfp->Request(
               BACnetNPDU( nlDestAddr
                  , packet.GetData(), packet.GetSize()
                  , (pDER && nlDER.boolValue ? 1 : 0)
                  , nlPriority.intValue
                  )
               );

            // success
            return true;
         }
      }

      // there must be a network or application layer message
      if (!nlMsg && !alMsg)
         throw "Network or application message keyword required";
      if (nlMsg && alMsg)
         throw "Specify network or application message but not both";

      // find the interesting keywords
      pVersion = GetKeywordValue( NULL, kwVERSION, nlVersion );
      pDNET    = GetKeywordValue( NULL, kwDNET, nlDNET );
      pDADR    = GetKeywordValue( NULL, kwDADR, nlDADR );
      pHopCount   = GetKeywordValue( NULL, kwHOPCOUNT, nlHopCount );
      pSNET    = GetKeywordValue( NULL, kwSNET, nlSNET );
      pSADR    = GetKeywordValue( NULL, kwSADR, nlSADR );

      // validate version
      if (pVersion && ((nlVersion.intValue < 0) || (nlVersion.intValue > 255)))
         throw ExecError( "Version out of range 0..255", pVersion->exprLine );

      if (pDNET || pDADR) {
         if (!pDNET)
            throw ExecError( "Destination network required (65535 = global broadcast)", pDADR->exprLine );
         if ((nlDNET.intValue < 0) || (nlDNET.intValue > 65535))
            throw ExecError( "Destination network out of range 0..65535", pDNET->exprLine );
//       if ((nlDNET.intValue != 65535) && ((!pDADR) || (nlDADR.strLen == 0)))
//          throw ExecError( "DADR required when DNET is not global broadcast", pDNET->exprLine );
         if (!pHopCount) {
            Msg( 3, execPacket->baseLineStart, "Hopcount defaulting to 255" );
            nlHopCount.intValue = 255;
         } else
         if ((nlHopCount.intValue < 0) || (nlHopCount.intValue > 255))
            throw ExecError( "Hop count out of range 0..255", pHopCount->exprLine );
      }
      if (pSNET || pSADR) {
         if (!pSNET)
            throw ExecError( "Source network required", pSADR->exprLine );
         if ((nlSNET.intValue < 0) || (nlSNET.intValue > 65534))
            throw ExecError( "Source network out of range 0..65534", pSNET->exprLine );
         if ((!pSADR) || (nlSADR.strLen == 0))
            throw ExecError( "SADR required when SNET is present", pSNET->exprLine );
      }

      // start with the version
      packet.Add( pVersion ? nlVersion.intValue : 1 );

      // build the control field
      if (nlMsg)
         ctrl |= 0x80;
      if (pDNET)
         ctrl |= 0x20;
      if (pSNET)
         ctrl |= 0x08;
      if (pDER && nlDER.boolValue)
         ctrl |= 0x04;
      ctrl |= nlPriority.intValue;

      // add the control field
      packet.Add( ctrl );

      // add the destination stuff
      if (pDNET) {
         packet.Add( nlDNET.intValue >> 8 );
         packet.Add( nlDNET.intValue & 0x0FF );
         if (pDADR) 
         {
            packet.Add( nlDADR.strLen );
            for (int i = 0; i < nlDADR.strLen; i++)
               packet.Add( nlDADR.strBuff[i] );
         } else
            packet.Add( 0x00 );
      }

      // add the source stuff
      if (pSNET) 
      {
         packet.Add( nlSNET.intValue >> 8 );
         packet.Add( nlSNET.intValue & 0x0FF );
         packet.Add( nlSADR.strLen );
         for (int i = 0; i < nlSADR.strLen; i++)
            packet.Add( nlSADR.strBuff[i] );
      }

      // add the hop count
      if (pDNET)
         packet.Add( nlHopCount.intValue );

      // if this is a network layer message, encode it
      if (nlMsg) {
         ScriptTokenList   nlList;

         // resolve the expressions
         ResolveExpr( nlMsg->exprValue, nlMsg->exprLine, nlList );
         if (nlList.Length() < 1)
            throw ExecError( "NL message keyword expected", nlMsg->exprLine );

         // get a reference to the first parameter
         const ScriptToken &t = nlList[0];

         // check to see if this is a keyword
         if (t.tokenType == scriptKeyword) {
            nlMsgID = t.Lookup( t.tokenSymbol, ScriptNLMsgTypeMap );
            if (nlMsgID < 0)
               throw ExecError( "Unrecognized keyword", nlMsg->exprLine );
         } else
         // it might be an explicit number
         if (t.IsInteger( nlMsgID )) {
            if ((nlMsgID < 0) || (nlMsgID > 255))
               throw ExecError( "Network message number out of range (0..255)", nlMsg->exprLine );
         } else
            throw ExecError( "Keyword or network message expected", nlMsg->exprLine );

         // add the message to the packet
         packet.Add( nlMsgID );

         // based on the number, check for other parameters
         try {
            switch (nlMsgID) {
               case 0:                 // WHO-IS-ROUTER-TO-NETWORK
                  SendWhoIsRouterToNetwork( nlList, packet );
                  break;
               case 1:                 // I-AM-ROUTER-TO-NETWORK
                  SendIAmRouterToNetwork( nlList, packet );
                  break;
               case 2:                 // I-COULD-BE-ROUTER-TO-NETWORK
                  SendICouldBeRouterToNetwork( nlList, packet );
                  break;
               case 3:                 // REJECT-MESSAGE-TO-NETWORK
                  SendRejectMessageToNetwork( nlList, packet );
                  break;
               case 4:                 // ROUTER-BUSY-TO-NETWORK
                  SendRouterBusyToNetwork( nlList, packet );
                  break;
               case 5:                 // ROUTER-AVAILABLE-TO-NETWORK
                  SendRouterAvailableToNetwork( nlList, packet );
                  break;
               case 6:                 // INITIALIZE-ROUTING-TABLE
               case 7:                 // INITIALIZE-ROUTING-TABLE-ACK
                  SendInitializeRoutingTable( nlList, packet );
                  break;
               case 8:                 // ESTABLISH-CONNECTION-TO-NETWORK
                  SendEstablishConnectionToNetwork( nlList, packet );
                  break;
               case 9:                 // DISCONNECT-CONNECTION-TO-NETWORK
                  SendDisconnectConnectionToNetwork( nlList, packet );
                  break;
            }
         }
         catch (const char *errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, nlMsg->exprLine );
         }
         catch (const CString &errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, bipMsg->exprLine );
         }

         // look for optional network layer data
         if (GetKeywordValue( NULL, kwNLDATA, nlData ) || GetKeywordValue( NULL, kwNL, nlData )) {
            TRACE( "Got additional network layer octet string\n" );
            for (int i = 0; i < nlData.strLen; i++)
               packet.Add( nlData.strBuff[i] );
         }
      }

      // if this is an application layer message, encode it
      if (alMsg) {
         ScriptTokenList   alList;

         // force property references to fail until context established
         execObjID = 0xFFFFFFFF;

         // resolve the expressions
         ResolveExpr( alMsg->exprValue, alMsg->exprLine, alList );
         if (alList.Length() < 1)
            throw ExecError( "AL message keyword expected", alMsg->exprLine );

         // get a reference to the first parameter
         const ScriptToken &t = alList[0];

         // check to see if this is a keyword
         if (t.tokenType == scriptKeyword) {
            alMsgID = t.Lookup( t.tokenSymbol, ScriptALMsgTypeMap );
            if (alMsgID < 0)
               throw ExecError( "Unrecognized keyword", alMsg->exprLine );
         } else
            throw ExecError( "Keyword expected", alMsg->exprLine );

         // based on the number, check for other parameters
         try {
            switch (alMsgID) {
               case 0:                 // CONFIRMED-REQUEST
                  SendConfirmedRequest( packet );
                  break;
               case 1:                 // UNCONFIRMED-REQUEST
                  SendUnconfirmedRequest( packet );
                  break;
               case 2:                 // SIMPLEACK
                  SendSimpleACK( packet );
                  break;
               case 3:                 // COMPLEXACK
                  SendComplexACK( packet );
                  break;
               case 4:                 // SEGMENTACK
                  SendSegmentACK( packet );
                  break;
               case 5:                 // ERROR
                  SendError( packet );
                  break;
               case 6:                 // REJECT
                  SendReject( packet );
                  break;
               case 7:                 // ABORT
                  SendAbort( packet );
                  break;
            }
         }
         catch (const char *errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, alMsg->exprLine );
         }
         catch (const CString &errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, alMsg->exprLine );
         }
      }

      // check to see if there was a BVCL prefix
      if ((bipMsgID == 4) || (bipMsgID == 9) || (bipMsgID == 10) || (bipMsgID == 11)) {
         // clean up the length
         int len = packet.GetSize();
         packet[2] = (len >> 8);
         packet[3] = (len & 0xFF);
      }

      // send it
      nfp->Request(
         BACnetNPDU( nlDestAddr
            , packet.GetData(), packet.GetSize()
            , (pDER && nlDER.boolValue ? 1 : 0)
            , nlPriority.intValue
            )
         );
   }
   catch (const ExecError &err) {
      // failed
      Msg( 3, err.errLineNo, err.errMsg );
      return false;
   }
   catch (const char *errMsg) {
      // failed
      Msg( 3, -1, errMsg );
      return false;
   }

   // success
   return true;
}

//
// ScriptExecutor::SendBVLCResult
//

void ScriptExecutor::SendBVLCResult( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;

   TRACE( "SendBVLCResult\n" );

   switch (tlist.Length()) {
      case 1:
         throw "Result code expected";
      case 2:
         {
            const ScriptToken &dnet = tlist[1];

            if (dnet.tokenType != scriptValue)
               throw "Result code expected";
            if (!dnet.IsInteger(valu))
               throw "Result code invalid format";
            if ((valu < 0) || (valu > 65535))
               throw "Result code out of range (0..65535)";

            TRACE1( "    Result code %d\n", valu );
            packet.Add( valu >> 8 );
            packet.Add( valu & 0x0FF );
            break;
         }
      default:
         throw "Too many values in BVLC-Result message";
   }
}

//
// ScriptExecutor::SendWriteBDT
//

void ScriptExecutor::SendWriteBDT( ScriptTokenList &tlist, CByteArray &packet )
{
   unsigned long     host, mask;
   unsigned short    port;

   TRACE0( "SendWriteBDT\n" );

   for (int i = 1; i < tlist.Length(); i++ ) {
      const ScriptToken &t = tlist[i];

      if (t.tokenType != scriptValue)
         throw "IP address expected";
      if (t.tokenEnc != scriptIPEnc)
         throw "IP address required";

      // convert to host, port and network mask
      BACnetIPAddr::StringToHostPort( t.tokenValue, &host, &mask, &port );

      // encode the host
      for (int j = 3; j >= 0; j--)
         packet.Add( (unsigned char)((host >> (j * 8)) & 0xFF) );

      // encode the port
      packet.Add( port >> 8 );
      packet.Add( port & 0xFF );

      // encode the mask
      for (int k = 3; k >= 0; k--)
         packet.Add( (unsigned char)((mask >> (k * 8)) & 0xFF) );
   }
}

//
// ScriptExecutor::SendReadBDTAck
//

void ScriptExecutor::SendReadBDTAck( ScriptTokenList &tlist, CByteArray &packet )
{
   unsigned long     host, mask;
   unsigned short    port;

   TRACE0( "SendReadBDTAck\n" );

   for (int i = 1; i < tlist.Length(); i++ ) {
      const ScriptToken &t = tlist[i];

      if (t.tokenType != scriptValue)
         throw "IP address expected";
      if (t.tokenEnc != scriptIPEnc)
         throw "IP address required";

      // convert to host, port and network mask
      BACnetIPAddr::StringToHostPort( t.tokenValue, &host, &mask, &port );

      // encode the host
      for (int j = 3; j >= 0; j--)
         packet.Add( (unsigned char)((host >> (j * 8)) & 0xFF) );

      // encode the port
      packet.Add( port >> 8 );
      packet.Add( port & 0xFF );

      // encode the mask
      for (int k = 3; k >= 0; k--)
         packet.Add( (unsigned char)((mask >> (k * 8)) & 0xFF) );
   }
}

//
// ScriptExecutor::SendForwardedNPDU
//

void ScriptExecutor::SendForwardedNPDU( ScriptTokenList &tlist, CByteArray &packet )
{
   TRACE0( "SendForwardedNPDU\n" );

   switch (tlist.Length()) {
      case 1:
         throw "IP address required";
      case 2:
         {
            unsigned long     host, mask;
            unsigned short    port;

            const ScriptToken &t = tlist[1];

            if (t.tokenType != scriptValue)
               throw "IP address expected";
            if (t.tokenEnc != scriptIPEnc)
               throw "IP address required";

            // convert to host, port and network mask
            BACnetIPAddr::StringToHostPort( t.tokenValue, &host, &mask, &port );

            // encode the host
            for (int j = 3; j >= 0; j--)
               packet.Add( (unsigned char)((host >> (j * 8)) & 0xFF) );

            // encode the port
            packet.Add( port >> 8 );
            packet.Add( port & 0xFF );

            break;
         }
      default:
         throw "Too many values";
   }
}

//
// ScriptExecutor::SendRegisterFD
//

void ScriptExecutor::SendRegisterFD( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;

   TRACE0( "SendRegisterFD\n" );

   switch (tlist.Length()) {
      case 1:
         throw "Time-to-live required";
      case 2:
         {
            const ScriptToken &ttl = tlist[1];

            if (ttl.tokenType != scriptValue)
               throw "Time-to-live expected";
            if (!ttl.IsInteger(valu))
               throw "Time-to-live invalid format";
            if ((valu < 0) || (valu > 65535))
               throw "Time-to-live out of range (0..65535)";

            TRACE1( "    TTL %d\n", valu );
            packet.Add( valu >> 8 );
            packet.Add( valu & 0x0FF );
            break;
         }
      default:
         throw "Too many values";
   }
}

//
// ScriptExecutor::SendReadFDTAck
//

void ScriptExecutor::SendReadFDTAck( ScriptTokenList &tlist, CByteArray &packet )
{
   int                  valu;

   TRACE0( "SendReadFDTAck\n" );

   for (int i = 1; i < tlist.Length(); ) {
      unsigned long     host, mask;
      unsigned short    port;

      const ScriptToken &t = tlist[i++];

      if (t.tokenType != scriptValue)
         throw "IP address expected";
      if (t.tokenEnc != scriptIPEnc)
         throw "IP address required";

      // convert to host, port and network mask
      BACnetIPAddr::StringToHostPort( t.tokenValue, &host, &mask, &port );

      // encode the host
      for (int j = 3; j >= 0; j--)
         packet.Add( (unsigned char)((host >> (j * 8)) & 0xFF) );

      // encode the port
      packet.Add( port >> 8 );
      packet.Add( port & 0xFF );

      if (i >= tlist.Length())
         throw "Time-to-live expected";
      const ScriptToken &ttl = tlist[i++];

      if (ttl.tokenType != scriptValue)
         throw "Time-to-live expected";
      if (!ttl.IsInteger(valu))
         throw "Time-to-live invalid format";

      TRACE1( "    Time-to-live %d\n", valu );

      // encode the value
      packet.Add( valu >> 8 );
      packet.Add( valu & 0xFF );

      if (i >= tlist.Length())
         throw "Time remaining expected";
      const ScriptToken &tr = tlist[i++];

      if (tr.tokenType != scriptValue)
         throw "Time remaining expected";
      if (!tr.IsInteger(valu))
         throw "Time remaining invalid format";

      TRACE1( "    Time remaining %d\n", valu );

      // encode the value
      packet.Add( valu >> 8 );
      packet.Add( valu & 0xFF );
   }
}

//
// ScriptExecutor::SendDeleteFDTEntry
//

void ScriptExecutor::SendDeleteFDTEntry( ScriptTokenList &tlist, CByteArray &packet )
{
   TRACE0( "SendDeleteFDTEntry\n" );

   switch (tlist.Length()) {
      case 1:
         throw "Foreign device IP address required";
      case 2:
         {
            unsigned long     host, mask;
            unsigned short    port;

            const ScriptToken &t = tlist[1];

            if (t.tokenType != scriptValue)
               throw "IP address expected";
            if (t.tokenEnc != scriptIPEnc)
               throw "IP address required";

            // convert to host, port and network mask
            BACnetIPAddr::StringToHostPort( t.tokenValue, &host, &mask, &port );

            // encode the host
            for (int j = 3; j >= 0; j--)
               packet.Add( (unsigned char)((host >> (j * 8)) & 0xFF) );

            // encode the port
            packet.Add( port >> 8 );
            packet.Add( port & 0xFF );

            break;
         }
      default:
         throw "Too many values";
   }
}

//
// ScriptExecutor::SendWhoIsRouterToNetwork
//

void ScriptExecutor::SendWhoIsRouterToNetwork( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;

   TRACE0( "SendWhoIsRouterToNetwork\n" );

   switch (tlist.Length()) {
      case 1:
         break;
      case 2:
         {
            const ScriptToken &dnet = tlist[1];

            if (dnet.tokenType != scriptValue)
               throw "DNET expected";
            if (!dnet.IsInteger(valu))
               throw "DNET invalid format";
            if ((valu < 0) || (valu > 65534))
               throw "DNET out of range (0..65534)";

            TRACE1( "    Network %d\n", valu );
            packet.Add( valu >> 8 );
            packet.Add( valu & 0x0FF );
            break;
         }
      default:
         throw "Too many values in Who-Is-Router-To-Network message";
   }
}

//
// ScriptExecutor::SendIAmRouterToNetwork
//

void ScriptExecutor::SendIAmRouterToNetwork( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;

   TRACE0( "SendIAmRouterToNetwork\n" );

   if (tlist.Length() == 1)
      throw "At least one network number required";

   for (int i = 1; i < tlist.Length(); i++ ) {
      const ScriptToken &t = tlist[i];

      if (t.tokenType != scriptValue)
         throw "Network number expected";
      if (!t.IsInteger(valu))
         throw "Network number invalid format";
      if ((valu < 0) || (valu > 65534))
         throw "DNET out of range (0..65534)";

      TRACE1( "    Network %d\n", valu );
      packet.Add( valu >> 8 );
      packet.Add( valu & 0x0FF );
   }
}

//
// ScriptExecutor::SendICouldBeRouterToNetwork
//

void ScriptExecutor::SendICouldBeRouterToNetwork( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;

   TRACE0( "SendICouldBeRouterToNetwork\n" );

   if (tlist.Length() != 3)
      throw "DNET and performance index required";

   const ScriptToken &dnet = tlist[1];

   if (dnet.tokenType != scriptValue)
      throw "DNET expected";
   if (!dnet.IsInteger(valu))
      throw "DNET invalid format";
   if ((valu < 0) || (valu > 65534))
      throw "DNET out of range (0..65534)";

   TRACE1( "    Network %d\n", valu );
   packet.Add( valu >> 8 );
   packet.Add( valu & 0x0FF );

   const ScriptToken &perf = tlist[2];

   if (perf.tokenType != scriptValue)
      throw "Performance index expected";
   if (!perf.IsInteger(valu))
      throw "Performance index invalid format";
   if ((valu < 0) || (valu > 255))
      throw "Performance index out of range (0..255)";

   TRACE1( "    Performance %d\n", valu );
   packet.Add( valu );
}

//
// ScriptExecutor::SendRejectMessageToNetwork
//

void ScriptExecutor::SendRejectMessageToNetwork( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;

   TRACE0( "SendRejectMessageToNetwork\n" );

   if (tlist.Length() != 3)
      throw "Reject reason and DNET required";

   const ScriptToken &reason = tlist[1];

   if (reason.tokenType != scriptValue)
      throw "Reject reason expected";
   if (!reason.IsInteger(valu))
      throw "Reject reason invalid format";
   if ((valu < 0) || (valu > 255))
      throw "Reject reason out of range (0..255)";

   TRACE1( "    Reject Reason %d\n", valu );
   packet.Add( valu );

   const ScriptToken &dnet = tlist[2];

   if (dnet.tokenType != scriptValue)
      throw "DNET expected";
   if (!dnet.IsInteger(valu))
      throw "DNET invalid format";
   if ((valu < 0) || (valu > 65534))
      throw "DNET out of range (0..65534)";

   TRACE1( "    Network %d\n", valu );
   packet.Add( valu >> 8 );
   packet.Add( valu & 0x0FF );
}

//
// ScriptExecutor::SendRouterBusyToNetwork
//

void ScriptExecutor::SendRouterBusyToNetwork( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;
   
   TRACE0( "SendRouterBusyToNetwork\n" );
   
   for (int i = 1; i < tlist.Length(); i++ ) {
      const ScriptToken &dnet = tlist[i];

      if (dnet.tokenType != scriptValue)
         throw "Network expected";
      if (!dnet.IsInteger(valu))
         throw "Network invalid format";
      if ((valu < 0) || (valu > 65534))
         throw "Network out of range (0..65534)";

      TRACE1( "    Network %d\n", valu );
      packet.Add( valu >> 8 );
      packet.Add( valu & 0x0FF );
   }
}

//
// ScriptExecutor::SendRouterAvailableToNetwork
//

void ScriptExecutor::SendRouterAvailableToNetwork( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;
   
   TRACE0( "SendRouterAvailableToNetwork\n" );

   for (int i = 1; i < tlist.Length(); i++ ) {
      const ScriptToken &dnet = tlist[i];

      if (dnet.tokenType != scriptValue)
         throw "DNET expected";
      if (!dnet.IsInteger(valu))
         throw "DNET invalid format";
      if ((valu < 0) || (valu > 65534))
         throw "DNET out of range (0..65534)";

      TRACE1( "    Network %d\n", valu );
      packet.Add( valu >> 8 );
      packet.Add( valu & 0x0FF );
   }
}

//
// ScriptExecutor::SendInitializeRoutingTable
//

void ScriptExecutor::SendInitializeRoutingTable( ScriptTokenList &tlist, CByteArray &packet )
{
   int                  valu;
   BACnetCharacterString   cstr;
   BACnetOctetString     ostr;

   TRACE0( "SendInitializeRoutingTable[Ack]\n" );

   // append the number of ports
   packet.Add( (tlist.Length() - 1) / 3 );

   for (int i = 1; i < tlist.Length(); ) {
      const ScriptToken &dnet = tlist[i++];

      if (dnet.tokenType != scriptValue)
         throw "DNET expected";
      if (!dnet.IsInteger(valu))
         throw "DNET invalid format";
      if ((valu < 0) || (valu > 65534))
         throw "DNET out of range (0..65534)";

      TRACE1( "    Network %d\n", valu );
      packet.Add( valu >> 8 );
      packet.Add( valu & 0x0FF );

      if (i >= tlist.Length())
         throw "Port ID expected";
      const ScriptToken &portID = tlist[i++];

      if (portID.tokenType != scriptValue)
         throw "Port ID expected";
      if (!portID.IsInteger(valu))
         throw "Port ID invalid format";

      TRACE1( "    Port ID %d\n", valu );
      packet.Add( valu );

      if (i >= tlist.Length())
         throw "Port information expected";
      const ScriptToken &portInfo = tlist[i++];

      if (portInfo.tokenType != scriptValue)
         throw "Port information expected";

      // Original code demanded character string.
      // But BACnet 6.4.7 says this is an octet string.
      // We now accept either one.
      
      // Test octet-string first, as BACnetCharacterString will accept hex but encode it wrong
      if (portInfo.IsEncodeable( ostr )) {
         packet.Add( ostr.Length() );
         for (int j = 0; j < ostr.Length(); j++) {
            packet.Add( ostr[j] );
         }
      }
      else if (portInfo.IsEncodeable( cstr )) {
         if (cstr.strEncoding != 0)
            throw "Port information must be ASCII encoded";

         packet.Add( cstr.strLen );
         for (unsigned int j = 0; j < cstr.strLen; j++) {
            packet.Add( cstr.strBuff[j] );
         }
      }
      else {
         throw "Port information invalid format: must be ASCII or octet-string";
      }
   }
}

//
// ScriptExecutor::SendEstablishConnectionToNetwork
//

void ScriptExecutor::SendEstablishConnectionToNetwork( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;

   TRACE0( "SendEstablishConnectionToNetwork\n" );

   if (tlist.Length() != 3)
      throw "DNET and termination time required";

   const ScriptToken &dnet = tlist[1];

   if (dnet.tokenType != scriptValue)
      throw "DNET expected";
   if (!dnet.IsInteger(valu))
      throw "DNET invalid format";
   if ((valu < 0) || (valu > 65534))
      throw "DNET out of range (0..65534)";

   TRACE1( "    Network %d\n", valu );
   packet.Add( valu >> 8 );
   packet.Add( valu & 0x0FF );

   const ScriptToken &term = tlist[2];

   if (term.tokenType != scriptValue)
      throw "Termination time expected";
   if (!term.IsInteger(valu))
      throw "Termination time invalid format";

   TRACE1( "    Termination time %d\n", valu );
   packet.Add( valu );
}

//
// ScriptExecutor::SendDisconnectConnectionToNetwork
//

void ScriptExecutor::SendDisconnectConnectionToNetwork( ScriptTokenList &tlist, CByteArray &packet )
{
   int      valu;

   TRACE0( "SendDisconnectConnectionToNetwork\n" );

   if (tlist.Length() != 2)
      throw "DNET required";

   const ScriptToken &dnet = tlist[1];

   if (dnet.tokenType != scriptValue)
      throw "DNET expected";
   if (!dnet.IsInteger(valu))
      throw "DNET invalid format";
   if ((valu < 0) || (valu > 65534))
      throw "DNET out of range (0..65534)";

   TRACE1( "    Network %d\n", valu );
   packet.Add( valu >> 8 );
   packet.Add( valu & 0x0FF );
}

//
// ScriptExecutor::SendDevPacket
//

void ScriptExecutor::SendDevPacket( void )
{
   int                  alMsgID, valu;
   BACnetAPDU           apdu;
   BACnetOctetString    nlDest;
   BACnetAddress        nlDestAddr;
   BACnetBoolean        nlDER;
   BACnetInteger        nlPriority, nlDNET;
   ScriptPacketExprPtr  nlDA, alMsg, pDER, pPriority;
   ScriptTokenList      alList;

   // we already know this is an application layer message
   alMsg = execPacket->packetExprList.Find( kwPDU );

   // make sure we have a device
   if ( execDB->GetDevices()->GetSize() == 0)
      throw ExecError( "No defined devices", alMsg->exprLine );

   // make it an error to have more than one for now
   if ( execDB->GetDevices()->GetSize() > 1)
      throw ExecError( "Too many defined devices, there can be only one", alMsg->exprLine );

   // get the destination.  The IUT will be the default for all outbound
   // messages, regardless of the message type (for example, a message that 
   // would normally be sent as a global broadcast will be sent directly to
   // the IUT).
   nlDA = execPacket->packetExprList.Find( kwDA );
   if (!nlDA)
   {
      if ( !execDB->LoadNamedAddress(&nlDestAddr, "IUT") )
         throw ExecError( "Default destination address IUT not found", execPacket->baseLineStart );
   } 
   else 
   {
      ScriptTokenList   daList;

      // resolve the expressions
      ResolveExpr( nlDA->exprValue, nlDA->exprLine, daList );
      if (daList.Length() < 1)
         throw ExecError( "Address, name or keyword expected", nlDA->exprLine );

      // get a reference to the first parameter
      const ScriptToken &t = daList[0];

      // check to see if this is a keyword
      if (t.tokenType == scriptKeyword)
      {
         if ((t.tokenSymbol == kwBROADCAST) || (t.tokenSymbol == kwLOCALBROADCAST))
         {
            nlDestAddr.LocalBroadcast();
         }
         else if ((t.tokenSymbol == kwGLOBALBROADCAST) || (t.tokenSymbol == kwGLOBAL_BROADCAST))
         {
            nlDestAddr.GlobalBroadcast();
         }
         else if (t.tokenSymbol == kwREMOTEBROADCAST)
         {
            if (daList.Length() == 1)
               throw ExecError( "DNET expected", nlDA->exprLine );

            const ScriptToken &dnet = daList[1];

            if (dnet.tokenType != scriptValue)
               throw "DNET expected";
            if (!dnet.IsInteger(valu))
               throw "DNET invalid format, integer required";
            if ((valu < 0) || (valu > 65534))
               throw "DNET out of range (0..65534)";

            nlDestAddr.RemoteBroadcast( valu );
         }
         else
         {
            throw ExecError( "Unrecognized keyword", nlDA->exprLine );
         }
      }
      else if (daList.Length() == 1)
      {
         // it might be a name
         if ((t.tokenType == scriptValue) && (t.tokenEnc == scriptASCIIEnc))
         {
            if ( !execDB->LoadNamedAddress(&nlDestAddr, t.RemoveQuotes()) )
               throw ExecError( "Destination address name not found", nlDA->exprLine );
         } 
         else if ((t.tokenType == scriptValue) && (t.tokenEnc == scriptIPEnc))
         {
            // it might be an IP address
            BACnetIPAddr nlIPAddr( t.tokenValue );
            nlDestAddr = nlIPAddr;
         }
         else if (t.IsEncodeable( nlDest ))
         {
            // it might be an explicit octet string
            nlDestAddr.LocalStation( nlDest.strBuff, nlDest.strLen );
         }
         else
         {
            throw ExecError( "Destination address expected", nlDA->exprLine );
         }
      }
      else if (daList.Length() == 2)
      {
         if (t.tokenType != scriptValue)
            throw "DNET expected";
         if (!t.IsInteger(valu))
            throw "DNET invalid format, integer required";
         if ((valu < 0) || (valu > 65534))
            throw "DNET out of range (0..65534)";

         const ScriptToken &dadr = daList[1];

         // it might be an IP address
         if ((dadr.tokenType == scriptValue) && (dadr.tokenEnc == scriptIPEnc))
         {
            BACnetIPAddr nlIPAddr( dadr.tokenValue );
            nlDestAddr = nlIPAddr;
         }
         else if (dadr.IsEncodeable( nlDest )) 
         {
            // it might be an explicit octet string
            nlDestAddr.LocalStation( nlDest.strBuff, nlDest.strLen );
         }
         else
         {
            throw ExecError( "Destination address expected", nlDA->exprLine );
         }

         nlDestAddr.addrType = remoteStationAddr;
         nlDestAddr.addrNet = valu;
      }
      else
      {
         throw ExecError( "Destination address expected", nlDA->exprLine );
      }
   }

   // copy the destination address into the apdu
   apdu.apduAddr = nlDestAddr;

   // force property references to fail until context established
   execObjID = 0xFFFFFFFF;

   // resolve the expressions
   ResolveExpr( alMsg->exprValue, alMsg->exprLine, alList );
   if (alList.Length() < 1)
      throw ExecError( "AL message keyword expected", alMsg->exprLine );

   // get a reference to the first parameter
   const ScriptToken &t = alList[0];

   // check to see if this is a keyword
   if (t.tokenType == scriptKeyword) 
   {
      alMsgID = t.Lookup( t.tokenSymbol, ScriptALMsgTypeMap );
      if (alMsgID < 0)
         throw ExecError( "Unrecognized keyword", alMsg->exprLine );
   } 
   else
   {
      throw ExecError( "Keyword expected", alMsg->exprLine );
   }

   // based on the number, check for other parameters
   try {
      switch (alMsgID) {
         case 0:                 // CONFIRMED-REQUEST
            SendDevConfirmedRequest( apdu );
            break;
         case 1:                 // UNCONFIRMED-REQUEST
            SendDevUnconfirmedRequest( apdu );
            break;
         case 2:                 // SIMPLEACK
            SendDevSimpleACK( apdu );
            break;
         case 3:                 // COMPLEXACK
            SendDevComplexACK( apdu );
            break;
         case 4:                 // SEGMENTACK
            SendDevSegmentACK( apdu );
            break;
         case 5:                 // ERROR
            SendDevError( apdu );
            break;
         case 6:                 // REJECT
            SendDevReject( apdu );
            break;
         case 7:                 // ABORT
            SendDevAbort( apdu );
            break;
      }
   }
   catch (const char *errMsg) {
      // one of the functions had an error
      throw ExecError( errMsg, alMsg->exprLine );
   }
   catch (const CString &errMsg) {
      // one of the functions had an error
      throw ExecError( errMsg, alMsg->exprLine );
   }

   // get some interesting keywords that can override the defaults
   pDER = GetKeywordValue( NULL, kwDER, nlDER );
   if (pDER)
      apdu.apduExpectingReply = nlDER.boolValue;

   pPriority   = GetKeywordValue( NULL, kwPRIORITY, nlPriority );
   if (!pPriority)
      pPriority = GetKeywordValue( NULL, kwPRIO, nlPriority );

   if (pPriority) {
      if ((nlPriority.intValue < 0) || (nlPriority.intValue > 3))
         throw ExecError( "Priority out of range 0..3", pPriority->exprLine );
      apdu.apduNetworkPriority = nlPriority.intValue;
   }

   // pass along to the device object
   TRACE( "SendAPDU %p at %u\n", this, GetTickCount() );
   (*execDB->GetDevices())[0]->SendAPDU( apdu );
}

//
// ScriptExecutor::SendDevConfirmedRequest
//

void ScriptExecutor::SendDevConfirmedRequest( BACnetAPDU &apdu )
{
   CByteArray           packet;
   ScriptPacketExprPtr  pService;
   BACnetInteger        alService;

   // set the packet type
   apdu.apduType = confirmedRequestPDU;
   apdu.apduExpectingReply = true;

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALConfirmedServiceMap );
   if (!pService)
      throw "Service choice (SERVICE) keyword required";

   // encode it
   apdu.apduService = alService.intValue;

   // encode the rest
   SendALData( packet );

   // set the apdu contents
   apdu.Append( packet.GetData(), packet.GetSize() );
}

//
// ScriptExecutor::SendDevUnconfirmedRequest
//

void ScriptExecutor::SendDevUnconfirmedRequest( BACnetAPDU &apdu )
{
   CByteArray           packet;
   ScriptPacketExprPtr  pService;
   BACnetInteger        alService;

   // set the packet type
   apdu.apduType = unconfirmedRequestPDU;
   apdu.apduExpectingReply = false;

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALUnconfirmedServiceMap );
   if (!pService)
      throw "Service choice (SERVICE) keyword required";

   // encode it
   apdu.apduService = alService.intValue;

   // encode the rest
   SendALData( packet );

   // set the apdu contents
   apdu.Append( packet.GetData(), packet.GetSize() );
}

//
// ScriptExecutor::SendDevSimpleACK
//

void ScriptExecutor::SendDevSimpleACK( BACnetAPDU &apdu )
{
   ScriptPacketExprPtr  pInvokeID, pService;
   BACnetInteger        alInvokeID, alService;

   // set the packet type
   apdu.apduType = simpleAckPDU;

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALConfirmedServiceMap );
   if (!pService)
      throw "Service-ACK-choice (SERVICE) keyword required";

   // encode it
   apdu.apduService = alService.intValue;

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   apdu.apduInvokeID = alInvokeID.intValue;
}

//
// ScriptExecutor::SendDevComplexACK
//

void ScriptExecutor::SendDevComplexACK( BACnetAPDU &apdu )
{
   CByteArray           packet;
   ScriptPacketExprPtr  pInvokeID, pService;
   BACnetInteger        alInvokeID, alService;

   // set the packet type
   apdu.apduType = complexAckPDU;
   apdu.apduExpectingReply = false;

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALConfirmedServiceMap );
   if (!pService)
      throw "Service-ACK-choice (SERVICE) keyword required";

   // encode it
   apdu.apduService = alService.intValue;

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   apdu.apduInvokeID = alInvokeID.intValue;

   // encode the rest
   SendALData( packet );

   // set the apdu contents
   apdu.Append( packet.GetData(), packet.GetSize() );
}

//
// ScriptExecutor::SendDevSegmentACK
//

void ScriptExecutor::SendDevSegmentACK( BACnetAPDU &apdu )
{
   ScriptPacketExprPtr  pNegativeACK, pServer, pInvokeID, pSeqNumber, pWindowSize;
   BACnetBoolean        alNegativeACK, alServer;
   BACnetInteger        alInvokeID, alSeqNumber, alWindowSize;

   // set the packet type
   apdu.apduType = segmentAckPDU;
   apdu.apduExpectingReply = false;

   // get the ACK
   pNegativeACK = GetKeywordValue( NULL, kwNEGATIVEACK, alNegativeACK );
   if (!pNegativeACK) throw "Negative-ACK keyword required";
   apdu.apduNak = (alNegativeACK.boolValue != 0);

   // get the SERVER
   pServer = GetKeywordValue( NULL, kwSERVER, alServer );
   if (!pServer) throw "Server keyword required";
   apdu.apduSrv = (alServer.boolValue != 0);

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";
   apdu.apduInvokeID = alInvokeID.intValue;

   // get the sequence number
   pSeqNumber = GetKeywordValue( NULL, kwSEQUENCENR, alSeqNumber );
   if (!pSeqNumber)
      throw "Sequence number (SEQUENCENR) keyword required";
   if ((alSeqNumber.intValue < 0) || (alSeqNumber.intValue > 255))
      throw "Sequence number out of range (0..255)";
   apdu.apduSeq = alSeqNumber.intValue;

   // get the actual window size
   pWindowSize = GetKeywordValue( NULL, kwWINDOWSIZE, alWindowSize );
   if (!pWindowSize)
      throw "Actual window size (WINDOWSIZE) keyword required";
   if ((alWindowSize.intValue < 0) || (alWindowSize.intValue > 255))
      throw "Actual window size out of range (0..255)";
   apdu.apduWin = alWindowSize.intValue;
}

//
// ScriptExecutor::SendDevError
//

void ScriptExecutor::SendDevError( BACnetAPDU &apdu )
{
   CByteArray           packet;
   ScriptPacketExprPtr  pInvokeID, pService;
   BACnetInteger        alInvokeID, alService;

   // set the packet type
   apdu.apduType = errorPDU;
   apdu.apduExpectingReply = false;

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALConfirmedServiceMap );
   if (!pService)
      throw "Service choice (SERVICE) keyword required";

   // encode it
   apdu.apduService = alService.intValue;

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   apdu.apduInvokeID = alInvokeID.intValue;

   // encode the rest
   SendALData( packet );

   // set the apdu contents
   apdu.Append( packet.GetData(), packet.GetSize() );
}

//
// ScriptExecutor::SendDevReject
//

void ScriptExecutor::SendDevReject( BACnetAPDU &apdu )
{
   ScriptPacketExprPtr  pInvokeID, pReason;
   BACnetInteger        alInvokeID, alReason;

   // set the packet type
   apdu.apduType = rejectPDU;
   apdu.apduExpectingReply = false;

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   apdu.apduInvokeID = alInvokeID.intValue;

   // get the reject reason
   pReason = GetKeywordValue( NULL, kwREJECTREASON, alReason, ScriptALRejectReasonMap );
   if (!pReason)
      throw "Reject reason keyword required";
   if ((alReason.intValue < 0) || (alReason.intValue > 255))
      throw "Reject reason out of range (0..255)";

   // encode it
   apdu.apduAbortRejectReason = alReason.intValue;
}

//
// ScriptExecutor::SendDevAbort
//

void ScriptExecutor::SendDevAbort( BACnetAPDU &apdu )
{
   ScriptPacketExprPtr  pServer, pInvokeID, pReason;
   BACnetBoolean        alServer;
   BACnetInteger        alInvokeID, alReason;

   // set the packet type
   apdu.apduType = abortPDU;
   apdu.apduExpectingReply = false;

   // see if this is being sent as a server
   pServer = GetKeywordValue( NULL, kwSERVER, alServer );
   if (!pServer) throw "Server keyword required";

   // encode it
   apdu.apduSrv = (alServer.boolValue != 0);

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   apdu.apduInvokeID = alInvokeID.intValue;

   // get the reject reason
   pReason = GetKeywordValue( NULL, kwABORTREASON, alReason, ScriptALAbortReasonMap );
   if (!pReason)
      throw "Abort reason keyword required";
   if ((alReason.intValue < 0) || (alReason.intValue > 255))
      throw "Abort reason out of range (0..255)";

   // encode it
   apdu.apduAbortRejectReason = alReason.intValue;
}

//
// ScriptExecutor::SendConfirmedRequest
//

void ScriptExecutor::SendConfirmedRequest( CByteArray &packet )
{
   ScriptPacketExprPtr  pSegMsg, pMOR, pSegResp, pMaxResp, pMaxSegs, pInvokeID, pSeq, pWindow, pService;
   BACnetBoolean        alSegMsg, alMOR, alSegResp;
   BACnetInteger        alMaxResp, alMaxSegs, alInvokeID, alSeq, alWindow, alService;

   // Segmented request?  Default is false, since it is almost impossible to do
   // segmentation manually.  Easier to use SendDevConfirmedRequest.
   pSegMsg = GetKeywordValue( NULL, kwSEGMSG, alSegMsg );
   if (!pSegMsg)
      pSegMsg = GetKeywordValue( NULL, kwSEGMENTEDMESSAGE, alSegMsg );

   if (alSegMsg.boolValue) {
      pMOR = GetKeywordValue( NULL, kwMOREFOLLOWS, alMOR );
      if (!pMOR)
         throw "More-follows keyword required";
   }

   // Do we accept a segmented response?  Default is false since it is almost
   // impossible to do segmentation manually.  Easier to use SendDevConfirmedRequest.
   pSegResp = GetKeywordValue( NULL, kwSEGRESP, alSegResp );
   if (!pSegResp)
      pSegResp = GetKeywordValue( NULL, kwSEGRESPACCEPTED, alSegResp );

   // encode the first byte of the APDU header
   packet.Add( (0 << 4)
         + ((alSegMsg.boolValue ? 1 : 0) << 3)
         + ((alMOR.boolValue ? 1 : 0) << 2)
         + ((alSegResp.boolValue ? 1 : 0) << 1)
      );

   // Lots of options for maximum response size.
   // Default to 480 for convenience in script writing.
   alMaxResp.intValue = 480;
   pMaxResp = GetKeywordValue( NULL, kwMAXRESP, alMaxResp );
   if (!pMaxResp)
      pMaxResp = GetKeywordValue( NULL, kwMAXRESPONSE, alMaxResp );
   if (!pMaxResp)
      pMaxResp = GetKeywordValue( NULL, kwMAXSIZE, alMaxResp );

   // translate the max response size into the code
   // Values less than 16 are encoded directly as the bit-field
   if (alMaxResp.intValue >= 16)
   {
      alMaxResp.intValue = MaxAPDUEncode( alMaxResp.intValue );
   }

   // Maximum number of response segments accepted.
   // Default of 0 "unspecified" is the historical value used by VTS
   pMaxSegs = GetKeywordValue( NULL, kwMAXSEGS, alMaxSegs );
   alMaxSegs.intValue = MaxSegsEncode( alMaxSegs.intValue );

   // encode the second byte of the APDU header
   packet.Add( (alMaxSegs.intValue << 4) | alMaxResp.intValue );

   // Get the invoke ID.  Default to 0 for convenience in script writing.
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   packet.Add( alInvokeID.intValue );

   // look up segmented message stuff
   if (alSegMsg.boolValue) {
      pSeq     = GetKeywordValue( NULL, kwSEQUENCENR, alSeq );
      if (!pSeq) throw "Sequence number (SEQUENCENR) keyword required";
      pWindow     = GetKeywordValue( NULL, kwWINDOWSIZE, alWindow );
      if (!pWindow) throw "Window size (WINDOWSIZE) keyword required";

      // encode it
      packet.Add( alSeq.intValue );
      packet.Add( alWindow.intValue );
   }

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALConfirmedServiceMap );
   if (!pService)
      throw "Service choice (SERVICE) keyword required";

   // encode it
   packet.Add( alService.intValue );

   // encode the rest
   SendALData( packet );
}

//
// ScriptExecutor::SendUnconfirmedRequest
//

void ScriptExecutor::SendUnconfirmedRequest( CByteArray &packet )
{
   ScriptPacketExprPtr  pService;
   BACnetInteger        alService;

   // encode the header
   packet.Add( 1 << 4 );

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALUnconfirmedServiceMap );
   if (!pService)
      throw "Service choice (SERVICE) keyword required";

   // encode it
   packet.Add( alService.intValue );

   // encode the rest
   SendALData( packet );
}

//
// ScriptExecutor::SendSimpleACK
//

void ScriptExecutor::SendSimpleACK( CByteArray &packet )
{
   ScriptPacketExprPtr  pInvokeID, pService;
   BACnetInteger        alInvokeID, alService;

   // encode the header
   packet.Add( 2 << 4 );

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   packet.Add( alInvokeID.intValue );

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALConfirmedServiceMap );
   if (!pService)
      throw "Service-ACK-choice (SERVICE) keyword required";

   // encode it
   packet.Add( alService.intValue );
}

//
// ScriptExecutor::SendComplexACK
//

void ScriptExecutor::SendComplexACK( CByteArray &packet )
{
   ScriptPacketExprPtr  pSegMsg, pMOR, pInvokeID, pSeq, pWindow, pService;
   BACnetBoolean        alSegMsg, alMOR;
   BACnetInteger        alInvokeID, alSeq, alWindow, alService;

   // find segmentation inforamtion
   pSegMsg = GetKeywordValue( NULL, kwSEGMSG, alSegMsg );
   if (!pSegMsg)
      pSegMsg = GetKeywordValue( NULL, kwSEGMENTEDMESSAGE, alSegMsg );
   if (!pSegMsg)
      alSegMsg.boolValue = BACnetBoolean::bFalse;
   if (alSegMsg.boolValue) {
      pMOR = GetKeywordValue( NULL, kwMOREFOLLOWS, alMOR );
      if (!pMOR)
         throw "More-follows keyword required";
   } else
      alMOR.boolValue = BACnetBoolean::bFalse;

   // encode it
   packet.Add( (3 << 4)
         + ((alSegMsg.boolValue ? 1 : 0) << 3)
         + ((alMOR.boolValue ? 1 : 0) << 2)
      );

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Original invoke ID (INVOKEID) keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Original invoke ID out of range (0..255)";

   // encode it
   packet.Add( alInvokeID.intValue );

   // if this is segmented
   if (alSegMsg.boolValue) {
      // get the sequence number
      pSeq = GetKeywordValue( NULL, kwSEQUENCENR, alSeq );
      if (!pSeq)
         throw "Sequence number (SEQUENCENR) keyword required";
      if ((alSeq.intValue < 0) || (alSeq.intValue > 255))
         throw "Sequence number out of range (0..255)";

      // encode it
      packet.Add( alSeq.intValue );

      // get the proposed window size
      pWindow = GetKeywordValue( NULL, kwWINDOWSIZE, alWindow );
      if (!pWindow)
         throw "Proposed window size keyword required";
      if ((alWindow.intValue < 1) || (alWindow.intValue > 127))
         throw "Proposed window size out of range (1..127)";

      // encode it
      packet.Add( alWindow.intValue );
   }

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALConfirmedServiceMap );
   if (!pService)
      throw "Service choice (SERVICE) keyword required";

   // encode it
   packet.Add( alService.intValue );

   // encode the rest
   SendALData( packet );
}

//
// ScriptExecutor::SendSegmentACK
//

void ScriptExecutor::SendSegmentACK( CByteArray &packet )
{
   ScriptPacketExprPtr  pNegativeACK, pServer, pInvokeID, pSeqNumber, pWindowSize;
   BACnetBoolean        alNegativeACK, alServer;
   BACnetInteger        alInvokeID, alSeqNumber, alWindowSize;

   // find the interesting keywords
   pNegativeACK = GetKeywordValue( NULL, kwNEGATIVEACK, alNegativeACK );
   if (!pNegativeACK) throw "Negative-ACK keyword required";
   pServer = GetKeywordValue( NULL, kwSERVER, alServer );
   if (!pServer) throw "Server keyword required";

   // encode it
   packet.Add( (4 << 4)
         + ((alNegativeACK.boolValue ? 1 : 0) << 1)
         + (alServer.boolValue ? 1 : 0)
      );

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   packet.Add( alInvokeID.intValue );

   // get the sequence number
   pSeqNumber = GetKeywordValue( NULL, kwSEQUENCENR, alSeqNumber );
   if (!pSeqNumber)
      throw "Sequence number (SEQUENCENR) keyword required";
   if ((alSeqNumber.intValue < 0) || (alSeqNumber.intValue > 255))
      throw "Sequence number out of range (0..255)";

   // encode it
   packet.Add( alSeqNumber.intValue );

   // get the actual window size
   pWindowSize = GetKeywordValue( NULL, kwWINDOWSIZE, alWindowSize );
   if (!pWindowSize)
      throw "Actual window size (WINDOWSIZE) keyword required";
   if ((alWindowSize.intValue < 0) || (alWindowSize.intValue > 255))
      throw "Actual window size out of range (0..255)";

   // encode it
   packet.Add( alWindowSize.intValue );
}

//
// ScriptExecutor::SendError
//

void ScriptExecutor::SendError( CByteArray &packet )
{
   ScriptPacketExprPtr  pInvokeID, pService;
   BACnetInteger        alInvokeID, alService;

   // encode the header
   packet.Add( 5 << 4 );

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   packet.Add( alInvokeID.intValue );

   // get the service choice
   pService = GetKeywordValue( NULL, kwSERVICE, alService, ScriptALConfirmedServiceMap );
   if (!pService)
      throw "Service choice (SERVICE) keyword required";

   // encode it
   packet.Add( alService.intValue );

   // encode the rest
   SendALData( packet );
}

//
// ScriptExecutor::SendReject
//

void ScriptExecutor::SendReject( CByteArray &packet )
{
   ScriptPacketExprPtr  pInvokeID, pReason;
   BACnetInteger        alInvokeID, alReason;

   // encode the header
   packet.Add( 6 << 4 );

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   packet.Add( alInvokeID.intValue );

   // get the reject reason
   pReason = GetKeywordValue( NULL, kwREJECTREASON, alReason, ScriptALRejectReasonMap );
   if (!pReason)
      throw "Reject reason keyword required";
   if ((alReason.intValue < 0) || (alReason.intValue > 255))
      throw "Reject reason out of range (0..255)";

   // encode it
   packet.Add( alReason.intValue );
}

//
// ScriptExecutor::SendAbort
//

void ScriptExecutor::SendAbort( CByteArray &packet )
{
   ScriptPacketExprPtr  pServer, pInvokeID, pReason;
   BACnetBoolean        alServer;
   BACnetInteger        alInvokeID, alReason;

   // see if this is being sent as a server
   pServer = GetKeywordValue( NULL, kwSERVER, alServer );
   if (!pServer) throw "Server keyword required";

   // encode it
   packet.Add( (7 << 4)
         + (alServer.boolValue ? 1 : 0)
      );

   // get the invoke ID
   pInvokeID = GetKeywordValue( NULL, kwINVOKEID, alInvokeID );
   if (!pInvokeID)
      throw "Invoke ID keyword required";
   if ((alInvokeID.intValue < 0) || (alInvokeID.intValue > 255))
      throw "Invoke ID out of range (0..255)";

   // encode it
   packet.Add( alInvokeID.intValue );

   // get the reject reason
   pReason = GetKeywordValue( NULL, kwABORTREASON, alReason, ScriptALAbortReasonMap );
   if (!pReason)
      throw "Abort reason keyword required";
   if ((alReason.intValue < 0) || (alReason.intValue > 255))
      throw "Abort reason out of range (0..255)";

   // encode it
   packet.Add( alReason.intValue );
}

//
// ScriptExecutor::SendALData
//
// There are two flavors of application layer variable encoding stuff.  One is with a keyword 
// on the front of the type with a bunch of parameters, the other is ALDATA followed by a 
// hex string.
//

void ScriptExecutor::SendALData( CByteArray &packet )
{
   int                  indx, len;
   BACnetOctetString    ostr;

   // get the index of the first data
   indx = execPacket->packetExprList.FirstData();
   if (indx < 0)
      return;     // in some cases, like Who-Is, there might not be any parameters

   // get the length
   len = execPacket->packetExprList.Length();

   for (; indx < len; indx++) {
      ScriptPacketExprPtr spep = execPacket->packetExprList.Child( indx );
      if (!spep)
         throw "Application variable element expected";

      // check for raw data
      if ((spep->exprKeyword == kwAL) || (spep->exprKeyword == kwALDATA)) {
         // translate the expression, resolve parameter names into values
         ScriptTokenList tlist;
         ResolveExpr( spep->exprValue, spep->exprLine, tlist );

         // just a little error checking
         if (tlist.Length() != 1)
            throw "ALDATA requires an octet string";

         // reference or data?
         if (tlist[0].tokenType == scriptReference) {
            BACnetAnyValue    bacnetEPICSProperty;

            GetEPICSProperty( tlist[0].tokenSymbol, &bacnetEPICSProperty, tlist[0].m_nIndex);

            BACnetAPDUEncoder enc(2048);     // big buffer size
            bacnetEPICSProperty.Encode(enc);

            // copy the contents into the byte array
            for (int i = 0; i < enc.pktLength; i++)
               packet.Add( enc.pktBuffer[i] );
         } else {
            if (!tlist[0].IsEncodeable( ostr ))
               throw "Octet string expected";

            // copy the contents into the byte array
            for (int j = 0; j < ostr.strLen; j++)
               packet.Add( ostr.strBuff[j] );
         }

         continue;
      }

      // skip things that aren't data
      if (!spep->exprIsData)
         continue;

      // do the type specific encoding
      try {
         int rslt = ScriptToken::Lookup( spep->exprKeyword, ScriptALMap );
         if (rslt < 0)
            throw "Invalid keyword";

         switch (rslt) {
            case 1:                       // NULL
               SendALNull( spep, packet );
               break;
            case 2:                       // BOOLEAN
               SendALBoolean( spep, packet );
               break;
            case 3:                       // UNSIGNED
               SendALUnsigned( spep, packet );
               break;
            case 4:                       // INTEGER
               SendALInteger( spep, packet );
               break;
            case 5:                       // REAL
               SendALReal( spep, packet );
               break;
            case 6:                       // DOUBLE
               SendALDouble( spep, packet );
               break;
            case 7:                       // OCTETSTRING
               SendALOctetString( spep, packet );
               break;
            case 8:                       // CHARACTERSTRING
               SendALCharacterString( spep, packet );
               break;
            case 9:                       // BITSTRING
               SendALBitString( spep, packet );
               break;
            case 10:                   // ENUMERATED
               SendALEnumerated( spep, packet );
               break;
            case 11:                   // DATE
               SendALDate( spep, packet );
               break;
            case 12:                   // TIME
               SendALTime( spep, packet );
               break;
            case 13:                   // OBJECTIDENTIFIER
               SendALObjectIdentifier( spep, packet );
               break;
            case 14:                   // DEVICEIDENTIFIER
               SendALDeviceIdentifier( spep, packet );
               break;
            case 15:                   // PROPERTYIDENTIFIER
               SendALPropertyIdentifier( spep, packet );
               break;
            case 16:                   // OPENINGTAG
               SendALOpeningTag( spep, packet );
               break;
            case 17:                   // CLOSINGTAG
               SendALClosingTag( spep, packet );
               break;
            case 18:                   // ANY
               SendALAny( spep, packet );
               break;
            default:
               throw "Invalid keyword";
               break;
         }
      }
      catch (const char *errMsg)
      {
         throw ExecError( errMsg, spep->exprLine );
      }
      catch (const CString &errMsg)
      {
         throw ExecError( errMsg, spep->exprLine );
      }
   }
}

//
// ScriptExecutor::SendALNull
//

void ScriptExecutor::SendALNull( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               context = kAppContext;
   BACnetNull        nullData;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 0)
      ;
   else
   if (tlist.Length() == 1) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
   } else
      throw "Too many keyword values";

   // encode it
   nullData.Encode( enc, context );

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALBoolean
//

void ScriptExecutor::SendALBoolean( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               indx, context = kAppContext;
   BACnetBoolean     boolData;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      indx = 0;
   } else
   if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } else
      throw "Boolean keyword requires 1 or 2 parameters";

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference)
   {
      BACnetAnyValue bacnetAny;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex);

      // verify the type
      if (bacnetAny.GetType() != ebool)
         throw "Boolean property value expected in EPICS";

      bacnetAny.Encode(enc, context);
   } else
   {
      if (!tlist[indx].IsEncodeable( boolData ))
         throw "Boolean value expected";

      boolData.Encode( enc, context );
   }

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALUnsigned
//

void ScriptExecutor::SendALUnsigned( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               indx, context = kAppContext;
   BACnetUnsigned    uintData;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   spep->exprValue.MakeUpper();

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      indx = 0;
   } else
   if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } else
      throw "Unsigned keyword requires 1 or 2 parameters";

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference)
   {
      BACnetAnyValue bacnetAny;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex);

      // verify the type
      if (bacnetAny.GetType() != uw )
         throw "Unsigned property value expected in EPICS";

      uintData.uintValue = ((BACnetUnsigned *) bacnetAny.GetObject())->uintValue;
   } else
   if (!tlist[indx].IsEncodeable( uintData ))
      throw "Unsigned value expected";

   // check for wierd versions
   if (spep->exprKeyword == kwUNSIGNED8) {
      if ((uintData.uintValue < 0) || (uintData.uintValue > 255))
         throw "Unsigned8 value out of range (0..255)";
      if (tlist.Length() == 2)
         throw "Unsigned8 keyword cannot be context encoded";
      packet.Add( (unsigned char) uintData.uintValue );
   } else
   if (spep->exprKeyword == kwUNSIGNED16) {
      if ((uintData.uintValue < 0) || (uintData.uintValue > 65535))
         throw "Unsigned16 value out of range (0..65535)";
      if (tlist.Length() == 2)
         throw "Unsigned16 keyword cannot be context encoded";
      packet.Add( (unsigned char) ((uintData.uintValue >> 8) & 0xFF) );
      packet.Add( (unsigned char) (uintData.uintValue & 0xFF) );
   } else
   if (spep->exprKeyword == kwUNSIGNED32) {
      if (tlist.Length() == 2)
         throw "Unsigned32 keyword cannot be context encoded";
      packet.Add( (unsigned char) ((uintData.uintValue >> 24) & 0xFF) );
      packet.Add( (unsigned char) ((uintData.uintValue >> 16) & 0xFF) );
      packet.Add( (unsigned char) ((uintData.uintValue >>  8) & 0xFF) );
      packet.Add( (unsigned char) (uintData.uintValue & 0xFF) );
   } else {
      // encode it
      uintData.Encode( enc, context );

      // copy the encoding into the byte array
      for (int i = 0; i < enc.pktLength; i++)
         packet.Add( enc.pktBuffer[i] );
   }
}

//
// ScriptExecutor::SendALInteger
//

void ScriptExecutor::SendALInteger( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               indx, context = kAppContext;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   spep->exprValue.MakeUpper();

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      indx = 0;
   } else
   if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } else
      throw "Integer keyword requires 1 or 2 parameters";

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue bacnetAny;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex);

      // verify the type
      if (!bacnetAny.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetUnsigned)) && !bacnetAny.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetInteger)) )
         throw "Integer property value expected in EPICS";

      bacnetAny.Encode(enc, context);
   } else
   {
      BACnetInteger intData;
      if (!tlist[indx].IsEncodeable( intData ))
         throw "Integer value expected";

      // encode it
      intData.Encode( enc, context );
   }

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALReal
//

void ScriptExecutor::SendALReal( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               indx, context = kAppContext;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      indx = 0;
   } else
   if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } else
      throw "Real keyword requires 1 or 2 parameters";

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference)
   {
      BACnetAnyValue bacnetAny;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex );

      // verify the type
      if ( bacnetAny.GetType() != flt )
         throw "Real property value expected in EPICS";

      bacnetAny.Encode(enc, context);
   } else
   {
      BACnetReal realData;

      if (!tlist[indx].IsEncodeable( realData ))
         throw "Real value expected";
      
      realData.Encode( enc, context );
   }

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALDouble
//

void ScriptExecutor::SendALDouble( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               indx, context = kAppContext;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      indx = 0;
   } else
   if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } else
      throw "Double keyword requires 1 or 2 parameters";

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue bacnetAny;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex);

      // verify the type
      if (bacnetAny.GetType() != flt )
         throw "Floating point property value expected in EPICS";

      bacnetAny.Encode(enc, context);
   } else
   {
      BACnetDouble      doubleData;

      if (!tlist[indx].IsEncodeable( doubleData ))
         throw "Floating point value expected";

      doubleData.Encode( enc, context );
   }

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALOctetString
//

void ScriptExecutor::SendALOctetString( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               indx, context = kAppContext;
   BACnetOctetString ostrData;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) 
   {
      indx = 0;
   } 
   else if (tlist.Length() == 2) 
   {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } 
   else 
   {
      throw "OctetString keyword requires 1 or 2 parameters";
   }

   if (tlist[indx].tokenType == scriptReference)
   {
      // Thus far, no object has a property that is an octetstring.
      // If we need one, will need to add a ParseType for octetstring,
      // and update various factories and switch statements.
      throw "Octet string property references not supported";
   }
   else if (!tlist[indx].IsEncodeable( ostrData ))
   {
      throw "Octet string expected";
   }

   // encode it
   ostrData.Encode( enc, context );

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALCharacterString
//
// The character string format has been extended a little.  The basic idea is to make 
// it easier to use regular strings, a novel concept.
//
//    CHARACTERSTRING   [ op ] cstring
//    CHARACTERSTRING   [ op ] ostring
//    CHARACTERSTRING   [ op ] { ref }
//    CHARACTERSTRING   [ op ] char-set ',' ( cstring | ostring )
//    CHARACTERSTRING   [ op ] tag ',' char-set ',' ( cstring | ostring )
//    CHARACTERSTRING   [ op ] tag ',' { ref }
//
// Most of ugly cases involves putting the last two tokens back together and 
// passing the whole thing off to Decode().  What a hack!  
// I don't want to allow the script to specify a different encoding than what 
// might be in the EPICS without 
//   (a) supporting translation from one encoding to another or 
//   (b) requiring that the EPICS content match the encoding specified in the 
//        script.
//  Option (a) is a pain (if it's even possible), and (b) doesn't seem to have 
//  much point.

void ScriptExecutor::SendALCharacterString( ScriptPacketExprPtr spep, CByteArray &packet )
{
  int                   indx,  // Used for retrieving property from EPICS
                        context = kAppContext;  // assume application data
  BACnetCharacterString cstrData;
  BACnetAPDUEncoder     enc;
  ScriptTokenList       tlist;

  // Tokenize the expression and resolve parameter names into values
  ResolveExpr( spep->exprValue, spep->exprLine, tlist );

  // Tag and CharSet are optional
  if (tlist.Length() == 1)
  {
    // Tag and CharSet omitted
    indx = 0; // the token is the first item in the list

    if (tlist[0].tokenType == scriptReference)
      indx = 0;
    else 
    if (!tlist[0].IsEncodeable(cstrData))
      throw "ASCII (ANSI X3.4) string value expected";
  }
  else if (tlist.Length() == 2)
  {
    indx = 1; // the token is the second item in the list

    // Tag or CharSet included
    if (tlist[0].IsInteger(context))
    {
      // It's the tag
      if (!tlist[1].IsEncodeable(cstrData))
        throw "ASCII (ANSI X3.4) string value expected";
    }
    else if (ScriptToken::Lookup(tlist[0].tokenSymbol, 
                                 ScriptCharacterTypeMap) >= 0)
    {
      // It's a character set; decode the combined CharSet and string
      CString buff;
      buff.Format("%s, %s", tlist[0].tokenValue, tlist[1].tokenValue);
      cstrData.Decode(buff);
    }
    else
    {
      // Neither Tag nor CharSet included
      throw "Tag number or Character Set expected";
    }
  }
  else if (tlist.Length() == 3)
  {
    indx = 2; // the token is the third item in the list

    // Tag AND CharSet included!
    if (!tlist[0].IsInteger(context))
      throw "Tag number expected";

    if (ScriptToken::Lookup( tlist[1].tokenSymbol, ScriptCharacterTypeMap ) < 0)
      throw "Unknown character set";

    // Decode the combined CharSet and string
    CString buff;
    buff.Format("%s, %s", tlist[1].tokenValue, tlist[2].tokenValue);
    cstrData.Decode(buff);
  }
  else
    if (tlist.Length() < 1)
      throw "Missing required parameters";
    else
      throw "Too many parameters";

  if (tlist[indx].tokenType != scriptReference)
    {
      indx = -1;  // A reference to an EPICS property was NOT used
    }

  // see if a reference was used
  if (indx >= 0)
  {
    BACnetAnyValue bacnetAny;

    GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex);

    // verify the type
    if (!bacnetAny.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetCharacterString)))
      throw "Character string property value expected in EPICS";

    bacnetAny.Encode(enc, context);
  }
  else
    cstrData.Encode(enc, context);

  // copy the encoding into the byte array
  for (int i = 0; i < enc.pktLength; i++)
    packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALBitString
//
// Yet another custom extension, bitstrings can be specified as a list of T/F values or
// B'1010100111'.
//

void ScriptExecutor::SendALBitString( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               indx, context = kAppContext, bit, i;
   BACnetBitString   bstrData;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1)
   {
      indx = 0;
   } 
   else if (tlist.Length() == 2)
   {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } 
   else
   {
      throw "Bitstring keyword requires 1 or 2 parameters";
   }

   const ScriptToken &data = tlist[indx];

   if (data.tokenType == scriptReference)
   {
      BACnetAnyValue bacnetEPICSProperty;
      GetEPICSProperty( data.tokenSymbol, &bacnetEPICSProperty, data.m_nIndex);

      // verify the type
      if ( bacnetEPICSProperty.GetType() != bits)
         throw "Bitstring property value expected in EPICS";

      bstrData = *((BACnetBitString *) bacnetEPICSProperty.GetObject());
   } 
   else if (data.tokenEnc == scriptComplex)
   {
      // Value enclosed by []
      if (!data.IsEncodeable( bstrData ))
         throw "Bitstring value expected";
   } 
   else if (data.tokenEnc == scriptBinaryEnc)
   {
      if (!data.IsEncodeable( bstrData ))
         throw "Bitstring value expected";
   } 
   else if (data.tokenType == scriptKeyword)
   {
      if (data.IsInteger( bit, ScriptBooleanMap ))
         bstrData += bit;
   } 
   else
   {
      throw "Bitstring expected";
   }

   // encode it
   bstrData.Encode( enc, context );

   // copy the encoding into the byte array
   for (i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALEnumerated
//

void ScriptExecutor::SendALEnumerated( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               indx, context = kAppContext;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      indx = 0;
   } else
   if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } else
      throw "Enumerated keyword requires 1 or 2 parameters";

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue bacnetAny;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex);

      // verify the type
      if ( bacnetAny.GetType() != et )
         throw "Enumerated property value expected in EPICS";

      bacnetAny.Encode(enc, context);
   } else {
      BACnetEnumerated  enumData;

      try {
         enumData.Decode( tlist[indx].tokenValue );
      }
      catch (...) {
         throw "Integer value expected";
      }

      enumData.Encode( enc, context );
   }

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALDate
//
//    DATE [ op ] [ tag ',' ] dow [ ',' ] mon '/' day '/' year
//    DATE [ op ] [ tag ',' ] { ref }
//
//    dow ::= MONDAY | ... | SUNDAY | '*' | '?'
//    mon ::= number | '*' | '?'
//    day ::= number | '*' | '?'
//    year ::= number | '*' | '?'
//          number could be two digit year (02) or full year (2002)
//
// In the future it would be nice to support these:
//
//    DATE [ op ] [ tag ',' ] dow [ ',' ] month [ [ ',' ] day ',' year ]
//    DATE [ op ] [ tag ',' ] dow [ ',' ] day '-' month [ '-' year ]
//
// which requires changes to BACnetDate::Decode(const char *).
//

void ScriptExecutor::SendALDate( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int            indx, context = kAppContext;
   BACnetDate     dateData;
   ScriptTokenList   tlist;
   BACnetAPDUEncoder enc;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      indx = 0;
   } else if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } else
      throw "Date keyword requires 1 or 2 parameters";

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue bacnetAny;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex);

      // verify the type
      if ( bacnetAny.GetType() != ptDate )
         throw "Date property value expected in EPICS";

      bacnetAny.Encode(enc, context);
   } else {
      try {
         dateData.Decode( tlist[indx].tokenValue );
      }
      catch (...) {
         CString str;
         str.Format(IDS_SCREX_BADDATADATE, tlist[indx].tokenValue );
         throw CString(str);
      }

      dateData.Encode( enc, context );
   }

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}




//
// ScriptExecutor::SendALTime
//
//    TIME [ op ] [ tag ',' ] hour ':' min ':' sec '.' hund
//    TIME [ op ] [ tag ',' ] { ref }
//
// The trick to parsing a time is the comma after the tag.  All four parts are 
// reqired, the '*' may be used for dont-care value.
//  Actually, the hundredths is not required.  If a period is present, it expects the rest.
//

void ScriptExecutor::SendALTime( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int            indx, context = kAppContext;
   BACnetTime     timeData;
   ScriptTokenList   tlist;
   BACnetAPDUEncoder enc;

   // TODO: Tag is present only if there is a comma.
   // Test for that, and I think we can avoid the need for [] around time.
   // - If comma get tag number
   // - If :, it's a time
   // - If {, its a script reference
   // ? How about variable reference?
   //
   // The problem may be that ResolveExpr parses the ENTIRE line.
   // Much more flexible to parse tokens as needed

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      indx = 0;
   } else if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } else
      throw "Time keyword requires 1 or 2 parameters";

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue bacnetAny;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex);

      // verify the type
      if ( bacnetAny.GetType() != ptTime )
         throw "Time property value expected in EPICS";

      bacnetAny.Encode(enc, context);
   } else {
      try {
         timeData.Decode( tlist[indx].tokenValue );
      }
      catch (...) {
         CString str;
         str.Format(IDS_SCREX_BADDATATIME, tlist[indx].tokenValue );
         throw CString(str);
      }

      timeData.Encode( enc, context );
   }

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALObjectIdentifier
//

void ScriptExecutor::SendALObjectIdentifier( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int                  indx, context = kAppContext, typ, inst;
   BACnetObjectIdentifier  id;
   BACnetAPDUEncoder    enc;
   ScriptTokenList      tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      indx = 0;
      if (tlist[0].tokenType != scriptReference)
         throw "Object identifier keyword expects an EPICS reference";
   } else
   if (tlist.Length() == 2) {
      indx = 0;
   } else
   if (tlist.Length() == 3) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      indx = 1;
   } else
      throw "Object identifier keyword requires 1, 2 or 3 parameters";

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue bacnetAny;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetAny, tlist[indx].m_nIndex);

      if ( bacnetAny.GetType() != ob_id )
         throw "Object identifier property value expected in EPICS";

      id.objID = ((BACnetObjectIdentifier *) bacnetAny.GetObject())->objID;
   } else {
      if (!tlist[indx].IsInteger( typ, ScriptObjectTypeMap ))
         throw "Object identifier type expected";
      if (!tlist[indx+1].IsInteger( inst ))
         throw "Object identifier instance expected";

      id.SetValue( (BACnetObjectType)typ, inst );
   }

   // encode it
   id.Encode( enc, context );

   // stash the context for later property references
   execObjID = id.objID;

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALDeviceIdentifier
//

void ScriptExecutor::SendALDeviceIdentifier( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int                  context = kAppContext, inst;
   BACnetAPDUEncoder    enc;
   ScriptTokenList      tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      if (!tlist[0].IsInteger( inst ))
         throw "Device identifier instance value expected";
   } else
   if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      if (!tlist[1].IsInteger( inst ))
         throw "Device identifier instance value expected";
   } else
      throw "Device identifier keyword requires 1 or 2 parameters";

   // encode it
   BACnetObjectIdentifier( 8, inst ).Encode( enc, context );

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALPropertyIdentifier
//

void ScriptExecutor::SendALPropertyIdentifier( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               context = kAppContext, valu;
   BACnetEnumerated  enumData;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      if (!tlist[0].IsInteger( valu, ScriptPropertyMap ))
         throw "Property name expected";
   } else
   if (tlist.Length() == 2) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      if (!tlist[1].IsInteger( valu, ScriptPropertyMap ))
         throw "Property name expected";
   } else
      throw "Property identifier keyword requires 1 or 2 parameters";

   // encode it
   enumData.m_enumValue = valu;
   enumData.Encode( enc, context );

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALOpeningTag
//

void ScriptExecutor::SendALOpeningTag( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               context;
   BACnetOpeningTag  openTag;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
   } else
      throw "Opening tag requires tag number";

   // encode it
   openTag.Encode( enc, context );

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALClosingTag
//

void ScriptExecutor::SendALClosingTag( ScriptPacketExprPtr spep, CByteArray &packet )
{
   int               context;
   BACnetClosingTag  closeTag;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
   } else
      throw "Closing tag requires tag number";

   // encode it
   closeTag.Encode( enc, context );

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
      packet.Add( enc.pktBuffer[i] );
}

//
// ScriptExecutor::SendALAny
//

void ScriptExecutor::SendALAny( ScriptPacketExprPtr spep, CByteArray &packet )
{
   // ANY tagged element.
   int               indx;
   BACnetANY         anyData;
   BACnetAPDUEncoder enc;
   ScriptTokenList   tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // no tag allowed, value parameter required
   if (tlist.Length() == 1) 
   {
      indx = 0;
   } 
   else 
   {
      throw "ANY keyword requires 1 parameter to specify value";
   }

   if (tlist[indx].tokenType == scriptReference)
   {
      BACnetAnyValue    bacnetEPICSProperty;
      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      // Set our data from the EPICS
      anyData.SetValue( *bacnetEPICSProperty.GetObject() );
   }
   else if (!tlist[indx].IsEncodeable( anyData ))
   {
      throw "ANY data expected";
   }

   // copy the encoding into the byte array
   for (int i = 0; i < anyData.dataLen; i++)
      packet.Add( anyData.dataBuff[i] );
}

//
// ScriptExecutor::ExpectPacket
//

bool ScriptExecutor::ExpectPacket( ScriptNetFilterPtr fp, const BACnetNPDU &npdu )
{
   int                  bipMsgID = -1, nlMsgID = -1, alMsgID = -1
   ,                    valu;
   BACnetOctet          ctrl = 0;
   CByteArray           packet;
   ScriptFilterPtr      sfp;
   ScriptNetFilterPtr   nfp;
   ScriptPacketExprPtr  pNet, bipMsg, nlMsg, nlSA, alMsg, pDADR, pSADR;
   BACnetAddress        nlSrcAddr;
   BACnetBoolean        nlDER;
   BACnetInteger        nlVersion, nlDNET, nlSNET
   ,                    nlPriority, nlHopCount;
   BACnetOctetString    nlSrc, nlDADR, nlSADR, nlData, alData;
   BACnetCharacterString   nlNetwork;
   ScriptParmPtr        pScriptParmNetwork = NULL, pScriptParm;

   try {
      // see if the network is provided
      pNet = GetKeywordValue( &pScriptParmNetwork, kwNETWORK, nlNetwork );

      // see if this should have a BVLCI header
      bipMsg = execPacket->packetExprList.Find( kwBVLCI );

      // see if we're looking for a network layer message
      nlMsg = execPacket->packetExprList.Find( kwMESSAGE );
      if (!nlMsg)
         nlMsg = execPacket->packetExprList.Find( kwMSG );

      // see if we're looking for an application layer message
      alMsg = execPacket->packetExprList.Find( kwPDU );

      // See if matching should come from device layer.  This is a quiet fail,
      // no message is going to be saved saying that this combination was 
      // skipped.  At some point the packet is going to be passed up through 
      // to a VTSServer or VTSClient and that will call ReceiveAPDU().
      if (!pNet && !bipMsg && !nlMsg && alMsg)
         return false;

      // look for the network (actually the filter name)
      if (gMasterFilterList.Length() == 1) {
         sfp = gMasterFilterList[0];
         if (pNet &&  !pNet->IsAssignment()  && !nlNetwork.Equals(sfp->filterName) )
            throw ExecError( "Port not found", pNet->exprLine );
      } else {
         if (!pNet)
            throw ExecError( "Network required", execPacket->baseLineStart );

         if (pNet->exprOp != '=' && !pNet->IsAssignment() && !pNet->IsDontCare())
            throw ExecError( "Equality or assignment operator required for network keyword", pNet->exprLine );
         
         if ( pNet->IsAssignment() )
         {
            if ( pScriptParmNetwork == NULL )
               throw ExecError( "Undefined script variable for NETWORK assignment", pNet->exprLine );

            sfp = fp;
            nlNetwork.SetValue(sfp->filterName);
            StuffScriptParameter( nlNetwork, pScriptParmNetwork, pNet->exprValue);
         }
         else if ( pNet->IsDontCare() )
         {
            sfp = fp;
         }
         else
         {
               int i;               // MAG 11AUG05 add this line, remove local declaration below since i is used out of that scope
            for (i = 0; i < gMasterFilterList.Length(); i++) {
               sfp = gMasterFilterList[i];
               if (nlNetwork.Equals(sfp->filterName))
                  break;
            }
            if (i >= gMasterFilterList.Length())
               throw ExecError( "Port not found", pNet->exprLine );
         }
      }

      // make sure its a network filter for now
      if (sfp->filterType != scriptNPDUFilter)
         throw ExecError( "NPDU filter only", pNet->exprLine );
      nfp = (ScriptNetFilterPtr)sfp;

      // did this packet come in on the network?
      if (nfp != fp)
         throw "Network mismatch";

      // get the source
      nlSA = execPacket->packetExprList.Find( kwSA );
      if (!nlSA) 
      {
         if ( !execDB->LoadNamedAddress(&nlSrcAddr, "IUT") )
            throw ExecError( "Default source address IUT not found", execPacket->baseLineStart );
      } 
      else 
      {
         ScriptTokenList   saList;

         if ((nlSA->exprOp != '=') && (!nlSA->IsAssignment()) && (!nlSA->IsDontCare() ))
            throw ExecError( "Equality or assignment operator required for source address keyword", nlSA->exprLine );

         // resolve the expressions
         pScriptParm = NULL;
         ResolveExpr( nlSA->exprValue, nlSA->exprLine, saList, nlSA->IsAssignment() ? &pScriptParm : NULL );
         if (saList.Length() < 1)
            throw ExecError( "Address, variable, name or keyword expected", nlSA->exprLine );

         // get a reference to the first parameter
         const ScriptToken &t = saList[0];

         if ( nlSA->IsAssignment() )
         {
            if ( pScriptParm == NULL )
               throw ExecError( "Undefined script variable for SA assignment", nlSA->exprLine );

            // See if we can find the provided address in our list of known names... if so, then use
            // that name (in quotes) for stuffing the parameter.
            // If not... stuff the parameter with the encoded (ASCII) address... it has to be decodeable
            // for use in other SEND statements.
            LPCSTR lpszName;

            switch( npdu.pduAddr.addrType )
            {
               case localBroadcastAddr:

                  StuffScriptParameter( BACnetCharacterString("LOCALBROADCAST"), pScriptParm, nlSA->exprValue);
                  break;

               case remoteBroadcastAddr:

                  StuffScriptParameter( BACnetCharacterString("BROADCAST"), pScriptParm, nlSA->exprValue);
                  break;

               default:

                  if ( (lpszName = execDB->AddrToName(npdu.pduAddr)) != NULL )
                  {
                     // found a name in our table that matches this address... encode the name into the variable
                     StuffScriptParameter( BACnetCharacterString(lpszName), pScriptParm, nlSA->exprValue);
                  }
                  else
                  {
                     // supplied address is unknown... just setup the thing as an octet string into the variable
                     StuffScriptParameter( BACnetOctetString((BACnetOctet *) npdu.pduAddr.addrAddr, (int) npdu.pduAddr.addrLen), pScriptParm, nlSA->exprValue);
                  }
            }

            nlSrcAddr = npdu.pduAddr;
         }
         else if ( nlSA->IsDontCare() )
            nlSrcAddr = npdu.pduAddr;
         else
         {
            // check to see if this is a keyword
            if (t.tokenType == scriptKeyword) {
               if ((t.tokenSymbol == kwBROADCAST) || (t.tokenSymbol == kwLOCALBROADCAST))
                  nlSrcAddr.LocalBroadcast();
               else if ((t.tokenSymbol == kwGLOBALBROADCAST) || (t.tokenSymbol == kwGLOBAL_BROADCAST))
                  nlSrcAddr.GlobalBroadcast();
               else
                  throw ExecError( "Unrecognized keyword supplied for SA", nlSA->exprLine );
            } else
            // it might be a name
            if ((t.tokenType == scriptValue) && (t.tokenEnc == scriptASCIIEnc)) 
            {
               if ( !execDB->LoadNamedAddress(&nlSrcAddr, t.RemoveQuotes()) )
                  throw ExecError( "Source address name not found", nlSA->exprLine );
            } 
            else if ((t.tokenType == scriptValue) && (t.tokenEnc == scriptIPEnc)) 
            {
               // it might be an IP address
               BACnetIPAddr nlIPAddr( t.tokenValue );
               nlSrcAddr = nlIPAddr;
            }
            else if (t.IsEncodeable( nlSrc )) 
            {
               // it might be an explicit octet string
               nlSrcAddr.LocalStation( nlSrc.strBuff, nlSrc.strLen );
            } 
            else
            {
               throw ExecError( "Source address expected", nlSA->exprLine );
            }
         }
      }

      // did this packet come from the expected station?
      if (!(npdu.pduAddr == nlSrcAddr))
         throw "Source address mismatch";

//madanner 6/03, can't do these tests here...  npdu.pduExpectingReply can't be examined because it hasn't
//been set when this function is called... neither is pduNetworkPriority.  We'll have to suck these
//values right out of the control octet...  Seen later.

      // match the DER
//    MatchBoolExpression( kwDER, BACnetBoolean(npdu.pduExpectingReply), "Data expecting reply mismatch: " );

      // match the priority
//    MatchEnumExpression( kwPRIORITY, BACnetEnumerated(npdu.pduNetworkPriority), NULL, "Priority mismatch: " );

      // the rest of this code will need a decoder
      BACnetAPDUDecoder dec( npdu.pduData, npdu.pduLen );

      // if this is a BVLL specific message, match it
      if (bipMsg) {
         ScriptTokenList   bvllList;

         if (bipMsg->exprOp != '=' && !bipMsg->IsAssignment()  && !bipMsg->IsDontCare() )
            throw ExecError( "Equality or assignment operator required for BVLCI", bipMsg->exprLine );

         // resolve the expressions
         ResolveExpr( bipMsg->exprValue, bipMsg->exprLine, bvllList, bipMsg->IsAssignment() ? &pScriptParm : NULL );
         if (bvllList.Length() < 1)
            throw ExecError( "BIP message keyword expected", bipMsg->exprLine );

         // get a reference to the first parameter
         const ScriptToken &t = bvllList[0];

         // This gets tricky with assignments... if the first token is the scriptValue, then we hope we're 
         // looking at the parameter to stuff, if assignment is true.

         // we could work hard to allow keyword specification here and var assignment for one of the
         // parameters... but that's another story.  For now, just assume we're assigning the BVLCI type.

         if ( !bipMsg->IsAssignment()  && !bipMsg->IsDontCare() )
         {
            // check to see if this is a keyword
            if (t.tokenType == scriptKeyword)
            {
               bipMsgID = t.Lookup( t.tokenSymbol, ScriptBIPMsgTypeMap );
               if (bipMsgID < 0)
                  throw ExecError( "BVLCI Unrecognized keyword", bipMsg->exprLine );
            }
            else
               throw ExecError( "BVLCI Keyword expected", bipMsg->exprLine );
         }

         // based on the number, check for other parameters
         try {
            // verify the message type and function
            if (0x81 != (dec.pktLength--,*dec.pktBuffer++))
               throw ExecError( "BVLCI expected", bipMsg->exprLine );

            if ( bipMsg->IsAssignment() )
            {
               if ( pScriptParm == NULL )
                  throw ExecError( "Undefined script variable for BVLCI assignment", bipMsg->exprLine );

               bipMsgID = *dec.pktBuffer;
               BACnetEnumerated  bacnetMsg(bipMsgID, NetworkSniffer::BAC_STRTAB_BVLL_Function);
               StuffScriptParameter( bacnetMsg, pScriptParm, bipMsg->exprValue);
            }
            else if ( bipMsg->IsDontCare() )
               bipMsgID = *dec.pktBuffer;

            if (bipMsgID != (dec.pktLength--,*dec.pktBuffer++))
               throw ExecError( "BVLCI message mismatch", bipMsg->exprLine );

            // verify the length
            int len;
            len = (dec.pktLength--,*dec.pktBuffer++);
            len = (len << 8) + (dec.pktLength--,*dec.pktBuffer++);
            if (len != npdu.pduLen)
               throw "BVLCI length incorrect";

            // go through the motions with all of these BVLCI parameter suckers.  Don't worry about
            // asignment just yet...

            switch (bipMsgID) {
               case 0:                 // BVLC-RESULT, rslt
                  ExpectBVLCResult( bvllList, dec );
                  break;
               case 1:                 // WRITE-BROADCAST-DISTRIBUTION-TABLE {,host/net:port}*
                  ExpectWriteBDT( bvllList, dec );
                  break;
               case 2:                 // READ-BROADCAST-DISTRIBUTION-TABLE
                  if (bvllList.Length() != 1)
                     throw "BVLC function takes no parameters";
                  break;
               case 3:                 // READ-BROADCAST-DISTRIBUTION-TABLE-ACK {,host/net:port}*
                  ExpectReadBDTAck( bvllList, dec );
                  break;
               case 4:                 // FORWARDED-NPDU, host:port
                  ExpectForwardedNPDU( bvllList, dec );
                  break;
               case 5:                 // REGISTER-FOREIGN-DEVICE, ttl
                  ExpectRegisterFD( bvllList, dec );
                  break;
               case 6:                 // READ-FOREIGN-DEVICE-TABLE
                  if (bvllList.Length() != 1)
                     throw "BVLC function takes no parameters";
                  break;
               case 7:                 // READ-FOREIGN-DEVICE-TABLE-ACK {,host:port,ttl,tr}*
                  ExpectReadFDTAck( bvllList, dec );
                  break;
               case 8:                 // DELETE-FOREIGN-DEVICE-TABLE-ENTRY
                  ExpectDeleteFDTEntry( bvllList, dec );
                  break;
               case 9:                 // DISTRIBUTE-BROADCAST-TO-NETWORK
               case 10:                // ORIGINAL-UNICAST-NPDU
               case 11:                // ORIGINAL-BROADCAST-NPDU
                  if (bvllList.Length() != 1)
                     throw "BVLC function takes no parameters";
                  break;
            }
         }
         catch (const char *errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, bipMsg->exprLine );
         }
         catch (const CString &errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, bipMsg->exprLine );
         }

         // if this doesn't have an NPDU that follows, we're done
         if ((bipMsgID != 4) && (bipMsgID != 9) && (bipMsgID != 10) && (bipMsgID != 11)) {
            // success
            return true;
         }
      }

      // there should be a network or application layer message
      if (!nlMsg && !alMsg)
//       return true;      // madanner 6/03 shouldn't we abort here if nothing defined?
         throw "Either Network (MESSAGE) or Application (PDU) layer message must be defined";

      if (nlMsg && alMsg)
         throw "Specify network or application message but not both";

      // match the version
      valu = (dec.pktLength--,*dec.pktBuffer++);
      MatchEnumExpression( kwVERSION, BACnetEnumerated(valu), NULL, "Version mismatch: " );

      // suck out the control field
      ctrl = (dec.pktLength--,*dec.pktBuffer++);

      // check to see if we got what we're looking for
      if (nlMsg && ((ctrl & 0x80) == 0))
         throw ExecError( "Not a network message", nlMsg->exprLine );
      if (alMsg && ((ctrl & 0x80) != 0))
         throw ExecError( "Not an application message", alMsg->exprLine );

      //bits 6 and 4 of NPCI must be zero
      ThrowPDUBitsError( ctrl & 0x50, "Network", alMsg ? alMsg : (nlMsg ? nlMsg : NULL) );

      // if we have a DNET/DADR, check it
      if ((ctrl & 0x20) != 0) {
         // suck out the DNET
         valu = (dec.pktLength--,*dec.pktBuffer++);
         valu = (valu << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // are we looking for a specific DNET?
         MatchEnumExpression( kwDNET, BACnetEnumerated(valu), ScriptNONEMap, "DNET mismatch: " );

         // suck out the length
         int len = (dec.pktLength--,*dec.pktBuffer++);

         // looking for a specific address?
         pDADR = GetKeywordValue( &pScriptParm, kwDADR, nlDADR, ScriptNONEMap );

// a length of 0 indicates a broadcast and is a valid entry for Destination specs - removed below LJT 1/12/2006
//       if (len == 0)
//          throw ExecError( "No DADR in packet", pDADR->exprLine );
//       if (len == 0) {
//          if (pDADR != 0)
//              throw ExecError( "No DADR in packet", pDADR->exprLine );
//          else
//             throw ExecError( "No DADR in packet" );
//       }  // add by Kare Sars

         // if length indicates we have an addr and not addr exists we have an error.  LJT 1/12/2006
         if ( len != 0 && pDADR == 0 )
            throw ExecError( "No DADR in packet" );

         // suck out the address data from the stream even if we aren't testing for DADR
         // This is a silly way to do this... but it forces BACnetOctetString to copy the data
         // into its own buffer instead of just referencing it... :(

         BACnetOctetString bacnetDADRData(BACnetOctetString(dec.pktBuffer, len));
         dec.pktBuffer += len;
         dec.pktLength -= len;

         TestOrAssign(pDADR, bacnetDADRData, nlDADR, pScriptParm, "DADR mismatch: " );
      }
      else
      {
         //OK... DNET/DADR is not present in packet... this will be fine if the script also
         //has nothing specified or NONE specified... but if there is something there it is implied
         //that DNET/DADR should be present in the packet

         // First check for DNET
         BACnetEnumerated bacnetEnum;
         pDADR = GetKeywordValue( &pScriptParm, kwDNET, bacnetEnum, ScriptNONEMap );

         if ( pDADR != NULL && !pDADR->IsNoneValue() )
            throw ExecError( "DNET expected in packet but not provided.", pDADR->exprLine );

         // now check for DADR
         pDADR = GetKeywordValue( &pScriptParm, kwDADR, nlDADR, ScriptNONEMap );

         if ( pDADR != NULL && !pDADR->IsNoneValue() )
            throw ExecError( "DADR expected in packet but not provided.", pDADR->exprLine );
      }

      // if we have an SNET/SADR, check it
      if ((ctrl & 0x08) != 0) {
         // suck out the SNET
         valu = (dec.pktLength--,*dec.pktBuffer++);
         valu = (valu << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // are we looking for a specific SNET?
         MatchEnumExpression( kwSNET, BACnetEnumerated(valu), ScriptNONEMap, "SNET mismatch: " );

         // suck out the length
         int len = (dec.pktLength--,*dec.pktBuffer++);

         // looking for a specific address?
         pSADR = GetKeywordValue( &pScriptParm, kwSADR, nlSADR, ScriptNONEMap );

         if (len == 0)
            throw ExecError( "No SADR in packet", pSADR->exprLine );

         // suck out the address data from the stream even if we aren't testing for DADR
         // This is a silly way to do this... but it forces BACnetOctetString to copy the data
         // into its own buffer instead of just referencing it... :(

         BACnetOctetString bacnetSADRData(BACnetOctetString(dec.pktBuffer, len));
         dec.pktBuffer += len;
         dec.pktLength -= len;

         TestOrAssign(pSADR, bacnetSADRData, nlSADR, pScriptParm, "SADR mismatch: " );
      }
      else
      {
         //OK... SNET/SADR is not present in packet... this will be fine if the script also
         //has nothing specified or NONE specified... but if there is something there it is implied
         //that SNET/SADR should be present in the packet

         // First check for SNET
         BACnetEnumerated bacnetEnum;
         pSADR = GetKeywordValue( &pScriptParm, kwSNET, bacnetEnum, ScriptNONEMap );

         if ( pSADR != NULL && !pSADR->IsNoneValue() )
            throw ExecError( "SNET expected in packet but not provided.", pSADR->exprLine );

         // now check for SADR
         pSADR = GetKeywordValue( &pScriptParm, kwSADR, nlSADR, ScriptNONEMap );

         if ( pSADR != NULL && !pSADR->IsNoneValue() )
            throw ExecError( "SADR expected in packet but not provided.", pSADR->exprLine );
      }

      // if we have a DNET/DADR, check the hop count
      if ((ctrl & 0x20) != 0) {
         // suck out the hop count
         valu = (dec.pktLength--,*dec.pktBuffer++);

         MatchEnumExpression( kwHOPCOUNT, BACnetEnumerated(valu), NULL, "Hop count mismatch: " );
      }

      // match the DER
      // can't rely on value in npdu.pduExpectingReply so suck it out of the control octet
      MatchBoolExpression( kwDER, BACnetBoolean(ctrl & 0x04), "Data expecting reply mismatch: " );

      // match the priority
      // can't rely on value in npdu.pduNetworkPriority so suck it out of control octet
      MatchEnumExpression( kwPRIORITY, BACnetEnumerated(ctrl & 0x03), NULL, "Priority mismatch: " );

      // if this is a network layer message, check it
      if (nlMsg) {
         ScriptTokenList   nlList;

         if (nlMsg->exprOp != '='  &&  !nlMsg->IsAssignment()  && !nlMsg->IsDontCare() )
            throw ExecError( "Equality or assignment operator required for network message", nlMsg->exprLine );

         // resolve the expressions
         ResolveExpr( nlMsg->exprValue, nlMsg->exprLine, nlList, nlMsg->IsAssignment() ? &pScriptParm : NULL );
         if (nlList.Length() < 1)
            throw ExecError( "Network message keyword expected", nlMsg->exprLine );

         // get a reference to the first parameter
         const ScriptToken &t = nlList[0];

         if ( nlMsg->IsAssignment() )
         {
            if ( pScriptParm == NULL )
               throw ExecError( "Undefined script variable for network message assignment", nlMsg->exprLine );

            nlMsgID = *dec.pktBuffer;
            BACnetEnumerated  bacnetMsg(nlMsgID, NetworkSniffer::BAC_STRTAB_NL_msgs);
            StuffScriptParameter( bacnetMsg, pScriptParm, nlMsg->exprValue);
         }
         else if ( nlMsg->IsDontCare() )
         {
            nlMsgID = *dec.pktBuffer;
         }
         else
         {
            // check to see if this is a keyword
            if (t.tokenType == scriptKeyword) {
               nlMsgID = t.Lookup( t.tokenSymbol, ScriptNLMsgTypeMap );
               if ( nlMsgID < 0 )
                  throw ExecError( "Network message unrecognized keyword", nlMsg->exprLine );
            }
            else
               throw ExecError( "Network message keyword expected", nlMsg->exprLine );
         }

         // based on the number, check for other parameters
         try {
            // verify the message type
            if (nlMsgID != (dec.pktLength--,*dec.pktBuffer++))
               throw ExecError( "Network message mismatch", nlMsg->exprLine );

            switch (nlMsgID) {
               case 0:                 // WHO-IS-ROUTER-TO-NETWORK
                  ExpectWhoIsRouterToNetwork( nlList, dec );
                  break;
               case 1:                 // I-AM-ROUTER-TO-NETWORK
                  ExpectIAmRouterToNetwork( nlList, dec );
                  break;
               case 2:                 // I-COULD-BE-ROUTER-TO-NETWORK
                  ExpectICouldBeRouterToNetwork( nlList, dec );
                  break;
               case 3:                 // REJECT-MESSAGE-TO-NETWORK
                  ExpectRejectMessageToNetwork( nlList, dec );
                  break;
               case 4:                 // ROUTER-BUSY-TO-NETWORK
                  ExpectRouterBusyToNetwork( nlList, dec );
                  break;
               case 5:                 // ROUTER-AVAILABLE-TO-NETWORK
                  ExpectRouterAvailableToNetwork( nlList, dec );
                  break;
               case 6:                 // INITIALIZE-ROUTING-TABLE
               case 7:                 // INITIALIZE-ROUTING-TABLE-ACK
                  ExpectInitializeRoutingTable( nlList, dec );
                  break;
               case 8:                 // ESTABLISH-CONNECTION-TO-NETWORK
                  ExpectEstablishConnectionToNetwork( nlList, dec );
                  break;
               case 9:                 // DISCONNECT-CONNECTION-TO-NETWORK
                  ExpectDisconnectConnectionToNetwork( nlList, dec );
                  break;

               // TODO: add a PROPRIETARY keyword, with parms for value and vendor ID
            }
         }
         catch (const char *errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, nlMsg->exprLine );
         }
         catch (const CString &errMsg) {
            // one of the functions had an error
            throw ExecError( errMsg, nlMsg->exprLine );
         }
      }

      // if this is an application layer message, check it
      if ( alMsg ) {
         ScriptTokenList   alList;

         // force property references to fail until context established
         execObjID = 0xFFFFFFFF;

         if (alMsg->exprOp != '='  &&  !alMsg->IsAssignment()  && !alMsg->IsDontCare() )
            throw ExecError( "Equality or assignment operator required for PDU", alMsg->exprLine );

         // resolve the expressions
         ResolveExpr( alMsg->exprValue, alMsg->exprLine, alList, alMsg->IsAssignment() ? &pScriptParm : NULL );
         if (alList.Length() < 1)
            throw ExecError( "AL message keyword expected", alMsg->exprLine );

         // get a reference to the first parameter
         const ScriptToken &t = alList[0];

         int pktMsgID = *dec.pktBuffer >> 4;
         if ( alMsg->IsAssignment() )
         {
            if ( pScriptParm == NULL )
               throw ExecError( "Undefined script variable for PDU assignment", alMsg->exprLine );

            // get the stream's version of what msg ID this is...
            alMsgID = pktMsgID;
            BACnetEnumerated  bacnetMsg(alMsgID, NetworkSniffer::BAC_STRTAB_PDU_typesENUM);
            StuffScriptParameter( bacnetMsg, pScriptParm, alMsg->exprValue);
         }
         else if ( alMsg->IsDontCare() )
         {
            alMsgID = pktMsgID;
         }
         else
         {
            // check to see if this is a keyword
            if ( t.tokenType == scriptKeyword ) {
               alMsgID = t.Lookup( t.tokenSymbol, ScriptALMsgTypeMap );
               if (alMsgID < 0)
                  throw ExecError( "AL Unrecognized keyword", alMsg->exprLine );

               if (pktMsgID != alMsgID)
               {
                  CString strError;
                  strError.Format("Expected PDU type %s; got %s.", 
                  NetworkSniffer::BAC_STRTAB_PDU_typesENUM.EnumString( alMsgID, "PDU_Type"),
                  NetworkSniffer::BAC_STRTAB_PDU_typesENUM.EnumString( pktMsgID, "PDU_Type") );
                  throw ExecError( strError, alMsg->exprLine );
               }
            } else {
               throw ExecError( "AL Keyword expected", alMsg->exprLine );
            }
         }

         // based on the number, check for other parameters
         // (This formerly vetted bits based on the PDU we WANT - not the one we GOT)
         try {
            switch (pktMsgID) {
               case 0:                 // CONFIRMED-REQUEST
                  ThrowPDUBitsError( *dec.pktBuffer & 0x01, "Confirmed-Request", alMsg ); // section 20.1.2.11
                  ExpectConfirmedRequest( dec );
                  break;
               case 1:                 // UNCONFIRMED-REQUEST
                  ThrowPDUBitsError( *dec.pktBuffer & 0x0F, "Unconfirmed-Request", alMsg );  // section 20.1.3.3
                  ExpectUnconfirmedRequest( dec );
                  break;
               case 2:                 // SIMPLEACK
                  ThrowPDUBitsError( *dec.pktBuffer & 0x0F, "Simple-Ack", alMsg );        // section 20.1.4.3
                  ExpectSimpleACK( dec );
                  break;
               case 3:                 // COMPLEXACK
                  ThrowPDUBitsError( *dec.pktBuffer & 0x03, "Complex-Ack", alMsg );       // section 20.1.5.8
                  ExpectComplexACK( dec );
                  break;
               case 4:                 // SEGMENTACK
                  ThrowPDUBitsError( *dec.pktBuffer & 0x0C, "Segment-Ack", alMsg );       // section 20.1.6.6
                  ExpectSegmentACK( dec );
                  break;
               case 5:                 // ERROR
                  ThrowPDUBitsError( *dec.pktBuffer & 0x0F, "Error", alMsg );             // section 20.1.7.4
                  ExpectError( dec );
                  break;
               case 6:                 // REJECT
                  ThrowPDUBitsError( *dec.pktBuffer & 0x0F, "Reject", alMsg );            // section 20.1.8.3
                  ExpectReject( dec );
                  break;
               case 7:                 // ABORT
                  ThrowPDUBitsError( *dec.pktBuffer & 0x0E, "Abort", alMsg );             // section 20.1.9.4
                  ExpectAbort( dec );
                  break;
            }
         }
         catch (const char *errMsg)
         {
            // one of the functions had an error
            throw ExecError( errMsg, alMsg->exprLine );
         }
         catch (const CString &errMsg)
         {
            throw ExecError( errMsg, alMsg->exprLine );
         }
      }
   }
   catch (const ExecError &err) {
      // failed
      Msg( 3, err.errLineNo, err.errMsg );
      return false;
   }
   catch (const char *errMsg) {
      // failed
      Msg( 3, execPacket->baseLineStart, errMsg );
      return false;
   }

   // we made it!
   return true;
}


//madanner 6/03, check for reserved zero bits in PDU

void ScriptExecutor::ThrowPDUBitsError( unsigned char b, char * pszPDUType, ScriptPacketExprPtr pep )
{
   if ( b != 0 )
   {
      BACnetBitString bacnetBits(8, &b);
      CString strError;
      strError.Format("Reserved bits of %s PDU control octet must be zero (%s).", pszPDUType, bacnetBits.ToString());
      throw ExecError(strError, pep != NULL ? pep->exprLine : 0);
   }
}


//
// ScriptExecutor::ExpectBVLCResult
//

void ScriptExecutor::ExpectBVLCResult( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int      valu1, valu2;

   TRACE0( "ExpectBVLCResult\n" );

   switch (tlist.Length()) {
      case 1:
         throw "Result code expected";
      case 2:
         {
            const ScriptToken &dnet = tlist[1];

            if (dnet.tokenType != scriptValue)
               throw "Result code expected";

            // madanner 11/4/02
            // account for don't care case.  Not sure if this is a good thing or not.

            bool fDontCare = dnet.IsDontCare();

            if ( !fDontCare )
            {
               if ( !dnet.IsInteger(valu1))
                  throw "Result code invalid format";
               if ((valu1 < 0) || (valu1 > 65535))
                  throw "Result code out of range (0..65535)";
            }

            TRACE1( "    Result code %d\n", valu1 );

            // extract the value
            valu2 = (dec.pktLength--,*dec.pktBuffer++);
            valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

            // check for a match
            if ( !fDontCare && valu1 != valu2)
               throw "Result code mismatch";

            break;
         }
      default:
         throw "Too many values in BVLC-Result message";
   }
}

//
// ScriptExecutor::ExpectWriteBDT
//

void ScriptExecutor::ExpectWriteBDT( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int               i;
   unsigned long     host1, host2, mask1, mask2;
   unsigned short    port1, port2;

   TRACE0( "ExpectWriteBDT\n" );

   try {
      for (i = 1; i < tlist.Length(); i++ ) {
         const ScriptToken &t = tlist[i];

         if (t.tokenType != scriptValue)
            throw "IP address expected";

         // madanner 11/4/02
         // account for don't care case.  Not sure if this is a good thing or not.

         bool fDontCare = t.IsDontCare();

         if ( !fDontCare )
         {
            if (t.tokenEnc != scriptIPEnc)
               throw "IP address required";

            // convert to host, port and network mask
            BACnetIPAddr::StringToHostPort( t.tokenValue, &host1, &mask1, &port1 );
         }

         // make sure there's enough data
         if (dec.pktLength < 10)
            throw "Additional BDT entries expected";

         // extract the host
         for (int j = 0; j < 4; j++)
            host2 = (host2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare && host1 != host2)
            throw "Host address mismatch";

         // extract the port
         port2 = (dec.pktLength--,*dec.pktBuffer++);
         port2 = (port2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if (!fDontCare && port1 != port2)
            throw "Port mismatch";

         // extract the mask
         for (int k = 0; k < 4; k++)
            mask2 = (mask2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare &&  mask1 != mask2)
            throw "Subnet mask mismatch";
      }

      if (dec.pktLength != 0)
         throw "Additional BDT entries not matched";
   }
   catch (const char *msg)
   {
      static char msgBuff[80];

      sprintf( msgBuff, "%s (entry %d)", msg, i );
      throw msgBuff;
   }
}

//
// ScriptExecutor::ExpectReadBDTAck
//

void ScriptExecutor::ExpectReadBDTAck( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int               i;
   unsigned long     host1, host2, mask1, mask2;
   unsigned short    port1, port2;

   TRACE0( "ExpectReadBDTAck\n" );

   try {
      for (i = 1; i < tlist.Length(); i++ ) {
         const ScriptToken &t = tlist[i];

         if (t.tokenType != scriptValue)
            throw "IP address expected";

         // madanner 11/4/02
         // account for don't care case.  Not sure if this is a good thing or not.

         bool fDontCare = t.IsDontCare();

         if ( !fDontCare )
         {
            if (t.tokenEnc != scriptIPEnc)
               throw "IP address required";

            // convert to host, port and network mask
            BACnetIPAddr::StringToHostPort( t.tokenValue, &host1, &mask1, &port1 );
         }

         // make sure there's enough data
         if (dec.pktLength < 10)
            throw "Additional BDT entries expected";

         // extract the host
         for (int j = 0; j < 4; j++)
            host2 = (host2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare && host1 != host2)
            throw "Host address mismatch";

         // extract the port
         port2 = (dec.pktLength--,*dec.pktBuffer++);
         port2 = (port2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare && port1 != port2)
            throw "Port mismatch";

         // extract the mask
         for (int k = 0; k < 4; k++)
            mask2 = (mask2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare && mask1 != mask2)
            throw "Subnet mask mismatch";
      }

      if (dec.pktLength != 0)
         throw "Additional BDT entries not matched";
   }
   catch (const char *msg)
   {
      static char msgBuff[80];

      sprintf( msgBuff, "%s (entry %d)", msg, i );
      throw msgBuff;
   }
}

//
// ScriptExecutor::ExpectForwardedNPDU
//

void ScriptExecutor::ExpectForwardedNPDU( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   TRACE0( "ExpectForwardedNPDU\n" );

   switch (tlist.Length()) {
      case 1:
         throw "IP address required";
      case 2:
         {
            unsigned long     host1, host2 = 0, mask1;
            unsigned short    port1, port2;

            const ScriptToken &t = tlist[1];

            if (t.tokenType != scriptValue)
               throw "IP address expected";

            // madanner 11/4/02
            // account for don't care case.  Not sure if this is a good thing or not.

            bool fDontCare = t.IsDontCare();

            if ( !fDontCare )
            {
               if (t.tokenEnc != scriptIPEnc)
                  throw "IP address required";

               // convert to host, port and network mask
               BACnetIPAddr::StringToHostPort( t.tokenValue, &host1, &mask1, &port1 );
            }

            // make sure there's enough data
            if (dec.pktLength < 6)
               throw "B/IP address expected";

            // extract the host
            for (int j = 0; j < 4; j++)
               host2 = (host2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

            // see if they match
            if ( !fDontCare && host1 != host2)
               throw "Host address mismatch";

            // extract the port
            port2 = (dec.pktLength--,*dec.pktBuffer++);
            port2 = (port2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

            // see if they match
            if ( !fDontCare && port1 != port2)
               throw "Port mismatch";
            break;
         }
      default:
         throw "Too many values";
   }
}

//
// ScriptExecutor::ExpectRegisterFD
//

void ScriptExecutor::ExpectRegisterFD( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int  valu1, valu2;
   bool fDontCare = false;

   TRACE0( "ExpectRegisterFD\n" );

   switch (tlist.Length()) {
      case 1:
         throw "Time-to-live required";
      case 2:
         {
            const ScriptToken &dnet = tlist[1];

            if (dnet.tokenType != scriptValue)
               throw "Time-to-live expected";

            // madanner 11/4/02
            // account for don't care case.  Not sure if this is a good thing or not.

            bool fDontCare = dnet.IsDontCare();

            if ( !fDontCare )
            {
               if (!dnet.IsInteger(valu1))
                  throw "Time-to-live invalid format";
               if ((valu1 < 0) || (valu1 > 65535))
                  throw "Time-to-live out of range (0..65535)";
            }

            TRACE1( "    TTL %d\n", valu1 );

            // extract the value
            valu2 = (dec.pktLength--,*dec.pktBuffer++);
            valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

            // check for a match
            if ( !fDontCare && valu1 != valu2)
               throw "Time-to-live mismatch";

            break;
         }
      default:
         throw "Too many values in Register-Foreign-Device message";
   }
}

//
// ScriptExecutor::ExpectReadFDTAck
//

void ScriptExecutor::ExpectReadFDTAck( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int               i, entry, valu1, valu2;
   unsigned long     host1, host2, mask1;
   unsigned short    port1, port2;

   TRACE0( "ExpectReadFDTAck\n" );

   try {
      for (i = 1, entry = 1; i < tlist.Length(); entry++ ) {
         const ScriptToken &t = tlist[i++];

         if (t.tokenType != scriptValue)
            throw "IP address expected";

         // madanner 11/4/02
         // account for don't care case.  Not sure if this is a good thing or not.

         bool fDontCare = t.IsDontCare();

         if ( !fDontCare )
         {
            if (t.tokenEnc != scriptIPEnc)
               throw "IP address required";

            // convert to host, port and network mask
            BACnetIPAddr::StringToHostPort( t.tokenValue, &host1, &mask1, &port1 );
         }

         // make sure there's enough data
         if (dec.pktLength < 10)
            throw "Additional FDT entries expected";

         // extract the host
         for (int j = 0; j < 4; j++)
            host2 = (host2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare &&  host1 != host2)
            throw "Host address mismatch";

         // extract the port
         port2 = (dec.pktLength--,*dec.pktBuffer++);
         port2 = (port2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare &&  port1 != port2)
            throw "Port mismatch";

         // extract the ttl
         if (i >= tlist.Length())
            throw "Time-to-live expected";
         const ScriptToken &ttl = tlist[i++];

         if (ttl.tokenType != scriptValue)
            throw "Time-to-live expected";

         fDontCare = ttl.IsDontCare();

         if ( !fDontCare )
         {
            if (!ttl.IsInteger(valu1))
               throw "Time-to-live invalid format";
            if ((valu1 < 0) || (valu1 > 65535))
               throw "Time-to-live out of range (0..65535)";
         }

         TRACE1( "    TTL %d\n", valu1 );

         // extract the value
         valu2 = (dec.pktLength--,*dec.pktBuffer++);
         valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // check for a match
         if ( !fDontCare && valu1 != valu2)
            throw "Time-to-live mismatch";

         // extract the tr
         if (i >= tlist.Length())
            throw "Time remaining expected";
         const ScriptToken &tr = tlist[i++];

         if (tr.tokenType != scriptValue)
            throw "Time remaining expected";

         fDontCare = tr.IsDontCare();

         if ( !fDontCare )
         {
            if (!tr.IsInteger(valu1))
               throw "Time remaining invalid format";
            if ((valu1 < 0) || (valu1 > 65535))
               throw "Time remaining out of range (0..65535)";
         }

         TRACE1( "    TR %d\n", valu1 );

         // extract the value
         valu2 = (dec.pktLength--,*dec.pktBuffer++);
         valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // check for a match
         if ( !fDontCare && valu1 != valu2)
            throw "Time remaining mismatch";
      }

      if (dec.pktLength != 0)
         throw "Additional BDT entries not matched";
   }
   catch (const char *msg)
   {
      static char msgBuff[80];

      sprintf( msgBuff, "%s (entry %d)", msg, entry );
      throw msgBuff;
   }
}

//
// ScriptExecutor::ExpectDeleteFDTEntry
//

void ScriptExecutor::ExpectDeleteFDTEntry( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   TRACE0( "ExpectDeleteFDTEntry\n" );

   switch (tlist.Length()) {
      case 1:
         throw "IP address required";
      case 2:
         {
            unsigned long     host1, host2 = 0, mask1;
            unsigned short    port1, port2;

            const ScriptToken &t = tlist[1];

            if (t.tokenType != scriptValue)
               throw "IP address expected";

            bool fDontCare = t.IsDontCare();

            if ( !fDontCare )
            {
               if (t.tokenEnc != scriptIPEnc)
                  throw "IP address required";

               // convert to host, port and network mask
               BACnetIPAddr::StringToHostPort( t.tokenValue, &host1, &mask1, &port1 );
            }

            // make sure there's enough data
            if (dec.pktLength < 6)
               throw "B/IP address expected";

            // extract the host
            for (int j = 0; j < 4; j++)
               host2 = (host2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

            // see if they match
            if ( !fDontCare && host1 != host2)
               throw "Entry address mismatch";

            // extract the port
            port2 = (dec.pktLength--,*dec.pktBuffer++);
            port2 = (port2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

            // see if they match
            if ( !fDontCare && port1 != port2)
               throw "Port mismatch";
            break;
         }
      default:
         throw "Too many values";
   }
}

//
// ScriptExecutor::ExpectWhoIsRouterToNetwork
//

void ScriptExecutor::ExpectWhoIsRouterToNetwork( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int      valu1, valu2;

   TRACE0( "ExpectWhoIsRouterToNetwork\n" );

   switch (tlist.Length()) {
      case 1:
         if (dec.pktLength != 0)
            throw "Network not matched";
         break;
      case 2:
         {
            const ScriptToken &dnet = tlist[1];

            if (dnet.tokenType != scriptValue)
               throw "Network expected";

            bool fDontCare = dnet.IsDontCare();

            if ( !fDontCare )
            {
               if (!dnet.IsInteger(valu1))
                  throw "Network invalid format";
               if ((valu1 < 0) || (valu1 > 65534))
                  throw "Network out of range (0..65534)";
            }

            TRACE1( "    Network %d\n", valu1 );

            // extract the value
            valu2 = (dec.pktLength--,*dec.pktBuffer++);
            valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

            // check for a match
            if ( !fDontCare && valu1 != valu2)
               throw "Network mismatch";

            break;
         }
      default:
         throw "Too many values in Who-Is-Router-To-Network message";
   }
}

//
// ScriptExecutor::ExpectIAmRouterToNetwork
//

void ScriptExecutor::ExpectIAmRouterToNetwork( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int               i, valu1, valu2;

   TRACE0( "ExpectIAmRouterToNetwork\n" );

   if (tlist.Length() == 1)
      throw "At least one network number required";

   try {
      for (i = 1; i < tlist.Length(); i++ ) {
         const ScriptToken &t = tlist[i];

         if (t.tokenType != scriptValue)
            throw "Network expected";

         bool fDontCare = t.IsDontCare();

         if ( !fDontCare )
         {
            if (!t.IsInteger(valu1))
               throw "Network invalid format";
            if ((valu1 < 0) || (valu1 > 65534))
               throw "Network out of range (0..65534)";
         }

         TRACE1( "    Network %d\n", valu1 );

         // got at least one more?
         if (dec.pktLength < 2)
            throw "Additional network expected";

         // extract the port
         valu2 = (dec.pktLength--,*dec.pktBuffer++);
         valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare && valu1 != valu2)
            throw "Network mismatch";
      }

      if (dec.pktLength != 0)
         throw "Additional network numbers not matched";
   }
   catch (const char *msg)
   {
      static char msgBuff[80];

      sprintf( msgBuff, "%s (entry %d)", msg, i );
      throw msgBuff;
   }
}

//
// ScriptExecutor::ExpectICouldBeRouterToNetwork
//

void ScriptExecutor::ExpectICouldBeRouterToNetwork( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int      valu1, valu2;

   TRACE0( "ExpectICouldBeRouterToNetwork\n" );

   if (tlist.Length() != 3)
      throw "DNET and performance index required";

   const ScriptToken &dnet = tlist[1];

   if (dnet.tokenType != scriptValue)
      throw "DNET expected";

   bool fDontCare = dnet.IsDontCare();

   if ( !fDontCare )
   {
      if (!dnet.IsInteger(valu1))
         throw "DNET invalid format";
      if ((valu1 < 0) || (valu1 > 65534))
         throw "DNET out of range (0..65534)";
   }

   TRACE1( "    Network %d\n", valu1 );

   // extract the network
   valu2 = (dec.pktLength--,*dec.pktBuffer++);
   valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

   // see if they match
   if ( !fDontCare && valu1 != valu2)
      throw "Network mismatch";

   const ScriptToken &perf = tlist[2];

   if (perf.tokenType != scriptValue)
      throw "Performance index expected";

   fDontCare = perf.IsDontCare();

   if ( !fDontCare )
   {
      if (!perf.IsInteger(valu1))
         throw "Performance index invalid format";
      if ((valu1 < 0) || (valu1 > 255))
         throw "Performance index out of range (0..255)";
   }

   TRACE1( "    Performance %d\n", valu1 );

   // extract the network
   valu2 = (dec.pktLength--,*dec.pktBuffer++);

   // see if they match
   if ( !fDontCare && valu1 != valu2)
      throw "Performance index mismatch";
}

//
// ScriptExecutor::ExpectRejectMessageToNetwork
//

void ScriptExecutor::ExpectRejectMessageToNetwork( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int      valu1, valu2;

   TRACE0( "ExpectRejectMessageToNetwork\n" );

   if (tlist.Length() != 3)
      throw "Reject reason and DNET required";

   const ScriptToken &reason = tlist[1];

   if (reason.tokenType != scriptValue)
      throw "Reject reason expected";

   bool fDontCare = reason.IsDontCare();

   if ( !fDontCare )
   {
      if (!reason.IsInteger(valu1))
         throw "Reject reason invalid format";
      if ((valu1 < 0) || (valu1 > 255))
         throw "Reject reason out of range (0..255)";
   }

   TRACE1( "    Reject reason %d\n", valu1 );

   // extract the network
   valu2 = (dec.pktLength--,*dec.pktBuffer++);

   // see if they match
   if ( !fDontCare && valu1 != valu2)
      throw "Reject reason mismatch";

   const ScriptToken &dnet = tlist[2];

   if (dnet.tokenType != scriptValue)
      throw "DNET expected";

   fDontCare = dnet.IsDontCare();

   if ( !fDontCare )
   {
      if (!dnet.IsInteger(valu1))
         throw "DNET invalid format";
      if ((valu1 < 0) || (valu1 > 65534))
         throw "DNET out of range (0..65534)";
   }

   TRACE1( "    DNET %d\n", valu1 );

   // extract the network
   valu2 = (dec.pktLength--,*dec.pktBuffer++);
   valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

   // see if they match
   if ( !fDontCare && valu1 != valu2)
      throw "Network mismatch";
}

//
// ScriptExecutor::ExpectRouterBusyToNetwork
//

void ScriptExecutor::ExpectRouterBusyToNetwork( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int               i, valu1, valu2;

   TRACE0( "ExpectRouterBusyToNetwork\n" );

   try {
      for (i = 1; i < tlist.Length(); i++ ) {
         const ScriptToken &t = tlist[i];

         if (t.tokenType != scriptValue)
            throw "Network expected";

         bool fDontCare = t.IsDontCare();

         if ( !fDontCare )
         {
            if (!t.IsInteger(valu1))
               throw "Network invalid format";
            if ((valu1 < 0) || (valu1 > 65534))
               throw "Network out of range (0..65534)";
         }

         TRACE1( "    Network %d\n", valu1 );

         // got at least one more?
         if (dec.pktLength < 2)
            throw "Additional network expected";

         // extract the port
         valu2 = (dec.pktLength--,*dec.pktBuffer++);
         valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare && valu1 != valu2)
            throw "Network mismatch";
      }

      if (dec.pktLength != 0)
         throw "Additional network numbers not matched";
   }
   catch (const char *msg) {
      static char msgBuff[80];

      sprintf( msgBuff, "%s (entry %d)", msg, i );
      throw msgBuff;
   }
}

//
// ScriptExecutor::ExpectRouterAvailableToNetwork
//

void ScriptExecutor::ExpectRouterAvailableToNetwork( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int               i, valu1, valu2;

   TRACE0( "ExpectRouterAvailableToNetwork\n" );

   try {
      for (i = 1; i < tlist.Length(); i++ ) {
         const ScriptToken &t = tlist[i];

         if (t.tokenType != scriptValue)
            throw "Network expected";

         bool fDontCare = t.IsDontCare();

         if ( !fDontCare )
         {
            if (!t.IsInteger(valu1))
               throw "Network invalid format";
            if ((valu1 < 0) || (valu1 > 65534))
               throw "Network out of range (0..65534)";
         }

         TRACE1( "    Network %d\n", valu1 );

         // got at least one more?
         if (dec.pktLength < 2)
            throw "Additional network expected";

         // extract the network
         valu2 = (dec.pktLength--,*dec.pktBuffer++);
         valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

         // see if they match
         if ( !fDontCare && valu1 != valu2)
            throw "Network mismatch";
      }

      if (dec.pktLength != 0)
         throw "Additional network numbers not matched";
   }
   catch (const char *msg) {
      static char msgBuff[80];

      sprintf( msgBuff, "%s (entry %d)", msg, i );
      throw msgBuff;
   }
}

//
// ScriptExecutor::ExpectInitializeRoutingTable
//

void ScriptExecutor::ExpectInitializeRoutingTable( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int                  valu1, valu2;
   BACnetCharacterString   cstr;

   TRACE0( "ExpectInitializeRoutingTable[Ack]\n" );

   // extract the number of ports
   valu1 = (dec.pktLength--,*dec.pktBuffer++);
   if (valu1 != (tlist.Length() - 1) / 3 )
      throw "Port count mismatch";

   for (int i = 1; i < tlist.Length(); ) {
      const ScriptToken &dnet = tlist[i++];

      if (dnet.tokenType != scriptValue)
         throw "DNET expected";

      bool fDontCare = dnet.IsDontCare();

      if ( !fDontCare )
      {
         if (!dnet.IsInteger(valu1))
            throw "DNET invalid format";
         if ((valu1 < 0) || (valu1 > 65534))
            throw "DNET out of range (0..65534)";
      }

      TRACE1( "    Network %d\n", valu1 );

      // extract the network
      valu2 = (dec.pktLength--,*dec.pktBuffer++);
      valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

      // see if they match
      if ( !fDontCare &&  valu1 != valu2)
         throw "Network mismatch";

      if (i >= tlist.Length())
         throw "Port ID expected";
      const ScriptToken &portID = tlist[i++];

      if (portID.tokenType != scriptValue)
         throw "Port ID expected";

      fDontCare = portID.IsDontCare();

      if ( !fDontCare )
      {
         if (!portID.IsInteger(valu1))
            throw "Port ID invalid format";
      }

      TRACE1( "    Port ID %d\n", valu1 );

      // extract the port ID
      valu2 = (dec.pktLength--,*dec.pktBuffer++);

      // see if they match
      if ( !fDontCare && valu1 != valu2)
         throw "Port ID mismatch";

      if (i >= tlist.Length())
         throw "Port information expected";
      const ScriptToken &portInfo = tlist[i++];

      if (portInfo.tokenType != scriptValue)
         throw "Port information expected";

      fDontCare = portInfo.IsDontCare();

      if ( !fDontCare )
      {
         if (!portInfo.IsEncodeable( cstr ))
            throw "Port information invalid format";
         if (cstr.strEncoding != 0)
            throw "Port information must be ASCII encoded";
      }

      // check the port information content
      if (cstr.strLen != (dec.pktLength--,*dec.pktBuffer++))
         throw "Port information mismatch";
      for (unsigned j = 0; j < cstr.strLen; j++)
         if (cstr.strBuff[j] != (dec.pktLength--,*dec.pktBuffer++))
            if ( !fDontCare )
               throw "Port information mismatch";
   }
}

//
// ScriptExecutor::ExpectEstablishConnectionToNetwork
//

void ScriptExecutor::ExpectEstablishConnectionToNetwork( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int      valu1, valu2;

   TRACE0( "ExpectEstablishConnectionToNetwork\n" );

   if (tlist.Length() != 3)
      throw "DNET and termination required";

   const ScriptToken &dnet = tlist[1];

   if (dnet.tokenType != scriptValue)
      throw "DNET expected";

   bool fDontCare = dnet.IsDontCare();

   if ( !fDontCare )
   {
      if (!dnet.IsInteger(valu1))
         throw "DNET invalid format";
      if ((valu1 < 0) || (valu1 > 65534))
         throw "DNET out of range (0..65534)";
   }

   TRACE1( "    Network %d\n", valu1 );

   // extract the network
   valu2 = (dec.pktLength--,*dec.pktBuffer++);
   valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

   // see if they match
   if ( !fDontCare &&  valu1 != valu2)
      throw "Network mismatch";

   const ScriptToken &term = tlist[2];

   if (term.tokenType != scriptValue)
      throw "Termination time expected";

   fDontCare = term.IsDontCare();

   if ( !fDontCare )
   {
      if (!term.IsInteger(valu1))
         throw "Termination time invalid format";
   }

   TRACE1( "    Termination time %d\n", valu1 );

   // extract the network
   valu2 = (dec.pktLength--,*dec.pktBuffer++);

   // see if they match
   if ( !fDontCare &&  valu1 != valu2)
      throw "Termination time mismatch";
}

//
// ScriptExecutor::ExpectDisconnectConnectionToNetwork
//

void ScriptExecutor::ExpectDisconnectConnectionToNetwork( ScriptTokenList &tlist, BACnetAPDUDecoder &dec )
{
   int      valu1, valu2;

   TRACE0( "ExpectDisconnectConnectionToNetwork\n" );

   if (tlist.Length() != 2)
      throw "DNET required";

   const ScriptToken &dnet = tlist[1];

   if (dnet.tokenType != scriptValue)
      throw "DNET expected";

   bool fDontCare = dnet.IsDontCare();

   if ( !fDontCare )
   {
      if (!dnet.IsInteger(valu1))
         throw "DNET invalid format";
      if ((valu1 < 0) || (valu1 > 65534))
         throw "DNET out of range (0..65534)";
   }

   TRACE1( "    Network %d\n", valu1 );

   // extract the network
   valu2 = (dec.pktLength--,*dec.pktBuffer++);
   valu2 = (valu2 << 8) + (dec.pktLength--,*dec.pktBuffer++);

   // see if they match
   if ( !fDontCare &&  valu1 != valu2)
      throw "Network mismatch";
}

//
// ScriptExecutor::ExpectDevPacket
//

bool ScriptExecutor::ExpectDevPacket( const BACnetAPDU &apdu )
{
   int                  valu;
   BACnetOctetString    nlSource;
   BACnetAddress        nlSourceAddr;
   BACnetBoolean        nlDER;
   BACnetInteger        nlPriority, nlDNET;
   ScriptPacketExprPtr  pNet, bipMsg, nlMsg, alMsg, nlSA, pPriority;
   ScriptTokenList      alList;
   ScriptParmPtr        pScriptParm = NULL;        // used in various places for variable assignment

   try {
      // see if the netework is provided
      pNet = execPacket->packetExprList.Find( kwNETWORK );

      // see if this should have a BVLCI header
      bipMsg = execPacket->packetExprList.Find( kwBVLCI );

      // see if we're looking for a network layer message
      nlMsg = execPacket->packetExprList.Find( kwMESSAGE );
      if (!nlMsg)
         nlMsg = execPacket->packetExprList.Find( kwMSG );

      // see if we're looking for an application layer message
      alMsg = execPacket->packetExprList.Find( kwPDU );

      // see if we should be matching, it should only be an application
      // layer packet.  No need to throw anything.
      if (!(!pNet && !bipMsg && !nlMsg && alMsg))
         return false;

      // If the code supports multiple device objects in the future there 
      // needs to be a parameter that decribes which device object this 
      // packet should match.  Hunt through the gMasterDevice list for the 
      // one specified.

      nlSA = execPacket->packetExprList.Find( kwSA );
      if (!nlSA) {
         if ( !execDB->LoadNamedAddress(&nlSourceAddr, "IUT") )
            throw ExecError( "Default source address IUT not found", execPacket->baseLineStart );
      } else {
         if (nlSA->exprOp != '='  &&  !nlSA->IsAssignment() )
            throw ExecError( "Equality or assignment operator required for source address", nlSA->exprLine );
         
         ScriptTokenList   saList;

         // resolve the expressions
         pScriptParm = NULL;
         ResolveExpr( nlSA->exprValue, nlSA->exprLine, saList, nlSA->IsAssignment() ? &pScriptParm : NULL );
         if (saList.Length() < 1)
            throw ExecError( "Address, variable or name expected", nlSA->exprLine );

         // get a reference to the first parameter
         const ScriptToken &t = saList[0];

         // If this is an assignment... just make sure a param has been found for stuffing...
         if ( nlSA->IsAssignment() )
         {
            if ( pScriptParm == NULL )
               throw ExecError( "Undefined script variable for DA assignment", nlSA->exprLine );

            // See if we can find the provided address in our list of known names... if so, then use
            // that name (in quotes) for stuffing the parameter.
            // If not... stuff the parameter with the encoded (ASCII) address... it has to be decodeable
            // for use in other SEND statements.

            LPCSTR lpszName;

            if ( (lpszName = execDB->AddrToName(apdu.apduAddr)) != NULL )
            {
               // found a name in our table that matches this address... encode the name into the variable
               StuffScriptParameter( BACnetCharacterString(lpszName), pScriptParm, nlSA->exprValue);
            }
            else
            {
               // supplied address is unknown... just setup the thing as an octet string into the variable
               StuffScriptParameter( BACnetOctetString((BACnetOctet *) apdu.apduAddr.addrAddr, (int) apdu.apduAddr.addrLen), pScriptParm, nlSA->exprValue);
            }
         }
         else
         {
            // check for name or octet string
            if (saList.Length() == 1) 
            {
               // it might be a name
               if ((t.tokenType == scriptValue) && (t.tokenEnc == scriptASCIIEnc)) {
                  if ( !execDB->LoadNamedAddress(&nlSourceAddr, t.RemoveQuotes()) )
                     throw ExecError( "Destination address name not found", nlSA->exprLine );
               } else
               // it might be an IP address
               if ((t.tokenType == scriptValue) && (t.tokenEnc == scriptIPEnc)) {
                  BACnetIPAddr nlIPAddr( t.tokenValue );
                  nlSourceAddr = nlIPAddr;
               } else
               // it might be an explicit octet string
               if (t.IsEncodeable( nlSource )) {
                  nlSourceAddr.LocalStation( nlSource.strBuff, nlSource.strLen );
               } else
                  throw ExecError( "Source address expected", nlSA->exprLine );
            } 
            else if (saList.Length() == 2) 
            {
               if (t.tokenType != scriptValue)
                  throw "SNET expected";
               if (!t.IsInteger(valu))
                  throw "SNET invalid format, integer required";
               if ((valu < 0) || (valu > 65534))
                  throw "DNET out of range (0..65534)";

               const ScriptToken &sadr = saList[1];

               // it might be an IP address
               if ((sadr.tokenType == scriptValue) && (sadr.tokenEnc == scriptIPEnc)) {
                  BACnetIPAddr nlIPAddr( sadr.tokenValue );
                  nlSourceAddr = nlIPAddr;
               } 
               else if (sadr.IsEncodeable( nlSource )) 
               {
                  // it might be an explicit octet string
                  nlSourceAddr.LocalStation( nlSource.strBuff, nlSource.strLen );
               } 
               else
               {
                  throw ExecError( "Source address expected", nlSA->exprLine );
               }

               nlSourceAddr.addrType = remoteStationAddr;
               nlSourceAddr.addrNet = valu;
            } 
            else
            {
               throw ExecError( "Source address expected", nlSA->exprLine );
            }
         }
      }

      // make sure the addresses match
      // but only if we're not assigning

      if ((!nlSA || !nlSA->IsAssignment()) && !(apdu.apduAddr == nlSourceAddr))
      {
         throw ExecError( "Source address mismatch"
               , (nlSA ? nlSA->exprLine : execPacket->baseLineStart) );
      }

      // force property references to fail until context established
      execObjID = 0xFFFFFFFF;

      // test operator for PDU expression...
      // madanner 11/12/02, added to support assignment in PDU

      BACnetEnumerated  bacnetAPDUData(apdu.apduType, NetworkSniffer::BAC_STRTAB_PDU_typesENUM);
      MatchEnumExpression( kwPDU, bacnetAPDUData, ScriptALMsgTypeMap, "PDU type mismatch: " );

      // get some interesting keywords that might match
      // moved these tests (DER and PRIORITY) before the decoding of the PDU

      MatchBoolExpression( kwDER, BACnetBoolean(apdu.apduExpectingReply), "Expecting reply mismatch: " );

      pPriority   = GetKeywordValue( &pScriptParm, kwPRIORITY, nlPriority );
      if (!pPriority)
         pPriority = GetKeywordValue( &pScriptParm, kwPRIO, nlPriority );

      TestOrAssign(pPriority, BACnetInteger(apdu.apduNetworkPriority), nlPriority, pScriptParm, "Network priority mismatch: " );

      // based on the number, check for other parameters
      try {
         switch (bacnetAPDUData.m_enumValue) {
            case 0:                 // CONFIRMED-REQUEST
               ExpectDevConfirmedRequest( apdu );
               break;
            case 1:                 // UNCONFIRMED-REQUEST
               ExpectDevUnconfirmedRequest( apdu );
               break;
            case 2:                 // SIMPLEACK
               ExpectDevSimpleACK( apdu );
               break;
            case 3:                 // COMPLEXACK
               ExpectDevComplexACK( apdu );
               break;
            case 4:                 // SEGMENTACK
               ExpectDevSegmentACK( apdu );
               break;
            case 5:                 // ERROR
               ExpectDevError( apdu );
               break;
            case 6:                 // REJECT
               ExpectDevReject( apdu );
               break;
            case 7:                 // ABORT
               ExpectDevAbort( apdu );
               break;
         }
      }
      catch (const char *errMsg)
      {
         // one of the functions had an error
         throw ExecError( errMsg, alMsg->exprLine );
      }
      catch (const CString &errMsg )
      {
         throw ExecError( errMsg, alMsg->exprLine );
      }
   }
   catch (const ExecError &err) {
      // failed
      Msg( 3, err.errLineNo, err.errMsg );
      return false;
   }
   catch (const char *errMsg) {
      // failed
      Msg( 3, execPacket->baseLineStart, errMsg );
      return false;
   }

   // we made it!
   return true;
}


void ScriptExecutor::TestOrAssign( ScriptPacketExprPtr pScriptExpr, BACnetEncodeable & rbacnetData, BACnetEncodeable & rbacnetScript, ScriptParmPtr pScriptParm, const char * pszErrorPrefix )
{
   // Return OK because the script expression doesn't actually exist in the script...
   if ( pScriptExpr == NULL )
      return;

   CString strError;

   // If the none value is set then we can't be assigning things, we shouldn't even be here to perform a test so
   // let's just get out with a failure

   if ( pScriptExpr->IsNoneValue() )
   {
      strError.Format("%s : Packet value (%s) must not be present.", pszErrorPrefix, rbacnetData.ToString());
      throw ExecError(strError, pScriptExpr->exprLine);
   }

   // if script parm is not null, then we must want to assign this value instead of test it
   if ( pScriptParm != NULL )
   {
      StuffScriptParameter( rbacnetData, pScriptParm, pScriptExpr->exprValue);
   }
   else if ( !rbacnetData.Match(rbacnetScript, pScriptExpr->exprOp, &strError) )
   {
      strError = pszErrorPrefix + strError;
      throw ExecError( strError, pScriptExpr->exprLine );
   }
}

//
// ScriptExecutor::ExpectDevConfirmedRequest
//

void ScriptExecutor::ExpectDevConfirmedRequest( const BACnetAPDU &apdu )
{
   // Check the service choice
   BACnetEnumerated  bacnetServiceData(apdu.apduService, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice );
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALConfirmedServiceMap, "Confirmed Request Service mismatch: " );

   // Check the invokeID
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(apdu.apduInvokeID), NULL, "Confirmed Request InvokeID mismatch: " );

   // expect the rest
   BACnetAPDUDecoder dec( apdu );
   ExpectALData( dec );
   ExpectNoMore(dec);
}



void ScriptExecutor::ExpectNoMore( BACnetAPDUDecoder & dec )
{
   // make sure all the data was matched
   if ( dec.pktLength != 0 )
      throw ExecError( "Additional application data not matched", execPacket->baseLineStart);
}


void ScriptExecutor::MatchEnumExpression( int nKeyword, BACnetEnumerated &rbacnetData, ScriptTranslateTablePtr pMap, const char * pszErrorPrefix )
{
   ScriptPacketExprPtr     pExpression;
   ScriptParmPtr        pScriptParm = NULL;        // used for variable assignment
   BACnetEnumerated     bacnetScript;

   bacnetScript = rbacnetData;         // point to same enum table and such

   pExpression = GetKeywordValue( &pScriptParm, nKeyword, bacnetScript, pMap );
   TestOrAssign(pExpression, rbacnetData, bacnetScript, pScriptParm, pszErrorPrefix );
}


void ScriptExecutor::MatchBoolExpression( int nKeyword, BACnetBoolean &rbacnetData, const char * pszErrorPrefix )
{
   ScriptPacketExprPtr     pExpression;
   ScriptParmPtr        pScriptParm = NULL;        // used for variable assignment
   BACnetBoolean        bacnetScript;

   bacnetScript = rbacnetData;         // point to same enum table and such

   pExpression = GetKeywordValue( &pScriptParm, nKeyword, bacnetScript );
   TestOrAssign(pExpression, rbacnetData, bacnetScript, pScriptParm, pszErrorPrefix );
}

//
// ScriptExecutor::ExpectDevUnconfirmedRequest
//

void ScriptExecutor::ExpectDevUnconfirmedRequest( const BACnetAPDU &apdu )
{
   // get the service choice
   BACnetEnumerated  bacnetServiceData(apdu.apduService, NetworkSniffer::BAC_STRTAB_BACnetUnconfirmedServiceChoice);
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALUnconfirmedServiceMap, "Unconfirmed Request Service mismatch: " );

   // expect the rest
   BACnetAPDUDecoder dec( apdu );
   ExpectALData( dec );
   ExpectNoMore(dec);
}

//
// ScriptExecutor::ExpectDevSimpleACK
//

void ScriptExecutor::ExpectDevSimpleACK( const BACnetAPDU &apdu )
{
   // Check the service choice
   BACnetEnumerated  bacnetServiceData(apdu.apduService, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice);
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALConfirmedServiceMap, "Simple Ack Service mismatch: " );

   // Check the invokeID
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(apdu.apduInvokeID), NULL, "Simple Ack InvokeID mismatch: " );
}

//
// ScriptExecutor::ExpectDevComplexACK
//

void ScriptExecutor::ExpectDevComplexACK( const BACnetAPDU &apdu )
{
   // Check the service choice
   BACnetEnumerated  bacnetServiceData(apdu.apduService, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice);
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALConfirmedServiceMap, "Complex Ack Service mismatch: " );

   // Check the invokeID
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(apdu.apduInvokeID), NULL, "Complex Ack InvokeID mismatch: " );

   // expect the rest
   BACnetAPDUDecoder dec( apdu );
   ExpectALData( dec );
   ExpectNoMore(dec);
}

//
// ScriptExecutor::ExpectDevSegmentACK
//

void ScriptExecutor::ExpectDevSegmentACK( const BACnetAPDU &apdu )
{
   MatchBoolExpression( kwNEGATIVEACK, BACnetBoolean(apdu.apduNak), "Segment Ack Negative Ack mismatch: " );

   MatchBoolExpression( kwSERVER, BACnetBoolean(apdu.apduSrv), "Segment Ack Server mismatch: " );

   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(apdu.apduInvokeID), NULL, "Segment Ack InvokeID mismatch: " );

   MatchEnumExpression( kwSEQUENCENR, BACnetEnumerated(apdu.apduSeq), NULL, "Segment Ack Sequence mismatch: " );

   MatchEnumExpression( kwWINDOWSIZE, BACnetEnumerated(apdu.apduWin), NULL, "Segment Ack Windowsize mismatch: " );
}

//
// ScriptExecutor::ExpectDevError
//

void ScriptExecutor::ExpectDevError( const BACnetAPDU &apdu )
{
   // Check the service choice
   BACnetEnumerated  bacnetServiceData(apdu.apduService, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice);
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALConfirmedServiceMap, "Error Service mismatch: " );

   // Check the invokeID
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(apdu.apduInvokeID), NULL, "Error InvokeID mismatch: " );

   // expect the rest
   BACnetAPDUDecoder dec( apdu );
   ExpectALData( dec );
   ExpectNoMore(dec);
}

//
// ScriptExecutor::ExpectDevReject
//

void ScriptExecutor::ExpectDevReject( const BACnetAPDU &apdu )
{
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(apdu.apduInvokeID), NULL, "Reject InvokeID mismatch: " );

   BACnetEnumerated  bacnetRejectData(apdu.apduAbortRejectReason, NetworkSniffer::BAC_STRTAB_BACnetReject);
   MatchEnumExpression( kwREJECTREASON, bacnetRejectData, ScriptALRejectReasonMap, "Reject Reason mismatch: " );
}

//
// ScriptExecutor::ExpectDevAbort
//

void ScriptExecutor::ExpectDevAbort( const BACnetAPDU &apdu )
{
   // Check the service choice
   MatchBoolExpression( kwSERVER, BACnetBoolean(apdu.apduSrv), "Abort Server mismatch: " );

   // Check the invokeID
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(apdu.apduInvokeID), NULL, "Abort InvokeID mismatch: " );

   // get the service choice
   BACnetEnumerated  bacnetAbortData(apdu.apduAbortRejectReason, NetworkSniffer::BAC_STRTAB_BACnetAbort);
   MatchEnumExpression( kwABORTREASON, bacnetAbortData, ScriptALAbortReasonMap, "Abort Reason mismatch: " );
}



//
// ScriptExecutor::ExpectConfirmedRequest
//

void ScriptExecutor::ExpectConfirmedRequest( BACnetAPDUDecoder &dec )
{
   int                  header, maxSegs, maxResp, valu;
   ScriptPacketExprPtr  pMaxSegs;
   BACnetInteger        alMaxSegs;
   ScriptPacketExprPtr  pMaxResp;
   BACnetInteger        alMaxResp;
   ScriptParmPtr        pScriptParm = NULL;        // used for variable assignment

   // These tables should match the values used by MaxSegsDecode and MaxAPDUDecode.
   // Duplicated here so we can use BACnetEnumerated
   const char * apMaxSegs[]  = {"0", "2", "4", "8", "16", "32", "64", "999"};
   const char * apMaxSizes[] = {"50", "128", "206", "480", "1024", "1476"};

   // check the header
   header = (dec.pktLength--,*dec.pktBuffer++);
   if ((header >> 4) != 0)
      throw "Confirmed request expected";

   // check segmented message
   MatchBoolExpression( kwSEGMSG, BACnetBoolean(header & 0x08), "Segmented message mismatch: " );
   MatchBoolExpression( kwSEGMENTEDMESSAGE, BACnetBoolean(header & 0x08), "Segmented message mismatch: " );

   // check more follows
   MatchBoolExpression( kwMOREFOLLOWS, BACnetBoolean(header & 0x04), "More-follows mismatch: " );

   // check segmented response
   MatchBoolExpression( kwSEGRESP, BACnetBoolean(header & 0x02), "Segmented response accepted mismatch: " );
   MatchBoolExpression( kwSEGRESPACCEPTED, BACnetBoolean(header & 0x02), "Segmented response accepted mismatch: " );

   // extract max-segs and max-response
   valu = (dec.pktLength--,*dec.pktBuffer++);
   maxSegs = (valu >> 4) & 0x07;
   maxResp = valu & 0x0F;

   // Max-segs
   pMaxSegs = GetKeywordValue( &pScriptParm, kwMAXSEGS, alMaxSegs );
   if (pMaxSegs)
   {
      // For max-resp, the raw values (50 to 1476) don't overlap the encoded values (0 to 15)
      // so the code can accept either.
      // But for max-segs, the raw values (2, 4, 8...) DO overlap the encoded values (0 to 7)
      // We assume a raw value
      alMaxSegs.intValue = MaxSegsEncode( alMaxSegs.intValue );

      // assign or check the value.  Comparison for "more than 64" is problematic.
      // Our table has 999, which is pretty much a random "big" value, but it will appear in
      // any error message to help the test writer.
      BACnetEnumerated  bacnetSegsData(maxSegs, apMaxSegs, 8);
      BACnetEnumerated  bacnetSegsScript(alMaxResp.intValue, apMaxSegs, 8);
      TestOrAssign(pMaxSegs, bacnetSegsData, bacnetSegsScript, pScriptParm, "Max segments accepted mismatch: " );
   }

   // lots of options for maximum response size
   pMaxResp = GetKeywordValue( &pScriptParm, kwMAXRESP, alMaxResp );
   if (!pMaxResp)
      pMaxResp = GetKeywordValue( &pScriptParm, kwMAXRESPONSE, alMaxResp );
   if (!pMaxResp)
      pMaxResp = GetKeywordValue( &pScriptParm, kwMAXSIZE, alMaxResp );
   if (pMaxResp)
   {
      // translate the max response size into the encoded value.
      // Values less than 16 are encoded directly as the bit-field
      if (alMaxResp.intValue >= 16)
      {
         alMaxResp.intValue = MaxAPDUEncode( alMaxResp.intValue );
      }

      // if we're making an assignment, encode alMax with enumeration of reserved int... then stuff
      BACnetEnumerated  bacnetSizeData(maxResp, maxResp > 5 ? NULL : apMaxSizes, maxResp > 5 ? 0 : 6);
      BACnetEnumerated  bacnetSizeScript(alMaxResp.intValue, alMaxResp.intValue > 5 ? NULL : apMaxSizes, alMaxResp.intValue > 5 ? 0 : 6);

      // now check it or assign it
      TestOrAssign(pMaxResp, bacnetSizeData, bacnetSizeScript, pScriptParm, "Max response size mismatch: " );
   }

   // extract the invoke ID
   valu = (dec.pktLength--,*dec.pktBuffer++);
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(valu), NULL, "InvokeID mismatch: " );

   // check sequence number if it is specified and is found in data
   TestOrAssignOptionValue( kwSEQUENCENR, "Sequence number mismatch: ", (header & 0x80) != 0 ? &dec : NULL );

   // now check window size if provided
   TestOrAssignOptionValue( kwWINDOWSIZE, "Window size mismatch: ", (header & 0x80) != 0 ? &dec : NULL );

   // extract the service choice
   valu = (dec.pktLength--,*dec.pktBuffer++);
   BACnetEnumerated  bacnetServiceData(valu, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice);
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALConfirmedServiceMap, "Service mismatch: " );

   // expect the rest
   ExpectALData( dec );
}



void ScriptExecutor::TestOrAssignOptionValue( int kw, const char * pszErrorPrefix, BACnetAPDUDecoder * pdec /* = NULL */ )
{
   int                  valu;
   ScriptPacketExprPtr     pExpr;
   BACnetInteger        bacnetInt;
   ScriptParmPtr        pScriptParm = NULL;        // used for variable assignment

   // check number if it is specified and is found in data
   pExpr = GetKeywordValue( &pScriptParm, kw, bacnetInt );

   // check segmented message stuff
   if ( pdec != NULL )
   {
      // extract the sequence number and check it
      valu = (pdec->pktLength--,*(pdec->pktBuffer++));
      TestOrAssign(pExpr, BACnetInteger(valu), bacnetInt, pScriptParm, pszErrorPrefix );
   }
   else if (pExpr)
   {
      CString str(pszErrorPrefix);

      if ( pScriptParm != NULL )
         str.Format("Assignment to %s illegal, received message not segmented", pScriptParm->parmName );
      else
         str += "Message not segmented";

      throw CString(str);
   }
}

//
// ScriptExecutor::ExpectUnconfirmedRequest
//

void ScriptExecutor::ExpectUnconfirmedRequest( BACnetAPDUDecoder &dec )
{
   int                  valu;

   // check the header
   valu = (dec.pktLength--,*dec.pktBuffer++);
   if ((valu >> 4) != 1)
      throw "UnconfirmedRequest expected";

   // extract the service choice
   valu = (dec.pktLength--,*dec.pktBuffer++);
   BACnetEnumerated  bacnetServiceData(valu, NetworkSniffer::BAC_STRTAB_BACnetUnconfirmedServiceChoice);
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALUnconfirmedServiceMap, "Service mismatch: " );

   // expect the rest
   ExpectALData( dec );
}

//
// ScriptExecutor::ExpectSimpleACK
//

void ScriptExecutor::ExpectSimpleACK( BACnetAPDUDecoder &dec )
{
   int                  valu;

   // check the header
   valu = (dec.pktLength--,*dec.pktBuffer++);
   if ((valu >> 4) != 2)
      throw "SimpleACK expected";

   // extract the invoke ID
   valu = (dec.pktLength--,*dec.pktBuffer++);
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(valu), NULL, "InvokeID mismatch: " );

   // extract the service choice
   valu = (dec.pktLength--,*dec.pktBuffer++);
   BACnetEnumerated  bacnetServiceData(valu, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice);
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALConfirmedServiceMap, "Service mismatch: " );
}

//
// ScriptExecutor::ExpectComplexACK
//

void ScriptExecutor::ExpectComplexACK( BACnetAPDUDecoder &dec )
{
   int                  header, valu;

   // check the header
   header = (dec.pktLength--,*dec.pktBuffer++);
   if ((header >> 4) != 3)
      throw "ComplexACK expected";

   // check segmented message
   MatchBoolExpression( kwSEGMSG, BACnetBoolean(header & 0x08), "Segmented message mismatch: " );
   MatchBoolExpression( kwSEGMENTEDMESSAGE, BACnetBoolean(header & 0x08), "Segmented message mismatch: " );

   // check more follows
   MatchBoolExpression( kwMOREFOLLOWS, BACnetBoolean(header & 0x04), "More-follows mismatch: " );

   // extract the invoke ID
   valu = (dec.pktLength--,*dec.pktBuffer++);
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(valu), NULL, "InvokeID mismatch: " );

   // check sequence number if it is specified and is found in data
   TestOrAssignOptionValue( kwSEQUENCENR, "Sequence number mismatch: ", (header & 0x08) != 0 ? &dec : NULL );

   // now check window size if provided
   TestOrAssignOptionValue( kwWINDOWSIZE, "Window size mismatch: ", (header & 0x08) != 0 ? &dec : NULL );

   // extract the service choice
   valu = (dec.pktLength--,*dec.pktBuffer++);
   BACnetEnumerated  bacnetServiceData(valu, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice);
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALConfirmedServiceMap, "Service mismatch: " );

   // expect the rest
   ExpectALData( dec );
}

//
// ScriptExecutor::ExpectSegmentACK
//

void ScriptExecutor::ExpectSegmentACK( BACnetAPDUDecoder &dec )
{
   int                  valu;

   // check the header
   valu = (dec.pktLength--,*dec.pktBuffer++);
   if ((valu >> 4) != 4)
      throw "SegmentACK expected";

   // check negative ACK
   MatchBoolExpression( kwNEGATIVEACK, BACnetBoolean(valu & 0x02), "Negative Ack mismatch: " );

   // check the server bit
   MatchBoolExpression( kwSERVER, BACnetBoolean(valu & 0x01), "Server mismatch: " );

   // extract the invoke ID
   valu = (dec.pktLength--,*dec.pktBuffer++);
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(valu), NULL, "InvokeID mismatch: " );

   // extract the service choice
   valu = (dec.pktLength--,*dec.pktBuffer++);
   BACnetEnumerated  bacnetServiceData(valu, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice);
   MatchEnumExpression( kwSERVICE, bacnetServiceData, ScriptALConfirmedServiceMap, "Service mismatch: " );

   // extract the sequence number
   valu = (dec.pktLength--,*dec.pktBuffer++);
   MatchEnumExpression( kwSEQUENCENR, BACnetEnumerated(valu), NULL, "Sequence number mismatch: " );

   // extract the actual window size
   valu = (dec.pktLength--,*dec.pktBuffer++);
   MatchEnumExpression( kwWINDOWSIZE, BACnetEnumerated(valu), NULL, "Window size mismatch: " );
}

//
// ScriptExecutor::ExpectError
//
//Added by Liangping Xu, 2002-8-5********//

void ScriptExecutor::ExpectError( BACnetAPDUDecoder &dec )
{
    int                  valu;

   // check the header
   valu = (dec.pktLength--,*dec.pktBuffer++);
   if ((valu >> 4) != 5)
      throw "Error expected";

   // extract the invoke ID
   valu = (dec.pktLength--,*dec.pktBuffer++);
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(valu), NULL, "InvokeID mismatch: " );

   // extract the Error Choice
   valu = (dec.pktLength--,*dec.pktBuffer++);

   // get the Error Choice
   BACnetEnumerated  bacnetErrorServiceData(valu, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice);
   MatchEnumExpression( kwERRORCHOICE, bacnetErrorServiceData, ScriptALConfirmedServiceMap, "Error Service mismatch: " );
// pErrorService = execPacket->packetExprList.Find( kwERRORCHOICE );
// errorService=pErrorService->exprValue;
// if(errorService.CompareNoCase(CString(NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice.m_pStrings[valu])))
//        throw ExecError( "Error Service mismatch", pErrorService->exprLine );

   //extract AL data,including the Error Class & Code Nmu
   ExpectALData( dec );
}


//
// ScriptExecutor::ExpectReject
//

void ScriptExecutor::ExpectReject( BACnetAPDUDecoder &dec )
{
   int                  valu;

   // check the header
   valu = (dec.pktLength--,*dec.pktBuffer++);
   if ((valu >> 4) != 6)
      throw "Reject expected";

   // extract the invoke ID
   valu = (dec.pktLength--,*dec.pktBuffer++);
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(valu), NULL, "InvokeID mismatch: " );

   // extract the reject reason
   valu = (dec.pktLength--,*dec.pktBuffer++);
   BACnetEnumerated  bacnetRejectData(valu, NetworkSniffer::BAC_STRTAB_BACnetReject);
   MatchEnumExpression( kwREJECTREASON, bacnetRejectData, ScriptALRejectReasonMap, "Reject Reason mismatch: " );
}

//
// ScriptExecutor::ExpectAbort
//

void ScriptExecutor::ExpectAbort( BACnetAPDUDecoder &dec )
{
   int                  valu;

   // check the header
   valu = (dec.pktLength--,*dec.pktBuffer++);
   if ((valu >> 4) != 7)
      throw "Abort expected";

   // check the server bit
   MatchBoolExpression( kwSERVER, BACnetBoolean(valu & 0x01), "Server mismatch: " );

   // extract the invoke ID
   valu = (dec.pktLength--,*dec.pktBuffer++);
   MatchEnumExpression( kwINVOKEID, BACnetEnumerated(valu), NULL, "InvokeID mismatch: " );

   // extract the abort reason
   valu = (dec.pktLength--,*dec.pktBuffer++);
   BACnetEnumerated  bacnetAbortData(valu, NetworkSniffer::BAC_STRTAB_BACnetAbort);
   MatchEnumExpression( kwABORTREASON, bacnetAbortData, ScriptALAbortReasonMap, "Abort Reason mismatch: " );
}

//
// ScriptExecutor::ExpectALData
//
// There are two flavors of application layer variable encoding stuff.  One is with a keyword 
// on the front of the type with a bunch of parameters, the other is ALDATA followed by a 
// hex string.
//

// madanner 9/25/02 Method drastically altered
// see prior versions for comparision (sorry)

void ScriptExecutor::ExpectALData( BACnetAPDUDecoder &dec )
{
   int                  indx, len;
   BACnetOctetString    ostr;
   CString strThrowMessage;

   // get the index of the first data
   indx = execPacket->packetExprList.FirstData();
   if (indx < 0)
      return;

   // get the length
   len = execPacket->packetExprList.Length();

   // loop through each of the packet expressions in the packet expression list

   for (; indx < len; indx++) {
      ScriptPacketExprPtr spep = execPacket->packetExprList.Child( indx );
      if (!spep)
         throw "Application variable element expected";

      // check for raw data
      if ((spep->exprKeyword == kwAL) || (spep->exprKeyword == kwALDATA)) {
         // translate the expression, resolve parameter names into values
         ScriptTokenList tlist;
         ResolveExpr( spep->exprValue, spep->exprLine, tlist );

         // just a little error checking
         if (tlist.Length() != 1)
            throw "ALDATA requires an octet string";

         // reference or data?
         if (tlist[0].tokenType == scriptReference)
         {
            BACnetAnyValue    bacnetEPICSProperty;

            GetEPICSProperty( tlist[0].tokenSymbol, &bacnetEPICSProperty, tlist[0].m_nIndex);

            // Will throw errors
            bacnetEPICSProperty.CompareToEncodedStream( dec, spep->exprOp, spep->exprValue );
         }
         else
         {
            // extract octet string
            if (!tlist[0].IsEncodeable( ostr ))
               throw "Octet string expected";

            ASSERT(0);     // not implemented...  
            //CompareStreamData(dec, spep->exprOp, ostr.strBuff, ostr.strLen, spep->exprValue ); 
         }

         continue;
      }

      // skip things that aren't data
      if (!spep->exprIsData)
         continue;

      // do the type specific encoding
      try {
         int rslt = ScriptToken::Lookup( spep->exprKeyword, ScriptALMap );
         if (rslt < 0)
            throw "Invalid keyword";

         switch (rslt) {
            case 1:                       // NULL
               ExpectALNull( spep, dec );
               break;
            case 2:                       // BOOLEAN
               ExpectALBoolean( spep, dec );
               break;
            case 3:                       // UNSIGNED
               ExpectALUnsigned( spep, dec );
               break;
            case 4:                       // INTEGER
               ExpectALInteger( spep, dec );
               break;
            case 5:                       // REAL
               ExpectALReal( spep, dec );
               break;
            case 6:                       // DOUBLE
               ExpectALDouble( spep, dec );
               break;
            case 7:                       // OCTETSTRING
               ExpectALOctetString( spep, dec );
               break;
            case 8:                       // CHARACTERSTRING
               ExpectALCharacterString( spep, dec );
               break;
            case 9:                       // BITSTRING
               ExpectALBitString( spep, dec );
               break;
            case 10:                   // ENUMERATED
               ExpectALEnumerated( spep, dec );
               break;
            case 11:                   // DATE
               ExpectALDate( spep, dec );
               break;
            case 12:                   // TIME
               ExpectALTime( spep, dec );
               break;
            case 13:                   // OBJECTIDENTIFIER
               ExpectALObjectIdentifier( spep, dec );
               break;
            case 14:                   // DEVICEIDENTIFIER
               ExpectALDeviceIdentifier( spep, dec );
               break;
            case 15:                   // PROPERTYIDENTIFIER
               ExpectALPropertyIdentifier( spep, dec );
               break;
            case 16:                   // OPENINGTAG
               ExpectALOpeningTag( spep, dec );
               break;
            case 17:                   // CLOSINGTAG
               ExpectALClosingTag( spep, dec );
               break;
            case 18:                   // ANY
               ExpectALAny( spep, dec );
               break;
            default:
               throw "Invalid keyword";
               break;
         }
      }
      catch (const char *errMsg)
      {
         throw ExecError( errMsg, spep->exprLine );
      }
      catch (const CString &errMsg)
      {
         throw ExecError( errMsg, spep->exprLine );
      }
   }

   // make sure all the data was matched
   if (dec.pktLength != 0)
      throw ExecError( "Additional application data not matched", execPacket->baseLineStart );
}


//
// ScriptExecutor::ExpectALNull
//

void ScriptExecutor::ExpectALNull( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext;
   BACnetNull        nullData;
   ScriptTokenList      tlist;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   indx = CheckExpressionParams( 0, 1, tlist.Length(), "Null");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), nullAppTag, "Null");

   nullData.Decode( dec );
}


//
// ScriptExecutor::ExpectALBoolean
//

void ScriptExecutor::ExpectALBoolean( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext;
   BACnetBoolean     boolData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr     pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Boolean");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), booleanAppTag, "Boolean");

   // tag is optional, now point to beginning of data token
   indx++;

   boolData.Decode( dec );

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      // verify the type
      if (bacnetEPICSProperty.GetType() != ebool)
         throw "Boolean property value expected in EPICS";

      scriptData.boolValue = ((BACnetBoolean *) bacnetEPICSProperty.GetObject())->boolValue;
   }
   else if ( spep->IsAssignment() )       // madanner 11/5/02, added >> operator support
      StuffScriptParameter(boolData, pScriptParm, spep->exprValue);
   else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
      return;
   else if (!tlist[indx].IsEncodeable( scriptData ))
      throw "Boolean value expected";

   CompareAndThrowError(boolData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILBOOL);
}


void ScriptExecutor::StuffScriptParameter(BACnetEncodeable &rbacnet, ScriptParmPtr pp, LPCSTR lpstrValue )
{
// ASSERT(pp != NULL);

   if (pp == NULL)
   {
      CString str;
      str.Format(IDS_SCREX_ASGN_UNDEFVAR, lpstrValue);
      throw CString(str);
   }

   // assign extracted value to parm
   rbacnet.Encode(pp->parmValue, BACnetEncodeable::FMT_SCRIPT);

   // Make the special call from the executor thread...  This posts the update, not sends.
   execDoc->m_pParmList->UpdateParameterVisual(pp);
}

//
// ScriptExecutor::ExpectALUnsigned
//

void ScriptExecutor::ExpectALUnsigned( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext;
   BACnetUnsigned    uintData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr     pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Unsigned");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), unsignedIntAppTag, "Unsigned");

   // tag is optional, now point to beginning of data token
   indx++;

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      // verify the type
      if ( !bacnetEPICSProperty.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetUnsigned)) )
         throw "Unsigned property value expected in EPICS";

      scriptData.uintValue = ((BACnetUnsigned *) bacnetEPICSProperty.GetObject())->uintValue;
   }
   else if ( spep->IsAssignment() )
      ;  // don't do anything just yet... wait until data is extracted
   else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
      spep->exprOp = '?=';
   else if (!tlist[indx].IsEncodeable( scriptData ))
      throw "Unsigned value expected";

   // There may be a bug here... possibly we should always Decode the uintData regardless of
   // what the scriptData indicates as size.  But I don't know enough yet to tell what this would
   // break?  madanner 11/4/02

   // check for wierd versions
   if (spep->exprKeyword == kwUNSIGNED8) {
//    if ((uintData.uintValue < 0) || (uintData.uintValue > 255))       // madanner 11/4/02, should be scriptData?
      if ((scriptData.uintValue < 0) || (scriptData.uintValue > 255))
         throw "Unsigned8 value out of range (0..255)";
      if (tlist.Length() == 2)
         throw "Unsigned8 keyword cannot be context encoded";

      uintData.uintValue = (dec.pktLength--,*dec.pktBuffer++);
   } else
   if (spep->exprKeyword == kwUNSIGNED16) {
//    if ((uintData.uintValue < 0) || (uintData.uintValue > 65535))     // madanner 11/4/02, should be scriptData?
      if ((scriptData.uintValue < 0) || (scriptData.uintValue > 65535))
         throw "Unsigned16 value out of range (0..65535)";
      if (tlist.Length() == 2)
         throw "Unsigned16 keyword cannot be context encoded";

      uintData.uintValue = (dec.pktLength--,*dec.pktBuffer++);
      uintData.uintValue = (uintData.uintValue << 8) + (dec.pktLength--,*dec.pktBuffer++);
   } else
   if (spep->exprKeyword == kwUNSIGNED32) {
      if (tlist.Length() == 2)
         throw "Unsigned32 keyword cannot be context encoded";

      uintData.uintValue = (dec.pktLength--,*dec.pktBuffer++);
      uintData.uintValue = (uintData.uintValue << 8) + (dec.pktLength--,*dec.pktBuffer++);
      uintData.uintValue = (uintData.uintValue << 8) + (dec.pktLength--,*dec.pktBuffer++);
      uintData.uintValue = (uintData.uintValue << 8) + (dec.pktLength--,*dec.pktBuffer++);
   } else {
      // decode it
      uintData.Decode( dec );
   }

   CompareAndThrowError(uintData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILUNSIGNED);

   // now suck out the data and slap it into the parm if needed
   if ( spep->IsAssignment() )
      StuffScriptParameter(uintData, pScriptParm, spep->exprValue);
}

//
// ScriptExecutor::ExpectALInteger
//

void ScriptExecutor::ExpectALInteger( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext;
   BACnetInteger     intData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr     pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Integer");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), integerAppTag, "Integer");

   // tag is optional, now point to beginning of data token
   indx++;

   intData.Decode( dec );

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      // verify the type
      if ( bacnetEPICSProperty.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetUnsigned)) )
         scriptData.intValue = ((BACnetUnsigned *) bacnetEPICSProperty.GetObject())->uintValue;
      else if ( bacnetEPICSProperty.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetInteger)) )
         scriptData.intValue = ((BACnetInteger *) bacnetEPICSProperty.GetObject())->intValue;
      else
         throw "Integer property value expected in EPICS";
   }
   else if ( spep->IsAssignment() )
      StuffScriptParameter(intData, pScriptParm, spep->exprValue);
   else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
      return;
   else if (!tlist[indx].IsEncodeable( scriptData ))
      throw "Integer value expected";

   CompareAndThrowError(intData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILINT);
}

//
// ScriptExecutor::ExpectALReal
//

void ScriptExecutor::ExpectALReal( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext;
   BACnetReal        realData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr     pScriptParm = NULL;
   
   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Real");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), realAppTag, "Real");

   // tag is optional, now point to beginning of data token
   indx++;

   realData.Decode( dec );

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      if ( !bacnetEPICSProperty.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetReal)) )
         throw "Real property value expected in EPICS";

      scriptData.realValue = ((BACnetReal *) bacnetEPICSProperty.GetObject())->realValue;
   }
   else if ( spep->IsAssignment() )
      StuffScriptParameter(realData, pScriptParm, spep->exprValue);
   else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
      return;
   else if (!tlist[indx].IsEncodeable( scriptData ))
      throw "Single precision floating point value expected";

   CompareAndThrowError(realData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILREAL);
}



// check the number of supplied parameters and throw an error.
// Also return the index into the parm list that is the tag... -1 for not supplied.

int ScriptExecutor::CheckExpressionParams( int nMinParms, int nMaxParms, int nSuppliedParms, const char * pszDataType )
{
   int               indx = 0;
   CString           strError;
   
   // If they've supplied more than the max parameters... let them know about it.

   if ( nSuppliedParms > nMaxParms &&  nMaxParms != -1 )    // handle no max case
   {
      strError.Format("%s keyword requires max of %d parameter%s.  %d supplied", pszDataType, nMaxParms, nMaxParms > 1 ? "s" : "", nSuppliedParms );
      throw CString(strError);
   }

   // if they've supplied less than the minimum number of parsm... well... you get the idea.

   if ( nSuppliedParms < nMinParms )
   {
      strError.Format("%s keyword requires at least %d parameter%s.  %d supplied", pszDataType, nMinParms, nMinParms > 1 ? "s" : "", nSuppliedParms );
      throw CString(strError);
   }

   // return the index for the expected tag in the token list...  If the tag is supplied in the parms... it's
   // always at zero.  Return -1 if it's not supplied.  This will be true if they've supplied the minimum number of parms.

   return nSuppliedParms - nMaxParms;
}



// Test for the proper tag and throw error if not.  The number of parms have already been supplied
// so we only need to pass the actual token of the supplied tag, if present.

void ScriptExecutor::ExpectALTag( BACnetAPDUDecoder &dec, ScriptToken * ptok, BACnetApplicationTag tagNumber, const char * pszDataType )
{
   int               context = kAppContext;
   BACnetAPDUTag     tag;
   CString           strError;
   
   if (dec.pktLength < 1)
   {
      // No data.  Avoid wild throw in Peek
      strError.Format("Out of data.  Expected %s", pszDataType);
   }
   else
   {
      // extract the tag
      tag.Peek( dec );

      // tag is optional... but at least check the type of stream data if they've supplied a don't care context
      if ( ptok == NULL  ||  (ptok != NULL && ptok->IsDontCare()) )
      {
         if (tag.tagClass)
            strError.Format("Application tagged %s expected", pszDataType );
         else if (tag.tagNumber != tagNumber)
            strError.Format("Mismatched data type, %s expected", pszDataType);
      }
      else
      {
         if ( !ptok->IsInteger(context) )
            strError.Format("Tag number expected but (%s) supplied as tag parameter", ptok->tokenValue);
         else if ( !tag.tagClass )
            strError.Format("Context tagged %s expected", pszDataType);
         else if ( tag.tagNumber != context )
            strError.Format("Mismatched context tag value.  Expected [%d]", context);
      }
   }

   if ( !strError.IsEmpty() )
   {
      CString str;
      tag.Encode(str, BACnetEncodeable::FMT_PLAIN);
      strError += ".  Got ";
      strError += str;
      throw CString(strError);
   }
}


//
// ScriptExecutor::ExpectALDouble
//

void ScriptExecutor::ExpectALDouble( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext;
   BACnetDouble      doubleData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr     pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Double");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), doubleAppTag, "Double");

   // tag is optional, now point to beginning of data token
   indx++;

   doubleData.Decode( dec );

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      if ( !bacnetEPICSProperty.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetReal)) )
         throw "Floating point property value expected in EPICS";

      scriptData.doubleValue = ((BACnetReal *) bacnetEPICSProperty.GetObject())->realValue;
   }
   else if ( spep->IsAssignment() )
      StuffScriptParameter(doubleData, pScriptParm, spep->exprValue);
   else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
      return;
   else if (!tlist[indx].IsEncodeable( scriptData ))
      throw "Double precision floating point expected";

   CompareAndThrowError(doubleData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILDOUBLE);
}

//
// ScriptExecutor::ExpectALOctetString
//

void ScriptExecutor::ExpectALOctetString( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext;
   BACnetOctetString ostrData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr     pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Octet string");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), octetStringAppTag, "Octet string");

   // tag is optional, now point to beginning of data token
   indx++;

   ostrData.Decode( dec );

   // no references
   if (tlist[indx].tokenType == scriptReference)
      throw "Octet string property references not supported";
   else if ( spep->IsAssignment() )
      StuffScriptParameter(ostrData, pScriptParm, spep->exprValue);
   else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
      return;
   else if (!tlist[indx].IsEncodeable( scriptData ))
      throw "Octet string value expected";

   CompareAndThrowError(ostrData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILOCTSTRING);
}

//
// ScriptExecutor::ExpectALCharacterString
//
// See SendALCharacterString for additional comments.
//

void ScriptExecutor::ExpectALCharacterString( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int                  indx, context = kAppContext;
   BACnetCharacterString   cstrData, scriptData;
   ScriptTokenList         tlist;
   ScriptParmPtr        pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 3, tlist.Length(), "Character string");

   switch ( tlist.Length() )
   {
      case 1:
            // tag is optional.  If there is only one token then we only need to check to see
            // if the stream's type is char.

            ExpectALTag( dec, NULL, characterStringAppTag, "Character string");
            indx++;     // prep for increment to char data
            break;

      case 2:
            // two supplied parameters.  This could mean that the first one is either a tag number
            // or the encoding, because the last one should be the value.
            // Check to see if it's an integer... if so, it's the tag, otherwise, it's the encoding.

            if ( tlist[0].IsDontCare() ||  tlist[0].IsInteger(context) )
            {
               // assume they've supplied the context tag and NOT the character encoding...
               ExpectALTag( dec, &(tlist[0]), characterStringAppTag, "Character string");

               // prep index to point to context, which will increment later to point to char data
               indx = 0;
               break;
            }

            // OK.  Let's then assume they've tried to supply the char encoding... then
            // context check should be as char implied... test NULL context and then test encoding

            ExpectALTag( dec, NULL, characterStringAppTag, "Character string");
            indx = -1;
            // drop through

      case 3:
            if ( indx >= 0 )
               ExpectALTag( dec,  &(tlist[indx]), characterStringAppTag, "Character string");
            indx++;

            if ( ScriptToken::Lookup( tlist[indx].tokenSymbol, ScriptCharacterTypeMap ) < 0 )
               throw "Unknown encoding for character keyword";

            tlist[indx+1].tokenValue = tlist[indx].tokenValue + ", " + tlist[indx+1].tokenValue;
   }

   indx++;

   cstrData.Decode( dec );

      // see if a reference was used
   if ( tlist[indx].tokenType == scriptReference )
   {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      if ( !bacnetEPICSProperty.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetCharacterString)) )
         throw "Character string property value expected in EPICS";

      scriptData.SetValue( (char *) ((BACnetCharacterString *) bacnetEPICSProperty.GetObject())->strBuff );
   }
   else if ( spep->IsAssignment() )
      StuffScriptParameter(cstrData, pScriptParm, spep->exprValue);
   else if ( tlist[indx].IsDontCare() ||  spep->IsDontCare() )
      return;
   else if ( !tlist[indx].IsEncodeable( scriptData ) )
      throw "Unable to encode string value";


   CompareAndThrowError(cstrData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILSTRING);
}

//
// ScriptExecutor::ExpectALBitString
//
// Yet another custom extension, bitstrings can be specified as
// - in EPICS format as [{T,F,T}]
// - in "traditional" EPICS format as B'101'
// - in a legacy format found in a number of old scripts as unwrapped T,F,T
//   (which is currently NOT allowed when sending...)
//
void ScriptExecutor::ExpectALBitString( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext, bit;
   BACnetBitString   bstrData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr     pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   // Because of the legacy format, can't rely on CheckExpressionParams to point at the tag.
   // (else we could just specify the max as 2)
   CheckExpressionParams( 1, -1, tlist.Length(), "Bitstring");

   indx = 0;   // set next to real data...
   if (tlist.Length() == 1)
   {
      // check data type in stream
      ExpectALTag( dec, NULL, bitStringAppTag, "Bitstring");
   }
   else
   {
      // they MIGHT have supplied the context along with the many bit values... better test
      // the waters for integer before assuming it's the context.

      if ( tlist[indx].tokenEnc != scriptBinaryEnc  &&  tlist[indx].IsInteger(context) )
         ExpectALTag( dec, &(tlist[indx++]), bitStringAppTag, "Bitstring");
   }

   // must be here to pass all the tag checks to avoid throws in decode
   bstrData.Decode( dec );

   // Reference or data?
   if (tlist[indx].tokenType == scriptReference)
   {
      BACnetAnyValue    bacnetEPICSProperty;
      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      if ( bacnetEPICSProperty.GetType() != bits)
         throw "Bitstring property value expected in EPICS";

      scriptData = *((BACnetBitString *) bacnetEPICSProperty.GetObject());
   }
   else if ( spep->IsAssignment() )
   {
      StuffScriptParameter(bstrData, pScriptParm, spep->exprValue);
   }
   else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
   {
      return;
   }
   else if ( tlist[indx].tokenEnc == scriptBinaryEnc)
   {
      if ( !tlist[indx].IsEncodeable( scriptData ) )
         throw "Bitstring value expected";
   }
   else if (tlist[indx].tokenEnc == scriptComplex)
   {
      // Look for the "official" [{T,F,T}] format
      try {
         scriptData.Decode( tlist[indx].tokenValue );
      }
      catch (...) {
         throw "Bitstring value expected";
      }
   }
   else
   {
      // Historical format, unfortunately used by many scripts: just T,F,T or 1,0,1
      for ( int nBitIndex = 0; indx < tlist.Length(); indx++ )
      {
         if ( tlist[indx].IsInteger( bit, ScriptBooleanMap ) )
            scriptData.SetBit(nBitIndex++, bit);
         else
            throw "Bitstring value expected";
      }
   }

   CompareAndThrowError(bstrData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILBITSTRING);
}


void ScriptExecutor::CompareAndThrowError( BACnetEncodeable & rbacnet1, BACnetEncodeable & rbacnet2, int iOperator, unsigned int nError )
{
   CString strError;

   if ( !rbacnet1.Match(rbacnet2, iOperator, &strError) )
   {
      CString strErrorPrefix;
      strErrorPrefix.LoadString(nError);
      throw CString(strErrorPrefix + strError);
   }
}


//
// ScriptExecutor::ExpectALEnumerated
//

void ScriptExecutor::ExpectALEnumerated( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext;
   BACnetEnumerated  enumData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr     pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Enumerated");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), enumeratedAppTag, "Enumerated");

   // tag is optional, now point to beginning of data token
   indx++;

   enumData.Decode( dec );

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      if ( !bacnetEPICSProperty.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetEnumerated)) )
         throw "Enumerated property value expected in EPICS";

      scriptData.m_enumValue = ((BACnetEnumerated *) bacnetEPICSProperty.GetObject())->m_enumValue;
   }
   else
   {
      if ( spep->IsAssignment() )
         StuffScriptParameter(enumData, pScriptParm, spep->exprValue);
      else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
         return;
      else
         try {
            scriptData.Decode( tlist[indx].tokenValue );
         }
         catch (...) {
            throw "Enumerated value expected";
         }
   }

   CompareAndThrowError(enumData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILENUM);
}

//
// ScriptExecutor::ExpectALDate
//

void ScriptExecutor::ExpectALDate( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int            indx, context = kAppContext;
   BACnetDate     dateData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr  pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Date");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), dateAppTag, "Date");

   // tag is optional, now point to beginning of data token
   indx++;

   dateData.Decode( dec );

   // reference or date data?
   if ( tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      if ( bacnetEPICSProperty.GetType() != ptDate )
         throw "Date property value expected in EPICS";

      scriptData = *((BACnetDate *) bacnetEPICSProperty.GetObject());
   }
   else
   {
      if ( spep->IsAssignment() )
         StuffScriptParameter(dateData, pScriptParm, spep->exprValue);
      else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
         return;
      else
         try {
            //tlist[indx].tokenType == scriptValue && tlist[indx].tokenEnc == scriptComplex )
            scriptData.Decode( tlist[indx].tokenValue );
         }
         catch (...) {
            CString str;
            str.Format(IDS_SCREX_BADDATADATE, tlist[indx].tokenValue );
            throw CString(str);
         }
   }

   CompareAndThrowError(dateData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILDATE);
}

//
// ScriptExecutor::ExpectALTime
//

void ScriptExecutor::ExpectALTime( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int            indx, context = kAppContext;
   BACnetTime     timeData, scriptData;
   ScriptTokenList   tlist;
   ScriptParmPtr  pScriptParm = NULL;
   
   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Time");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), timeAppTag, "Time");

   // tag is optional, now point to beginning of data token
   indx++;

   timeData.Decode( dec );

   // reference or date data?
   if ( tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      if ( bacnetEPICSProperty.GetType() != ptTime )
         throw "Time property value expected in EPICS";

      scriptData = *((BACnetTime *) bacnetEPICSProperty.GetObject());
   }
   else
   {
      if ( spep->IsAssignment() )
         StuffScriptParameter(timeData, pScriptParm, spep->exprValue);
      else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
         return;
      else
         try {
            //tlist[indx].tokenType == scriptValue && tlist[indx].tokenEnc == scriptComplex )
            scriptData.Decode( tlist[indx].tokenValue );
         }
         catch (...) {
            CString str;
            str.Format(IDS_SCREX_BADDATATIME, tlist[indx].tokenValue );
            throw CString(str);
         }
   }

   CompareAndThrowError(timeData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILTIME);
}

//
// ScriptExecutor::ExpectALObjectIdentifier
//

void ScriptExecutor::ExpectALObjectIdentifier( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int                  indx, context = kAppContext, objType, instanceNum;
   BACnetObjectIdentifier  objData, scriptData;
   ScriptTokenList         tlist;
   BACnetAPDUTag        tag;
   ScriptParmPtr        pScriptParm = NULL;
   
   // extract the tag
   tag.Peek( dec );
   
   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 3, tlist.Length(), "Object Identifier");

   switch ( tlist.Length() )
   {
      case 2:  
            // two supplied parameters.  This could mean that the first one is either a tag number
            // or the object type.
            // Check to see if it's an integer... if so, it's the tag, otherwise, it's the encoding.
            // if it's a don't care... assume it's the tag

            if ( tlist[0].IsDontCare() ||  tlist[0].IsInteger(context) )
            {
               // assume they've supplied the context tag and NOT the character encoding...
               ExpectALTag( dec, &(tlist[0]), objectIdentifierAppTag, "Object Identifier");

               // prep index to point to context, which will increment later to point to char data
               indx = 1;
               break;
            }

            // Drop through and test the implied tag. Let's then assume they've tried to supply the other stuff... but if we stop here..
            // it had better be a var stuffer or EPICS reference.

      case 1:  
            // tag is optional.  If there is only one token then we only need to check to see
            // if the stream's type is char.

            ExpectALTag( dec, NULL, objectIdentifierAppTag, "Object Identifier");
            indx = 0;      // prep for increment to char data
            break;

      case 3:  
            // Three parms mean the tag and object type and instance..

            ExpectALTag( dec,  &(tlist[0]), objectIdentifierAppTag, "Object Identifier");
            indx = 1;
   }

   objData.Decode( dec );

   // reference or real data?
   if (tlist[indx].tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;

      GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

      if ( bacnetEPICSProperty.GetType() != ob_id )
         throw "Object identifier property value expected in EPICS";

      scriptData = *((BACnetObjectIdentifier *) bacnetEPICSProperty.GetObject());
   } else {
      if ( spep->IsAssignment() )
         StuffScriptParameter(objData, pScriptParm, spep->exprValue);
      else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
         return;
      else
      {
         if ( !tlist[indx++].IsInteger( objType, ScriptObjectTypeMap) )
            throw "Object identifier type expected";
         if ( tlist.Length() <= indx || !tlist[indx].IsInteger( instanceNum) )
            throw "Object identifier instance expected";

         scriptData.SetValue((BACnetObjectType) objType, instanceNum);
      }
   }
   
   CompareAndThrowError(objData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILOBJID);

   // store the context for property references that may appear later
   execObjID = objData.objID;
}

//
// ScriptExecutor::ExpectALDeviceIdentifier
//

void ScriptExecutor::ExpectALDeviceIdentifier( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int                  indx, context = kAppContext, instanceNum;
   BACnetObjectIdentifier  objData;
   ScriptTokenList         tlist;
   ScriptParmPtr        pScriptParm = NULL;
   
   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Device Identifier");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), objectIdentifierAppTag, "Device Identifier");

   // tag is optional, now point to beginning of data token
   indx++;

   // decode it
   objData.Decode( dec );

   if ( spep->IsAssignment() )
      StuffScriptParameter(objData, pScriptParm, spep->exprValue);
   else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
      return;
   else if (!tlist[indx].IsInteger( instanceNum ))
      throw "Device identifier instance value expected";

   CompareAndThrowError(objData, BACnetObjectIdentifier(8, instanceNum), spep->exprOp, IDS_SCREX_COMPFAILDEVID);
}

//
// ScriptExecutor::ExpectALPropertyIdentifier
//

void ScriptExecutor::ExpectALPropertyIdentifier( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               indx, context = kAppContext, valu;
   BACnetEnumerated  enumData(0, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier);
   ScriptTokenList      tlist;
   ScriptParmPtr     pScriptParm = NULL;

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   indx = CheckExpressionParams( 1, 2, tlist.Length(), "Property Identifier");
   ExpectALTag( dec, indx < 0 ? NULL : &(tlist[indx]), enumeratedAppTag, "Property Identifier");

   // tag is optional, now point to beginning of data token
   indx++;

   enumData.Decode( dec );

   if ( spep->IsAssignment() )
      StuffScriptParameter(enumData, pScriptParm, spep->exprValue);
   else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
      return;
   else if (!tlist[indx].IsInteger( valu, ScriptPropertyMap ))
      throw "Property name expected";

   CompareAndThrowError(enumData, BACnetEnumerated(valu, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier), spep->exprOp, IDS_SCREX_COMPFAILPROP);
}

//
// ScriptExecutor::ExpectALOpeningTag
//

void ScriptExecutor::ExpectALOpeningTag( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               context;
   BACnetOpeningTag  openTag;
   ScriptTokenList   tlist;
   BACnetAPDUTag     tag;

   // extract the tag
   tag.Peek( dec );

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      if (tag.tagNumber != context)
         throw "Mismatched context tag value";
   } else
      throw "Opening tag requires tag number";

   // decode it
   openTag.Decode( dec );
}

//
// ScriptExecutor::ExpectALClosingTag
//

void ScriptExecutor::ExpectALClosingTag( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   int               context;
   BACnetClosingTag  closeTag;
   ScriptTokenList   tlist;
   BACnetAPDUTag     tag;

   // extract the tag
   tag.Peek( dec );

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist );

   // tag is optional
   if (tlist.Length() == 1) {
      if (!tlist[0].IsInteger( context ))
         throw "Tag number expected";
      if (tag.tagNumber != context)
         throw "Mismatched context tag value";
   } else
      throw "Closing tag requires tag number";

   // decode it
   closeTag.Decode( dec );
}

//
// ScriptExecutor::ExpectALAny
//
// ANY tagged item until the next unmatched closing tag or the end of the APDU
//
void ScriptExecutor::ExpectALAny( ScriptPacketExprPtr spep, BACnetAPDUDecoder &dec )
{
   BACnetANY         anyData, scriptData;
   ScriptTokenList   tlist;
   BACnetAPDUTag     tag;
   ScriptParmPtr     pScriptParm = NULL;

   // extract the tag.  BACnetAPDUTag will throw a BACnetError if there is no data available
   try
   {
      tag.Peek( dec );
   }
   catch (BACnetError &)
   {
      // Scripting.html says about ANY "Decoding stops when it reaches the end
      // of the APDU, or an unmatched closing tag, whichever occurs first."
      // We interpret that to mean that the absence of tag or data is acceptable.
      // Just continue with dec.pktLength == 0
      TRACE( "Tag parse for ANY failed" );
   }

   // translate the expression, resolve parameter names into values
   ResolveExpr( spep->exprValue, spep->exprLine, tlist, spep->IsAssignment() ? &pScriptParm : NULL );

   // We have at most one parameter.  Tag is not allowed
   int indx = CheckExpressionParams( 0, 1, tlist.Length(), "Any type");

   // decode it
   anyData.Decode( dec );

   if (indx >= 0)
   {
      // reference or real data?
      if (tlist[indx].tokenType == scriptReference) {
         BACnetAnyValue    bacnetEPICSProperty;
         GetEPICSProperty( tlist[indx].tokenSymbol, &bacnetEPICSProperty, tlist[indx].m_nIndex);

         // Set our data from the EPICS
         scriptData.SetValue( *bacnetEPICSProperty.GetObject() );
      }
      else if ( spep->IsAssignment() )
         StuffScriptParameter(anyData, pScriptParm, spep->exprValue);
      else if ( tlist[indx].IsDontCare() || spep->IsDontCare() )
         return;
      else if (!tlist[indx].IsEncodeable( scriptData ))
         throw "'Any' value expected";

      CompareAndThrowError(anyData, scriptData, spep->exprOp, IDS_SCREX_COMPFAILANY);
   }
}

//
// ScriptExecutor::GetKeywordValue
//

ScriptPacketExprPtr ScriptExecutor::GetKeywordValue( ScriptParmPtr * ppScriptParm, int keyword, BACnetEncodeable &enc, ScriptTranslateTablePtr tp )
{
   ScriptPacketExprPtr  pep = 0;

   // find the keyword
   pep = execPacket->packetExprList.Find( keyword );
   if (!pep)
      return 0;

   ScriptTokenList      tlist;

   if ( ppScriptParm != NULL )         // just initialize for left overs
      *ppScriptParm = NULL;

      
   // translate the expression, resolve parameter names into values
   ResolveExpr( pep->exprValue, pep->exprLine, tlist, pep->IsAssignment() ? ppScriptParm : NULL );
   if (tlist.Length() < 1)
      throw ExecError( "Keyword value expected", pep->exprLine );

   if ( pep->IsAssignment() )
   {
      CString str;

      if ( ppScriptParm == NULL )
      {
         str.Format("Use of assignment operator requires script variable in expression (%s)", pep->exprValue);
         throw ExecError( str, pep->exprLine );
      }

      if ( *ppScriptParm == NULL )
      {
         str.Format("Script variable undefined (%s) for assignment operator", pep->exprValue);
         throw ExecError( str, pep->exprLine);
      }

      // Assignement is fine...return to caller and he'll deal with stuffing the parameter
      return pep;
   }

   if ( pep->IsDontCare() )      // they're using ?= operator... just return
      return pep;

   // get a reference to the first parameter
   const ScriptToken &t = tlist[0];

   // if it is a keyword, lookup the value
   // if it is a EPICS property reference
   if (t.tokenType == scriptReference) {
      BACnetAnyValue    bacnetEPICSProperty;
      switch(keyword) {
      case kwMAXRESP:
      case kwMAXRESPONSE:
      case kwMAXSIZE:
         GetEPICSDeviceProperty(t.tokenSymbol, &bacnetEPICSProperty);
         break;
      default:
         break;
      }
      if (bacnetEPICSProperty.GetType() == 0) {
         throw ExecError( "Invalid EPICS reference", pep->exprLine );
      }
      CString value;
      (bacnetEPICSProperty.GetObject())->Encode(value, BACnetEncodeable::FMT_SCRIPT);
      enc.Decode(value);
   }else if ((t.tokenType == scriptKeyword) && tp) {     
      // if it is a keyword, lookup the value
      int rslt = t.Lookup( t.tokenSymbol, tp );
      if (rslt < 0)
         throw ExecError( "Invalid keyword", pep->exprLine );

      //madanner 6/03, allow for possiblity of NONE type in keyword list...
      //we'll have to cheat.

      if ( tp != ScriptNONEMap )
      {
         char buff[16];
         sprintf( buff, "%d", rslt );

         try {
            enc.Decode( buff );
         }
         catch (...) {
            throw ExecError( "Keyword translation format error", pep->exprLine );
         }
      }
      else
         pep->SetNoneValue(true);
   }
   else if (!t.IsEncodeable( enc ))
   {
      CString str;
      str.Format("Incorrect data type on expression (%s), %s type required", pep->exprValue, enc.GetRuntimeClass()->m_lpszClassName);
      throw ExecError( str, pep->exprLine );
   }

   // success
   return pep;
}

//
// ScriptExecutor::ResetFamily
//

void ScriptExecutor::ResetFamily( ScriptBasePtr sbp )
{
   // set the object status
   sbp->baseStatus = SCRIPT_STATUS_NONE;

   // tell the app to set the image list element to match
   SetImageStatus( sbp, SCRIPT_STATUS_NONE );

   // change the children
   for (POSITION xpos = sbp->GetHeadPosition(); xpos; )
      ResetFamily( (ScriptBasePtr)sbp->GetNext( xpos ) );
}

//
// ScriptExecutor::ResetTest
//

void ScriptExecutor::ResetTest( ScriptTestPtr stp )
{
   // reset all non-dependancy elements
   for (int j = 0; j < stp->Length(); j++) {
      ScriptBasePtr sbp = (ScriptBasePtr)stp->Child( j );
      if (sbp->baseType != ScriptBase::scriptDependency)
         ResetFamily( sbp );
   }

   // change this test status (which can update the section)
   SetTestStatus( stp, SCRIPT_STATUS_NONE );
}

//
// ScriptExecutor::SetTestStatus
//

void ScriptExecutor::SetTestStatus( ScriptTestPtr stp, int stat )
{
   bool  depChanged;

   ASSERT( stp->baseType == ScriptBase::scriptTest );

   // set the object status
   stp->baseStatus = stat;

   // tell the app to set the image list element to match
   SetImageStatus( stp, stat );

   // update the status of the dependencies
   // get the parent section
   ScriptSectionPtr ssp = (ScriptSectionPtr)stp->baseParent;

   // loop through the tests
   for (int i = 0; i < ssp->Length(); i++) {
      ScriptTestPtr tp = (ScriptTestPtr)ssp->Child( i );

      // reset the change flag
      depChanged = false;

      // check for a dependancy
      for (int j = 0; j < tp->Length(); j++) {
         ScriptDependencyPtr sdp = (ScriptDependencyPtr)tp->Child( j );
         if (sdp->baseType != ScriptBase::scriptDependency)
            continue;

         // check the name
         if (sdp->baseLabel == stp->baseLabel) {
            // set the object status
            sdp->baseStatus = stat;

            // tell the app to set the image list element to match
            SetImageStatus( sdp, stat );

            depChanged = true;
         }
      }

      // verify the test status to match the deps
      if (depChanged && (tp != stp)) {
         int newStatus = CalcTestStatus( tp );
         if (newStatus != tp->baseStatus)
            SetTestStatus( tp, newStatus );     // ### if A deps on B and B on A, recursive nightmare!
      }
   }

   // verify the section status
   VerifySectionStatus( ssp );
}

//
// ScriptExecutor::SetPacketStatus
//

//void ScriptExecutor::SetPacketStatus( ScriptPacketPtr spp, int stat )
void ScriptExecutor::SetPacketStatus( ScriptBasePtr spp, int stat )
{
   TRACE2( "Base %08X status %d\n", spp, stat );

   // set the object status
   spp->baseStatus = stat;

   // tell the app to set the image list element to match
   SetImageStatus( spp, stat );
}

//
// ScriptExecutor::CalcTestStatus
//

int ScriptExecutor::CalcTestStatus( ScriptTestPtr stp )
{
   int      newStatus = stp->baseStatus;

   // loop through the deps
   for (int i = 0; i < stp->Length(); i++) {
      ScriptDependencyPtr sdp = (ScriptDependencyPtr)stp->Child( i );
      if (sdp->baseType != ScriptBase::scriptDependency)
         continue;
      
      // test all the combinations
      switch ((newStatus << 4) + sdp->baseStatus) {
         case 0x00:     // no status for some
         case 0x10:
         case 0x01:
            newStatus = SCRIPT_STATUS_NONE;
            break;

         case 0x11:     // everything OK so far, stay green
            newStatus = SCRIPT_STATUS_OK;
            break;

         case 0x02:     // found (or had) a warning
         case 0x12:
         case 0x20:
         case 0x21:
         case 0x22:
            newStatus = SCRIPT_STATUS_WARN;
            break;

         case 0x03:     // found (or had) a failure
         case 0x13:
         case 0x23:
         case 0x30:
         case 0x31:
         case 0x32:
         case 0x33:
            newStatus = SCRIPT_STATUS_FAIL;
            break;
      }
   }

   // return the result
   return newStatus;
}

void ScriptExecutor::VerifySectionStatus( ScriptSectionPtr ssp )
{
   int      newStatus = SCRIPT_STATUS_NONE;
   int      len = ssp->Length();

   // no tests in section leaves it at 0
   if (len == 0) {
      // set the object status
      ssp->baseStatus = newStatus;

      // tell the app to set the image list element to match
      SetImageStatus( ssp, newStatus );

      return;
   }

   // pick up the first child status
   newStatus = ssp->Child( 0 )->baseStatus;

   // loop through the deps
   for (int i = 1; i < ssp->Length(); i++) {
      ScriptTestPtr stp = (ScriptTestPtr)ssp->Child( i );
      
      // test all the combinations
      switch ((newStatus << 4) + stp->baseStatus) {
         case 0x00:     // no status for some
         case 0x10:
         case 0x01:
            newStatus = 0;
            break;

         case 0x11:     // everything OK so far, stay green
            newStatus = 1;
            break;

         case 0x02:     // found (or had) a warning
         case 0x12:
         case 0x20:
         case 0x21:
         case 0x22:
            newStatus = 2;
            break;

         case 0x03:     // found (or had) a failure
         case 0x13:
         case 0x23:
         case 0x30:
         case 0x31:
         case 0x32:
         case 0x33:
            newStatus = 3;
            break;
      }
   }

   // see if the status changed
   if (newStatus != ssp->baseStatus) {
      // set the object status
      ssp->baseStatus = newStatus;

      // tell the app to set the image list element to match
      SetImageStatus( ssp, newStatus );
   }
}

//
// ScriptExecutor::SendNPDU
//
// When a packet has been built it is sent to a specific filter for forwarding 
// to the correct endpoint.  This function is provided as a compliment to 
// ReceiveNPDU().
//

void ScriptExecutor::SendNPDU( ScriptNetFilterPtr fp, const BACnetNPDU &npdu )
{
   fp->Indication( npdu );
}

//
// ScriptExecutor::ReceiveNPDU
//
// This function is called by a filter when some message is coming up through 
// the stack.  The executor will check to see if it passes whatever test is 
// pending in the currently running script.
//

void ScriptExecutor::ReceiveNPDU( ScriptNetFilterPtr fp, const BACnetNPDU &npdu )
{
   CSingleLock    lock( &execCS );

   // lock to prevent multiple thread access
   lock.Lock();

   // if were not running, just toss the message
   if (execState != execRunning)
      return;

   TRACE( "ScriptExecutor::ReceiveNPDU %p: Got an NPDU on %s at %u\n", this, fp->filterName, GetTickCount() );

   // if we're not expecting something, toss it
   if (!execPending) {
      TRACE( "(not expecting a packet)\n" );
      Msg( 3, 0, "Executor not expecting a packet" );
      return;
   }

   // TODO: We get null here when running a sequence of tests.
   // Is this enough protection, or is there other stuff to clean up?
   if (execPacket == NULL) {
      TRACE( "ScriptExecutor::ReceiveNPDU: execPacket is NULL. \n" );
      return;
   }
   
   if (execPacket->packetType != ScriptPacket::expectPacket) {
      TRACE( "(not pointing to an expect packet)\n" );
      Msg( 3, 0, "Executor not pointing to an EXPECT packet" );
      return;
   }
   
   // match against the pending tests
   for (ScriptPacketPtr pp = execPacket; pp; pp = (ScriptPacketPtr) pp->m_pcmdFail)
   {
      if ((pp->baseStatus == 2) && (pp->baseType == ScriptBase::scriptPacket))
      {
         // this test is still pending, stash the execPacket
         ScriptPacketPtr savePacket = execPacket;
         execPacket = pp;
         execCommand = pp;  // added by LJT
         
         //Modified by Zhu Zhenhua, 2003-11-25
         if (ExpectPacket(fp,npdu)) 
         {
            if (!pp->bpacketNotExpect)
            {
               // test was successful, reset all pending packets to unprocessed
               for (ScriptPacketPtr pp1 = savePacket; pp1; pp1 = (ScriptPacketPtr) pp1->m_pcmdFail)
               {
                  if ((pp1->baseStatus == 2) && (pp1->baseType == ScriptBase::scriptPacket) && (pp1 != pp))
                  {
                     SetPacketStatus( pp1, SCRIPT_STATUS_NONE );
                  }
               }

               // move on to next statement
               NextPacket( true );
               break;
            }
            else
            {
               for (ScriptPacketPtr pp1 = savePacket; pp1; pp1 = (ScriptPacketPtr) pp1->m_pcmdFail)
               {
                  if ((pp1->baseStatus == 2) && (pp1->baseType == ScriptBase::scriptPacket) && (pp1 != pp))
                  {
                     SetPacketStatus( pp1, SCRIPT_STATUS_NONE );
                  }
                  
                  // move on to next statement
                  NextPacket( false );

                  // TODO: wtf?  Why the loop if we are going to exit first time round?
                  break;
               }
            }
         }
         execPacket = savePacket;
      }
   }

   // unlock
   lock.Unlock();
}

//
// ScriptExecutor::ReceiveAPDU
//
// This function is called when a complete APDU is received from the device
// object.
//
//  Return true if the script ate the APDU, else false
//

bool ScriptExecutor::ReceiveAPDU( const BACnetAPDU &apdu )
{
   bool retval = false;
   CSingleLock    lock( &execCS );

   // lock to prevent multiple thread access
   lock.Lock();

   // if were not running, just toss the message
   if (execState != execRunning)
      return retval;

   TRACE( "ScriptExecutor::ReceiveAPDU %p: Got an APDU at %u\n", this, GetTickCount() );

   // if we're not expecting something, toss it
   if (!execPending) 
   {
      TRACE( "(not expecting a packet)\n" );
      Msg( 3, 0, "Executor not expecting a packet" );
   }

   // TODO: We get null here when running a sequence of tests.
   // Is this enough protection, or is there other stuff to clean up?
   else if (execPacket == NULL) 
   {
      TRACE( "ScriptExecutor::ReceiveAPDU: execPacket is NULL. \n" );
   }

   else if (execPacket->packetType != ScriptPacket::expectPacket) 
   {
      TRACE( "(not pointing to an expect packet)\n" );
      Msg( 3, 0, "Executor not pointing to an EXPECT packet" );
   }

   // match against the pending tests
   else
   {
      for (ScriptPacketPtr pp = execPacket; pp; pp = (ScriptPacketPtr) pp->m_pcmdFail)
      {
         if ((pp->baseStatus == 2) && (pp->baseType == ScriptBase::scriptPacket))
         {
            // this test is still pending, stash the execPacket
            ScriptPacketPtr savePacket = execPacket;
            execPacket = pp;
            execCommand = pp;
            if (ExpectDevPacket(apdu)) 
            {
               // test was successful, reset all pending packets to unprocessed
               for (ScriptPacketPtr pp1 = savePacket; pp1; pp1 = (ScriptPacketPtr) pp1->m_pcmdFail)
               {
                  if ((pp1->baseStatus == 2) && 
                     (pp1->baseType == ScriptBase::scriptPacket) && 
                     (pp1 != pp))
                  {
                     SetPacketStatus( pp1, SCRIPT_STATUS_NONE );
                  }
               }

               // Don't let the Device see the packet
               // TODO: Base this on a keyword-controlled flag?
               retval = true;

               // move on to next statement
               NextPacket( true );
               break;
            }
            execPacket = savePacket;
            execCommand = savePacket;
         }
      }
   }

   // unlock
   lock.Unlock();

   return retval;
}

//
// ScriptExecutor::SetImageStatus
//

void ScriptExecutor::SetImageStatus( ScriptBasePtr sbp, int stat )
{
   // queue it and send it
   execMsgQueue.Fire( new ScriptMsgStatus(execDoc, sbp, stat) );
}


LPCSTR OperatorToString(int iOperator)
{
   switch( iOperator )
   {
      case '>>':  return ">>";   // assignment
      case '?=':  return "?=";   // don't care case
      case '=': return "=";
      case '<': return "<";
      case '>': return ">";
      case '<=': return "<=";
      case '>=': return ">=";
      case '!=': return "!=";
   }

   ASSERT(0);
   return NULL;
}


//
// Matching Functions
//

bool Match( int op, int a, int b )
{
   switch (op) {
      case '?=':  return true;   // don't care case
      case '=': return (a == b);
      case '<': return (a < b);
      case '>': return (a > b);
      case '<=': return (a <= b);
      case '>=': return (a >= b);
      case '!=': return (a != b);
   }

   return false;
}

bool Match( int op, unsigned long a, unsigned long b )
{
   switch (op) {
      case '?=':  return true;   // don't care case
      case '=': return (a == b);
      case '<': return (a < b);
      case '>': return (a > b);
      case '<=': return (a <= b);
      case '>=': return (a >= b);
      case '!=': return (a != b);
   }

   return false;
}

// Testing floating point numbers for "almost equality" is surprisingly hard.
// Read all about it at
// http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
//
// VTS formerly used an epsilon of 10e-5.  But that turns out to be silly:
//   72.000000 as an IEEE float is the bit pattern 0x42900000
// Adding one to the bit pattern (incrementing the mantissa, we get
//   72.000008 as an IEEE float is the bit pattern 0x42900001
// Note that the difference is 8e-6, which is smaller than the epsilon.
// Things get even siller for larger numbers:
//   1e30 is 1000000015047466200000000000000.000000 is 0x7149F2CA
//           1000000090605329900000000000000.000000 is 0x7149F2CB
// For very small numbers (negative exponent), thing go wonky the other way.
//
// So we follow the method outlined in the link above, and compare mantissa bits.

// Union to help us juggle floats
union Float_t
{
   Float_t(float num = 0.0f)
   : m_float(num)
   {
      ASSERT( sizeof(float) == sizeof(UINT) );
      ASSERT( sizeof(Float_t) == sizeof(UINT) );
   }

   bool IsNaN() const
   {
      // IEEE 754 defines 32-bit NaN values as:
      // - sign bit is don't care
      // - 8 exponent bits all set
      // - 23 mantissa bits, not all 0
      return ((m_uint & 0x7F800000) == 0x7F800000) & ((m_uint & 0x007FFFFF) != 0);
   }

   bool IsInf() const
   {
      // IEEE 754 defines 32-bit infinity values as:
      // - sign bit
      // - 8 exponent bits all set
      // - 23 mantissa bits, all 0
      return ((m_uint & 0x7FFFFFFF) == 0x7F800000);
   }

   bool IsNegative() const { return (m_uint & 0x80000000) != 0; }
   UINT Mantissa() const { return m_uint & 0x007FFFFF; }
   UINT Exponent() const { return (m_uint >> 23) & 0xFF; }

   UINT  m_uint;
   float m_float;
};

// Union to help us juggle doubles
union Double_t
{
   Double_t(double num = 0.0)
   : m_double(num)
   {
      ASSERT( sizeof(double) == sizeof(unsigned _int64) );
      ASSERT( sizeof(Double_t) == sizeof(unsigned _int64) );
   }

   bool IsNaN() const
   {
      // IEEE 754 defines 64-bit NaN values as:
      // - sign bit is don't care
      // - 11 exponent bits all set
      // - 52 mantissa bits, not all 0
      return ((m_uint & 0x7FF0000000000000) == 0x7FF0000000000000) & ((m_uint & 0x000FFFFFFFFFFFFF) != 0);
   }

   bool IsInf() const
   {
      // IEEE 754 defines 64-bit infinity values as:
      // - sign bit
      // - 11 exponent bits all set
      // - 52 mantissa bits, all 0
      return ((m_uint & 0x7FFFFFFFFFFFFFFF) == 0x7FF0000000000000);
   }

   bool IsNegative() const { return (m_uint & ((unsigned _int64)1<<63)) != 0; }
   unsigned _int64 Mantissa() const { return m_uint & 0x000FFFFFFFFFFFFF; }
   unsigned _int64 Exponent() const { return (m_uint >> 52) & 0x7FF; }

   unsigned _int64  m_uint;
   double m_double;
};

// Maximumm difference in bits between "close" values:
// 24-bit mantissa is a little over 7 signifcant digits
#define MANTISSA_BIT_DIFFERENCE 4

// Return true if the value is Not a Number
bool IsNaN( float theValue )
{
   Float_t ft(theValue);
   return ft.IsNaN();
}

// Return true if the value is Not a Number
bool IsNaN( double theValue )
{
   Double_t dub(theValue);
   return dub.IsNaN();
}

// Return true if the values are equal or "very close"
bool DamnClose( float a, float b )
{
   Float_t fta(a), ftb(b);

   // If either or even BOTH values are NaN, all C comparisons except inequality
   // return false. That is nice mathematically, but when we compare for
   // equality of a value read from a device to the value specified in an EPICS,
   // we mean "NaN is a NaN". So we do some fussing for equality and inequality.
   bool retval = fta.IsNaN();
   if (retval)
   {
      // At least one NaN: opt out of normal comparisons
      retval = ftb.IsNaN();                          // true only if BOTH are NaN
   }
   else if (fta.IsInf() || ftb.IsInf())
   {
      // At least one is "infinity"
      retval = (a == b);
   }
   else if (a == b)
   {
      retval = true;
   }
   else if (fta.IsNegative() != ftb.IsNegative())
   {
      // Signs differ.  ADD THEM to get bit difference
      retval = ((fta.m_uint + ftb.m_uint) & 0x7FFFFFFF) < MANTISSA_BIT_DIFFERENCE;
   }
   else
   {
      // Signs are the same. Compare bit difference
      retval = abs( (int)fta.m_uint - (int)ftb.m_uint ) < MANTISSA_BIT_DIFFERENCE;
   }

   return retval;
}

// Return true if the values are equal or "very close"
bool DamnClose( double a, double b )
{
   Double_t dta(a), dtb(b);

   // If either or even BOTH values are NaN, all C comparisons except inequality
   // return false. That is nice mathematically, but when we compare for
   // equality of a value read from a device to the value specified in an EPICS,
   // we mean "NaN is a NaN". So we do some fussing for equality and inequality.
   bool retval = dta.IsNaN();
   if (retval)
   {
      // At least one NaN: opt out of normal comparisons
      retval = dtb.IsNaN();                          // true only if BOTH are NaN
   }
   else if (dta.IsInf() || dtb.IsInf())
   {
      // At least one is "infinity"
      retval = (a == b);
   }
   else if (a == b)
   {
      retval = true;
   }
   else if (dta.IsNegative() != dtb.IsNegative())
   {
      // Signs differ.  ADD THEM to get bit difference
      retval = ((dta.m_uint + dtb.m_uint) & 0x7FFFFFFFFFFFFFFF) < MANTISSA_BIT_DIFFERENCE;
   }
   else
   {
      // Signs are the same. Compare bit difference
      retval = abs( (_int64)dta.m_uint - (_int64)dtb.m_uint ) < (_int64)MANTISSA_BIT_DIFFERENCE;
   }

   return retval;
}

bool Match( int op, float a, float b )
{
   switch (op) {
      case '?=':  return true;   // don't care case

      case '=':  return DamnClose( a, b );
      case '!=': return !DamnClose( a, b );

      case '<':  return (a < b);
      case '>':  return (a > b);
      case '<=': return (a <= b) || DamnClose( a, b );
      case '>=': return (a >= b) || DamnClose( a, b );
   }

   return false;
}

bool Match( int op, double a, double b )
{
   switch (op) {
      case '?=':  return true;   // don't care case

      case '=':  return DamnClose( a, b );
      case '!=': return !DamnClose( a, b );

      case '<':  return (a < b);
      case '>':  return (a > b);
      case '<=': return (a <= b) || DamnClose( a, b );
      case '>=': return (a >= b) || DamnClose( a, b );
   }

   return false;
}

// Convert theValue into a string.  Special values are shown as NaN, +inf, and -inf.
const char* FloatToString( CString &theString, float theValue, bool fullResolution )
{
   Float_t fl(theValue);
   if (fl.IsNaN())
   {
      theString = "NaN";
   }
   else if (fl.IsInf())
   {
      theString = (fl.IsNegative()) ? "-inf" : "+inf";
   }
   else
   {
      // Use %g to avoid silly decimal point position with very large and small numbers.
      // # ensures that a decimal point is always shown.
      // A 24-bit mantissa is 7.2 sig. digits.  Showing 8 is enough so that the
      // effect of even a one-bit change to the mantissa is visible.
      theString.Format( (fullResolution) ? "%#.8g" : "%#g", theValue );
   }

   return (const char*)theString;
}

// Convert theValue into a string.  Special values are shown as NaN, +inf, and -inf.
const char* DoubleToString( CString &theString, double theValue, bool fullResolution )
{
   Double_t dub(theValue);
   if (dub.IsNaN())
   {
      theString = "NaN";
   }
   else if (dub.IsInf())
   {
      theString = (dub.IsNegative()) ? "-inf" : "+inf";
   }
   else
   {
      // Use %g to avoid silly decimal point position with very large and small numbers.
      // # ensures that a decimal point is always shown.
      // A 52-bit mantissa is 15.6 sig. digits.  Showing 17 is enough so that the
      // effect of even a one-bit change to the mantissa is visible.
      theString.Format( (fullResolution) ? "%#.17lg" : "%#lg", theValue );
   }

   return (const char*)theString;
}

// Convert string into a floating point number.
// Accepts special values NaN, inf, +inf, and -inf.
// Returns the number of characters (if any) read from the string.
// In order that the count be unambiguous, DOES NOT accept leading whitespace.
int StringToFloat( const char *pString, float &theValue )
{
   int retval = 0;
   UINT special;

   if (_strnicmp( pString, "inf", 3 ) == 0)
   {
      // Plus infinity: sign bit, 8 exponent bits all set, 23 mantissa bits all 0
      special = 0x7F800000;
      theValue = *(float*)&special;
      retval = 3;
   }
   else if (_strnicmp( pString, "+inf", 4 ) == 0)
   {
      // Plus infinity: sign bit, 8 exponent bits all set, 23 mantissa bits all 0
      special = 0x7F800000;
      theValue = *(float*)&special;
      retval = 4;
   }
   else if (_strnicmp( pString, "-inf", 4 ) == 0)
   {
      // Minus infinity: sign bit, 8 exponent bits all set, 23 mantissa bits all 0
      special = 0xFF800000;
      theValue = *(float*)&special;
      retval = 4;
   }
   else if (_strnicmp( pString, "NaN", 3 ) == 0)
   {
      // Not a number: sign bit, 8 exponent bits all set, 23 mantissa bits not all 0
      // We set bit22, which makes this a quiet NaN according to IEEE 754.
      special = 0x7FC00001;
      theValue = *(float*)&special;
      retval = 3;
   }
   else
   {
      // Get a float, count characters
      int nChar;
      if (sscanf( pString, "%f%n", &theValue, &nChar ) == 1)
      {
         retval = nChar;
      }
   }

   return retval;
}

// Convert string into a floating point number.
// Accepts special values NaN, inf, +inf, and -inf.
// Returns the number of characters (if any) read from the string.
// In order that the count be unambiguous, DOES NOT accept leading whitespace.
int StringToDouble( const char *pString, double &theValue )
{
   int retval = 0;
   unsigned _int64 special;

   if (_strnicmp( pString, "inf", 3 ) == 0)
   {
      // Plus infinity: sign bit, 11 exponent bits all set, 52 mantissa bits all 0
      special = 0x7FF0000000000000;
      theValue = *(double*)&special;
      retval = 3;
   }
   else if (_strnicmp( pString, "+inf", 4 ) == 0)
   {
      // Plus infinity: sign bit, 11 exponent bits all set, 52 mantissa bits all 0
      special = 0x7FF0000000000000;
      theValue = *(double*)&special;
      retval = 4;
   }
   else if (_strnicmp( pString, "-inf", 4 ) == 0)
   {
      // Minus infinity: sign bit, 11 exponent bits all set, 52 mantissa bits all 0
      special = 0xFFF0000000000000;
      theValue = *(double*)&special;
      retval = 4;
   }
   else if (_strnicmp( pString, "NaN", 3 ) == 0)
   {
      // Not a Number: sign bit, 11 exponent bits all set, 52 mantissa bits not all 0
      // We set bit51, which makes this a quiet NaN according to IEEE 754.
      special = 0x7FF8000000000001;
      theValue = *(double*)&special;
      retval = 3;
   }
   else
   {
      // Get a double, count characters
      int nChar;
      if (sscanf( pString, "%lf%n", &theValue, &nChar ) == 1)
      {
         retval = nChar;
      }
   }

   return retval;
}

#if 0
void TestFloater( CString &str, Double_t &fta, Double_t &ftb )
{
   CString m2;
   m2.Format( "Compare %.10lg is 0x%16llX %d\n", ftb.m_double, ftb.m_uint, DamnClose( fta.m_double, ftb.m_double ) );
   str += m2;
   ftb.m_uint += 1;
}

void TestFloater( double theValue )
{
   CString m1, m2;
   Double_t fta( theValue );
   m2.Format( "%.10lg is 0x%16llX\n", fta.m_double, fta.m_uint );
   m1 += m2;

   Double_t ftb( theValue );
   ftb.m_uint -= 4;
   TestFloater( m1, fta, ftb );
   TestFloater( m1, fta, ftb );
   TestFloater( m1, fta, ftb );
   TestFloater( m1, fta, ftb );
   TestFloater( m1, fta, ftb );
   TestFloater( m1, fta, ftb );
   TestFloater( m1, fta, ftb );
   TestFloater( m1, fta, ftb );
   TestFloater( m1, fta, ftb );

   AfxMessageBox( m1 );
}

void TestFloaters()
{
   TestFloater( 1.1 );
   TestFloater( 1e-30 );
   TestFloater( 1e30 );
}
#endif

/*
void ScriptExecutor::CompareStreamData( BACnetAPDUDecoder & dec, int iOperator, const BACnetOctet * pData, int nLen, LPCSTR lpstrValueName )
{
   // Let's just compare byte by byte
   // If the comparison failed, we won't even get here due to throw condition

   CString strThrowMessage;

   // check the length
   if (dec.pktLength < nLen)
      strThrowMessage.Format(IDS_SCREX_COMPSHORTDATA, dec.pktLength, lpstrValueName, nLen );
   else
   {
      // Good ol' memcmp will do for this but we need to check to see if we're supposed to equal or not equal...

      int iCompResult = memcmp(dec.pktBuffer, (void *) pData, nLen);
      switch( iOperator )
      {
         case '=':

            if ( iCompResult != 0 )
               strThrowMessage.Format(IDS_SCREX_COMPSTREAMFAIL_NE, lpstrValueName );
            break;

         case '!=':

            if ( iCompResult == 0 )
               strThrowMessage.Format(IDS_SCREX_COMPSTREAM_E, lpstrValueName );
            break;

         default:    // just give up, eh?

            strThrowMessage.Format(IDS_SCREX_COMPEQREQ, lpstrValueName );
      }
   }

   if ( !strThrowMessage.IsEmpty() )
      throw CString(strThrowMessage);

   dec.pktLength -= nLen;
   dec.pktBuffer += nLen;
}
*/

//
// VTSQueue<T>::VTSQueue
//

template<class T>
   VTSQueue<T>::VTSQueue()
      : qFirst(0), qLast(0)
   {
   }

//
// VTSQueue<T>::~VTSQueue
//

template<class T>
   VTSQueue<T>::~VTSQueue()
   {
      if (qFirst)
         TRACE0( "Warning: queue not empty\n" );
   }

//
// VTSQueue<T>::Read
//
// This function returns a pointer to the first (oldest) element that was
// added to the queue.  If the queue is empty it returns nil.
//

template<class T>
   T* VTSQueue<T>::Read( void )
   {
      // lock the queue
      qCS.Lock();

      VTSQueueElem   *cur = qFirst;
      T           *rslt = 0;

      // if the queue not is empty, extract the first element
      if (cur) {
         // set result
         rslt = cur->qElem;

         // remove from list
         qFirst = cur->qNext;
         if (!qFirst)
            qLast = 0;

         // delete wrapper element
         delete cur;
      }

      // unlock the queue
      qCS.Unlock();

      // fini
      return rslt;
   }

//
// VTSQueue<T>::Write
//
// This function adds a pointer to an element to the end of the queue.
//

template<class T>
   void VTSQueue<T>::Write( T* tp )
   {
      // lock the queue
      qCS.Lock();

      VTSQueueElem *cur = new VTSQueueElem;
      cur->qElem = tp;
      cur->qNext = 0;

      if (qFirst)
         qLast->qNext = cur;
      else
         qFirst = cur;

      qLast = cur;

      // unlock the queue
      qCS.Unlock();
   }


template<class T>
   void VTSQueue<T>::Fire( T* tp )
   {
      Write(tp);
      ::PostThreadMessage( AfxGetApp()->m_nThreadID, WM_VTS_EXECMSG, (WPARAM)0, (LPARAM)0 );
   }
