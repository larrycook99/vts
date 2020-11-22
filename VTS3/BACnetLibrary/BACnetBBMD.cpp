
#include "stdafx.h"

#include <iostream>

#include <stdlib.h>
#include <string.h>

#include "BACnetIP.hpp"
#include "BACnetBBMD.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetBBMD::BACnetBBMD
//

BACnetBBMD::BACnetBBMD( const BACnetAddress &addr )
	: BACnetTask( recurringTask, 1000 )
	, bbmdAddress(addr), bbmdLocalIndex(-1), bbmdBDTSize(0), bbmdFDTSize(0)
{
	InstallTask();
}

//
//	BACnetBBMD::~BACnetBBMD
//

BACnetBBMD::~BACnetBBMD( void )
{
}

//
//	BACnetBBMD::Indication
//
//	This is called when the appliction has a message to send out.  This function 
//	attaches the proper BIP header to the front and sends it down to the endpoint
//	for delivery.
//

void BACnetBBMD::Indication( const BACnetNPDU &npdu )
{
	int				i, len
	;
	BACnetOctet		*msg, *msgPtr
	;
	
	switch (npdu.pduAddr.addrType) {
		case localStationAddr:
			len = 4 + npdu.pduLen;
			msg = msgPtr = new BACnetOctet[ len ];
			
			*msgPtr++ = kBVLCType;						// BVLL message
			*msgPtr++ = bvlcOriginalUnicastNPDU;		// original unicast
			*msgPtr++ = (len >> 8) & 0xFF;				// length
			*msgPtr++ = (len & 0xFF);
			memcpy( msgPtr, npdu.pduData, npdu.pduLen );
			
			Request( BACnetNPDU( npdu.pduAddr, msg, len ) );
			delete[] msg;
			break;
			
		case localBroadcastAddr:
			// first, send an original broadcast to the local network
			len = 4 + npdu.pduLen;
			msg = msgPtr = new BACnetOctet[ len ];
			
			*msgPtr++ = kBVLCType;						// BVLL message
			*msgPtr++ = bvlcOriginalBroadcastNPDU;		// original broadcast
			*msgPtr++ = (len >> 8) & 0xFF;				// length
			*msgPtr++ = (len & 0xFF);
			memcpy( msgPtr, npdu.pduData, npdu.pduLen );
			
			// send it to the local network
			Request( BACnetNPDU( BACnetAddress(localBroadcastAddr), msg, len ) );
			
			// done with this message
			delete[] msg;
			
			// create a forwarded NPDU message with self as the source
			len = 10 + npdu.pduLen;
			msg = msgPtr = new BACnetOctet[ len ];
			
			*msgPtr++ = kBVLCType;						// BVLC Type
			*msgPtr++ = blvcForwardedNPDU;				// Forwarded-NPDU
			*msgPtr++ = (len >> 8) & 0xFF;				// length
			*msgPtr++ = (len & 0xFF);
			memcpy( msgPtr, bbmdAddress.addrAddr, 6 );
			msgPtr+= 6;
			memcpy( msgPtr, npdu.pduData, npdu.pduLen );
			
			// send it to all of the foreign devices
			for (i = 0; i < bbmdFDTSize; i++)
				Request(
					BACnetNPDU( bbmdFDT[i].fdAddress
						, msg, len
						)
					);
			
			// send to all of the BBMD peers
			for (i = 0; i < bbmdBDTSize; i++)
				Request(
					BACnetNPDU(
						BACnetIPAddr( bbmdBDT[i].bdtIPAddr | (~bbmdBDT[i].bdtMask), bbmdBDT[i].bdtPort )
						,	msg, len
						)
					);
			
			// done with this message as well
			delete[] msg;
			break;
            
        default:
            throw_(1); // should never get any other kind of address
	}
}

//
//	BACnetBBMD::Confirmation
//
//	This is called when the endpoint has received a message.  The BIP header is 
//	examined and the results passed up to the application.  If it is not one of 
//	ours, dump it.
//

