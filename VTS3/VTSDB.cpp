// VTSDB.cpp: implementation of the VTSDB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VTS.h"

//MAD_DB #include "JConfig.hpp"

#include "VTSDB.h"
//MAD_DB #include "VTSValue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/* MAD_DB  Moved to VTSPort
const char *gVTSPortTypes[6] =
	{ "Null"
	, "IP"
	, "Ethernet"
	, "ARCNET"
	, "MS/TP"
	, "PTP"
	};
*/

//
//	VTSPacketHeader::VTSPacketHeader
//
//	The packet header contains information about the packet: where it 
//	came from, where it was going, how to interpret the contents, etc.
//	It is stored in the packet list array in the database, while the 
//	packet contents (which vary in length) are stored in JDBOctetStr 
//	objects.
//

VTSPacketHeader::VTSPacketHeader( void )
//MAD_DB	: packetPortID(0)				// port identifier
	: packetProtocolID(0)			// protocol identifier for decoding
	, packetFlags(0)				// protocol specific flags
	, packetType(txData)			// not really transmit
	, packetSource(nullAddr)		// source address
	, packetDestination(nullAddr)	// destination address
//MAD_DB	, packetDataID(0)				// no assigned data (yet)
{
    ::GetSystemTimeAsFileTime( &packetTime );	// gets current time
	memset(m_szPortName, 0, sizeof(m_szPortName));
}

//
//	VTSPacket::VTSPacket
//
//	If the packet is constructed with known data (the length is non-zero 
//	and the data is not null) then the packet just uses that information 
//	rather than make a copy of it.  The idea is to keep the amount of 
//	buffer creation and memory copies to a minimum.
//
//	In the ReadPacket function, the packet is created without data, then 
//	new data is provided once the information is available from the database.
//	Because the database object is released, it is important to make a copy.
//	The application may use the same packet object in multiple reads (like 
//	in a search loop).
//

VTSPacket::VTSPacket( int len, BACnetOctetPtr data )
	: packetHdr()					// empty header
	, ownData(false)				// packet does not own data
	, packetLen(0)					// number of octets in data
	, packetData(0)					// pointer to data
{
	// make a reference to the data
	if (len != 0) {
		packetLen = len;
		packetData = data;
	}
}

//
//	VTSPacket::~VTSPacket
//
//	If the packet owns its data pointer, delete it.
//

VTSPacket::~VTSPacket( void )
{
	// toss current data iff owned
	if (ownData && packetData)
		delete[] packetData;
}

//
//	VTSPacket::NewData
//
//	This function is called when the packet contents should reflect a new 
//	record from the database.
//

void VTSPacket::NewData( BACnetOctetPtr data, int len )
{
	// toss current data iff owned
	if (ownData && packetData)
		delete[] packetData;

	// take ownership of the pointer
	ownData = true;

	// make a copy of the data
	packetLen = len;
	packetData = new BACnetOctet[ len ];
	memcpy( packetData, data, len );
}


/*
VTSPort * VTSPacket::GetPortLink(void)
{
	VTSDoc * pdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
	if ( pdoc )
		return pdoc->GetPorts()->Find(packetHdr.m_szPortName);

	return NULL;
}
*/



//
//	VTSPacket::NewDataRef
//
//	This function is called when the packet contents should reflect new data
//	and the application knows that the packet will be destroyed at the time 
//	the packet goes away (no dangling pointers).
//

void VTSPacket::NewDataRef( BACnetOctetPtr data, int len, bool fOwned /* = false */ )
{
	// toss current data iff owned
	if (ownData && packetData)
		delete[] packetData;

	// this is not owned data
	ownData = fOwned;			// MAD_DB

	// refer to the new data
	packetLen = len;
	packetData = data;
}


void VTSPacket::operator =( const VTSPacket& pkt)
{
	memcpy(&packetHdr, &pkt.packetHdr, sizeof(packetHdr));
	NewData(pkt.packetData, pkt.packetLen);
}


//
//	VTSDB::VTSDB
//

//VTSDB::VTSDB( void )  MAD_DB
VTSDB::VTSDB( LPCTSTR lpszFileName )
//MAD_DB	: pObjMgr(0), pArrayMgr(0), pListMgr(0),
//	: m_pfilePackets(NULL), m_strPacketFileName(lpszFileName)
{
	m_pfilePackets = NULL;
	m_strPacketFileName = lpszFileName;
}

