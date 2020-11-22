// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "VTS.h"
#include "BACnet.hpp"

#include "VTSPreferences.h"
#include "ChildFrm.h"
#include "MainFrm.h"

#include "ListSummaryView.h"

#include "DetailView.h"
#include "HexView.h"
#include "VTSStatisticsCollector.h"

#include "VTSPreferences.h"

#include "ReadAllPropSettingsDlg.h"

#include "BakRestoreExecutor.h"
#include "InconsistentParsExecutor.h"
#include "DiscoveryExecutor.h"

#include "ScriptFrame.h"
#include "ScriptExecutor.h"

#define MAX_LENGTH_OF_OBJECTTYPE    30
#define MAX_LENGTH_OF_PROPERTYNAME     100
#define MAX_DIGITS_OF_INSTANCENUMBER   10

#ifndef dword
typedef unsigned long dword;
#endif
#ifndef word
typedef unsigned short word;
#endif
#ifndef octet
typedef unsigned char octet;
#endif



///////////////////////////////
namespace PICS {
#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"
}

PICS::PICSdb *gPICSdb = NULL;



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern VTSPreferences gVTSPreferences;

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)


BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
   //{{AFX_MSG_MAP(CChildFrame)
   ON_WM_CANCELMODE()
   ON_UPDATE_COMMAND_UI(ID_FILE_WKS_NEW, OnUpdateFileWksNew)
   ON_UPDATE_COMMAND_UI(ID_FILE_WKS_SWITCH, OnUpdateFileWksSwitch)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
   ON_UPDATE_COMMAND_UI(ID_EDIT_REFRESH, OnUpdateEditRefresh)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PORTS, OnUpdateEditPorts)
   ON_UPDATE_COMMAND_UI(ID_EDIT_NAMES, OnUpdateEditNames)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DEVICES, OnUpdateEditDevices)
   ON_UPDATE_COMMAND_UI(ID_EDIT_LOGFILE, OnUpdateEditLogfile)
   ON_UPDATE_COMMAND_UI(ID_EDIT_CAPTUREFILTER, OnUpdateEditCaptureFilters)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DISPLAYFILTER, OnUpdateEditDisplayFilters)
// ON_UPDATE_COMMAND_UI(ID_FILE_WKS_SAVEAS, OnUpdateFileWksSaveAs)
// ON_UPDATE_COMMAND_UI(AFX_IDS_FILESAVEAS, OnUpdateFileWksSaveAs)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FIRSTFRAME, OnUpdateViewFirstFrame)
   ON_UPDATE_COMMAND_UI(ID_VIEW_LASTFRAME, OnUpdateViewLastFrame)
   ON_UPDATE_COMMAND_UI(ID_VIEW_NEXTFRAME, OnUpdateViewNextFrame)
   ON_UPDATE_COMMAND_UI(ID_VIEW_PREVFRAME, OnUpdateViewPrevFrame)
   ON_UPDATE_COMMAND_UI(ID_EPICS_LOAD, OnUpdateEPICSLoad)
   ON_UPDATE_COMMAND_UI(ID_EPICS_READALLPROPERTY, OnUpdateEPICSReadallproperty)
   ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
   ON_COMMAND(ID_EDIT_REFRESH, OnEditRefresh)
   ON_COMMAND(ID_EDIT_PORTS, OnEditPorts)
   ON_COMMAND(ID_EDIT_NAMES, OnEditNames)
   ON_COMMAND(ID_EDIT_DEVICES, OnEditDevices)
   ON_COMMAND(ID_EDIT_LOGFILE, OnEditLogfile)
   ON_COMMAND(ID_EDIT_CAPTUREFILTER, OnEditCaptureFilters)
   ON_COMMAND(ID_EDIT_DISPLAYFILTER, OnEditDisplayFilters)
   ON_COMMAND(ID_EDIT_PREFERENCES, OnEditPreferences)
   ON_COMMAND_RANGE( 0x8100, 0x81FF, OnSendSelectPort)
   ON_COMMAND_RANGE( 0x8200, 0x82FF, OnSendSelectPacket)
   ON_COMMAND(ID_VIEW_FIRSTFRAME, OnViewFirstFrame)
   ON_COMMAND(ID_VIEW_PREVFRAME, OnViewPrevFrame)
   ON_COMMAND(ID_VIEW_NEXTFRAME, OnViewNextFrame)
   ON_COMMAND(ID_VIEW_LASTFRAME, OnViewLastFrame)
   ON_COMMAND(ID_SEND_NEWPACKET, OnSendNewPacket)
   ON_COMMAND(ID_VIEW_DETAIL, OnViewDetail)
   ON_COMMAND(ID_VIEW_HEX, OnViewHex)
   ON_COMMAND(ID_VIEW_EPICS, OnViewEPICS)
   ON_UPDATE_COMMAND_UI(ID_VIEW_DETAIL, OnUpdateViewDetail)
   ON_UPDATE_COMMAND_UI(ID_VIEW_HEX, OnUpdateViewHex)
   ON_UPDATE_COMMAND_UI(ID_VIEW_EPICS, OnUpdateViewEPICS)
   ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
   ON_COMMAND(ID_EPICS_LOAD, OnEPICSLoad)
   ON_COMMAND(ID_EPICS_LOADAUTO, OnEPICSLoadAuto)
   ON_WM_DESTROY()
   ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
   ON_COMMAND(ID_FILE_PRINT_SETUP,OnFilePrintSetup)
   ON_UPDATE_COMMAND_UI(ID_FILE_PRINT,OnUpdateFilePrint)
   ON_COMMAND(ID_EPICS_READALLPROPERTY, OnReadAllProperty)
   ON_COMMAND(ID_GLOBAL_SCRIPT_STEPPASS, OnGlobalScriptSteppass)
   ON_UPDATE_COMMAND_UI(ID_GLOBAL_SCRIPT_STEPPASS, OnUpdateGlobalScriptSteppass)
   ON_COMMAND(ID_GLOBAL_SCRIPT_STEPFAIL, OnGlobalScriptStepfail)
   ON_UPDATE_COMMAND_UI(ID_GLOBAL_SCRIPT_STEPFAIL, OnUpdateGlobalScriptStepfail)
   ON_COMMAND(ID_GLOBAL_SCRIPT_STEP, OnGlobalScriptStep)
   ON_UPDATE_COMMAND_UI(ID_GLOBAL_SCRIPT_STEP, OnUpdateGlobalScriptStep)
   ON_COMMAND(ID_GLOBAL_SCRIPT_KILL, OnGlobalScriptKill)
   ON_UPDATE_COMMAND_UI(ID_GLOBAL_SCRIPT_KILL, OnUpdateGlobalScriptKill)
   ON_COMMAND(ID_GLOBAL_SCRIPT_HALT, OnGlobalScriptHalt)
   ON_UPDATE_COMMAND_UI(ID_GLOBAL_SCRIPT_HALT, OnUpdateGlobalScriptHalt)
   ON_COMMAND(ID_GLOBAL_DISABLE_PORT, OnGlobalDisablePort)
   ON_UPDATE_COMMAND_UI(ID_GLOBAL_DISABLE_PORT, OnUpdateGlobalDisablePort)
   ON_COMMAND(ID_GLOBAL_SCRIPT_CHECKSYNTAX, OnGlobalScriptChecksyntax)
   ON_UPDATE_COMMAND_UI(ID_GLOBAL_SCRIPT_CHECKSYNTAX, OnUpdateGlobalScriptChecksyntax)
   ON_COMMAND(ID_GLOBAL_SCRIPT_RESET, OnGlobalScriptReset)
   ON_UPDATE_COMMAND_UI(ID_GLOBAL_SCRIPT_RESET, OnUpdateGlobalScriptReset)
   ON_COMMAND(ID_GLOBAL_SCRIPT_RUN, OnGlobalScriptRun)
   ON_UPDATE_COMMAND_UI(ID_GLOBAL_SCRIPT_RUN, OnUpdateGlobalScriptRun)
   ON_COMMAND(ID_VIEW_MAINTOOLBAR, OnViewMaintoolbar)
   ON_COMMAND(ID_VIEW_GLOBAL_TOOLBAR, OnViewGlobalToolbar)
   ON_UPDATE_COMMAND_UI(ID_VIEW_MAINTOOLBAR, OnUpdateViewMaintoolbar)
   ON_UPDATE_COMMAND_UI(ID_VIEW_GLOBAL_TOOLBAR, OnUpdateViewGlobalToolbar)
   ////////////////////////////////////////////////
   //}}AFX_MSG_MAP
   ON_UPDATE_COMMAND_UI(ID_BACKUP_RESTORE, OnUpdateBackupRestore)
   ON_COMMAND(ID_BACKUP_RESTORE, OnBackupRestore)
   ON_UPDATE_COMMAND_UI(ID_TESTS_INCONSISTENTPARS, OnUpdateInconsistentPars)
   ON_COMMAND(ID_TESTS_INCONSISTENTPARS, OnInconsistentPars)
   ON_UPDATE_COMMAND_UI(ID_TOOLS_DISCOVER_DEVICES, OnUpdateDiscovery)
   ON_COMMAND(ID_TOOLS_DISCOVER_DEVICES, OnDiscoverDevices)
   ON_UPDATE_COMMAND_UI(ID_TOOLS_GENERATE_EPICS, OnUpdateDiscovery)
   ON_COMMAND(ID_TOOLS_GENERATE_EPICS, OnGenerateEPICS)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
   //
   // NOTE: the document and frame count for context is initialized in
   // OnCreateClient().
   //
   m_frameContext = new CFrameContext();
   m_bPrintSetup = false;
   m_bInPrinting = false;
}

