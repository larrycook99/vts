// VTSPortDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "VTSPortDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSPortDlg dialog

#pragma warning(disable:4355)

VTSPortDlg::VTSPortDlg( VTSDoc * pdoc )
   : CDialog( VTSPortDlg::IDD, NULL ),
     m_pageNull(this),  m_pageIP(this),  m_pageEthernet(this),  m_pageARCNET(this),  m_pageMSTP(this),  m_pagePTP(this)
{
   // duplicate the member initalization that the class 'wizard' adds
   m_strName = _T("");
   m_fEnabled = FALSE;
   m_nNetwork = -1;

   m_iSelectedPort = -1;      // initialize port index, Jingbo Gao, 2004-11-10

   m_pdoc = pdoc;
   m_pdevices = pdoc->GetDevices();
   m_pports = pdoc->GetPorts();

   CopyMainPortArray();

   for (int i = 0; i < PORT_PAGE_MAX; i++ )
      m_pstrPageData[i] = new CString(_T(""));

   m_ppages[0] = &m_pageNull;
   m_ppages[1] = &m_pageIP;
   m_ppages[2] = &m_pageEthernet;
   m_ppages[3] = &m_pageARCNET;
   m_ppages[4] = &m_pageMSTP;
   m_ppages[5] = &m_pagePTP;

   m_iSelectedPort=0;  // changed by Kare Sars 2005
}


VTSPortDlg::~VTSPortDlg()
{
   // only memory allocated are strings for isolated config parm creation

   for ( int i = 0; i < PORT_PAGE_MAX; i++ )
      delete m_pstrPageData[i];
}


void VTSPortDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(VTSPortDlg)
   DDX_Control(pDX, IDC_PORTLIST, m_PortList);
   DDX_Text(pDX, IDC_PORTNAME, m_strName);
   DDX_Check(pDX, IDC_ENABLEPORT, m_fEnabled);
   DDX_Control(pDX, IDC_DEVICECOMBO, m_DeviceCombo);
   //}}AFX_DATA_MAP

   if ( pDX->m_bSaveAndValidate )
   {
      CString str;
      GetDlgItem(IDC_NETWORK2)->GetWindowText(str);
      if ( str.IsEmpty() )
         m_nNetwork = -1;
      else
      {
         DDX_Text(pDX, IDC_NETWORK2, m_nNetwork);
         DDV_MinMaxInt(pDX, m_nNetwork, 0, 65536);
      }
   }
   else
   {
      if ( m_nNetwork == -1 )
         GetDlgItem(IDC_NETWORK2)->SetWindowText(_T(""));
      else
         DDX_Text(pDX, IDC_NETWORK2, m_nNetwork);
   }
}


BEGIN_MESSAGE_MAP(VTSPortDlg, CDialog)
   //{{AFX_MSG_MAP(VTSPortDlg)
   ON_BN_CLICKED(IDC_NEWPORT, OnNewPort)
   ON_NOTIFY(LVN_ITEMCHANGING, IDC_PORTLIST, OnItemchangingPortList)
   ON_NOTIFY(NM_DBLCLK, IDC_PORTLIST, OnDblclkPortList)
   ON_BN_CLICKED(ID_DELETE, OnDelete)
   ON_EN_UPDATE(IDC_PORTNAME, SaveChanges)
   ON_BN_CLICKED(IDC_ENABLEPORT, SaveChanges)
   ON_EN_UPDATE(IDC_NETWORK2, SaveChanges)
   ON_CBN_SELCHANGE(IDC_DEVICECOMBO, SaveChanges)
   ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNow)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPortDlg message handlers

//
// VTSPortDlg::OnInitDialog
//