//
//	VTSDB::~VTSDB
//

VTSDB::~VTSDB( void )
{
	// MAD_DB
	if ( m_pfilePackets != NULL )
		delete m_pfilePackets;

	m_pfilePackets = NULL;
}

//
//	VTSDB::Init
//

/*MAD_DB
void VTSDB::Init( void )
{
	int		stat
	;
	
	// let the database set itself up
	JDB::Init();

	// set the version
	fileDesc->majorVersion = kVTSDBMajorVersion;
	fileDesc->minorVersion = kVTSDBMinorVersion;

	// build some managers for objects and arrays
	pObjMgr = new JDBObjMgr( this );
	pArrayMgr = new JDBArrayMgr( pObjMgr );
	pListMgr = new JDBListMgr( pObjMgr );

	// allocate a descriptor
	dbDesc = (VTSDescObjPtr)AllocDesc( kVTSDescSig, kVTSDescObjSize );

	// create the port list
	if ((stat = pListMgr->NewList( dbPortList, kObjIdSize )) != 0) {
		return;
	}

	// save the list ID in the descriptor
	dbDesc->portListID = dbPortList.objID;

	// create the name list
	if ((stat = pListMgr->NewList( dbNameList, kVTSNameDescSize )) != 0) {
		return;
	}

	// save the list ID in the descriptor
	dbDesc->nameListID = dbNameList.objID;

	// create the packet list
	if ((stat = pArrayMgr->NewArray( dbPacketList, kVTSPacketHeaderSize, 4096 / kVTSPacketHeaderSize )) != 0) {
		return;
	}

	// save the packet list ID in the descriptor
	dbDesc->packetListID = dbPacketList.objID;

	// create the device list
	if ((stat = pListMgr->NewList( dbDeviceList, kObjIdSize )) != 0) {
		return;
	}

	// save the list ID in the descriptor
	dbDesc->deviceListID = dbDeviceList.objID;

	// MAD_DB
	// Don't care about results here until we remove the DB
	Open(m_strPacketFileName);
}

//
//	VTSDB::Open
//

void VTSDB::Open( void )
{
	int		stat
	;
	char	errMsg[80]
	;

	// open the database
	JDB::Open();

	// make sure the versions are compatible
	if ((fileDesc->majorVersion != kVTSDBMajorVersion) || (fileDesc->minorVersion > kVTSDBMinorVersion)) {
		sprintf( errMsg, "Version %d.%d incompatible with this release (Version %d.%d expected)"
			, fileDesc->majorVersion, fileDesc->minorVersion
			, kVTSDBMajorVersion, kVTSDBMinorVersion
			);

		// it opened, so close it
		CloseFile();

		throw errMsg;
	}

	// build some managers for objects and arrays
	pObjMgr = new JDBObjMgr( this );
	pArrayMgr = new JDBArrayMgr( pObjMgr );
	pListMgr = new JDBListMgr( pObjMgr );
	
	// update if we need to
	if (fileDesc->minorVersion < kVTSDBMinorVersion) {
		// it probably grew bigger
		ResizeDesc( kVTSDescSig, kVTSDescObjSize );
	}

	// get a pointer to our descriptor
	dbDesc = (VTSDescObjPtr)GetDesc( kVTSDescSig );

	// load the port list
	if ((stat = pListMgr->GetList( dbPortList, dbDesc->portListID )) != 0)
		return;

	// load the name list
	if ((stat = pListMgr->GetList( dbNameList, dbDesc->nameListID )) != 0)
		return;

	// load the packet array
	if ((stat = pArrayMgr->GetArray( dbPacketList, dbDesc->packetListID )) != 0)
		return;

	if (fileDesc->minorVersion == 0) {
		// create the device list
		if ((stat = pListMgr->NewList( dbDeviceList, kObjIdSize )) != 0) {
			return;
		}

		// save the list ID in the descriptor
		dbDesc->deviceListID = dbDeviceList.objID;
	} else {
		// load the device list
		if ((stat = pListMgr->GetList( dbDeviceList, dbDesc->deviceListID )) != 0)
			return;
	}

	if (fileDesc->minorVersion < kVTSDBMinorVersion) {
		objId			devDescID
		;
		VTSDeviceDesc	devDesc
		;

		// loop through the device list
		for (int i = 0; i < dbDeviceList.Length(); i++) {
			// get the port descriptor ID's
			stat = dbDeviceList.ReadElem( i, &devDescID );

			// read the current descriptor
			stat = pObjMgr->ReadObject( devDescID, &devDesc );

			// make sure there is an initialized object list
			if (devDesc.objSize < kVTSDeviceDescSize) {
				// create a new object and property list
				JDBListPtr lp = new JDBList();
				stat = pListMgr->NewList( *lp, kVTSObjPropValueSize );
				devDesc.deviceObjPropValueListID = lp->objID;
				delete lp;

				// update the object size
				devDesc.objSize = kVTSDeviceDescSize;

				// save the updated descriptor
				stat = pObjMgr->WriteObject( devDescID, &devDesc );
			}
		}
	}

	// updated to current version
	fileDesc->minorVersion = kVTSDBMinorVersion;

	// MAD_DB
	// Don't care about results here until we remove the DB
	Open(m_strPacketFileName);
}
*/


