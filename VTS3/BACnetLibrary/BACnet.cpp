
#include "stdafx.h"
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "VTS.h"
#include "VTSDoc.h"
#include "StringTables.h"


#ifdef _MSC_VER
#define ENDIAN_SWAP     1
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#if (__DECCXX && __ALPHA)
#define ENDIAN_SWAP     1
#endif

#ifndef ENDIAN_SWAP
#define ENDIAN_SWAP     0
#endif

#if (__DECCXX)
#include "cvtdef.h"
#include <ssdef.h>

extern "C" {
int cvt$convert_float( const void *, int, void *, int, int );
}
#endif

// Replacements for character classification functions that can handle character values above 127
// (Microsoft's debug versions assert on >255 or less than 0, which is a problem for
// ISO 8859-1 since char is signed)
// Of course, NONE of these work correctly for UTF-8, which has replaced ASCII
// as BACnet character set 0...
int IsSpace( int theChar )
{
   return isspace( theChar & 0xFF );
}

int IsAlpha( int theChar )
{
   return (((theChar >= 'A') && (theChar <= 'Z')) ||
          ((theChar >= 'a') && (theChar <= 'z')));
}

int IsDigit( int theChar )
{
   return ((theChar >= '0') && (theChar <= '9'));
}

int IsXDigit( int theChar )
{
   return (((theChar >= 'A') && (theChar <= 'F')) ||
          ((theChar >= 'a') && (theChar <= 'f')) ||
          ((theChar >= '0') && (theChar <= '9')));
}

int IsAlnum( int theChar )
{
   return (((theChar >= 'A') && (theChar <= 'Z')) ||
          ((theChar >= 'a') && (theChar <= 'z')) ||
          ((theChar >= '0') && (theChar <= '9')));
}

int IsUpper( int theChar )
{
   return ((theChar >= 'A') && (theChar <= 'Z'));
}

int IsLower( int theChar )
{
   return ((theChar >= 'a') && (theChar <= 'z'));
}

int ToUpper( int theChar )
{
   if (IsLower(theChar))
   {
      theChar -= ('a' - 'A');
   }

   return theChar;
}

int ToLower( int theChar )
{
   if (IsUpper(theChar))
   {
      theChar += ('a' - 'A');
   }

   return theChar;
}

//madanner 9/04
namespace PICS {
#include "vtsapi.h"
#include "db.h"
#include "dudapi.h"
}
#include "propid.h"

#include "Props.h"
#include "BACnet.hpp"


// Remove leading whitespace
void EatWhite( const char* &pString )
{
   while (IsSpace(*pString))
   {
      pString += 1;
   }
}

// Return true if the next non-white character matches theChar.
// Bypasses any leading whitespace.
// if theChar is found, bypassing whitespace after it.
bool IsChar( const char* &pString, char theChar )
{
   EatWhite(pString);
   bool retval = (*pString == theChar);
   if (retval)
   {
      pString += 1;
      EatWhite(pString);
   }
   return retval;
}


// Assert if the next non-white character does not match theChar.
// Bypasses any leading whitespace.
// if theChar is found, bypassing whitespace after it.
void RequireChar( const char* &pString, char theChar )
{
   EatWhite(pString);
   if (*pString != theChar)
   {
      throw_(50);  // invalid character found
   }
   pString += 1;
   EatWhite(pString);
}


#define nPRIO 16

#ifndef _MSC_VER

int stricmp( const char *, const char * );

int stricmp( const char *a, const char *b )
{
   while (*a && *b) {
      int cmp = (ToLower(*b++) - ToLower(*a++));
      if (cmp != 0)
         return (cmp < 0 ? -1 : 1);
   }
   if (*a)
      return 1;
   else
   if (*b)
      return -1;
   else
      return 0;
}

#endif

#define  VTSScanner     1

#if VTSScanner

#include "VTS.h"
#include "ScriptBase.h"
#include "ScriptKeywords.h"

#endif

const char * apszBinaryPVNames[] = {"INACTIVE", "ACTIVE"};

bool ValuesEqual( int v1, int v2 );
bool ValuesGreater( int v1, int v2 );
bool ValuesLess( int v1, int v2 );

// Case insensitive compare to string.
// If match, return true and bypass the string
// Else return false and cursor unchanged
//
// if theAbbrevLen is > 0, also accept a match to that length.
// Thus, December or Dec, etc.
static bool MatchAndEat( const char* &theCursor, const char* theMatchString, int theAbbrevLen = -1 )
{
   int len = strlen( theMatchString );
   bool retval = _strnicmp( theCursor, theMatchString, len ) == 0;
   if (retval)
   {
      // Matches full string
      theCursor += len;
   }
   else if ((theAbbrevLen > 0) && (theAbbrevLen < len))
   {
      retval = _strnicmp( theCursor, theMatchString, theAbbrevLen ) == 0;
      if (retval)
      {
         // Matches abbreviated string
         theCursor += theAbbrevLen;
      }
   }

   return retval;
}

// Append bytes to X'1234' hex
void AppendXhex( CString &theResult, const unsigned char *pTheData, int theLength )
{
   theResult += "X\'";

   // encode the content
   for (int i = 0; i < theLength; i++) 
   {
      char buf[3];
      sprintf( buf, "%02X", pTheData[i] );
      theResult += buf;
   }

   theResult += '\'';
}

//
// BACnetAddress
//

BACnetAddress::BACnetAddress( const unsigned char *addr, const unsigned short len )
   : addrType((len != 0) ? localStationAddr : localBroadcastAddr), addrNet(0), addrLen(len)
{
   if (len && addr)
      memcpy( addrAddr, addr, addrLen );
}

BACnetAddress::BACnetAddress( const unsigned short net, const unsigned char *addr, const unsigned short len )
   : addrType((len != 0) ? remoteStationAddr : remoteBroadcastAddr), addrNet(net), addrLen(len)
{
   if (len && addr)
      memcpy( addrAddr, addr, addrLen );
}

BACnetAddress::BACnetAddress( const BACnetAddressType typ, const unsigned short net, const unsigned char *addr, const unsigned short len )
   : addrType(typ), addrNet(net), addrLen(len)
{
   if (len && addr)
      memcpy( addrAddr, addr, addrLen );
}

void BACnetAddress::LocalStation( const unsigned char *addr, const unsigned short len )
{
   addrType = (len != 0) ? localStationAddr : localBroadcastAddr;
   addrNet = 0;
   addrLen = len;
   memcpy( addrAddr, addr, addrLen );
}

void BACnetAddress::RemoteStation( const unsigned short net, const unsigned char *addr, const unsigned short len )
{
   addrType = (len != 0) ? remoteStationAddr : remoteBroadcastAddr;
   addrNet = net;
   addrLen = len;
   memcpy( addrAddr, addr, addrLen );
}

void BACnetAddress::LocalBroadcast( void )
{
   addrType = localBroadcastAddr;
   addrNet = 0;
   addrLen = 0;
}

void BACnetAddress::RemoteBroadcast( const short net )
{
   addrType = remoteBroadcastAddr;
   addrNet = net;
   addrLen = 0;
}

void BACnetAddress::GlobalBroadcast( void )
{
   addrType = globalBroadcastAddr;
   addrNet = 65535;
   addrLen = 0;
}

BACnetAddress & BACnetAddress::operator =( const BACnetAddress &arg )
{
   addrType = arg.addrType;
   addrNet = arg.addrNet;
   addrLen = arg.addrLen;
   memcpy( addrAddr, arg.addrAddr, addrLen );
   return *this;
}

// Show just a hex MAC address, without network
CString BACnetAddress::MacAddress() const
{
   CString str;
   if (addrLen == 0)
   {
      str = "broadcast";
   }
   else
   {
      // 12-34-56-78
      char buf[4];
      for (UINT ix = 0; ix < addrLen; ix++)
      {
         if (ix > 0)
            str += '-';

         sprintf( buf, "%02X", addrAddr[ix] );
         str += buf;
      }
   }

   return str;
}

// Convert string with or without dashes, to MAC address
// DOES NOT change addrType
bool BACnetAddress::SetMacAddress( const char *addrString )
{
   bool retval = true;
   if ((strcmp( addrString, "*" ) == 0) || (_stricmp( addrString, "broadcast" ) == 0))
   {
      addrLen = 0;
   }
   else
   {
      // Optionally, as X'123456'
      bool quoted = (addrString[0] == 'X') && (addrString[1] == '\'');
      if (quoted)
         addrString += 2;

      unsigned int len = 0;
      while (*addrString && (len < 8))
      {
         unsigned int val, nchar = 0;
         retval = (sscanf( addrString, "%02X%n", &val, &nchar ) >= 1) && (nchar == 2);
         if (!retval)
            break;

         addrAddr[len++] = val;
         addrString += 2;

         // Optional - or colon
         if (!quoted && ((*addrString == '-') || (*addrString == ':')))
            addrString += 1;

         if (quoted && (*addrString == '\''))
            break;
      }

      if (retval)
      {
         retval = (quoted) ? ((addrString[0] == '\'') && (addrString[1] == 0))
                       : (addrString[0] == 0);
      }

      if (retval)
      {
         addrLen = len;
      }
   }

   return retval;
}

// MAC address as text.  If port is provided, address format may be nicer (dotted decimal IP address, etc.)
CString BACnetAddress::PrettyMacAddress( VTSPort *pThePort ) const
{
   CString str;
   if ((addrType == remoteBroadcastAddr) || (addrType == globalBroadcastAddr) ||
       (pThePort == NULL) || (pThePort->m_nPortType != ipPort))
   {
      // Basic address
      str = MacAddress();
   }
   else
   {
      // We COULD show other addresses in specific formats:
      // - MS/TP might be decimal byte
      // - ZigBee or IPv6 VMAC might be decimal version of the three-bytes value
      // - Ethernet might use colons instead of dashes
      // But the single-value versions are easily confused: is 10 ten or sixteen?
      // So we just do IP, which is the biggest pain to head-convert.
      str.Format( "%u.%u.%u.%u:%u",
                  addrAddr[0], addrAddr[1], addrAddr[2], addrAddr[3],
                  (addrAddr[4] << 8) + addrAddr[5] );
   }

   return str;
}

// Network and MAC address as text.  If port is provided, address format may be nicer (dotted decimal IP address, etc.)
CString BACnetAddress::PrettyAddress( VTSPort *pThePort ) const
{
   CString str( PrettyMacAddress( pThePort ) );

   if ((addrType == remoteStationAddr) || (addrType == remoteBroadcastAddr))
   {
      // Remote network.
      CString net;
      net.Format( " on network %u", addrNet );
      str += net;
   }

   return str;
}

bool operator ==( const BACnetAddress &addr1, const BACnetAddress &addr2 )
{
   int         i;

   // address types must match
   if (addr1.addrType != addr2.addrType)
      return false;

   // remote broadcast and remote station have a network, localStation and remote
   // station have an address.
   switch (addr1.addrType) {
      case nullAddr:
      case localBroadcastAddr:
      case globalBroadcastAddr:
         break;

      case remoteBroadcastAddr:
         if (addr1.addrNet != addr2.addrNet) return false;
         break;

      case remoteStationAddr:
         if (addr1.addrNet != addr2.addrNet) return false;
         // FALL INTO next case
      case localStationAddr:
         if (addr1.addrLen != addr2.addrLen) return false;
         for (i = 0; i < addr1.addrLen; i++)
            if (addr1.addrAddr[i] != addr2.addrAddr[i])
               return false;
         break;

      default:
         throw_(1); // no other address types allowed
   }

   // must be equal
   return true;
}

#if _TSMDebug
//
// operator <<(ostream &strm,const BACnetAddress &addr)
//

ostream &operator <<(ostream &strm,const BACnetAddress &addr)
{
   const static char hex[] = "0123456789ABCDEF"
   ;
   int      i
   ;
   
   strm << '[';
   
   switch (addr.addrType) {
      case nullAddr:
         break;
         
      case remoteStationAddr:
         strm << addr.addrNet << ':';
      case localStationAddr:
         strm << "0x";
         for (i = 0; i < addr.addrLen; i++) {
            strm << hex[ addr.addrAddr[i] >> 4 ];
            strm << hex[ addr.addrAddr[i] & 0x0F ];
         }
         break;
         
      case localBroadcastAddr:
         strm << '*';
         break;
         
      case remoteBroadcastAddr:
         strm << addr.addrNet << ":*";
         break;
         
      case globalBroadcastAddr:
         strm << "*:*";
         break;
   }
   
   strm << ']';
   
   return strm;
}
#endif


IMPLEMENT_DYNAMIC(BACnetEncodeable, CObject)

//
// BACnetEncodeable
//

BACnetEncodeable::BACnetEncodeable()
{
   m_fDataInvalid = false;
}


void BACnetEncodeable::Encode( CString &enc, Format /*theFormat*/ ) const
{
   // Show the class with the missing method.
   enc = this->GetRuntimeClass()->m_lpszClassName;
}


void BACnetEncodeable::Decode( const char * )
{
   m_fDataInvalid = true;
   TRACE1("%s.Decode(char*) Unsupported", this->GetRuntimeClass()->m_lpszClassName);
}


void BACnetEncodeable::Encode( BACnetAPDUEncoder& enc, int context /* = kAppContext */)
{
   TRACE1("%s.Encode(BACnetAPDUEncode& enc) Unsupported", this->GetRuntimeClass()->m_lpszClassName);
}


void BACnetEncodeable::Decode( BACnetAPDUDecoder& dec )
{
   m_fDataInvalid = true;
   TRACE1("%s.Decode(BACnetAPDUDecoder& enc) Unsupported", this->GetRuntimeClass()->m_lpszClassName);
}

int BACnetEncodeable::DataType(void) const
{
   // TODO: but 0 == ob_id in PROPS.H
   return 0;
}

void BACnetEncodeable::Peek( BACnetAPDUDecoder& dec )
{
   int               saveLen = dec.pktLength;
   const BACnetOctet *saveBuff = dec.pktBuffer;

   // use regular decoder
   Decode( dec );

   // restore pointer and length
   dec.pktLength = saveLen;
   dec.pktBuffer = saveBuff;
}


const char * BACnetEncodeable::ToString( Format theFormat ) const
{
   // Use internal buffer and return to caller.
   //
   // The original version had only one static CString, so caller had to
   // copy data out before calling us again.
   // We now use a rota of 8 strings, easing life for our callers
   static int ix = 0;
   static CString str[8];
   ix = (ix + 1) % 8;      // next string

   Encode(str[ix], theFormat);
   return (const char*)(LPCTSTR)str[ix];
}


BACnetEncodeable * BACnetEncodeable::clone()
{
   ASSERT(0);     // shouldn't be called here...
   return NULL;
}

// Initial checks for Match.
// Returns true if no further matching should be done.  Caller should return "isMatch".
// Returns false if the caller should do additional matching.  In this case, rbacnet is
// guaranteed to be of the same class as this.
bool BACnetEncodeable::PreMatch( bool &isMatch, const CRuntimeClass *pClass,
                                 BACnetEncodeable &rbacnet, int iOperator, CString *pstrError ) const
{
   bool retval = true;
   isMatch = false;

   if ((iOperator == '?=') || (iOperator == '>>'))
   {
      // ?= and >> are ALWAYS "Match"
      isMatch = true;
   }
   else if (!rbacnet.IsKindOf(pClass))
   {
      // rbacnet is not of the same type as this.
      if (iOperator == '!=')
      {
         // Test for inequality: rbacnet of different type is DEFINITELY unequal to this!
         isMatch = true;
      }
      else
      {
         // Different type objects are "No match" for everything except inequality.
         if (pstrError != NULL)
         {
            CString strError;
            strError.Format(IDS_SCREX_COMP_TYPES_DIFFER, pClass->m_lpszClassName, rbacnet.GetRuntimeClass()->m_lpszClassName );
            *pstrError = strError;
         }
         isMatch = false;
      }
   }
   else
   {
      // rbacnet is of the same type as this.  Caller must do additional matching.
      retval = false;
   }

   return retval;
}

// Initial checks for Match, when only equality and inequality tests are valid.
// Returns true if no further matching should be done.  Caller should return "isMatch".
// Returns false if the caller should do additional matching.  In this case, rbacnet is
// guaranteed to be of the same class as this.
bool BACnetEncodeable::PreMatchEquality( bool &isMatch, const CRuntimeClass *pClass,
                                         BACnetEncodeable &rbacnet, int iOperator, CString * pstrError ) const
{
   // Do initial test.
   bool retval = PreMatch( isMatch, pClass, rbacnet, iOperator, pstrError );
   if (!retval)
   {
      if ((iOperator != '!=') && (iOperator != '='))
      {
         // Unacceptable comparison operator
         if (pstrError != NULL)
         {
            CString strError, str1(ToString());
            strError.Format(IDS_SCREX_COMPEQREQ, rbacnet.ToString(), OperatorToString(iOperator), str1);
            *pstrError = strError;
         }
         isMatch = false;
         retval = true;
      }
   }

   return retval;
}

// Do comparison for equality/inequality
bool BACnetEncodeable::DoEqualityMatch( bool areSame, BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   if ((areSame && (iOperator == '=')) || (!areSame && (iOperator == '!=')))
      return true;

   // Fail, formatting any error message
   return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);
}

// Base class Match is called by default from most all of the classes.  It just fails and formats the error
bool BACnetEncodeable::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   if (pstrError != NULL)
   {
      CString strError;
      strError.Format(IDS_SCREX_COMPFAILTYPE, ToString(), OperatorToString(iOperator), rbacnet.ToString() );

      // TODO: do we really want to APPEND the error message?
      *pstrError += strError;
   }

   return false;
}

// Factory method to instantiate the correct BACnetEncodeable type from a parse type and 
// decoder...
BACnetEncodeable * BACnetEncodeable::Factory( int nParseType, BACnetAPDUDecoder & dec, int nPropID /* = -1 */ )
{
   BACnetEncodeable *pRetval;
   switch ( nParseType )
   {
      case u127:     // 1..127 ---------------------------------
      case u16:      // 1..16 ----------------------------------
      case ud:       // unsigned dword -------------------------
      case uw:       // unsigned word --------------------------
         // TODO: But the BACnetUnsigned will report only ONE OF THESE as its DataType.
         // That could cause problems for code that compares parseTypes to determine
         // "is this BACnetEncodeable the same type as me".
         // This issue applies to all cases below which share the same code
         pRetval = new BACnetUnsigned(dec);
         break;

      case ssint:    // short signed int -----------------------     // actually the same type
      case sw:       // signed word ----------------------------
      case ptInt32:
         pRetval = new BACnetInteger(dec);
         break;

      case flt:      // float ----------------------------------
         pRetval = new BACnetReal(dec);
         break;

      case pab:      // priority array bpv ---------------------     deal with index cases (-1=all, 0=element count, base 1=index
      case paf:      // priority array flt ---------------------
      case pau:      // priority array unsigned ----------------
      case ptPai:    // priority array signed
         pRetval = new BACnetPriorityArray(dec);
         break;

      case ebool:    // boolean enumeration ---------------------------------
         pRetval = new BACnetBoolean(dec);
         break;

      case bits:     // octet of 1 or 0 flags
      case pss:      // protocol_services_supported
      case pos:      // protocol_objects_supported
         pRetval = new BACnetBitString(dec);
         break;

      case ob_id:    // object identifier
         pRetval = new BACnetObjectIdentifier(dec);
         break;

      case ch_string:     // char [MAX_TEXT_STRING]
         pRetval = new BACnetCharacterString(dec);
         break;

      case enull:    // null enumeration ------------------------------------
         pRetval = new BACnetNull(dec);
         break;

      case et:       // enumation, type determined by property  -------------
         pRetval = BACnetEnumerated::Factory(nPropID);
         pRetval->Decode(dec);
         break;

      case ptDate:   // date ------------------------------------------------
      case ddate:
         pRetval = new BACnetDate(dec);
         break;

      case ptTime:   // time -------------------------------------------------
      case ttime:
         pRetval = new BACnetTime(dec);
         break;

      case dt:       // date/time stamp -------------------------------------
         pRetval = new BACnetDateTime(dec);
         break;

      case dtrange:  // range of dates ---------------------------------------
         pRetval = new BACnetDateRange(dec);
         break;

      case calist:   // array of calendar entries -----------------------------
         pRetval = new BACnetListOfCalendarEntry(dec);
         break;

      case dabind:   // device address binding list--------------------------------
         pRetval = new BACnetListOfAddressBinding(dec);
         break;

      case dabindelem:  // device address binding --------------------------------
         pRetval = new BACnetAddressBinding(dec);
         break;

      case lobj:     // array of object identifiers ----------------------------
      case looref:   // list (or array) of object IDs
         // TODO: I think these should be the SAME:
         // - DUDTOOL turns OBJECT_LIST and CONFIGURATION_FILES into lobj
         // - stdobjpr defines OBJECT_LIST and CONFIGURATION_FILES as looref
         pRetval = new BACnetObjectIDList(dec);
         break;

      case uwarr:    // unsigned array ------------------------------------------
      case stavals:  // list of unsigned ----------------------------------------
         pRetval = new BACnetUnsignedArray(dec);
         break;

      case statext:
      case actext:   // character string array ----------------------------------
         pRetval = new BACnetTextArray(dec);
         break;

      case prival:   // single priority value----------------------------------
         pRetval = new BACnetPriorityValue(dec);
         break;

      case calent:   // single calendar entry ----------------------------------
         pRetval = new BACnetCalendarEntry(dec);
         break;

      case TSTMP:    // time stamp, could be multiple type---------------------
         pRetval = new BACnetTimeStamp(dec);
         break;

      case TSTMParr: // array of time stamp, could be multiple type---------------------
         pRetval = new BACnetTimeStampArray(dec);
         break;

      case setref:
      case propref:  // object prop refs
         pRetval = new BACnetObjectPropertyReference(dec);
         break;

      case lopref:   // list of device object property references
         pRetval = new BACnetListOfDeviceObjectPropertyReference(dec);
         break;

      case devobjref:
         pRetval = new BACnetDeviceObjectReference(dec);
         break;

      case lodoref:  // LJT  List Of Device Object References
         pRetval = new BACnetListOfDeviceObjectReference(dec);
         break;

      case devobjpropref:  // deviceobject prop refs
         pRetval = new BACnetDeviceObjectPropertyReference(dec);
         break;

      case recip:    // bacnet recipient
         pRetval = new BACnetRecipient(dec);
         break;

      case tsrecip:  // list of time synch recipients
         pRetval = new BACnetListOfRecipient(dec);
         break;

      case vtcl:     // vt classes
         pRetval = new BACnetListOfVTClass(dec);
         break;

      case destination:    //
         pRetval = new BACnetDestination(dec);
         break;

      case reciplist:   // list of BACnetDestination
         pRetval = new BACnetListOfDestination(dec);
         break;

      case COVSub:
         pRetval = new BACnetCOVSubscription(dec);
         break;

      case lCOVSub:
         pRetval = new BACnetListOfCOVSubscription(dec);
         break;

      case raslist:  // list of readaccessspecs
         pRetval = new BACnetReadAccessSpecification(dec);
         break;

      case act:      // action array
         pRetval = new BACnetActionCommand(dec);
         break;

      case evparm:   // event parameter
         pRetval = new BACnetEventParameter(dec);
         break;

      case skeys:    // session keys
         pRetval = new BACnetSessionKey(dec);
         break;

      case xsched:    // exception schedule: array[] of specialevent
         pRetval = new BACnetExceptionSchedule(dec);
         break;

      case wsched:   // weekly schedule: array[7] of list of timevalue
         pRetval = new BACnetTimeValue(dec);
         break;

      case vtse:     // list of active  vt sessions (parse type) 
         pRetval = new BACnetVTSession(dec);
         break;

      case PT_PROPLIST: // List of BACnetPropertyIdentifiers
         // Make a prototype enumerated value
         pRetval = new BACnetEnumerated( 0, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier );
         // Create an array of them
         pRetval = new BACnetGenericArray(pRetval);
         // Decode the array
         pRetval->Decode(dec);
         break;

      default:
         pRetval = NULL;
         break;
   }

   return pRetval;
}


IMPLEMENT_DYNAMIC(BACnetNull, BACnetEncodeable)

//
// BACnetNull
//

BACnetNull::BACnetNull( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


BACnetNull::BACnetNull( )
{
}


void BACnetNull::Encode( BACnetAPDUEncoder &enc, int context )
{
   // check for space
   enc.CheckSpace( 1 );

   // encode it
   if (context != kAppContext)
      enc.pktBuffer[enc.pktLength++] = ((BACnetOctet)context << 4) + 0x08;
   else
      enc.pktBuffer[enc.pktLength++] = 0x00;
}

void BACnetNull::Decode( BACnetAPDUDecoder &dec )
{
   // enough for the tag byte?
   if (dec.pktLength < 1)
      throw_(4) /* not enough data */;

   // suck out the tag
   BACnetOctet tag = (dec.pktLength--,*dec.pktBuffer++);

   // verify its a null
   if (((tag & 0x08) == 0) && ((tag & 0xF0) != 0x00))
      throw_(5) /* mismatched data type */;
}

void BACnetNull::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc = "Null";    // true 135.1 : NULL;
}

void BACnetNull::Decode( const char *dec )
{
   if (_stricmp( dec, "Null") != 0)
      throw_(6) /* null must be 'null' */;
}

int BACnetNull::DataType() const
{
   return enull;
}

BACnetEncodeable * BACnetNull::clone()
{
   return new BACnetNull();
}


bool BACnetNull::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetNull), rbacnet, iOperator, pstrError))
      return isMatch;

   return DoEqualityMatch( true, rbacnet, iOperator, pstrError );
}


//==========================================================================

IMPLEMENT_DYNAMIC(BACnetAddr, BACnetEncodeable)

BACnetAddr::BACnetAddr()
{
}

BACnetAddr::BACnetAddr( BACnetAddress * paddr )
{
   m_bacnetAddress = *paddr;
}


BACnetAddr::BACnetAddr( BACnetOctet * paMAC, unsigned int nMACLen )
{
   AssignAddress(0, paMAC, nMACLen);
}


BACnetAddr::BACnetAddr( unsigned int nNet, BACnetOctet * paMAC, unsigned int nMACLen )
{
   AssignAddress(nNet, paMAC, nMACLen);
}


BACnetAddr::BACnetAddr( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


void BACnetAddr::Encode( BACnetAPDUEncoder& enc, int context )
{
   BACnetUnsigned bacnetNet(m_bacnetAddress.addrNet);
   BACnetOctetString bacnetMAC(m_bacnetAddress.addrAddr, m_bacnetAddress.addrLen);

   // TODO: is this used?  Seems to put the same context tag on the
   // ELEMENTS.  Shouldn't a context tag WRAP the item?
   // The elements here should ALWAYS get primitive tags
   bacnetNet.Encode(enc, context);
   bacnetMAC.Encode(enc, context);
}


void BACnetAddr::Decode( BACnetAPDUDecoder& dec )
{
   BACnetUnsigned bacnetNet(dec);
   BACnetOctetString bacnetMAC(dec);

   AssignAddress(bacnetNet.uintValue, bacnetMAC.strBuff, bacnetMAC.strBuffLen);
}


void BACnetAddr::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc.Format( "{%d,", m_bacnetAddress.addrNet );
   if (m_bacnetAddress.addrLen > 0)
   {
      AppendXhex( enc, m_bacnetAddress.addrAddr, m_bacnetAddress.addrLen );
   }
   else
   {
      // Broadcast
      enc += '*';
   }
   enc += '}';
}


void BACnetAddr::AssignAddress(unsigned int nNet, BACnetOctet * paMAC, unsigned int nMACLen )
{
   if ( nMACLen == 0 )
   {
      // broadcast
      if ( nNet == 0 )
         m_bacnetAddress.LocalBroadcast();
      else
         m_bacnetAddress.RemoteBroadcast(nNet);
   }
   else
   {
      // specific address
      if ( nNet == 0 )
         m_bacnetAddress.LocalStation(paMAC, nMACLen);
      else
         m_bacnetAddress.RemoteStation(nNet, paMAC, nMACLen);
   }
}

int BACnetAddr::DataType(void) const
{
   // TODO: need a parsetype
   return -1;
}

BACnetEncodeable * BACnetAddr::clone()
{
   return new BACnetAddr(m_bacnetAddress.addrNet, m_bacnetAddress.addrAddr, m_bacnetAddress.addrLen);
}

bool BACnetAddr::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetAddr), rbacnet, iOperator, pstrError))
      return isMatch;

   isMatch = (m_bacnetAddress == ((BACnetAddr&)rbacnet).m_bacnetAddress);
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}

BACnetAddr & BACnetAddr::operator =( const BACnetAddr & arg )
{
   m_bacnetAddress = arg.m_bacnetAddress;
   return *this;
}


IMPLEMENT_DYNAMIC(BACnetBoolean, BACnetEncodeable)

//
// BACnetBoolean
//
BACnetBoolean::BACnetBoolean( int bvalu )
: boolValue(bvalu ? bTrue : bFalse)
{
}

BACnetBoolean::BACnetBoolean( bool bvalu )
: boolValue(bvalu ? bTrue : bFalse)
{
}

BACnetBoolean::BACnetBoolean( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


void BACnetBoolean::Encode( BACnetAPDUEncoder& enc, int context )
{
   // Note: A context tagged boolean is not encoded the same as an
   // application tagged boolean.  See clause 20.2.3.
   //
   if (context == kAppContext) {
      enc.CheckSpace( 1 );

      enc.pktBuffer[enc.pktLength++] = 0x10 + (boolValue == bFalse ? 0x00 : 0x01);
   } else {
      enc.CheckSpace( 2 );

      enc.pktBuffer[enc.pktLength++] = ((BACnetOctet)context << 4) + 0x09;
      enc.pktBuffer[enc.pktLength++] = (boolValue == bFalse ? 0x00 : 0x01);
   }
}


void BACnetBoolean::Decode( BACnetAPDUDecoder &dec )
{
   BACnetOctet tag;

   // enough for the tag byte?
   if (dec.pktLength < 1)
      throw(7) /* not enough data */;

   tag = (dec.pktLength--,*dec.pktBuffer++);

   // it could be application tagged
   if (tag == 0x10)
   {
      boolValue = bFalse;
   } 
   else if (tag == 0x11)
   {
      boolValue = bTrue;
   }
   else
   {
      // verify context tagged and length
      if ((tag & 0x0F) != 0x09)
         throw_(8) /* bad length */;

      // check for more data
      if (dec.pktLength < 1)
         throw_(9);

      boolValue = (eBACnetBoolean)(dec.pktLength--,*dec.pktBuffer++);
   }
}


void BACnetBoolean::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc = (boolValue ? "True" : "False");  // true 135.1  T or TRUE, F or FALSE
}


void BACnetBoolean::Decode( const char *dec )
{
   // check for a keyword
   switch (*dec++) {
      case '1':
      case 'S': case 's':           // Set
      case 'T': case 't':           // True
      case 'E': case 'e':           // Enable
         boolValue = bTrue;
         break;

      case '0':
      case 'R': case 'r':           // Reset
      case 'F': case 'f':           // False
      case 'D': case 'd':           // Disable
         boolValue = bFalse;
         break;

      case 'O': case 'o':
         boolValue = (eBACnetBoolean)((*dec == 'N') || (*dec == 'n'));  // On (all others are Off)
         break;

      default:
         throw_(10) /* unknown keyword */;
   }
}


int BACnetBoolean::DataType() const
{
   return ebool;
}

BACnetEncodeable * BACnetBoolean::clone()
{
   return new BACnetBoolean(boolValue);
}


