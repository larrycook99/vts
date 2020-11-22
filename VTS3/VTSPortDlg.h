#if !defined(AFX_VTSPORTDLG_H__DF2DB570_3513_11D4_BEA5_00A0C95A9812__INCLUDED_)
#define AFX_VTSPORTDLG_H__DF2DB570_3513_11D4_BEA5_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPortDlg.h : header file
//

#include "VTSDoc.h"
#include "VTSPortIPDialog.h"
#include "VTSPortEthernetDialog.h"
#include "VTSPortPTPDialog.h"
#include "VTSPortARCNETDialog.h"
#include "VTSPortMSTPDialog.h"

/////////////////////////////////////////////////////////////////////////////
// VTSPortDlg dialog

#define PORT_PAGE_MAX      6

class VTSPortDlg : public CDialog, public VTSPageCallbackInterface
{
   CPropertyPage * m_ppages[PORT_PAGE_MAX];
   CString       * m_pstrPageData[PORT_PAGE_MAX];
   bool        m_fModified;

   // mirrors manipulation of local m_ports array so we can tell which ports have been modified, added
   // and change status

   CPtrArray      m_apOriginalPort;

   void ReloadTypeData( int nActiveType, CString * pstr );
   void DeactivateChangedPorts(void);
   void MoveLocalPortsToMain(void);
   void CopyMainPortArray(void);
   void EnableOK(void);

// Construction
public:
   VTSPortDlg( VTSDoc * pdoc );
   ~VTSPortDlg();

   CPropertySheet          m_sheet;
   VTSPortNullDialog       m_pageNull;
   VTSPortIPDialog         m_pageIP;
   VTSPortEthernetDialog   m_pageEthernet;
   VTSPortARCNETDialog     m_pageARCNET;
   VTSPortMSTPDialog       m_pageMSTP;
   VTSPortPTPDialog        m_pagePTP;
   
   void PortStatusChange( void );                  // status of some port has changed

   virtual void DataChangeNotification(void);      // call back from pages
   virtual void * GetActiveData(void);

// Dialog Data
   //{{AFX_DATA(VTSPortDlg)
   enum { IDD = IDD_PORT };
   CListCtrl   m_PortList;
   CString     m_strName;
   BOOL        m_fEnabled;
   int         m_nNetwork;
   CComboBox   m_DeviceCombo;
   //}}AFX_DATA

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(VTSPortDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   VTSDevices *      m_pdevices;                // pointer to list of devices
   VTSPorts *        m_pports;
   VTSDoc *          m_pdoc;
   VTSPorts          m_ports;                   // new copy of ports...

   int               m_iSelectedPort;           // index of selected port
   CImageList        m_ilStatus;                // status images

   void SetSelection( int indx );
   void ResetSelection( void );
   void SynchronizeControls( void );

   void UpdatePortListItem( int nIndex, VTSPort * pport );
   afx_msg void SaveChanges();                     // save changes in port desc

   // Generated message map functions
   //{{AFX_MSG(VTSPortDlg)
   afx_msg void OnNewPort();
   virtual BOOL OnInitDialog();
   afx_msg void OnItemchangingPortList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDblclkPortList(NMHDR* pNMHDR, LRESULT* pResult);
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnDelete();
   afx_msg void OnApplyNow();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPORTDLG_H__DF2DB570_3513_11D4_BEA5_00A0C95A9812__INCLUDED_)
