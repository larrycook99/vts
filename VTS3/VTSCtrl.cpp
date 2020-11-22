
#include "stdafx.h"

#include <iostream>

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include"VTS.h"
#include "VTSCtrl.h"
#include "VTSDoc.h"

#include "ScriptBase.h"
#include "ScriptKeywords.h"

#include "SendPage.h"
#include "VTSVendorPropIDDlg.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////

//
// VTSCtrl::VTSCtrl
//

VTSCtrl::VTSCtrl( const CWnd* wp, int id )
   : ctrlNull(true), ctrlEnabled(true)
   , ctrlWindow(wp), ctrlID(id)
{
}

//
// VTSCtrl::~VTSCtrl
//

VTSCtrl::~VTSCtrl( void )
{
}

//
// VTSCtrl::Enabled
//

void VTSCtrl::Enable( void )
{
   if (ctrlEnabled)
      return;

   ctrlEnabled = true;
   ctrlWindow->GetDlgItem( ctrlID )->EnableWindow( true );
}

//
// VTSCtrl::Disable
//

void VTSCtrl::Disable( void )
{
   if (!ctrlEnabled)
      return;

   ctrlEnabled = false;
   ctrlWindow->GetDlgItem( ctrlID )->EnableWindow( false );
}

//
// VTSCtrl::UpdateData
//

void VTSCtrl::UpdateData( BOOL bCtrlToObj )
{
   if (bCtrlToObj)
      CtrlToObj();
   else
      ObjToCtrl();
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSAddrCtrl
//

VTSAddrCtrl::VTSAddrCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
   , emptyIsNull( true )
{
}

//
// VTSAddrCtrl::CtrlToObj
//

void VTSAddrCtrl::CtrlToObj( void )
{
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // no data might mean empty is null
   ctrlNull = (!*s && emptyIsNull);

   try {
      int            valu;
      ScriptScanner  scan( s );
      ScriptToken    tok;

      // get the content
      scan.Next( tok );
      if (tok.tokenType == scriptEOL)
         return;

      if (tok.tokenEnc == scriptIntegerEnc) {
         tok.IsInteger( valu );

         Flush();
         if ((valu >= 0) && (valu < 256))
            Append( valu );
      }
      else if (tok.tokenEnc == scriptHexEnc) {
         // use the built-in decoder
         Decode( s );
      }
      else if (tok.tokenEnc == scriptIPEnc) {
         BACnetIPAddr   addr( s );

         // flush the current contents
         Flush();

         // copy the new stuff in
         Insert( addr.addrAddr, addr.addrLen, 0 );
      }
   }
   catch (...) {
   }
}

//
// VTSAddrCtrl::ObjToCtrl
//

void VTSAddrCtrl::ObjToCtrl( void )
{
   CString  str;
   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSAddrCtrl::SaveCtrl
//

void VTSAddrCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSAddrCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSAddrCtrl::RestoreCtrl
//

void VTSAddrCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSAddrCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

VTSAddrComboCtrl::VTSAddrComboCtrl( const CWnd* wp, int cid, int tid )
   : VTSCtrl( wp, tid ), m_ctrlComboID(cid), m_pnamesCtrl(0)
{
}


void VTSAddrComboCtrl::LoadCombo( VTSNames *pnames, VTSPort * pport, bool okBroadcast /* = false */ )
{
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( m_ctrlComboID );
   int            indx;

   VTSName * pname;

   if (m_pnamesCtrl)       // already initialized?
      return;

   // save a pointer to the name list
   m_pnamesCtrl = pnames;

   // scan through the names
   for (int i = 0; i < m_pnamesCtrl->GetSize(); i++)
   {
      pname = (VTSName *) (*m_pnamesCtrl)[i];

      // if it matches our port, and the shape of our address, save the name and index
      if (((pname->m_pportLink == pport) || (pname->m_pportLink == NULL)) 
            && 
         (IsNameMatch(pname, pport, okBroadcast)))
      {
         indx = cbp->AddString( pname->m_strName );
         if ( indx == CB_ERR || indx == CB_ERRSPACE )
            break;

         cbp->SetItemData( indx, i );
      }
   }
}


bool VTSAddrComboCtrl::IsNameMatch( VTSName * pname, VTSPort * pport, bool okBroadcast ) const
{
   // The Name's address must either be the same length as ours, or be a broadcast
   return ((pname->m_bacnetaddr.addrType == localStationAddr) && 
         (pname->m_bacnetaddr.addrLen == pport->portEndpoint->portLocalAddr.addrLen))
            ||
         (okBroadcast && (pname->m_bacnetaddr.addrType == localBroadcastAddr))
            || 
         (okBroadcast && (pname->m_bacnetaddr.addrType == globalBroadcastAddr));
}


void VTSAddrComboCtrl::Selchange( void )
{
   CComboBox     * cbp = (CComboBox *)ctrlWindow->GetDlgItem( m_ctrlComboID );
   int            i = cbp->GetItemData(cbp->GetCurSel());

   AssignAddress( (VTSName *) (*m_pnamesCtrl)[i] );
   ctrlNull = false;
   ObjToCtrl();
}



void VTSAddrComboCtrl::FindName( LPCSTR lpszName )
{
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( m_ctrlComboID );
   VTSName        *pname;

   // scan through the names
   for (int i = 0; i < cbp->GetCount(); i++)
   {
      pname = (VTSName *) (*m_pnamesCtrl)[cbp->GetItemData(i)];

      // if it matches the name, set the selection
      if ( pname->m_strName.Compare(lpszName) == 0 )
      {
         cbp->SetCurSel( i );
         Selchange();
         break;
      }
   }
}



void VTSAddrComboCtrl::Enable( void )
{
   VTSCtrl::Enable();
   ctrlWindow->GetDlgItem( m_ctrlComboID )->EnableWindow( true );
}


void VTSAddrComboCtrl::Disable( void )
{
   VTSCtrl::Disable();
   ctrlWindow->GetDlgItem( m_ctrlComboID )->EnableWindow( false );
}

//
// VTSEnetAddrCtrl::CtrlToObj
//

void VTSAddrComboCtrl::CtrlToObj( void )
{
   CString     str;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );

   CtrlToAddress((LPCTSTR) str);

   if ( ctrlNull )
      return;

   // get the combo box
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( m_ctrlComboID );

   for (int i = 0; i < cbp->GetCount(); i++)
   {
      VTSName * pname = (VTSName *) (*m_pnamesCtrl)[cbp->GetItemData(i)];

      // if it matches our port, save the name and index
      if ( IsAddressMatch(&pname->m_bacnetaddr) )
      {
         cbp->SetCurSel( i );
         return;
      }
   }

   // clear if no matching name found
   cbp->SetWindowText( _T("") );
}


void VTSAddrComboCtrl::CtrlToAddress( LPCTSTR s )
{
   int         upperNibble, lowerNibble;
   char     c;

   // remove contents
   addrLen = 0;

   // translate the text into octets
   for (;;) {
      // look for a hex digit
      while ((c = ToUpper(*s++)) && !IsXDigit(c));

      if (!c)
         break;

      upperNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

      // look for another hex digit
      while ((c = ToUpper(*s++)) && !IsXDigit(c));

      if (!c)
         break;

      lowerNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

      // add the byte
      if (addrLen < sizeof(addrAddr))
         addrAddr[ addrLen++ ] = (upperNibble << 4) + lowerNibble;
   }

   // if nothing specified, set value to null
   ctrlNull = (addrLen == 0);
}


bool VTSAddrComboCtrl::IsAddressMatch( BACnetAddress * pbacnetaddr )
{
   return pbacnetaddr->addrLen == addrLen  &&  memcmp(pbacnetaddr->addrAddr, addrAddr, addrLen) == 0;
}


//
// VTSEnetAddrCtrl::ObjToCtrl
//

void VTSAddrComboCtrl::ObjToCtrl( void )
{
   char  buff[kMaxAddressLen * 3], *s = buff;

   // clear the buffer
   buff[0] = 0;

   // encode the address
   if (!ctrlNull)
      for (int i = 0; i < addrLen; i++)
      {
         if (s != buff)
            *s++ = '-';
         sprintf( s, "%02X", addrAddr[i] );
         s += 2;
      }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( buff );
}


void VTSAddrComboCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
   if (ctrlNull)
      BACnetNull().Encode( enc );
   else {
      BACnetInteger( addrType ).Encode( enc );
      BACnetInteger( addrNet ).Encode( enc );
      BACnetOctetString( addrAddr, addrLen ).Encode( enc );
   }
}


void VTSAddrComboCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag)
   {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   }
   else
   {
      BACnetInteger  type, net;
      BACnetOctetString addr( 6 );

      ctrlNull = false;

      type.Decode( dec );
      addrType = (BACnetAddressType)type.intValue;
      net.Decode( dec );
      addrNet = net.intValue;
      addr.Decode( dec );
      addrLen = addr.strLen;
      memcpy( addrAddr, addr.strBuff, addrLen );
   }
}


