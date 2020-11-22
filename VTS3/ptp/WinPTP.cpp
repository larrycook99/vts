// WinPTP.cpp: implementation of the WinPTP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VTS.h"
#include "WinPTP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UINT WinPTPThreadFunc( LPVOID pParam );


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WinPTP::WinPTP( const char *config )
{
	InitPort(config);	
}

void WinPTP::Indication( const BACnetNPDU &pdu )
{	
	BACnetOctet	*msg;

	// check for nothing to send
	if (pdu.pduLen == 0)
		return;

	// allocate a buffer big enough for the data
	msg = new BACnetOctet[ pdu.pduLen ];
	memcpy( msg, pdu.pduData, pdu.pduLen );

	SendData(msg, pdu.pduLen);
	delete msg;
}

void WinPTP::SendData( BACnetOctet *data, int len )
{	
	byte  framebuf[1014]; //max size we need if all are escaped
	int framelength;
	
	//get PTP frame
	ptpNPDUtoFrame(data, len, framebuf, sizeof(framebuf), &framelength);
	
	FilterData( framebuf, framelength, portSending );
	
	ptpTransmit(data, len);
}

WinPTP::~WinPTP()
{
	ptpDisconnect(0);
	ptpDeinit();
}

void WinPTP::InitPort(const char *config)
{
    if( ptpInit(config) != ptpERSuccess )
    {
		TRACE("can not init the serial port");
		portStatus = -1;
    } 	

	if( ptpConnect("Password") != ptpERSuccess )
    {		
		TRACE("can not connect the serial port");
		portStatus = -1;
    } 	

	m_Continue = true;

	// start it suspended
	m_Thread = AfxBeginThread( WinPTPThreadFunc, this, 
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED	);

	// dont let windows delete the thread object
	m_Thread->m_bAutoDelete = FALSE;

	// now let it run
	// Called to resume execution of a thread created with the CREATE_SUSPENDED flag
	m_Thread->ResumeThread();

	// clear the port status, all is well
	portStatus = 0;
}


UINT WinPTPThreadFunc( LPVOID pParam )
{
	WinPTPPtr pServer = (WinPTPPtr)pParam;
	BACnetAddress srcAddr;

	PKTPTP packet;
	byte  buf[1014]; //max size we need if all are escaped
	int length;

	//  packet.pktPreamble = 0x55FF;
	//	packet.pktFrameType = 2;
	//  packet.pktLen=7;
	//	packet.pktHeaderCRC = 0x21;
	//	packet.pktData[0] = 10;
	//	packet.pktData[1] = 10;
	//	packet.pktData[2] = 10;
	//	packet.pktData[3] = 99;
	//	packet.pktData[4] = 0xBA;
	//	packet.pktData[5] = 0xC0;
	//	packet.pktData[6] = 0xAB;
	//	packet.pktDataCRC = 0x94E3;
	

	while (pServer->m_Continue) 
	{
		if(ptpReceive(&packet))
		{
			TRACE("receive ptp data");

			//get PTP frame
			ptpGetFrame(packet, buf, sizeof(buf), &length);

			// let the application filter it			 
			pServer->FilterData( buf, length, BACnetPort::portReceiving );
			
			pServer->Response( BACnetNPDU(srcAddr, packet.pktData, packet.pktLen));
		}
		else
			Sleep(50);
	}
	return 0;
}
