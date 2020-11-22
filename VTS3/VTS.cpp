// VTS.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxinet.h"
#include "VTS.h"

#include "VTSPreferences.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#include "VTSDoc.h"
#include "VTSView.h"

#include "DetailView.h"
#include "HexView.h"
#include "ListSummaryView.h"

#include "ScriptDocument.h"
#include "ScriptFrame.h"
#include "ScriptExecutor.h"

#include "WinWinPcap.hpp"
#include "WinBACnetTaskManager.hpp"

#include "ScriptMsgMake.h"
#include "ScriptMakeDlg.h"

#include "file_ver.h"

// URL at which to check the latest release of VTS
static const char s_latestReleaseURL[] = "http://vts.sourceforge.net/vtsversion.txt";
// URL from which to download the latest release of VTS
static const char s_downloadURL[] =      "http://sourceforge.net/projects/vts/";

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define NT_DebugMemState      0
#define  NT_DebugScriptFilters   1

int __cdecl VTSAllocHook(
   int      nAllocType,
   void     *pvData,
   size_t   nSize,
   int      nBlockUse,
   long     lRequest,
   const unsigned char *szFileName,
   int      nLine
   );

#endif

#if NT_DebugScriptFilters
ScriptDebugNetFilter gDebug1( "_1" );
ScriptDebugNetFilter gDebug2( "_2" );
#endif

extern CWinThread *gBACnetWinTaskThread;

// Build Hash tables from enum strings
extern void MakeHashTables();
extern void FreeHashTables();

//
// VTSApp
//

VTSApp theApp;

VTSPreferences gVTSPreferences;

// Variables used to pass VTS version information read from s_latestReleaseURL.
// We use this hack to decouple the reader thread from the VTSApp, which in
// rare slow-web cases MIGHT be deinitialized before the thread completes.
static bool    s_gotVersionInfo = false;
static CString s_versionInfo;
static CString s_changeList;

BEGIN_MESSAGE_MAP(VTSApp, CWinApp)
   //{{AFX_MSG_MAP(VTSApp)
   ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
   ON_COMMAND(ID_APP_CHECK_FOR_UPDATES, OnCheckForUpdates)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
   // Standard file based document commands
   ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_MRU_WKS1, ID_FILE_MRU_WKSLAST, OnUpdateRecentWorkspaceMenu)
   ON_COMMAND_EX_RANGE(ID_FILE_MRU_WKS1, ID_FILE_MRU_WKSLAST, OnOpenRecentWorkspace)
   ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
   ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
   ON_COMMAND(ID_FILE_WKS_NEW, OnFileWksNew)
   ON_COMMAND(ID_FILE_WKS_SWITCH, OnFileWksSwitch)

   // Standard print setup command
   ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

//
// VTSApp::VTSApp
//

VTSApp::VTSApp(void)
: m_pdoctempConfig(NULL)
, m_pRecentWorkspaceList(NULL)
, m_version("?.?.?")
{
   // TODO: add construction code here,
   // Place all significant initialization in InitInstance

   // Breakpoint on heap allocation to debug heap leaks
// _CrtSetBreakAlloc( 1638 );
}


VTSApp::~VTSApp(void)
{
   if ( m_pdoctempConfig != NULL )     // kill saved workspace template
      delete m_pdoctempConfig;

   if (m_pRecentWorkspaceList != NULL)
      delete m_pRecentWorkspaceList;
}

// Get the full path of theDirectory relative to the executable
void VTSApp::GetRelativeToExe( CString &thePath, LPCTSTR theDirectory ) const
{
   // During development, the executable is ...\debug\vts.exe or ...\release\vts.exe
   // and "theDirectory" is a peer.
   // As installed, the executable is ...\vts.exe
   // and "theDirectory" is a subdirectory
   CString base = m_pszHelpFilePath;
   int pathLen = base.ReverseFind( '\\' );
   if (pathLen >= 0)
   {
      // path without filename
      base = base.Left( pathLen );
   }
   
   CFileStatus status;
   thePath = base + "\\" + theDirectory;
   if (CFile::GetStatus( thePath, status ))
   {
      // found the directory below us
   }
   else 
   {
      pathLen = base.ReverseFind( '\\' );
      if (pathLen >= 0)
      {
         // Go up a directory
         base = base.Left( pathLen );
      }
      thePath = base + "\\" + theDirectory;
      if (CFile::GetStatus( thePath, status ))
      {
         // found the directory as our peer
      }
      else
      {
         // Can't find the directory.  Just use our directory
         thePath = base;
      }
   }
}


void VTSApp::AddToRecentWorkspaceList(LPCTSTR lpszPathName)
{
   ASSERT_VALID(this);
   ASSERT(lpszPathName != NULL);
   ASSERT(AfxIsValidString(lpszPathName));

   if (m_pRecentWorkspaceList != NULL)
      m_pRecentWorkspaceList->Add(lpszPathName);
}


void VTSApp::LoadWorkspaceMRU(UINT nMaxMRU)
{
   ASSERT_VALID(this);
   ASSERT(m_pRecentWorkspaceList == NULL);

   if (nMaxMRU != 0)
   {
      // create file MRU since nMaxMRU not zero
      m_pRecentWorkspaceList = new CRecentFileList(0, _T("Recent Workspace List"), _T("File%d"),   nMaxMRU);
      m_pRecentWorkspaceList->ReadList();
   }

// m_nNumPreviewPages = GetProfileInt(_afxPreviewSection, _afxPreviewEntry, 0);
}

//=============================================================================
// Thread to check VTS version on the web
UINT __cdecl CheckWebVersion( LPVOID pParam )
{
   // OpenURL throws an exception on failure
   try
   {
      CInternetSession sess;
      CStdioFile *pFile = sess.OpenURL( s_latestReleaseURL, 1,
                                        INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_DONT_CACHE );
      if (pFile)
      {
         CHttpFile *pHttpFile = (CHttpFile*)pFile;
         DWORD status;
         if (pFile->IsKindOf(RUNTIME_CLASS(CHttpFile)) &&
             pHttpFile->QueryInfoStatusCode(status) &&
             (status == 200))
         {
            // The file consists of
            // - Initial line "VTS Version: this file is parsed by VTS.exe to determine the latest release"
            // - Version number 3.4.5.6
            // - Zero or more lines describing major new features
            CString str;
            pFile->ReadString( str );
            if (str.Find( "VTS Version" ) == 0)
            {
               pFile->ReadString( s_versionInfo );

               while (pFile->ReadString(str))
               {
                  s_changeList += str;
                  s_changeList += '\n';
               }

               s_gotVersionInfo = true;

               if (pParam != NULL)
               {
                  // Tell the UI thread that we have the version data.
                  ::PostThreadMessage( AfxGetApp()->m_nThreadID, WM_VTS_GOT_VERSION, (WPARAM)0, (LPARAM)0 );
               }
            }
         }

         pFile->Close();
         delete pFile;
      }
      sess.Close();
   }
   catch (CInternetException *pExcept)
   {
      pExcept->Delete();
   }

   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Kick off a check for an updated version of VTS
void VTSApp::OnCheckForUpdates()
{
   // Check the latest version on the web.  NULL, so we don't send WM_VTS_GOT_VERSION
   CheckWebVersion( NULL );

   // Show the results or lack thereof.
   ShowUpdateInfo();
}

/////////////////////////////////////////////////////////////////////////////
// Show dialog with update information.
// Invoked manually via "Help", "Check for updates", and automatically
// if a new version is detected on the website.
void VTSApp::ShowUpdateInfo()
{
   CString str;
   if (s_gotVersionInfo)
   {
      if (s_versionInfo > m_version)
      {
         str.Format( "This is VTS version %s.\n"
                     "Version %s is available from %s"
                     "\n\n"
                     "%s",
                     (LPCTSTR)m_version,
                     (LPCTSTR)s_versionInfo, s_downloadURL,
                     (LPCTSTR)s_changeList );
      }
      else
      {
         str.Format( "This is VTS version %s.\n"
                     "No updates are currently available.",
                     (LPCTSTR)m_version );
      }

      // Save the version, so we don't keep pestering the user
      WriteProfileString( "Version", "lastShown", s_versionInfo );

      // Save the date on which we checked, so we check only weekly.
      // CAUTION: in current Visual Studio, time_t is 64 bits.
      // In VC++ 6.0 it was 32-bits.
      // This code will work with either size until 2038....
      time_t now = time(NULL);
      WriteProfileInt( "Version", "lastChecked", (int)now );
   }
   else
   {
      str.Format( "This is VTS version %s.\n"
                  "Update check failed.\n"
                  "You may check manually at %s",
                  (LPCTSTR)m_version,
                  s_downloadURL );
   }

   AfxMessageBox( str, MB_ICONINFORMATION | MB_OK);
}

//
// VTSApp::InitInstance
//

BOOL VTSApp::InitInstance()
{
   // Get our version
   CString str( m_pszHelpFilePath );
   int dot = str.Find( ".HLP" );
   if (dot >= 0)
   {
      str = str.Left( dot );
      str += ".EXE";
      // Get our version resource
      CFileVersionInfo  info;
      info.ReadVersionInfo( str );
      if (info.IsValid())
      {
         m_version = info.GetFileVersionString();
      }
   }

   if (!AfxSocketInit())
   {
      AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
      return FALSE;
   }

   AfxEnableControlContainer();

   // Standard initialization

   // These calls became obsolete in MFC 5.0
#if _MFC_VER < 0x0500
#ifdef _AFXDLL
   Enable3dControls();        // Call this when using MFC in a shared DLL
#else
   Enable3dControlsStatic();  // Call this when linking to MFC statically
#endif
#endif

   // Change the registry key under which our settings are stored.
   SetRegistryKey(IDS_REGISTRYKEY);

   // Build Hash tables from enum strings
   MakeHashTables();

   LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)
   LoadWorkspaceMRU(10);

   // Register the application's document templates.  Document templates
   //  serve as the connection between documents, frame windows and views.

   m_pdoctempConfig = new CMultiDocTemplate(IDR_VDBTYPE, RUNTIME_CLASS(VTSDoc), RUNTIME_CLASS(CChildFrame), RUNTIME_CLASS(CListSummaryView));
   AddDocTemplate(new CMultiDocTemplate(IDR_VTSTYPE, RUNTIME_CLASS(ScriptDocument), RUNTIME_CLASS(ScriptFrame), RUNTIME_CLASS(CEditView)));

   // create main MDI Frame window
   CMainFrame* pMainFrame = new CMainFrame;
   if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
      return FALSE;
   m_pMainWnd = pMainFrame;

   // Enable drag/drop open
   m_pMainWnd->DragAcceptFiles();

   // Enable DDE Execute open
   EnableShellOpen();
   RegisterShellFileTypes(TRUE);

   // Parse command line for standard shell commands, DDE, file open
   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

   // disable the creation of a new session, allow others
   if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
      cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

   // Dispatch commands specified on the command line
   if (!ProcessShellCommand(cmdInfo))
      return FALSE;

   // The main window has been initialized, so show and update it.
   pMainFrame->ShowWindow(m_nCmdShow);
   pMainFrame->UpdateWindow();

   // initialize the random number generator
   srand( 1 );

   // make sure the adapter list is intialized
// InitAdapterList();

#if NT_DebugMemState
   _CrtSetAllocHook( VTSAllocHook );
#endif

#if NT_DebugScriptFilters
   Bind( &gDebug1, &gDebug2 );
#endif

   // no initial environment
   gCurrentEnv = 0;

   // load the preferences
   gVTSPreferences.Load();

   // check WinPcap
   CheckWinPcapVersion();

   // Get the last opened workspace....  Will be empty if none.
   CString strLast = (*m_pRecentWorkspaceList)[0];

   // initialize the BACnet task manager
   gTaskManager = new WinBACnetTaskManager();

   // attempt to open the last document if they've had one... 
   if ( !strLast.IsEmpty() )
   {
      if ( !m_pdoctempConfig->OpenDocumentFile(strLast) )
      {
         // Open failed, ask if they want to open default config
         CString strErr;
         strErr.Format(IDS_ERR_FILEMOVEON, strLast, VTSDoc::m_pszDefaultFilename);
         switch ( AfxMessageBox(strErr, MB_ICONQUESTION | MB_YESNOCANCEL) )
         {
            case IDYES:    strLast = VTSDoc::m_pszDefaultFilename; break;
            case IDNO:     strLast.Empty();  break;
            default:    return FALSE;
         }
      }
      else
      {
         strLast.Empty();
      }
   }
   else
   {
      strLast = VTSDoc::m_pszDefaultFilename;
   }

   // when we get here strLast is empty if the file has already been opened...

   if ( !strLast.IsEmpty() )
      if ( !m_pdoctempConfig->OpenDocumentFile(VTSDoc::m_pszDefaultFilename) )
         m_pdoctempConfig->OpenDocumentFile(NULL);

   // move on from here... if we haven't opened a document file... we're just going to have to allow
   // the user to attempt one

   // Once a week, check to see if a new version is available
   // CAUTION: in current Visual Studio, time_t is 64 bits.
   // In VC++ 6.0 it was 32-bits.
   // This code will work with either size until 2038....
   time_t now = time(NULL);
   UINT lastChecked = GetProfileInt( "Version", "lastChecked", 0 );
   if ((UINT)now - lastChecked > 7*24*60*60)
   {
      // Kick off a thread to read the version of the most recent release of VTS
      AfxBeginThread( CheckWebVersion, this );
   }

   if ( gVTSPreferences.Setting_IsLoadEPICS() )
      pMainFrame->PostMessage(WM_COMMAND, ID_EPICS_LOADAUTO, NULL);

   // Move and add it from WinBACnetTaskManager::WinBACnetTaskManager ,Modified by xuyiping-hust
   gBACnetWinTaskThread->ResumeThread();
   return TRUE;
}