BACnetBoolean & BACnetBoolean::operator =( const BACnetBoolean & arg )
{
   boolValue = arg.boolValue;
   return *this;
}


bool BACnetBoolean::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetBoolean), rbacnet, iOperator, pstrError))
      return isMatch;

   isMatch = (boolValue == ((BACnetBoolean &) rbacnet).boolValue);
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


IMPLEMENT_DYNAMIC(BACnetEnumerated, BACnetEncodeable)

//
// BACnetEnumerated
//
BACnetEnumerated::BACnetEnumerated( int evalu, const NetworkSniffer::BACnetStringTable &nameList )
: m_enumValue( evalu )
, m_papNameList(nameList.m_pStrings)
, m_nListSize(nameList.m_nStrings)
{
}

BACnetEnumerated::BACnetEnumerated( int evalu /* = 0*/, const char* const *papNameList /* = NULL */, int nListSize /* = 0 */ )
: m_enumValue( evalu )
, m_papNameList(papNameList)
, m_nListSize(nListSize)
{
}

BACnetEnumerated::BACnetEnumerated( BACnetAPDUDecoder & dec )
: m_papNameList(NULL)
, m_nListSize(0)
{
   Decode(dec);
}


void BACnetEnumerated::Encode( BACnetAPDUEncoder& enc, int context )
{
   int            len;
   unsigned long  valuCopy;

   // reduce the value to the smallest number of octets
   len = 4;
   valuCopy = (unsigned long)m_enumValue;
   while ((len > 1) && ((valuCopy & 0xFF000000) == 0)) {
      len -= 1;
      valuCopy = (valuCopy << 8);
   }
   
   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, len ).Encode( enc );
   else
      BACnetAPDUTag( enumeratedAppTag, len ).Encode( enc );

   // fill in the data
   while (len--) {
      enc.pktBuffer[enc.pktLength++] = (BACnetOctet)(valuCopy >> 24);
      valuCopy = (valuCopy << 8);
   }
}

void BACnetEnumerated::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;
   int            rslt;

   // extract the tag
   tag.Decode( dec );

   // check the type
   if (!tag.tagClass && (tag.tagNumber != enumeratedAppTag))
      throw_(11) /* mismatched data type */;

   // copy out the data
   rslt = 0;
   while (tag.tagLVT) {
      rslt = (rslt << 8) + (dec.pktLength--,*dec.pktBuffer++);
      tag.tagLVT -= 1;
   }

   // save the result
   m_enumValue = rslt;
}

void BACnetEnumerated::Encode( CString &enc, Format /*theFormat*/ ) const
{
   Encode( enc, m_papNameList, m_nListSize );
}

// TODO: use the string tables to avoid explicit size?
void BACnetEnumerated::Encode( CString &enc, const char * const *table, int tsize ) const
{
   int   valu = m_enumValue;

   if ((m_enumValue < 0) || !table || (m_enumValue >= tsize))
      enc.Format( "%d", valu );
   else
      enc = table[m_enumValue];
}

void BACnetEnumerated::Decode( const char *dec )
{
   Decode( dec, m_papNameList, m_nListSize );
}

void BACnetEnumerated::Decode( const char *dec, const char * const *table, int tsize )
{
   if (IsDigit(*dec))
   {
      // explicit number
      for (m_enumValue = 0; *dec; dec++)
      {
         if (!IsDigit(*dec))
            throw_(12) /* invalid character */;
         else
            m_enumValue = (m_enumValue * 10) + (*dec - '0');
      }
   }
   else if (!table)
   {
      throw_(13) /* no translation available */;
   }
   else
   {
      m_enumValue = 0;
      while (m_enumValue < tsize)
      {
         if (strncmp(dec,*table,strlen(dec)) == 0)
            break;
         if (_stricmp(dec,*table) == 0)
            break;
         table += 1;
         m_enumValue += 1;
      }
      if (m_enumValue >= tsize)
         throw_(14) /* no matching translation */;
   }
}


int BACnetEnumerated::DataType() const
{
   return et;
}

BACnetEncodeable * BACnetEnumerated::clone()
{
   return new BACnetEnumerated(m_enumValue, m_papNameList, m_nListSize);
}


BACnetEnumerated & BACnetEnumerated::operator =( const BACnetEnumerated & arg )
{
   m_enumValue = arg.m_enumValue;
   m_nListSize = arg.m_nListSize;
   m_papNameList = arg.m_papNameList;
   return *this;
}


bool BACnetEnumerated::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetEnumerated), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!::Match(iOperator, m_enumValue, ((BACnetEnumerated &) rbacnet).m_enumValue) )
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}


// TODO: if the BACnetEncodeable::Factory took an enumtype instead of a propertyID,
// this method could be replaced by a few lines using the enum-type from the table entry
BACnetEnumerated * BACnetEnumerated::Factory(int nPropID)
{
   const NetworkSniffer::BACnetStringTable *petable;

   switch ( nPropID )
   {
      case OBJECT_TYPE:
         petable = NetworkSniffer::GetEnumStringTable(eiObjectTypes);
         break;

      case EVENT_STATE:
         petable = NetworkSniffer::GetEnumStringTable(eiEvState);
         break;

      case EVENT_TYPE:
         petable = NetworkSniffer::GetEnumStringTable(eiEvType);
         break;

      case RELIABILITY:
         petable = NetworkSniffer::GetEnumStringTable(eiReli);
         break;

      case UNITS:
         petable = NetworkSniffer::GetEnumStringTable(eiEU);
         break;

      case NOTIFY_TYPE:
         petable = NetworkSniffer::GetEnumStringTable(eiNT);
         break;

      case PRESENT_VALUE:
      case RELINQUISH_DEFAULT:
         // TODO: eiBPV isn't correct for AccessDoor.
         petable = NetworkSniffer::GetEnumStringTable(eiBPV);
         break;

      case POLARITY:
         petable = NetworkSniffer::GetEnumStringTable(eiPolar);
         break;

      case ALARM_VALUE:
         petable = NetworkSniffer::GetEnumStringTable(eiBPV);
         break;

      case SYSTEM_STATUS:
         petable = NetworkSniffer::GetEnumStringTable(eiDS);
         break;

      case SEGMENTATION_SUPPORTED:
         petable = NetworkSniffer::GetEnumStringTable(eiSegOpt);
         break;

      case FILE_ACCESS_METHOD:
         petable = NetworkSniffer::GetEnumStringTable(eiFAM);
         break;

      case OUTPUT_UNITS:
         petable = NetworkSniffer::GetEnumStringTable(eiEU);
         break;

      case CONTROLLED_VARIABLE_UNITS:
         petable = NetworkSniffer::GetEnumStringTable(eiEU);
         break;

      case ACTION:
         petable = NetworkSniffer::GetEnumStringTable(eiLoopAct);
         break;

      case PROPORTIONAL_CONSTANT_UNITS:
         petable = NetworkSniffer::GetEnumStringTable(eiEU);
         break;

      case INTEGRAL_CONSTANT_UNITS:
         petable = NetworkSniffer::GetEnumStringTable(eiEU);
         break;

      case DERIVATIVE_CONSTANT_UNITS:
         petable = NetworkSniffer::GetEnumStringTable(eiEU);
         break;

      case PROGRAM_STATE:
         petable = NetworkSniffer::GetEnumStringTable(eiPrState);
         break;

      case REASON_FOR_HALT:
         petable = NetworkSniffer::GetEnumStringTable(eiPrErr);
         break;

      case VT_CLASSES_SUPPORTED:
         petable = NetworkSniffer::GetEnumStringTable(eiVTCls);
         break;

      case BACKUP_AND_RESTORE_STATE:
         petable = NetworkSniffer::GetEnumStringTable(eiBackupState);
         break;

      case LOGGING_TYPE:
         petable = NetworkSniffer::GetEnumStringTable(eiLoggingType);
         break;

      default:
         return new BACnetEnumerated(0);
   }

   return new BACnetEnumerated( 0, *petable );
}


//
// BACnetUnsigned
//
IMPLEMENT_DYNAMIC(BACnetUnsigned, BACnetEncodeable)

BACnetUnsigned::BACnetUnsigned( unsigned long uivalu )
   : uintValue( uivalu )
{
}


BACnetUnsigned::BACnetUnsigned( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


void BACnetUnsigned::Encode( BACnetAPDUEncoder& enc, int context )
{
   int            len;
   unsigned long  valuCopy;

   // reduce the value to the smallest number of bytes
   len = 4;
   valuCopy = uintValue;
   while ((len > 1) && ((valuCopy & 0xFF000000) == 0)) {
      len -= 1;
      valuCopy = (valuCopy << 8);
   }

   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, len ).Encode( enc );
   else
      BACnetAPDUTag( unsignedIntAppTag, len ).Encode( enc );

   // fill in the data
   while (len--) {
      enc.pktBuffer[enc.pktLength++] = (BACnetOctet)(valuCopy >> 24);
      valuCopy = (valuCopy << 8);
   }
}

void BACnetUnsigned::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;
   unsigned long  rslt;

   // extract the tag
   tag.Decode( dec );

   // check the type
   if (!tag.tagClass && (tag.tagNumber != unsignedIntAppTag))
      throw_(15) /* mismatched data type */;

   // copy out the data
   rslt = 0;
   while (tag.tagLVT) {
      rslt = (rslt << 8) + (dec.pktLength--,*dec.pktBuffer++);
      tag.tagLVT -= 1;
   }

   // save the result
   uintValue = rslt;
}

void BACnetUnsigned::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc.Format( "%lu", uintValue );
}


void BACnetUnsigned::Decode( const char *dec )
{
   unsigned int   t;

   // figure out what encoding to use
   //Moved by Yajun Zhou, 2002-8-16
   //Moved to line 647
   // if (IsDigit(*dec)) {                            // nnn
   //    // integer encoding
   //    for (uintValue = 0; *dec; dec++)
   //       if (!IsDigit(*dec))
   //          throw_(16) /* invalid character */;
   //       else
   //          uintValue = (uintValue * 10) + (*dec - '0');
   // } else
   ////////////////////////////////////////
   if ( ((dec[0] == 'D') && (dec[1] == '\''))               // D'nnn'
      || ((dec[0] == 'd') && (dec[1] == '\''))
      ) {
      // decimal encoding
      dec += 2;
      if (((strlen(dec) - 1) % 3) != 0)         // must be triplet
         throw_(17) /* must be triplet */;
      for (uintValue = 0; *dec != '\''; ) {
         if (!IsDigit(*dec))
            throw_(18) /* invalid character */;
         t = (*dec++ - '0');

         if (!IsDigit(*dec))
            throw_(19) /* invalid character */;
         t = (t * 10) + (*dec++ - '0');

         if (!IsDigit(*dec))
            throw_(20) /* invalid character */;
         t = (t * 10) + (*dec++ - '0');

         uintValue = (uintValue * 256) + t;
      }
   } else
   if ( ((dec[0] == '0') && (dec[1] == 'x'))
      || ((dec[0] == 'X') && (dec[1] == '\''))
      || ((dec[0] == 'x') && (dec[1] == '\''))
      || ((dec[0] == '&') && (dec[1] == 'x'))
      || ((dec[0] == '&') && (dec[1] == 'X'))
      ) {
      // hex encoding
      dec += 2;
      for (uintValue = 0; *dec && (*dec != '\''); dec++) {
         if (!IsXDigit(*dec))
            throw_(21) /* invalid character */;
         uintValue = (uintValue * 16) + (IsDigit(*dec) ? (*dec - '0') : (*dec - 'A' + 10));
      }
   } else
   if ( ((dec[0] == '0') && (dec[1] == 'o'))
      || ((dec[0] == 'O') && (dec[1] == '\''))
      || ((dec[0] == 'o') && (dec[1] == '\''))
      || ((dec[0] == '&') && (dec[1] == 'O'))
      || ((dec[0] == '&') && (dec[1] == 'o'))
      ) {
      // octal encoding
      dec += 2;
      for (uintValue = 0; *dec && (*dec != '\''); dec++) {
         if ((*dec < '0') || (*dec > '7'))
            throw_(21) /* invalid character */;
         //Modified by Yajun Zhou, 2002-8-16
         //uintValue = (uintValue * 16) + (*dec - '0');
         uintValue = (uintValue * 8) + (*dec - '0');
         ///////////////////////////////////
      }
   } else
   if ( ((dec[0] == '0') && (dec[1] == 'b'))
      || ((dec[0] == 'B') && (dec[1] == '\''))
      || ((dec[0] == 'b') && (dec[1] == '\''))
      || ((dec[0] == '&') && (dec[1] == 'B'))
      || ((dec[0] == '&') && (dec[1] == 'b'))
      ) {
      // binary encoding
      dec += 2;
      for (uintValue = 0; *dec && (*dec != '\''); dec++) {
         if ((*dec < '0') || (*dec > '1'))
            throw_(22) /* invalid character */;
         uintValue = (uintValue * 2) + (*dec - '0');
      }
   } else
   //Moved by Yajun Zhou, 2002-8-16
   if (IsDigit(*dec)) {                            // nnn
      // integer encoding
      for (uintValue = 0; *dec; dec++)
         if (!IsDigit(*dec))
            throw_(16) /* invalid character */;
         else
            uintValue = (uintValue * 10) + (*dec - '0');
   } else
   //////////////////////////////////
      throw_(23) /* unknown or invalid encoding */;
}


int BACnetUnsigned::DataType() const
{
   return ud;
}

BACnetEncodeable * BACnetUnsigned::clone()
{
   return new BACnetUnsigned(uintValue);
}


bool BACnetUnsigned::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetUnsigned), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!::Match(iOperator, uintValue, ((BACnetUnsigned &) rbacnet).uintValue))
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}


IMPLEMENT_DYNAMIC(BACnetInteger, BACnetEncodeable)

//
// BACnetInteger
//
BACnetInteger::BACnetInteger( int ivalu )
   : intValue( ivalu )
{
}


BACnetInteger::BACnetInteger( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


void BACnetInteger::Encode( BACnetAPDUEncoder& enc, int context )
{
   int      len, valuCopy;

   // reduce the value to the smallest number of bytes, be careful about 
   // the next upper bit down being sign extended
   len = 4;
   valuCopy = intValue;
   while (len > 1) {
      if ((intValue >= 0) && ((valuCopy & 0xFF800000) != 0x00000000))
         break;
      if ((intValue < 0) && ((valuCopy & 0xFF800000) != 0xFF800000))
         break;

      len -= 1;
      valuCopy = (valuCopy << 8);
   }

   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, len ).Encode( enc );
   else
      BACnetAPDUTag( integerAppTag, len ).Encode( enc );

   // fill in the data
   while (len--) {
      enc.pktBuffer[enc.pktLength++] = (valuCopy >> 24) & 0x0FF;
      valuCopy = (valuCopy << 8);
   }
}

void BACnetInteger::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;
   int            rslt;

   // extract the tag
   tag.Decode( dec );

   // check the type
   if (!tag.tagClass && (tag.tagNumber != integerAppTag))
      throw_(24) /* mismatched data type */;

   // check for sign extension
   if ((*dec.pktBuffer & 0x80) != 0)
      rslt = -1;
   else
      rslt = 0;

   // copy out the data
   while (tag.tagLVT) {
      rslt = (rslt << 8) + (dec.pktLength--,*dec.pktBuffer++);
      tag.tagLVT -= 1;
   }

   // save the result
   intValue = rslt;
}

void BACnetInteger::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc.Format( "%d", intValue );
}

void BACnetInteger::Decode( const char *dec )
{
   bool           negValue = false;
   int            t;

   // look for a sign
   if (*dec == '-') {
      negValue = true;
      dec += 1;
   } else
   if (*dec == '+')
      dec += 1;

   // figure out what encoding to use
   //Moved by Yajun Zhou, 2002-8-17
   //Moved to line 834
   // if (IsDigit(*dec)) {                            // nnn
   //    // integer encoding
   //    for (intValue = 0; *dec; dec++)
   //       if (!IsDigit(*dec))
   //          throw_(25) /* invalid character */;
   //       else
   //          intValue = (intValue * 10) + (*dec - '0');
   // } else
   ////////////////////////////////////////////
   if ( ((dec[0] == 'D') && (dec[1] == '\''))               // D'nnn'
      || ((dec[0] == 'd') && (dec[1] == '\''))
      ) {
      // decimal encoding
      dec += 2;
      if (((strlen(dec) - 1) % 3) != 0)         // must be triplet
         throw_(26) /* must be triplet */;
      for (intValue = 0; *dec != '\''; ) {
         if (!IsDigit(*dec))
            throw_(27) /* invalid character */;
         t = (*dec++ - '0');

         if (!IsDigit(*dec))
            throw_(28) /* invalid character */;
         t = (t * 10) + (*dec++ - '0');

         if (!IsDigit(*dec))
            throw_(29) /* invalid character */;
         t = (t * 10) + (*dec++ - '0');

         intValue = (intValue * 256) + t;
      }
   } else
   if ( ((dec[0] == '0') && (dec[1] == 'x'))             // 0xFF, X'FF', &xFF
      || ((dec[0] == 'X') && (dec[1] == '\''))
      || ((dec[0] == 'x') && (dec[1] == '\''))
      || ((dec[0] == '&') && (dec[1] == 'x'))
      || ((dec[0] == '&') && (dec[1] == 'X'))
      ) {
      // hex encoding
      dec += 2;
      for (intValue = 0; *dec && (*dec != '\''); dec++) {
         if (!IsXDigit(*dec))
            throw_(30) /* invalid character */;
         intValue = (intValue * 16) + (IsDigit(*dec) ? (*dec - '0') : (*dec - 'A' + 10));
      }
   } else
   if ( ((dec[0] == '0') && (dec[1] == 'o'))             // 0o377, O'377', &O377
      || ((dec[0] == 'O') && (dec[1] == '\''))
      || ((dec[0] == 'o') && (dec[1] == '\''))
      || ((dec[0] == '&') && (dec[1] == 'O'))
      || ((dec[0] == '&') && (dec[1] == 'o'))
      ) {
      // octal encoding
      dec += 2;
      for (intValue = 0; *dec && (*dec != '\''); dec++) {
         if ((*dec < '0') || (*dec > '7'))
            throw_(31) /* invalid character */;
         //Modified by Yajun Zhou, 2002-8-17
         //intValue = (intValue * 16) + (*dec - '0');
         intValue = (intValue * 8) + (*dec - '0');
         /////////////////////////////////////////
      }
   } else
   if ( ((dec[0] == '0') && (dec[1] == 'b'))             // 0b11111111, B'11111111', &B11111111
      || ((dec[0] == 'B') && (dec[1] == '\''))
      || ((dec[0] == 'b') && (dec[1] == '\''))
      || ((dec[0] == '&') && (dec[1] == 'B'))
      || ((dec[0] == '&') && (dec[1] == 'b'))
      ) {
      // binary encoding
      dec += 2;
      for (intValue = 0; *dec && (*dec != '\''); dec++) {
         if ((*dec < '0') || (*dec > '1'))
            throw_(32) /* invalid character */;
         intValue = (intValue * 2) + (*dec - '0');
      }
   } else
   //Moved by Yajun Zhou, 2002-8-17
   if (IsDigit(*dec)) {                            // nnn
      // integer encoding
      for (intValue = 0; *dec; dec++)
         if (!IsDigit(*dec))
            throw_(25) /* invalid character */;
         else
            intValue = (intValue * 10) + (*dec - '0');
   } else
   //////////////////////////////////////////////////
      throw_(33) /* unknown or invalid encoding */;

   // update for sign
   if (negValue)
      intValue = (intValue * -1);
}


int BACnetInteger::DataType() const
{
   return sw;
}

BACnetEncodeable * BACnetInteger::clone()
{
   return new BACnetInteger(intValue);
}


bool BACnetInteger::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetInteger), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!::Match(iOperator, intValue, ((BACnetInteger &) rbacnet).intValue))
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}


IMPLEMENT_DYNAMIC(BACnetReal, BACnetEncodeable)

//
// BACnetReal
//
BACnetReal::BACnetReal( float rvalu )
   : realValue( rvalu )
{
}


BACnetReal::BACnetReal( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


void BACnetReal::Encode( BACnetAPDUEncoder& enc, int context )
{
   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, 4 ).Encode( enc );
   else
      BACnetAPDUTag( realAppTag, 4 ).Encode( enc );

   // fill in the data
#if (__DECCXX)
   cvt$convert_float( &realValue, CVT$K_VAX_F
      , enc.pktBuffer+enc.pktLength, CVT$K_IEEE_S
      , CVT$M_ROUND_TO_NEAREST + CVT$M_BIG_ENDIAN
      );
   enc.pktLength += 4;
#else
#ifdef ENDIAN_SWAP
   unsigned long cpy = *(unsigned long *)&realValue;
   for (int j = 3; j >= 0; j--)
      enc.pktBuffer[enc.pktLength++] = (BACnetOctet)(cpy >> (j * 8));
#else
   memcpy( enc.pktBuffer+enc.pktLength, &realValue, (size_t)4 );
   enc.pktLength += 4;
#endif
#endif
}

void BACnetReal::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

   // verify the tag can be extracted
   tag.Decode( dec );

   // check the type and length
   if (!tag.tagClass && (tag.tagNumber != realAppTag))
      throw_(34) /* mismatched data type */;
   if (tag.tagLVT != 4)
      throw_(35) /* four bytes of data expected */;

   // copy out the data
#if (__DECCXX)
   cvt$convert_float( dec.pktBuffer, CVT$K_IEEE_S
      , &realValue, CVT$K_VAX_F
      , CVT$M_ROUND_TO_NEAREST + CVT$M_BIG_ENDIAN
      );
   dec.pktBuffer += 4;
   dec.pktLength -= 4;
#else
#ifdef ENDIAN_SWAP
   unsigned long cpy = 0;
   for (int j = 3; dec.pktLength && j >= 0; j--)
      cpy = (cpy << 8) + (dec.pktLength--,*dec.pktBuffer++);
   realValue = *(float *)&cpy;
#else
   memcpy( &realValue, dec.pktBuffer, (size_t)4 );
   dec.pktBuffer += 4;
   dec.pktLength -= 4;
#endif
#endif
}

void BACnetReal::Encode( CString &enc, Format theFormat ) const
{
   // We use FloatToString() to get nice treatment of NaN and inf
   // For EPICS and script use, ask for full resolution.
   // For "plain" use, use a more friendly resolution
   FloatToString( enc, realValue, (theFormat != FMT_PLAIN) );
}

void BACnetReal::Decode( const char *dec )
{
   // We use StringToFloat() to also accept NaN and inf
   int nChar = StringToFloat( dec, realValue );
   if (nChar != strlen(dec))
      throw_(36) /* format error */;
}


int BACnetReal::DataType() const
{
   return flt;
}

BACnetEncodeable * BACnetReal::clone()
{
   return new BACnetReal(realValue);
}


bool BACnetReal::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetReal), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!::Match(iOperator, realValue, ((BACnetReal &) rbacnet).realValue ))
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}


IMPLEMENT_DYNAMIC(BACnetDouble, BACnetEncodeable)

//
// BACnetDouble
//
BACnetDouble::BACnetDouble( double dvalu )
   : doubleValue( dvalu )
{
}

BACnetDouble::BACnetDouble( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


void BACnetDouble::Encode( BACnetAPDUEncoder& enc, int context )
{
   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, 8 ).Encode( enc );
   else
      BACnetAPDUTag( doubleAppTag, 8 ).Encode( enc );
   
   // fill in the data
#if (__DECCXX)
   // VAX anyone?
   cvt$convert_float( &doubleValue, CVT$K_VAX_G
      , enc.pktBuffer+enc.pktLength, CVT$K_IEEE_T
      , CVT$M_ROUND_TO_NEAREST + CVT$M_BIG_ENDIAN
      );
   enc.pktLength += 8;
#else
#ifdef ENDIAN_SWAP
   union {
      double   src;
      struct {
         unsigned long  t1, t2;
      }  s;
   }     x;

   x.src = doubleValue;
   // High half first, high byte first
   for (int j = 3; j >= 0; j--)
      enc.pktBuffer[enc.pktLength++] = (BACnetOctet)(x.s.t2 >> (j * 8));
   for (int k = 3; k >= 0; k--)
      enc.pktBuffer[enc.pktLength++] = (BACnetOctet)(x.s.t1 >> (k * 8));
#else
   memcpy( enc.pktBuffer+enc.pktLength, &doubleValue, (size_t)8 );
   enc.pktLength += 8;
#endif
#endif
}

void BACnetDouble::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

   // verify the tag can be extracted
   tag.Decode( dec );

   // check the type and length
   if (!tag.tagClass && (tag.tagNumber != doubleAppTag))
      throw_(37) /* mismatched data type */;
   if (tag.tagLVT != 8)
      throw_(38) /* eight bytes of data expected */;

   // copy out the data
#if (__DECCXX)
   cvt$convert_float( dec.pktBuffer, CVT$K_IEEE_T
      , &doubleValue, CVT$K_VAX_G
      , CVT$M_ROUND_TO_NEAREST + CVT$M_BIG_ENDIAN
      );
   dec.pktBuffer += 8;
   dec.pktLength -= 8;
#else
#ifdef ENDIAN_SWAP
   union {
      double   src;
      struct {
         unsigned long  t1, t2;
      }  s;
   }     x;

   x.s.t2 = x.s.t1 = 0;
   // High half first, high byte first
   for (int j = 3; dec.pktLength && j >= 0; j--)
      x.s.t2 = (x.s.t2 << 8) + (dec.pktLength--,*dec.pktBuffer++);
   for (int k = 3; dec.pktLength && k >= 0; k--)
      x.s.t1 = (x.s.t1 << 8) + (dec.pktLength--,*dec.pktBuffer++);
   doubleValue = x.src;
#else
   memcpy( &doubleValue, dec.pktBuffer, (size_t)8 );
   dec.pktBuffer += 8;
   dec.pktLength -= 8;
#endif
#endif
}

void BACnetDouble::Encode( CString &enc, Format theFormat ) const
{
   // We use DoubleToString() to get nice treatment of NaN and inf
   // For EPICS and script use, ask for full resolution.
   // For "plain" use, use a more friendly resolution
   DoubleToString( enc, doubleValue, (theFormat != FMT_PLAIN) );
}

void BACnetDouble::Decode( const char *dec )
{
   // We use StringToDouble() to also accept NaN and inf
   int nChar = StringToDouble( dec, doubleValue );
   if (nChar != strlen(dec))
      throw_(39) /* format error */;
}


int BACnetDouble::DataType() const
{
   return flt;
}

BACnetEncodeable * BACnetDouble::clone()
{
   return new BACnetDouble(doubleValue);
}


bool BACnetDouble::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetDouble), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!::Match(iOperator, doubleValue, ((BACnetDouble &) rbacnet).doubleValue ))
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}


IMPLEMENT_DYNAMIC(BACnetCharacterString, BACnetEncodeable)

//
// BACnetCharacterString
//

//BACnetCharacterString::BACnetCharacterString( char *svalu )
BACnetCharacterString::BACnetCharacterString( LPCSTR svalu )
   : strEncoding(0)
{
   strBuff = NULL;
   Initialize(svalu);
}


//void BACnetCharacterString::Initialize( char * svalu )
void BACnetCharacterString::Initialize( LPCSTR svalu )
{
   strLen = (svalu ? strlen(svalu) : 0);
   if ( strLen )
   {
      delete[] strBuff;
      strBuff = new BACnetOctet[strLen];
   }
   else
   {
      strBuff = NULL;
   }

   if (svalu && strBuff)
      memcpy( strBuff, svalu, (size_t)strLen );
}


BACnetCharacterString::BACnetCharacterString( BACnetAPDUDecoder & dec )
                 :strEncoding(0)
{
   strBuff = NULL;
   Initialize(NULL);
   Decode(dec);
}


BACnetCharacterString::BACnetCharacterString( BACnetCharacterString & cpy )
                 :strEncoding(cpy.strEncoding)
{
   strBuff = NULL;
   Initialize((char *) cpy.strBuff);
}


BACnetCharacterString::BACnetCharacterString( CString & rstr )
                 :strEncoding(0)
{
   strBuff = NULL;
   Initialize(rstr.GetBuffer(1));
}


BACnetCharacterString::~BACnetCharacterString( void )
{
   KillBuffer();
}


void BACnetCharacterString::KillBuffer(void)
{
   if ( strBuff != NULL )
      delete[] strBuff;
   strBuff = NULL;
}


void BACnetCharacterString::SetValue( const char *svalu, int enc )
{
   // toss the old stuff
   KillBuffer();

   // copy in the new
   strEncoding = enc;
   Initialize(svalu);

// strLen = (svalu ? strlen(svalu) : 0);
// strBuff = new BACnetOctet[strLen];
// if (svalu)
//    memcpy( strBuff, svalu, (size_t)strLen );
}

bool BACnetCharacterString::Equals( const char *valu )
{
   // can't compare against non-ASCII strings
   if (strEncoding != 0)
      return false;

   // not the same if different lengths
   if (strlen(valu) != strLen)
      return false;

   // case insensitive, strBuff is not null-terminated
   for (unsigned i = 0; i < strLen; i++)
      if (strBuff == NULL || ToLower(strBuff[i]) != ToLower(valu[i]))
         return false;

   // success
   return true;
}

void BACnetCharacterString::Encode( BACnetAPDUEncoder& enc, int context )
{
   int  len = strLen + 1;

   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, len ).Encode( enc );
   else
      BACnetAPDUTag( characterStringAppTag, len ).Encode( enc );

   // fill in the data
   enc.pktBuffer[enc.pktLength++] = strEncoding;
   len -= 1;

   if ( strBuff != NULL )
      memcpy( enc.pktBuffer+enc.pktLength, strBuff, (size_t)len );

   enc.pktLength += len;
}

void BACnetCharacterString::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

   // verify the tag can be extracted
   tag.Decode( dec );

   // check the type and length
   if (!tag.tagClass && (tag.tagNumber != characterStringAppTag))
      throw_(40) /* mismatched data type */;

   // extract the encoding
   strEncoding = (dec.pktLength--,*dec.pktBuffer++);

   // skip the encoding and set the length
   tag.tagLVT -= 1;
   strLen = tag.tagLVT;

   // allocate a new buffer
   KillBuffer();
   if ( strLen )
      strBuff = new BACnetOctet[strLen];

   // copy out the data, null terminated
   if ( strBuff != NULL )
      memcpy( strBuff, dec.pktBuffer, (size_t)strLen );

   dec.pktBuffer += strLen;
   dec.pktLength -= strLen;
}

