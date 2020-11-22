#include "afxcmn.h"
#if !defined(AFX_SENDRECEIVEEXECUTORPROGRESSDLG_H__912FB78F_CA73_47CC_8157_3F9E32F26F6D__INCLUDED_)
#define AFX_SENDRECEIVEEXECUTORPROGRESSDLG_H__912FB78F_CA73_47CC_8157_3F9E32F26F6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendReceiveExecutorProgressDlg.h : header file
//

class SendReceiveExecutor;

/////////////////////////////////////////////////////////////////////////////
// SendReceiveExecutorProgressDlg dialog

class SendReceiveExecutorProgressDlg : public CDialog
{
// Construction
public:
   SendReceiveExecutorProgressDlg( SendReceiveExecutor *pExecutor,
                                   const char          *pTitle,
                                   CWnd                *pParent = NULL );

   void OutMessage(const CString& msg, BOOL bnewLine = TRUE);
   void BeginTestProcess(void);
   void EndTestProcess(void);
// Dialog Data
   //{{AFX_DATA(SendReceiveExecutorProgressDlg)
   enum { IDD = IDD_SEND_RECEIVE_PROGRESS };
   CButton  m_okCtrl;
   CButton  m_killCtrl;
   CEdit   m_editOutput;
   //}}AFX_DATA

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SendReceiveExecutorProgressDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL
   virtual void OnOK();
   virtual void OnCancel();

// Implementation
protected:
   SendReceiveExecutor  *m_pExecutor;
   CString              m_title;

   // Generated message map functions
   //{{AFX_MSG(SendReceiveExecutorProgressDlg)
   afx_msg void OnKill();
   virtual BOOL OnInitDialog();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDRECEIVEEXECUTORPROGRESSDLG_H__912FB78F_CA73_47CC_8157_3F9E32F26F6D__INCLUDED_)
