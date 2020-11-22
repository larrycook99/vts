#ifndef _BACnet
#define _BACnet

#ifndef _TSMDebug
#define _TSMDebug 0
#endif

#if _TSMDebug
#include <iostream.h>
#endif

#include "StringTables.h"
#include "VTS.h"
class VTSPort;

//
// General Typedefs
//

typedef unsigned char   BACnetOctet, *BACnetOctetPtr; // unsigned character

//
// General Constants
//

const unsigned    kVendorID = 260;  // BACnet Stack at SourceForge Vendor ID
const int      kMaxAddressLen = 8;     // longest address supported

namespace PICS {
#include "db.h"
}
//
// BACnet Address
//
// These address types are listed in increasing order of complexity
//

enum BACnetAddressType
   { nullAddr
   , localBroadcastAddr
   , localStationAddr
   , remoteBroadcastAddr
   , remoteStationAddr
   , globalBroadcastAddr
   };

struct BACnetAddress
{
   // TODO: aside from nullAddr, the type is redundant.
   // Eliminating it (or replacing it with a method that derives the type)
   // would simplify various code.
   // - if addrLen == 0, then the address is a broadcast, else "station"
   // - if addrNet is 0xFFFF, then global (must be broadcast)
   // - if addrNet is "this network", then local, else remote
   BACnetAddressType addrType;

   unsigned short    addrNet;
   unsigned short    addrLen;
   unsigned char     addrAddr[kMaxAddressLen];

   BACnetAddress( const unsigned char *addr, const unsigned short len );
   BACnetAddress( const unsigned short net, const unsigned char *addr, const unsigned short len );
   BACnetAddress( const BACnetAddressType typ = nullAddr, const unsigned short net = 0, const unsigned char *addr = 0, const unsigned short len = 0 );
   BACnetAddress &operator =( const BACnetAddress &arg );

   // initializers (when constructor can't be used)
   void LocalStation( const unsigned char *addr, const unsigned short len );
   void RemoteStation( const unsigned short net, const unsigned char *addr, const unsigned short len );
   void LocalBroadcast( void );
   void RemoteBroadcast( const short net );
   void GlobalBroadcast( void );

   CString MacAddress() const;
   bool SetMacAddress( const char *addrString );

   // MAC address as text.  If port is provided, address format may be nicer (dotted decimal IP address, etc.)
   CString PrettyMacAddress( VTSPort *pThePort = NULL ) const;

   // Network and MAC address as text.  If port is provided, address format may be nicer (dotted decimal IP address, etc.)
   CString PrettyAddress( VTSPort *pThePort = NULL ) const;
};

typedef BACnetAddress *BACnetAddressPtr;

bool operator ==( const BACnetAddress &addr1, const BACnetAddress &addr2 );
#if _TSMDebug
ostream &operator <<(ostream &strm,const BACnetAddress &addr);
#endif

//
// BACnetContext
//

const int kAppContext = -1;

// BACnetEncodeable

class BACnetAPDUEncoder;
class BACnetAPDUDecoder;

// madanner 9/26/02
// Decends from MFC base object for dynamic type checking, serialization and other goodies


class BACnetEncodeable : public CObject
{
protected:
   bool m_fDataInvalid;    // set to true when unsupported decoder methods called, madanner 9/04

public:
   BACnetEncodeable();

   // Format when encoding as a string
   enum Format
   {
      FMT_PLAIN,     // Unwrapped string for use in edit control etc.
      FMT_EPICS,     // Wrapped per EPICS rules: (date) and (objectID)
      FMT_SCRIPT     // Wrapped per script rules: [date] and [time]
   };

   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   const char* ToString( Format theFormat = FMT_EPICS ) const;
   void Peek( BACnetAPDUDecoder& dec );
   bool PreMatch( bool &isMatch, const CRuntimeClass *pClass, 
                  BACnetEncodeable &rbacnet, int iOperator, CString *pstrError = NULL ) const;
   bool PreMatchEquality( bool &isMatch, const CRuntimeClass *pClass, 
                          BACnetEncodeable &rbacnet, int iOperator, CString *pstrError = NULL ) const;
   bool DoEqualityMatch( bool areSame, BACnetEncodeable &rbacnet, int iOperator, CString *pstrError = NULL );

   // static factory
   static BACnetEncodeable * Factory( int nParseType, BACnetAPDUDecoder & dec, int nPropID = -1 );

   DECLARE_DYNAMIC(BACnetEncodeable)
};

typedef BACnetEncodeable *BACnetEncodeablePtr;


class BACnetAddr : public BACnetEncodeable
{
public:
   BACnetAddress  m_bacnetAddress;

   void AssignAddress(unsigned int nNet, BACnetOctet * paMAC, unsigned int nMACLen );

public:
   BACnetAddr();
   BACnetAddr( BACnetAddress * paddr );
   BACnetAddr( BACnetOctet * paMAC, unsigned int nMACLen );
   BACnetAddr( unsigned int nNet, BACnetOctet * paMAC, unsigned int nMACLen );
   BACnetAddr( BACnetAPDUDecoder & dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   BACnetAddr &operator =( const BACnetAddr & arg );

   DECLARE_DYNAMIC(BACnetAddr)
};

//
// BACnetPDU Atomic Types
//

class BACnetNull : public BACnetEncodeable
{
public:
   BACnetNull();
   BACnetNull( BACnetAPDUDecoder& dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetNull)
};


class BACnetBoolean : public BACnetEncodeable
{
public:
   enum eBACnetBoolean { bFalse = 0, bTrue = 1 };
   eBACnetBoolean    boolValue;

   BACnetBoolean( int bvalu = 0 );
   BACnetBoolean( bool bvalu );
   BACnetBoolean( BACnetAPDUDecoder& dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   BACnetBoolean & operator =( const BACnetBoolean & arg );

   DECLARE_DYNAMIC(BACnetBoolean)
};

class BACnetEnumerated : public BACnetEncodeable
{
private:
   const char* const *m_papNameList;
   int                m_nListSize;

public:
   int                m_enumValue;

   BACnetEnumerated( int evalu, const NetworkSniffer::BACnetStringTable &nameList );

   // Where possible, we suggest using the BACnetStringTable version when a
   // table of strings is required, to avoid maintenance hassles 
   BACnetEnumerated( int evalu = 0, const char* const *papNameList = NULL, int nListSize = 0 );

   BACnetEnumerated( BACnetAPDUDecoder& dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   void Encode( CString &enc, const char* const *table, int tsize ) const;
   void Decode( const char *dec, const char* const *table, int tsize );

   BACnetEnumerated & operator =( const BACnetEnumerated & arg );

   //madanner 9/04
   static BACnetEnumerated * Factory(int nPropID);

   DECLARE_DYNAMIC(BACnetEnumerated)
};

class BACnetUnsigned : public BACnetEncodeable
{
public:
   unsigned long  uintValue;

   BACnetUnsigned( unsigned long uivalu = 0 );
   BACnetUnsigned( BACnetAPDUDecoder& dec );

   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   BACnetUnsigned &operator = ( const BACnetUnsigned &arg ) //Added by Zhu Zhenhua, 2003-9-4
   {
      this->uintValue=arg.uintValue;
      return *this;  
   }
   DECLARE_DYNAMIC(BACnetUnsigned)
};

class BACnetInteger : public BACnetEncodeable
{
public:
   int            intValue;

   BACnetInteger( int ivalu = 0 );
   BACnetInteger( BACnetAPDUDecoder& dec );
   inline int getValue(void) { return intValue; }

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetInteger)
};

class BACnetReal : public BACnetEncodeable
{
public:
   float       realValue;

   BACnetReal( float rvalu = 0.0 );
   BACnetReal( BACnetAPDUDecoder& dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetReal)
};

class BACnetDouble : public BACnetEncodeable
{
public:
   double         doubleValue;

   BACnetDouble( double dvalu = 0.0 );
   BACnetDouble( BACnetAPDUDecoder & dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetDouble)
};

class BACnetCharacterString : public BACnetEncodeable
{
private:
   void Initialize( LPCSTR svalu );

public:
   int            strEncoding;   // encoding
   unsigned    strLen;        // number of octets
   BACnetOctet    *strBuff;      // pointer to data

   BACnetCharacterString( LPCSTR svalu = NULL );
   BACnetCharacterString( CString & rstr );
   BACnetCharacterString( BACnetCharacterString & cpy);
   BACnetCharacterString( BACnetAPDUDecoder& dec );
   virtual ~BACnetCharacterString( void );

   void SetValue( const char *svalu, int enc = 0 );

   bool Equals( const char *valu ); // true iff matches, must be ASCII encoding

   bool operator ==( const BACnetCharacterString &arg );
   bool operator !=( const BACnetCharacterString &arg );
   bool operator <=( const BACnetCharacterString &arg );
   bool operator >=( const BACnetCharacterString &arg );
   bool operator <( const BACnetCharacterString &arg );
   bool operator >( const BACnetCharacterString &arg );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   void KillBuffer(void);

   bool Match( BACnetCharacterString & rstring, int iOperator );

   DECLARE_DYNAMIC(BACnetCharacterString)
};

class BACnetOctetString : public BACnetEncodeable
{
public:
   int            strLen;        // number of octets
   int            strBuffLen;    // non-zero if owned
   BACnetOctet    *strBuff;      // pointer to data

   void PrepBuffer( int size );  // make buffer owned and at least (size) octets