void BACnetCharacterString::Encode( CString &enc, Format /*theFormat*/ ) const
{
   char buff[10];

   // check for the simple case
   if (strEncoding == 0)
   {
      // ANSI (or UTF-8 in latter years...), shown in double quotes
      // TODO: if there are characters above 127, show the string as hex, or
      // at least show the UTF-8 groups in hex, at least for some formats

      // 135.1 clause 4.3 says to use ' or ` quoting if the string contains ",
      // and " quoting if it contains ' or `.
      // DOES NOT say what to do about a string that contains BOTH.
      // We at least handle the easy cases.
      char wrap = '"';
      enc = wrap;

      const char *src = (char *)strBuff;
      for (unsigned i = 0; i < strLen; i++)
      {
         TCHAR ch = (TCHAR)strBuff[i];
         if (ch == 0)
         {
            // This indicates a NULL within the counted bytes.
            // This is a bug in some implementations seen in the wild.
            // They seem to hope we stop at null, so we will...
            break;
         }
         else if (ch < ' ')
         {
            // TODO: should we use \r \n \t for the common control characters?
            // 135.1 4.3 specifies no method.
            sprintf( buff, "\\x%02X", ch );
            enc += buff;
         }
         else
         {
            // We COULD \-escape the double quote here, but we follow 135.1 4.3
            if (ch == '"')
            {
               // String contains double-quotes.  Change the wrapper to single-quote
               wrap = '\'';
               enc.SetAt( 0, wrap );
            }
            enc += ch;
         }
      }

      enc += wrap;
   }
   else 
   {
      // Some other character set
      switch (strEncoding) 
      {
      case 1:
         enc = "IBM-MICROSOFT-DBCS, X'";
         break;
      case 2:
         enc = "JIS-C-6226, X'";
         break;
      case 3:
         enc = "UCS-4, X'";
         break;
      case 4:
         enc = "UCS-2, X'";
         break;
      case 5:
         enc = "ISO-8859-1, X'";
         break;
      default:
         enc.Format( "%d, X'", strEncoding );
         break;
      }

      // encode the content
      for (unsigned i = 0; i < strLen && strBuff != NULL; i++) 
      {
         sprintf( buff, "\\x%02X", strBuff[i] );
         enc += buff;
      }

      enc += '\'';
   }
}

void BACnetCharacterString::Decode( const char *dec )
{
   char        *dst;
   const char  *src;

   if ( dec == NULL )
      return;

   // TODO: A' isn't in 135.1.  Is it used anywhere?
   // If so, perhaps REMEMBER AND USE the "ASCII" flag.
   //
   // check for explicit ASCII string
   if ((*dec == 'A') && (*(dec+1) == '\''))
      dec += 1;

   // 135.1-2013 says
   // "character strings are represented as one or more characters enclosed in
   // double, single or accent grave quotes as defined in 4.3: 'text' or `text'
   // or "text";
   //
   // If the first character is a quote, process as a normal string
   char quoteChar = *dec;
   if ((quoteChar == '\"') || (quoteChar == '\'') || (quoteChar == '`'))
   {
      // Quoted string.  Assume ASCII/UTF-8 encoding
      strEncoding = 0;
      dec++;

      // String begun with accent grave (`) ends with single quote (')
      if (quoteChar == '`')
         quoteChar = '\'';

      // Count characters until closing quote.
      strLen = 0;
      src = dec;
      while (*src && (*src != quoteChar))
      {
         if (*src == '\\')
         {
            if (ToLower(*(src+1)) == 'x')
               src += 4;
            else
               src += 2;
         }
         else
         {
            src += 1;
         }
         strLen++;
      }

      // Allocate a new buffer
      KillBuffer();
      if ( strLen )
         strBuff = new BACnetOctet[ strLen ];

      // Copy the data
      src = dec;
      dst = (char *)strBuff;
      while (*src && (*src != quoteChar) && dst != NULL)
      {
         if (*src == '\\')
         {
            if (ToLower(*(src+1)) == 'x')
            {
               src += 2;
               *dst = (IsDigit(*src) ? *src - '0' : (ToUpper(*src) - 'A') + 10) << 4;
               src += 1;
               *dst++ += (IsDigit(*src) ? *src - '0' : (ToUpper(*src) - 'A') + 10);
               src += 1;
            }
            else
            {
               // TODO: this is bogus: \" will insert double-quote, but \n will insert "n".
               src += 1;
               *dst++ = *src++;
            }
         }
         else
         {
            *dst++ = *src++;
         }
      }
   } else {
#if VTSScanner
      int      encType;

      // create a scanner bound to the text
      ScriptScanner  scan( dec );
      ScriptToken    tok;

      // get something
      scan.Next( tok );

      // if a hex string was given, allow default to ASCII
      if ((tok.tokenType == scriptValue) && (tok.tokenEnc == scriptHexEnc))
         strEncoding = 0;
      else {
         if (!tok.IsInteger( encType, ScriptCharacterTypeMap ))
            throw_(41) /* encoding type keyword expected */;
         else
         if ((encType < 0) || (encType > 255))
            throw_(42) /* out of range */;
         strEncoding = encType;

         // get the next token
         scan.Next( tok );

         // skip the comma if it was entered
         if ((tok.tokenType == scriptSymbol) && (tok.tokenSymbol == ','))
            scan.Next( tok );
      }

      // if the next beast is an ASCII or hex string
      if ((tok.tokenType == scriptValue) && (tok.tokenEnc == scriptASCIIEnc)) {
         int saveEncoding = strEncoding;
         Decode( tok.tokenValue );
         strEncoding = saveEncoding;
      } else
      if ((tok.tokenType == scriptValue) && (tok.tokenEnc == scriptHexEnc)) {
         BACnetOctetString ostr;
         ostr.Decode( tok.tokenValue );

         // build a new buffer
         KillBuffer();
         strLen = ostr.strLen;
         if ( strLen )
            strBuff = new BACnetOctet[ strLen ];

         if ( strBuff != NULL )
            memcpy( strBuff, ostr.strBuff, strLen );
      } else
         throw_(43) /* ASCII or hex string expected */;
#else
      throw_(44) /* not implemented */;
#endif
   }
}


int BACnetCharacterString::DataType() const
{
   return ch_string;
}

BACnetEncodeable * BACnetCharacterString::clone()
{
   return new BACnetCharacterString(*this);
}


bool BACnetCharacterString::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetCharacterString), rbacnet, iOperator, pstrError))
      return isMatch;

   // check encoding
   if (strEncoding != ((BACnetCharacterString &) rbacnet).strEncoding)
   {
      pstrError->Format(IDS_SCREX_COMPFAILSTRENCODING, ((BACnetCharacterString &) rbacnet).strEncoding, strEncoding );
      return false;
   }

   // This may present a problem where the encoding is non ASCII or whatever the heck CString uses...
   // So let's just keep the old stuff around for the sake of it, eh?
   if ( !Match((BACnetCharacterString &) rbacnet, iOperator) )
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}


bool BACnetCharacterString::Match( BACnetCharacterString & rstring, int iOperator )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetCharacterString), rstring, iOperator, NULL))
      return isMatch;

   switch(iOperator)
   {
      case '=':   return *this == rstring;
      case '<':   return *this < rstring;
      case '>':   return *this > rstring;
      case '<=':  return *this <= rstring;
      case '>=':  return *this >= rstring;
      case '!=':  return *this != rstring;
      default:
         ASSERT(0);
   }
   return false;
}


bool BACnetCharacterString::operator ==( const BACnetCharacterString & arg )
{
   return (CString((LPCTSTR)strBuff, strLen) == CString((LPCTSTR)arg.strBuff, arg.strLen)) != 0;      // account for nasty Microsoft BOOL stuff
}


bool BACnetCharacterString::operator !=( const BACnetCharacterString & arg )
{
   return (CString((LPCTSTR)strBuff, strLen) != CString((LPCTSTR)arg.strBuff, arg.strLen)) != 0;      // account for nasty Microsoft BOOL stuff
}


bool BACnetCharacterString::operator <=( const BACnetCharacterString & arg )
{
   return (CString((LPCTSTR)strBuff, strLen) <= CString((LPCTSTR)arg.strBuff, arg.strLen)) != 0;      // account for nasty Microsoft BOOL stuff
}


bool BACnetCharacterString::operator <( const BACnetCharacterString & arg )
{
   return (CString((LPCTSTR)strBuff, strLen) < CString((LPCTSTR)arg.strBuff, arg.strLen)) != 0;    // account for nasty Microsoft BOOL stuff
}


bool BACnetCharacterString::operator >=( const BACnetCharacterString & arg )
{
   return (CString((LPCTSTR)strBuff, strLen) >= CString((LPCTSTR)arg.strBuff, arg.strLen)) != 0;      // account for nasty Microsoft BOOL stuff
}


bool BACnetCharacterString::operator >( const BACnetCharacterString & arg )
{
   return (CString((LPCTSTR)strBuff, strLen) > CString((LPCTSTR)arg.strBuff, arg.strLen)) != 0;    // account for nasty Microsoft BOOL stuff
}


/*  Moved from ExpectCharacterString, madanner 10/02
   // verify the encoding
   if (cstrData.strEncoding != scriptData.strEncoding)
      throw "Character string encoding mismatch";

   // verify the value
   minLen = (cstrData.strLen < scriptData.strLen ? cstrData.strLen : scriptData.strLen);

   switch (spep->exprOp) {
      case '<':
         for (i = 0; i < minLen; i++)
            if (cstrData.strBuff[i] >= scriptData.strBuff[i])
               throw "Character string mismatch";
         // ### what about the rest?
         break;
      case '>':
         for (i = 0; i < minLen; i++)
            if (cstrData.strBuff[i] <= scriptData.strBuff[i])
               throw "Character string mismatch";
         // ### what about the rest?
         break;
      case '<=':
         for (i = 0; i < minLen; i++)
            if (cstrData.strBuff[i] < scriptData.strBuff[i])
               throw "Character string mismatch";
         // ### what about the rest?
         break;
      case '>=':
         for (i = 0; i < minLen; i++)
            if (cstrData.strBuff[i] > scriptData.strBuff[i])
               throw "Character string mismatch";
         // ### what about the rest?
         break;
      case '=':
         if (cstrData.strLen != scriptData.strLen)
            throw "Character string mismatch";
         for (i = 0; i < cstrData.strLen; i++)
            if (cstrData.strBuff[i] != scriptData.strBuff[i])
               throw "Character string mismatch";
         break;
      case '!=':
         if (cstrData.strLen != scriptData.strLen)
            break;
         for (i = 0; i < cstrData.strLen; i++)
            if (cstrData.strBuff[i] != scriptData.strBuff[i])
               break;
         if (i >= cstrData.strLen)
            throw "Character string mismatch";
         break;
   }
*/


IMPLEMENT_DYNAMIC(BACnetOctetString, BACnetEncodeable)

//
// BACnetOctetString::BACnetOctetString
//
BACnetOctetString::BACnetOctetString( void )
   : strLen(0), strBuffLen(0), strBuff(NULL)
{
}


BACnetOctetString::BACnetOctetString( BACnetAPDUDecoder & dec )
   : strLen(0), strBuffLen(0), strBuff(NULL)
{
   Decode(dec);
}


//
// BACnetOctetString::BACnetOctetString
//
BACnetOctetString::BACnetOctetString( int len )
{
   if (len > 0) {
      strLen = 0;
      strBuffLen = len;
      strBuff = new BACnetOctet[ len ];
   } else {
      strLen = strBuffLen = 0;
      strBuff = NULL;
   }
}

//
// BACnetOctetString::BACnetOctetString
//
BACnetOctetString::BACnetOctetString( const BACnetOctetString &cpy )
{
   strLen = strBuffLen = cpy.strLen;
   if (strLen)
   {
      strBuff = new BACnetOctet[ cpy.strLen ];
      memcpy( strBuff, cpy.strBuff, (size_t)cpy.strLen );
   }
   else
   {
      strBuff = NULL;
   }
}

//
// BACnetOctetString::BACnetOctetString
// CAUTION: KEEPS the octet buffer, and DOES NOT delete it in our destructor
// since strBufLen == 0.
//
// Caller must ensure that the buffer persists for the life of this
// object, and must ensure that the buffer is deleted.
//
// Note the contrasting behavior of the const BACnetOctet version
// of the constructor.
//
BACnetOctetString::BACnetOctetString( BACnetOctet *bytes, int len )
{
   strLen = len;
   strBuff = bytes;
   strBuffLen = 0;
}


// This one copies the data... 
BACnetOctetString::BACnetOctetString( const BACnetOctet *bytes, int len )
{
   strLen = strBuffLen = len;
   if ( strLen )
   {
      strBuff = new BACnetOctet[len];
      memcpy( strBuff, bytes, len );
   }
   else
   {
      strBuff = NULL;
   }
}


//
// BACnetOctetString::~BACnetOctetString
//
BACnetOctetString::~BACnetOctetString( void )
{
   Flush();
}

//
// BACnetOctetString::PrepBuffer
//
void BACnetOctetString::PrepBuffer( int size )
{
   BACnetOctet *newBuff;
   const int   blockFactor = 512;
   int         newSize = size + (blockFactor - (size % blockFactor));

   // allocate a new buffer big enough
   newBuff = new BACnetOctet[ newSize ];

   // if there is existing data, copy it
   if (strBuff && strLen)
      memcpy( newBuff, strBuff, strLen );

   // if current buffer owned, delete it
   //   TODO: Seems as if strBuffLen SHOULD BE used to determine ownership,
   //   since the destructor won't delete strBuff if it is zero.
   //   Who removed the test and why?
// if (strBuff && strBuffLen)
   if (strBuff)
      delete[] strBuff;

   // point to the new buffer
   strBuff = newBuff;
   strBuffLen = newSize;
}

//
// BACnetOctetString::Flush
//
void BACnetOctetString::Flush( void )
{
   // strBuffLen is also used to determine ownership of this buffer...
   // So even if strBuff is allocated, if strBuffLen = 0 we shouldn't delete it (trust me).
   if (strBuff && strBuffLen)
      delete[] strBuff;

   strBuff = NULL;
   strLen = strBuffLen = 0;
}

//
// BACnetOctetString::Append
//
void BACnetOctetString::Append( BACnetOctet byte )
{
   // make sure the buffer can handle it
   if (strLen + 1 > strBuffLen)
      PrepBuffer( strLen + 1 );

   // append the data
   strBuff[strLen++] = byte;
}

//
// BACnetOctetString::Insert
//
void BACnetOctetString::Insert( BACnetOctet *bytes, int len, int position )
{
   if (position < strLen)
   {
      // Insert within the buffer.
      // make sure the buffer can handle it.
      if (strLen + len > strBuffLen)
         PrepBuffer( strLen + len );

      // move existing data out of the way
      memmove( strBuff+position+len, strBuff+position, len );
   }
   else
   {
      // Insert at or past the end of the buffer.
      // make sure the buffer can handle it.
      if (position + len > strBuffLen)
         PrepBuffer( position + len );
   }

   // copy in new data
   memcpy( strBuff+position, bytes, len );
   strLen += len;
}

//
// BACnetOctetString::Insert
//
void BACnetOctetString::Insert( const BACnetOctetString &cpy, int position )
{
   Insert( cpy.strBuff, cpy.strLen, position );
}

//
// BACnetOctetString::operator []
//
BACnetOctet &BACnetOctetString::operator [](const int indx)
{
   return strBuff[indx];
}

//
// BACnetOctetString::Reference
//
void BACnetOctetString::Reference( BACnetOctet *bytes, int len )
{
   // TODO: Seems as if strBuffLen SHOULD BE used to determine ownership,
   // since the destructor won't delete strBuff if it is zero.
   // Who removed the test and why?
// if (strBuff && strBuffLen)
   if (strBuff)
      delete[] strBuff;

   strBuff = bytes;
   strLen = len;
   strBuffLen = 0;      // DO NOT delete strBuff in Flush or destructor
}

int BACnetOctetString::Length( void )
{
   return strLen;
}

void BACnetOctetString::Encode( BACnetAPDUEncoder& enc, int context )
{
   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, strLen ).Encode( enc );
   else
      BACnetAPDUTag( octetStringAppTag, strLen ).Encode( enc );

   // fill in the data
   if ( strBuff )
      memcpy( enc.pktBuffer+enc.pktLength, strBuff, (size_t)strLen );
   enc.pktLength += strLen;
}

void BACnetOctetString::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

   // verify the tag can be extracted
   tag.Decode( dec );

   // check the type and length
   if (!tag.tagClass && (tag.tagNumber != octetStringAppTag))
      throw_(45) /* mismatched data type */;

   // check for space
   if ((strBuffLen != 0) && (strBuffLen != tag.tagLVT)) {
      if ( strBuff )
         delete[] strBuff;
      strLen = strBuffLen = tag.tagLVT;
      if ( strLen )
         strBuff = new BACnetOctet[ tag.tagLVT ];
      else
         strBuff = NULL;
   }
   else if (!strBuff) {
      strLen = strBuffLen = tag.tagLVT;
      if ( strLen )
         strBuff = new BACnetOctet[ tag.tagLVT ];
   }

   // copy the data
   strLen = tag.tagLVT;

   if ( strBuff )
      memcpy( strBuff, dec.pktBuffer, (size_t)strLen );

   dec.pktBuffer += tag.tagLVT;
   dec.pktLength -= tag.tagLVT;
}

void BACnetOctetString::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc.Empty();
   if (!strBuff)
   {
      // TODO: original had this, but EPICS format would show X'' instead?
      return;
   }

   AppendXhex( enc, strBuff, strLen ); 
}

void BACnetOctetString::Decode( const char *dec )
{
   int   value;
   char  c;

   // toss existing content
   Flush();

   // According to 135.1-2011, octet strings are encoded as
   // "pairs of hex digits enclosed in either single quotes (X'2D') or accent
   // graves (X'60'), and preceded by the letter "X": X'001122'"
   //
   // Our Encode( CString &enc ) produces this format.
   // But for convenience during data entry we also accept
   // - x' and x`
   // - pairs of unwrapped hex characters with or without spaces
   // - bytes as 0xNN separated by space
   // - bytes as &xNN or &XNN separated by space because the code we started from did...
   c = ToUpper( dec[0] );
   if ( (c == 'X') && ((dec[1] == '\'') || (dec[1] == '`')) )
   {
      // Canonical format: pairs of hex digits until the closing quote or end of the string
      dec += 2;
      while (*dec && (*dec != '\'') && (*dec != '`'))
      {
         c = ToUpper( *dec++ );
         if (!IsXDigit(c))
            throw_(46) /* invalid character  (including NULL) */;
         value = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

         c = ToUpper( *dec++ );
         if (!IsXDigit(c))
            throw_(47) /* invalid character  (including NULL) */;
         value = (value << 4) + (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

         Append( value );
      }
   }
   else
   {
      // Alternate format: unwrapped pairs, or bytes
      // 0x5 0x66 0X7 0X88
      // &x5 &x66 &X7 &X88
      // 5 66 7 88
      // 05660788
      while (*dec)
      {
         // Remove any leading whitespace
         while (IsSpace(*dec)) 
            dec += 1;

         if (*dec == 0)
            break;

         bool isByte = ( ((dec[0] == '0') && (dec[1] == 'x')) ||
                         ((dec[0] == '0') && (dec[1] == 'X')) ||
                         ((dec[0] == '&') && (dec[1] == 'x')) ||
                         ((dec[0] == '&') && (dec[1] == 'X')) );
         if (isByte)
         {
            dec += 2;
         }

         c = ToUpper( *dec++ );
         if (!IsXDigit(c))
            throw_(46) /* invalid character (including NULL) */;
         value = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

         c = ToUpper( *dec );
         if (IsXDigit(c))
         {
            // Two-digit value
            dec += 1;
            value = (value << 4) + (IsDigit(c) ? (c - '0') : (c - 'A' + 10));
         }
         else if ((c == 0) || IsSpace(c))
         {
            // One digit followed by space or NULL is OK.
         }
         else
         {
            throw_(47) /* invalid character (including NULL) */;
         }

         // Byte with lead-in MUST be followed by a space or end of string
         if (isByte && (*dec != 0) && !IsSpace(*dec))
         {
            throw_(47) /* invalid terminator */;
         }

         Append( value );
      }
   }
}

int BACnetOctetString::DataType() const
{
   // TODO: formerly didn't override DataType, so used base class returning 0
   // (which is actually ob_id...)
   // But there IS no type value in PROPS.H for octet string
   return 0;
}


BACnetEncodeable * BACnetOctetString::clone()
{
   return new BACnetOctetString(*this);
}


bool BACnetOctetString::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetOctetString), rbacnet, iOperator, pstrError))
      return isMatch;

   // This formerly compared the ALLOCATED bytes, even though not all of them
   // were necessarily initialized.
   isMatch = (strLen == ((BACnetOctetString&)rbacnet).strLen)
                &&
             ((strLen == 0) || (memcmp(strBuff, ((BACnetOctetString&)rbacnet).strBuff, strLen) == 0));
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


IMPLEMENT_DYNAMIC(BACnetWeekNDay, BACnetOctetString)

BACnetWeekNDay::BACnetWeekNDay()
            :BACnetOctetString(3)
{
   Initialize(0xFF, 0xFF, 0xFF);
}


BACnetWeekNDay::BACnetWeekNDay( int nMonth, int nWeekOfMonth, int nDayOfWeek )
            :BACnetOctetString(3)
{
   Initialize(nMonth, nWeekOfMonth, nDayOfWeek);
}


BACnetWeekNDay::BACnetWeekNDay( BACnetAPDUDecoder& dec )
            :BACnetOctetString(3)
{
   Decode(dec);
}


void BACnetWeekNDay::Initialize( int nMonth, int nWeekOfMonth, int nDayOfWeek )
{
   m_nMonth = nMonth;
   m_nWeekOfMonth = nWeekOfMonth;
   m_nDayOfWeek = nDayOfWeek;

   // Allow even/odd months
   ASSERT( (nMonth >= 1 && nMonth <= 14) || (nMonth == 0xFF) );
   ASSERT( (nWeekOfMonth >= 1 && nWeekOfMonth <= 6) || (nWeekOfMonth == 0xFF) );
   ASSERT( (nDayOfWeek >= 1 && nDayOfWeek <= 7) || (nDayOfWeek == 0xFF) );
   LoadBuffer();
}


void BACnetWeekNDay::LoadBuffer()
{
   strBuff[0] = (BACnetOctet) m_nMonth;
   strBuff[1] = (BACnetOctet) m_nWeekOfMonth;
   strBuff[2] = (BACnetOctet) m_nDayOfWeek;
   strLen = 3;
}


void BACnetWeekNDay::UnloadBuffer()
{
   m_nMonth = strBuff[0];
   m_nWeekOfMonth = strBuff[1];
   m_nDayOfWeek = strBuff[2];
}


void BACnetWeekNDay::Encode( BACnetAPDUEncoder& enc, int context )
{
   // just in case the values have changed...
   LoadBuffer();
   BACnetOctetString::Encode(enc,context);
}


void BACnetWeekNDay::Decode( BACnetAPDUDecoder& dec )
{
   BACnetOctetString::Decode(dec);
   UnloadBuffer();
}


void BACnetWeekNDay::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc.Format( "X\'%02X%02X%02X\'", m_nMonth, m_nWeekOfMonth, m_nDayOfWeek );
}


void BACnetWeekNDay::Decode( const char *dec )
{
   BACnetOctetString::Decode(dec);
   UnloadBuffer();
}


BACnetEncodeable * BACnetWeekNDay::clone()
{
   return new BACnetWeekNDay(m_nMonth, m_nWeekOfMonth, m_nDayOfWeek);
}


bool BACnetWeekNDay::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetWeekNDay), rbacnet, iOperator, pstrError))
      return isMatch;

   isMatch = m_nDayOfWeek   == ((BACnetWeekNDay&)rbacnet).m_nDayOfWeek &&
             m_nMonth       == ((BACnetWeekNDay&)rbacnet).m_nMonth &&
             m_nWeekOfMonth == ((BACnetWeekNDay&)rbacnet).m_nWeekOfMonth;
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


IMPLEMENT_DYNAMIC(BACnetBitString, BACnetEncodeable)

//
// BACnetBitString::BACnetBitString
//
BACnetBitString::BACnetBitString( void )
   : bitLen(0), bitBuffLen(0), bitBuff(0)
{
}


BACnetBitString::BACnetBitString( BACnetAPDUDecoder & dec )
   : bitLen(0), bitBuffLen(0), bitBuff(0)
{
   Decode(dec);
}


//
// BACnetBitString::BACnetBitString
//
BACnetBitString::BACnetBitString( int siz )
   : bitLen(siz), bitBuffLen((siz + 31) / 32), bitBuff(0)
{
   if ( bitBuffLen )
      bitBuff = new unsigned long[ bitBuffLen ];

   if (bitBuff)
      for (int i = 0; i < bitBuffLen; i++)
         bitBuff[i] = 0;
}


BACnetBitString::BACnetBitString( int siz, unsigned char * pbits )
   : bitLen(siz), bitBuffLen((siz + 31) / 32), bitBuff(0)
{
   if ( bitBuffLen )
      bitBuff = new unsigned long[ bitBuffLen ];

   if (bitBuff)
      LoadBitsFromByteArray(pbits);
}

//
// BACnetBitString::BACnetBitString
//
BACnetBitString::BACnetBitString( const BACnetBitString &cpy )
   : bitLen( cpy.bitLen ), bitBuffLen( cpy.bitBuffLen )
{
   unsigned long  *src, *dst = NULL;

   src = cpy.bitBuff;
   if ( bitBuffLen )
      dst = bitBuff = new unsigned long[ bitBuffLen ];

   for (int i = 0; i < bitBuffLen && dst != NULL; i++)
      *dst++ = *src++;  // reversed this so that it is copied correctly LJT 10/27/2005
}

//
// BACnetBitString::~BACnetBitString
//
BACnetBitString::~BACnetBitString( void )
{
   KillBuffer();
}


void BACnetBitString::KillBuffer(void)
{
   if ( bitBuff )
      delete[] bitBuff;
   bitBuff = NULL;
}

//
// BACnetBitString::SetSize
//
void BACnetBitString::LoadBitsFromByteArray( unsigned char * pabBits )
{
   ASSERT(pabBits != NULL);

   // bits stored in char array are already in BACnet order, that is, left to right where
   // msb is first flag, etc.

   for ( int i = 0; pabBits != NULL && i < bitLen; i++ )
      SetBit(i, pabBits[i/8] & (unsigned char)(1<<(7-(i%8))) );
}


void BACnetBitString::SetSize( int siz )
{
   int      newBuffLen = (siz + 31) / 32;

   if (newBuffLen != bitBuffLen) {
      int            i;
      unsigned long  *src = bitBuff,   *dst, *rslt = NULL;

      if ( newBuffLen )
         rslt = new unsigned long[newBuffLen];

      dst = rslt;
      for (i = 0; (i < bitBuffLen) && (i++ < newBuffLen) && dst != NULL; i++)
         *dst++ = *src++;
      while (i++ < newBuffLen)
         *dst++ = 0;

      if ( bitBuff )
         delete[] bitBuff;

      bitBuff = rslt;
      bitBuffLen = newBuffLen;
   }
   bitLen = siz;
}

//
// BACnetBitString::SetBit
//
void BACnetBitString::SetBit( int bit, int valu )
{
   int     intOffset = (bit / 32);
   int     bitOffset = 31 - (bit % 32);

   if (bit >= bitLen)
      SetSize( bit+1 );
   if (bitBuff)
      if (valu)
         bitBuff[intOffset] |= (1 << bitOffset);
      else
         bitBuff[intOffset] &= 0xFFFFFFFF - (1 << bitOffset);
}

//
// BACnetBitString::ResetBit
//
void BACnetBitString::ResetBit( int bit )
{
   int   intOffset = (bit / 32);
   int   bitOffset = 31 - (bit % 32);

   if ((bit < bitLen) && bitBuff)
      bitBuff[intOffset] &= 0xFFFFFFFF - (1 << bitOffset);
}

//
// BACnetBitString::GetBit
//
int BACnetBitString::GetBit( int bit ) const
{
   int   intOffset = (bit / 32);
   int   bitOffset = 31 - (bit % 32);

   if ((bit >= bitLen) || (!bitBuff))
      return 0;

   return ((bitBuff[intOffset] >> bitOffset) & 0x01);
}

//
// BACnetBitString::operator []
//
const int BACnetBitString::operator [](int bit)
{
   int   intOffset = (bit / 32);
   int   bitOffset = 31 - (bit % 32);

   if ((bit >= bitLen) || (!bitBuff))
      return 0;

   return ((bitBuff[intOffset] >> bitOffset) & 0x01);
}

//
// BACnetBitString::operator +=
//
BACnetBitString &BACnetBitString::operator +=( const int bit )
{
   int   intOffset = (bit / 32);
   int   bitOffset = 31 - (bit % 32);

   if (bit >= bitLen)
      SetSize( bit+1 );
   if (bitBuff)
      bitBuff[intOffset] |= (1 << bitOffset);
   return *this;
}

//
// BACnetBitString::operator -=
//
BACnetBitString &BACnetBitString::operator -=( const int bit )
{
   int   intOffset = (bit / 32);
   int   bitOffset = 31 - (bit % 32);

   if ((bit < bitLen) && bitBuff)
      bitBuff[intOffset] &= 0xFFFFFFFF - (1 << bitOffset);
   return *this;
}

//
// BACnetBitString::operator =
//
BACnetBitString &BACnetBitString::operator =( const BACnetBitString &arg )
{
   int      i;

   if (bitLen < arg.bitLen)
      SetSize( arg.bitLen );

   unsigned long  *src = arg.bitBuff,  *dst = bitBuff;

   for (i = 0; i < arg.bitBuffLen && dst != NULL; i++)
      *dst++ = *src++;

   while (i++ < bitBuffLen && dst != NULL )
      *dst++ = 0;

   bitLen = arg.bitLen;

   return *this;
}

//
// BACnetBitString::operator |=
//
BACnetBitString &BACnetBitString::operator |=( const BACnetBitString &arg )
{
   if (bitLen < arg.bitLen)
      SetSize( arg.bitLen );

   unsigned long  *src = arg.bitBuff,  *dst = bitBuff;
   
   for (int i = 0; i < arg.bitBuffLen && dst != NULL; i++)
      *dst++ |= *src++;

   return *this;
}

//
// BACnetBitString::operator &=
//
BACnetBitString &BACnetBitString::operator &=( const BACnetBitString &arg )
{
   int         i;
   int         siz = (bitBuffLen < arg.bitBuffLen ? bitBuffLen : arg.bitBuffLen);
   unsigned long  *src = arg.bitBuff,  *dst = bitBuff;

   for (i = 0; i < siz && dst != NULL; i++)
      *dst++ &= *src++;

   while (i++ < bitBuffLen && dst != NULL)
      *dst++ = 0;

   return *this;
}

//
// BACnetBitString::operator ==
//
bool BACnetBitString::operator ==( BACnetBitString &arg )
{
   if (bitLen != arg.bitLen)
      return false;

   int i;
   for (i = 0; i < bitLen  && GetBit(i) == arg.GetBit(i);  i++ );       // [] doesn't work for some reason

   return i >= bitLen;     // made it through, must be OK
}


void BACnetBitString::Encode( BACnetAPDUEncoder& enc, int context )
{
   int    len = (bitLen + 7) / 8 + 1;

   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, len ).Encode( enc );
   else
      BACnetAPDUTag( bitStringAppTag, len ).Encode( enc );
   
   // fill in the data
   enc.pktBuffer[enc.pktLength++] = 8 - (((bitLen + 7) % 8) + 1);
   len -= 1;

#ifdef ENDIAN_SWAP
   int i = 0;
   while (len && bitBuff) {
      int cpy = bitBuff[i++];
      for (int j = 3; len && j >= 0; j--, len--)
         enc.pktBuffer[enc.pktLength++] = (cpy >> (j * 8)) & 0xFF;
   }
#else
   if ( bitBuff )
      memcpy( enc.pktBuffer+enc.pktLength, bitBuff, (size_t)len );

   enc.pktLength += len;
#endif
}

