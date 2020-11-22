// VTSDB.h: interface for the VTSDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSDB_H__D75BFF4F_ED36_11D3_BE6B_00A0C95A9812__INCLUDED_)
#define AFX_VTSDB_H__D75BFF4F_ED36_11D3_BE6B_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxmt.h>

/* MAD_DB
#include "JDB.hpp"
#include "JDBObj.hpp"
#include "JDBArray.hpp"
#include "JDBList.hpp"
#include "JDBOctetStringObj.hpp"
*/

#include "BACnet.hpp"

//
//	VTSPacket
//
//	The guts of the application is dealing with packets, here is a simple 
//	class for reading records out of the database.  Note that the WritePacket() 
//	methods in VTSDB do not use this object type, it is easier to simply pass 
//	the entire NPDU in and let the database figure out how to save it.
//
//	Packets are created when there is data going out from or going into the 
//	application from the network.  It is independant of the client/server bindings, 
//	so the old concept of 'upstream' and 'downstream' packets is defunct.
//
//	For LAN ports, both the source and destination addresses are filled in.  Both
//	will be null for PTP ports.
//

enum VTSPacketType
	{ txData									// outbound from the application
	, rxData									// inbound from the network
	, msgData									// comment from application
	};

struct VTSPacketHeader {
// MAD_DB	objId			packetPortID;				// port identifier
	char		    m_szPortName[32];			// store name for associated port
	int				packetProtocolID;			// protocol identifier for decoding
	int				packetFlags;				// protocol specific flags
	FILETIME		packetTime;					// transmit/receive time
	VTSPacketType	packetType;					// path as above
	BACnetAddress	packetSource;				// source address
	BACnetAddress	packetDestination;			// destination address
// MAD_DB	objId			packetDataID;				// objId of packet contents

	VTSPacketHeader::VTSPacketHeader( void );
};

typedef VTSPacketHeader *VTSPacketHeaderPtr;
const int kVTSPacketHeaderSize = sizeof( VTSPacketHeader );

class VTSPort;

class VTSPacket : public CObject
{
	protected:
		bool			ownData;				// object owns the data

	public:
		// Begin persistent data
		VTSPacketHeader	packetHdr;				// header contents
		int				packetLen;				// number of octets in data
		BACnetOctetPtr	packetData;				// pointer to data

		VTSPacket( int len = 0, BACnetOctetPtr data = 0 );
		~VTSPacket( void );

		void NewData( BACnetOctetPtr data, int len );		// flush old, copy new (owned)
		void NewDataRef( BACnetOctetPtr data, int len, bool fOwned = false );	// flush old, reference new (not owned)
		VTSPort *		GetPortLink(void);

//	private:
		VTSPacket( const VTSPacket& );				// disable copy constructor
		void operator =( const VTSPacket& pkt);				// disable assignment operator
	};

typedef VTSPacket *VTSPacketPtr;


//
//	VTSPortDescObj
//

#if Mac_Platform
#pragma options align=mac68k
#endif
#if NT_Platform
#pragma pack(push,2)
#endif

const int kVTSPortNameLength = 32;
const int kVTSPortConfigLength = 512;

enum VTSPortType
	{ nullPort
	, ipPort
	, ethernetPort
	, arcnetPort
	, mstpPort
	, ptpPort
	};

extern const char *gVTSPortTypes[6];					// label for port type

/* MAD_DB
struct VTSPortDesc : JDBObj {
	VTSPortType		portType;							// port type
	int				portEnabled;						// true iff IO should be enabled
	char			portName[kVTSPortNameLength];		// port name
	int				portNet;							// network associated with device
	objId			portDeviceObjID;					// ID of bound device
	char			portConfig[kVTSPortConfigLength];	// configuration string
	};

typedef VTSPortDesc *VTSPortDescPtr;
const int kVTSPortDescSize = sizeof( VTSPortDesc );
*/

#if Mac_Platform
#pragma options align=reset
#endif
#if NT_Platform
#pragma pack(pop)
#endif

//
//	VTSNameDesc
//

#if Mac_Platform
#pragma options align=mac68k
#endif
#if NT_Platform
#pragma pack(push,2)
#endif

const int kVTSMaxNameLength = 32;

/* MAD_DB
struct VTSNameDesc {
	char			nameName[kVTSMaxNameLength];		// name
	BACnetAddress	nameAddr;							// address
	objId			namePort;							// specific to a port
	};
*/


/* MAD_DB
typedef VTSNameDesc *VTSNameDescPtr;
const int kVTSNameDescSize = sizeof( VTSNameDesc );
*/

#if Mac_Platform
#pragma options align=reset
#endif
#if NT_Platform
#pragma pack(pop)
#endif

//
//	VTSDeviceDesc
//

#if Mac_Platform
#pragma options align=mac68k
#endif
#if NT_Platform
#pragma pack(push,2)
#endif