BOOL VTSPortDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // Normally we would add the pages before creation... but this seems to barf the release
   // version of MFC during create.  We have to point to one temporary thing to set the
   // size, then after creation, remove it and place the real pages.  Bogus.

   CPropertyPage  pageTemp(IDD_PORTPAGE_NULL);
   m_sheet.AddPage(&pageTemp);

   m_sheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
   m_sheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
   m_sheet.ModifyStyle( 0, WS_TABSTOP );

   m_sheet.RemovePage(&pageTemp);

   for ( int n = 0; n < PORT_PAGE_MAX; n++ )
      m_sheet.AddPage(m_ppages[n]);

   CRect rcSheet;
   GetDlgItem(IDC_PORTPAGE_HOLDER)->GetWindowRect( &rcSheet );
   ScreenToClient( &rcSheet );
   m_sheet.SetWindowPos( NULL, rcSheet.left-7, rcSheet.top-7, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );

   // initialize the port list
   m_PortList.SetExtendedStyle( LVS_EX_FULLROWSELECT ); // Select full row (default is just the first column)
   m_PortList.m_nFlags |= LVS_SINGLESEL;
   m_PortList.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 96 );
   m_PortList.InsertColumn( 1, _T("Type"), LVCFMT_LEFT, 64 );
   m_PortList.InsertColumn( 2, _T("Config"), LVCFMT_LEFT, 96 );
   m_PortList.InsertColumn( 3, _T("Status"), LVCFMT_LEFT, 65 ); // 256 );

   // initialize the status image list
   m_ilStatus.Create( IDB_PORTSTATUS, 16, 1, RGB(255,0,255) );
   m_PortList.SetImageList( &m_ilStatus, LVSIL_SMALL );

   // add items and subitems
   for (int i = 0; i < m_ports.GetSize(); i++)
   {
      // get a pointer to the ith port
      VTSPortPtr     curPort = m_ports[i];

      // copy the descriptions into the list
      m_PortList.InsertItem( i, curPort->GetName(), curPort->portStatus );
      UpdatePortListItem(i, curPort );
   }

   // Add the devices
   m_DeviceCombo.AddString( "(no device)" );

   for (int j = 0; j < m_pdevices->GetSize(); j++)
      m_DeviceCombo.AddString( (*m_pdevices)[j]->GetName() );

   // make sure nothing is selected
   if ( m_ports.GetSize() )
      SetSelection(0);
   else
      ResetSelection();

   m_PortList.SetItemState( m_iSelectedPort, LVIS_SELECTED, LVIS_SELECTED );

   SynchronizeControls();
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


void VTSPortDlg::CopyMainPortArray()
{
   m_ports.DeepCopy(m_pports);

   // Now that the ports have been copied, setup the array to track changes
   m_apOriginalPort.RemoveAll();

   for ( int n = 0; n < m_pports->GetSize(); n++ )
      m_apOriginalPort.Add((*m_pports)[n]);

   m_fModified = false;
}


void VTSPortDlg::DataChangeNotification(void)
{
   // called by child property pages... we don't care about things here...
   if ( m_iSelectedPort == -1 )
      return;

   VTSPort * curPort = m_ports[m_iSelectedPort];
   curPort->SetConfig(*m_pstrPageData[curPort->m_nPortType]);

   if ( curPort->IsDirty() )
      m_fModified = true;

   // set the image for the list to match the status of the port
   m_PortList.SetItem( m_iSelectedPort, 0, TVIF_IMAGE, 0, curPort->portStatus, 0, 0, 0 );

   // update the list
   UpdatePortListItem(m_iSelectedPort, curPort );
   EnableOK();
}


void * VTSPortDlg::GetActiveData(void)
{
   if ( m_iSelectedPort < 0 )
      return NULL;

   // Called from the selected page in an attempt to get the current data...
   // What is current port?
   // What is current selected page?

   VTSPort * curPort = m_ports[m_iSelectedPort];
   curPort->SetPortType( (VTSPortType) m_sheet.GetActiveIndex() );
   curPort->SetConfig( *m_pstrPageData[curPort->m_nPortType] );
   
   if ( curPort->IsDirty() )
   {
      m_fModified = true;
      EnableOK();
   }

   return (void *) m_pstrPageData[curPort->m_nPortType];
}