CChildFrame::~CChildFrame()
{
   delete m_frameContext;
   delete m_pwndDetailViewBar;
   delete m_pwndHexViewBar;

   if ( gPICSdb )
   {
      PICS::MyDeletePICSObject( gPICSdb->Database );
      delete gPICSdb;
      gPICSdb = NULL;
   }

   delete m_pwndEPICSViewBar;
   delete m_wndGlobalBar;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: Modify the Window class or styles here by modifying
   //  the CREATESTRUCT cs

   //MAD_DB
   // Get rid of the overlapping min, restore, max buttons... this should
   // always be maximized.

   cs.style = (cs.style & ~WS_OVERLAPPEDWINDOW) | WS_OVERLAPPED | WS_CAPTION;

   if( !CMDIChildWnd::PreCreateWindow(cs) )
      return FALSE;

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
   CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
   CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
   // Let the clients know which context to use
   gNewFrameContext = m_frameContext;

   // Tell the frame context which document, and get frame count.  Be careful
   // because the frame count in the document might not be fully initialized
   // if this is a new frame for a new document, VTSDoc::OnNewDocument() has
   // not been called yet.
   ((VTSDoc*)pContext->m_pCurrentDoc)->BindFrameContext( m_frameContext );

   // it all worked, we now have two splitter windows which contain
   // three different views
   m_pwndHexViewBar    = new CDockingHexViewBar(pContext);
   m_pwndDetailViewBar = new CDockingDetailViewBar(pContext);
   m_pwndEPICSViewBar  = new CDockingEPICSViewBar(pContext);

   if (!m_pwndHexViewBar->Create(_T("Hex View"), this, 123))
   {
      TRACE0("Failed to create mybar\n");
      return -1;
   }

   m_pwndHexViewBar->SetBarStyle(m_pwndHexViewBar->GetBarStyle() |
                                 CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
   m_pwndHexViewBar->SetSCBStyle(SCBS_SIZECHILD);
   m_pwndHexViewBar->EnableDocking(CBRS_ALIGN_ANY);
   EnableDocking(CBRS_ALIGN_ANY);
   #ifdef _SCB_REPLACE_MINIFRAME
      m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
   #endif //_SCB_REPLACE_MINIFRAME
   
   if (!m_pwndDetailViewBar->Create(_T("Detail View"), this, 124))
   {
      TRACE0("Failed to create mybar\n");
      return -1;
   }
   m_pwndDetailViewBar->SetBarStyle(m_pwndDetailViewBar->GetBarStyle() |
   CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
   m_pwndDetailViewBar->SetSCBStyle(SCBS_SIZECHILD);

   m_pwndDetailViewBar->EnableDocking(CBRS_ALIGN_ANY);

   EnableDocking(CBRS_ALIGN_ANY);
   #ifdef _SCB_REPLACE_MINIFRAME
      m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
   #endif //_SCB_REPLACE_MINIFRAME

   if (!m_pwndEPICSViewBar->Create(_T("EPICS View"), this, 125))
   {
      TRACE0("Failed to create EPICS mybar\n");
      return -1;
   }

   m_pwndEPICSViewBar->SetBarStyle(m_pwndEPICSViewBar->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
   m_pwndEPICSViewBar->SetSCBStyle(SCBS_SIZECHILD);
   m_pwndEPICSViewBar->EnableDocking(CBRS_ALIGN_ANY);

   DockControlBar(m_pwndDetailViewBar,AFX_IDW_DOCKBAR_RIGHT);
   DockControlBar(m_pwndEPICSViewBar,AFX_IDW_DOCKBAR_RIGHT);
   // Moved default Hex to the bottom: four-wide is TOO wide.
   // Ideal (for me) would be to have hex view only below the list view, so that
   // Detail and EPICS views would get full height.
   // The user CAN move the hex view to the lower part of the detail or EPICS view,
   // but apparently NOT just below the list view.
   // And I don't see a simple way to do any of that programmatically.
   DockControlBar(m_pwndHexViewBar, AFX_IDW_DOCKBAR_BOTTOM);

   //////////////////////////////////////////////////////////////////////////////////
   m_wndGlobalBar = new CToolBar();
   if (!m_wndGlobalBar->CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
       | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY |CBRS_HIDE_INPLACE| CBRS_SIZE_DYNAMIC) ||
       !m_wndGlobalBar->LoadToolBar(IDR_GLOBALBAR))
   {
      TRACE0("Failed to create globalbar\n");
      return -1;      // fail to create
   }
   m_wndGlobalBar->EnableDocking( CBRS_ALIGN_ANY );
   DockControlBar( m_wndGlobalBar );

   CMDIChildWnd::OnCreateClient(lpcs,pContext);

   m_pDetailView = m_pwndDetailViewBar->m_pList;
   m_pHexView = m_pwndHexViewBar->m_pHexView;
   m_pSummaryView=(CListSummaryView *)GetDlgItem(AFX_IDW_PANE_FIRST);

   // set up the tab ring
   m_pSummaryView->m_pTabRing = m_pDetailView;
   m_pDetailView->m_pTabRing = m_pHexView;
   m_pHexView->m_pTabRing = m_pwndEPICSViewBar;
   m_pwndEPICSViewBar->m_pTabRing = m_pSummaryView;

   m_pwndDetailViewBar->LoadState(_T("Detail Bar Status"));
   m_pwndHexViewBar->LoadState(_T("Hex Bar Status"));
   m_pwndEPICSViewBar->LoadState(_T("EPICS Bar Status"));
   LoadBarState("Bar Status");

   m_pwndEPICSViewBar->OnDockPositionChanged();
   return TRUE;
}

BOOL CChildFrame::DestroyWindow() 
{
   SaveBarStates();
   return CMDIChildWnd::DestroyWindow();
}


void CChildFrame::SaveBarStates()
{
   if(!m_pwndDetailViewBar->IsFloating()&&!m_pwndHexViewBar->IsFloating())
      SaveBarState("Bar Status");
   if(!m_pwndDetailViewBar->IsFloating())
      m_pwndDetailViewBar->SaveState(_T("Detail Bar Status"));
   if(!m_pwndHexViewBar->IsFloating())
      m_pwndHexViewBar->SaveState(_T("Hex Bar Status"));
   if(!m_pwndEPICSViewBar->IsFloating())
      m_pwndEPICSViewBar->SaveState(_T("EPICS Bar Status"));
}

void CChildFrame::OnDestroy() 
{
   SaveBarStates();
   CMDIChildWnd::OnDestroy();
}


void CChildFrame::OnCancelMode() 
{
   CMDIChildWnd::OnCancelMode();
   TRACE0( "CChildFrame::OnCancelMode() ?\n" );
}

void CChildFrame::OnUpdateFileWksNew(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(true);
}

void CChildFrame::OnUpdateFileWksSwitch(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(true);
}

void CChildFrame::OnUpdateFileWksSaveAs(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(true);
}

void CChildFrame::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateEditDelete()\n" );
   pCmdUI->Enable( (m_frameContext->m_PacketCount > 0) );
}

void CChildFrame::OnUpdateEditRefresh(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( (m_frameContext->m_PacketCount > 0) );
}

void CChildFrame::OnUpdateEditPorts(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateEditPorts()\n" );
   pCmdUI->Enable( true );
}

void CChildFrame::OnUpdateEditNames(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateEditNames()\n" );
   pCmdUI->Enable( true );
}

void CChildFrame::OnUpdateEditDevices(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateEditDevices()\n" );
   pCmdUI->Enable( true );
}

void CChildFrame::OnUpdateEditLogfile(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( true );
}

void CChildFrame::OnUpdateEditCaptureFilters(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateEditCaptureFilters()\n" );
   pCmdUI->Enable( true );
}

void CChildFrame::OnUpdateEditDisplayFilters(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateEditDisplayFilters()\n" );
   pCmdUI->Enable( true );
}

void CChildFrame::OnUpdateViewFirstFrame(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateViewFirstFrame()\n" );
   pCmdUI->Enable( (m_frameContext->m_PacketCount > 0) && (m_frameContext->m_CurrentPacket != 0) );
}

void CChildFrame::OnUpdateViewPrevFrame(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateViewPrevFrame()\n" );
   pCmdUI->Enable( (m_frameContext->m_PacketCount > 0) && (m_frameContext->m_CurrentPacket > 0) );
}

void CChildFrame::OnUpdateViewNextFrame(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateViewNextFrame()\n" );
   pCmdUI->Enable( (m_frameContext->m_PacketCount > 0) && (m_frameContext->m_CurrentPacket >= 0) && (m_frameContext->m_CurrentPacket < (m_frameContext->m_PacketCount - 1)) );
}

void CChildFrame::OnUpdateViewLastFrame(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateViewLastFrame()\n" );
   pCmdUI->Enable( (m_frameContext->m_PacketCount > 0) && (m_frameContext->m_CurrentPacket != (m_frameContext->m_PacketCount - 1)) );
}

void CChildFrame::OnUpdateViewSend(CCmdUI* pCmdUI) 
{
// TRACE0( "VTSDoc::OnUpdateViewSend()\n" );
   pCmdUI->Enable( true );
}

///////////////////////////////////////////////////////////
// Added by Zhenhua Zhu, 2003-6-2
void CChildFrame::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(!m_bInPrinting);
}