   BACnetOctetString( void );
   BACnetOctetString( int len );
   BACnetOctetString( BACnetOctet *bytes, int len );
   BACnetOctetString( const BACnetOctet *bytes, int len );
   BACnetOctetString( const BACnetOctetString &cpy );
   BACnetOctetString( BACnetAPDUDecoder& dec );
   virtual ~BACnetOctetString( void );

   void Flush( void );
   void Append( BACnetOctet byte );
   void Insert( BACnetOctet *bytes, int len, int position );
   void Insert( const BACnetOctetString &cpy, int position );
   BACnetOctet &operator [](const int indx);
   void Reference( BACnetOctet *bytes, int len );
   int Length( void );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetOctetString)
};


class BACnetWeekNDay : public BACnetOctetString
{
private:
   int m_nMonth, m_nWeekOfMonth, m_nDayOfWeek;

   void LoadBuffer();
   void UnloadBuffer();

public:
   BACnetWeekNDay( void );
   BACnetWeekNDay( int nMonth, int nWeekOfMonth, int nDayOfWeek );
   BACnetWeekNDay( BACnetAPDUDecoder& dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   // virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   void Initialize( int nMonth, int nWeekOfMonth, int nDayOfWeek );
   int GetMonth() { return m_nMonth; };
   int GetWeekOfMonth() { return m_nWeekOfMonth; };
   int GetDayOfWeek() { return m_nDayOfWeek; };

   DECLARE_DYNAMIC(BACnetWeekNDay)
};


class BACnetBitString : public BACnetEncodeable
{
protected:
   int            bitLen;
   int            bitBuffLen;
   unsigned long  *bitBuff;

   void  SetSize( int siz );

public:
   BACnetBitString( void );
   BACnetBitString( int siz );
   BACnetBitString( int siz, unsigned char * pabBits );
   BACnetBitString( const BACnetBitString &cpy );
   BACnetBitString( BACnetAPDUDecoder& dec );
   virtual ~BACnetBitString( void );

   void  SetBit( int bit, int valu = 1 );
   void  ResetBit( int bit );
   int   GetBit( int bit ) const;
   int   GetBitCount() const { return bitLen; }

   const int operator [](int bit);
   BACnetBitString &operator +=( const int bit );
   BACnetBitString &operator -=( const int bit );

   BACnetBitString &operator =( const BACnetBitString &arg );
   BACnetBitString &operator |=( const BACnetBitString &arg );
   BACnetBitString &operator &=( const BACnetBitString &arg );

   bool operator ==( BACnetBitString &arg );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   void LoadBitsFromByteArray( unsigned char * pabBits );
   void KillBuffer(void);

   DECLARE_DYNAMIC(BACnetBitString)
};


class BACnetDate : public BACnetEncodeable
{
public:
   int         year;                // year - 1900, 255 = don't care
   int         month;               // 1..12
   int         day;                 // 1..31
   int         dayOfWeek;           // 1 = Monday

   BACnetDate( void );
   BACnetDate( int y, int m, int d );
   BACnetDate( BACnetAPDUDecoder& dec );

   BACnetDate &operator =( const BACnetDate & arg );
   bool operator ==( const BACnetDate &arg );
   bool operator !=( const BACnetDate &arg );
   bool operator <=( const BACnetDate &arg );
   bool operator >=( const BACnetDate &arg );
   bool operator <( const BACnetDate &arg );
   bool operator >( const BACnetDate &arg );

   void  CalcDayOfWeek( void );              // compute dayOfWeek from date

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   // Convert to integer time in 100 nsec ticks since 1 Jan 1600 (FILETIME)
   LONGLONG AsInt(void) const;

   bool Match( BACnetDate & rdate, int iOperator );
   bool IsValid(void);

   static void TestDateComps(void);

   DECLARE_DYNAMIC(BACnetDate)

protected:
   void DecodeMonth( const char **ppDec );
   void DecodeDayOfMonth( const char **ppDec );
};

class BACnetTime : public BACnetEncodeable
{
public:
   int         hour;                // 0..23, 255 = don't care
   int         minute;                 // 0..59
   int         second;                 // 0..59
   int         hundredths;             // 0..99

   BACnetTime( void );
   BACnetTime( int hr, int mn = 0, int sc = 0, int hun = 0 );
   BACnetTime( BACnetAPDUDecoder& dec );

   BACnetTime &operator =( const BACnetTime & arg );
   bool operator ==( const BACnetTime &arg );
   bool operator !=( const BACnetTime &arg );
   bool operator <=( const BACnetTime &arg );
   bool operator >=( const BACnetTime &arg );
   bool operator <( const BACnetTime &arg );
   bool operator >( const BACnetTime &arg );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   bool Match( BACnetTime & rtime, int iOperator );
   bool IsValid(void);

   static void TestTimeComps(void);

   DECLARE_DYNAMIC(BACnetTime)
};


class BACnetDateTime : public BACnetEncodeable
{
public:
   BACnetDate  bacnetDate;
   BACnetTime  bacnetTime;

   BACnetDateTime(void);
   BACnetDateTime( int y, int m, int d, int hr, int mn, int sc, int hun = 0);
   BACnetDateTime( const BACnetDateTime &orig );
   // Lets add to the enjoyment of having a structure with the same name as a class
   // and give it its own copy constructor
   BACnetDateTime( const PICS::BACnetDateTime &orig );
   BACnetDateTime( BACnetAPDUDecoder& dec );

   BACnetDateTime &operator =( const BACnetDateTime & arg );
   bool operator ==( const BACnetDateTime &arg );
   bool operator !=( const BACnetDateTime &arg );
   bool operator <=( const BACnetDateTime &arg );
   bool operator >=( const BACnetDateTime &arg );
   bool operator <( const BACnetDateTime &arg );
   bool operator >( const BACnetDateTime &arg );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   bool Match( BACnetDateTime & rdatetime, int iOperator );

   DECLARE_DYNAMIC(BACnetDateTime)
};

class BACnetDateRange : public BACnetEncodeable
{
public:
   BACnetDate  bacnetDateStart, bacnetDateEnd;

   BACnetDateRange(void);
   BACnetDateRange( int y, int m, int d, int y2, int m2, int d2 );
   BACnetDateRange( BACnetAPDUDecoder& dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   BACnetDateRange &operator =( const BACnetDateRange & arg );
   bool operator ==( const BACnetDateRange &arg );
   bool operator !=( const BACnetDateRange &arg );
   bool operator <=( const BACnetDateRange &arg );
   bool operator >=( const BACnetDateRange &arg );
   bool operator <( const BACnetDateRange &arg );
   bool operator >( const BACnetDateRange &arg );

   LONGLONG GetSpan(void) const;

   bool Match( BACnetDateRange & rdaterange, int iOperator );

   DECLARE_DYNAMIC(BACnetDateRange)
};

class BACnetObjectIdentifier : public BACnetEncodeable 
{
public:
   unsigned int   objID;

   BACnetObjectIdentifier( BACnetAPDUDecoder& dec );
   BACnetObjectIdentifier( int objType = 0, int instanceNum = 0 );
   BACnetObjectIdentifier( unsigned int nobjID );
   BACnetObjectIdentifier( const BACnetObjectIdentifier& objectId ); // Jingbo Gao, Sep 11, 2004

   BACnetObjectIdentifier &operator =( const BACnetObjectIdentifier &arg );

   void SetValue( BACnetObjectType objType, int instanceNum );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   unsigned int GetObjectType() const{ return (objID>> 22) & 0x000003ff;} //Added by Zhu Zhenhua 2003-7-22, to Get Object Type
   unsigned int GetObjectInstance() const{ return objID & 0x003fffff;}

   DECLARE_DYNAMIC(BACnetObjectIdentifier)
};

//madanner 9/04
class BACnetObjectPropertyReference : public BACnetEncodeable
{
public:
   BACnetObjectIdentifier m_objID;
   BACnetEnumerated     m_bacnetenumPropID;
   int                  m_nIndex;

   BACnetObjectPropertyReference() {}
   BACnetObjectPropertyReference( BACnetAPDUDecoder& dec );
   BACnetObjectPropertyReference( unsigned int obj_id, unsigned int prop_id, int index = -1 );

   BACnetObjectPropertyReference &operator =( const BACnetObjectPropertyReference &arg );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetObjectPropertyReference)
};


//madanner 9/04
class BACnetDeviceObjectPropertyReference : public BACnetEncodeable
{
public:
   BACnetObjectPropertyReference m_objpropref;
   unsigned int            m_unObjectID;

   BACnetDeviceObjectPropertyReference() {}
   BACnetDeviceObjectPropertyReference( BACnetAPDUDecoder& dec );
   BACnetDeviceObjectPropertyReference( unsigned int obj_id, unsigned int prop_id, int index = -1, unsigned int devobj_id = 0xFFFFFFFF );

   BACnetDeviceObjectPropertyReference &operator =( const BACnetDeviceObjectPropertyReference &arg );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetDeviceObjectPropertyReference)
};

class BACnetDeviceObjectReference : public BACnetEncodeable
{
public:
   BACnetObjectIdentifier  m_objID;
   unsigned int            m_undevID; // optional

   BACnetDeviceObjectReference() {}
   BACnetDeviceObjectReference( BACnetAPDUDecoder& dec );
   BACnetDeviceObjectReference( unsigned int obj_id, unsigned int devobj_id = 0xFFFFFFFF );

   BACnetDeviceObjectReference &operator =( const BACnetDeviceObjectReference &arg );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetDeviceObjectReference)
};



// TODO: msdanner 9/2004 - These are not implemented yet.

