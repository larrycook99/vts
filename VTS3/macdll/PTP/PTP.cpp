// PTP.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "resource.h"

#include "sjack32.h"
#include "ptp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////
const char	szAppName[]		=	"PTP";
const char	ptpversion[]	=	"BACnet Point-To-Point MAC Layer v0.05";	//	***003
#define		ver100				5				//version*100							***003
const char	EstablishConnection[] = "BACnet\r";
//const char	PTPPrefs[]		=	"PTP.INI";		//preferences file, don't need preferences file any more.Xiao Shiyuan 2002-5 
///////////////////////////////////////////////////////////////////////
//	Module Constants
#define		maxlog				100				//max entries in log list
#define		maxtrace			100				//max entries in rx,tx trace lists
#define		tracewidth			64				//width of trace line in chars
#define		maxrx				512				//max chars in rx buffer
#define		TimerID				1				//main timer identifier
#define		TimerInterval		50				//50 ms									***003
#define		WM_USER_INITIATE	(WM_USER+1)
#define		WM_USER_SYSTRAY		(WM_USER+2)
#define		MENU_VIEW			0				//positional indices for menus
#define		MENU_TEST			1
#define		MENU_ABOUT			2

///////////////////////////////////////////////////////////////////////
//	global variables
BOOL        hasInited = false; //Xiao Shiyuan 2002-5-11 
HWND		hStatus=0;							//handle of our Status Dialog
HWND		hAbout=0;							//handle of our About Box window
HWND 		hLog=0;								//handle of log listbox 
HINSTANCE	hDLLInst=0;
HBITMAP		hRed=0,hGreen=0, hShake=0, hNoshake=0;

int			Running=40;							//downcounter for about box				***003
int			shakecnt=0;

enum		{lrsmIdle,lrsmPreamble,lrsmHeader,lrsmData}
			lrstate=lrsmIdle;					//frame receive machine state
enum		{csmDisconnected,csmOutbound,csmInbound,csmConnected,csmDisconnecting}
			cstate=csmDisconnected;				//connection state machine
enum		{tsmIdle,tsmBlocked,tsmReady,tsmPending}
			tstate=tsmIdle;						//transmit state machine
enum		{rsmIdle,rsmReady,rsmData,rsmDataAck,rsmDataNak}
			rstate=rsmIdle;						//reception state machine
enum		{notBlocked,isBlocked,almostBlocked}
			ReceptionBlocked=notBlocked;

octet		hb[4];								//header buffer
int			hindex=0;							//header index
octet		hcrc=0; //header CRC
octet		DLEMask=0;
word		dcrc=0; //Data CRC
dword		rxid=0;
int			SilenceTimer=0;
int			HeartbeatTimer=0;
int			RetryCount=0;
int			ResponseTimer=0;
int			rlen=0; //length of data
int			elen=0;
int			TxSequence=0;
int			RxSequence=0;
BOOL		TransmissionBlocked=false;
BOOL		AckReceived[2]={false,false};
BOOL		NakReceived[2]={false,false};
BOOL		logall=true;
byte		outboundpassword[32];				//buffer for outbound connections
octet		rbuf[maxrx]; //rx buffer
mpkt		*lasttx=NULL;
pktq		incoming={NULL,NULL,0};				//received packets
pktq		outgoing={NULL,NULL,0};				//transmit requested packets

int			Nretries=3;							//number of times a frame is sent
int			Tconnrqst=15;
int			Tconnrsp=15;
int			Tframeabort=2;
int			Theartbeat=15;
int			Tinactivity=60;
int			Tresponse=5;

char		logfile[144];						//path to logfile
char		ConnectPassword[32];
char		ptpname[]="PTP on COMxx";

int			rxbol=0, txbol=0;
int			rxindex=0, txindex=0;
char		rxtrace[tracewidth+1]={0};
char		txtrace[tracewidth+1]={0};

///////////////////////////////////////////////////////////////////////
//	function prototypes

LRESULT CALLBACK MainWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK About(HWND,UINT,WPARAM,LPARAM); 
int		AddItem(int,const char *);
void	AddSlash(char *);
void	ClearItems(int);
word	CalcDataCRC(octet,word);
octet	CalcHeaderCRC(octet,octet);
void	ConnectionStateMachine(void);
void	DrawBitmap(HWND,HBITMAP,int,int);
int		EnterItem(int,const char *);
octet 	*EscapeOctets(octet *,octet);
void	EverySecond(void);
void	ExitPTP(void);
void	ForceDisconnect(void);
int		GetListCount(HWND,int);
int		GetListText(int,int,char *);
mpkt	*GetQ(pktq *);
void	GotActivity(void);
word	HexParse(char *,int);
void	LogToFile(char *);
void	MainLoop(void); 
void	MakeLog(char *);
void	MakeLogAll(char *);
void	NewTrace(int);
void	PutQ(pktq *,mpkt *);
int		ReadPreferences(const char *config);
void	ReceiveFrameStateMachine(void);
void	ReceiveIndicate(int, byte, int, byte*, word);
void	ReceptionStateMachine(void);
void	RemoveItem(HWND,int,int);
int		ReplaceItem(int,int,char *);
void	SelectItem(int,int);
int		Send(byte *,dword);
int		SendFrame(int,int,octet *);
void 	SetItemVisible(HWND,int,BOOL);
void	ShowAboutWindow(int);
void	ShowHandshake(void);
void	ShowIdle(BOOL);
void	Startup(void);
void	TimerEvent(void);
void	Trace(int,octet);
void	TransmissionStateMachine(void);
int		YesNo(char *);

/////////////////////////////////////////////////////////////////////////
static AFX_EXTENSION_MODULE PTPDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);	

	hDLLInst = hInstance;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("PTP.DLL Initializing!\n");		

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(PTPDLL, hInstance))
			return 0;	
		
		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(PTPDLL);		
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("PTP.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(PTPDLL);
	}	

	return 1;   // ok
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//	initialize serial port
//
//out:	ptpERSuccess	ok
//		else			ptpERxxx error