void CChildFrame::OnUpdateEPICSReadallproperty(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(gPICSdb != NULL);
}

void CChildFrame::OnEditDelete() 
{
   if ( !gVTSPreferences.Setting_IsVerifyDelete() || AfxMessageBox(IDS_DELETEAREYOUSURE, MB_YESNO | MB_ICONQUESTION) == IDYES )
   {
      delete m_frameContext->m_pDoc->m_pStatisticsCollector;
      m_frameContext->m_pDoc->m_pStatisticsCollector=new VTSStatisticsCollector();
      m_frameContext->m_pDoc->DeletePackets();
   }
}

void CChildFrame::OnEditRefresh() 
{
   m_frameContext->m_pDoc->ReloadPacketStore();
}

void CChildFrame::OnEditPorts() 
{
   m_frameContext->m_pDoc->DoPortsDialog();
}

void CChildFrame::OnEditNames() 
{
   m_frameContext->m_pDoc->DoNamesDialog();
}

void CChildFrame::OnEditDevices() 
{
   m_frameContext->m_pDoc->DoDevicesDialog();
}

void CChildFrame::OnEditLogfile() 
{
   m_frameContext->m_pDoc->DoPacketFileNameDialog();
}

void CChildFrame::OnEditCaptureFilters() 
{
   m_frameContext->m_pDoc->DoCaptureFiltersDialog();
}

void CChildFrame::OnEditDisplayFilters() 
{
   m_frameContext->m_pDoc->DoDisplayFiltersDialog();
}

void CChildFrame::OnEditPreferences()
{
   gVTSPreferences.DoPrefsDlg();
}

void CChildFrame::OnViewFirstFrame() 
{
   m_frameContext->SetCurrentPacket( 0 );
}

void CChildFrame::OnViewPrevFrame() 
{
   m_frameContext->SetCurrentPacket( m_frameContext->m_CurrentPacket - 1 );
}

