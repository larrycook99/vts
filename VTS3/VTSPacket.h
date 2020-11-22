// VTSPacket.h: interface for the VTSPacketKill class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSPACKET_H__A0B80A54_F202_415D_9E8B_AB750BE8E4A9__INCLUDED_)
#define AFX_VTSPACKET_H__A0B80A54_F202_415D_9E8B_AB750BE8E4A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxmt.h>

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
	char		    m_szPortName[32];			// store name for associated port
	int				packetProtocolID;			// protocol identifier for decoding
	int				packetFlags;				// protocol specific flags
	FILETIME		packetTime;					// transmit/receive time
	VTSPacketType	packetType;					// path as above
	BACnetAddress	packetSource;				// source address
	BACnetAddress	packetDestination;			// destination address

	VTSPacketHeader( void );
};

typedef VTSPacketHeader *VTSPacketHeaderPtr;

class VTSPort;
class VTSDoc;

class VTSPacket : public CObject
{
protected:
	bool			ownData;				// object owns the data
	
public:
	// Begin persistent data
	VTSPacketHeader	packetHdr;				// header contents
	int				packetLen;				// number of octets in data
	BACnetOctetPtr	packetData;				// pointer to data
	
	CString keyParameters; //add: 2004/11/10 author:Xiao Shiyuan purpose:
	BOOL    bErrorDecode;  //add: 2004/11/10 author:Xiao Shiyuan purpose:
	
	VTSPacket( int len = 0, BACnetOctetPtr data = 0 );
	~VTSPacket( void );

	VTSPacket( const VTSPacket& );
	void operator =( const VTSPacket& pkt);
	
	void NewData( BACnetOctetPtr data, int len );		// flush old, copy new (owned)
	void NewDataRef( BACnetOctetPtr data, int len, bool fOwned = false );	// flush old, reference new (not owned)
	
	LPCSTR GetTimeStampString(void);
	
	//modified: 2004/11/01 author:Xiao Shiyuan			
	LPCSTR GetAddressString( VTSDoc * pdoc, bool bSource, bool bIncludeNetwork = true ) const;

	bool GetNetworkSource( BACnetAddress &theAddress ) const;
	bool GetNetworkDestination( BACnetAddress &theAddress ) const;

	BOOL GetSNET(unsigned short& snet) const;              //2004/11/01 author:Xiao Shiyuan return value: FALSE SNET doesn't exist
	CString GetSADRString(VTSDoc * pdoc, BOOL bAlias=TRUE) const;
	
	BOOL GetDNET(unsigned short& dnet) const;              //2004/11/02 author:Xiao Shiyuan return value: FALSE SNET doesn't exist
	CString GetDADRString(VTSDoc * pdoc, BOOL bAlias=TRUE) const;
	
	BOOL SetDesAddress(BACnetAddress& addr);
	BOOL SetSNET(unsigned short snet, BOOL bReserveSnet=TRUE);
	BOOL SetDNET(unsigned short dnet, BOOL bReserveDnet=TRUE);
	
	BOOL SetDADR(unsigned char *dadr, unsigned int len);
	BOOL SetSADR(unsigned char *sadr, unsigned int len);	

private:
	int FindNPDUStartPos() const;
};

typedef VTSPacket *VTSPacketPtr;


const int kVTSPortNameLength = 32;
const int kVTSPortConfigLength = 512;

/*  MAD_DB Moved to VTSDoc.h
enum VTSPortType
	{ nullPort
	, ipPort
	, ethernetPort
	, arcnetPort
	, mstpPort
	, ptpPort
	};
*/

extern const char *gVTSPortTypes[6];					// label for port type


#endif // !defined(AFX_VTSPACKET_H__A0B80A54_F202_415D_9E8B_AB750BE8E4A9__INCLUDED_)
