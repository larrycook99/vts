// ScriptFrame.cpp : implementation of the ScriptFrame class
//

#include "stdafx.h"
#include "VTS.h"

#include "ScriptDocument.h"
#include "ScriptFrame.h"
#include "ScriptBase.h"
#include "ScriptExecutor.h"
//MAD_DB Not needed anymore with single session VTS
//#include "ScriptSelectSession.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptFrame

//madanner 5/03
//IMPLEMENT_DYNCREATE(ScriptFrame, CMDIChildWnd)
IMPLEMENT_DYNCREATE(ScriptFrame, CMDIFrameWnd)

//madanner 5/03 BEGIN_MESSAGE_MAP(ScriptFrame, CMDIChildWnd)
BEGIN_MESSAGE_MAP(ScriptFrame, CMDIFrameWnd)
   //{{AFX_MSG_MAP(ScriptFrame)
   ON_UPDATE_COMMAND_UI(ID_SCRIPT_CHECK_SYNTAX, OnUpdateScriptCheckSyntax)
   ON_UPDATE_COMMAND_UI(ID_SCRIPT_ENV, OnUpdateScriptEnvironment)
   ON_UPDATE_COMMAND_UI(ID_SCRIPT_RUN, OnUpdateScriptRun)
   ON_UPDATE_COMMAND_UI(ID_SCRIPT_HALT, OnUpdateScriptHalt)
   ON_UPDATE_COMMAND_UI(ID_SCRIPT_STEP, OnUpdateScriptStep)
   ON_UPDATE_COMMAND_UI(ID_SCRIPT_STEPPASS, OnUpdateScriptStepPass)
   ON_UPDATE_COMMAND_UI(ID_SCRIPT_STEPFAIL, OnUpdateScriptStepFail)
   ON_UPDATE_COMMAND_UI(ID_SCRIPT_KILL, OnUpdateScriptKill)
   ON_UPDATE_COMMAND_UI(ID_SCRIPT_RESET, OnUpdateScriptReset)
   ON_COMMAND(ID_SCRIPT_CHECK_SYNTAX, OnScriptCheckSyntax)
   ON_COMMAND(ID_SCRIPT_ENV, OnScriptEnvironment)
   ON_COMMAND(ID_SCRIPT_RUN, OnScriptRun)
   ON_COMMAND(ID_SCRIPT_HALT, OnScriptHalt)
   ON_COMMAND(ID_SCRIPT_STEP, OnScriptStep)
   ON_COMMAND(ID_SCRIPT_STEPPASS, OnScriptStepPass)
   ON_COMMAND(ID_SCRIPT_STEPFAIL, OnScriptStepFail)
   ON_COMMAND(ID_SCRIPT_KILL, OnScriptKill)
   ON_COMMAND(ID_SCRIPT_RESET, OnScriptReset)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptFrame construction/destruction

ScriptFrame::ScriptFrame()
   : m_pDoc(0), m_bSyntaxOK(FALSE)
{
}

ScriptFrame::~ScriptFrame()
{
}

BOOL ScriptFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: Modify the Window class or styles here by modifying
   //  the CREATESTRUCT cs

//madanner 5/03   if( !CMDIChildWnd::PreCreateWindow(cs) )

   if( !CMDIFrameWnd::PreCreateWindow(cs) )
      return FALSE;

   return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// ScriptFrame diagnostics

#ifdef _DEBUG
void ScriptFrame::AssertValid() const
{
//madanner 5/03   CMDIChildWnd::AssertValid();
   CMDIFrameWnd::AssertValid();
}

void ScriptFrame::Dump(CDumpContext& dc) const
{
//madanner 5/03   CMDIChildWnd::Dump(dc);
   CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ScriptFrame message handlers

static UINT Script_indicators[] =
{
   ID_SEPARATOR,           // status line indicator
// Added by Yajun Zhou, 2002-4-22
   ID_LNCOLINDEX,
/////////////////////////
   ID_INDICATOR_CAPS,
   ID_INDICATOR_NUM,
   ID_INDICATOR_SCRL,
};


void ScriptFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
   CFrameWnd::OnUpdateFrameTitle(bAddToTitle);
}


