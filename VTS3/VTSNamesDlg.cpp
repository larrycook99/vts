// VTSNamesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "VTSNamesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSNamesDlg dialog

VTSNamesDlg::VTSNamesDlg(  VTSNames * pnames, VTSPorts * pports, CWnd* pParent /* = NULL */ )
   : CDialog( VTSNamesDlg::IDD, pParent )
{
   // duplicate the member initalization that the class 'wizard' adds
   m_Name = _T("");
   m_Address = _T("");
   m_Network = _T("");

   m_names.DeepCopy(pnames);
   m_pnames = pnames;
   m_pports = pports;
   m_fWarnedAlready = false;
   m_iSelectedName = -1;
}

void VTSNamesDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(VTSNamesDlg)
   DDX_Control(pDX, IDC_NAMELIST, m_NameList);
   DDX_Text(pDX, IDC_NAME, m_Name);
   DDX_Control(pDX, IDC_PORTCOMBO, m_PortCombo);
   DDX_Control(pDX, IDC_ADDRESS, m_AddressCtrl);   
   DDX_Text(pDX, IDC_ADDRESS, m_Address);
   DDX_Text(pDX, IDC_NETWORK, m_Network);
   DDX_Radio(pDX, IDC_NULLADDR, m_AddrType);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(VTSNamesDlg, CDialog)
   //{{AFX_MSG_MAP(VTSNamesDlg)
   ON_BN_CLICKED(ID_NEWNAME, OnNew)
   ON_BN_CLICKED(ID_DELETENAME, OnDelete)
   ON_BN_CLICKED(ID_IMPORTNAMES, OnImportNames)
   ON_BN_CLICKED(ID_EXPORTNAMES, OnExportNames)
   ON_NOTIFY(LVN_ITEMCHANGING, IDC_NAMELIST, OnItemchangingNamelist)
   ON_NOTIFY(NM_DBLCLK, IDC_NAMELIST, OnDblclkNamelist)
   ON_EN_CHANGE(IDC_NAME, SaveChanges)
   ON_BN_CLICKED(IDC_NULLADDR, SaveChanges)
   ON_BN_CLICKED(IDC_LOCALBROADCAST, SaveChanges)
   ON_BN_CLICKED(IDC_LOCALSTATION, SaveChanges)
   ON_BN_CLICKED(IDC_REMOTEBROADCAST, SaveChanges)
   ON_BN_CLICKED(IDC_REMOTESTATION, SaveChanges)
   ON_BN_CLICKED(IDC_GLOBALBROADCAST, SaveChanges)
   ON_CBN_SELCHANGE(IDC_PORTCOMBO, SaveChanges)
   ON_EN_CHANGE(IDC_NETWORK, SaveChanges)
   ON_EN_CHANGE(IDC_ADDRESS, SaveChanges)
   ON_EN_KILLFOCUS(IDC_ADDRESS, OnKillfocusAddress)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSNamesDlg message handlers

BOOL VTSNamesDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   
   // disable import and export for now
   GetDlgItem( ID_IMPORTNAMES )->EnableWindow( false );
   GetDlgItem( ID_EXPORTNAMES )->EnableWindow( false );

   InitPortList();               // get the ports from the database
   InitNameList();               // fill in the table from the database

   if (m_names.GetSize() > 0)
   {
      m_iSelectedName = 0;
      NameToCtrl( (VTSName *)m_names[m_iSelectedName] );
      m_NameList.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
   }
   else
   {
      ResetSelection();
   }
   
   return FALSE;  // return TRUE  unless you set the focus to a control
}

void VTSNamesDlg::InitPortList()
{
   // Add the ports
   m_PortCombo.AddString( "(any port)" );

   // add items and subitems
   for (int i = 0; i < m_pports->GetSize(); i++)
      m_PortCombo.AddString( (*m_pports)[i]->GetName() );

   // set the first item as the default
   m_PortCombo.SetCurSel( 0 );
}