bool VTSDB::Open( LPCTSTR lpszFileName )
{
	// Close the file if previously open... should not be
	if ( m_pfilePackets != NULL )
		delete m_pfilePackets;

	m_pfilePackets = new CFile();

	if ( m_pfilePackets == NULL )
		return false;

	CFileException e;

	if ( !m_pfilePackets->Open( lpszFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive, &e ) )
	{
		TCHAR   szCause[255];
		CString strError;

		e.GetErrorMessage(szCause, 255);

		strError.Format(IDS_ERR_FILEPKTOPEN, lpszFileName);
	    strError += szCause;

		AfxMessageBox(strError);
		ClosePackets();
		return false;
	}

	return true;
}


//
//	VTSDB::Close
//

void VTSDB::Close( void )
{
/* MAD_DB
	// all done with the managers
	if (pObjMgr)
		delete pObjMgr;
	if (pArrayMgr)
		delete pArrayMgr;
	if (pListMgr)
		delete pListMgr;

	// pass it along
	JDB::Close();
*/

	// MAD_DB
	ClosePackets();
}


void VTSDB::ClosePackets()
{
	if ( m_pfilePackets != NULL )
	{
		if ( m_pfilePackets->m_hFile != (UINT)CFile::hFileNull )
		{
			try
			{
				m_pfilePackets->Flush();
				m_pfilePackets->Close();
			}
			catch (CFileException e )
			{}
		}

		delete m_pfilePackets;
		m_pfilePackets = NULL;
	}
}

//
//	VTSDB::GetPacketCount
//
//	This simple function returns the number of packets in the database.  Unlike the
//	ReadPacket and WritePacket functions, no changes are made, and the array object 
//	is really bound into the database.  So just to make sure that things like the 
//	cache don't get modified by another thread, lock the writeLock.
//

/* MAD_DB
int VTSDB::GetPacketCount( void )
{
	int			count
	;
	CSingleLock lock( &writeLock )
	;
	
	lock.Lock();
	count = dbPacketList.Count();
	lock.Unlock();

	return count;
}
*/

//
//	VTSDB::DeletePackets
//

void VTSDB::DeletePackets( void )
{
/* MAD_DB
	int					stat, i
	;
	VTSDBTransaction	trans( this )				// build a transaction wrapper
	;
	VTSPacketHeader		hdr
	;

	// delete all of the packet data objects
	for (i = 0; i < dbPacketList.Count(); i++) {
		// read the header
		if ((stat = dbPacketList.ReadElem( i, &hdr )) != 0)
			throw stat;

		// if there is a JDBOctetStringObj there, delete it
		if (hdr.packetDataID != 0)
			pObjMgr->DeleteObject( hdr.packetDataID );
	}

	// now delete the array
	if ((stat = pArrayMgr->DeleteArray( dbPacketList.objID )) != 0) {
		return;
	}

	// create a new packet list
	if ((stat = pArrayMgr->NewArray( dbPacketList, kVTSPacketHeaderSize, 4096 / kVTSPacketHeaderSize )) != 0) {
		return;
	}

	// save the packet list ID in the descriptor
	dbDesc->packetListID = dbPacketList.objID;

	// finished with update
	trans.CompleteTransaction();
*/

	// We're messing with file pointers so lock out other threads before this write
	CSingleLock lock( &writeLock );
	lock.Lock();

	try
	{
		m_pfilePackets->SetLength(0);
		m_pfilePackets->Flush();
		m_pfilePackets->SeekToEnd();
	}
	catch (CFileException e)
	{
	}

	lock.Unlock();
}

