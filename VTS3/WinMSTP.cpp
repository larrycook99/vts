// WinMSTP.cpp: implementation of the WinMSTP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "WinMSTP.h"
#include "MSTP_NBLinkDlg.h"

#include <winsock.h>

#include "nbsettings.h"				//AAM NB-Link Setup DLL Header

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern unsigned char MSTPCalcHeaderCRC( const unsigned char * pba, int len );
extern unsigned short MSTPCalcDataCRC( const unsigned char * pba, int len );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WinMSTPData::WinMSTPData(LPCSTR lpszBaud, int nMAC, int nMaxMaster, int nMaxInfoFrame)
{
	m_strBaud = lpszBaud;
	m_nMAC = nMAC;
	m_nMaxMaster = nMaxMaster;
 	m_nMaxInfoFrame = nMaxInfoFrame;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


WinMSTP::WinMSTP(  const char * config )
{
	m_nMaxMaster = 0;
	m_nMaxInfoFrame = 0;
	m_nMAC = 0;
	m_pmstpimp = NULL;

	InitPort( config );
}



WinMSTP::~WinMSTP()
{
	shutdown();
}



void WinMSTP::shutdown()
{
	if ( m_pmstpimp != NULL )
	{
		m_pmstpimp->shutdown();
		delete m_pmstpimp;
	}

	m_pmstpimp = NULL;
}



void WinMSTP::Indication( const BACnetNPDU &pdu )
{	
	BACnetOctet	*msg;

	// check for nothing to send
	if ( pdu.pduLen == 0  ||  m_pmstpimp == NULL )
		return;

	// allocate a buffer big enough for the address and data
	msg = new BACnetOctet[pdu.pduLen + 10];

	// Setup MSTP frame
	msg[0] = 0x55;				// preamble
	msg[1] = 0xFF;
	msg[2] = pdu.pduExpectingReply == 0 ? 0x06 : 0x05;

	// destination address
	msg[3] = pdu.pduAddr.addrType == localBroadcastAddr ? 0xFF : pdu.pduAddr.addrAddr[0];

	// source address
	msg[4] = portLocalAddr.addrAddr[0];

	// length of actual data, MS first
	msg[5] = (pdu.pduLen >> 8) & 0xFF;
	msg[6] = pdu.pduLen & 0xFF;

	// Header CRC
	msg[7] = ~MSTPCalcHeaderCRC(msg + 2, 5);

	// Copy the rest of the actual message
	memcpy( msg+8, pdu.pduData,  pdu.pduLen );

	// Encode the data CRC, LS first
	*(unsigned short *)(msg + pdu.pduLen + 8) = ~MSTPCalcDataCRC(pdu.pduData, pdu.pduLen);

	SendData(msg, pdu.pduLen + 10);

	delete msg;
}




void WinMSTP::InitPort(const char *config)
{
	// clear the port status, startup happening...
	SetPortStatus(2, "MSTP Starting");

	char szBaud[100];
	char szImpName[100];
	memset(szBaud, 0, sizeof(szBaud));
	memset(szImpName, 0, sizeof(szImpName));

	// insert code here to push decoded object data into dialog variables

	//MSTP config format for string is:
	// MAC, MaxMaster, MaxInfoFrame, Baud String, Implementation Name, Implementation Parm String
	// Ex:  80, 127, 10, 9600, NB-Link, [blah, blah, blah]
	// Only read the first common parms... save the bracketed stuff for the implementation

	if ( config != NULL )
	{
		sscanf(config, "%d, %d, %d, %s %s", &m_nMAC, &m_nMaxMaster, &m_nMaxInfoFrame, szBaud, szImpName );
		szBaud[sizeof(szBaud)-1] = 0;
		szImpName[sizeof(szImpName)-1] = 0;

		// Setup the port's local and broadcast address
		portLocalAddr.addrType = localStationAddr;
		portLocalAddr.addrLen = 1;
		portLocalAddr.addrAddr[0] = m_nMAC;
	
		// enable broadcast
		portBroadcastAddr.addrType = localStationAddr;
		portBroadcastAddr.addrLen = 1;
		portBroadcastAddr.addrAddr[0] = 0xFF;

		m_strBaud = szBaud;

		// Get string to implementation specific parms
		CString strConfig(config);
		int nPos = strConfig.Find('[');

		if ( nPos != -1 )
		{
			strConfig = strConfig.Right(strConfig.GetLength()-nPos-1);
			if ( strConfig[strConfig.GetLength()-1] == ']' )
				strConfig = strConfig.Left(strConfig.GetLength()-1);
		}

		// Now decide which implementation to use, create it

		if ( strcmp(szImpName, "NB-Link") == 0 )
		{
			m_pmstpimp = new MSTPImp_NBLink(this, strConfig);
			m_pmstpimp->Init();
		}
//		put other implementations here
//	    else if ( strcmp(szImpName, "Other") == 0 )
//		{
//		}

	}
}



void WinMSTP::SendData( BACnetOctet *data, int len )
{	
	FilterData( data, len, portSending );

	if ( m_pmstpimp != NULL )
		m_pmstpimp->SendData(data, len);
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


MSTPImplementation::MSTPImplementation( WinMSTP * pmstp, const char * config )
{
	m_pmstpPort = pmstp;
}


MSTPImplementation::~MSTPImplementation()
{
	shutdown();
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



extern void CheckLastError( const char *pTheFormat );
extern const char* GetSystemErrorMsg( CString &theResult, const char *pTheFormat, int nError );

//const u_short	kDefaultPort = 17284;

#define BFROMA(x)		((x) - ((x) > 0x40 ? 0x37 : 0x30))
#define AFROMB(x)		((x) + ((x) > 0x09 ? 0x37 : 0x30))


MSTPImp_NBLink::MSTPImp_NBLink( WinMSTP * pmstpPort, const char * config )
			   :MSTPImplementation( pmstpPort, config )
{
	m_nLocalPort = 0;
	m_nListenPort = 0;
	m_fInitOnStartup = TRUE;

	//MSTP config format for string is:
	// ListenPort, IP, Subnet, LocalPort, InitOnStartup (0=false, 1=true)
	
	if ( config != NULL )
	{
		char szIP[100];
		char szSN[100];
		char szGW[100];
		int nStartup = 0;

		sscanf(config, "%d %s %s %s %d %d", &m_nListenPort, szIP, szSN, szGW, &m_nLocalPort, &nStartup );
		szIP[sizeof(szIP)-1] = 0;
		szSN[sizeof(szSN)-1] = 0;
		szGW[sizeof(szGW)-1] = 0;
		m_strIPAddr = szIP;
		m_strIPMask = szSN;
		m_strIPGateway = szGW;
		m_fInitOnStartup = (nStartup != 0);
	}

	m_psocket = NULL;
}


void MSTPImp_NBLink::Init()
{
	m_psocket = NULL;
	m_Continue = true;

	// Test to see if we should query the device and reset if necessary
//	if ( m_fInitOnStartup  && !SetupNBLinkDevice() )
//		return;

	// start it suspended
	m_Thread = AfxBeginThread( MSTP_NBLink_ThreadFunc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED	);

	// dont let windows delete the thread object
	m_Thread->m_bAutoDelete = FALSE;

	// now let it run
	m_Thread->ResumeThread();
}


// Static parameter formatter

void MSTPImp_NBLink::FormatNBLinkString( CString * pstrOut, LPCSTR lpszIPRemote, LPCSTR lpszIP, LPCSTR lpszIPMask, LPCSTR lpszGateway, LPCSTR lpszBaud,
												int nListenPort, int nLocalPort, int nMAC, int nMaxMaster, int nMaxInfoFrame )
{
	int baud;

	//determine the baud rate and correlate it with those for NB-Link	
	//per clause 9.2.3 of ASHRAE 135-2001 possible baud rates are:
	//9600, 19200, 38400, and 76800
	//note that 76800 is not supported in the NB-Link
	//we could also support 57600 and 115200 if we wanted to

	if ( _stricmp(lpszBaud, "19200") == 0 )
		baud = NBBR19200;
	else if ( _stricmp(lpszBaud, "38400") == 0 )
		baud = NBBR38400;
	else 
		baud = NBBR9600;

	//IP Address, Subnet Mask, Gateway Address, Serial Protocol, Speed, Character Size, Parity, Stop Bit, Flow Control,
	//Remote IP Address, Remote Port, Local Port, MS/TP Unit ID, MS/TP Max Master, MS/TP Max Info Frames

	pstrOut->Format("%s,%s,%s,%d,%d,%d,%d,%d,%d,%s,%d,%d,%d,%d,%d",
						lpszIP, lpszIPMask, lpszGateway,
						NBRS4852WIRE, baud,	NB8BIT,	0, NB1STOPBIT, NBNOFLOWCTL,
						lpszIPRemote, nListenPort, nLocalPort, nMAC, nMaxMaster, nMaxInfoFrame);
}


bool MSTPImp_NBLink::SetupNBLinkDevice()
{
	int nError = NBInit(m_strIPAddr);

	if ( nError == 0 )
	{
		m_pmstpPort->SetPortStatus(2, "Querying NB-Link configuration...");

		char szQueryBuffer[512];	// should be enough
		memset(szQueryBuffer,0,sizeof(szQueryBuffer));

		if ( (nError = NBQueryLinkConfiguration(szQueryBuffer, sizeof(szQueryBuffer))) == 0  &&  szQueryBuffer[0] != 0 )
		{
			// Now see if the parms are equal... kind of cheesy, eh?
			CString strNBLinkParms;
			CString strIPThis;
			GetLocalIPAddrString(&strIPThis);

			if ( CompareParameters(szQueryBuffer, strIPThis) )
			{
				// Parameters don't match what's in the device...  reset it then
				m_pmstpPort->SetPortStatus(2, "Resetting NB-Link configuration... Rebooting.");

				FormatNBLinkString(&strNBLinkParms, strIPThis, m_strIPAddr, m_strIPMask, m_strIPGateway,	m_pmstpPort->m_strBaud,
								m_nListenPort, m_nLocalPort, m_pmstpPort->m_nMAC, m_pmstpPort->m_nMaxMaster, m_pmstpPort->m_nMaxInfoFrame);

				nError = NBSetLinkConfiguration(strNBLinkParms);

				if ( nError != 0  &&  nError == INVALID_SUBNET )
				{
					m_pmstpPort->SetPortStatus(3, "NB-Link config reset failure");
					AfxMessageBox("MS/TP NB-Link reconfiguration failure.  The subnet mask is invalid.");
				}
			}
		}
	}

	if ( nError > 0 )		// use -1 if error but not WSAE error, or WAIT_TIMEOUT
	{
		CString str;
		if ( nError == WAIT_TIMEOUT )
		{
			// If we've times out here it's because we're trying to query the device or set the config
			// and we can't seem to talk to the device....  that's fine, really, because it might
			// mean we don't actually have the correct IP address for the device and it's really only important
			// for the NB-Link device to have OUR address... which it might.

			// So just move on after reporting the error... don't halt the connection
			str.Format("The configuration for an NB-Link device attached to an active MS/TP port could not be retrieved.  "
					    "A timeout condition ocurred querying the NB-Link device at IP %s.  The device may still be active and configured to "
						"connect with this machine, therefore the port is still in a wait state.", m_strIPAddr);
			nError = 0;
		}
		else
		{
			m_pmstpPort->SetPortStatus(3, "NB-Link initialization failure");
			GetSystemErrorMsg( str, "MS/TP NBLink Init Error:\n%s\n\nThe port has been halted.", nError );
		}

		AfxMessageBox(str);
	}

	NBUnload();

	return nError == 0;
}



// Disect returned query string and return true if any of the parameters and different
// from what we think we're configured as...

bool MSTPImp_NBLink::CompareParameters( char * lpszParms, LPCSTR lpszIPThis )
{
	//IP, Subnet Mask, Keep Alive, Gateway, Line Mode, Line Speed, 
	//Flow, Local Port, Remote Port, Remote IP, Unit ID, Max Master, Max Info Frames

	LPTSTR p = strtok(lpszParms, ",");
	if ( p == NULL || m_strIPAddr.CompareNoCase(p) )		// ip address
		return true;

	p = strtok(NULL, ",");
	if ( p == NULL || m_strIPMask.CompareNoCase(p) )		// ip mask
		return true;

	p = strtok(NULL, ",");									// keep alive... don't care much
	if ( p == NULL )
		return true;

	p = strtok(NULL, ",");									// gateway 
	if ( p == NULL || m_strIPGateway.CompareNoCase(p) )
		return true;

	p = strtok(NULL, ",");									// line mode... should be NBRS485WIRE
	if ( p == NULL )
		return true;

	p = strtok(NULL, ",");									// baud
	if ( p == NULL )
		return true;

	int nBaud = atoi(p);
	if ( nBaud == NBBR19200  &&  m_pmstpPort->m_strBaud.Compare("19200") )
		return true;

	if ( nBaud == NBBR38400 &&  m_pmstpPort->m_strBaud.Compare("38400")  ) 
		return true;

	if ( nBaud == NBBR9600 && m_pmstpPort->m_strBaud.Compare("9600") )
		return true;

	p = strtok(NULL, ",");									// flow
	if ( p == NULL || atoi(p) != NBNOFLOWCTL )
		return true;

	p = strtok(NULL, ",");
	if ( p == NULL || atoi(p) != m_nLocalPort )				// local port
		return true;

	p = strtok(NULL, ",");
	if ( p == NULL || atoi(p) != m_nListenPort )			// remote port
		return true;

	p = strtok(NULL, ",");
	if ( p == NULL || _stricmp(p, lpszIPThis)  )				// Test where this NBLink thinks it should talk to
		return true;

	p = strtok(NULL, ",");
	if ( p == NULL || atoi(p) != m_pmstpPort->m_nMAC )		// MAC
		return true;

	p = strtok(NULL, ",");
	if ( p == NULL || atoi(p) != m_pmstpPort->m_nMaxMaster )	// max master
		return true;

	p = strtok(NULL, ",");
	if ( p == NULL || atoi(p) != m_pmstpPort->m_nMaxInfoFrame )		// maxinfoframe
		return true;

	return false;
}


void MSTPImp_NBLink::LoadConfigString( CString * pstr )
{
	if ( pstr != NULL )
		pstr->Format("%d %s %s %s %d %d", m_nListenPort, m_strIPAddr, m_strIPMask, m_strIPGateway, m_nLocalPort, (m_fInitOnStartup ? 1 : 0) );
}



void MSTPImp_NBLink::GetLocalIPAddrString( CString * pstrOut )
{
	unsigned long lIP = GetLocalIPAddr();
	byte * p = (byte *) &lIP;
	pstrOut->Format("%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
}


unsigned long MSTPImp_NBLink::GetLocalIPAddr()
{
	char	sName[256];
	HOSTENT	*pHostInfo;

	// get the host name
	if ( gethostname( sName, sizeof(sName)) == SOCKET_ERROR )
		return 0;

	if ( (pHostInfo = gethostbyname(sName)) == NULL )
		return 0;
	
	return *(unsigned long *)*pHostInfo->h_addr_list;
}



void MSTPImp_NBLink::shutdown()
{
	m_Continue = false;

	if ( m_psocket != NULL )
		m_psocket->ShutDown();

	if ( m_Thread != NULL )
	{
		DWORD dwExitCode;
		::GetExitCodeThread( m_Thread->m_hThread, &dwExitCode );

		if ( dwExitCode == STILL_ACTIVE )
		{
			int loopCount;
		
			for (loopCount = 0; loopCount < 5; loopCount++)
				if (::WaitForSingleObject( m_Thread->m_hThread, 1000 ) == WAIT_OBJECT_0)
					break;

			if ( loopCount == 5 )
				::TerminateThread( m_Thread->m_hThread, 0 );
		}

		delete m_Thread;
		m_Thread = NULL;
	}
}



void MSTPImp_NBLink::CloseConnection()
{
	if ( m_psocket != NULL )
	{
		m_psocket->Close();
		delete m_psocket;
		m_psocket = NULL;
	}
}


void MSTPImp_NBLink::SendData( BACnetOctet *data, int len )
{	
	// Must employ a critical section here...

	// NB-Link needs data sent to it in ASCII form... double the buffer
	// and convert

	// NB-Link will calculate the data CRC itself...
	// The buffer we've been passed to send already has this (last two bytes)
	// so strip this off

//	if ( len > 8 )
//		len -= sizeof(unsigned short);

//	unsigned char * pbuf = new unsigned char[len*2];
//	if ( pbuf != NULL )
//	{
//		unsigned char ascii[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

//		for ( int i=0; i < len; i++ )
//		{
//			pbuf[i*2] = ascii[data[i] >> 4];		//AFROMB(data[i] >> 4);
//			pbuf[i*2+1] = ascii[data[i] & 0xF];		//AFROMB(data[i] & 0x0F);
//		}

		if ( m_psocket != NULL  && m_psocket->m_hSocket != NULL )
		{
//			while ( m_psocket->Send(pbuf, len*2) == SOCKET_ERROR )
			while ( m_psocket->Send(data, len) == SOCKET_ERROR )
			{
				int e = m_psocket->GetLastError();
				if ( e != WSAEINPROGRESS )
				{
					CString str;
					str.Format("MS/TP Send Error: WSAE Error = %d", e);
					m_pmstpPort->Msg(str);

#ifdef _DEBUG
					GetSystemErrorMsg( str, "MS/TP Send Error:\n%s", e );
					AfxMessageBox(str);
#endif
					break;
				}
			}
		}

//		delete pbuf;
//	}

	//end critical section
}



int MSTPImp_NBLink::ConfigureDlg( CWnd * pwndParent )
{
	CMSTP_NBLinkDlg dlg(pwndParent);

	dlg.m_nLocalPort = m_nLocalPort;
	dlg.m_nListenPort = m_nListenPort;
	dlg.m_strIPAddr = m_strIPAddr;
	dlg.m_strIPMask = m_strIPMask;
	dlg.m_strIPGateway = m_strIPGateway;
	dlg.m_fInitOnStartup = m_fInitOnStartup ? TRUE : FALSE;
	dlg.m_pnblink = this;


	int nRet = dlg.DoModal();
	if ( nRet == IDOK )
	{
		m_nLocalPort = dlg.m_nLocalPort;
		m_nListenPort = dlg.m_nListenPort;
		m_strIPAddr = dlg.m_strIPAddr;
		m_strIPMask = dlg.m_strIPMask;
		m_strIPGateway = dlg.m_strIPGateway;
		m_fInitOnStartup = (bool) (dlg.m_fInitOnStartup == TRUE);
	}

	return nRet;
}



UINT MSTP_NBLink_ThreadFunc( LPVOID pParam )
{
	MSTPImp_NBLink * pNBLink = (MSTPImp_NBLink *) pParam;
	WinMSTP * pServer = pNBLink->m_pmstpPort;

	BACnetAddress srcAddr;
	unsigned char buf[1014];		 //max size we need if all are escaped

	// Must deal with NBLink intitialization issues...
	// Test to see if we should query the device and reset if necessary

	if ( pNBLink->m_fInitOnStartup  && !pNBLink->SetupNBLinkDevice() )
		return 0;


	// have to call this because we're using this stuff in a separate thread
	AfxSocketInit();

	// Setup server listening socket
	// Set listener to blocking
	DWORD dwBlocking = 0;

//	CSocket socketSvr;
	CAsyncSocket socketSvr;
	if ( !socketSvr.Create(pNBLink->m_nListenPort, SOCK_STREAM) ||
		 !socketSvr.AsyncSelect(0)  ||
		 !socketSvr.IOCtl(FIONBIO, &dwBlocking) )
	{
		CString str;
		GetSystemErrorMsg( str, "MSTP NB-Link startup error:\n%s", socketSvr.GetLastError() );
		AfxMessageBox(str);
		pNBLink->m_Continue = false;
	}


	while ( pNBLink->m_Continue  &&  socketSvr.Listen() )
	{
		// Initialize status to not quite connected
		pServer->SetPortStatus(2, "Waiting on NB-Link connection");

		// Begin critical section
		pNBLink->CloseConnection();

		pNBLink->m_psocket = new CAsyncSocket();
		// end critical section

		if ( (pNBLink->m_Continue = (socketSvr.Accept(*(pNBLink->m_psocket))) == TRUE) )
		{
			pNBLink->m_psocket->AsyncSelect(FD_READ);
			dwBlocking = 1;
			pNBLink->m_psocket->IOCtl(FIONBIO, &dwBlocking);
			
			//Get some information about the client
			CString peeraddr,rcvaddr;
			UINT peerport, rcvport;
			pNBLink->m_psocket->GetPeerName(peeraddr,peerport);

			// Set connected status
			CString strConn;
			strConn.Format("NB-Link (%s) connection ready",peeraddr);
			pServer->SetPortStatus(1, strConn.GetBuffer(0));
			strConn.ReleaseBuffer();
			//pServer->SetPortStatus(1, "NB-Link connection ready");

			//need to open a UDP socket to receive data from NB-Link
			CAsyncSocket socketData;
			if ( !socketData.Create(pNBLink->m_nListenPort, SOCK_DGRAM) ||
				!socketData.AsyncSelect(0)  ||
				!socketData.IOCtl(FIONBIO, &dwBlocking) )
			{
				CString str;
				GetSystemErrorMsg( str, "MSTP NB-Link UDP error:\n%s", socketSvr.GetLastError() );
				pServer->SetPortStatus(1, "NB-Link Data Socket Failure");
				AfxMessageBox(str);
				pNBLink->m_Continue = false;
				break;
			}
			//End UDP

			while ( pNBLink->m_Continue && pNBLink->m_psocket != NULL  &&  pNBLink->m_psocket->m_hSocket != NULL )
			{
				//Receive data on our UDP socket
				//The use of UDP introduced a new problem, what if someone other then our peer (connected client)
				//sends UDP data?
				//So, here, we need to make sure the address we received data from is the same as our connected client
				//if the data comes from where we expect, then it's good -- pass it on
							
				int nError = socketData.ReceiveFrom(buf,sizeof(buf),rcvaddr,rcvport,0);
				//int nError = pNBLink->m_psocket->Receive(buf, sizeof(buf));
				
				if ( (nError > 0)  && (rcvaddr == peeraddr))
				{
					//NB-Link no-longer sends data in ASCII
					//unsigned char * pbuf = new unsigned char[nError/2];
					unsigned char * pbuf = new unsigned char[nError+1];
					if ( pbuf != NULL )
					{
						memset(pbuf, 0, nError+1);		//clear the buffer
						memcpy(pbuf,buf,nError+1);		//copy the buffer
						//for ( int i=0; i < nError; i+=2 )
						//	pbuf[i/2] = (BFROMA(buf[i]) << 4) | BFROMA(buf[i+1]);

						// Have to recalc CRC because NB-Link screws with it...
						// Assume that if we get a message at all from the NB-Link, the CRC was correct
						// Calculation of CRC is for sniffer only...

						// section added by Kare Sars 
                        // preamble should be 0x55FF not 0000
						pbuf[0]=0x55;
						pbuf[1]=0xFF;
						// recalculate data crc
						pbuf[7]=~MSTPCalcHeaderCRC(pbuf+2, 5);

						//*(unsigned short *)(pbuf + nError/2 - sizeof(unsigned short)) = ~MSTPCalcDataCRC(pbuf+8, nError/2 - 10);
						*(unsigned short *)(pbuf + nError - sizeof(unsigned short)+1) = ~MSTPCalcDataCRC(pbuf+8, nError - 9);

						// let the application filter it
						//pServer->FilterData( pbuf, nError/2, BACnetPort::portReceiving );
						pServer->FilterData( pbuf, nError+1, BACnetPort::portReceiving );

						// Now de-frame the data
						//pServer->Response( BACnetNPDU(BACnetAddress(pbuf+4, 1), pbuf+8, nError/2 - 10));
						pServer->Response( BACnetNPDU(BACnetAddress(pbuf+4, 1), pbuf+8, nError - 9));
						delete pbuf;
					}
				}
				else if ( nError == SOCKET_ERROR )
				{
					int nkill = pNBLink->m_psocket->GetLastError();
					if ( nkill == WSAEWOULDBLOCK )
						Sleep(50);
					else
					{
						CString str;
						str.Format("MS/TP Receive Error: WSAE Error = %d, Port status waiting on connection re-establish from NB-Link", nkill);
						pServer->Msg(str);
#if _DEBUG
						GetSystemErrorMsg( str, "MS/TP Receive Error:\n%s", nkill );
						OutputDebugString(str);
#endif
						pNBLink->m_psocket->Close();
						delete pNBLink->m_psocket;
						pNBLink->m_psocket = NULL;
					}
				}
			}
		}
	}

	pNBLink->CloseConnection();
	pServer->SetPortStatus(3, "NB-Link connection dead");
	delete pNBLink->m_psocket;
	pNBLink->m_psocket = NULL;
	return 0;
}