void VTSNamesDlg::InitNameList()
{
   int   len = m_names.GetSize();

   // only allow one selected name at a time
   m_NameList.m_nFlags |= LVS_SINGLESEL;

   // do not sort the list
   m_NameList.m_nFlags &= ~LBS_SORT;

   // Select full row (default is just the first column)
   m_NameList.SetExtendedStyle( LVS_EX_FULLROWSELECT );

   // add columns.
   m_NameList.InsertColumn (0, _T("Name"), LVCFMT_LEFT, 96 );
   m_NameList.InsertColumn (1, _T("Port"), LVCFMT_LEFT, 64 );
   m_NameList.InsertColumn (2, _T("Network"), LVCFMT_LEFT, 64 );
   m_NameList.InsertColumn (3, _T("Address"), LVCFMT_LEFT, 128 );

   // add names
   for (int i = 0; i < len; i++)
   {
      // make a placeholder for the item
      m_NameList.InsertItem( i, _T("") );

      VTSName * p = (VTSName *) m_names[i];
      NameToList( (VTSName *) m_names[i], i );
   }
}

void VTSNamesDlg::ResetSelection()
{
   // no name selected
   m_iSelectedName = -1;

   // clear out the contents of the local variables
   m_AddrType = -1;
   m_Name = _T("");
   m_PortCombo.SetCurSel( 0 );
   m_Address = _T("");
   m_Network = _T("");

   // make sure the control values reflect the empty settings
   UpdateData( false );

   // make sure all the controls are properly disabled
   SynchronizeControls();
}

void VTSNamesDlg::SynchronizeControls()
{
   // the name is enabled when there is something selected
   GetDlgItem( IDC_NAME )->EnableWindow( m_iSelectedName != -1 );

   // so are the radio buttons for the address type
   GetDlgItem( IDC_NULLADDR )->EnableWindow( m_iSelectedName != -1 );
   GetDlgItem( IDC_LOCALBROADCAST )->EnableWindow( m_iSelectedName != -1 );
   GetDlgItem( IDC_LOCALSTATION )->EnableWindow( m_iSelectedName != -1 );
   GetDlgItem( IDC_REMOTEBROADCAST )->EnableWindow( m_iSelectedName != -1 );
   GetDlgItem( IDC_REMOTESTATION )->EnableWindow( m_iSelectedName != -1 );
   GetDlgItem( IDC_GLOBALBROADCAST )->EnableWindow( m_iSelectedName != -1 );
   
   // so is the port selection
   GetDlgItem( IDC_PORTCOMBO )->EnableWindow( m_iSelectedName != -1 );

   // network and address controls are a little harder
   switch (m_AddrType) {
      case -1: // no selection
      case 0:     // null address
      case 1:     // local broadcast
      case 5:     // global broadcast
         GetDlgItem( IDC_NETWORK )->EnableWindow( false );
         GetDlgItem( IDC_ADDRESS )->EnableWindow( false );
         break;

      case 2:     // local station
         GetDlgItem( IDC_NETWORK )->EnableWindow( false );
         GetDlgItem( IDC_ADDRESS )->EnableWindow( true );
         break;

      case 3:     // remote broadcast
         GetDlgItem( IDC_NETWORK )->EnableWindow( true );
         GetDlgItem( IDC_ADDRESS )->EnableWindow( false );
         break;

      case 4:     // remote station
         GetDlgItem( IDC_NETWORK )->EnableWindow( true );
         GetDlgItem( IDC_ADDRESS )->EnableWindow( true );
         break;
   }
}

//
// VTSNamesDlg::NameToList
//

