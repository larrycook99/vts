// ScriptBase.h: interface for the ScriptBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTBASE_H__18DBD511_B069_11D4_BEEA_00A0C95A9812__INCLUDED_)
#define AFX_SCRIPTBASE_H__18DBD511_B069_11D4_BEEA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

#include "BACnet.hpp"

//
// ScriptBase
//
// All of the script elements are derived from this base class that 
// provides basic container functionality and information to display 
// the heirarchy in the tree view.
//

class ScriptBase;
typedef ScriptBase *ScriptBasePtr;
class ScriptScanner;

class ScriptBase : public CList<ScriptBasePtr,ScriptBasePtr>
{
   public:
      enum ScriptBaseType
            { scriptNull
            , scriptSection
            , scriptTest
            , scriptDependency
            , scriptReference
            , scriptPacket
            , scriptCase
            , scriptCheck
            , scriptMake
            , scriptAssign  //Added by Zhu Zhenhua, 2003-12-24, to ASSIGN statement
            , scriptWait    //Added by Zhu Zhenhua, 2003-12-31, to WAIT statement
            };

      ScriptBase();
      virtual ~ScriptBase();

      UINT           healthCheck;
      ScriptBaseType baseType;                     // runtime type info
      CString        baseLabel;                    // label for tree view
      int            baseLineStart, baseLineCount; // where in source

      HTREEITEM      baseItem;                     // item handle when bound
      int            baseImage, baseStatus;        // index and offset of status

      ScriptBasePtr  baseParent;                   // pointer to parent

      CString        m_strFile;                    // full path to file element resides ("" if base)

      // list operations
      void Append( ScriptBasePtr sbp, ScriptScanner * pscan = NULL );  // add a child at the end
      void Remove( int indx );                     // remove a child

      bool IsValid() const;                        // return true for valid object
      void Validate() const;                       // assert on invalid object
      int Length( void );                          // number of children
      ScriptBasePtr Child( int indx );             // specific child
};

//
// ScriptSection
//
class ScriptSection : public ScriptBase
{
   public:
      ScriptSection( const CString& title );
};

typedef ScriptSection *ScriptSectionPtr;

//
// ScriptDependency
//
class ScriptDependency : public ScriptBase
{
   public:
      ScriptDependency( const CString& number );
};

typedef ScriptDependency *ScriptDependencyPtr;

//
// ScriptReference
//
class ScriptReference : public ScriptBase
{
   public:
      ScriptReference( const CString& clause );
};

typedef ScriptReference *ScriptReferencePtr;

//
// ScriptCase
//
class ScriptCase : public ScriptBase
{
   public:
      ScriptCase     *caseGroup;
      ScriptCase     *caseSubgroup;

      ScriptCase( const CString& number );
};

typedef ScriptCase *ScriptCasePtr;

//
// ScriptTest
//
class ScriptCommand;

class ScriptTest : public ScriptCase {
   public:
      ScriptTest( const CString& number );

      ScriptTest     *testNext;              // next test
//    ScriptPacket   *testFirstPacket;       // first packet in test
      ScriptCommand  *testFirstCommand;      // first packet in test
   };

typedef ScriptTest *ScriptTestPtr;

//
// ScriptToken
//
enum ScriptTokenType
      { scriptError                          // invalid character
      , scriptKeyword                        // word
      , scriptTitle                          // title string
      , scriptSymbol                         // operator or symbol
      , scriptValue                          // some kind of value, see encoding
      , scriptReference                      // EPICS reference name
      , scriptEOL                            // end of line
      , scriptEOF                            // end of file
      };

enum ScriptEncodingType
      { scriptIntegerEnc                     // [+|-](0..9)*
      , scriptFloatEnc                       // [+|-]N[.N][e[-]N]
      , scriptDecimalEnc                     // D'nnn'
      , scriptHexEnc                         // 0xFF, X'FF', &xFF
      , scriptOctalEnc                       // 0o377, O'377', &O377
      , scriptBinaryEnc                      // 0b11111111, B'11111111', &B11111111
      , scriptASCIIEnc                       // A'hi', "hi", 'hi'
      , scriptIPEnc                          // n.n.n.n[/n][:n]
      , scriptComplex                        // Complex hunk o' data, enclosed in brackets [] to parse
                                             //   madanner 11/6/02, should be text like a string
                                             //   without quotes.  Each complex data type decodes.
      };

struct ScriptTranslateTable {
   int      scriptKeyword;
   int      scriptValue;
   };

typedef ScriptTranslateTable *ScriptTranslateTablePtr;

