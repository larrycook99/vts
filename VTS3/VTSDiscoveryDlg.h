// VTSDiscoveryDlg.h: Dialog for Discovery / EPICS generation
// John Hartman, 15 August 2014
//////////////////////////////////////////////////////////////////////

#include "afxwin.h"
#if !defined(AFX_VTSDISCOVERYDLG_H__82C2A51F_AD40_42AD_B7BB_336533F2279E__INCLUDED_)
#define AFX_VTSDISCOVERYDLG_H__82C2A51F_AD40_42AD_B7BB_336533F2279E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDiscoveryDlg.h : header file
//

class VTSNames;
class VTSPorts;
/////////////////////////////////////////////////////////////////////////////
// VTSDiscoveryDlg dialog

class VTSDiscoveryDlg : public CDialog
{
public:
   const VTSNames& m_names;
   const VTSPorts& m_ports;
// Construction
public:
   VTSDiscoveryDlg(bool doEpics, const VTSNames& names, const VTSPorts& ports, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(VTSDiscoveryDlg)
   enum { IDD = IDD_DISCOVER_EPICS };
   CComboBox   m_portCtrl;
   CComboBox   m_deviceCtrl;
   CString     m_strFileName;
   CString     m_strDevice;
   CString     m_strPort;
   CString     m_strDevObjInst;
   int         m_includeAllValues;
   int         m_includeUnsupported;
   int         m_writeBack;
   //}}AFX_DATA

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(VTSDiscoveryDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(VTSDiscoveryDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnBrowse();
   virtual void OnOK();
   afx_msg void OnSelendokDevicecombo();
   afx_msg void OnSelendokPortcombo();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   bool m_doEpics;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDISCOVERYDLG_H__82C2A51F_AD40_42AD_B7BB_336533F2279E__INCLUDED_)