void CChildFrame::OnViewNextFrame() 
{
   m_frameContext->SetCurrentPacket( m_frameContext->m_CurrentPacket + 1 );
}

void CChildFrame::OnViewLastFrame() 
{
   m_frameContext->SetCurrentPacket( m_frameContext->m_PacketCount - 1 );
}

void CChildFrame::OnSendNewPacket() 
{
   m_frameContext->m_pDoc->DoSendWindow( -1, -1 );
}

// CChildFrame::OnUpdateEPICSLoad
//
// It's kinda lame, but the menu item is checked when an EPICS has been 
// successfully loaded.

afx_msg void CChildFrame::OnUpdateEPICSLoad(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( true );
   pCmdUI->SetCheck( gPICSdb != 0 );
}

//
// CChildFrame::OnSendSelectPort
//
// Selecting a port from the Send menu will change the content below the packet list
// to reflect the types of packets available according to that port type.  Because the
// menu is built every time the menu is clicked on (or any other time that
// CMainFrame::OnInitMenuPopup is called) the only thing to do is set the global.
//
// Note that because the port list is global, you can select a port that is not in the
// foreground session.
//

extern int gSelectedPort;

void CChildFrame::OnSendSelectPort( UINT uID )
{
   TRACE1( "Select port 0x%04X\n", uID );
   gSelectedPort = (uID & 0x00FF);
}

//
// CChildFrame::OnSendSelectPacket
//

void CChildFrame::OnSendSelectPacket( UINT uID )
{
   TRACE1( "Select packet 0x%04X\n", uID );
   m_frameContext->m_pDoc->DoSendWindow( (uID >> 4) & 0x0F, uID & 0x0F );
}

//
// CChildFrame::ActivateFrame
//
// This interesting little chunk of code sucked right out of the tech notes
// opens a child window maximized if there are no other child windows open.
//

void CChildFrame::ActivateFrame(int nCmdShow)
{
   // if another window is open, use default
   if(GetMDIFrame()->MDIGetActive())
      CMDIChildWnd::ActivateFrame(nCmdShow); 
   else
      CMDIChildWnd::ActivateFrame(SW_SHOWMAXIMIZED); // else open maximized
}

void CChildFrame::OnViewDetail()
{
   ShowControlBar(m_pwndDetailViewBar, !m_pwndDetailViewBar->IsVisible(), FALSE);
}

void CChildFrame::OnViewHex()
{
   ShowControlBar(m_pwndHexViewBar, !m_pwndHexViewBar->IsVisible(), FALSE);
}

void CChildFrame::OnViewEPICS()
{
   ShowControlBar(m_pwndEPICSViewBar, !m_pwndEPICSViewBar->IsVisible(), FALSE);
}

void CChildFrame::OnUpdateViewDetail(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
   pCmdUI->SetCheck(m_pwndDetailViewBar->IsVisible());
}

void CChildFrame::OnUpdateViewHex(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
   pCmdUI->SetCheck(m_pwndHexViewBar->IsVisible());
}

void CChildFrame::OnUpdateViewEPICS(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
   pCmdUI->SetCheck(m_pwndEPICSViewBar->IsVisible());
}


void CChildFrame::OnFileExport()
{
   int nPacketCount = m_frameContext->m_pDoc->GetPacketCount();

   // pop up dialog asking for export filename
   CFileDialog exportFileDlg(FALSE,NULL,NULL,NULL,"Text Files(*.txt)|*.txt||", NULL);
   if ( exportFileDlg.DoModal() != IDOK )
      return;

   CString strPathName = exportFileDlg.GetPathName();
   if( exportFileDlg.GetFileExt() != "txt")
      strPathName += ".txt";
      
   CString str;
   CStdioFile exportFile;

   CProgressCtrl * pprogress = ((CMainFrame *) AfxGetApp()->m_pMainWnd)->InitializeProgress();
   pprogress->SetRange(0, m_frameContext->m_pDoc->GetPacketCount());

   try
   {
      exportFile.Open(strPathName, CFile::modeCreate | CFile::modeWrite);

      // for some reason we have to have two info objects, one just for the summary and one for 
      // the detail lines...  If not, the detail lines are out of order.

      BACnetPIInfo   summary( true, false );

      for( int i = 0; i < m_frameContext->m_pDoc->GetPacketCount(); i++ )
      {
         pprogress->SetPos(i);

         VTSPacketPtr ppkt = m_frameContext->m_pDoc->GetPacket(i);
         if ( ppkt == NULL )
            break;

         str.Format("*************************** [ Packet %d Start ] ***************************\n\n" \
                   "[ Summary Information ]\n\n", i);
         exportFile.WriteString(str);

         str.Format("Time:        %s\n", ppkt->GetTimeStampString());
         exportFile.WriteString(str);

         str.Format("Source:      %s\n", ppkt->GetAddressString(m_frameContext->m_pDoc, true));
         exportFile.WriteString(str);

         str.Format("Destination: %s\n", ppkt->GetAddressString(m_frameContext->m_pDoc, false));
         exportFile.WriteString(str);
         
         NetworkSniffer::SetLookupContext( ppkt->packetHdr.m_szPortName );
         summary.Interpret( (BACnetPIInfo::ProtocolType) ppkt->packetHdr.packetProtocolID, (char *) ppkt->packetData, ppkt->packetLen);

         str.Format("Summary:     %s\n\n" \
                  "[ Detail Information ]\n\n", summary.summaryLine );
         exportFile.WriteString(str);

         // have to call the interpreter twice because of the summery/detail breakdown problem..
         BACnetPIInfo   detail(false, true);
         detail.Interpret( (BACnetPIInfo::ProtocolType) ppkt->packetHdr.packetProtocolID, (char *) ppkt->packetData, ppkt->packetLen);

         for (int j = 0; j < detail.detailCount; j++ )
         {
            // Careful... BACnetPIInfo does not allocate space for the first element, which should be
            // used for the timestamp.  We already do that so everything's fine.  Just skip it.

            if ( detail.detailLine[j] != NULL )
            {
               exportFile.WriteString(detail.detailLine[j]->piLine);
               exportFile.WriteString("\n");
            }
         }

         str.Format("*************************** [ Packet %d End ] ***************************\n\n", i);
         exportFile.WriteString(str);
      }

      exportFile.Close();
   }
   catch(CFileException *e)
   {
      e->GetErrorMessage(str.GetBuffer(100), 100);
      AfxMessageBox(str);
      e->Delete();
   }

   ((CMainFrame *) AfxGetApp()->m_pMainWnd)->ReleaseProgress();
}