void VTSNamesDlg::NameToList( const VTSName * pname, int elem )
{
   CString  netStr, addrStr;

   ASSERT(pname != NULL);

   // update the display list
   m_NameList.SetItemText( elem, 0, pname->m_strName );

   bool showAsIP = false;
   if (pname->m_pportLink == NULL)
   {
      m_NameList.SetItemText( elem, 1, _T("*") );
   }
   else
   {
      m_NameList.SetItemText( elem, 1, _T(pname->m_pportLink->GetName()) );

      showAsIP = (pname->m_pportLink->m_nPortType == ipPort) &&
               (pname->m_bacnetaddr.addrType == localStationAddr);
   }

   // check for some remote broadcast or remote station
   if ((pname->m_bacnetaddr.addrType == remoteBroadcastAddr) || 
      (pname->m_bacnetaddr.addrType == remoteStationAddr))
   {
      netStr.Format( "%d", pname->m_bacnetaddr.addrNet );
   }
   m_NameList.SetItemText( elem, 2, netStr );

   // check for local or remote station
   if ((pname->m_bacnetaddr.addrType == localStationAddr) || 
      (pname->m_bacnetaddr.addrType == remoteStationAddr)) 
   {
      if (showAsIP)
      {
         // Show as a B/IP address, to avoid the nuisance of hex entry
         addrStr.Format( "%u.%u.%u.%u:%u",
            pname->m_bacnetaddr.addrAddr[0],
            pname->m_bacnetaddr.addrAddr[1],
            pname->m_bacnetaddr.addrAddr[2],
            pname->m_bacnetaddr.addrAddr[3],
            (pname->m_bacnetaddr.addrAddr[4] << 8) + pname->m_bacnetaddr.addrAddr[5] );

      }
      else
      {
         static const char hex[] = "0123456789ABCDEF";
         char  buff[kMaxAddressLen * 3], *s = buff;
         
         // clear the buffer
         buff[0] = 0;
         
         // encode the address
         for (int i = 0; i < pname->m_bacnetaddr.addrLen; i++) {
            if (i) *s++ = '-';
            *s++ = hex[ pname->m_bacnetaddr.addrAddr[i] >> 4 ];
            *s++ = hex[ pname->m_bacnetaddr.addrAddr[i] & 0x0F ];
         }
         *s = 0;
         addrStr = buff;
      }
   } 
   m_NameList.SetItemText( elem, 3, addrStr );
}

//
// VTSNamesDlg::NameToCtrl
//

void VTSNamesDlg::NameToCtrl( const VTSName * pname )
{
   // set the contents of the member vars from the record
   m_Name = pname->m_strName;
   m_AddrType = pname->m_bacnetaddr.addrType;

   m_Network = _T("");
   m_Address = _T("");

   if ((pname->m_bacnetaddr.addrType == remoteBroadcastAddr) || 
      (pname->m_bacnetaddr.addrType == remoteStationAddr))
   {
      m_Network.Format( "%d", pname->m_bacnetaddr.addrNet );
   }

   if ((pname->m_bacnetaddr.addrType == localStationAddr) || 
      (pname->m_bacnetaddr.addrType == remoteStationAddr)) 
   {
      if ((pname->m_pportLink != NULL) &&
         (pname->m_pportLink->m_nPortType == ipPort) &&
         (pname->m_bacnetaddr.addrType == localStationAddr))
      {
         // Show as a B/IP address, to avoid the nuisance of hex entry
         m_Address.Format( "%u.%u.%u.%u:%u",
                  pname->m_bacnetaddr.addrAddr[0],
                  pname->m_bacnetaddr.addrAddr[1],
                  pname->m_bacnetaddr.addrAddr[2],
                  pname->m_bacnetaddr.addrAddr[3],
                  (pname->m_bacnetaddr.addrAddr[4] << 8) + pname->m_bacnetaddr.addrAddr[5] );

      }
      else
      {
         static const char hex[] = "0123456789ABCDEF";
         char  buff[kMaxAddressLen * 3], *s = buff;
         
         // clear the buffer
         buff[0] = 0;
         
         // encode the address
         for (int i = 0; i < pname->m_bacnetaddr.addrLen; i++) {
            if (i) *s++ = '-';
            *s++ = hex[ pname->m_bacnetaddr.addrAddr[i] >> 4 ];
            *s++ = hex[ pname->m_bacnetaddr.addrAddr[i] & 0x0F ];
         }
         *s = 0;
         
         m_Address = buff;
      }
   }

   m_PortCombo.SelectString(0, (pname->m_pportLink == NULL) ? "(any port)" : pname->m_pportLink->GetName());

   // sync the controls
   SynchronizeControls();

   // sync the dialog controls with the member variables
   UpdateData( false );
}