//
//	VTSDB::ReadPacket
//
//	Read a packet from the database.  Because the cache may move around and change 
//	some pointers, lock it first.  There is no reason to create a transaction because 
//	no modifications will be made.
//

/* MAD_DB
void VTSDB::ReadPacket( int indx, VTSPacket& pkt )
{
	int			stat
	;
	CSingleLock lock( &writeLock )
	;
	
	// make sure no other threads are mucking around
	lock.Lock();

	// read the packet header from the array
	if ((stat = dbPacketList.ReadElem( indx, &pkt.packetHdr )) != 0)
		throw stat;

	// if there's data copy it from the octet string
	if (pkt.packetHdr.packetDataID != 0) {
		// load up the packet data
		JDBOctetStringPtr	osp = new JDBOctetString( pObjMgr, pkt.packetHdr.packetDataID )
		;

		// copy it to the pkt
		pkt.NewData( osp->GetDataPtr(), osp->GetLength() );

		// finished with object
		delete osp;
	} else
		pkt.NewDataRef( 0, 0 );

	// be nice and release it before returning
	lock.Unlock();
}
*/


// returns -1 if file problem or no more records

ULONGLONG VTSDB::ReadNextPacket(VTSPacket& pkt, ULONGUONG lPosition )
{
	ASSERT(m_pfilePackets != NULL && m_pfilePackets->m_hFile != CFile::hFileNull );

	if ( m_pfilePackets == NULL || m_pfilePackets->m_hFile == CFile::hFileNull )
		return -1;

	CSingleLock lock( &writeLock );
	
	// make sure no other threads are mucking around
	lock.Lock();

	

	// If caller specified a position to read from, go there first... 
	// otherwise, use the current position

	try
	{

    // Get current position to restore if read problem
	  ULONGULONG nCurrentPosition = m_pfilePackets->Seek(0, CFile::current);
	  BACnetOctetPtr pbuffer = NULL;
	  ULONGULONG nNewPosition;
	  m_pfilePackets->Seek(lPosition, CFile::begin);

		// Attempt to read the packet header... throw exception if header isn't there
		if ( m_pfilePackets->Read(&pkt.packetHdr, sizeof(pkt.packetHdr)) != sizeof(pkt.packetHdr) )
			AfxThrowFileException(CFileException::endOfFile);

		// Attempt to read the size of the octets... throw exception if size isn't there
		if ( m_pfilePackets->Read(&pkt.packetLen, sizeof(int)) != sizeof(int) )
			AfxThrowFileException(CFileException::endOfFile);

		// OK...  See if there is any octet data.  If not, buffer will already be NULL
		// and length will be zero.

		if ( pkt.packetLen != 0 )
		{
			// Yep.  Allocate size for data and pull it from the file
			pbuffer = new BACnetOctet[pkt.packetLen];		// ought to be enough

			if ( pbuffer == NULL )
				AfxThrowFileException(CFileException::none);

			if ( m_pfilePackets->Read(pbuffer, pkt.packetLen) != (UINT) pkt.packetLen )
				AfxThrowFileException(CFileException::endOfFile);
		}

		// pull off remaining number of bytes read
		// Doesn't matter if blows here

		int nBogus;
		m_pfilePackets->Read(&nBogus, sizeof(int));

		nNewPosition = m_pfilePackets->Seek(0, CFile::current);

		// Point to new octet buffer and make the packet own the stuff so it kills it later
		// Don't assign it until all file operations have completed (so we don't have to undo the assignment).

		pkt.NewDataRef(pbuffer, pkt.packetLen, true);
	}
	catch(...)
	{
		// A problem ocurred while reading the file...  Try to clean up
		if ( pbuffer != NULL )
		{
			pkt.packetLen = 0;
			delete pbuffer;
		}

		// reset file pointer and return read error or end of file (-1)
		m_pfilePackets->Seek(nCurrentPosition, CFile::begin);
	}

	// be nice and release it before returning
	lock.Unlock();
	return nNewPosition;
}


//
//	VTSDB::WritePacket
//

