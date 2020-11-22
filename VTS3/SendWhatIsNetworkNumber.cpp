// SendWhatIsNetworkNumber.cpp : implementation file
//
// Dialog to send a What-Is-Network-Number NPDU

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendWhatIsNetworkNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSendWhatIsNetworkNumber, CPropertyPage)

CSendWhatIsNetworkNumber::CSendWhatIsNetworkNumber()
: CSendPage( CSendWhatIsNetworkNumber::IDD )
{
}

CSendWhatIsNetworkNumber::~CSendWhatIsNetworkNumber()
{
}

void CSendWhatIsNetworkNumber::DoDataExchange(CDataExchange* pDX)
{
   TRACE1( "CSendWhatIsNetworkNumber::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSendWhatIsNetworkNumber)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendWhatIsNetworkNumber, CPropertyPage)
   //{{AFX_MSG_MAP(CSendWhatIsNetworkNumber)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSendWhatIsNetworkNumber::OnInitDialog() 
{
   TRACE0( "CSendWhatIsNetworkNumber::OnInitDialog()\n" );
   CPropertyPage::OnInitDialog();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// virtual
void CSendWhatIsNetworkNumber::InitPage( void )
{
   TRACE0( "CSendWhatIsNetworkNumber::InitPage()\n" );

   // Tell the NPCI this is a network layer message
   pageParent->NPCIPage.m_isNetMessage = true;
}

// virtual
void CSendWhatIsNetworkNumber::EncodePage( CByteArray* contents )
{
   TRACE0( "CSendWhatIsNetworkNumber::EncodePage()\n" );

   // Encode the message type
   contents->InsertAt( 0, (BYTE)0x12 );
}
