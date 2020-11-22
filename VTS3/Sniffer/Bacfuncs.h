// More BACnet decode and display

// Revision: Sep 18 2001 added new properties published in Addendum 135b to ANSI/ASHRAE Standard 135-1995
// Revision: Feb 3 2010  added new properties published ANSI/ASHRAE Standard 135-2008
// Revision: March 5 2010: rewrote most of it.
/**************************************************************************/

// Parse BACnet tags
class BacParser
{
public:
   // Constructor takes the offset of the first tag from buffer cursor.
   // Parses first tag
   BacParser( int theOffset );

   // Parse the next tag.  Return false for end of buffer, or
   // illegal tag (application tag with paired delimiter etc)
   // Upon return, offset points at first databyte, if any.
   bool ParseTag();

   // Consume the data for the current tag
   bool EatData();

   // Is there more in the buffer?
   bool HasMore() const;

   // Is the last tag parsed valid?
   bool IsValid() const { return m_isValid; }

   // Synchronize with the cursor, reset offset to 0
   void Synch();

   enum TagType { TAG_PRIMITIVE, TAG_OPEN, TAG_CLOSE };

   // Get the components of the current tag
   int          Offset() const { return m_offset; }
   unsigned int TagValue() const { return m_tagValue; }
   unsigned int DataLength() const { return m_dataLength; }
   bool         ContextTag() const { return m_contextTag; }
   bool         FixedTag() const { return (m_tagType == TAG_PRIMITIVE); }
   bool         OpeningTag() const { return (m_tagType == TAG_OPEN); }
   bool         ClosingTag() const { return (m_tagType == TAG_CLOSE); }
   TagType      GetTagType() const { return m_tagType; }

protected:
   int          m_offset;
   unsigned int m_tagValue;
   unsigned int m_dataLength;
   bool         m_contextTag;
   bool         m_isValid;
   TagType      m_tagType;
};

BacParser::BacParser( int theOffset )
: m_offset( theOffset )
{
   ParseTag();
}

// Is there more in the buffer?
bool BacParser::HasMore() const
{
   return (pif_offset + (int)m_offset < pif_end_offset);
}

// Synchronize with the cursor, reset offset to 0
void BacParser::Synch()
{
   m_offset = 0;
}

bool BacParser::ParseTag()
{
   // Must have at least one byte for the tag
   m_isValid = HasMore();
   if (m_isValid)
   {
      unsigned int tagbuff = pif_get_byte( m_offset );
      m_offset += 1;
      m_contextTag = (tagbuff & 0x08) != 0;
      m_tagValue   = (tagbuff & 0xF0)>>4;

      if (m_tagValue >= 15)
      {
         // Extended tag in next byte
         m_tagValue = pif_get_byte( m_offset );
         m_offset += 1;
      }

      m_tagType = TAG_PRIMITIVE;
      // Get the Length/Value/Type field
      m_dataLength = (tagbuff & 0x07);
      if (!m_contextTag && (m_tagValue < 2)) // is a "Value", no following bytes
      {
         // Application-tagged NULL or BOOLEAN: no length
         m_dataLength = 0;
      }
      else if (m_dataLength == 5)
      {
         // Extended length in next byte
         m_dataLength = pif_get_byte( m_offset );
         m_offset += 1;
         if (m_dataLength == 254)
         {
            // Two-byte extension
            m_dataLength = pif_get_word_hl( m_offset );
            m_offset += 2;
         }
         else if (m_dataLength == 255)
         {
            // Four-byte extension
            m_dataLength = (unsigned int)pif_get_long_hl( m_offset );
            m_offset += 4;
         }
      }
      else if (m_dataLength > 5)    // is a "Type"
      {
         // Paired tag
         m_tagType = (m_dataLength == 6) ? TAG_OPEN : TAG_CLOSE;
         m_dataLength = 0;

         // Paired tag MUST be a context tag
         m_isValid = m_contextTag;
      }

      if (m_isValid)
      {
         // Are we still in the buffer?
         // Allow one byte past the end so we can parse the final element
         m_isValid = (pif_offset + (int)m_offset <= pif_end_offset);
      }
   }

   if (!m_isValid)
   {
      // Buffer doesn't have enough data for this tag.  
      // Set bogus tag values so they don't match anything
      m_tagValue = ~0;
      m_contextTag = true;
      m_tagType    = TAG_PRIMITIVE;
      m_dataLength = 0;
   }

   return m_isValid;
}

// Consume the data for the current tag
bool BacParser::EatData()
{
   bool retval = true;
   if (m_tagType == TAG_OPEN)
   {
      // Opening tag.  Consume tagged items until the matching
      // closing tag
      unsigned int searchTag = m_tagValue;
      while (retval)
      {
         // Save the location of the start of the next tag
         unsigned int savedOffset = m_offset;
         retval = ParseTag();
         if (!retval)
            break;
         
         if (ClosingTag() && (m_tagValue == searchTag))
         {
            // Matching closing tag.  Back off before the tag.
            m_offset = savedOffset;
            break;
         }
         else
         {
            // Opening or fixed tag.  Eat its contents
            retval = EatData();
         }
      }
   }
   else
   {
      // Skip over the data.  (Includes closing tag, with length 0)
      m_offset += m_dataLength;
   }

   return retval;
}

// Return a string containing text for the specified object type.
// If the type is undefined or in the proprietary range, the
// string will say that, and show the numeric value
const char* ObjectTypeString( unsigned int theObjectType )
{
   return BAC_STRTAB_BACnetObjectType.EnumString( theObjectType );
}

// Return a string containing text for the specified property identifier.
// If the proeprty is undefined or in the proprietary range, the
// string will say that, and sow the numeric value
const char* PropertyIdentifierString( unsigned int theIdentifier )
{
   return BAC_STRTAB_BACnetPropertyIdentifier.EnumString( theIdentifier );
}

// For vsprintf
#include <stdarg.h>

// Show an error message, claiming current position and length 1 octet
void ShowErrorDetail( const char *pFormat, ... )
{ 
   va_list args;
   va_start( args, pFormat );
   if (pFormat != NULL)
   {
      vsprintf( get_int_line(pi_data_current,pif_offset,1,NT_ERROR), 
                pFormat, args );
   }
   va_end(args);
}

enum BACnetSequenceParm
{
   BSQ_REQUIRED,
   BSQ_OPTIONAL,
   BSQ_CHOICE
};

//=============================================================================
// Class to simplify display of tagged PDUs
class BACnetSequence
{
public:
   BACnetSequence();
   ~BACnetSequence();

   // Has an error been encountered?
   bool IsOK() const { return m_ok; }

   // Force an error, stopping further decoding, return false
   bool Fail( const char *pFormat, ... );

   // Get previously parsed Object Identitier, or ~0 if none parsed
   unsigned int LastObjectIdentifier() const { return m_objectIdentifier; }

   // Get previously parsed Object Type, or -1 if none parsed
   int LastObjectType() const { return m_objectType; }

   // Get previously parsed propertyID, or -1 if none parsed
   int LastPropertyID() const { return m_propertyID; }

   // Get previously parsed property index, or -1 if none parsed
   int LastPropertyIndex() const { return m_propertyIndex; }

   // Get previously parsed unsigned Boolean, or false if none parsed
   bool LastBoolean() const { return m_lastBoolean; }

   // Get previously parsed unsigned integer, or ~0 if none parsed
   unsigned int LastUnsigned() const { return m_lastUnsigned; }
   
   // Get previously parsed enumeration, or ~0 if none parsed
   unsigned int LastEnumeration() const { return m_lastEnumerated; }
   
   // Return true of the PDU has more data.
   bool HasMore() const;
   
   // Parse the next tag if is has not already been parsed.
   // Return false if end of buffer or tag encoding error
   bool Parse();

   // Synchronize the parser with the cursor
   void Synch();
   
