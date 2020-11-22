// VTSDevicesTreePropPage.cpp : implementation file
//
// Property ID selection page for Device Configuration tree

#include "stdafx.h"
#include "vts.h"
#include "Propid.h"
#include "StringTables.h"

#include "VTSPropValue.h"
#include "VTSDevicesTreePropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char s_numeric[] = "<Numeric>";

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreePropPage property page

IMPLEMENT_DYNCREATE(VTSDevicesTreePropPage, VTSPropertyPage)

VTSDevicesTreePropPage::VTSDevicesTreePropPage( VTSPageCallbackInterface * pparent )
                      : VTSPropertyPage(VTSDevicesTreePropPage::IDD, pparent)
{
   //{{AFX_DATA_INIT(VTSDevicesTreePropPage)
   m_nPropID = 0;
   m_fIsArray = FALSE;
   //}}AFX_DATA_INIT

   m_pdevproperty = NULL;
}


VTSDevicesTreePropPage::VTSDevicesTreePropPage() : VTSPropertyPage()
{
   ASSERT(0);
}


VTSDevicesTreePropPage::~VTSDevicesTreePropPage()
{
}


void VTSDevicesTreePropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(VTSDevicesTreePropPage)
   DDX_Control(pDX, IDC_PROPCOMBO, m_propCombo);
   DDX_Text(pDX, IDC_PROPID, m_nPropID);
   DDX_Check(pDX, IDC_PROPISARRAY, m_fIsArray);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDevicesTreePropPage, VTSPropertyPage)
   //{{AFX_MSG_MAP(VTSDevicesTreePropPage)
   ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelChangePropCombo)
   ON_EN_CHANGE(IDC_PROPID, OnChangePropID)
   ON_BN_CLICKED(IDC_PROPISARRAY, OnPropIsArray)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreePropPage message handlers

BOOL VTSDevicesTreePropPage::OnKillActive() 
{
   VTSPropertyPage::OnKillActive();
   CtrlToProp(m_pdevproperty);

   return TRUE;
}

BOOL VTSDevicesTreePropPage::OnSetActive()
{
   // Alphabetical list of propertyIDs, plus a handle for numeric entry
   m_propCombo.ResetContent();
   NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.FillCombo( m_propCombo, true );
   m_propCombo.AddString( s_numeric );

   m_pdevproperty = (VTSDevProperty *) RetrieveCurrentData();
   if ( m_pdevproperty != NULL && !m_pdevproperty->IsKindOf(RUNTIME_CLASS(VTSDevProperty)) )
      m_pdevproperty = NULL;

   PropToCtrl(m_pdevproperty);

   // calls DDX
   VTSPropertyPage::OnSetActive();

   GetDlgItem(IDC_PROPISARRAY)->EnableWindow( m_pdevproperty != NULL && m_pdevproperty->GetValues()->GetSize() <= 1 );
   return m_pdevproperty != NULL;
}


void VTSDevicesTreePropPage::PropToCtrl( VTSDevProperty * pdevproperty )
{
   if (pdevproperty == NULL)
      return;

   const char *pString;
   int nStrings = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_nStrings;
   m_nPropID = pdevproperty->GetID();
   if (m_nPropID < nStrings)
   {
      // Sorted list: index is NOT ID, so select by STRING
      pString = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString(m_nPropID);
   }
   else
   {
      // Select the numeric-entry option
      pString = s_numeric;
   }
   m_propCombo.SelectString( 0, pString );

   if ( pdevproperty->GetValues()->GetSize() > 1 )
      pdevproperty->SetIsArray(true);

   m_fIsArray = pdevproperty->IsArray();
}


void VTSDevicesTreePropPage::CtrlToProp( VTSDevProperty * pdevproperty )
{
   if (pdevproperty == NULL)
      return;

   int sel = m_propCombo.GetCurSel();
   if (sel < 0)
      return;

   if (sel < NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_nStrings)
   {
      // Control is sorted, so selection index is NOT the object type.
      // Use the string to look up the type
      CString str;
      m_propCombo.GetLBText( sel, str );
      m_nPropID = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumValue( str );
   }
   else
   {
      // Numeric entry: edit control has set m_nPropID
   }

   pdevproperty->SetID(m_nPropID);
   pdevproperty->SetIsArray(m_fIsArray == TRUE);
}


void VTSDevicesTreePropPage::OnSelChangePropCombo()
{
   UpdateData(TRUE);
   CtrlToProp(m_pdevproperty);

   CDataExchange dx(this, FALSE);
   DDX_Text(&dx, IDC_PROPID, m_nPropID);

   NotifyOfDataChange();
}

// User changed the propertyID edit field
void VTSDevicesTreePropPage::OnChangePropID()
{
   CDataExchange dx(this, TRUE);
   DDX_Text(&dx, IDC_PROPID, m_nPropID);

   if ( m_nPropID < 0 )
      m_nPropID = 0;

   int nProps = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_nStrings;
   if (m_nPropID < nProps)
   {
      // Standard property: select by name
      m_propCombo.SelectString( 0, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString(m_nPropID) );
   }
   else
   {
      // Numeric entry: select final combo entry
      m_propCombo.SetCurSel( nProps );
   }

   // Update the combo
   OnSelChangePropCombo();
}

void VTSDevicesTreePropPage::OnPropIsArray()
{
   UpdateData(TRUE);
   CtrlToProp(m_pdevproperty);
   NotifyOfDataChange();
}