void BACnetBitString::Decode( BACnetAPDUDecoder& dec )
{
   int            bLen;
   BACnetAPDUTag  tag;

   // verify the tag can be extracted
   tag.Decode( dec );

   // check the type and length
   if (!tag.tagClass && (tag.tagNumber != bitStringAppTag))
      throw_(48) /* mismatched data type */;

   // make sure the destination has enough space
   bLen = ((tag.tagLVT - 1) * 8 - (dec.pktLength--,*dec.pktBuffer++));
   if (bLen > bitLen)
      SetSize( bLen );
   tag.tagLVT -= 1;

   // Don't throw here: SetSize sets the buffer size, but if the size is
   // zero, and the buffer has not yet been allocated, null is OK here:
   // the copy loop below won't do anything
// if (!bitBuff)
//    throw_(49) /* destination too small */;
   
   // copy out the data
#ifdef ENDIAN_SWAP
   int i = 0, copyLen = tag.tagLVT;

   // don't copy more than copyLen octets
   while (dec.pktLength && copyLen) {
      bitBuff[i] = 0;
//    for (int j = 3; (dec.pktLength - 1) > 0 && j >= 0; j--) {         // madanner 5/03, added equality... infinite loop
      for (int j = 3; (dec.pktLength - 1) >= 0 && j >= 0; j--) {
         bitBuff[i] |= (dec.pktLength--,*dec.pktBuffer++) << (j * 8);
         if (!--copyLen)
            break;
      }
      i += 1;
   }
#else
   if ( bitBuff )
      memcpy( bitBuff, dec.pktBuffer, (size_t)tag.tagLVT );

   bitLen = bLen;
   dec.pktBuffer += tag.tagLVT;
   dec.pktLength -= tag.tagLVT;
#endif
}

void BACnetBitString::Encode( CString &enc, Format theFormat ) const
{
   if (theFormat == FMT_SCRIPT)
   {
      enc = "[{";
   }
   else
   {
      enc = "{";
   }

   for (int i = 0; i < bitLen; i++)
   {
      if (i > 0)
         enc += ',';
      enc += (GetBit(i) ? 'T' : 'F');
   }

   if (theFormat == FMT_SCRIPT)
   {
      enc += "}]";
   }
   else
   {
      enc += "}";
   }
}

// TODO: 
// 135.1 wants bitstring as {T,F,F} in the EPICS
// - This is annoying, since {} is also used to delimit constructed data, meaning
//   that a bitstring and a list of BOOLEAN will have the same ASCII encoding.
//   But thus sayeth the standard.
// - Can't pass {T,F,F} here directly from a script, since ResolveExpr gives us
//   only the first bit.
// - [{T,F,F}] comes here (scriptComplex), but with the [] intact. The
//   ScriptToken class has no functions to get at the token string to remove
//   the brackets, so the easiest thing is to allow them here.
// - B'100' was formerly allowed.  Allow it as well so old scripts
//   will continue to work.
//
void BACnetBitString::Decode( const char *dec )
{
   int   bit = 0;
   char  c;

   // toss existing content
   SetSize( 0 );

   // Allow [ wrapping, as when passed from a script
   bool isBracketed = IsChar( dec, '[' );

   // Require { wrapping
   if ( dec[0] == '{' )
   {
      dec++;
      while (*dec && (*dec != '}'))
      {
         c = *dec++;
         if ( c == 'T' || c == '1' )
            SetBit( bit++, 1 );
         else if ( c== 'F' || c == '0' )
            SetBit( bit++, 0 );
         else if ( c != ' ' && c != ',' )
            throw_(50);  // invalid character found
      }
      RequireChar( dec, '}' );
   }
   else if ((dec[0] == 'B') && (dec[1] == '\''))
   {
      // Old format
      dec += 2;
      while (*dec && (*dec != '\'')) 
      {
         c = *dec++;
         if (c == '0')
            SetBit( bit++, 0 );
         else if (c == '1')
            SetBit( bit++, 1 );
         else
            throw_(50); // invalid character 
      }
      RequireChar( dec, '\'' );
   }
   else
   {
      throw_(50);  // invalid character found
   }

   if (isBracketed)
   {
      RequireChar( dec, ']' );
   }
}

int BACnetBitString::DataType() const
{
   return bits;
}

BACnetEncodeable * BACnetBitString::clone()
{
   BACnetBitString * pbits = new BACnetBitString();
   *pbits = *this;
   return pbits;
}


bool BACnetBitString::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetBitString), rbacnet, iOperator, pstrError))
      return isMatch;

   isMatch = (*this == ((BACnetBitString&)rbacnet));
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


IMPLEMENT_DYNAMIC(BACnetDate, BACnetEncodeable)

// Declare these here to avoid confusion and danger of general usage
#define DATE_DONT_CARE       0xFF      // BACnet don't-care value for Time and Date: appears on the wire
#define DATE_IGNORE_ON_INPUT 0xFE      // "Don't care" value for receive to IGNORE an incoming value

//
// BACnetDate::BACnetDate
//
BACnetDate::BACnetDate( void )
{
   time_t      now;
   struct tm   *currtime;

   time( &now );
   currtime = localtime( &now );

   year = currtime->tm_year;
   month = currtime->tm_mon + 1;
   day = currtime->tm_mday;

   CalcDayOfWeek();
}

//
// BACnetDate::BACnetDate
//
BACnetDate::BACnetDate( int y, int m, int d )
   : year(y), month(m), day(d)
{
   if ( y == 0 && m == 0 && d == 0 )
      year = month = day = DATE_IGNORE_ON_INPUT;

   CalcDayOfWeek();
}


BACnetDate::BACnetDate( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


bool BACnetDate::IsValid()
{
   if ( (dayOfWeek != DATE_DONT_CARE) && (dayOfWeek != DATE_IGNORE_ON_INPUT) && (dayOfWeek < 1 || dayOfWeek > 7) )
      return false;

   // Accept LAST, ODD, and EVEN
   if ( (day != DATE_DONT_CARE) && (day != DATE_IGNORE_ON_INPUT) && (day < 1 || day > 34) )
      return false;

   // Accept ODD and EVEN
   if ( (month != DATE_DONT_CARE) && (month != DATE_IGNORE_ON_INPUT) && (month < 1 || month > 14 ))
      return false;

   if ( (year != DATE_DONT_CARE) && (year != DATE_IGNORE_ON_INPUT) && (year < 0 || year > 255))
      return false;

   return true;
}

//
// BACnetDate::CalcDayOfWeek
//
void BACnetDate::CalcDayOfWeek( void )
{
   // Don't try with "don't care", "ignore", or other special values (last, even, odd)
   if ((year == DATE_DONT_CARE)  || (year == DATE_IGNORE_ON_INPUT)  || 
      (month == DATE_DONT_CARE) || (month == DATE_IGNORE_ON_INPUT) || (month > 12) || 
      (day == DATE_DONT_CARE)   || (day == DATE_IGNORE_ON_INPUT)   || (day > 31)) 
   {
      dayOfWeek = DATE_DONT_CARE;
      return;
   }

   // This originally used CTime.
   // But CTime, mktime et al can't handle dates before 1 Jan 1970, while
   // BACnet goes back to 1900.  Such dates have been seen in the wild
   // in ReadRange requests from certain BACnet workstations: they say
   // "give me your first 100 log entries on or after 1 Jan 1900".
   // Thus, we use SYSTEMTIME, which is good back to 1600, instead of CTime.
   SYSTEMTIME systime;
   memset( &systime, 0, sizeof(systime) );
   systime.wYear = year + 1900;
   systime.wMonth = month;
   systime.wDay = day;

   // Convert to FILETIME, then back to SYSTIME to set day of week
   FILETIME filetime;
   ::SystemTimeToFileTime( &systime, &filetime );
   ::FileTimeToSystemTime( &filetime, &systime );

   // SYSTIME says 0 = Sunday, 1 = Monday.
   // We need 1=Monday, ... 7 = Sunday
   dayOfWeek = (systime.wDayOfWeek + 6) % 7 + 1;
}

void BACnetDate::Encode( BACnetAPDUEncoder& enc, int context )
{
   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, 4 ).Encode( enc );
   else
      BACnetAPDUTag( dateAppTag, 4 ).Encode( enc );

   // fill in the data, turning special values into 0xFF = "don't care" on the wire
   enc.pktBuffer[enc.pktLength++] = (year < DATE_IGNORE_ON_INPUT) ? year : 0xFF;
   enc.pktBuffer[enc.pktLength++] = (month < DATE_IGNORE_ON_INPUT) ? month : 0xFF;
   enc.pktBuffer[enc.pktLength++] = (day < DATE_IGNORE_ON_INPUT) ? day : 0xFF;
   enc.pktBuffer[enc.pktLength++] = (dayOfWeek < DATE_IGNORE_ON_INPUT) ? dayOfWeek: 0xFF;
}

void BACnetDate::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

   // verify the tag can be extracted
   tag.Decode( dec );

   // check the type and length
   if (!tag.tagClass && (tag.tagNumber != dateAppTag))
      throw_(51) /* mismatched data type */;
   if (tag.tagLVT != 4)
      throw_(52) /* four bytes of data expected */;

   // copy out the data
   year     = *dec.pktBuffer++;
   month    = *dec.pktBuffer++;
   day         = *dec.pktBuffer++;
   dayOfWeek   = *dec.pktBuffer++;
   dec.pktLength -= 4;

   if ( !IsValid() )
      throw_(90);    // invalid values in date
}

void BACnetDate::Encode( CString &enc, Format theFormat ) const
{
   char buff[10];

   // 135.1 clause 4.4(j) says:
   // "Dates are represented enclosed in parenthesis: (Monday, 24-January-1998).
   // Any "wild card" or unspecified field is shown by an asterisk (X'2A'): 
   // (Monday, *-January-1998). The omission of day of week implies that the day is
   // unspecified: (24-January-1998)"
   //
   // Wrapping is only required for EPICS and current scripting,
   // and it is annoying in a dialog field.
   // So do it only as required.
   if (theFormat == FMT_SCRIPT)
   {
      enc = "[(";
   }
   else if (theFormat == FMT_EPICS)
   {
      enc = "(";
   }
   else
   {
      enc.Empty();
   }

   // day of week
   if (dayOfWeek == DATE_DONT_CARE)
   {
      // This doesn't match 135.1, but it is how the previous version worked
      enc += '*';
   }
   else if (dayOfWeek == DATE_IGNORE_ON_INPUT)
   {
      enc += '?';
   }
   else 
   {
      // Shows full day of week, illegal values numerically as "DOWnn"
      enc += NetworkSniffer::BAC_STRTAB_day_of_week.EnumString( dayOfWeek, "DOW" );
   }

   enc += ',';

   // We historically generated July/4/2014.
   // But that format can't be READ by our EPICS parser,
   // and 135.1 4.4(j) says day-month-year

   // day of month
   if (day == DATE_DONT_CARE)
      enc += '*';
   else if (day == DATE_IGNORE_ON_INPUT)
      enc += '?';
   else if (day == 32)
      enc += "LAST";
   else if (day == 33)
      enc += "ODD";
   else if (day == 34)
      enc += "EVEN";
   else
   {
      sprintf( buff, "%d", day );
      enc += buff;
   }

   enc += '-';

   // month
   if (month == DATE_DONT_CARE)
   {
      enc += '*';
   }
   else if (month == DATE_IGNORE_ON_INPUT)
   {
      enc += '?';
   }
   else 
   {
      // Handles normal months, EVEN, ODD.  Shows numeric for anything else
      enc += NetworkSniffer::BAC_STRTAB_month.EnumString( month );
   }

   enc += '-';

   // year
   if (year == DATE_DONT_CARE)
   {
      enc += '*';
   }
   else if (year == DATE_IGNORE_ON_INPUT)
   {
      enc += '?';
   }
   else
   {
      sprintf( buff, "%d", year + 1900 );
      enc += buff;
   }

   if (theFormat == FMT_SCRIPT)
   {
      enc += ")]";
   }
   else if (theFormat == FMT_EPICS)
   {
      enc += ")";
   }
}

// Decode Month: wildcard, integer, or text
void BACnetDate::DecodeMonth( const char **ppDec )
{
   const char *dec = *ppDec;

   // Check for month
   while (*dec && IsSpace(*dec)) dec++;
   if ( *dec == '*' )
   {
      month = DATE_DONT_CARE;
      dec += 1;
   }
   else if ( *dec == '?' )
   {
      month = DATE_IGNORE_ON_INPUT;
      dec += 1;
   }
   else
   {
      if (IsDigit(*dec))
      {
         // Numeric month
         for (month = 0; IsDigit(*dec); dec++)
         {
            month = (month * 10) + (*dec - '0');
         }
      }
      else
      {
         // Month names, abbreviations, ODD and EVEN
         for (int ix = 1; ix < NetworkSniffer::BAC_STRTAB_month.m_nStrings; ix++)
         {
            if (MatchAndEat( dec, NetworkSniffer::BAC_STRTAB_month.m_pStrings[ix], 3 ))
            {
               month = ix;
            }
         }
      }
      
      // Allow ODD=13 and EVEN=14
      if ( month < 1 || month > 14)
         throw_(92);                      // code for bad month, interpreted in caller's context
   }

   while (*dec && IsSpace(*dec)) dec++;
   *ppDec = dec;
}

// Decode Day of Month: wildcard or integer
void BACnetDate::DecodeDayOfMonth( const char **ppDec )
{
   const char *dec = *ppDec;

   // Check for day of month
   while (*dec && IsSpace(*dec)) dec++;
   if ( *dec == '*' )
   {
      day = DATE_DONT_CARE;
      dec += 1;
   }
   else if ( *dec == '?' )
   {
      day = DATE_IGNORE_ON_INPUT;
      dec += 1;
   }
   else 
   {
      if (IsDigit(*dec))
      {
         // Numeric day of month
         for (day = 0; IsDigit(*dec); dec++)
            day = (day * 10) + (*dec - '0');
      }
      else
      {
         // Allow LAST, EVEN or ODD
         if (MatchAndEat( dec, "LAST" ))
         {
            day = 32;
         }
         else if (MatchAndEat( dec, "ODD" ))
         {
            day = 33;
         }
         else if (MatchAndEat( dec, "EVEN" ))
         {
            day = 34;
         }
      }

      // they've supplied a day of month (I think)
      // Allow LAST=32 ODD=33 and EVEN=34
      if ( day < 1 || day > 34)           // doesn't account for month/day invalids (feb 30)
         throw_(94);                      // code for bad day, interpreted in caller's context
   }

   while (*dec && IsSpace(*dec)) dec++;
   *ppDec = dec;
}

void BACnetDate::Decode( const char *dec )
{
   // Initialize everything to don't care
   dayOfWeek = month = day = year = DATE_DONT_CARE;

   // The EPICS format is (Wednesday, 31-January-2010)
   // - allows the DOW to be omitted entirely.
   // - allows any field to be wildcarded as *
   // - doesn't talk about even, odd, last
   //
   // The format here up to 3.5.3 was (Wednesday, 1-31-2010) or (Wednesday, 1/31/2010),
   // allowing * or ? in any position, but not allowing even, odd, or last
   //
   // Current format (deviating from EPICS for backward compatibility)
   // - Require DOW (or *, or ?, or even just a comma)
   // - order is month/day/year or day-month-year, using slash  or dash to distinguish.
   // - also accept month=13 or "odd", 14 or "even"; and day=32 or "last", 33 or "odd", or 34 or "even"

   // Allow [ wrapping, as when passed from a script
   bool isBracketed = IsChar( dec, '[' );

   // Date is complex data type so must enclose in parenthesis for EPICS format.
   // We make it optional, for convenient use in dialogs etc.
   bool isParenned = IsChar( dec, '(' );

   // If the date has slashes, then month first, else day first
   bool monthFirst = strchr( dec, '/' ) != NULL;

   // Set true below if no day of week information is provided
   bool calculateDOW = false;

   // Check for dow
   if ( *dec == '*' )
   {
      dayOfWeek = DATE_DONT_CARE;
      dec += 1;
   }
   else if ( *dec == '?' )
   {
      dayOfWeek = DATE_IGNORE_ON_INPUT;
      dec += 1;
   }
   else if (IsAlpha(*dec)) 
   {
      // They've provided a day...  read it and test for validity
      for (int i = 1; (i < NetworkSniffer::BAC_STRTAB_day_of_week.m_nStrings); i++)
      {
         if (MatchAndEat(dec, NetworkSniffer::BAC_STRTAB_day_of_week.m_pStrings[i], 3)) {
            dayOfWeek = i;
            break;
         }
      }

      // scanned through... test for validity
      if ( dayOfWeek == DATE_DONT_CARE )
         throw_(91);                         // code for bad supplied day (interpreted in caller's context)
   }
   else
   {
      // No Day of Week provided
      calculateDOW = true;
   }

   // Skip over space and comma
   while (*dec && IsSpace(*dec)) dec++;
   if (*dec == ',')
   {
      dec += 1;
   }

   if (monthFirst)
   {
      // month/day/year
      DecodeMonth( &dec );
      if (*dec != '/')
      {
         throw_(93);                         // code for bad date separator, interpreted in caller's context
      }
      dec += 1;

      DecodeDayOfMonth( &dec );
      if (*dec != '/')
      {
         throw_(93);                         // code for bad date separator, interpreted in caller's context
      }
      dec += 1;
   }
   else
   {
      // day-month-year
      DecodeDayOfMonth( &dec );
      if (*dec != '-')
      {
         throw_(93);                         // code for bad date separator, interpreted in caller's context
      }
      dec += 1;

      DecodeMonth( &dec );
      if (*dec != '-')
      {
         throw_(93);                         // code for bad date separator, interpreted in caller's context
      }
      dec += 1;
   }

   // Check for year
   if ( *dec == '*' )
   {
      year = DATE_DONT_CARE;
      dec += 1;
   }
   else if ( *dec == '?' )
   {
      year = DATE_IGNORE_ON_INPUT;
      dec += 1;
   }
   else 
   {
      int   yr = -1;       // start with no supplied year

      // if they've supplied any number we'll go through this once at least...
      if ( IsDigit(*dec) )
         for (yr = 0; IsDigit(*dec); dec++)
            yr = (yr * 10) + (*dec - '0');

      // 0..40 -> 2000..2040, 41.. -> 1941..
      // negative = error

      if ( yr < 0 )
         throw_(94);                         // code for no supplied year, interpreted in caller's context
      if (yr < 40)
         year = yr + 100;
      else if (yr < 100)
         year = yr;
      else if ((yr >= 1900) && (yr <= (1900 + 254)))
         year = (yr - 1900);
      else
         throw_(95);                         // code for bad year, interpreted in caller's context
   }

   // Close out () or [] wrapping
   EatWhite( dec );
   if (isParenned)
   {
      RequireChar( dec, ')' );
   }

   if (isBracketed)
   {
      RequireChar( dec, ']' );
   }

   // if we've gotten this far, all values have been read in and are correct
   if (calculateDOW)
   {
      // Calculate the day of week (NOP if Y M or D is wildcard or date pattern)
      CalcDayOfWeek();
   }
}

// Convert to integer time in 100 nsec ticks since 1 Jan 1600 (FILETIME)
LONGLONG BACnetDate::AsInt(void) const
{
   ASSERT( year != DATE_DONT_CARE  && month != DATE_DONT_CARE && day != DATE_DONT_CARE);
   ASSERT( year != DATE_IGNORE_ON_INPUT  && month != DATE_IGNORE_ON_INPUT && day != DATE_IGNORE_ON_INPUT);
   ASSERT( (month >= 1) && (month <= 12) && (day >= 1) && (day <= 31));

   // This originally used and returned CTime.
   // But CTime, mktime et al can't handle dates before 1 Jan 1970, while
   // BACnet goes back to 1900.  Such dates have been seen in the wild
   // in ReadRange requests from certain BACnet workstations: they say
   // "give me your first 100 log entries on or after 1 Jan 1900".
   // Thus, we use SYSTEMTIME, which is good back to 1600, instead of CTime.
   SYSTEMTIME systime;
   memset( &systime, 0, sizeof(systime) );
   systime.wYear = year + 1900;
   systime.wMonth = month;
   systime.wDay = day;

   // Convert to FILETIME (nsec since 1 Jan 1600)
   FILETIME filetime;
   ::SystemTimeToFileTime( &systime, &filetime );

   LARGE_INTEGER largeINT;
   largeINT.HighPart = filetime.dwHighDateTime;
   largeINT.LowPart  = filetime.dwLowDateTime;

   return (LONGLONG)largeINT.QuadPart;
}


BACnetDate & BACnetDate::operator =( const BACnetDate & arg )
{
   day = arg.day;
   dayOfWeek = arg.dayOfWeek;
   month = arg.month;
   year = arg.year;

   return *this;
}


int BACnetDate::DataType() const
{
   return ptDate;
}


BACnetEncodeable * BACnetDate::clone()
{
   return new BACnetDate(year, month, day);
}


bool BACnetDate::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetDate), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!Match((BACnetDate &) rbacnet, iOperator))
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}


bool BACnetDate::Match( BACnetDate & rdate, int iOperator )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetDate), rdate, iOperator, NULL))
      return isMatch;

   switch(iOperator)
   {
      case '=':   return *this == rdate;
      case '<':   return *this < rdate;
      case '>':   return *this > rdate;
      case '<=':  return *this <= rdate;
      case '>=':  return *this >= rdate;
      case '!=':  return *this != rdate;
      default:
         ASSERT(0);
   }
   return false;
}


bool ValuesEqual( int v1, int v2 )
{
   // True if either value is "ignore", or if the values are equal
   return (v1 == DATE_IGNORE_ON_INPUT) || (v2 == DATE_IGNORE_ON_INPUT) || (v1 == v2);
/*
   // if neither of them are ignore-on-input, then we should check their value... even if one of them
   // is don't care. 
   if ((v1 != DATE_IGNORE_ON_INPUT) && (v2 != DATE_IGNORE_ON_INPUT) && (v1 != v2))
      return false;

   // OK... Now test for equality when one is don't care and other is shouldn't care

   if ( ((v1 == DATE_SHOULDNT_CARE) && (v2 != DATE_DONT_CARE) && (v2 != DATE_SHOULDNT_CARE)) || 
       ((v2 == DATE_SHOULDNT_CARE) && (v1 != DATE_DONT_CARE) && (v1 != DATE_SHOULDNT_CARE)) )
      return false;

   return true;
*/
}


bool ValuesGreater( int v1, int v2 )
{
   // True if either value is "ignore", or if v1 > v2
   return (v1 == DATE_IGNORE_ON_INPUT) || (v2 == DATE_IGNORE_ON_INPUT) || (v1 > v2);
/*
   if ( v1 != DATE_DONT_CARE  &&  v2 != DATE_DONT_CARE  &&
       v1 != DATE_SHOULDNT_CARE  &&  v2 != DATE_SHOULDNT_CARE  &&  v1 > v2 )
      return true;

   // we've weeded out the don't care case and already tested for greather than...  if it failed...
   // the next best we can do is equal or less than... in either case, the greater than test failed
   // so we should just return false

   return false;
*/
}


bool ValuesLess( int v1, int v2 )
{
   // True if either value is "ignore", or if v1 < v2
   return (v1 == DATE_IGNORE_ON_INPUT) || (v2 == DATE_IGNORE_ON_INPUT) || (v1 < v2);
/*
   if ( v1 != DATE_DONT_CARE  &&  v2 != DATE_DONT_CARE  &&
       v1 != DATE_SHOULDNT_CARE  &&  v2 != DATE_SHOULDNT_CARE  &&  v1 < v2 )
      return true;

   // we've weeded out the don't care case and already tested for less than...  if it failed...
   // the next best we can do is equal or greater... in either case, the less than test failed
   // so we should just return false

   return false;
*/
}


bool BACnetDate::operator ==( const BACnetDate & arg )
{
   // test all of the values... if one is out of sorts... then the whole thing isn't equal

   if ( !ValuesEqual(year, arg.year) )
      return false;

   if ( !ValuesEqual(month, arg.month) )
      return false;

   if ( !ValuesEqual(day, arg.day) )
      return false;

   if ( !ValuesEqual(dayOfWeek, arg.dayOfWeek) )
      return false;

   return true;
}


bool BACnetDate::operator !=( const BACnetDate & arg )
{
   // test all values for equality... if one is not equal... then the whole thing is not equal
   // and we don't have to keep testing...
   // We could probably just invert the == operator, but my brain is hurting

   if ( !ValuesEqual(year, arg.year) )
      return true;

   if ( !ValuesEqual(month, arg.month) )
      return true;

   if ( !ValuesEqual(day, arg.day) )
      return true;

   if ( !ValuesEqual(dayOfWeek, arg.dayOfWeek) )
      return true;

   return false;
}


bool BACnetDate::operator <=( const BACnetDate & arg )
{
   if ( !ValuesEqual(year, arg.year) )
      return ValuesLess(year, arg.year);

   // years are now equal or we don't care...
   if ( !ValuesEqual(month, arg.month) )
      return ValuesLess(month, arg.month);

   // years and months are equal or don't care... check day
   if ( !ValuesEqual(day, arg.day) )
      return ValuesLess(day, arg.day);

   // all important values are either equal or don't care... check final day of week
   if ( !ValuesEqual(dayOfWeek, arg.dayOfWeek) )
      return ValuesLess(dayOfWeek, arg.dayOfWeek);

   // values must be equal
   return true;
}


bool BACnetDate::operator <( const BACnetDate & arg )
{
   if ( !ValuesEqual(year, arg.year) )
      return ValuesLess(year, arg.year);

   // years are now equal or we don't care...
   if ( !ValuesEqual(month, arg.month) )
      return ValuesLess(month, arg.month);

   // years and months are equal or don't care... check day
   if ( !ValuesEqual(day, arg.day) )
      return ValuesLess(day, arg.day);

   // all important values are either equal or don't care... check final day of week
   if ( !ValuesEqual(dayOfWeek, arg.dayOfWeek) )
      return ValuesLess(dayOfWeek, arg.dayOfWeek);

   // values must be equal so return false
   return false;
}


bool BACnetDate::operator >=( const BACnetDate & arg )
{
   if ( !ValuesEqual(year, arg.year) )
      return ValuesGreater(year, arg.year);

   // years are now equal or we don't care...
   if ( !ValuesEqual(month, arg.month) )
      return ValuesGreater(month, arg.month);

   // years and months are equal or don't care... check day
   if ( !ValuesEqual(day, arg.day) )
      return ValuesGreater(day, arg.day);

   // all important values are either equal or don't care... check final day of week
   if ( !ValuesEqual(dayOfWeek, arg.dayOfWeek) )
      return ValuesGreater(dayOfWeek, arg.dayOfWeek);

   // values must be equal now
   return true;
}


bool BACnetDate::operator >( const BACnetDate & arg )
{
   if ( !ValuesEqual(year, arg.year) )
      return ValuesGreater(year, arg.year);

   // years are now equal or we don't care...
   if ( !ValuesEqual(month, arg.month) )
      return ValuesGreater(month, arg.month);

   // years and months are equal or don't care... check day
   if ( !ValuesEqual(day, arg.day) )
      return ValuesGreater(day, arg.day);

   // all important values are either equal or don't care... check final day of week
   if ( !ValuesEqual(dayOfWeek, arg.dayOfWeek) )
      return ValuesGreater(dayOfWeek, arg.dayOfWeek);

   // values must be equal so return false
   return false;
}


void BACnetDate::TestDateComps()
{
   bool f;
   int max = 22;
   BACnetDate date1, date2;
   char * t[] = {  "[WED, 11/20/02]", "[WED, 11/20/02]",       // test years
               "[WED, 11/20/02]", "[WED, 11/20/03]",
               "[WED, 11/20/03]", "[WED, 11/20/00]",
               "[WED, 11/20/?]", "[WED, 11/20/?]",
               "[WED, 11/20/03]", "[WED, 11/20/?]",
               "[WED, 11/20/*]", "[WED, 11/20/*]",
               "[WED, 11/20/03]", "[WED, 11/20/*]",
               "[WED, 11/20/*]", "[WED, 11/20/?]",

               "[WED, 11/20/02]", "[WED, 11/21/02]",        // test months
               "[WED, 11/21/02]", "[WED, 11/20/02]",
               "[WED, 11/?/02]", "[WED, 11/?/02]",
               "[WED, 11/?/02]", "[WED, 11/20/02]",
               "[WED, 11/?/02]", "[WED, 11/*/02]",
               "[WED, 11/*/02]", "[WED, 11/20/02]",
               "[WED, 11/*/02]", "[WED, 11/*/02]",

               "[WED, 11/20/02]", "[WED, 12/21/02]",        // test days
               "[WED, 12/20/02]", "[WED, 11/20/02]",
               "[WED, ?/20/02]", "[WED, ?/20/02]",
               "[WED, ?/20/02]", "[WED, 11/20/02]",
               "[WED, ?/20/02]", "[WED, */20/02]",
               "[WED, */20/02]", "[WED, 11/20/02]",
               "[WED, */20/02]", "[WED, */20/02]"
            };


   for ( int i = 0; i < max; i++ )
   {
      date1.Decode( (const char *) t[i*2] );
      date2.Decode( (const char *) t[(i*2)+1] );

      f = date1 == date2;
      f = date1 != date2;
      f = date1 <  date2;
      f = date1 <= date2;
      f = date1 >  date2;
      f = date1 >= date2;
   }
}


IMPLEMENT_DYNAMIC(BACnetTime, BACnetEncodeable)

//
// BACnetTime::BACnetTime
//
BACnetTime::BACnetTime( void )
{
   time_t      now;
   struct tm   *currtime;

   time( &now );
   currtime = localtime( &now );

   hour = currtime->tm_hour;
   minute = currtime->tm_min;
   second = currtime->tm_sec;
   hundredths = 0;
}

//
// BACnetTime::BACnetTime
//
BACnetTime::BACnetTime( int hr, int mn, int sc, int hun )
   : hour(hr), minute(mn), second(sc), hundredths(hun)
{
}


BACnetTime::BACnetTime( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


bool BACnetTime::IsValid()
{
   if ( hour != DATE_DONT_CARE && hour != DATE_IGNORE_ON_INPUT && (hour < 0 || hour > 23) )
      return false;

   if ( minute != DATE_DONT_CARE &&  minute != DATE_IGNORE_ON_INPUT  &&  (minute < 0 || minute > 59) )
      return false;

   if ( second != DATE_DONT_CARE &&  second != DATE_IGNORE_ON_INPUT &&  (second < 0 || second > 59 ))
      return false;

   if ( hundredths != DATE_DONT_CARE &&  hundredths != DATE_IGNORE_ON_INPUT &&  (hundredths < 0 ||  hundredths > 99))
      return false;

   return true;
}


void BACnetTime::Encode( BACnetAPDUEncoder& enc, int context )
{
   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, 4 ).Encode( enc );
   else
      BACnetAPDUTag( timeAppTag, 4 ).Encode( enc );
   
   // fill in the data
   // JLH: ended up encoding * (DATE_IGNORE_ON_INPUT) as 0xFE
   enc.pktBuffer[enc.pktLength++] = (hour < DATE_IGNORE_ON_INPUT) ? hour : 0xFF;
   enc.pktBuffer[enc.pktLength++] = (minute < DATE_IGNORE_ON_INPUT) ? minute : 0xFF;
   enc.pktBuffer[enc.pktLength++] = (second < DATE_IGNORE_ON_INPUT) ? second : 0xFF;
   enc.pktBuffer[enc.pktLength++] = (hundredths < DATE_IGNORE_ON_INPUT) ? hundredths : 0xFF;
}

void BACnetTime::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

   // verify the tag can be extracted
   tag.Decode( dec );
   
   // check the type and length
   if (!tag.tagClass && (tag.tagNumber != timeAppTag))
      throw_(53) /* mismatched data type */;
   if (tag.tagLVT != 4)
      throw_(54) /* four bytes of data expected */;
   
   // copy out the data
   hour     = *dec.pktBuffer++;
   minute      = *dec.pktBuffer++;
   second      = *dec.pktBuffer++;
   hundredths  = *dec.pktBuffer++;
   dec.pktLength -= 4;

   if ( !IsValid() )
      throw_(100);               // code meaning invalid time values, interpreted by caller's context
}

