/*-------------------------------------------------------------------                     
	©1999, PolarSoft Inc.
	
module:		SJACK.C
desc:		JACK's Serial Library
authors:	Jack Neyer, David M. Fisher
last edit:	10-Dec-99 [009] JN  moved final handle closing into Listener thread
			10-Dec-99 [008] DMF add recovery for strange WaitObjects returns
			30-Nov-99 [007] JN  add individual character transmit
			24-Nov-99 [006] JN  wait on RX character event
			11-Nov-99 [005] JN  changed ListenTime to 50 ms
			11-May-99 [004] DMF fix early shutdown lockup
			06-May-99 [003] DMF fix SJACKtx error return 
			03-May-99 [002] DMF fix error returns to be ints
			17-Feb-99 [001] JN  First Cut (based on ISN 029)
----------------------------------------------------------------------*/

// This module is a set of library primatives for sending and receiving 
// protocol independent bytes on up to 4 serial ports.

#include <windows.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sjack32.h"

///////////////////////////////////////////////////////////////////////
//	Module Constants
#define		maxports			4
#define		ListenTime			50L			//listener thread timeout					***005
///////////////////////////////////////////////////////////////////////
// Global variables
unsigned long hListener=0;						//handle of Listener Thread process
SJACKport	ports[maxports]={0};				//parameters for each port
OVERLAPPED	portRX[maxports]={0,0,0,0,0,0,0,0};	//event objects for each port
HANDLE		hportRX[maxports]={0,0,0,0};		//redundant with portRX[i].hEvent
int			oktolisten=1;						//for listener thread shutdown
int			listening=0;						//true when listener is actually running ***004
word		usedports=0;						//number actually used
///////////////////////////////////////////////////////////////////////
//	function prototypes
void Listener(void *);

//-------------------------------- Begin API Functions --------------------------------

///////////////////////////////////////////////////////////////////////
//	Initialization called from Application
//
//in:	nports	the number of ports to support (1-4)
//out:	0		success
//		<0		error code: see sjack32.h for a list of error codes

__declspec(dllexport) int SJACKinit(word nports)	//									***002
{   int		i;
   
	if ((nports<1)||(nports>maxports)) return SJACK_InvalidParameter;		//complain
	usedports=nports;
	for (i=0;i<nports;i++)							//init all ports
	{//create two event objects for each port
		portRX[i].hEvent=CreateEvent(NULL,		//no security
									true,			//must use manual reset signal because of FILE_FLAG_OVERLAPPED!
									false,			//initially event hasn't occurred
									NULL);			//don't need a name for it
		hportRX[i]=portRX[i].hEvent;
		ports[i].comport=0;							//this port is not configured yet
	}
	//now create the Listener Thread process
	if ((hListener=_beginthread(Listener,0,NULL))==-1)
	{	for (i=0;i<usedports;i++)
		{	if (portRX[i].hEvent) CloseHandle(portRX[i].hEvent);
			hportRX[i]=NULL;
		}
		return SJACK_CantCreateThread;
	}
    return SJACK_Success;
}

///////////////////////////////////////////////////////////////////////
//	Set up a binding between a logical port and a physical port
//in:	i			port number 0..usedports-1
//		params		the text of the initialization params in the form:
//					COMx:baud,parity,databits,stopbits
//					where:	baud=the baudrate (9600, 19200, etc.)
//							parity=N,O,E
//							databits=5,6,7,8
//							stopbits=0,1,2
//out:	0		success
//		<0		error code: see sjack32.h for a list of error codes