BOOL ScriptFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
   // save a pointer to the document
   m_pDoc = (ScriptDocument*)pContext->m_pCurrentDoc;

   // Attempt to create toolbar and status bars
   // madanner 5/03

   if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP  | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_VTSTYPE))
   {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
   }

   if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(Script_indicators,  sizeof(Script_indicators)/sizeof(UINT)))
   {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
   }

   // TODO: Delete these three lines if you don't want the toolbar to
   //  be dockable
   m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );
   EnableDocking( CBRS_ALIGN_ANY );
   DockControlBar( &m_wndToolBar );

   // create a splitter with 2 rows, 1 column
   if (!m_wndSplit1.CreateStatic(this, 2, 1))
   {
      TRACE0("Failed to CreateStaticSplitter\n");
      return FALSE;
   }

   // add the first splitter pane - the edit view in row 0
// Changed by Yajun Zhou, 2002-4-22
// if (!m_wndSplit1.CreateView(0, 0,
//    RUNTIME_CLASS(CEditView), CSize(0, 300), pContext))

   RECT  rt;
   GetWindowRect(&rt);

   // adjust size:  left = full width, top = 1/3 of full height
   rt.left = rt.right - rt.left;
   rt.top = (rt.bottom - rt.top) / 3;

// if (!m_wndSplit1.CreateView(0, 0, RUNTIME_CLASS(ScriptEdit), CSize(0, 300), pContext))
   if (!m_wndSplit1.CreateView(0, 0, RUNTIME_CLASS(ScriptEdit), CSize(rt.left, rt.top * 2), pContext))
   {
      TRACE0("Failed to create edit pane\n");
      return FALSE;
   }
   
   // nice to have around
// Changed by Yajun Zhou, 2002-4-22
// m_pEditView = (CEditView*)m_wndSplit1.GetPane( 0, 0 );
   m_pEditView = (ScriptEdit*)m_wndSplit1.GetPane( 0, 0 );

   //madanner 5/03
   // Hack to get frame because we're not really a frame window
   m_pEditView->SetFrame(this);
//////////////////////////////////////////////////////////

   // pass it along to the document
   m_pDoc->m_editData = &m_pEditView->GetEditCtrl();

   // add the second splitter pane - which is a nested splitter with 2 columns
   if (!m_wndSplit2.CreateStatic(
      &m_wndSplit1,     // our parent window is the first splitter
      1, 2,               // the new splitter is 1 row, 2 columns
      WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
      m_wndSplit1.IdFromRowCol(1, 0)
         // new splitter is in the second row, first column of first splitter
      ))
   {
      TRACE0("Failed to create nested splitter\n");
      return FALSE;
   }

   // now create the two views inside the nested splitter
// int cxHalf = max(lpcs->cx / 2, 20);

   // tree view gets left half
// if (!m_wndSplit2.CreateView(0, 0, RUNTIME_CLASS(ScriptContentTree), CSize(cxHalf, 0), pContext))
   if (!m_wndSplit2.CreateView(0, 0, RUNTIME_CLASS(ScriptContentTree), CSize(rt.left/2, rt.top), pContext))
   {
      TRACE0("Failed to create tree pane\n");
      return FALSE;
   }

   // nice to have around, and pass along to the document
   m_pContentTree = (ScriptContentTree*)m_wndSplit2.GetPane( 0, 0 );
   m_pContentTree->m_pDoc = m_pDoc;
   m_pContentTree->SetFrame(this);

   m_pDoc->m_pContentTree = m_pContentTree;
// Added by Yajun Zhou, 2002-6-20
   m_pContentTree->m_pEditView = m_pEditView;
//////////////////////////////////////////////////////////

   // list view gets right half
   if (!m_wndSplit2.CreateView(0, 1, RUNTIME_CLASS(ScriptParmList), CSize(0, 0), pContext))
   {
      TRACE0("Failed to create parameter list pane\n");
      return FALSE;
   }

   // nice to have around, and pass along to the document
   m_pParmList = (ScriptParmList*)m_wndSplit2.GetPane( 0, 1 );
   m_pDoc->m_pParmList = m_pParmList;

   // success
   return TRUE;
// return CreateClient(lpcs, NULL);
}


void ScriptFrame::SetLnPaneText(LPCSTR lpszText)
{
   int i = m_wndStatusBar.CommandToIndex(ID_LNCOLINDEX);
   m_wndStatusBar.SetPaneText(i, lpszText);
}

//
// ScriptFrame::OnUpdateScriptCheckSyntax
//
// The syntax for a script can only be checked if there is no executor,
// that is to say, nothing is running.
//

afx_msg void ScriptFrame::OnUpdateScriptCheckSyntax(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( gExecutor.IsIdle() );
}

//
// ScriptFrame::OnUpdateScriptEnvironment
//
// The menu is checked when the parameter list in this frame is the 
// current environment.
//