void BACnetTime::Encode( CString &enc, Format theFormat ) const
{
   char buff[10];

   // Wrapping is only required for current scripting,
   // and it is annoying in a dialog field.
   // So do it only as required.
   if (theFormat == FMT_SCRIPT)
   {
      enc = "[";
   }
   else
   {
      enc.Empty();
   }

   // hour
   if (hour == DATE_DONT_CARE)
      enc += '*';
   else if (hour == DATE_IGNORE_ON_INPUT)
      enc += '?';
   else {
      sprintf( buff, "%02d", hour );
      enc += buff;
   }
   enc += ':';

   // minute
   if (minute == DATE_DONT_CARE)
      enc += '*';
   else if (minute == DATE_IGNORE_ON_INPUT)
      enc += '?';
   else {
      sprintf( buff, "%02d", minute );
      enc += buff;
   }
   enc += ':';

   // second
   if (second == DATE_DONT_CARE)
      enc += '*';
   else if (second == DATE_IGNORE_ON_INPUT)
      enc += '?';
   else {
      sprintf( buff, "%02d", second );
      enc += buff;
   }
   enc += '.';

   // hundredths
   if (hundredths == DATE_DONT_CARE)
      enc += '*';
   else if (hundredths == DATE_IGNORE_ON_INPUT)
      enc += '?';
   else {
      sprintf( buff, "%02d", hundredths );
      enc += buff;
   }

   if (theFormat == FMT_SCRIPT)
   {
      enc += "]";
   }
}


void BACnetTime::Decode( const char *dec )
{
   // Defaults
   hour = minute = second = hundredths = DATE_DONT_CARE;

   // Allow [ wrapping, as when passed from a script
   bool isBracketed = IsChar( dec, '[' );

   // EPICS doesn't wrap time in (), but VTS used to.
   // We make it optional, for backward compatibility
   bool isParenned = IsChar( dec, '(' );

   // Check for hour
   if ( *dec == '*' )
   {
      hour = DATE_DONT_CARE;
      dec += 1;
   }
   else if ( *dec == '?' )
   {
      hour = DATE_IGNORE_ON_INPUT;
      dec += 1;
   }
   else
   {
      // dont' care not specified, they MUST specify hours...
      hour = -1;
      
      if ( IsDigit(*dec) )
         for (hour = 0; IsDigit(*dec); dec++)
            hour = (hour * 10) + (*dec - '0');

      // test validity and report
      if ( hour < 0 || hour > 23 )
         throw_(101);                           // invalid hour specification, interpreted by caller's context
   }
   // add by xlp
   while (*dec && IsSpace(*dec)) dec++;

   if (*dec == ':')
      dec += 1;
   else
      throw_(102);                              // bad time separator, (used to be 55 invalid character)

   // add by xlp
   while (*dec && IsSpace(*dec)) dec++;

   // check for minute
   if ( *dec == '*' ) 
   {
      minute = DATE_DONT_CARE;
      dec += 1;
   }
   else if ( *dec == '?' )
   {
      minute = DATE_IGNORE_ON_INPUT;
      dec += 1;
   }
   else
   {
      // MUST now supply minute value
      minute = -1;
      
      if ( IsDigit(*dec) )
         for (minute = 0; IsDigit(*dec); dec++)
            minute = (minute * 10) + (*dec - '0');

      // test validity and report
      if ( minute < 0 || minute > 59 )
         throw_(103);                           // invalid minute specification, interpreted by caller's context
   }
   // add by xlp
   while (*dec && IsSpace(*dec)) dec++;

   if (*dec == ':')
      dec += 1;
   else
      throw_(102);                              // bad time separator, (used to be 56 invalid character)

   // add by xlp
   while (*dec && IsSpace(*dec)) dec++;

   // check for second
   if ( *dec == '*' )
   {
      second = DATE_DONT_CARE;
      dec += 1;
   }
   else if ( *dec == '?' )
   {
      second = DATE_IGNORE_ON_INPUT;
      dec += 1;
   }
   else
   {
      // MUST now supply the second value
      second = -1;
      
      if ( IsDigit(*dec) )
         for (second = 0; IsDigit(*dec); dec++)
            second = (second * 10) + (*dec - '0');

      // test validity and report
      if ( second < 0 || second > 59 )
         throw_(104);                           // invalid second specification, interpreted by caller's context
   }
   // add by xlp
   while (*dec && IsSpace(*dec)) dec++;

   // hundredths specification is optional...
   if (*dec == '.')
   {
      dec += 1;

      // so we're now scanning for a valid hundredth number
      while (*dec && IsSpace(*dec)) dec++;

      if ( *dec == '*' )
      {
         hundredths = DATE_DONT_CARE;
         dec += 1;
      }
      else if ( *dec == '?' )
      {
         hundredths = DATE_IGNORE_ON_INPUT;
         dec += 1;
      }
      else
      {
         // MUST now supply the hundredth value
         hundredths = -1;
      
         if ( IsDigit(*dec) )
            for (hundredths = 0; IsDigit(*dec); dec++)
               hundredths = (hundredths * 10) + (*dec - '0');

         // test validity and report
         if ( hundredths < 0 || hundredths > 99 )
            throw_(105);                           // invalid hundredth specification, interpreted by caller's context
      }
   }

   // Close out () or [] wrapping
   EatWhite( dec );
   if (isParenned)
   {
      RequireChar( dec, ')' );
   }

   if (isBracketed)
   {
      RequireChar( dec, ']' );
   }
}


BACnetTime & BACnetTime::operator =( const BACnetTime & arg )
{
   hour = arg.hour;
   hundredths = arg.hundredths;
   minute = arg.minute;
   second = arg.second;

   return *this;
}


int BACnetTime::DataType() const
{
   return ptTime;
}

BACnetEncodeable * BACnetTime::clone()
{
   return new BACnetTime(hour, minute, second, hundredths);
}


bool BACnetTime::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetTime), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!Match((BACnetTime &) rbacnet, iOperator))
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}


bool BACnetTime::Match( BACnetTime & rtime, int iOperator )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetTime), rtime, iOperator, NULL))
      return isMatch;

   switch(iOperator)
   {
      case '=':   return *this == rtime;
      case '<':   return *this < rtime;
      case '>':   return *this > rtime;
      case '<=':  return *this <= rtime;
      case '>=':  return *this >= rtime;
      case '!=':  return *this != rtime;
      default:
         ASSERT(0);
   }
   return false;
}


bool BACnetTime::operator ==( const BACnetTime & arg )
{
   // test all of the values... if one is out of sorts... then the whole thing isn't equal

   if ( !ValuesEqual(hour, arg.hour) )
      return false;

   if ( !ValuesEqual(minute, arg.minute) )
      return false;

   if ( !ValuesEqual(second, arg.second) )
      return false;

   if ( !ValuesEqual(hundredths, arg.hundredths) )
      return false;

   return true;
}


bool BACnetTime::operator !=( const BACnetTime & arg )
{
   // test all values for equality... if one is not equal... then the whole thing is not equal
   // and we don't have to keep testing...

   if ( !ValuesEqual(hour, arg.hour) )
      return true;

   if ( !ValuesEqual(minute, arg.minute) )
      return true;

   if ( !ValuesEqual(second, arg.second) )
      return true;

   if ( !ValuesEqual(hundredths, arg.hundredths) )
      return true;

   return false;
}


bool BACnetTime::operator <=( const BACnetTime & arg )
{
   if ( !ValuesEqual(hour, arg.hour) )
      return ValuesLess(hour, arg.hour);

   // years are now equal or we don't care...
   if ( !ValuesEqual(minute, arg.minute) )
      return ValuesLess(minute, arg.minute);

   // years and months are equal or don't care... check day
   if ( !ValuesEqual(second, arg.second) )
      return ValuesLess(second, arg.second);

   // all important values are either equal or don't care... check final day of week
   if ( !ValuesEqual(hundredths, arg.hundredths) )
      return ValuesLess(hundredths, arg.hundredths);

   // values must be equal
   return true;
}


bool BACnetTime::operator <( const BACnetTime & arg )
{
   if ( !ValuesEqual(hour, arg.hour) )
      return ValuesLess(hour, arg.hour);

   // hours are now equal or we don't care...
   if ( !ValuesEqual(minute, arg.minute) )
      return ValuesLess(minute, arg.minute);

   // hours and minutes are equal or don't care... check second
   if ( !ValuesEqual(second, arg.second) )
      return ValuesLess(second, arg.second);

   // all important values are either equal or don't care... check final second of week
   if ( !ValuesEqual(hundredths, arg.hundredths) )
      return ValuesLess(hundredths, arg.hundredths);

   // values must be equal so return false
   return false;
}


bool BACnetTime::operator >=( const BACnetTime & arg )
{
   if ( !ValuesEqual(hour, arg.hour) )
      return ValuesGreater(hour, arg.hour);

   // hours are now equal or we don't care...
   if ( !ValuesEqual(minute, arg.minute) )
      return ValuesGreater(minute, arg.minute);

   // hours and minutes are equal or don't care... check second
   if ( !ValuesEqual(second, arg.second) )
      return ValuesGreater(second, arg.second);

   // all important values are either equal or don't care... check final second of week
   if ( !ValuesEqual(hundredths, arg.hundredths) )
      return ValuesGreater(hundredths, arg.hundredths);

   // values must be equal now
   return true;
}


bool BACnetTime::operator >( const BACnetTime & arg )
{
   if ( !ValuesEqual(hour, arg.hour) )
      return ValuesGreater(hour, arg.hour);

   // hours are now equal or we don't care...
   if ( !ValuesEqual(minute, arg.minute) )
      return ValuesGreater(minute, arg.minute);

   // hours and minutes are equal or don't care... check second
   if ( !ValuesEqual(second, arg.second) )
      return ValuesGreater(second, arg.second);

   // all important values are either equal or don't care... check final second of week
   if ( !ValuesEqual(hundredths, arg.hundredths) )
      return ValuesGreater(hundredths, arg.hundredths);

   // values must be equal so return false
   return false;
}


void BACnetTime::TestTimeComps()
{
   bool f;
   int max = 7;
   BACnetTime t1, t2;
   char * t[] = {  "10:11:12.99", "10:11:12.99",
               "10:11:12.99", "10:11:12.50",
               "10:11:12.99", "10:11:12.*",
               "10:11:12.99", "10:*:12.50",
               "10:?:12.99", "10:11:12.50",
               "10:*:12.99", "10:?:12.50",
               "?:11:12.99", "12:?:15.50"
            };


   for ( int i = 0; i < max; i++ )
   {
      t1.Decode( (const char *) t[i*2] );
      t2.Decode( (const char *) t[(i*2)+1] );

      f = t1 == t2;
      f = t1 != t2;
      f = t1 <  t2;
      f = t1 <= t2;
      f = t1 >  t2;
      f = t1 >= t2;
   }
}


//
// BACnetDateTime ===============================================
//
IMPLEMENT_DYNAMIC(BACnetDateTime, BACnetEncodeable)

BACnetDateTime::BACnetDateTime( void )
{
   // Current time, so CTime is just fine
   CTime ctime = CTime::GetCurrentTime();

   struct tm currtime;
   ctime.GetLocalTm( &currtime );

   bacnetTime.hour   = currtime.tm_hour;
   bacnetTime.minute = currtime.tm_min;
   bacnetTime.second = currtime.tm_sec;
   bacnetTime.hundredths = 0;
   bacnetDate.year  = currtime.tm_year;
   bacnetDate.month = currtime.tm_mon + 1;
   bacnetDate.day   = currtime.tm_mday;
   bacnetDate.CalcDayOfWeek();
}


BACnetDateTime::BACnetDateTime( int y, int m, int d, int hr, int mn, int sc, int hun )
            :bacnetDate(y,m,d), bacnetTime(hr,mn,sc,hun)
{
}

BACnetDateTime::BACnetDateTime( const BACnetDateTime &orig )
   :bacnetDate(orig.bacnetDate.year, orig.bacnetDate.month, orig.bacnetDate.day), 
    bacnetTime(orig.bacnetTime.hour, orig.bacnetTime.minute, orig.bacnetTime.second, orig.bacnetTime.hundredths )
{
}

BACnetDateTime::BACnetDateTime( const PICS::BACnetDateTime &orig )
   :bacnetDate(orig.date.year, orig.date.month, orig.date.day_of_month), 
    bacnetTime(orig.time.hour, orig.time.minute, orig.time.second, orig.time.hundredths  )
{
}

BACnetDateTime::BACnetDateTime( BACnetAPDUDecoder & dec )
            :bacnetDate(DATE_DONT_CARE,DATE_DONT_CARE,DATE_DONT_CARE), 
             bacnetTime(DATE_DONT_CARE,DATE_DONT_CARE,DATE_DONT_CARE,DATE_DONT_CARE)
{
   Decode(dec);
}


void BACnetDateTime::Encode( BACnetAPDUEncoder& enc, int context )
{
   // TODO: this is only proper for context = primitive.
   // Otherwise, should WRAP the date and time in open/close context tag.
   // But Decode won't handle wrapping.
   bacnetDate.Encode(enc, context);
   bacnetTime.Encode(enc, context);
}


void BACnetDateTime::Decode( BACnetAPDUDecoder& dec )
{
   bacnetDate.Decode(dec);
   bacnetTime.Decode(dec);
}


void BACnetDateTime::Encode( CString &enc, Format theFormat ) const
{
   CString str;
   // DateTime is complex data structure... must use brackets to enclose data
   enc = '{';
   bacnetDate.Encode(str, theFormat);
   enc += str;
   enc += ", ";
   bacnetTime.Encode(str, theFormat);
   enc += str;
   enc += '}';
}


void BACnetDateTime::Decode( const char *dec )
{
   // skip blank on front
   while (*dec && IsSpace(*dec)) dec++;

   // Whole thing  is complex data type so must enclose in brackets
   if ( *dec++ != '{' ) // LJT changed from [ to {
      throw_(110);         // missing start bracket for complex data structures

   bacnetDate.Decode(dec);

   // skip blank on front
   while (*dec && IsSpace(*dec)) dec++;

   // skip over comma and more space
   if (*dec++ != ',')
      throw_(120);         // missing comma

   bacnetTime.Decode(dec);

   // clear white space and look for close bracket
   while (*dec && IsSpace(*dec)) dec++;
   if ( *dec++ != '}' ) // LJT changed from ] to }
      throw_(111);                           // missing close bracket code
}


BACnetDateTime & BACnetDateTime::operator =( const BACnetDateTime & arg )
{
   bacnetDate = arg.bacnetDate;
   bacnetTime = arg.bacnetTime;
   return *this;
}


int BACnetDateTime::DataType() const
{
   return dt;
}

BACnetEncodeable * BACnetDateTime::clone()
{
   BACnetDateTime * pdatetime = new BACnetDateTime();
   *pdatetime = *this;
   return pdatetime;
}


bool BACnetDateTime::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetDateTime), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!Match((BACnetDateTime &) rbacnet, iOperator))
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}

bool BACnetDateTime::Match( BACnetDateTime & rdatetime, int iOperator )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetDateTime), rdatetime, iOperator, NULL))
      return isMatch;

   switch(iOperator)
   {
      case '=':   return *this == rdatetime;
      case '<':   return *this < rdatetime;
      case '>':   return *this > rdatetime;
      case '<=':  return *this <= rdatetime;
      case '>=':  return *this >= rdatetime;
      case '!=':  return *this != rdatetime;
      default:
         ASSERT(0);
   }
   return false;
}


bool BACnetDateTime::operator ==( const BACnetDateTime & arg )
{
   return bacnetDate == arg.bacnetDate  &&  bacnetTime == arg.bacnetTime;
}

// changed comparison to use || from && per AWood@nz.schneider-electric.com for #1244867 8/31/2005
bool BACnetDateTime::operator !=( const BACnetDateTime & arg )
{
   return bacnetDate != arg.bacnetDate  ||  bacnetTime != arg.bacnetTime;
}


bool BACnetDateTime::operator <=( const BACnetDateTime & arg )
{
   return bacnetDate < arg.bacnetDate  || (bacnetDate == arg.bacnetDate && bacnetTime <= arg.bacnetTime);
}


bool BACnetDateTime::operator <( const BACnetDateTime & arg )
{
   return bacnetDate < arg.bacnetDate  || (bacnetDate == arg.bacnetDate && bacnetTime < arg.bacnetTime);
}


bool BACnetDateTime::operator >=( const BACnetDateTime & arg )
{
   return bacnetDate > arg.bacnetDate  || (bacnetDate == arg.bacnetDate && bacnetTime >= arg.bacnetTime);
}


bool BACnetDateTime::operator >( const BACnetDateTime & arg )
{
   return bacnetDate > arg.bacnetDate  || (bacnetDate == arg.bacnetDate && bacnetTime > arg.bacnetTime);
}


//
// BACnetDateTime ===============================================
//
IMPLEMENT_DYNAMIC(BACnetDateRange, BACnetEncodeable)

BACnetDateRange::BACnetDateRange( void )
{
}


BACnetDateRange::BACnetDateRange( int y, int m, int d, int y2, int m2, int d2 )
            :bacnetDateStart(y,m,d), bacnetDateEnd(y2, m2, d2)
{
}


BACnetDateRange::BACnetDateRange( BACnetAPDUDecoder& dec )
//          :bacnetDateStart(dec), bacnetDateEnd(dec)    // can't guarantee calling order, but we need it :)
{
   Decode(dec);
}


void BACnetDateRange::Encode( BACnetAPDUEncoder& enc, int context )
{
   // TODO: this is only proper for context = primitive.
   // Otherwise, should WRAP the dates in open/close context tag.
   // But Decode won't handle wrapping.
   bacnetDateStart.Encode(enc, context);
   bacnetDateEnd.Encode(enc, context);
}


void BACnetDateRange::Decode( BACnetAPDUDecoder& dec )
{
   bacnetDateStart.Decode(dec);
   bacnetDateEnd.Decode(dec);
}


void BACnetDateRange::Encode( CString &enc, Format theFormat ) const
{
   CString str;
   // DateRange is complex data structure... must use brackets to enclose data
   enc = '{';

   bacnetDateStart.Encode(str, theFormat);
   enc += str;
   enc += ", ";
   bacnetDateEnd.Encode(str, theFormat);
   enc += str;

   enc += '}';
}


void BACnetDateRange::Decode( const char *dec )
{
   // skip blank on front
   while (*dec && IsSpace(*dec)) dec++;

   // Whole thing  is complex data type so must enclose in brackets
   if ( *dec++ != '{' ) // LJT changed from [ to {
      throw_(110);         // missing start bracket for complex data structures

   bacnetDateStart.Decode(dec);

   // skip blank on front
   while (*dec && IsSpace(*dec)) dec++;

   // skip over comma and more space
   if (*dec++ != ',')
      throw_(120);         // missing comma

   bacnetDateEnd.Decode(dec);

   // clear white space and look for close bracket
   while (*dec && IsSpace(*dec)) dec++;
   if ( *dec++ != '}' ) // LJT changed from ] to }
      throw_(111);                           // missing close bracket code
}


int BACnetDateRange::DataType() const
{
   return dtrange;
}

BACnetEncodeable * BACnetDateRange::clone()
{
   BACnetDateRange * pdaterange = new BACnetDateRange();
   *pdaterange = *this;
   return pdaterange;
}


bool BACnetDateRange::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   // TODO: Comparisons for < > etc. are based on the NUMBER OF DAYS in ther
   // respective DateRanges.  I am skeptical that this is useful, but...
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetDateRange), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!Match((BACnetDateRange &) rbacnet, iOperator))
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);

   return true;
}


BACnetDateRange & BACnetDateRange::operator =( const BACnetDateRange & arg )
{
   bacnetDateEnd = arg.bacnetDateEnd;
   bacnetDateStart = arg.bacnetDateStart;
   return *this;
}


// See comments near calls of GetSpan on why I think this
// is silly.
LONGLONG BACnetDateRange::GetSpan() const
{
   return (bacnetDateEnd.AsInt() - bacnetDateStart.AsInt());
}


bool BACnetDateRange::Match( BACnetDateRange & rdaterange, int iOperator )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetDateRange), rdaterange, iOperator, NULL))
      return isMatch;

   switch(iOperator)
   {
      case '=':   return *this == rdaterange;
      case '<':   return *this < rdaterange;
      case '>':   return *this > rdaterange;
      case '<=':  return *this <= rdaterange;
      case '>=':  return *this >= rdaterange;
      case '!=':  return *this != rdaterange;
      default:
         ASSERT(0);
   }
   return false;
}


bool BACnetDateRange::operator ==( const BACnetDateRange & arg )
{
   return (bacnetDateStart == arg.bacnetDateStart) && (bacnetDateEnd == arg.bacnetDateEnd);
}


bool BACnetDateRange::operator !=( const BACnetDateRange & arg )
{
   return (bacnetDateStart != arg.bacnetDateStart) || (bacnetDateEnd != arg.bacnetDateEnd);
}

// TODO: these comparisons are silly:
// if arg1 = (Jan 1 2010 - Jan 2 2010) and arg2 = (Jan 1 1980 to Jan 10 1980),
// span1 is 1 day, and span2 is 9 days.  But in what sense is arg1 LESS THAN arg2?
// But maybe someone has a use, so we won't remove it now.
// Updated to work with non-CTime implementation.
bool BACnetDateRange::operator <=( const BACnetDateRange & arg )
{
   return (GetSpan() <= arg.GetSpan());
}


bool BACnetDateRange::operator <( const BACnetDateRange & arg )
{
   return GetSpan() < arg.GetSpan();
}


bool BACnetDateRange::operator >=( const BACnetDateRange & arg )
{
   return GetSpan() >= arg.GetSpan();
}


bool BACnetDateRange::operator >( const BACnetDateRange & arg )
{
   return GetSpan() > arg.GetSpan();
}


IMPLEMENT_DYNAMIC(BACnetObjectIdentifier, BACnetEncodeable)

//
// BACnetObjectIdentifier::BACnetObjectIdentifier
//

BACnetObjectIdentifier::BACnetObjectIdentifier( int objType, int instanceNum )
   : objID( (objType << 22) + instanceNum )
{
}


BACnetObjectIdentifier::BACnetObjectIdentifier( unsigned int nobjID )
                  :objID(nobjID)
{
}


BACnetObjectIdentifier::BACnetObjectIdentifier( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

BACnetObjectIdentifier::BACnetObjectIdentifier( const BACnetObjectIdentifier& objectId )  // Jingbo Gao, Sep 11, 2004
:objID(objectId.objID)
{
}

//
// BACnetObjectIdentifier::SetValue
//

void BACnetObjectIdentifier::SetValue( BACnetObjectType objType, int instanceNum )
{
   objID = ((int)objType << 22) + instanceNum;
}

void BACnetObjectIdentifier::Encode( BACnetAPDUEncoder& enc, int context )
{
   // encode the tag
   if (context != kAppContext)
      BACnetAPDUTag( context, 4 ).Encode( enc );
   else
      BACnetAPDUTag( objectIdentifierAppTag, 4 ).Encode( enc );
   
   // fill in the data
#ifdef ENDIAN_SWAP
   for (int j = 3; j >= 0; j--)
      enc.pktBuffer[enc.pktLength++] = (objID >> (j * 8)) & 0xFF;
#else
   memcpy( enc.pktBuffer+enc.pktLength, &objID, (size_t)4 );
   enc.pktLength += 4;
#endif
}

void BACnetObjectIdentifier::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag  tag;

   // verify the tag can be extracted
   tag.Decode( dec );

   // check the type and length
   if (!tag.tagClass && (tag.tagNumber != objectIdentifierAppTag))
      throw_(60) /* mismatched data type */;
   if (tag.tagLVT != 4)
      throw_(61) /* four bytes of data expected */;

   // copy out the data
#ifdef ENDIAN_SWAP
   for (int j = 3; dec.pktLength && j >= 0; j--)
      objID = (objID << 8) + (dec.pktLength--,*dec.pktBuffer++);
#else
   memcpy( &objID, dec.pktBuffer, (size_t)4 );
   dec.pktBuffer += 4;
   dec.pktLength -= 4;
#endif
}

void BACnetObjectIdentifier::Encode( CString &enc, Format theFormat ) const
{
   int objType = (objID >> 22);
   int instanceNum = (objID & 0x003FFFFF);
   const char *s;

#if VTSScanner
   s = NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString( objType );
#else
   Hey! Do we really need to do this without strings?
#endif

   // This is a mess:
   // - EPICS format says to wrap objectID in parenthesis.
   // - But the script parser takes the parenthesis as tokens, and won't accept an objectID in parenthesis.
   //   For Bitstring, Time and Date, scripting requires [] wrapping to deal with this.  But for
   //   whatever reason, VTS has historically NOT [] wrapped Object Identifiers.
   // - This also means a script can't use >> to set a variable with an objectID read from a target device.
   //   TODO: what does "this" refer to here?
   // - The code here has changed back and forth: with and without:
   //   "changed this back (with parenthesis) because it broke the Send Dialog VTSANY entry of ObjId 3/10/2006"
   //
   // So let's wrap the way the customer wants it
   if (theFormat == FMT_SCRIPT)
   {
//    enc.Format( "[(%s, %d)]", s, instanceNum );  // if we wanted to be like Date
      enc.Format( "%s, %d", s, instanceNum );      // your VTS heritage
   }
   else if (theFormat == FMT_EPICS)
   {
      enc.Format( "(%s, %d)", s, instanceNum );
   }
   else
   {
      enc.Format( "%s, %d", s, instanceNum );
   }
}

#if VTSScanner
void BACnetObjectIdentifier::Decode( const char *dec )
{
   int      objType, instanceNum;

   // Allow [ wrapping, as when passed from a script
   // This doesn't (yet) work, as [] causes our caller's parser to find
   // ONE token rather than TWO (type and instance).
   bool isBracketed = IsChar( dec, '[' );

   // Object Identifier is complex data type so must enclose in parenthesis for EPICS format.
   // We make it optional, for convenient use in dialogs etc.
   bool isParenned = IsChar( dec, '(' );

   // create a scanner bound to the text
   ScriptScanner  scan( dec );
   ScriptToken    tok;
   scan.Next( tok );
   if ((tok.tokenType == scriptKeyword) && (tok.tokenSymbol == kwRESERVED))
   {
      // next must be a number in the reserved range
      scan.Next( tok );
      if (!tok.IsInteger( objType ))
         throw_(62) /* integer expected */;
      if ((objType < 0) || (objType >= 128))
         throw_(63) /* out of range */;
   }
   else if ((tok.tokenType == scriptKeyword) && (tok.tokenSymbol == kwVENDOR))
   {
      // next must be a number in the vendor range
      scan.Next( tok );
      if (!tok.IsInteger( objType ))
         throw_(64) /* integer expected */;
      if ((objType < 128) || (objType >= (1 << 10)))
         throw_(65) /* out of range */;
   } 
   else if (!tok.IsInteger( objType, ScriptObjectTypeMap ))
   {
      throw_(66) /* object type keyword expected */;
   }

   if ((objType < 0) || (objType >= (1 << 10)))
   {
      throw_(67) /* out of range */;
   }

   // get the next token
   scan.Next( tok );

   // skip the ',' if it was entered
   if ((tok.tokenType == scriptSymbol) && (tok.tokenSymbol == ','))
   {
      scan.Next( tok );
   }

   // make sure it's an integer
   if (!tok.IsInteger( instanceNum ))
      throw_(68) /* instance expected */;
   if ((instanceNum < 0) || (instanceNum >= (1 << 22)))
      throw_(69) /* out of range */;

   // everything checks out
   objID = (objType << 22) + instanceNum;

   // Close out () or [] wrapping
   EatWhite( scan.scanSrc );
   if (isParenned)
   {
      RequireChar( scan.scanSrc, ')' );
   }

   if (isBracketed)
   {
      RequireChar( scan.scanSrc, ']' );
   }
}
#else
void BACnetObjectIdentifier::Decode( const char * )
{
   // TODO: LJT implement this ...
   throw_(70) /* not implemented */;
}
#endif


int BACnetObjectIdentifier::DataType() const
{
   return ob_id;
}

BACnetEncodeable * BACnetObjectIdentifier::clone()
{
   return new BACnetObjectIdentifier(objID);
}


bool BACnetObjectIdentifier::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   // It may be marginally useful to say that (analog-input, 1) is "<" (analog-input, 2),
   // but saying (analog-input, 1) is "<" (analog-output, 1) seems dubious.  But...
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetObjectIdentifier), rbacnet, iOperator, pstrError))
      return isMatch;

   if (!::Match(iOperator, (unsigned long) objID, (unsigned long) ((BACnetObjectIdentifier &) rbacnet).objID ))
   {
      return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);
   }

   return true;
}


BACnetObjectIdentifier & BACnetObjectIdentifier::operator =( const BACnetObjectIdentifier &arg )
{
   objID = arg.objID;
   return *this;
}


//==============================================================================

IMPLEMENT_DYNAMIC(BACnetObjectPropertyReference, BACnetEncodeable)

BACnetObjectPropertyReference::BACnetObjectPropertyReference( unsigned int obj_id, unsigned int prop_id, int index /* = -1 */)
: m_objID(obj_id)
, m_bacnetenumPropID(prop_id, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier)
, m_nIndex(index)
{
}

BACnetObjectPropertyReference::BACnetObjectPropertyReference( BACnetAPDUDecoder & dec )
: m_bacnetenumPropID(0, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier)
{
   Decode(dec);
}

void BACnetObjectPropertyReference::Encode( BACnetAPDUEncoder& enc, int context )
{
   if ( context != kAppContext )
      BACnetOpeningTag().Encode(enc, context);

   m_objID.Encode(enc, 0);
   m_bacnetenumPropID.Encode(enc, 1);

   if ( m_nIndex != -1 && m_nIndex != NotAnArray)
      BACnetUnsigned(m_nIndex).Encode(enc, 2);

   if ( context != kAppContext )
      BACnetClosingTag().Encode(enc, context);
}


void BACnetObjectPropertyReference::Decode(BACnetAPDUDecoder& dec)
{
   m_objID.Decode(dec);
   m_bacnetenumPropID.Decode(dec);

   m_nIndex = -1;
   BACnetAPDUTag     tagTestType;
   dec.ExamineTag(tagTestType);
   if ( tagTestType.tagClass == contextTagClass  &&  tagTestType.tagNumber == 2 )
      m_nIndex = BACnetUnsigned(dec).uintValue;
}


void BACnetObjectPropertyReference::Encode( CString &enc, Format theFormat ) const
{
   CString obj;
   CString prop;

   m_objID.Encode(obj, theFormat);
   m_bacnetenumPropID.Encode(prop, theFormat);

   if ((m_nIndex >= 0) && (m_nIndex != NotAnArray))
   {
      enc.Format( "{%s, %s, %u}", (LPCTSTR)obj, (LPCTSTR)prop, m_nIndex );
   }
   else
   {
      enc.Format( "{%s, %s}", (LPCTSTR)obj, (LPCTSTR)prop );
   }
}


int BACnetObjectPropertyReference::DataType() const
{
   return propref;
}


BACnetEncodeable * BACnetObjectPropertyReference::clone()
{
   return new BACnetObjectPropertyReference(m_objID.objID, (unsigned int) m_bacnetenumPropID.m_enumValue, m_nIndex);
}