//
// VTSNamesDlg::CtrlToName
//

void VTSNamesDlg::CtrlToName( VTSName * pname )
{
   LPCSTR      s;
   char        *pNext;
   CComboBox   *cbp = (CComboBox *)GetDlgItem( IDC_PORTCOMBO );

   // sync the member variables with the dialog controls
   UpdateData( true );

   // make sure the correct UI elements are enable/disabled
   SynchronizeControls();

   // copy the vars into the name
   pname->m_strName = m_Name;

   // do the address type
   pname->m_bacnetaddr.addrType = (BACnetAddressType)m_AddrType;

   // must do more than assign pointer link... we've got to set up the name as well
   // so if the ports are changed this NAME object can relink when ports dialog is used.
   if (cbp->GetCurSel() == 0)
   {
      pname->m_pportLink = NULL;
      pname->m_strPortNameTemp.Empty();
   }
   else
   {
      pname->m_pportLink = (*m_pports)[cbp->GetCurSel() - 1];
      pname->m_strPortNameTemp = pname->m_pportLink->GetName();
   }

   // check for some remote broadcast or remote station
   if ((pname->m_bacnetaddr.addrType == remoteBroadcastAddr) || 
      (pname->m_bacnetaddr.addrType == remoteStationAddr)) 
   {
      pname->m_bacnetaddr.addrNet = (UINT)strtoul( m_Network, &pNext, 0 );
   }

   // check for local or remote station
   if ((pname->m_bacnetaddr.addrType == localStationAddr) || 
      (pname->m_bacnetaddr.addrType == remoteStationAddr)) 
   {
      s = m_Address;
      if (strchr( s, '.' ))
      {
         // Enter as dotted-decimal IP address, colon, UDP port
         UINT a=0, b=0, c=0, d=0, e=0;
         pname->m_bacnetaddr.addrLen = 0;
         if ((sscanf( s, "%u.%u.%u.%u:%u", &a,&b,&c,&d,&e ) >= 1) &&
            (a < 256) && (b < 256) && (c < 256) && (d < 256) && (e < 65536))
         {
            pname->m_bacnetaddr.addrAddr[ 0 ] = (unsigned char)a;
            pname->m_bacnetaddr.addrAddr[ 1 ] = (unsigned char)b;
            pname->m_bacnetaddr.addrAddr[ 2 ] = (unsigned char)c;
            pname->m_bacnetaddr.addrAddr[ 3 ] = (unsigned char)d;
            pname->m_bacnetaddr.addrAddr[ 4 ] = (unsigned char)(e >> 8);
            pname->m_bacnetaddr.addrAddr[ 5 ] = (unsigned char)e;
            pname->m_bacnetaddr.addrLen = 6;
         }
         else
         {
            // Not a legal IP address
            MessageBeep(-1);
         }
      }
      else
      {
         // Enter as a string of hex bytes separated by dashes
         int ix;
         for (ix = 0; *s && (ix < kMaxAddressLen); ix++) 
         {
            UINT valu = (UINT)strtoul( s, &pNext, 16 );
            if (valu > 255)
            {
               MessageBeep(-1);
               break;
            }

            pname->m_bacnetaddr.addrAddr[ ix ] = (unsigned char)valu;
            s = pNext;
            if ((*s == '-') || (*s == ':'))
            {
               s = s + 1;
            }
            else if (*s != 0)
            {
               MessageBeep(-1);
               break;
            }
         }
         pname->m_bacnetaddr.addrLen = ix;
      }
   }
}