/////////////////////////////////////////////////////////////////////////////

//
// VTSEnetAddrCtrl
//

// MAD_DB
VTSEnetAddrCtrl::VTSEnetAddrCtrl( const CWnd* wp, int cid, int tid )
// : VTSCtrl( wp, tid ), ctrlComboID(cid), ctrlNameList(0)
   : VTSAddrComboCtrl( wp, cid, tid )
{
}


void VTSEnetAddrCtrl::AssignAddress(VTSName * pname)
{
   unsigned char  broadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

   addrLen = 6;
   if (pname->m_bacnetaddr.addrType == localBroadcastAddr)
      memcpy( addrAddr, broadcastAddr, 6 );
   else
      memcpy( addrAddr, pname->m_bacnetaddr.addrAddr, 6 );
}




/* MAD_DB  Now in baseclass

void VTSEnetAddrCtrl::LoadCombo( VTSNameList *nameList, unsigned int portID )
{
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   int            indx
   ;
//MAD_DB VTSNameDesc    name;
   VTSName * pname;

   // already initialized?
   if (m_pnamesCtrl)
      return;

   // save a pointer to the name list
   m_pnamesCtrl = pnames;

   // scan through the names
   for (int i = 0; i < nameList->Length(); i++) {
      // read the description
      ctrlNameList->ReadName( i, &name );

      // if it matches our port, save the name and index
      if ((name.namePort == portID) || (name.namePort == 0))
         if (((name.nameAddr.addrType == localStationAddr) && (name.nameAddr.addrLen == 6))
            || (name.nameAddr.addrType == localBroadcastAddr)) {
               indx = cbp->AddString( name.nameName );
               if ((indx == CB_ERR) || (indx == CB_ERRSPACE))
                  break;
               cbp->SetItemData( indx, i );
            }
   }
}


void VTSEnetAddrCtrl::Selchange( void )
{
   unsigned char  broadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
   ;
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   VTSNameDesc    name
   ;
   int            i = cbp->GetItemData( cbp->GetCurSel() )
   ;

   ctrlNameList->ReadName( i, &name );

   addrLen = 6;
   if (name.nameAddr.addrType == localBroadcastAddr)
      memcpy( addrAddr, broadcastAddr, 6 );
   else
      memcpy( addrAddr, name.nameAddr.addrAddr, 6 );

   ctrlNull = false;

   ObjToCtrl();
}
//
// VTSEnetAddrCtrl::FindName
//

void VTSEnetAddrCtrl::FindName( const char *name )
{
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   VTSNameDesc    nameDesc
   ;

   // scan through the names
   for (int i = 0; i < cbp->GetCount(); i++) {
      // read the description
      ctrlNameList->ReadName( cbp->GetItemData( i ), &nameDesc );

      // if it matches the name, set the selection
      if (strcmp(nameDesc.nameName,name) == 0) {
         cbp->SetCurSel( i );
         Selchange();
         break;
      }
   }
}

//
// VTSEnetAddrCtrl::Enable
//

void VTSEnetAddrCtrl::Enable( void )
{
   VTSCtrl::Enable();
   ctrlWindow->GetDlgItem( ctrlComboID )->EnableWindow( true );
}

//
// VTSEnetAddrCtrl::Disable
//

void VTSEnetAddrCtrl::Disable( void )
{
   VTSCtrl::Disable();
   ctrlWindow->GetDlgItem( ctrlComboID )->EnableWindow( false );
}

//
// VTSEnetAddrCtrl::CtrlToObj
//

void VTSEnetAddrCtrl::CtrlToObj( void )
{
   int         upperNibble, lowerNibble
   ;
   char     c
   ;
   CString     str
   ;
   LPCTSTR     s
   ;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // remove contents
   addrLen = 0;

   // translate the text into octets
   for (;;) {
      // look for a hex digit
      while ((c = ToUpper(*s++)) && !IsXDigit(c))
         ;
      if (!c) break;
      upperNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

      // look for another hex digit
      while ((c = ToUpper(*s++)) && !IsXDigit(c))
         ;
      if (!c) break;
      lowerNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

      // add the byte
      if (addrLen < sizeof(addrAddr))
         addrAddr[ addrLen++ ] = (upperNibble << 4) + lowerNibble;
   }

   // if nothing specified, set value to null
   ctrlNull = (addrLen == 0);
   if (ctrlNull)
      return;

   // get the combo box
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   VTSNameDesc    name
   ;

   // scan through the names
   bool found = false;
   for (int i = 0; i < cbp->GetCount(); i++) {
      // read the description
      ctrlNameList->ReadName( cbp->GetItemData( i ), &name );

      // if it matches our port, save the name and index
      if ((name.nameAddr.addrLen == addrLen) && (memcmp(name.nameAddr.addrAddr,addrAddr,addrLen) == 0)) {
         cbp->SetCurSel( i );
         found = true;
         break;
      }
   }

   // clear if no matching name found
   if (!found)
      cbp->SetWindowText( _T("") );
}


void VTSEnetAddrCtrl::ObjToCtrl( void )
{
   char  buff[kMaxAddressLen * 3], *s = buff
   ;

   // clear the buffer
   buff[0] = 0;

   // encode the address
   if (!ctrlNull)
      for (int i = 0; i < addrLen; i++) {
         if (s != buff)
            *s++ = '-';
         sprintf( s, "%02X", addrAddr[i] );
         s += 2;
      }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( buff );
}

//
// VTSEnetAddrCtrl::SaveCtrl
//

void VTSEnetAddrCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSEnetAddrCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else {
      BACnetInteger( addrType ).Encode( enc );
      BACnetInteger( addrNet ).Encode( enc );
      BACnetOctetString( addrAddr, addrLen ).Encode( enc );
   }
}
//
// VTSEnetAddrCtrl::RestoreCtrl
//

void VTSEnetAddrCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag
   ;

// TRACE0( "VTSEnetAddrCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      BACnetInteger  type, net
      ;
      BACnetOctetString addr( 6 )
      ;

      ctrlNull = false;

      type.Decode( dec );
      addrType = (BACnetAddressType)type.intValue;
      net.Decode( dec );
      addrNet = net.intValue;
      addr.Decode( dec );
      addrLen = addr.strLen;
      memcpy( addrAddr, addr.strBuff, addrLen );
   }
}
*/



/////////////////////////////////////////////////////////////////////////////

//
// VTSMSTPAddrCtrl
//

VTSMSTPAddrCtrl::VTSMSTPAddrCtrl( const CWnd* wp, int cid, int tid )
   : VTSAddrComboCtrl( wp, cid, tid )
{
}


void VTSMSTPAddrCtrl::AssignAddress(VTSName * pname)
{
   addrLen = 1;
   addrAddr[0] = pname->m_bacnetaddr.addrType == localBroadcastAddr ? 0xFF : addrAddr[0] = pname->m_bacnetaddr.addrAddr[0];
}


void VTSMSTPAddrCtrl::CtrlToAddress( LPCTSTR s )
{
   ctrlNull = (s == NULL || *s == 0);

   if ( !ctrlNull )
   {
      int n = atoi(s);
      if ( n < 0 || n > 255 )
         n = 0;

      addrType = n == 255 ? localBroadcastAddr : localStationAddr;
      addrLen = 1;
      addrAddr[0] = n & 0xFF;
   }
}


void VTSMSTPAddrCtrl::ObjToCtrl( void )
{
   char txt[10];

   txt[0] = 0;
   if ( !ctrlNull )
      _itoa((int) addrAddr[0], txt, 10);

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( txt );
}


/////////////////////////////////////////////////////////////////////////////

//
// VTSIPAddrCtrl
//

VTSIPAddrCtrl::VTSIPAddrCtrl( const CWnd* wp, int cid, int tid )
// : VTSCtrl( wp, tid ), ctrlComboID(cid), ctrlNameList(0)
   : VTSAddrComboCtrl( wp, cid, tid )
{
}


void VTSIPAddrCtrl::AssignAddress(VTSName * pname)
{

   unsigned char  localbroadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xBA, 0xC0 };
   char szHostName[128];
   if( gethostname(szHostName, 128) == 0 )
   {
      // Get host adresses
      struct hostent * pHost;
      pHost = gethostbyname(szHostName);
      LPSTR lpAddr = pHost->h_addr_list[0];
      if (lpAddr)
         memcpy (&localbroadcastAddr, lpAddr, 3);
   }  //To get localbroadcast address
      //Modified by Zhu Zhenhua, 2003-12-12


   unsigned char  globalbroadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xBA, 0xC0 };

   addrLen = 6;
   if (pname->m_bacnetaddr.addrType == localBroadcastAddr)
      memcpy( addrAddr, localbroadcastAddr, 6 );
   else if(pname->m_bacnetaddr.addrType == globalBroadcastAddr)
      memcpy( addrAddr, globalbroadcastAddr, 6 );
   else
      memcpy( addrAddr, pname->m_bacnetaddr.addrAddr, 6 );
}