bool BACnetObjectPropertyReference::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetObjectPropertyReference), rbacnet, iOperator, pstrError))
      return isMatch;

   isMatch = (m_objID.objID == ((BACnetObjectPropertyReference &)rbacnet).m_objID.objID) &&
             (m_bacnetenumPropID.m_enumValue == ((BACnetObjectPropertyReference &)rbacnet).m_bacnetenumPropID.m_enumValue);
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


BACnetObjectPropertyReference & BACnetObjectPropertyReference::operator =( const BACnetObjectPropertyReference &arg )
{
   m_objID = arg.m_objID;
   m_bacnetenumPropID = arg.m_bacnetenumPropID;
   m_nIndex = arg.m_nIndex;
   return *this;
}


//==============================================================================

IMPLEMENT_DYNAMIC(BACnetDeviceObjectPropertyReference, BACnetEncodeable)

BACnetDeviceObjectPropertyReference::BACnetDeviceObjectPropertyReference( unsigned int obj_id, unsigned int prop_id, int index /* = -1 */, unsigned int devobj_id /* = 0xFFFFFFFF */ )
: m_objpropref(obj_id, prop_id, index)
, m_unObjectID(devobj_id)
{
}


BACnetDeviceObjectPropertyReference::BACnetDeviceObjectPropertyReference( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


void BACnetDeviceObjectPropertyReference::Encode( BACnetAPDUEncoder& enc, int context )
{
   if ( context != kAppContext )
      BACnetOpeningTag().Encode(enc, context);

   m_objpropref.Encode(enc);

   if ( m_unObjectID != 0xFFFFFFFF )
      ::BACnetObjectIdentifier((unsigned int) m_unObjectID).Encode(enc, 3);

   if ( context != kAppContext )
      BACnetClosingTag().Encode(enc, context);
}


void BACnetDeviceObjectPropertyReference::Decode(BACnetAPDUDecoder& dec)
{
   m_objpropref.Decode(dec);

   BACnetAPDUTag     tagTestType;
   dec.ExamineTag(tagTestType);

   m_unObjectID = 0xFFFFFFFF;
   if ( tagTestType.tagClass == contextTagClass  &&  tagTestType.tagNumber == 3 )
      m_unObjectID = BACnetObjectIdentifier(dec).objID;
}


void BACnetDeviceObjectPropertyReference::Encode( CString &enc, Format theFormat ) const
{
   CString obj;
   CString prop;

   m_objpropref.m_objID.Encode(obj, theFormat);
   m_objpropref.m_bacnetenumPropID.Encode(prop, theFormat);

   if ((m_objpropref.m_nIndex != -1) && (m_objpropref.m_nIndex != NotAnArray))
   {
      enc.Format( "{%s, %s, %u", (LPCTSTR)obj, (LPCTSTR)prop, m_objpropref.m_nIndex );
   }
   else
   {
      enc.Format( "{%s, %s", (LPCTSTR)obj, (LPCTSTR)prop );
   }

   if (m_unObjectID != 0xFFFFFFFF)
   {
      CString device;
      BACnetObjectIdentifier((unsigned int) m_unObjectID).Encode(device, theFormat);
      enc += ", ";
      enc += device;
   }

   enc += '}';
}

int BACnetDeviceObjectPropertyReference::DataType() const
{
   return devobjpropref;
}


BACnetEncodeable * BACnetDeviceObjectPropertyReference::clone()
{
   return new BACnetDeviceObjectPropertyReference(m_objpropref.m_objID.objID, (unsigned int) m_objpropref.m_bacnetenumPropID.m_enumValue, m_objpropref.m_nIndex, m_unObjectID);
}


bool BACnetDeviceObjectPropertyReference::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetDeviceObjectPropertyReference), rbacnet, iOperator, pstrError))
      return isMatch;

   // Compare for equality of the parts, do not log mismatch (since we may be checking INEQUALITY)
   isMatch = m_objpropref.Match(((BACnetDeviceObjectPropertyReference&)rbacnet).m_objpropref, '=', NULL) &&
             (m_unObjectID == ((BACnetDeviceObjectPropertyReference&)rbacnet).m_unObjectID);
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


BACnetDeviceObjectPropertyReference & BACnetDeviceObjectPropertyReference::operator =( const BACnetDeviceObjectPropertyReference &arg )
{
   m_objpropref = arg.m_objpropref;
   m_unObjectID = arg.m_unObjectID;
   return *this;
}

//==============================================================================


IMPLEMENT_DYNAMIC(BACnetDeviceObjectReference, BACnetEncodeable)

BACnetDeviceObjectReference::BACnetDeviceObjectReference( unsigned int obj_id, unsigned int devobj_id /* = 0xFFFFFFFF */ )
                        : m_objID(obj_id)
{
   m_undevID = devobj_id;
}


BACnetDeviceObjectReference::BACnetDeviceObjectReference( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


void BACnetDeviceObjectReference::Encode( BACnetAPDUEncoder& enc, int context )
{
   if ( context != kAppContext )
      BACnetOpeningTag().Encode(enc, context);

   if ( m_undevID != 0xFFFFFFFF )
      ::BACnetObjectIdentifier((unsigned int) m_undevID).Encode(enc, 0);

   m_objID.Encode(enc, 1);

   if ( context != kAppContext )
      BACnetClosingTag().Encode(enc, context);
}


void BACnetDeviceObjectReference::Decode(BACnetAPDUDecoder& dec)
{
   BACnetAPDUTag     tagTestType;   
   dec.ExamineTag(tagTestType);
   
   if ( tagTestType.tagClass == contextTagClass  &&  tagTestType.tagNumber == 0 )
      m_undevID = BACnetObjectIdentifier(dec).objID;

   m_objID.Decode(dec);

}


void BACnetDeviceObjectReference::Encode( CString &enc, Format theFormat ) const
{
   CString obj;
   CString device;

   enc = '{';
   if (m_undevID != 0xFFFFFFFF)
   {
      BACnetObjectIdentifier((unsigned int) m_undevID).Encode(device, theFormat);
      enc += device;
      enc += ", ";
   }
   m_objID.Encode(obj, theFormat);
   enc += obj;
   enc += '}';
}


int BACnetDeviceObjectReference::DataType() const
{
   return devobjref;
}


BACnetEncodeable * BACnetDeviceObjectReference::clone()
{
   return new BACnetDeviceObjectReference(m_objID.objID, m_undevID);
}


bool BACnetDeviceObjectReference::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetDeviceObjectReference), rbacnet, iOperator, pstrError))
      return isMatch;

   isMatch = (m_objID.objID == ((BACnetDeviceObjectReference&)rbacnet).m_objID.objID) &&
             (m_undevID == ((BACnetDeviceObjectReference&)rbacnet).m_undevID);
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


BACnetDeviceObjectReference & BACnetDeviceObjectReference::operator =( const BACnetDeviceObjectReference &arg )
{
   m_objID = arg.m_objID;
   m_undevID = arg.m_undevID;
   return *this;
}

//==============================================================================

IMPLEMENT_DYNAMIC(BACnetReadAccessSpecification, BACnetEncodeable)

BACnetReadAccessSpecification::BACnetReadAccessSpecification()
{
   m_fDataInvalid = true;
}

BACnetReadAccessSpecification::BACnetReadAccessSpecification( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

int BACnetReadAccessSpecification::DataType(void) const
{
   return raslist;
}

//==============================================================================

IMPLEMENT_DYNAMIC(BACnetActionCommand, BACnetEncodeable)

BACnetActionCommand::BACnetActionCommand()
{
   m_fDataInvalid = true;
}

BACnetActionCommand::BACnetActionCommand( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

int BACnetActionCommand::DataType(void) const
{
   return act;
}

//==============================================================================

IMPLEMENT_DYNAMIC(BACnetEventParameter, BACnetEncodeable)

BACnetEventParameter::BACnetEventParameter()
{
   m_fDataInvalid = true;
}

BACnetEventParameter::BACnetEventParameter( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

int BACnetEventParameter::DataType(void) const
{
   return evparm;
}

//==============================================================================

IMPLEMENT_DYNAMIC(BACnetSessionKey, BACnetEncodeable)

BACnetSessionKey::BACnetSessionKey()
{
   m_fDataInvalid = true;
}

BACnetSessionKey::BACnetSessionKey( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

int BACnetSessionKey::DataType(void) const
{
   return skeys;
}

//==============================================================================

IMPLEMENT_DYNAMIC(BACnetExceptionSchedule, BACnetEncodeable)

BACnetExceptionSchedule::BACnetExceptionSchedule()
{
   m_fDataInvalid = true;
}

BACnetExceptionSchedule::BACnetExceptionSchedule( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

int BACnetExceptionSchedule::DataType(void) const
{
   return xsched;
}

//==============================================================================

IMPLEMENT_DYNAMIC(BACnetTimeValue, BACnetEncodeable)

BACnetTimeValue::BACnetTimeValue()
{
   m_fDataInvalid = true;
}

BACnetTimeValue::BACnetTimeValue( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

int BACnetTimeValue::DataType(void) const
{
   return wsched;
}

//==============================================================================

IMPLEMENT_DYNAMIC(BACnetVTSession, BACnetEncodeable)

BACnetVTSession::BACnetVTSession()
{
   m_fDataInvalid = true;
}

BACnetVTSession::BACnetVTSession( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

int BACnetVTSession::DataType(void) const
{
   return vtse;
}


//==============================================================================
IMPLEMENT_DYNAMIC(BACnetAddressBinding, BACnetEncodeable)

BACnetAddressBinding::BACnetAddressBinding()
{
}

BACnetAddressBinding::BACnetAddressBinding( unsigned int nobjID, unsigned short nNet, BACnetOctet * paMAC, unsigned short nMACLen )
                 :m_bacnetObjectID(nobjID), m_bacnetAddr(nNet, paMAC, nMACLen)
{
}


BACnetAddressBinding::BACnetAddressBinding( BACnetAPDUDecoder& dec )
//              :m_bacnetObjectID(dec), m_bacnetAddr(dec)            // can't guarantee calling order this way
{
   Decode(dec);
}


void BACnetAddressBinding::Encode( BACnetAPDUEncoder& enc, int context )
{
   m_bacnetObjectID.Encode(enc, context);
   m_bacnetAddr.Encode(enc, context);
}

void BACnetAddressBinding::Encode( CString &enc, Format theFormat ) const
{
   CString str;
   enc = '{';
   m_bacnetObjectID.Encode( str, theFormat );
   enc += str;
   enc += ", ";
   m_bacnetAddr.Encode( str, theFormat );
   enc += str;
   enc += '}';
}

void BACnetAddressBinding::Decode( BACnetAPDUDecoder& dec )
{
   m_bacnetObjectID.Decode(dec);
   m_bacnetAddr.Decode(dec);
}

BACnetAddressBinding & BACnetAddressBinding::operator =( const BACnetAddressBinding & arg )
{
   m_bacnetObjectID = arg.m_bacnetObjectID;
   m_bacnetAddr = arg.m_bacnetAddr;
   return *this;
}


int BACnetAddressBinding::DataType() const
{
   return dabindelem;
}

BACnetEncodeable * BACnetAddressBinding::clone()
{
   BACnetAddressBinding * paddrbind = new BACnetAddressBinding();
   *paddrbind = *this;
   return paddrbind;
}


bool BACnetAddressBinding::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetAddressBinding), rbacnet, iOperator, pstrError))
      return isMatch;

   // Compare for equality of the parts, do not log mismatch (since we may be checking INEQUALITY)
   isMatch = m_bacnetObjectID.Match(((BACnetAddressBinding &) rbacnet).m_bacnetObjectID, '=', NULL ) &&
             m_bacnetAddr.Match(((BACnetAddressBinding &) rbacnet).m_bacnetAddr, '=', NULL );
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


//
// BACnetOpeningTag
//
void BACnetOpeningTag::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc = '{';
}

void BACnetOpeningTag::Encode( BACnetAPDUEncoder& enc, int context )
{
   if (context < 15) {
      enc.CheckSpace( 1 );
      enc.pktBuffer[enc.pktLength++] = ((context & 0x0F) << 4) + 0x0E;
   } else {
      enc.CheckSpace( 2 );
      enc.pktBuffer[enc.pktLength++] = 0xFE;
      enc.pktBuffer[enc.pktLength++] = (context & 0xFF);
   }
}

void BACnetOpeningTag::Decode( BACnetAPDUDecoder& dec )
{
   BACnetOctet tag;

   // enough for the tag byte?
   if (dec.pktLength < 1)
      throw_(71) /* not enough data */;

   tag = (dec.pktLength--,*dec.pktBuffer++);

   // check the type
   if ((tag & 0x0F) != 0x0E)
      throw_(72) /* mismatched tag class */;

   // check for a big context
   if ((tag & 0xF0) == 0xF0) {
      if (dec.pktLength < 1)
         throw_(73) /* not enough data */;
      dec.pktLength -= 1;
      dec.pktBuffer += 1;
   }
}

///////////////////////////////////////////////////////////////////////////////////
//
// BACnetDaysOfWeek

IMPLEMENT_DYNAMIC(BACnetDaysOfWeek, BACnetBitString)

BACnetDaysOfWeek::BACnetDaysOfWeek(void)
: BACnetBitString(7)
{
}

BACnetDaysOfWeek::BACnetDaysOfWeek(
      bool bMon, bool bTue, bool bWed,
      bool bThu, bool bFri, bool bSat, 
      bool bSun
      )
: BACnetBitString(7)
{
   SetBit(0, bMon);
   SetBit(1, bTue);
   SetBit(2, bWed);
   SetBit(3, bThu);
   SetBit(4, bFri);
   SetBit(5, bSat);
   SetBit(6, bSun);
}

void BACnetDaysOfWeek::StringToValue( const char *dec )
{
   static char *DaysNames[]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
   int i = 0;
   int count = 1;
   for( i = 0; i < 7; i++ )
   {
      SetBit( i, 0 );
   }
   bool bFound = false;
   while ( *dec != '\0' )
   {
      if ( count > 7 ) {
         break;
      }
      count++;
      while ( *dec == ' ' || *dec == ',' )
      {
         dec++;
      }
      for( i = 0; i < 7; i++ )
      {
         if ( _strnicmp( dec, DaysNames[ i ], 3 ) == 0 )
         {
            SetBit( i, 1 );
            bFound = true;
            while ( *dec != '\0' && *dec != ' ' && *dec != ',' ) {
               dec++;
            }
            break;
         }
      }
      if ( bFound == false ) {
         throw "There's a invalid character for BACnetDaysOfWeek's decoding.";
      }
   }
}
int BACnetDaysOfWeek::DataType() const
{
   return dsofweek;
}
//end of class BACnetDaysOfWeek

///////////////////////////////////////////////////////////////////////////////////
//
// BACnetEventTransitionBits
//
IMPLEMENT_DYNAMIC(BACnetEventTransitionBits, BACnetBitString)

BACnetEventTransitionBits::BACnetEventTransitionBits(void)
: BACnetBitString(3)
{
}

BACnetEventTransitionBits::BACnetEventTransitionBits(bool bOffnormal, bool bFault, bool bNormal)
: BACnetBitString(3)
{
   SetBit(0, bOffnormal);
   SetBit(1, bFault);
   SetBit(2, bNormal);
}
int BACnetEventTransitionBits::DataType() const
{
   return eventransbits;
}
//end of class BACnetEventTransitionBits

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//    BACnetRecipient

IMPLEMENT_DYNAMIC(BACnetRecipient, BACnetObjectContainer)

BACnetRecipient::BACnetRecipient(void)
{
}

BACnetRecipient::BACnetRecipient(const BACnetRecipient &src)
{
   *this = src;
}

BACnetRecipient::BACnetRecipient( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

BACnetRecipient::BACnetRecipient( BACnetEncodeable * pbacnetEncodeable )
               :BACnetObjectContainer(pbacnetEncodeable)
{
}

int BACnetRecipient::GetChoice()
{
   if(pbacnetTypedValue->IsKindOf(RUNTIME_CLASS(BACnetObjectIdentifier)))
      return 0;
   else return 1;
}

BACnetRecipient & BACnetRecipient::operator =( const BACnetRecipient & arg )
{
   this->SetObject(arg.pbacnetTypedValue->clone());
   return *this;
}

void BACnetRecipient::Encode( CString &enc, Format theFormat ) const
{
   int tagval;
   CString str;

   // TODO: LJT add context tag in square brackets for 135.1 compliance...
   //
   // JLH: could condition wrapping theFormat theFormat if desired
   if (pbacnetTypedValue->IsKindOf(RUNTIME_CLASS(BACnetObjectIdentifier)))
   {
      tagval = 0;
      pbacnetTypedValue->Encode( str, theFormat );
   }
   else
   {
      tagval = 1;
      pbacnetTypedValue->Encode( str, theFormat );
   }

   enc.Format( "[%d] %s", tagval, (LPCTSTR)str );
}

void BACnetRecipient::Encode(BACnetAPDUEncoder& enc, int context)
{
   if (pbacnetTypedValue == NULL)
      return;

   if ( context != kAppContext )
      BACnetOpeningTag().Encode(enc, context);

   if (pbacnetTypedValue->IsKindOf(RUNTIME_CLASS(BACnetObjectIdentifier)))
   {
      pbacnetTypedValue->Encode(enc, 0);
   }
   else
   {
      BACnetOpeningTag().Encode(enc,1);
      pbacnetTypedValue->Encode(enc);
      BACnetClosingTag().Encode(enc,1);
   }

   if ( context != kAppContext )
      BACnetClosingTag().Encode(enc, context);
}


void BACnetRecipient::Decode(BACnetAPDUDecoder& dec)
{
   BACnetAPDUTag     tagTestType;
   dec.ExamineTag(tagTestType);

   if (tagTestType.tagClass == openingTagClass)
   {
      BACnetOpeningTag().Decode(dec);
      SetObject( new BACnetAddr(dec) ); 
      BACnetClosingTag().Decode(dec);
   }
   else
   {
   // if(tagTestType.tagNumber == 0)
      SetObject( new BACnetObjectIdentifier(dec) );
/*
      else
      {  TRACE0("INVALID type in encoded stream for BACnetRecipient");
      ASSERT(0);
      }*/
   }

}


int BACnetRecipient::DataType() const
{
   return recip;
}

BACnetEncodeable * BACnetRecipient::clone()
{
   return new BACnetRecipient( pbacnetTypedValue == NULL ? (BACnetEncodeable *) NULL : pbacnetTypedValue->clone());
}


bool BACnetRecipient::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetRecipient), rbacnet, iOperator, pstrError))
      return isMatch;

   ASSERT(pbacnetTypedValue != NULL);

   return pbacnetTypedValue->Match(*((BACnetRecipient &)rbacnet).GetObject(), iOperator, pstrError);
}


/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(BACnetRecipientProcess, BACnetEncodeable)

BACnetRecipientProcess::BACnetRecipientProcess(void)
{
}


BACnetRecipientProcess::BACnetRecipientProcess( ::BACnetRecipient & rc, unsigned int nID )
                  :m_bacnetunsignedID(nID)
{
   m_bacnetrecipient = rc;
}


BACnetRecipientProcess::BACnetRecipientProcess( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


BACnetRecipientProcess & BACnetRecipientProcess::operator =( const BACnetRecipientProcess & arg )
{
   m_bacnetrecipient = arg.m_bacnetrecipient;
   m_bacnetunsignedID = arg.m_bacnetunsignedID;
   return *this;
}

void BACnetRecipientProcess::Encode( CString &enc, Format theFormat ) const
{
   CString str;
   enc = '{';
   m_bacnetrecipient.Encode( str, theFormat );
   enc += str;
   enc += ", ";
   m_bacnetunsignedID.Encode( str, theFormat );
   enc += str;
   enc += '}';
}

void BACnetRecipientProcess::Encode(BACnetAPDUEncoder& enc, int context)
{
   if ( context != kAppContext )
      BACnetOpeningTag().Encode(enc, context);

   m_bacnetrecipient.Encode(enc, 0);
   m_bacnetunsignedID.Encode(enc, 1);

   if ( context != kAppContext )
      BACnetClosingTag().Encode(enc, context);
}


void BACnetRecipientProcess::Decode(BACnetAPDUDecoder& dec)
{
   BACnetAPDUTag     tagTestType;   
   dec.ExamineTag(tagTestType);

// if (tagTestType.tagClass == openingTagClass)
//    BACnetOpeningTag().Decode(dec);

   m_bacnetrecipient.Decode(dec);
   m_bacnetunsignedID.Decode(dec);

// if (tagTestType.tagClass == openingTagClass)
//    BACnetClosingTag().Decode(dec);
}


int BACnetRecipientProcess::DataType() const
{
   return recipproc;
}


BACnetEncodeable * BACnetRecipientProcess::clone()
{
   BACnetRecipientProcess * p = new BACnetRecipientProcess();
   *p = *this;
   return p;
}


bool BACnetRecipientProcess::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetRecipientProcess), rbacnet, iOperator, pstrError))
      return isMatch;

   // Compare for equality of the parts, do not log mismatch (since we may be checking INEQUALITY)
   isMatch = m_bacnetrecipient.Match(((BACnetRecipientProcess &)rbacnet).m_bacnetrecipient, '=', NULL) &&
             m_bacnetunsignedID.Match(((BACnetRecipientProcess &)rbacnet).m_bacnetunsignedID, '=', NULL);
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(BACnetCOVSubscription, BACnetEncodeable)


BACnetCOVSubscription::BACnetCOVSubscription( ::BACnetRecipientProcess & rcp, ::BACnetObjectPropertyReference & objpr,
                        bool fNotify, unsigned int nTimeRemaining, float flCOVIncrement )
                        :m_bacnetunsignedTimeRemaining(nTimeRemaining), m_bacnetrealCOVIncrement(flCOVIncrement),
                        m_bacnetboolNotification(fNotify)
{
   m_bacnetrecipprocess = rcp;
   m_bacnetobjpropref = objpr;
}


BACnetCOVSubscription::BACnetCOVSubscription( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}


BACnetCOVSubscription & BACnetCOVSubscription::operator =( const BACnetCOVSubscription & arg )
{
   m_bacnetrecipprocess = arg.m_bacnetrecipprocess;
   m_bacnetobjpropref = arg.m_bacnetobjpropref;
   m_bacnetboolNotification = arg.m_bacnetboolNotification;
   m_bacnetunsignedTimeRemaining = arg.m_bacnetunsignedTimeRemaining;
   m_bacnetrealCOVIncrement.realValue = arg.m_bacnetrealCOVIncrement.realValue;
   return *this;

// BACnetCOVSubscription* newsub = new BACnetCOVSubscription();
// newsub->m_bacnetrecipprocess = arg.m_bacnetrecipprocess;
// newsub->m_bacnetobjpropref = arg.m_bacnetobjpropref;
// newsub->m_bacnetboolNotification = arg.m_bacnetboolNotification;
// newsub->m_bacnetunsignedTimeRemaining = arg.m_bacnetunsignedTimeRemaining;
// newsub->m_bacnetrealCOVIncrement.realValue = arg.m_bacnetrealCOVIncrement.realValue;
// return *newsub;
}

void BACnetCOVSubscription::Encode( CString &enc, Format theFormat ) const
{
   CString str;
   enc  = '{';
   m_bacnetrecipprocess.Encode( str, theFormat );
   enc += str;
   enc += ", ";
   m_bacnetobjpropref.Encode( str, theFormat );
   enc += str;
   enc += ", ";
   m_bacnetboolNotification.Encode( str, theFormat );
   enc += str;
   enc += ", ";
   m_bacnetunsignedTimeRemaining.Encode( str, theFormat );
   enc += str;
   enc += ", ";

   if (m_bacnetrealCOVIncrement.realValue > 0)
      m_bacnetrealCOVIncrement.Encode( str, theFormat );
   else
      str = "0";
   
   enc += str;
   enc += '}';
}

void BACnetCOVSubscription::Decode( const char *dec )
{
   // todo: write decoder if it is ever used...
}


void BACnetCOVSubscription::Encode(BACnetAPDUEncoder& enc, int context)
{
   if ( context != kAppContext )
      BACnetOpeningTag().Encode(enc, context);

   m_bacnetrecipprocess.Encode(enc, 0);
   m_bacnetobjpropref.Encode(enc, 1);
   m_bacnetboolNotification.Encode(enc, 2);
   m_bacnetunsignedTimeRemaining.Encode(enc, 3);
   if ( m_bacnetrealCOVIncrement.realValue > 0 )
      m_bacnetrealCOVIncrement.Encode(enc,4);

   if ( context != kAppContext )
      BACnetClosingTag().Encode(enc, context);
}


void BACnetCOVSubscription::Decode(BACnetAPDUDecoder& dec)
{
   BACnetAPDUTag     tagTestType;   
   dec.ExamineTag(tagTestType);

// if (tagTestType.tagClass == openingTagClass)
//    BACnetOpeningTag().Decode(dec);

   m_bacnetrecipprocess.Decode(dec);
   m_bacnetobjpropref.Decode(dec);
   m_bacnetboolNotification.Decode(dec);
   m_bacnetunsignedTimeRemaining.Decode(dec);
   m_bacnetrealCOVIncrement.Decode(dec);

// if (tagTestType.tagClass == openingTagClass)
//    BACnetClosingTag().Decode(dec);
}


int BACnetCOVSubscription::DataType() const
{
   return COVSub;
}


BACnetEncodeable * BACnetCOVSubscription::clone()
{
   BACnetCOVSubscription * p = new BACnetCOVSubscription();
   *p = *this;
   return p;
}


bool BACnetCOVSubscription::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetCOVSubscription), rbacnet, iOperator, pstrError))
      return isMatch;

   // Compare sub-parts for equality, do not log mismatch (since we may be checking INEQUALITY)
   isMatch = m_bacnetrecipprocess.Match(((BACnetCOVSubscription &)rbacnet).m_bacnetrecipprocess, '=', NULL) &&
             m_bacnetobjpropref.Match(((BACnetCOVSubscription &)rbacnet).m_bacnetobjpropref, '=', NULL) &&
             m_bacnetboolNotification.Match(((BACnetCOVSubscription &)rbacnet).m_bacnetboolNotification, '=', NULL) &&
             m_bacnetunsignedTimeRemaining.Match(((BACnetCOVSubscription &)rbacnet).m_bacnetunsignedTimeRemaining, '=', NULL) &&
             m_bacnetrealCOVIncrement.Match(((BACnetCOVSubscription &)rbacnet).m_bacnetrealCOVIncrement, '=', NULL);
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//
// BACnetDestination
IMPLEMENT_DYNAMIC(BACnetDestination, BACnetEncodeable)

BACnetDestination::BACnetDestination(
   BACnetDaysOfWeek &validDays,
   BACnetTime &fromTime,
   BACnetTime &toTime,
   BACnetRecipient &recipient,
   BACnetUnsigned &processID,
   BACnetBoolean &issueConfirmedNotifications,
   BACnetEventTransitionBits &transitions
   )
: m_validDays(validDays), m_recipient(recipient),
  m_transitions(transitions)
{
   m_fromTime = fromTime;
   m_toTime = toTime;
   m_processID = processID;
   m_issueConfirmedNotifications = issueConfirmedNotifications;
}


BACnetDestination::BACnetDestination( BACnetAPDUDecoder& dec )
{
   Decode(dec);
}


BACnetDestination::BACnetDestination(const BACnetDestination  &src)
{
   *this = src;
}


BACnetDestination::BACnetDestination(void)
{
}


BACnetDestination & BACnetDestination::operator =( const BACnetDestination & arg )
{
   m_validDays = arg.m_validDays;
   m_recipient = arg.m_recipient;
   m_transitions = arg.m_transitions;
   m_fromTime = arg.m_fromTime;
   m_toTime = arg.m_toTime;
   m_processID = arg.m_processID;
   m_issueConfirmedNotifications = arg.m_issueConfirmedNotifications;
   return *this;
}


void BACnetDestination::Encode(BACnetAPDUEncoder &enc, int nContext)
{
   m_validDays.Encode(enc);
   m_fromTime.Encode(enc);
   m_toTime.Encode(enc);
   m_recipient.Encode(enc);
   m_processID.Encode(enc);
   m_issueConfirmedNotifications.Encode(enc);
   m_transitions.Encode(enc);
}


void BACnetDestination::Decode(BACnetAPDUDecoder &dec)
{
   m_validDays.Decode(dec);
   m_fromTime.Decode(dec);
   m_toTime.Decode(dec);
   m_recipient.Decode(dec);
   m_processID.Decode(dec);
   m_issueConfirmedNotifications.Decode(dec);
   m_transitions.Decode(dec);
}


int BACnetDestination::DataType() const
{
   return destination;
}


void BACnetDestination::Encode( CString &enc, Format theFormat ) const
{
   CString str;
   enc = "{";
   m_validDays.Encode(str, theFormat);
   enc += str + ",";
   m_fromTime.Encode(str, theFormat);
   enc += str + ",";
   m_toTime.Encode(str, theFormat);
   enc += str + ",";
   m_recipient.Encode(str, theFormat);
   enc += str + ",";
   m_processID.Encode(str, theFormat);
   enc += str + ",";
   m_issueConfirmedNotifications.Encode(str, theFormat);
   enc += str + ",";
   m_transitions.Encode(str, theFormat);
   enc += str + "}";
}


bool BACnetDestination::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetDestination), rbacnet, iOperator, pstrError))
      return isMatch;

   // Compare for equality, do not log mismatch (since we may be checking INEQUALITY)
   BACnetDestination *pRight = DYNAMIC_DOWNCAST( BACnetDestination, &rbacnet );
   isMatch = m_validDays.Match( pRight->m_validDays, '=', NULL ) &&
             m_recipient.Match( pRight->m_recipient, '=', NULL ) &&
             m_transitions.Match( pRight->m_transitions, '=', NULL ) &&
             m_fromTime.Match( pRight->m_fromTime, '=', NULL ) &&
             m_toTime.Match( pRight->m_toTime, '=', NULL ) &&
             m_processID.Match( pRight->m_processID, '=', NULL ) &&
             m_issueConfirmedNotifications.Match( pRight->m_issueConfirmedNotifications, '=', NULL );
   return DoEqualityMatch( isMatch, rbacnet, iOperator, pstrError );
}


//
// BACnetClosingTag
//
void BACnetClosingTag::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc = '}';
}

void BACnetClosingTag::Encode( BACnetAPDUEncoder& enc, int context )
{
   if (context < 15) {
      enc.CheckSpace( 1 );
      enc.pktBuffer[enc.pktLength++] = ((context & 0x0F) << 4) + 0x0F;
   } else {
      enc.CheckSpace( 2 );
      enc.pktBuffer[enc.pktLength++] = 0xFF;
      enc.pktBuffer[enc.pktLength++] = (context & 0xFF);
   }
}

void BACnetClosingTag::Decode( BACnetAPDUDecoder& dec )
{
   BACnetOctet tag;

   // enough for the tag byte?
   if (dec.pktLength < 1)
      throw_(74) /* not enough data */;

   tag = (dec.pktLength--,*dec.pktBuffer++);

   // check the type
   if ((tag & 0x0F) != 0x0F)
      throw_(75) /* mismatched tag class */;

   // check for a big context
   if ((tag & 0xF0) == 0xF0) {
      if (dec.pktLength < 1)
         throw_(76) /* not enough data */;
      dec.pktLength -= 1;
      dec.pktBuffer += 1;
   }
}

// Handle decoding of arbitrary tagged items in an APDU
IMPLEMENT_DYNAMIC(BACnetANY, BACnetEncodeable)

BACnetANY::BACnetANY()
: spaceLen(0)
, dataLen(0)
, dataBuff(NULL)
{
}

BACnetANY::~BACnetANY()
{
   delete[] dataBuff;
}

