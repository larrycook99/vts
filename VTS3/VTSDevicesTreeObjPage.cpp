// VTSDevicesTreeObjPage.cpp : implementation file
//
// Object ID selection page for Device Configuration tree

#include "stdafx.h"
#include "vts.h"
#include "Propid.h"

#include "VTSPropValue.h"
#include "VTSDevicesTreeObjPage.h"

#include "bacnet.hpp"
#include "StringTables.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char s_numeric[] = "<Numeric>";

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeObjPage property page

IMPLEMENT_DYNCREATE(VTSDevicesTreeObjPage, VTSPropertyPage)

VTSDevicesTreeObjPage::VTSDevicesTreeObjPage( VTSPageCallbackInterface * pparent )
                      :VTSPropertyPage(VTSDevicesTreeObjPage::IDD, pparent)
{
   //{{AFX_DATA_INIT(VTSDevicesTreeObjPage)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   ClearData();
   m_pdevobject = NULL;
}


VTSDevicesTreeObjPage::VTSDevicesTreeObjPage() : VTSPropertyPage()
{
   ASSERT(0);
}


VTSDevicesTreeObjPage::~VTSDevicesTreeObjPage()
{
}


void VTSDevicesTreeObjPage::ClearData(void)
{
   m_nObjType = 0;
   m_nInstance = 0;
   ValidateTypeCode();
}


void VTSDevicesTreeObjPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(VTSDevicesTreeObjPage)
   DDX_Control(pDX, IDC_OBJTYPECOMBO, m_ObjTypeCombo);
   DDX_Text(pDX, IDC_NUMERIC_TYPE, m_nObjType);
   DDV_MinMaxInt(pDX, m_nObjType, 0, 1023);
   DDX_Text(pDX, IDC_INSTANCE, m_nInstance);
   DDV_MinMaxInt(pDX, m_nInstance, 0, 4194303);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDevicesTreeObjPage, VTSPropertyPage)
   //{{AFX_MSG_MAP(VTSDevicesTreeObjPage)
   ON_CBN_SELCHANGE(IDC_OBJTYPECOMBO, OnSelChangeObjCombo)
   ON_EN_CHANGE(IDC_NUMERIC_TYPE, OnChangeObjType)
   ON_EN_CHANGE(IDC_INSTANCE, OnChangeInstance)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL VTSDevicesTreeObjPage::OnSetActive()
{
   // Alphabetical list of types, plus a handle for numeric entry
   m_ObjTypeCombo.ResetContent();
   NetworkSniffer::BAC_STRTAB_BACnetObjectType.FillCombo( m_ObjTypeCombo, true );
   m_ObjTypeCombo.AddString( s_numeric );

   m_pdevobject = (VTSDevObject *) RetrieveCurrentData();
   if ( m_pdevobject != NULL &&  !m_pdevobject->IsKindOf(RUNTIME_CLASS(VTSDevObject)) )
      m_pdevobject = NULL;

   ObjToCtrl(m_pdevobject);

   // calls DDX
   VTSPropertyPage::OnSetActive();
   EnableControls( m_pdevobject != NULL );

   NotifyOfDataChange();
   return m_pdevobject != NULL;
}


BOOL VTSDevicesTreeObjPage::OnKillActive()
{
   // calls DDX
   VTSPropertyPage::OnKillActive();
   CtrlToObj(m_pdevobject);
   EnableControls(false);

   return TRUE;
}


void VTSDevicesTreeObjPage::ObjToCtrl( VTSDevObject * pdevobject )
{
   if (pdevobject == NULL)
   {
      ClearData();
      return;
   }

   m_nObjType = pdevobject->GetType();
   m_nInstance = pdevobject->GetInstance();

   // The combo was populated with NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nStrings
   const char *pString;
   int nStrings = NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nStrings;
   if (m_nObjType < nStrings)
   {
      pString = NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_pStrings[m_nObjType];
   }
   else
   {
      pString = s_numeric;
   }

   m_ObjTypeCombo.SelectString( 0, pString );
}


