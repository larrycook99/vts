// SendVendorNPDU.cpp : implementation file
//
// This isn't ONLY vendor NPDU anymore - now also allows standard network message

#include "stdafx.h"
#include "VTS.h"
#include "Send.h"
#include "SendVendorNPDU.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendVendorNPDU::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendVendorNPDU dialog

IMPLEMENT_DYNCREATE( CSendVendorNPDU, CPropertyPage )

#pragma warning( disable : 4355 )
CSendVendorNPDU::CSendVendorNPDU( void )
   : CSendPage( CSendVendorNPDU::IDD )
   , m_MsgType( this, IDC_MSGTYPE )
   , m_VendorID( this, IDC_VENDORID )
   , m_Data( this, IDC_DATA )
{
   //{{AFX_DATA_INIT(CSendVendorNPDU)
   //}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void CSendVendorNPDU::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSendVendorNPDU)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendVendorNPDU, CPropertyPage)
   //{{AFX_MSG_MAP(CSendVendorNPDU)
   ON_EN_CHANGE(IDC_MSGTYPE, OnChangeMsgType)
   ON_EN_CHANGE(IDC_VENDORID, OnChangeVendorID)
   ON_EN_CHANGE(IDC_DATA, OnChangeData)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
// CSendVendorNPDU::InitPage
//

void CSendVendorNPDU::InitPage( void )
{
   TRACE0( "CSendVendorNPDU::InitPage()\n" );

   // flush the data, disable vendorID unless it is needed
   m_MsgType.ctrlNull = true;
   m_VendorID.ctrlNull = true;
   m_Data.Flush();

   pageParent->NPCIPage.m_isNetMessage = true;
}

//
// CSendVendorNPDU::EncodePage
//

void CSendVendorNPDU::EncodePage( CByteArray* contents )
{
   CByteArray  header;

   TRACE0( "CSendVendorNPDU::EncodePage()\n" );

   // Validate message type
   if (m_MsgType.ctrlNull)
      throw "Message type required";
   if (m_MsgType.intValue > 0xFF)
      throw "Message type out of range 0..255";
   header.Add( m_MsgType.intValue );

   // If message type is 0x80 or higher, validate vendor ID
   if (m_MsgType.intValue >= 0x80)
   {
      if (m_VendorID.ctrlNull)
         throw "Vendor ID required";
      if ((m_VendorID.intValue < 0) || (m_VendorID.intValue > 65535))
         throw "Vendor ID out of range 0..65535";

      header.Add( m_VendorID.intValue >> 8 );
      header.Add( m_VendorID.intValue & 0x0FF );
   }

   // encode the contents
   for (int i = 0; i < m_Data.Length(); i++)
   {
      header.Add( m_Data[i] );
   }

   // stuff the header on the front
   contents->InsertAt( 0, &header );
}

//
// CSendVendorNPDU::SavePage
//

void CSendVendorNPDU::SavePage( void )
{
   TRACE0( "CSendVendorNPDU::SavePage\n" );

   pageContents.Flush();

   m_MsgType.SaveCtrl( pageContents );
   m_VendorID.SaveCtrl( pageContents );
   m_Data.SaveCtrl( pageContents );
}

//
// CSendVendorNPDU::RestorePage
//

void CSendVendorNPDU::RestorePage( int index )
{
   BACnetAPDUDecoder dec( pageContents );

   TRACE0( "CSendVendorNPDU::RestorePage\n" );

   if (dec.pktLength == 0)
      return;

   m_MsgType.RestoreCtrl( dec );
   m_VendorID.RestoreCtrl( dec );
   m_Data.RestoreCtrl( dec );
}

void CSendVendorNPDU::OnChangeMsgType() 
{
   TRACE0( "CSendVendorNPDU::OnChangeMsgType()\n" );
   m_MsgType.UpdateData();

   // Enable of disable the vendorID control
   if (m_MsgType.intValue >= 0x80)
   {
      // Must have a vendorID for a proprietary message
      m_VendorID.Enable();
   }
   else
   {
      // Empty and hide the vendorID for a standard network message
      GetDlgItem( IDC_VENDORID )->SetWindowText( "" );
      m_VendorID.Disable();
   }

   SavePage();
   UpdateEncoded();
}

void CSendVendorNPDU::OnChangeVendorID() 
{
   TRACE0( "CSendVendorNPDU::OnChangeVendorID()\n" );
   m_VendorID.UpdateData();
   SavePage();
   UpdateEncoded();
}

void CSendVendorNPDU::OnChangeData() 
{
   TRACE0( "CSendVendorNPDU::OnChangeData()\n" );
   m_Data.UpdateData();
   SavePage();
   UpdateEncoded();
}