//
// ScriptToken
//
class ScriptParmList;
typedef ScriptParmList *ScriptParmListPtr;

const int kTokenBufferSize = 8192;

class ScriptToken {
   private:
      void KillIndex();                         // madanner 10/24/02

   public:
      ScriptToken( void );                      // initialize
      virtual ~ScriptToken( void );             // madanner 10/24/02, added virtual

      ScriptToken( const ScriptToken &cpy );    // copy constructor allowed
      void operator =( const ScriptToken &ref );// assignment allowed  MAG 01FEB06 add void

      ScriptTokenType      tokenType;           // token type
      ScriptEncodingType   tokenEnc;            // value encoding
      int                  tokenSymbol;         // character code or keyword hash code
      int                  tokenLine;           // line number
      ULONGLONG            tokenOffset, tokenLength;   // char offset and length
      int                  m_nIndex;            // index into value when resolved (-1=all, 0=size, 1>= index)

      CString              tokenValue;          // token value
      bool                 m_fIgnoreEscape;     // madanner 6/03, true if quotes should ignore '\'

      ScriptToken *        pTokenIndex;         // madanner 10/24/02, pointer to token for possible index

      void SetIndex( ScriptToken * ptoken );    // madanner 10/24/02, index token chain helper
      void ResolveIndex(ScriptParmListPtr parms);  // madanner 10/24/02, index token chain built, time to resolve to number

      bool IsInteger( int &valu, ScriptParmListPtr parms = 0 ) const;   // acceptable integer form with parm lookup
      bool IsInteger( int &valu, ScriptTranslateTablePtr tp ) const; // with keyword lookup
      bool IsPropertyReference( int &prop ) const;             // with keyword lookup
      bool IsEncodeable( BACnetEncodeable &enc ) const;        // can this be interpreted?
      bool IsDontCare(void) const;                             // madanner 11/4/02, is this the '?' value?

      CString RemoveQuotes( void ) const;                      // remove quotes from strings

      static int HashCode( const char *key );                  // return a hash code
      static int Lookup( int code, ScriptTranslateTablePtr tp );  // find keyword in a table, -1 if not found
   };

typedef ScriptToken *ScriptTokenPtr;

//
// ScriptTokenList
//

class ScriptTokenList : public CList<ScriptTokenPtr,ScriptTokenPtr> {
   public:
      ScriptTokenList( void );
      ~ScriptTokenList( void );

      void Append( const ScriptToken& tok );       // add a token (makes a copy)

      int Length( void );                          // number of tokens
                                                   //madanner 10/24/02, const removed
      ScriptToken& operator []( int i );           // index operator
   };

typedef ScriptTokenList *ScriptTokenListPtr;

//
// ScriptScanner
//

class ScriptDocument;

class ScriptScanner {
   protected:

//madanner 6/03 for #include support
//    CEdit*         scanSource;                   // where this token came from (?)

      int         scanLine;                        // line number of source
      ULONGLONG   m_nLineOffset;                   // # of chars in file to current line start

      char        scanLineBuffer[kTokenBufferSize];  // current line being parsed
      char        scanValueBuffer[kTokenBufferSize]; // current token being built

      void FormatError( ScriptToken &tok, char *msg );   // sets up the token and throws the message
      void Deblank(void);                          // madanner 10/24/02, frequently called
      void ScanIndexTokens( ScriptToken & tok );   // madanner 10/24/02, scan for [] tokens
      void Initialize(void);

   public:
      const char     *scanSrc;                     // ptr to current/next token

      ScriptDocument * m_pdocSource;               // token comes from VTS document
      CStdioFile *   m_fileSource;                 // from include files

      ScriptScanner( ScriptDocument * pdoc );      // source of parsing
      ScriptScanner( CStdioFile * fileSource );    // source of parsing for include files
      ScriptScanner( const char *src );            // source of parsing
      ~ScriptScanner( void );

      void Next( ScriptToken& tok );               // put the next token in here
      void Peek( ScriptToken& tok );               // non-destructive look into stream
      void NextTitle( ScriptToken& tok );          // the rest of the line is a title
      void NextLine( ScriptToken& tok );           // move to the next line in the source

      void ReportSyntaxError( ScriptToken * ptok, LPCSTR lpszErrorMsg );
      CString GetPathName(void);
};

typedef ScriptScanner *ScriptScannerPtr;

typedef CTypedPtrArray<CPtrArray, ScriptScanner *> ScriptScanners;

#endif // !defined(AFX_SCRIPTBASE_H__18DBD511_B069_11D4_BEEA_00A0C95A9812__INCLUDED_)
