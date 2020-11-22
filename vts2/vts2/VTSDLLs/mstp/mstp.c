/*--------------------------------------------------------------------
	©1998-99, PolarSoft® Inc., All Rights Reserved
	
module:		MSTP.C
desc:		BACnet MS/TP Driver DLL for Win32
author:		David M. Fisher, Jack Neyer
last edit:	12-Nov-99 [001] JN  First Cut - includes Bill Swan's changes to state machine ***BSW
----------------------------------------------------------------------*/
// This Dll uses a Multimedia High Res Timer for Driving the State Machines. It's setup
// for a hard INT every 5 msec. Because it runs at INT level and the Windows functions
// that are available to it are limited, it does a PostMessage for driving the State Machines.
//
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "sjack32.h"
#include "resource.h"
#include "macp32.h"
#include "mstp.h"
///////////////////////////////////////////////////////////////////////
const char	szAppName[]		=	"MSTP";
const char	mstpversion[]	=	"PolarSoft® BACnet MS/TP MAC Layer v0.01";
#define		ver100				1				//version*100
const char	MSTPPrefs[]		=	"MSTP.INI";		//preferences file
///////////////////////////////////////////////////////////////////////
//	Module Constants
#define		maxlog				100				//max entries in log list
#define		maxtrace			100				//max entries in rx,tx trace lists
#define		tracewidth			64				//width of trace line in chars
#define		maxrx				512				//max chars in rx buffer
#define		TimerInterval		5				//5 ms
#define		WM_USER_INITIATE	(WM_USER+1)
#define		WM_USER_SYSTRAY		(WM_USER+2)
#define		WM_USER_TIMER		(WM_USER+3)
#define		MENU_VIEW			0				//positional indices for menus
//#define		MENU_TEST			1
#define		MENU_ABOUT			1

///////////////////////////////////////////////////////////////////////
//	global variables

#pragma data_seg("BDSHARE")	
// Note:	ALL of the shared variables MUST be initialized explicitly when declared
//			or the pragma won't work!
HWND		hStatus=0;							//handle of our Status Dialog
HWND		hAbout=0;							//handle of our About Box window
HWND 		hLog=0;								//handle of log listbox 
HANDLE		hDLLInst=0;
HBITMAP		hRed=0,hGreen=0, hToken=0, hNotoken=0;

int			Running=1;							//downcounter for about box
int			inTimer=0;
word		timerID=0;
UINT		resolution=5;						//the timer resolution in millisecs
UINT		period=5;							//how often callback gets called
dword		starttime=0,thistime=0,elapsedtime=0,eventtime=0,lastelapsedtime=0;
BOOL		timerstarted=false;

enum		{lrsmIdle,lrsmPreamble,lrsmHeader,lrsmData}
			lrstate=lrsmIdle;					//low level receive machine state
enum		{mnsmInitialize,mnsmIdle,mnsmUseToken,mnsmWaitForReply,mnsmDoneWithToken,
				mnsmPassToken,mnsmNoToken,mnsmPollForMaster,mnsmAnswerDataRequest}
			mnstate=mnsmInitialize;					//master node state machine
BOOL		initialized=false;
octet		hb[6];								//header buffer
												// [0]=type
												// [1]=D.A.
												// [2]=S.A.
												// [3]=lenhi
												// [4]=lenlo
												// [5]=crc
int			hindex=0;							//header index
octet		hcrc=0;
word		dcrc=0;
dword		rxid=0;
BOOL		havetoken=false;
BOOL		lasttoken=false;

// MSTP Variables
int			eventcount=0;
int			framecount=0;						//frames sent during token hold
octet		ns=0;								//next station to get token
octet		ps=0;								//last station to which we send Poll for Master
octet		ts=0;								//this station
BOOL		receivedinvalidframe=false;
BOOL		receivedvalidframe=false;
dword		validframecount=0;
dword		invalidframecount=0;
dword		unwantedframecount=0;
int			ReplyTimer=0;
int			retrycount=0;
int			SilenceTimer=0;
BOOL		solemaster=false;
int			tokencount=0;						//tokens received by us
// MSTP Parameters
int			Nmaxinfoframes=1;					//number of frames to send before passing token
												//Max_Info_Frames Property of the DeviceObject
octet		Nmaxmaster=127;						//Max_Master Property of the DeviceObject
int			Npoll=50;							//number of tokens recieved or used before Poll for Master (fixed)
int			Nretrytoken=1;						//number of rtetries on sending the token (fixed)
int			Nminoctets=4;						//number of "events (octets) for active line (fixed)
int			Tframeabort=100;					//(60 bit times) 100 ms max (fixed)
int			Tframegap=20;						//(20 bit times) (fixed) 
int			Tnotoken=500;						//silence time for loss of token (fixed)
int			Tpostdrive=15;						//(15 bit times) (fixed)
int			Treplydelay=250;					//250 ms (fixed)
int			Treplytimeout=300;					//255-300 ms (fixed)
int			Tslot=10;							//10 ms (fixed)
int			Tturnaround=5;						//40 bit times (for 9600 baud 4.1 millisecs) (fixed)
int			Tusagetimeout=100;					//20-100 ms (fixed)

int			rlen=0;
int			elen=0;
BOOL		logall=true;
octet		rbuf[maxrx];
pq			incoming={NULL,NULL,0};				//received packets
pq			outgoingrequest={NULL,NULL,0};		//transmit requested packets
pq			outgoingreply={NULL,NULL,0};		//transmit reply packets

char		logfile[144]={0};					//path to logfile
char		mstpname[]="MSTP Station xxx on COMxx";


int			rxbol=0, txbol=0;
int			rxindex=0, txindex=0;
char		rxtrace[tracewidth+1]={0};
char		txtrace[tracewidth+1]={0};

#pragma data_seg()
///////////////////////////////////////////////////////////////////////
//	function prototypes

LRESULT CALLBACK MainWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK About(HWND,UINT,WPARAM,LPARAM); 
void	CALLBACK TimerCallBack(WORD, WORD, DWORD, DWORD, DWORD);
int		AddItem(int,const char *);
void	AddSlash(char *);
word	BuildMSTP (octet *,int,octet,int,octet *);
void	ClearItems(int);
word	CalcDataCRC(octet,word);
octet	CalcHeaderCRC(octet,octet);
void	cvhexa(char *,byte);
char	*cvhex(char *,byte *);
void	cvhexn(char *,byte *,int);
int		DoTransmit(MApacket *,BOOL);
void	DrawBitmap(HWND,HBITMAP,int,int);
int		EnterItem(int,const char *);
void	EverySecond(void);
void	ExitMSTP(void);
void	FormatTime (int, char *,BOOL);
int		GetListCount(HWND,int);
int		GetListText(int,int,char *);
mmpkt	*GetQ(pq *);
void	LogToFile(char *);
void	MainLoop(void); 
void	MakeLog(char *);
void	MakeLogAll(char *);
void	MasterNodeStateMachine(void);
void	myYield(void);
void	NewTrace(int);
BOOL	ParseTest (char *, MApacket *);
void	PutQ(pq *,mmpkt *);
int		ReadPreferences(void);
void	ReceiveFrameStateMachine(void);
void	ReceiveIndicate(octet,octet,octet,int,octet *);
void	RemoveItem(HWND,int,int);
int		ReplaceItem(int,int,char *);
void	SelectItem(int,int);
int		Send(byte *,dword);
int		SendFrame(int,octet,int,octet *);
void 	SetItemVisible(HWND,int,BOOL);
void	ShowAboutWindow(int);
void	ShowToken();
void	ShowIdle(BOOL);
void	ShowTestResponse (mmpkt *);
void	SimulateTestResponse (int,octet *);
int		StartTimer(void);
void	StopTimer(void); 
void	Startup(void);
void	TimerEvent(void);
void	Trace(int,octet,char *);
int		YesNo(char *);