//
// VTSPortDlg::SetSelection
//

void VTSPortDlg::SetSelection( int indx )
{
   VTSPortPtr     curPort;
   TRACE1( "SetSelection( %d )\n", indx );

   // if something is selected and its not this one, reset
   if ((m_iSelectedPort >= 0) && (indx != m_iSelectedPort))
      ResetSelection();

   // get a pointer to the port
   curPort = m_ports[indx];

   // set the local variables to the port information
   m_iSelectedPort = indx;
   m_strName = curPort->GetName();
   m_fEnabled = curPort->m_fEnabled;
   m_nNetwork = curPort->m_nNet;

   // now set the correct parm string
   ReloadTypeData(curPort->m_nPortType, &curPort->m_strConfigParms);
   m_sheet.SetActivePage(curPort->m_nPortType);

   if ( curPort->m_pdevice == NULL )
      m_DeviceCombo.SetCurSel( 0 );
   else
      m_DeviceCombo.SelectString(0, curPort->m_pdevice->GetName() );

   // make sure the controls are properly enabled
   SynchronizeControls();

   // let the CDialog sync the controls with the local vars
   UpdateData( false );
}


void VTSPortDlg::ReloadTypeData( int nActiveType, CString * pstr )
{
   for ( int i = 0; i < PORT_PAGE_MAX; i++)
   {
      if ( nActiveType == i && pstr != NULL )
         *m_pstrPageData[i] = *pstr;
      else
         m_pstrPageData[i]->Empty();
   }
}


//
// VTSPortDlg::ResetSelection
//

void VTSPortDlg::ResetSelection( void )
{
   TRACE0( "ResetSelection()\n" );

   // nothing selected, just return
   if (m_iSelectedPort < 0)
      return;

#if 0
   // deselect from the list
   POSITION selPos = m_PortList.GetFirstSelectedItemPosition();
   if (selPos != NULL) {
      int nItem = m_PortList.GetNextSelectedItem( selPos );
      m_PortList.SetItemState( nItem, 0, LVIS_SELECTED );
   }
#endif

   // clear out the contents of the local vars
   m_iSelectedPort = -1;
   m_strName = _T("");
   m_fEnabled = FALSE;
   m_nNetwork = -1;

   ReloadTypeData(m_iSelectedPort, NULL);
   m_sheet.SetActivePage(0);

   m_DeviceCombo.SetCurSel( 0 );

   // let the CDialog sync the controls with the local vars
   UpdateData( false );

   // make sure the controls are properly disabled
   SynchronizeControls();
}

//
// VTSPortDlg::SynchronizeControls
//

void VTSPortDlg::SynchronizeControls( void )
{
   GetDlgItem( IDC_PORTNAME )->EnableWindow( m_iSelectedPort >= 0 );
   GetDlgItem( IDC_ENABLEPORT )->EnableWindow( m_iSelectedPort >= 0 );
   GetDlgItem( IDC_NETWORK2 )->EnableWindow( m_iSelectedPort >= 0 );
   GetDlgItem( IDC_DEVICECOMBO )->EnableWindow( m_iSelectedPort >= 0 );
   EnableOK();
}


void VTSPortDlg::EnableOK()
{
   GetDlgItem( IDOK )->EnableWindow(m_fModified);
   GetDlgItem( ID_APPLY_NOW )->EnableWindow(m_fModified);
}

//
// VTSPortDlg::PortStatusChange
//

void VTSPortDlg::PortStatusChange( void )
{
   // look through copy of ports and refresh all of the status values from their originals
   // then update the status display

   for (int i = 0; i < m_ports.GetSize(); i++)
   {
      // if original is NULL, this value was added and can't have a status change... hasn't been activated yet.
      if ( m_apOriginalPort[i] != NULL )
      {
         m_ports[i]->portStatus = ((VTSPort *) m_apOriginalPort[i])->portStatus;
         m_ports[i]->portStatusDesc = ((VTSPort *) m_apOriginalPort[i])->portStatusDesc;
      }

      m_PortList.SetItem( i, 0, TVIF_IMAGE, 0, m_ports[i]->portStatus, 0, 0, 0 );
      m_PortList.SetItemText( i, 3, (LPCTSTR)(m_ports[i]->portStatusDesc ? m_ports[i]->portStatusDesc : "") );
   }
}