class BACnetReadAccessSpecification : public BACnetEncodeable
{
public:
   BACnetReadAccessSpecification();
   BACnetReadAccessSpecification( BACnetAPDUDecoder& dec );

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetReadAccessSpecification)
};

class BACnetActionCommand : public BACnetEncodeable
{
public:
   BACnetActionCommand();
   BACnetActionCommand( BACnetAPDUDecoder& dec );

// BACnetActionCommand &operator =( const BACnetActionCommand &arg );

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetActionCommand)
};


class BACnetEventParameter : public BACnetEncodeable
{
public:
   BACnetEventParameter();
   BACnetEventParameter( BACnetAPDUDecoder& dec );

// BACnetEventParameter &operator =( const BACnetEventParameter &arg );

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetEventParameter)
};

class BACnetSessionKey : public BACnetEncodeable
{
public:
   BACnetSessionKey();
   BACnetSessionKey( BACnetAPDUDecoder& dec );

// BACnetSessionKey &operator =( const BACnetSessionKey &arg );

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetSessionKey)
};

class BACnetExceptionSchedule : public BACnetEncodeable
{
public:
   BACnetExceptionSchedule();
   BACnetExceptionSchedule( BACnetAPDUDecoder& dec );

// BACnetExceptionSchedule &operator =( const BACnetExceptionSchedule &arg );

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetExceptionSchedule)
};

class BACnetAddressBinding : public BACnetEncodeable
{
public: //modified by HUMENG
   BACnetObjectIdentifier  m_bacnetObjectID;
   BACnetAddr              m_bacnetAddr;

public:
   BACnetAddressBinding();
   BACnetAddressBinding( unsigned int nobjID, unsigned short nNet, BACnetOctet * paMAC, unsigned short nMACLen );
   BACnetAddressBinding( BACnetAPDUDecoder& dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   BACnetAddressBinding &operator =( const BACnetAddressBinding & arg );

   DECLARE_DYNAMIC(BACnetAddressBinding)
};

class BACnetTimeValue : public BACnetEncodeable
{
public:
   BACnetTimeValue();
   BACnetTimeValue( BACnetAPDUDecoder& dec );

// BACnetTimeValue &operator =( const BACnetTimeValue &arg );

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetTimeValue)
};

class BACnetVTSession : public BACnetEncodeable
{
public:
   BACnetVTSession();
   BACnetVTSession( BACnetAPDUDecoder& dec );

// BACnetVTSession &operator =( const BACnetVTSession &arg );

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetVTSession)
};

enum BACnetApplicationTag
{
   unusedAppTag               = -1,
   nullAppTag                 = 0,
   booleanAppTag              = 1,
   unsignedIntAppTag          = 2,
   integerAppTag              = 3,
   realAppTag                 = 4,
   doubleAppTag               = 5,
   octetStringAppTag          = 6,
   characterStringAppTag      = 7,
   bitStringAppTag            = 8,
   enumeratedAppTag           = 9,
   dateAppTag                 = 10,
   timeAppTag                 = 11,
   objectIdentifierAppTag     = 12,
   reservedAppTag13           = 13,
   reservedAppTag14           = 14,
   reservedAppTag15           = 15
};

class BACnetBinaryPriV : public BACnetEnumerated
{
public:
   BACnetBinaryPriV( BACnetAPDUDecoder& dec );
   BACnetBinaryPriV( int nValue = 0 );

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   // virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetBinaryPriV)
};

class BACnetObjectContainer : public BACnetEncodeable
{
public:
   BACnetEncodeable * pbacnetTypedValue; //Modified By Zhu Zhenhua 2003-9-10

public:
   //changed to public by HUMENG
   virtual void SetObject( BACnetEncodeable * pbacnetEncodeable );

   BACnetObjectContainer();
   BACnetObjectContainer( BACnetEncodeable * pbacnetEncodeable );
   virtual ~BACnetObjectContainer();

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   // virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   bool IsObjectType( CRuntimeClass * pruntimeclass );
   CRuntimeClass * GetObjectType(void);
   BACnetEncodeable *  GetObject();

   DECLARE_DYNAMIC(BACnetObjectContainer)
};


class BACnetPriorityValue : public BACnetObjectContainer
{
private:
   void CreateTypedObject( BACnetApplicationTag tag );

public:
   BACnetPriorityValue();
   BACnetPriorityValue( BACnetAPDUDecoder & dec );
   BACnetPriorityValue( BACnetEncodeable * pbacnetEncodeable );

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   //virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetPriorityValue)
};

///////////////////////////////////////////////////////////////////////////////////
//
// BACnetDaysOfWeek
////Added by Zhu Zhenhua 2003-9-4
class BACnetDaysOfWeek : public BACnetBitString
{
public:
   BACnetDaysOfWeek(
      bool bMon, bool bTue, bool bWed,
      bool bThu, bool bFri, bool bSat, 
      bool bSun
      );

   BACnetDaysOfWeek(void);

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   void StringToValue( const char *dec );

   DECLARE_DYNAMIC(BACnetDaysOfWeek)
};

///////////////////////////////////////////////////////////////////////////////////
//
// BACnetEventTransitionBits
////Added by Zhu Zhenhua 2003-9-4
class BACnetEventTransitionBits : public BACnetBitString
{
public:
   BACnetEventTransitionBits(
      bool bOffnormal, 
      bool bFault, 
      bool bNormal
      );

   BACnetEventTransitionBits(void);

   // Overrides of base methods
   // virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   // virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   // virtual BACnetEncodeable * clone(void);
   // virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetEventTransitionBits)
};

class BACnetCalendarEntry : public BACnetObjectContainer
{
public:

   BACnetCalendarEntry();
   BACnetCalendarEntry( BACnetAPDUDecoder & dec );
   BACnetCalendarEntry( BACnetEncodeable * pbacnetEncodeable );
   int      m_nChoice;

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   // virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetCalendarEntry)
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//    BACnetRecipient
//Added by Zhu Zhenhua 2003-9-4
class BACnetRecipient : public BACnetObjectContainer
{
public:
   int GetChoice();
   BACnetRecipient(void);
   BACnetRecipient(const BACnetRecipient &src);
   BACnetRecipient( BACnetAPDUDecoder & dec );
   BACnetRecipient( BACnetEncodeable * pbacnetEncodeable ); 

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   BACnetRecipient &operator = ( const BACnetRecipient &arg );
   DECLARE_DYNAMIC(BACnetRecipient)
};


class BACnetRecipientProcess : public BACnetEncodeable
{
public:

   ::BACnetRecipient    m_bacnetrecipient;
   ::BACnetUnsigned     m_bacnetunsignedID;

   BACnetRecipientProcess(void);
   BACnetRecipientProcess( BACnetAPDUDecoder & dec );
   BACnetRecipientProcess( ::BACnetRecipient & rcp, unsigned int nID );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   // virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   BACnetRecipientProcess &operator = ( const ::BACnetRecipientProcess &arg );

   DECLARE_DYNAMIC(BACnetRecipientProcess)
};



class BACnetCOVSubscription : public BACnetEncodeable
{
public:
   ::BACnetRecipientProcess         m_bacnetrecipprocess;
   ::BACnetObjectPropertyReference  m_bacnetobjpropref;
   ::BACnetBoolean               m_bacnetboolNotification;
   ::BACnetUnsigned              m_bacnetunsignedTimeRemaining;
   ::BACnetReal                  m_bacnetrealCOVIncrement;

public:
   BACnetCOVSubscription() {};
   BACnetCOVSubscription( ::BACnetRecipientProcess & rcp, ::BACnetObjectPropertyReference & objpr,
                          bool fNotify, unsigned int nTimeRemaining, float flCOVIncrement );
   BACnetCOVSubscription( BACnetAPDUDecoder& dec );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   BACnetCOVSubscription &operator =( const BACnetCOVSubscription & arg );

   DECLARE_DYNAMIC(BACnetCOVSubscription)
};


///////////////////////////////////////////////////////////////////////////////////
//
// BACnetDestination
//Added by Zhu Zhenhua, 2003-9-4
class  BACnetDestination : public BACnetEncodeable
{
public:
   BACnetDaysOfWeek           m_validDays;
   BACnetTime                 m_fromTime;
   BACnetTime                 m_toTime;
   BACnetRecipient            m_recipient;
   BACnetUnsigned             m_processID;
   BACnetBoolean              m_issueConfirmedNotifications;
   BACnetEventTransitionBits  m_transitions;

   BACnetDestination(
      BACnetDaysOfWeek &validDays,
      BACnetTime &fromTime,
      BACnetTime &toTime,
      BACnetRecipient &recipient,
      BACnetUnsigned &processID,
      BACnetBoolean &issueConfirmedNotifications,
      BACnetEventTransitionBits &transitions
      );

   BACnetDestination(void);
   BACnetDestination(const BACnetDestination &src);
   BACnetDestination( BACnetAPDUDecoder & dec );
   BACnetDestination &operator = ( const BACnetDestination &arg );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   //virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetDestination)
};


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//Added by Zhu Zhenhua  2003-8-27
// BACnetSequenceOf
// TODO: WTF are these mystical fifties?  Alas, duplicates of equates in PROPS.H
#define sequenceof      52       //BACnetSequenceOf
#define listof          53       //BACnetListOf
#define arrayof         54       //BACnetArrayOf

class BACnetUnsigned;

template<class MyDataType> 
class BACnetSequenceOf: public BACnetEncodeable 
{
protected:
   CList<MyDataType*,MyDataType*> listSequenceOf;
   int               m_typeID;
   int               m_itemTypeID;
public:
   int               m_nObjID;
public:
   BACnetSequenceOf(int typeID = sequenceof)
      :m_typeID(typeID)
   {
   }