//
// VTSApp::ExitInstance
//

int VTSApp::ExitInstance()
{
   // save the preferences
   gVTSPreferences.Save();

   // delete the task manager
   delete gTaskManager;

   // clean up the adapter list
   if (gAdapterList)
      delete[] gAdapterList;

   if (m_pRecentWorkspaceList != NULL)
      m_pRecentWorkspaceList->WriteList();

   FreeHashTables();

   // continue with normal egress
   return CWinApp::ExitInstance();
}


void VTSApp::OnFileWksNew()
{
   CFileStatus fs;
   LPCSTR lpszFile = NULL;

   // test to see if the default config file already exists... if it does it will get overwritten so
   // we need to make sure the user wants to do this...

   if ( CFile::GetStatus(VTSDoc::m_pszDefaultFilename, fs) )
   {
      // config file we're about to overwrite already exists...
      // Get the full path of the default file we're trying to create

      CString strmsg;
      TCHAR szFullPathNew[_MAX_PATH];

      LPTSTR lpszFilePart;
      GetFullPathName(VTSDoc::m_pszDefaultFilename, _MAX_PATH, szFullPathNew, &lpszFilePart);

      // Test to see if the full file is really the same as the one currently loaded
      CDocument * pdocCurrent = GetWorkspace();

      if ( pdocCurrent != NULL && pdocCurrent->GetPathName().CompareNoCase(szFullPathNew) == 0 )
      {
         // default file exists already...  and it happens to be the same one that is currently
         // loaded.  Going ahead will rewrite the currently loaded workspace with defaults
         // losing what we've got.  Ask user if he wants to go ahead.
         // If yes, just drop through and overwrite this file

         strmsg.Format(IDS_WKS_OVERWRITESAVE, VTSDoc::m_pszDefaultFilename);

         switch( AfxMessageBox(strmsg, MB_YESNOCANCEL | MB_ICONQUESTION) )
         {
            case IDNO:     // user wants to save currently workspace to different name... 
                        // if he goes through with this, close down and drop through
                        // to reopen file and recreate.
               
                        if ( !GetWorkspace()->DoSave(NULL) )
                           return;
                        break;

            case IDCANCEL: return;           // abort all
         }
      }
      else
      {
         // OK... file to be recreated is NOT the same as the one we've got loaded
         // so ask user if he wants to proceed.
         // If yes, just drop through and overwrite the new file... the current one is already saved

         strmsg.Format(IDS_WKS_OVERWRITE, VTSDoc::m_pszDefaultFilename);

         switch( AfxMessageBox(strmsg, MB_YESNOCANCEL | MB_ICONQUESTION) )
         {
            case IDNO:     // User wants to switch to default file and NOT overwrite...
                        // so perform same as Workspace Switch...
                        lpszFile = VTSDoc::m_pszDefaultFilename;  break;

            case IDCANCEL: return;           // abort all
         }
      }
   }

   m_pdoctempConfig->CloseAllDocuments(TRUE);
   m_pdoctempConfig->OpenDocumentFile(lpszFile);
}


void VTSApp::OnFileWksSwitch()
{
   CString newName;
   if ( !DoPromptFileName(newName,IDS_WKS_SWITCH, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, m_pdoctempConfig))
      return;

   m_pdoctempConfig->CloseAllDocuments(TRUE);
   m_pdoctempConfig->OpenDocumentFile(newName);
}


CDocument * VTSApp::GetWorkspace(void)
{
   ASSERT(m_pdoctempConfig != NULL);
   if ( m_pdoctempConfig == NULL )
      return NULL;

   POSITION pos = m_pdoctempConfig->GetFirstDocPosition();
   return m_pdoctempConfig->GetNextDoc(pos);
}


/////////////////////////////////////////////////////////////////////////////
// MRU workspace list default implementation

void VTSApp::OnUpdateRecentWorkspaceMenu(CCmdUI* pCmdUI)
{
   ASSERT_VALID(this);

   if (m_pRecentWorkspaceList == NULL) // no MRU files
      pCmdUI->Enable(FALSE);
   else
      m_pRecentWorkspaceList->UpdateMenu(pCmdUI);
}


/////////////////////////////////////////////////////////////////////////////
// MRU workspace list default implementation

