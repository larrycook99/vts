// SendNetworkNumberIs.cpp : implementation file
//
// Dialog to send a Network-Number-Is NPDU

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendNetworkNumberIs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendNetworkNumberIs::pageContents;

IMPLEMENT_DYNAMIC(CSendNetworkNumberIs, CPropertyPage)

CSendNetworkNumberIs::CSendNetworkNumberIs()
: CSendPage( CSendNetworkNumberIs::IDD )
, m_networkNumber( this, IDC_DNET )
, m_typeCombo( this, IDC_TYPECOMBO, NetworkSniffer::BAC_STRTAB_NetworkNumberType, true )
{
}

CSendNetworkNumberIs::~CSendNetworkNumberIs()
{
}

void CSendNetworkNumberIs::DoDataExchange(CDataExchange* pDX)
{
   TRACE1( "CSendNetworkNumberIs::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSendNetworkNumberIs)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP

   m_networkNumber.UpdateData( pDX->m_bSaveAndValidate );
   m_typeCombo.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendNetworkNumberIs, CPropertyPage)
   //{{AFX_MSG_MAP(CSendNetworkNumberIs)
   ON_EN_CHANGE(IDC_DNET, OnChangeNetworkNumber)
   ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnSelchangeTypeCombo)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSendNetworkNumberIs::OnInitDialog() 
{
   TRACE0( "CSendNetworkNumberIs::OnInitDialog()\n" );
   CPropertyPage::OnInitDialog();

   // load the enumeration table
   m_typeCombo.LoadCombo();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// virtual
void CSendNetworkNumberIs::InitPage( void )
{
   TRACE0( "CSendNetworkNumberIs::InitPage()\n" );

   // flush the data
   m_networkNumber.ctrlNull = true;
   m_typeCombo.ctrlNull = false;
   m_typeCombo.m_enumValue = 1;

   // tell the NPCI this is a network layer message
   pageParent->NPCIPage.m_isNetMessage = true;
}

// virtual
void CSendNetworkNumberIs::EncodePage( CByteArray* contents )
{
   TRACE0( "CSendNetworkNumberIs::EncodePage()\n" );

   // Encode the message type
   contents->InsertAt( 0, (BYTE)0x13 );

   // Check and encode the contents
   if (!m_networkNumber.ctrlNull && !m_typeCombo.ctrlNull) {
      if ((m_networkNumber.intValue < 1) || (m_networkNumber.intValue > 65534))
         throw "Network number out of range 1..65534";

      contents->InsertAt( 1, m_networkNumber.intValue >> 8 );
      contents->InsertAt( 2, m_networkNumber.intValue & 0xFF );

      // encode the virtual terminal class
      BYTE val = 1;
      val = (BYTE)m_typeCombo.m_SelectValue;
      contents->InsertAt( 3, val);
   }
}

// virtual
void CSendNetworkNumberIs::SavePage( void )
{
   TRACE0( "CSendNetworkNumberIs::SavePage\n" );

   pageContents.Flush();
   m_networkNumber.SaveCtrl( pageContents );
   m_typeCombo.SaveCtrl( pageContents );
}

// virtual
void CSendNetworkNumberIs::RestorePage( int index )
{
   BACnetAPDUDecoder dec( pageContents );

   TRACE0( "CSendNetworkNumberIs::RestorePage\n" );

   if (dec.pktLength == 0)
      return;

   m_networkNumber.RestoreCtrl( dec );
   m_typeCombo.RestoreCtrl( dec );
}

// CSendNetworkNumberIs message handlers

void CSendNetworkNumberIs::OnChangeNetworkNumber() 
{
   m_networkNumber.UpdateData();
   SavePage();
   UpdateEncoded();
}

void CSendNetworkNumberIs::OnSelchangeTypeCombo()
{
   m_typeCombo.UpdateData();
   SavePage();
   UpdateEncoded();
}