/* MAD_DB
struct VTSDeviceDesc : JDBObj {
	char				deviceName[kVTSMaxNameLength];	// name
	int					deviceInstance;					// instance number
	int					deviceRouter;					// true iff device should act like a router
	BACnetSegmentation	deviceSegmentation;				// supports segments requests
	int					deviceSegmentSize;				// how to divide up chunks
	int					deviceWindowSize;				// how many to send
	int					deviceMaxAPDUSize;				// maximum APDU size
	int					deviceNextInvokeID;				// next invoke ID for client
	int					deviceAPDUTimeout;				// how long to wait for ack
	int					deviceAPDUSegmentTimeout;		// how long to wait between segments
	int					deviceAPDURetries;				// how many retries are acceptable
	int					deviceVendorID;					// which vendor is this?
	objId				deviceObjPropValueListID;		// list of objects, properties and values
	};

typedef VTSDeviceDesc *VTSDeviceDescPtr;
const int kVTSDeviceDescSize = sizeof( VTSDeviceDesc );
*/

#if Mac_Platform
#pragma options align=reset
#endif
#if NT_Platform
#pragma pack(pop)
#endif

//
//	VTSDescObj
//
//	The VTS descriptor object is a special object that sits in the database at block 
//	zero and holds general information, for example, the object identifiers of global 
//	lists.
//

/* MAD_DB
struct VTSDescObj : public JDBDescObj {
	objId		portListID;
	objId		nameListID;
	objId		packetListID;
	objId		deviceListID;
	};

typedef VTSDescObj *VTSDescObjPtr;
const int kVTSDescObjSize = sizeof( VTSDescObj );

const short kVTSDescSig = 0x0001;
*/

//
//	VTSDB
//

// TODO: the values here differ from those in VTS.CPP (shown in Abort box)
// They are used only in commented-out code in VTSDB.cpp.
// If that code is ever re-enabled, then figure out how the versioning needs to work.
//const int kVTSDBMajorVersion = 3;			// current version
//const int kVTSDBMinorVersion = 3;

//class VTSDB : public JDB {
class VTSDB : public CObject
{
	public:
//		VTSDB( void );  MAD_DB
		VTSDB( LPCTSTR lpszFileName );
		virtual ~VTSDB( void );

//MAD_DB		virtual void Init( void );				// newly created database
//		virtual void Open( void );				// existing database
		virtual void Close( void );				// clean up operations

		// MAD_DB
		bool Open( LPCTSTR lpszFileName );
		void ClosePackets( void );
	
// MAD_DB		int GetPacketCount( void );						// packet count
		void DeletePackets( void );						// clear out the packets

// MAD_DB		void ReadPacket( int indx, VTSPacket& pkt );	// read a packet
		ULONGLONG WritePacket(VTSPacket& pkt);				// write a packet

		ULONGULONG ReadNextPacket(VTSPacket& pkt, ULONGULONG lPosition);		// MAD_DB

		CCriticalSection	writeLock;			// locked when transaction in progress

/* MAD_DB
		JDBObjMgrPtr	pObjMgr;				// object manager
		JDBArrayMgrPtr	pArrayMgr;				// array manager
		JDBListMgrPtr	pListMgr;				// list manager

		VTSDescObjPtr	dbDesc;					// pointer to the descriptor

		JDBList			dbPortList;				// list of ports
		JDBList			dbNameList;				// list of name/address translations
		JDBArray		dbPacketList;			// array of packet objects
		JDBList			dbDeviceList;			// list of device objects
*/

		CString			m_strPacketFileName;
		CFile *			m_pfilePackets;			// pointer to packet file dump
	};

typedef VTSDB *VTSDBPtr;

//
//	VTSDBTransaction
//
//	The VTSTransaction object is used to wrap a transaction around changes to the 
//	database in a safe way.  When the object is created inside a procedure, it is 
//	bound to a database.  The StartTransaction function locks the database writeLock
//	to make sure nobody else is accessing the database, then starts a transaction.
//	The CompleteTransaction function is called to let the database know that modifications 
//	are complete, then the lock is released to allow other threads access.
//
//	If an error is thrown and not caught, this object will know that the transaction 
//	was not completed successfully.  It will tell the database to cancel the transaction 
//	before releasing the lock.
//
//	The CompleteTransaction function is the only one that application needs to call
//	when it has completed the updates.  The constructor will call StartTransaction, and 
//	the destructor will call CancelTransaction if necessary.
//

/* MAD_DB
class VTSDBTransaction {
	public:
		VTSDBTransaction( VTSDBPtr dbp );			// database to bind to
		~VTSDBTransaction( void );				// release transaction

		void StartTransaction( void );			// start a transaction
		void CompleteTransaction( void );		// complete the transaction
		void CancelTransaction( void );			// cancel the transaction

	protected:
		VTSDBPtr	dbPtr;						// pointer to wrapped database
		bool		dbTransInProgress;			// transaction in progress
	};

typedef VTSDBTransaction *VTSDBTransactionPtr;
*/

#endif // !defined(AFX_VTSDB_H__D75BFF4F_ED36_11D3_BE6B_00A0C95A9812__INCLUDED_)