   BACnetSequenceOf(const BACnetSequenceOf<MyDataType> &src)
      :m_typeID(src.m_typeID), 
      m_itemTypeID(src.m_itemTypeID), m_nObjID(src.m_nObjID)
   {
      for(int i=0; i < src.listSequenceOf.GetCount(); i++)
      {
         POSITION pos = src.listSequenceOf.FindIndex(i);
         MyDataType* pElem = new MyDataType(*(src.listSequenceOf.GetAt(pos)));
         listSequenceOf.AddTail(pElem);
      }
   }

   ~BACnetSequenceOf()
   {
      for(int i=0; i < listSequenceOf.GetCount(); i++)
      {
         POSITION pos = listSequenceOf.FindIndex(i);
         delete listSequenceOf.GetAt(pos);   
      }
   }

   void AddElement(MyDataType* pSrc)
   {
      MyDataType* pNewElem = new MyDataType(*pSrc);
      if(pNewElem != NULL)
      listSequenceOf.AddTail(pNewElem);
   }

   void DeleteElement(unsigned int index)
   {
      if(index >=(unsigned int) listSequenceOf.GetCount())
      {
         throw "The specified index is beyond the scope of the sequenceof.";
      }
      POSITION pos = listSequenceOf.FindIndex(index);
      MyDataType* pDeleteElem=listSequenceOf.GetAt(pos);

      if(pDeleteElem != NULL)
      {
         delete pDeleteElem;
         pDeleteElem = NULL;
      }

      listSequenceOf.RemoveAt(pos);
   }

   void DeleteAllElements()
   {
      for(int i=0; i < listSequenceOf.GetCount(); i++)
      {
         POSITION pos = listSequenceOf.FindIndex(i);
         delete listSequenceOf.GetAt(pos);
      }
      listSequenceOf.RemoveAll();
   }

   void SetElement(unsigned int index, MyDataType* src)
   {
      if((int)index >= listSequenceOf.GetCount())
         throw "The specified index is beyond the scope of the sequenceof.";
      POSITION pos = listSequenceOf.FindIndex(index);    
      MyDataType* elem=listSequenceOf.GetAt(pos);
      *elem = *src;
   }

   MyDataType* GetElement(unsigned int index) const
   {
      if(index>=(unsigned int)listSequenceOf.GetCount())
         throw "The specified index is beyond the scope of the sequenceof.";
      POSITION pos = listSequenceOf.FindIndex(index);       
      return listSequenceOf.GetAt(pos);
   }

   int GetItemCount() const
   {
      return listSequenceOf.GetCount();
   }

   const int GetItemType() const
   {
      MyDataType item;
      return item.DataType();
   }

   void SetItemType(int typeID)
   {
      m_itemTypeID = typeID;
   }

   void SetType(int typeID)
   {
      if(typeID == sequenceof || typeID == arrayof || typeID == listof)
         m_typeID = typeID;
      else
         throw "Bad typeID!";
   }

   MyDataType* operator[](const unsigned int index)
   {
      if(m_typeID != arrayof)
         throw "It's not an array.";
      
      if(index>=listSequenceOf.GetCount())
         throw "The specified index is beyond the scope of the sequenceof.";
      POSITION pos = listSequenceOf.FindIndex(index);
      return listSequenceOf.GetAt(pos);
   }

   BACnetSequenceOf<MyDataType>& operator=(const BACnetSequenceOf<MyDataType> &src)
   {
      if(this != &src)
      {
         this->DeleteAllElements();
         for(int i = 0; i < src.listSequenceOf.GetCount(); i++)
         {
            POSITION pos = src.listSequenceOf.FindIndex(i); 
            MyDataType* pElem = new MyDataType(*(src.listSequenceOf.GetAt(pos)));
            listSequenceOf.AddTail(pElem);
         }

         m_typeID = src.m_typeID; 
         m_itemTypeID = src.m_itemTypeID;
         m_nObjID = src.m_nObjID;
      }
      return *this;
   }

   bool operator == ( const BACnetSequenceOf<MyDataType> &arg )
   {
      if ( listSequenceOf.GetCount() != arg.GetItemCount() ) {
         return false;
      }
      for( int index = 0; index < listSequenceOf.GetCount(); index++ )
      {
         POSITION pos = listSequenceOf.FindIndex(index);    
         if ( !( *(listSequenceOf.GetAt(pos)) == *(arg.GetElement(index)) ) )  {
            return false;
         }
      }
      return true;
   }

   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext )
   {
      for(int i = 0; i < listSequenceOf.GetCount(); i++)
      {
         POSITION pos = listSequenceOf.FindIndex(i);  
         listSequenceOf.GetAt(pos)->Encode(enc);
      }
   }

   virtual void Decode( BACnetAPDUDecoder& dec )
   {
      DeleteAllElements();
      while(dec.pktLength != 0)
      {
         //if the code is closing tag then break
         BACnetAPDUTag tag;
         dec.ExamineTag(tag);
         if(tag.tagClass == closingTagClass)
            break;

         MyDataType* temp = new MyDataType();
//       switch(temp->DataType()) 
//       {
//       case BACNET_READACCESSRESULT_LISTOFRESULT:
//       // ((ReadAccessResult::ListOfResults*)temp)->SetObjectID(m_nObjID);
//          break;
//       case BACNET_WRITEACCESSSPECIFICATION:
//       // ((BACnetPropertyValue*)temp)->SetObjectID(m_nObjID);
//          break;
//       default:
//          break;
//       }

         temp->Decode(dec);
         listSequenceOf.AddTail(temp);
      }
   }

   virtual int DataType(void) const
   {
      return m_typeID;
   }
// DECLARE_DYNAMIC(BACnetSequenceOf)
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//
// BACnetListOf
//
template<class MyDataType >
class BACnetListOf : public BACnetSequenceOf<MyDataType>
{
public:
   BACnetListOf(void):BACnetSequenceOf<MyDataType>()
   {
      m_typeID = listof;
   }
};
template<class MyDataType >
class BACnetArrayOf : public BACnetSequenceOf<MyDataType>
{
public:
   BACnetArrayOf(void):BACnetSequenceOf<MyDataType>()
   {
      m_typeID = arrayof;
   }
};
class BACnetTimeStamp : public BACnetObjectContainer
{
public:
   BACnetTimeStamp();
   BACnetTimeStamp( BACnetAPDUDecoder & dec );
   BACnetTimeStamp( BACnetEncodeable * pbacnetEncodeable );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   //virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetTimeStamp)
};

//Shiyuan Xiao 
class BACnetScale : public BACnetObjectContainer
{
public:
   BACnetScale();
   BACnetScale(float value);
   BACnetScale(int value);
   BACnetScale( BACnetAPDUDecoder & dec );
   BACnetScale( BACnetEncodeable * pbacnetEncodeable );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   //virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetScale)
};

//Shiyuan Xiao 
class BACnetPrescale : public BACnetEncodeable
{
public:
   unsigned multiplier;
   unsigned moduloDivide;

   BACnetPrescale();
   BACnetPrescale(unsigned multiplier, unsigned moduloDivide);
   BACnetPrescale( BACnetAPDUDecoder & dec );   

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   //virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetPrescale)
};

//Shiyuan Xiao 
class BACnetAccumulatorRecord : public BACnetEncodeable
{
public:
   PICS::BACnetDateTime timestamp;
   unsigned presentValue;
   unsigned accumulatedValue;
   unsigned accumulatorStatus;

   BACnetAccumulatorRecord();
   BACnetAccumulatorRecord(PICS::BACnetDateTime timestamp, unsigned presentValue, 
                           unsigned accumulatedValue, unsigned accumulatorStatus);
   BACnetAccumulatorRecord( BACnetAPDUDecoder & dec );   

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   //virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetAccumulatorRecord)
};

class BACnetShedLevel : public BACnetEncodeable
{
public:
   unsigned context;
   float value;

   BACnetShedLevel();
   BACnetShedLevel(unsigned context, float value);
   BACnetShedLevel( BACnetAPDUDecoder & dec );  

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   virtual BACnetEncodeable * clone(void);
   //virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetShedLevel)
};

class BACnetGenericArray : public BACnetEncodeable
{
protected:
   CTypedPtrArray<CObArray, BACnetEncodeable *> m_apBACnetObjects;

   void ClearArray();

   int m_nElemType;
   int m_nType;
   BACnetEncodeable *m_pPrototype;

public:
   BACnetGenericArray();
   BACnetGenericArray( int nDataType, int nSize = -1 );
   BACnetGenericArray( BACnetEncodeable *pPrototype );
   virtual ~BACnetGenericArray();

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   virtual int DataType(void) const;
   //virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   virtual BACnetEncodeable * NewDecoderElement(BACnetAPDUDecoder& dec);

   BACnetEncodeable * operator[](int nIndex) const;
   BACnetEncodeable & operator[](int nIndex);
   int Add( BACnetEncodeable * pbacnetEncodeable );
   int GetSize(void) const;

   // I can't get [] to work, so let's make a method
   const BACnetEncodeable* GetGenericElement(int nIndex) const;

   DECLARE_DYNAMIC(BACnetGenericArray)
};


class BACnetPriorityArray : public BACnetGenericArray
{
public:
   BACnetPriorityArray();
   BACnetPriorityArray( BACnetAPDUDecoder& dec );
   BACnetPriorityArray( float * paPriority, int nMax, float fNull );
   BACnetPriorityArray( unsigned short * paPriority, int nMax, unsigned short uNull, boolean binaryPV );