__declspec(dllexport) int SJACKbind(word i,char *params)	//							***002
{	char	comx[]="COMx";
	byte	cp;
	dword	dw;

	if (ports[i].comport) return SJACK_AlreadyBound;
	cp=params[3]-'0';
	if ((i>=usedports)||(cp<1)||(cp>4)) return SJACK_InvalidPort;
	comx[3]='0'+cp;								//setup "COMx"
	if (!BuildCommDCB(params,&ports[i].dcb)) return SJACK_InvalidParameter;
	if ((ports[i].handle=CreateFile(comx,		//open port on COMx
				GENERIC_READ|GENERIC_WRITE,		//for reading and writing
				0,								//exclusive access
				NULL,							//no security
				OPEN_EXISTING,					//required for serial ports
				FILE_FLAG_OVERLAPPED,			//asynchronous access
				NULL))==INVALID_HANDLE_VALUE)	//it failed
	{	ports[i].comport=0;						//remember it failed
		return SJACK_CantCreateFile;
	}
	//it worked
	ports[i].dcb.fBinary=1;
	ports[i].dcb.fRtsControl=RTS_CONTROL_DISABLE;
	ports[i].dcb.fParity=0;
	ports[i].dcb.fOutxCtsFlow=0;
	ports[i].dcb.fOutxDsrFlow=0;
	ports[i].dcb.fDtrControl=DTR_CONTROL_ENABLE;
	ports[i].dcb.fDsrSensitivity=0;
	ports[i].dcb.fTXContinueOnXoff=1;
	ports[i].dcb.fOutX=0;
	ports[i].dcb.fInX=0;
	ports[i].dcb.fErrorChar=0;
	ports[i].dcb.fNull=0;
	ports[i].dcb.fAbortOnError=0;
	ports[i].dcb.EvtChar=0;							//don't really care
	ports[i].comport=cp;							//say we're configured
    if (!SetCommState(ports[i].handle,&ports[i].dcb))	//an error occurred
		{	ports[i].comport=0;						//remember it failed
			return SJACK_BindingFailed;
		}
	if (!SetCommMask(ports[i].handle,EV_ERR|EV_RXCHAR))		//we want events when error or after a time or after RX_CHAR***006
	{	ports[i].comport=0;							//remember it failed
		return SJACK_BindingFailed;
	}
	ClearCommError(ports[i].handle,&dw,NULL);		//reset comm errors
	PurgeComm(ports[i].handle,PURGE_RXABORT|PURGE_RXCLEAR);	//flush receive queue
	if (portRX[i].hEvent==NULL						//couldn't create the event object
			||
		!WaitCommEvent(ports[i].handle,&dw,&portRX[i]))	//or can't setup the event
	{	if (GetLastError()!=ERROR_IO_PENDING)
		{	ports[i].comport=0;						//remember it failed
			return SJACK_BindingFailed;
		}
	}
	return SJACK_Success;
}

///////////////////////////////////////////////////////////////////////
//	Deinitialization called from Application

__declspec(dllexport) void SJACKdeinit()
{	//int		i;								//										***009

	if (oktolisten==1&&listening==1)			//thread is still active				***004
	{	oktolisten=0;							//signal Listener to terminate
		while(oktolisten==0)					//wait for handshake
			Sleep(100L);
	}
//	CloseHandle(hListener);						//shut down our receiver thread
/*	for (i=0;i<usedports;i++)					//										***009 Begin
	{	if (portRX[i].hEvent) CloseHandle(portRX[i].hEvent);
		hportRX[i]=NULL;
		if (ports[i].comport!=0)				//this port is configured
			CloseHandle(ports[i].handle);		//shut it down
	}
*/												//										***009 End
}


///////////////////////////////////////////////////////////////////////
//	Get a Byte from a Port's Receive Queue
//
//in:	port	the port number (0..usedports-1)
//out:	-1		none available
//		else	the byte (actually an int is returned with the MS byte 
//							indicating good or bad data. MS=0=good, 
//							MS=1=framing error, MS=2=parity error, 
//							MS=3=overrun

__declspec(dllexport) int SJACKrx(word port)	//										***002
{	short		b;

	if ((port>=usedports)||(ports[port].rcvcount==0)) return -1;		//none available
	b=ports[port].rcv[ports[port].rcvf];		//get the front byte from the queue
	ports[port].rcvcount--;
	if (++ports[port].rcvf>=maxrcvqsize)		//wrapped around
		ports[port].rcvf=0;
	return b;
}

///////////////////////////////////////////////////////////////////////
//	Send a message
//
//in:	port	the port number (0..usedports-1)
//		bp		pointer to the buffer to send
//		n		number to send
//out:	>=0		number sent
//		else	error

__declspec(dllexport) int SJACKtx(word port,byte *bp,dword n)	//						***002
{	dword	dw;

	if ((port>=usedports)) return SJACK_InvalidPort;
	if (WriteFile(ports[port].handle,		//port handle
			bp,								//port's tx buffer
			n,								//length to transmit
			&dw,							//return actual length here
			&ports[port].ovTX))				//semaphore for completion
		return (int)dw;						//success									***003 Begin
	else
	{	if (GetLastError()==ERROR_IO_PENDING)
			return SJACK_TransmitterBusy;	//There's a pending transmit in progress on this port don't handle this guy right now
		else
			return SJACK_TransmitError;
	}										//											***003 End
}

