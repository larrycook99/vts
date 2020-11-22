// ReadAllPropSettingsDlg.cpp : implementation file
/*--------------------------------------------------------------------------------
last edit:  03-SEP-03[001] GJB  added the support two keywords: DNET and DADR
-----------------------------------------------------------------------------*/ 
#include "stdafx.h"
#include "vts.h"
#include "ReadAllPropSettingsDlg.h"

#include "VTSDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReadAllPropSettingsDlg dialog


CReadAllPropSettingsDlg::CReadAllPropSettingsDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CReadAllPropSettingsDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CReadAllPropSettingsDlg)
   m_strIUTIP = _T("");
   m_strNetwork = _T("");
   m_bDNET = FALSE;           //****001
   m_bUseRPM = FALSE;
   m_strDnetDadr = _T("");       //****001
   //}}AFX_DATA_INIT
}


void CReadAllPropSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CReadAllPropSettingsDlg)
   DDX_Control(pDX, IDC_RPALL_DNET_DADR, m_ctrlDnetDadr);         //****001
   DDX_CBString(pDX, IDC_RPALL_DA, m_strIUTIP);
   DDX_CBString(pDX, IDC_RPALL_NETWORK, m_strNetwork);
   DDX_Check(pDX, IDC_DNETPRESENT, m_bDNET);                //****001
   DDX_Check(pDX, IDC_USE_RPM, m_bUseRPM);
   DDX_CBString(pDX, IDC_RPALL_DNET_DADR, m_strDnetDadr);         //****001
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReadAllPropSettingsDlg, CDialog)
   //{{AFX_MSG_MAP(CReadAllPropSettingsDlg)
   ON_BN_CLICKED(IDC_DNETPRESENT, OnDnetpresent)               //****001
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadAllPropSettingsDlg message handlers
//MAD_DB extern VTSPortList gMasterPortList;

BOOL CReadAllPropSettingsDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   int i;

   CComboBox *pComboBox = (CComboBox*)GetDlgItem(IDC_RPALL_NETWORK);
   VTSDoc *pdoc = (VTSDoc*) ((VTSApp*)AfxGetApp())->GetWorkspace();

   VTSPorts *pports = (pdoc == NULL) ? NULL : pdoc->GetPorts();
   for (i = 0; pports != NULL && i < pports->GetSize(); i++)
   {
      // get a pointer to the ith port
      pComboBox->AddString( (*pports)[i]->GetName() );
   }
   pComboBox->SetCurSel(0);

   // Now set up the names...
   pComboBox = (CComboBox*)GetDlgItem(IDC_RPALL_DA);
   CComboBox* pComboDNET = (CComboBox*)GetDlgItem(IDC_RPALL_DNET_DADR);    //****001

   // TODO: if we require named destinations, we don't NEED both
   // addresses: use routing.
   VTSNames *pnames = (pdoc == NULL) ? NULL : pdoc->GetNames();
   for (i = 0; pnames != NULL && i < pnames->GetSize(); i++)
   {
      pComboBox->AddString((*pnames)[i]->GetName());
      pComboDNET->AddString((*pnames)[i]->GetName());                   //****001
   }

   // TODO: select IUT, or whatever is selected on the EPICS view's drop-list.
   //
   // For now, try for IUT.  SelectString won't change the selection if it fails, 
   // so we will get 0 if IUT is not defined
   pComboBox->SetCurSel(0);
   pComboBox->SelectString( 0, "IUT" );

   m_ctrlDnetDadr.EnableWindow(m_bDNET);                             //****001

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CReadAllPropSettingsDlg::OnOK() 
{
   // TODO: Add extra validation here
   UpdateData();
   //****001 begin
   if(m_bDNET && m_strDnetDadr == "")
   {
      AfxMessageBox("You must choose a VTSPort!");
      return;
   }
   //****001 end
   if(m_strNetwork == "")
   {
      AfxMessageBox("You must choose a VTSPort!");
      return;
   }
   if(m_strIUTIP == "")
   {
      AfxMessageBox("You must specified the IUTIP!");
      return;
   }

   CDialog::OnOK();
}
//****001 begin
void CReadAllPropSettingsDlg::OnDnetpresent() 
{
   // TODO: Add your control notification handler code here
   UpdateData(TRUE);

   m_ctrlDnetDadr.EnableWindow(m_bDNET);
}
//****001 end
