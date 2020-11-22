// VTSObjectIdentifierDialog.cpp : implementation file
//
#include "stdafx.h"
#include "VTS.h"
#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSObjectIdentifierDialog dialog

VTSObjectIdentifierDialog::VTSObjectIdentifierDialog(CWnd* pParent /*=NULL*/)
   : CDialog(VTSObjectIdentifierDialog::IDD, pParent)
   , objID(0), validObjID(false)
{
   //{{AFX_DATA_INIT(VTSObjectIdentifierDialog)
   m_Encoded = _T("");
   //}}AFX_DATA_INIT
}

void VTSObjectIdentifierDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(VTSObjectIdentifierDialog)
   DDX_Control(pDX, IDC_VENDOR, m_Vendor);
   DDX_Control(pDX, IDC_RESERVED, m_Reserved);
   DDX_Control(pDX, IDC_INSTANCE, m_Instance);
   DDX_Control(pDX, IDC_OBJTYPECOMBO, m_ObjTypeCombo);
   DDX_Text(pDX, IDC_ENCODED, m_Encoded);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(VTSObjectIdentifierDialog, CDialog)
   //{{AFX_MSG_MAP(VTSObjectIdentifierDialog)
   ON_CBN_SELCHANGE(IDC_OBJTYPECOMBO, OnSelchangeObjTypeCombo)
   ON_EN_CHANGE(IDC_RESERVED, OnChangeReserved)
   ON_EN_CHANGE(IDC_VENDOR, OnChangeVendor)
   ON_EN_CHANGE(IDC_INSTANCE, OnChangeInstance)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSObjectIdentifierDialog message handlers

BOOL VTSObjectIdentifierDialog::OnInitDialog()
{
   CString     txt;

   CDialog::OnInitDialog();

   // Sorted list of object types
   m_ObjTypeCombo.ResetContent();
   NetworkSniffer::BAC_STRTAB_BACnetObjectType.FillCombo( m_ObjTypeCombo, true );

   // TODO: Vendor versus proprietary
   // Add these to allow values outside the standard range
   m_ObjTypeCombo.AddString("Reserved");
   m_ObjTypeCombo.AddString("Vendor");

   // use value given to us
   objType = (objID >> 22);
   instance = (objID & ((1 << 22) - 1));

   // format the object type
   txt.Format( "%d", objType );

   // Check for known values. MAX_DEFINED_OBJ is defined in VTS.h
   // TODO: these could use BAC_STRTAB_BACnetObjectType values
   // to guarantee a match with the known strings.
   if (objType < MAX_DEFINED_OBJ)
   {
      // Standard, known type
      // Select by STRING rather than index, since the list may be sorted
      m_ObjTypeCombo.SelectString( 0, NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString(objType) );

      m_Reserved.SetWindowText( txt );
      m_Reserved.EnableWindow( true );
      m_Vendor.SetWindowText( _T("") );
      m_Vendor.EnableWindow( false );
   }
   else if (objType < 128)
   {
      // Reserved type
      m_ObjTypeCombo.SetCurSel( MAX_DEFINED_OBJ );

      m_Reserved.SetWindowText( txt );
      m_Reserved.EnableWindow( true );
      m_Vendor.SetWindowText( _T("") );
      m_Vendor.EnableWindow( false );
   }
   else
   {
      // Vendor proprietary type
      m_ObjTypeCombo.SetCurSel( MAX_DEFINED_OBJ + 1 );

      m_Reserved.SetWindowText( _T("") );
      m_Reserved.EnableWindow( false );
      m_Vendor.SetWindowText( txt );
      m_Vendor.EnableWindow( true );
   }

   // format the instance
   txt.Format( "%d", instance );
   m_Instance.SetWindowText( txt );

   // update the encoded description
   UpdateEncoding( true );

   // finished
   return TRUE;
}