   BACnetPriorityValue * operator[](int nIndex) const;
   BACnetPriorityValue & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetPriorityArray)
};


class BACnetTextArray : public BACnetGenericArray
{
public:
   BACnetTextArray( BACnetAPDUDecoder& dec );
   BACnetTextArray( char * paText[], int nMax = -1 );
   BACnetTextArray( char * pText );

   BACnetCharacterString * operator[](int nIndex) const;
   BACnetCharacterString & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetTextArray)
};


class BACnetListOfCalendarEntry : public BACnetGenericArray
{
public:
   BACnetListOfCalendarEntry();
   BACnetListOfCalendarEntry( BACnetAPDUDecoder& dec );

   BACnetCalendarEntry * operator[](int nIndex) const;
   BACnetCalendarEntry & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfCalendarEntry)
};

class BACnetBooleanArray : public BACnetGenericArray
{
public:
   BACnetBooleanArray( BACnetAPDUDecoder& dec );
   BACnetBooleanArray( PICS::BooleanList *ub );

   BACnetBoolean * operator[](int nIndex) const;
   BACnetBoolean & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetBooleanArray)
};

class BACnetUnsignedArray : public BACnetGenericArray
{
public:
   BACnetUnsignedArray( BACnetAPDUDecoder& dec );
   BACnetUnsignedArray( unsigned char paUnsigned[], int nMax = -1 );
   BACnetUnsignedArray( unsigned short paUnsigned[], int nMax = -1 );
   BACnetUnsignedArray( PICS::UnsignedList *ul );

   BACnetUnsigned * operator[](int nIndex) const;
   BACnetUnsigned & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetUnsignedArray)
};


class BACnetObjectIDList : public BACnetGenericArray
{
public:
   BACnetObjectIDList();
   BACnetObjectIDList( int nSize );
   BACnetObjectIDList( BACnetAPDUDecoder& dec );

   BACnetObjectIdentifier * operator[](int nIndex) const;
   BACnetObjectIdentifier & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetObjectIDList)
};


class BACnetTimeStampArray : public BACnetGenericArray
{
public:
   BACnetTimeStampArray();
   BACnetTimeStampArray( int nSize );
   BACnetTimeStampArray( BACnetAPDUDecoder& dec );

   BACnetTimeStamp * operator[](int nIndex) const;
   BACnetTimeStamp & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetTimeStampArray)
};

/*
class BACnetListOfObjectPropertyReference : public BACnetGenericArray
{
public:
   BACnetListOfObjectPropertyReference();
   BACnetListOfObjectPropertyReference( BACnetAPDUDecoder& dec );
   BACnetListOfObjectPropertyReference( PICS::BACnetObjectPropertyReference * pobjprops );

   BACnetObjectPropertyReference * operator[](int nIndex) const;
   BACnetObjectPropertyReference & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfObjectPropertyReference)
};
*/

class BACnetListOfDeviceObjectPropertyReference : public BACnetGenericArray
{
public:
   BACnetListOfDeviceObjectPropertyReference();
   BACnetListOfDeviceObjectPropertyReference( BACnetAPDUDecoder& dec );
   BACnetListOfDeviceObjectPropertyReference( PICS::BACnetDeviceObjectPropertyReference * pobjprops );

   BACnetDeviceObjectPropertyReference * operator[](int nIndex) const;
   BACnetDeviceObjectPropertyReference & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfDeviceObjectPropertyReference)
};

class BACnetListOfDeviceObjectReference : public BACnetGenericArray
{
public:
   BACnetObjectIdentifier m_devID;
   BACnetObjectIdentifier m_objID;

   BACnetListOfDeviceObjectReference();
   BACnetListOfDeviceObjectReference( BACnetAPDUDecoder& dec );
   BACnetListOfDeviceObjectReference(PICS::BACnetDeviceObjectReference * pobjs);

   BACnetDeviceObjectReference * operator[](int nIndex) const;
   BACnetDeviceObjectReference & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfDeviceObjectReference)
};


class BACnetListOfEnum : public BACnetGenericArray
{
   int m_nTableId;
public:
   BACnetListOfEnum(int tableId);
   BACnetListOfEnum( BACnetAPDUDecoder& dec, int tableId );

   virtual BACnetEncodeable * NewDecoderElement(BACnetAPDUDecoder& dec);

   BACnetEnumerated * operator[](int nIndex) const;
   BACnetEnumerated & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfEnum)
};

class BACnetListOfVTClass : public BACnetGenericArray
{
public:
   BACnetListOfVTClass();
   BACnetListOfVTClass( BACnetAPDUDecoder& dec );

   virtual BACnetEncodeable * NewDecoderElement(BACnetAPDUDecoder& dec);

   BACnetEnumerated * operator[](int nIndex) const;
   BACnetEnumerated & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfVTClass)
};


class BACnetListOfDestination : public BACnetGenericArray
{
public:
   BACnetListOfDestination();
   BACnetListOfDestination( BACnetAPDUDecoder& dec );

   BACnetDestination * operator[](int nIndex) const;
   BACnetDestination & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfDestination)
};

class BACnetListOfRecipient : public BACnetGenericArray
{
public:
   BACnetListOfRecipient();
   BACnetListOfRecipient( BACnetAPDUDecoder& dec );

   BACnetRecipient * operator[](int nIndex) const;
   BACnetRecipient & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfRecipient)
};


class BACnetListOfCOVSubscription : public BACnetGenericArray
{
public:
   BACnetListOfCOVSubscription();
   BACnetListOfCOVSubscription( BACnetAPDUDecoder& dec );

   BACnetCOVSubscription * operator[](int nIndex) const;
   BACnetCOVSubscription & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfCOVSubscription)
};

class BACnetListOfAddressBinding : public BACnetGenericArray
{
public:
   BACnetListOfAddressBinding();
   BACnetListOfAddressBinding( BACnetAPDUDecoder& dec );

   BACnetAddressBinding * operator[](int nIndex) const;
   BACnetAddressBinding & operator[](int nIndex);

   DECLARE_DYNAMIC(BACnetListOfAddressBinding)
};


class BACnetAnyValue : public BACnetObjectContainer
{
private:
   int m_nType;

public:

   BACnetAnyValue();
   BACnetAnyValue( BACnetEncodeable * pbacnetEncodeable );

   // Overrides of base methods
   virtual int DataType(void) const;

   int GetType() const;
   void SetType(int nNewType);

   // Specific type creations
   bool CompareToEncodedStream( BACnetAPDUDecoder & dec, int iOperator, LPCSTR lpstrValueName );
   void SetObject( int nNewType, BACnetEncodeable * pbacnetEncodeable );
   virtual void SetObject( BACnetEncodeable * pbacnetEncodeable );      // to derive type

   DECLARE_DYNAMIC(BACnetAnyValue)
};




//
// Opening and Closing Tags
//
// These are not native BACnet object types per se, they are just used to open and close 
// a known context.  There are no attributes to keep track of, so there's no constructor 
// and the object really doesn't take up any size.
//
// There is no application encoding for an opening tag, so the context parameter is 
// required.  During decoding the application can peek at the tag and see what kind of 
// thing the next piece is, along with the context if it's context encoded.
//

class BACnetOpeningTag : public BACnetEncodeable
{
public:
   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   //virtual int DataType(void) const;
   //virtual BACnetEncodeable * clone(void);
   //virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );
};

class BACnetClosingTag : public BACnetEncodeable
{
public:
   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   //virtual int DataType(void) const;
   //virtual BACnetEncodeable * clone(void);
   //virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );
};

// Handle decoding of arbitrary tagged items in an APDU
class BACnetANY: public BACnetEncodeable
{
public:
   int            spaceLen;      // size of buffer
   int            dataLen;    // number of octets
   BACnetOctet    *dataBuff;     // pointer to data

   BACnetANY();
   ~BACnetANY();

   // Set value from any other BACnetEncodeable
   void SetValue( BACnetEncodeable &theEnc );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   virtual void Decode( const char *dec );
   //virtual int DataType(void) const;
   //virtual BACnetEncodeable * clone(void);
   virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );

   DECLARE_DYNAMIC(BACnetANY)
};

//
// BACnetAPDUTag
//
// A BACnetAPDUTag is a description of content that sits in front of each object in 
// the stream of data.  According to the standard, the tag 'class' specifies that the 
// content should be interpreted as 'application' (which describes the type) and 
// 'context' (which really means you have to look it up in the standard to figure 
// out the datatype, you can't tell just by looking at the tag).
//
// To help various applications peek into the datastream before decoding it, the 
// BACnetTagClass defines opening and closing tag classes.  They are not described as 
// such in the standard, but it really helps simplify the decoding code.
//

enum BACnetTagClass
{
   applicationTagClass           = 0,
   contextTagClass               = 1,
   openingTagClass               = 2,
   closingTagClass               = 3
};

class BACnetAPDUTag : public BACnetEncodeable 
{
public:
   BACnetTagClass       tagClass;         // class
   BACnetApplicationTag tagNumber;        // tag number
   int                  tagLVT;           // length/value/type

   BACnetAPDUTag( BACnetApplicationTag tnum = nullAppTag, int tlen = 0 );  // application
   BACnetAPDUTag( int context, int tlen = 0 );                       // context
   BACnetAPDUTag( BACnetTagClass tclass, BACnetApplicationTag tnum, int tlen = 0 );