void BACnetBBMD::Confirmation( const BACnetNPDU &npdu )
{
	int					i, srcType, srcLen, len, ttl, rsltCode
	;
	BACnetAddress		addr
	;
	const BACnetOctet	*srcPtr = npdu.pduData
	;
	BACnetOctet			*msg, *msgPtr
	;
	
	// check for one of our headers
	if ((npdu.pduLen < 4) || (*srcPtr++ != kBVLCType))
		return;
	
	srcType = *srcPtr++;
	srcLen = *srcPtr++;
	srcLen = (srcLen << 8) + *srcPtr++;
	
	// make sure the length is correct
	if (npdu.pduLen != srcLen)
		return;
	
	switch (srcType) {
		case blvcWriteBroadcastDistributionTable:
			break;
			
		case blvcReadBroadcastDistributionTable:
			msg = msgPtr = new BACnetOctet[ len = (4 + 10 * bbmdBDTSize) ];
			
			*msgPtr++ = kBVLCType;
			*msgPtr++ = blvcReadBroadcastDistributionTableAck;
			*msgPtr++ = (len >> 8) & 0xFF;
			*msgPtr++ = (len & 0xFF);
			for (i = 0; i < bbmdBDTSize; i++) {
				// remember that bbmdBDT[i].bdtAddress has the host all 1's
				memcpy( msgPtr, &bbmdBDT[i].bdtIPAddr, 4 );
				msgPtr += 4;
				memcpy( msgPtr, &bbmdBDT[i].bdtPort, 2 );
				msgPtr += 2;
				memcpy( msgPtr, &bbmdBDT[i].bdtMask, 4 );
				msgPtr += 4;
			}
			
			// send back result code
			Request( BACnetNPDU( npdu.pduAddr, msg, len ) );
			delete[] msg;
			break;
			
		case blvcForwardedNPDU:
			// extract the address from the header
			addr.LocalStation( srcPtr, 6 );
			
			// use result to forward up to application, if there's one bound
			if (serverPeer)
				Response( BACnetNPDU( addr, srcPtr + 6, srcLen - 10 ) );
			
			// send it to all foreign devices
			for (i = 0; i < bbmdFDTSize; i++)
				Request( BACnetNPDU( bbmdFDT[i].fdAddress, npdu.pduData, npdu.pduLen ) );
			
			// see if this should be broadcast on the local network, true iff my mask in the 
			// BDT says direct messages forwarded messages to me
			if ((bbmdLocalIndex >= 0) && (bbmdBDT[bbmdLocalIndex].bdtMask == 0xFFFFFFFF))
				Request( BACnetNPDU( BACnetAddress(localBroadcastAddr), npdu.pduData, npdu.pduLen ) );
			break;
			
		case bvlcRegisterForeignDevice:
			ttl = *srcPtr++;
			ttl = (ttl << 8) + *srcPtr++;
			
			// attempt to register
			rsltCode = RegisterForeignDevice( npdu.pduAddr, ttl );
			
			// build a response message
			len = 6;
			msg = msgPtr = new BACnetOctet[ len ];
			
			*msgPtr++ = kBVLCType;
			*msgPtr++ = bvlcResult;
			*msgPtr++ = (len >> 8) & 0xFF;
			*msgPtr++ = (len & 0xFF);
			*msgPtr++ = (rsltCode >> 8) & 0xFF;
			*msgPtr++ = (rsltCode & 0xFF);
			
			// send back result code
			Request( BACnetNPDU( npdu.pduAddr, msg, len ) );
			delete[] msg;
			break;
			
		case bvlcReadForeignDeviceTable:
			msg = msgPtr = new BACnetOctet[ len = (4 + 10 * bbmdFDTSize) ];
			
			*msgPtr++ = kBVLCType;
			*msgPtr++ = bvlcReadForeignDeviceTableAck;
			*msgPtr++ = (len >> 8) & 0xFF;
			*msgPtr++ = (len & 0xFF);
			for (i = 0; i < bbmdFDTSize; i++) {
				memcpy( msgPtr, bbmdFDT[i].fdAddress.addrAddr, 6 );
				msgPtr += 6;
				*msgPtr++ = (bbmdFDT[i].fdTTL >> 8) & 0xFF;
				*msgPtr++ = bbmdFDT[i].fdTTL & 0xFF;
				*msgPtr++ = (bbmdFDT[i].fdRemain >> 8) & 0xFF;
				*msgPtr++ = bbmdFDT[i].fdRemain & 0xFF;
			}
			
			// send back result code
			Request( BACnetNPDU( npdu.pduAddr, msg, len ) );
			delete[] msg;
			break;
			
		case bvlcDeleteForeignDeviceTableEntry:
			break;
			
		case bvlcDistributeBroadcastToNetwork:
			// allow application to get a copy, if there's one bound
			if (serverPeer)
				Response( BACnetNPDU( npdu.pduAddr, srcPtr, srcLen - 4 ) );
			
			// create a forwarded NPDU message
			len = 10 + npdu.pduLen - 4;
			msg = msgPtr = new BACnetOctet[ len ];
			
			*msgPtr++ = kBVLCType;						// BVLC Type
			*msgPtr++ = blvcForwardedNPDU;				// Forwarded-NPDU
			*msgPtr++ = (len >> 8) & 0xFF;				// length
			*msgPtr++ = (len & 0xFF);
			memcpy( msgPtr, npdu.pduAddr.addrAddr, 6 );	// copy the address where this came from
			msgPtr += 6;
			memcpy( msgPtr, srcPtr, srcLen - 4 );
			
			// do a local broadcast
			Request( BACnetNPDU( BACnetAddress(localBroadcastAddr), msg, len ) );
			
			// send it to all foreign devices (except the one it came from)
			for (i = 0; i < bbmdFDTSize; i++)
				if (!(bbmdFDT[i].fdAddress == npdu.pduAddr))
					Request( BACnetNPDU( bbmdFDT[i].fdAddress, msg, len ) );
			
			// send to all of the BBMD peers (except myself)
			for (i = 0; i < bbmdBDTSize; i++)
				if (i != bbmdLocalIndex)
					Request( BACnetNPDU( bbmdBDT[i].bdtAddress, msg, len ) );
			
			// done with this message as well
			delete[] msg;
			break;
			
		case bvlcOriginalUnicastNPDU:
			// pass up to the next layer, keep the source that came up from the endpoint
			Response( BACnetNPDU( npdu.pduAddr, srcPtr, srcLen - 4 ) );
			break;
			
		case bvlcOriginalBroadcastNPDU:
			// allow application to get a copy
			Response( BACnetNPDU( npdu.pduAddr, srcPtr, srcLen - 4 ) );
			
			// create a forwarded NPDU message
			len = 10 + npdu.pduLen - 4;
			msg = msgPtr = new BACnetOctet[ len ];
			
			*msgPtr++ = kBVLCType;						// BVLC Type
			*msgPtr++ = blvcForwardedNPDU;				// Forwarded-NPDU
			*msgPtr++ = (len >> 8) & 0xFF;				// length
			*msgPtr++ = (len & 0xFF);
			memcpy( msgPtr, npdu.pduAddr.addrAddr, 6 );	// copy the address where this came from
			msgPtr += 6;
			memcpy( msgPtr, srcPtr, srcLen - 4 );
			
			// send it to all foreign devices
			for (i = 0; i < bbmdFDTSize; i++)
				Request( BACnetNPDU( bbmdFDT[i].fdAddress, msg, len ) );
			
			// send to all of the BBMD peers (except myself)
			for (i = 0; i < bbmdBDTSize; i++)
				if (i != bbmdLocalIndex)
					Request( BACnetNPDU( bbmdBDT[i].bdtAddress, msg, len ) );
			
			// done with this message as well
			delete[] msg;
			break;
	}
}