AFX_EXT_API int ptpInit(const char *config)
{	int			r;
	WNDCLASS	wc;
	HFONT		hFont=NULL;

	if(hasInited)
	 return ptpERSuccess;
	
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = DLGWINDOWEXTRA;
    wc.hInstance = hDLLInst;
    wc.hIcon = LoadIcon(hDLLInst, MAKEINTRESOURCE(IDI_PTP));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (struct HBRUSH__ *)GetStockObject(LTGRAY_BRUSH);
    wc.lpszMenuName =  "MainMenu";
    wc.lpszClassName = "PTPCL";
    if (!RegisterClass(&wc))
    	return ptpERFailedClassRegistration;		//failed

    wc.lpfnWndProc = About;
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "PTPABOUTCL";
    if (!RegisterClass(&wc))
    	return ptpERFailedClassRegistration;		//failed
    
//	create the windows we need and allocate a timer
    hStatus = CreateDialog(hDLLInst, MAKEINTRESOURCE(IDD_PTPSTATUS),0,NULL);
    if (!hStatus) 
	{
		DWORD error = GetLastError();
		return ptpERCantCreateWindow;   
	}
	hAbout = CreateDialog(hDLLInst,MAKEINTRESOURCE(IDD_PTPABOUT),0,NULL);	// Begin
    if (!hAbout)
	{	DestroyWindow(hStatus);						//we don't need the window now
      	return ptpERCantCreateWindow;
	}
    hLog=GetDlgItem(hStatus,IDC_LOG);
    
    if (!SetTimer(hStatus,TimerID,TimerInterval,NULL))
	{	DestroyWindow(hStatus);						//we don't need the window now
      	return ptpERNoFreeTimers;					// End
	}
   
	if((r=ReadPreferences(config))<0) return r;

	hRed=LoadBitmap(hDLLInst,MAKEINTRESOURCE(IDB_REDLED));
	hGreen=LoadBitmap(hDLLInst,MAKEINTRESOURCE(IDB_GREENLED));
	hShake=LoadBitmap(hDLLInst,MAKEINTRESOURCE(IDB_HANDSHAKE));
	hNoshake=LoadBitmap(hDLLInst,MAKEINTRESOURCE(IDB_NOSHAKE));
     
    hFont=CreateFont(15,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
   					  CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_MODERN|FIXED_PITCH,
   					  "Courier New");
   	SendDlgItemMessage(hStatus,IDC_RX,WM_SETFONT,(WPARAM)hFont,0);
   	SendDlgItemMessage(hStatus,IDC_TX,WM_SETFONT,(WPARAM)hFont,0);
	SetWindowText(GetDlgItem(hAbout,IDC_RUNTITLE),ptpversion); //show our version string
	SetWindowText(hStatus,ptpversion); 				//also in main window
	ShowWindow(hStatus,SW_HIDE);					//make main window invisible
	CheckMenuItem(GetSubMenu(GetMenu(hStatus),MENU_VIEW),
				  ID_TRACEALL,
				  MF_BYCOMMAND | (logall ? MF_CHECKED : MF_UNCHECKED));
	ShowHandshake();								//update handshake status initially
	ShowAboutWindow(1);								//display about box for 1 second 
    SendMessage (hStatus,WM_USER_INITIATE,0,0L);	//Get things going
	return ptpERSuccess;							//success
}

///////////////////////////////////////////////////////////////////////
//	deinitialize serial port

AFX_EXT_API void ptpDeinit()
{
	ExitPTP();										//shut down everything
}

///////////////////////////////////////////////////////////////////////
//	get our port name
//
//in:	p				points to string which is appended with port name

AFX_EXT_API void ptpCatName(char *p)
{
	strcat(p,ptpname);								//append our name to it
}

///////////////////////////////////////////////////////////////////////
//	try to connect (like a DL-CONNECT.request)
//
//in:	pass			points to password (or NULL)
//out:	ptpERSuccess	ok
//		else			ptpERxxx error

AFX_EXT_API int ptpConnect(char *pass)
{	int			r;

	if(cstate!=csmDisconnected)
	{	MakeLogAll("Reject Connect: presently connected");
		return ptpERNotDisconnected;
	}
	if(pass==NULL)
		outboundpassword[0]=0;
	else
	{	if(strlen(pass)>(sizeof(outboundpassword)-1))
		{	MakeLogAll("Reject Connect: password too long");
			return ptpERPasswordTooLong;
		}
		strcpy((char *)outboundpassword,pass);
	}
	MakeLogAll("Try to establish connection");

	//send "BACnet\r" sequence, not include '\0'
	if (r=Send((byte *)EstablishConnection,sizeof(EstablishConnection)-1))
			return r+ptpERSjack;

	RetryCount=0;
	ResponseTimer=0;
	cstate=csmOutbound; //change connection state to outbound
	return ptpERSuccess;
}

///////////////////////////////////////////////////////////////////////
//	try to disconnect (like a DL-DISCONNECT.request)
//
//in:	reason			0=done, 1=preempted, 3=other
//out:	ptpERSuccess	ok
//		else			ptpERxxx error

AFX_EXT_API int ptpDisconnect(octet reason)
{
	if(cstate!=csmConnected)
	{	MakeLogAll("Reject Disconnect: not connected");
		return ptpERNotConnected;
	}
	SendFrame(ftDisconnectRequest,1,&reason);
	ResponseTimer=0;
	//in theory we issue a DL-CONNECT.confirm(success) here
	MakeLogAll("Disconnect requested");
	cstate=csmDisconnecting;
	return ptpERSuccess;
}

///////////////////////////////////////////////////////////////////////
//	try to transmit (like a DL-UNITDATA.request)
//
//in:	mp				points to MApacket to transmit
//out:	ptpERSuccess	ok
//		else			ptpERxxx error

AFX_EXT_API int ptpTransmit(byte *mp, int len)
{	mpkt	*p;

	if((p=(mpkt *)malloc(sizeof(mpkt)))==NULL)
	{	MakeLogAll("Transmit failed: no free buffers");
		return ptpERNoFreeBuffers;
	}
	if(len>sizeof(p->buf))
	{	free(p);
		MakeLogAll("Transmit failed: request too large");
		return ptpERTransmitRequestTooLarge;
	}
	CopyMemory(p->buf,mp,len);
	p->length=len;
	PutQ(&outgoing,p);
	return ptpERSuccess;
}

///////////////////////////////////////////////////////////////////////
//	try to see if there is a received packet available
//
//in:	mp				points to MApacket to fill-in
//out:	true			packet available, mp filled-in

AFX_EXT_API BOOL ptpReceive(PKTPTP *mp)
{
	mpkt	*p;	
	
	if((p=GetQ(&incoming))==NULL)
		return false;
		
	mp->pktPreamble = 0x55FF;
	mp->pktFrameType = p->frametype;
	mp->pktDataCRC = p->dataCRC;	
	mp->pktLen = p->length;
	mp->pktHeaderCRC = p->headerCRC;			
		
	CopyMemory(mp->pktData, p->buf, p->length);	
	
	free(p);	

	return true;																
}


///////////////////////////////////////////////////////////////////////
//	Timer Event Handler (every 50ms)

void TimerEvent()
{	static int	InMainLoop=false;					//lock flag
	static int	SecondsCounter=20;					//								***003
	
	if (Running)									//showing about box only
	{	if (--Running==0)							//end of about box display
			ShowWindow(hAbout,SW_HIDE);
		return;										//all we need to do in run mode
	}

	if ((SecondsCounter--)==0)						//a second has gone by
	{	SecondsCounter=20;							//reset downcounter				***003
	    EverySecond();								//20*50ms = 1 second, do once per second stuff here
	}    
    if (InMainLoop) return;							//don't reenter this part yet
    InMainLoop=true;								//lock out reentrancy
    MainLoop();										//perform main loop functions
    InMainLoop=false;								//release reentrancy lock
}