//
// VTSPortDlg::OnNewPort
//

void VTSPortDlg::OnNewPort()
{
   VTSPortPtr  curPort = new VTSPort();

   // If this is the first/only port, set default values per the Survival Guide
   if (m_ports.GetSize() == 0)
   {
      curPort->SetName( "MyPort" );
      curPort->SetPortType( ipPort );
      curPort->SetNetwork( 1 );
      curPort->SetDevice( m_pdevices->GetAt(0) );
      curPort->SetEnabled( true );
   }

   // get a pointer to the new port
   int indx = m_ports.Add(curPort);
   m_apOriginalPort.Add(NULL);

   // copy the descriptions into the list
   m_PortList.InsertItem( indx, curPort->GetName() );
   UpdatePortListItem(indx, curPort);

   // make sure the record is visible and selected
   m_PortList.EnsureVisible( indx, false );
   m_PortList.SetItemState( indx, LVIS_SELECTED, LVIS_SELECTED );

   m_fModified = true;
   EnableOK();
}


void VTSPortDlg::UpdatePortListItem( int nIndex, VTSPort * pport )
{
   // copy the descriptions into the list
   m_PortList.SetItemText( nIndex, 0, pport->GetName() );
   m_PortList.SetItemText( nIndex, 1, pport->GetTypeDesc() );
   m_PortList.SetItemText( nIndex, 2, pport->m_strConfigParms );
   m_PortList.SetItemText( nIndex, 3, (LPCTSTR)(pport->portStatusDesc ? pport->portStatusDesc : "") );
}

//
// VTSPortDlg::OnItemchangingPortList
//

void VTSPortDlg::OnItemchangingPortList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   // forget messages that dont change anything
   if (pNMListView->uOldState == pNMListView->uNewState)
      return;

   if ((pNMListView->uNewState & LVIS_SELECTED) != 0)
      SetSelection( pNMListView->iItem );
   else
   if (pNMListView->iItem == this->m_iSelectedPort)
      ResetSelection();

   *pResult = 0;
}

//
// VTSPortDlg::OnDblclkPortList
//

void VTSPortDlg::OnDblclkPortList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   // set the focus to the name for editing, preselect all of the text
   GetDlgItem( IDC_PORTNAME )->SetFocus();
   ((CEdit*)GetDlgItem( IDC_PORTNAME ))->SetSel( 0, -1 );

   *pResult = 0;
}

//
// VTSPortDlg::SaveChanges
//

void VTSPortDlg::SaveChanges() 
{
   VTSPort *   curPort;

   ASSERT( m_iSelectedPort >= 0 );

   // get a pointer to the new port
   curPort = m_ports[m_iSelectedPort];

   UpdateData( true );

   curPort->SetName(m_strName);
   curPort->SetEnabled(m_fEnabled);
   curPort->SetPortType( (VTSPortType) m_sheet.GetActiveIndex() );
   curPort->SetNetwork(m_nNetwork);

   // check the device
   CComboBox   *cbp = (CComboBox *)GetDlgItem( IDC_DEVICECOMBO );
   if ( cbp->GetCurSel() == 0 )
   {
      curPort->SetDevice(NULL);
      curPort->m_strDevice.Empty();
   }
   else
   {
      curPort->SetDevice( (*m_pdevices)[cbp->GetCurSel()-1] );
      curPort->m_strDevice = curPort->m_pdevice->GetName();
   }

   if ( curPort->IsDirty() )
      m_fModified = true;

   // set the image for the list to match the status of the port
   m_PortList.SetItem( m_iSelectedPort, 0, TVIF_IMAGE, 0, curPort->portStatus, 0, 0, 0 );

   // update the list
   UpdatePortListItem(m_iSelectedPort, curPort );
   EnableOK();
}