void VTSIPAddrCtrl::CtrlToAddress( LPCTSTR s )
{
   unsigned long  host;
   unsigned short port;

   ctrlNull = (s == NULL || *s == 0);

   if ( !ctrlNull )
   {
      BACnetIPAddr::StringToHostPort( s, &host, 0, &port );

      // Can't call BACnetIPAddr's Pack method because we don't inherit
      // from BACnetIPAddr anymore...  Duplicate Pach method and call the static

      addrType = localStationAddr;
      addrLen = sizeof(unsigned long) + sizeof(unsigned short);
      BACnetIPAddr::Pack( addrAddr, host, port );
   }
}

//
// VTSIPAddrCtrl::ObjToCtrl
//

void VTSIPAddrCtrl::ObjToCtrl( void )
{
   char  *txt = (ctrlNull ? "" : BACnetIPAddr::AddressToString(addrAddr));

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( txt );
}


/* MAD_DB Now in base class
void VTSIPAddrCtrl::LoadCombo( VTSNameList *nameList, unsigned int portID )
{
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   int            indx
   ;
   VTSNameDesc    name;

   // already initialized?
   if (ctrlNameList)
      return;

   // save a pointer to the name list
   m_pnamesCtrl = pnames;

   // scan through the names
   for (int i = 0; i < nameList->Length(); i++) {
      // read the description
      ctrlNameList->ReadName( i, &name );

      // if it matches our port, save the name and index
      if ((name.namePort == portID) || (name.namePort == 0))
         if (((name.nameAddr.addrType == localStationAddr) && (name.nameAddr.addrLen == 6))
            || (name.nameAddr.addrType == localBroadcastAddr)) {
               indx = cbp->AddString( name.nameName );
               if ((indx == CB_ERR) || (indx == CB_ERRSPACE))
                  break;
               cbp->SetItemData( indx, i );
            }
   }
}

void VTSIPAddrCtrl::Selchange( void )
{
   unsigned char  broadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xBA, 0xC0 }
   ;
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   VTSNameDesc    name
   ;
   int            i = cbp->GetItemData( cbp->GetCurSel() )
   ;

   ctrlNameList->ReadName( i, &name );

   addrLen = 6;
   if (name.nameAddr.addrType == localBroadcastAddr)
      memcpy( addrAddr, broadcastAddr, 6 );
   else
      memcpy( addrAddr, name.nameAddr.addrAddr, 6 );

   ctrlNull = false;

   ObjToCtrl();
}
//
// VTSIPAddrCtrl::FindName
//

void VTSIPAddrCtrl::FindName( const char *name )
{
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   VTSNameDesc    nameDesc
   ;

   // scan through the names
   for (int i = 0; i < cbp->GetCount(); i++) {
      // read the description
      ctrlNameList->ReadName( cbp->GetItemData( i ), &nameDesc );

      // if it matches the name, set the selection
      if (strcmp(nameDesc.nameName,name) == 0) {
         cbp->SetCurSel( i );
         Selchange();
         break;
      }
   }
}

//
// VTSIPAddrCtrl::Enable
//

void VTSIPAddrCtrl::Enable( void )
{
   VTSCtrl::Enable();
   ctrlWindow->GetDlgItem( ctrlComboID )->EnableWindow( true );
}

//
// VTSIPAddrCtrl::Disable
//

void VTSIPAddrCtrl::Disable( void )
{
   VTSCtrl::Disable();
   ctrlWindow->GetDlgItem( ctrlComboID )->EnableWindow( false );
}

//
// VTSIPAddrCtrl::CtrlToObj
//

void VTSIPAddrCtrl::CtrlToObj( void )
{
   CString        str
   ;
   unsigned long  host
   ;
   unsigned short port
   ;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );

   ctrlNull = str.IsEmpty();
   if (ctrlNull)
      return;

   // interpret the string
   StringToHostPort( (char *)str.operator LPCTSTR(), &host, 0, &port );
   Pack( host, port );

   // get the combo box
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   VTSNameDesc    name
   ;

   // scan through the names
   bool found = false;
   for (int i = 0; i < cbp->GetCount(); i++) {
      // read the description
      ctrlNameList->ReadName( cbp->GetItemData( i ), &name );

      // if it matches our port, save the name and index
      if ((name.nameAddr.addrLen == addrLen) && (memcmp(name.nameAddr.addrAddr,addrAddr,addrLen) == 0)) {
         cbp->SetCurSel( i );
         found = true;
         break;
      }
   }

   // clear if no matching name found
   if (!found)
      cbp->SetWindowText( _T("") );
}

//
// VTSIPAddrCtrl::SaveCtrl
//

void VTSIPAddrCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSIPAddrCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else {
      BACnetInteger( addrType ).Encode( enc );
      BACnetInteger( addrNet ).Encode( enc );
      BACnetOctetString( addrAddr, addrLen ).Encode( enc );
   }
}

//
// VTSIPAddrCtrl::RestoreCtrl
//

void VTSIPAddrCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag
   ;

// TRACE0( "VTSIPAddrCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      BACnetInteger  type, net
      ;
      BACnetOctetString addr( 6 )
      ;

      ctrlNull = false;

      type.Decode( dec );
      addrType = (BACnetAddressType)type.intValue;
      net.Decode( dec );
      addrNet = net.intValue;
      addr.Decode( dec );
      addrLen = addr.strLen;
      memcpy( addrAddr, addr.strBuff, addrLen );
   }
}
*/


/////////////////////////////////////////////////////////////////////////////

//
// VTSRemoteAddrCtrl
//

VTSRemoteAddrCtrl::VTSRemoteAddrCtrl( const CWnd* wp, VTSIntegerCtrl *icp, int cid, int tid )
// : VTSCtrl( wp, tid ), ctrlNet(icp), ctrlComboID(cid), ctrlNameList(0)
   : VTSAddrComboCtrl( wp, cid, tid ), ctrlNet(icp)
{
}


bool VTSRemoteAddrCtrl::IsNameMatch( VTSName * pname, VTSPort * /*pport*/, bool okBroadcast ) const
{
   return (   (pname->m_bacnetaddr.addrType == remoteStationAddr)
          || (okBroadcast && (pname->m_bacnetaddr.addrType == remoteBroadcastAddr))
         || (okBroadcast && (pname->m_bacnetaddr.addrType == globalBroadcastAddr)) );
}


void VTSRemoteAddrCtrl::AssignAddress(VTSName * pname)
{
   if (pname->m_bacnetaddr.addrType == remoteBroadcastAddr)
   {
      ctrlNet->ctrlNull = false;
      ctrlNet->intValue = pname->m_bacnetaddr.addrNet;
      ctrlNet->ObjToCtrl();
      addrLen = 0;
   }
   else
   {
      if (pname->m_bacnetaddr.addrType == globalBroadcastAddr)
      {
         ctrlNet->ctrlNull = false;
         ctrlNet->intValue = 65535;
         ctrlNet->ObjToCtrl();
         addrLen = 0;
      }
      else
      {
         ctrlNet->ctrlNull = false;
         ctrlNet->intValue = pname->m_bacnetaddr.addrNet;
         ctrlNet->ObjToCtrl();

         addrLen = pname->m_bacnetaddr.addrLen;
         memcpy( addrAddr, pname->m_bacnetaddr.addrAddr, addrLen );
      }
   }
}


bool VTSRemoteAddrCtrl::IsAddressMatch( BACnetAddress * pbacnetaddr )
{
   if (   pbacnetaddr->addrType == remoteStationAddr
      && pbacnetaddr->addrNet == ctrlNet->intValue
      && pbacnetaddr->addrLen == addrLen
      && memcmp(pbacnetaddr->addrAddr,addrAddr,addrLen) == 0 )
      return true;

   if (   pbacnetaddr->addrType == remoteBroadcastAddr
      && pbacnetaddr->addrNet == ctrlNet->intValue
      && addrLen == 0 )
      return true;

   if (   pbacnetaddr->addrType == globalBroadcastAddr
      && ctrlNet->intValue == 65535
      && addrLen == 0 )
      return true;

   return false;
}


//
// VTSRemoteAddrCtrl::RestoreCtrl
//

void VTSRemoteAddrCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSRemoteAddrCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      BACnetInteger  type, net
      ;
      BACnetOctetString addr( 8 )         // only difference in base class is 8 instead of 6...
      ;

      ctrlNull = false;

      type.Decode( dec );
      addrType = (BACnetAddressType)type.intValue;
      net.Decode( dec );
      addrNet = net.intValue;
      addr.Decode( dec );
      addrLen = addr.strLen;
      memcpy( addrAddr, addr.strBuff, addrLen );
   }
}