///////////////////////////////////////////////////////////////////////
//	Called Every Second

void EverySecond()
{
	SilenceTimer++; //SilenceTimer adds a second
	ResponseTimer++;
	HeartbeatTimer++;
	if (shakecnt)
	{	shakecnt--;
		if (shakecnt==0) ForceDisconnect();
	}
}

///////////////////////////////////////////////////////////////////////
//	Main Loop (every 50ms)															***003

void MainLoop()
{
	ReceiveFrameStateMachine();
	ConnectionStateMachine();
	TransmissionStateMachine();
	ReceptionStateMachine();
}

///////////////////////////////////////////////////////////////////////
//	Receive Frame State Machine

void ReceiveFrameStateMachine()
{	int		c;
	
	while(true)
	{	if(SilenceTimer>Tframeabort)			//timed out waiting
			goto ResetToIDLE;
		if ((c=SJACKrx(0))<0) break;			//no characters available, quit now
		ShowIdle(false);
		//some error occurred,because the max value of a correct byte  is 255
		//Eat An Error
		if (c>255)								
		{
ResetToIDLE:
			lrstate=lrsmIdle;					//reset our state to IDLE
			hindex=0;
			SilenceTimer=0;
			break;
		}
		//Flow Control
		if (c==0x11||c==0x13)					//always ignore XON/XOFF
		{	SilenceTimer=0;
			break;
		}
		switch(lrstate)							//handle it based on present state
		{
//[IDLE]---------------------------------------------------------------------------
		case lrsmIdle:
			SilenceTimer=0;
			if (c==0x55)						//preamble start
			{	lrstate=lrsmPreamble;			//PREAMBLE state
				NewTrace(IDC_RX);
				Trace(IDC_RX,(octet)c);
			}
			else
			{	Trace(IDC_RX,(octet)c);
				if (cstate==csmDisconnected &&
					c==EstablishConnection[hindex])	//is this the connection message?
				{	if (EstablishConnection[++hindex]==0)	//was this the end?
					{	SendFrame(ftConnectRequest,0,NULL);
						RetryCount=0;
						ResponseTimer=0;
						cstate=csmInbound;
						hindex=0;
						MakeLogAll("Got Inbound Connection Message");
					}
				}
				else
					hindex=0;					//not the connection message
			}
			break;
//[PREAMBLE]-----------------------------------------------------------------------
		case lrsmPreamble:
			Trace(IDC_RX,(octet)c);
			//Repeated Preamble
			if(c==0x55)							//preamble was repeated
			{	SilenceTimer=0;
				break;
			}
			if(c!=0xFF)							//not the second preamble octet
				goto ResetToIDLE;
			SilenceTimer=0;
			DLEMask=0x00;
			hcrc=0xFF;
			hindex=0;
			lrstate=lrsmHeader;					//HEADER state
			break;
//[HEADER]-----------------------------------------------------------------------
		case lrsmHeader:
			Trace(IDC_RX,(octet)c);
			//DLE Received
			if(c==0x10)							
			{	DLEMask=0x80;
				break;
			}

			//Frame Type when hindex = 0
			//Length1 when hindex = 1, hb[1] is the high byte of frame length 
			//Length2 when hindex = 2, hb[2] is the high byte of frame length
			//HeaderCRC when hindex = 3
			hb[hindex]=(octet)(c&(~DLEMask));
			hcrc=CalcHeaderCRC(hb[hindex],hcrc);
			hindex++;
			DLEMask=0x00;

			//HEADER CRC state, need not receive a byte
			if(hindex==4)						//got a header
			{	if(hcrc!=0x55)
					goto ResetToIDLE;			//bad header CRC

				rlen=(hb[1]<<8)+hb[2];			//get data length

				//Frame Too Long
				if(rlen>sizeof(rbuf))
				{	MakeLog("Received a too large frame");
					goto ResetToIDLE;
				}

				if(rlen==0)
				{	ReceiveIndicate(hb[0], hb[3], 0, NULL, 0);	//no data for this one
					goto ResetToIDLE;
				}

				//Data
				hindex=0;
				dcrc=0xFFFF;
				elen=rlen+2;					//data+2 CRC octets
				lrstate=lrsmData;				//DATA state
			}
			break;
//[DATA]-----------------------------------------------------------------------
		case lrsmData:
			Trace(IDC_RX,(octet)c);

			//DLE Received
			if(c==0x10)							
			{	DLEMask=0x80;
				break;
			}
			//CRC 1 when hindex = rlen
			//CRC 2 when hindex = rlen + 1			
			rbuf[hindex]=(octet)(c&(~DLEMask));
			dcrc=CalcDataCRC(rbuf[hindex],dcrc);
			hindex++;
			DLEMask=0x00;

			//DATA CRC State
			if(hindex==elen)					//got a complete frame
			{
				//BAD CRC
				if(dcrc!=0xF0B8)
				{	MakeLog("BAD CRC on received frame");
					goto ResetToIDLE;
				}

				//Good CRC
				//rbuf is the data field of the PTP frame and is not the entire PTP frame
				ReceiveIndicate(hb[0], hb[3], rlen, rbuf, rbuf[rlen] + (rbuf[rlen + 1] << 8) ); 

				goto ResetToIDLE;
			}
			break;
//---------------------------------------------------------------------------------
		}
	}
	ShowIdle(true);
}

///////////////////////////////////////////////////////////////////////
//	Connection State Machine