void VTSPortDlg::OnOK() 
{
   DeactivateChangedPorts();
   MoveLocalPortsToMain();
   m_pdoc->FixupNameToPortLinks(false);         // must refixup while ports are inactive...
   m_pdoc->FixupFiltersToPortLinks(false);
   m_pdoc->ReActivateAllPorts();

   CDialog::OnOK();
}


void VTSPortDlg::OnApplyNow() 
{
   DeactivateChangedPorts();
   MoveLocalPortsToMain();
   m_pdoc->FixupNameToPortLinks(false);
   m_pdoc->FixupFiltersToPortLinks(false);

   // Now copy these ports back to local storage.. Local array needs to be here first
   // so we can properly respond to status changes calls produced by activating all ports

   CopyMainPortArray();
   m_pdoc->ReActivateAllPorts();
   PortStatusChange();
   m_pdoc->SaveConfiguration();
   EnableOK();
}


// Deactivate all ports that have changed...  kill ports ports that shouldn't exist anymore
// and move activated ports that haven't changed over to the local array for, well, moving later.

void VTSPortDlg::DeactivateChangedPorts()
{
   for ( int i = 0; i < m_pports->GetSize(); i++ )
   {
      // Find index into local array for this active port...

      int nLocalIndex;
      for ( nLocalIndex = 0; nLocalIndex < m_apOriginalPort.GetSize(); nLocalIndex++ )
         if ( m_apOriginalPort[nLocalIndex] == (CObject *) (*m_pports)[i] ) 
            break;

      if ( nLocalIndex < m_apOriginalPort.GetSize() )
      {
         // found cooresponding port in local array...
         // should we kill it?  Deactivate or what?

         if ( m_ports[nLocalIndex]->IsDirty() )
            (*m_pports)[i]->Deactivate();
         else
         {
            // Original port not dirty... so keep this one (might be active) by moving
            // out of the main array into the local one... we'll move local to main later
            delete m_ports[nLocalIndex];
            m_ports[nLocalIndex] = (*m_pports)[i];
            (*m_pports)[i] = NULL;
         }
      }
      else
      {
         (*m_pports)[i]->Deactivate();
      }
   }
}


void VTSPortDlg::MoveLocalPortsToMain()
{
   //Now it's safe to kill all of the ports in the main list... they'll be moved over later...
   m_pports->KillContents();

   // OK.. all active ports that should be deleted or have been changed are now deactivated...
   // Now move ports over from local storage into main array...
   // Just move them and destroy local array (not contents)
   
   for ( int i = 0; i < m_ports.GetSize(); i++ )
      m_pports->Add(m_ports[i]);

   // Kill local array without destroying memory (destructor will delete contents.. so make no pointers).
   m_ports.RemoveAll();
}


void VTSPortDlg::OnCancel() 
{
   // skip everything we were doing here...
   m_ports.KillContents();
   CDialog::OnCancel();
}



void VTSPortDlg::OnDelete() 
{
   POSITION selPos = m_PortList.GetFirstSelectedItemPosition();

   // make sure something was selected
   if (selPos == NULL)
      return;

   // figure out which item it is
   int    nItem = m_PortList.GetNextSelectedItem( selPos );

   // now deselect it, clearing out the controls in the process
   m_PortList.SetItemState( nItem, 0, LVIS_SELECTED | LVIS_FOCUSED );

   // delete it from the list
   m_PortList.DeleteItem( nItem );

   // delete the port from the local list
   delete m_ports[nItem];
   m_ports.RemoveAt(nItem);
   m_apOriginalPort.RemoveAt(nItem);

   // now select the next item if there is one... select the previous item if not... until null
   if ( nItem >= m_PortList.GetItemCount() )
      nItem = m_PortList.GetItemCount() - 1;

   if ( nItem >= 0 )
      m_PortList.SetItemState( nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

   m_fModified = true;
   EnableOK();
}
