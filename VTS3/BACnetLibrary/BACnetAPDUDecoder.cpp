
#include "stdafx.h"

#include <string.h>

#include "BACnet.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
// BACnetAPDUDecoder
//
BACnetAPDUDecoder::BACnetAPDUDecoder( const BACnetOctet *buffer, int len )
   : pktBuffer(buffer), pktLength(len)
{
}

BACnetAPDUDecoder::BACnetAPDUDecoder( const BACnetAPDUEncoder &enc )
   : pktBuffer(enc.pktBuffer), pktLength(enc.pktLength)
{
}

BACnetAPDUDecoder::BACnetAPDUDecoder( const BACnetNPDU &npdu )
   : pktBuffer(npdu.pduData), pktLength(npdu.pduLen)
{
}

//
// BACnetAPDUDecoder::SetBuffer
//
void BACnetAPDUDecoder::SetBuffer( const BACnetOctet *buffer, int len )
{
   pktBuffer = buffer;
   pktLength = len;
}

//
// ExamineTag leaves buffer pointer and size unchanged.
//
void BACnetAPDUDecoder::ExamineTag( BACnetAPDUTag &t )
{
   const BACnetOctet *pkt = pktBuffer;    // save the packet pointer
   int               len = pktLength;     // save the packet length

   // call the (destructive) decoder
   t.Decode( *this );

   // restore the pointer and length
   pktBuffer = pkt;
   pktLength = len;
}

bool BACnetAPDUDecoder::ExamineOption(int nContext)   //Added by Zhu Zhenhua, 2004-6-14
{
   if (pktLength <= 0)
      return false;

   BACnetAPDUTag tag;

   try
   {
      ExamineTag(tag);
   }
   catch(...)
   {
      return false;
   }
   
   if (tag.tagClass == closingTagClass)
      return false;
   else
      return (tag.tagNumber == nContext); 
}
//
// BACnetAPDUDecoder::Skip
//
void BACnetAPDUDecoder::Skip( void )
{
   BACnetAPDUTag  tag;

   // extract the tag
   tag.Decode( *this );

   // don't update length for an application tagged boolean (there isn't any)
   if (!tag.tagClass && (tag.tagNumber == booleanAppTag))
      return;

   // skip the data
   // TODO: Should we check for and throw if we are called on a Context tag?
   pktBuffer += tag.tagLVT;
   pktLength -= tag.tagLVT;   // TODO: This was PLUS the length
}

//
// BACnetAPDUDecoder::CopyOctets
//
// A crude but effective way of copying out a chunk of information.  Note that
// this does not account for tagged data, nor does it check to make sure there
// is actually enough data in the buffer to copy out.  This routine is very
// seldom called, and when it is it's usually for a very short buffer (like
// one octet) so it is not worth the overhead of calling memcpy().
//
void BACnetAPDUDecoder::CopyOctets( BACnetOctet *buff, int len )
{
   while (len--)
      *buff++ = (pktLength--,*pktBuffer++);
}

//
// BACnetAPDUDecoder::ExtractData
//
// This routine is used to copy the data portion of tagged information into
// a buffer supplied by the caller.  The return value is the number of octets
// copied.
//
// This is a destructive call (the packet pointer and length is updated to the
// next tag).
//
int BACnetAPDUDecoder::ExtractData( BACnetOctet *buffer )
{
   BACnetAPDUTag  tag;

   // extract the tag
   tag.Decode( *this );

   // don't copy data for an application tagged boolean (there isn't any)
   if (!tag.tagClass && (tag.tagNumber == booleanAppTag))
      return 0;

   // copy out the data
   CopyOctets( buffer, tag.tagLVT );

   // return the length
   return tag.tagLVT;
}

//
// BACnetAPDUDecoder::ExtractTagData
//
int BACnetAPDUDecoder::ExtractTagData( BACnetOctet *buffer )
{
   int               taglen = 0;
   const BACnetOctet *start = pktBuffer;
   BACnetAPDUTag     tag;

   // extract the tag
   tag.Decode( *this );

   // copy the tag that was just decoded
   while (start != pktBuffer) {
      *buffer++ = *start++;
      taglen += 1;
   }

   // don't copy data for an application tagged boolean (there isn't any)
   if (!tag.tagClass && (tag.tagNumber == booleanAppTag))
      return taglen;

   // copy the data, which eats content
   CopyOctets( buffer, tag.tagLVT );

   // return the length, including the tag
   return (taglen + tag.tagLVT);
}

//
// BACnetAPDUDecoder::FindContext
//
bool BACnetAPDUDecoder::FindContext( int context, BACnetAPDUDecoder &dec )
{
   BACnetAPDUTag  tag;
   int      level = 0;

   // start where we are
   dec = *this;

   while (dec.pktLength != 0) {
      dec.ExamineTag( tag );

      if (tag.tagClass == openingTagClass) {
         if ((level == 0) && (tag.tagNumber == context))
            return true;
         level += 1;
      } else
      if (tag.tagClass == closingTagClass) {
         if (level == 0)
            return false;
         level -= 1;
      } else
      if ((level == 0) && (tag.tagClass == contextTagClass)) {
         if (tag.tagNumber == context)
            return true;
         else
         if (tag.tagNumber > context)
            return false;
      }

      dec.Skip();
   }

   // must not be around
   return false;
}