BOOL VTSApp::OnOpenRecentWorkspace(UINT nID)
{
   ASSERT(m_pRecentFileList != NULL);

   ASSERT(nID >= ID_FILE_MRU_WKS1);
   ASSERT(nID < ID_FILE_MRU_WKS1 + (UINT)m_pRecentWorkspaceList->GetSize());
   int nIndex = nID - ID_FILE_MRU_WKS1;
   ASSERT((*m_pRecentWorkspaceList)[nIndex].GetLength() != 0);

   m_pdoctempConfig->CloseAllDocuments(TRUE);
   if ( m_pdoctempConfig->OpenDocumentFile((*m_pRecentWorkspaceList)[nIndex]) == NULL )
   {
      CString strErr;
      strErr.Format(IDS_ERR_FILEOPENRCT, (*m_pRecentWorkspaceList)[nIndex] );
      AfxMessageBox(strErr, MB_OK);

      m_pRecentWorkspaceList->Remove(nIndex);
   }

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

//
// VTS Release Version
//
// Every released version of VTS gets this number bumped up by one.  If the major 
// or minor database version changes, this can be reset back down to one.
//
// Release History
//
// 3.1.2
//
//       New release versioning mechanism installed.
//
// 3.1.3
//       Changed the configuration character string field in the port descriptor
//       from 64 to 512 bytes to make room for BTR and BBMD configuration info.
//       The old value of 64 bytes is way to short for a list of peer IP addresses.
//
// 3.1.4
//       Massive changes to APDU structure, encoding, and decoding.  Changes to device 
//       object to allow it to limit itself to looking like it is a local object on 
//       one network.
//       
//          By ALC:
//          Enhancements to begin adding support for BACnet Addendum 135b
//          1) Added support for the Event_Time_Stamps property in each event-generating object. 
//          2) Added support for the TrendLog object, except for the Log_Buffer property. 
//          3) Added support for the Multi-state Value Object 
//          4) Added support for the Averaging Object 
//          By "added support", I mean that these objects and their properties can now be 
//          parsed from the EPICS correctly, and they are decoded properly in the 
//          interactive sniffer.   
//          Added the following macros to replace hard-coded maximums.
//          These are defined in VTS.H
//             MAX_DEFINED_OBJ 
//             MAX_PROP_ID
//             MAX_SERVS_SUPP
//
// 3.1.5
//       Built in support for BACnet/IP endpoints to be managed automatically (the peer list 
//       for BTR's and BBMD's have not been finished yet).  Added a new set of functions 
//       to support scripting at the device object level.  If a script does not specify a 
//       network, or BVLCI or network layer message then the SEND and EXPECT packet are 
//       assumed to operate from the context of the device object.  Only one device object 
//       may be defined, until there is a new keyword that describes which device object 
//       should be used.
//
// 3.1.6
//       Increased the size of the source and destination in the summary pane, and if the 
//       name lookup fails, show the address in hex.
//
//  3.1.7   Added SendUTCTimeSync to the send window.
//          Fixed minor problems in the Send ReadRange dialog.
//
// 3.1.8 Session data is now cleared, so it doesn't grow continuously, but is not compressed
//          That will be a future enhancement.
//       The Send windows have been sized up to make room for more buttons and clearer indicators.
//       The IDD_SENDREADPROPMULT, IDD_SENDREADPROPMULTACK and IDD_SENDWRITEPROPMULT dialog
//          boxes now have shuffle-up and shuffle-down buttons.  With all the focus and 
//          selection change messages flying around I'm sure the interface doesn't quite do 
//          what is expected.
//       All of the Send dialog boxes that have lists now have 'Show selection always' turned
//          on, which doesn't always seem to do what it's supposed to.
//       There is now an automatic invoke ID for the send window.  It is incremented when the
//          Send button is clicked.  The value is saved in the preferences (registry), so it 
//          comes back for the next VTS launch.  Note that this is NOT the same invoke ID that 
//          will be used by a built-in device object, and I think that is a good thing.
//       Now checks the version of BACMACNT.SYS it can find.  Only version 3.0 works, but I
//          don't know why.
//       Support environment parameters.  One open script can be selected as the 'environment'
//          and all other open scripts will inherit the defined parameters.
//       TRANSMIT and RECEIVE are now aliases to SEND and EXPECT.  The error messages and 
//          user visible indicators should also be renamed, but I don't expect the code to 
//          be renamed.
//
//          The following SourceForge items were addressed:
//       444144: Add line numbers to script window
//       
//       A control has been added in the left of the edit view to display line numbers.
//       A status area has been added in the status bar to display the current line number, 
//       and current line will be highlighted.
//       A menu item "Go to Line" has been added. 
//       The problem " two consecutive dashes look like one long dash" has been solved now.
//       
//       444147: Export session contents
//       
//       A menu item "Export" has been added for exporting session contents to an ASCII file, 
//       the contents include the summary and detail information.
//       
//       531519: RPM error parsing acked-transitions
//       
//       Parsing the "ack-required" bit string is also incorrect, and error is same as 
//       parsing the "acked_transitions" bit string. These bugs have been solved now.
//       
//       544513: Allow sniffer panes to be arranged
//       
//       In former VTS3, there were 3 panes each contains information of packet summary, 
//       packet details, and packet hex codes.  Now VTS3 will create 2 docking bar each 
//       contains a view in it, one displays the packet details, and the other one displays 
//       the packet hex codes. Either of these two docking bars can be dragged, resized and 
//       visible/invisible. So VTS3 allows all three windows view to be viewed full width - 
//       one over the other and select which combination of the three is displayed.
//       The docking views are implemented using a free library, CSizingControlBar library, 
//       which is a framework for creating resizable docking windows. 
//       This is a free code library whose functionality is to realize Microsoft DevStudio 
//       like docking bars. This code is free for personal and commercial use, providing 
//       the copyright notice remains intact in the source files and all eventual changes 
//       are clearly marked with comments. One must obtain the author's consent before he 
//       can include this code in a software library. For more information, 
//       please visit http://www.datamekanix.com/.
//       
//       544516: Sniffer statistics view
//       
//       Now VTS3 has the capability to provide statistics information in real time when 
//       the operator chooses to view the statistics. The statistics include:
//       1. The overall traffic load;
//       2. The packet number of one or several kinds of messages; for example, 
//           how many Who-Is packets are received/send, How many Alarm&Events packets 
//           are received/send etc;
//       3. The packet number of certain size or of certain size range; for example, 
//           how many packets whose size ranges from 100 to 1000 are received/send etc.
//       
//       444231: Associate script outline with editor
//       
//       Now when operator double click a section item in the tree/outline overview of a 
//       script (the lower left pane of a script window), VTS3 can auto-scroll to the 
//       relative position and highlight the first line of the section.
//       
//       562488: B/IP 'BDT Entry' UI needs easier format 
//       
//       Now BDT entry has been broken into three fields from single field. 
//       4 Octet B/IP address (IP Address:Port) field, 2Octet UDP port field and 4 Octet 
//       Broadcast Distribution Mask field.
//       
//       558884:Active_COV_Subscriptions not selectable
//       
//       Now property "Active_COV_Subscriptions" is selectable in the device object for 
//       read-property requests. In order to parse this property, some data structures and 
//       decoding methods have been added in VTS3, All these works were done according 
//       to Addendum C to ANSI/ASHRAE Standard 135-1995.
//       
//       561641:Cat,Sched 'profile' prop not selectable
//       
//       Now property "Profile_Name" is selectable in all standard objects for 
//       read-property requests. All these works were done according to Addendum E 
//       to ANSI/ASHRAE Standard 135-1995.
//       
//       In Addendum E, the sequence number of property "Profile_Name" is 168, in order to 
//       add this property, we must add it into data structure "BACnetPropertyIdentifier[]". 
//       So we have added other properties whose sequence numbers are in 152 to 168 into 
//       "BACnetPropertyIdentifier[]", but we haven't parsed them at present. These 
//       properties have been given in Addendum c and e, but we don't know if they have 
//       been included in ANSI/ASHRAE Standard 135-2001 as they have been described in 
//       Addendum C and E .
//
// 3.1.9 472392: ReadPropertyMultiple error (UI fix)
//       
//       Init new properties with Present_Value, now shows in list.  Increased the Property
//       column length as well as the property drop-down list.  Performed same mods to
//       ReadPropertyMultipleAck and WritePropertyMultiple dialog.  Previously the combos
//       in the Ack dialog were not large enough to drop-down.
// 
//       In relation to the list control problem in the ReadPropertyMultiple bug, a general 
//       clean up of all dialogs presenting list controls was performed.  The following lists were
//       affected:
//          ReadBDTAck
//                      - Added controls for IP / Port creation instead of edit box
//                      - Address defaults and appears in list, rather than empty space item
//                      - List keeps selection, preventing GPF in removals of list items
//          ReadFDTAck
//                      - Added controls for IP / Port creation instead of edit box
//                      - Address defaults and appears in list, rather than empty space item
//                      - List keeps selection, preventing GPF in removals of list items
//                      - Changed tab order of dialog items
//          IAmRTN, RouterBusyToNetwork, RouterAvailToNetwork, InitRT, InitRTAck
//                      - Default '1' for DNET address, port (where applicable) default to 0xBAC0
//                      - Address defaults and appears in list, rather than empty space item
//                      - List keeps selection, preventing GPF in removals of list items
//          UnconfCOVNotification, ConfCOVNotification, CreateObject
//                      - Default 'Present_Value' for property
//                      - Property value appears in list, rather than empty space item
//                      - List keeps selection, preventing GPF in removals of list items
//          VTClose, VTCloseAck
//                      - Default '1' for Session ID
//                      - ID appears in list, rather than empty space item
//                      - List keeps selection, preventing GPF in removals of list items
//          WriteFile, ReadFileAck
//                      - Default '(record data)' as string for adding data
//                      - Character data no longer needs to be placed in quotes in edit box (see below)
//                      - Data appears in list instead of empty space
//                      - List keeps selection, preventing GPF in removals of list items
//          ReadPropMult, ReadPropMultAck, WritePropMult
//                      - Default object ID
//                      - Defaulted data appears in list instead of blank space item
//                      - Resized property drop-downs
//                      - List keeps selection, preventing GPF in removals of list items
//          GetAlarmSummaryAck, GetEnrollmentSummaryAck
//                      - Default object ID and state
//                      - ID and state appear in list instead of empty space
//                      - Enabled transition checks - checks were previously always disabled 
//                      - List keeps selection, preventing GPF in removals of list items
//                      - Changed 'property' label to 'Event State' for proper field
//          ComplexObjectType
//                      - Default 'Present_Value' in list
//                      - Property appears in list instead of empty space
//                      - Disabled controls on dialog start when no items in list (formerly enabled)
//                      - List keeps selection, preventing GPF in removals of list items
//          VTSAny
//                      - List keeps selection, preventing GPF in removals of list items
//
//       511345, 511412:  Charstring handling
//
//       User was unable to see char string entered for passwords in DCC & Charstring was apparently
//       not sent to device in WP/WPM.  This problem was actually caused by the method for entering
//       char strings in edit boxes.  Single or double quotes were required within the edit box to
//       properly input the desired text for passwords, messages, names, etc..  Strings were using the
//       script parser to identify strings but this is not necessary in the UI.  Edit fields requiring 
//       strings throughout VTS have been altered accept what is entered.  If quotes are present, the quotes
//       are assumed to be part of the text and sent to the device in the relavent BACnet message.  This
//       change effects the following areas:
//
//          SendAckAlarm -> ack source
//          ConfirmedEventNotification -> message text
//          ConfirmedTextMessage -> character class, text message
//          DeviceCommunicationControl -> password (original bug report)
//          IHave -> object name
//          ReinitializeDevice -> password
//          Test? -> character string
//          UnconfirmedEventNotification -> message text
//          UnconfirmedTextMessage -> character class, text message
//          WhoHas -> object name
//          Any -> character string
//
//       Drop-downs for property values have been resized in various places to allow more than only a few
//       properties to be visible.
//
//       444134: Auto Segmentation Support
//
//       User can now select a device from the Send menu, or the port drop down menu.  There is a new 
//       Device page which is looking for a destination address (type, network, and address) and the 
//       outgoing APDU will be set to that address.  The device object will take care of segmentation,
//       retries, etc, as necessary.  Properly configuring a built-in device object and binding it to a 
//       port will be part of a separate document.
//
//       511406:  Status flags in a variable comp fails
//
//       Comparisons for status flags in RP/M, WP/M work properly now.  This problem was caused
//       by reading one byte passed the encoded PDU for the bitstring.  The comparison failed 
//       because the tag was being interpreted as part of the incoming bitstring.
//
//       508646:  rp to UTC_Offset fails in a script
//
//       Comparing the returned UTC_Offset value in a script to EPICS failed because the data type
//       used in VTS's internal structures for UTC_Offset was a float instead of int.  This has been
//       corrected by changing it's internal type to 'ssint' and processing accordingly.
//
//       508643:  VTS3 unable to read device-address-binding(RP, RPM)
//
//       'Property value not known' was returned correctly when comparing device-address-binding to EPICS when
//       the value was '?'.  It returned this error incorrectly when the EPICS value was defined as empty ().
//       It now compares the valid empty condition.  This error would also show up on other non-primitive data types
//       whose value could be empty ().
//
//       508640, Array bugs in rp (1E,1F,1G) 
//       
//       Case 1e has been fixed by creating a BACnet type class structure that will facilitate easier type
//       comparisons.  (See below).   Cases 1F and 1G are errors in the script parser that don't deal with array
//       indices.
//
//       Reworked implementation for comparing data in EXPECT statements from values retrieved from EPICS and
//       specified in scripts.   Any AL value being compared to EPICS data was using a method to match encoded
//       streams.  This scheme would fail in cases where numbers used different encoding sizes, different type
//       comparisons that are legal (such as float vs. null in priority arrays) and many other.  More detailed
//       comparisons were impossible with this method because VTS had no way of determining the type of data 
//       and could not determine the proper end of the stream, among other reasons.  
//
//       To implement type specific comparisons for all possible BACnet data types, including arrays, lists and 
//       complex structures, the c++ class representation for BACnet data types has been extended.  These 
//       classes serve as a foundation that allows the use of more tools (MFC, polymorphism, etc.) to 
//       facilitate feature additions and more flexibility where BACnet data type are concerned.  Type specific 
//       matching methods, display methods, decoding, assignment and comparative operators are now available.  
//
//       Using these classes to perform decoding and comparisons in the heart of EXPECT statements required 
//       the need for a retreval method to access the internal EPICS structures and convert them to BACnet 
//       type classes.   The immediate result of these changes can be seen in more meaningful text in test 
//       fail results.  In addition, comparisons can now be made to the following data types:
//
//          Unsigned values - all sizes.
//          Integer values - all sizes.
//          Real and doubles.
//          Priority arrays for floats, reals, enumerated and NULL choices.  Indexes not supported yet from 
//                   script parser yet but comparisons are ready.
//          Boolean.
//          Bit strings.
//          Object IDs.
//          Character strings - any size.
//          Enumerated values.
//          Date values - all comparison operators.
//          Time values - all comparison operators.
//          DateTime values - all comparison operators.
//          DateRange - all comparison operators (where applicable).
//          Calendar Entries and lists.
//          Device Address Bindings.
//          Array of object identifiers.
//          Arrays and lists of unsigned values.
//          Arrays of text for active/inactive.
//
//       The following type comparisons are not implemented yet:
//          List of read access specs.
//          Action commands and arrays of action commands.
//          VT classes.
//          Event parameters.
//          Session keys.
//          Time Synch recipients.
//          Recipients.
//          List of Recipients.
//          Array of exceptions schedule events.
//          Array of weekly schedules.
//          List of object property references.
//          Setpoint reference.
//          List of active VT sessions.
//
//       Adding support for these remaining types is simply a matter of constructing the object from the 
//       EPICS internal structures and decoding the object from a stream.  The mechanisms for performing the 
//       comparisons are already in place.  The development of support for proprietary objects in EPICS 
//       and scripts will be assisted by BACnet classes as well.
//
//       Other benefits of this work will be seen later as the EPICS store gradually converts to store 
//       data internally in these classes.  The greatest benefit to this foundation work will be seen as data 
//       becomes extractable from Complex Acks either compared, tested or even assigned to variables of complex
//       types in scripts.  Additionally, script parsing will be effected by these changes once all of the 
//       data types are supported and proper scanning methods for each will allow a single source code source 
//       for the creation of such objects.
//
//       Most of the SEND methods have been altered to retrieve EPICS data using these classes as well as 
//       creation and encoding of send streams from these object types.  Specifically, the following classes 
//       have been added or altered:
//          All BACnet classes enhanced with Runtime type declaratives, ToString and Match virtuals.  
//          Most classes now have constructors that take an APDUDecoder to construct the value from an 
//          encoded stream.  BACnetEncodeable now decends from CObject, added Match and ToString virtuals.  
//          BACnetAddr, BACnetUnsigned, BACnetCharacterString, BACnetBinaryPriV, BACnetCalendarArray, 
//          BACnetCalendarEntry, BACnetDate, BACnetDateTime, BACnetDateRange, BACnetTime, BACnetTimeStamp, 
//          BACnetWeekNDay, BACnetObjectIDList, BACnetGenericArray, BACnetObjectContainer, 
//          BACnetAnyValue (to facility "any" type flexibility - but mostly to encapsulate dynamically 
//          allocated objects for proper destruction when there's so much 'throwing' goings on...).
//
//       617618:  RPM MAX script crashes VTS
//
//       This problem was caused during an attempt to reallocate an APDU buffer to accomodate a larger
//       size APDU.  The problem has been fixed.  VTS no longer crashes.  Proper segmentation should occur.
//
//       508640:  Array bugs in rp(1E, 1F, 1G)
//
//       Cases 1F (test for size of array [0]) and 1G (test of single index value in array [int]) have
//       now been fixed.  Indexing into array properties is now supported in the following manner:
//             {property[int]}, {property[var]}
//       The index must either be a literal integer or a parameter which resolves to an integer.  Errors will
//       be reported upon execution otherwise.  Index references as properties, other indexed properties or even
//       indexes into parms that are arrays may be easily added in the future.  At present, specifications
//       such as these:
//             {property[{property}]}, {property[{property[int]}]}, etc.
//       are not supported.
//
//       618176: EXPECT needs "Don't Care" arguments
//
//       Expressions within an EXPECT statement now support the don't care operation.  Equations and values
//       can be specified as don't care in one of the following ways:
//
//          Method 1: Replace data in expression with '?',  Keyword = ?
//          EX:   Unsigned = ?, Bitstring = ?, PrimitiveType = ?
//
//          Data does not have to be valid for the given type.  '?' is also useful for replacing addresses
//          returned for EXPECT statements when applied to:  WriteBDT, BVLLResult, ReadBDTAck, ForwardedNPDU,
//          RegisterFD, ReadFDTAck, DeleteFDTEntry, WhoIsRouterToNetwork, IAmRouterToNetwork,
//          ICouldBeRouterToNetwork, RejectMsgToNetwork, RouterBusyToNetwork, RouterAvailToNetwork,
//          InitializeRoutingTable, EstablishConnectionToNetwork, DisconnectConnectionToNetwork.  
//          This format for don't care is not valid for Device responses and is also not valid for AL type data
//          where the datatype is determined by the EPICS reference on the right of the expression.
//
//          Method 2: Use of the don't care operator:  ?=,   Keyword ?= data
//          EX:  Unsigned ?= 20, Bitstring ?= F, T, AL ?= {EPICSReference}
//
//          Use of the don't care operator within an EXPECT statement will cause the data stream to be
//          parsed correctly according to the data type but the value will not be tested.  This operator
//          works on primitive types (Unsigned ?= 20) as well as the AL data.  The difference here is that
//          the value on the right of the operator must be valid for the datatype.  EPICS references for AL
//          expressions must be valid in order to determine the data stream type.  Exeptions to this are 
//          expressions where both the ?= operator AND the ? don't care data case both be used (although
//          redundant):  Bitstring ?= ?.  An advantage to using the operator is in keywords.  Object IDs can
//          be ignored (Object ?= 0, OBJECTVAR), and the like.
//
//          Date / Time improvements
//
//          Several changes have taken place in the handling of the primitive Date and Time data types.
//          Unless noted, both the Date and Time data type  handling were affected equally by
//          these corrections and enhancements.
//
//          - The Date and Time data types were not accepting a script variable substitution when used
//            in Send and Expect statements.  Full support for receiving Date and Time values from
//            variables has been added.
//          - The Date and Time data types now compare don't care ('?', '*') values properly.
//          - Added support for the don't care value that other data types recently support.
//          - Date and Time values specified in EPICS that had don't care values would not be
//            allowed in references. This restriction has been removed and now supports all
//            combinations of don't care specifications (?/?/?).
//          - Full comparison support has been added to the Date and Time primitive types. 
//          - Various parsing problems in Date/Time have been fixed.
//
//          Text representations of complex data types now require the use of brackets '[]' to separate
//          their data from other parameters.  The brackets are required even if no other parameters
//          in an expression are present.  The Date and Time text representations now take the following
//          form:
//                Date = [dow, mm/dd/yy]     ( '-' can also be used to separate date values)
//                Time = [hh:mm:ss.HH]
//                Where:
//                      dow = 3 characters (case insensitive) for MON, TUE, WED, THU, FRI, SAT, SUN.
//                           This value must be present and can be substituted with ? as in
//                           [?, mm/dd/yy].  A comma separates this value and leading and trailing
//                           spaces between each atomic type are ignored 
//                           (i.e. [?   ,   mm  /  dd  / yy] is the same as [?,mm/dd/yy]).
//                      mm, dd, yy, hh, mm, ss, HH = numeric values with leading or no zeros.
//                             Values are checked for validity in every case except invalid dates as
//                           a whole (Feb 31st).
//                      yy = Year specification given as either the full year (2002) or a partial
//                           year (02).  Values interpreted as partial years range from 1941 to 2040.
//                      HH =  hundredth value in the range 0 - 99.  This value is optional in the text
//                           representation.  If the period is present in the text, the value MUST follow
//                           or a parsing error is generated and the Time is invalid.
//
//          Any and all values can be substituted with the don't care value ?.   [12:?:44.?]
//          Enclosing these complex representations inside brackets gives clarity to expressions with
//          multiple parameters as in the case of tag specifications:  Date = tag, [TUE, 11/4/02]
//          -or-  Date = tag, VAR  where VAR = [TUE, 11/4/02].   Bracketing this compound data can be
//          nested as in the case of the text representation for a DateTime and DateRange:
//                DateTime = [DATE, TIME] = [[dow, mm/dd/yy], [hh:mm:ss]]
//                DateRange = [DATE1, DATE2] = [[dow, mm/dd/yy], [dow, mm/dd/yy]]
//
//          DateTime and DateRange are not considered primitive types and currently can only be used
//          when referencing EPICS data and so a text representation for these types has not been needed
//          as yet.  Like quotes for strings, the brackets are only necessary in situations where other
//          types of data may be read as well.  Brackets are not necessary when date and time values
//          are specified inside edit fields within the VTS user interface. 
//          They are, however, necessary in scripts and subsequently, like quotes, necessary inside
//          script parameter data, since this data is partially parsed.
//
//          [444200] Extract data from incoming packets
//
//          Support for stuffing incoming data elements into script variables has been added throughout
//          the EXPECT statement, except where noted.  Syntax for the new operator '>>' in assignments
//          take the following form:
//                Keyword >> VAR
//                Keyword >> tag, VAR
//          This will place a text representation of the incoming data given by the keyword into the
//          script parameter VAR.  The VAR parameter MUST be previously created in the SETUP section
//          of the script or an error will be reported.
//
//          Existing support for other keywords is as follows:
//             Object >> tag, VAR   Assigns:  OBJECT-TYPE, Number
//             Object >> VAR
//               This assignment will place the object type keyword AND the object instance number
//            inside the script parameter value. 
//             Device >> VAR     Assigns:  DEVICE, Number
//            This assignment will also place the keyword 'DEVICE', followed by a comma, and the
//            instance number inside the script variable.  Brackets will not accompany the data.
//             Property >> tag, VAR Assigns:  Number or PropertyKeyword
//            The property keyword (i.e. PRESENT_VALUE) will be placed inside the variable. 
//            This will be properly decoded and compared in other equality expressions in the current
//            or future EXPECT statements.
//
//          Special notes about other keywords:
//          MAXSIZE >> VAR    Assigns: Enumeration of ("50", "128", "206", "480", "1024", "1476")  or Number if < 16.
//                 Variable supplied as number, converts to code before SEND.
//          NETWORK >> VAR    Assigns: CharacterString for currently defined port (in quotes).
//          BVLCI >> VAR
//             where type BVLCI type is: READ-BROADCAST-DISTRIBUTION-TABLE-ACK {,host/net:port}*
//          MESSAGE >> VAR, ?, ?, ?
//             These keywords have parameters that follow which dictate how the values are extracted
//             from the received packet.   The parameters that follows must still accompany the expression.
//
//          An error was discovered and corrected concerning the specification of DNET/DADR and SNET/SADR
//          pairs in the script.  The test for whether or not these pairs were present in the received
//          packet was being determined by whether or not the DNET/DADR keywords were supplied in the
//          script.
//
//  3.1.10     Date / Time handling correction for '*'
//
//          Added support in recent changes for Date/Time handling to make a distinction between the
//          '?' value (matches to anything for equality) and the '*' value (MUST match the anything value).   
//          Comparisons were affected in the following ways:
//
//             '?' appearing in any field means that field (m/d/y, dow) or (h:m:s.h) will match with
//             any other value in a comparison.  <, > tests will result in failure for that piece of
//             the date/time - which will effect the entire comparison.
//
//             '*' appearing in any field means that the field it's matching to MUST be either '*' or '?'.
//             
//  3.2.0      [618172] Conditional flow control for scripts
//
//          Added conditional script compilation with use of IFDEF type keyword.  IF is a pre-processor
//          conditional used during the "Syntax Check" function.  The expression
//          in the conditional will not be evaluated at script execution.  Syntax is as follows:
//
//             IF (expression {[OR | AND] expression} )
//                .
//             {ELSEIF (expression {[OR | AND] expression} )}
//                .
//             {ELSE} 
//                .
//             ENDIF
//
//          The pre-processor will evaluate the expressions contained within the perenthesis following
//          the 'IF' keyword before scanning the next line.  If the expression evaluates to true, all
//          of the following lines up to the ENDIF keyword (or ELSEIF or ELSE, if supplied) will be
//          included in the compile.  Multiple expressions can be evaluated.  Evaluation will occur left
//          to right until a false condition is detected.
//
//          See Sourceforge item 618172 for details regarding usage of the IF statement.
//
//          [44152] New CHECK statement
//
//          Added CHECK statement to script commands that will popup a modal dialog box and allow the user
//          to PASS or FAIL the script test.  Script execution is halted while the dialog is up.  Syntax
//          for the CHECK statement is as follows:
//
//          CHECK "Title" (
//             "Text line"
//             ...
//             "Text line"
//             )
//
//          Usage details can be found in a usage document attached to the sourceforge item 44153 for MAKE.
//
//          [44153] New non-modal MAKE statement
//
//          Added MAKE statement to script commands.  It will pop up a dialog with a supplied message and
//          allow the user to alter a condition that generates a receive packet.  When followed by an
//          EXPECT statement, script execution continues with indefinately extended EXPECT timers.  Syntax
//          is exactly like the CHECK statement with the exception of the keyword MAKE.  Usage details can
//          be found in the document attached to the sourceforge item.
//
//          
//  3.2.1   [567046] 
//          Now Detail pane has been changed into treelist pane, we expanded some nodes by 
//          default to display the informations that we think they are very important.
//
// 3.3      [485733] Add RP/WP support to internal device objects.  Minor version number changed to reflect
//       change to the database (the device descriptor now has an ID of a list of object/property/value 
//       components.
//
//          [626601] Crashes when openning a read-only VDB
//        A message is now returned when an attempt to open a read-only VDB is made.
//
//          Fixed all of the child popups decending from the Send dialog to chain properly.  Each (tedious) 
//          ocurrance was altered to report to the proper parent such that all of the popups are now modal
//          to the modeless Send dialog.  Multiple Send dialogs can now be up simultaneously.
//
//       Trapped corrupted and inconsistent database opens, report the problem and control document close.
//
// 3.4      - Removed internal database and replaced with workspace files (.cfg).
//       - Replaced BACMACNT driver with WinPcap library calls.  No changes to the database format, just the
//       - Workspaces menu added to main menu.
//       - VTS can switch workspaces without shutting down.
//       - Only one workspace can be loaded at once.
//       - VTS will open the most recently used workspace file upon VTS start.
//       - Menu items have been changed to reflect workspace handling (on separate menu) and document handling (on main File menu).
//       - VTS will not associate .VDB files with VTS upon startup anymore.
//       - Scripts are opened in a separate frame window with a menu and button bar appropriate for script handling.
//       - VTS main window remembers packet view column sizes.
//       - Packet summary window now has timestamp and port name columns. 
//       - Button bar has been expanded on main VTS packet view and menu as been trimmed to commands relative to main VTS (not scripts).
//       - Port names on Send menu are grayed if inactive and show port type.
//       - Refresh added to reload packet view.  This is useful if you have changed name/address mappings.
//       - Revamped 'Export' function.
//       - Added progress bar on status bar that shows during lengthy operations (loading large packet files, exporting, etc.).
//       - Changed main list view to handle virtually unlimited number of packets.
//       - Delete all packets reduces packet file to zero and does not affect configuration file.
//       - VTS can switch packet files via Change Packet file menu item.
//       - Changed port configuration for usability. 
//       - Changed device configuration for usability.
//       - Added ability to edit object, property and values to devices.
//       - Added ReadProperty and WriteProperty support to devices.
//       - Added automatic RP and WP to parameters of configured devices.
//       - Changed names configuration for usability.
//       - VTS had a registry problem confusing an INI path with key names.
//       - Added preferences dialog.  Preferences are stored in "Setting" subkey inside registry.
//       - Added preference setting for specification of number of slots used for main packet view cache.
//       - Added timeout for autoscroll.
//       - Added preference setting for number of seconds auto-scroll is to remain inactive.
//       - Added preference for relative vs. absolute packet file path association in workspace.
//
//
//  3.4.1
//       [ 747747 ] EPICS loading problems:
//       Fixed this problem loading bitstrings larger than one byte. 
//       Additional EPICS loading problems have been fixed:
//       
//          - VTS would cause an exception when property names were not found in AVG, Trend-Log and
//            Multistate Value objects.
//          - VTS would not properly recognize property names if comments followed on the same line.
//          - EPICS parser was incorrectly looking for the following in Supported Services and Objects: 
//                "Read Range"  -> now changed to "ReadRange"
//                "UTC-Time-Synchronization"  -> now changed to "UTCTimeSynchronization"
//                "Trend-Log"  -> now changed to "Trend Log"
//                "trendlog"  -> now changed to "Trend-Log" for object type specification
//          - Fixed VTS to skip consistency check (and subsequent dialog box) if user cancels at
//            any encountered parsing error.
//          - Fixed EPICS parsing error notifications to parent script window properly.
//          - Fixed problem where EPICS wouldn't load again if user canceled a prior EPICS load attempt
//            without without restarting VTS.
//          - Fixed EPICS parser to barf on malformed object property reference in list-of-object-property-reference.
//            If delimeter is not present, parsing notifies with an error instead of continuing and
//            subsequently attempts to recognize extraneous garbage as property names.
//          - Fixed whitespace trim from right function - which never worked correctly.  This may fix
//            spurrious recognition problems throughout parsing.
//
//       Added option in preferences for verification on delete packets.  Defaults to true.
//
//
//  3.4.2
//       [681223] - Changed the default delay in EXPECT and SEND packets to 3 seconds and changed the maximum
//       delay to 2 minutes.  The default and max were previously set to 1.8 seconds, which was a bug.
//
//       Added support to parse "Event-Time-Stamps" property.  Unfortunately this is not yet a property
//       that can be handled inside a script... It only parses EPICS correctly for these objects:
//          - MSI, MSO, MSV, AI, AO, AV, BI, BO, EE, LOOP, BV, BV
//          - TR object support already existed, but it was the only one.
//
//       [618217] - INCLUDE directive added to scripts.  Syntax for including files:
//
//             INCLUDE "filename.vts"
//
//          Usage of the INCLUDE directive follows these rules:
//             - The INCLUDE keyword must be the first item on a line (excluding whitespace) and is 
//               followed by a filename to include in quotes.
//             - The extension for included files is the same as script files.
//             - Any number of files can be included.
//             - Included files can contain nested includes.
//             - The file name should include a relative path that is always relative to the location
//               of the file that contains the include statement.  This is also true for each included
//               file.  This means that file specifications included from the base file will be
//               relative to the base file but files that are included from an include file will be
//               relative to the location of the include file.  For example:  If included files are
//               in a subdirectory called 'inc', only the base file, which is located one directory up
//               from 'inc' will have the statement:  INCLUDE "inc\file1.vts".
//               If file1.vts contains include statements that pull in files from the inc directory as
//               well, the statement will be:  INCLUDE "file2.vts".
//             - Absolute paths may be used as well if specified properly (C:\, \, etc.)
//             - INCLUDE statements can occur anywhere within the script file with the exception of 
//               inside statements.  This means that they cannot appear within SEND or EXPECT
//               statements even though these statements may span several lines.  CHECK, MAKE and
//               other multi-line statements are part of this restriction.
//             - Circular includes are not allowed.
//             - If variables are defined and initialized in included files that are already
//               defined in the base file, the values will be overridden with the last encountered declaration.
//
//          There are a few things to note when files are included:
//             - Include files are opened and parsed during the "Check Syntax" command.
//             - When errors are encountered within an included file during parsing, an error message 
//               will present (as usual) and the line and text that produced the error will be 
//               highlighted in a separate editing window that allows the imediate editing and 
//               saving of the included file.  This window is modal and parsing execution halts.
//               A quick correction followed by another Syntax Check will have you on your way.
//             - Nodes created on the tree (following a successful syntax check) from include files
//               will appear in blue text.  Clicking once on these nodes will display the fully
//               qualified path to the containing include file in the editor's status bar.  Double
//               clicking on these nodes will open a modal editing window and the statement's line
//               will be highlighted.  This editing window will allow the file to be saved (no SaveAs).
//
//       A bug was fixed in the script editor that would not initially place the caret on the 
//       line if goto was used.
//
//       Moved Load EPICS command from Script frame to main frame under "EPICS" menu.  This menu will be populated
//       with other EPICS type functions in future versions.
//
//       Altered EPICS load results dialog to use static windows (instead of read only edit boxes) to do automatic
//       word wrap.
//       
//       Added "Load last EPICS on startup" to preferences section.
//
//       Fixed a bug in the specification of the EPICS file to load.  It was using the filename only, even if
//       the file specified from the file dialog found the file in other directories.
//
//       Moved "Read All Properties" command to main window menu.  Revamped generation of script file for reading
//       all properties (bug not generating last object RP).
//          - The filename "ReadAllProperties.vts" will be generated when this command is given.  The file will reside
//            in the current working directory and will not be deleted.
//          - This file will be overriden when the command is performed a second time.
//          - The user may edit the vts file as any other script and perform SaveAs to retain the generated file.
//
//       Fixed a GPF when recompiling script after prior compile.
//
//       [608821] - Very important error in EXPECT
//       Finally got a handle on all of the issues surrounding errors herein:
//
//       Fixed an error in the script executor to throw an error, halting execution, if neither a network layer
//       message (MESSAGE = ) was defined in EXPECT or an application layer message (PDU =) was defined.  Scripts now
//       force one of these statements within an EXPECT clause.
//
//       Fixed an error that prevented the ability to stuff a received value into the PDU field with the assignment
//       operator.  PDU >> VAR now works just fine.  Before the fix, the MESSAGE statement had to be an assignment as
//       well as the PDU statement, which was impossible because both statements cannot occur at once.
//
//       Fixed an error that never evaluated DER statement against incoming packet.  The DER = TRUE statement ALWAYS
//       failed and the DER = FALSE statement ALWAYS passed.  Now the DER statement is evaluated properly.
//
//       Fixed an error that never evaluated the PRIORITY statement properly.  Priority was always tested again zero.
//       Now the test is performed by examining the statements value against the incomming PRIORITY.
//
//       Fixed an error that prematurely passed an address evaluation with the minimum of the address lengths
//       (between the incoming SADR and the script SADR) used in the comparison.  Now the two addresses must match
//       exactly.
//
//       Added the ability to specify SNET/SADR and DNET/DADR pairs in an EXPECT statement with a NONE value that
//       indicates the received packet must not have SNET/SADR or DNET/DADR values.  In addition, if these statements
//       are present in an EXPECT clause the received packet MUST contain the values.  Previously the test was
//       only evaluated if the received message had the values.  Usage is as follows:
//          SNET = NONE, SADR = NONE, DNET = NONE, DADR = NONE.
//       So now if the SNET/SADR or DNET/DADR values are specified in the script, the values must exist in the 
//       received message.  Conversely, if they are specified in the script with the NONE value, the src/dst addresses
//       must NOT be present in the received packet.
//
//       New tests have been added in the script executor that force the reserved bits in each PDU control octet to 
//       be zero.  An error is reported if they are not.  Bits 6 and 4 are tested for network layer messages and application
//       layer messages have different reserved bits for each PDU type.
//
//  3.4.3   Capture and display filters added, PTP unfinished.  Caputure filters changed what is saved in the 
//       database, but not what is otherwise processed by the application.
//
//       The capture and display filters are a list of ACCEPT or REJECT filter elements,
//       each of which can specify a variety of source and destination addresses, as well
//       as "function groups" that match the function groups in the Send menu.
//
//       Addresses are "deep parsed", so you can filter on anything to and/or from a remote
//       network because the filter will look into the NPCI for routing information. Ack's
//       to a confirmed service (both simple acks and complex acks) are paired up with the
//       function group.
//
//       Note that this does not change the behavior of the application, only what is saved
//       in the packet database and/or shown in the window. For this version, script
//       generated "packets" are not subject to filtering.
//
//       (The up and down arrow buttons to change the order of the filter elements are not
//       working).
//
//
//
//       Bug 567046: Reworked the Detail View to collapse tag details and expand the
//       application data by default. This should make it easier to use the Detail View to
//       spot the most important data quickly.
//
//
//       Feature request 444130: The Property Identifier selection list is now context
//       sensitive to the Object Type selected. The Properties at the top of the list are
//       the standard Properties for the associated Object. These are followed by a string
//       of dashes (---------) to serve as a visual separator, and then the remaining
//       BACnet Properties are listed. At the end of the list is a Property named "", which
//       allows the user to enter a numerical Property Identifier for proprietary
//       Properties.
//
//       Proprietary Property Identifiers are now encoded and decoded correctly and shown
//       as numerical Property Identifiers, but the value of the proprietary Property will
//       not be decoded correctly if it is a constructed data type.
//
//       Property Identifiers are no longer restricted to the first 123 enumerations.
//       Property Identifiers up to enumeration 168 (profile-name) are now decoded.
//       Property Identifiers can now be up to enumeration 4194303. (This was limited to
//       two octets in prior versions). Enumerations higher than 168 are shown as numerical
//       Property Identifiers.
//
//
//       Feature request 544511: More information was added to each row of the summary view
//       in the sniffer. This feature needs to be reviewed to see if the most useful data
//       was added for each message type.
//
//       Fixed the following bugs:
//
//          727597: Commas needed in bacfuncs.h. The strings "SCHEDULE" and "AVERAGING"
//          are now separated.
//          787626: EPICS parser does not know about new properties. The following
//          Properties were added:
//             ProfileName (all objects)
//             Device.database-revision
//             Device.configuration-files
//             Device.last-restore-time
//             Device.backup-failure-timeout
//             Device.active-cov-subscriptions
//             Device.max-segments-accepted
//          779139: Send Dialog missing vertical scroll bar
//          788953: Send UTCTimeSynchronization service is bad
//          767301: UTCTimeSynchronization has extra fields
//          759946: Object_Property_Reference property mislabelled
//          767311: Signed values displayed as unsigned in Detail view
//          787629: EPICS parser requires optional properties. The EPICS parser was
//          generating an error when a) the device supports Event Notification services
//          and b) any object that COULD support intrinsic alarming doesn't have the
//          properties to do so (time-delay, notification-class, etc). Now the EPICS
//          parser checks to see if ANY of the intrinsic reporting properties exist in
//          each Object, and if so, they all must exist.
//
//  3.4.4     January 2004
//
//       
//       Feature Request 837460: Provide a method for failing a test script if a particular message
//       is received. (This is essentially a negative form of the EXPECT statement). An additional
//       parameter has been added in the EXPECT statement to negate the expected condition. The
//       syntax for this for of an EXPECT statement is:
//             EXPECT NOT
//       
//       or 
//             EXPECT BEFORE xxx NOT
//
//       Example: 
//         EXPECT BEFORE 200 NOT (
//           NETWORK = "Untitled"
//           SA = IUT_IP
//           DER = FALSE
//           BVLCI = ORIGINAL-UNICAST-NPDU 
//           PDU = ComplexAck
//           Service = ReadProperty
//           Object = 0, OBJECT1
//           Property = 1, PRESENT-VALUE
//           OpenTag 3
//             REAL = LOWVAL                
//           CloseTag 3
//         )
//       
//       If VTS receives the specified packet in the given time then this test fails. 
//       
//       
//       Feature Request 837459: It was requested that scripts should cause a syntax error if an
//       unrecognized keyword is encountered rather than assuming that the unrecognized keyword was
//       an uninitialized variable. VTS now checks all of the keywords in the script file, and
//       triggers an error message if a keyword is not a keyword predefined in VTS.
//       
//       
//       Bug 827465: VTS now recognizes the Device object's Max_Segments_Accepted property - both
//       in the interactive portion of VTS as well as in scripts.
//       
//       
//       Bug 843232: Now the default time delay in scripts is assigned by the variable 'Internal
//       Processing Fail Time' if this variable has been defined in the EPICS, otherwise the
//       default time delay is still 3 seconds.
//       
//       
//       Feature Request 843336: An ASSIGN statement has been added to the script commands so that
//       a script can explicitly set the test result to PASSED or FAILED based on scripted
//       conditionals instead of being dependent on failed EXPECT statements. For example:
//          IF (timeRemaining = 0)
//          ASSIGN FAILED
//          ENDIF
//       
//       
//
//       Feature Request 843292: A WAIT statement has been added to the script commands to provide
//       a method for inserting an unconditional wait into a script. The syntax for the WAIT
//       statement is described by:
//         <wait statement> ::= WAIT <timer value>
//       
//         "The TD shall pause the amount of time specified by the 
//         <timer value> before proceeding to the next test step.
//         The <timer value> shall be one of the timers specified in
//         6.3 or as otherwise specified."
//       
//         <timer value> ::= <constant> | <variable> | '{' <Fail Time> '}'
//       
//         <Fail Time> ::= Notification Fail Time |
//                         Internal Processing Fail Time | 
//                         Minimum ON/OFF Time | 
//                         Schedule Evaluation Fail Time | 
//                         External Command Fail Time | 
//                         Program Object State Change Fail Time | 
//                         Acknowledgement Fail Time
//
//       
//       
//       Feature Request 843302: Within a script window, the user can now select which SECTION to
//       run, as well as which test within that SECTION.
//       
//       
//       Feature Request 843304: VTS scripts now have a sort of "auto-complete" feature that will
//       automatically type the remaining characters of some keywords when the script writer begins
//       typeing a recognized keyword. This facility was implemented in lieu of the drop down menu
//       for common script parameters that was requested.
//
//       
//       Bug 843245: VTS can now distinguish between the global broadcast IP address and the local
//       broadcast IP address.
//       
//       
//       Feature Request 843288: VTS scripts now support the following format for testing if the
//       IUT supports a particular BACnet service as defined in the loaded EPICS file:
//         IF '(' '{'<service>', ' EXECUTE | INITIATE'}' '=' TRUE | FALSE')'
//       
//
//       
//       Feature Request 843284: In scripts, now EPICS references can be supported in conjunction
//       with delays indicated by the keywords AFTER and BEFORE. Command syntax:
//          SEND AFTER '{'<Fail Time>'}'
//       
//          <Fail Time> ::= Notification Fail Time |
//                          Internal Processing Fail Time | 
//                          Minimum ON/OFF Time | 
//                          Schedule Evaluation Fail Time | 
//                          External Command Fail Time | 
//                          Program Object State Change Fail Time | 
//                          Acknowledgement Fail Time
//
//       
//          Example: 
//
//          SEND AFTER {Internal Processing Fail Time}(
//          ...
//          )
//       
//
//       
//       Feature Request 843265: Added support for the MAXSEGS keyword in scripts. 
//       
//
//       Feature Request 843281: In scripts, the following EPICS references are now supported: 
//         {Max_APDU_Length_Accepted}
//         {Max_Segments_Accepted}
//       
//       Examples: 
//          MAXSEGS = {Max_Segments_Accepted}
//          MAXRESP = {Max_APDU_Length_Accepted}
//       
//       
//       
//       Feature Request 843274: EPICS references are now allowed within IF expressions in a
//       script. Examples:
//         IF ({binary-value, 1, OUT-OF-SERVICE} = FALSE)
//           OBJECT1 = binary-value, 1 
//         ELSE
//           OBJECT1 = binary-value, 2 
//         ENDIF
//       
//         OBJECT = binary-value, 1 
//         IF ({OBJECT, OUT-OF-SERVICE} = FALSE)
//           OBJECT1 = binary-value, 2 
//         ELSE
//           OBJECT1 = binary-value, 3 
//         ENDIF
//       
//       
//       
//       Bug 843243: This bug says that the don't care value '?' is not allowed to be assigned to
//       the invokeID parameter within an EXPECT statement in a script. This is true, but the user
//       could have used the don't care operator to achieve the desired results. For example:
//          EXPECT (
//             ...
//             PDU = Confirmed-Request
//             ...
//             invokeID = ?
//             ...
//          )
//       
//       Does not work as a don't care operation on the invokeID, but this does ... 
//          EXPECT (
//             ...
//             PDU = Confirmed-Request
//             ...
//             invokeID ?= 0
//             ...
//          )
//       
//       However, VTS has been changed so that the first format now also works as a don't care
//       operation on the invokeID in an EXPECT statement.
//       
//
//       Bug 843258: If an ENDIF statement was missing in a script, VTS would crash. This bug has
//       been fixed.
//       
//       
//       Bug 843252: This bug states that the CHECK statement requires that the open parenthesis
//       must be followed by a carriage return. In other words, the following is not allowed:
//        CHECK "Title" (IF (VAR = 1)
//                   "Text"
//          ELSE
//                   "Text2"
//                 ENDIF
//        )
//       
//       But the following syntax is allowed:
//          CHECK "Title" (
//             IF (VAR = 1)
//                      "Text"
//                    ELSE
//                      "Text2"
//             ENDIF
//          )
//       
//       The SEND and EXPECT statements do not support the first syntax either. Since the
//       work-around is simple, and since other statements must be changed if the CHECK statement
//       was changed, there are no plans to fix this bug.
//
// 3.4.5    April, 2004 ====================
//    
//       Added MS/TP support through American Automatrix NB-Link IP device.
//       VTS supports multiple NB-Link devices, once for each MS/TP network VTS is connected to.
//       Configure the device by adding the NS/TP port type in the ports dialog.
//          The dlg allows communication with the device by either setting parameters manually in 
//          the dialog or by setting the parameters and having VTS check and set them when the ports
//          startup (set on if NB-Link parameters from the device are not as expected upon query).
//
//       Secondary implementations of the MS/TP port can be added by subclassing the MSTPImplementation
//       class in the WinMSTP.h  header (which defines an MS/TP implementation API) and adding 
//       a configuration dialog specific to a new implementation.  Any new implementations can be
//       modeled after the MSTPImp_NBLink class in the WinMSTP.cpp file.
//
//       The NB-Link device searches for a TCP/IP connection on its specific remote IP:Port every 2 seconds
//       until VTS receives the connection when its port opens.  The port status will indicate proper
//       connection status and serious port error messages will be written during script execution if the 
//       MS/TP port encounters errors.
//
// 3.4.6    July, 2004 ====================
//
//         # The interface to the NB-Link MS/TP device was modified to improve performance.
//         # Send windows can now be minimized.
//         # Bug 974701 & 996723: Fixed a problem with VTS crashing if the network adapter name exceeded 64 characters.
//         # Bug 444179:  Added a "Send & Close" button to the Send dialog box that causes the Send dialog to close immediately upon sending the message.
//         # Bug 799217:  Now VTS can automatically send a BACnet SegmentACK, if the current active port has been bound to a device.  So when using VTS to send a RP/RPM for a property that will force the IUT to send a segmented response, if the IUT does send a segmented response, then VTS will automatically respond with BACnet SegmentACK packets as needed to receive the entire segmented response.
//         # Bug 895889:  When a script referenced a timeout specified in EPICS, the script interpretted the value as seconds instead of milliseconds.  Now VTS understands that the timeouts specified in the EPICS are specified in milliseconds, and it converts these values into seconds for the script executor.
//         # Bug 899927:  VTS now properly accepts the section title "BACnet Object Types Supported:" in an EPICS file.
//         # Bug 791999: In the EPICS file, the Total_Record_Count property can now accept large numerical values.
//         # Bug 922818: Fixed a problem with VTS crashing during the EPICS consistency check.
//         # Changes to conform to 135-2001 addendum b:
//
//          * Feature request 950150:  Awareness of the BUFFER_READY Event Type (context 10) was added.
//          * Feature request 950159: The decoder for the ConfirmedEventNotification service has been enhanced to decode the new context 10 (buffer-ready) production of the BACnetNotificationParameters, and the sniffer and the script executor also have been updated to make the new variables in the context 10 production available to the script via a RECEIVE (EXPECT) statement.
//          * Feature request 950145: ReadRange was modified for 2001b: 
//                o The 'By Time' option for the 'Range' parameter produces a context 7 encoding, not context 4
//                o The 'Time Range' option for the 'Range' parameter has been removed.
//                o A new 'By Sequence Number' option for the 'Range' parameter was added that produces a context 6 encoding, which uses the "referenceIndex" and "count" parameters.
//                o The two reference time parameters in the ReadRange dialog have been converted into a single Reference time parameter named 'Reference Time'.
//                o Decoding of ReadRange-ACK was modified to expect the optional 'firstSequenceNumber' parameter, which is context 6, of datatype unsigned32.
//          * Feature request 950136: TrendLog Object was modified to conform to 135-2001 addendum b:
//                o Removed Previous_Notify_Time & Current_Notify_Time properties.
//                o Added Last_Notify_Record property.
//
//      # Feature request 950059: Added support for the GetInformation service.
//      # Feature request 444242: Added custom dialog boxes (for use in the Send window) for entering the following complex datatypes:
//
//         1. BACnetAction
//         2. BACnetActionCommand
//         3. BACnetActionList
//         4. BACnetBinaryPV
//         5. BACnetClientCOV
//         6. BACnetCOVSubscription
//         7. BACnetDailySchedule
//         8. BACnetDaysOfWeek
//         9. BACnetDeviceStatus
//        10. BACnetDeviceObjectPropertyReference
//        11. BACnetDeviceObjectReference
//        12. BACnetEngineeringUnits
//        13. BACnetEventParameter
//        14. BACnetEventState
//        15. BACnetEventTransitionBits
//        16. BACnetEventType
//        17. BACnetFileAccessMethod
//        18. BACnetLifeSafetyMode
//        19. BACnetLifeSafetyOperation
//        20. BACnetLifeSafetyState
//        21. BACnetLimitEnable
//        22. BACnetLogRecord
//        23. BACnetLogStatus
//        24. BACnetMaintenance
//        25. BACnetNotifyType
//        26. BACnetObjectPropertyReference
//        27. BACnetObjectPropertyValue
//        28. BACnetObjectType
//        29. BACnetObectTypesSupported
//        30. BACnetPolarity
//        31. BACnetPriorityArray
//        32. BACnetPriorityValue
//        33. BACnetProgramError
//        34. BACnetProgramRequest
//        35. BACnetProgramState
//        36. BACnetPropertyIdentifier
//        37. BACnetPropertyReference
//        38. BACnetPropertyStates
//        39. BACnetPropertyValue
//        40. BACnetRecipient
//        41. BACnetRecipientProcess
//        42. BACnetReliability
//        43. BACnetResultFlags
//        44. BACnetSegmentation
//        45. BACnetServicesSupported
//        46. BACnetSessionKey
//        47. BACnetSetpointReference
//        48. BACnetSilenceState
//        49. BACnetSpecialEvent
//        50. BACnetStatusFlags
//        51. BACnetTimeStamp
//        52. BACnetTimeValue
//        53. BACnetVTClass
//        54. BACnetVTSession
//        55. BACnetWeekNDay
//        56. ListOfBACnetDestination
//        57. ListOfBACnetReadAccessResult
//        58. ListOfBACnetReadAccessSpecification
//
// 3.4.7    September, 2004 ====================
//      Added the EPICS view.
//      Upgraded EPICS functionality to the level of 135.1-2003.
//      Rewrote EPICS consistency tests.

//      # Bug 799851: Fixed Script line numbers off by one in VTS messages
// ScriptBase.cpp has a class member named scanLine. That member is file local;
// there are no references to it outside the file. It was being initialized to
// -1 in ScriptBase::Initialize(). I changed it to initialize to 0 in
// ScriptBase::Initialize(), and I moved the location of where scanLine += 1;
// executes, so that other data structures which are indexed by scanLine are
// unaffected. They formerly executed after scanLine += 1; which had started at
// -1, now they execute before scanLine += 1; but scanLine starts at 0.

//  3.4.8   July 6, 2005
//        Sniffer improvements were made
//        The following bugs were fixed:
//        1069018, 1067771, 1021196, 1006602, 
//        799851, 485728, 895879
//        The following feature requests were closed:
//        544511, 544324, 508589, 444238
         
//  3.4.9    November 21, 2005
//         EPICS parsing improvements made
//         Column resizing fixed
//         Backup and Restore tests are now included
//   The following bugs were closed:
//         1275709, 1261344, 1260152, 1260136, 1259984,
//         1256856, 1254028, 1244867, 1190196, 1167286,
//         1155180, 1149504, 1149490, 1056200, 1031010,
//         1031009, 1031004, 1029342, 974907, 787623 
//         966851, 974991
//   The following feature requests were closed:
//        1259864, 996131

//  3.4.10     October 21, 2006
//         General Bug Fixes
//   The following bugs were closed:
//           617212,  960274,  965935,  960275,
//           960273, 1540412, 1538594, 1501806,
//          1486033, 1461695, 1452771, 1440025,
//          1431140, 1431111, 1400472, 1380510,
//          1264041, 1256830, 1025962, 1021192,
//       1021191,  806427,  787641,  779138,
//        779132, 1565943
//
//  3.4.10.a     November 21, 2006
//  Backup and Restore Test fixes.  This includes changes to the reinit dialog.
//  There are also changes made for stability and use in VC2005 projects.  These changes
//  were supplied by JCI (Glen).  The only change not applied was in the BakRestoreExecutor.cpp.
//  
//
//  Files changes are:
//      BakRestoreExecutor.cpp/h, ChildFrm.cpp, DetailTreeCtrl.cpp,
//      MainFrm.cpp, resource.h, ScriptBase.cpp/h, ScriptEditIncludeDlg.cpp,
//      Send.cpp/h, SendReinitDevice.cpp/h, sizecbar.cpp, StdAfx.h,
//      VTS.rc, VTSBackupRestoreDlg.cpp/h, VTSBackupRestoreProgress.cpp/h, 
//      VTSCtrl.cpp, VTSDB.cpp/h, VTSDoc.cpp, VTSPacket.cpp, VTSPacketDB.cpp/h,
//      VTSPortIPDialog.cpp/h, VTSPortMSTPDialog.cpp
//
//  3.5.1      July 14, 2008
//
//    Several items were fixed in the Backup and Restore procedures.
//    AtomicWriteFile decoding and display was fixed.
//    ReinitializeDevice was fixed to show enumeration correctly.
//    Added several new properties and structures.
//    Added EventType handling for: UnsignedRange, Extended, and ChangeOfLifeSafety.
//    Fixed CreateObject dialog
//    Fixed EventEnrollment dialog
//    Updated EPICS Consistency for new workstation BIBBs (Addendum L)
//    EPICS now accepts 1476.
//    ReadAllProperties script creation now produces correct syntax.
//    VTS Server now supports ReinitializeDevice and GetEventInformation.
//    VTS Server now responds to most required device properties.
//    VTS Server creates Object_List and Object_Type based on objects defined in the VTS Device.
//    Started work on import and export of VTS device objects.  Not Complete.
//    Changed several dialogs to use standard combo box lists for properties and object types.
//    Encoding/Decoding of string formats now better matches 135.1 formats.
//    Fixed proprietary usage in WritePropertyMultiple
//
//    Known Bugs Fixed: 1921977, 1912929, 1841379, 1841067, 1799670,
//                1615874, 1592190, 1554997, 1477101, 1447133, 
//                1431122, 1254440, 994901, 974844, 895890, 895882,
//                855084. 829159
//
///      3.5.2 March 7, 2009   See release documents in Source Forge
//
//    3.5.2a   June 30, 2009  Minor release to SoftDEL only
//       Fixed DeviceAddressBinding display in EPICS view
//       Fixed InvokeID for auto-generated script.  Added this back this version.
//       Fixed Accumulator Scale decoding.
//       Fixed VTS error code response for unknown-property
//       Fixed EPICS consistency for Schedule BIBBs
//       Fixed Restore test to not write File_Size if IUT size is already 0.
//       Changed size of firmware revision allowed in EPICS from 10 to 32.
//    3.5.2c   
//       Fixed the 'any' password test in backup and restore
//       Fixed ActionList decoding for sniffer
//       Fixed date and time decoding for scripting
//    
//    3.5.3 October 21, 2009
//       Added Inconsistent Parameters tests (13.4.3, 13.4.4, 13.4.5)
//
//    3.5.4 May 30, 2010
//       Rewrote much of the sniffer, to simplify the coding of new messages and datatypes
//         Revised the sniffer detail view to indent items enclosed in paired tags (similar to WireShark)
//         Completed/fixed segmentation and network support in scripting
//         Changed IP code to use the appropriate subnet broadcast rather than hard-coded /24
//         Removed some string-length restrictions in the EPICS reader
//         Added template insertion and find/replace to the script editor
//         Made it possible to stop a running script entirely, and possible to close a running script without crashing VTS
//         Started a “Survival Guide” to help people start using VTS (the QuickStart, at half a meg doesn’t really do this) 
//         Cleaned up the scripts using 
//            Submission from Buddy Lott (ID 1704394 and 1704507)
//          Cleanup to use generic “DA” rather than specifying BVLL etc.  This allows scripts to be reused on different networks simply by changing Name or config values.  It also allows script messages to be segmented.  (Thanks to my buddies grep and sed for this effort…)
//              Replaced some AL=hex-string with current encoding for bitstrings etc. that were formerly not encodable
//
//       Known bugs fixed:
//          1592644     Scripting does not like Accumulator object identifiers
//          2979777     Netmask for B/IP is hardcoded
//          2415662     Array bounds exceeded
//
//    3.5.5 January 30, 2011
//       Known Bugs Fixed: 
//          3163632  Units enumeartions above 120 incorrectly decoded
//          3156501  ReadPropertyMultiple-ACK displayed wrong in details view
//          3094189  Crash on BVLL functions
//          3073740  How to configure any complex property value for VTS
//          3046835  vts crashes when editing bit string value
//          3063918  Still a problem with "VTS does not display in a Wind"
//          3010449  EPICS consistency errors showing as msg box not in view
//          3074176 Image for how to configure complex property value in VTS
//          1592195  "File too large to load" message when "Read All Properties"
// 
//       Corrected the list of properties added by the real Appendix j, and 
//          their associated strings.
//       Fixed some of the template types used in CreateCommonProperty().
//       Increased the object_name allocation in generic_object from 32 to 64 chars.
//       Filled out a few more icons for the EPICS tree view. 
// 
//       Replace CTime in Date/Time encoders with SYSTEMTIME/FILETIME: CTime starts at 1970, BACnet at 1900.
//       Correct and clean up time and date comparisons and use of * and ? fields 
//       Use %g for all floating point output.Accept INF, 1.#INF0, NAN, 1.#QNAN etc. for floating point input  
//       Avoid crash if filters are defined and packet file is missing or empty.
//       Eliminate memory leak of CFileExecptions at end of file 
//       Extend "any" keyword:- save or load from variable ot EPICS- compare 
//       Add SafeCopy and SafeAppend, add maxlengths to avoid buffer overflows on long string data 
//       Eliminated CheckEPICSCons dialog, which has been obsolete for some time.  This data is shown in the EPICS View pane 
//       CR 3010449.  Use GetTempPath to get appropriate location for EPICS error temporary file 
 
//       - Replace unsafe char* parameter in BACnet Encode(char*) with Encode(CString&)
//       - eliminate heap crash due to lack of a copy constructor in WPM dialog (as invoked from EPICS viewer.  Did you know it could do that?)
//       - remove "const int" defs for sizeof various classes.  Not used, and scary to think how they might be...
//       - various small cleanup (sorry, I waited too long to check in, and things got intertwined) 
 
//       Scripting uses hashes of keywords, including object-type, property-ids etc.  The precomputed values were several revisions behind.  
//       Changed to COMPUTE the hash tables for object-types, property-ID, abort and reject reason, services - the enumerations that are expected to change most frequently

//       Eliminate stPropIDs - use the StringTable version instead to reduce maintenance
//       Eliminate unused tables from StringTables.cpp
//       Make case and punctuation of strings in StringTables.cpp match 135-2008
//       Add reserved and extension counts to StringTable, to allow eventual replacement of the redundant etables

//       Change "log-enable" and "LOG_ENABLE" to "enable" and "ENABLE" per 135-2004b-5 

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

   // Dialog Data
   //{{AFX_DATA(CAboutDlg)
   enum { IDD = IDD_ABOUTBOX };
   CString  m_version;
   CString  m_downloadURL;
   //}}AFX_DATA

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CAboutDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   //{{AFX_MSG(CAboutDlg)
      // No message handlers
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
   //{{AFX_DATA_INIT(CAboutDlg)
   m_version = _T("?.?.?");
   m_downloadURL = s_downloadURL;
   //}}AFX_DATA_INIT

   m_version.Format( "Visual Test Shell version %s", (LPCTSTR)theApp.GetVersion() );

   if (s_gotVersionInfo)
   {
      CString str;
      str.Format( "\nLatest version is %s", (LPCTSTR)s_versionInfo );
      m_version += str;
   }
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDlg)
   DDX_Text(pDX, IDC_VERSION, m_version);
   DDX_Text(pDX, IDC_MFCLINK1, m_downloadURL);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
   //{{AFX_MSG_MAP(CAboutDlg)
      // No message handlers
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void VTSApp::OnAppAbout()
{
   CAboutDlg aboutDlg;
   aboutDlg.DoModal();
}