///////////////////////////////////////////////////////////
// Added by Zhenhua Zhu, 2003-6-2
//Modifyed by Zhenhua Zhu, 2003-6-11
void CChildFrame::OnFilePrint()
{
   HPRIVATEFONT   hFont;
   if(!m_bPrintSetup)
   {
      if(m_printer.Dialog()!=-1)
         m_bPrintSetup= true;
      else 
         return;
   }
   m_printer.StartPrint();
   m_bInPrinting = true;
   hFont = m_printer.AddFontToEnvironment("Arial");   
   m_printer.SetMargins(50,130,50,10);
   m_printer.SetDistance(2);
   m_printer.StartPage();  

   int nPacketCount = m_frameContext->m_pDoc->GetPacketCount();
   CString str;
   CProgressCtrl * pprogress = ((CMainFrame *) AfxGetApp()->m_pMainWnd)->InitializeProgress();
   pprogress->SetRange(0, m_frameContext->m_pDoc->GetPacketCount()); 

   BACnetPIInfo   summary( true, false );
   for( int i = 0; i < m_frameContext->m_pDoc->GetPacketCount(); i++ )
   {
      pprogress->SetPos(i);
      VTSPacketPtr ppkt = m_frameContext->m_pDoc->GetPacket(i);
      if ( ppkt == NULL )
         break;
      str.Format("*************************** [ Packet %d Start ] ***************************", i);
      m_printer.Print(hFont, str, FORMAT_NORMAL);
      m_printer.Print(hFont, "", FORMAT_NORMAL);

      m_printer.Print(hFont, "[ Summary Information ]", FORMAT_NORMAL);
      m_printer.Print(hFont, "", FORMAT_NORMAL);

      str.Format("Time:        %s", ppkt->GetTimeStampString());
      m_printer.Print(hFont, str, FORMAT_NORMAL);

      str.Format("Source:      %s", ppkt->GetAddressString(m_frameContext->m_pDoc, true));
      m_printer.Print(hFont, str, FORMAT_NORMAL);

      str.Format("Destination: %s", ppkt->GetAddressString(m_frameContext->m_pDoc, false));
      m_printer.Print(hFont, str, FORMAT_NORMAL);
         
      NetworkSniffer::SetLookupContext( ppkt->packetHdr.m_szPortName );
      summary.Interpret( (BACnetPIInfo::ProtocolType) ppkt->packetHdr.packetProtocolID, (char *) ppkt->packetData, ppkt->packetLen);

      str.Format("Summary:     %s", summary.summaryLine );
      m_printer.Print(hFont, str, FORMAT_NORMAL);
      m_printer.Print(hFont, "", FORMAT_NORMAL);
      m_printer.Print(hFont, "[ Detail Information ]", FORMAT_NORMAL);
      m_printer.Print(hFont, "", FORMAT_NORMAL);
      // have to call the interpreter twice because of the summery/detail breakdown problem..
      BACnetPIInfo   detail(false, true);
      detail.Interpret( (BACnetPIInfo::ProtocolType) ppkt->packetHdr.packetProtocolID, (char *) ppkt->packetData, ppkt->packetLen);

      for (int j = 0; j < detail.detailCount; j++ )
      {
         // Careful... BACnetPIInfo does not allocate space for the first element, which should be
         // used for the timestamp.  We already do that so everything's fine.  Just skip it.
         if ( detail.detailLine[j] != NULL )
         {
            m_printer.Print(hFont, detail.detailLine[j]->piLine, FORMAT_NORMAL);
         }
      }
      str.Format("*************************** [ Packet %d End ] ***************************", i);
      m_printer.Print(hFont, str, FORMAT_NORMAL);
      m_printer.Print(hFont, "", FORMAT_NORMAL);
   }
   m_printer.EndPage();
   m_printer.EndPrint();
   ((CMainFrame *) AfxGetApp()->m_pMainWnd)->ReleaseProgress();
}

void CChildFrame::OnFilePrintSetup()
{
   if (m_printer.Dialog()!=-1)
      m_bPrintSetup = true;
}


// CChildFrame::OnEPICSLoad
//
void CChildFrame::OnEPICSLoad()
{
   CString filePath;
   if (CEPICSTreeView::GetEpicsFilename( filePath ))
   {
      EPICSLoad(filePath);
   }
}


void CChildFrame::EPICSLoad( LPCSTR lpszFileName )
{
   if ( m_pwndEPICSViewBar != NULL )
   {
      m_pwndEPICSViewBar->EPICSLoad(lpszFileName);
      if (!m_pwndEPICSViewBar->IsVisible())
      {
         // Show the EPICS view
         ShowControlBar(m_pwndEPICSViewBar, TRUE, FALSE);
      }
   }
}


void CChildFrame::OnEPICSLoadAuto()
{
   EPICSLoad(gVTSPreferences.Setting_GetLastEPICS());
}



// ScriptFrame::OnReadAllProperty

void CChildFrame::OnReadAllProperty()
{
   CString strFileName;
   CReadAllPropSettingsDlg dlg;

   if ( gPICSdb && (dlg.DoModal() == IDOK) && CreateScriptFile(&strFileName, &dlg, 0) )
   {
      // Open the file (with frame) and kill it from the recent list
      if ( AfxGetApp()->OpenDocumentFile(strFileName) != NULL )
         ((VTSApp *) AfxGetApp())->GetRecentFileList()->Remove(0);

      // Should we delete the file?
      // DeleteFile(strFileName);
   }
}


// For protected access

void CChildFrame::DoReadAllProperties()
{
   OnReadAllProperty();
}

void CChildFrame::DoReadSingleProperties( LPCSTR lpszFileName, unsigned long ulObjectID )
{
   CString strFileName = lpszFileName;
   CReadAllPropSettingsDlg dlg;

   if ( gPICSdb && dlg.DoModal() == IDOK && CreateScriptFile(&strFileName, &dlg, ulObjectID) )
   {
      // Open the file (with frame) and kill it from the recent list
      if ( AfxGetApp()->OpenDocumentFile(strFileName) != NULL )
         ((VTSApp *) AfxGetApp())->GetRecentFileList()->Remove(0);
   }
}


//******************************************************************
// Author:     Yajun Zhou
// Date:    2002-11-4
//  last edit: 03-SEP-03 GJB  added the support two keywords: DNET and DADR
// Purpose: Create a script file to read all the properties
// In:         void
// Out:     BOOL: TRUE when success
//               FALSE when fault
//******************************************************************