//
//	BACnetBBMD::AddForeignDevice
//

void BACnetBBMD::AddForeignDevice( char *spec, int ttl )
{
	unsigned long	ipAddr, ipMask
	;
	unsigned short	ipPort
	;
	
	if (bbmdFDTSize >= kBACnetBBMDMaxFDTSize)
		throw_(2); // no more space
	
	// Parse the spec
	BACnetIPAddr::StringToHostPort( spec, &ipAddr, &ipMask, &ipPort );
	
	// load the table
	RegisterForeignDevice( BACnetIPAddr( ipAddr, ipPort ), ttl );
}

//
//	BACnetBBMD::DeleteForeignDevice
//

void BACnetBBMD::DeleteForeignDevice( char *spec )
{
	int				i
	;
	unsigned long	ipAddr, ipMask
	;
	unsigned short	ipPort
	;
	
	// Parse the spec
	BACnetIPAddr::StringToHostPort( spec, &ipAddr, &ipMask, &ipPort );
	
	// find it
	for (i = 0; i < bbmdFDTSize; i++)
		if ((bbmdFDT[i].fdIPAddr == ipAddr) && (bbmdFDT[i].fdPort == ipPort))
			break;
	
	// if found, delete it
	if (i < bbmdFDTSize) {
		while (i < bbmdFDTSize - 1) {
			bbmdFDT[i] = bbmdFDT[ i + 1 ];
			i += 1;
		}
		bbmdFDTSize -= 1;
	}
}

//
//	BACnetBBMD::AddPeer
//