#if _TSMDebug
//
// ostream &operator <<(ostream &strm, const BACnetAPDUDecoder &dec )
//
// A handy debugging function.
//
ostream &operator <<(ostream &strm, const BACnetAPDUDecoder &dec )
{
   int               i, len;
   const static char hex[] = "0123456789ABCDEF";
   BACnetAPDUDecoder decCopy = dec;
   BACnetAPDUTag     t;
   BACnetOctet       buff[1024];
   char           msg[1024];

   while (decCopy.pktLength != 0) {
      try {
         decCopy.ExamineTag( t );
//       strm << "tagNumber : " << (int)t.tagNumber << endl;
//       strm << "tagClass : " << (int)t.tagClass << endl;
//       strm << "tagLVT : " << t.tagLVT << endl;
         
         switch (t.tagClass) {
            case contextTagClass:
               strm << "Context specific data (tag " << (int)t.tagNumber << "): ";
               len = decCopy.ExtractData( buff );
               for (i = 0; i < len; i++) {
                  strm << hex[(buff[i] >> 4) & 0x0F];
                  strm << hex[buff[i] & 0x0F];
                  strm << '.';
               }
               strm << endl;
               break;
               
            case openingTagClass: {
               BACnetOpeningTag  openingTag
               ;
               
               openingTag.Decode( decCopy );
               strm << "Opening tag : " << (int)t.tagNumber << endl;
               break;
               }
               
            case closingTagClass: {
               BACnetClosingTag  closingTag
               ;
               
               closingTag.Decode( decCopy );
               strm << "Closing tag : " << (int)t.tagNumber << endl;
               break;
               }
            
            case applicationTagClass:
               switch (t.tagNumber) {
                  case unusedAppTag:
                     throw_(1); // should never get here
                  
                  case nullAppTag: {
                     BACnetNull     nullValue
                     ;
                     
                     nullValue.Decode( decCopy );
                     strm << "Null" << endl;
                     break;
                     }
                     
                  case booleanAppTag: {
                     BACnetBoolean  boolValue
                     ;
                     
                     boolValue.Decode( decCopy );
                     strm << "Boolean : " << (int)boolValue.boolValue << endl;
                     break;
                     }
                     
                  case unsignedIntAppTag: {
                     BACnetUnsigned uintValue
                     ;
                     
                     uintValue.Decode( decCopy );
                     strm << "Unsigned integer : " << uintValue.uintValue << endl;
                     break;
                     }
                     
                  case integerAppTag: {
                     BACnetInteger  intValue
                     ;
                     
                     intValue.Decode( decCopy );
                     strm << "Integer : " << intValue.intValue << endl;
                     break;
                     }
                     
                  case realAppTag: {
                     BACnetReal  realValue
                     ;
                     
                     realValue.Decode( decCopy );
                     strm << "Real : " << realValue.realValue << endl;
                     break;
                     }
                     
                  case doubleAppTag: {
                     BACnetDouble   doubleValue
                     ;
                     
                     doubleValue.Decode( decCopy );
                     strm << "Double : " << doubleValue.doubleValue << endl;
                     break;
                     }
                     
                  case octetStringAppTag:
                     strm << "Octet string : ";
                     goto suckData;
                     
                  case characterStringAppTag:
                     strm << "Character string : ";
                     goto suckData;
                     
                  case bitStringAppTag:
                     strm << "Bit string : ";
                     
                  suckData:
                     len = decCopy.ExtractData( buff );
                     for (i = 0; i < len; i++)
                        if ((buff[i] > ' ') && (buff[i] <= '~'))
                           strm << (char)buff[i] << '.';
                        else {
                           strm << hex[(buff[i] >> 4) & 0x0F];
                           strm << hex[buff[i] & 0x0F];
                           strm << '.';
                        }
                     strm << endl;
                     break;
                     
                  case enumeratedAppTag: {
                     BACnetEnumerated  enumValue
                     ;
                     
                     enumValue.Decode( decCopy );
                     strm << "Enumerated : " << enumValue.m_enumValue << endl;
                     break;
                     }
                     
                  case dateAppTag: {
                     BACnetDate        dateValue
                     ;
                     
                     dateValue.Decode( decCopy );
                     strm << "Date : "
                        << "year = " << dateValue.year << " (" << dateValue.year + 1900 << ")"
                        << ", month = " << dateValue.month
                        << ", day = " << dateValue.day
                        << ", dayOfWeek = " << dateValue.dayOfWeek
                        << endl;
                     break;
                     }
                     
                  case timeAppTag: {
                     BACnetTime        timeValue
                     ;
                     
                     timeValue.Decode( decCopy );
                     strm << "Time : "
                        << "hour = " << timeValue.hour
                        << ", minute = " << timeValue.minute
                        << ", second = " << timeValue.second
                        << ", hundredths = " << timeValue.hundredths
                        << endl;
                     break;
                     }
                     
                  case objectIdentifierAppTag: {
                     BACnetObjectIdentifier  objId
                     ;
                     
                     objId.Decode( decCopy );
                     objId.Encode( msg );
                     strm << "Object Identifier : " << msg << endl;
                     break;
                     }
                                                        
                  case reservedAppTag13:
                     strm << "Reserved (13) : ";
                     goto suckData;
                                                        
                  case reservedAppTag14:
                     strm << "Reserved (14) : ";
                     goto suckData;
                                                        
                  case reservedAppTag15:
                     strm << "Reserved (15) : ";
                     goto suckData;
               }
            }
      }
      catch (...) {
         strm << "Decoding error" << endl;
         break;
      }
   }
   
   // final blank line
   strm << endl;
   
   return strm;
}
#endif