//
// VTSApp::CheckWinPcapVersion
//
// Thanks to Thomas Weller and www.codeguru.com, this function checks to see if 
// WinPcap is installed and checks the version.
//

void VTSApp::CheckWinPcapVersion( void )
{
   CFileVersionInfo  m_info;

#if 0
   // get the information
   // wtf? This isn't WinPCAP
   m_info.ReadVersionInfo( "C:\\WINNT\\system32\\drivers\\bacmacnt.sys" );

   if (m_info.IsValid())
   {
      CString version = m_info.GetFileVersionString();
      int   ver = atoi(version);
      if (ver < 3)
         AfxMessageBox( _T("WARNING: Installed version of BACMACNT.SYS must be 3.0 or later.") );
   }
#endif
}

//
// VTSAllocHook
//

#if NT_Platform
int __cdecl VTSAllocHook(
   int      nAllocType,
   void  *pvData,
   size_t   nSize,
   int      nBlockUse,
   long  lRequest,
   const unsigned char *szFileName,
   int      nLine
   )
{
   char *operation[] = { "", "allocating", "re-allocating", "freeing" };
   char *blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client" };
   if ( nBlockUse == _CRT_BLOCK )   // Ignore internal C runtime library allocations
      return( TRUE );

   _ASSERT( ( nAllocType > 0 ) && ( nAllocType < 4 ) );
   _ASSERT( ( nBlockUse >= 0 ) && ( nBlockUse < 5 ) );

   return( TRUE );         // Allow the memory operation to proceed
}
#endif