LONG VTSDB::WritePacket( VTSPacket& pkt )
{
//MAD_DB
/*
	int					stat, rnum
	;
	VTSDBTransaction	trans( this )				// build a transaction wrapper
	;

	// is this updating an existing record?
	if (indx >= 0) {
		VTSPacketHeader		oldHdr
		;
		
		// read the old header
		if ((stat = dbPacketList.ReadElem( indx, &oldHdr )) != 0)
			throw stat;

		// if there is a JDBOctetStringObj there, delete it
		if (oldHdr.packetDataID != 0)
			pObjMgr->DeleteObject( oldHdr.packetDataID );
	}

	// create a new object with the packet contents
	JDBOctetStringPtr	osp = new JDBOctetString( pObjMgr, pkt.packetData, pkt.packetLen )
	;

	// save the object ID into the packet header
	pkt.packetHdr.packetDataID = osp->objID;

	// finished saving contents (already saved when it was created)
	delete osp;

	// madanner 3/03, attempt to catch write error and report... close transaction
	try
	{
		// write back if this is an update, otherwise append
		if (indx >= 0) {
			if ((stat = dbPacketList.WriteElem( indx, &pkt.packetHdr )) != 0)
				throw stat;
		} else {
			if ((stat = dbPacketList.NewElem( &rnum, &pkt.packetHdr )) != 0)
				throw stat;
		}
	}
	catch(...)
	{
		TRACE0("PROBLEM WRITING TO DB !!! ");
		AfxMessageBox("An unexpected exception ocurred writing a packet to database.  Please close the database to prevent corruption.");
	}

	// finished with update
	trans.CompleteTransaction();
*/

	// MAD_DB
	if ( m_pfilePackets == NULL )
		return 0;

	// We're messing with file pointers so lock out other threads before this write
	CSingleLock lock( &writeLock );
	lock.Lock();

	DWORD	dwFileLength = m_pfilePackets->SeekToEnd();

	try
	{
		m_pfilePackets->Write(&pkt.packetHdr, sizeof(pkt.packetHdr));
		m_pfilePackets->Write(&pkt.packetLen, sizeof(int));

		if ( pkt.packetLen != 0 )
			m_pfilePackets->Write(pkt.packetData, pkt.packetLen);

		// now write the total length of all this data so we can pull it out later
		// This allows us to move backwards through the file one record at a time

		int nTotal = sizeof(pkt.packetHdr) + sizeof(int) + pkt.packetLen;
		m_pfilePackets->Write(&nTotal, sizeof(int));
	}
	catch (CFileException e)
	{
		// There was a problem writing one of these guys... rollback the file and return the error
		m_pfilePackets->SetLength(dwFileLength);
		m_pfilePackets->Flush();
		// return an error
	}

	lock.Unlock();
	return dwFileLength;		// return file position where this record was written
}

/* MAD_DB
//
//	VTSDBTransaction::VTSDBTransaction
//
//	Bind to a database and start a transaction.  The writeLock will be locked first, 
//	the the database will be told to start.
//

VTSDBTransaction::VTSDBTransaction( VTSDBPtr dbp )
	: dbPtr(dbp), dbTransInProgress(false)
{
	StartTransaction();
}

//
//	VTSDBTransaction::~VTSDBTransaction
//
//	When this object is destroyed, the transaction context is complete.  If there 
//	was an exception thrown, the application might not have the chance to call 
//	CancelTransaction or CompleteTransaction.  The safe thing to do is cancel it.
//

VTSDBTransaction::~VTSDBTransaction( void )
{
	if (dbTransInProgress)
		CancelTransaction();
}

//
//	VTSDBTransaction::StartTransaction
//

void VTSDBTransaction::StartTransaction( void )
{
	// lock the database
	dbPtr->writeLock.Lock();

	// start the transaction
	dbPtr->StartTransaction();
	dbTransInProgress = true;
}

//
//	VTSDBTransaction::CompleteTransaction
//
//	Let the datbase know that the transaction has completed successfully and then 
//	unlock it for other threads.
//

void VTSDBTransaction::CompleteTransaction( void )
{
	// complete the transaction
	dbPtr->CompleteTransaction();
	dbTransInProgress = false;

	// unlock for other updates
	dbPtr->writeLock.Unlock();
}

//
//	VTSDBTransaction::CancelTransaction
//
//	Something bad happened and the transaction needs to be canceled.  Make sure 
//	it is properly cleaned up before releasing the lock for other threads.
//

void VTSDBTransaction::CancelTransaction( void )
{
	// cancel the transaction
	dbPtr->CancelTransaction();
	dbTransInProgress = false;

	// unlock for other updates
	dbPtr->writeLock.Unlock();
}
*/