//
// VTSRemoteAddrCtrl::LoadCombo
//

/* MAD_DB now in base class
void VTSRemoteAddrCtrl::LoadCombo( VTSNameList *nameList, unsigned int portID, bool okBroadcast )
{
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   int            indx
   ;
   VTSNameDesc    name
   ;

   // already initialized?
   if (ctrlNameList)
      return;

   // save a pointer to the name list
   ctrlNameList = nameList;

   // scan through the names
   for (int i = 0; i < nameList->Length(); i++) {
      // read the description
      ctrlNameList->ReadName( i, &name );

      // if it matches our port, save the name and index
      if ((name.namePort == portID) || (name.namePort == 0))
         if ((name.nameAddr.addrType == remoteStationAddr)
            || (okBroadcast && (name.nameAddr.addrType == remoteBroadcastAddr))
            || (okBroadcast && (name.nameAddr.addrType == globalBroadcastAddr)) ) {
               indx = cbp->AddString( name.nameName );
               if ((indx == CB_ERR) || (indx == CB_ERRSPACE))
                  break;
               cbp->SetItemData( indx, i );
            }
   }
}


//
// VTSRemoteAddrCtrl::Selchange
//

void VTSRemoteAddrCtrl::Selchange( void )
{
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   VTSNameDesc    name
   ;
   int            i = cbp->GetItemData( cbp->GetCurSel() )
   ;

   ctrlNameList->ReadName( i, &name );

   if (name.nameAddr.addrType == remoteBroadcastAddr) {
      ctrlNet->ctrlNull = false;
      ctrlNet->intValue = name.nameAddr.addrNet;
      ctrlNet->ObjToCtrl();
      addrLen = 0;
   } else
   if (name.nameAddr.addrType == globalBroadcastAddr) {
      ctrlNet->ctrlNull = false;
      ctrlNet->intValue = 65535;
      ctrlNet->ObjToCtrl();
      addrLen = 0;
   } else {
      ctrlNet->ctrlNull = false;
      ctrlNet->intValue = name.nameAddr.addrNet;
      ctrlNet->ObjToCtrl();

      addrLen = name.nameAddr.addrLen;
      memcpy( addrAddr, name.nameAddr.addrAddr, addrLen );
   }

   ctrlNull = false;

   ObjToCtrl();
}

//
// VTSRemoteAddrCtrl::FindName
//

void VTSRemoteAddrCtrl::FindName( const char *name )
{
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   VTSNameDesc    nameDesc
   ;

   // scan through the names
   for (int i = 0; i < cbp->GetCount(); i++) {
      // read the description
      ctrlNameList->ReadName( cbp->GetItemData( i ), &nameDesc );

      // if it matches the name, set the selection
      if (strcmp(nameDesc.nameName,name) == 0) {
         cbp->SetCurSel( i );
         Selchange();
         break;
      }
   }
}

//
// VTSRemoteAddrCtrl::Enable
//

void VTSRemoteAddrCtrl::Enable( void )
{
   VTSCtrl::Enable();
   ctrlWindow->GetDlgItem( ctrlComboID )->EnableWindow( true );
}

//
// VTSRemoteAddrCtrl::Disable
//

void VTSRemoteAddrCtrl::Disable( void )
{
   VTSCtrl::Disable();
   ctrlWindow->GetDlgItem( ctrlComboID )->EnableWindow( false );
}

//
// VTSRemoteAddrCtrl::CtrlToObj
//

void VTSRemoteAddrCtrl::CtrlToObj( void )
{
   int         upperNibble, lowerNibble
   ;
   char     c
   ;
   CString     str
   ;
   LPCTSTR     s
   ;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // remove contents
   addrLen = 0;

   // translate the text into octets
   for (;;) {
      // look for a hex digit
      while ((c = ToUpper(*s++)) && !IsXDigit(c))
         ;
      if (!c) break;
      upperNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

      // look for another hex digit
      while ((c = ToUpper(*s++)) && !IsXDigit(c))
         ;
      if (!c) break;
      lowerNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

      // add the byte
      if (addrLen < sizeof(addrAddr))
         addrAddr[ addrLen++ ] = (upperNibble << 4) + lowerNibble;
   }

   // if nothing specified, set value to null
   ctrlNull = (addrLen == 0);
   if (ctrlNull)
      return;

   // get the combo box
   CComboBox      *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlComboID )
   ;
   VTSNameDesc    name
   ;

   // scan through the names
   bool found = false;
   for (int i = 0; i < cbp->GetCount(); i++) {
      // read the description
      ctrlNameList->ReadName( cbp->GetItemData( i ), &name );

      // if it matches our stuff, set it to the current selection
      if ((name.nameAddr.addrType == remoteStationAddr)
            && (name.nameAddr.addrNet == ctrlNet->intValue)
            && (name.nameAddr.addrLen == addrLen)
            && (memcmp(name.nameAddr.addrAddr,addrAddr,addrLen) == 0) )
         found = true;
      else
      if ((name.nameAddr.addrType == remoteBroadcastAddr)
            && (name.nameAddr.addrNet == ctrlNet->intValue)
            && (addrLen == 0) )
         found = true;
      else
      if ((name.nameAddr.addrType == globalBroadcastAddr)
            && (ctrlNet->intValue == 65535)
            && (addrLen == 0) )
         found = true;
      else
         ;
      if (found) {
         cbp->SetCurSel( i );
         break;
      }
   }

   // clear if no matching name found
   if (!found)
      cbp->SetWindowText( _T("") );
}
//
// VTSRemoteAddrCtrl::ObjToCtrl
//

void VTSRemoteAddrCtrl::ObjToCtrl( void )
{
   char  buff[kMaxAddressLen * 3], *s = buff
   ;

   // clear the buffer
   buff[0] = 0;

   // encode the address
   if (!ctrlNull)
      for (int i = 0; i < addrLen; i++) {
         if (s != buff)
            *s++ = '-';
         sprintf( s, "%02X", addrAddr[i] );
         s += 2;
      }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( buff );
}

//
// VTSRemoteAddrCtrl::SaveCtrl
//

void VTSRemoteAddrCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSRemoteAddrCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else {
      BACnetInteger( addrType ).Encode( enc );
      BACnetInteger( addrNet ).Encode( enc );
      BACnetOctetString( addrAddr, addrLen ).Encode( enc );
   }
}
*/


/////////////////////////////////////////////////////////////////////////////

//
// VTSBooleanCtrl
//

VTSBooleanCtrl::VTSBooleanCtrl( const CWnd* wp, int id, bool isCheckBox )
   : VTSCtrl( wp, id )
   , m_bCheckBox(isCheckBox)
{
}

//
// VTSBooleanCtrl::CtrlToObj
//

void VTSBooleanCtrl::CtrlToObj( void )
{
   CString     str;
   LPCTSTR     s;

   if (m_bCheckBox) {
      ctrlNull = false;
      boolValue = (eBACnetBoolean)(((CButton *)ctrlWindow->GetDlgItem( ctrlID ))->GetState() & 0x0003);
      return;
   }

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // if no data available, set ctrlNull and return
   ctrlNull = (!*s);
   if (ctrlNull)
      return;

   try {
      Decode( s );
   }
   catch (...) {
   }
}

//
// VTSBooleanCtrl::ObjToCtrl
//

void VTSBooleanCtrl::ObjToCtrl( void )
{
   CString  str;

   if (m_bCheckBox)
      SetCheck(boolValue ? 1 : 0);     // madanner 9/4/02, new check method
   else {
      // values are normalized
      if (!ctrlNull)
      {
         Encode( str, FMT_PLAIN );
      }

      // set the text
      ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
   }
}


// madanner 9/4/02, added check method for encapsulation

void VTSBooleanCtrl::SetCheck( bool fChecked )
{
   ((CButton *)ctrlWindow->GetDlgItem( ctrlID ))->SetCheck( fChecked ? 1 : 0 );
}


//
// VTSBooleanCtrl::SaveCtrl
//

void VTSBooleanCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSBooleanCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSBooleanCtrl::RestoreCtrl
//

void VTSBooleanCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSBooleanCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

// Separator between properties of an object and all properties
static const char s_dashes[] = "------------------------------";

//
// VTSEnumeratedCtrl
//

VTSEnumeratedCtrl::VTSEnumeratedCtrl( const CWnd* wp, int id, NetworkSniffer::BACnetStringTable &table,
                                      bool isCombo, bool sortStrings )
: VTSCtrl( wp, id )
, m_Table(table.m_pStrings)
, m_TableSize(table.m_nStrings)
, m_bCombo(isCombo)
, m_sortStrings(sortStrings)
, m_nObjType(-1)
, m_prevObjType(-1)
, m_SelectValue(-1)
, m_VendorPropID(-1)
{
}