   // Overrides of base methods
   virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
   virtual void Decode( BACnetAPDUDecoder& dec );
   virtual void Encode( CString &enc, Format theFormat ) const;
   //virtual void Decode( const char *dec );
   //virtual int DataType(void) const;
   //virtual BACnetEncodeable * clone(void);
   //virtual bool Match( BACnetEncodeable &rbacnet, int iOperator, CString * pstrError );
};

//
// BACnetNPDU
//
// A BACnetNPDU is the object that gets passed between a client and an endpoint
// (a server).  It encapsulates the address (source or destination depending on
// the context) and a stream of octets.  It also has some extra information that 
// is also encoded in the NPCI (a reply is expected and the network priority) to 
// make it simpler to access (rather than sucking the same information out of the
// encoded NPCI).
//

struct BACnetNPDU
{
   BACnetAddress     pduAddr;
   const BACnetOctet *pduData;
   int               pduLen;
   int               pduExpectingReply;      // see 6.2.2 (1 or 0)
   int               pduNetworkPriority;     // see 6.2.2 (0, 1, 2, or 3)
   
   BACnetNPDU( const BACnetAddress &addr, const BACnetOctet *data, const int len,
               const int reply = 0, const int priority = 0 );
};

typedef BACnetNPDU *BACnetNPDUPtr;

//
// BACnetAPDUEncoder
//
// An encoder is an object passed to the various types to store the encoded version 
// of themselves.
//

const int kDefaultBufferSize = 1024;

class BACnetAPDUEncoder
{
protected:
   int               pktBuffSize;         // allocated size

public:
   BACnetAPDUEncoder( int initBuffSize = kDefaultBufferSize );
   BACnetAPDUEncoder( BACnetOctet *buffPtr, int buffLen = 0 );    // already have a buffer
   BACnetAPDUEncoder( const BACnetAPDUEncoder &theEncoder );
   ~BACnetAPDUEncoder( void );

   BACnetOctet       *pktBuffer;          // pointer to start of buffer
   int               pktLength;           // number of encoded octets

   void SetBuffer( const BACnetOctet *buffer, int len );
   void NewBuffer( int len );             // allocate a new buffer

   void CheckSpace( int len );            // resize iff necessary
   void Append( BACnetOctet ch );         // simple copy, should be inline!
   void Append( const BACnetOctet *buff, int len );   // raw copy into buffer
   void Flush( void );                    // remove all contents

private:
   // Private to prevent copy
   BACnetAPDUEncoder& operator=( BACnetAPDUEncoder const & );
};

typedef BACnetAPDUEncoder *BACnetAPDUEncoderPtr;

//
// BACnetAPDUDecoder
//
// A decoder sets up a context that is used to decode objects.  It is passed to the 
// objects to decode themselves.
//

class BACnetAPDUDecoder
{
public:
   const BACnetOctet *pktBuffer;          // pointer to buffer
   int               pktLength;           // number of encoded octets

   BACnetAPDUDecoder( const BACnetOctet *buffer = 0, int len = 0 );
   BACnetAPDUDecoder( const BACnetAPDUEncoder &enc );
   BACnetAPDUDecoder( const BACnetNPDU &npdu );

   void SetBuffer( const BACnetOctet *buffer, int len );

   void ExamineTag( BACnetAPDUTag &t );         // just peek at the next tag
   bool ExamineOption(int nContext);   //Added by Zhu Zhenhua, 2004-6-14
   void Skip( void );            // skip the tag

   void CopyOctets( BACnetOctet *buff, int len );  // raw copy into buffer

   int ExtractData( BACnetOctet *buffer );         // skip the tag and extract the data
   int ExtractTagData( BACnetOctet *buffer );    // copy the tag and the data

   bool FindContext( int context, BACnetAPDUDecoder &dec ); // return a decoder for a specific context
};

typedef BACnetAPDUDecoder *BACnetAPDUDecoderPtr;

#if _TSMDebug
ostream &operator <<(ostream &strm, const BACnetAPDUDecoder &dec );
#endif

//
// BACnetNetClient
// BACnetNetServer
//

enum BACnetNetworkMessageType
      {  WhoIsRouterToNetwork       = 0x00
      ,  IAmRouterToNetwork            = 0x01
      ,  ICouldBeRouterToNetwork       = 0x02
      ,  RejectMessageToNetwork        = 0x03
      ,  RouterBusyToNetwork           = 0x04
      ,  RouterAvailableToNetwork      = 0x05
      ,  InitializeRoutingTable        = 0x06
      ,  InitializeRoutingTableAck     = 0x07
      ,  EstablishConnectionToNetwork  = 0x08
      ,  DisconnectConnectionToNetwork = 0x09

      ,  ChallengeRequest           = 0x0A
      ,  SecurityPayload               = 0x0B
      ,  SecurityResponse           = 0x0C
      ,  RequestKeyUpdate           = 0x0D
      ,  UpdateKeySet               = 0x0E

      ,  UpdateDistributionKey         = 0x0F
      ,  RequestMasterKey           = 0x10
      ,  SetMasterKey               = 0x11
      ,  WhatIsNetworkNumber           = 0x12
      ,  NetworkNumberIs               = 0x13
      };

class BACnetNetClient;
typedef BACnetNetClient *BACnetNetClientPtr;

class BACnetNetServer;
typedef BACnetNetServer *BACnetNetServerPtr;

class BACnetNetClient {
      friend void Bind( BACnetNetClientPtr, BACnetNetServerPtr );
      friend void Unbind( BACnetNetClientPtr, BACnetNetServerPtr );
      friend bool IsBound( BACnetNetClientPtr, BACnetNetServerPtr );
      
   protected:
      BACnetNetServerPtr      clientPeer;
      
   public:
      BACnetNetClient( void );
      virtual ~BACnetNetClient( void );
      
      void Request( const BACnetNPDU &pdu );
      virtual void Confirmation( const BACnetNPDU &pdu ) = 0;
   };

class BACnetNetServer {
      friend void Bind( BACnetNetClientPtr, BACnetNetServerPtr );
      friend void Unbind( BACnetNetClientPtr, BACnetNetServerPtr );
      friend bool IsBound( BACnetNetClientPtr, BACnetNetServerPtr );
      
   protected:
      BACnetNetClientPtr      serverPeer;
      
   public:
      BACnetNetServer( void );
      virtual ~BACnetNetServer( void );
      
      virtual void Indication( const BACnetNPDU &pdu ) = 0;
      void Response( const BACnetNPDU &pdu );
   };

#if _TSMDebug
class BACnetDebugNPDU : public BACnetNetClient, public BACnetNetServer {
   protected:
      const char        *label;
      
   public:
      BACnetDebugNPDU( const char *lbl );
      BACnetDebugNPDU( BACnetNetServerPtr sp, const char *lbl );
      
      virtual void Indication( const BACnetNPDU &pdu );
      virtual void Confirmation( const BACnetNPDU &pdu );
   };
#endif

//
// BACnetTask
//
// A task is something that needs to be done.  There is a global list of all
// of the tasks that need to be processed, the task manager takes care of making
// sure all of the tasks get processed.
//
// The taskTime is the time in clock ticks that the task should be processed.  If 
// it's less than the current time (including zero) the task will fire.  An active
// task is one that has been installed and will fire, NOT that a task is currently
// running.
//
// The taskType describes one three types of tasks: one-shot, one-shot and deleted, 
// and recurring.  The taskInterval specifies how often a recurring task should 
// execute.  If the taskInterval is zero for these recurring tasks, the task will 
// be processed once each time the TaskManager function is called.
//
// The suspend and resume methods are virtual so other functions can be tied to them
// (like activating/deactivating a text field).  The ProcessTask function is pure 
// virtual so useful objects must supply a method.
//
// The TaskManager function is called by the main event loop.  It could be called 
// by anybody that might tie up the machine for a while, like inside a TrackControl
// loop.  The TaskControlProc is a glue routine that should be assigned to controls
// where no other function would normally be provided.
//

class BACnetTask {
   public:
      enum BACnetTaskType
            { oneShotTask     = 0
            , oneShotDeleteTask  = 1
            , recurringTask      = 2
            };

      BACnetTaskType taskType;            // how to process
      long        taskInterval;        // how often to reschedule (ms)
      int            isActive;            // task is in queue to fire
      
      BACnetTask( BACnetTaskType typ = oneShotTask, long delay = 0 );
      virtual ~BACnetTask(void);
      
      void InstallTask(void);             // install into queue

      // Install, rescheduling if necessary
      void InstallTask( BACnetTaskType theType, long theInterval );

      void SuspendTask(void);             // remove from execution queue
      void ResumeTask(void);              // put back in
      
      virtual void ProcessTask(void) = 0;    // do something
   };

typedef BACnetTask *BACnetTaskPtr;

//
// BACnetTaskManager
//

class BACnetTaskManager {
   public:
      BACnetTaskManager( void ) {};
      virtual ~BACnetTaskManager( void ) {};

      virtual void InstallTask( BACnetTaskPtr tp ) = 0;
      virtual void SuspendTask( BACnetTaskPtr tp ) = 0;
      virtual void ResumeTask( BACnetTaskPtr tp ) = 0;
      
      virtual void ProcessTasks( void ) = 0;
   };

typedef BACnetTaskManager *BACnetTaskManagerPtr;

extern BACnetTaskManagerPtr gTaskManager;

//
// BACnetPort
//
// A port is a kind of server that is the lowest connecting point to 
// the operating system.  Normally the req/conf interface is used, but 
// if the application has a reason to send raw data out the port it 
// can use this interface.  If it needs to track the raw data that was 
// sent and received, it can override the FilterData member function.
//