void VTSDevicesTreeObjPage::CtrlToObj( VTSDevObject * pdevobject )
{
   if (pdevobject == NULL)
      return;

   // put member data back into device object
   // Validate all control variables... stuff later.

   ValidateTypeCode();

   // Control is sorted, so selection index is NOT the object type.
   // Use the string to look up the type
   int sel = m_ObjTypeCombo.GetCurSel();
   if (sel < 0)
      return;

   int nStrings = NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nStrings;
   if (sel < nStrings)
   {
      CString str;
      m_ObjTypeCombo.GetLBText( sel, str );
      m_nObjType = NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumValue( str );
   }
   else
   {
      // Numeric entry: edit control has set m_nObjType
   }

   unsigned oldID = pdevobject->GetID();
   pdevobject->SetID(m_nObjType, m_nInstance);

   // If no current properties,
   CObArray * pobarray = (CObArray *)pdevobject->GetProperties();
   if ( pobarray != NULL && pobarray->GetSize() == 0 )
   {
      // TODO: LJT add all required properties for selected object type
      //       Create file format to read defaults from?? Suggest use same format as
      //       chosen for the import/export functionality
      //
      // NOTE: This code is largely hidden by similar code in VTSDevicesTreeDlg::OnNewObject,
      // although it will still be invoked if all properties of an object are deleted.
      // The problem with creating properties HERE is that we have no access to the
      // tree control, so we can't SHOW the new properties until the dialog is closed
      // and re-opened, while OnNewObject can refresh the tree.
      //
      // Note also that the initial value doesn't quite work:
      // - Create object
      // - Change object-type: this code generates name as "type 0"
      // - Change instance, but name already exists, and isn't touched.
      if ( m_nObjType < MAX_DEFINED_OBJ )
      {
         // For now, at least ONE property...
         VTSDevValue * pdevvalue = new VTSDevValue();
         pdevvalue->m_nType = PRIM_CHARACTER_STRING;
         pdevvalue->m_nContext = -1;

         BACnetAPDUEncoder compEncoder;
         BACnetCharacterString xx;
         CString str;
         str.Format( "%s %u", NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString( m_nObjType ), m_nInstance );
         xx.SetValue( str, 0 );
         xx.Encode( compEncoder, pdevvalue->m_nContext );
//       VTSCharacterStringCtrl m_CharStr(this, 0);
//       m_CharStr.SetValue( "ObjectName", 0 );
//       m_CharStr.Encode( compEncoder, pdevvalue->m_nContext );
         memcpy( pdevvalue->m_abContent, compEncoder.pktBuffer, compEncoder.pktLength );
         pdevvalue->m_nLength = compEncoder.pktLength;

         VTSDevProperty * devprop = new VTSDevProperty();
         devprop->SetID(OBJECT_NAME);
         CObArray * pobvalarray = (CObArray *)devprop->GetValues();
         pobvalarray->Add(pdevvalue);

         pobarray->Add(devprop);
      }
   }
/*
   // Load Default values for all objects
   PICS::PICSdb * pd = new PICS::PICSdb;

   // need to open file and load objects into db
   File ifile = fopen( "myfile", "r" );
   char data[500];

   readline(data, sizeof(data));

   // keep reading until find 'objects in db'
   PICS::ReadObjects(pd);

   // now repeat for each object until we find the matching type

   PICS::generic_object * pObj = (PICS::generic_object *) pd->Database;
   DWORD dwPropID;
   char szPropName[100];
   BACnetAnyValue bacnetAnyValue;
   // Load default values for object need to find object type
   for (int i = 0; i < sizeof(pObj->propflags); i++ )
   {
      if ( PICS::GetPropNameSupported(szPropName, i, pObj->object_type, pObj->propflags, &dwPropID, NULL) > 0 )
      {
         // got number add Property using dwPropID;
         VTSDevProperty * devprop = new VTSDevProperty();
         pobarray->Add(devprop);
         devprop->SetID(dwProp);

         // try to determine value
         if ( (pObj->propflags[i] & ValueUnknown) != 0 )
         {
            // property not specified don't do anything
         }
         else
         {
            try
            {
               PICS::CreatePropertyFromEPICS( pObj, (PICS::BACnetPropertyIdentifier) dwPropID, &bacnetAnyValue );
            }
            catch(...)
            {
               bacnetAnyValue.SetObject(NULL);
            }
            // now set value
            CObArray * pobvalarray = (CObArray *)devprop->GetValues();
            VTSDevValue * pdevvalue = new VTSDevValue();
            pobvalarray->Add(pdevvalue);
            pdevvalue->m_nContext = -1;

            pdevvalue->m_nType = bacnetAnyValue.DataType();

            BACnetAPDUEncoder compEncoder;
            bacnetAnyValue.Encode( compEncoder, -1 );
            memcpy( pdevvalue->m_abContent, compEncoder.pktBuffer, compEncoder.pktLength );
         }
      }
   }
*/
}


void VTSDevicesTreeObjPage::ValidateTypeCode(void)
{
   if (m_nInstance < 0)
      m_nInstance = 0;
   else if (m_nInstance >= (1 << 22))
      m_nInstance = (1 << 22)-1;

   if (m_nObjType < 0)
      m_nObjType = 0;
   else if (m_nObjType >= 1024)
      m_nObjType = 1023;
}


void VTSDevicesTreeObjPage::EnableControls( bool fEnable )
{
   GetDlgItem(IDC_OBJTYPECOMBO)->EnableWindow( fEnable );
   GetDlgItem(IDC_NUMERIC_TYPE)->EnableWindow( fEnable );
   GetDlgItem(IDC_INSTANCE)->EnableWindow( fEnable );
}

void VTSDevicesTreeObjPage::OnSelChangeObjCombo()
{
   UpdateData(TRUE);
   CtrlToObj(m_pdevobject);

   CDataExchange dx(this, FALSE);
   DDX_Text(&dx, IDC_NUMERIC_TYPE, m_nObjType);

   NotifyOfDataChange();
}

// User changed the object type edit field
void VTSDevicesTreeObjPage::OnChangeObjType()
{
   CDataExchange dx(this, TRUE);
   DDX_Text(&dx, IDC_NUMERIC_TYPE, m_nObjType);

   if ( m_nObjType < 0 )
      m_nObjType = 0;

   int nTypes = NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nStrings;
   if (m_nObjType < nTypes)
   {
      // Standard object type: select by name
      m_ObjTypeCombo.SelectString( 0, NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString(m_nObjType) );
   }
   else
   {
      // Numeric entry: select final combo entry
      m_ObjTypeCombo.SetCurSel( nTypes );
   }

   // Update the combo
   OnSelChangeObjCombo();
}

void VTSDevicesTreeObjPage::OnChangeInstance( void )
{
   UpdateData(TRUE);
   CtrlToObj(m_pdevobject);
   NotifyOfDataChange();
}
