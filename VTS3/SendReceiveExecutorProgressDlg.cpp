// SendReceiveExecutorProgressDlg.cpp : implementation file
//
// Simple status and control dialog for use by SendReceiveExecutor and subclasses

#include "stdafx.h"
#include "vts.h"
#include "SendReceiveExecutor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SendReceiveExecutorProgressDlg dialog

SendReceiveExecutorProgressDlg::SendReceiveExecutorProgressDlg( SendReceiveExecutor *pExecutor,
                                                                const char          *pTitle,
                                                                CWnd                *pParent /*=NULL*/)
: CDialog(SendReceiveExecutorProgressDlg::IDD, pParent)
, m_pExecutor(pExecutor)
, m_title(pTitle)
{
   //{{AFX_DATA_INIT(SendReceiveExecutorProgressDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void SendReceiveExecutorProgressDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SendReceiveExecutorProgressDlg)
   DDX_Control(pDX, IDOK, m_okCtrl);
   DDX_Control(pDX, IDC_BACKUP_RESTORE_KILL, m_killCtrl);
   DDX_Control(pDX, IDC_BACKUP_RESTORE_OUTPUT, m_editOutput);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SendReceiveExecutorProgressDlg, CDialog)
   //{{AFX_MSG_MAP(SendReceiveExecutorProgressDlg)
   ON_BN_CLICKED(IDC_BACKUP_RESTORE_KILL, OnKill)
   ON_WM_SIZE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SendReceiveExecutorProgressDlg message handlers

void SendReceiveExecutorProgressDlg::OutMessage(const CString& msg, BOOL bnewLine /*= TRUE*/)
{
   CString str(msg);
   if (bnewLine)
   {
      str += "\r\n";
   }
   else
   {
      str += "\t";
   }
   m_editOutput.SetSel(-1, -1);
   m_editOutput.ReplaceSel(str);
   m_editOutput.SetSel(-1, -1);
}

void SendReceiveExecutorProgressDlg::OnOK()
{
   DestroyWindow ();
}

void SendReceiveExecutorProgressDlg::OnCancel()
{
   DestroyWindow ();
}

void SendReceiveExecutorProgressDlg::PostNcDestroy()
{
   CDialog::PostNcDestroy();
   m_pExecutor->DestroyOutputDlg();
   delete this;
}

void SendReceiveExecutorProgressDlg::OnKill() 
{
   m_pExecutor->Kill();
   m_killCtrl.EnableWindow(FALSE);
   m_okCtrl.EnableWindow(TRUE);
   CMenu* pMenu = GetSystemMenu(FALSE);
   pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);
}


void SendReceiveExecutorProgressDlg::BeginTestProcess()
{
   CString str;
   str.Format( "Beginning %s...", (LPCTSTR)m_title );
   OutMessage(str);
   m_killCtrl.EnableWindow(TRUE);
   m_okCtrl.EnableWindow(FALSE);
   CMenu* pMenu = GetSystemMenu(FALSE);
   pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
}


void SendReceiveExecutorProgressDlg::EndTestProcess()
{
   m_killCtrl.EnableWindow(FALSE);
   m_okCtrl.EnableWindow(TRUE);
   CMenu* pMenu = GetSystemMenu(FALSE);   
   pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);
}

BOOL SendReceiveExecutorProgressDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // Set our title
   SetWindowText( m_title );

   // Make sure our output window has lots of room
   m_editOutput.SetLimitText( ~0 );
   
   m_killCtrl.EnableWindow(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void SendReceiveExecutorProgressDlg::OnSize(UINT nType, int cx, int cy) 
{
   CDialog::OnSize(nType, cx, cy);
   
   if (m_killCtrl || m_okCtrl || m_editOutput)
   { 
      // Get size of the dialog
      void* p = &m_killCtrl;
      CRect rect;
      GetClientRect(&rect);
      // get size of the kill button
      CRect rectKill;
      m_killCtrl.GetClientRect(&rectKill);
      // get size of the ok button
      CRect rectOk;
      m_okCtrl.GetClientRect(&rectOk);
      // get size of the output window
      CRect rectOutput;
      m_editOutput.GetClientRect(&rectOutput);
      // move the position and resize output
      m_editOutput.MoveWindow(15, 15, rect.Width()-30, rect.Height()-15-(rectOk.Height())-30);
      // move and resize kill button
      m_killCtrl.MoveWindow((int)(0.339*rect.Width()-0.5*rectKill.Width()), rect.Height()-15-(rectKill.Height()), 
         rectKill.Width(), rectKill.Height());
      // move and resize ok button
      m_okCtrl.MoveWindow((int)(0.661*rect.Width()-0.5*rectOk.Width()), rect.Height()-15-(rectOk.Height()), 
         rectOk.Width(), rectOk.Height());
   }
}