class BACnetPort : public BACnetNetServer {
    public:
      enum BACnetPortDirection { portSending, portReceiving };
   
      BACnetPort( void );
      virtual ~BACnetPort( void );

      BACnetAddress        portLocalAddr;    // port has this address
      BACnetAddress        portBroadcastAddr;   // use this to broadcast (could be null)

      int         portStatus;                // non-zero iff something is wrong
      virtual void PortStatusChange( void );    // notification that something changed

      virtual void SendData( BACnetOctet *, int len ) = 0;
      virtual void FilterData( BACnetOctet *, int len, BACnetPortDirection dir );
   };

typedef BACnetPort *BACnetPortPtr;

//
// BACnetAPDU
//

enum BACnetAPDUType
   { confirmedRequestPDU   = 0x00
   , unconfirmedRequestPDU = 0x01
   , simpleAckPDU          = 0x02
   , complexAckPDU         = 0x03
   , segmentAckPDU         = 0x04
   , errorPDU              = 0x05
   , rejectPDU             = 0x06
   , abortPDU              = 0x07
   };

class BACnetAPDU : public BACnetAPDUEncoder  {
   public:
      BACnetAddress     apduAddr;            // source/destination address
      BACnetAPDUType    apduType;
      bool              apduSeg;             // segmented
      bool              apduMor;             // more follows
      bool              apduSA;              // segmented response accepted
      bool              apduSrv;             // sent by server
      bool              apduNak;             // negative acknowledgement
      int               apduSeq;             // sequence number
      int               apduWin;             // actual/proposed window size
      int               apduMaxResp;         // max response accepted (decoded)
      int               apduMaxSegs;         // max segments accepted
      int               apduService;
      int               apduInvokeID;
      int               apduAbortRejectReason;
      int               apduExpectingReply;  // see 6.2.2 (1 or 0)
      int               apduNetworkPriority; // see 6.2.2 (0, 1, 2, or 3)

      BACnetAPDU( int initBuffSize = kDefaultBufferSize );     // new buffer
      BACnetAPDU( BACnetOctet *buffPtr, int buffLen = 0 );     // already have a buffer
      BACnetAPDU( const BACnetAPDU &pdu );                  // copy constructor
      ~BACnetAPDU();

      void Encode( BACnetAPDUEncoder& enc ) const;          // encode
      void Decode( const BACnetAPDUDecoder& dec );          // decode
   };

typedef BACnetAPDU *BACnetAPDUPtr;

//
// BACnetConfirmedServiceAPDU
//

enum BACnetConfirmedServiceChoice {
// Alarm and Event Services
      acknowledgeAlarm           = 0,
      confirmedCOVNotification   = 1,
      confirmedEventNotification = 2,
      getAlarmSummary            = 3,
      getEnrollmentSummary       = 4,
      subscribeCOV               = 5,

// File Access Services
      atomicReadFile             = 6,
      atomicWriteFile            = 7,

// Object Access Services
      addListElement             = 8,
      removeListElement          = 9,
      createObject               = 10,
      deleteObject               = 11,
      readProperty               = 12,
      readPropertyConditional    = 13,
      readPropertyMultiple       = 14,
      writeProperty              = 15,
      writePropertyMultiple      = 16,

// Remote Device Management Services
      deviceCommunicationControl = 17,
      confirmedPrivateTransfer   = 18,
      confirmedTextMessage       = 19,
      reinitializeDevice         = 20,

// Virtual Terminal Services
      vtOpen                     = 21,
      vtClose                    = 22,
      vtData                     = 23,

// Security Services
      authenticate               = 24,
      requestKey                 = 25,

//services after 1995         //Added by Zhu Zhenhua, 2004-5-25
      getEventInformation        = 29
      };

class BACnetConfirmedServiceAPDU : public BACnetAPDU {
   public:
      BACnetConfirmedServiceAPDU( BACnetConfirmedServiceChoice ch );
   };

//
// BACnetUnconfirmedServiceAPDU
//

enum BACnetUnconfirmedServiceChoice {
      iAm                        = 0,
      iHave                   = 1,
      unconfirmedCOVNotification    = 2,
      unconfirmedEventNotification  = 3,
      unconfirmedPrivateTransfer    = 4,
      unconfirmedTextMessage        = 5,
      timeSynchronization           = 6,
      whoHas                     = 7,
      whoIs                   = 8
      };

class BACnetUnconfirmedServiceAPDU : public BACnetAPDU {
   public:
      BACnetUnconfirmedServiceAPDU( BACnetUnconfirmedServiceChoice ch );
   };

//
// BACnetSimpleAckAPDU
//

class BACnetSimpleAckAPDU : public BACnetAPDU {
   public:
      BACnetSimpleAckAPDU( BACnetConfirmedServiceChoice ch, BACnetOctet invID );
   };

//
// BACnetComplexAckAPDU
//

class BACnetComplexAckAPDU : public BACnetAPDU {
   public:
      BACnetComplexAckAPDU( BACnetConfirmedServiceChoice ch, BACnetOctet invID = 0 );
      
      void SetInvokeID( BACnetOctet id );
   };

//
// BACnetSegmentAckAPDU
//

class BACnetSegmentAckAPDU : public BACnetAPDU {
   public:
      BACnetSegmentAckAPDU( const BACnetAddress &dest, BACnetOctet invID, int nak, int srv, BACnetOctet seg, BACnetOctet win );
   };

//
// BACnetErrorAPDU
//

class BACnetErrorAPDU : public BACnetAPDU {
   public:
      BACnetErrorAPDU( BACnetConfirmedServiceChoice ch, BACnetOctet invID );
   };

//
// BACnetRejectAPDU
//

enum BACnetRejectReason {
      otherReject                = 0,
      bufferOverflowReject       = 1,
      inconsistentParamtersReject      = 2,
      invalidParameterDataTypeReject   = 3,
      invalidTagReject           = 4,
      missingRequiredParameterReject   = 5,
      parameterOutOfRangeReject     = 6,
      tooManyArgumentsReject        = 7,
      undefinedEnumerationReject    = 8,
      unrecognizedService           = 9
      };

class BACnetRejectAPDU : public BACnetAPDU {
   public:
      BACnetRejectAPDU( BACnetOctet invID, BACnetRejectReason reason );
   };

//
// BACnetAbortAPDU
//

enum BACnetAbortReason {
      otherAbort                    = 0,
      bufferOverflowAbort              = 1,
      invalidAPDUInThisStateAbort         = 2,
      preemptedByHigherPriorityTaskAbort  = 3,
      segmentationNotSupportedAbort    = 4,
      
      // custom abort codes
      apduTimeoutAbort              = 63
      };

class BACnetAbortAPDU : public BACnetAPDU {
   public:
      BACnetAbortAPDU( const BACnetAddress &dest, int srv, BACnetOctet invID, BACnetAbortReason reason );
   };

//
// BACnetAppClient
// BACnetAppServer
//

class BACnetAppClient;
typedef BACnetAppClient *BACnetAppClientPtr;

class BACnetAppServer;
typedef BACnetAppServer *BACnetAppServerPtr;

class BACnetAppClient {
      friend void Bind( BACnetAppClientPtr, BACnetAppServerPtr );
      friend void Unbind( BACnetAppClientPtr, BACnetAppServerPtr );
      friend bool IsBound( BACnetAppClientPtr, BACnetAppServerPtr );
      
   protected:
      BACnetAppServerPtr      clientPeer;
      
   public:
      BACnetAppClient( void );
      virtual ~BACnetAppClient( void );
      
      void Request( const BACnetAPDU &pdu );
      virtual void Confirmation( const BACnetAPDU &pdu ) = 0;
   };

class BACnetAppServer {
      friend void Bind( BACnetAppClientPtr, BACnetAppServerPtr );
      friend void Unbind( BACnetAppClientPtr, BACnetAppServerPtr );
      friend bool IsBound( BACnetAppClientPtr, BACnetAppServerPtr );
      
   protected:
      BACnetAppClientPtr      serverPeer;
      
   public:
      BACnetAppServer( void );
      virtual ~BACnetAppServer( void );
      
      virtual void Indication( const BACnetAPDU &pdu ) = 0;
      void Response( const BACnetAPDU &pdu );
   };

//
// BACnetDeviceInfo
//

enum BACnetSegmentation {
      segmentedBoth     = 0,
      segmentedTransmit = 1,
      segmentedReceive  = 2,
      noSegmentation    = 3
      };

class BACnetDeviceInfo {
   public:
      unsigned int         deviceInst;
      BACnetAddress        deviceAddr;
      BACnetSegmentation   deviceSegmentation;        // supports segments requests
      int                  deviceSegmentSize;         // how to divide up chunks
      int                  deviceWindowSize;          // how many to send
      int                  deviceMaxAPDUSize;         // maximum APDU size
      int                  deviceNextInvokeID;        // next invoke ID for client
//    int                  deviceVendorID;            // which vendor is this
      int                  deviceMaxSegs;             // Maximum number of response segments accepted
      
      BACnetDeviceInfo     *deviceNext;               // list of information blocks
   };

typedef BACnetDeviceInfo *BACnetDeviceInfoPtr;

//
// BACnetDevice
//

class BACnetClient;
typedef BACnetClient *BACnetClientPtr;

class BACnetServer;
typedef BACnetServer *BACnetServerPtr;

class BACnetDevice : public BACnetNetClient, public BACnetDeviceInfo {
   public:
      int                  deviceAPDUTimeout;         // how long to wait for ack
      int                  deviceAPDUSegmentTimeout;  // how long to wait between segments
      int                  deviceAPDURetries;         // how many retries are acceptable
      