static void ShowTheBad( const char *theStr )
{
   // Set true if user tells us to not show any more messages
   static bool shutup = false;
   
   TRACE( "%s\n", theStr );
   if (!shutup)
   {
      CString str( theStr );
      str += "\nDo you want to see more useless messages like this?";
      shutup = AfxMessageBox( (LPCTSTR)str, MB_ICONEXCLAMATION | MB_YESNO )
              == IDNO;
   }
}


//
// VTSApp::PreTranslateMessage
//
// This function is called just before Windows does its usual message processing.  All 
// of the normal messages are passed through.  Application messages (those begining 
// with WM_APP and up) are reserved for VTS for communicating between IO threads and 
// main windows.
//

BOOL VTSApp::PreTranslateMessage(MSG* pMsg) 
{
   if (pMsg->message >= WM_APP) {
//    TRACE3( "***** User Msg %d, %d, 0x%08X\n"
//       , pMsg->message
//       , pMsg->wParam, pMsg->lParam
//       );
      
      // ### we really need a mechanism to make sure that the lParam
      // is a VTSDocPtr without dereferencing it.

      // TODO: unless we go MDI, how about
      VTSDocPtr pMsgDoc = (VTSDocPtr)(pMsg->lParam);
      CDocument *pDoc = GetWorkspace();
      bool ourDoc = (pMsgDoc == pDoc);

      switch (pMsg->message)
      {
         case WM_VTS_RCOUNT:
            if (ourDoc)
               pMsgDoc->ProcessPacketStoreChange();
            break;

         case WM_VTS_MAXPACKETS:
            // maximum packets have been reached... we're here because the main app thread
            // has to tell us about it...
            AfxMessageBox(IDS_ERR_MAXPACKETS, MB_ICONSTOP | MB_OK);
            break;

         case WM_VTS_PORTSTATUS:
            if (ourDoc)
               pMsgDoc->PortStatusChange();
            break;

         case WM_VTS_EXECMSG:
            {
               ScriptExecMsg * pexecmsg;
               while ((pexecmsg = gExecutor.ReadMsg()) != NULL)
               {
                  if (pexecmsg->IsKindOf( RUNTIME_CLASS(ScriptExecMsg) ))
                  {
                     switch (pexecmsg->GetType())
                     {
                     case ScriptExecMsg::msgStatus:
                        {
                           ScriptMsgStatus * pmsgstatus = (ScriptMsgStatus *) pexecmsg;
                           // Do a sanity check
                           if ((pmsgstatus->m_pdoc != NULL) && (pmsgstatus->m_pbase != NULL))
                           {
                              pmsgstatus->m_pdoc->SetImageStatus( pmsgstatus->m_pbase, pmsgstatus->m_nStatus );
                           }
                           else
                           {
                              CString str;
                              str.Format("ScriptMsgStatus has doc=%X base=%X",
                                       pmsgstatus->m_pdoc, pmsgstatus->m_pbase );
                              ShowTheBad( str );
                              // DON'T delete this thing - it probably has been already
                              pexecmsg = NULL;
                           }
                        }
                        break;

                     case ScriptExecMsg::msgMakeDlg:
                        {
                           ScriptMsgMake * pmsgmake = (ScriptMsgMake *) pexecmsg;
                           if (pmsgmake->m_pdlg != NULL)
                           {
                              switch (pmsgmake->m_maketype)
                              {
                              case ScriptMsgMake::msgmakeCreate:
                                 pmsgmake->m_pdlg->DoModeless();
                                 break;

                              case ScriptMsgMake::msgmakeDestroy:
                                 pmsgmake->m_pdlg->DestroyWindow();
                                 delete pmsgmake->m_pdlg;
                                 break;
                              default:
                                 {
                                    CString str;
                                    str.Format("Unknown ScriptMsgMake type %X", pmsgmake->m_maketype);
                                    ShowTheBad( str );
                                    // DON'T delete this thing - it probably has been already
                                    pexecmsg = NULL;
                                 }
                                 break;
                              }
                           }
                           else
                           {
                              CString str;
                              str.Format("ScriptMsgMake has dlg=%X",
                                       pmsgmake->m_pdlg );
                              ShowTheBad( str );
                              // DON'T delete this thing - it probably has been already
                              pexecmsg = NULL;
                           }
                        }
                        break;

                     default:
                        {
                           CString str;
                           str.Format("Unknown ScriptExecMsg type %X", pexecmsg->GetType());
                           ShowTheBad( str );
                        }
                        // DON'T delete this thing - it probably has been already
                        pexecmsg = NULL;
                        break;
                     }
                  }
                  else
                  {
                     ShowTheBad("Supposed ScriptExecMsg isn't");
                     // DON'T delete this thing - it probably has been already
                     pexecmsg = NULL;
                  }
                  delete pexecmsg;
               }
            }
            break;

         case WM_VTS_GOT_VERSION:
            // Worker thread has placed version information in s_versionInfo
            if (s_gotVersionInfo && (s_versionInfo > m_version))
            {
               // Web version is newer than ours.
               CString str = GetProfileString( "Version", "lastShown", "" );
               if (s_versionInfo != str)
               {
                  // We haven't put this version in the user's face yet
                  ShowUpdateInfo();
               }
            }
            break;

         default:
            // pass along to regular processing
            return CWinApp::PreTranslateMessage(pMsg);
      }

      // we completely processed this message
      return true;
   }

   // pass along to regular processing
   return CWinApp::PreTranslateMessage(pMsg);
}

CRecentFileList* VTSApp::GetRecentFileList()
{
   return m_pRecentFileList;
}
///////////////////////////////