// Old-style constructor, deprecated in favor of the constructor taking NetworkSniffer::BACnetStringTable
VTSEnumeratedCtrl::VTSEnumeratedCtrl( const CWnd* wp, int id, char **table, int tableSize,
                                      bool isCombo, bool sortStrings )
: VTSCtrl( wp, id )
, m_Table(table)
, m_TableSize(tableSize)
, m_bCombo(isCombo)
, m_sortStrings(sortStrings)
, m_nObjType(-1)
, m_prevObjType(-1)
, m_SelectValue(-1)
, m_VendorPropID(-1)
{
}

// Check whether propertyID is one of the Object's standard properties 
extern bool Check_Obj_Prop(int ObjType, unsigned int propertyID);

// Helpers to make a sorted list of strings
struct OurSorter
{
   UINT         m_ID;
   const char  *m_pString;
};

int SortOurStuff( const void *pFirst, const void *pSecond )
{
   const OurSorter *pPS1 = (const OurSorter*)pFirst;
   const OurSorter *pPS2 = (const OurSorter*)pSecond;
   return strcmp( pPS1->m_pString, pPS2->m_pString );
}

//
// VTSEnumeratedCtrl::LoadCombo
//
void VTSEnumeratedCtrl::LoadCombo( void )
{
   if (!m_bCombo)
      return;

   CComboBox *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlID );
   if ((m_nObjType != m_prevObjType) && (cbp->GetCount() != 0))
   {
      // We have changed object type, so delete the old content
      cbp->Clear();
      cbp->ResetContent();
   }

   if (cbp->GetCount() == 0)
   {
      // Fill the combo

      // Copy the strings into a buffer in case we need to sort them
      int i;
      OurSorter *pStuff = new OurSorter[m_TableSize];
      for (i = 0; i < m_TableSize; i++)
      {
         pStuff[i].m_ID = i;
         pStuff[i].m_pString = m_Table[i];
      }

      if (m_sortStrings)
      {
         qsort( pStuff, m_TableSize, sizeof(OurSorter), &SortOurStuff );
      }

      if (m_nObjType == -1)
      {
         // General enumeration
         for (i = 0; i < m_TableSize; i++)
         {
            cbp->AddString( pStuff[i].m_pString );
         }
      }
      else
      {
         // PropertyID selection combo.
         m_prevObjType = m_nObjType;
         if (m_nObjType < MAX_DEFINED_OBJ)
         {
            // Standard object type: add the properties supported by this object type
            for (i = 0; i < m_TableSize; i++)
            {
               if (Check_Obj_Prop(m_nObjType, pStuff[i].m_ID))
               {
                  cbp->AddString( pStuff[i].m_pString );
               }
            }

            // Add a dashed line and all the other defined BACnet properties
            cbp->AddString(s_dashes);
         }

         // Add all properties not already in the list
         for (i = 0; i < m_TableSize; i++)
         {
            if (cbp->FindString(-1, pStuff[i].m_pString) < 0)
            {
               cbp->AddString( pStuff[i].m_pString );
            }
         }

         // TODO: should we make this available to ALL enum combos that want it?
         // If we always had a BACnetStringTable, we would even know the
         // limits on reserved versus proprietary.
         //
         // This would change and simplify the object-type combo, eliminating a couple fields
         // We MIGHT make the drop-lists into REAL combos, and allow direct editing.
         //
         // Add item to allow arbitrary value.
         cbp->AddString("< Enter numeric value >");
      }

      delete[] pStuff;

      // Make sure at least eight lines are visible
      SetDropDownSize( 16 );
   }

   // Select current value
   if (!ctrlNull)
   {
      EnumToSelect();
      cbp->SetCurSel( m_SelectValue );
   }
}

//
// VTSEnumeratedCtrl::SetDropDownSize
//
// This code was found on the web from:
//
//    http://mulvey.dyndns.com/Katy/VisualC/code/cbresize.html
//
// Thank you Katy!
//
void VTSEnumeratedCtrl::SetDropDownSize( UINT lines )
{
   CComboBox *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlID );
   ::SetDropDownSize( *cbp, lines );
}

void SetDropDownSize( CComboBox& box, UINT lines )
{
   ASSERT( IsWindow(box) );

   CRect    cbSize;
   int      height;

   box.GetClientRect( cbSize );

   // start with the size of the edit box
   height = box.GetItemHeight(-1);

   // add enough for the lines
   height += box.GetItemHeight(0) * lines;

   // now add on the height of the border of the edit box
   height += GetSystemMetrics(SM_CYEDGE) * 2; // top & bottom edges

   // now add the height of the border of the dropdown box
   height += GetSystemMetrics(SM_CYEDGE) * 2; // top & bottom edges

   // set the size of the window
   box.SetWindowPos( NULL,                   // not relative to any other windows
                     0, 0,                   // TopLeft corner doesn't change
                     cbSize.right, height,   // existing width, new height
                     SWP_NOMOVE | SWP_NOZORDER
                   );
}

//
// VTSEnumeratedCtrl::CtrlToObj
//
void VTSEnumeratedCtrl::CtrlToObj( void )
{
   if (m_bCombo)
   {
      CComboBox   *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlID );
      ctrlNull = false;

      m_SelectValue = cbp->GetCurSel();
      if (m_SelectValue < 0)
      {
         ctrlNull = true;
         return;
      }

      // PropertyID selection code
      CString str;
      cbp->GetLBText(m_SelectValue,str);
      if (!str.Compare(s_dashes))
      {
         // They selected the dashes.  Don't change anything
         return;
      }

      // Convert the combo selection into an enumeration value
      SelectToEnum();
      return;
   }

   // TODO: ARE there any like this: Edit control instead of combo?
   // CSendWritePropMultError m_PropertyID for one
   // (and it SHOULD BE a combo)
   CEdit    *ctrl = (CEdit *)ctrlWindow->GetDlgItem( ctrlID );
   CString  str;
   LPCTSTR  s;

   // get the text from the control
   ctrl->GetWindowText( str );
   s = str.operator LPCTSTR();

   // if no data available, set ctrlNull and return
   ctrlNull = (!*s);
   if (ctrlNull)
      return;

   try {
      // use the built-in decoder
      Decode( s, (const char **)m_Table, m_TableSize );
   }
   catch (...) {
   }

#if 0
   // assume it hasn't been found
   m_enumValue = -1;

   // try and find it
   if (!m_Table)
      m_enumValue = atoi( s );
   else
      for (int i = 0; i < m_TableSize; i++)
         if (strncmp(s,m_Table[i],strlen(s)) == 0) {
            enumValue = i;
            break;
         }
#endif
#if 0
   // This interesting code changes the control text to the complete
   // enumerated value and sets the selection to what wasn't entered
   // by the user.  Would be better if the rest of the text was gray
   // but that would take time I don't have right now.
   // NOTE: the control is disabled to prevent message loops.
   if (m_enumValue >= 0) {
      int len = strlen(s);
      str = m_Table[m_enumValue];

      // set the text
      ctrlEnabled = false;
      ctrl->SetWindowText( str );
      ctrl->SetSel( len, str.GetLength() );
      ctrlEnabled = true;
   }
#endif
}

//
// VTSEnumeratedCtrl::ObjToCtrl
//

void VTSEnumeratedCtrl::ObjToCtrl( void )
{
   if (m_bCombo)
   {
      CComboBox *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlID );

      // Convert the enumeration value to a selection index and select it.
      EnumToSelect();
      cbp->SetCurSel( m_SelectValue );

      if (m_SelectValue < 0)
         ctrlNull = true;

      return;
   }

   // TODO: are there any of these?
   CString  str;

   // values are normalized to decimal
   if (!ctrlNull)
   {
      Encode( str, (const char **)m_Table, m_TableSize );
   }

#if 0
   // values are from the table
   if (!ctrlNull)
      if (!m_Table)
         str.Format( "%d", m_enumValue );
      else
      if ((m_enumValue < 0) || (m_enumValue >= m_TableSize))
         str = "(out of bounds)";
      else
         str = m_Table[m_enumValue];
#endif

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSEnumeratedCtrl::SaveCtrl
//

void VTSEnumeratedCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSEnumeratedCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSEnumeratedCtrl::RestoreCtrl
//

void VTSEnumeratedCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSEnumeratedCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

// Select an enumerated value, populating and showing the control if necessary
void VTSEnumeratedCtrl::SetEnumValue( int theValue )
{
   m_enumValue = theValue;    // set the value
   ctrlNull = false;          // enable showing the control
   ObjToCtrl();               // make it so!
}