      BACnetDevice( void );
      
      BACnetClientPtr         deviceClients;
      BACnetServerPtr         deviceServers;
      
      void Bind( BACnetClientPtr cp );
      void Unbind( BACnetClientPtr cp );
      void Bind( BACnetServerPtr sp );
      void Unbind( BACnetServerPtr sp );
      
      BACnetDeviceInfoPtr GetInfo( unsigned int inst );
      BACnetDeviceInfoPtr GetInfo( const BACnetAddress &addr );
      
      void Indication( const BACnetAPDU &apdu );      // outgoing packet
      void Confirmation( const BACnetNPDU &apdu ); // incoming packet
      
      int GetInvokeID( void );                  // new invoke ID
   };

typedef BACnetDevice *BACnetDevicePtr;

int MaxAPDUEncode( int siz );
int MaxAPDUDecode( int siz );

int MaxSegsEncode( int siz );
int MaxSegsDecode( int siz );

//
// BACnetAPDUSegment
//

class BACnetTSM;
typedef BACnetTSM *BACnetTSMPtr;

class BACnetAPDUSegment {
   public:
      BACnetAPDU           segAPDU;          // returned PDU
   
      BACnetAPDUSegment( const BACnetAPDU &apdu, BACnetTSMPtr tp );
      BACnetAPDUSegment( int ssize, BACnetTSMPtr tp );
      ~BACnetAPDUSegment( void );
      
      const BACnetAPDU &operator[](const int indx);   // get a segment
      int AddSegment( const BACnetAPDU &apdu );    // add on end
      
      const BACnetAPDU &ResultAPDU( void );        // return complete message

      int TotalLength() const { return segLen; }

   protected:
      BACnetTSMPtr         segTSMPtr;           // invoke ID, seg size, window stuff
      BACnetOctet          *segBuff;            // pointer to buffer
      int                  segBuffSize;         // allocated size
      int                  segLen;              // current length
   };

typedef BACnetAPDUSegment *BACnetAPDUSegmentPtr;

//
// BACnetTSM
//

enum BACnetTSMState
      { tsmIdle
      , tsmSegmentedRequest
      , tsmAwaitConfirmation
      , tsmSegmentedConfirmation
      , tsmAwaitResponse
      , tsmSegmentedResponse
      , tsmBusy
      };

class BACnetTSM : public BACnetTask {
      friend class BACnetDevice;
      friend class BACnetAPDUSegment;

   protected:
      BACnetTSMState       tsmState;
      BACnetAPDUSegmentPtr tsmSeg;

      BACnetAddress        tsmAddr;
      int                  tsmInvokeID;
      int                  tsmRetryCount;

      BACnetSegmentation   tsmSegmentation;
      int                  tsmSegmentSize;            // how big are chunks
      int                  tsmSegmentRetryCount;
      int                  tsmSegmentCount;
      int                  tsmSegmentsSent;
      int                  tsmMaxSegs;

      int                  tsmInitialSequenceNumber;  // first sequence number
      int                  tsmLastSequenceNumber;     // last valid seq number received
      
      int                  tsmActualWindowSize;
      int                  tsmProposedWindowSize;

   public:
      BACnetDevicePtr      tsmDevice;                 // bound device

      BACnetTSM( BACnetDevicePtr dp );
      virtual ~BACnetTSM( void );

      void StartTimer( int msecs );
      void StopTimer( void );
      void RestartTimer( int msecs );
      
      void FillWindow( int seqNum );
      int InWindow( int seqNum, int initSeqNum );
      
   protected:
      void SetState( BACnetTSMState newState );
   };

//
// BACnetClientTSM
//

class BACnetClientTSM : public BACnetTSM, public BACnetAppServer {
   public:
      BACnetClientTSM( BACnetDevicePtr dp );
      virtual ~BACnetClientTSM( void );
      
      void Request( const BACnetAPDU &pdu );       // msg to device
      
      void Indication( const BACnetAPDU &pdu );    // msg from client
      void Confirmation( const BACnetAPDU &pdu );     // msg from device
      
      void ProcessTask( void );
      
      void SegmentedRequest( const BACnetAPDU &apdu );
      void SegmentedRequestTimeout( void );
      void AwaitConfirmation( const BACnetAPDU &apdu );
      void AwaitConfirmationTimeout( void );
      void SegmentedConfirmation( const BACnetAPDU &apdu );
      void SegmentedConfirmationTimeout( void );
   };

//
// BACnetServerTSM
//

class BACnetServerTSM : public BACnetTSM, public BACnetAppClient {
   public:
      BACnetServerTSM( BACnetDevicePtr dp );
      virtual ~BACnetServerTSM( void );
      
      void Response( const BACnetAPDU &apdu );     // msg to device
      
      void Indication( const BACnetAPDU &apdu );      // msg from device
      void Confirmation( const BACnetAPDU &apdu ); // msg from server
      
      void ProcessTask( void );
      
      void Idle( const BACnetAPDU &apdu );
      void SegmentedRequest( const BACnetAPDU &apdu );
      void SegmentedRequestTimeout( void );
      void AwaitResponse( const BACnetAPDU &apdu );
      void SegmentedResponse( const BACnetAPDU &apdu );
      void SegmentedResponseTimeout( void );
   };

//
// BACnetClient
//

class BACnetClient : public BACnetAppClient {
      friend class BACnetDevice;
      
   private:
      BACnetClientTSM      clientTSM;
      BACnetClientPtr      clientNext;
      
   public:
      BACnetClient( BACnetDevicePtr dp );
      virtual ~BACnetClient( void );
   };

//
// BACnetServer
//

class BACnetServer : public BACnetAppServer {
      friend class BACnetDevice;
      
   private:
      BACnetServerTSM      serverTSM;
      BACnetServerPtr      serverNext;
      
   public:
      BACnetServer( BACnetDevicePtr dp );
      virtual ~BACnetServer( void );
   };

//
// BACnetError
//

class BACnetError {
   public:
      const char  *errFile;
      const int   errLine;
      const int   errError;

      BACnetError( const char *file, const int line, const int err )
         : errFile(file), errLine(line), errError(err)
      {
      }
   };

// Define error-class and error-code as in Clause 18  Jingbo Gao, Sep 14, 2004
class ErrorClass : public BACnetEnumerated
{
public:
   static enum EnumValue
   {
      DEVICE      = 0,
      OBJECT      = 1,
      PROPERTY    = 2,
      RESOURCES   = 3,
      SECURITY    = 4,
      SERVICES    = 5,
      VT_ErrorCLass  = 6
      //......
   };
};

class ErrorCode : public BACnetEnumerated
{
public:
   static enum EnumValue
   {
      OTHER                            = 0,
      AUTHENTICATION_FAILED            = 1,
      CHARACTER_SET_NOT_SUPPORTED      = 41,
      CONFIGURATION_IN_PROGRESS        = 2,
      DEVICE_BUSY                      = 3,
      DYNAMIC_CREATION_NOT_SUPPORTED   = 4,
      FILE_ACCESS_DYNIED               = 5,
      INCOMPATIBLE_SECURITY_LEVELS     = 6,
      INCONSISTENT_PARAMETERS          = 7,
      INCONSISTERN_SELECTION_CRITERION = 8,
      INVALID_ARRAY_INDEX              = 42,
      INVALID_CONFIGURATION_DATA       = 46,
      INVALID_DATA_TYPE                = 9,
      INVALID_FILE_ACCESS_METHOD       = 10,
      INVALID_FLIE_START_POSITION      = 11,
      INVALID_OPERATOR_NAME            = 12,
      INVALID_PARAMETER_DATA_TYPE      = 13,
      INVALID_TIME_STAMP               = 14,
      KEY_GENERATION_ERROR             = 15,
      MISSING_REQUIRED_PARAMETER       = 16,
      NO_OBJECTS_OF_SPECIFIED_TYPE     = 17,
      NO_SPACE_FOR_OBJECT              = 18,
      NO_SPACE_TO_ADD_LIST_ELEMENT     = 19,
      NO_SPACE_TO_WRITE_PROPERTY       = 20,
      NO_VT_SESSIONS_AVAILABLE         = 21,
      OBJECT_DELETION_NOT_PERMITTED    = 23,
      OBJECT_IDENTIFIER_ALREADY_EXISTS = 24,
      OPERATIONAL_PROBLEM              = 25,
      OPTIONAL_FUNCTIONALITY_NOT_SUPPORTED= 45,
      PASSWORD_FAILURE                 = 26,
      PROPERTY_IS_NOT_A_LIST           = 22,
      READ_ACCESS_DENIED               = 27,
      SECURITY_NOT_SUPPORTED           = 28,
      SERVICE_REQUEST_DENIED           = 29,
      TIMEOUT                          = 30,
      UNKNOWN_OBJECT                   = 31,
      UNKNOWN_PROPERTY                 = 32,
      UNKNOWN_VT_CLASS                 = 34,
      UNKNOWN_VT_SESSION               = 35,
      UNSUPPORTED_OBJECT_TYPE          = 36,
      VALUE_OUT_OF_RANGE               = 37,
      VT_SESSION_ALREADY_CLOSED        = 38,
      VT_SESSION_TERMINATION_FAILURE   = 39,
      WRITE_ACCESS_DENIED              = 40,
      COV_SUBSCRIPTION_FAILED          = 43,
      NOT_COV_PROPERTY                 = 44,
      //
   };
};

#define throw_(x) throw BACnetError( __FILE__, __LINE__, x )


#endif
// vinculum - a unifying bond, link, or tie