void ConnectionStateMachine()
{
	switch(cstate)								//handle it based on present state
	{
//[DISCONNECTED]----------------------------------------------------------------------
	case csmDisconnected:
		break;

//[OUTBOUND]--------------------------------------------------------------------------
	case csmOutbound:
		if(ResponseTimer>=Tconnrsp)				//no response from caller
		{	if(RetryCount<Nretries)				//can we try again?
			{	RetryCount++;					//yes
				ResponseTimer=0;
				MakeLogAll("Retrying Outbound Establish Connection");
				Send((byte *)EstablishConnection,sizeof(EstablishConnection)-1);
			}
			else
			{	//in theory we issue a DL-CONNECT.confirm(fail) here
				MakeLogAll("Outbound Connection Failed");
				ForceDisconnect();				//never mind, must be gone now
			}
		}
		break;

//[INBOUND]---------------------------------------------------------------------------
	case csmInbound:
		if(ResponseTimer>=Tconnrsp)				//no response from caller
		{	if(RetryCount<Nretries)				//can we try again?
			{	RetryCount++;					//yes
				ResponseTimer=0;
				MakeLogAll("Retrying Inbound Connect Request");
				SendFrame(ftConnectRequest,0,NULL);
			}
			else
			{	MakeLogAll("Give up on Inbound connection request");
				ForceDisconnect();				//never mind, must be gone now
			}
		}
		break;

//[CONNECTED]-------------------------------------------------------------------------
	case csmConnected:
		break;

//[DISCONNECTING]---------------------------------------------------------------------
	case csmDisconnecting:
		if(ResponseTimer>=Tresponse)			//no response from caller
		{	if(RetryCount<Nretries)				//can we try again?
			{	RetryCount++;					//yes
				ResponseTimer=0;
				MakeLogAll("Retrying Disconnect Request");
				SendFrame(ftDisconnectRequest,0,NULL);
			}
			else
			{	MakeLogAll("Give up on disconnect response");
				ForceDisconnect();				//never mind, must be gone now
			}
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////
//	Force disconnection

void ForceDisconnect()
{
	shakecnt=0;
	MakeLog("Lost handshake with PTP Peer");
	ShowHandshake();
	cstate=csmDisconnected;
}

///////////////////////////////////////////////////////////////////////
//	Transmission State Machine

void TransmissionStateMachine()
{
	switch(tstate)								//handle it based on present state
	{
//[IDLE]----------------------------------------------------------------------
	case tsmIdle:
		if(cstate==csmConnected)
		{	if(ReceptionBlocked==notBlocked)
				SendFrame(ftHeartbeatXON,0,NULL);
			else
				SendFrame(ftHeartbeatXOFF,0,NULL);
			TxSequence=0;
			tstate=tsmBlocked;
		}
		break;
//[BLOCKED]----------------------------------------------------------------------
	case tsmBlocked:
		if(!TransmissionBlocked)				//our peer is back
			tstate=tsmReady;
		else if(cstate==csmDisconnected)		//we've been disconnected
			tstate=tsmIdle;
		else if(HeartbeatTimer>Theartbeat)		//just heartbeat
		{	if(ReceptionBlocked==notBlocked)
				SendFrame(ftHeartbeatXON,0,NULL);
			else
				SendFrame(ftHeartbeatXOFF,0,NULL);
			HeartbeatTimer=0;					//so we don't send it again
		}
		break;
//[READY]----------------------------------------------------------------------
	case tsmReady:
		if(cstate==csmDisconnected)				//we've been disconnected
			tstate=tsmIdle;
		else
		{	if(TransmissionBlocked)				//peer asked us to stop
				tstate=tsmBlocked;
			else if(outgoing.count>0)			//got something to transmit
			{	RetryCount=0;
				ResponseTimer=0;
				tstate=tsmPending;
				lasttx=GetQ(&outgoing);			//get next one to transmit
				MakeLogAll("Transmit frame...");
				SendFrame(ftData0+TxSequence,lasttx->length,&lasttx->buf[0]);
			}
			else if(HeartbeatTimer>Theartbeat)	//just heartbeat
			{	if(ReceptionBlocked==notBlocked)
					SendFrame(ftHeartbeatXON,0,NULL);
				else
					SendFrame(ftHeartbeatXOFF,0,NULL);
				HeartbeatTimer=0;				//so we don't send it again
			}
		}
		break;
//[PENDING]----------------------------------------------------------------------
	case tsmPending:
		if(cstate==csmDisconnected)				//we've been disconnected
			tstate=tsmIdle;
		else
		{	if(AckReceived[TxSequence])			//we got acknowledgement
			{	TxSequence=1-TxSequence;
				AckReceived[0]=AckReceived[1]=false;
				free(lasttx);					//free up the transmit packet
				lasttx=NULL;
				tstate=tsmReady;
			}
			else if(NakReceived[TxSequence]||ResponseTimer>Tresponse)
			{	if(RetryCount<Nretries)
				{	RetryCount++;
					MakeLogAll("Retransmit frame(no ACK received)...");
					SendFrame(ftData0+TxSequence,lasttx->length,&lasttx->buf[0]);
				}
				else
				{	RetryCount=0;
					free(lasttx);				//free up the transmit packet
					lasttx=NULL;
					tstate=tsmReady;
				}
				NakReceived[0]=NakReceived[1]=false;
				ResponseTimer=0;
			}
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////
//	Reception State Machine

void ReceptionStateMachine()
{
	switch(rstate)								//handle it based on present state
	{
//[IDLE]----------------------------------------------------------------------
	case rsmIdle:
		if(cstate==csmConnected)
		{	RxSequence=0;
			rstate=rsmReady;
		}
		break;
//[READY]----------------------------------------------------------------------
	case rsmReady:
		if(cstate==csmDisconnected)
			rstate=rsmIdle;
		break;
	}
}

///////////////////////////////////////////////////////////////////////
//	Handle Incoming Frame
//in:	ft			frame type
//		rlen		received length
//		rp			receive buffer pointer (or NULL if rlen==0)

void ReceiveIndicate(int ft, byte headerCRC, int rlen, byte *rp, word dataCRC)
{	octet	ans;
	BOOL	disconn=false, xoff=true;
	mpkt	*m;
	int		x;


	switch(ft)									//based on frame type...
	{
	case ftData0:
	case ftData1:
		MakeLogAll("<Data>");
		GotActivity();
		x=ft-ftData0;							//get 0 or 1 in x
		if(ReceptionBlocked==isBlocked)			//can't eat it
			SendFrame(((x==RxSequence)?ftDataNak0XOFF:ftDataAck0XOFF)+x,0,NULL);
		else
		{	if(x!=RxSequence)
				SendFrame(ftDataAck0XON+x,0,NULL);
			else
			{	if((m=(mpkt *)malloc(sizeof(mpkt)))==NULL)	//can't get a buffer for it
					SendFrame(ftDataNak0XON+x,0,NULL);
				else
				{	
					m->frametype = ft;
					m->length=rlen;
					m->headerCRC = headerCRC;
					CopyMemory(&m->buf[0],rp,rlen);	//rlen will always be <512 length
					m->dataCRC = dataCRC;
					PutQ(&incoming,m);			//issue DL-UNITDATA.indication here
					SendFrame(((ReceptionBlocked==notBlocked)?ftDataAck0XON:ftDataAck0XOFF)+x,0,NULL);
					RxSequence=1-RxSequence;
				}
			}
		}
		break;

	case ftHeartbeatXON:
		xoff=false;
		//fall through intentionally
	case ftHeartbeatXOFF:
		MakeLogAll("<Heartbeat>");
		GotActivity();
		TransmissionBlocked=xoff;
		//Strictly speaking this is probably not a good thing to do.
		//However, if we get a heartbeat, then we should be connected right?
		//I did this because after a restart, if you've been gone less than Tinactivity
		//then your peer doesn't know you've restarted. What he doesn't know shouldn't
		//hurt him. The DISCONNECTED state in the connection machine does not rule
		//on incoming frames...
		cstate=csmConnected;
		break;

	case ftDataAck0XON:
	case ftDataAck1XON:
		xoff=false;
		//fall through intentionally
	case ftDataAck0XOFF:
	case ftDataAck1XOFF:
		MakeLogAll("<DataAck>");
		GotActivity();
		x=ft&1;									//low bit is 0/1
		TransmissionBlocked=xoff;
		if(x==TxSequence)
			AckReceived[x]=true;
		break;

	case ftDataNak0XON:
	case ftDataNak1XON:
		xoff=false;
		//fall through intentionally
	case ftDataNak0XOFF:
	case ftDataNak1XOFF:
		MakeLogAll("<DataNak>");
		GotActivity();
		x=ft&1;									//low bit is 0/1
		TransmissionBlocked=xoff;
		if(x==TxSequence)
			NakReceived[x]=true;
		break;

	case ftConnectRequest:
		MakeLogAll("<ConnectRequest>");
		if(cstate==csmDisconnecting) goto rix;
		SendFrame(ftConnectResponse,strlen((const char*)outboundpassword),outboundpassword);
		MakeLogAll("<-Sent ConnectResponse");
		//In theory,
		//if(cstate==csmOutbound)
		//	issue a DL-CONNECT.confirm here
		//else
		//	issue a DL-CONNECT.indication here
		cstate=csmConnected;
		break;

	case ftConnectResponse:
		MakeLogAll("<ConnectResponse>");
		if(cstate==csmDisconnecting) goto rix;
		if(*ConnectPassword)					//we need a password
		{	if(((word)rlen)!=strlen(ConnectPassword))	//password was bad
				disconn=true;
			else if(strncmp(ConnectPassword,(const char*)rp,rlen)!=0)
				disconn=true;
			if(disconn)
			{
				cstate=csmDisconnecting;
				ans=0x02;						//received password is invalid
				SendFrame(ftDisconnectRequest,1,&ans);
				break;
			}
		}
		//In theory, we issue a DL-CONNECT.indication here
		cstate=csmConnected;
		break;

	case ftDisconnectRequest:
		MakeLogAll("<DisconnectRequest>");
rix:	SendFrame(ftDisconnectResponse,0,NULL);
		//if(cstate==csmConnected)
		//	issue a DL-DISCONNECT.indication here
		ForceDisconnect();
		break;

	case ftDisconnectResponse:
		MakeLogAll("<DisconnectResponse>");
		ForceDisconnect();
		break;

	case ftTestRequest:
		MakeLogAll("<TestRequest>");
		GotActivity();
		SendFrame(ftTestResponse,rlen,rp);
		break;

	case ftTestResponse:
		MakeLogAll("<TestResponse>");
		GotActivity();
		//	issue a DL-UNITDATA.indication here
		break;

	}
}

///////////////////////////////////////////////////////////////////////
//	Shut down the DLL

void ExitPTP()
{	NOTIFYICONDATA	tnd;

	tnd.uID=100;
	tnd.cbSize=sizeof(tnd);
	tnd.hWnd=hStatus;
	tnd.uCallbackMessage=WM_USER_SYSTRAY;
	Shell_NotifyIcon(NIM_DELETE,&tnd);			// End

	KillTimer(hStatus,TimerID);					//give up our timer
	SJACKdeinit();								//shut off serial module
	DeleteObject(hRed);							//give back resources
	DeleteObject(hGreen);
	DeleteObject(hShake);
	DeleteObject(hNoshake);
	DestroyWindow(hStatus);						//shut off main window
    UnregisterClass("PTPCL",hDLLInst);			//give up classes we've registered
    UnregisterClass("PTPABOUTCL",hDLLInst);
}

///////////////////////////////////////////////////////////////////////
//	Display the About Window
                            
void ShowAboutWindow(int forsecs)
{	Running=forsecs*20;							//convert to 50ms ticks					***003                           
    ShowWindow(hAbout,SW_SHOWNORMAL);
}

///////////////////////////////////////////////////////////////////////
//	About Window Message Processor

LRESULT CALLBACK About(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
    switch (message)
    { 
      case WM_CREATE:
      case WM_DESTROY:
       	break;
       	
      default:
       	return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return 0;
}
     
///////////////////////////////////////////////////////////////////////
//	Main Window Message Processor

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{   
	HMENU hmenu;
	BOOL check;
	long	cWnd;

    switch (message)
    { case WM_COMMAND:
      {	switch(wParam)
      	{
          case ID_RESET:
		   	ClearItems(IDC_LOG);
           	return 0;

          case IDC_RXLABEL:
          case ID_RESETRX:
		   	ClearItems(IDC_RX);
           	return 0;

          case IDC_TXLABEL:
          case ID_RESETTX:
		   	ClearItems(IDC_TX);
           	return 0;

		  case ID_TRACEALL:
			hmenu = GetSubMenu(GetMenu(hWnd),MENU_VIEW);	//get handle to View popup menu
			check = GetMenuState(hmenu,wParam,MF_BYCOMMAND) & MF_CHECKED;
			CheckMenuItem(hmenu,wParam,MF_BYCOMMAND | (check ? MF_UNCHECKED : MF_CHECKED));
			if (check)
			{	logall=false;
				MakeLog("!Tracing Off");
			}
			else
			{	logall=true;
				MakeLog("!Tracing On");
			}
			return 0;

          case ID_HELP_ABOUTPTP:
			ShowAboutWindow(10);				//turn on the about window for 10 seconds
           	return 0;

          default:
           	return (DefWindowProc(hWnd, message, wParam, lParam));
       	} //switch wParam
      } // case WM_COMMAND

	  case WM_USER_INITIATE:
		Startup();								//do all startup things
		return 0;

	  case WM_USER_SYSTRAY:						//handle systray messages Begin
		switch(lParam)							//handle based on message
		{
		case WM_LBUTTONDOWN:
			ShowWindow(hStatus,SW_SHOW);		//make main window visible
			SetWindowPos(hStatus,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			break;
		case WM_RBUTTONDOWN:
			//popup menu goes here
			break;
		}
		return 0;

	  case WM_CLOSE:
		ShowWindow(hStatus,SW_HIDE);			//make main window invisible
		return 0;								// End
		
      case WM_TIMER:
        TimerEvent();							//perform timer stuff
        return 0;
        
      case WM_CREATE:
       	return 0;

      case WM_DESTROY:
        if (hAbout) DestroyWindow(hAbout);
       	return 0;
       	
      case WM_CTLCOLORLISTBOX:					//colorize controls
        cWnd=GetWindowLong((HWND)lParam,GWL_ID);
      	if (cWnd==IDC_RX)
      		SetTextColor((HDC)wParam,RGB(255,0,0));
      	else if (cWnd==IDC_TX)
      		SetTextColor((HDC)wParam,RGB(0,0,128));
      	else
      		return (LRESULT) NULL;

		SetBkMode((HDC)wParam,OPAQUE);
   		return (LRESULT)GetStockObject(WHITE_BRUSH);

      default:
       	return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//	Startup code called from WM_USER_INITIATE

void Startup()
{	NOTIFYICONDATA	tnd;
	char	b[128];

	ClearItems(IDC_LOG);
	strcpy(b,ptpversion);
	strcat(b," Restarted");
	MakeLog(b);
	strcpy(tnd.szTip,ptpversion);				//tooltip
	tnd.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uID=100;
	tnd.cbSize=sizeof(tnd);
	tnd.hWnd=hStatus;
	tnd.uCallbackMessage=WM_USER_SYSTRAY;
	tnd.hIcon=LoadIcon(hDLLInst, MAKEINTRESOURCE(IDI_PTPTRAY));
	Shell_NotifyIcon(NIM_ADD,&tnd);
}

///////////////////////////////////////////////////////////////////////
//	Draw a bitmap on a window
//in:	hWnd	handle of window to draw on
//		hBM		handle of bitmap
//		x,y		window-relative coordinates to draw to

void DrawBitmap (HWND hWnd,HBITMAP hBM,int x,int y)
{	BITMAP	bm;
	HDC		hDC,hDCmem;
	POINT	ptSize,ptOrg;
	
	hDC=GetDC(hWnd);							//get window's DC
	hDCmem=CreateCompatibleDC(hDC);				//temporarily make another DC for the bitmap
	SelectObject(hDCmem,hBM);					//let it use the bitmap
	SetMapMode(hDCmem,GetMapMode(hDC));
	GetObject(hBM,sizeof(BITMAP),(LPSTR) &bm);	//get bitmap's size info
	ptSize.x=bm.bmWidth;
	ptSize.y=bm.bmHeight;
	DPtoLP(hDC,&ptSize,1);
	ptOrg.x=0;
	ptOrg.y=0;
	DPtoLP(hDCmem,&ptOrg,1);
	BitBlt(hDC,x,y,ptSize.x,ptSize.y,hDCmem,ptOrg.x,ptOrg.y,SRCCOPY);
	DeleteDC(hDCmem);
	ReleaseDC(hWnd,hDC);
}	

///////////////////////////////////////////////////////////////////////
//	Show Idle/Busy LEDs
//in:	st	true if idle, false if busy

void ShowIdle (BOOL st)
{	int	x=15,y=44;
	DrawBitmap(hStatus,(st ? hGreen : hRed),x,y);
}

///////////////////////////////////////////////////////////////////////
//	Show Handshake Status

void ShowHandshake()
{	int	x=10,y=70;
	DrawBitmap(hStatus,(shakecnt ? hShake : hNoshake),x,y);
}

///////////////////////////////////////////////////////////////////////
//	We have some activity from our peer

void GotActivity()
{	if(shakecnt==0)								//he woke up
		MakeLog("Restored handshake with PTP peer");
	shakecnt=Tinactivity;						//restore timer
	ShowHandshake();
}

///////////////////////////////////////////////////////////////////////
//	return 0 or 1 depending on the yes/no value in a string
//in:	*r	points to string
//out:	1	if yes or 1 or Y
//		0	otherwise

int YesNo (char *r)
{
  if (*r=='y'||*r=='Y'||*r=='1') return 1;
  return 0;
}
 
///////////////////////////////////////////////////////////////////////
// Make a dialog control visible or invisible

void SetItemVisible(HWND hw,int item,BOOL v)
{
	ShowWindow(GetDlgItem(hw,item),v?SW_SHOWNORMAL:SW_HIDE);
}

///////////////////////////////////////////////////////////////////////
//	remove all list items from one of the combos in Status Dialog

void ClearItems(int nDlgItem)
{ SendDlgItemMessage(hStatus,nDlgItem,(nDlgItem&IsCombo)?CB_RESETCONTENT:LB_RESETCONTENT,0, 0L);}

///////////////////////////////////////////////////////////////////////
//	get the list count from a list

int GetListCount(HWND hDlg,int nDlgItem)
{ return (int) SendDlgItemMessage(hDlg,nDlgItem,(nDlgItem&IsCombo)?CB_GETCOUNT:LB_GETCOUNT,0, 0L);}

///////////////////////////////////////////////////////////////////////
//	get the text from a list item in one of the combos in Status Dialog

int GetListText(int nDlgItem,int nIndex,char *p)
{ return (int) SendDlgItemMessage(hStatus,nDlgItem,(nDlgItem&IsCombo)?CB_GETLBTEXT:LB_GETTEXT,nIndex,(LPARAM) ((LPSTR) p));}

///////////////////////////////////////////////////////////////////////
//	replace text in a list item in one of the combos in Status Dialog

int ReplaceItem(int nDlgItem,int nIndex,char *p)
{	SendDlgItemMessage(hStatus,nDlgItem,(nDlgItem&IsCombo)?CB_DELETESTRING:LB_DELETESTRING,nIndex, 0L);	//first delete that item
	return (int) SendDlgItemMessage(hStatus,nDlgItem,(nDlgItem&IsCombo)?CB_INSERTSTRING:LB_INSERTSTRING,nIndex,(LPARAM) ((LPSTR) p));
}

///////////////////////////////////////////////////////////////////////
//	remove a list item from any list

void RemoveItem(HWND hDlg,int nDlgItem,int nIndex)
{ SendDlgItemMessage(hDlg,nDlgItem,(nDlgItem&IsCombo)?CB_DELETESTRING:LB_DELETESTRING,nIndex, 0L);}

///////////////////////////////////////////////////////////////////////
//	select a list item from one of the combos in Status Dialog

void SelectItem(int nDlgItem,int nIndex)
{ SendDlgItemMessage(hStatus,nDlgItem,(nDlgItem&IsCombo)?CB_SETCURSEL:LB_SETCURSEL,nIndex, 0L);}

///////////////////////////////////////////////////////////////////////
//	add a list item to one of the combos in Status Dialog

int AddItem(int nDlgItem,const char *p)
{	int	r;
	r=(int) SendDlgItemMessage(hStatus,nDlgItem,(nDlgItem&IsCombo)?CB_ADDSTRING:LB_ADDSTRING,0,(LPARAM) ((LPSTR) p));
	return r;
}

///////////////////////////////////////////////////////////////////////
//	add and select a list item to one of the combos in Status Dialog

int EnterItem(int nDlgItem,const char *p)
{	int	r;
	r=(int) SendDlgItemMessage(hStatus,nDlgItem,(nDlgItem&IsCombo)?CB_ADDSTRING:LB_ADDSTRING,0,(LPARAM) ((LPSTR) p));
	SendDlgItemMessage(hStatus,nDlgItem,(nDlgItem&IsCombo)?CB_SETCURSEL:LB_SETCURSEL,r, 0L);
	return r;
}

///////////////////////////////////////////////////////////////////////
//	add a new entry to the Error Log in Status Dialog if logging all

void MakeLogAll(char *what)
{
	if(logall) MakeLog(what);
}

///////////////////////////////////////////////////////////////////////
//	add a new entry to the Error Log in Status Dialog

void MakeLog(char *what)
{	char 	log[128]; 
	time_t	ltime;
	
	while (GetListCount(hStatus,IDC_LOG)>=maxlog)
		RemoveItem(hStatus,IDC_LOG,0);
		
	time(&ltime);
	sprintf(log,"%.24s %s",ctime(&ltime),what);
   	EnterItem(IDC_LOG,log);
//	if (emode==logSafe) LogToFile(log);
}

/////////////////////////////////////////////////////////////////////// 
//	append a log line to the log file
//in:	ln	is the text line to log

void LogToFile (char *ln)
{	FILE *f;

	if (*logfile==0) return;				//no log file  
	if ((f=fopen(logfile,"a"))==NULL)
	{	MakeLog(logfile);
  		MakeLog(strerror(errno));
  	}
	else
	{	fprintf(f,"%s\n",ln);
		fclose(f);
	}
}

///////////////////////////////////////////////////////////////////////
//	Make sure a path ends in a backslash

void AddSlash(char *r)
{	int	i;

	if (*r!=0)									//if string isn't null
	{	i=strlen(r);
		if (r[i-1]!='\\') strcat(r,"\\");
	}
}

///////////////////////////////////////////////////////////////////////
//	Parse hex number										***003 Begin
//
//in:	p		points to buffer
//		cnt		count of hex chars to parse (1..4)
//out:	hex word scanned

word	HexParse(char *p,int cnt)
{	word	v=0;
	int		i;
	char	c;

	for (i=0;i<cnt;i++)
	{	c=*p++;					//get next char
		if (c>='0'&&c<='9')
			v=(v<<4)+(c-'0');
		else if (c>='a'&&c<='f')
			v=(v<<4)+(c-'a')+10;
		else if (c>='A'&&c<='F')
			v=(v<<4)+(c-'A')+10;
		else
			break;
	}
	return v;
}								//							***003 End

///////////////////////////////////////////////////////////////////////
//	Transmit a frame
//in:	ft		frame type
//		dlen	length of data dp points to (0..501)
//		dp		points to (unescaped) data to send (or NULL if dlen==0)
//out:	ptpERSuccess
//		ptpERxxx

int SendFrame(int ft,int dlen,octet *dp)
{	word	actualLen;
	int		i,r;
	octet	*p,tcrc=0xFF;
	union {	word dw; octet db[2]; }u;
	octet	TxBuf[1014];						//max size we need if all are escaped

	p=&TxBuf[0];								//point to transmit buffer
	*p++=0x55;
	*p++=0xFF;
	p=EscapeOctets(p,(octet)ft);
	tcrc=CalcHeaderCRC((octet)ft,tcrc);
	u.dw=dlen;
	//the encode of "dlen" may contain X'10" or X'11" or X'13"
	p=EscapeOctets(p,u.db[1]); 
	tcrc=CalcHeaderCRC(u.db[1],tcrc);
	p=EscapeOctets(p,u.db[0]);
	tcrc=CalcHeaderCRC(u.db[0],tcrc);
	p=EscapeOctets(p,(octet)~tcrc);
	if (dlen!=0)
	{	
		//now u.dw is used for data CRC
		u.dw=0xFFFF;							//data CRC is a word
		for (i=0;i<dlen;i++)
		{	u.dw=CalcDataCRC(*dp,u.dw);
			p=EscapeOctets(p,*dp++);
		}
		p=EscapeOctets(p,(octet)~u.db[0]);
		p=EscapeOctets(p,(octet)~u.db[1]);
	}
//	*p++=0xFF;									//dummy at the end (only for MS/TP)
	actualLen=p-&TxBuf[0];

	if (r=Send((byte *)TxBuf,actualLen))
			return r+ptpERSjack;
	return ptpERSuccess;
}

///////////////////////////////////////////////////////////////////////
//	calculate Header CRC (from BACnet Appendix G)
//in:	dv	data value to accumulate
//		cv	current crc value
//out:	new crc

octet CalcHeaderCRC(octet dv,octet cv)
{	word	crc;

	crc=cv^dv;
	crc=crc^(crc<<1)^(crc<<2)^(crc<<3)^(crc<<4)^(crc<<5)^(crc<<6)^(crc<<7);
	return (crc&0xFE)^((crc>>8)&1);
}

///////////////////////////////////////////////////////////////////////
//	calculate Data CRC (from BACnet Appendix G)
//in:	dv	data value to accumulate
//		cv	current crc value
//out:	new crc

word CalcDataCRC(octet dv,word cv)
{	word	crcLow;
	crcLow=(cv&0xFF)^dv;
	return (cv>>8)^(crcLow<<8)^(crcLow<<3)^(crcLow<<12)^(crcLow>>4)^(crcLow&0x0F)^((crcLow&0x0F)<<7);
}

///////////////////////////////////////////////////////////////////////
//	encode and escape an octet
//in:	p		points to buffer for octets to go in
//		d		(unescaped) data to send
//out:	p advanced by 1 or 2 as required

octet *EscapeOctets(octet *p,octet d)
{
	//X'10' => X'10'X'90
	//X'11' => X'10'X'91
	//X'13' => X'10'X'93
	if (d==0x10||d==0x11||d==0x13)				//these need escaping
	{	*p++=0x10;
		*p++=d|0x80;
	}
	else
		*p++=d;									//else pass it on as-is
	return p;
}

///////////////////////////////////////////////////////////////////////
//	Add a packet to a pktq
//
//in:	q		points to the pktq to insert into
//		p		points to the packet to be inserted

void PutQ(pktq *q, mpkt *p)
{	mpkt	*lg=q->rear;						//lg is the last guy in present queue

	if(lg==NULL)								//p is the first and only guy
		q->front=p;								//so front also points to him
	else										//in all other cases, rear guy must point to new guy
		lg->next=p;
	q->rear=p;									//new guy is now the last one
	p->next=NULL;
	q->count++;
}

///////////////////////////////////////////////////////////////////////
//	remove a packet from a pktq
//
//in:	q		points to the pktq to remove from
//out:	NULL	it's empty
//		else	pointer to an mpkt

mpkt *GetQ(pktq *q)
{	mpkt	*fg=q->front;						//fg is the first guy in present queue

	if(fg==NULL) return NULL;					//empty
	if(--q->count==0) q->rear=NULL;
	q->front=fg->next;
	return fg;									//return the first one
}
	
///////////////////////////////////////////////////////////////////////
//	Read Preferences from PTP.INI
//
//[General]
//	Port=COMx:baud,parity,data,stop
//  Logfile=path\foo.log
//	Nretries=3
//	Tconnrqst=15
//	Tconnrsp=15
//	Tframeabort=2
//	Theartbeat=15
//	Tinactivity=60
//	Tresponse=5
//
//out:	<0		error

int ReadPreferences(const char *config)
{	
	//char	*p;
	int		r;
	//LPVOID	lpMsgBuf; //	***002
	//dword	result;
	dword   e;			  //	***002

	if ((r=SJACKinit(1))<0) 
		return r+ptpERSjack;	//we only support one logical port
	
	if ((r=SJACKbind(0, config))<0)	//bind to that port, '0' is the logical port number, the physical port is pointed by parameter 'b'
	{
		e=GetLastError();						//							***002 Begin
		//result=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
		//			FORMAT_MESSAGE_IGNORE_INSERTS,NULL,
		//			e,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), // Default language
		//			(LPTSTR) &lpMsgBuf,0,NULL);
		//		sprintf(config,"Error: [%d] ",e);
		//		if (lpMsgBuf!=NULL)
		//		{	strcat(config,lpMsgBuf);
		//			LocalFree(lpMsgBuf);
		//		}
		//		MessageBox(hStatus,config,szAppName,MB_ICONSTOP);	
					
		return r+ptpERSjack;		
	}
	
	//	p=strchr(config,':');
	//	*p=0;
	//	sprintf(ptpname,"%.3u PTP on %s",ptpport,config);
	

	//	GetPrivateProfileString("General","LogFile","",logfile,sizeof(logfile),PTPPrefs);
	//	GetPrivateProfileString("General","ConnectPassword","",ConnectPassword,sizeof(ConnectPassword),PTPPrefs);
	//	Nretries=GetPrivateProfileInt("General","Nretries",3,PTPPrefs);
	//	Tconnrqst=GetPrivateProfileInt("General","Tconnrqst",15,PTPPrefs);
	//	Tconnrsp=GetPrivateProfileInt("General","Tconnrsp",15,PTPPrefs);
	//	Tframeabort=GetPrivateProfileInt("General","Tframeabort",2,PTPPrefs);
	//	Theartbeat=GetPrivateProfileInt("General","Theartbeat",15,PTPPrefs);
	//	Tinactivity=GetPrivateProfileInt("General","Tinactivity",60,PTPPrefs);
	//	Tresponse=GetPrivateProfileInt("General","Tresponse",5,PTPPrefs);
	

	return ptpERSuccess;							//OK
}

///////////////////////////////////////////////////////////////////////
// Transmit a buffer with tracing
//
//in:	p			the buffer to send
//		n			the length of p to send

int Send(byte *p, dword n)
{	byte	*q=p;
	dword	i;

	NewTrace(IDC_TX);
	for (i=0;i<n;i++)
		Trace(IDC_TX,(octet)*q++);
	return SJACKtx(0,p,n);
}

///////////////////////////////////////////////////////////////////////
//	add a new entry to a trace Log in Status Dialog

void NewTrace(int which)
{
	while (GetListCount(hStatus,which)>=maxtrace)
		RemoveItem(hStatus,which,0);
		
// 	AddItem(which,"");							//used "add" not "enter" so list doesn't jump!
   	EnterItem(which,"");
	if(which==IDC_RX)
	{	rxbol=0;
		rxindex=GetListCount(hStatus,IDC_RX)-1;
		rxtrace[0]=0;
	}
	else
	{	txbol=0;
		txindex=GetListCount(hStatus,IDC_TX)-1;
		txtrace[0]=0;
	}
}

///////////////////////////////////////////////////////////////////////
// Trace a transmitted or received octet
//
//in:	w			the window to add trace to
//		c			the new octet to trace

void Trace(int w,octet c)
{	if(w==IDC_RX)
	{	if(rxbol>=(tracewidth/2)) NewTrace(IDC_RX);
		sprintf(&rxtrace[(rxbol++)*2],"%.2X",c);
		ReplaceItem(IDC_RX,rxindex,rxtrace);
	}
	else
	{	if(txbol>=(tracewidth/2)) NewTrace(IDC_TX);
		sprintf(&txtrace[(txbol++)*2],"%.2X",c);
		ReplaceItem(IDC_TX,txindex,txtrace);
	}
}


///////////////////////////////////////////////////////////////////////
//
//pkt:
//buf: PTP frame buffer
//buflen: PTP frame buffer length
//plen: pointer to length of PTP frame
AFX_EXT_API int ptpGetFrame(PKTPTP pkt, byte *framebuf, int buflen, int *plen)
{
	unsigned char *p;
	unsigned char tcrc=0xFF;
	union {	word dw; octet db[2]; }u;
	unsigned char TxBuf[1014];	//max size we need if all are escaped		

	p = &TxBuf[0]; //point to transmit buffer
	*p++ = 0x55;
	*p++ = 0xFF;
	tcrc = CalcHeaderCRC((octet)pkt.pktFrameType, tcrc);
	p = EscapeOctets(p, pkt.pktFrameType);	
	u.dw = pkt.pktLen;
	//the encode of "dlen" may contain X'10" or X'11" or X'13"
	p = EscapeOctets(p, u.db[1]); 
	tcrc = CalcHeaderCRC(u.db[1], tcrc);
	p = EscapeOctets(p, u.db[0]); 
	tcrc = CalcHeaderCRC(u.db[0], tcrc);
	p = EscapeOctets(p, (octet)~tcrc);

	if (pkt.pktLen > 0)
	{	
		//now u.dw is used for data CRC
		u.dw = 0xFFFF;							//data CRC is a word
		for(int i = 0; i < pkt.pktLen; i++)
		{	
			u.dw = CalcDataCRC(pkt.pktData[i], u.dw);
			p = EscapeOctets(p, pkt.pktData[i]);
		}
		p=EscapeOctets(p, (octet)~u.db[0]);
		p=EscapeOctets(p, (octet)~u.db[1]);
	}

	*plen = p - &TxBuf[0]; //frame length 

	//buffer not enough
	if(buflen < *plen)
		return -1;

	CopyMemory(framebuf, TxBuf, *plen);

	return 0;
}

AFX_EXT_API int  ptpNPDUtoFrame(byte *npdu, int npdulen, byte *framebuf, int buflen, int *plen)
{
	unsigned char *p;
	unsigned char tcrc=0xFF;
	union {	word dw; octet db[2]; }u;
	unsigned char TxBuf[1014];	//max size we need if all are escaped	

	//no data
	if(npdulen < 0 || npdu == NULL)
		return -1;

	p = &TxBuf[0]; //point to transmit buffer
	*p++ = 0x55;
	*p++ = 0xFF;
	*p++ = ftData0;

	tcrc = CalcHeaderCRC((octet)ftData0, tcrc);
	u.dw = npdulen;
	//the encode of "dlen" may contain X'10" or X'11" or X'13"
	p = EscapeOctets(p, u.db[1]); 
	tcrc = CalcHeaderCRC(u.db[1], tcrc);
	p = EscapeOctets(p, u.db[0]); 
	tcrc = CalcHeaderCRC(u.db[0], tcrc);
	p = EscapeOctets(p, (octet)~tcrc);

	if (npdulen > 0)
	{	
		//now u.dw is used for data CRC
		u.dw = 0xFFFF;							//data CRC is a word
		for( int i = 0; i < npdulen; i++)
		{	
			u.dw = CalcDataCRC(*npdu, u.dw);
			p = EscapeOctets(p, *npdu++);
		}
		p=EscapeOctets(p, (octet)~u.db[0]);
		p=EscapeOctets(p, (octet)~u.db[1]);
	}

	*plen = p - &TxBuf[0]; //frame length 

	//buffer not enough
	if(buflen < *plen)
		return -1;

	CopyMemory(framebuf, TxBuf, *plen);

	return 0;
}