// Set value from any other BACnetEncodeable
void BACnetANY::SetValue( BACnetEncodeable &theEnc )
{
   // Encode the input as a BACnet stream
   BACnetAPDUEncoder enc(100);
   theEnc.Encode( enc );

   // Copy the data into ourself
   int len = enc.pktLength;
   if (len > spaceLen)
   {
      delete[] dataBuff;
      dataBuff = new BACnetOctet[ len ];
        spaceLen = len;
   }

   dataLen = len;
   if (len > 0)
   {
      memcpy( dataBuff, enc.pktBuffer, len );
   }
}

void BACnetANY::Encode( BACnetAPDUEncoder& enc, int context )
{
   // Copy raw bytes into the APDU
   if (dataBuff)
   {
      enc.Append( dataBuff, dataLen );
   }
}

void BACnetANY::Decode( BACnetAPDUDecoder& dec )
{
   int nesting = 0;
   BACnetAPDUTag taggy( 0 );

   const BACnetOctet *pStart = dec.pktBuffer;
   while (dec.pktLength > 0)
   {
      int saveLen = dec.pktLength;
      const BACnetOctet *saveBuff = dec.pktBuffer;
      
      taggy.Decode( dec );
      if (taggy.tagClass == openingTagClass)
      {
         // Opening tag
         nesting += 1;
      }
      else if (taggy.tagClass == closingTagClass)
      {
         // Closing tag
         nesting -= 1;
         if (nesting < 0)
         {
            // Back up to the closing tag so they token after us can parse it
            dec.pktLength = saveLen;
            dec.pktBuffer = saveBuff;
            break;
         }
      }
      else if ((taggy.tagClass != applicationTagClass) || (taggy.tagNumber != booleanAppTag))
      {
         // Bypass tag data
         dec.pktLength -= taggy.tagLVT;
         dec.pktBuffer += taggy.tagLVT;
      }
   }

   // Copy the data bytes
   int len = dec.pktBuffer - pStart;
   if (len > spaceLen)
   {
      delete[] dataBuff;
      dataBuff = new BACnetOctet[ len ];
        spaceLen = len;
   }

   dataLen = len;
   if (len > 0)
   {
      memcpy( dataBuff, pStart, len );
   }
}

// Encode as ASCII text X'123456'
void BACnetANY::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc.Empty();
   AppendXhex( enc, dataBuff, dataLen );
}

// Decode from ASCII text X'123456'
void BACnetANY::Decode( const char *dec )
{
   int      upperNibble, lowerNibble, c;

   // verify preamble
   if ((dec[0] != 'X') || (dec[1] != '\''))
      throw_(12) /* invalid character */;

   dec += 2;

   // Length in bytes, two characters each
   int len = strlen( dec ) / 2;
   if (len > spaceLen)
   {
      delete[] dataBuff;
      dataBuff = new BACnetOctet[ len ];
        spaceLen = len;
   }

   dataLen = len;

   int ix = 0;
   while (*dec && (*dec != '\'')) 
   {
      c = ToUpper( *dec++ );
      if (!IsXDigit(c))
         throw_(46) /* invalid character */;
      upperNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

      c = ToUpper( *dec++ );
      if (!IsXDigit(c))
         throw_(47) /* invalid character */;
      lowerNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

      // stick this on the end
      dataBuff[ix++] = (upperNibble << 4) + lowerNibble;
   }
}

bool BACnetANY::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetANY), rbacnet, iOperator, pstrError))
      return isMatch;

   // Convert the input into a BACnetANY so we can compare to ANYTHING
   BACnetANY right;
   right.SetValue( rbacnet );

   isMatch = (dataLen == right.dataLen) && (memcmp( dataBuff, right.dataBuff, dataLen ) == 0);
   if (iOperator == '!=')
      isMatch = !isMatch;

   if (isMatch)
      return true;

   return BACnetEncodeable::Match(rbacnet, iOperator, pstrError);
}


IMPLEMENT_DYNAMIC(BACnetBinaryPriV, BACnetEnumerated)

BACnetBinaryPriV::BACnetBinaryPriV( int nValue )
            :BACnetEnumerated(nValue)
{
}


void BACnetBinaryPriV::Encode( CString &enc, Format /*theFormat*/ ) const
{
   BACnetEnumerated::Encode(enc, apszBinaryPVNames, 2);
}


void BACnetBinaryPriV::Decode( const char *dec )
{
   BACnetEnumerated::Decode(dec, apszBinaryPVNames, 2);
}


IMPLEMENT_DYNAMIC(BACnetObjectContainer, BACnetEncodeable)

BACnetObjectContainer::BACnetObjectContainer()
{
   pbacnetTypedValue = NULL;
}


BACnetObjectContainer::BACnetObjectContainer( BACnetEncodeable * pbacnetEncodeable )
{
   pbacnetTypedValue = NULL;
   SetObject(pbacnetEncodeable);
}


void BACnetObjectContainer::Decode( BACnetAPDUDecoder& dec )
{
   ASSERT(pbacnetTypedValue != NULL);

   if ( pbacnetTypedValue != NULL )
      pbacnetTypedValue->Decode(dec);
}


void BACnetObjectContainer::Encode( BACnetAPDUEncoder& enc, int context )
{
   ASSERT(pbacnetTypedValue != NULL);

   if ( pbacnetTypedValue != NULL )
      pbacnetTypedValue->Encode(enc, context);
}


void BACnetObjectContainer::Encode( CString &enc, Format theFormat ) const
{
//   ASSERT(pbacnetTypedValue != NULL);

   if (pbacnetTypedValue != NULL)
      pbacnetTypedValue->Encode(enc, theFormat);
   else
      enc.Empty();
}


void BACnetObjectContainer::Decode( const char *dec )
{
   ASSERT(pbacnetTypedValue != NULL);

   if ( pbacnetTypedValue != NULL )
      pbacnetTypedValue->Decode(dec);
}


void BACnetObjectContainer::SetObject( BACnetEncodeable * pBACnetEncodeable )
{
   if ( pbacnetTypedValue != NULL )
      delete pbacnetTypedValue;

   pbacnetTypedValue = pBACnetEncodeable;
}


bool BACnetObjectContainer::IsObjectType( CRuntimeClass * pruntimeclass )
{
   ASSERT(pruntimeclass != NULL);

   return pbacnetTypedValue == NULL ? false : (pbacnetTypedValue->IsKindOf(pruntimeclass) != 0);   // accounts for BOOL(int) cases to convert to bool.
}


CRuntimeClass * BACnetObjectContainer::GetObjectType()
{
   return pbacnetTypedValue == NULL ? NULL : pbacnetTypedValue->GetRuntimeClass();
}


BACnetEncodeable *  BACnetObjectContainer::GetObject()
{
   return pbacnetTypedValue;
}


BACnetObjectContainer::~BACnetObjectContainer()
{
   SetObject(NULL);
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetPriorityValue, BACnetObjectContainer)

BACnetPriorityValue::BACnetPriorityValue()
{
}


BACnetPriorityValue::BACnetPriorityValue( BACnetAPDUDecoder & dec )
               :BACnetObjectContainer()
{
   Decode(dec);
}


BACnetPriorityValue::BACnetPriorityValue( BACnetEncodeable * pbacnetEncodeable )
               :BACnetObjectContainer(pbacnetEncodeable)
{
}


void BACnetPriorityValue::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag     tagNullTest;

   // Peek for received null value of zero, don't advance APDU pointer
   tagNullTest.Peek(dec);
   CreateTypedObject(tagNullTest.tagNumber);

   BACnetObjectContainer::Decode(dec);
}


void BACnetPriorityValue::CreateTypedObject(BACnetApplicationTag tag)
{
   switch( tag )
   {
      case nullAppTag:
         SetObject(new BACnetNull());
         break;
      case unsignedIntAppTag:
         SetObject(new BACnetUnsigned());
         break;
      case realAppTag:
         SetObject(new BACnetReal());
         break;
      case enumeratedAppTag:
         SetObject(new BACnetBinaryPriV());
         break;
      default:
         TRACE1("Attempt to create BACnetEncodeable type %d from Tag", tag );
         ASSERT(0);
         SetObject(NULL);
   }
}

int BACnetPriorityValue::DataType() const
{
   return prival;
}


BACnetEncodeable * BACnetPriorityValue::clone()
{
   return new BACnetPriorityValue( pbacnetTypedValue == NULL ? (BACnetEncodeable *) NULL : pbacnetTypedValue->clone());
}


bool BACnetPriorityValue::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetPriorityValue), rbacnet, iOperator, pstrError))
      return isMatch;

   // JLH 26 January 2010:
   // Compare for equality, greater, or less returns true only if
   // - matching CHOICE (via GetRuntimeClass)
   // - compare values
   //
   // Compare for inequality returns true if either
   // - non-matching CHOICE (via GetRuntimeClass)
   // - compare values
   // Use OperatorToString to show operator
   //
   isMatch = (pbacnetTypedValue != NULL) &&
             (((BACnetPriorityValue&)rbacnet).GetObject()->GetRuntimeClass() == pbacnetTypedValue->GetRuntimeClass());
   if (!isMatch)
   {
      if (iOperator == '!=')
      {
         isMatch = true;
      }
      else
      {
         CString vBAC( ((BACnetPriorityValue&)rbacnet).GetObject()->ToString() );
         CString vEPICS( pbacnetTypedValue->ToString() );
         TRACE( "Got %s and %s\n", vBAC, vEPICS );

         pstrError->Format( "(%s) different type from (%s)", vBAC, vEPICS );
      }
   }
   else
   {
      isMatch = pbacnetTypedValue->Match(*((BACnetPriorityValue &)rbacnet).GetObject(), iOperator, pstrError);
   }

   return isMatch;
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetCalendarEntry, BACnetObjectContainer)

BACnetCalendarEntry::BACnetCalendarEntry()
{
}


BACnetCalendarEntry::BACnetCalendarEntry( BACnetAPDUDecoder & dec )
               :BACnetObjectContainer()
{
   Decode(dec);
}


BACnetCalendarEntry::BACnetCalendarEntry( BACnetEncodeable * pbacnetEncodeable )
               :BACnetObjectContainer(pbacnetEncodeable)
{
   if ( pbacnetEncodeable != NULL )
   {
      switch(pbacnetEncodeable->DataType())
      {
         case ptDate: 
            m_nChoice = 0;
            break;
         case dtrange: 
            m_nChoice = 1;
            break;
         case dsofweek: 
            m_nChoice = 2;
            break;
      }
   }
}

void BACnetCalendarEntry::Encode( BACnetAPDUEncoder& enc, int context)
{
   if(pbacnetTypedValue == NULL)
      return;

   if ( context != kAppContext )
      m_nChoice = context;
   else if ( pbacnetTypedValue->IsKindOf(RUNTIME_CLASS(BACnetDate)))
      m_nChoice = 0;
   else if (pbacnetTypedValue->IsKindOf(RUNTIME_CLASS(BACnetDateRange)))
      m_nChoice = 1;
   else
      m_nChoice = 2;

   switch (m_nChoice) 
   {
   case 0:
      pbacnetTypedValue->Encode(enc, 0);
      break;
   case 1:
      BACnetOpeningTag().Encode(enc, 1);
      pbacnetTypedValue->Encode(enc);
      BACnetClosingTag().Encode(enc, 1);
      break;
   case 2:
      pbacnetTypedValue->Encode(enc, 2);
      break;
   default:
      throw "Unknown choice.";
      break;
   }
}


void BACnetCalendarEntry::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag     tagTestType;
   dec.ExamineTag(tagTestType);
   m_nChoice = tagTestType.tagNumber;     
   switch (m_nChoice) 
   {
   case 0:
      SetObject( new BACnetDate(dec) );
      break;
   case 1:
      BACnetOpeningTag().Decode(dec);
      SetObject( new BACnetDateRange(dec) ); 
      BACnetClosingTag().Decode(dec);
      break;
   case 2:
      SetObject( new BACnetWeekNDay(dec) );
      break;
   default:
      throw "Unknown choice.";
      break;
   }
}


int BACnetCalendarEntry::DataType() const
{
   return calent;
}


BACnetEncodeable * BACnetCalendarEntry::clone()
{
   return new BACnetCalendarEntry( pbacnetTypedValue == NULL ? (BACnetEncodeable *) NULL : pbacnetTypedValue->clone());
}


bool BACnetCalendarEntry::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatchEquality(isMatch, RUNTIME_CLASS(BACnetCalendarEntry), rbacnet, iOperator, pstrError))
      return isMatch;

   ASSERT(pbacnetTypedValue != NULL);
   return pbacnetTypedValue->Match(*((BACnetCalendarEntry &)rbacnet).GetObject(), iOperator, pstrError);
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetTimeStamp, BACnetObjectContainer)

BACnetTimeStamp::BACnetTimeStamp()
{
}


BACnetTimeStamp::BACnetTimeStamp( BACnetAPDUDecoder & dec )
            :BACnetObjectContainer()
{
   Decode(dec);
}


BACnetTimeStamp::BACnetTimeStamp( BACnetEncodeable * pbacnetEncodeable )
               :BACnetObjectContainer(pbacnetEncodeable)
{
}

void BACnetTimeStamp::Encode( BACnetAPDUEncoder& enc, int context)
{
   if(pbacnetTypedValue == NULL)
      return;

   if ( context == -1 )
   {
      if(pbacnetTypedValue->IsKindOf(RUNTIME_CLASS(BACnetDateTime)))
         context = 2;
      else if (pbacnetTypedValue->IsKindOf(RUNTIME_CLASS(BACnetTime)))
         context = 0;
      else if(pbacnetTypedValue->IsKindOf(RUNTIME_CLASS(BACnetUnsigned)))
         context = 1;
   }

   if ( context > 1 )   // only do an explicit Application tag if DateTime type
   {
      BACnetOpeningTag().Encode(enc,context);
      pbacnetTypedValue->Encode(enc);
      BACnetClosingTag().Encode(enc,context);
   }
   else
   {
      pbacnetTypedValue->Encode(enc, context);
   }
}

void BACnetTimeStamp::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag     tagTestType;

   //Modified by HUMENG
   dec.ExamineTag(tagTestType);

   // Tag has 0, 1, 2 for time, unsigned, datetime

   if (tagTestType.tagClass == openingTagClass)
   {
      BACnetOpeningTag().Decode(dec);
      SetObject( new BACnetDateTime(dec) ); 
      BACnetClosingTag().Decode(dec);
   }
   else
    switch (tagTestType.tagNumber)
   {
      case 0:              SetObject( new BACnetTime(dec) );   break;
      case 1:              SetObject( new BACnetUnsigned(dec) ); break;
//    case 2:              SetObject( new BACnetDateTime(dec) ); break;
      default:          TRACE0("INVALID type in encoded stream for TimeStamp");
                        ASSERT(0);
   }
}


int BACnetTimeStamp::DataType() const
{
   return TSTMP;
}


BACnetEncodeable * BACnetTimeStamp::clone()
{
   return new BACnetTimeStamp( pbacnetTypedValue == NULL ? (BACnetEncodeable *) NULL : pbacnetTypedValue->clone());
}


bool BACnetTimeStamp::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetTimeStamp), rbacnet, iOperator, pstrError))
      return isMatch;

   isMatch = (pbacnetTypedValue != NULL) &&
             (((BACnetTimeStamp &)rbacnet).GetObject() != NULL) &&
             (pbacnetTypedValue->DataType() == ((BACnetTimeStamp &)rbacnet).GetObject()->DataType());
   if (isMatch)
   {
      // Items are of the same CHOICE, so we can compare < > etc.
      isMatch = pbacnetTypedValue->Match(*((BACnetTimeStamp &)rbacnet).GetObject(), iOperator, pstrError);
   }
   else
   {
      // Items are NOT of the same CHOICE, so only = and != make sense
      isMatch = DoEqualityMatch( false, rbacnet, iOperator, pstrError );
   }

   return isMatch;
}

//====================================================================

IMPLEMENT_DYNAMIC(BACnetScale, BACnetObjectContainer)

BACnetScale::BACnetScale()
{
}

BACnetScale::BACnetScale(float value)
{
   SetObject( new BACnetReal(value) );
}

BACnetScale::BACnetScale(int value)
{
   SetObject( new BACnetUnsigned(value) );
}

BACnetScale::BACnetScale( BACnetAPDUDecoder & dec )
            :BACnetObjectContainer()
{
   Decode(dec);
}


BACnetScale::BACnetScale( BACnetEncodeable * pbacnetEncodeable )
               :BACnetObjectContainer(pbacnetEncodeable)
{
}

void BACnetScale::Encode( BACnetAPDUEncoder& enc, int context)
{
   if(pbacnetTypedValue == NULL)
      return;
   pbacnetTypedValue->Encode(enc, context);
}

void BACnetScale::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag     tagTestType;
   dec.ExamineTag(tagTestType);
    switch (tagTestType.tagNumber)
   {
      case 0:              SetObject( new BACnetReal(dec) );   break;
      case 1:              SetObject( new BACnetUnsigned(dec) ); break;
      default:          TRACE0("INVALID type in encoded stream for BACnetScale");
                        ASSERT(0);
   }
}

int BACnetScale::DataType() const
{
   return escale;
}


BACnetEncodeable * BACnetScale::clone()
{
   return new BACnetScale( pbacnetTypedValue == NULL ? (BACnetEncodeable *) NULL : pbacnetTypedValue->clone());
}


bool BACnetScale::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetScale), rbacnet, iOperator, pstrError))
      return isMatch;

   ASSERT(pbacnetTypedValue != NULL);
   return pbacnetTypedValue->Match(*((BACnetScale &)rbacnet).GetObject(), iOperator, pstrError);
}

//====================================================================

IMPLEMENT_DYNAMIC(BACnetPrescale, BACnetEncodeable)

BACnetPrescale::BACnetPrescale()
{
}

BACnetPrescale::BACnetPrescale(unsigned multiplier, unsigned moduloDivide)
{
   this->multiplier = multiplier;
   this->moduloDivide = moduloDivide;
}

BACnetPrescale::BACnetPrescale( BACnetAPDUDecoder & dec )
{
   Decode(dec);
}

void BACnetPrescale::Encode( BACnetAPDUEncoder& enc, int context)
{
   BACnetUnsigned value1(multiplier);
   BACnetUnsigned value2(moduloDivide);

   value1.Encode(enc, context);
   value2.Encode(enc, context);
}

void BACnetPrescale::Decode( BACnetAPDUDecoder& dec )
{  
   BACnetUnsigned value1(dec);
   BACnetUnsigned value2(dec);
   // Tamp down the warnings with casts
   multiplier = (unsigned short) value1.uintValue;
   moduloDivide = (unsigned short) value2.uintValue;
}

void BACnetPrescale::Encode( CString &enc, Format /*theFormat*/ ) const
{
   enc.Format( "{%d, %d}", multiplier, moduloDivide );
}


int BACnetPrescale::DataType() const
{
   return eprescl;
}


BACnetEncodeable * BACnetPrescale::clone()
{  
   return new BACnetPrescale(multiplier, moduloDivide); 
}

//====================================================================
IMPLEMENT_DYNAMIC(BACnetAccumulatorRecord, BACnetEncodeable)
BACnetAccumulatorRecord::BACnetAccumulatorRecord()    
{
}

BACnetAccumulatorRecord::BACnetAccumulatorRecord(PICS::BACnetDateTime timestamp, 
   unsigned presentValue, unsigned accumulatedValue, unsigned accumulatorStatus)
{
   this->timestamp = timestamp;
   this->presentValue = presentValue;
   this->accumulatedValue = accumulatedValue;
   this->accumulatorStatus = accumulatorStatus;
}

BACnetAccumulatorRecord::BACnetAccumulatorRecord( BACnetAPDUDecoder & dec )
{
}

void BACnetAccumulatorRecord::Decode( BACnetAPDUDecoder& dec )
{
}

void BACnetAccumulatorRecord::Encode( BACnetAPDUEncoder& enc, int context)
{
}

void BACnetAccumulatorRecord::Encode( CString &enc, Format /*theFormat*/ ) const
{
   // If this was a BACnet timestamp instead of a PICS::BACnetTimeStamp we could just
   // use timestamp.Encode( str )
   enc.Format( "{{(%d-%s-%d), %d:%d:%d.%d},%d, %d, %s}", 
      timestamp.date.day_of_month, 
      timestamp.date.month == 0xFF ? "*" : 
         NetworkSniffer::BAC_STRTAB_month.EnumString( timestamp.date.month ),
      timestamp.date.year + 1900, 
      timestamp.time.hour, timestamp.time.minute, timestamp.time.second, timestamp.time.hundredths, 
      presentValue, accumulatedValue, 
      NetworkSniffer::BAC_STRTAB_BACnetAccumulatorStatus.EnumString( accumulatorStatus, "BACnetAccumulatorStatus:" )
      );
}


int BACnetAccumulatorRecord::DataType(void) const
{
   return eaclr;
}

BACnetEncodeable * BACnetAccumulatorRecord::clone(void)
{
   return new BACnetAccumulatorRecord(timestamp, presentValue, accumulatedValue,
      accumulatorStatus); 
}


//====================================================================
IMPLEMENT_DYNAMIC(BACnetShedLevel, BACnetEncodeable)
BACnetShedLevel::BACnetShedLevel()     
: context(0)
, value(0.0f)
{
}

BACnetShedLevel::BACnetShedLevel(unsigned a_context, float a_value)
: context(a_context)
, value(a_value)
{
}

BACnetShedLevel::BACnetShedLevel( BACnetAPDUDecoder & dec )
{
}

void BACnetShedLevel::Decode( BACnetAPDUDecoder& dec )
{
   BACnetAPDUTag     tagTestType;
   dec.ExamineTag(tagTestType);
   context = tagTestType.tagNumber;
   if ((context == 0) || (context == 1))
   {
      BACnetUnsigned u1(dec); 
      value = (float)u1.uintValue;
   }
   else if (context == 2)
   {
      BACnetReal r1(dec);
      value = r1.realValue;
   }
   else
   {
      TRACE0("INVALID type in encoded stream for BACnetShedLevel");
      ASSERT(0);
   }
}

void BACnetShedLevel::Encode( BACnetAPDUEncoder& enc, int context)
{
   // TODO: needed?
}

void BACnetShedLevel::Encode( CString &enc, Format /*theFormat*/ ) const
{
   switch (context)
   {
   case 0:
   case 1:
      enc.Format( "[%u] %u", context, (unsigned)value );
      break;
   case 2:
      enc.Format( "[2] %lf", value );
      break;
   default:
      enc.Format( "[%d] ?", context );
      break;
   }
}


int BACnetShedLevel::DataType(void) const
{
   return shedlevel;
}

BACnetEncodeable * BACnetShedLevel::clone(void)
{
   return new BACnetShedLevel(context, value); 
}

//====================================================================

IMPLEMENT_DYNAMIC(BACnetGenericArray, BACnetEncodeable)

BACnetGenericArray::BACnetGenericArray()
: m_apBACnetObjects()
, m_nElemType(-1)
, m_nType(0)
, m_pPrototype(NULL)
{
}

// TODO: Shouldn't we have TWO types: the array's AND the element's?
// nDataType is only the element's type.
BACnetGenericArray::BACnetGenericArray( int nDataType, int nSize /* = -1 */)
: m_apBACnetObjects()
, m_nElemType(nDataType)
, m_nType(0)
, m_pPrototype(NULL)
{
   if ( nSize != -1 )
      m_apBACnetObjects.SetSize(nSize);
}

// Create by prototype
BACnetGenericArray::BACnetGenericArray( BACnetEncodeable *pPrototype )
: m_apBACnetObjects()
, m_nElemType(-1)
, m_nType(0)
, m_pPrototype(pPrototype)
{
}

void BACnetGenericArray::Decode( BACnetAPDUDecoder& dec )
{
   ClearArray();
   BACnetAPDUTag tag;

   while (true)
   {
      tag.Peek(dec);
      if ( tag.tagClass == closingTagClass )
         break;

      Add(NewDecoderElement(dec));
   }
}


void BACnetGenericArray::Encode( BACnetAPDUEncoder& enc, int context )
{
// BACnetUnsigned bacnetSize(m_apBACnetObjects.GetSize());
// bacnetSize.Encode(enc, context);

   if ( context != kAppContext )
      BACnetOpeningTag().Encode(enc,context);

   for ( int i = 0; i < m_apBACnetObjects.GetSize(); i++)
      if ( m_apBACnetObjects[i] != NULL )
         m_apBACnetObjects[i]->Encode(enc);

   if ( context != kAppContext )
      BACnetClosingTag().Encode(enc,context);
}


void BACnetGenericArray::Encode( CString &enc, Format theFormat ) const
{
   // TODO: 135.1 says that ARRAYS are enclosed in {}, while LISTS are
   // enclosed in ().  Probably should make another class to
   // handle the wrapping (or else a parm to BACnetGenericArray's constructor)
   enc = "{";
   for (int i = 0; i < m_apBACnetObjects.GetSize(); i++ )
   {
      if ( i > 0 )
         enc += ", ";

      CString element;
      m_apBACnetObjects[i]->Encode( element, theFormat );
      enc += element;
   }
   enc += "}";
}


void BACnetGenericArray::Decode( const char *dec )
{
   // TODO: Either DON'T OVERRIDE this method, or implement it:
   // Just the reverse of Encode(CString)
}

int BACnetGenericArray::DataType(void) const
{
   return m_nType;
}

BACnetEncodeable * BACnetGenericArray::NewDecoderElement( BACnetAPDUDecoder& dec )
{
   BACnetEncodeable *pRetval;
   if (m_pPrototype != NULL)
   {
      // Create element as a copy of the prototype
      pRetval = m_pPrototype->clone();
      // Decode into the copy
      pRetval->Decode(dec);
   }
   else
   {
      // Create element via the factory
      pRetval = BACnetEncodeable::Factory(m_nElemType, dec);
   }

   return pRetval;
}


BACnetEncodeable * BACnetGenericArray::operator[](int nIndex) const
{
   ASSERT(nIndex < m_apBACnetObjects.GetSize());
   return m_apBACnetObjects[nIndex];
}


int BACnetGenericArray::Add( BACnetEncodeable * pbacnetEncodeable )
{
   return m_apBACnetObjects.Add(pbacnetEncodeable);
}


BACnetEncodeable & BACnetGenericArray::operator[](int nIndex)
{
   return *m_apBACnetObjects[nIndex];
}

const BACnetEncodeable* BACnetGenericArray::GetGenericElement(int nIndex) const
{
   return (nIndex < m_apBACnetObjects.GetSize()) ? m_apBACnetObjects[nIndex] : NULL;
}

BACnetGenericArray::~BACnetGenericArray()
{
   ClearArray();
   delete m_pPrototype;
}


int BACnetGenericArray::GetSize() const
{
   return m_apBACnetObjects.GetSize();
}


void BACnetGenericArray::ClearArray()
{
   for ( int i = 0; i < m_apBACnetObjects.GetSize(); i++)
      if ( m_apBACnetObjects[i] != NULL )
         delete m_apBACnetObjects[i];

   m_apBACnetObjects.RemoveAll();
}


bool BACnetGenericArray::Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError )
{
   bool isMatch;
   if (PreMatch(isMatch, RUNTIME_CLASS(BACnetGenericArray), rbacnet, iOperator, pstrError))
      return isMatch;

   CString strError;

   // want to compare entire array...  Better make sure lengths are the same
   if (GetSize() != ((BACnetGenericArray &) rbacnet).GetSize())
   {
      if (pstrError != NULL)
         pstrError->Format(IDS_SCREX_COMPARRAYSIZE, GetSize(), ((BACnetGenericArray &) rbacnet).GetSize() );

      return false;
   }

   isMatch = true;
   int nIndex;
   for (nIndex = 0; isMatch && (nIndex < GetSize()); nIndex++)
   {
      isMatch = m_apBACnetObjects[nIndex]->Match(((BACnetGenericArray &)rbacnet)[nIndex], iOperator, &strError);
   }

   if (!isMatch && (pstrError != NULL) && !strError.IsEmpty())
   {
      char szBuff[20];
      sprintf(szBuff, "[%d] ", nIndex);
      *pstrError += szBuff + strError;
   }

   return isMatch;
}


//====================================================================
//====================================================================

IMPLEMENT_DYNAMIC(BACnetPriorityArray, BACnetGenericArray)

BACnetPriorityArray::BACnetPriorityArray()
               :BACnetGenericArray(prival)
{
}


BACnetPriorityArray::BACnetPriorityArray( BACnetAPDUDecoder& dec )
               :BACnetGenericArray(prival, nPRIO)
{
   Decode(dec);
}


BACnetPriorityArray::BACnetPriorityArray( float * paPriority, int nMax, float fNull )
               :BACnetGenericArray(prival)
{
   m_nType = paf;

   for ( int i = 0; i < nMax; i++ )
      if ( paPriority[i] == fNull )
         Add(new BACnetPriorityValue(new BACnetNull()));
      else
         Add(new BACnetPriorityValue(new BACnetReal(paPriority[i])));
}


/*
BACnetPriorityArray::BACnetPriorityArray( int * paPriority, int nMax, int bNull )
               :BACnetGenericArray(prival)
{
   m_nType = pab;

   for ( int i = 0; i < nMax; i++ )
      if ( paPriority[i] == bNull )
         Add(new BACnetPriorityValue(new BACnetNull()));
      else
         Add(new BACnetPriorityValue(new BACnetBinaryPriV(paPriority[i])));
}
*/

BACnetPriorityArray::BACnetPriorityArray( unsigned short * paPriority, int nMax, unsigned short uNull, boolean binaryPV )
               :BACnetGenericArray(prival)
{
   m_nType = pau;

   for ( int i = 0; i < nMax; i++ )
   {
      if ( paPriority[i] == uNull )
      {
         Add(new BACnetPriorityValue(new BACnetNull()));
      }
      else
      {
         if ( binaryPV && uNull == bpaNULL)
             Add(new BACnetPriorityValue(new BACnetBinaryPriV(paPriority[i])));
         else if (binaryPV)   // special case of any Enumerated
            Add(new BACnetPriorityValue(new BACnetEnumerated(paPriority[i])));
         else
             Add(new BACnetPriorityValue(new BACnetUnsigned(paPriority[i])));
      }
   }
}


BACnetPriorityValue * BACnetPriorityArray::operator[](int nIndex) const
{
   return (BACnetPriorityValue *) m_apBACnetObjects[nIndex];
}