///////////////////////////////////////////////////////////////////////
//	DLL entrypoint called from Windows

BOOL APIENTRY DllMain(HANDLE hInst, DWORD ul_reason_being_called, LPVOID lpReserved)
{
    hDLLInst = hInst;
    return 1;
	UNREFERENCED_PARAMETER(ul_reason_being_called);
	UNREFERENCED_PARAMETER(lpReserved);
}

//-------------------------------- Begin API Functions --------------------------------

///////////////////////////////////////////////////////////////////////
//	initialize serial port
//
//out:	mstpERSuccess	ok
//		else			mstpERxxx error

int APIENTRY mstpInit()
{	int			r;
	WNDCLASS	wc;
	HFONT		hFont=NULL;
   
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = DLGWINDOWEXTRA;
    wc.hInstance = hDLLInst;
    wc.hIcon = LoadIcon(hDLLInst, "MSTPICON");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
    wc.lpszMenuName =  "MainMenu";
    wc.lpszClassName = "MSTPCL";
    if (!RegisterClass(&wc))
    	return mstpERFailedClassRegistration;		//failed
    wc.lpfnWndProc = About;
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "MSTPABOUTCL";
    if (!RegisterClass(&wc))
    	return mstpERFailedClassRegistration;		//failed
//	create the windows we need and allocate a timer
    hStatus = CreateDialog(hDLLInst,"MSTPSTATUS",0,NULL);
    if (!hStatus) return mstpERCantCreateWindow;   
	hAbout = CreateDialog(hDLLInst,"MSTPABOUT",0,NULL);
    if (!hAbout)
	{	DestroyWindow(hStatus);							//we don't need the window now
      	return mstpERCantCreateWindow;
	}
    hLog=GetDlgItem(hStatus,IDC_LOG);
    
	if (StartTimer()!=0)
	{	DestroyWindow(hStatus);						//we don't need the window now
      	return mstpERCantCreateTimer;
	}
   
	if((r=ReadPreferences())<0) return r;
	initialized=true;								//is this the proper place for this???
	hRed=LoadBitmap(hDLLInst,"REDLED");
	hGreen=LoadBitmap(hDLLInst,"GREENLED");
	hToken=LoadBitmap(hDLLInst,"TOKEN");
	hNotoken=LoadBitmap(hDLLInst,"NOTOKEN");
     
    hFont=CreateFont(15,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
   					  CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_MODERN|FIXED_PITCH,
   					  "Courier New");
   	SendDlgItemMessage(hStatus,IDC_RX,WM_SETFONT,(WPARAM)hFont,0);
   	SendDlgItemMessage(hStatus,IDC_TX,WM_SETFONT,(WPARAM)hFont,0);
	SetWindowText(GetDlgItem(hAbout,IDC_RUNTITLE),mstpversion); //show our version string
	SetWindowText(hStatus,mstpversion); 				//also in main window
	ShowWindow(hStatus,SW_HIDE);						//make main window invisible
	CheckMenuItem(GetSubMenu(GetMenu(hStatus),MENU_VIEW),
				  ID_TRACEALL,
				  MF_BYCOMMAND | (logall ? MF_CHECKED : MF_UNCHECKED));
	ShowToken();								//update token status initially
	ShowAboutWindow(1);								//display about box for 2 second 
    SendMessage (hStatus,WM_USER_INITIATE,0,0L);	//Get things going
	return mstpERSuccess;							//success
}

///////////////////////////////////////////////////////////////////////
//	deinitialize serial port

void APIENTRY mstpDeinit()
{
	ExitMSTP();										//shut down everything
}

///////////////////////////////////////////////////////////////////////
//	get our port name
//
//in:	p				points to string which is appended with port name

void APIENTRY mstpCatName(char *p)
{
	strcat(p,mstpname);								//append our name to it
}

///////////////////////////////////////////////////////////////////////
//	try to transmit data (like a DL-UNITDATA.request)
//
//in:	mp				points to MApacket to transmit
//out:	mstpERSuccess	ok
//		else			mstpERxxx error

int APIENTRY mstpTransmit(MApacket *mp)
{
	return DoTransmit (mp,true);
}

///////////////////////////////////////////////////////////////////////
//	try to transmit a test (like a DL-UNITDATA.request)
//
//in:	mp				points to MApacket to transmit
//out:	mstpERSuccess	ok
//		else			mstpERxxx error

int APIENTRY mstpTransmitTest(MApacket *mp)
{
	return DoTransmit (mp,false);
}

///////////////////////////////////////////////////////////////////////
//	Transmit data or a test message
//
//in:	mp				points to MApacket to transmit
//		data			TRUE if data, else test
//out:	mstpERSuccess	ok
//		else			mstpERxxx error

int DoTransmit(MApacket *mp,BOOL data)
{	mmpkt	*p;

	if((p=(mmpkt *)malloc(sizeof(mmpkt)))==NULL)
	{	MakeLogAll("Transmit failed: no free buffers");
		return mstpERNoFreeBuffers;
	}
	if(mp->maNlen>sizeof(p->buf))
	{	free(p);
		MakeLogAll("Transmit failed: request too large");
		return mstpERTransmitRequestTooLarge;
	}
	if (mp->maNlen) CopyMemory(p->buf,mp->maNPDU,mp->maNlen);
	p->length=mp->maNlen;
	if (mp->maBroadcast==1)
		p->destaddr=0xFF;
	else
		p->destaddr=mp->maDadr[0];
	if (data)
	{	if (mp->maNPDU[1]&NPDUexpectsreply)				//must be a confirmed request
		{	p->frametype=mftBACnetDataExpectingReply;
			PutQ(&outgoingrequest,p);
		}
		else if (p->destaddr!=0xFF)						//must be a cack, sack, error, etc.
		{	p->frametype=mftBACnetDataNotExpectingReply;
			PutQ(&outgoingreply,p);
		}
		else											//must be an unconfirmed request
		{	p->frametype=mftBACnetDataNotExpectingReply;
			PutQ(&outgoingrequest,p);
		}
	}
	else
	{	p->frametype=mftTestRequest;
		PutQ(&outgoingrequest,p);
	}
	return mstpERSuccess;
}

///////////////////////////////////////////////////////////////////////
//	try to see if there is a received packet available
//
//in:	mp				points to MApacket to fill-in
//out:	true			packet available, mp filled-in