// Set m_enumValue (enum value) from m_SelectValue (combo index)
// CAUTION: if the control is sorted, these are NOT the same number!
//
// If the selection is a "proprietary" item at the end of the list,
// show a numeric entry Dialog, and the selection may change based on 
// the value entered.
//
void VTSEnumeratedCtrl::SelectToEnum()
{
   CComboBox   *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlID );
   int nStrings = cbp->GetCount();
   if ((m_nObjType != -1) && (nStrings > 0) && (m_SelectValue == (nStrings - 1)))
   {
      // Selected last item in the list.  Show dialog to enter the property numerically.
      VTSVendorPropIDDlg dlg;
      if (m_VendorPropID != -1)
         dlg.m_PropID = m_VendorPropID;

      int res = dlg.DoModal();
      if (res == IDOK)
      {
         m_VendorPropID = dlg.m_PropID;
         m_enumValue = m_VendorPropID;
      }

      if (m_enumValue >= m_TableSize)
      {
         // Show reserved or proprietary value numerically
         CString text;
         cbp->DeleteString(cbp->GetCount() - 1);
         text.Format("%d", m_enumValue);
         m_SelectValue = cbp->AddString(text);
      }
      else
      {
         // Show standard value entered as an integer as the corresponding string
         EnumToSelect();
      }
      cbp->SetCurSel( m_SelectValue );
      return;
   }

   if (cbp->GetCount() != 0)
   {
      // Since the control may be sorted, the selection index may not be
      // equal to the enumeration value.  Conver the TEXT to the enumeration
      CString str;
      // According to Microsoft you can't use getWindowText when using CBN_SELCHANGE
      cbp->GetLBText(m_SelectValue, str);
      for (int i = 0; i < m_TableSize; i++)
      {
         if (str.Compare(m_Table[i]) == 0)
         {
            m_enumValue = i;
            break;
         }
      }
   }
}

