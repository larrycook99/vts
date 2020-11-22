// VTSPortIPDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "VTSDoc.h"
#include "WinIP.hpp"

#include "VTSPortIPDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSPortIPDialog dialog

IMPLEMENT_DYNCREATE(VTSPortIPDialog, VTSPropertyPage)

VTSPortIPDialog::VTSPortIPDialog( VTSPageCallbackInterface * pparent )
: VTSPropertyPage(VTSPortIPDialog::IDD, pparent)
{
   //{{AFX_DATA_INIT(VTSPortEthernetDialog)
   m_HostAddr = _T("");
   m_TTL = _T("");
   m_nPortType = 3;     // Default is simple B/IP
   //}}AFX_DATA_INIT

   m_pstrConfigParms = NULL;
}

VTSPortIPDialog::VTSPortIPDialog(void) : VTSPropertyPage()
{
   ASSERT(0);
}


VTSPortIPDialog::~VTSPortIPDialog()
{
}


void VTSPortIPDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(VTSPortIPDialog)
   DDX_Control(pDX, IDC_INTERFACE, m_interfaceCombo);
   DDX_Text(pDX, IDC_UDP_PORT, m_UDP_Port);
   DDX_Text(pDX, IDC_HOSTADDR, m_HostAddr);
   DDX_Text(pDX, IDC_TTL, m_TTL);
   DDX_Radio(pDX, IDC_RAW, m_nPortType);
   //}}AFX_DATA_MAP
}

BOOL VTSPortIPDialog::OnInitDialog()
{
   BOOL retval = CPropertyPage::OnInitDialog();
   if (retval)
   {
      WinIP::FillInterfaceCombo( m_interfaceCombo );
   }

   return retval;
}

BEGIN_MESSAGE_MAP(VTSPortIPDialog, VTSPropertyPage)
   //{{AFX_MSG_MAP(VTSPortIPDialog)
   ON_BN_CLICKED(IDC_RAW, OnDataChange)
   ON_BN_CLICKED(ID_BTRPEERS, OnBTRPeers)
   ON_BN_CLICKED(ID_BBMDPEERS, OnBBMDPeers)
   ON_BN_CLICKED(IDC_BTR, OnDataChange)
   ON_BN_CLICKED(IDC_BBMD, OnDataChange)
   ON_BN_CLICKED(IDC_BIP, OnDataChange)
   ON_BN_CLICKED(IDC_FOREIGN, OnDataChange)
   ON_EN_CHANGE(IDC_UDP_PORT, OnDataChange)
   ON_EN_CHANGE(IDC_HOSTADDR, OnDataChange)
   ON_EN_CHANGE(IDC_TTL, OnDataChange)
   ON_CBN_SELCHANGE(IDC_INTERFACE, OnDataChange)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL VTSPortIPDialog::OnSetActive()
{
   m_pstrConfigParms = (CString * ) RetrieveCurrentData();

   ObjToCtrl();
   CtrlToObj();      // must suck the data back out to rectify parm string

   // calls DDX
   VTSPropertyPage::OnSetActive();
   NotifyOfDataChange();

   SynchControls(m_pstrConfigParms != NULL);

   // if we return FALSE here then this page will not be activated.  It will move on to the next. 
   // That's good because it's extremely painful to disable all of the stuff.

   return m_pstrConfigParms != NULL;
}


BOOL VTSPortIPDialog::OnKillActive()
{
   // calls DDX
   VTSPropertyPage::OnKillActive();
   CtrlToObj();

   return TRUE;
}


void VTSPortIPDialog::OnDataChange()
{
   // sync the member variables with the dialog controls
   UpdateData( true );
   CtrlToObj();
   SynchControls(m_pstrConfigParms != NULL);
   NotifyOfDataChange();
}

void VTSPortIPDialog::SynchControls( int fEnable /* = true */ )
{
   GetDlgItem( IDC_HOSTADDR )->EnableWindow( fEnable && m_nPortType == 4 );
   GetDlgItem( IDC_TTL )->EnableWindow( fEnable && m_nPortType == 4 );
   GetDlgItem( ID_BTRPEERS )->EnableWindow( fEnable && m_nPortType == 1 );
   GetDlgItem( ID_BBMDPEERS )->EnableWindow( fEnable && m_nPortType == 2 );
}


void VTSPortIPDialog::ObjToCtrl()
{
   if ( m_pstrConfigParms == NULL )
      return;

   int    argc = 0;
   LPCSTR p = *m_pstrConfigParms;
   char   config[kVTSPortConfigLength], *src, *argv[16];

   // copy the configuration, split it up
   if (*p == 0)
      strcpy( config, "0xBAC0" );
   else
      strcpy( config, p );

   for (src = config; *src; ) {
      argv[argc++] = src;
      while (*src && (*src != ','))
         src++;

      if (*src == ',')
         *src++ = 0;
   }

   for (int i = argc; i < 16; i++)
      argv[i] = src;

   // For backwards compatibility, the text before the first comma
   // is "port;adapter"
   char *pIface = strchr( argv[0], ';' );
   if (pIface)
      *pIface++ = 0; // terminate port, point at interface

   // set the UDP port to the string up until the semicolon (or entire string)
   m_UDP_Port = argv[0];

   // set the interface to the string after the semicolon, or first entry
   if (pIface)
      m_interfaceCombo.SelectString( 0, pIface );
   else
      m_interfaceCombo.SetCurSel( 0 );

   if (argc == 1)
   {
      // if that's the only thing, that's OK: set to simple B/IP
      m_nPortType = 3;
   }
   else
   {
      m_nPortType = atoi( argv[1] );
      switch (m_nPortType) {
         case 0:           // raw
         case 1:           // BTR
         case 2:           // BBMD
         case 3:           // BIP simple
            break;
         case 4:
            m_HostAddr = argv[2];
            m_TTL = argv[3];
            break;
      }
   }
}


// When the user accepts the configuration information and clicks OK,
// this function is called which transfers the new configuration into
// the config data provided by the caller, VTSPort::Configuration.

void VTSPortIPDialog::CtrlToObj()
{
   if ( m_pstrConfigParms == NULL )
      return;

   *m_pstrConfigParms = m_UDP_Port;
   *m_pstrConfigParms += ';';

   // Get the interface name from the combo (may do some trimming)
   CString str;
   WinIP::GetInterfaceName( str, m_interfaceCombo );

   *m_pstrConfigParms += str;

   switch (m_nPortType)
   {
      case 0:           // Raw
         *m_pstrConfigParms += ",0";
         break;

      case 1:           // BTR [, ipaddr ]*
         *m_pstrConfigParms += ",1";
         break;

      case 2:           // BBMD [, ipaddr/host:socket ]*
         *m_pstrConfigParms += ",2";
         break;

      case 3:           // BIP
         *m_pstrConfigParms += ",3";
         break;

      case 4:           // FOREIGN, ipaddr:socket, TTL
         *m_pstrConfigParms += ",4," + m_HostAddr + "," + m_TTL;
         break;
   }
}

void VTSPortIPDialog::OnBTRPeers()
{
   AfxMessageBox("Unfortunately I have no clue what this BTRPeers button should do at this point.");
}

void VTSPortIPDialog::OnBBMDPeers()
{
   AfxMessageBox("Unfortunately I have no clue what this BBMDPeers should do at this point.");
}