BOOL CChildFrame::CreateScriptFile( CString * pstrFileName, CReadAllPropSettingsDlg * pdlg, unsigned long ulObjectID )
{
   // use dynamic memory here so open on create will throw... the same as WriteString
   CStdioFile  *   pscript;
   CString        strError;

   // Use RPM if requested and EPICS says it is supported.
   bool useRPM = pdlg->m_bUseRPM != 0;
   if ((gPICSdb->BACnetStandardServices[14] & ssExecute) == 0)
   {
      useRPM = false;
   }

   try
   {
      bool includeAllDetails = true;

      VTSPortType nPortType;
      int i;
      CString str;
      char  szTemp[255];
      PICS::generic_object * pDatabase;
      static const char hex[] = "0123456789ABCDEF";

      if ( pstrFileName->IsEmpty() )
         *pstrFileName = "ReadAllProperties.vts";

      pscript = new CStdioFile(*pstrFileName, CFile::modeCreate|CFile::modeWrite);
      *pstrFileName = pscript->GetFilePath();

      pscript->WriteString(" ;Read All Property Tests\n"
                           " ;-------------------------------------------------------------------------------------\n" );
      if (useRPM)
      {
         pscript->WriteString("  SETUP ReadPropertyMultiple Tests\n" );
      }
      else
      {
         pscript->WriteString("  SETUP ReadProperty Tests\n" );
      }

      pscript->WriteString(" ;-------------------------------------------------------------------------------------\n\n");

      VTSDoc * pdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
      VTSPorts * pports = pdoc == NULL ? NULL : pdoc->GetPorts();
      VTSNames * pnames = pdoc == NULL ? NULL : pdoc->GetNames();

      // Get port type
      nPortType = ipPort;
      for ( i = 0; pports != NULL && i < pports->GetSize(); i++ )
      {
         if ( (*pports)[i]->m_strName.CompareNoCase(pdlg->m_strNetwork) == 0 ) {
            nPortType = (*pports)[i]->m_nPortType;
            break;
         }
      }
      str.Format("  ACTIVENET = \'%s\'\n", pdlg->m_strNetwork );
      pscript->WriteString(str);

      // get DA
      int nIndex = pnames->FindIndex(pdlg->m_strIUTIP);
      if (nIndex >= 0)
      {
         // Name.
         if (!pdlg->m_bDNET && (pports->GetSize() == 1))
         {
            // Dialog specifies a Name, no network override, and only one interface.
            // Don't include all the gory PDU details, for greater portability
            includeAllDetails = false;
         }
         else
         {
            // Dialog specifies a Name, and either network override or multiple interfaces.
            // Use the name as DA, adding network info as specified
            BACnetAddress* pAdr = &((*pnames)[nIndex]->m_bacnetaddr);
            if ((pAdr->addrType == nullAddr) ||
               (pAdr->addrType == remoteBroadcastAddr) || 
               (pAdr->addrType == remoteStationAddr))
            {
               AfxMessageBox("DA address must be on a local network!");
               return FALSE;
            }
         }

         // Use the name in quotes: send will get datalink, network, and segmentation details
         str.Format( "  IUT_ADDR = \"%s\"\n", (LPCTSTR)pdlg->m_strIUTIP );
      }
      else
      {
         // String isn't a known Name. interpret it as an address.
         // NOT quoted, or script engine demands a name.
         str.Format("  IUT_ADDR = %s\n", (LPCTSTR)pdlg->m_strIUTIP );
      }
      pscript->WriteString(str);

      // Get DNET, DADR if they exist
      if (pdlg->m_bDNET && ((nIndex = pnames->FindIndex(pdlg->m_strDnetDadr)) >= 0))
      {
         BACnetAddress* pAdr = &((*pnames)[nIndex]->m_bacnetaddr);
         if ((pAdr->addrType == nullAddr) || 
            (pAdr->addrType == localBroadcastAddr) ||
            (pAdr->addrType == localStationAddr)) 
         {
            AfxMessageBox("DNET must be a non-local network!");
            return   FALSE;
         }

         char *s = szTemp;
         szTemp[0] = 0;
         // encode the address as an octet string
         for ( i = 0; i < pAdr->addrLen; i++) {
            *s++ = hex[ pAdr->addrAddr[i] >> 4 ];
            *s++ = hex[ pAdr->addrAddr[i] & 0x0F ];
         }
         *s = 0;

         str.Format("  IUT_DNET = %d\n"
                    "  IUT_DADR = X'%s'\n\n", pAdr->addrNet, szTemp );
         pscript->WriteString(str);
      }

      // Generate list of all parameters for each object found in DB
      int nObjNum;
      for ( nObjNum = 1, pDatabase = gPICSdb->Database;  pDatabase != NULL;  pDatabase = (PICS::generic_object *) pDatabase->next, nObjNum++ )
      {
         BACnetObjectIdentifier  bacnetObjID((unsigned int) pDatabase->object_id);
         CString oid;
         bacnetObjID.Encode(oid, BACnetEncodeable::FMT_SCRIPT);
         str.Format("  OBJECT%d = %s\n", nObjNum, (LPCTSTR)oid);
         pscript->WriteString(str);
      }

      // Now build each SECTION and TEST for each object
      for ( nObjNum = 1, pDatabase = gPICSdb->Database;  pDatabase != NULL;  pDatabase = (PICS::generic_object *) pDatabase->next, nObjNum++ )
      {
         // madanner 8/04
         // Added support for single object properties script

         if ( ulObjectID != 0  &&  ulObjectID != pDatabase->object_id )
            continue;

         str.Format("\n ;-------------------------------------------------------------------------------------\n"
                    "  SECTION Read Properties of OBJECT%d\n\n", nObjNum);
         pscript->WriteString(str);

         int cur_invokeid = 0;
         if (useRPM)
         {
            // Use ReadPropertyMultiple
            str.Format(" ;-------------------------------------------------------------------------------------\n"
                       "  TEST#%d.1 - Read Multiple\n"
                       "  DEPENDENCIES None\n\n", nObjNum);
            pscript->WriteString(str);

            pscript->WriteString( "    SEND (\n" );

            if (includeAllDetails)
            {
               pscript->WriteString( "\tNETWORK = ACTIVENET\n" );
            }

            pscript->WriteString( "\tDA = IUT_ADDR\n"); 

            if (pdlg->m_bDNET)
            {
               pscript->WriteString( "\tDNET = IUT_DNET\n"
                                     "\tDADR = IUT_DADR\n" );
            }

            if (includeAllDetails)
            {
               pscript->WriteString("\tDER = TRUE\n");
               if (nPortType == ipPort)
               {
                  pscript->WriteString("\tBVLCI = ORIGINAL-UNICAST-NPDU\n");
               }
               str.Format( "\tPDU = Confirmed-Request\n"
                           "\tService = ReadPropertyMultiple\n"
                           "\tInvokeID = %d\n"
                           "\tSegMsg = 0\n"
                           "\tSegResp = 0\n"
                           "\tMaxResp = 1476\n"
                           "\tObject = 0, OBJECT%d\n"
                           "\tOpenTag 1\n"
                           "\t   Property = 0, ALL\n"
                           "\tCloseTag 1\n"
                           "    )\n\n", cur_invokeid, nObjNum );

               cur_invokeid = (cur_invokeid+1)%256;
            }
            else
            {
               str.Format( "\tPDU = Confirmed-Request\n"
                           "\tService = ReadPropertyMultiple\n"
                           "\tObject = 0, OBJECT%d\n"
                           "\tOpenTag 1\n"
                           "\t   Property = 0, ALL\n"
                           "\tCloseTag 1\n"
                           "    )\n\n", nObjNum );
            }
            pscript->WriteString(str);

            // Reply
            pscript->WriteString( "    EXPECT (\n" );

            if (includeAllDetails)
            {
               pscript->WriteString( "\tNETWORK = ACTIVENET\n" );
            }

            pscript->WriteString( "\tSA = IUT_ADDR\n" ); 

            if (pdlg->m_bDNET)
            {
               pscript->WriteString( "\tSNET = IUT_DNET\n"
                                     "\tSADR = IUT_DADR\n" );
            }

            if (includeAllDetails)
            {
               pscript->WriteString("\tDER = FALSE\n");
               if (nPortType == ipPort) 
               {
                  pscript->WriteString("\tBVLCI = ORIGINAL-UNICAST-NPDU\n");
               }
            }

            str.Format( "\tPDU = ComplexAck\n"
                        "\tService = ReadPropertyMultiple\n"
                        "\tObject = 0, OBJECT%d\n"
                        "\tOpenTag 1\n"
                        "\t   ANY\n"
                        "\tCloseTag 1\n"
                        "    )\n\n", nObjNum, szTemp, szTemp );
            pscript->WriteString(str);
         }
         else
         {
            // Use ReadProperty

            // Now create a TEST section for each of the defined properties
            for ( int nPropIndex = 0; nPropIndex < sizeof(pDatabase->propflags)/sizeof(pDatabase->propflags[0]); nPropIndex++ )
            {
               dword dwbogus;

               if ( PICS::GetPropNameSupported(szTemp, nPropIndex, pDatabase->object_type, pDatabase->propflags, &dwbogus, NULL) > 0 )
               {
                  str.Format(" ;-------------------------------------------------------------------------------------\n"
                             "  TEST#%d.%d - Read %s\n"
                             "  DEPENDENCIES None\n\n", nObjNum, nPropIndex+1, szTemp);
                  pscript->WriteString(str);

                  pscript->WriteString( "    SEND (\n" );

                  if (includeAllDetails)
                  {
                     pscript->WriteString( "\tNETWORK = ACTIVENET\n" );
                  }

                  pscript->WriteString( "\tDA = IUT_ADDR\n"); 

                  if (pdlg->m_bDNET) 
                  {
                     pscript->WriteString( "\tDNET = IUT_DNET\n"
                                           "\tDADR = IUT_DADR\n" );
                  }

                  if (includeAllDetails)
                  {
                     pscript->WriteString("\tDER = TRUE\n");
                     if (nPortType == ipPort) 
                     {
                        pscript->WriteString("\tBVLCI = ORIGINAL-UNICAST-NPDU\n");
                     }
                     str.Format( "\tPDU = Confirmed-Request\n"
                                 "\tService = ReadProperty\n"
                                 "\tInvokeID = %d\n"
                                 "\tSegMsg = 0\n"
                                 "\tSegResp = 0\n"
                                 "\tMaxResp = 1476\n"
                                 "\tObject = 0, OBJECT%d\n"
                                 "\tProperty = 1, %s\n"
                                 "    )\n\n", cur_invokeid, nObjNum, szTemp );

                     cur_invokeid = (cur_invokeid+1)%256;
                  }
                  else
                  {
                     str.Format( "\tPDU = Confirmed-Request\n"
                                 "\tService = ReadProperty\n"
                                 "\tObject = 0, OBJECT%d\n"
                                 "\tProperty = 1, %s\n"
                                 "    )\n\n", nObjNum, szTemp );
                  }
                  pscript->WriteString(str);

                  // Reply
                  pscript->WriteString( "    EXPECT (\n" );

                  if (includeAllDetails)
                  {
                     pscript->WriteString( "\tNETWORK = ACTIVENET\n" );
                  }

                  pscript->WriteString( "\tSA = IUT_ADDR\n" ); 

                  if (pdlg->m_bDNET) 
                  {
                     pscript->WriteString( "\tSNET = IUT_DNET\n"
                                           "\tSADR = IUT_DADR\n" );
                  }

                  if (includeAllDetails)
                  {
                     pscript->WriteString("\tDER = FALSE\n");
                     if (nPortType == ipPort) 
                     {
                        pscript->WriteString("\tBVLCI = ORIGINAL-UNICAST-NPDU\n");
                     }
                  }

                  if (_stricmp( szTemp, "log-buffer" ) == 0)
                  {
                     // Can't read log-buffer except via ReadRange.  So expect an error
                     str.Format( "\tPDU = Error\n"
                                 "\tService = ReadProperty\n"
                                 "\tEnum 2  -- properties\n"
                                 "\tEnum 27 -- read-access-denied\n"
                                 "    )\n\n" );
                  }
                  else if (pDatabase->propflags[nPropIndex] & ValueUnknown)
                  {
                     // EPICS says "unknown": accept any value
                     str.Format( "\tPDU = ComplexAck\n"
                                 "\tService = ReadProperty\n"
                                 "\tObject = 0, OBJECT%d\n"
                                 "\tProperty = 1, %s\n"
                                 "\tOpenTag 3\n"
                                 "\t\tANY\n"
                                 "\tCloseTag 3\n"
                                 "    )\n\n", nObjNum, szTemp );
                  }
                  else
                  {
                     // Compare to value from EPICS
                     str.Format( "\tPDU = ComplexAck\n"
                                 "\tService = ReadProperty\n"
                                 "\tObject = 0, OBJECT%d\n"
                                 "\tProperty = 1, %s\n"
                                 "\tOpenTag 3\n"
                                 "\t\tAL = {%s}\n"
                                 "\tCloseTag 3\n"
                                 "    )\n\n", nObjNum, szTemp, szTemp );
                  }
                  pscript->WriteString(str);
               }
            }     // end loop on properties
         }        // end if ReadMultiple / else ReadProperty
      }           // end loop on objects

      // Now close off the file for good measure
      pscript->WriteString(";---------------------- End of generated Read Property file ---------------------------");
   }

   catch(CFileException *e)
   {
      TCHAR szErr[255];
      e->GetErrorMessage(szErr, sizeof(szErr));
      strError.Format(_T("Error Generating Script: %s"), szErr);
      AfxMessageBox((LPCTSTR)strError);
   }

   if ( pscript != NULL )        // closes file as well
      delete pscript;

   return strError.IsEmpty();
}

