// VTSDiscoveryDlg.cpp: Dialog for Discovery / EPICS generation
// John Hartman, 15 August 2014
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSDiscoveryDlg.h"
#include "VTSDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDiscoveryDlg dialog

VTSDiscoveryDlg::VTSDiscoveryDlg(bool doEpics,
                                 const VTSNames& names, const VTSPorts& ports,
                                 CWnd* pParent /*=NULL*/)
: CDialog(VTSDiscoveryDlg::IDD, pParent)
, m_names(names)
, m_ports(ports)
, m_doEpics(doEpics)
{
   //{{AFX_DATA_INIT(VTSDiscoveryDlg)
   m_strFileName = _T("");
   m_strDevice = _T("");
   m_strPort = _T("");
   m_strDevObjInst = _T("");
   m_includeAllValues = 0;
   m_includeUnsupported = 0;
   m_writeBack = 0;
   //}}AFX_DATA_INIT
}

void VTSDiscoveryDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(VTSDiscoveryDlg)
   DDX_Control(pDX, IDC_PORTCOMBO, m_portCtrl);
   DDX_CBString(pDX, IDC_PORTCOMBO, m_strPort);
   DDX_Control(pDX, IDC_DEVICECOMBO, m_deviceCtrl);
   DDX_CBString(pDX, IDC_DEVICECOMBO, m_strDevice);
   DDX_Text(pDX, IDC_FILENAME, m_strFileName);
   DDX_Text(pDX, IDC_DEVICEOBJINST, m_strDevObjInst);
   DDV_MaxChars(pDX, m_strDevObjInst, 7);
   DDX_Check(pDX, IDC_CHECK1, m_includeAllValues);
   DDX_Check(pDX, IDC_CHECK2, m_includeUnsupported);
   DDX_Check(pDX, IDC_CHECK3, m_writeBack);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(VTSDiscoveryDlg, CDialog)
   //{{AFX_MSG_MAP(VTSDiscoveryDlg)
   ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
   ON_CBN_SELENDOK(IDC_DEVICECOMBO, OnSelendokDevicecombo)
   ON_CBN_SELENDOK(IDC_PORTCOMBO, OnSelendokPortcombo)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDiscoveryDlg message handlers

BOOL VTSDiscoveryDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   int i;
   VTSPortPtr pPort;
   for (i = 0; i < m_ports.GetSize(); i++)
   {
      pPort = (VTSPort *) m_ports.GetAt(i);
      if (pPort->IsEnabled())
      {
         m_portCtrl.AddString( pPort->m_strName );
         m_portCtrl.SelectString(-1, pPort->m_strName);
      }
   }

   const char *pSelName = "IUT";
   if (m_doEpics)
   {
      for (i = 0 ; i < m_names.GetSize(); i++)
      {
         // Skip broadcast names
         VTSName *pName = m_names.GetAt(i);
         if ((pName->m_bacnetaddr.addrType == localStationAddr) ||
             (pName->m_bacnetaddr.addrType == remoteStationAddr))
         {
            m_deviceCtrl.AddString( pName->m_strName );
         }
      }
   }
   else
   {
      // Discovery dialog gets all names, selects first global broadcast, if any
      int globalBroadcastIndex = 0;
      for (i = 0 ; i < m_names.GetSize(); i++)
      {
         VTSName *pName = m_names.GetAt(i);
         m_deviceCtrl.AddString( pName->m_strName );

         // Remember any global broadcast address as the default for Discovery
         if (pName->m_bacnetaddr.addrType == globalBroadcastAddr)
         {
            pSelName = pName->m_strName;
         }
      }
   }
   m_deviceCtrl.SelectString(-1, pSelName);

   // Hide or show EPICS-related controls
   int show = (m_doEpics) ? SW_SHOW : SW_HIDE;
   GetDlgItem( IDC_STATIC3 )->ShowWindow( show );
   GetDlgItem( IDC_STATIC4 )->ShowWindow( show );
   GetDlgItem( IDC_DEVICEOBJINST )->ShowWindow( show );
   GetDlgItem( IDC_CHECK1 )->ShowWindow( show );
   GetDlgItem( IDC_CHECK2 )->ShowWindow( show );
   GetDlgItem( IDC_CHECK3 )->ShowWindow( show );
   GetDlgItem( IDC_STATIC5 )->ShowWindow( show );
   GetDlgItem( IDC_FILENAME )->ShowWindow( show );
   GetDlgItem( IDC_BROWSE )->ShowWindow( show );

   if (!m_doEpics)
   {
      SetWindowText( "Discover Devices" );
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void VTSDiscoveryDlg::OnBrowse()
{
   CFileDialog fd( FALSE, "tpi", NULL,
                   OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                   "EPICS Files (*.tpi)|*.tpi|All Files (*.*)|*.*||" );
   if (fd.DoModal() == IDOK)
   {
      m_strFileName = fd.GetPathName();
      CWnd* pWnd = GetDlgItem(IDC_FILENAME);
      pWnd->SetWindowText(m_strFileName);
   }

}

void VTSDiscoveryDlg::OnOK()
{
   UpdateData();
   if (m_strPort.IsEmpty())
   {
      AfxMessageBox("VTS port must be specified!");
      m_portCtrl.SetFocus();
      return;
   }
   if (m_strDevice.IsEmpty())
   {
      AfxMessageBox("Device must be specified!");
      m_portCtrl.SetFocus();
      return;
   }

   // Optional device instance: empty control determines it automatically
   if (!m_strDevObjInst.IsEmpty())
   {
      int num = atoi(m_strDevObjInst);
      if (num < 0 || num > 4194302)
      {
         AfxMessageBox("Device Obj Instance number must be between 0 and 4194302!");
         m_portCtrl.SetFocus();
         GetDlgItem(IDC_DEVICEOBJINST)->SetFocus();
         return;
      }
   }

   // Only require this is we ONLY generate EPICS
//   if (m_strFileName.IsEmpty())
//   {
//      AfxMessageBox("Filename must be specified!");
//      GetDlgItem(IDC_BACKUPFILENAME)->SetFocus();
//      return;
//   }

   CDialog::OnOK();
}

void VTSDiscoveryDlg::OnSelendokDevicecombo()
{
   CString strDevice;
   m_deviceCtrl.GetWindowText(strDevice);
   CString strPort;
   m_portCtrl.GetWindowText(strPort);

   // Consisteny check between Device and Port
   VTSPortPtr pPort;
   int index = const_cast<VTSNames&>(m_names).FindIndex(strDevice);
   ASSERT(index != -1);
   VTSName* pName = m_names.GetAt(index);
   if (pName->m_pportLink != NULL)
   {
      pPort = pName->m_pportLink;
      if (pPort->m_strName.CompareNoCase(strPort))
      {
         m_portCtrl.SetCurSel(-1);
      }
   }
}

void VTSDiscoveryDlg::OnSelendokPortcombo()
{
   CString strDevice;
   m_deviceCtrl.GetWindowText(strDevice);
   CString strPort;
   m_portCtrl.GetWindowText(strPort);

   // Consisteny check between Device and Port
   if (!strDevice.IsEmpty())
   {
      VTSPortPtr pPort;
      int index = const_cast<VTSNames&>(m_names).FindIndex(strDevice);
      ASSERT(index != -1);
      VTSName* pName = m_names.GetAt(index);
      if (pName->m_pportLink != NULL)
      {
         pPort = pName->m_pportLink;
         if (pPort->m_strName.CompareNoCase(strPort)) 
         {
            m_deviceCtrl.SetCurSel(-1);
         }
      }
   }
}