   // Validate the next item as a primitive against tag, do choice logic
   // if theTag >= 0: context tag, ignore theAppTag
   // else application tag theAppTag
   bool VetPrimitive( int theTag, int theAppTag = -1, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Get the parser for special low-level processing
   BacParser& Parser();

   // In the following methods, if m_ok is false, no
   // actions are taken.  If the function returns bool, it is false.

   // Begin a titled sequence not delimited by tags, as when a name is given
   // to a SEQUENCE.  
   // Returns true if no error has been encountered.
   // Call EndWrap() to declare the end of the sequence.
   //
   // Effects are:
   // - Shows the title as a header line in the detail
   // - Highlights data in the hex view until EndWrap
   bool BeginWrap( const char *pTitle );

   // Declare the end of a wrapped region.
   // The only visible effect it to end the highlighting of data in the hex view.
   void EndWrap();

   // Begin a SEQUENCE OF wrapped in opening/closing tags.
   // Returns true if the tag is found.  After decoding the contents,
   // call ClosingTag() to declare the end of the sequence.
   bool OpeningTag( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Decleare the end of a SEQUENCE OF.  Displays the closing tag.
   void ClosingTag();

   // Declare the beginning of a group of CHOICEs
   void BeginChoice();

   // Declare the end of a group of CHOICEs.
   // If a choice has not been found, declare error and return false
   bool EndChoice();

   // Declare the beginning of a list wrapped context tags.
   // Parse theTag as required opening tag
   // Followed by while (HasListElement()) to process the elements
   bool ListOf( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Declare the beginning of a list not wrapped in context tags.
   // The Title detail line selects the entire list in the hex view.
   // Followed by while (HasListElement()) to process the elements until closing tag or end of PDU
   bool ListOf( const char *pTitle );

   // Declare the beginning of an untitled list not wrapped in context tags.
   // Followed by while (HasListElement()) to process the elements until closing tag or end of PDU
   bool ListOf();

   // Test for end of list (called from a while after ListOf)
   bool HasListElement();

   // Use "while (ArrayOf())" to process an array property:
   // - if index is zero, decode and show unsigned array size, return false
   // - if index is positive, return true once, (and modify theIndex as a flag)
   // - if index is -1, begin all elements list: return true for each element (and modify theIndex as a flag)
   bool ArrayOf( int &theIndex );

   // In the following methods:
   // BSQ_REQUIRED: if tag is found, show error and set m_ok false;
   // BSQ_OPTIONAL: if tag is found, show, return true; else show nothing and return false
   // BSQ_CHOICE: if the tag is found, mark as choice, show, return true; else
   //             show nothing and return false;  Also error if choice already found

   // Parse and show a Null value.
   bool Null( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show a Boolean value.
   bool Boolean( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show a signed integer value.
   bool Integer( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show an unsigned integer value.
   bool Unsigned( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show a Real value.
   bool Real( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show a Double value.
   bool Double( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show an unsigned property array index.
   bool PropertyArrayIndex( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show an enumerated value.  If table is NULL or tableLen is 0, show only numeric value.
   bool Enumerated( int theTag, const char *pTitle, 
                    BACnetStringTable *pTheTable = NULL,
                    BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show a Property Identifier value.
   bool PropertyIdentifier( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show a text string value.
   bool TextString( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show an octet string value.
   bool OctetString( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show a bit string value.
   bool BitString( int theTag, const char *pTitle, 
                   BACnetStringTable *pTheTable = NULL,
                   BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show a Date value.
   bool Date( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show a Time value.
   bool Time( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show an Object Identifier value.
   bool ObjectIdentifier( int theTag, const char *pTitle, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Parse and show any tagged item
   // if allowContext is false, Fail if a context tag is seen
   // if allowContext is true, show single-delimiter tag as hex bytes,
   // show paired-delimiter tags by recursive descent
   bool AnyTaggedItem( bool allowContext = true);

protected:

   enum BACnetNestType
   {
      BNT_SEQUENCE,
      BNT_CHOICE,
      BNT_LIST,
      BNT_WRAP
   };

   struct Nester
   {
      Nester         *m_pNext;
      BACnetNestType m_nestType;
      int            m_tagValue;
//    const char     *m_pTitle;
      bool           m_foundChoice;
      int            m_firstLineIndex;
   };

   // Validate the next item against tag, do choice logic
   // if theTag >= 0: context tag, ignore theAppTag
   // else application tag theAppTag
   // theTagType is 0 for primitive tag, 6 for open tag, 7 for close tag
   bool Vet( int theTag, int theAppTag, 
             BacParser::TagType theTagType, BACnetSequenceParm theType = BSQ_REQUIRED );

   // Allocate and push a block on the stack
   void Push( BACnetNestType theNestType, int theTagValue, const char *pTheTitle );

   // Pop a block off the stack.  Fail if top of stack is of a different type
   // Caller must delete the return item.
   Nester* Pop( BACnetNestType theNestType );

   bool         m_ok;
   bool         m_parsed;
   BacParser    m_parser;
   Nester       *m_pStack;

   // Saved values for use in property value decoding
   unsigned int m_objectIdentifier;
   int          m_objectType;
   int          m_propertyID;
   int          m_propertyIndex;
   bool         m_lastBoolean;
   unsigned int m_lastUnsigned;
   unsigned int m_lastEnumerated;
};

BACnetSequence::BACnetSequence()
: m_ok(true)
, m_parser(0)
, m_pStack(NULL)
, m_objectIdentifier((unsigned int)(~0))
, m_objectType(-1)
, m_propertyID(-1)
, m_propertyIndex(-1)
, m_lastBoolean(false)
, m_lastUnsigned((unsigned int)(~0))
, m_lastEnumerated((unsigned int)(~0))
{
   m_parsed = m_parser.IsValid();   // because BacParser constructor parses
}

BACnetSequence::~BACnetSequence()
{
   if (m_ok)
   {
      int leftOver = pif_end_offset - pif_offset;
      if (leftOver > 0)
      {
         // Unparsed stuff at the end of the APDU
         sprintf( get_int_line(pi_data_current, pif_offset, leftOver, NT_ERROR), 
                  "%d unexpected bytes at end of APDU", leftOver );
      }
   }

   while (m_pStack != NULL)
   {
      Nester *pNext = m_pStack->m_pNext;
      delete m_pStack;
      m_pStack = pNext;
   }
}

// Force an error, stopping further decoding
bool BACnetSequence::Fail( const char *pFormat, ... )
{ 
   va_list args;
   va_start( args, pFormat );
   if (pFormat != NULL)
   {
      vsprintf( get_int_line(pi_data_current,pif_offset,1,NT_ERROR), 
                pFormat, args );
   }
   va_end(args);
   
   m_ok = false; 
   return false;
}

// Return true of the PDU has more data.
bool BACnetSequence::HasMore() const
{
   // True if we have an unprocessed tag, or more in the buffer
   return (m_parsed || m_parser.HasMore());
}

// Parse the next tag if is has not already been parsed
bool BACnetSequence::Parse()
{
   if (m_ok && !m_parsed)
   {
      m_parsed = m_parser.ParseTag();
   }

   return m_parsed;
}

// Get the parser for special low-level processing
BacParser& BACnetSequence::Parser()
{
   return m_parser; 
}

// Synchronize the parser with the cursor
void BACnetSequence::Synch()
{
   m_parser.Synch();
   m_parsed = false;
}

// Push a block on the stack
void BACnetSequence::Push( BACnetNestType theNestType, int theTagValue, const char *pTheTitle )
{
   Nester *pNest = new Nester;
   pNest->m_pNext    = m_pStack;
   pNest->m_nestType = theNestType;
   pNest->m_tagValue = theTagValue;
// pNest->m_pTitle   = pTheTitle;
   pNest->m_foundChoice = false;
   pNest->m_firstLineIndex = get_next_detail_index();

   m_pStack = pNest;
}

// Pop a block off the stack.  Fail if top of stack is of a different type
// Caller must delete the return item.
BACnetSequence::Nester* BACnetSequence::Pop( BACnetNestType theNestType )
{
   Nester *pNest = m_pStack;
   if (pNest != NULL)
   {
      if (pNest->m_nestType != theNestType)
      {
         Fail( "Probable bug in VTS: decoder nesting wants %u, got %u", 
               pNest->m_nestType, theNestType );
      }

      m_pStack = pNest->m_pNext;
   }
   else
   {
      Fail( "Probable bug in VTS: decoder nesting stack underflow" );
   }

   return pNest;
}

// Return a string describing the tag
static const char* WhatWeGot( const BacParser &theParser )
{
   char *pGot = TempTextBuffer();
   unsigned int tag = theParser.TagValue();
   if (!theParser.IsValid())
   {
      pGot = "end of buffer";
   }
   else if (theParser.ContextTag())
   {
      sprintf( pGot, "%s [%u]",
              (theParser.GetTagType() == BacParser::TAG_OPEN) ? "opening" :
              (theParser.GetTagType() == BacParser::TAG_CLOSE) ? "closing" :
              "primitive", tag );
   }
   else
   {
      strcpy( pGot, BAC_STRTAB_ApplicationTypes.EnumString( tag, "Application" ) );
   }

   return pGot;
}

// In the following methods:
// BSQ_REQUIRED: if tag is found, show error and set m_ok false;
// BSQ_OPTIONAL: if tag is found, show, return true; else show nothing and return false
// BSQ_CHOICE: if the tag is found, mark as choice, show, return true; else
//             show nothing and return false;  Also error if choice already found

// Validate the next item as a primitive against tag, do choice logic
// if theTag >= 0: context tag, ignore theAppTag
// else application tag theAppTag
bool BACnetSequence::VetPrimitive( int theTag, int theAppTag, BACnetSequenceParm theType )
{
   return Vet( theTag, theAppTag, BacParser::TAG_PRIMITIVE, theType );
}

// Verify the tag and do choice logic
bool BACnetSequence::Vet( int theTag, int theAppTag, 
                          BacParser::TagType theTagType, BACnetSequenceParm theType )
{
   bool retval = IsOK();
   if (retval)
   {
      if ((m_pStack != NULL) && (m_pStack->m_nestType == BNT_CHOICE) && 
          (theType == BSQ_CHOICE) && (m_pStack->m_foundChoice))
      {
         // Already found our choice.  Don't process this item
         retval = false;
      }
      else
      {
         retval = Parse();
         if (((theTag >= 0) && m_parser.ContextTag() && (theTag == (int)m_parser.TagValue()) && 
             (theTagType == m_parser.GetTagType()))
               ||
             ((theTag < 0) && !m_parser.ContextTag() && (theAppTag == (int)m_parser.TagValue())))
         {
            // Matches next tag
            if ((m_pStack != NULL) && (m_pStack->m_nestType == BNT_CHOICE) && (theType == BSQ_CHOICE))
            {
               // Found the choice
               m_pStack->m_foundChoice = true;
            }
         }
         else if (theType == BSQ_REQUIRED)
         {
            // No match.  Fatal for REQUIRED
            char *pExpected = TempTextBuffer();
            if (theTag >= 0)
            {
               sprintf( pExpected, "%s [%u]",
                        (theTagType == BacParser::TAG_OPEN) ? "(opening)" :
                        (theTagType == BacParser::TAG_CLOSE) ? "(closing)" :
                        "primitive", theTag );
            }
            else 
            {
               strcpy( pExpected, BAC_STRTAB_ApplicationTypes.EnumString( theAppTag, "Application" ) );
            }
            
            retval = Fail("Missing required tag: expected %s got %s", pExpected, WhatWeGot( m_parser ) );
         }
         else
         {
            // No match.  OK for OPTIONAL and CHOICE.
            retval = false;
         }
      }
   }

   return retval;
}

// Begin a titled sequence not delimited by tags, as when a name is given
// to a SEQUENCE.  
// Returns true if no error has been encountered.
// Call EndWrap() to declare the end of the sequence.
//
// Effects are:
// - Shows the title as a header line in the detail
// - Highlights data in the hex view until EndWrap
bool BACnetSequence::BeginWrap( const char *pTheTitle )
{
   bool retval = IsOK();
   if (retval)
   {
      // Push a marker
      Push( BNT_WRAP, -1, pTheTitle );

      sprintf(get_int_line(pi_data_current,pif_offset,1,NT_OPEN_TAG), pTheTitle);
   }

   return retval;
}

// Declare the end of a wrapped region.
// The only visible effect it to end the highlighting of data in the hex view.
void BACnetSequence::EndWrap()
{
   if (IsOK())
   {
      Nester *pNest = Pop( BNT_WRAP );
      if (pNest != NULL)
      {
         // Set the length of the wrap's start line
         set_detail_line_length( pNest->m_firstLineIndex, pif_offset );
         delete pNest;
      }
   }
}

// Begin a SEQUENCE OF wrapped in opening/closing tags.
// Returns true if the tag is found.  After decoding the contents,
// call ClosingTag() to declare the end of the sequence.
bool BACnetSequence::OpeningTag( int theTag, const char *pTheTitle, BACnetSequenceParm theType )
{
   bool retval = Vet( theTag, -1, BacParser::TAG_OPEN, theType );
   if (retval)
   {
      // Got an opening tag.  Remember it, and its title, so we can show closing tag
      Push( BNT_SEQUENCE, theTag, pTheTitle );

      // Show the opening tag, but do not dump its value
      retval = m_ok = show_head_context_tag( 0, pTheTitle, theTag, false );
      if (m_ok)
      {
         // Deconstruct the opening tag and advance cursor
         show_tag();
      }
      Synch();
   }

   return retval;
}

// Decleare the end of a SEQUENCE OF.  Displays the closing tag.
void BACnetSequence::ClosingTag()
{
   if (IsOK())
   {
      Nester *pNest = Pop( BNT_SEQUENCE );
      if (pNest != NULL)
      {
         // Vet and show closing tag
         if (Vet( pNest->m_tagValue, -1, BacParser::TAG_CLOSE, BSQ_REQUIRED ))
         {
            // Don't show as a header line, since we are indenting this under the 
            // header for the opening tag
            // m_ok = show_head_context_tag( 0, pNest->m_pTitle, pNest->m_tagValue, false );
            // if (m_ok)
            {
               // Deconstruct the closing tag and advance cursor
               show_tag();
            }
            Synch();
         }

         delete pNest;
      }
   }
}

// Declare the beginning of a group of CHOICEs
// TODO: some CHOICEs are context tagged, and all have titles.
// Should we add a tag and title to this function?
void BACnetSequence::BeginChoice()
{
   if (IsOK())
   {
      Push( BNT_CHOICE, -1, "" );
   }
}

// Declare the end of a group of CHOICEs.
// If a choice has not been found, declare error and return false
bool BACnetSequence::EndChoice()
{
   bool retval = IsOK();
   if (retval)
   {
      Nester *pNest = Pop( BNT_CHOICE );
      if (pNest != NULL)
      {
         if (!pNest->m_foundChoice)
         {
            retval = Fail("no valid choice");
         }

         delete pNest;
      }
   }

   return retval;
}

// Declare the beginning of a list wrapped context tags.
// Parse theTag as required opening tag
// Followed by while (HasListElement()) to process the elements
bool BACnetSequence::ListOf( int theTagValue, const char *pTheTitle, BACnetSequenceParm theType )
{
   bool retval = Vet( theTagValue, -1, BacParser::TAG_OPEN, theType );
   if (retval)
   {
      Push( BNT_LIST, theTagValue, pTheTitle );

      // Show the opening tag, but do not dump its value
      retval = m_ok = show_head_context_tag( 0, pTheTitle, theTagValue, false );
      if (m_ok)
      {
         // Deconstruct the tag and advance cursor
         show_tag();
      }
      Synch();
   }

   return retval;
}

// Declare the beginning of a list not wrapped in context tags.
// Followed by while (HasListElement()) to process the elements until closing tag or end of PDU
bool BACnetSequence::ListOf( const char *pTheTitle )
{
   bool retval = IsOK();
   if (retval)
   {
      // HasListElement uses tag value to control highlighting of text
      Push( BNT_LIST, -1, pTheTitle );

      // NT_OPEN_TAG tells the detail tree to open this branch
      sprintf(get_int_line(pi_data_current,pif_offset,1,NT_OPEN_TAG), pTheTitle);
   }

   return retval;
}

// Declare the beginning of an untitled list not wrapped in context tags.
// Followed by while (HasListElement()) to process the elements until closing tag or end of PDU
bool BACnetSequence::ListOf()
{
   bool retval = IsOK();
   if (retval)
   {
      // HasListElement uses tag value to control highlighting of text
      Push( BNT_LIST, -2, NULL );
   }

   return retval;
}

// Test for end of list (called from a while after ListOf)
bool BACnetSequence::HasListElement()
{
   bool retval = IsOK();
   if (retval)
   {
      Nester *pNest = m_pStack;
      if (pNest != NULL)
      {
         // Parse this byte and look for closing tag or end of PDU
         bool more = Parse();    // see if we reached the end 
         if ( !more || m_parser.ClosingTag() )
         {
            // End of the list, expected or otherwise
            int tag = pNest->m_tagValue;
            if (tag >= 0)
            {
               // List must be enclosed by the specified tag pair
               if (!more || (tag != (int)m_parser.TagValue()))
               {
                  Fail( "Missing or incorrect closing tag: expected [%u], got %s",
                        tag, WhatWeGot( m_parser ) );
               }
               else
               {
                  // Don't show as a header line, since we are indenting this under the 
                  // header for the opening tag
                  // m_ok = show_head_context_tag( 0, pNest->m_pTitle, pNest->m_tagValue, false );
                  // if (m_ok)
                  {
                     // Deconstruct the closing tag and advance cursor
                     show_tag();
                  }
               }
            }

            Synch();
            pNest = Pop( BNT_LIST );
            if ((pNest != NULL) && (pNest->m_tagValue != -2))
            {
               // Set the length of the wrap's start line
               // (tag of -2 means unwrapped, untitled list)
               set_detail_line_length( pNest->m_firstLineIndex, pif_offset );
            }
            delete pNest;

            // No more elements
            retval = false;
         }
      }
   }

   return retval;
}

// Use "while (ArrayOf())" to process an array property:
// - if index is zero, decode and show unsigned array size, return false
// - if index is positive, return true once, (and modify theIndex as a flag)
// - if index is -1, begin all-elements list: return true for each element (and modify theIndex as a flag)
// - other negative index values are used as control values by ArrayOf, and should not be passed
//   in on an initial call.
bool BACnetSequence::ArrayOf( int &theIndex )
{
   bool retval = false;

   if (theIndex == 0)
   {
      // Element 0: decode and show size, return false
      Unsigned( -1, "ArraySize" );
   }
   else if (theIndex > 0)
   {
      // Single element: return true ONCE
      theIndex = -3;
      retval = true;
   }
   else if (theIndex == -1)
   {
      // All elements: begin list, test for first element
      theIndex = -2;
      ListOf();
      retval = HasListElement();
   }
   else if (theIndex == -2)
   {
      // Subsequent elements of all-elements
      retval = HasListElement();
   }
   // Any other index value returns false.

   return retval;
}

// Parse and show a Null value.
bool BACnetSequence::Null( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   bool retval = VetPrimitive( theTag, PRIM_NULL, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Null";
      }
      
      if (theTag < 0)
      {
         // Application-tag
         sprintf( get_int_line(pi_data_current, pif_offset, 1, NT_ITEM_HEAD), 
                  "%s:  Null", pTitle );
      }
      else
      {
         // Context-tag
         sprintf( get_int_line(pi_data_current, pif_offset, 1, NT_ITEM_HEAD), 
                  "[%u] Null", theTag, pTitle );
      }

      show_tagged_data();
      Synch();
   }

   return retval;
}

bool BACnetSequence::Boolean( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   m_lastBoolean = false;
   bool retval = VetPrimitive( theTag, PRIM_BOOLEAN, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Boolean";
      }
      
      // The interest here is that app-tagged boolean has its value in
      // the bottom three bits of the tag, while context tag has it as a value byte.
      unsigned int boolVal;
      if (theTag < 0)
      {
         // Application-tag: value is the length/value field.
         // (NOT available from parser.DataLength(), which says length is 0)
         boolVal = pif_get_byte(0) & 0x01;
         sprintf( get_int_line(pi_data_current, pif_offset, 1, NT_ITEM_HEAD), 
                  "%s:  %s", pTitle, (boolVal) ? "TRUE" : "FALSE" );
      }
      else
      {
         // Context-tag: value octet follows
         boolVal = get_bac_unsigned(1, m_parser.DataLength());
         sprintf( get_int_line(pi_data_current, pif_offset, 2, NT_ITEM_HEAD), 
                  "[%u] %s:  %s", theTag, pTitle, (boolVal) ? "TRUE" : "FALSE" );
      }

      // Save the value for use in special decoders
      m_lastBoolean = (boolVal != 0);

      // This isn't ideal for context-tagged boolean: shows "tagged data X'01'"
      show_tagged_data();
      Synch();
   }

   return retval;
}

bool BACnetSequence::Unsigned( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   m_lastUnsigned = (unsigned int)(~0);
   bool retval = VetPrimitive( theTag, PRIM_UNSIGNED, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Unsigned";
      }
      
      show_head_unsigned( 1, pTitle, theTag);
      unsigned int len = show_tag();

      // Save the value for use in special decoders
      m_lastUnsigned = get_bac_unsigned( 0, len );

      show_bac_unsigned(len);
      Synch();
   }

   return retval;
}

bool BACnetSequence::Integer( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   bool retval = VetPrimitive( theTag, PRIM_SIGNED, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Integer";
      }
      
      show_head_signed( 1, pTitle, theTag);
      unsigned int len = show_tag();

      show_bac_signed(len);
      Synch();
   }

   return retval;
}

// Parse and show a Real value.
bool BACnetSequence::Real( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   bool retval = VetPrimitive( theTag, PRIM_REAL, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Real";
      }
      
      show_head_real( 1, pTitle, theTag);
      unsigned int len = show_tag();
      show_bac_real(len);
      Synch();
   }

   return retval;
}

// Parse and show a Double value.
bool BACnetSequence::Double( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   bool retval = VetPrimitive( theTag, PRIM_DOUBLE, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Double";
      }
      
      show_head_double( 0, pTitle, theTag);
      unsigned int len = show_tag();
      show_bac_double(len);
      Synch();
   }

   return retval;
}

// Parse and show a Date value.
bool BACnetSequence::Date( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   bool retval = VetPrimitive( theTag, PRIM_DATE, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Date";
      }
      
      show_head_date( 1, pTitle, theTag);
      unsigned int len = show_tag();
      show_bac_date(len);
      Synch();
   }

   return retval;
}

// Parse and show a Time value.
bool BACnetSequence::Time( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   bool retval = VetPrimitive( theTag, PRIM_TIME, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Time";
      }
      
      show_head_time( 1, pTitle, theTag);
      unsigned int len = show_tag();
      show_bac_time(len);
      Synch();
   }

   return retval;
}

// Parse and show an unsigned property array index.
bool BACnetSequence::PropertyArrayIndex( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   m_propertyIndex = -1;
   bool retval = VetPrimitive( theTag, PRIM_UNSIGNED, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Unsigned";
      }
      
      show_head_unsigned( 1, pTitle, theTag);
      unsigned int len = show_tag();

      // Save the array index for use in property value decoding
      m_propertyIndex = (int)get_bac_unsigned( 0, len );
      
      show_bac_unsigned(len);
      Synch();
   }

   return retval;
}

// Parse and show an enumerated value.  If table is NULL or tableLen is 0, show only numeric value.
bool BACnetSequence::Enumerated( int theTag, const char *pTitle, 
                                 BACnetStringTable *pTheTable,
                                 BACnetSequenceParm theType )
{
   m_lastEnumerated = (unsigned int) (~0);
   bool retval = VetPrimitive( theTag, PRIM_ENUMERATED, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Enumerated";
      }
      
      const char* const *pTable = (pTheTable) ? pTheTable->m_pStrings : NULL;
      unsigned int nStrings     = (pTheTable) ? pTheTable->m_nStrings : 0;
      show_head_enumerated( 1, pTitle, theTag, pTable, nStrings );

      unsigned int len = show_tag();

      // Save the value for use in special decoders
      m_lastEnumerated = get_bac_unsigned( 0, len );

      show_bac_enumerated(len);
      Synch();
   }

   return retval;
}

// Parse and show a Property Identifier value.
bool BACnetSequence::PropertyIdentifier( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   m_propertyID = -1;
   bool retval = VetPrimitive( theTag, PRIM_ENUMERATED, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Enumerated";
      }
      
      show_head_enumerated( 1, pTitle, theTag, 
                            BAC_STRTAB_BACnetPropertyIdentifier.m_pStrings, 
                            BAC_STRTAB_BACnetPropertyIdentifier.m_nStrings );
      
      unsigned int len = show_tag();

      // Save the ID for use in property value decoding
      m_propertyID = (int)get_bac_unsigned( 0, len );

      show_bac_enumerated(len);
      Synch();
   }

   return retval;
}

// Parse and show an Object Identifier value.
bool BACnetSequence::ObjectIdentifier( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   m_objectIdentifier =(unsigned int)(~0);
   m_objectType       = -1;
   bool retval = VetPrimitive( theTag, PRIM_OBJECT_IDENTIFIER, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Object Identifier";
      }
      
      show_head_obj_id(1, pTitle, theTag);
      unsigned int len = show_tag();

      // Save the Object ID and type for use in property value decoding
      m_objectIdentifier = get_bac_unsigned( 0, len );
      m_objectType = (int)(m_objectIdentifier >> 22);

      show_bac_object_identifier(len);
      Synch();
   }

   return retval;
}

// Parse and show a Bitstring value.
bool BACnetSequence::BitString( int theTag, const char *pTitle, 
                                BACnetStringTable *pTheTable,
                                BACnetSequenceParm theType )
{
   bool retval = VetPrimitive( theTag, PRIM_BIT_STRING, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Bit string";
      }
      
      show_head_bit_string(0, pTitle, theTag);
      unsigned int len = show_tag();

      int byteLength = m_parser.DataLength();
      int unusedBits = pif_get_byte(0);
      if ((byteLength < 2) || (unusedBits > 7))
      {
         return Fail("Error: bitstring claims %u bytes and %u unused bits", 
                     byteLength, unusedBits);
      }

      // Show the byte data (after unused-bits)
      pif_offset += 1;
      byteLength -= 1;
      sprintf(outstr,"%"FW"s = %%s","Bit string data");
      pif_show_nbytes_hex(outstr, byteLength);

      if (pTheTable != NULL)
      {
         // Backup the cursor to point at the data
         pif_offset -= byteLength;
         
         // Show a line with a string for each true bit
         int val, jx = 0, mask = 0;
         int bitLength = byteLength*8 - unusedBits;
         for (int ix = 0; ix < bitLength; ix++)
         {
            if (mask == 0)
            {
               mask = 0x80;
               val = pif_get_byte(jx);
               jx += 1;
            }

            if (val & mask)
            {
               if (ix < pTheTable->m_nStrings)
               {
                  sprintf( get_int_line(pi_data_current,pif_offset, byteLength), 
                           "%u: %s = TRUE", ix, pTheTable->m_pStrings[ix] );
               }
               else
               {
                  sprintf( get_int_line(pi_data_current,pif_offset, byteLength), 
                          "%u = TRUE", ix );
               }
            }
            mask = mask >> 1;
         }

         pif_offset += byteLength;
      }
      
      Synch();
   }

   return retval;
}

// Parse and show a Text string value.
bool BACnetSequence::TextString( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   bool retval = VetPrimitive( theTag, PRIM_CHARACTER_STRING, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Character string";
      }
      
      show_head_char_string(0, pTitle, theTag);
      unsigned int len = show_tag();

      show_bac_charstring(len);
      Synch();
   }

   return retval;
}

// Parse and show an Octet string value.
bool BACnetSequence::OctetString( int theTag, const char *pTitle, BACnetSequenceParm theType )
{
   bool retval = VetPrimitive( theTag, PRIM_OCTET_STRING, theType );
   if (retval)
   {
      if ((pTitle == NULL) || (*pTitle == 0))
      {
         pTitle = "Octet string";
      }
      
      show_head_octet_string(0, pTitle, theTag);
      unsigned int len = show_tag();
      show_bac_octetstring(len);
      Synch();
   }

   return retval;
}

// Parse and show any tagged item
// if allowContext is false, Fail if a context tag is seen
// if allowContext is true, show single-delimiter tag as hex bytes,
// show paired-delimiter tags by recursive descent
bool BACnetSequence::AnyTaggedItem( bool allowContext )
{
   Parse();
   bool retval = IsOK();
   if (retval)
   {
      if (!allowContext && m_parser.ContextTag())
      {
         Fail( "Unexpected %s", WhatWeGot( m_parser ) );
      }
      else
      {
         show_head_tagged_data();
         show_tagged_data();
         Synch();
      }
   }

   return retval;
}

/**************************************************************************/
/* The functions that follow are used to complement the PIF functions     */
/* supplied by Network General. There primary purpose is to allow greater */
/* flexibility in displaying the various BACnet constructs.               */
/**************************************************************************/

/*************************************************************************/
// Show byte at cursor according to format string
// Advance cursor
void bac_show_byte( const char *label, const char *format_str )
/*************************************************************************/
/* Advantage: Allows FW alignment */
{
   sprintf(outstr,"%"FW"s = ",label); /* Set up alignment of output */
   strcat(outstr,format_str);
   sprintf(get_int_line(pi_data_current,pif_offset,1),outstr,(unsigned char)pif_get_byte(0));
   pif_offset += 1;
}

const char* LookupName( int net, const unsigned char *addr, int len );

/*************************************************************************/
void bac_show_enetaddr( const char *label )
/*************************************************************************/
{
   const unsigned char *addr = (const unsigned char *)msg_origin + pif_offset;

   const char  *name;

   /* check for a broadcast */
   if (memcmp(addr,"\377\377\377\377\377\377",6) == 0)
      name = LookupName( 0, 0, 0 );
   else
      name = LookupName( 0, addr, 6 );

   sprintf(outstr,"%"FW"s = %%02X-%%02X-%%02X-%%02X-%%02X-%%02X",label); /* Set up alignment of output */
   sprintf(get_int_line(pi_data_current,pif_offset,6), outstr
           , pif_get_byte(0)
           , pif_get_byte(1)
           , pif_get_byte(2)
           , pif_get_byte(3)
           , pif_get_byte(4)
           , pif_get_byte(5)
          );
   pif_offset += 6;

   if (name) pif_append_ascii( ", %s", name );
}

/*************************************************************************/
void bac_show_bipaddr( const char *label )
/*************************************************************************/
{
   const char  *name = LookupName( 0, (const unsigned char *)msg_origin + pif_offset, 6 );

   sprintf(outstr,"%"FW"s = %%u.%%u.%%u.%%u:0x%%04X",label); /* Set up alignment of output */
   sprintf(get_int_line(pi_data_current,pif_offset,6), outstr
           , pif_get_byte(0)
           , pif_get_byte(1)
           , pif_get_byte(2)
           , pif_get_byte(3)
           , (unsigned short)pif_get_word_hl(4)
          );
   pif_offset += 6;

   if (name) pif_append_ascii( ", %s", name );
}

/*************************************************************************/
// Mask the byte at the cursor and show it using the format string.
// Set flag-bit indent 10 spaces left of the '=' in the format string
// Advance the cursor.
void bac_show_flag( const char *pStr, unsigned char mask )
/*************************************************************************/
/* Advantage: Allows flags to be shown with format specifiers other than
   just %d. Thus flags can be displayed in hex. */
{
   unsigned char x = pif_get_byte(0) & mask;
   sprintf(get_int_line(pi_data_current,pif_offset,1),pStr,x);

   pif_flagbit_indent = strcspn(pStr,"=") - 10;
   if (pif_flagbit_indent < 0)
      pif_flagbit_indent = 0;
   pif_offset += 1;
}

/**************************************************************************/
void bac_show_nbytes( unsigned int len, const char *str )
/**************************************************************************/
/* Advantage: Highlights n bytes without 99 byte limitation of
   pif_show_nbytes_hex. Does not do any interpretation */
{
   sprintf(get_int_line(pi_data_current,pif_offset,len),str);
   pif_offset += len;
}

/*************************************************************************/
void bac_show_word_hl( const char *label, const char *format_str )
/*************************************************************************/
/* Advantage: Allows FW alignment */
{
  sprintf(outstr,"%"FW"s = ",label); /* Set up alignment of output */
  strcat(outstr,format_str);
  sprintf(get_int_line(pi_data_current,pif_offset,2),outstr,
          pif_get_word_hl(0));
  pif_offset += 2;
}

/*************************************************************************/
void bac_show_long_hl( const char *label, const char *format_str )
/*************************************************************************/
/* Advantage: Allows FW alignment */
{
  sprintf(outstr,"%"FW"s = ",label); /* Set up alignment of output */
  strcat(outstr,format_str);
  sprintf(get_int_line(pi_data_current,pif_offset,4),outstr,
          pif_get_long_hl(0));
  pif_offset += 4;
}

/*************************************************************************/
void show_str_eq_str ( const char *str1, const char *str2, unsigned int len )
/*************************************************************************/
{
  sprintf(get_int_line(pi_data_current,pif_offset,len),
          "%"FW"s = %s",str1,str2);
}

/*************************************************************************/
// TODO: this sucker is an amazing piece of work...
// Used once times with mask 07 for length from show_tagged_data
// Show the byte at cursor-1
void bac_show_flagmask ( unsigned char mask, const char *str )
/*************************************************************************/
/* Advantage: Allows the value of the masked-off field to be displayed
   in a formatted string to right of multi-bit field */
{
   unsigned char bitptr,i,j,startbit,numbits,val,saveval,firstbit;
   char strvar[80];

/* Save the byte that is being displayed.                                */

   saveval = pif_get_byte(-1);

/* This part of the routine prepends blanks to the output string to line */
/* things up with the previous pif_show_flagbit or pif_show_flagmask.    */

   for (i=0;i<pif_flagbit_indent;i++) 
      strvar[i] = ' ';

/* This part of the routine evaluates the mask and finds the position of */
/* the starting '1' bit and the number of '1' bits. The MSB is bit 7.    */

   firstbit = 0;
   startbit = 0;
   numbits  = 0;
   val = mask;

   for (j=8;j>0;j--) {
      if ((val/(1<<(j-1)))==0) {
        if (firstbit) {
          numbits = startbit - (j-1);
          firstbit = 0;
        }
      }
      else {
         val=val-(1<<(j-1));
         if (!firstbit) {
            firstbit = 1;
            startbit = j-1;
         }
      }
      if ((j==1) && (firstbit))
         numbits = startbit + 1;
   }

/* This part of the routine takes the value to be placed in the bit field */
/* and encodes it as 0's and 1's, placing it in STRVAR in PIF format.     */
      
   bitptr = 7;
   while (bitptr > startbit) {
      if (bitptr == 3) {
         strvar[i] = ' ';
         i += 1;
      }
      strvar[i] = '.';
      bitptr -= 1;
      i += 1;
   }

/* saveval will now contain the value in the bit field, right shifted */

   saveval = ((saveval & mask)>>(8-((7-startbit)+numbits)));
   val = saveval;

   for (j=numbits;j>0;j--) {
      if (bitptr == 3) {
         strvar[i] = ' ';
         i += 1;
      }
      if ((val/(1<<(j-1)))==0) {
         strvar[i]='0';
      }
      else {
         strvar[i]='1';
         val=val-(1<<(j-1));
      }
      i += 1;
      bitptr -= 1;
   }
   for (j=8-((7-startbit)+numbits);j>0;j--) {
      if (bitptr == 3) {
         strvar[i] = ' ';
         i += 1;
      }
      strvar[i] = '.';
      i += 1;
      bitptr -= 1;
   }
   strvar[i] = '\0';
   strcat(strvar," = ");
   strcat(strvar,str);
   sprintf(get_int_line(pi_data_current,pif_offset-1,1),strvar,saveval);
}

/**************************************************************************/
// Show PDU bytes according to the specified propertyID, using
// objectType and index as necessary.
void show_bac_ANY( BACnetSequence &seq, int obj_type, unsigned int prop_id, int prop_idx )
{
   switch (prop_id)
   {
   case ACKED_TRANSITIONS:  /* bit string */
      seq.BitString( -1, "", &BAC_STRTAB_BACnetEventTransitionBits );
      break;
   case ACK_REQUIRED:  /* bit string */
      seq.BitString( -1, "", &BAC_STRTAB_BACnetEventTransitionBits );
      break;
   case ACTION: 
      switch (obj_type)
      {
      case 7: // Command Object: ARRAY OF (LIST OF BACnetActionCommands)
         while (seq.ArrayOf(prop_idx)) 
         {
            show_bac_action_list(seq);
         }
         break;
      case 12: // loop object: enumeration BACnetAction
         seq.Enumerated( -1, "", &BAC_STRTAB_BACnetAction );
         break;
      }
      break;
   case ACTION_TEXT:  /* ARRAY OF Character Strings */
      while (seq.ArrayOf(prop_idx)) 
      {
         seq.TextString( -1, "" );
      }
      break;
   case ACTIVE_TEXT:  /* Character String  */
      seq.TextString( -1, "" );
      break;
   case ACTIVE_VT_SESSIONS:
      if (seq.ListOf())
      {
         while (seq.HasListElement()) 
         {
            show_bac_VT_session(seq);
         }
      }
      break;
   case ALARM_VALUE:
      // BinaryPV for BI, BV
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetBinaryPV );
      break;
   case ALARM_VALUES:
   case FAULT_VALUES:
      if (seq.ListOf())
      {
         while (seq.HasListElement()) {
            switch (obj_type) {
            case 21: // LIFE_SAFETY_POINT
            case 22: // LIFE_SAFETY_ZONE
               // List of BACnetLifeSafetyState
               seq.Enumerated( -1, "", &BAC_STRTAB_BACnetLifeSafetyState );
               break;
            case 30: // ACCESS_DOOR
               // List of BACnetDoorAlarmState
               seq.Enumerated( -1, "", &BAC_STRTAB_BACnetDoorAlarmState );
               break;
            default: // Multistate input and value
               // List Unsigned for MI, MV
               seq.Unsigned( -1, "" );
               break;
            }
         }
      }
      break;
   case ALL:
      seq.Fail( "Illegal data claims to be ALL" );
      break;
   case ALL_WRITES_SUCCESSFUL:      /* Boolean  */
      seq.Boolean( -1, "" );
      break;
   case APDU_SEGMENT_TIMEOUT:    /* Unsigned */
      seq.Unsigned( -1, "" );
      break;
   case APDU_TIMEOUT:            /* Unsigned */
      seq.Unsigned( -1, "" );
      break;
   case APPLICATION_SOFTWARE_VERSION:  /* Character String  */
      seq.TextString( -1, "" );
      break;
   case ARCHIVE:  /* Boolean */
      seq.Boolean( -1, "" );
      break;
   case BIAS:   /*  Real  */
      seq.Real( -1, "" );
      break;
   case CHANGE_OF_STATE_COUNT:      /* 15 Unsigned  */
      seq.Unsigned( -1, "" );
      break;
   case CHANGE_OF_STATE_TIME:    /* 16 BACnet DateTime  */
      seq.Date( -1, "" );
      seq.Time( -1, "" );
      break;
   case CONTROLLED_VARIABLE_REFERENCE: /* 19 BACnetObjectPropertyReference  */
      show_bac_object_property_reference(seq);
      break;
   case CONTROLLED_VARIABLE_UNITS: /* 20 BACnetEngineeringUnits  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetEngineeringUnits );
      break;
   case CONTROLLED_VARIABLE_VALUE:  /*  Real  */
      seq.Real( -1, "" );
      break;
   case COV_INCREMENT:        // 22
      seq.Real( -1, "" );
      break;
   case DATE_LIST:  /* List of BACnetCalendarEntry  */
      if (seq.ListOf())
      {
         while (seq.HasListElement()) {
            show_calendar_entry( seq );
         }
      }
      break;
   case DAYLIGHT_SAVINGS_STATUS:  /* Boolean  */
      seq.Boolean( -1, "" );
      break;
   case DEADBAND:             // 25
      seq.Real( -1, "" );
      break;
   case DERIVATIVE_CONSTANT: /* Real  */
      seq.Real( -1, "" );
      break;
   case DERIVATIVE_CONSTANT_UNITS: /* Enumeration  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetEngineeringUnits );
      break;
   case DESCRIPTION:    /* 28 Character String  */
      seq.TextString( -1, "" );
      break;
   case DESCRIPTION_OF_HALT: /* Character String */
      seq.TextString( -1, "" );
      break;
   case SLAVE_ADDRESS_BINDING:
   case MANUAL_SLAVE_ADDRESS_BINDING:
   case DEVICE_ADDRESS_BINDING:  /* sequence of BACnetAddressBinding */
      if (seq.ListOf())
      {
         while (seq.HasListElement()) {
            seq.ObjectIdentifier( -1, "deviceObjectIdentifier" );
            show_bacnet_address( seq, "deviceAddress" );
         }
      }
      break;
   case DEVICE_TYPE: /*  Character String  */
      seq.TextString( -1, "" );
      break;
   case EFFECTIVE_PERIOD: /*  BACnetDateRange  */
      seq.Date( -1, "startDate" );
      seq.Date( -1, "endDate" );
      break;
   case ELAPSED_ACTIVE_TIME: /* Unsigned  */
      seq.Unsigned( -1, "" );
      break;
   case ERROR_LIMIT:
      seq.Real( -1, "" );
      break;
   case EVENT_ENABLE: /* Bitstring  */
      seq.BitString( -1, "", &BAC_STRTAB_BACnetEventTransitionBits );
      break;
   case EVENT_PARAMETERS: /* List of BACnetEventParameters  */
      show_bac_event_parameters(seq);
      break;
   case EVENT_STATE:  /*  BACnetEventState */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetEventState );
      break;
   case EVENT_TYPE: /* BACnetEventType  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetEventType );
      break;
   case EXCEPTION_SCHEDULE: /*  ARRAY of BACnet SpecialEvents  */
      while (seq.ArrayOf(prop_idx)) 
      {
         show_bac_special_event(seq);
      }
      break;
// case FAULT_VALUES:  see "ALARM_VALUES" case above
   case FEEDBACK_VALUE:
      // BinaryPV for binary output, Unsigned for multistate output
      if (obj_type == 4)
      {
         seq.Enumerated( -1, "", &BAC_STRTAB_BACnetBinaryPV );
      }
      else
      {
         seq.Unsigned( -1, "" );
      }
      break;
   case FILE_ACCESS_METHOD:  /* Enumerated  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetFileAccessMethod );
      break;
   case FILE_SIZE: /*  Unsigned  */
      seq.Unsigned( -1, "" );
      break;
   case FILE_TYPE: /*  Character String  */
      seq.TextString( -1, "" );
      break;
   case FIRMWARE_REVISION: /* Character String  */
      seq.TextString( -1, "" );
      break;
   case HIGH_LIMIT:
      switch (obj_type)
      {
      case 23: // ACCUMULATOR
         seq.Unsigned( -1, "" );
         break;
      default: // Analog, pulse converter
         seq.Real( -1, "" );
         break;
      }
      break;
   case INACTIVE_TEXT: /*  Character String  */
      seq.TextString( -1, "" );
      break;
   case IN_PROCESS: /* Boolean  */
      seq.Boolean( -1, "" );
      break;
   case INSTANCE_OF: /* Character String  */
      seq.TextString( -1, "" );
      break;
   case INTEGRAL_CONSTANT: /* Real  */
      seq.Real( -1, "" );
      break;
   case INTEGRAL_CONSTANT_UNITS: /* Enumerated  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetEngineeringUnits );
      break;
   case LIMIT_ENABLE:
      seq.BitString( -1, "", &BAC_STRTAB_BACnetLimitEnable );
      break;
   case LIST_OF_GROUP_MEMBERS:   /* list of ReadAccessSpecification */
      if (seq.ListOf())
      {
         while (seq.HasListElement())
         {
            show_read_access_spec(seq);
         }
      }
      break;
   case LIST_OF_OBJECT_PROPERTY_REFERENCES: /* List of object_property_references  */
      if (seq.ListOf())
      {
         while (seq.HasListElement())
         {
            show_bac_dev_obj_prop_ref(seq);
         }
      }
      break;
   case LIST_OF_SESSION_KEYS:
      if (seq.ListOf())
      {
         while (seq.HasListElement())
         {
            seq.OctetString( -1, "sessionKey" );
            show_bacnet_address( seq, "peerAddress" );
         }
      }
      break;
   case LOCAL_DATE: /* Date */
      seq.Date( -1, "" );
      break;
   case LOCAL_TIME: /*  Time  */
      seq.Time( -1, "" );
      break;
   case LOCATION: /* Character String  */
      seq.TextString( -1, "" );
      break;
   case LOW_LIMIT:
      switch (obj_type)
      {
      case 23: // ACCUMULATOR
         seq.Unsigned( -1, "" );
         break;
      default: // Analog, pulse converter
         seq.Real( -1, "" );
         break;
      }
      break;
   case MANIPULATED_VARIABLE_REFERENCE:
      show_bac_object_property_reference(seq);
      break;
   case MAXIMUM_OUTPUT: /* Real  */
      seq.Real( -1, "" );
      break;
   case MAX_APDU_LENGTH_ACCEPTED:  /* Unsigned  */
      seq.Unsigned( -1, "" );
      break;
   case MAX_PRES_VALUE:  /* Real  */
      seq.Real( -1, "" );
      break;
   case MINIMUM_OFF_TIME: /* Unsigned 32 */
      seq.Unsigned( -1, "" );
      break;
   case MINIMUM_ON_TIME: /* Unsigned 32 */
      seq.Unsigned( -1, "" );
      break;
   case MINIMUM_OUTPUT: /* Real  */
      seq.Real( -1, "" );
      break;
   case MIN_PRES_VALUE: /*  Real  */
      seq.Real( -1, "" );
      break;
   case MODEL_NAME:  /* Character String  */
      seq.TextString( -1, "" );
      break;
   case MODIFICATION_DATE:  /*  BACnetDateTime  */
      seq.Date( -1, "" );
      seq.Time( -1, "" );
      break;
   case NOTIFICATION_CLASS:
      seq.Unsigned( -1, "" );
      break;
   case NOTIFY_TYPE:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetNotifyType );
      break;
   case NUMBER_OF_APDU_RETRIES:  /*  Unsigned  */
      seq.Unsigned( -1, "" );
      break;
   case NUMBER_OF_STATES: /*  Unsigned  */
      seq.Unsigned( -1, "" );
      break;
   case OBJECT_IDENTIFIER: /*  BACnetObjectIdentifier  */
      seq.ObjectIdentifier( -1, "" );
      break;
   case OBJECT_LIST:
   case STRUCTURED_OBJECT_LIST:     /* Array of Structured View object IDs */
      while (seq.ArrayOf( prop_idx )) 
      {
         seq.ObjectIdentifier( -1, "" );
      }
      break;
   case OBJECT_NAME:  /* Character String  */
      seq.TextString( -1, "" );
      break;
   case OBJECT_PROPERTY_REFERENCE: /*  BACnetObjectPropertyReference */ 
      show_bac_object_property_reference(seq);
      break;
   case OBJECT_TYPE: /*  BACnetObjectType  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetObjectType );
      break;
   case PROP_OPTIONAL: 
      seq.Fail( "Illegal data claims to be OPTIONAL" );
      break;
   case OUT_OF_SERVICE: /* Boolean  */
      seq.Boolean( -1, "" );
      break;
   case OUTPUT_UNITS: /* BACnetEngineeringUnits  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetEngineeringUnits );
      break;
   case POLARITY: /* BACnetPolarity  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetPolarity );
      break;
   case PRESENT_VALUE: /*  Data Type depends on Object Type! */
      switch (obj_type) {
      case 0:  /* Analog_Input - Real */
      case 1:  /* Analog_Output - Real */
      case 2:  /* Analog_Value - Real */
      case 12: /* Loop - Real */
      case 24: /* Pulse Converter - Real */
         seq.Real( -1, "" );
         break;
      case 6:  /* Calendar - BOOLEAN */
         seq.Boolean( -1, "" );
         break;
      case 7:  /* Command - Unsigned */
      case 13: /* Multistate_Input - Unsigned */
      case 14: /* Multistate_Output - Unsigned */
      case 19: /* Multistate_Value - Unsigned */
      case 23: /* Accumulator -  Unsigned */
         seq.Unsigned( -1, "" );
         break;
      case 17: /* Schedule - ANY Primitive Type */
         seq.AnyTaggedItem(false);
         break;
      case 21: // LIFE_SAFETY_POINT
      case 22: // LIFE_SAFETY_ZONE
         seq.Enumerated( -1, "", &BAC_STRTAB_BACnetLifeSafetyState );
         break;
      case 3:  /* Binary_Input - BACnetBinaryPV */
      case 4:  /* Binary_Output - BACnetBinaryPV */
      case 5:  /* Binary_Value - BACnetBinaryPV */
         seq.Enumerated( -1, "", &BAC_STRTAB_BACnetBinaryPV );
          break;
      case 28: /* Load Control - Enumerated */
         seq.Enumerated( -1, "", &BAC_STRTAB_BACnetShedState );
         break;
      case 30: /* Access Door - Enumerated */
         seq.Enumerated( -1, "", &BAC_STRTAB_BACnetDoorValue );
         break;
      case 11: /* Group - List of Read Access Result */
         if (seq.ListOf())
         {
            while (seq.HasListElement())
            {
               show_read_access_result( seq );
            }
         }
         break;
      case 40: // characterstringValue - BACnetCharacterString
         seq.TextString( -1, "" );
         break;
      case 44: // datetimeValue BACnetDateTime  
         seq.Date( -1, "" );
         seq.Time( -1, "" );
         break;
      case 45: // integerValue:
         seq.Integer( -1, "" );
         break;
      case 48: // positiveIntegerValue:
         seq.Unsigned( -1, "" );
         break;

      // case 22: /* Life Safety Zone - No Present Value */
      // case 25: /* Event Log - No Present Value*/
      // case 26: /*  */
      // case 27: /* Trend Log Multiple - No Present Value*/
      // case 29: /* Structured View - No Present Value */
      default: 
         // We don't know what this is.
         // MAY be illegal, but it might also be
         // - a new object type we don't know
         // - some case where we haven't been given the object type,
         //   such as the parameters for ReadConditional
         seq.AnyTaggedItem(true);
         break;
      }
      break;
   case PRIORITY:
      while (seq.ArrayOf(prop_idx)) 
      {
         seq.Unsigned( -1, "" );
      }
      break;
   case PRIORITY_ARRAY: /* BACnetPriorityArray  */
      while (seq.ArrayOf(prop_idx)) 
      {
         show_bac_priority_value( seq, obj_type );
      }
      break;
   case PRIORITY_FOR_WRITING: /* Unsigned  */
      seq.Unsigned( -1, "" );
      break;
   case PROCESS_IDENTIFIER:
      seq.Unsigned( -1, "" );
      break;
   case PROGRAM_CHANGE: /*  BACnetProgramRequest  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetProgramRequest );
      break;
   case PROGRAM_LOCATION: /* Character String  */
      seq.TextString( -1, "" );
      break;
   case PROGRAM_STATE: /*  BACnetProgramState  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetProgramState );
      break;
   case PROPORTIONAL_CONSTANT: /*  Real  */
      seq.Real( -1, "" );
      break;
   case PROPORTIONAL_CONSTANT_UNITS: /* BACnetEngineeringUnits  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetEngineeringUnits );
      break;
   case PROTOCOL_CONFORMANCE_CLASS: /*  Unsigned  */
      seq.Unsigned( -1, "" );
      break;
   case PROTOCOL_OBJECT_TYPES_SUPPORTED: /* BACnetObjectTypesSupported  */
      seq.BitString( -1, "", &BAC_STRTAB_BACnetObjectType );
      break;
   case PROTOCOL_SERVICES_SUPPORTED: /* BACnetServicesSupported  */
      seq.BitString( -1, "", &BAC_STRTAB_BACnetServicesSupported );
      break;
   case PROTOCOL_VERSION: /*  Character String  */
      seq.Unsigned( -1, "" );
      break;
   case READ_ONLY: /*  Boolean  */
      seq.Boolean( -1, "" );
      break;
   case REASON_FOR_HALT: /*  BACnetProgramError  */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetProgramError );
      break;
   case RECIPIENT_LIST: // SEQEUNCE OF BACnetDestination
      if (seq.ListOf())
      {
         while (seq.HasListElement())
         {
            show_bac_destination( seq );
         }
      }
      break;
   case RELIABILITY:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetReliability );
      break;
   case RELINQUISH_DEFAULT:  /* Data Type depends on Object Type! */
      show_bac_commandable_value( seq, obj_type );
      break;
   case REQUIRED:  
      seq.Fail( "Illegal data claims to be REQUIRED" );
      break;
   case RESOLUTION:  /* Real */
      seq.Real( -1, "" );
      break;
   case SCHEDULE_DEFAULT:
      seq.AnyTaggedItem( false );
      break;
   case SEGMENTATION_SUPPORTED:  /* BACnetSegmentation */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetSegmentation );
      break;
   case SETPOINT:  /* Real */
      seq.Real( -1, "" );
      break;
   case SETPOINT_REFERENCE:  /* [0] BACnetObjectPropertyReference */
      if (seq.OpeningTag( 0, "setpointReference", BSQ_OPTIONAL ))
      {
         show_bac_object_property_reference( seq );
         seq.ClosingTag();
      }
      break;
   case STATE_TEXT :  /* Array of Character Strings */
      while (seq.ArrayOf(prop_idx)) 
      {
         seq.TextString( -1, "" );
      }
      break;
   case STATUS_FLAGS:  /*  Bit String */
      seq.BitString( -1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
      break;
   case SYSTEM_STATUS:  /* BACnetDeviceStatus */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetDeviceStatus );
      break;
   case TIME_DELAY:
      seq.Unsigned( -1, "" );
      break;
   case TIME_OF_ACTIVE_TIME_RESET:  /* BACnetDateTime   */
   case TIME_OF_STATE_COUNT_RESET:  /* BACnetDateTime   */
      seq.Date( -1, "" );
      seq.Time( -1, "" );
      break;
   case TIME_SYNCHRONIZATION_RECIPIENTS:  // SEQUENCE OF BACnetRecipient
   case UTC_TIME_SYNCHRONIZATION_RECIPIENTS:
   case RESTART_NOTIFICATION_RECIPIENTS:
   case COVU_RECIPIENTS:
      if (seq.ListOf())
      {
         while (seq.HasListElement())
         {
            show_bac_recipient( seq );
         }
      }
      break;
   case UNITS:  /* BACnetEngineering Units */
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetEngineeringUnits );
      break;
   case UPDATE_INTERVAL:  /*  Unsigned */
      seq.Unsigned( -1, "" );
      break;
   case UTC_OFFSET:  /*  Signed Integer  */
      seq.Integer( -1, "" );
      break;
   case VENDOR_IDENTIFIER:
      seq.Unsigned( -1, "" );
      break;
   case VENDOR_NAME:  /* Character String   */
      seq.TextString( -1, "" );
      break;
   case VT_CLASSES_SUPPORTED:  /*  List of BACnet VT Classes */
      if (seq.ListOf())
      {
         while (seq.HasListElement())
         {
            seq.Enumerated( -1, "vtCalss", &BAC_STRTAB_BACnetVTClass );
         }
      }
      break;
   case WEEKLY_SCHEDULE:  /* ARRAY of BACnetTimeValue */
      while (seq.ArrayOf(prop_idx)) 
      {
         if (seq.ListOf( 0, "day-schedule" ))
         {
            while (seq.HasListElement())
            {
               show_bac_time_value( seq );
            }
         }
      }
      break;
   case ATTEMPTED_SAMPLES:             // Unsigned 32     
      seq.Unsigned( -1, "" );
      break;
   case AVERAGE_VALUE:              // Real         
      seq.Real( -1, "" );
      break;
   case BUFFER_SIZE:                   // Unsigned 32   
      seq.Unsigned( -1, "" );
      break;
   case CLIENT_COV_INCREMENT:          // BACnetClientCOV  
      seq.BeginChoice();
      seq.Real( -1, "real-increment", BSQ_CHOICE );
      seq.Null( -1, "default-increment", BSQ_CHOICE );
      seq.EndChoice();
      break;
   case COV_RESUBSCRIPTION_INTERVAL:   // Unsigned 32
      seq.Unsigned( -1, "" );
      break;
   case EVENT_TIME_STAMPS:               // Array of BACnetTimeStamp
      while (seq.ArrayOf(prop_idx)) 
      {
         show_time_stamp( seq, -1, "" );
      }
      break;
   case LOG_BUFFER:                      // List of BACnetLogRecord
      switch (obj_type)
      {
      case 20:
         show_log_buffer( seq );          // TrendLog
         break;
      case 25:
         show_event_log_buffer( seq );    // EventLog
         break;
      case 27:
         show_log_multiple_buffer( seq ); // TrendLogMultiple
         break;
      default:
         // Object we don't know.
         // Show as a sequence of anything, terminated by closing tag
         if (seq.ListOf())
         {
            while (seq.HasListElement()) {
               seq.AnyTaggedItem();
            }
         }
         break;
      }
      break;
   case LOG_DEVICE_OBJECT_PROPERTY:
      if (obj_type == 20)
      {
         // Trend object: BACnetDeviceObjectPropertyReference
         show_bac_dev_obj_prop_ref( seq );
      }
      else
      {
         // Trend-Multiple object: ARRAY of BACnetDeviceObjectPropertyReference
         while (seq.ArrayOf(prop_idx)) 
         {
            show_bac_dev_obj_prop_ref( seq );
         }
      }
      break;
   case ENABLE:                  // Boolean    
      seq.Boolean( -1, "" );
      break;
   case LOG_INTERVAL:                  // Unsigned    
      seq.Unsigned( -1, "" );
      break;
   case MAXIMUM_VALUE:                 // Real       
      seq.Real( -1, "" );
      break;
   case MINIMUM_VALUE:              // Real
      seq.Real( -1, "" );
      break;
   case NOTIFICATION_THRESHOLD:        // Unsigned 32
      seq.Unsigned( -1, "" );
      break;
   case PROTOCOL_REVISION:          // Unsigned 32
      seq.Unsigned( -1, "" );
      break;
   case RECORDS_SINCE_NOTIFICATION: // Unsigned 32
      seq.Unsigned( -1, "" );
      break;
   case RECORD_COUNT:               // Unsigned 32       
      seq.Unsigned( -1, "" );
      break;
   case START_TIME:              // BACnetDateTime     
      seq.Date( -1, "" );
      seq.Time( -1, "" );
      break;
   case STOP_TIME:                  // BACnetDateTime   
      seq.Date( -1, "" );
      seq.Time( -1, "" );
      break;
   case STOP_WHEN_FULL:               // Boolean        
      seq.Boolean( -1, "" );
      break;
   case TOTAL_RECORD_COUNT:           // Unsigned 32    
      seq.Unsigned( -1, "" );
      break;
   case VALID_SAMPLES:               // Unsigned 32
      seq.Unsigned( -1, "" );
      break;
   case WINDOW_INTERVAL:             // Unsigned 32
      seq.Unsigned( -1, "" );
      break;
   case WINDOW_SAMPLES:                 // Unsigned 32
      seq.Unsigned( -1, "" );
      break;
   case MAXIMUM_VALUE_TIMESTAMP:        // BACnetDateTime
   case MINIMUM_VALUE_TIMESTAMP:        // BACnetDateTime
      seq.Date( -1, "" );
      seq.Time( -1, "" );
      break;
   case VARIANCE_VALUE:                 // Real
      seq.Real( -1, "" );
      break;
   case ACTIVE_COV_SUBSCRIPTIONS:
      if (seq.ListOf())
      {
         while (seq.HasListElement()) {
            show_bac_COV_Subscription( seq );
         }
      }
      break;
   case PROFILE_NAME:                 // 168
      seq.TextString( -1, "" );
      break;
   case MEMBER_OF:
      show_bac_dev_obj_prop_ref(seq);      
      break;
   case LAST_RESTORE_TIME:
      show_time_stamp( seq, -1, "" );
      break;
   case SLAVE_PROXY_ENABLE:
   case AUTO_SLAVE_DISCOVERY: // ARRAY OF BOOLEAN
      while (seq.ArrayOf(prop_idx)) 
      {
         seq.Boolean( -1, "" );
      }
      break;
   case DATABASE_REVISION:
      seq.Unsigned( -1, "" );
      break;
   case SCALE:             // BACnetScale
      show_bac_scale( seq );
      break;
   case DOOR_STATUS:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetDoorStatus );
      break;
   case LOCK_STATUS:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetLockStatus );
      break;
   case SECURED_STATUS:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetDoorSecuredStatus );
      break;
   case DOOR_ALARM_STATE:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetDoorAlarmState );
      break;
   case MAINTENANCE_REQUIRED:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetMaintenance );
      break;
   case NODE_TYPE:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetNodeType );
      break;
   case LOGGING_TYPE:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetLoggingType );
      break;
   case ACTUAL_SHED_LEVEL:       // BACnetShedLevel
   case EXPECTED_SHED_LEVEL:     // BACnetShedLevel
   case REQUESTED_SHED_LEVEL:    // BACnetShedLevel
      show_bac_shed_level( seq );
      break;
   case DOOR_MEMBERS:         // Array of BACnetDeviceObjectReference
   case SUBORDINATE_LIST:     // Array of BACnetDeviceObjectReference
      while (seq.ArrayOf(prop_idx)) 
      {
         show_bac_dev_obj_ref( seq );
      }
      break;
   case SUBORDINATE_ANNOTATIONS:  // Array of Character Strings
   case SHED_LEVEL_DESCRIPTIONS:  // Array of Character Strings
      while (seq.ArrayOf(prop_idx)) 
      {
         seq.TextString( -1, "" );
      }
      break;
   case SHED_LEVELS:          // Array of Unsigned
      while (seq.ArrayOf(prop_idx)) 
      {
         seq.Unsigned( -1, "" );
      }
      break;
   case BACKUP_AND_RESTORE_STATE:
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetBackupState );
      break;
   case BACKUP_PREPARATION_TIME:
   case RESTORE_COMPLETION_TIME:
   case RESTORE_PREPARATION_TIME:
      seq.Unsigned( -1, "" );    // Unsigned seconds
      break;
   case BIT_MASK:                // Bitstring Object: BITSTRING
      seq.BitString( -1, "", &BAC_STRTAB_BACnetEventTransitionBits );
      break;
   case BIT_TEXT:                // Bitstring Object: Array of character string
      while (seq.ArrayOf(prop_idx))
      {
         seq.TextString( -1, "" );
      }
      break;
   case IS_UTC:                  // DateTime Value Object: BOOLEAN
      seq.Boolean( -1, "" );
      break;
   case GROUP_MEMBERS:           // Global Group: array of BACnetDeviceObjectPropertyReference
      while (seq.ArrayOf(prop_idx)) 
      {
         show_bac_dev_obj_prop_ref( seq );
      }
      break;
   case GROUP_MEMBER_NAMES:      // Global Group: array of character string
      while (seq.ArrayOf(prop_idx))
      {
         seq.TextString( -1, "" );
      }
      break;
   case MEMBER_STATUS_FLAGS:     // Global Group: BACnetStatusFlags
      seq.BitString( -1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
      break;
   case REQUESTED_UPDATE_INTERVAL:  // Global Group: Unsigned 
   case COVU_PERIOD:                // Global Group: Unsigned 
      seq.Unsigned( -1, "" );
      break;

   // case COVU_RECIPIENTS: Global Group: List of BACnetRecipient handled above

   case EVENT_MESSAGE_TEXTS:        // Array[3] of character string
   case EVENT_MESSAGE_TEXTS_CONFIG: // Array[3] of character string
      while (seq.ArrayOf(prop_idx))
      {
         seq.TextString( -1, "" );
      }
      break;
   case EVENT_DETECTION_ENABLE:     // BOOLEAN 
   case EVENT_ALGORITHM_INHIBIT:    // BOOLEAN
      seq.Boolean( -1, "" );
      break;
   case EVENT_ALGORITHM_INHIBIT_REF: // BACnetObjectPropertyReference
      show_bac_object_property_reference(seq);
      break;
   case TIME_DELAY_NORMAL:          // Unsigned
      seq.Unsigned( -1, "" );
      break;
   case RELIABILITY_EVALUATION_INHIBIT: // BOOLEAN
      seq.Boolean( -1, "" );
      break;

   // case FAULT_PARAMETERS:        // EEO: BACnetFaultParameter
   // TODO: Big ugly CHOICE.

   case FAULT_TYPE:                 // EEO: BACnetFaultType (enum)
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetFaultType );
      break;
   case LOCAL_FORWARDING_ONLY:      // Notification Forwarder: BOOLEAN
      seq.Boolean( -1, "" );
      break;

   // case PROCESS_IDENTIFIER_FILTER: // Notification Forwarder: BACnetProcessIdSelection
   // TODO: CHOICE

   // case SUBSCRIBED_RECIPIENTS:   // Notification Forwarder: List of BACnetEventNotificationSubscription
   // TODO: LIST of SEQUENCE

   // case PORT_FILTER:             // Notification Forwarder: Array of BACnetPortPermission
   // TODO: Array of SEQUENCE

   // case AUTHORIZATION_EXEMPTIONS: // Access Credential LIST of BACnetAuthorizationExemption
   // TODO: LIST of enum

   case ALLOW_GROUP_DELAY_INHIBIT:  // Channel: BOOLEAN
      seq.Boolean( -1, "" );
      break;
   case CHANNEL_NUMBER:             // Channel: Unsigned
      seq.Unsigned( -1, "" );
      break;
   case CONTROL_GROUPS:             // Channel: Array of Unsigned32
   case EXECUTION_DELAY:            // Channel: Array of Unsigned
      while (seq.ArrayOf(prop_idx))
      {
         seq.Unsigned( -1, "" );
      }
      break;
   case LAST_PRIORITY:              // Channel: Unsigned
      seq.Unsigned( -1, "" );
      break;
   case WRITE_STATUS:               // Channel: BACnetWriteStatus
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetWriteStatus );
      break;
   case PROPERTY_LIST:              // Array of BACnetPropertyIdentifier
      while (seq.ArrayOf(prop_idx))
      {
         seq.Enumerated( -1, "", &BAC_STRTAB_BACnetPropertyIdentifier );
      }
      break;
   case SERIAL_NUMBER:              // Device: character string
      seq.TextString( -1, "" );
      break;
   case BLINK_WARN_ENABLE:          // Lighting: BOOLEAN
      seq.Boolean( -1, "" );
      break;
   case DEFAULT_FADE_TIME:          // Lighting: Unsigned
      seq.Unsigned( -1, "" );
      break;
   case DEFAULT_RAMP_RATE:          // Lighting: REAL
   case DEFAULT_STEP_INCREMENT:     // Lighting: REAL
      seq.Real( -1, "" );
      break;
   case EGRESS_TIME:                // Lighting: Unsigned
      seq.Unsigned( -1, "" );
      break;
   case IN_PROGRESS:                // Lighting: BACnetLIghtingInProgress
   case INSTANTANEOUS_POWER:        // Lighting: REAL
      seq.Real( -1, "" );
      break;

   // case LIGHTING_COMMAND:        // Lighting: BACnetLightingCommand
   // TODO: SEQUENCE

   case LIGHTING_COMMAND_DEFAULT_PRIORITY: // Lighting: Unsigned
      seq.Unsigned( -1, "" );
      break;
   case MAX_ACTUAL_VALUE:           // Lighting: REAL
   case MIN_ACTUAL_VALUE:           // Lighting: REAL
   case POWER:                      // Lighting: REAL
      seq.Real( -1, "" );
      break;
   case TRANSITION:                 // Lighting: BACnetLightingTransition
      seq.Enumerated( -1, "", &BAC_STRTAB_BACnetLightingTransition );
      break;
   case EGRESS_ACTIVE:              // Lighting: BOOLEAN (Last in 135_2012)
      seq.Boolean( -1, "" );
      break;

   default:
      // Show as a sequence of anything, terminated by closing tag
      if (seq.ListOf())
      {
         while (seq.HasListElement())
         {
            seq.AnyTaggedItem();
         }
      }
      break;
   }
}