//
// VTSNamesDlg::OnNew
//

void VTSNamesDlg::OnOK()
{
   // First we have to validate the data...  so far, the only thing can could go wrong is
   // when we let them off of the address edit control having entered an invalid address
   // for specific types...  loop through all names and check 'em.

   for ( int n = 0; n < m_names.GetSize(); n++ )
   {
      VTSName * pname = (VTSName *) m_names[n];

      if ( ((pname->m_bacnetaddr.addrType == localStationAddr) ||
           (pname->m_bacnetaddr.addrType == remoteStationAddr))
          &&
           ((pname->m_bacnetaddr.addrLen < 1) || (pname->m_bacnetaddr.addrLen > 6)) )
      {
         // select the item in the list...  then change focus to address edit control

         m_NameList.SetItemState( n, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
         m_AddressCtrl.SetFocus();

         AfxMessageBox(IDS_NAME_ADDRERROR, MB_ICONEXCLAMATION | MB_OK);
         return;  // don't allow OK
      }
   }

   // copy names array...   then copy array.  Elements remain allocated
   m_pnames->KillContents();

   // copy only the element references... changes ownership of element memory
   for ( int i = 0; i < m_names.GetSize(); i++ )
   {
      VTSName * p = (VTSName *) m_names[i];
      m_pnames->Add(m_names[i]);
   }

   // empty list but don't remove elements...  this will avoid destructor
   // killing the memory... which we've already transferred ownership to document's member
   // names list

   m_names.RemoveAll();

   CDialog::OnOK(); // This will close the dialog and DoModal will return.
}


void VTSNamesDlg::OnCancel()
{
   // remove allocated temp elements...  destructor will kill array
   m_names.KillContents();

   CDialog::OnCancel(); // This will close the dialog and DoModal will return.
}


void VTSNamesDlg::OnNew()
{
   int listLen = m_names.GetSize();
   VTSName * pname = new VTSName();

   // If there is no name "IUT", make one with default values per the Survival Guide
   if (m_names.Find( "IUT" ) == NULL)
   {
      pname->m_strName = "IUT";

      // Link to the default (usually only) Port
      VTSPort *pPort = m_pports->GetAt(0);
      if (pPort)
      {
         pname->m_pportLink = pPort;
         pname->m_strPortNameTemp = pPort->GetName();

         // Set initial address as appropriate to the port.
         // Set to nullAddress and all zero bytes by the constructor
         pname->m_bacnetaddr.addrType = pPort->portEndpoint->portLocalAddr.addrType;
         pname->m_bacnetaddr.addrLen = pPort->portEndpoint->portLocalAddr.addrLen;
         if (pPort->m_nPortType)
         {
            // B/IP address.  Copy the UDP port from TD
            pname->m_bacnetaddr.addrAddr[4] = pPort->portEndpoint->portLocalAddr.addrAddr[4];
            pname->m_bacnetaddr.addrAddr[5] = pPort->portEndpoint->portLocalAddr.addrAddr[5];
         }
      }
   }

   // deselect if something was selected
   POSITION selPos = m_NameList.GetFirstSelectedItemPosition();
   if (selPos != NULL) {
      int    nItem = m_NameList.GetNextSelectedItem( selPos );
      m_NameList.SetItemState( nItem, 0, LVIS_SELECTED );
   }
   
   // create a new name
   m_names.Add(pname);

   // make a placeholder for the item
   m_NameList.InsertItem( listLen, _T(pname->m_strName) );

   // copy the contents to the list
   NameToList( pname, listLen );

   // make sure it is visible and selected, this also transfers the info to the ctrls
   m_NameList.EnsureVisible( listLen, false );
   m_NameList.SetItemState( listLen, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

   // set the focus to the name for editing, select all of the text
   GetDlgItem( IDC_NAME )->SetFocus();
   ((CEdit*)GetDlgItem( IDC_NAME ))->SetSel( 0, -1 );
}

//
// VTSNamesDlg::OnDelete
//

void VTSNamesDlg::OnDelete()
{
   POSITION selPos = m_NameList.GetFirstSelectedItemPosition();

   // make sure something was selected
   if (selPos == NULL)
      return;

   // figure out which item it is
   int    nItem = m_NameList.GetNextSelectedItem( selPos );

   // now deselect it, clearing out the controls in the process
   m_NameList.SetItemState( nItem, 0, LVIS_SELECTED | LVIS_FOCUSED );

   // delete it from the list
   m_NameList.DeleteItem( nItem );

   // delete the name from the database
   m_names.Remove( nItem );

   // now select the next item if there is one... select the previous item if not... until null
   if ( nItem >= m_NameList.GetItemCount() )
      nItem = m_NameList.GetItemCount() - 1;

   if ( nItem >= 0 )
      m_NameList.SetItemState( nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
}

//
// VTSNamesDlg::OnImportNames
//

void VTSNamesDlg::OnImportNames()
{
}

//
// VTSNamesDlg::OnExportNames
//

void VTSNamesDlg::OnExportNames()
{
}

//
// VTSNamesDlg::OnItemchangingNamelist
//

void VTSNamesDlg::OnItemchangingNamelist(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_LISTVIEW*   pNMListView = (NM_LISTVIEW*)pNMHDR;
   
#if 0
   TRACE3( "Item %d from %d to %d\n"
      , pNMListView->iItem
      , pNMListView->uOldState
      , pNMListView->uNewState
      );
#endif

   // forget messages that don't change anything
   if (pNMListView->uOldState == pNMListView->uNewState)
      return;

   if ((pNMListView->uNewState & LVIS_SELECTED) != 0) {
      m_iSelectedName = pNMListView->iItem;

      // transfer the values to the controls
      NameToCtrl( (VTSName *) m_names[m_iSelectedName]);

// } else {
//    if (pNMListView->iItem == m_iSelectedName)
//       ResetSelection();
   }

   *pResult = 0;
}

//
// VTSNamesDlg::OnDblclkNamelist
//

void VTSNamesDlg::OnDblclkNamelist(NMHDR* pNMHDR, LRESULT* pResult)
{
   // set the focus to the name for editing, select all of the text
   GetDlgItem( IDC_NAME )->SetFocus();
   ((CEdit*)GetDlgItem( IDC_NAME ))->SetSel( 0, -1 );

   *pResult = 0;
}

//
// VTSNamesDlg::SaveChanges
//

void VTSNamesDlg::SaveChanges()
{
   // transfer the contents of the controls to the record
   CtrlToName( (VTSName *) m_names[m_iSelectedName] );

   // bring the list up-to-date
   NameToList( (VTSName *) m_names[m_iSelectedName], m_iSelectedName );
}

void VTSNamesDlg::OnKillfocusAddress()
{
   if ( m_iSelectedName == -1 || m_iSelectedName > m_names.GetSize() - 1)
      return;

   VTSName * pname = (VTSName *) m_names[m_iSelectedName];

   CWnd *cwt;
   int nFocusID=0;

   cwt = GetFocus();
   if(cwt != NULL)
      nFocusID = cwt->GetDlgCtrlID();  // GetFocus() sometimes returns NULL, i.e. when new focus is not VTS

   // Check for validity and warn if we haven't already warned and they didn't
   // press the cancel or delete key...

   if (((pname->m_bacnetaddr.addrLen < 1) || (pname->m_bacnetaddr.addrLen > 6)) &&
      (nFocusID != GetDlgItem(IDCANCEL)->GetDlgCtrlID()) &&
      (nFocusID != GetDlgItem(ID_DELETENAME)->GetDlgCtrlID()))
   {
      if ( !m_fWarnedAlready )
      {
         AfxMessageBox(IDS_NAME_ADDRERROR, MB_ICONEXCLAMATION | MB_OK);

         m_AddressCtrl.SetFocus();
         m_fWarnedAlready = true;
      }
   }
}