BOOL APIENTRY mstpReceive(MApacket *mp)
{	mmpkt	*p;

	if((p=GetQ(&incoming))==NULL)
		return false;
	mp->maLink=NULL;
	mp->maTrace=rxid++;
	mp->maLport=mstpport;
	mp->maAlen=0;
	mp->maLsap=0x82;								//BACnet LSAP
	mp->maBroadcast=(p->destaddr==0xFF)?1:0;
	mp->maDadr[0]=p->destaddr;
	mp->maSadr[0]=p->sourceaddr;
	mp->maAlen=1;
	mp->maNlen=p->length;
	CopyMemory(mp->maNPDU,p->buf,p->length);
	free(p);
	return true;
}

///////////////////////////////////////////////////////////////////////
//	Give upper level access to Nmaxinfoframes 
//
//in:	val				points to place to put/get variable
//		write			TRUE if it's a write

void APIENTRY mstpVarMaxInfoFrames(int *val,BOOL write)
{	
	if (write)
		Nmaxinfoframes=*val;
	else
		*val=Nmaxinfoframes;
}

///////////////////////////////////////////////////////////////////////
//	Give upper level access to Nmaxmaster
//
//in:	val				points to place to put/get variable
//		write			TRUE if it's a write

void APIENTRY mstpVarMaxMaster(int *val,BOOL write)
{	
	if (write)
	{	if ((*val>0)&&(*val<127)) 
			Nmaxmaster=(octet)(*val);
	}
	else
		*val=(int)Nmaxmaster;
}

///////////////////////////////////////////////////////////////////////
//	Return our MSTP Station address
//
//out:	our mstp station address (ts)

byte APIENTRY mstpGetTS()
{	
	return ts;
}

///////////////////////////////////////////////////////////////////////
//	Get the Multimedia timer going
//
//
// out:	0		success
//		else	error