void BACnetBBMD::AddPeer( char *spec )
{
	unsigned long	ipAddr, subnetMask
	;
	unsigned short	port
	;
	
	if (bbmdBDTSize >= kBACnetBBMDMaxBDTSize)
		throw_(3);		// peer table overflow
	
	// parse the addess string
	BACnetIPAddr::StringToHostPort( spec, &ipAddr, &subnetMask, &port );
	
	// define the table elements
	bbmdBDT[bbmdBDTSize].bdtIPAddr = ipAddr;
	bbmdBDT[bbmdBDTSize].bdtPort = port;
	bbmdBDT[bbmdBDTSize].bdtMask = subnetMask;
	
	// address might be a directed broadcast
	bbmdBDT[bbmdBDTSize].bdtAddress = BACnetIPAddr( ipAddr | ~subnetMask, port );
	
	// check to see if I should do a local broadcast
	if (BACnetIPAddr(ipAddr,port) == bbmdAddress)
		bbmdLocalIndex = bbmdBDTSize;
	
	// update the count
	bbmdBDTSize += 1;
}

//
//	BACnetBBMD::DeletePeer
//

void BACnetBBMD::DeletePeer( char *spec )
{
	unsigned long	ipAddr, subnetMask
	;
	unsigned short	port
	;
	
	if (bbmdBDTSize >= kBACnetBBMDMaxBDTSize)
		throw_(4);		// peer table overflow
	
	// parse the addess string
	BACnetIPAddr::StringToHostPort( spec, &ipAddr, &subnetMask, &port );
	
	for (int i = 0; i < bbmdBDTSize; i++)
		if ((ipAddr == bbmdBDT[i].bdtIPAddr) && (port == bbmdBDT[i].bdtPort)) {
			// delete it from the table
			for (int j = i+1; j < bbmdBDTSize; j++, i++)
				bbmdBDT[i] = bbmdBDT[j];
			bbmdBDTSize -= 1;
			
			// fix my local index
			if (bbmdLocalIndex == i)
				bbmdLocalIndex = -1;
			else
			if (bbmdLocalIndex > i)
				bbmdLocalIndex -= 1;
			
			// finished
			return;
		}
	
	// not found
	throw_(5);
}

//
//	BACnetBBMD::ProcessTask
//

void BACnetBBMD::ProcessTask( void )
{
	int				i
	;
	bool			flag = false
	;
	BACnetFDTEntry	*src, *dst
	;
	
#ifdef _BACnetOStreamAddr
	// debug
	cout << "[tick]" << endl;
#endif
	
	src = dst = bbmdFDT;
	for (i = 0; i < bbmdFDTSize; i++)
		if (--src->fdRemain != 0) {
			if (!flag) {
				dst++;
				src++;
			} else
				*dst++ = *src++;
		} else {
#ifdef _BACnetOStreamAddr
			// debug
			cout << "Expired: " << src->fdAddress << endl;
#endif
			
			src += 1;
			flag = true;
		}
	
	if (flag)
		bbmdFDTSize = (dst - bbmdFDT);
}

//
//	BACnetBBMD::RegisterForeignDevice
//

int BACnetBBMD::RegisterForeignDevice( const BACnetIPAddr &addr, int ttl )
{
	int			i
	;
	
	// table overflow
	if (bbmdFDTSize >= kBACnetBBMDMaxFDTSize)
		return 0x0030;
	
	// check to see if it's already in the table
	for (i = 0; i < bbmdFDTSize; i++)
		if (bbmdFDT[i].fdAddress == addr) {
			bbmdFDT[i].fdTTL = ttl;
			bbmdFDT[i].fdRemain = ttl + kBACnetBBMDForeignTTLExt;
			
#ifdef _BACnetOStreamAddr
			// debug
			cout << "Re-registered: " << addr << endl;
#endif
			
			// success
			return 0;
		}
	
	// load the table
	bbmdFDT[bbmdFDTSize].fdAddress = addr;
	addr.Unpack( &bbmdFDT[bbmdFDTSize].fdIPAddr
		, &bbmdFDT[bbmdFDTSize].fdPort
		);
	bbmdFDT[bbmdFDTSize].fdTTL = ttl;
	bbmdFDT[bbmdFDTSize].fdRemain = ttl;
	
	// update the count
	bbmdFDTSize += 1;
	
#ifdef _BACnetOStreamAddr
	// debug
	cout << "Registered: " << addr << endl;
#endif
	
	// success
	return 0;
}
