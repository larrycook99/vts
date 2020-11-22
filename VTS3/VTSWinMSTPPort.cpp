// VTSWinMSTPPort.cpp: implementation of the VTSWinMSTPPort class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSWinMSTPPort.h"
#include "PI.h"
#include "send.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



extern CSendGroupPtr gMSTPGroupList[];


VTSWinMSTPPort::VTSWinMSTPPort( VTSPortPtr pp )
	: WinMSTP( pp->m_strConfigParms )
	, m_pPort( pp )
{
	// let the port know which send group to use
	m_pPort->portSendGroup = gMSTPGroupList;
}


VTSWinMSTPPort::~VTSWinMSTPPort()
{
	// reset the send group
	m_pPort->portSendGroup = 0;
}


void VTSWinMSTPPort::FilterData( BACnetOctet *data, int len, BACnetPortDirection dir )
{
	VTSPacket	pkt	;
	
	// fill in the packet header
	strncpy(pkt.packetHdr.m_szPortName, m_pPort->GetName(), sizeof(pkt.packetHdr.m_szPortName)-1 );

	pkt.packetHdr.packetProtocolID = (int)BACnetPIInfo::mstpProtocol;
	pkt.packetHdr.packetFlags = 0;
	pkt.packetHdr.packetType = (dir == portSending) ? txData : rxData;
	
	// ignore the source or destination
	if (dir == portSending)
	{
		if ( data[3] == 0xFF )
			pkt.packetHdr.packetDestination.LocalBroadcast();
		else
			pkt.packetHdr.packetDestination.LocalStation( data + 3, 1);
	}
	else if (dir == portReceiving) 
	{
		if ( data[4] == 0xFF )
			pkt.packetHdr.packetSource.LocalBroadcast();
		else
			pkt.packetHdr.packetSource.LocalStation( data + 4, 1 );
	}

	// let the packet refer to the pdu contents, cast away const
	pkt.NewDataRef( data, len );
	
	// save it in the database;
	m_pPort->portDoc->WritePacket( pkt );
}


void VTSWinMSTPPort::Msg( const char * msg )
{
	unsigned char	buff[256];
	unsigned char * dst = buff + 21;
	VTSPacket		pkt;
	
	memset(buff, 0, sizeof(buff));
	strcpy( (char *)dst, msg );
	while (*dst) dst++;

	// fill in the packet header
	pkt.packetHdr.packetProtocolID = (int)BACnetPIInfo::msgProtocol;
	pkt.packetHdr.packetFlags = 0;
	pkt.packetHdr.packetType = msgData;

	// let the packet refer to the pdu contents, cast away const
	pkt.NewDataRef( buff, (dst - buff) + 1 );

	// save it in the database;
	m_pPort->portDoc->WritePacket( pkt );
}



void VTSWinMSTPPort::SetPortStatus( int nStatus, char * pszStatusMsg )
{
	portStatus = nStatus;
	m_pPort->portStatus = nStatus;

	if ( pszStatusMsg != NULL )
		m_pPort->portStatusDesc = pszStatusMsg;

	PortStatusChange();
}


void VTSWinMSTPPort::PortStatusChange( void )
{
	// set the VTSPort info to reflect this status
	if ( portStatus == 0 )
	{
		m_pPort->portStatus = 0;
		m_pPort->portStatusDesc = 0;
	}

	// tell the application that something changed.
	::PostThreadMessage( AfxGetApp()->m_nThreadID, WM_VTS_PORTSTATUS, (WPARAM)0, (LPARAM)m_pPort->portDoc );
}