// Set m_SelectValue (combo index) from m_enumValue (enum value)
// CAUTION: if the control is sorted, these are NOT the same number!
void VTSEnumeratedCtrl::EnumToSelect()
{
   CComboBox *cbp = (CComboBox *)ctrlWindow->GetDlgItem( ctrlID );

   if (ctrlNull)
   {
      // Leave the control empty until someone drops it
      return;
   }

   if (cbp->GetCount() == 0)
   {
      // Fill the control or FindString below will fail
      LoadCombo();
   }

   if ((m_nObjType != -1) && (m_enumValue >= m_TableSize))
   {
      // Numeric propertyID: select the last item in the list
      m_SelectValue = cbp->GetCount() - 1;
      m_VendorPropID = m_enumValue;
   }
   else if ((m_enumValue >= 0) && (m_enumValue < m_TableSize))
   {
      // Get the string value and find it in the control
      m_SelectValue = cbp->FindString(-1, m_Table[m_enumValue]);
   }
   else
   {
      // There is no selection corresponding to this enum.
      // If we extend the vendor proprietary to ALL enumerations,
      // we could merge this case with the propertyID case above
      m_SelectValue = -1;
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSUnsignedCtrl
//

VTSUnsignedCtrl::VTSUnsignedCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
{
}

//
// VTSUnsignedCtrl::CtrlToObj
//

void VTSUnsignedCtrl::CtrlToObj( void )
{
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // if no data available, set ctrlNull and return
   ctrlNull = (!*s);
   if (ctrlNull)
      return;

   try {
      // use the built-in decoder
      Decode( s );
   }
   catch (...) {
   }
}

//
// VTSUnsignedCtrl::ObjToCtrl
//

void VTSUnsignedCtrl::ObjToCtrl( void )
{
   CString  str;

   // values are normalized to decimal
   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSUnsignedCtrl::SaveCtrl
//

void VTSUnsignedCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSUnsignedCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSUnsignedCtrl::RestoreCtrl
//

void VTSUnsignedCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSUnsignedCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSIntegerCtrl
//

VTSIntegerCtrl::VTSIntegerCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
{
}

//
// VTSIntegerCtrl::CtrlToObj
//

void VTSIntegerCtrl::CtrlToObj( void )
{
   bool     negative = false;
   CString  str;
   LPCTSTR  s;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // if no data available, set ctrlNull and return
   ctrlNull = (!*s);
   if (ctrlNull)
      return;

   try {
      // use the built-in decoder
      Decode( s );
   }
   catch (...) {
   }
}

//
// VTSIntegerCtrl::ObjToCtrl
//

void VTSIntegerCtrl::ObjToCtrl( void )
{
   CString  str;

   // values are normalized to decimal
   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSIntegerCtrl::SaveCtrl
//

void VTSIntegerCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSIntegerCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSIntegerCtrl::RestoreCtrl
//

void VTSIntegerCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSIntegerCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSRealCtrl
//

VTSRealCtrl::VTSRealCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
{
}

//
// VTSRealCtrl::CtrlToObj
//

void VTSRealCtrl::CtrlToObj( void )
{
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // if no data available, set ctrlNull and return
   ctrlNull = (!*s);
   if (ctrlNull)
      return;

   try {
      // use the built-in decoder
      Decode( s );
   }
   catch (...) {
   }
}

//
// VTSRealCtrl::ObjToCtrl
//

void VTSRealCtrl::ObjToCtrl( void )
{
   CString  str;

   // values are normalized to decimal
   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSRealCtrl::SaveCtrl
//

void VTSRealCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSRealCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSRealCtrl::RestoreCtrl
//

void VTSRealCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSRealCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSDoubleCtrl
//

VTSDoubleCtrl::VTSDoubleCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
{
}

//
// VTSDoubleCtrl::CtrlToObj
//

void VTSDoubleCtrl::CtrlToObj( void )
{
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // if no data available, set ctrlNull and return
   ctrlNull = (!*s);
   if (ctrlNull)
      return;

   try {
      // use the built-in decoder
      Decode( s );
   }
   catch (...) {
   }
}

//
// VTSDoubleCtrl::ObjToCtrl
//

void VTSDoubleCtrl::ObjToCtrl( void )
{
   CString  str;

   // values are normalized to decimal
   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSDoubleCtrl::SaveCtrl
//

void VTSDoubleCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSDoubleCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSDoubleCtrl::RestoreCtrl
//

void VTSDoubleCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSDoubleCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSCharacterStringCtrl
//

VTSCharacterStringCtrl::VTSCharacterStringCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
   , emptyIsNull( true )
{
}


VTSCharacterStringCtrl::~VTSCharacterStringCtrl()
{
   // must call this from here.. multiple inheritence messes the BACnetCharacterString
   // virtual destructor up big time...

   KillBuffer();
}


//
// VTSCharacterStringCtrl::CtrlToObj
//

void VTSCharacterStringCtrl::CtrlToObj( void )
{
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // no data might mean empty is null
   ctrlNull = (!*s && emptyIsNull);
   if (ctrlNull)
      return;

   // madanner 8/27/02, We're pulling data from an edit field so we don't need to scan for quotes, escape stuff, etc.
   // We'll just put this directly into the buffer.  If we decide to allow encoding of escapes here... then we'll
   // either need to add quotes just before we call Decode or alter Decode to not deal with quotes.

   KillBuffer();
   strLen = str.GetLength();
   if ( strLen )
      strBuff = new BACnetOctet[strLen];

   if ( strBuff != NULL )
      memcpy(strBuff, str, strLen);

// try {
//    Decode( s );
// }
// catch (...) {
// }
}

//
// VTSCharacterStringCtrl::ObjToCtrl
//

void VTSCharacterStringCtrl::ObjToCtrl( void )
{
   // madanner 8/27/02, No need to encode string to send to this control.  Encoding only has relevance
   // when reading from script files.  Here, ALL data in the edit box should be placed in the BACnet string.

   CString     str((LPCSTR)strBuff, strLen);

   // values are normalized
   // ### this could be trouble depending on the encoding and number of chars that need escaping
   if (ctrlNull)
//    Encode( str.GetBuffer( strLen + 16 ) );
      str.Empty();

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}


//
// VTSCharacterStringCtrl::SaveCtrl
//

void VTSCharacterStringCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSCharacterStringCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSCharacterStringCtrl::RestoreCtrl
//

void VTSCharacterStringCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSCharacterStringCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSOctetStringCtrl
//

VTSOctetStringCtrl::VTSOctetStringCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
   , emptyIsNull( true )
{
}


VTSOctetStringCtrl::~VTSOctetStringCtrl()
{
   // must call this from here because multiple inheritence messes with the virtual destructor mechanism
   // in an unhealthy way. :)

   Flush();
}

//
// VTSOctetStringCtrl::CtrlToObj
//

void VTSOctetStringCtrl::CtrlToObj( void )
{
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // no data might mean empty is null
   ctrlNull = (!*s && emptyIsNull);

   try {
      // use the built-in decoder
      Decode( s );
   }
   catch (...) {
   }
}

//
// VTSOctetStringCtrl::ObjToCtrl
//

void VTSOctetStringCtrl::ObjToCtrl( void )
{
   CString  str;

   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSOctetStringCtrl::SaveCtrl
//

void VTSOctetStringCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSOctetStringCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSOctetStringCtrl::RestoreCtrl
//

void VTSOctetStringCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSOctetStringCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSBitStringCtrl
//

VTSBitStringCtrl::VTSBitStringCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
   , emptyIsNull( true )
{
}


VTSBitStringCtrl::~VTSBitStringCtrl()
{
   // have to call this here because virtual destructors get in trouble with multiple inheritence
   KillBuffer();
}

//
// VTSBitStringCtrl::CtrlToObj
//

void VTSBitStringCtrl::CtrlToObj( void )
{
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // no data might mean empty is null
   ctrlNull = (!*s && emptyIsNull);
   if (ctrlNull)
      return;

   try {
      // use the built-in decoder
      Decode( s );
   }
   catch (...) {
   }
}

//
// VTSBitStringCtrl::ObjToCtrl
//

void VTSBitStringCtrl::ObjToCtrl( void )
{
   CString  str;

   // loop through the bits
   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSBitStringCtrl::SaveCtrl
//

void VTSBitStringCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSBitStringCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSBitStringCtrl::RestoreCtrl
//

void VTSBitStringCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSBitStringCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSDateCtrl
//

VTSDateCtrl::VTSDateCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
{
}

//
// VTSDateCtrl::CtrlToObj
//

void VTSDateCtrl::CtrlToObj( void )
{
   CString     str;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );

   // if no data available, set ctrlNull and return
   ctrlNull = str.IsEmpty();
   if (ctrlNull)
      return;

   // BACnetDate now makes [] and () optional
   try {
      // use the built-in decoder
      Decode( (LPCTSTR) str );
   }
   catch (...) {
   }
}

//
// VTSDateCtrl::ObjToCtrl
//

void VTSDateCtrl::ObjToCtrl( void )
{
   CString  str;

   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSDateCtrl::SaveCtrl
//

void VTSDateCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSDateCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSDateCtrl::RestoreCtrl
//

void VTSDateCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSDateCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSTimeCtrl
//

VTSTimeCtrl::VTSTimeCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
{
}

//
// VTSTimeCtrl::CtrlToObj
//

void VTSTimeCtrl::CtrlToObj( void )
{
   CString     str;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );

   // if no data available, set ctrlNull and return
   ctrlNull = str.IsEmpty();
   if (ctrlNull)
      return;

   // BACnetTime now makes [] and () optional
   try {
      // use the built-in decoder
      Decode( (LPCTSTR) str );
   }
   catch (...) {
   }
}

//
// VTSTimeCtrl::ObjToCtrl
//

void VTSTimeCtrl::ObjToCtrl( void )
{
   CString  str;

   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSTimeCtrl::SaveCtrl
//

void VTSTimeCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSTimeCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSTimeCtrl::RestoreCtrl
//

void VTSTimeCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSTimeCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSObjectIdentifierCtrl
//

VTSObjectIdentifierCtrl::VTSObjectIdentifierCtrl( const CWnd* wp, int id )
   : VTSCtrl( wp, id )
{
}

//
// VTSObjectIdentifierCtrl::CtrlToObj
//

void VTSObjectIdentifierCtrl::CtrlToObj( void )
{
   CString     str;
   LPCTSTR     s;

   // get the text from the control
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->GetWindowText( str );
   s = str.operator LPCTSTR();

   // if no data available, set ctrlNull and return
   ctrlNull = (!*s);
   if (ctrlNull)
      return;

   try {
      Decode( s );
   }
   catch (...) {
   }
}

//
// VTSObjectIdentifierCtrl::ObjToCtrl
//

void VTSObjectIdentifierCtrl::ObjToCtrl( void )
{
   CString  str;

   if (!ctrlNull)
   {
      Encode( str, FMT_PLAIN );
   }

   // set the text
   ((CEdit *)ctrlWindow->GetDlgItem( ctrlID ))->SetWindowText( str );
}

//
// VTSObjectIdentifierCtrl::SaveCtrl
//

void VTSObjectIdentifierCtrl::SaveCtrl( BACnetAPDUEncoder& enc )
{
// TRACE0( "VTSObjectIdentifierCtrl::SaveCtrl\n" );

   if (ctrlNull)
      BACnetNull().Encode( enc );
   else
      Encode( enc );
}

//
// VTSObjectIdentifierCtrl::RestoreCtrl
//

void VTSObjectIdentifierCtrl::RestoreCtrl( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

// TRACE0( "VTSObjectIdentifierCtrl::RestoreCtrl\n" );

   dec.ExamineTag( tag );
   if (tag.tagNumber == nullAppTag) {
      ctrlNull = true;
      BACnetNull().Decode( dec );
   } else {
      ctrlNull = false;
      Decode( dec );
   }
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSListCtrl::VTSListCtrl
//

VTSListCtrl::VTSListCtrl( void )
   : listSelectedRow(-1), listWindow(0), listCtrl(0), listItems(0)
{
}

//
// VTSListCtrl::~VTSListCtrl
//
// The list control doesn't maintain any data outside of the list contents,
// so there's nothing to do.  The class has virtual functions, so this is
// a virtual dtor.
//

VTSListCtrl::~VTSListCtrl( void )
{
}

//
// VTSListCtrl::Init
//
// When the CSendPage is initialized the OnInitDialog function is called, so all
// of the controls for the window have been created.  That function calls this
// one to initialize the list.
//

void VTSListCtrl::Init( CSendPage* wp, CListCtrl *cp, VTSListColDesc *dp  )
{
   // nothing is selected
   listSelectedRow = -1;
   listAddInProgress = false;

   // keep copies of the parameters
   listWindow = wp;
   listCtrl = cp;
   listItems = dp;

   // create the columns and disable the item controls
   for (int i = 0; listItems[i].colCtrlID; i++) {
      listCtrl->InsertColumn( i
         , listItems[i].colName, listItems[i].colFormat, listItems[i].colWidth
         );
      SetCtrlText( listItems[i].colCtrlID, "" );
      EnableCtrl( listItems[i].colCtrlID, false );
   }
}

//
// VTSListCtrl::GetItemCount
//
// In theory it would be simpler to get the item count directly from the list control,
// but it is better to be balanced.
//

int   VTSListCtrl::GetItemCount( void )
{
   return (listCtrl ? listCtrl->GetItemCount() : 0);
}

//
// VTSListCtrl::EnableCtrl
//
// The list will request that a control be enabled or disabled.  Normally this
// simply enables or disables the contorl, but in case there is some more
// processing needed, this is passed along to the derived class.
//

void VTSListCtrl::EnableCtrl( int ctrlID, int enable )
{
   TRACE2( "EnableCtrl( %d, %d )\n", ctrlID, enable );

   listWindow->GetDlgItem( ctrlID )->EnableWindow( enable );
}

//
// VTSListCtrl::GetItemText
//
// This function extracts the text from the list.  It is usually used in the
// encoding routines.  Because this uses a local static buffer, it is not
// thread safe.
//
// WARNING: Do not use CListCtrl::GetItemText(r,c), it doesn't work, don't ask
// me why.
//

const char *VTSListCtrl::GetItemText( int row, int col )
{
   static char    buff[512];
   LVITEM         itm;

   itm.mask = LVIF_TEXT;
   itm.iItem = row;
   itm.iSubItem = col;
   itm.pszText = buff;
   itm.cchTextMax = sizeof(buff);

   listCtrl->GetItem( &itm );

   return buff;
}

//
// VTSListCtrl::SetCtrlText
//

void VTSListCtrl::SetCtrlText( int ctrlID, const char *text )
{
   TRACE2( "SetCtrlText( %d, '%s' )\n", ctrlID, text );
   listWindow->GetDlgItem( ctrlID )->SetWindowText( text );
}

//
// VTSListCtrl::GetCtrlText
//

const char *VTSListCtrl::GetCtrlText( int ctrlID )
{
   static CString    rString;

   listWindow->GetDlgItem( ctrlID )->GetWindowText( rString );
   TRACE2( "GetCtrlText( %d ) = '%s'\n", ctrlID, rString );

   return rString.operator LPCTSTR();
}

//
// VTSListCtrl::AddButtonClick
//

void VTSListCtrl::AddButtonClick( void )
{
   int      listLen = listCtrl->GetItemCount();

   // deselect if something was selected
   POSITION selPos = listCtrl->GetFirstSelectedItemPosition();
   if (selPos != NULL) {
      int nItem = listCtrl->GetNextSelectedItem( selPos );
      listCtrl->SetItemState( nItem, 0, LVIS_SELECTED );
   }

   // create a new item
   listAddInProgress = true;
   listCtrl->InsertItem( listLen, "?2" );

   // enable the controls and empty them
   for (int i = 0; listItems[i].colCtrlID; i++) {
      // enable the control
      EnableCtrl( listItems[i].colCtrlID, true );

      // initialize it (which may not end up being an empty string)
      SetCtrlText( listItems[i].colCtrlID, "" );

      // let the virtual function return the string (it might be translated)
      listCtrl->SetItemText( listLen, i, GetCtrlText( listItems[i].colCtrlID ) );
   }

   // make sure it is visible and selected
   listCtrl->EnsureVisible( listLen, false );
   listCtrl->SetItemState( listLen
      , LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED
      );

   // give the control the focus
   listCtrl->SetFocus();

   // store the item number
   listSelectedRow = listLen;
   listAddInProgress = false;

   // build the encoding
   listWindow->UpdateEncoded();
}

//
// VTSListCtrl::RemoveButtonClick
//

void VTSListCtrl::RemoveButtonClick( void )
{
   int      listLen = listCtrl->GetItemCount();
   int      curRow = listSelectedRow;

   // must have a selected row
   if (curRow < 0)
      return;

   // deselect the row
   listCtrl->SetItemState( curRow, 0, LVIS_SELECTED | LVIS_FOCUSED );

   // delete the row
   listLen -= 1;
   listCtrl->DeleteItem( curRow );

   // see if the next item can be selected
   if (curRow >= listLen)
      curRow -= 1;

   if (curRow >= 0) {
      // make sure it is visible and selected
      listCtrl->EnsureVisible( curRow, false );
      listCtrl->SetItemState( curRow
         , LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED
         );

      // give it the focus
      listCtrl->SetFocus();
   }

   // build the encoding
   listWindow->UpdateEncoded();}

//
// VTSListCtrl::OnItemChanging
//

void VTSListCtrl::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
   NM_LISTVIEW*   pNMListView = (NM_LISTVIEW*)pNMHDR;

   // forget messages that don't change the state
   if (pNMListView->uOldState == pNMListView->uNewState)
      return;

   // skip messages during new item initialization
   if (listAddInProgress)
      return;

   if ((pNMListView->uNewState & LVIS_SELECTED) != 0) {
      // item becoming selected
      listSelectedRow = pNMListView->iItem;

      // enable the controls and copy the contents
      for (int i = 0; listItems[i].colCtrlID; i++) {
         EnableCtrl( listItems[i].colCtrlID, true );
         SetCtrlText( listItems[i].colCtrlID, GetItemText( listSelectedRow, i ) );
      }
   } else {
      // item no longer selected
      if (pNMListView->iItem == listSelectedRow) {
         listSelectedRow = -1;

         for (int j = 0; listItems[j].colCtrlID; j++) {
            SetCtrlText( listItems[j].colCtrlID, "" );
            EnableCtrl( listItems[j].colCtrlID, false );
         }
      }
   }
}

//
// VTSListCtrl::OnChangeItem
//

void VTSListCtrl::OnChangeItem( int ctrlID )
{
   int         col;
   const char  *txt;

   // must have a selected row
   if (listSelectedRow < 0)
      return;

   // find the column number
   for (col = 0; listItems[col].colCtrlID != ctrlID; col++)
      ;

   // control must be owned by the list
   ASSERT( listItems[col].colCtrlID != 0 );

   // get the new control text
   txt = GetCtrlText( ctrlID );

   // transfer it to the column of the current row
   listCtrl->SetItemText( listSelectedRow, col, txt );

   // build the encoding
   listWindow->UpdateEncoded();
}

/////////////////////////////////////////////////////////////////////////////

//
// VTSStatusFlags::VTSStatusFlags
//

VTSStatusFlags::VTSStatusFlags( const CWnd* wp )
   : BACnetBitString( 4 )
   , ctrlEnabled(true)
   , ctrlWindow(wp)
{
}

//
// VTSStatusFlags::Enable
//
// Enabling the flags means enabling all of the buttons and making their status reflect
// the value in the bit string.
//

void VTSStatusFlags::Enable( void )
{
   CButton  *inAlarmButton
   ,     *faultButton
   ,     *overriddenButton
   ,     *outOfServiceButton
   ;

   inAlarmButton = (CButton *)ctrlWindow->GetDlgItem( IDC_INALARM );
   faultButton = (CButton *)ctrlWindow->GetDlgItem( IDC_FAULT );
   overriddenButton = (CButton *)ctrlWindow->GetDlgItem( IDC_OVERRIDDEN );
   outOfServiceButton = (CButton *)ctrlWindow->GetDlgItem( IDC_OUTOFSERVICE );

   ctrlEnabled = true;
   inAlarmButton->EnableWindow( true );
   inAlarmButton->SetCheck( GetBit(0) );
   faultButton->EnableWindow( true );
   faultButton->SetCheck( GetBit(1) );
   overriddenButton->EnableWindow( true );
   overriddenButton->SetCheck( GetBit(2) );
   outOfServiceButton->EnableWindow( true );
   outOfServiceButton->SetCheck( GetBit(3) );
}

//
// VTSStatusFlags::Disable
//
// Disabling the status flags means disabling all of the buttons.  I also clear them out
// (make sure they are unchecked) to make the display look nice.
//

void VTSStatusFlags::Disable( void )
{
   CButton  *inAlarmButton
   ,     *faultButton
   ,     *overriddenButton
   ,     *outOfServiceButton
   ;

   inAlarmButton = (CButton *)ctrlWindow->GetDlgItem( IDC_INALARM );
   faultButton = (CButton *)ctrlWindow->GetDlgItem( IDC_FAULT );
   overriddenButton = (CButton *)ctrlWindow->GetDlgItem( IDC_OVERRIDDEN );
   outOfServiceButton = (CButton *)ctrlWindow->GetDlgItem( IDC_OUTOFSERVICE );

   ctrlEnabled = false;
   inAlarmButton->SetCheck( 0 );
   inAlarmButton->EnableWindow( false );
   faultButton->SetCheck( 0 );
   faultButton->EnableWindow( false );
   overriddenButton->SetCheck( 0 );
   overriddenButton->EnableWindow( false );
   outOfServiceButton->SetCheck( 0 );
   outOfServiceButton->EnableWindow( false );
}

//
// VTSStatusFlags::InAlarmClick
//

void VTSStatusFlags::InAlarmClick( void )
{
   CButton  *inAlarmButton = (CButton *)ctrlWindow->GetDlgItem( IDC_INALARM );

   SetBit( 0, (inAlarmButton->GetState() & 0x0001) );
}

//
// VTSStatusFlags::FaultClick
//

void VTSStatusFlags::FaultClick( void )
{
   CButton  *faultButton = (CButton *)ctrlWindow->GetDlgItem( IDC_FAULT );

   SetBit( 1, (faultButton->GetState() & 0x0001) );
}

//
// VTSStatusFlags::OverriddenClick
//

void VTSStatusFlags::OverriddenClick( void )
{
   CButton  *overriddenButton = (CButton *)ctrlWindow->GetDlgItem( IDC_OVERRIDDEN );

   SetBit( 2, (overriddenButton->GetState() & 0x0001) );
}

//
// VTSStatusFlags::OutOfServiceClick
//

void VTSStatusFlags::OutOfServiceClick( void )
{
   CButton  *outOfServiceButton = (CButton *)ctrlWindow->GetDlgItem( IDC_OUTOFSERVICE );

   SetBit( 3, (outOfServiceButton->GetState() & 0x0001) );
}

//
// VTSStatusFlags::CtrlToObj
//

void VTSStatusFlags::CtrlToObj( void )
{
   InAlarmClick();
   FaultClick();
   OverriddenClick();
   OutOfServiceClick();
}

//
// VTSStatusFlags::ObjToCtrl
//

void VTSStatusFlags::ObjToCtrl( void )
{
   CButton  *inAlarmButton
   ,     *faultButton
   ,     *overriddenButton
   ,     *outOfServiceButton
   ;

   inAlarmButton = (CButton *)ctrlWindow->GetDlgItem( IDC_INALARM );
   faultButton = (CButton *)ctrlWindow->GetDlgItem( IDC_FAULT );
   overriddenButton = (CButton *)ctrlWindow->GetDlgItem( IDC_OVERRIDDEN );
   outOfServiceButton = (CButton *)ctrlWindow->GetDlgItem( IDC_OUTOFSERVICE );

   inAlarmButton->SetCheck( GetBit(0) );
   faultButton->SetCheck( GetBit(1) );
   overriddenButton->SetCheck( GetBit(2) );
   outOfServiceButton->SetCheck( GetBit(3) );
}

//
// VTSStatusFlags::UpdateData
//

void VTSStatusFlags::UpdateData( BOOL bCtrlToObj )
{
   if (bCtrlToObj)
      CtrlToObj();
   else
      ObjToCtrl();
}

