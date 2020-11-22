// VTSPacket.cpp: implementation of the VTSPacketKill class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSPacket.h"
#include "VTSDoc.h"
#include "PI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//
//	VTSPacketHeader::VTSPacketHeader
//
//	The packet header contains information about the packet: where it 
//	came from, where it was going, how to interpret the contents, etc.
//

VTSPacketHeader::VTSPacketHeader( void )
	: packetProtocolID(0)			// protocol identifier for decoding
	, packetFlags(0)				// protocol specific flags
	, packetType(txData)			// not really transmit
	, packetSource(nullAddr)		// source address
	, packetDestination(nullAddr)	// destination address
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

	bErrorDecode = FALSE;
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

VTSPacket::VTSPacket( const VTSPacket& pkt)
{
	if(this != &pkt)
	{
		memcpy(&packetHdr, &pkt.packetHdr, sizeof(packetHdr));
		ownData = false;
		packetData = 0;
		packetLen = 0;
		NewData(pkt.packetData, pkt.packetLen);
	}
}


void VTSPacket::operator =( const VTSPacket& pkt)
{
	memcpy(&packetHdr, &pkt.packetHdr, sizeof(packetHdr));
	NewData(pkt.packetData, pkt.packetLen);
}


LPCSTR VTSPacket::GetTimeStampString()
{
	FILETIME	locFileTime;
	SYSTEMTIME	st;
	static char		theTime[16];

	::FileTimeToLocalFileTime( &packetHdr.packetTime, &locFileTime );
	::FileTimeToSystemTime( &locFileTime, &st );
	sprintf( theTime, "%02d:%02d:%02d.%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	// Warning... must do something with this before calling this method again
	// or data will be overwritten

	return theTime;
}


LPCSTR VTSPacket::GetAddressString( VTSDoc *pdoc, bool bSource, bool bIncludeNetwork ) const
{
	static CString nameBuffer;

	nameBuffer.Empty();
	if (bIncludeNetwork)
	{
		// If the packet was routed, get the remote destination
		BACnetAddress remoteAddr;
		bool hasNetwork = false;
		if (bSource)
		{
			// Look for SNET/SADDR
			hasNetwork = GetNetworkSource( remoteAddr );
		}
		else
		{
			// Look for DNET/DADDR
			hasNetwork = GetNetworkDestination( remoteAddr );
		}

		if (hasNetwork)
		{
			// Look for Name for remoteAddress
			if (pdoc != NULL)
				nameBuffer = pdoc->AddrToName( remoteAddr, packetHdr.m_szPortName );
			
			if (nameBuffer.IsEmpty())
			{
				// Show network and MAC
				nameBuffer.Format( "(%d, %s)", remoteAddr.addrNet, remoteAddr.MacAddress() );
			}

			nameBuffer += " via ";
		}
	}

	const BACnetAddress *paddr = bSource ? &packetHdr.packetSource : &packetHdr.packetDestination;

	// attempt to resolve name... if no doc supplied, we can't, but no harm done.
	CString str;
	if (pdoc != NULL)
		str = pdoc->AddrToName( *paddr, packetHdr.m_szPortName );

	if (str.IsEmpty())
	{
		switch(packetHdr.packetProtocolID)
		{
			case BACnetPIInfo::ipProtocol:
				str = BACnetIPAddr::AddressToString(paddr);
				break;

			case BACnetPIInfo::ethernetProtocol:
			case BACnetPIInfo::arcnetProtocol:
			case BACnetPIInfo::mstpProtocol:
			case BACnetPIInfo::ptpProtocol:
			case BACnetPIInfo::msgProtocol:
			default:
				str = paddr->MacAddress();
				break;
		}
	}

	// Warning... must do something with this before calling this method again
	// or data will be overwritten
	nameBuffer += str;
	return (LPCSTR)nameBuffer;
}

// Return the offset to the NPDU.
// If there is no NPDU, return packetData, causing caller to bail
int VTSPacket::FindNPDUStartPos() const
{
	int retval = 0;
	if (packetData == NULL)
	{
		// Claim end of PDU: caller will say "not enough data" and bail
		return packetLen;
	}

	switch ((BACnetPIInfo::ProtocolType)packetHdr.packetProtocolID)
	{
		case BACnetPIInfo::ipProtocol:
			// skip the fake ip header, address (4), and port (2)
			retval += 6;
			
			// BVLC.  Must have room for 0x81, function byte, and 2 length bytes
			if ((retval+4 < packetLen) && (packetData[retval] == 0x81))
			{
				retval++;

				// extract the function
				int func = packetData[retval];

				retval += 3; //BVLC Function: 1 bytes; BVLC Length: 2 bytes

				// set the function group
				switch ((BVLCFunction)func)
				{
					case bvlcDistributeBroadcastToNetwork:
					case bvlcOriginalUnicastNPDU:
					case bvlcOriginalBroadcastNPDU:
						// These have an NPDU
						break;

					case blvcForwardedNPDU:
						// Has an NPDU just after the forwarded-from address
						retval += 6;
						break;

					case bvlcResult:
					case blvcWriteBroadcastDistributionTable:
					case blvcReadBroadcastDistributionTable:
					case blvcReadBroadcastDistributionTableAck:
					case bvlcRegisterForeignDevice:
					case bvlcReadForeignDeviceTable:
					case bvlcReadForeignDeviceTableAck:
					case bvlcDeleteForeignDeviceTableEntry:
					default:
						// BVLC messages with no NPDU.
						// Set bogus offset so caller will bail
						retval = packetLen;
						break;
				}
			}			
			else
			{
				// Set bogus offset so caller will bail
				retval = packetLen;
			}
			break;

		case BACnetPIInfo::ethernetProtocol:
			// skip over source (6), destination (6), length (2), and SAP (3)
			// Our WinPCAP filter will give us only frames with proper
			// BACnet DSAP, SSAP, and LLC, so this is safe
			retval += 17;			
			break;

		case BACnetPIInfo::arcnetProtocol:
			// skip over source (1), destination (1), SAP (3), LLC (1)
			retval += 6;			
			break;

		case BACnetPIInfo::mstpProtocol:
			// skip over preamble
			// TODO: if this really IS preamble, then need to bypass addresses,
			// then look at frame type to be sure there is an NPDU
			retval += 2;			
			break;
		
		case BACnetPIInfo::msgProtocol:
		case BACnetPIInfo::textMsgProtocol:
			// Not a PDU. Claim end of PDU so caller will say "not enough data" and bail
			retval = packetLen;
			break;
	}

	return retval;
}

// Get SNET and SADDR, else return false
bool VTSPacket::GetNetworkSource( BACnetAddress &theAddress ) const
{
	int ix = FindNPDUStartPos();
	if (packetLen - ix < 2)
		return false;

	if (!(packetData[ix + 1] & 0x08)) // SNET doesn't exist
		return false;

	if (packetData[ix + 1] & 0x20)
	{
		// Skip over DNET/DADR
		ix = ix + 3 + packetData[ix + 4]; 		
	}

	ix += 2;
	theAddress.addrNet = (packetData[ix] << 8) | packetData[ix + 1];

	int len = packetData[ ix + 2 ];
	theAddress.addrLen = len;
	memcpy( theAddress.addrAddr, packetData + ix + 3, len );		

	theAddress.addrType = (len == 0) ? remoteBroadcastAddr : remoteStationAddr;
	
	return true;
}

// Get DNET and DADDR, else return false
bool VTSPacket::GetNetworkDestination( BACnetAddress &theAddress ) const
{
	int ix = FindNPDUStartPos();
	if (packetLen - ix < 2)
		return false;

	if (!(packetData[ix + 1] & 0x20)) // DNET doesn't exist
		return false;

	ix += 2;
	theAddress.addrNet = (packetData[ix] << 8) | packetData[ix + 1];

	int len = packetData[ ix + 2 ];
	theAddress.addrLen = len;
	memcpy( theAddress.addrAddr, packetData + ix + 3, len );		

	theAddress.addrType = (len == 0) ? remoteBroadcastAddr : remoteStationAddr;
	
	return true;
}

BOOL VTSPacket::GetSNET(unsigned short& snet) const
{
	int npduindex = FindNPDUStartPos();
	if (packetLen - npduindex < 2)
		return FALSE;

	if (!(packetData[npduindex + 1] & 0x08)) //SNET doesn't exist
		return FALSE;

	int index = 2;

	if (packetData[npduindex + 1] & 0x20) //DNET and DADR exist
	{
		index = index + 3 + packetData[npduindex + 4]; 		
	}

	snet = packetData[npduindex + index];	
	snet = (snet << 8) | packetData[npduindex + index + 1];
	
	return TRUE;
}

CString VTSPacket::GetSADRString(VTSDoc * pdoc, BOOL bAlias) const
{
	CString sadrStr;
	BACnetAddress sadr;

	if (GetNetworkSource( sadr ))
	{
		if (pdoc != NULL && bAlias)
			sadrStr = pdoc->AddrToName(sadr, packetHdr.m_szPortName);

		if (sadrStr == "")
		{
			// The original called BACnetAddr::ToString.
			// But that shows as {net,mac}, and we want only the MAC here
			sadrStr = sadr.MacAddress();
		}
	}

	return sadrStr;
}

BOOL VTSPacket::GetDNET(unsigned short& dnet) const
{
	CString str;
	BACnetAddress dadr;
	
	int npduindex = FindNPDUStartPos();
	if (packetLen - npduindex< 2)
		return FALSE;		

	if( !(packetData[npduindex + 1] & 0x20) ) //DNET and DADR don't exist
		return FALSE;

	dnet = packetData[npduindex + 2];	
	dnet = (dnet << 8) | packetData[npduindex + 3];	

	return TRUE;
}

CString VTSPacket::GetDADRString(VTSDoc * pdoc, BOOL bAlias) const
{
	CString dadrStr;
	BACnetAddress dadr;

	if (GetNetworkDestination( dadr ))
	{
		if (pdoc != NULL && bAlias)
			dadrStr = pdoc->AddrToName(dadr, packetHdr.m_szPortName);

		if (dadrStr == "")
		{
			// The original called BACnetAddr::ToString.
			// But that shows as {net,mac}, and we want only the MAC here
			dadrStr = dadr.MacAddress();
		}
	}

	return dadrStr;
}

BOOL VTSPacket::SetDesAddress(BACnetAddress& addr)
{
	switch(packetHdr.packetProtocolID)
	{
	case BACnetPIInfo::ipProtocol:
		if(addr.addrLen != 6)
			return FALSE;
			
		memcpy(packetData, addr.addrAddr, 6);
		break;

	case BACnetPIInfo::ethernetProtocol:
		break;

	case BACnetPIInfo::mstpProtocol:
		break;
	}

	return TRUE;
}

BOOL VTSPacket::SetSNET(unsigned short snet, BOOL bReserveSnet)
{
	BOOL bDnet = FALSE;
	BOOL bSnet = FALSE;
	
	int npduindex = FindNPDUStartPos();
	if (packetLen - npduindex < 2)
		return FALSE;

	if(!(packetData[npduindex + 1] & 0x08)) //SNET doesn't exist
	{
		bSnet = FALSE;
	}
	else
	{
		bSnet = TRUE;
	}

	if(packetData[npduindex + 1] & 0x20) //DNET and DADR exist
	{
		bDnet = TRUE;			
	}
	else
	{
		bDnet = FALSE;
	}

	if( bSnet )
	{
		if( bReserveSnet )
		{
			if( bDnet )
			{
				unsigned char dlen = packetData[npduindex + 4];
				packetData[npduindex + 5 + dlen] = (snet & 0xFF00) >> 8;	
				packetData[npduindex + 6 + dlen] = (snet & 0x00FF);
			}
			else
			{
				packetData[npduindex + 2] = (snet & 0xFF00) >> 8;	
				packetData[npduindex + 3] = (snet & 0x00FF);
			}			
		}
		else
		{
			packetData[npduindex + 1] &= 0xF7; //set SNET bit 0

			BACnetOctetPtr oldbuff = packetData;
			int oldPacketLen = packetLen;

			if( bDnet )
			{
				unsigned char dlen = oldbuff[npduindex + 4];
				unsigned char slen = oldbuff[npduindex + 7 + dlen];
				packetLen -= 3 + slen;

				packetData = new BACnetOctet[packetLen];
				memcpy(packetData, oldbuff, npduindex + 5 + dlen);
				memcpy(packetData + npduindex + 5 + dlen, oldbuff + npduindex + 8 + dlen + slen, 
					oldPacketLen - (npduindex + 8 + dlen + slen));
			}
			else
			{
				unsigned char slen = oldbuff[npduindex + 4];
				packetLen -= 3 + slen;

				packetData = new BACnetOctet[packetLen];
				memcpy(packetData, oldbuff, npduindex + 2);
				memcpy(packetData + npduindex + 2, oldbuff + npduindex + 5 + slen, 
					oldPacketLen - (npduindex + 5 + slen));
			}

			delete[] oldbuff;
		}
	}
	else
	{
		if( bReserveSnet )
		{
			packetData[npduindex + 1] |= 0x08; //set SNET bit 1

			BACnetOctetPtr oldbuff = packetData;
			int oldPacketLen = packetLen;

			packetLen += 3;
			packetData = new BACnetOctet[packetLen];

			if( bDnet )
			{
				unsigned char dlen = oldbuff[npduindex + 4];
				
				memcpy(packetData, oldbuff, npduindex + 5 + dlen);
				packetData[npduindex + 5 + dlen] = (snet & 0xFF00) >> 8;
				packetData[npduindex + 6 + dlen] = (snet & 0x00FF);

				packetData[npduindex + 7 + dlen] = 0; //slen = 0

				memcpy(packetData + npduindex + 8 + dlen, oldbuff + npduindex + 5 + dlen,
					oldPacketLen - (npduindex + 5 + dlen));
			}
			else
			{
				memcpy(packetData, oldbuff, npduindex + 2);

				packetData[npduindex + 2] = (snet & 0xFF00) >> 8;	
				packetData[npduindex + 3] = (snet & 0x00FF);

				packetData[npduindex + 4] = 0; //slen = 0

				memcpy(packetData + npduindex + 5, oldbuff + npduindex + 2,
					oldPacketLen - (npduindex + 2));
			}
		}
	}

	return TRUE;
}

//bReserveDnet = FALSE, ignore parameter 'dnet', delete DNET/DLEN/DADR from packet if they exit
//
BOOL VTSPacket::SetDNET(unsigned short dnet, BOOL bReserveDnet)
{
	CString str;
	BACnetAddress dadr;
	BOOL bDnet = FALSE;
	BOOL bSnet = FALSE;

	int npduindex = FindNPDUStartPos();
	if (packetLen - npduindex< 2)
		return FALSE;		

	if( !(packetData[npduindex + 1] & 0x20) ) 
		bDnet = FALSE; //DNET and DADR don't exist
	else
		bDnet = TRUE;

	if(!(packetData[npduindex + 1] & 0x08)) 
		bSnet = FALSE; //SNET doesn't exist
	else
		bSnet = TRUE;

	if( bDnet )
	{
		if( bReserveDnet )
		{
			packetData[npduindex + 2] = (dnet & 0xFF00) >> 8;
			packetData[npduindex + 3] = (dnet & 0x00FF);		
		}
		else
		{
			packetData[npduindex + 1] &= 0xDF; //set DNET bit 0
			unsigned char dlen = packetData[npduindex + 4];

			BACnetOctetPtr oldbuff = packetData;
			int oldPacketLen = packetLen;
			packetLen = packetLen - 4 - dlen; //DNET 2 bytes, DLEN 1 byte, DADR dlen bytes, Hop Count 1 byte
			packetData = new BACnetOctet[packetLen];
			memcpy(packetData, oldbuff, npduindex + 2);

			if( bSnet )
			{
				unsigned char slen = oldbuff[npduindex + 7 + dlen];
				memcpy(packetData + npduindex + 2, oldbuff + npduindex + 5 + dlen, 
				3 + slen);
				memcpy(packetData + npduindex + 5 + slen, oldbuff + npduindex + 9 + dlen + slen,
					oldPacketLen - (npduindex + 9 + dlen + slen));

			}
			else
			{
				memcpy(packetData + npduindex + 2, oldbuff + npduindex + 6 + dlen, 
				oldPacketLen - (npduindex + 6 + dlen)); //do not forget hopcount byte
			}			

			delete[] oldbuff;
		}		
	}
	else
	{
		if( bReserveDnet )
		{
			packetData[npduindex + 1] |= 0x20; //set DNET bit 1
			
			BACnetOctetPtr oldbuff = packetData;
			int oldPacketLen = packetLen;
			packetLen = packetLen + 4; //add DNET and DLEN and Hop Count, 4 byte
			packetData = new BACnetOctet[packetLen];
			memcpy(packetData, oldbuff, npduindex + 2);
			
			packetData[npduindex + 2] = (dnet & 0xFF00) >> 8;
			packetData[npduindex + 3] = (dnet & 0x00FF);
			packetData[npduindex + 4] = 0; //dlen = 0
			
			if( bSnet )
			{			
				unsigned char slen = oldbuff[npduindex + 4];
				memcpy(packetData + npduindex + 5, oldbuff + npduindex + 2, 3 + slen);
				packetData[npduindex + 8 + slen] = 0; //hopcount = 0
				memcpy(packetData + npduindex + 9 + slen, oldbuff + npduindex + 5 + slen,
					oldPacketLen - (npduindex + 5 + slen));
			}
			else
			{
				packetData[npduindex + 5] = 0; //hopcount = 0
				memcpy(packetData + npduindex + 6, oldbuff + npduindex + 2, 
					oldPacketLen - (npduindex + 2));
			}			
			
			delete[] oldbuff;
		}		
	}	
	
	return TRUE;
}

BOOL VTSPacket::SetDADR(unsigned char *dadr, unsigned int len)
{
	int npduindex = FindNPDUStartPos();
	if (packetLen - npduindex< 2)
		return FALSE;		

	if( !(packetData[npduindex + 1] & 0x20) ) //DNET and DADR don't exist
		return FALSE;

	unsigned char dlen = packetData[npduindex + 4];
	if( dlen == len )
	{
		memcpy(packetData + npduindex + 5, dadr, dlen);
	}
	else
	{
		BACnetOctetPtr oldbuff = packetData;
		int oldPacketLen = packetLen;
		packetLen = packetLen + len - dlen;
		packetData = new BACnetOctet[packetLen];
		memcpy(packetData, oldbuff, npduindex + 4);
		packetData[npduindex + 4] = len;
		memcpy(packetData + npduindex + 5, dadr, len);
		memcpy(packetData + npduindex + 5 + len, oldbuff + npduindex + 5 + dlen, 
			oldPacketLen - (npduindex + 5 + dlen));

		delete[] oldbuff;
	}

	return TRUE;
}

BOOL VTSPacket::SetSADR(unsigned char *sadr, unsigned int len)
{
	int npduindex = FindNPDUStartPos();
	if (packetLen - npduindex < 2)
		return FALSE;

	if(!(packetData[npduindex + 1] & 0x08)) //SADR doesn't exist
		return FALSE;

	int index = 4;

	if(packetData[npduindex + 1] & 0x20) //DNET and DADR exist
	{
		index = index + 3 + packetData[npduindex + 4]; 		
	}

	unsigned char slen = packetData[npduindex + index];
	if( slen == len )
	{
		memcpy(packetData + npduindex + index + 1, sadr, slen);
	}
	else
	{
		BACnetOctetPtr oldbuff = packetData;
		int oldPacketLen = packetLen;
		packetLen = packetLen + len - slen;
		packetData = new BACnetOctet[packetLen];
		memcpy(packetData, oldbuff, npduindex + index);
		packetData[npduindex + index] = len;
		memcpy(packetData + npduindex + index + 1, sadr, len);
		memcpy(packetData + npduindex + index + 1 + len, oldbuff + npduindex + index + 1 + slen, 
			oldPacketLen - (npduindex + index + 1 + slen));

		delete[] oldbuff;
	}

	return TRUE;
}