afx_msg void ScriptFrame::OnUpdateScriptEnvironment(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( true );
   pCmdUI->SetCheck( m_pParmList == gCurrentEnv );
}

//
// ScriptFrame::OnUpdateScriptRun
//
// The run command is enabled if the executor is idle and there is 
// content to run or the executor is bound to this document and it is
// not running.
//
// If a particular test is selected, then the run command is just for 
// that test, otherwise it is for all tests in the script.
//

afx_msg void ScriptFrame::OnUpdateScriptRun(CCmdUI* pCmdUI)
{
   if (gExecutor.IsIdle() && m_pDoc->m_pContentTree)
   {
      pCmdUI->Enable( m_bSyntaxOK );

      // Modified by Zhu Zhenhua, 2003-12-18, to run select section
      if (m_pDoc->SelectedSection() != NULL)
      {
         CString  str( "&Run " );

         str += m_pDoc->SelectedSection()->baseLabel;
         str += "\tF5";

         pCmdUI->SetText( str );
      }
      else if (m_pDoc->SelectedTest() != NULL)
      {
         CString  str( "&Run " );

         str += m_pDoc->SelectedTest()->baseLabel;
         str += "\tF5";

         pCmdUI->SetText( str );
      }
      else
      {
         pCmdUI->SetText( "&Run all tests\tF5" );
      }
   } 
   else if (gExecutor.IsBound(m_pDoc)) 
   {
      pCmdUI->SetText( "&Resume" );
      pCmdUI->Enable( !gExecutor.IsRunning() );
   } 
   else
   {
      pCmdUI->SetText( "Run\tF5" );
      pCmdUI->Enable( m_bSyntaxOK );
   }
}

//
// ScriptFrame::OnUpdateScriptHalt
//
// The halt menu option is only enabled if there is a running script associated with 
// this frame.
//

afx_msg void ScriptFrame::OnUpdateScriptHalt(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( gExecutor.IsRunning() && gExecutor.IsBound(m_pDoc) );
}

//
// ScriptFrame::OnUpdateScriptStep
//
// The step command is similar to the run command.  It will allow a single step to 
// be processed when this script is associated with the executor but is not running.
//

afx_msg void ScriptFrame::OnUpdateScriptStep(CCmdUI* pCmdUI)
{
   if (gExecutor.IsIdle() && m_pDoc->m_pContentTree)
   {
      if (m_pDoc->SelectedTest() != NULL)
      {
         CString  str( "&Step " );

         str += m_pDoc->SelectedTest()->baseLabel;
         str += "\tF6";

         pCmdUI->SetText( str );
         pCmdUI->Enable( true );
      }
      else
      {
         pCmdUI->SetText( "Step\tF6" );
         pCmdUI->Enable( false );
      }
   }
   else if (gExecutor.IsBound(m_pDoc))
   {
      pCmdUI->SetText( "&Step\tF6" );
      pCmdUI->Enable( !gExecutor.IsRunning() );
   }
   else
   {
      pCmdUI->SetText( "Step\tF6" );
      pCmdUI->Enable( false );
   }
}

//
// ScriptFrame::OnUpdateScriptStepPass
//
// This step command is similar to the simple step, but the executor must be stopped.
//

afx_msg void ScriptFrame::OnUpdateScriptStepPass(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( gExecutor.IsStopped() && gExecutor.IsBound(m_pDoc) );
}

//
// ScriptFrame::OnUpdateScriptStepFail
//
// This step command is similar to the simple step, but the executor must be stopped.
//

afx_msg void ScriptFrame::OnUpdateScriptStepFail(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( gExecutor.IsStopped() && gExecutor.IsBound(m_pDoc) );
}

//
// ScriptFrame::OnUpdateScriptKill
//
// If there is a running script associated with this document, allow it to be 
// killed (even if it has not been halted first).
//

afx_msg void ScriptFrame::OnUpdateScriptKill(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( gExecutor.IsBound(m_pDoc) );
}

//
// ScriptFrame::OnUpdateScriptReset
//
// The reset command can only be executed if there is no script running.
//

afx_msg void ScriptFrame::OnUpdateScriptReset(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( gExecutor.IsIdle() );
}

//
// ScriptFrame::OnScriptCheckSyntax
//

void ScriptFrame::OnScriptCheckSyntax() 
{
   // This may take a while for large scripts
   CWaitCursor cur;
   m_bSyntaxOK = m_pDoc->CheckSyntax();
}

//
// ScriptFrame::OnScriptEnvironment
//