extern BakRestoreExecutor gBakRestoreExecutor;
void CChildFrame::OnUpdateBackupRestore(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!gBakRestoreExecutor.IsRunning());
}

void CChildFrame::OnBackupRestore()
{
   m_frameContext->m_pDoc->DoBackupRestore();
}

extern InconsistentParsExecutor gInconsistentParsExecutor;
void CChildFrame::OnUpdateInconsistentPars(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!gInconsistentParsExecutor.IsRunning());
}

void CChildFrame::OnInconsistentPars()
{
   m_frameContext->m_pDoc->DoInconsistentPars();
}

// Discovery and EPICS generation share an executor, so enable/disable together
extern DiscoveryExecutor gDiscoveryExecutor;
void CChildFrame::OnUpdateDiscovery(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!gDiscoveryExecutor.IsRunning());
}

void CChildFrame::OnDiscoverDevices()
{
   gDiscoveryExecutor.ExecuteTest( DiscoveryExecutor::FN_DISCOVER_DEVICES );
}

void CChildFrame::OnGenerateEPICS()
{
   gDiscoveryExecutor.ExecuteTest( DiscoveryExecutor::FN_CREATE_EPICS );
}

/////////////////////////////////////////////////////////////////////
//Added by Zhu Zhenhua, 2004-11-27,for #508589 request 
void CChildFrame::OnUpdateViewMaintoolbar(CCmdUI* pCmdUI)
{
   CMainFrame* pFrame = (CMainFrame*) GetParentFrame();
   pCmdUI->Enable();
   pCmdUI->SetCheck(pFrame->GetToolBarStatus());
}