//																						***006 Begin
///////////////////////////////////////////////////////////////////////
//	Send a character
//
//in:	port	the port number (0..usedports-1)
//		bp		pointer to the buffer to send
//		n		number to send
//out:	>=0		number sent
//		else	error

__declspec(dllexport) int SJACKtxchar(word port,byte b)
{	dword	dw;

	if ((port>=usedports)) return SJACK_InvalidPort;
	while ((TransmitCommChar(ports[port].handle,b))==false)
		dw=GetLastError();					//let's try the naive approach first and keep trying
	return 1;								//success
}
//																						***006 End
//I Am A Thread---I Am A Thread---I Am A Thread---I Am A Thread---I Am A Thread---
//--------------------------------------------------------------------------------
//	Handle incoming received frames
//
void Listener(void *dummy)
{	int			i,n;
	byte		rb[512];
	word		j;
	dword		dw,dwErrors;
	COMSTAT		cstat;
	OVERLAPPED	ros={0};

	while(oktolisten)							//loop forever (until shutdown by SJACKdeinit)
	{	listening=1;							//now we are running					***004
		dw=WaitForMultipleObjects(usedports,hportRX,false,ListenTime);	//block until one of our ports gets something (an error) or a timeout
		if ((dw>=WAIT_OBJECT_0)&&(dw<(WAIT_OBJECT_0+usedports)))		//got something
			i=dw-WAIT_OBJECT_0;									//i is the port which got something
		else if (dw==WAIT_TIMEOUT)
			i=0;												//we had a timeout here
		else
		{	dw=GetLastError();
//			oktolisten=2;										//						***008
//			return;												//this will terminate the thread!***008
		}
		for (n=0;n<usedports;n++)
		{	ResetEvent(hportRX[i]);								//reset the event signal manually
			ClearCommError(ports[i].handle,&dwErrors,&cstat);	//get port status
			dw=(cstat.cbInQue>512)?512:cstat.cbInQue;			//max we can handle is 256
			if (dw)												//we have something to deal with
			{
				//It isn't clear that we need to check ReadFile's return here.
				//dw should return with a count of bytes transferred into rb,
				//which should always match dw going in anyway. It isn't clear
				//whether there should be any reason, ever, why ros is actually
				//needed here, other than the fact that the port is opened with
				//FILE_FLAG_OVERLAPPED so ros is required. We know that there are
				//dw bytes available in the input queue because of the cbInQue count.
				//So...ReadFile should NEVER return a failure.
				ReadFile(ports[i].handle,rb,dw,&dw,&ros);	//try to read some of them
				for (j=0;j<dw;j++)
				{	if (ports[i].rcvcount==maxrcvqsize) //receive queue is full
						break;							//to stop accumulating	
					else								//plenty of room left
					{	ports[i].rcv[ports[i].rcvr]=(int)rb[j];
						ports[i].rcv[ports[i].rcvr]&=0xFF;	//put the byte in the rear of the queue
						if (j==dw-1)						//mark any errors
						{	if (dwErrors&CE_FRAME) ports[i].rcv[ports[i].rcvr]+=0x100;
							else if (dwErrors&CE_RXPARITY) ports[i].rcv[ports[i].rcvr]+=0x200;
							else if (dwErrors&CE_OVERRUN) ports[i].rcv[ports[i].rcvr]+=0x300;
						}
						ports[i].rcvcount++;				//one more in queue
						if (++ports[i].rcvr>=maxrcvqsize)	//wrap around pointer
							ports[i].rcvr=0;
					}
				}
			}
			WaitCommEvent(ports[i].handle,&dw,&portRX[i]);		//setup the event again
			i++;
		}
	}														//go back and wait again
	for (i=0;i<usedports;i++)					//										***009 Begin
	{	if (portRX[i].hEvent) CloseHandle(portRX[i].hEvent);
		hportRX[i]=NULL;
		portRX[i].hEvent=NULL;
		if (ports[i].comport!=0)				//this port is configured
		{	CloseHandle(ports[i].handle);		//shut it down
			ports[i].comport=0;
		}
	}												//									***009 End
	oktolisten=2;											//handshake with main process
}