void ScriptFrame::OnScriptEnvironment()
{
   ScriptParmListPtr oldEnv;

   // remember the current environment
   oldEnv = gCurrentEnv;

   // reset the current environment and unload everyone else
   if (gCurrentEnv) {
      gCurrentEnv->ResetCurrentEnv();

      for (int i = 0; i < gScriptParmLists.Length(); i++) {
         ScriptParmListPtr splp = gScriptParmLists[i];

         if (splp != gCurrentEnv)
            splp->UnloadEnv();
      }

      gCurrentEnv = 0;
   }

   // if this becomes the new environment, set it and load everyone else
   if (m_pParmList != oldEnv) {
      gCurrentEnv = m_pParmList;

      gCurrentEnv->SetCurrentEnv();

      for (int j = 0; j < gScriptParmLists.Length(); j++) {
         ScriptParmListPtr splp = gScriptParmLists[j];

         if (splp != gCurrentEnv)
            splp->LoadEnv();
      }
   }
}

//
// ScriptFrame::OnScriptRun
//

void ScriptFrame::OnScriptRun() 
{
   // if the test is already running, ignore this (probably sync issue)
   if (gExecutor.IsRunning()) {
      TRACE0( "Already running\n" );
      return;
   }

   // if the test has been stopped, allow it to continue
   if (gExecutor.IsStopped()) {
      TRACE0( "Resume\n" );
      gExecutor.Resume();
      return;
   }

   // new test to run
   TRACE0( "New run\n" );

   // if it can be set up, let it run
   if (DoInitialSetup())
      gExecutor.Run();
}

//
// ScriptFrame::OnScriptHalt
//

void ScriptFrame::OnScriptHalt() 
{
   // make sure the executor is bound and running
   if (gExecutor.IsIdle()) {
      TRACE0( "Error: executor is idle\n" );
      return;
   }
   if (!gExecutor.IsRunning()) {
      TRACE0( "Error: executor not running\n" );
      return;
   }

   // stop the executor
   gExecutor.Halt();
}

//
// ScriptFrame::OnScriptStep
//

void ScriptFrame::OnScriptStep() 
{
   // if it is idle, bind it to the selected test and single step
   if (gExecutor.IsIdle()) {
      // new test to run
      TRACE0( "Step into new run\n" );

      // get the executor ready
      if (!DoInitialSetup())
         return;
   } else
   // if it is running, toss an error
   if (gExecutor.IsRunning()) {
      TRACE0( "Error: executor running\n" );
      return;
   }

   // let it run for a packet
   gExecutor.Step();
}

//
// ScriptFrame::OnScriptStepPass
//

void ScriptFrame::OnScriptStepPass() 
{
   if (!gExecutor.IsStopped()) {
      // new test to run
      TRACE0( "Error: executor is not stopped\n" );
      return;
   }

   // assume current packet passed
   gExecutor.Step( true );
}

//
// ScriptFrame::OnScriptStepFail
//

void ScriptFrame::OnScriptStepFail() 
{
   if (!gExecutor.IsStopped()) {
      // new test to run
      TRACE0( "Error: executor is not stopped\n" );
      return;
   }

   // assume current packet failed
   gExecutor.Step( false );
}

//
// ScriptFrame::OnScriptKill
//

void ScriptFrame::OnScriptKill() 
{
   // verify the executor is running or stopped
   if (gExecutor.IsIdle()) {
      TRACE0( "Error: executor is idle\n" );
      return;
   }

   // kill the test, executor returns to idle
   gExecutor.Kill( false );
}

//
// ScriptFrame::OnScriptReset
//

void ScriptFrame::OnScriptReset() 
{
   // make sure the executor is idle
   if (!gExecutor.IsIdle()) {
      TRACE0( "Error: executor is not idle\n" );
      return;
   }

   // let the document reset
   m_pDoc->Reset();
}

//
// ScriptFrame::DoInitialSetup
//

bool ScriptFrame::DoInitialSetup( void )
{
   VTSDocPtr         vdp = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();

   // find a database for test messages
   if ( vdp == NULL ) {
      // alert the user
      AfxMessageBox( _T("No session database available to receive test results") );
      return false;
   }

   // tell the executor to run the test
   gExecutor.Setup( vdp, m_pDoc, m_pDoc->SelectedTest() );

   // success
   return true;
}

//
// ScriptFrame::Serialize
//

void ScriptFrame::Serialize(CArchive& ar) 
{
   if (ar.IsStoring())
   {  // storing code
   }
   else
   {  // loading code
   }
}