void CChildFrame::OnUpdateViewGlobalToolbar(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
   pCmdUI->SetCheck(m_wndGlobalBar->IsVisible());
}

void CChildFrame::OnUpdateGlobalDisablePort(CCmdUI* pCmdUI) 
{  
   BOOL bEnable = FALSE;
   VTSPorts ports;
   VTSPorts* pPorts = ((VTSDoc*)GetActiveDocument())->GetPorts();
   ports.DeepCopy(pPorts);
   for (int i = 0; i < ports.GetSize(); i++)
   {
      VTSPortPtr     curPort = ports[i];
      if (curPort->IsEnabled())
      {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable(bEnable);
}

void CChildFrame::OnUpdateGlobalScriptChecksyntax(CCmdUI* pCmdUI) 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(!pFrame)
   {
      pCmdUI->Enable(FALSE);
      return;
   }
   pCmdUI->Enable( gExecutor.IsIdle() );
}

void CChildFrame::OnUpdateGlobalScriptReset(CCmdUI* pCmdUI) 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(!pFrame)
   {
      pCmdUI->Enable(FALSE);
      return;
   }
   pCmdUI->Enable( gExecutor.IsIdle() );
}

void CChildFrame::OnUpdateGlobalScriptRun(CCmdUI* pCmdUI) 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(!pFrame)
   {
      pCmdUI->Enable(FALSE);
      return;
   }
   if (gExecutor.IsIdle() && pFrame->m_pDoc->m_pContentTree)
      pCmdUI->Enable( pFrame->m_bSyntaxOK );
   else if (gExecutor.IsBound(pFrame->m_pDoc))
      pCmdUI->Enable( !gExecutor.IsRunning() );
   else
      pCmdUI->Enable( pFrame->m_bSyntaxOK );
}

void CChildFrame::OnUpdateGlobalScriptSteppass(CCmdUI* pCmdUI) 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(!pFrame)
   {
      pCmdUI->Enable(FALSE);
      return;
   }
   pCmdUI->Enable( gExecutor.IsStopped() && gExecutor.IsBound(pFrame->m_pDoc) );
}

void CChildFrame::OnUpdateGlobalScriptStepfail(CCmdUI* pCmdUI) 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(!pFrame)
   {
      pCmdUI->Enable(FALSE);
      return;
   }
   pCmdUI->Enable( gExecutor.IsStopped() && gExecutor.IsBound(pFrame->m_pDoc) );
}

void CChildFrame::OnUpdateGlobalScriptStep(CCmdUI* pCmdUI) 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(!pFrame)
   {
      pCmdUI->Enable(FALSE);
      return;
   }

   if (gExecutor.IsIdle() && pFrame->m_pDoc->m_pContentTree)
   {
      if (pFrame->m_pDoc->SelectedTest() != NULL)
         pCmdUI->Enable( true );
      else
         pCmdUI->Enable( false );
   }
   else
   {
      if (gExecutor.IsBound(pFrame->m_pDoc))
         pCmdUI->Enable( !gExecutor.IsRunning() );
      else
         pCmdUI->Enable( false );
   }
}

void CChildFrame::OnUpdateGlobalScriptKill(CCmdUI* pCmdUI) 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(!pFrame)
   {
      pCmdUI->Enable(FALSE);
      return;
   }
   pCmdUI->Enable( gExecutor.IsBound(pFrame->m_pDoc) );
}

void CChildFrame::OnUpdateGlobalScriptHalt(CCmdUI* pCmdUI) 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(!pFrame)
   {
      pCmdUI->Enable(FALSE);
      return;
   }
   pCmdUI->Enable( gExecutor.IsRunning() && gExecutor.IsBound(pFrame->m_pDoc) ); 
}

void CChildFrame::OnGlobalScriptChecksyntax() 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(pFrame)
      pFrame->PostMessage(WM_COMMAND, ID_SCRIPT_CHECK_SYNTAX);
}

void CChildFrame::OnGlobalScriptReset() 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(pFrame)
      pFrame->PostMessage(WM_COMMAND, ID_SCRIPT_RESET);  
}

void CChildFrame::OnGlobalScriptRun() 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(pFrame)
      pFrame->PostMessage(WM_COMMAND, ID_SCRIPT_RUN); 
}

void CChildFrame::OnGlobalScriptSteppass() 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   pFrame->PostMessage(WM_COMMAND, ID_SCRIPT_STEPPASS);
}

void CChildFrame::OnGlobalScriptStepfail() 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(pFrame)
      pFrame->PostMessage(WM_COMMAND, ID_SCRIPT_STEPFAIL);
}

void CChildFrame::OnGlobalScriptStep() 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(pFrame)
      pFrame->PostMessage(WM_COMMAND, ID_SCRIPT_STEP);
}

void CChildFrame::OnGlobalScriptKill() 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(pFrame)
      pFrame->PostMessage(WM_COMMAND, ID_SCRIPT_KILL);
}

void CChildFrame::OnGlobalScriptHalt() 
{
   ScriptFrame* pFrame = (ScriptFrame*)((CMainFrame*) GetParentFrame())->GetChildFrame(RUNTIME_CLASS(ScriptFrame));
   if(pFrame)
      pFrame->PostMessage(WM_COMMAND, ID_SCRIPT_HALT);
}

void CChildFrame::OnGlobalDisablePort() 
{
   int i;
   VTSPorts ports;
   VTSPorts* pPorts = ((VTSDoc*)GetActiveDocument())->GetPorts();
   ports.DeepCopy(pPorts);
   for (i = 0; i < ports.GetSize(); i++)
   {
      VTSPortPtr     curPort = ports[i];
      if (curPort->IsEnabled())
         curPort->SetEnabled(false);
   }
   pPorts->KillContents(); 
   for (i = 0; i < ports.GetSize(); i++ )
      pPorts->Add(ports[i]);
   ports.RemoveAll();
   ((VTSDoc*)GetActiveDocument())->FixupNameToPortLinks(false);         // must refixup while ports are inactive...
   ((VTSDoc*)GetActiveDocument())->FixupFiltersToPortLinks(false);
   ((VTSDoc*)GetActiveDocument())->ReActivateAllPorts();
}

void CChildFrame::OnViewMaintoolbar() 
{
   CMainFrame* pFrame = (CMainFrame*) GetParentFrame();
   pFrame->ShowOwnToolBar();
}

void CChildFrame::OnViewGlobalToolbar() 
{
   ShowControlBar(m_wndGlobalBar, !m_wndGlobalBar->IsVisible(), FALSE);
}