BACnetPriorityValue & BACnetPriorityArray::operator[](int nIndex)
{
   return (BACnetPriorityValue &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetListOfCalendarEntry, BACnetGenericArray)

BACnetListOfCalendarEntry::BACnetListOfCalendarEntry()
                     :BACnetGenericArray(calent)
{
   m_nType = calist;
}


BACnetListOfCalendarEntry::BACnetListOfCalendarEntry( BACnetAPDUDecoder& dec )
                     :BACnetGenericArray(calent)
{
   m_nType = calist;
   Decode(dec);
}


BACnetCalendarEntry * BACnetListOfCalendarEntry::operator[](int nIndex) const
{
   return (BACnetCalendarEntry *) m_apBACnetObjects[nIndex];
}


BACnetCalendarEntry & BACnetListOfCalendarEntry::operator[](int nIndex)
{
   return  (BACnetCalendarEntry &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetTextArray, BACnetGenericArray)

BACnetTextArray::BACnetTextArray( char * paText[], int nMax /* = -1 */ )
            :BACnetGenericArray(ch_string)
{
   m_nType = statext;

   int nSize = 0;

   while ( paText[nSize] != NULL  && (nMax == -1 || nSize < nMax) )
      m_apBACnetObjects.Add(new BACnetCharacterString(paText[nSize++]));
}


// Constructor for only one array element
BACnetTextArray::BACnetTextArray( char * pText )
            :BACnetGenericArray(ch_string)
{
   m_nType = statext;
   m_apBACnetObjects.Add(new BACnetCharacterString(pText));
}


BACnetTextArray::BACnetTextArray( BACnetAPDUDecoder& dec )
            :BACnetGenericArray(ch_string)
{
   m_nType = statext;
   Decode(dec);
}


//void BACnetTextArray::Decode( BACnetAPDUDecoder& dec )
//{
   // Don't need unsigned at beginning of decode stream to tell us how many strings follow
   // We'll figure that out by encountering a null in place of a string

// BACnetAPDUTag  tag;

// while ( true )
// {
//    tag.Peek(dec);
//    if ( tag.tagNumber != characterStringAppTag )
//       break;

//    Add(new BACnetCharacterString(dec));
// }
//}


BACnetCharacterString * BACnetTextArray::operator[](int nIndex) const
{
   return (BACnetCharacterString *) m_apBACnetObjects[nIndex];
}


BACnetCharacterString & BACnetTextArray::operator[](int nIndex)
{
   return  (BACnetCharacterString &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetBooleanArray, BACnetGenericArray)

BACnetBooleanArray::BACnetBooleanArray( PICS::BooleanList *paBoolean )
               :BACnetGenericArray(ebool)
{
   m_nType = eboollist;

   int nSize = 0;
   while ( paBoolean != NULL )
   {
      m_apBACnetObjects.Add(new ::BACnetBoolean(paBoolean->value));
      paBoolean = paBoolean->next;
   }
}

BACnetBooleanArray::BACnetBooleanArray( BACnetAPDUDecoder& dec )
               :BACnetGenericArray(ebool)
{
   m_nType = eboollist;
   Decode(dec);
}


BACnetBoolean * BACnetBooleanArray::operator[](int nIndex) const
{
   return (BACnetBoolean *) m_apBACnetObjects[nIndex];
}


BACnetBoolean & BACnetBooleanArray::operator[](int nIndex)
{
   return  (BACnetBoolean &) *m_apBACnetObjects[nIndex];
}

//====================================================================

IMPLEMENT_DYNAMIC(BACnetUnsignedArray, BACnetGenericArray)

// nMax, if specified, says don't check for zero to stop...

BACnetUnsignedArray::BACnetUnsignedArray( unsigned char paUnsigned[], int nMax /* = -1 */ )
               :BACnetGenericArray(ud)
{
   m_nType = stavals;

   int nSize = 0;

   while ( (paUnsigned[nSize] != 0 || nMax != -1)  && (nMax == -1 || nSize < nMax) )
      m_apBACnetObjects.Add(new BACnetUnsigned( (unsigned int) paUnsigned[nSize++]));
}


BACnetUnsignedArray::BACnetUnsignedArray( unsigned short paUnsigned[], int nMax /* = -1 */ )
               :BACnetGenericArray(ud)
{
   m_nType = stavals;

   // Same as for chars but, well, different.
   // This will probably go away if I change the EPICS internal store to have 'word' types
   // for alarm_values and fault_values in the MSI and MSO object, like in the MSV object.

   int nSize = 0;

   while ( (paUnsigned[nSize] != 0  || nMax != -1)  && (nMax == -1 || nSize < nMax) )
      m_apBACnetObjects.Add(new BACnetUnsigned( (unsigned int) paUnsigned[nSize++]));
}

BACnetUnsignedArray::BACnetUnsignedArray( PICS::UnsignedList *paUnsigned )
               :BACnetGenericArray(ud)
{
   m_nType = stavals;

   int nSize = 0;
   while ( paUnsigned != NULL )
   {
      m_apBACnetObjects.Add(new ::BACnetUnsigned((unsigned int) paUnsigned->value));
      paUnsigned = paUnsigned->next;
   }
}

BACnetUnsignedArray::BACnetUnsignedArray( BACnetAPDUDecoder& dec )
               :BACnetGenericArray(ud)
{
   m_nType = stavals;
   Decode(dec);
}


BACnetUnsigned * BACnetUnsignedArray::operator[](int nIndex) const
{
   return (BACnetUnsigned *) m_apBACnetObjects[nIndex];
}


BACnetUnsigned & BACnetUnsignedArray::operator[](int nIndex)
{
   return  (BACnetUnsigned &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetTimeStampArray, BACnetGenericArray)

// nMax, if specified, says don't check for zero to stop...

BACnetTimeStampArray::BACnetTimeStampArray()
               :BACnetGenericArray(TSTMP)
{
   m_nType = TSTMParr;
}


BACnetTimeStampArray::BACnetTimeStampArray( int nSize )
               :BACnetGenericArray(TSTMP, nSize)
{
   m_nType = TSTMParr;
}


BACnetTimeStampArray::BACnetTimeStampArray( BACnetAPDUDecoder& dec )
               :BACnetGenericArray(TSTMP)
{
   m_nType = TSTMParr;
   Decode(dec);
}


BACnetTimeStamp * BACnetTimeStampArray::operator[](int nIndex) const
{
   return (BACnetTimeStamp *) m_apBACnetObjects[nIndex];
}


BACnetTimeStamp & BACnetTimeStampArray::operator[](int nIndex)
{
   return  (BACnetTimeStamp &) *m_apBACnetObjects[nIndex];
}

//====================================================================

IMPLEMENT_DYNAMIC(BACnetObjectIDList, BACnetGenericArray)

BACnetObjectIDList::BACnetObjectIDList()
               :BACnetGenericArray(ob_id)
{
   m_nType = lobj;
}


BACnetObjectIDList::BACnetObjectIDList( int nSize )
               :BACnetGenericArray(ob_id, nSize)
{
   m_nType = lobj;
}


BACnetObjectIDList::BACnetObjectIDList( BACnetAPDUDecoder& dec )
               :BACnetGenericArray(ob_id)
{
   m_nType = lobj;
   Decode(dec);
}


//void BACnetObjectIDList::Decode( BACnetAPDUDecoder& dec )
//{
   // Object ID lists suck out of the stream until a non-object ID tag is found...
// BACnetAPDUTag tag;

// BACnetGenericArray::Decode(dec);    // don't use unsigned int in front of list to determine size

// while (true)
// {
//    tag.Peek(dec);
//    if ( tag.tagNumber != objectIdentifierAppTag )
//       break;

//    Add(new BACnetObjectIdentifier(dec));
// }
//}


BACnetObjectIdentifier * BACnetObjectIDList::operator[](int nIndex) const
{
   return (BACnetObjectIdentifier *) m_apBACnetObjects[nIndex];
}


BACnetObjectIdentifier & BACnetObjectIDList::operator[](int nIndex)
{
   return  (BACnetObjectIdentifier &) *m_apBACnetObjects[nIndex];
}


//====================================================================

/*
IMPLEMENT_DYNAMIC(BACnetListOfObjectPropertyReference, BACnetGenericArray)


BACnetListOfObjectPropertyReference::BACnetListOfObjectPropertyReference()
                           :BACnetGenericArray(propref)
{
   m_nType = lopref;
}


BACnetListOfObjectPropertyReference::BACnetListOfObjectPropertyReference( BACnetAPDUDecoder& dec )
                           :BACnetGenericArray(propref)
{
   m_nType = lopref;
   Decode(dec);
}


BACnetListOfObjectPropertyReference::BACnetListOfObjectPropertyReference( PICS::BACnetObjectPropertyReference * pobjprops )
                           :BACnetGenericArray(propref)
{
   m_nType = lopref;
   ClearArray();
   while ( pobjprops != NULL )
   {
      Add(new ::BACnetObjectPropertyReference(pobjprops->object_id, pobjprops->property_id, pobjprops->pa_index));
      pobjprops = pobjprops->next;
   }
}


BACnetObjectPropertyReference * BACnetListOfObjectPropertyReference::operator[](int nIndex) const
{
   return (BACnetObjectPropertyReference *) m_apBACnetObjects[nIndex];
}



BACnetObjectPropertyReference & BACnetListOfObjectPropertyReference::operator[](int nIndex)
{
   return  (BACnetObjectPropertyReference &) *m_apBACnetObjects[nIndex];
}
*/

//====================================================================

IMPLEMENT_DYNAMIC(BACnetListOfDeviceObjectPropertyReference, BACnetGenericArray)

BACnetListOfDeviceObjectPropertyReference::BACnetListOfDeviceObjectPropertyReference()
                           :BACnetGenericArray(propref)
{
   m_nType = lopref;
}


BACnetListOfDeviceObjectPropertyReference::BACnetListOfDeviceObjectPropertyReference( BACnetAPDUDecoder& dec )
                           :BACnetGenericArray(propref)
{
   m_nType = lopref;
   Decode(dec);
}


BACnetListOfDeviceObjectPropertyReference::BACnetListOfDeviceObjectPropertyReference( PICS::BACnetDeviceObjectPropertyReference * pobjprops )
                           :BACnetGenericArray(propref)
{
   m_nType = lopref;
   ClearArray();
   while ( pobjprops != NULL )
   {
      Add(new ::BACnetDeviceObjectPropertyReference(pobjprops->Objid, pobjprops->wPropertyid, pobjprops->ulIndex, pobjprops->DeviceObj));
      pobjprops = pobjprops->next;
   }
}


BACnetDeviceObjectPropertyReference * BACnetListOfDeviceObjectPropertyReference::operator[](int nIndex) const
{
   return (BACnetDeviceObjectPropertyReference *) m_apBACnetObjects[nIndex];
}


BACnetDeviceObjectPropertyReference & BACnetListOfDeviceObjectPropertyReference::operator[](int nIndex)
{
   return  (BACnetDeviceObjectPropertyReference &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetListOfDeviceObjectReference, BACnetGenericArray)

// These (lodoref) are arrays of devobjref: BACnetARRAY[N] of BACnetDeviceObjectReference
BACnetListOfDeviceObjectReference::BACnetListOfDeviceObjectReference()
                           :BACnetGenericArray(devobjref)
{
   m_nType = lodoref;
}


BACnetListOfDeviceObjectReference::BACnetListOfDeviceObjectReference( BACnetAPDUDecoder& dec )
                           :BACnetGenericArray(devobjref)
{
   m_nType = lodoref;
   Decode(dec);
}


BACnetListOfDeviceObjectReference::BACnetListOfDeviceObjectReference( PICS::BACnetDeviceObjectReference * pobjprops )
                           :BACnetGenericArray(devobjref)
{
   m_nType = lodoref;
   ClearArray();
   while ( pobjprops != NULL )
   {
      Add(new ::BACnetDeviceObjectReference(pobjprops->Objid, pobjprops->DeviceObj));
      pobjprops = pobjprops->next;
   }
}


BACnetDeviceObjectReference * BACnetListOfDeviceObjectReference::operator[](int nIndex) const
{
   return (BACnetDeviceObjectReference *) m_apBACnetObjects[nIndex];
}


BACnetDeviceObjectReference & BACnetListOfDeviceObjectReference::operator[](int nIndex)
{
   return  (BACnetDeviceObjectReference &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetListOfEnum, BACnetGenericArray)

BACnetListOfEnum::BACnetListOfEnum(int tableId)
               :BACnetGenericArray(etl)
{
   m_nType = etl;
   m_nTableId = tableId;  // from enumeration enumeration (eiXXX) in prop.h
}


BACnetListOfEnum::BACnetListOfEnum( BACnetAPDUDecoder& dec, int tableId )
               :BACnetGenericArray(etl)
{
   m_nType = etl;
   m_nTableId = tableId;
   Decode(dec);
}


BACnetEncodeable * BACnetListOfEnum::NewDecoderElement( BACnetAPDUDecoder& dec )
{
   // TODO: But Factory wants PROPERTY ID, not ENUMTABLE ID
   BACnetEncodeable * penum = BACnetEncodeable::Factory(etl, dec, m_nTableId);
   // TODO: Didn't the Factory's constructor call Decode?
   penum->Decode(dec);
   return penum;
}


BACnetEnumerated * BACnetListOfEnum::operator[](int nIndex) const
{
   return (BACnetEnumerated *) m_apBACnetObjects[nIndex];
}


BACnetEnumerated & BACnetListOfEnum::operator[](int nIndex)
{
   return  (BACnetEnumerated &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetListOfVTClass, BACnetGenericArray)

BACnetListOfVTClass::BACnetListOfVTClass()
               :BACnetGenericArray(et)
{
   m_nType = vtcl;
}


BACnetListOfVTClass::BACnetListOfVTClass( BACnetAPDUDecoder& dec )
               :BACnetGenericArray(et)
{
   m_nType = vtcl;
   Decode(dec);
}


BACnetEncodeable * BACnetListOfVTClass::NewDecoderElement( BACnetAPDUDecoder& dec )
{
   BACnetEncodeable * penum = BACnetEncodeable::Factory(et, dec, VT_CLASSES_SUPPORTED);
   // TODO: Didn't the Factory's constructor call Decode?
   penum->Decode(dec);
   return penum;
}


BACnetEnumerated * BACnetListOfVTClass::operator[](int nIndex) const
{
   return (BACnetEnumerated *) m_apBACnetObjects[nIndex];
}


BACnetEnumerated & BACnetListOfVTClass::operator[](int nIndex)
{
   return  (BACnetEnumerated &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetListOfDestination, BACnetGenericArray)

BACnetListOfDestination::BACnetListOfDestination()
                  :BACnetGenericArray(destination)
{
   m_nType = reciplist;
}


BACnetListOfDestination::BACnetListOfDestination( BACnetAPDUDecoder& dec )
                  :BACnetGenericArray(destination)
{
   m_nType = reciplist;
   Decode(dec);
}


BACnetDestination * BACnetListOfDestination::operator[](int nIndex) const
{
   return (BACnetDestination *) m_apBACnetObjects[nIndex];
}


BACnetDestination & BACnetListOfDestination::operator[](int nIndex)
{
   return  (BACnetDestination &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetListOfRecipient, BACnetGenericArray)

BACnetListOfRecipient::BACnetListOfRecipient()
                 :BACnetGenericArray(recip)
{
   m_nType = tsrecip;
}


BACnetListOfRecipient::BACnetListOfRecipient( BACnetAPDUDecoder& dec )
                 :BACnetGenericArray(recip)
{
   m_nType = tsrecip;
   Decode(dec);
}


BACnetRecipient * BACnetListOfRecipient::operator[](int nIndex) const
{
   return (BACnetRecipient *) m_apBACnetObjects[nIndex];
}


BACnetRecipient & BACnetListOfRecipient::operator[](int nIndex)
{
   return  (BACnetRecipient &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetListOfCOVSubscription, BACnetGenericArray)

BACnetListOfCOVSubscription::BACnetListOfCOVSubscription()
                 :BACnetGenericArray(COVSub)
{
   m_nType = lCOVSub;
}


BACnetListOfCOVSubscription::BACnetListOfCOVSubscription( BACnetAPDUDecoder& dec )
                 :BACnetGenericArray(COVSub)
{
   m_nType = lCOVSub;
   Decode(dec);
}


BACnetCOVSubscription * BACnetListOfCOVSubscription::operator[](int nIndex) const
{
   return (BACnetCOVSubscription *) m_apBACnetObjects[nIndex];
}



BACnetCOVSubscription & BACnetListOfCOVSubscription::operator[](int nIndex)
{
   return  (BACnetCOVSubscription &) *m_apBACnetObjects[nIndex];
}

//====================================================================

IMPLEMENT_DYNAMIC(BACnetListOfAddressBinding, BACnetGenericArray)

BACnetListOfAddressBinding::BACnetListOfAddressBinding()
                     :BACnetGenericArray(dabindelem)
{
   m_nType = dabind;
}


BACnetListOfAddressBinding::BACnetListOfAddressBinding( BACnetAPDUDecoder& dec )
                     :BACnetGenericArray(dabindelem)
{
   m_nType = dabind;
   Decode(dec);
}


BACnetAddressBinding * BACnetListOfAddressBinding::operator[](int nIndex) const
{
   return (BACnetAddressBinding *) m_apBACnetObjects[nIndex];
}


BACnetAddressBinding & BACnetListOfAddressBinding::operator[](int nIndex)
{
   return (BACnetAddressBinding &) *m_apBACnetObjects[nIndex];
}


//====================================================================

IMPLEMENT_DYNAMIC(BACnetAnyValue, BACnetObjectContainer)

BACnetAnyValue::BACnetAnyValue()
{
   SetType(0);
}


BACnetAnyValue::BACnetAnyValue( BACnetEncodeable * pbacnetEncodeable )
               :BACnetObjectContainer(pbacnetEncodeable)
{
   SetType( (pbacnetEncodeable != NULL) ? pbacnetEncodeable->DataType() : 0 );
}


void BACnetAnyValue::SetType(int nNewType)
{
   m_nType = nNewType;
}


int BACnetAnyValue::GetType() const
{
   return m_nType;
}

int BACnetAnyValue::DataType(void) const
{
   return m_nType;
}

void BACnetAnyValue::SetObject( int nNewType, BACnetEncodeable * pbacnetEncodeable )
{
   SetType(nNewType);
   BACnetObjectContainer::SetObject(pbacnetEncodeable);
}


// Used to compute type instead of set it...

void BACnetAnyValue::SetObject( BACnetEncodeable * pbacnetEncodeable )
{
   if ( pbacnetEncodeable == NULL )
      SetType(0);
   else
      SetType(pbacnetEncodeable->DataType());

   BACnetObjectContainer::SetObject(pbacnetEncodeable);
}

// This method is called only from ScriptExecutor::ExpectALData with this == EPICS property
bool BACnetAnyValue::CompareToEncodedStream( BACnetAPDUDecoder & dec, int iOperator, LPCSTR lpstrValueName )
{
   bool retval = false;
   CString strThrowMessage;

   if (GetObject() == NULL)
   {
      // no data found
      strThrowMessage.Format(IDS_SCREX_COMPEPICSNULL, lpstrValueName);
   }
   else if (dec.pktBuffer == NULL)
   {
      // no data to decode
      strThrowMessage.Format(IDS_SCREX_COMPDATANULL, lpstrValueName);
   }
   else
   {
      // Make an object according to this's type and compare it
      // Factory, Decode, or Match may throw...
      BACnetEncodeable *pLeft = NULL;
      try
      {
         pLeft = Factory( GetType(), dec );
         if (pLeft != NULL)
         {
            retval = pLeft->Match(*GetObject(), iOperator, &strThrowMessage );
            delete pLeft;
         }
         else
         {
            strThrowMessage.Format(IDS_SCREX_COMPUNSUPPORTED, GetType() );
         }
      }
      catch (...)
      {
         // Don't leak, the re-throw the error
         delete pLeft;
         throw;
      }
   }

   if (!strThrowMessage.IsEmpty())
   {
      CString strFailString;
      strFailString.Format(IDS_SCREX_COMPFAIL, lpstrValueName, (LPCSTR) strThrowMessage );

      throw CString(strFailString);
   }

   return retval;
}


//
// BACnetAPDUTag::BACnetAPDUTag
//

BACnetAPDUTag::BACnetAPDUTag( BACnetTagClass tclass, BACnetApplicationTag tnum, int tlen )
   : tagClass(tclass), tagNumber(tnum), tagLVT(tlen)
{
}

BACnetAPDUTag::BACnetAPDUTag( BACnetApplicationTag tnum, int tlen )
   : tagClass(applicationTagClass), tagNumber(tnum), tagLVT(tlen)
{
}

BACnetAPDUTag::BACnetAPDUTag( int context, int tlen )
   : tagClass(contextTagClass), tagNumber((BACnetApplicationTag)context), tagLVT(tlen)
{
}

//
// BACnetAPDUTag::Encode
//
// Note that the context is unused, but required to override the pure virtual 
// member function of BACnetEncodeable.
//

void BACnetAPDUTag::Encode( BACnetAPDUEncoder& enc, int )
{
   int         len = 0;
   BACnetOctet tnum;

   // compute the tag length, including the data
   len = 1;
   if ((tagClass == openingTagClass) || (tagClass == closingTagClass))
      ;
   else if ((tagClass == applicationTagClass) && (tagNumber == booleanAppTag))
      ;
   else {
      // if we are context specific, use the context tag number
      tnum = (BACnetOctet)tagNumber;
      
      // extra big tag number?
      if (tnum >= 15)
         len += 1;
      
      // long lengths?
      if (tagLVT < 5)
         ;
      else if (tagLVT <= 253)
         len += 1;
      else if (tagLVT <= 65535)
         len += 3;
      else
         len += 5;

      // add the rest of the data
      len += tagLVT;
   }
   
   // check to see if there's enough space
   enc.CheckSpace( len );
   
   // check for special encoding of open and close tags
   if (tagClass == openingTagClass) {
      enc.pktBuffer[enc.pktLength++] = (((BACnetOctet)tagNumber & 0x0F) << 4) + 0x0E;
      return;
   }
   if (tagClass == closingTagClass) {
      enc.pktBuffer[enc.pktLength++] = (((BACnetOctet)tagNumber & 0x0F) << 4) + 0x0F;
      return;
   }

   // check for context encoding
   tnum = (BACnetOctet)tagNumber;
   if (tagClass == contextTagClass)
      enc.pktBuffer[enc.pktLength] = 0x08;
   else
      enc.pktBuffer[enc.pktLength] = 0x00;

   // this first byte is a killer
   enc.pktBuffer[enc.pktLength++] +=
      (((tnum < 15) ? tnum : 0x0F) << 4)
      + ((tagLVT < 5) ? tagLVT : 0x05);
   if (tnum >= 15)
      enc.pktBuffer[enc.pktLength++] = tnum;

   // really short lengths already done
   if (tagLVT < 5)
      ;
   else {
      if (tagLVT <= 253) {
         // byte lengths
         enc.pktBuffer[enc.pktLength++] = tagLVT;
      } else
      if (tagLVT <= 65535) {
         // short lengths
         enc.pktBuffer[enc.pktLength++] = 254;
         enc.pktBuffer[enc.pktLength++] = (tagLVT >> 8) & 0x0FF;
         enc.pktBuffer[enc.pktLength++] = tagLVT & 0x0FF;
      } else {
         // long lengths
         enc.pktBuffer[enc.pktLength++] = 255;
         enc.pktBuffer[enc.pktLength++] = (tagLVT >> 24) & 0x0FF;
         enc.pktBuffer[enc.pktLength++] = (tagLVT >> 16) & 0x0FF;
         enc.pktBuffer[enc.pktLength++] = (tagLVT >>  8) & 0x0FF;
         enc.pktBuffer[enc.pktLength++] = tagLVT & 0x0FF;
      }
   }
}

void BACnetAPDUTag::Decode( BACnetAPDUDecoder& dec )
{
   BACnetOctet tag;

   // enough for the tag byte?
   if (dec.pktLength < 1)
      throw_(77) /* not enough data */;

   tag = (dec.pktLength--,*dec.pktBuffer++);

   // extract the type
   tagClass = (BACnetTagClass)((tag >> 3) & 0x01);

   // extract the number
   tagNumber = (BACnetApplicationTag)(tag >> 4);
   if (tagNumber == 0x0F) {
      if (dec.pktLength < 1)
         throw_(78) /* not enough data */;
      tagNumber = (BACnetApplicationTag)(dec.pktLength--,*dec.pktBuffer++);
   }

   // extract the length
   tagLVT = (tag & 0x07);
   if (tagLVT < 5)
      ;
   else if (tagLVT == 5) {
      if (dec.pktLength < 1)
         throw_(79) /* not enough data */;
      tagLVT = (dec.pktLength--,*dec.pktBuffer++);
      if (tagLVT == 254) {
         if (dec.pktLength < 2)
            throw_(80) /* not enough data */;
         tagLVT = (dec.pktLength--,*dec.pktBuffer++);
         tagLVT = (tagLVT << 8) + (dec.pktLength--,*dec.pktBuffer++);
      } else if (tagLVT == 255) {
         if (dec.pktLength < 4)
            throw_(81) /* not enough data */;
         tagLVT = (dec.pktLength--,*dec.pktBuffer++);
         tagLVT = (tagLVT << 8) + (dec.pktLength--,*dec.pktBuffer++);
         tagLVT = (tagLVT << 8) + (dec.pktLength--,*dec.pktBuffer++);
         tagLVT = (tagLVT << 8) + (dec.pktLength--,*dec.pktBuffer++);
      }
   } else if (tagLVT == 6) {
      tagClass = openingTagClass;
      tagLVT = 0;
   } else if (tagLVT == 7) {
      tagClass = closingTagClass;
      tagLVT = 0;
   }
   
   // check for enough data (except for an application tagged boolean)
   if (!tagClass && (tagNumber == booleanAppTag))
      ;
   else if (dec.pktLength < tagLVT)
      throw_(82);
}

// This really a debug method to dump a parsed tag
void BACnetAPDUTag::Encode( CString &enc, Format /*theFormat*/ ) const
{
   switch (tagClass)
   {
   case applicationTagClass:
      enc.Format( "%s of length %d", 
                  NetworkSniffer::BAC_STRTAB_ApplicationTypes.EnumString(tagNumber), tagLVT );
      break;
   case contextTagClass:
      enc.Format( "[%d] of length %d", tagNumber, tagLVT );
      break;
   case openingTagClass:
      enc.Format( "Opening [%d]", tagNumber );
      break;
   case closingTagClass:
      enc.Format( "Closing [%d]", tagNumber );
      break;
   }
}


//----------
//
// BACnetNPDU::BACnetNPDU
//

BACnetNPDU::BACnetNPDU( const BACnetAddress &addr, const BACnetOctet *data, const int len, const int reply, const int priority )
   : pduAddr(addr), pduData(data), pduLen(len)
   , pduExpectingReply(reply), pduNetworkPriority(priority)
{
}

//
// BACnetNetClient::BACnetNetClient
//

BACnetNetClient::BACnetNetClient( void )
   : clientPeer(0)
{
}

//
// BACnetNetClient::~BACnetNetClient
//

BACnetNetClient::~BACnetNetClient( void )
{
   if (clientPeer)
      Unbind( this, clientPeer );
}

//
// BACnetNetClient::Request
//

void BACnetNetClient::Request( const BACnetNPDU &pdu )
{
   if (clientPeer)
      clientPeer->Indication( pdu );
}

//
// BACnetNetServer::BACnetNetServer
//

BACnetNetServer::BACnetNetServer( void )
   : serverPeer(0)
{
}

//
// BACnetNetServer::~BACnetNetServer
//

BACnetNetServer::~BACnetNetServer( void )
{
   if (serverPeer)
      Unbind( serverPeer, this );
}

//
// BACnetNetServer::Response
//

void BACnetNetServer::Response( const BACnetNPDU &pdu )
{
   if (serverPeer)
      serverPeer->Confirmation( pdu );
}

//
// Bind
//

void Bind( BACnetNetClientPtr cp, BACnetNetServerPtr sp )
{
   cp->clientPeer = sp;
   sp->serverPeer = cp;
}

//
// Unbind
//

void Unbind( BACnetNetClientPtr cp, BACnetNetServerPtr sp )
{
   cp->clientPeer = 0;
   sp->serverPeer = 0;
}

//
// IsBound
//

bool IsBound( BACnetNetClientPtr cp, BACnetNetServerPtr sp )
{
   return ((cp->clientPeer == sp) && (sp->serverPeer == cp));
}

#if _TSMDebug

//
// BACnetDebugNPDU::BACnetDebugNPDU
//

const char debugHex[] = "0123456789ABCDEF";

BACnetDebugNPDU::BACnetDebugNPDU( const char *lbl )
   : label(lbl)
{
}

//
// BACnetDebugNPDU::BACnetDebugNPDU
//

BACnetDebugNPDU::BACnetDebugNPDU( BACnetNetServerPtr sp, const char *lbl )
   : label(lbl)
{
   Bind( this, sp );
}

//
// BACnetDebugNPDU::Indication
//

void BACnetDebugNPDU::Indication( const BACnetNPDU &pdu )
{
   cout << '(' << label << " '";
   for (int i = 0; i < pdu.pduLen; i++) {
      cout << debugHex[ (pdu.pduData[i] >> 4) & 0x0F ];
      cout << debugHex[ pdu.pduData[i] & 0x0F ];
      cout << '.';
   }
   cout << "' to " << pdu.pduAddr << ')';
   cout << endl;
   
   Request( pdu );
}

//
// BACnetDebugNPDU::Confirmation
//

void BACnetDebugNPDU::Confirmation( const BACnetNPDU &pdu )
{
   cout << '(' << label << " '";
   for (int i = 0; i < pdu.pduLen; i++) {
      cout << debugHex[ (pdu.pduData[i] >> 4) & 0x0F ];
      cout << debugHex[ pdu.pduData[i] & 0x0F ];
      cout << '.';
   }
   cout << "' from " << pdu.pduAddr << ')';
   cout << endl;
   
   Response( pdu );
}

#endif

//
// BACnetPort::BACnetPort
//
// This ctor sets the port status to uninitialized.  It will be up to a derived 
// class to clear the status when the port is up and running.
//

BACnetPort::BACnetPort( void )
   : portStatus(-1)
{
}

//
// BACnetPort::~BACnetPort
//
// This dtor doesn't look like it does much, but it is virtual.  So when a pointer 
// to a BACnetPort is deleted, the real derived class dtor is called.  All classes 
// that have a virtual member function should have a virtual dtor.
//

BACnetPort::~BACnetPort( void )
{
}

//
// BACnetPort::FilterData
//
// The default for ports is to do no filtering.  This member function allows the 
// application to get a chance to see what is being sent and received.  It currently 
// does not allow the derived class to interfere with the processing of the packet,
// but that might be a nice feature to add.
//

void BACnetPort::FilterData( BACnetOctet *, int, BACnetPortDirection )
{
}

//
// BACnetPort::PortStatusChange
//
// This function is called when the port needs to change its status.  It can be overridden 
// by a derived class to do something with the new status.
//

void BACnetPort::PortStatusChange( void )
{
}

//----------

//
// BACnetAppClient::BACnetAppClient
//

BACnetAppClient::BACnetAppClient( void )
   : clientPeer(0)
{
}

//
// BACnetAppClient::~BACnetAppClient
//

BACnetAppClient::~BACnetAppClient( void )
{
   if (clientPeer)
      Unbind( this, clientPeer );
}

//
// BACnetAppClient::Request
//

void BACnetAppClient::Request( const BACnetAPDU &pdu )
{
   if (!clientPeer) throw_(83);
   clientPeer->Indication( pdu );
}

//
// BACnetAppServer::BACnetAppServer
//

BACnetAppServer::BACnetAppServer( void )
   : serverPeer(0)
{
}

//
// BACnetAppServer::~BACnetAppServer
//

BACnetAppServer::~BACnetAppServer( void )
{
   if (serverPeer)
      Unbind( serverPeer, this );
}

//
// BACnetAppServer::Response
//

void BACnetAppServer::Response( const BACnetAPDU &pdu )
{
   if (!serverPeer) throw_(84);
   serverPeer->Confirmation( pdu );
}

//
// Bind
//

void Bind( BACnetAppClientPtr cp, BACnetAppServerPtr sp )
{
   cp->clientPeer = sp;
   sp->serverPeer = cp;
}

//
// Unbind
//

void Unbind( BACnetAppClientPtr cp, BACnetAppServerPtr sp )
{
   cp->clientPeer = 0;
   sp->serverPeer = 0;
}

//
// IsBound
//

bool IsBound( BACnetAppClientPtr cp, BACnetAppServerPtr sp )
{
   return ((cp->clientPeer == sp) && (sp->serverPeer == cp));
}