void VTSObjectIdentifierDialog::OnSelchangeObjTypeCombo()
{
   CString     txt;

   // Get the selected index from the control
   objType = m_ObjTypeCombo.GetCurSel();

   // Check for known values
   if (objType < MAX_DEFINED_OBJ)
   {
      // Since the list may be sorted, the selection index may not be
      // equal to the enumeration value.
      // Get the string from the combo and ask the StringTable to convert it to an int
      m_ObjTypeCombo.GetLBText(objType, txt);
      objType = NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumValue( txt );

      // Show numeric value of standard type
      txt.Format( "%d", objType );

      m_Reserved.SetWindowText( txt );
      m_Reserved.EnableWindow( true );
      m_Vendor.SetWindowText( _T("") );
      m_Vendor.EnableWindow( false );
   }
   else if (objType == MAX_DEFINED_OBJ)
   {
      // Show numeric value of reserved type, starting with first unassigned type
      txt.Format( "%d", objType );

      m_Reserved.SetWindowText( txt );
      m_Reserved.EnableWindow( true );
      m_Vendor.SetWindowText( _T("") );
      m_Vendor.EnableWindow( false );
   }
   else
   {
      // Show numeric value of vendor-proprietary type, starting with first proprietary value
      objType = 128;
      txt.Format( "%d", objType );

      m_Reserved.SetWindowText( _T("") );
      m_Reserved.EnableWindow( false );
      m_Vendor.SetWindowText( txt );
      m_Vendor.EnableWindow( true );
   }

   // update the encoded id
   UpdateEncoding( true );
}

void VTSObjectIdentifierDialog::OnChangeReserved()
{
   int         valu;
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   m_Reserved.GetWindowText( str );
   s = str.operator LPCTSTR();

   // make sure there's data
   if (!*s) {
      UpdateEncoding( false );
      return;
   }

   // extract the value, disallow out of range values
   valu = atoi( s );
   if ((valu < 0) || (valu >= 128)) {
      UpdateEncoding( false );
      return;
   }

   // save the new value
   objType = valu;

   // Update the control to reflect the new value
   if (objType < MAX_DEFINED_OBJ)
   {
      // Select known type by name
      m_ObjTypeCombo.SelectString( 0, NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString(objType) );
   }
   else
   {
      // Select "Reserved" string
      m_ObjTypeCombo.SetCurSel( MAX_DEFINED_OBJ );
   }

   // update the encoded id
   UpdateEncoding( true );
}

void VTSObjectIdentifierDialog::OnChangeVendor()
{
   int         valu;
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   m_Vendor.GetWindowText( str );
   s = str.operator LPCTSTR();

   // make sure there's data
   if (!*s) {
      UpdateEncoding( false );
      return;
   }

   // extract the value, disallow out of range values
   valu = atoi( s );
   if ((valu < 128) || (valu >= 1024)) {
      UpdateEncoding( false );
      return;
   }

   // save the new value
   objType = valu;

   // TODO: select known type by name...

   // update the encoded id
   UpdateEncoding( true );
}

void VTSObjectIdentifierDialog::OnChangeInstance()
{
   int         valu;
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   m_Instance.GetWindowText( str );
   s = str.operator LPCTSTR();

   // make sure there's data
   if (!*s) {
      UpdateEncoding( false );
      return;
   }

   // extract the value, disallow out of range values
   valu = atoi( s );
   if ((valu < 0) || (valu >= (1 << 22))) {
      UpdateEncoding( false );
      return;
   }

   // save the new value
   instance = valu;

   // update the encoded id
   UpdateEncoding( true );
}

//
// VTSObjectIdentifierDialog::UpdateEncoding
//

void VTSObjectIdentifierDialog::UpdateEncoding( bool valid )
{
   if (!valid) {
      m_Encoded = _T("Error");

      validObjID = false;
      objID = 0;
   } else {
      m_Encoded.Format( "%s, %d", NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString(objType), instance );

      validObjID = true;
      objID = (objType << 22) + instance;
   }

   // let the controls get synced
   UpdateData( false );
}