int StartTimer()
{
	starttime=timeGetTime();							//get some point of reference
	timeBeginPeriod(resolution);						//set the minimum resolution
// Set up the callback event. The callback function  MUST be in a FIXED CODE DLL!!!
	timerID=timeSetEvent(period,resolution,(LPTIMECALLBACK)TimerCallBack,(LONG)hStatus,TIME_PERIODIC);                                
	if (timerID)
	{	timerstarted=true;
		return 0;
	}
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////
//	Shutdown the Multimedia timer
//
//
// out:	0		success
//		else	error

void StopTimer()
{
	if (timerstarted)
	{	timeKillEvent(timerID);							//destroy the timer callback event
		timeEndPeriod(resolution);						//clear the minimum we set
	}
}

//-------------------------------- CALLBACK --------------------------------
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//	The timer Call Back function
//
// Note: The system functions that can be called by this guy are limited!!!
//
// in:	wID		the timer event ID
//		wMsg	reserved
//		dwUser	the user supplied dat from setTimeEvent (in our case it's hStatus)
//		dwl		reserved
//		dw2		reserved

void CALLBACK TimerCallBack(WORD wID, WORD wMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	thistime=timeGetTime();						//get current time
	eventtime+=(thistime-starttime)-elapsedtime;
	elapsedtime=thistime-starttime;
	SilenceTimer+=elapsedtime-lastelapsedtime;
	ReplyTimer+=elapsedtime-lastelapsedtime;
	lastelapsedtime=elapsedtime;
    if (eventtime>=TimerInterval)
	{	PostMessage (hStatus,WM_USER_TIMER,0,0L);	//Signal the main loop
		eventtime=0;
	}
}

///////////////////////////////////////////////////////////////////////
//	Timer Event Handler (approximately every 5ms)

void TimerEvent()
{	static int	InMainLoop=false;					//lock flag
	static int	SecondsCounter=1000/TimerInterval;
	
	if ((SecondsCounter--)==0)						//a second has gone by
	{	SecondsCounter=1000/TimerInterval;			//reset downcounter
		EverySecond();								//do once per second stuff here
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
	if (Running)										//showing about box only
	{	if (--Running==0)								//end of about box display
			ShowWindow(hAbout,SW_HIDE);
	}
}

///////////////////////////////////////////////////////////////////////
//	Main Loop (approximately every 20ms)

void MainLoop()
{	static dword	lasttime=0;
	static word		lastmnstate=0;
	dword			dtime;
	char			b[128];
	
	ReceiveFrameStateMachine();
	MasterNodeStateMachine();
	if (logall)
	{	if (lasttime==0)
			lasttime=starttime;
		dtime=elapsedtime-lasttime;
		lasttime=elapsedtime;
		FormatTime(SilenceTimer,b,false);
		SetDlgItemText(hStatus,IDC_SILENCE,b);
		sprintf (b,"%.2lu msec",dtime);
		SetDlgItemText(hStatus,IDC_DELTA,b);
		if (lastmnstate!=mnstate)
		{	switch (mnstate)
			{
			case mnsmInitialize:
				SetDlgItemText(hStatus,IDC_MNSTATE,"Init");
				break;
			case mnsmIdle:
				SetDlgItemText(hStatus,IDC_MNSTATE,"Idle");
				break;
			case mnsmUseToken:
				SetDlgItemText(hStatus,IDC_MNSTATE,"UseToken");
				break;
			case mnsmWaitForReply:
				SetDlgItemText(hStatus,IDC_MNSTATE,"WaitForReply");
				break;
			case mnsmDoneWithToken:
				SetDlgItemText(hStatus,IDC_MNSTATE,"DoneWithToken");
				break;
			case mnsmPassToken:
				SetDlgItemText(hStatus,IDC_MNSTATE,"PassToken");
				break;
			case mnsmNoToken:
				SetDlgItemText(hStatus,IDC_MNSTATE,"NoToken");
				break;
			case mnsmPollForMaster:
				SetDlgItemText(hStatus,IDC_MNSTATE,"PollForMaster");
				break;
			case mnsmAnswerDataRequest:
				SetDlgItemText(hStatus,IDC_MNSTATE,"AnswerDataReq");
				break;
			default:
				SetDlgItemText(hStatus,IDC_MNSTATE,"????????");
			}
			lastmnstate=mnstate;
		}

	}
}

///////////////////////////////////////////////////////////////////////
//	Receive Frame State Machine
//
// Note: When a conplete frame has been received, this machine leaves the
//       recieved frame in hb and rbuf with receivedvalidframe==true OR 
//       receivedinvalidframe==true. It's then up to MasterNodeStateMachine
//		 to process the frame.

void ReceiveFrameStateMachine()
{	int		c;
	
	while(true)
	{	if((SilenceTimer>Tframeabort)&&(lrstate!=lrsmIdle))			//timed out waiting
			goto ResetToIDLEx;
		if ((c=SJACKrx(0))<0) break;			//no characters available, quit now
		ShowIdle(false);
		if (c>255)								//some error occurred
		{	eventcount++;
			c&=0xFF00;
			switch(c)
			{
			case 0x100:
				Trace(IDC_RX,(octet)c,"<FRM>");
				break;
			case 0x200:
				Trace(IDC_RX,(octet)c,"<PAR>");
				break;
			case 0x300:
				Trace(IDC_RX,(octet)c,"<OVR>");
				break;
			default:
				Trace(IDC_RX,(octet)c,"<??>");
				break;
			}
ResetToIDLEx:
			if ((lrstate==lrsmHeader)||(lrstate==lrsmData))
				receivedinvalidframe=true;
ResetToIDLE:
			lrstate=lrsmIdle;					//reset our state to IDLE
			hindex=0;
			SilenceTimer=0;
			break;
		}
		switch(lrstate)							//handle it based on present state
		{
//[IDLE]---------------------------------------------------------------------------
		case lrsmIdle:
			SilenceTimer=0;
			eventcount++;
			if (c==0x55)						//preamble start
			{	lrstate=lrsmPreamble;			//PREAMBLE state
				NewTrace(IDC_RX);
				Trace(IDC_RX,(octet)c,NULL);
			}
			hindex=0;							//not a preamble start
			break;
//[PREAMBLE]-----------------------------------------------------------------------
		case lrsmPreamble:
			SilenceTimer=0;
			eventcount++;
			Trace(IDC_RX,(octet)c,NULL);
			if(c==0x55)
				break;							//preamble was repeated
			if (c!=0xFF)						//not the second preamble octet
				goto ResetToIDLE;
			hcrc=0xFF;
			hindex=0;
			lrstate=lrsmHeader;					//HEADER state
			break;
//[HEADER]-----------------------------------------------------------------------
		case lrsmHeader:
			SilenceTimer=0;
			eventcount++;
			Trace(IDC_RX,(octet)c,NULL);
			hb[hindex]=(octet)(c&0xFF);
			hcrc=CalcHeaderCRC(hb[hindex++],hcrc);
			if(hindex==6)						//got a header
			{	
//[HEADER_CRC]-----------------------------------------------------------------------
				if(hcrc!=0x55)
				{	Trace(IDC_RX,(octet)c,"<-CRCErr");
					MakeLog("BAD header CRC on received frame");
					goto ResetToIDLE;			//bad header CRC
				}
				rlen=(hb[3]<<8)+hb[4];			//get data length
				if ((hb[1]!=0xFF)&&(hb[1]!=ts))	//Not for us
					goto ResetToIDLE;
				if(rlen>sizeof(rbuf))
				{	MakeLog("Received a too large frame");
					goto ResetToIDLE;
				}
				if(rlen==0)
				{	receivedvalidframe=true;
					goto ResetToIDLE;
				}
				hindex=0;
				dcrc=0xFFFF;
				elen=rlen+2;					//data+2 CRC octets
				lrstate=lrsmData;				//DATA state
			}
			break;
//[DATA]-----------------------------------------------------------------------
		case lrsmData:
			SilenceTimer=0;
			eventcount++;
			Trace(IDC_RX,(octet)c,NULL);
			rbuf[hindex]=(octet)(c&0xFF);
			dcrc=CalcDataCRC(rbuf[hindex++],dcrc);
			if(hindex==elen)					//got a complete frame
			{	
//[DATA_CRC]-----------------------------------------------------------------------
				if(dcrc==0xF0B8)
				{	receivedvalidframe=true;
					goto ResetToIDLE;
				}
				else
				{	Trace(IDC_RX,(octet)c,"<-CRCErr");
					MakeLog("BAD CRC on received frame");
					goto ResetToIDLEx;
				}
			}
			break;
//---------------------------------------------------------------------------------
		}
	}
	ShowIdle(true);
}

///////////////////////////////////////////////////////////////////////
//	Master Node State Machine

void MasterNodeStateMachine()
{	mmpkt	*p;
	octet	tsplus1,psplus1,nsplus1;

	switch (mnstate)							//handle it based on present state
	{
//[INITIALIZE]----------------------------------------------------------------------
	case mnsmInitialize:						//master node initialize
		if (initialized==false) break;
		ns=ts;									//next station is unknown
		ps=ts;
		tokencount=Npoll;						//so we Poll for Master when we receive token
		solemaster=false;
		mnstate=mnsmIdle;
		//intentionally fall into next state

//[IDLE]----------------------------------------------------------------------
	case mnsmIdle:								//wait for a frame								
idle:	havetoken=false;
/*		if (SilenceTimer>Tnotoken)	//***Moved to the bottom!
		{	MakeLogAll("Lost Token");			//LostToken
			mnstate=mnsmNoToken;
			goto notoken;
		}
		else if (receivedinvalidframe)			//ReceivedInvalidFrame
*/
		if (receivedinvalidframe)			//ReceivedInvalidFrame
		{	invalidframecount++;
			receivedinvalidframe=false;
			mnstate=mnsmIdle;
		}
		else if (receivedvalidframe)			//we saw a valid frame
		{	validframecount++;
			if (hb[1]==0xFF)					//we have a broadcast
			{	switch (hb[0])
				{	
				case mftTestResponse:
				case mftBACnetDataNotExpectingReply:
					goto ReceivedDataNoReply;
				case mftToken:
				case mftBACnetDataExpectingReply:
				case mftTestRequest:
				default:
ReceivedUnwantedFrame:
					unwantedframecount++;		//ReceivedUnwantedFrame
//					MakeLogAll("<Unwanted Frame>");
					mnstate=mnsmIdle;
					break;
				}
			}
			else if (hb[1]==ts)					//it's for us
			{	switch (hb[0])
				{
				case mftToken:					//ReceivedToken
//					MakeLogAll("<Token>");
					framecount=0;
					solemaster=false;			//							***BSW
					mnstate=mnsmUseToken;
//***
					receivedvalidframe=false;	//needed because we no longer break
					goto usetoken;
//					break;
//***
				case mftPollForMaster:			//ReceivedPFM
//					MakeLogAll("<PollForMaster>");
					SendFrame(mftReplyToPollForMaster,hb[2],0,NULL);
					mnstate=mnsmIdle;
					break;
				case mftBACnetDataNotExpectingReply:
				case mftTestResponse:
ReceivedDataNoReply:
					ReceiveIndicate(hb[0],hb[1],hb[2],rlen,rbuf);
					mnstate=mnsmIdle;
					break;
				case mftTestRequest:				//ReceivedDataNeedingReply (Mgmt)
					ReceiveIndicate(hb[0],hb[1],hb[2],rlen,rbuf);
					mnstate=mnsmIdle;			//Note: request has been answered!
					break;
				case mftBACnetDataExpectingReply:	//ReceivedDataNeedingReply
					ReceiveIndicate(hb[0],hb[1],hb[2],rlen,rbuf);
					ReplyTimer=0;
					mnstate=mnsmAnswerDataRequest;
					break;
				default:
					goto ReceivedUnwantedFrame;
				}
			}
			receivedvalidframe=false;
		}
		else if (SilenceTimer>Tnotoken)
		{	MakeLogAll("Lost Token");			//LostToken
			mnstate=mnsmNoToken;
			goto notoken;
		}
		break;

//[USE_TOKEN]----------------------------------------------------------------------
	case mnsmUseToken:							//we have the token, send some frames
usetoken:
		havetoken=true;
		if (((p=GetQ(&outgoingreply))!=NULL)||((p=GetQ(&outgoingrequest))!=NULL))	//we have something to do
		{	switch (p->frametype)
			{
			case mftTestRequest:
			case mftBACnetDataExpectingReply:
				mnstate=mnsmWaitForReply;		//SendAndWait
				goto oktosend;
			case mftTestResponse:
			case mftBACnetDataNotExpectingReply:
				mnstate=mnsmDoneWithToken;		//SendNoWait
oktosend:		SendFrame(p->frametype,p->destaddr,p->length,(p->length==0)?NULL:p->buf);
				framecount++;
				free(p);
				break;
			default:
				free(p);
			}
		}
		else
		{	framecount=Nmaxinfoframes;			//NothingToSend
			mnstate=mnsmDoneWithToken;
			goto donewithtoken;
		}
		break;

//[WAIT_FOR_REPLY]----------------------------------------------------------------------
	case mnsmWaitForReply:						//wait for a reply from another node
		havetoken=true;
		if (SilenceTimer>=Treplytimeout)
		{	MakeLogAll("Reply Timeout");
			framecount=Nmaxinfoframes;			//ReplyTimeout
			mnstate=mnsmDoneWithToken;
		}
		else if (receivedinvalidframe)			//InvalidFrame
		{	invalidframecount++;
			receivedinvalidframe=false;
			mnstate=mnsmDoneWithToken;
		}
		else if (receivedvalidframe)			//we saw a valid frame
		{	validframecount++;
			if (hb[1]==ts)						//it's for us
			{	switch (hb[0])
				{
				case mftBACnetDataNotExpectingReply:
				case mftTestResponse:
												//ReceivedReply
					ReceiveIndicate(hb[0],hb[1],hb[2],rlen,rbuf);
					mnstate=mnsmDoneWithToken;
					break;
				case mftReplyPostponed:
//					MakeLogAll("<ReplyPostponed>");
					mnstate=mnsmDoneWithToken;	//ReceivedPostpone
					break;
				default:
ReceivedUnexpectedFrame:
//					MakeLogAll("<Unexpected>");
					unwantedframecount++;		//ReceivedUnexpectedFrame
					mnstate=mnsmIdle;
					break;
				}
			}
			else //(if ((hb[1]==0xFF)||(hb[1]!=ts))		//we have a broadcast or not for us
				goto ReceivedUnexpectedFrame;
			receivedvalidframe=false;
		}
		break;

//[DONE_WITH_TOKEN]----------------------------------------------------------------------
	case mnsmDoneWithToken:						//send another frame, pass the token or poll for a master
donewithtoken:
		havetoken=true;
		if (framecount<Nmaxinfoframes)
		{	mnstate=mnsmUseToken;				//SendAnotherFrame
//***
			ReceiveFrameStateMachine();			//this is required or we drop bytes
			goto usetoken;
//***
		}
		tsplus1=(ts!=Nmaxmaster)?ts+1:0;
		if (ns==tsplus1) goto sendtoken;		//(normally we pass token to ts+1)
		if (tokencount<Npoll)
		{	if (solemaster)
			{	framecount=0;					//SoleMaster
				tokencount++;
				mnstate=mnsmUseToken;
				goto usetoken;
			}
			else								//we're not the sole master
			{									//SendToken	
sendtoken:		tokencount++;
sendtoken2:		retrycount=0;
				eventcount=0;
				SendFrame(mftToken,ns,0,NULL);
				mnstate=mnsmPassToken;
			}
		}
		else //(tokencount>=Npoll)
		{	psplus1=(ps!=Nmaxmaster)?ps+1:0;
			if (psplus1!=ns)
			{	ps=psplus1;						//SendMaintenancePFM
sendpfm:		retrycount=0;
				SendFrame(mftPollForMaster,ps,0,NULL);	//we have to find a node to send the token to
				mnstate=mnsmPollForMaster;
				break;
			}
			else //(psplus1==ns)
			{	if (solemaster)					//							***BSW Begin
				{	nsplus1=(ns!=Nmaxmaster)?ns+1:0;	//SoleMasterRestartMaintenancePFM
					ps=nsplus1;
					ns=ts;						//no known successor
					tokencount=0;
					eventcount=0;
					goto sendpfm;
				}
				else //(solemaster==false)		//							***BSW End
				{	ps=ts;						//ResetMaintenancePFM
					tokencount=0;
					goto sendtoken2;			//we know who to send the token to
				}
			}
		}
		break;

//[PASS_TOKEN]----------------------------------------------------------------------
	case mnsmPassToken:							//listen for a successor to begin using the token we just passed
		havetoken=true;
		if ((SilenceTimer<Tusagetimeout)&&(eventcount>Nminoctets))
		{	mnstate=mnsmIdle;					//SawTokenUser
			goto idle;
		}
		if (SilenceTimer>=Tusagetimeout)
		{	if (retrycount<Nretrytoken)
			{	retrycount++;					//RetrySendToken
				SendFrame(mftToken,ns,0,NULL);
				eventcount=0;
			}
			else
			{	nsplus1=(ns!=Nmaxmaster)?ns+1:0;	//FindNewSuccessor
				ps=nsplus1;					
				SendFrame(mftPollForMaster,ps,0,NULL);
				ns=ts;							//no known successor
				retrycount=0;
				tokencount=0;
				eventcount=0;
				mnstate=mnsmPollForMaster;
			}
		}
		break;

//[NO_TOKEN]----------------------------------------------------------------------
	case mnsmNoToken:							//the line has been silent, wait some more to see if we can create a token
notoken:
		if (SilenceTimer>=(Tnotoken+(Tslot*ts)))	//&&(SilenceTimer<(Tnotoken+(Tslot*(ts+1)))))
		{	havetoken=true;
			tsplus1=(ts!=Nmaxmaster)?ts+1:0;	//GenerateToken
			ps=tsplus1;					
			SendFrame(mftPollForMaster,ps,0,NULL);
			ns=ts;								//next station is unknown
			tokencount=0;
			retrycount=0;
			eventcount=0;
			mnstate=mnsmPollForMaster;
			goto pfm;
		}
		else if (eventcount>Nminoctets)
		{	mnstate=mnsmIdle;					//SawFrame
			goto idle;
		}
		havetoken=false;
		break;

//[POLL_FOR_MASTER]----------------------------------------------------------------------
	case mnsmPollForMaster:						//listen for reply to Poll for Master to find sucessor node
pfm:	havetoken=true;
		if (receivedvalidframe)
		{	validframecount++;
			if ((hb[1]==ts)&&(hb[0]==mftReplyToPollForMaster))
			{	//MakeLogAll("<Reply To PFM>");
				ns=hb[2];						//ReceivedReplyToPFM
				SendFrame(mftToken,ns,0,NULL);
				ps=ts;
				tokencount=0;
				retrycount=0;
				solemaster=false;				//							***BSW
				mnstate=mnsmPassToken;
			}
			else
			{	//MakeLogAll("<Unexpected>");
				unwantedframecount++;			//ReceivedUnexpectedFrame 
				mnstate=mnsmIdle;				//this will drop the token
			}
			receivedvalidframe=false;
		}
		else if (receivedinvalidframe)
		{	invalidframecount++;
			receivedinvalidframe=false;
			goto pfmfailed;	
		}
		else if (SilenceTimer>=Tusagetimeout)
		{
pfmfailed:
			if (solemaster)
			{	framecount=0;					//SoleMaster
				mnstate=mnsmUseToken;
			}
			else
			{	if (ns!=ts)
				{	eventcount=0;				//DoneWithPFM
					SendFrame(mftToken,ns,0,NULL);
					retrycount=0;
					mnstate=mnsmPassToken;
				}
				else
				{	psplus1=(ps!=Nmaxmaster)?ps+1:0;
					if (psplus1!=ts)
					{	ps=psplus1;				//SendNextPFM
						SendFrame(mftPollForMaster,ps,0,NULL);
						retrycount=0;
					}
					else	//(psplus1==ts)
					{	solemaster=true;		//DeclareSoleMaster
						framecount=0;
						mnstate=mnsmUseToken;
					}
				}
			}
		}
		break;

//[ANSWER_DATA_REQUEST]----------------------------------------------------------------------
	case mnsmAnswerDataRequest:					//we received a request that requires an answer
		havetoken=false;
		if((p=GetQ(&outgoingreply))!=NULL)
		{	SendFrame(p->frametype,p->destaddr,p->length,(p->length==0)?NULL:p->buf);	//Reply
			mnstate=mnsmIdle;
			goto idle;
		}	
		else if (ReplyTimer>Treplydelay)
		{	SendFrame(mftReplyPostponed,hb[2],0,NULL);	//DeferredReply
			mnstate=mnsmIdle;
			goto idle;
		}
		break;
	}
	if (havetoken!=lasttoken)
	{	ShowToken();
		lasttoken=havetoken;
	}
}

///////////////////////////////////////////////////////////////////////
//	Handle Incoming Frame
//in:	ft			frame type
//		da			destination address
//		sa			source address
//		rlen		received length
//		rp			receive buffer pointer (or NULL if rlen==0)

void ReceiveIndicate(octet ft, octet da, octet sa, int rlen, octet *rp)
{	mmpkt	*m;

	switch(ft)									//based on frame type...
	{
	case mftBACnetDataExpectingReply:
//		MakeLogAll("<DataXReply>");
		goto indicate;
	case mftTestResponse:
//		MakeLogAll("<TestResp>");
		goto indicate;
	case mftBACnetDataNotExpectingReply:
//		MakeLogAll("<DataNoReply>");
indicate:
		if((m=(mmpkt *)malloc(sizeof(mmpkt)))!=NULL)	//can't get a buffer for it
		{	m->frametype=ft;					//we may or may not care about this
			m->destaddr=da;						//so we know who to reply to
			m->sourceaddr=sa;
			m->length=rlen;
			CopyMemory(&m->buf[0],rp,rlen);		//rlen will always be <512 length
			if (ft!=mftTestResponse)
				PutQ(&incoming,m);				//issue DL-UNITDATA.indication here
			else
			{	ShowTestResponse(m);				//for now we'll deal with it
				free(m);
			}
		}
		else
			MakeLogAll("Out of Memory");
// Note: Pretend we passed it on to the app. If a response is expected, we'll eventually send a 
//		 ReplyPostponed and the sender will timeout and either retry or fail.
		break;

	case mftTestRequest:
//		MakeLogAll("<TestReq>");
		SendFrame(mftTestResponse,sa,rlen,(rlen)?rp:NULL);
		break;

	}
}

///////////////////////////////////////////////////////////////////////
//	Shut down the DLL

void ExitMSTP()
{	NOTIFYICONDATA	tnd;

	tnd.uID=100;
	tnd.cbSize=sizeof(tnd);
	tnd.hWnd=hStatus;
	tnd.uCallbackMessage=WM_USER_SYSTRAY;
	Shell_NotifyIcon(NIM_DELETE,&tnd);			// End

	StopTimer();								//give up our timer
	SJACKdeinit();								//shut off serial module
	DeleteObject(hRed);							//give back resources
	DeleteObject(hGreen);
	DeleteObject(hToken);
	DeleteObject(hNotoken);
	DestroyWindow(hStatus);						//shut off main window
    UnregisterClass("MSTPCL",hDLLInst);			//give up classes we've registered
    UnregisterClass("MSTPABOUTCL",hDLLInst);
}

///////////////////////////////////////////////////////////////////////
//	Display the About Window
                            
void ShowAboutWindow(int forsecs)
{	Running=forsecs;                           
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
	HMENU		hmenu;
	BOOL		check;
	long		cWnd;
	char		b[256];
	MApacket	mp;
	int			i;

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

          case ID_HELP_ABOUTMSTP:
			ShowAboutWindow(10);				//turn on the about window for 10 seconds
           	return 0;

          case IDC_SENDTEST:
			GetDlgItemText(hWnd,IDC_TESTMESSAGE,b,256);
			if (ParseTest(b,&mp))
			{	if ((i=mstpTransmitTest(&mp))==0)
					SetDlgItemText(hWnd,IDC_TESTMESSAGE,"Waiting for Response");
				else
				{	sprintf (b,"Transmit Error %d",i);
					SetDlgItemText(hWnd,IDC_TESTMESSAGE,b);
				}
			}
			else
				SetDlgItemText(hWnd,IDC_TESTMESSAGE,"Error--try: hex unit=test message (hex)");
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
		
	  case WM_USER_TIMER:
		TimerEvent();
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

      case WM_CTLCOLORSTATIC:					//colorize text
        cWnd=GetWindowLong((HWND)lParam,GWL_ID);
		if (cWnd==IDC_TOKEN)
			SetTextColor((HDC)wParam,RGB(255,0,0));
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
	strcpy(b,mstpversion);
	strcat(b," Restarted");
	MakeLog(b);
	MakeLog(mstpname);
	strcpy(tnd.szTip,mstpversion);				//tooltip
	tnd.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uID=100;
	tnd.cbSize=sizeof(tnd);
	tnd.hWnd=hStatus;
	tnd.uCallbackMessage=WM_USER_SYSTRAY;
	tnd.hIcon=LoadIcon(hDLLInst, "TRAYICON");
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
//	DrawBitmap(hStatus,(st ? hGreen : hRed),x,y);	***TEMP
}

///////////////////////////////////////////////////////////////////////
//	Show Token Status

void ShowToken()
{	//int	x=10,y=70;
	//DrawBitmap(hStatus,(havetoken ? hToken : hNotoken),x,y);
	SetDlgItemText(hStatus,IDC_TOKEN,(havetoken)?"*":" ");
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
//	Transmit a frame
//in:	ft		frame type
//		da		destination address (0xFF=broadcast)
//		dlen	length of data dp points to (0..501)
//		dp		points to data to send (or NULL if dlen==0)
//out:	mstpERSuccess
//		mstpERxxx

int SendFrame(int ft,octet da,int dlen,octet *dp)
{	word	actualLen;
	octet	*p,*sp,tcrc=0xFF;
	union {	word dw; octet db[2]; }u;
	int		i,r;
	octet	TxBuf[512];							//2 preamble+6 header+501 data+2 crc+1 pad=512

//	while (SilenceTimer<Tturnaround)			//								***TEMP
//		myYield();								//								***TEMP
	SilenceTimer=0;
	sp=dp;
	p=&TxBuf[0];								//point to transmit buffer
	*p++=0x55;
	*p++=0xFF;
	*p++=(octet)(ft&0xFF);
	tcrc=CalcHeaderCRC((octet)(ft&0xFF),tcrc);
	*p++=da;
	tcrc=CalcHeaderCRC(da,tcrc);
	*p++=ts;									//sa always=ts
	tcrc=CalcHeaderCRC(ts,tcrc);
	u.dw=dlen;
	*p++=u.db[1];
	tcrc=CalcHeaderCRC(u.db[1],tcrc);
	*p++=u.db[0];
	tcrc=CalcHeaderCRC(u.db[0],tcrc);
	*p++=(~tcrc);
	if (dlen!=0)
	{	u.dw=0xFFFF;							//data CRC is a word
		for (i=0;i<dlen;i++)
		{	u.dw=CalcDataCRC(*dp,u.dw);
			*p++=*dp++;
		}
		*p++=(~u.db[0]);
		*p++=(~u.db[1]);
	}
	*p++=0xFF;									//dummy at the end (only for MS/TP)
	actualLen=p-&TxBuf[0];
	r=Send((byte *)TxBuf,actualLen);

// We do this here instead of the top so we can see stuff in the TX/RX window
	if ((ft==mftTestRequest)&&(da==ts))			//this is a loopback test
		SimulateTestResponse (dlen,sp);
/*	switch(ft)
	{
	case ftToken:
		MakeLogAll("<-Send Token");
		break;
	case ftPollForMaster:
		MakeLogAll("<-Send PFM");
		break;
	case ftReplyToPollForMaster:
		MakeLogAll("<-Send Reply to PFM");
		break;
	case ftTestRequest:
		MakeLogAll("<-Send Test Req");
		break;
	case ftTestResponse:
		MakeLogAll("<-Send Test Resp");
		break;
	case ftBACnetDataExpectingReply:
		MakeLogAll("<-Send DataXReply");
		break;
	case ftBACnetDataNotExpectingReply:
		MakeLogAll("<-Send DataNoReply");
		break;
	case ftReplyPostponed:
		MakeLogAll("<-Send ReplyPostoned");
		break;
	}
*/
	if (r>=0)
		return mstpERSuccess;
	else
		return r+mstpERSjack;
}


///////////////////////////////////////////////////////////////////////
//	Simulate an incoming test response
//in:	dlen	the length of the test
//		dp		the test data

void SimulateTestResponse (int dlen,octet *dp)
{	union {	word dw; octet db[2]; }u;
	octet	tcrc=0xFF;
	int		i;

	NewTrace(IDC_RX);
	Trace(IDC_RX,0x55,NULL);
	Trace(IDC_RX,0xFF,NULL);
	hb[0]=mftTestResponse;
	hb[1]=ts;
	hb[2]=ts;
	u.dw=dlen;
	hb[3]=u.db[1];
	hb[4]=u.db[0];
	for (i=0;i<5;i++)
	{	Trace(IDC_RX,hb[i],NULL);
		tcrc=CalcHeaderCRC(hb[i],tcrc);
	}
	hb[5]=(~tcrc);
	Trace(IDC_RX,hb[5],NULL);
	if (dlen!=0)
	{	u.dw=0xFFFF;							//data CRC is a word
		for (i=0;i<dlen;i++)
		{	u.dw=CalcDataCRC(*dp,u.dw);
			rbuf[i]=*dp++;
			Trace(IDC_RX,rbuf[i],NULL);
		}
		rlen=dlen;
		rbuf[i]=(~u.db[0]);
		Trace(IDC_RX,rbuf[i++],NULL);
		rbuf[i]=(~u.db[1]);
		Trace(IDC_RX,rbuf[i],NULL);
	}
	receivedvalidframe=true;
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
{	if (d==0x10||d==0x11||d==0x13)				//these need escaping
	{	*p++=0x10;
		*p++=d|0x80;
	}
	else
		*p++=d;									//else pass it on as-is
	return p;
}

///////////////////////////////////////////////////////////////////////
//	Add a packet to a pq
//
//in:	q		points to the pq to insert into
//		p		points to the packet to be inserted

void PutQ(pq *q, mmpkt *p)
{	mmpkt	*lg=q->rear;						//lg is the last guy in present queue

	if(lg==NULL)								//p is the first and only guy
		q->front=p;								//so front also points to him
	else										//in all other cases, rear guy must point to new guy
		lg->next=p;
	q->rear=p;									//new guy is now the last one
	p->next=NULL;
	q->count++;
}

///////////////////////////////////////////////////////////////////////
//	remove a packet from a pq
//
//in:	q		points to the pq to remove from
//out:	NULL	it's empty
//		else	pointer to an mmpkt

mmpkt *GetQ(pq *q)
{	mmpkt	*fg=q->front;						//fg is the first guy in present queue

	if(fg==NULL) return NULL;					//empty
	if(--q->count==0) q->rear=NULL;
	q->front=fg->next;
	return fg;									//return the first one
}
	
///////////////////////////////////////////////////////////////////////
//	Read Preferences from MSTP.INI
//
//[General]
//	Port=COMx:baud,parity,data,stop
//  Logfile=path\foo.log
//	MaxInfoFrames=1
//	MaxMaster=127
//	TS=0
//
//out:	<0		error

int ReadPreferences()
{	char	b[256],*p;
	int		r;
	LPVOID	lpMsgBuf;
	dword	result,e;

	if ((r=SJACKinit(1))<0) return r+mstpERSjack;	//we only support one port
	GetPrivateProfileString("General","Port","",b,sizeof(b),MSTPPrefs);
	if ((r=SJACKbind(0,b))<0)					//bind to that port
	{	e=GetLastError();
		result=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,NULL,
			e,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,0,NULL);
		sprintf(b,"Error: [%d] ",e);
		if (lpMsgBuf!=NULL)
		{	strcat(b,lpMsgBuf);
			LocalFree(lpMsgBuf);
		}
		MessageBox(hStatus,b,szAppName,MB_ICONSTOP);
		return r+mstpERSjack;		
	}

	GetPrivateProfileString("General","LogFile","",logfile,sizeof(logfile),MSTPPrefs);
	Nmaxinfoframes=GetPrivateProfileInt("General","MaxInfoFrames",1,MSTPPrefs);
	Nmaxmaster=GetPrivateProfileInt("General","MaxMaster",127,MSTPPrefs);
	ts=GetPrivateProfileInt("General","TS",0,MSTPPrefs);
	p=strchr(b,':');
	*p=0;
	sprintf(mstpname,"%.3u MSTP Node %u on %s",mstpport,ts,b);
	return mstpERSuccess;							//OK
}

///////////////////////////////////////////////////////////////////////
// Transmit a buffer with tracing
//
//in:	p			the buffer to send
//		n			the length of p to send

int Send(byte *p, dword n)
{	byte	*q=p;
	dword	i;
	int		result;

// Note: the spec says
//	1. turn on transmitter
//	2. for (i=0;i<n;i++) send octet, SilenceTimer=0;
//  3. wait Tpostdrive?? (not needed because we have a pad???)
//  4. disable transmit
	NewTrace(IDC_TX);
	for (i=0;i<n;i++)
	{	result=SJACKtxchar(0,*q);					//ship the characters one at a time
		if (result!=1) return result;
		Trace(IDC_TX,(octet)*q++,NULL);
		SilenceTimer=0;
	}
	return n;
/*	result=SJACKtx(0,p,n);
	SilenceTimer=0;
	return result;
*/
}

///////////////////////////////////////////////////////////////////////
// Yield some time
//
void myYield ()
{	MSG		msg;

	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{	TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
///////////////////////////////////////////////////////////////////////
//	add a new entry to a trace Log in Status Dialog

void NewTrace(int which)
{	
	if (logall==false) return;
	while (GetListCount(hStatus,which)>=maxtrace)
		RemoveItem(hStatus,which,0);
		
// 	AddItem(which,"");							//used "add" not "enter" so list doesn't jump!
 	EnterItem(which,"");
	if(which==IDC_RX)
	{	rxtrace[0]='[';
		FormatTime (elapsedtime,&rxtrace[1],true);
		strcat (rxtrace,"]- ");
//		rxbol=strlen(rxtrace);
		rxbol=7;
		rxindex=GetListCount(hStatus,IDC_RX)-1;
//		rxtrace[0]=0;
	}
	else
	{ 	txtrace[0]='[';
		FormatTime (elapsedtime,&txtrace[1],true);
		strcat (txtrace,"]- ");
//		txbol=strlen(txtrace);
		txbol=7;
		txindex=GetListCount(hStatus,IDC_TX)-1;
//		txtrace[0]=0;
	}
}

///////////////////////////////////////////////////////////////////////
// Trace a transmitted or received octet
//
//in:	w			the window to add trace to
//		c			the new octet to trace
//		msg			if !=NULL, display in place of the character

void Trace(int w,octet c,char *msg)
{	if (logall==false) return;
	if(w==IDC_RX)
	{	if(rxbol>=(tracewidth/2)) NewTrace(IDC_RX);
		{	if (msg==NULL)
				cvhexa(&rxtrace[(rxbol++)*2],c);
			else
				strcat(&rxtrace[rxbol*2],msg);
		}
		ReplaceItem(IDC_RX,rxindex,rxtrace);
	}
	else
	{	if(txbol>=(tracewidth/2)) NewTrace(IDC_TX);
		{	if (msg==NULL)
				cvhexa(&txtrace[(txbol++)*2],c);
			else
				strcat(&txtrace[txbol*2],msg);
		}
		ReplaceItem(IDC_TX,txindex,txtrace);
	}
}

///////////////////////////////////////////////////////////////////////
// Convert a byte to asciiz. <-----NOTE this!!! 
//
//in:	cp	points to the buffer to put it into
//		b	the byte to convert

void cvhexa(char *cp,byte b)
{	byte	nh,nl;

	nh=(b>>4)&0xF;
	nl=b&0xF;
	if (nh<10)
		*cp++=(char)(nh+0x30);			//+'0'
	else
		*cp++=(char)(nh+0x37);			//-10+'A'
	if (nl<10)
		*cp++=(char)(nl+0x30);
	else
		*cp++=(char)(nl+0x37);
	*cp=0;
}

///////////////////////////////////////////////////////////////////////
// Format a time to min:sec.msec
//
//in:	timer		the time to format
//		cp			pointer to the character buffer

void FormatTime (int timer, char *cp, BOOL padded)
{	dword	min,sec,msec;

	sec=timer/1000;
	msec=timer%1000;
	if (padded)
	{	min=sec/60;
		sec=sec%60;
		sprintf (cp,"%3lu:%.2lu.%.3lu",min,sec,msec);
	}
	else
	{	if (sec<60)
			sprintf (cp,"%lu.%.3lu",sec,msec);
		else
		{	min=sec/60;
			sec=sec%60;
			sprintf (cp,"%lu:%.2lu.%.3lu",min,sec,msec);
		}
	}
}

/////////////////////////////////////////////////////////////////////// 
//	Parse an MS/TP test message
// in:	cp		ascii test message of the form: hex unit=hex message
//					eg.01=122334455 
//		mp		the MApacket to build
// out:			TRUE=ok, else error

BOOL ParseTest (char *cp, MApacket *mp)
{	char	*bp;
	int		n;
	
	bp=strchr(cp,'=');
	if (bp==NULL) return false;
	cvhexn(cp,mp->maDadr,1);
	if (mp->maDadr[0]==0xFF) return false;
	bp++;
	n=strlen(bp);
	n=(++n)>>1;
	if (n)
		cvhexn(bp,mp->maNPDU,n);
	mp->maNlen=n;
	mp->maLport=mstpport;
	mp->maAlen=1;
	mp->maLsap=0x82;								//BACnet LSAP
	mp->maBroadcast=0;
	return true;
}

/////////////////////////////////////////////////////////////////////// 
//	Show the response to an MS/TP test request
//in:	m		points to an mmpkt
void ShowTestResponse (mmpkt *m)
{	char	b[128],*p;
	int		i,n;

	b[0]=0;
	i=0;
	p=&b[0];
	sprintf(p,"%.2X ",m->sourceaddr);
	p[2]='=';
	p[3]=0;
	p+=3;
	n=(m->length<62)?m->length:62;
	while (i<n)
	{	sprintf(p,"%.2X",m->buf[i++]);
		p[2]=0;
		p+=2;
	}
	SetDlgItemText(hStatus,IDC_TESTMESSAGE,b);

}

/////////////////////////////////////////////////////////////////////// 
//	Convert n*2 HEX chars to binary byte [n] array (little endian)
//in:	src		points to n*2 hex chars
//		dst		points to where n byte value goes
void cvhexn(char *src,byte *dst,int n)
{	int	i;
	for (i=0;i<n;i++)
		src=cvhex(src,&dst[i]);
	return;
}

/////////////////////////////////////////////////////////////////////// 
//	Convert HEX chars to binary byte
//in:	src		points to 2 hex chars
//		dst		points to byte to receive the value
//out:	ptr to 1st non-hex char, or 2 past src
char *cvhex(char *src,byte *dst)
{	if (!isxdigit(*src))
	{	*dst=0;									//assume none
		return src;
	}
	if (isdigit(*src))
		*dst=*src-'0';
	else
		*dst=(*src&0xDF)-55;
	src++;
	if (!isxdigit(*src)) return src;
	if (isdigit(*src))
		*dst=(*dst<<4)+(*src-'0');
	else
		*dst=(*dst<<4)+((*src&0xDF)-55);
	src++;
	return src;
}
