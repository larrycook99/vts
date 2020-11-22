/*---------------------------------------------------------------------------
	©1998, PolarSoft Inc. and National Institute for Standards and Technology

module:		NALSERVNT.C
desc:		BACnet Network & Application Layer Server for Windows NT DLL v2.18
authors:	David M. Fisher, Jack R. Neyer
last edit:	01-Mar-00 [218] JN  mac32.h for maximum of 2 adapters
			16-Dec-99 [216]	JN	add option to be a passive server
			14-Dec-99 [215] JN  if there's a source addr in TX, make hopcount look like it's passed thru router  
			13-Dec-99 [214] DMF fix response to routed WhoIs
			15-Dec-99 [213] JN  use mymalloc and myfree
								add option to be a passive server
								fix segmented server receive
			15-Dec-99 [212] JN  add PTP support
			08-Dec-98 [211] JN  check port validity, systray, share pragma
			13-Nov-98 [210] JN  support for multiple adapters
			01-Sep-98 [200] JN  support for ARCNET
			01-Sep-98 [190] JN  allow transmit of simple-ack, error anytime
			20-Oct-98 [170] JN  fixes for routing
			30-Sep-98 [165] JN  fix InhibitTX
			24-Aug-98 [162] JN  fix for configuration params
			21-Jul-98 [161] JN  fix toggle of network layer display
			15-Jul-98 [149] JN  make sure broadcast address is set in DoTransmit
			12-Jun-98 [148] JN  fixed receipt of routing info
			20-May-98 [147] JN  removed ***145
								fixed window size management
								added byte-to-any, any-to-byte
			11-May-98 [146] JN  BACMAC
			08-Apr-98 [145] JN  send I_am when we receive an I_am
			--------- [144]     skipped
			20-Mar-98 [143] JN  critical mgmt in Listener
			20-Mar-98 [142] JN  replace _disable,_enable with critical section mgmt
			19-Mar-98 [141] JN  fix MApacket queue sync
			12-Mar-98 [140] JN  threaded reads
			10-Mar-98 [131] JN  wait for transmit completion
			23-Feb-98 [128] JN  fix so process is ended properly
			23-Feb-98 [127] JN  more MainWndProc/DllMain etc. for 32-bit
			18-Feb-98 [101] JN  send Iam, WhoIs at startup
			10-Feb-98 [100] JN  windows 95 version	
x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-
			10-Feb-98 [043] JN  refix free in NALSAPIrequest
			10-Feb-98 [042] JN  fix to send segmented CACKS and segmented NACKS
			21-Nov-96 [041] JN  add source addr for transmits/fix free in NALSAPIrequest 
			28-Oct-96 [040] JN  fix maxserverids checking in timers
			05-Jun-96 [039] DMF change for use with MACAPI 0.11 (MACSYS)              
			15-May-96 [038] DMF fix network number display
			31-Jan-96 [037] DMF fix Hop Count skipping and return of NL packets
			14-Dec-95 [036] JN  changes to pass all received packets to VTS per Peter D.@ NIST
			07-Dec-95 [035] JN  remove abort tx in idle state
			06-Dec-95 [034] DMF fix segmentation bit when transmitting, fix 2nd segment of segmented request
			13-Nov-95 [033] JN  add niRESET, niFLUSH_CANCEL
			03-Nov-95 [032] JN  add subnet/node init for echelon and eliminate neuron id addressing
			01-Nov-95 [031] DMF add special echelon configuration, fix segmented transmissions
			01-Nov-95 [030] DMF add network layer stuff to segment ACKs
			01-Nov-95 [029] DMF fix DNET size adjustment in ReceiveIndicate
			31-Oct-95 [028] DMF remove some echelon support (now in MACAPI)
			30-Oct-95 [027] DMF	fix DNET, SNET length counting in ReceiveIndicate
			27-Oct-95 [026] JN  fixes for echelon
			25-Oct-95 [025] JN  response from server not expected to confirmed req's not for us
								don't send confirmed acks to packets not for us
			25-Oct-95 [024] JN  fix memory compare in IsForUs and maBroadcast checks
			24-Oct-95 [023] DMF fix Dadr size check
			18-Oct-95 [022] JN  support for promiscuous mode
			11-Oct-95 [021] DMF add echelon VxD support
			05-Oct-95 [020] DMF fix Dlen,Dadr when Rlen=0
			04-Oct-95 [019] DMF add NALSAPIgetapdu, 
								NALSAPIgetmaxresponse renamed from getmaxresponse
								when local PDU is received, return nalsDlen+nalsDadr if not broadcast
			20-Sep-95 [018] DMF	fix NL interpretation in ShowAsHex
			18-Sep-95 [017] DMF fix minimize at startup
			13-Sep-95 [016] JN  add TimeSync functions
			12-Sep-95 [015] JN  use WhoIs, IAm primatives from BACPRIM
			11-Sep-95 [014] DMF change calls to dENUM,dUINT,dOBJECTID,dCONTEXT
			07-Sep-95 [013] DMF change WhoIsIAmInterval to 0=disable from -1=disable
			04-Sep-95 [012] JN  support for routing
			04-Sep-95 [011] JN  add NALSAPIgetlen, NALSAPIcopyrequest
								fix getmaxlen
			04-Sep-95 [010] DMF remove unused config dialog variables
			28-Aug-95 [009] JN  add peer management 
			28-Aug-95 [008] JN  fix vendor name setup, max tx len for unconfirmed requests
			22-Aug-95 [007] DMF move encoding primitives to BACPRIM
			15-Aug-95 [006] JN  fix Nretry
			11-Aug-95 [005] JN  add additional config variables (limited marks)
								add our device object (us)
			31-Jul-95 [004] JN  Add encoding primitives
			28-Jul-95 [003] DMF fix NL interpretation display
			20-Jul-95 [002] JN  Support for Segmentation
			14-Apr-95 [001] DMF First Cut
-----------------------------------------------------------------------------*/ 

// This version is Windows 95 compatible and uses the NDIS packet driver 
// BACMAC95. Its interface to the mac layer is through the PACKET95 library
// and is similar to BACdor95.

// This version DOES NOT SUPPORT ECHELON!!!


#include <windows.h>
#include <windowsx.h>
#include <process.h>						//												***140
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <dos.h>
#include <sys\stat.h>
#include <sys\types.h>
#include "nals-res.h" 
#include "macp32.h"
#include "nalsapi.h"
#include "error.h"
#include "service.h"
#include "db.h"
#include "stdobj.h"
#include "bacprim.h"
#include "packet32.h"
#include "ntddndis.h"
#include "ptp.h"									//									***212
#include "mstp.h"									//									***217
///////////////////////////////////////////////////////////////////////
//	Module Constants
#define		TextWidth			55					//width of list boxes in chars		***038
#define		TimerID				1					//main timer identifier
#define		TimerInterval		100					//100 ms
#define		Interleave			2
#define		maxlistitems		500					//max size of hex lists				***037
#define		WM_USER_INITIATE	(WM_USER+1)
#define		WM_SYSTRAY			(WM_USER+2)			//our systray message				***211
#define		BACnetLSAP			0x82
#define		InitialHopCount		255					//									***013
#define		maxserverids		32					//max number of simultaneous server requests
//#define		maxlogicalboards	1					//for now only one logical board***210
#define		maxpeers			32					//also see peerlist below			***009

#define		HandShake			100L				//handshake time (ms)				***140
#define		ListenTime			500L				//listener thread timeout (ms)		***140
#define		XMAXMAS				100					//									***140***143

#define		MENU_EXIT			0					//positional indices for menus
#define		MENU_CONFIGURE		1
#define		MENU_VIEW			2
#define		MENU_HELP			3

//																					***142 Begin
#define		mydisable	EnterCriticalSection(&lockout)
#define		myenable	LeaveCriticalSection(&lockout)
//																					***142 End

///////////////////////////////////////////////////////////////////////
// Global variables
const char	szAppName[]="NALSERVNT";					
const char	version[] = "BACnet NL&AL Server DLL v2.18";	//						***218 Begin
#define		vstring		"v2.18"
#define		ver100		218									//version*100			***218 End
#define		inifile		"\\NALSERV32.INI"
#define		hlpfile		"NALSERV32.HLP"
#define		peerfile	"\\NALSERV32.PER"				//									***009

#pragma data_seg("BDSHARE")					//										***211 Begin
HWND		hStatus=NULL;							//handle of our Status Dialog
HWND		hAbout=NULL;							//handle of our About Box window 
HANDLE		hDLLInst=NULL; 
HBITMAP		hRed=NULL,hGreen=NULL;
HICON		hIcon=NULL;								//handle of our icon					***211
BOOL		Minimized=FALSE;
BOOL		InhibitTX=FALSE;						//									***013 
BOOL		UsePTP=false;							//									***212
BOOL		ptpinitialized=false;					//									***212
BOOL		checkptp=false;							//									***212
BOOL		UseMSTP=false;							//									***217
BOOL		mstpinitialized=false;					//									***217
BOOL		PrefsDirty=FALSE;
BOOL		receivefilter=FALSE;					//state of receive filtering
BOOL		MadeNLComplaint=FALSE;
BOOL		MadeALComplaint=FALSE;
dword		TxCounter=0;							//count of outgoing packets
dword		RxCounter=0;							//count of incoming packets			***100
dword		RxTO=0;									//count of timeouts					***100
dword		RxError=0;								//count of Errors (malformed bacnet packets)		***100
dword		RxToss=0;								//count of tossed packets			***100
dword		RxNoMAs=0;								//										***140 Begin
unsigned long hListener=0;							//handle of Listener Thread process
HANDLE		RXEvents[MAXADAPTERS*MAXREADREQS]={NULL};	//									***210
int			oktolisten=1;							//for listener thread shutdown
MApacket	*usedma=NULL;		//,*freema=NULL,mabuff[MAXMAS];
word		maused=0;
word		maxmaused=0;							//									***140 End
static CRITICAL_SECTION	lockout={0};				//									***142
int			DLLusers=0;
int			IAmCounter=0;							//start these at 0 not 5			***212 Begin
int			SynchCounter=0;
int			WhoIsCounter=0;							//									***212 End
NALSstats	ourstats={0};							//local copy of statistics
FARPROC		NLserver=NULL;							//points to an NL callback
FARPROC		ALserver=NULL;							//points to an AL callback
char		HelpFile[128]={"?"};					//path to NALSERV32.HLP
char		nalsdir[128]={"?"}; 					//path to NALSERV32 files
POINT		ULtxrx={0};								//upper left corner of tx/rx packet lists in client pixel offsets
MApacket	mainmap={NULL};							//unloading packet					***039

NALSpacket *ALQfront=NULL;							//									***212
NALSpacket *ALQrear=NULL;							//									***212

BOOL		showstats=true;						//show MAC stats						***100
int			nextInvokeID=0;
CLIENTid	cid[256]={0};
SERVERid	sid[maxserverids]={0};

NALSpeers	peerlist[maxpeers]={0};				//										***009
//NALSlboards	logicalboards[maxlogicalboards]={0};									***210
int			boundadapters=0;					//										***210
CONTROL_BLOCK	Adapter[MAXADAPTERS]={0,0};		//info about our adapter(s)			***210 Begin***218
//PVOID		PacketRX[SIMULTANEOUS_READS]={NULL};	//packet objects for reading		***110
//PVOID		PacketRX;								//a packet object for reading		***110
//PVOID		PacketTX=NULL;							//a packet object for writing		***131
//																						***210 End
int			NEXTREAD=0;								//									***110

word		cidcount=0;								//current idcount cid
word		sidcount=0;								//current idcount sid				***100 End
//																						***005 Begin
device_obj_type	us=
				{
													//generic object properties
					NULL,							//next
					1,								//object id (default)
					"NALTESTDeviceObject",			//object name
					DEVICE,							//object type
					"NALTEST Device Object",       	//description
					{idProp+nameProp+typeProp},		//std props							***040
													//device object properties:
					OPERATIONAL,					//system status
					"PolarSoft® Inc.",				//vendor name (default)				***212
					4,								//vendor id (default)
					"NALSERV 32-bit",				//model name
					vstring,						//firmware rev						***013
					vstring,						//application software ver 			***013
					"NIST",							//location,
					1,								//protocol ver
					3,								//protocol conf class
					{0,0,0,0,0},					//protocol services supported (later)
					{0,0,0},						//protocol types supported (later)
					0,								//number of objects					***039
					NULL,							//object list (later)
					1470,							//max apdu length accepted (later)
					SEGMENTED_BOTH,					//segmentation supported (default)
					NULL,							//*vt classes supported
					NULL,							//*active vt sessions
					{0,0,0,0},						//local date						***039
					{0,0,0,0},						//local time						***039
					0,								//utc offset (in minutes)
					TRUE,							//day savings status
					2000,							//apdu segment timeout (msec) (default)
					60000,							//apdu timeout (msec) (default)
					3,								//number apdu retries (default)
					NULL,							//list session keys
					NULL,							//time synch recipients
					0,								//max master
					0,								//max info frames
					NULL							//device add binding					
				};
//																						***005 End
			
octet eupdomain[33]	= { 						//to set subnet,node and domain			***032 Begin
						0x22,0x1F,				//niNETMGMT|niTQ,length
						0x60,0x08,0x11,			//explicit msg|REQUEST|no authenticate|cookie=0,nml|primary|not completion|explicit|defaultpath|outgoing|other,length
						0,0,0,0,0,0,0,0,0,0,0,	//explicit address field (11 octets)
						0x63,					//Update domain
						0,						//domain=0
						0,0,0,0,0,0,			//domain id (6 bytes)
						0xFF,0xFF,				//subnet=255,node 127 + msb set (defaults)
						0x00,					//domain id length
						0xFF,0xFF,0xFF,0xFF,0xFF,0xFF	//encryption key (6 bytes)
					   };
	
typedef struct {	//echadd
	octet	esubnet;
	octet	enode;
	} echadd;									//										***032 End

//---------------------------------------------------------------------
//	Configuration Variables (read from NALSERV.INI)
int			MinimizeStatus=0;					//0=no, 1=yes
char		OurPeername[25];					//asciz peername for this server
int			WhoIsIAmInterval=0;					//minutes between WhoIs/IAms			***212
int			TimeSynchInterval=0;				//minutes between TimeSyncs				***212
uint		Tseg=2*10;							//in 100 millisecond increments (also us.apdu_segment_timeout)
uint		Tout=60*10;							//in 100 millisecond increments (also us.apdu_timeout)
uint		Nretry=3;							//number of times to try (also us.number_apdu_retries)
uint		MaxSizeALapdu=8192;					//max size of a assembled APDU from segments
//																						***005 Begin
uint		OurInstance=1;						//for now a unit instead of a dword (also us.object_id)
uint		OurVendorID=4;						//(also us.vendor_id)
char		OurVendorName[65]="PolarSoft® Inc.";	//(also us.vendor_name)				***212
uint		MaxTXapduLen=1470;					//local restrictions (for Ethernet)
uint		MaxRXapduLen=1470;					//local restrictions (for Ethernet) (also us.max_apdu_length_accepted)
uint		WindowSize=1;						//window size for client confirmed requests
int			SegmentedRX=1;						//0=no, 1=yes (also us.segmentation_supported) 
int			SegmentedTX=1;						//0=no, 1=yes (also us.segmentation_supported)
int			passive=1;							//passive server						***216
int			ispromiscuous=1;					//									***NALS
word		PTPsnet=0x1234;						//fake source net for PTP				***212 Begin
word		PTPslen=1;							//fake source address length for PTP
octet		PTPsadr[8]={0x56,0,0,0,0,0,0,0};	//fake source address for PTP			***212 End
#pragma data_seg()								//										***211 End
//																						***005 End
//---------------------------------------------------------------------
//  Large Static Tables
char		*NLMessages[]={
			"Who-Is-Router-To-Network",			//0
			"I-Am-Router-To-Network",			//1
			"I-Could-Be-Router-To-Network",		//2
			"Reject-Message-To-Network",		//3
			"Router-Busy-To-Network",			//4
			"Router-Available-To-Network",		//5
			"Initialize-Routing-Table",			//6
			"Initialize-Routing-Table-Ack",		//7
			"Establish-Connection-To-Network",	//8
			"Disconnect-Connection-To-Network"	//9
			};
#define		NumNLMessages						10
///////////////////////////////////////////////////////////////////////
//	function prototypes
LRESULT CALLBACK  MainWndProc(HWND, UINT, WPARAM, LPARAM);			//				***100 Begin
LRESULT CALLBACK  Configure(HWND, UINT,WPARAM,LPARAM);
LRESULT CALLBACK  About(HWND,UINT,WPARAM,LPARAM);					//				***100 End
//---------------------------------------------------------------------
int		AddItem(HWND,int,const char *);						//						***021
BOOL	BadPort(word);										//						***212
void	BuildNALSpacket(NALSpacket *,octet,dword,octet *,octet,BOOL,int); //		***212
void	ClearItems(HWND,int);								//						***021
void	CloseAdapters(int,int);								//						***210
void	CopyToComma(char **,char **);
void	CreateTrayIcon(void);								//						***211
char	*cvhex(char *,byte *);
dword	cvhexdw(char *);
void	cvhexn(char *,byte *,int);							//						***009
void	cvhex4be(char *,word *);
void	cvhex8be(char *,dword *);
void	DeleteTrayIcon(void);								//						***211
int		DoTransmit(MApacket *);								//						***212
void	DrawBitmap (HWND,HBITMAP,int,int);
int		EnterItem(int,const char *);
word	FindAPDU(MApacket *);								//						***212
int		FillWindow(int,ALTYPE);
int		getinvokeid (void);
int		GetListCount(int);
int		GetListText(int,int,char *);
uint	getmaxlength (NALSpacket *);						//						***212
uint	getmaxlengthNIC (word);
BOOL	GetNextALPacket(NALSpacket *);						//						***212
int		GetSelectItem(HWND,int);
LPVOID	GetSystemError(void);								//						***140
LPVOID	GMALLOC (dword);
void	GFREE (LPVOID);
void	HandleWhoisIam (NALSpacket *);
BOOL	InWindow (octet,octet,int,ALTYPE);
BOOL	IsCheckedMenuItem(int,UINT);
BOOL	IsEcho (octet,octet *);								//						***212
BOOL	IsForUs (octet,octet *);							//						***212
BOOL	IsSource (int, octet, NALSpacket *, ALTYPE);
UINT	lcb(int,UINT,UINT);
void	Listener(void *);									//						***140
void	LogNPDUerror (MApacket *,char *);					//						***212
void	MainLoop(void);
void	MakeLog(char *,int);
char	*Nxt(char *);
void	ReadPeerList (void);
void	ReadPreferences (void);
void	ReceiveIndicate(MApacket *mp);						//						***212
BOOL	ReceiveIndicateClient (int,MApacket *,NALSpacket *,BOOL *);		//			***100
BOOL	ReceiveIndicateServer (int,MApacket *,NALSpacket *);			//			***100
void	RemoveItem(int,int);
int		ReplaceItem(int,int,char *);
void	SavePreferences (void);
void	SelectItem(HWND,int,int);								//					***021
void	SendAbort(MApacket *,octet,octet,ALTYPE);			//						***212
void	SendSegmentedAck(MApacket *,NALSpacket *,octet,octet,octet,BOOL,ALTYPE); //	***212
void	ShowAsHex(MApacket *,UINT);							//						***212
void	ShowIdle (BOOL);
void	ShowPacketWindows(void);
void	StartUp(void);
void	TimerEvent(void);
void	TransmitTest(void); 
void	ToggleCheckMenuItem(int,UINT);
word	UnicodeToAscii (char *,wchar_t *,word);		//									***210
word	wbige(word);								//									***003
word	wpbige(word *);								//									***003
int		YesNo (char *);

///////////////////////////////////////////////////////////////////////
//	DLL entrypoint called from Windows					//								***100 Begin

BOOL APIENTRY DllMain(HANDLE hInst, DWORD ul_reason_being_called, LPVOID lpReserved)
{	
	hDLLInst = hInst;
	switch (ul_reason_being_called)
	{
    case DLL_PROCESS_ATTACH:
		break;
    case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
//	UNREFERENCED_PARAMETER(ul_reason_being_called);
//	UNREFERENCED_PARAMETER(lpReserved);
}																			//		***100 End

//-------------------------------- Begin API Functions --------------------------------
///////////////////////////////////////////////////////////////////////
//	DLL initialization called from Application

int APIENTRY NALSAPIinit(word *vp)			//								***100
{   WNDCLASS	wc;
	int			i;

	*vp=ver100;									//return our version
	if (DLLusers++>1) return NALSE_Success;		//just return if already inited
	
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = DLGWINDOWEXTRA;
    wc.hInstance = hDLLInst;
    hIcon = LoadIcon(hDLLInst,"NALSICON");		//											***211
    wc.hIcon = hIcon;							//											***211
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
    wc.lpszMenuName =  "MainMenu";
    wc.lpszClassName = szAppName;
    if (!RegisterClass(&wc))
    	return NALSE_FailedClassRegistration;	//failed
    
//	create the windows we need and allocate a timer
    hStatus = CreateDialog(hDLLInst,"NALSERVNT",0,NULL);
    if (!hStatus) return NALSE_CantCreateWindow;
    
    if (!SetTimer(hStatus,TimerID,TimerInterval,NULL))
	{	DestroyWindow(hStatus);					//we don't need the window now
      	return NALSE_NoFreeTimers;
	}

	InitializeCriticalSection(&lockout);			//get a critical section object		***142

//																						***009 Begin
	for (i=0;i<maxpeers;i++)
		peerlist[i].nalspLport=255;
	
/*	for (i=0;i<maxlogicalboards;i++)													***210 Begin
	{	logicalboards[i].nalslLboard=255;		//										***022
		logicalboards[i].nalslAlen=0;			//										***022
		memset(&logicalboards[i].nalslAdr,0,8);		
	}
*/												//										***210 End
//																						***009 End	
  	for (i=0;i<=255;i++)
		cid[i].cidState=stateFree;

  	for (i=0;i<maxserverids;i++)
  		sid[i].sidState=stateFree;

  	ReadPreferences();							//locate and read preferences			***NALS

   	ClearItems(hStatus,IDC_LOG);				//so we see bindings					***210
 	if (MACPInit()!=MAE_Success)				//										***100 Begin
	{	DestroyWindow(hStatus);
		return NALSE_MACPNotInstalled;			//										***100 End
	}

   	ReadPeerList();								//locate and read peerlist				***009

//	removed build logical board list													***100

	hRed=LoadBitmap(hDLLInst,"REDLED");
	hGreen=LoadBitmap(hDLLInst,"GREENLED");

//	ShowWindow(hStatus,(MinimizeStatus?SW_SHOWMINIMIZED:SW_SHOWNORMAL));				***017 Delete
    SendMessage (hStatus,WM_USER_INITIATE,0,0L); //Get things going
	for (i=0;i<boundadapters;i++)				//										***210 Begin
   	{	if (Adapter[i].hFile!=INVALID_HANDLE_VALUE)			//This should never happen
		{	NALSAPItransmitIam((octet)i,BroadcastDNET,NULL,0);
			NALSAPItransmitWhois((octet)i,BroadcastDNET,NULL,0,FALSE,0,0);
			if(mstpinitialized)												//			***217 Begin
			{	NALSAPItransmitIam((octet)mstpport,BroadcastDNET,NULL,0);	
	 			NALSAPItransmitWhois((octet)mstpport,BroadcastDNET,NULL,0,false,0,0);
			}									//										***217 End
		}
	}											//										***210 End
    return NALSE_Success;
}

///////////////////////////////////////////////////////////////////////
//	DLL deinitialization called from Application

void APIENTRY NALSAPIdeinit()			//									***100 Begin
{	DWORD	stat;
	if (--DLLusers==0)
	{
		DeleteTrayIcon();						//remove us from systray	***211
		KillTimer(hStatus,TimerID);		//give up our timer					***102
		MACPDeinit();					//									***102
		DeleteCriticalSection(&lockout);		//kill a critical section object	***142
		DeleteObject(hRed);										
		DeleteObject(hGreen);
		DestroyWindow(hStatus);
		if (!UnregisterClass(szAppName,hDLLInst))
		{	stat=GetLastError();
		}
	}
}										//									***100 End
///////////////////////////////////////////////////////////////////////
//	Get MAC Layer Port List, called from Application

word APIENTRY NALSAPIgetportlist(char *pl)			//									***100
{	return MACPgetportlist(pl);						//									***100
}

//																						***011 Begin
///////////////////////////////////////////////////////////////////////
//	Get MAC Layer Length of Addresses on a Logical Board, called from Application

word APIENTRY NALSAPIgetlen(word lbd)				//									***100 Begin
{	octet	addr[8];
	return MACPgetaddr(lbd,addr);					//									***100 End					
}	

//																						***011 End
//																						***100 Begin
///////////////////////////////////////////////////////////////////////
//	Get the max size of an NPDU

word APIENTRY NALSAPImaxNPDUlength(word lport)
{	if(BadPort(lport)) return 0;					//invalid port						***212
	return MACPmaxNPDUlength(lport);
}
//																						***100 End
//																						***022 Begin
///////////////////////////////////////////////////////////////////////
//	Get MAC Layer Address of a Logical Board, called from Application
// in:	lport 	has the logical port
//		ap		points to buffer to fill in with address
// out:	0		invalid logical port
//		!=0		address length
word APIENTRY NALSAPIgetaddr(word lport, octet *ap)	//									***212
{	if(BadPort(lport)) return 0;					//invalid port						***212
	return MACPgetaddr(lport,ap);								//						***100
}													//									***022 End

//																						***011 End
///////////////////////////////////////////////////////////////////////					***019 Begin
//Test for received APDU, called from application
//in:	np			points to a NALSpacket which will be filled in
//		apmax		size of the buffer ap ppoints to
//		ap			points to a buffer which the APDU will be copied into
//out:	NALSE_Success					there was an APDU, and it was copied successfully
//		NALSE_ReceivedPacketTooLarge	can't fit the packet into apmax
//										In this case, np is filled in, but ap is not...
//		NALSE_NoPacketsAvailable		no packets in the queue

int APIENTRY NALSAPIgetapdu(NALSpacket *np,word apmax,octet *ap)	//			***100
{	int	r;

	if (GetNextALPacket(np)==FALSE) return NALSE_NoPacketsAvailable;	//there is no packet		***022
	if (np->nalsAlen>apmax)					//is it too big?
		r=NALSE_ReceivedPacketTooLarge;		//yes
	else
	{	memcpy(ap,np->nalsAPDU,np->nalsAlen);
		r=NALSE_Success;					//copied ok
	}
	if (np->nalsAPDU!=NULL) GFREE(np->nalsAPDU);
	np->nalsAPDU=NULL;						//don't leave pointers around
	return r;
}												//						***019 End

///////////////////////////////////////////////////////////////////////
//	Get Next APDU, called from Application.
//
//         Note: Is not promiscuous!!
// out:	TRUE  if packet is available, caller must free the APDU packet
//		FALSE if no packet in the queue
//																		***022 Begin
BOOL APIENTRY NALSAPIgetpacket(NALSpacket *np)					//						***212
{
	if (GetNextALPacket(np)==FALSE) return FALSE;				//no packets are in the queue
	if (BadPort(np->nalsLport)) return false;	//										***212
	if ((np->nalsDlen==0)||(IsForUs(np->nalsLport,&np->nalsDadr[0])))	//either a broadcast or for us ***026
		return TRUE;
	else
	{	if (np->nalsAPDU!=NULL) GFREE(np->nalsAPDU);
		np->nalsAPDU=NULL;								//don't leave pointers around
		return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////
//	Get Next APDU, from the AL queue

BOOL GetNextALPacket(NALSpacket *np)
{	NALSpacket *tnp;							//											***212

	if (ALQfront==NULL) return FALSE;			//no packets in the AL queue
	mydisable;									//disable task premption					***142
	memcpy(np,ALQfront,sizeof(NALSpacket));		//copy to caller's NALSpacket
	tnp=ALQfront;								//save ptr to it
	if ((ALQfront=ALQfront->nalsNext)==NULL)	//delink front guy
		ALQrear=NULL;							//it was the only guy in the queue
	myenable;									//											***142
	GFREE(tnp);								//give up the space
	return TRUE;
}
//																						***022 End

//																						***011 Begin
///////////////////////////////////////////////////////////////////////
//Transmit APDU, called from application
//in:	np			points to a NALSpacket containing description of the packet to send
//					except that the np->nalsAPDU field is not initialized!
//		ap			points to the APDU to be copied
//out:	error code or NALSE_Success

int APIENTRY NALSAPIcopyrequest(NALSpacket *np,octet *ap)					//			***100
{	FILE *op;
	op = fopen("d:\\lastapdu.bin","wb");
	if(op != NULL){
		fwrite(ap,1,np->nalsAlen,op);
		fclose(op);
	}
	
	if ((np->nalsAPDU=GMALLOC(np->nalsAlen))==NULL)
		return NALSE_NoFreeSpaceAvailable;
	memcpy(np->nalsAPDU,ap,np->nalsAlen);
	return NALSAPIrequest(np);
}
//																						***011 End
///////////////////////////////////////////////////////////////////////
//	Transmit APDU, called from Application
//
//in:	np				points to NALSpacket to control transmit
//		np->nalsDlen	=0 if broadcast
//		np->nalsSlen	=0 normally									***019
//						>0 if source routing to be added here		***019
//out:	error code or NALSE_Success

int  APIENTRY NALSAPIrequest(NALSpacket *np)		//			***100
{	ASDU		*ap;
	ASDUREQ		*apr;
	MApacket	*mp;
	int			pdutype,i,j,id,isnode;
	uint		maximum_transmittable_length;
	octet		*pp,*ppp;
	char		b[80];

	if(BadPort(np->nalsLport)) return NALSE_InvalidPort;	//							***212
	ap=(ASDU *)np->nalsAPDU;						//point to APDU header
	if ((mp = GMALLOC(sizeof (MApacket)))==NULL)	//allocate space for MA packet
	{	GFREE(ap);									//									***041
		return NALSE_NoFreeSpaceAvailable;
	}

//																						***012	Begin
/*	Routing is handled in NALSERV APDU transmissions in the according to the following guidelines:
		1. if nalsDnet == 0 then send on local network only (No Routing)
			maDadr = nalsDadr
			maLport = nalsLport
			no NSDU DNET,DADR
		2. if nalsDnet == 0xFFFF then broadcast on all ports, Global network
			NSDU DNET == 0xFFFF
			no NSDU DADR 
		3. if nals Dnet == 1 to 0xFFFE
			maDadr = nalsRadr
			maLport = nalsLport
			NSDU DNET = nalsDnet
			if nalsDnet!=0 then NSDU DADR = nalsDadr
		Note: normally nalsSlen=0 meaning "don't add source routing info"
			  if nalsSlen!=0 then add source routing info
*/			

	pdutype=((ap->asduPDUheader&ASDUpdutype)>>ASDUpdutypeshr); //get the pdu type field
	mp->maLsap=BACnetLSAP;
	pp=&mp->maNPDU[0];								//point to NPCI header
	*pp++=1;										//BACnet version 1
	*pp=(np->nalsPri&3)+(np->nalsExpectReply?4:0); 	//APDU, assume no DNET, no SNET
	mp->maNlen=2;									//initial length
	mp->maLport=np->nalsLport;
	if(mp->maLport==ptpport)						//handle ptp specially				***212 Begin***217 Begin
	{	np->nalsSnet=PTPsnet;						//force PTP to always appear routed
		np->nalsSlen=(octet)PTPslen;
		memcpy(&np->nalsSadr,&PTPsadr,PTPslen);
		mp->maAlen=0;
	}												//									***212 End
	else if (mp->maLport==mstpport)					//handle mstp specially also
	{	mp->maAlen=1;								//get address length
		mp->maSadr[0]=mstpGetTS();					//for debugging
		mp->maDadr[0]=0xFF;
	}
	else											//must be ethernet or arcnet
	{	mp->maAlen=(octet)Adapter[mp->maLport].Alen;		//get address length		***210 Begin
		memcpy (&mp->maSadr[0],&Adapter[mp->maLport].Adr[0],mp->maAlen);		//for debugging
		if (Adapter[mp->maLport].AdapterType==ETH_802_3_ADAPTER)				//		***200 Begin
			memset(&mp->maDadr,0xFF,sizeof(mp->maDadr));
		else
			memset(&mp->maDadr,0,sizeof(mp->maDadr));
	}
//																						***200 End
//																						***210 End***217 End
	switch (np->nalsDnet)
	{
	case 0:										//Local network
		*pp|=(np->nalsSlen==0?0:NSDUhasSNET);			//								***019
		pp++;											//just point past NPCI
		if (np->nalsDlen==0)							//broadcast if Dlen=0
		{	
//			memset(&mp->maDadr,0xFF,sizeof(mp->maDadr));						***200
			mp->maBroadcast=1;
		}
		else											//specific destination
		{	memcpy(&mp->maDadr,&np->nalsDadr,mp->maAlen);
			mp->maBroadcast=0;
		}
		if (np->nalsSlen)								//add source routing info ***019 Begin
		{	*pp++=(octet)(np->nalsSnet>>8)&0xFF;
			*pp++=(octet)np->nalsSnet&0xFF;
			*pp++=np->nalsSlen;
			memcpy(pp,&np->nalsSadr,np->nalsSlen);
			pp+=np->nalsSlen;
			mp->maNlen+=(3+np->nalsSlen);
		}												//						***019 End
		break;

	case BroadcastDNET:							//Global Network
		*pp|=NSDUhasDNET|(np->nalsSlen==0?0:NSDUhasSNET);	//					***019
		pp++;
		*pp++=(BroadcastDNET>>8)&0xFF;					//Dnet=Global
		*pp++=BroadcastDNET&0xFF;
		*pp++=0;										//Dlen=0
		i=4;											//4 octets so far		***019 Begin
		if (np->nalsSlen)								//add source routing info
		{	*pp++=(octet)(np->nalsSnet>>8)&0xFF;
			*pp++=(octet)np->nalsSnet&0xFF;
			*pp++=np->nalsSlen;
			memcpy(pp,&np->nalsSadr,np->nalsSlen);
			pp+=np->nalsSlen;
			i+=(3+np->nalsSlen);
		}												//								***019 End
		*pp++=(np->nalsSlen)?InitialHopCount-1:InitialHopCount;		//Hop count			***013***215
		mp->maNlen+=i;									//initial length
/*		for (i=0;i<maxlogicalboards;i++)				//TEMP for now only on first board found***210 Begin
		{	if (logicalboards[i].nalslLboard!=255)		//								***022
			{	mp->maLport=logicalboards[i].nalslLboard;	//							***022
				break;
			}
		}
*/														//								***210 End
//		memset(&mp->maDadr,0xFF,sizeof(mp->maDadr));	//also broadcast address		***200
		mp->maBroadcast=1;
		break;
		
	default:									//Route 
		*pp|=NSDUhasDNET|(np->nalsSlen==0?0:NSDUhasSNET);	//					***019 Begin
		pp++;
		*pp++=(octet)(np->nalsDnet>>8)&0xFF;			//Dnet
		*pp++=(octet)np->nalsDnet&0xFF;
		i=4;
		if (np->nalsRlen==0)							//broadcast if Rlen=0
		{	
//			memset(&mp->maDadr,0xFF,sizeof(mp->maDadr));							***200
			mp->maBroadcast=1;
		}
		else											//specific destination
		{	 memcpy(&mp->maDadr,&np->nalsRadr,mp->maAlen);
			mp->maBroadcast=0;
		}
		*pp++=np->nalsDlen;								//Dlen					***020 Begin
		if (np->nalsDlen)
		{	 memcpy(pp,&np->nalsDadr,np->nalsDlen); 	//Dadr
			pp+=np->nalsDlen;
			i+=np->nalsDlen;							//initial length
		}												//						***020 End

		if (np->nalsSlen)								//add source routing info
		{	*pp++=(octet)(np->nalsSnet>>8)&0xFF;
			*pp++=(octet)np->nalsSnet&0xFF;
			*pp++=np->nalsSlen;
			memcpy(pp,&np->nalsSadr,np->nalsSlen);
			pp+=np->nalsSlen;
			i+=(3+np->nalsSlen);
		}												//								***019 End
		*pp++=(np->nalsSlen)?InitialHopCount-1:InitialHopCount;		//Hop count			***215
		mp->maNlen+=i;									//initial length				***019 End
	}
//																						***012 End
	switch(pdutype)
	{
	case CONF_REQ_PDU:								//CLIENT Transmit							
		id = getinvokeid();
		if (id==NALSE_NoFreeInvokeIDs)
		{	i=id; 													//					***041
			goto sexit;												//					***041
		}
		np->nalsInvokeID=id;										//so requestor can match response
		cid[id].cidRequestorisserver=np->nalsRequestorisserver;			
		apr=(ASDUREQ *)ap;											//so we can reference confirmed request ASDU
		maximum_transmittable_length=getmaxlength(np);
        cid[id].cidAPDUPacket=(octet *)ap;							//it's our responsibility to free this packet
		cid[id].cidDlen=np->nalsDlen;								//so we know who the reply is directed at	***009
		memcpy(cid[id].cidDadr,np->nalsDadr,cid[id].cidDlen);		//							***009
		cid[id].cidDnet=np->nalsDnet;								//we have to provide our own network number		***009
		cid[id].cidLport=mp->maLport;
		if (np->nalsAlen>maximum_transmittable_length)				//this guy requires segmentation
		{	if (SegmentedTX!=1)
			{	i=NALSE_TransmitPacketTooLarge;						//segmentation not supported***041
				goto sexit;											//							***041
			}
 			*pp++=apr->asdureqPDUheader | ASDUissegmented | ASDUmoresegments | (SegmentedRX*ASDUsegmentrespaccepted);		//PDU header
			*pp++=(octet)NALSAPIgetmaxresponse((word)mp->maLport);	//max response							***019
			*pp++=(octet)id;										//invoke id
			*pp++=0;												//sequence number
			*pp++=(octet)WindowSize;								//proposed window size
			cid[id].cidNPCI_APCILen=(mp->maNlen)+6;					//number of octets of overhead before the APDU Data (including NPCI) ***034
 			memcpy(pp,&apr->asdureqServiceChoice-2,maximum_transmittable_length+mp->maNlen-(cid[id].cidNPCI_APCILen)+1);	//copy APDU octets including service choice ***034
            cid[id].cidMaxLen=maximum_transmittable_length;			//actual size of APDU
            mp->maNlen+=maximum_transmittable_length;				//actual size of NPDU we're transmitting
            cid[id].cidOctetCount=(np->nalsAlen-4);					//total number of APDU data bytes remaining to be sent (exclude header, maxresp, invokeID, service choice) ***034
            cid[id].cidAPDUData=&apr->asdureqServiceChoice-1;		//where we'll get the data from now on		***034
            cid[id].cidTxPacket=(octet *)mp;						//the packet we'll transmit
            cid[id].cidSequence=cid[id].cidInitialSequence=0;
//            cid[id].cidActualWindowSize=(octet)WindowSize;		//					***147
			cid[id].cidActualWindowSize=1;							//better get an ack right after the first one ***147
            cid[id].cidMore=TRUE;          
			cid[id].cidTimer=Tseg;
            cid[id].cidTries=Nretry+1;								//										***006
			mp->maTrace=TxCounter++;
			if (i=DoTransmit(mp))
			{	sprintf(b,"NPDU Transmit Failed:%d",i);
				MakeLog(b,FALSE);									//					***041
				goto sexit; 										//					***043							***041
			}
			else
            {	cid[id].cidState=stateSegmentedRequest;
				goto sucxmit;										//					***100
			}
//			goto sexit; 											//					***043							***041
		}
		else										//no segmentation is required
		{	*pp++=apr->asdureqPDUheader|(SegmentedRX*ASDUsegmentrespaccepted);	//PDU header				***034
			*pp++=(octet)NALSAPIgetmaxresponse((word)mp->maLport);	//max response							***019
			*pp++=(octet)id;										//invoke id
 			memcpy(pp,&apr->asdureqServiceChoice-2,(np->nalsAlen)-3);		//copy APDU data octets
            mp->maNlen+=np->nalsAlen;
            cid[id].cidTxPacket=(octet *)mp;						//this is the packet we'll transmit
           	cid[id].cidTimer=Tout;
            cid[id].cidTries=Nretry+1;								//							***006
			mp->maTrace=TxCounter++;
			if (i=DoTransmit(mp))
			{	sprintf(b,"NPDU Transmit Failed:%d",i);
				MakeLog(b,FALSE);									//							***041
				goto sexit;											//					***043
 			}
			else
			{	cid[id].cidState=stateAwaitConf;            
sucxmit:		ShowAsHex(mp,IDC_TXLIST);			//display this packet in the transmit list
				memcpy(&np->nalsSadr,&mp->maSadr,mp->maAlen);	//copy source address	***019
				np->nalsSlen=mp->maAlen;					//							***019
//				i=NALSE_Success;							//							***043
				return NALSE_Success;						//							***043
			}
//			goto sexit; 									//							***041***043
		}
		break;
		
	case UNCONF_REQ_PDU:
		maximum_transmittable_length=getmaxlength(np);		//								***008					
		if (np->nalsAlen>maximum_transmittable_length)
		{	i=NALSE_TransmitPacketTooLarge;					//we cannot segment				***041
			goto sexit; 									//								***041
		}
reqtransmit:
		mp->maNlen+=np->nalsAlen;							//add length of APDU
		memcpy(pp,np->nalsAPDU,np->nalsAlen);				//copy APDU octets
		mp->maTrace=TxCounter++;
		if (i=DoTransmit(mp))
		{	sprintf(b,"NPDU Transmit Failed:%d",i);
			MakeLog(b,FALSE);
		}
		else
			ShowAsHex(mp,IDC_TXLIST);						//display this packet in the transmit list
		memcpy(&np->nalsSadr,&mp->maSadr,mp->maAlen);		//copy source address			***019
		np->nalsSlen=mp->maAlen;							//								***019
		goto sexit; 										//								***041

	case SIMPLE_ACK_PDU:								//								***190 Begin
	case ERROR_PDU:
		goto reqtransmit;								//								***190 End

	default:									//all others must be in AwaitResponse state 
												//(i.e. we're going to reply to a request)
	
												//we're acting as a server--
												//see if we can find an entry in sid that corresponds to this
												//destination address and invoke ID. If yes, then this is a
												//response to a server packet and must be managed by our
												//server manager. Otherwise toss the packet.
		for (i=0;i<maxserverids;i++)			//use i since invoke id != index into sid
//																						***009 Begin
		{	isnode=memcmp(&sid[i].sidSadr,&np->nalsDadr,np->nalsDlen);			//is node address=destination
			if ((sid[i].sidState==stateAwaitResponse)&&
				(sid[i].sidLport==np->nalsLport)&&
			    (sid[i].sidSnet==np->nalsDnet)&&
			    (isnode==0)&&
			    (sid[i].sidInvokeID==ap->asduInvokeID)) goto reqserver;			//this source and invoke ID have been found
//																						***009 End				
		}
		i=NALSE_InvalidRequestThisState;			//we couldn't find a match..toss it	***041
		goto sexit;									//									***041

reqserver:
		//NOTE: At this point we know sid[i].sidState=stateAwaitResponse--no need to check

		switch (pdutype)
		{
		case COMPLEX_ACK_PDU:
			maximum_transmittable_length=getmaxlength(np);
			if ((np->nalsAlen+mp->maNlen)>maximum_transmittable_length)
			//this guy requires segmentation
			{	if (SegmentedTX!=1)
				{	GFREE(mp);
					return NALSE_TransmitPacketTooLarge;				//segmentation not supported
				}
				if (np->nalsDlen==0)									//broadcast if Dlen=0 (should never happen)
				{	i=NALSE_InvalidDestinationAddr;						//						***041
					goto sexit;											//						***041
				}
				if (sid[i].sidSegmentAccepted==FALSE)					//requestor does not support segmentation
				{	SendAbort (mp, ap->asduInvokeID, ABORT_SegmentationNotSupported, SERVER);
					i=NALSE_CannotSendSegmentedCACK;					//						***041
					goto sexit;											//						***041
				}
				sid[i].sidState=stateSegmentedResponse;					//this will be our next state
				sid[i].sidSlen=np->nalsDlen;							//so we know the source address for the reply
				memcpy(sid[i].sidSadr,np->nalsDadr,sid[i].sidSlen);
				sid[i].sidSnet=np->nalsDnet;
				ppp=&mp->maNPDU[0];										//point to NPCI header
				*ppp++;
				*ppp=(*ppp&0xFB)|4;										//segmented complex ack requires response
				*pp++=ap->asduPDUheader | ASDUissegmented | ASDUmoresegments;		//PDU header
				*pp++=ap->asduInvokeID;									//original id
				*pp++=0;												//sequence number
				*pp++=(octet)WindowSize;								//proposed window size ***147
	            sid[i].sidNPCI_APCILen=(mp->maNlen)+5;					//number of octets of overhead before the APDU Data (including NPCI) ***034
	 			memcpy(pp,&ap->asduServiceAckChoice-2,maximum_transmittable_length+mp->maNlen-(sid[i].sidNPCI_APCILen)+1);	//copy APDU octets including service choice ***034
	            sid[i].sidMaxLen=maximum_transmittable_length;			//max APDU length
	            mp->maNlen+=maximum_transmittable_length;				//actual NPDU length
	            sid[i].sidAPDUPacket=(octet *)ap;						//it's our responsibility to free this packet
				sid[i].sidInvokeID=ap->asduInvokeID;					//since i != invoke ID
	            sid[i].sidOctetCount=(np->nalsAlen-3);					//total number of APDU data bytes remaining to be sent (exclude header,invokeID,service choice ***034)
	            sid[i].sidAPDUData=&ap->asduServiceAckChoice-1;			//where we'll get the data from now on	***034
	            sid[i].sidTxPacket=(octet *)mp;							//the packet we'll transmit
	            sid[i].sidMore=TRUE;          
	            sid[i].sidSequence=sid[i].sidInitialSequence=0;
//	            sid[i].sidActualWindowSize=(octet)WindowSize;			//				***147
	            sid[i].sidActualWindowSize=1;							//				***147
				sid[i].sidTimer=Tseg;
	            sid[i].sidTries=Nretry+1;          						//									***006
				mp->maTrace=TxCounter++;
				if (j=DoTransmit(mp))
				{	sprintf(b,"NPDU Transmit Failed:%d",j);
					MakeLog(b,FALSE);
					sid[i].sidState = stateFree;        //							***041
					i=j;								//							***043
					goto sexit;							//							***043
				}
				else
	            {	ShowAsHex(mp,IDC_TXLIST);			//display this packet in the transmit list
//					j=NALSE_Success;												***043	
					return NALSE_Success;				//cant' free ma, ap here..they're in cid struct		***043
				}
//				i=j;                                    //					    ***041***043
//				goto sexit; 							//						***041***043
			}
		
			//else no segmentation is required, so just ship it!
//		case SIMPLE_ACK_PDU:						//											***190
//		case ERROR_PDU:								//											***190
		case SEGMENT_ACK_PDU:
		case REJECT_PDU:
		case ABORT_PDU:
			sid[i].sidState=stateFree;
			goto reqtransmit;
				
		default:									//cannot transmit invalid or unknown PDU
			i=NALSE_InvalidPDUType;					//oops										***041 Begin
		}
	}
sexit:
	if (mp!=NULL) GFREE(mp);						//free the mp we got						***100
	if (ap!=NULL) GFREE(ap);						//we must free APDU packet from the caller	***100
	return i;										//											***041 End
}

////////////////////////////////////////////////////////////////////// 		***013 Begin
//	Transmit a packet with InhibitTX support

int DoTransmit (MApacket *mp)
{	int		i;
	
	i=mp->maLport;																//		***210 Begin
	if(InhibitTX) return NALSE_InhibitTX;			//									***212 Begin
	if ((i!=ptpport)&&(i!=mstpport))				//									***217
	{	if (Adapter[i].hFile==INVALID_HANDLE_VALUE)
			return NALSE_InhibitTX;					//									***165
		if (mp->maBroadcast==1)
		{	if (Adapter[i].AdapterType==ETH_802_3_ADAPTER)		//ethernet
				memset(&mp->maDadr,0xFF,sizeof(mp->maDadr));	//let's make sure
			else
				memset(&mp->maDadr,0,sizeof(mp->maDadr));		//let's make sure
		}
		memcpy (&mp->maSadr[0],&Adapter[i].Adr[0],mp->maAlen);		//for debugging
	}												//									***212 End
	else if (i==mstpport)							//									***217 Begin
	{	if (mp->maBroadcast==1) mp->maDadr[0]=0xFF;	//make sure
		mp->maSadr[0]=mstpGetTS();
	}												//									***217 End
	MACPtransmit(mp);
	return MAE_Success;								//									***210 End
}

///////////////////////////////////////////////////////////////////////					***212 Begin
//	validate a port number
//
//out:	true	if port number is invalid

BOOL BadPort(word lport)
{	if(lport==ptpport) return false;				//ptp is ok
	if(lport==mstpport) return false;				//mstp is ok						***217
	if(lport>=(word)boundadapters) return true;		//invalid port
	return false;
}													//									***212 End

///////////////////////////////////////////////////////////////////////
//  Allocate a New Invoke ID for a Client
//in:	nothing
//out:	int value (0-255 valid invokeid, NALSE_NoFreeInvokeIDs none free)
int getinvokeid ()
{	int	i,j;

	for (j=0;j<256;j++)
	{	i = nextInvokeID;
		if (nextInvokeID++==255)
			nextInvokeID=0;
		if (cid[i].cidState==stateFree)
			goto gidend;
	}
	i=NALSE_NoFreeInvokeIDs;
gidend:
	return i;	
}

///////////////////////////////////////////////////////////////////////
//  Get the maximum length of a APDU in octets from minimum of:
//
//	1. maximum apdu length for this NIC
//	2. maximum local restriction (MaxTXapduLen)
//	3. maximum apdu length target can accept
//
//in:	far pointer to NALS packet
//out:	integer value of maximum length of an APDU
uint getmaxlength (NALSpacket *np)
{	uint	maxlen,maxtargetlen;
	int	i;

	maxtargetlen=MaxTXapduLen;							//in case we don't know about this guy
	if (np->nalsDnet==0)
	{	for (i=0;i<maxpeers;i++)						//first check if this is a known peer
		{	if ((peerlist[i].nalspLport!=255)&&
				(peerlist[i].nalspLport==np->nalsLport)&&
				(peerlist[i].nalspNet==np->nalsDnet)&&
				(peerlist[i].nalspAlen==np->nalsDlen)&&
				(memcmp(&peerlist[i].nalspAdr[0],&np->nalsDadr[0],peerlist[i].nalspAlen)==0))
			{	maxtargetlen=peerlist[i].nalspMaxapdulen;
				break;									//this is a known peer
			}
		}
	}
	else if (np->nalsDnet!=BroadcastDNET)
	{	for (i=0;i<maxpeers;i++)						//first check if this is a known peer
		{	if ((peerlist[i].nalspLport==255)&&
				(peerlist[i].nalspLport==np->nalsLport)&&
				(peerlist[i].nalspNet==np->nalsDnet)&&
				(peerlist[i].nalspAlen==np->nalsRlen)&&
				(memcmp(&peerlist[i].nalspAdr[0],&np->nalsDadr[0],peerlist[i].nalspAlen)==0))	//***170
					if ((peerlist[i].nalspRlen!=0) && (peerlist[i].nalspRlen==np->nalsRlen) && 
						(memcmp(&peerlist[i].nalspRadr[0],&np->nalsRadr[0],peerlist[i].nalspRlen)==0))	//	***170
						maxtargetlen=peerlist[i].nalspMaxapdulen;
				break;									//this is a known peer
		}
	}
	maxlen=__min(__min(getmaxlengthNIC(np->nalsLport),MaxTXapduLen),maxtargetlen);
  	if (maxlen<=50) maxlen=50;				//50 is always the minimum
	return maxlen;	
}

///////////////////////////////////////////////////////////////////////
//  Get the maximum length of a APDU in octets for our NIC
//in:	logical port
//out:	integer value of maximum length of an APDU
uint getmaxlengthNIC (word Lport)					//									***WIN95
{	uint	maxlenNIC;

// First determine the maximum length of an APDU for the local NIC
	maxlenNIC = MACPmaxNPDUlength(Lport);			//This is the maximum length of an NPDU for this NIC
	if (maxlenNIC<=50)
		return 50;									//50 is always the minimum

	if (maxlenNIC==501)								//looks like an ARCNET
	{  	maxlenNIC-=14;								//deduct max length of NPCI to get max length APDU 
		goto gmln1;
	}
	
	maxlenNIC-=24;									//use worst case max length of NPCI to get max length APDU 
													//								***021 End
gmln1:
	if (maxlenNIC<50)
		maxlenNIC=50;								//50 is always the minimum
	
	return maxlenNIC;
 }

///////////////////////////////////////////////////////////////////////
//  Get the maximum response code that we can accept
//in:	logical port
//out:	integer value of the maximum response (0-5)
//			0=up to 50
//			1=up to 128
//			2=up to 206 (fits Lontalk)
//			3=up to 480 (fits ARCNET)
//			4=up to 1024
//			5=up to 1470 (fits ISO 8802-3)

word APIENTRY NALSAPIgetmaxresponse(word Lport)		//									***100
{	word	i;
	word	maxlen;
	word	maxapdulen[5]=
			{128,206,480,1024,1470};

	if(BadPort(Lport)) return 0;					//									***212
	maxlen=__min(getmaxlengthNIC((octet)Lport),MaxRXapduLen);
	for (i=0;i<5;i++)
	{	if (maxlen<maxapdulen[i])
			break;
	}
	return i;
}

///////////////////////////////////////////////////////////////////////
//	DLL statistics called from Application

void APIENTRY NALSAPIgetstatistics(NALSstats *ps)	//				***100
{	memcpy(ps,&ourstats,sizeof(NALSstats));
}

///////////////////////////////////////////////////////////////////////
//	turn receive filtering on/off

void APIENTRY NALSAPIreceivefilter(BOOL rf)				//				***100
{	receivefilter=rf;
}

///////////////////////////////////////////////////////////////////////
//	add a string to the status log list
//in:	ls		points to the asciz string to add

void APIENTRY NALSAPIputlog(char *ls)	//						***100
{	MakeLog(ls,FALSE);
}

///////////////////////////////////////////////////////////////////////	***147 Begin
//	A routine to copy any type to a byte array

void APIENTRY NALSAPIanytobyte(void *anyp,byte *bytep,word num)
{	if (num!=0) memcpy (bytep,anyp,num);
}

///////////////////////////////////////////////////////////////////////
//	A routine to copy a byte array to any type

void APIENTRY NALSAPIbytetoany(byte *bytep,void *anyp,word num)
{	if (num!=0) memcpy (anyp,bytep,num);
}																	//	***147 End

////////////////////////////////////////////////////////////////////////
//	Transmit NPDU, called from Network Layer
//
//in:	mp			points to NALSpacket to control transmit
//out:	MACAPI error code or MAE_Success

int APIENTRY NALSAPItransmit(MApacket *mp)	//					***100
{	MApacket	*tmp;
	char		b[80];
	int			i;

	if ((tmp = GMALLOC(sizeof (MApacket)))==NULL)	//allocate space for MA packet
		return MAE_NoFreeBuffers;
	memcpy(tmp,mp,sizeof(MApacket));				//copy entire packet
	tmp->maTrace=TxCounter++;
	if (i=DoTransmit(tmp))
	{	sprintf(b,"NPDU Transmit Failed:%d",i);
		MakeLog(b,FALSE);
	}
	else
		ShowAsHex(tmp,IDC_TXLIST);					//display this packet in the transmit list

	GFREE(tmp);
	return i;
}

//////////////////////////////////////////////////////////////////////
//	get a string from the status log list
//in:	ls		points to an 81 byte buffer we can put the string in
//out:	true	if a string was returned in ls
//		false	if none available (ls is stuffed with asciz)

BOOL APIENTRY NALSAPIgetlog(char *ls)		//							***100
{	if (GetListCount(IDC_LOG)==0)				//none available
	{	*ls=0;									//make it asciz anyway
		return FALSE;
	}
	GetListText(IDC_LOG,0,ls);					//get first list item
	RemoveItem(IDC_LOG,0);						//and discard it
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
//	register a Network Layer server callback
//in:	srv		points to a callback function (or NULL)
//out:	0		if server was registered successfully
//		!0		an error occurred

int APIENTRY NALSAPIregisterNLserver(FARPROC srv)		//					***100
{	if (srv!=NULL)										//we're registering
	  if (NLserver!=NULL)								//already have a server
	  	return NALSE_NLserverAlreadyRegistered;
	NLserver=srv;
	MadeNLComplaint=FALSE;
	return NALSE_Success;
}

///////////////////////////////////////////////////////////////////////
//	register an Application Layer server callback
//in:	srv		points to a callback function (or NULL)
//out:	0		if server was registered successfully
//		!0		an error occurred

int APIENTRY NALSAPIregisterALserver(FARPROC srv)	//							***100
{	if (srv!=NULL)									//we're registering
	  if (ALserver!=NULL)							//already have a server
	  	return NALSE_ALserverAlreadyRegistered;
	ALserver=srv;
	MadeALComplaint=FALSE;
	return NALSE_Success;
}

///////////////////////////////////////////////////////////////////////
//	Display DLL Configuration Dialog called from Application

void APIENTRY NALSAPIconfigure()								//						***100
{   FARPROC	lpProc;
	
   	lpProc = MakeProcInstance(Configure, hDLLInst);
   	DialogBox(hDLLInst, "NALSConfig",0, lpProc);
   	FreeProcInstance(lpProc);
}

///////////////////////////////////////////////////////////////////////
//	Message Processor for "Configure" dialog box

LRESULT CALLBACK Configure(HWND hDlg,unsigned message,WPARAM wParam,LPARAM lParam)	//	***100
{	static BOOL	Inited,Dirty;
	word		notify;
	int			i,j;
	char		b[20];														//			***212

    switch (message)
    { case WM_INITDIALOG:
    	Inited=FALSE;
    	Dirty=FALSE;
		SendMessage(GetDlgItem(hDlg,IDC_OURPEERNAME),EM_SETLIMITTEXT,24,0L); //			***161
		SendMessage(GetDlgItem(hDlg,IDC_VENDORNAME),EM_SETLIMITTEXT,64,0L); //			***161
		CheckDlgButton(hDlg,IDC_MINIMIZESTATUS,MinimizeStatus);
		CheckDlgButton(hDlg,IDC_INHIBITTX,InhibitTX);		//							***013
		SetDlgItemInt(hDlg,IDC_WHOISIAMINTERVAL,WhoIsIAmInterval,FALSE);
		SetDlgItemInt(hDlg,IDC_TIMESYNCHINTERVAL,TimeSynchInterval,FALSE); //			***016
		SetDlgItemText(hDlg,IDC_OURPEERNAME,OurPeername);
		SetDlgItemInt(hDlg,IDC_TSEG,Tseg/10,FALSE);
		SetDlgItemInt(hDlg,IDC_TOUT,Tout/10,FALSE);
		SetDlgItemInt(hDlg,IDC_NRETRY,Nretry,FALSE);
		SetDlgItemInt(hDlg,IDC_MAXSIZEAPDU,MaxSizeALapdu,FALSE);
		SetDlgItemInt(hDlg,IDC_MAXTXLEN,MaxTXapduLen,FALSE);
		SetDlgItemInt(hDlg,IDC_MAXRXLEN,MaxRXapduLen,FALSE);
		SetDlgItemInt(hDlg,IDC_WINDOWSIZE,WindowSize,FALSE);
		CheckDlgButton(hDlg,IDC_SEGMENTEDRX,SegmentedRX);
		CheckDlgButton(hDlg,IDC_SEGMENTEDTX,SegmentedTX);		
		CheckDlgButton(hDlg,IDC_PASSIVE,passive);								//		***216
		SetDlgItemInt(hDlg,IDC_DEVICEINSTANCE,OurInstance,FALSE);
		SetDlgItemInt(hDlg,IDC_VENDORID,OurVendorID,FALSE);
		SetDlgItemText(hDlg,IDC_VENDORNAME,OurVendorName);
		SendMessage(GetDlgItem(hDlg,IDC_PTPSNET),EM_SETLIMITTEXT,4,0L);		//			***212 Begin
		SetDlgItemInt(hDlg,IDC_PTPSNET,PTPsnet,false);
		SendMessage(GetDlgItem(hDlg,IDC_PTPSLEN),EM_SETLIMITTEXT,1,0L);
		SetDlgItemInt(hDlg,IDC_PTPSLEN,PTPslen,false);
		SendMessage(GetDlgItem(hDlg,IDC_PTPSADR),EM_SETLIMITTEXT,16,0L);
		sprintf(b,"%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
					PTPsadr[0],PTPsadr[1],PTPsadr[2],PTPsadr[3],
					PTPsadr[4],PTPsadr[5],PTPsadr[6],PTPsadr[7]);
		SetDlgItemText(hDlg,IDC_PTPSADR,b);									//			***212 End
		SetDlgItemInt(hDlg,IDC_MAXMASTER,us.max_master,false);				//			***217
		SetDlgItemInt(hDlg,IDC_MAXINFOFRAMES,us.max_info_frames,false);		//			***217
		Inited=TRUE;
        return (TRUE);

      case WM_COMMAND:
        if (wParam == IDOK)
        {	MinimizeStatus=IsDlgButtonChecked(hDlg,IDC_MINIMIZESTATUS);
			InhibitTX=IsDlgButtonChecked(hDlg,IDC_INHIBITTX);	//						***013
			WhoIsIAmInterval=GetDlgItemInt(hDlg,IDC_WHOISIAMINTERVAL,NULL,FALSE);
			TimeSynchInterval=GetDlgItemInt(hDlg,IDC_TIMESYNCHINTERVAL,NULL,FALSE); //	***016
			GetDlgItemText(hDlg,IDC_OURPEERNAME,OurPeername,25);					//	***162
			Tseg=GetDlgItemInt(hDlg,IDC_TSEG,NULL,FALSE)*10;
			Tout=GetDlgItemInt(hDlg,IDC_TOUT,NULL,FALSE)*10;
			Nretry=GetDlgItemInt(hDlg,IDC_NRETRY,NULL,FALSE);
			MaxSizeALapdu=GetDlgItemInt(hDlg,IDC_MAXSIZEAPDU,NULL,FALSE);
			MaxTXapduLen=GetDlgItemInt(hDlg,IDC_MAXTXLEN,NULL,FALSE);
			MaxRXapduLen=GetDlgItemInt(hDlg,IDC_MAXRXLEN,NULL,FALSE);
			WindowSize=GetDlgItemInt(hDlg,IDC_WINDOWSIZE,NULL,FALSE);
			SegmentedRX=IsDlgButtonChecked(hDlg,IDC_SEGMENTEDRX);
			SegmentedTX=IsDlgButtonChecked(hDlg,IDC_SEGMENTEDTX);
			passive=IsDlgButtonChecked(hDlg,IDC_PASSIVE);			//					***216
			ispromiscuous=IsDlgButtonChecked(hDlg,IDC_PROMISCUOUS);
			OurInstance=GetDlgItemInt(hDlg,IDC_DEVICEINSTANCE,NULL,FALSE);
			OurVendorID=GetDlgItemInt(hDlg,IDC_VENDORID,NULL,FALSE);
			GetDlgItemText(hDlg,IDC_VENDORNAME,OurVendorName,65);			//			***162
			PTPsnet=(word)GetDlgItemInt(hDlg,IDC_PTPSNET,NULL,false);		//			***212 Begin
			PTPslen=(word)GetDlgItemInt(hDlg,IDC_PTPSLEN,NULL,false);
			if(PTPslen>8) PTPslen=8;
			GetDlgItemText(hDlg,IDC_PTPSADR,b,PTPslen*2);
			cvhexn(b,PTPsadr,PTPslen);										//			***212 End
			i=(int)GetDlgItemInt(hDlg,IDC_MAXMASTER,NULL,false);			//			***217 Begin
			mstpVarMaxMaster(&i,true);										//write to mstp dll
			j=(int)GetDlgItemInt(hDlg,IDC_MAXINFOFRAMES,NULL,false);
			mstpVarMaxInfoFrames(&j,true);									//write to mstp dll
//																						***005 Begin
			us.max_master=(word)i;
			us.max_info_frames=(word)j;										//			***217 End
			us.apdu_segment_timeout=((dword)Tseg)*100;			//now setup us
			us.apdu_timeout=((dword)Tout)*100;
			us.number_apdu_retries=Nretry;
			us.go.object_id=OurInstance;
			us.vendor_id=OurVendorID;
			for (i=0;i<=sizeof (us.vendor_name)-1;i++)			//						***008       
			{	us.vendor_name[i]=OurVendorName[i];
				if (us.vendor_name[i]==0)
				{	for (j=i+1;j<=sizeof (us.vendor_name)-1;j++)	//					***008
					   us.vendor_name[j]=0;
					break;
				}
			}
			us.max_apdu_length_accepted=MaxRXapduLen;
			if (SegmentedRX==1)
			{
				if (SegmentedTX==1)
					us.segmentation_supported=SEGMENTED_BOTH;
				else
					us.segmentation_supported=SEGMENTED_RECEIVE;
			}
			else if (SegmentedTX==1)
				us.segmentation_supported=SEGMENTED_TRANSMIT;
			else
				us.segmentation_supported=NO_SEGMENTATION;
//																			***005 End
/*			if (Dirty) PrefsDirty=TRUE;
*/
			PrefsDirty=TRUE;
        }
        if (wParam == IDOK || wParam == IDCANCEL)
        { EndDialog(hDlg, TRUE);
          return (TRUE);
        }
        
        //if we get here then one of the dialog's fields must have been changed
        //so we set the dirty flag to force a save to occur later in the main loop
        notify=HIWORD(lParam);
		if (Inited)
			if (notify==EN_CHANGE||notify==BN_CLICKED) Dirty=TRUE; //force a save
        break;
    }
    return (FALSE);
}

///////////////////////////////////////////////////////////////////////
//	About Window Message Processor

LRESULT CALLBACK About(HWND hDlg,unsigned message,WPARAM wParam,LPARAM lParam)	//		***100
{
    switch (message)
    { 
      case WM_INITDIALOG:
		SetDlgItemText(hDlg,IDC_RUNTITLE,version);
       	return TRUE;

      case WM_COMMAND:
      	switch(wParam)
      	{
      	case IDOK:
      	case IDCANCEL:
			EndDialog(hDlg,0);
			return TRUE;
       	}
       	break;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////
//	Main Window Message Processor

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)	//	***100
{   FARPROC	lpProc;
	dword	cWnd;												//						***100

    switch (message)
    {
      case WM_COMMAND:
      	switch(wParam)
      	{
          case ID_CONFIGURE:
           	lpProc = MakeProcInstance(Configure, hDLLInst);
           	DialogBox(hDLLInst,"NALSConfig",hWnd,lpProc);
           	FreeProcInstance(lpProc);
           	return 0;

          case ID_ACTIONS_TRANSMITTEST:
          	TransmitTest();
          	return 0;
          	
          case ID_VIEW_CLEARLOG:
		   	ClearItems(hStatus,IDC_LOG);
           	return 0;

           case IDC_RXLBL:										//						***212 Begin
		   	ClearItems(hStatus,IDC_RXLIST);
           	return 0;

          case IDC_TXLBL:
		   	ClearItems(hStatus,IDC_TXLIST);
           	return 0;											//						***212 End

		  case ID_VIEW_NL:
          case ID_VIEW_RECEIVEPACKETS:
          case ID_VIEW_TRANSMITPACKETS:
			ToggleCheckMenuItem(MENU_VIEW,wParam);
			ShowPacketWindows();
           	return 0;

		  case ID_HELP_CONTENTS:
			WinHelp(hWnd, HelpFile, 1, 1L);
			return 0;

		  case ID_HELP_SEARCHFORHELPON:
			WinHelp(hWnd, HelpFile, 261, 0L);	//"");
			return 0;

          case ID_HELP_ABOUTNALSERV:
			lpProc = MakeProcInstance(About, hDLLInst);
			DialogBox(hDLLInst,"NALSAbout",hWnd,lpProc);
			FreeProcInstance(lpProc);
           	return 0;
       	} //switch wParam
      // case WM_COMMAND
      	break;

	  case WM_USER_INITIATE:
	  	StartUp();
        return 0;
		
	  case WM_SYSTRAY:							//callback from systray					***211 Begin
		if (lParam==WM_LBUTTONDOWN||lParam==WM_RBUTTONDOWN)
		{	ShowWindow(hStatus,SW_SHOWNORMAL);
			return (LONG)true;
		}
		break;									//										***211 End

      case WM_TIMER:
        TimerEvent();						//perform timer stuff
        return 0;
        
      case WM_CREATE:
       	return 0;

      case WM_DESTROY:
//		KillTimer(hStatus,TimerID);				//give up our timer						***100
//     	PostQuitMessage(0);																***100
       	return 0;
       	
      case WM_CTLCOLORLISTBOX:							//colorize controls	***100 Begin
        cWnd=GetWindowLong((HWND)lParam,GWL_ID);
      	if (cWnd==IDC_RXLIST)
      		SetTextColor((HDC) wParam,RGB(255,0,0));
      	else if (cWnd==IDC_TXLIST)
      		SetTextColor((HDC) wParam,RGB(0,0,128));
      	else
      		return (LRESULT) NULL;

		SetBkMode((HDC) wParam,OPAQUE);
   		return (LRESULT) GetStockObject(WHITE_BRUSH);	//					***100 End

	  case WM_SIZE:
	  	Minimized=(wParam==SIZE_MINIMIZED?TRUE:FALSE);
		break;										//				***211 Begin

	  case WM_SYSCOMMAND:
		if (wParam==SC_CLOSE)
		{
			ShowWindow(hStatus,SW_HIDE);		//hide window
			return (LONG) true;					//					***211 End
		}
    }
   	return (DefWindowProc(hWnd, message, wParam, lParam));
}

///////////////////////////////////////////////////////////////////////
//	Start Up Initialization (after initial window display)

void StartUp()
{	HWND	hrx;							//handle for rx list
	RECT	rc;
	char	b[80];

   	hrx=GetDlgItem(hStatus,IDC_RXLIST);		//get handle for rx list (topmost one)
   	GetWindowRect(hrx,&rc);					//get it's position on the screen
   	ULtxrx.x=rc.left;						//make ULtxrx be the upper left of it in screen units
   	ULtxrx.y=rc.top;
   	ScreenToClient(hStatus,&ULtxrx);		//convert to client coordinates within status window
   	
   	ClearItems(hStatus,IDC_RXLIST);			//clear out status lists
//   	ClearItems(hStatus,IDC_LOG);												***210
   	ClearItems(hStatus,IDC_TXLIST);
   	ShowPacketWindows();					//resize packet windows accordingly
   	strcpy(b,version);
   	strcat(b," Restarted");
   	MakeLog(b,TRUE);
//	ShowWindow(hStatus,(MinimizeStatus?SW_SHOWMINIMIZED:SW_SHOWNORMAL));	//	***017	***211 Begin
	ShowWindow(hStatus,(MinimizeStatus?SW_HIDE:SW_SHOWNORMAL));
	CreateTrayIcon();						//make a systray entry for us				***211 End
}

///////////////////////////////////////////////////////////////////////
//	Timer Event Handler (every 100ms)

void TimerEvent()
{	static int	InMainLoop=FALSE;			//lock flag
	static int	SecondsCounter=10;
	int			i;
	char		b[128];						//								***100
	MACStatTable stats;						//								***100

	if ((SecondsCounter--)==0)				//a second has gone by
	{ SecondsCounter=10;					//reset downcounter
	  if (IAmCounter>0) IAmCounter--;		//update downcounters
	  if (SynchCounter>0) SynchCounter--;
	  if (WhoIsCounter>0) WhoIsCounter--;
  	}
  	if (showstats)							//										***100 Begin
  	{   MACPgetstats(&stats);						//get stats
		sprintf(b,"MAC TX=%lu RX=%lu TO=%lu ERROR=%u",stats.MTotalTxPackets,stats.MTotalRxPackets,stats.MTimeouts,stats.MErrors);		//***110***120
		SetWindowText(GetDlgItem(hStatus,IDC_MACSTATS),b); //show our stats
		sprintf(b,"NOMA=%lu  MAUSED=%u MAXUSED=%u",stats.MNoMAs,maused,maxmaused);
		SetWindowText(GetDlgItem(hStatus,IDC_CACSTATS),b); //show our stats
 	}										//										***100 End
	cidcount=0;														//				***100
	sidcount=0;														//				***100
	for (i=0;i<=255;i++)
  	{	if ((cid[i].cidState != stateFree) && (cid[i].cidTimer>0))
  			cid[i].cidTimer--;
		if (cid[i].cidState!=stateFree) cidcount++;					//				***100
	}
  	for (i=0;i<maxserverids;i++)
  	{	if ((sid[i].sidState != stateFree) && (sid[i].sidTimer>0))
  			sid[i].sidTimer--;
		if (sid[i].sidState!=stateFree) sidcount++;					//				***100
	}
    itoa(cidcount,b,10);							//								***100 Begin
 	SetWindowText(GetDlgItem(hStatus,IDC_CIDS),b);
    itoa(sidcount,b,10);
 	SetWindowText(GetDlgItem(hStatus,IDC_SIDS),b);	//								***100 End

    if (InMainLoop) return;					//don't reenter this part yet
    InMainLoop=TRUE;						//lock out reentrancy
    MainLoop();								//perform main loop functions
    InMainLoop=FALSE;						//release reentrancy lock
}

///////////////////////////////////////////////////////////////////////
//	Main Loop (every 100ms)

void MainLoop()
{	NALSpacket	*crip;
	MApacket	*tp;						//											***212
	char b[80];
	int	i,j;

	ShowIdle(FALSE);
//	if (MACPreceive(&mainmap))				//we got something from the MAC layer		***131
	while (MACPreceive(&mainmap))			//											***131
		ReceiveIndicate(&mainmap);

//																		//				***210 Begin
   	if ((IAmCounter==0)&&(WhoIsIAmInterval!=0))
	{	for (i=0;i<boundadapters;i++)
   		{	if (Adapter[i].hFile!=INVALID_HANDLE_VALUE)
   				NALSAPItransmitIam((octet)i,BroadcastDNET,NULL,0);
		}
		if(ptpinitialized)					//											***212
			NALSAPItransmitIam((octet)ptpport,BroadcastDNET,NULL,0);	//				***212
		if(mstpinitialized)					//											***217
			NALSAPItransmitIam((octet)mstpport,BroadcastDNET,NULL,0);	//				***217
	 	IAmCounter=WhoIsIAmInterval*60;
	}
   	if ((WhoIsCounter==0)&&(WhoIsIAmInterval!=0))
	{	for (i=0;i<boundadapters;i++)
    	{	if (Adapter[i].hFile!=INVALID_HANDLE_VALUE)
				NALSAPItransmitWhois((octet)i,BroadcastDNET,NULL,0,FALSE,0,0);	//broadcast Whois for this logical port, global network ***015***022
		}
		if(ptpinitialized)					//											***212
			NALSAPItransmitWhois((octet)ptpport,BroadcastDNET,NULL,0,false,0,0); //		***212
		if(mstpinitialized)					//											***217
			NALSAPItransmitWhois((octet)mstpport,BroadcastDNET,NULL,0,false,0,0); //	***217
 		WhoIsCounter=WhoIsIAmInterval*60;
	}
	if ((SynchCounter==0)&&(TimeSynchInterval!=0))
	{	for (i=0;i<boundadapters;i++)
     	{	if (Adapter[i].hFile!=INVALID_HANDLE_VALUE)
				NALSAPItransmitTimeSynch((octet)i,BroadcastDNET,NULL,0);	//broadcast Time Sync for this logical port, global network***022
		}
		if(ptpinitialized)					//											***212
			NALSAPItransmitTimeSynch((octet)ptpport,BroadcastDNET,NULL,0);	//			***212
		if(mstpinitialized)					//											***217
			NALSAPItransmitTimeSynch((octet)mstpport,BroadcastDNET,NULL,0);	//			***217
		SynchCounter=TimeSynchInterval*60;
   	}
//																						***210 End
	for (i=0;i<=255;i++)								//Check Client timers for expiration
	{
		if ((cid[i].cidTimer==0) && (cid[i].cidState!=stateFree))
		{	RxTO++;
			switch (cid[i].cidState)
			{	
			case stateSegmentedRequest:					//Our Client is sending a segmented confirmed request
				if (--cid[i].cidTries<=0)
					goto mainclientto;
				tp = (MApacket *) cid[i].cidTxPacket;	//the packet we'll transmit
				cid[i].cidMore = TRUE;					//to insure we send another
				if (j=FillWindow(i,CLIENT))
					goto mainclientto;
				cid[i].cidTimer=Tseg;
				break;
			case stateAwaitConf:						//Our Client has sent an unsegmented confirmed request and is awaiting confirmation
				if (--cid[i].cidTries<=0)
					goto mainclientto;
				tp = (MApacket *) cid[i].cidTxPacket;	//the packet we'll transmit
				tp->maTrace=TxCounter++;
				if (j=DoTransmit(tp))
				{	sprintf(b,"NPDU Transmit Failed:%d",j);
					MakeLog(b,FALSE);
					goto mainclientto;
				}
				else
				{
					ShowAsHex(tp,IDC_TXLIST);			//display this packet in the transmit list
					cid[i].cidTimer=Tout;
				}
				break;
			case stateSegmentedConf:					//Our Client is receiving a segmented complex ack
				GFREE(cid[i].cidAPDUData);				//We were building an unsegmented cack
				cid[i].cidAPDUData=NULL;
mainclientto:	GFREE(cid[i].cidAPDUPacket);
				cid[i].cidAPDUPacket=NULL;
				GFREE(cid[i].cidTxPacket);
				cid[i].cidTxPacket=NULL;
				if ((crip=GMALLOC(sizeof(NALSpacket)))!=NULL)
				{	crip->nalsNext=NULL;
					crip->nalsExpectReply=0;
					crip->nalsInvokeID=(octet) i;
					crip->nalsRequestorisserver=cid[i].cidRequestorisserver;
					crip->nalsRxFlag=NALSE_Timeout;
					crip->nalsPri=0;
					crip->nalsLport=cid[i].cidLport;
					crip->nalsSnet=0;
					crip->nalsSlen=0;
					crip->nalsDnet=0;
					crip->nalsDlen=0;
					crip->nalsAlen=0;
					crip->nalsAPDU=NULL;
					if (ALQrear==NULL)				//first guy to be queued
						ALQfront=crip;
					else
						ALQrear->nalsNext=crip;
					ALQrear=crip;
				}
				cid[i].cidState=stateFree;
				break;
				
			default:
				cid[i].cidState=stateFree;			//let's make sure..this should never happen
				cid[i].cidTimer=0;
			}
		}
	}

	for (i=0;i<maxserverids;i++)					//Check Server timers for expiration 		***040
	{
		if ((sid[i].sidTimer==0) && (sid[i].sidState!=stateFree))
		{
			RxTO++;									//											***100
			switch (sid[i].sidState)
			{	
			case stateSegmentedResponse:			//Our Server is sending a segmented complex ACK
				sid[i].sidTries--;
				if (sid[i].sidTries>0)
				{	sid[i].sidMore=TRUE;			//to insure we send one more
					if (j=FillWindow(i,SERVER))
						goto mainserverto;
					sid[i].sidTimer=Tseg;
 				}
				else
					goto mainserverto;
				break;
			case stateSegmentedRequest:				//Our Server is receiving a segmented confirmed request
				GFREE(sid[i].sidAPDUData);			//Free the memory we're using 
				sid[i].sidAPDUData=NULL;
mainserverto:	GFREE(sid[i].sidAPDUPacket);
				sid[i].sidAPDUPacket=NULL;
				GFREE(sid[i].sidTxPacket);
				sid[i].sidTxPacket=NULL;
				sid[i].sidState=stateFree;
				break;

			case stateAwaitResponse:			//Our Server is waiting for a response (to a confirmed request) from A.L.
				if ((tp = GMALLOC(sizeof (MApacket)))!=NULL)				//allocate space for MA packet for transmitting
				{  
		            tp->maLport=sid[i].sidLport;
		            tp->maAlen=sid[i].sidSlen;
		            tp->maLsap=BACnetLSAP;
		            tp->maBroadcast=0;
					memcpy(&tp->maDadr,&sid[i].sidSadr,sid[i].sidSlen);	//source becomes destination for transmit
					// handle routing here!?
					SendAbort (tp,sid[i].sidInvokeID,ABORT_Other,SERVER);
					GFREE(tp);												//the transmit packet we got
//					sid[i].sidState=stateFree;							//							***213
				}
				sid[i].sidState=stateFree;								//							***213
				break;
				
				
			default:
				sid[i].sidState=stateFree;		//let's make sure..this should never happen
				sid[i].sidTimer=0;
			}
		}
	}

    if (PrefsDirty) SavePreferences();			//save them as they get sullied
	ShowIdle(TRUE);
}

///////////////////////////////////////////////////////////////////////
//	Generate a Test Packet (WhoIS)

void TransmitTest()
{	int		i;

	for (i=0;i<boundadapters;i++)				//									***210 Begin
   	{	if (Adapter[i].hFile!=INVALID_HANDLE_VALUE)							//This should never happen
 	 	{	NALSAPItransmitWhois((octet)i,BroadcastDNET,NULL,0,FALSE,0,0);	//broadcast Whois for this logical port, global network ***015***022
		}
	}											//									***210 End
	if(ptpinitialized)							//									***212
		NALSAPItransmitWhois((octet)ptpport,BroadcastDNET,NULL,0,false,0,0); //		***212
	if(mstpinitialized)							//									***217
		NALSAPItransmitWhois((octet)mstpport,BroadcastDNET,NULL,0,false,0,0); //	***217
}

///////////////////////////////////////////////////////////////////////
//	Display Received Packets in HEX format
//in:	mp			points to packet
//		lst			the ID of a list to display it in

void ShowAsHex(MApacket *mp,UINT lst)
{	char	b[80],*p;
	word	i,j,k,m,*wp;
	octet	*pi,*sp;							//							***018
	NPCI	*np;
		
	while (GetListCount(lst)>=maxlistitems)		//limit list to maxlistitems
		RemoveItem(lst,0);						//remove oldest list items
		
	if (mp->maLport==ptpport)					//								***217 Begin
		sprintf(b,"[#%lu p:%u(PTP) L:%u] D:",mp->maTrace,mp->maLport,mp->maNlen);
	else if (mp->maLport==mstpport)
		sprintf(b,"[#%lu p:%u(MSTP) L:%u] D:",mp->maTrace,mp->maLport,mp->maNlen);
	else										//								***217 End
		sprintf(b,"[#%lu p:%u L:%u] D:",mp->maTrace,mp->maLport,mp->maNlen);	//***003

	p=strchr(b,0);							//point to the asciz
	for (i=0;i<mp->maAlen;i++)
	{	sprintf(p,"%.2X",mp->maDadr[i]);
		p+=2;
	}
	strcat(p," S:");
	p+=3;
	for (i=0;i<mp->maAlen;i++)
	{	sprintf(p,"%.2X",mp->maSadr[i]);
		p+=2;
	}
	*p=0;
   	EnterItem(lst,b);							//first item is selected when added to list
	j=0;
	if (IsCheckedMenuItem(MENU_VIEW,ID_VIEW_NL)) //Interpret Network Layer?
	{	if (mp->maNlen<Min_NPDU_Length) goto sahrest; //can't interpret these reliably
		np=(NPCI *)&mp->maNPDU;					//get pointer to the NPDU
		if (np->npciVersion!=1) goto sahrest;	//don't know about this version
		j=2;									//account for version and control octets
		b[0]=0;
		p=&b[0];
		if (np->npciControl&NSDUhasDNET)
		{	if (np->npciDNET==0xFFFF)
				sprintf(b,"DNET: broadcast, DA:");
			else
				sprintf(b,"DNET:%u, DA:",wbige(np->npciDNET));	//			***003
			p=strchr(b,0);						//point to asciz
			if (np->npciDLEN)					//format dest addr			***003 Begin
			{	for (i=0;i<np->npciDLEN;i++)
				{	sprintf(p,"%.2X ",np->npciDADR[i]);
					p+=3;
				}
			}
			else
			{	strcpy(p,"broadcast ");
				p=strchr(p,0);				//							***003 End 
			}
			sp=(octet *)&np->npciDADR;			//							***018
			sp+=np->npciDLEN; 					//point to SNET				***018
			j+=(3+np->npciDLEN);
		}
		else									//no DNET, use defaults
			sp=(octet *)&np->npciDNET;			//							***018 Begin
			
		if (np->npciControl&NSDUhasSNET)
		{	EnterItem(lst,b);
			sprintf(b,"SNET:%u, SA:",wbige(((NSRC *)sp)->SNET));
			p=strchr(b,0);						//point to asciz
			for (i=0;i<((NSRC *)sp)->SLEN;i++)
			{	sprintf(p,"%.2X ",((NSRC *)sp)->SADR[i]);
				p+=3;
			}
			j+=(3+((NSRC *)sp)->SLEN);			//account for SNET, SLEN and SADR octets used
			sp+=3+((NSRC *)sp)->SLEN;				//							***003
		}
		if (np->npciControl&NSDUhasDNET)
		{	sprintf(p,"HOP:%u",*sp++);			//format hopcount also		***018 End
			j++;
		}
//Here, sp points past the DNET/DLEN/DADR/SNET/SLEN/SADR/HOPCOUNT as appropriate
		if (j!=2)
			EnterItem(lst,b);					//display first NL line
		sprintf(b,"Pri:%u, Reply:%s",(np->npciControl&NSDUpriority),(np->npciControl&NSDUexpectsreply)?"Yes":"No");
		EnterItem(lst,b);						//display second NL line
		if (np->npciControl&NSDUisNLmessage)	//this is an NL message
		{	wp=(word *)(sp+1);					//many types use words following the message type ***018
			if (*sp<NumNLMessages)
			{	strcpy(b,NLMessages[*sp]);	//first copy description of the message type	***018 
				p=strchr(b,0);
				j++;							//account for message type octet
				switch(*sp)						//							***018
				{
				case Who_Is_Router_To_Network:
					if (mp->maNlen>=4)			//we have a DNET argument
					{	sprintf(p," %u",wpbige(wp));	//					***003
						j+=2;					//account for DNET
					}
					EnterItem(lst,b);			//add list line
					break;
				case I_Am_Router_To_Network:
				case Router_Busy_To_Network:
				case Router_Available_To_Network:
					for (i=(mp->maNlen-3)/2;i>0;i--)	//					***038 Begin
					{	if (strlen(b)>(TextWidth-7))
						{	EnterItem(lst,b);
							b[0]=0;
						}
						p=strchr(b,0);					//find asciz
						if (b[0]) *p++=',';
						sprintf(p,"%u",wpbige(wp++));	//					***003
						j+=2;							//account for DNET
					}
					if (b[0]) EnterItem(lst,b);	//add list line				***038 End
					break;
				case I_Could_Be_Router_To_Network:
					sprintf(p," %u, perf:%u",wpbige(wp),sp[3]); //			***018
					EnterItem(lst,b);			//add list line
					j+=3;						//account for DNET and performance index
					break;
				case Reject_Message_To_Network:
					wp=(word *)(sp+2);			//							***018
					sprintf(p," %u, reason:%u",wpbige(wp),sp[1]); //		***018
					EnterItem(lst,b);			//add list line
					j+=3;						//account for DNET and reject reason
					break;
				case Initialize_Routing_Table:
				case Initialize_Routing_Table_Ack:
					k=sp[1];					//number of ports			***018
					sprintf(p,", ports:%u",k);
					EnterItem(lst,b);
					j++;						//account for number of ports
					pi=sp+2;
					for (i=0;i<k;i++)
					{	wp=(word *)pi;
						sprintf(b,"DNET:%u, ID:%u, Info:",wpbige(wp),pi[2]); //***003
						for (m=0;m<pi[3];m++)
						{	p=strchr(b,0);
							sprintf(p," %.2X",pi[4+m]);
						}
						EnterItem(lst,b);
						j+=(4+pi[3]);
						pi+=(4+pi[3]);			//skip to next port
					}
					break;
				case Establish_Connection_To_Network:
					sprintf(p," %u, time:%u",wpbige(wp),sp[3]); //			***018
					EnterItem(lst,b);			//add list line
					j+=3;						//account for DNET and termination time
					break;
				case Disconnect_Connection_To_Network:
					sprintf(p," %u",wpbige(wp));	//						***003
					EnterItem(lst,b);			//add list line
					j+=2;						//account for DNET
					break;
				}
			}
			else if (*sp>0x7F) 					//proprietary message type	***018 Begin
			{	sprintf(b,"Vendor[%u] Proprietary NL Message %u",
							wpbige(wp),			//							***003
							*sp);
				EnterItem(lst,b);
			}
			else								//some message we don't know about
			{   sprintf(b,"Unknown NL Message Type (%u)",*sp);
				EnterItem(lst,b);
			}									//							***018 End
		}
	}
//display the rest of the packet in hex
sahrest:
	b[0]=0;
	i=0;
	p=&b[0];
	while (j<mp->maNlen)
	{	sprintf(p,"%.2X ",mp->maNPDU[j++]);
		p[3]=0;
		p+=3;
		if ((i++)==16)
		{	AddItem(hStatus,lst,b);
			p=&b[0];
			i=0;
		}
	}
	if (i) AddItem(hStatus,lst,b);
	AddItem(hStatus,lst,"----------------------------------------------------------------------------------");
}


///////////////////////////////////////////////////////////////////////		//		***100 Begin
//	Processor for Received Packets from MACAPI
/*
	if npci<7>==1 then goto NLmessage			//NL message
	if npci<5>==1 then
	  if DNET!=FFFF then goto NLmessage			//needs routing
	  local AL needs to see this broadcast
 */////////////////////////////////////////////////////////////////////
void ReceiveIndicate(MApacket *mp)
{	NPCI		*np;
	NALSpacket	rip,*crip;
	NSRC		*sp;
	ASDU		*ap;
	int			pdutype,i;
	BOOL		isserver;

	mp->maTrace=(++RxCounter);					//in case we keep it					***100
	ShowAsHex(mp,IDC_RXLIST);					//display this packet in the receive list
	if (mp->maNlen<Min_NPDU_Length)
	{	LogNPDUerror(mp,"Invalid Length");
		RxCounter--;							//										***100
		RxError++;								//										***100	
		goto riexit;
	}
	np=(NPCI *)&mp->maNPDU;						//get pointer to the NPDU
	if (np->npciVersion!=1)
	{	LogNPDUerror(mp,"Unsupported Version");
		RxCounter--;							//										***100
		RxError++;								//										***100	
		goto riexit;
	}
//																						***036 Begin
	rip.nalsLport=mp->maLport;
	rip.nalsAlen=mp->maNlen;					//for now length of entire NPDU
	rip.nalsRlen=0;								//initialize							***147
	if (np->npciControl&NSDUhasDNET)
	{	rip.nalsDnet=wbige(np->npciDNET);		//save destination stuff
		rip.nalsDlen=np->npciDLEN;				//how do we figure out the broadcast DADR here?
		if (rip.nalsDlen!=0)					//										***148
			memcpy(rip.nalsDadr,np->npciDADR,rip.nalsDlen);	//								
		rip.nalsRlen=np->npciDLEN;
		if (rip.nalsRlen!=0)					
			memcpy(rip.nalsRadr,np->npciDADR,rip.nalsRlen);	
		sp=(NSRC *)&np->npciDADR;
		(octet *)sp=(octet *)sp+rip.nalsDlen; 	//point to SNET
		if (mp->maBroadcast)					//										***148 Begin
		{	rip.nalsRlen=0;
			rip.nalsBroadcast=TRUE;
		}
		else 
		{	rip.nalsRlen=mp->maAlen;
			memcpy(rip.nalsRadr,mp->maDadr,sizeof(rip.nalsRadr));	//need to return the dest addr
		    rip.nalsBroadcast=FALSE;
		}										//										***148 End
	}
	else										//no DNET, use defaults
	{	rip.nalsDnet=0;
		if (mp->maBroadcast)
		{	rip.nalsDlen=0;
			rip.nalsBroadcast=TRUE;
		}
		else 
		{	rip.nalsDlen=mp->maAlen;
			memcpy(rip.nalsDadr,mp->maDadr,sizeof(rip.nalsDadr));	//need to return the dest addr
		    rip.nalsBroadcast=FALSE;
		}
		sp=(NSRC *)&np->npciDNET;
	}
	if (np->npciControl&NSDUhasSNET)
	{	rip.nalsSnet=wbige(sp->SNET);			//save source stuff
		rip.nalsSlen=sp->SLEN;
		memcpy(rip.nalsSadr,sp->SADR,rip.nalsSlen);
		(octet *)sp=(octet *)sp+rip.nalsSlen+3;
		memcpy(rip.nalsRadr,mp->maSadr,sizeof(rip.nalsRadr));	//router's address		***214
		rip.nalsRlen=mp->maAlen;								//						***214
	}
	else										//no SNET, use defaults
	{	rip.nalsSnet=0;
		rip.nalsSlen=mp->maAlen;
		memcpy(rip.nalsSadr,mp->maSadr,sizeof(rip.nalsSadr));
	}

	if ((np->npciControl&NSDUisNLmessage)==0)	//this is an APDU						***037 Begin
	{	rip.nalsExpectReply=(np->npciControl&NSDUexpectsreply)?TRUE:FALSE;
		rip.nalsPri=(np->npciControl&NSDUpriority);
		if ((rip.nalsDlen==0)||(IsForUs(rip.nalsLport,&rip.nalsDadr[0])))	//either a broadcast or for us
			rip.nalsRxType=1;					//it's an APDU for the local station or broadcast
		else
			rip.nalsRxType=2;					//it's an APDU not for the local station and not broadcast
			
		rip.nalsAlen-=2;						//minus version and control octets of NPDU
		if (np->npciControl&NSDUhasDNET)
		{	rip.nalsAlen-=(4+rip.nalsDlen);		//account for DNET, DLEN, DADR and HOPCOUNT octets used
			(octet *)sp=(octet *)sp+1;			//skip over hopcount
		}
		if (np->npciControl&NSDUhasSNET)
			rip.nalsAlen-=(3+rip.nalsSlen);		//account for SNET, SLEN and SADR octets used
		if (np->npciControl&NSDUhasDNET 		//this PDU has a DNET
			&&									//and
			np->npciDNET!=BroadcastDNET			//it's not the broadcast DNET			***212 Begin
			&&									//and
			rip.nalsLport!=ptpport)				//it's not the ptpport					***212 End
			{	rip.nalsRxType=2;				//it's an APDU not for the local station and not broadcast
				goto riNLcheck;					//skip AL checks
			}
//here, its either a local APDU, or a broadcast APDU so we will want AL processing		***037 End
//here, sp points to the beginning of the apdu
		if ((rip.nalsAPDU=GMALLOC(rip.nalsAlen))==NULL)	
		{	LogNPDUerror(mp,"Can't get free space for APDU");
			RxCounter--;							//									***100
			RxError++;								//									***100	
			goto riexit;
		}
		memcpy(rip.nalsAPDU,sp,rip.nalsAlen);	//copy the apdu data bytes
		ap=(ASDU *)rip.nalsAPDU;				//point to APDU header
		pdutype=((ap->asduPDUheader&ASDUpdutype)>>ASDUpdutypeshr); //get the pdu type field

/******************************************
	New code for segmentation starts here!!
******************************************/
//									mp points at incoming MApacket
//									np points at NPDU in mp
//									sp points at APDU in mp
//									rip is a local NALSpacket
//									ap points to allocated APDU (rip.nalsAPDU)

		switch (pdutype)
		{
		case UNCONF_REQ_PDU:
		case CONF_REQ_PDU:
			if (rip.nalsRxType==1)		//it's an APDU for the local station or broadcast
				if (passive==0)			//we're not passive							***216
					if (!ReceiveIndicateServer(pdutype,mp,&rip)) goto riexit;
			isserver=TRUE;
			goto riReturnPacketToApp;

		case SIMPLE_ACK_PDU:
		case COMPLEX_ACK_PDU:
			if (rip.nalsRxType==1)		//it's an APDU for the local station or broadcast
				if (!ReceiveIndicateClient(pdutype,mp,&rip,&isserver)) goto riexit;
			goto riReturnPacketToApp;

		case SEGMENT_ACK_PDU:
		case ABORT_PDU:
		//Either our Client or Server Function must process this packet.
		//We must decide based on the the status of the server bit in the
		//pdu header of the received packet.
			 
			if (ap->asduPDUheader&ASDUserver)
//it was from a server and we're acting as a client
			{	if (rip.nalsRxType==1)		//it's an APDU for the local station or broadcast
					if (!ReceiveIndicateClient(pdutype,mp,&rip,&isserver)) goto riexit;
				goto riReturnPacketToApp;
			}

												//it was from a client and we're acting as a server--
												//see if we can find an entry in sid that corresponds to this
												//source address and invoke ID. If yes, then this is a
												//response to a server packet and must be managed by our
												//server manager. Otherwise toss the packet.
			if (rip.nalsRxType==1)		//it's an APDU for the local station or broadcast
				if (passive!=0)			//										***216
					if (!ReceiveIndicateServer(pdutype,mp,&rip)) goto riexit;
			rip.nalsRequestorisserver=TRUE;
			isserver=TRUE;
			goto riReturnPacketToApp;

		case REJECT_PDU:
		case ERROR_PDU:
		//Either our Client or Server Function must process this packet.
		//We must decide by searching cid and sid for a match between invoke
		//id and the source address of the packet.
			                                                                 
			i=ap->asduInvokeID;
//																									***009 Begin
			if ((cid[i].cidState!=stateFree)&&(IsSource(i, (octet)i, &rip, CLIENT)))
//it was from a server and we're acting as a client
			if (rip.nalsRxType==1)		//it's an APDU for the local station or broadcast
				if (!ReceiveIndicateClient(pdutype,mp,&rip,&isserver)) goto riexit;
			goto riReturnPacketToApp;
			
												//it may have been from a client and we're acting as a server--
												//see if we can find an entry in sid that corresponds to this
												//source address and invoke ID. If yes, then this is a
												//response to a server packet and must be managed by our
												//server manager. Otherwise toss the packet.
			if (rip.nalsRxType==1)		//it's an APDU for the local station or broadcast
				if (passive!=0)			//										***216
					if (!ReceiveIndicateServer(pdutype,mp,&rip)) goto riexit;
			rip.nalsRequestorisserver=TRUE;
			isserver=TRUE;
			goto riReturnPacketToApp;

		default:
			GFREE(rip.nalsAPDU);
			goto riexit;						//toss packet
		}		
	}
	else										//this is an NL message
	{	sp=(NSRC *)&np->npciVersion;			//always point to NPCI for NL messages				***037
		rip.nalsRxType=0;						//it's an NL packet									***037

	//here, it's either an NPDU, or an APDU with a non-Broadcast DNET								***036 Begin
	//in either case, pass it onto an NL server if it's registered and also needed...
riNLcheck:
		if ((rip.nalsAPDU=GMALLOC(rip.nalsAlen))==NULL)	
		{	LogNPDUerror(mp,"Can't get free space for APDU");
			goto riexit;
		}
		memcpy(rip.nalsAPDU,sp,rip.nalsAlen);	//copy the apdu data bytes
//																									***036 End
		if (NLserver!=NULL)						//is somebody registered to deal with these?
			(*NLserver)(mp);					//issue callback (NOTE:This is promiscuous)			***022
		else									//no NL server available
		{	if (!MadeNLComplaint)
			{	LogNPDUerror(mp,"No NL Server Registered");
				MadeNLComplaint=TRUE;
			}
		}
//																									***036 Begin
		if ((crip=GMALLOC(sizeof(NALSpacket)))==NULL)		//put it into the application queue
		{	LogNPDUerror(mp,"Can't get free space for NPDU");
			GFREE(rip.nalsAPDU);						//have to give this up first
			goto riexit;
		}
		memcpy(crip,&rip,sizeof(NALSpacket));			//Note: we intentionally copy rip.nalsAPDU here!
		rip.nalsAPDU=NULL;
		if (ALQrear==NULL)								//first guy to be queued
			ALQfront=crip;
		else
			ALQrear->nalsNext=crip;
		ALQrear=crip;
		crip->nalsNext=NULL;
//																									***036 End
		goto riexit;		
	}


	//here, we have an APDU that has to be returned to an Application 
	//Entity either by placing it in ALQ or calling an application callback

riReturnPacketToApp:
	if (receivefilter)								//only want broadcasts and responses to things client transmitted
	{	if ((mp->maBroadcast!=0) &&	isserver)		//wasn't from the client originally
			goto riCheckServer;						//filter out these guys
	}

	if ((crip=GMALLOC(sizeof(NALSpacket)))==NULL)
	{	LogNPDUerror(mp,"Can't get free space for NPDU");
		GFREE(rip.nalsAPDU);						//have to give this up first
		goto riexit;
	}
	memcpy(crip,&rip,sizeof(NALSpacket));			//Note: we intentionally copy rip.nalsAPDU here!
	rip.nalsAPDU=NULL;
	mydisable;										//									***142
	if (ALQrear==NULL)								//first guy to be queued
		ALQfront=crip;
	else
		ALQrear->nalsNext=crip;
	ALQrear=crip;
	crip->nalsNext=NULL;
	myenable;										//									***142

riCheckServer:
	if ((pdutype==CONF_REQ_PDU ||					//if it's a request, or 
	     pdutype==UNCONF_REQ_PDU ||
		 isserver)&&								//this one was from the server originally	***022 Begin
		 (mp->maBroadcast!=0)||						//and it's either a brodcast				***024
		 (IsForUs(mp->maLport,&mp->maDadr[0])))		//or for us									***022 End***026
		 
	{	if (ALserver!=NULL)							//and somebody is registered to deal with these
		{	if (rip.nalsAPDU==NULL)
			{	if ((rip.nalsAPDU=GMALLOC(rip.nalsAlen))==NULL)
				{	LogNPDUerror(mp,"Can't get free space for APDU");
					goto riexit;
				}
				 memcpy(rip.nalsAPDU,sp,rip.nalsAlen);	//copy the apdu data bytes
			}
			(*ALserver)(&rip);						//issue callback
		}
		else										//no AL server registered
		{	if (!MadeALComplaint)
			{	LogNPDUerror(mp,"No AL Server Registered");
				MadeALComplaint=TRUE;
				goto riexit;
			}
		}
	}
riexit: ;
}

////////////////////////////////////////////////////////////////
BOOL ReceiveIndicateServer (int pdutype,MApacket *mp,NALSpacket *np)
// Process Requests/Responses to our Server (eg. confirmed/unconfirmed requests)
//
// in:	pdutype		the type of pdu received
//		mp			pointer to an MApacket
//		np			pointer to a NALSpacket
//
// out:	TRUE		pass it on to the server
//		FALSE		toss it or it's been handled

{	int			i,j,e;
	ASDUREQ		*apr;
	MApacket	*tp;
	ASDU		*ap;
	octet		*rap;							//for doing mem copies
	octet		*srap;							//for doing mem copies

	ap=(ASDU *)np->nalsAPDU;					//point to APDU header
	switch (pdutype)
	{
	case UNCONF_REQ_PDU:
		HandleWhoisIam(np);										//we handle who is and I am's		***009
		np->nalsRxFlag=NALSE_Success;
		np->nalsRequestorisserver=TRUE;
		return TRUE;
	
	case CONF_REQ_PDU:
		apr=(ASDUREQ *) ap;
		if (((ap->asduPDUheader&ASDUissegmented)!=0) && (apr->asdureqSequenceNumber!=0))
//it's segmented and sequence!=0
			goto riconfreqRXseg;										
		else 
//it's unsegmented or sequence==0
		{   i=-1;													//i will indicate first free slot in sid
			for (j=0;j<maxserverids;j++)							//use j since invoke id != index into sid
			{	if((i==-1) && (sid[j].sidState==stateFree))
					i=j;											//first free was found
				if ((sid[j].sidState!=stateFree)&&(IsSource(j, apr->asdureqInvokeID, np, SERVER))) //***009
					goto risexit;								//this must be a duplicate request, just toss it!
			}
			if ((tp = GMALLOC(sizeof (MApacket)))==NULL)			//allocate space for MA packet for transmitting
			{	LogNPDUerror(mp,"Can't get free space for Transmit Packet");
				goto risexit;
			}
	        tp->maLport=mp->maLport;
	        tp->maAlen=mp->maAlen;
	        tp->maLsap=mp->maLsap;
	        tp->maBroadcast=mp->maBroadcast;
			memcpy(&tp->maDadr,&mp->maSadr,mp->maAlen);				//source becomes destination for transmit

			if (i==-1)												//no free slots in sid are available
			{	LogNPDUerror(mp,"Can't get server invoke ID for NPDU");
rinofreespace:		//send ABORT
				SendAbort (tp,apr->asdureqInvokeID,ABORT_BufferOverflow,SERVER);
				GFREE(tp);
				goto risexit;
			}
		}
//it's unsegmented or sequence==0 and we just found a new sid
		sid[i].sidLport=mp->maLport;
		sid[i].sidSlen=mp->maAlen;
		memcpy(sid[i].sidSadr,mp->maSadr,mp->maAlen);
		sid[i].sidSnet=np->nalsSnet;
		sid[i].sidMaxResponse=apr->asdureqMaxResponse&ASDUMaxResp;
		sid[i].sidSegmentAccepted=(apr->asdureqPDUheader&ASDUsegmentrespaccepted)?TRUE:FALSE;
			
		if ((ap->asduPDUheader&ASDUissegmented)==0)
//it's unsegmented
		{	if (IsForUs(np->nalsLport,np->nalsDadr)) //for us?							***025 Begin***026
			{	sid[i].sidInvokeID=apr->asdureqInvokeID;
				sid[i].sidState=stateAwaitResponse;		//this is our next state
				sid[i].sidTimer=Tout;
			}
			else sid[i].sidState=stateFree;				//don't expect response from server	***025 End

			np->nalsInvokeID=ap->asduInvokeID;				
			GFREE(tp);
			np->nalsRxFlag=NALSE_Success;
			np->nalsRequestorisserver=TRUE;
			return TRUE;
		}

//else it's segmented
riconfreqRXseg:
		if ((apr->asdureqSequenceNumber)==0)
//new seqmented request i points to the sid to use, tp points to an mapacket to use
		{   if (SegmentedRX==0)											//Segmented RX is not supported!
			{	//send ABORT
				SendAbort (tp,apr->asdureqInvokeID,ABORT_SegmentationNotSupported,SERVER);
				GFREE(tp);
				goto risexit;
			}
			if ((sid[i].sidAPDUData=GMALLOC(MaxSizeALapdu))==NULL)		//Get space to accumulate full, unsegmented request
			{	LogNPDUerror(mp,"Can't get free space for APDU");
				goto rinofreespace;
			}
	        sid[i].sidTxPacket=(octet *) tp;							//the packet we'll transmit (in case we have to transmit)
			sid[i].sidState=stateSegmentedRequest;
			srap=(octet *)sid[i].sidAPDUData;
			*srap++=(octet)apr->asdureqPDUheader&ASDUpdutype;			//application will not know request is segmented
			*srap++=(octet)apr->asdureqMaxResponse;
			*srap++=(octet)apr->asdureqInvokeID;
			*srap++=(octet)apr->asdureqServiceChoice;					//			***031
			sid[i].sidInvokeID=apr->asdureqInvokeID;
			sid[i].sidSequence=0;
            sid[i].sidMaxLen=MACPmaxNPDUlength (mp->maLport);			//do we need this ??
	        sid[i].sidOctetCount=4;										//we've processed 4 octets so far ***031
//																		//		***147 Begin
//	        sid[i].sidWindowSize=sid[i].sidActualWindowSize=apr->asdureqProposedWindowSize;
	        sid[i].sidWindowSize=1;										//we want to send an ack right away
// BACnet policy is that we get to choose the window size which must be <= proposed. 
			sid[i].sidActualWindowSize=(WindowSize<=apr->asdureqProposedWindowSize)?WindowSize:apr->asdureqProposedWindowSize;
//																		//		***147 End
			goto riServerRX1;											//get APDU data	
		}
		else
//request is in progress, search sid for invoke ID and source
		{	for (i=0;i<maxserverids;i++)							//use i since invoke id != index into sid
			{	if ((sid[i].sidState==stateSegmentedRequest)&&(IsSource(i, apr->asdureqInvokeID, np, SERVER)))
					goto riServerRX1;						//this source and invoke ID have been found
			}
			LogNPDUerror(mp,"Can't find in-progress server invoke id for NPDU");
			//send ABORT
			if ((tp = GMALLOC(sizeof (MApacket)))==NULL)			//allocate space for MA packet for transmitting
			{	LogNPDUerror(mp,"Can't get free space for Transmit Packet");
				goto risexit;
			}
	        tp->maLport=mp->maLport;
	        tp->maAlen=mp->maAlen;
	        tp->maLsap=mp->maLsap;
	        tp->maBroadcast=mp->maBroadcast;
			memcpy(&tp->maDadr,&mp->maSadr,mp->maAlen);			//source becomes destination for transmit
			SendAbort (tp,apr->asdureqInvokeID,ABORT_InvalidAPDUInThisState,SERVER);	//			***035
			GFREE(tp);
			goto risexit;
		}
		case SEGMENT_ACK_PDU:
		case ABORT_PDU:
		case REJECT_PDU:
		case ERROR_PDU:
//it was from a client and we're acting as a server--see if we can find an entry in sid 
//that corresponds to this source address and invoke ID. If yes, then this is a
//response to a server packet and must be managed by our server manager. 
//Otherwise toss the packet.
			for (i=0;i<maxserverids;i++)		//use i since invoke id != index into sid
			{	if ((sid[i].sidState!=stateFree)&&(IsSource(i, ap->asduInvokeID, np, SERVER)))	//***009
					goto riServerRX1;						//this source and invoke ID have been found
			}
			goto risexit;  						//toss packet

	}	//end of switch (pdutype)		

//segmented (don't care if it's the first or not) 
//at this point, i = index into sid for this transaction 
//the packet type can be CONF_REQ_PDU, ABORT_PDU, SEGMENT_ACK_PDU depending of state

riServerRX1:
	tp=(MApacket *)sid[i].sidTxPacket;					//the packet we'll transmit (in case we have to transmit)
														//Note: not valid for stateAwaitResponse
	switch(sid[i].sidState)
	{
	case stateSegmentedRequest:							
//our Server is receiving a segmented Confirmed Request
		switch(pdutype)
		{
		case CONF_REQ_PDU:
			if (sid[i].sidSequence!=apr->asdureqSequenceNumber)
			//send SEQMENTED NACK
			{	if(IsForUs(np->nalsLport,np->nalsDadr))		//										***025***026
					SendSegmentedAck(tp,np,sid[i].sidInvokeID,
									(octet)((sid[i].sidSequence)-1),sid[i].sidActualWindowSize,FALSE,SERVER); //		***030***042
				
				goto risexit;									//toss the packet
			}
			srap=(octet *)sid[i].sidAPDUData;
			srap+=sid[i].sidOctetCount;							//point srap at accumulating APDU data
			if ((sid[i].sidOctetCount+=np->nalsAlen-6)>MaxSizeALapdu)	//								***031
			//send ABORT
			{   SendAbort (tp, sid[i].sidInvokeID, ABORT_BufferOverflow, SERVER);
rifreepkt3:				GFREE(sid[i].sidTxPacket);
				sid[i].sidTxPacket=NULL;
				GFREE(sid[i].sidAPDUData);
				sid[i].sidAPDUData=NULL;
				sid[i].sidState=stateFree;
				GFREE(tp);
				goto risexit;
			}
			rap=(octet *)np->nalsAPDU;
			rap+=6;											//skip confirmed request overhead		***031
			memcpy(srap,rap,(np->nalsAlen-6));				//copy the APDU data					***031
			if (--sid[i].sidWindowSize==0)					//time to acknowledge
			//send SEQMENTED ACK
			{	if(IsForUs(np->nalsLport,np->nalsDadr))		//										***025***026
					SendSegmentedAck (tp,np, sid[i].sidInvokeID, 
									sid[i].sidSequence, sid[i].sidActualWindowSize, TRUE, SERVER); //	***030
				sid[i].sidWindowSize=sid[i].sidActualWindowSize;
			}
			if ((apr->asdureqPDUheader&ASDUmoresegments)!=0)
			//more segments to come
			{	sid[i].sidTimer=Tseg;
				sid[i].sidSequence++;
				goto risexit1;
			}
			else
			//no more segments to come, we've got all the segments
			{	if((sid[i].sidWindowSize!=sid[i].sidActualWindowSize)&&	//we need to send the last ack	***025
				  (IsForUs(np->nalsLport,np->nalsDadr)))			//								***025***026
					SendSegmentedAck(tp,np,sid[i].sidInvokeID,
									sid[i].sidSequence,sid[i].sidActualWindowSize,TRUE,SERVER); //		***030
				GFREE(sid[i].sidTxPacket);
				sid[i].sidTxPacket=NULL;
				GFREE(np->nalsAPDU);							//we must now point np at unsegmented buffer
				np->nalsInvokeID=sid[i].sidInvokeID;
				np->nalsAPDU=sid[i].sidAPDUData;
				np->nalsAlen=(uint) sid[i].sidOctetCount;
				np->nalsRxFlag=NALSE_Success;
				np->nalsRequestorisserver=TRUE;
				if (IsForUs(np->nalsLport,np->nalsDadr))	//									***025 Begin***026
				{	sid[i].sidState=stateAwaitResponse;			//this is our next state
					sid[i].sidTimer=Tout;
				}
				else
					sid[i].sidState=stateFree;					//no response expected from server	***025 End
				
				return TRUE;
			}
		case ABORT_PDU:
			goto rifreepkt3;
		default:												//invalid PDU
			SendAbort(tp,sid[i].sidInvokeID,ABORT_InvalidAPDUInThisState,SERVER);
			goto rifreepkt3;
		}	// end switch (pdutype)
	case stateSegmentedResponse:								
//our Server is sending a segmented Complex Ack
 		switch (pdutype)
 		{
		case SEGMENT_ACK_PDU:
			if (InWindow(ap->asduSequenceNumber,sid[i].sidInitialSequence,i,SERVER))
			{	if(sid[i].sidMore)
				//more segments to send
				{	do
					{   sid[i].sidOctetCount-=sid[i].sidMaxLen-5;		//must account for APCI header	***034
						sid[i].sidAPDUData+=sid[i].sidMaxLen-5;			//								***034
					}
					while (sid[i].sidInitialSequence++!=ap->asduSequenceNumber);
															//at this point OctetCount, APDUData are updated
															//and InitialSequence=asduSequenceNumber+1 (mod 256)
					sid[i].sidTries=Nretry+1;				//									***006
					sid[i].sidActualWindowSize=ap->asduWindowSize;
		 			if (e=FillWindow(i,SERVER))
		 			{	np->nalsRxFlag=NALSE_TransmitError;
						goto rifreepkt2;
					}															
				}
				else
				//no more segments to send
				{	
rifreepkt2:			GFREE(sid[i].sidTxPacket);				//the transmit packet we got
					sid[i].sidTxPacket=NULL;
					GFREE(sid[i].sidAPDUPacket);			//the APDU packet from the A.L.
					sid[i].sidAPDUPacket=NULL;
					sid[i].sidState=stateFree;
				}
			}
			else
			//duplicate ack -- ignore it
			{	sid[i].sidTimer=Tseg;					//restart the timer
				RxToss++;								//							***100
			}
			goto risexit1; 
		case ABORT_PDU:
			goto rifreepkt2;
		default:										//invalid PDU
			SendAbort(tp, sid[i].sidInvokeID,ABORT_InvalidAPDUInThisState,SERVER);
			goto rifreepkt2;
		}	// end switch (pdutype)

	case stateAwaitResponse:
//our server is awaiting a response from the app above it
		switch (pdutype)
		{
		case ABORT_PDU:
			sid[i].sidState=stateFree;
			break;
		default:										//invalid PDU
			SendAbort(tp,sid[i].sidInvokeID,ABORT_InvalidAPDUInThisState,SERVER);
			sid[i].sidTxPacket=NULL;
			sid[i].sidState=stateFree;
			RxToss++;
		}	// end switch (pdutype)
		GFREE(tp);								//the transmit packet we got
		goto risexit1;

 	default:											//should never happen
		RxToss++;
		goto risexit;
	}	//end of switch(sid[i].sidState)
risexit:
	RxToss++;
risexit1:
	np->nalsAPDU=NULL;										//actually..this is not needed!
	GFREE(np->nalsAPDU);
	return FALSE;
}
////////////////////////////////////////////////////////////////
BOOL ReceiveIndicateClient (int pdutype,MApacket *mp,NALSpacket *np,BOOL *isserver)
// Process Responses to our Client (eg. complex/simple acks)
//
// in:	pdutype		the type of pdu received
//		mp			pointer to an MA packet
//		np			pointer to a NALSpacket
//		iserver		place to return indication if response is for a server
//
// out:	TRUE		pass it on to the server
//		FALSE		toss it or it's been handled

{	int			i,e;
	ASDU		*sap;							//for accumulating an entire unsegmented packet
	MApacket	*tp=NULL;						//											***213
	ASDU		*ap;
	octet		*rap;							//for doing mem copies
	octet		*srap;							//for doing mem copies
	BOOL		ismore;							//											***042

	ap=(ASDU *)np->nalsAPDU;					//point to APDU header
	switch (pdutype)
	{
	case SIMPLE_ACK_PDU:
	case COMPLEX_ACK_PDU:
	case SEGMENT_ACK_PDU:
	case ABORT_PDU:
	case REJECT_PDU:
	case ERROR_PDU:
//Our Client Function must process this packet.
		i=ap->asduInvokeID;								//Invoke Id is also the index into cid
		np->nalsInvokeID=i;
        tp = (MApacket *) cid[i].cidTxPacket;			//the packet we'll transmit (in case we have to transmit)
		switch (cid[i].cidState)
		{	
		case stateAwaitConf:			
//Our Client has sent a confirmed request and is awaiting confirmation
			switch (pdutype)
			{
			case COMPLEX_ACK_PDU:
				if ((ap->asduPDUheader&ASDUissegmented)!=0)
//we've received a segmented complex ack
				{   if (SegmentedRX==0)					//Segmented RX is not supported!
					{	np->nalsRxFlag=NALSE_ReceivedPacketTooLarge;
						//send ABORT
						SendAbort (tp,(octet)i,ABORT_SegmentationNotSupported,CLIENT);
						RxToss++;
						goto rireturnpkt3;
					}
					if ((cid[i].cidAPDUData=GMALLOC(MaxSizeALapdu))==NULL)		//Get space to accumulate full, unsegmented cack
					{	LogNPDUerror(mp,"Can't get free space for NPDU");
						np->nalsRxFlag=NALSE_NoFreeSpaceAvailable;
						//send ABORT
						SendAbort (tp,(octet)i,ABORT_BufferOverflow,CLIENT);
						RxToss++;
						goto rireturnpkt3;
					}
					sap=(ASDU *)cid[i].cidAPDUData;
					sap->asduPDUheader=ap->asduPDUheader&ASDUpdutype;	//application will not know CACK is segmented
					sap->asduInvokeID=ap->asduInvokeID;
//The following is a hack because we need to leave the ServiceAckChoice in the third
//octet in the fully assembled APDU. Since the fully assembled APDU does not include
//sequence information, we can't use the ASDU structure's asduServiceAckChoice field
					sap->asduSequenceNumber=ap->asduServiceAckChoice;	//			***031
			        cid[i].cidMaxLen=MACPmaxNPDUlength(mp->maLport);	//do we need this??
			        cid[i].cidSequence=0;
			        cid[i].cidOctetCount=3;								//for counting total octets received ***031
		            cid[i].cidWindowSize=1;								//we need to segment ack the first one***213
					cid[i].cidActualWindowSize=ap->asduWindowSize;		//						***213
			        cid[i].cidState=stateSegmentedConf;
			        goto riSegmentedConf;
				}
				//non-segmented complex ack --fall into same case as simple ack
			case SIMPLE_ACK_PDU:
risuccess:			np->nalsRxFlag=NALSE_Success;
				goto rireturnpkt2;
			case REJECT_PDU:
				np->nalsRxFlag=NALSE_RejectReceived;
				goto rireturnpkt2;
			case ERROR_PDU:
				np->nalsRxFlag=NALSE_ErrorReceived;
				goto rireturnpkt2;
			case ABORT_PDU:
				np->nalsRxFlag=NALSE_AbortReceived;
				goto rireturnpkt2;
			default:
riinvalidresponse:	np->nalsRxFlag=NALSE_InvalidResponse;
				RxToss++;
rireturnpkt3:	GFREE(np->nalsAPDU);									//packet we got for accumulating segmented CACK
rireturnpkt2:	GFREE(cid[i].cidTxPacket);								//packet we got for transmitting
				cid[i].cidTxPacket=NULL;
				GFREE(cid[i].cidAPDUPacket);							//packet from A.L.
				cid[i].cidAPDUPacket=NULL;
				*isserver=cid[i].cidRequestorisserver;
				cid[i].cidState=stateFree;
				return TRUE;
			}	//end switch (pdutype)
			case stateSegmentedConf:
//Our Client is receiving a segmented complex ack
			switch (pdutype)
			{
			case COMPLEX_ACK_PDU:
				if ((ap->asduPDUheader&ASDUissegmented)==0)
				{	//send ABORT
					SendAbort(tp,(octet)i,ABORT_InvalidAPDUInThisState,CLIENT);
					goto riinvalidresponse;
				}
				if (ap->asduSequenceNumber!=cid[i].cidSequence)
				{	if(IsForUs(np->nalsLport,np->nalsDadr))	//									***025***026
						//send SEGMENTED NACK
						SendSegmentedAck(tp,np,(octet)i,(octet)((cid[i].cidSequence)-1),
										cid[i].cidActualWindowSize,FALSE,CLIENT);	//			***030***042
					cid[i].cidWindowSize=cid[i].cidActualWindowSize;	//reset window			***213
					cid[i].cidTimer=Tseg;
//					goto ricexit;										//						***213 Begin
// Note: We can't go to ricexit here or we'll release cidAPDUData which has 
//       been put into nalsAPDU
					return false;										//						***213 End
				}
				sap=(ASDU *)cid[i].cidAPDUData;
riSegmentedConf:	srap=(octet *) sap;
				srap+=cid[i].cidOctetCount;
				if ((cid[i].cidOctetCount+=np->nalsAlen-5)>MaxSizeALapdu)	//					***031
				{	np->nalsRxFlag=NALSE_ReceivedPacketTooLarge;
					RxToss++;
					goto rireturnpkt3;
				}
				rap=(octet *)np->nalsAPDU;
				rap+=5;										//skip cack overhead				***031
				memcpy(srap,rap,(np->nalsAlen-5));			//copy the apdu data bytes			***031
				ismore=ap->asduPDUheader&ASDUmoresegments;	//									***042
				GFREE(np->nalsAPDU);						//we must now point np at unsegmented buffer
				np->nalsAPDU=cid[i].cidAPDUData;
				np->nalsAlen=(uint)cid[i].cidOctetCount;
				if ((cid[i].cidWindowSize-=1)==0)
				{	if(IsForUs(np->nalsLport,np->nalsDadr))//									***025***026
					//send SEQMENTED ACK
						SendSegmentedAck(tp,np,(octet)i,cid[i].cidSequence,
										cid[i].cidActualWindowSize,TRUE,CLIENT);	//			***030
					cid[i].cidWindowSize=cid[i].cidActualWindowSize;
				}
				if (ismore)														//				***042
				{	cid[i].cidTimer=Tseg*4;
					cid[i].cidSequence+=1;
					return FALSE;
				}
				else
				{	if ((cid[i].cidWindowSize!=cid[i].cidActualWindowSize)&&	//				***025	
					   (IsForUs(np->nalsLport,np->nalsDadr)))	//								***025***026
						SendSegmentedAck(tp,np,(octet)i,cid[i].cidSequence,
										cid[i].cidActualWindowSize,TRUE,CLIENT);	//			***030
					goto risuccess;								//All Done -- Complete Segmented Complex Ack Received
				}

			case ABORT_PDU:								
				np->nalsRxFlag=NALSE_AbortReceived;
				goto rireturnpkt2;
			default:
				//send ABORT
				SendAbort(tp,(octet)i,ABORT_InvalidAPDUInThisState,CLIENT);
				goto riinvalidresponse;

 			}	//end switch (pdutype)
		case stateSegmentedRequest:
//Our Client is sending a segmented confirmed request
			switch (pdutype)
			{
			case SEGMENT_ACK_PDU:
				if (InWindow(ap->asduSequenceNumber,cid[i].cidInitialSequence,i,CLIENT))
				//All outstanding segments are now ack'd
				{	if (cid[i].cidMore)
					//More Segments to send
					{	do
						{  	cid[i].cidOctetCount-=(cid[i].cidMaxLen-6);		//must account for APCI header	***034
							cid[i].cidAPDUData+=cid[i].cidMaxLen-6;			//								***034
						}
						while (cid[i].cidInitialSequence++!=ap->asduSequenceNumber);
														//at this point OctetCount, APDUData are updated
														//and InitialSequence=asduSequenceNumber+1 (mod 256)
         				cid[i].cidActualWindowSize=ap->asduWindowSize;          
          				cid[i].cidTries = Nretry+1;		//										***006
         				if (e=FillWindow(i,CLIENT))
         				{	np->nalsRxFlag=NALSE_TransmitError;
 							goto rireturnpkt2;
						}			          					
					}
					else
					//All segments have been sent, now wait for confirmation
						cid[i].cidState = stateAwaitConf;
				}
          		cid[i].cidTimer=Tseg;
//				goto ricexit;															//	***213 Begin
// Note: We can't go to ricexit here or we'll release cidAPDUData which has 
//       been put into nalsAPDU
				return false;															//	***213 End
  			case ABORT_PDU:								
				np->nalsRxFlag=NALSE_AbortReceived;
				goto rireturnpkt2;
			default:
				//send ABORT
				SendAbort (tp,(octet)i,ABORT_InvalidAPDUInThisState,CLIENT);			//	***213
				np->nalsRxFlag=NALSE_InvalidResponse;
				goto rireturnpkt2;
            }	//end switch (pdutype)

		default:											//unexpected APDU
			if ((pdutype==COMPLEX_ACK_PDU)&&((ap->asduPDUheader&ASDUissegmented)!=0))	//		***035
			{	if (tp==NULL)								//									***213
				{	if ((tp = GMALLOC(sizeof (MApacket)))==NULL)	//allocate space for MA packet for transmitting
					{	LogNPDUerror(mp,"Can't get free space for Transmit Packet");
						goto ricexit;
					}
				}
		        tp->maLport=mp->maLport;
		        tp->maAlen=mp->maAlen;
		        tp->maLsap=mp->maLsap;
		        tp->maBroadcast=mp->maBroadcast;
				memcpy(&tp->maDadr,&mp->maSadr,mp->maAlen);	//source becomes destination for transmit
				SendAbort(tp,(octet)i,ABORT_InvalidAPDUInThisState,CLIENT);
				GFREE(tp);
			}
			goto ricexit;
		}	//end switch (cid[i].cidState)
	}	//end switch (pdutype)
ricexit:
	RxToss++;
	np->nalsAPDU=NULL;										//actually..this is not needed!
	GFREE(np->nalsAPDU);
	return FALSE;
}
//																				***100 End

///////////////////////////////////////////////////////////////////////					***022 Begin
//	Determine if the specified address address is one of our boards
//
//	in:		lport is the logical port													***026
//	daddr is the specified address
//	out:	TRUE/FALSE
//
BOOL IsForUs (octet lport,octet *daddr)						//							***212
{	word	i;

	if(ptpinitialized&&lport==ptpport) return true;			//handle ptp specially		***212
	if(mstpinitialized&&lport==mstpport) return true;		//handle mstp specially		***217
	for (i=0;i<boundadapters;i++)							//							***210 Begin
	{	if (Adapter[i].hFile!=INVALID_HANDLE_VALUE)
		{	if (memcmp(daddr,&Adapter[i].Adr[0],Adapter[i].Alen)==0)	//				***210 End
				return TRUE;
		}
	}
	return FALSE;

}
//																						***022 End 
///////////////////////////////////////////////////////////////////////					***100 Begin
//	Determine if the specified address is one of our boards
//
//	in:		lport is the logical port
//	saddr is the specified address
//	out:	TRUE/FALSE
//
BOOL IsEcho (octet lport,octet *saddr)				//									***212
{	return IsForUs(lport,saddr);
}													//									***100 End

//																						***009 Begin
/////////////////////////////////////////////////////////////////////// 
//	Determine if the ADPU is a response to an outstanding client/server
//  response/request
//in:	i			is the index into cid or sid
//		invokeId	is the invoke ID of the response APDU (required for SERVER only)
//		*np 		points to the NALS packet for the response
//		agent		is CLIENT or SERVER 
//out:  TRUE or FALSE

BOOL IsSource (int i, octet invokeID, NALSpacket *np, ALTYPE agent)
{	int 	isnode;
	if (agent==CLIENT)
	{	isnode=memcmp(&cid[i].cidDadr,&np->nalsSadr,np->nalsSlen); 		//is node address=original destination
		return ((cid[i].cidLport==np->nalsLport)&&
		    (cid[i].cidDnet==np->nalsSnet)&&
		    (isnode==0));
	}
	else
	{	isnode=memcmp(&sid[i].sidSadr,&np->nalsSadr,np->nalsSlen); 		//is node address=original destination
		return ((sid[i].sidLport==np->nalsLport)&&
		    (sid[i].sidSnet==np->nalsSnet)&&
		    (isnode==0)&&
		    (sid[i].sidInvokeID==invokeID));
	}
}

/////////////////////////////////////////////////////////////////////// 
//	Determine if the ADPU is Who is or Iam and handle it
//in:	*np			points to the NALSpacket
//		alen		is the size of the apdu in bytes
//out:  nothing

void HandleWhoisIam (NALSpacket *np)
{	octet		*ap;
	dword		lowlim,highlim,instance;			//										***170
	word		type;
    int			i,free1;
    
	ap=(octet *)np->nalsAPDU;						//point to APDU header
	if (ap[1]==who_Is)
	{	if (np->nalsAlen>2)									//we must check our instance with the limits
		{	ap+=2;
			if ((*ap&0xf8)!=0x08)						//it's not context tag 0
				return;
			lowlim=dCONTEXT(&ap,FALSE);					//						***014
			if ((*ap&0xf8)!=0x18)						//it's not context tag 1
				return;
			highlim=dCONTEXT(&ap,FALSE); 				//						***014
			if ((OurInstance<lowlim)||(OurInstance>highlim))
				return;
		} 
		NALSAPItransmitIam(np->nalsLport,(dword)np->nalsSnet,np->nalsRadr,np->nalsRlen);	// ***214
		return;
	}
	else if (ap[1]==I_Am)
	{	
/*																							***170 Begin
//		NALSAPItransmitIam(np->nalsLport,(dword)np->nalsSnet,np->nalsSadr,np->nalsSlen);	//let's make sure he knows about us ***145***147
		ap+=2;
		free1=-1;
		for (i=0;i<maxpeers;i++)						//first check if this is a known peer
		{	if ((peerlist[i].nalspLport==255)&&(free1==-1))
				free1=i;								//this is first free entry in peerlist
			else if ((peerlist[i].nalspLport==np->nalsLport)&&
				(peerlist[i].nalspNet==np->nalsSnet)&&
				(peerlist[i].nalspAlen==np->nalsSlen)&&
				(memcmp(&peerlist[i].nalspAdr[0],&np->nalsSadr[0],peerlist[i].nalspAlen)==0))
				break;									//this is a known peer
		}
		if (i>=maxpeers)								//unknown peer, must add new peer to list
		{	if (free1==-1)
				return;
    		i=free1;									//cuz we're lazy
    		peerlist[i].nalspLport=np->nalsLport;
    		peerlist[i].nalspNet=np->nalsSnet;
			peerlist[i].nalspAlen=np->nalsSlen;
			memcpy(&peerlist[i].nalspAdr[0],&np->nalsSadr[0],peerlist[i].nalspAlen);
		}
		if (*ap!=0xC4)									//it's not a device instance
			return;
		peerlist[i].nalspInstance=dOBJECTID(&ap,&type);	//					***014 Begin
		if ((*ap&0xf8)!=0x20)									//it's not a device instance
			return;
		peerlist[i].nalspMaxapdulen=(uint)dUINT(&ap);
		if ((*ap&0xf8)!=0x90)									//it's not a max apdu len
			return;
		peerlist[i].nalspSegsupported=(octet)dENUM(&ap);
		if ((*ap&0xf8)!=0x20)									//it's not a max apdu len
			return;
		peerlist[i].nalspVendorid=(uint)dUINT(&ap);		//					***014 End
*/
	 	ap+=2;
		if (*ap!=0xC4) return;							//it's not a device instance..not much we can do
		instance=dOBJECTID(&ap,&type);
		free1=-1;
		for (i=0;i<maxpeers;i++)						//first check if this is a known peer
		{	if (peerlist[i].nalspInstance==instance) 
				break;									//this is a known peer
			else if ((peerlist[i].nalspLport==255)&&(free1==-1))
				free1=i;								//this is first free entry in peerlist
		}
		if (i>=maxpeers)								//unknown peer, must add new peer to list
		{	if (free1==-1) return;						//oops no space
    		i=free1;									//cuz we're lazy
		}
// Note: We always update peer list in case address etc. has changed..assume device id is unique
		peerlist[i].nalspInstance=instance;
		peerlist[i].nalspLport=np->nalsLport;
    	peerlist[i].nalspNet=np->nalsSnet;
		peerlist[i].nalspAlen=np->nalsSlen;
		memcpy(&peerlist[i].nalspAdr[0],&np->nalsSadr[0],peerlist[i].nalspAlen);
		peerlist[i].nalspRlen=np->nalsRlen;
		if (peerlist[i].nalspRlen!=0)
			memcpy(&peerlist[i].nalspRadr[0],&np->nalsRadr[0],peerlist[i].nalspRlen);

		if ((*ap&0xf8)!=0x20)							//it's not a max apdu
			return;
		peerlist[i].nalspMaxapdulen=(uint)dUINT(&ap);
		if ((*ap&0xf8)!=0x90)							//it's not a seg supported
			return;
		peerlist[i].nalspSegsupported=(octet)dENUM(&ap);
		if ((*ap&0xf8)!=0x20)							//it's not a vendor id
			return;
		peerlist[i].nalspVendorid=(uint)dUINT(&ap);
//																					***170 End
	}		
	else
		return;
}
//																			***009 End

///////////////////////////////////////////////////////////////////////
//  Send an ABORT packet 
     
//in:	the MApacket to transmit
//		index into cid for this client transaction (this is also the invoke ID of the transaction)
//		the reason for aborting
//		TRUE if server, FALSE if client

void SendAbort (MApacket *tp, octet id, octet reason, ALTYPE agent)	//					***212
{	octet		*pp;								//									***212
	char		b[80];
	int			i;

	pp=&tp->maNPDU[0];								//point to NPCI header (in case we have to transmit)
	*pp++=1;										//BACnet version 1
	*pp++=(*pp&NSDUpriority)&(~NSDUexpectsreply);	//APDU, no DNET, no SNET, no reply
	if (agent==SERVER)
		*pp++=((ABORT_PDU<<ASDUpdutypeshr)&ASDUpdutype)|ASDUserver;		//PDU header
	else
		*pp++=((ABORT_PDU<<ASDUpdutypeshr)&ASDUpdutype)&(~ASDUserver);	//PDU header
	*pp++=id;										//invoke id
	*pp++=reason;									//abort reason
	tp->maNlen=5;									//length
	tp->maTrace=TxCounter++;
	if (i=DoTransmit(tp))
	{	sprintf(b,"NPDU Transmit Failed:%d",i);
		MakeLog(b,FALSE);
	}
	else
		ShowAsHex(tp,IDC_TXLIST);					//display this packet in the transmit list
	return; 
}

///////////////////////////////////////////////////////////////////////
//  Send an Segmented ACK packet 
     
//in:	mp			the MApacket to transmit
//		np			where the segment we're acking came from						***030 Begin
//		id			the invoke ID of the transaction
//		seq			the sequence
//		winsize		the window size
//		ack			TRUE if ack, FALSE if nack
//		agent		server or client

void SendSegmentedAck (MApacket *tp, NALSpacket *np, octet id, octet seq, octet winsize, BOOL ack, ALTYPE agent) //***212
{	octet		*pp;								//									***212
	char		b[80];
	int			i;

	pp=&tp->maNPDU[0];								//point to NPCI header (in case we have to transmit)
	*pp++=1;										//BACnet version 1
	*pp=np->nalsPri&(~NSDUexpectsreply);			//APDU, no DNET, no SNET, no reply
	if (np->nalsSnet==0)							//this wasn't routed to us, so no DNET/DADR is needed in the reply
		pp++;										//skip past NPCI header
	else											//this was routed to us
	{	*pp++=*pp|NSDUhasDNET;						//we need to route the reply
		*pp++=(octet)(np->nalsSnet>>8);				//hi byte first
		*pp++=(octet)(np->nalsSnet&0xFF);			//lo byte
		*pp++=np->nalsSlen;							//send back to the guy we got it from originally
		memcpy(pp,np->nalsSadr,np->nalsSlen);
		pp+=np->nalsSlen;
		*pp++=255;									//init hop count
	}
	if (agent==SERVER)
		*pp=((SEGMENT_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype)|ASDUserver;		//PDU header
	else
		*pp=((SEGMENT_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype)&(~ASDUserver);	//PDU header
	if (ack)
		*pp++=*pp&(~ASDUnak);						//ACK
	else
		*pp++=*pp|ASDUnak;							//NACK
	*pp++=id;										//invoke id
	*pp++=seq;										//sequence number
	*pp++=winsize;									//window size
	tp->maNlen=(pp-&tp->maNPDU[0]);					//length						***030 End
	tp->maTrace=TxCounter++;
	if (i=DoTransmit(tp))
	{	sprintf(b,"NPDU Transmit Failed:%d",i);
		MakeLog(b,FALSE);
	}
	else
		ShowAsHex(tp,IDC_TXLIST);					//display this packet in the transmit list
	return; 
}

///////////////////////////////////////////////////////////////////////
//  Compare the modulo-256 difference between seqA and seqB (seqA-seqB) against 
//  cidActualWindowSize/sidActualWindowSize for the index i in cid/sid.
     
//in:	seqA and seqB are the sequences to compare
//		index into cid for this client transaction (this is also the invoke ID of the transaction)
//      or index into sid for server transaction
//		TRUE if server, FALSE if client
//out:	TRUE if (seqA - seqB) < ActualWindowSize
//		FALSE if (seqA-seqB) >= ActualWindowSize

BOOL InWindow (octet seqA, octet seqB, int i, ALTYPE agent)
{	octet		winsize;
	winsize=(agent==SERVER)?sid[i].sidActualWindowSize:cid[i].cidActualWindowSize;
	return (((octet)(seqA-seqB))<winsize)?TRUE:FALSE;		//								***213
}

///////////////////////////////////////////////////////////////////////
//  Transmit a number of segments (cidActualWindowSize) of a segmented confirmed request if agent = CLIENT
//  or a number of segments (sidActualWindowSize) of a segmented complex ack if agent = SERVER. If there
//  is not enough data remianing in the ADPU to fill cidActualWindowSize/sidActualWindowSize packets, 
//  then send the number available and set cidMore/sidMore = FALSE to indicate that all segments have
//  been transmitted at least once. This function does not update cidInitialSequence, cidAPDUData or
//  cidOctetCount/sidInitialSequence, sidAPDUData or sidOctetCount in case a retry is necessary. It's
// the responsibility of the client/server receiver to updata these parameters.
  
//in:	index into cid for this client transaction (this is also the invoke ID of the transaction)
//		or index into sid for this transaction
//out:	0=success, else error transmitting

int FillWindow (int i, ALTYPE agent)
{	octet		s;
	int			j;
	word		l, os;
	ASDUREQ		*apr;				//													***212 Begin
	ASDU		*ap;
	MApacket	*mp;
	ASDUREQ 	*ppr;
	ASDU		*pp;
	char		b[80];
	octet		*op;				//													***212 End
	
	j=NALSE_NoMoreSegments;			//in case there are no more segments to send
	if (agent==SERVER)				//we're a server sending a segmented complex ack
	{	s=sid[i].sidInitialSequence;
		mp=(MApacket *) sid[i].sidTxPacket;
//		ap=(ASDU *) sid[i].sidAPDUData+1;						//					***042
		ap=(ASDU *) sid[i].sidAPDUData;							//					***042
		os=sid[i].sidOctetCount;
		while ((sid[i].sidMore) && ((s-sid[i].sidInitialSequence) < sid[i].sidActualWindowSize))
		{	pp=(ASDU *)&mp->maNPDU[FindAPDU(mp)];				//point to APDU skipping NPCI header		***031
			pp->asduSequenceNumber=s;							//sequence number
			if (os>(sid[i].sidMaxLen-5))						//must account for APCI header				***034
			{	sid[i].sidMore=TRUE;							//there's more after this
				l=sid[i].sidMaxLen-5;							//											***034
				pp->asduPDUheader|=ASDUmoresegments;
				pp->asduWindowSize=sid[i].sidActualWindowSize;
			}
			else
			{	sid[i].sidMore=FALSE;  							//this is the last packet
				l=sid[i].sidOctetCount;
				pp->asduPDUheader&= ~ASDUmoresegments;
				pp->asduWindowSize=(s-sid[i].sidInitialSequence)+1;
			}
			op=(octet *)pp;										//point at APDU
			op+=5;												//skip over ASDI							***034
			memcpy(op,ap,l);									//copy APDU octets
			mp->maNlen=(sid[i].sidNPCI_APCILen)+l;
		    os-=l;												//total number of APDU data bytes remaining to be sent (exclude header, maxresp, and invokeID)
		    op=(octet *)ap;
		    op++;												//account for amount we're sending now		***003
		    ap=(ASDU *)op;			
			mp->maTrace=TxCounter++;
			if (j=DoTransmit(mp))
			{	sprintf(b,"NPDU Transmit Failed:%d",j);
				MakeLog(b,FALSE);
				goto cfwend;
			}
			else
		    {	sid[i].sidState = stateSegmentedResponse;            
				sid[i].sidTimer=Tseg;
				ShowAsHex(mp,IDC_TXLIST);						//display this packet in the transmit list
			}
			s++;												//											***003
		}
	}
	else 							//we're a client sending a segmented confirmed request
	{	s=cid[i].cidInitialSequence;
		mp=(MApacket *)cid[i].cidTxPacket;
		apr=(ASDUREQ *)cid[i].cidAPDUData;
		os=cid[i].cidOctetCount;
		while ((cid[i].cidMore)&&((s-cid[i].cidInitialSequence) < cid[i].cidActualWindowSize))
		{	ppr=(ASDUREQ *)&mp->maNPDU[FindAPDU(mp)];			//point to APDU skipping NPCI header		***031
			ppr->asdureqSequenceNumber = s;						//sequence number
			if (os>(cid[i].cidMaxLen-6))						//must account for APCI header				***034
			{   cid[i].cidMore=TRUE;							//there's more after this
				l=cid[i].cidMaxLen-6;							//											***034
				ppr->asdureqPDUheader|=ASDUmoresegments;
				ppr->asdureqProposedWindowSize=cid[i].cidActualWindowSize;
			}
			else
			{	cid[i].cidMore=FALSE;  							//this is the last packet
//				l=cid[i].cidOctetCount;							//							***213A
				l=os;											//							***213A
				ppr->asdureqPDUheader&= ~ASDUmoresegments;
				ppr->asdureqProposedWindowSize=(s-cid[i].cidInitialSequence)+1;
			}
			op=(octet *)ppr;									//point at APDU
			op+=6;												//skip over ASDI							***034
			memcpy(op,apr,l);									//copy APDU octets
			mp->maNlen=(cid[i].cidNPCI_APCILen)+l;
		    os-=l;												//total number of APDU data bytes remaining to be sent (exclude header, maxresp, and invokeID)
		    op=(octet *)apr;
//			op++;												//account for amount we're sending now***213A
			op+=l;												//THIS IS NOT one it's ell		***213A
		    apr=(ASDUREQ *)op;			
			mp->maTrace=TxCounter++;
			if (j=DoTransmit(mp))
			{	sprintf(b,"NPDU Transmit Failed:%d",j);
				MakeLog(b,FALSE);
				goto cfwend;
			}
			else
		    {	cid[i].cidState=stateSegmentedRequest;            
				cid[i].cidTimer=Tseg;
				ShowAsHex(mp,IDC_TXLIST);						//display this packet in the transmit list
			}
			s++;												//											***003
		}
	}
cfwend:
	return j;
}

///////////////////////////////////////////////////////////////////////					***031 Begin
//	Find the APDU in an NPDU
//in:	mp		has the NPDU in it
//out:	number of octets offset from mp->maNPDU to the first APDU octet

word FindAPDU(MApacket *mp)							//									***212 Begin
{	NPCI	*np;
	NSRC	*sp;									//									***212 End
	word	w=2;
	
	np=(NPCI *)(&mp->maNPDU[0]);					//									***212
	if (np->npciControl&NSDUhasDNET)
	{	w+=4+np->npciDLEN;
		sp=(NSRC *)(((octet *)&np->npciDADR[0])+np->npciDLEN);	//						***212
	}
	else
		sp=(NSRC *)&np->npciDNET;					//									***212
	if (np->npciControl&NSDUhasSNET)
		w+=3+sp->SLEN;
	return w;
}													//									***031 End

///////////////////////////////////////////////////////////////////////
//	Log an Ignored NPDU
//in:	mp		points to it
//		why		points to a description of the problem

void LogNPDUerror (MApacket *mp,char *why)			//									***212
{	char	b[80];

	sprintf(b,"NPDU[%lu] Ignored:%s",mp->maTrace,why);
	MakeLog(b,FALSE);
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
{
	if (!Minimized)
		DrawBitmap(hStatus,(st ? hGreen : hRed),0,0);
}

///////////////////////////////////////////////////////////////////////
//	Show Transmit and/or Receive Windows

void ShowPacketWindows()
{	BOOL	tw,rw;
	HWND	htw,hrw,htl,hrl,hs,hc;				//								***100***140
	RECT	rc,rct,rcr,rcs;						//								***140
	int		h,w,ht,hr,wt,wr,hss;				//								***140

	GetClientRect(hStatus,&rc);					//get width and height of client area
	w=rc.right-(ULtxrx.x*2);					//leave a margin around both sides of the list
	h=rc.bottom-(ULtxrx.x+ULtxrx.y);			//leave left margin at bottom also
	tw=IsCheckedMenuItem(MENU_VIEW,ID_VIEW_TRANSMITPACKETS);
	rw=IsCheckedMenuItem(MENU_VIEW,ID_VIEW_RECEIVEPACKETS);
	htw=GetDlgItem(hStatus,IDC_TXLIST);
	hrw=GetDlgItem(hStatus,IDC_RXLIST);
	htl=GetDlgItem(hStatus,IDC_TXLBL);
	hrl=GetDlgItem(hStatus,IDC_RXLBL);
	hs=GetDlgItem(hStatus,IDC_MACSTATS);		//								***100
	hc=GetDlgItem(hStatus,IDC_CACSTATS);		//								***140
	GetWindowRect(htl,&rcr);
	GetWindowRect(hrl,&rct);
	GetWindowRect(hs,&rcs);						//								***140
	ht=rct.bottom-rct.top;
	hr=rcr.bottom-rcr.top;
	hss=rcs.bottom-rcs.top;						//								***140
	wt=rct.right-rct.left;
	wr=rcr.right-rcr.left;
	
	h-=((showstats)?hss*2:0);										//				***140
	if (tw&&rw)									//both windows are displayed
	{	h/=2;
		MoveWindow(hrw,ULtxrx.x,ULtxrx.y,w,h,FALSE);	
		MoveWindow(htw,ULtxrx.x,ULtxrx.y+h,w,h,FALSE);	
		MoveWindow(hrl,ULtxrx.x-wr,ULtxrx.y,wr,hr,FALSE);	
		MoveWindow(htl,ULtxrx.x-wt,ULtxrx.y+h,wt,ht,FALSE);	
		if (showstats)												//				***140 Begin
		{	MoveWindow(hs,ULtxrx.x,ULtxrx.y+(2*h),w,hss,FALSE);
			MoveWindow(hc,ULtxrx.x,ULtxrx.y+(2*h)+hss,w,hss,FALSE);
		}															//				***140 End
	}
	else if (tw||rw)							//if either one is checked then make it visible
	{	MoveWindow((tw?htw:hrw),ULtxrx.x,ULtxrx.y,w,h,FALSE);
		if (rw)
			MoveWindow(hrl,ULtxrx.x-wr,ULtxrx.y,wr,hr,FALSE);
		else	
			MoveWindow(htl,ULtxrx.x-wt,ULtxrx.y,wt,ht,FALSE);	
		if (showstats)												//				***140 Begin
		{	MoveWindow(hs,ULtxrx.x,ULtxrx.y+h,w,hss,FALSE);
			MoveWindow(hc,ULtxrx.x,ULtxrx.y+h+hss,w,hss,FALSE);
		}
	}
	else
	{	if (showstats)
		{	MoveWindow(hs,ULtxrx.x,ULtxrx.y+h,w,hss,FALSE);
			MoveWindow(hc,ULtxrx.x,ULtxrx.y+h+hss,w,hss,FALSE);
		}
	}																//				***140 End
	ShowWindow(htw,(tw?SW_SHOWNORMAL:SW_HIDE));	//update both lists
	ShowWindow(hrw,(rw?SW_SHOWNORMAL:SW_HIDE));
	ShowWindow(htl,(tw?SW_SHOWNORMAL:SW_HIDE));	//update both lists
	ShowWindow(hrl,(rw?SW_SHOWNORMAL:SW_HIDE));
	ShowWindow(hs,(showstats?SW_SHOWNORMAL:SW_HIDE));					//		***140
	ShowWindow(hc,(showstats?SW_SHOWNORMAL:SW_HIDE));					//		***140
	InvalidateRect(hStatus,NULL,TRUE);			//update the whole window
	UpdateWindow(hStatus);
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
//	toggle the checked state of a menu item in Status Dialog

void ToggleCheckMenuItem(int menuindex,UINT nDlgItem)
{	HMENU hmenu;
	BOOL check;

	hmenu = GetSubMenu(GetMenu(hStatus),menuindex);	//get handle to popup menu
	check = GetMenuState(hmenu,nDlgItem,MF_BYCOMMAND) & MF_CHECKED;
	CheckMenuItem(hmenu,nDlgItem,MF_BYCOMMAND | (check ? MF_UNCHECKED : MF_CHECKED));
}

///////////////////////////////////////////////////////////////////////
//	return the checked state of a menu item in Status Dialog

BOOL IsCheckedMenuItem(int menuindex,UINT nDlgItem)
{	HMENU hmenu;
	BOOL check;

	hmenu = GetSubMenu(GetMenu(hStatus),menuindex);	//get handle to popup menu
	check = GetMenuState(hmenu,nDlgItem,MF_BYCOMMAND) & MF_CHECKED;
	return (check ? TRUE : FALSE);
}

///////////////////////////////////////////////////////////////////////
//	return one of two message values depending on whether a dialog
//  item is a list or a combobox
//in:	nDlgItem	the item which might be a list or a combo
//		uList		message to return if a list
//		uCombo		message to return if a combo

UINT lcb(int nDlgItem,UINT uList,UINT uCombo)
{	//NOTE:	Ultimately this may need to be a more sophisticated test...
	return (nDlgItem==IDC_LOG)?uCombo:uList;
}

///////////////////////////////////////////////////////////////////////
//	remove all list items from one of the lists in a dialog

void ClearItems(HWND hWnd,int nDlgItem)
{ SendDlgItemMessage(hWnd,nDlgItem,
					lcb(nDlgItem,LB_RESETCONTENT,CB_RESETCONTENT),
					0, 0L);
}

///////////////////////////////////////////////////////////////////////
//	get the list count from one of the lists in Status Dialog

int GetListCount(int nDlgItem)
{ return (int) SendDlgItemMessage(hStatus,nDlgItem,
					lcb(nDlgItem,LB_GETCOUNT,CB_GETCOUNT),
					0, 0L);
}

///////////////////////////////////////////////////////////////////////
//	get the text from a list item in one of the lists in Status Dialog

int GetListText(int nDlgItem,int nIndex,char *p)
{ return (int) SendDlgItemMessage(hStatus,nDlgItem,
					lcb(nDlgItem,LB_GETTEXT,CB_GETLBTEXT),
					nIndex,(LPARAM) ((LPSTR) p));
}

///////////////////////////////////////////////////////////////////////
//	replace text in a list item in one of the lists in Status Dialog

int ReplaceItem(int nDlgItem,int nIndex,char *p)
{	SendDlgItemMessage(hStatus,nDlgItem,	//first delete that item
					lcb(nDlgItem,LB_DELETESTRING,CB_DELETESTRING),
					nIndex, 0L);
	return (int) SendDlgItemMessage(hStatus,nDlgItem,
					lcb(nDlgItem,LB_INSERTSTRING,CB_INSERTSTRING),
					nIndex,(LPARAM) ((LPSTR) p));
}

///////////////////////////////////////////////////////////////////////
//	remove a list item from one of the lists in Status Dialog

void RemoveItem(int nDlgItem,int nIndex)
{ SendDlgItemMessage(hStatus,nDlgItem,
					lcb(nDlgItem,LB_DELETESTRING,CB_DELETESTRING),
					nIndex, 0L);
}

///////////////////////////////////////////////////////////////////////
//	select a list item from one of the lists in a dialog

void SelectItem(HWND hWnd,int nDlgItem,int nIndex)
{ SendDlgItemMessage(hWnd,nDlgItem,
					lcb(nDlgItem,LB_SETCURSEL,CB_SETCURSEL),
					nIndex, 0L);
}

///////////////////////////////////////////////////////////////////////
//	get the selection of lists in a dialog

int GetSelectItem(HWND hWnd,int nDlgItem)
{	int	r;
	r=(int) SendDlgItemMessage(hWnd,nDlgItem,
					lcb(nDlgItem,LB_GETCURSEL,CB_GETCURSEL),
					0, 0L);
	return r;
}

///////////////////////////////////////////////////////////////////////
//	add a list item to one of the lists in a dialog

int AddItem(HWND hWnd,int nDlgItem,const char *p)
{	int	r;
	r=(int) SendDlgItemMessage(hWnd,nDlgItem,
					lcb(nDlgItem,LB_ADDSTRING,CB_ADDSTRING),
					0,(LPARAM) ((LPSTR) p));
	return r;
}

///////////////////////////////////////////////////////////////////////
//	add and select a list item to one of the lists in Status Dialog

int EnterItem(int nDlgItem,const char *p)
{	int	r;
	r=(int) SendDlgItemMessage(hStatus,nDlgItem,
					lcb(nDlgItem,LB_ADDSTRING,CB_ADDSTRING),
					0,(LPARAM) ((LPSTR) p));
	SendDlgItemMessage(hStatus,nDlgItem,
					lcb(nDlgItem,LB_SETCURSEL,CB_SETCURSEL),
					r, 0L);
	return r;
}

///////////////////////////////////////////////////////////////////////
//	add a new entry to the Error Log in Status Dialog

void MakeLog (char *what, int needstimestamp)
{	char 	log[32]; 
	time_t	ltime;
	
	while (GetListCount(IDC_LOG)>=NALS_MAX_STATUS) //make room for more errors in the log list
		RemoveItem(IDC_LOG,0);					//remove the oldest one
		
    if (needstimestamp)							//add a timestamp log to the list
	{	time(&ltime);
		sprintf(log,"%.24s",ctime(&ltime));
    	AddItem(hStatus,IDC_LOG,log);
    }
	if (strlen(what)>80)						//better trim this one before adding it to the list
		what[80]=0;								//force it to be 80 chars asciz
   	EnterItem(IDC_LOG,what);					//add to log list
}

/////////////////////////////////////////////////////////////////////// 
//	Find the next comma or EOS
//in:	p	points to the beginning of the string to look in
//out:	p	unchanged, but the comma (if found) is changed to asciz
//		returns pointer past asciz if comma was found or NULL if EOS

char *Nxt(char *p)
{	char *q;									//temp pointer
	if ((q=strchr(p,','))!=NULL)				//got a comma
		*q++=0x00;								//make it asciz and point past it	
	return q;									//return pointer to next string
}

/////////////////////////////////////////////////////////////////////// 
//	Copy source string to destination until comma or asciz
//in:	*s	source string or NULL
//		*d	destination string
//out:	*s	points past comma, or to asciz
//		*d	points past copied asciz
//			note that comma is changed to asciz during copy
//			also note that if *s==NULL then dest will get an asciz anyway

void CopyToComma (char **d,char **s)
{	if (*s==NULL)						//pointer was NULL
	{	*(*d)++=0;
		return;
	}
	do
		if (**s==',')
		{	*(*d)++=0;					//coerce commas into asciz
			(*s)++;						//point past comma
			return;
		}
		else if (**s==0)				//stick at the asciz
		{	*(*d)++=0;
			return;
		}
	while (*(*d)++=*(*s)++);				//otherwise copy forever
}

/////////////////////////////////////////////////////////////////////// 
//	append a log line to a log file
//in:	fn	is the file name
//		ln	is the text line to log

void LogToFile (char *fn,char *ln)
{ FILE *f;
  
	if ((f=fopen(fn,"a"))==NULL)
  		MakeLog(strerror(errno),TRUE);
	else
	{   fprintf(f,"%s\n",ln);
		fclose(f);
	}
}

///////////////////////////////////////////////////////////////////////
//	Read Preferences from NALSERV.INI
//
//initialized variables: 
//	UsePTP																				***212
//	UseMSTP																				***217 
//	MinimizeStatus
//	OurPeername
//	OurVendorName																		***005 Begin
//	OurVendorID
//	OurInstance																			***005 End
//	WhoIsIAmInterval
//	TimeSynchInterval																	***016
//	nalsdir
//	HelpFile
//	BACnetTseg
//	BACnetTout
//	BACnetNretry
//	MaxSizeALapdu
//	MaxTXapduLen																		***005 Begin
//	MaxRXapduLen
//	WindowSize
//	SegmentedRX
//	SegmentedTX	
//  Promiscuous																			***005 End
//	PTPsnet, PTPslen, PTPsadr															***212

 void ReadPreferences ()
{	char WinDir[144],lb[128];
	char *r;
	int i,j;
	FILE *f;

	WinDir[0]=0x00;
	GetWindowsDirectory(WinDir, 144);					//make path in windows directory
	strcat(WinDir,inifile);
	if ((f=fopen(WinDir,"r"))==NULL) return;
	while (feof(f)==0)
	{	fgets(lb,128,f);								//read a line from the file 
		if ((r=strstr(lb,"\n"))!=NULL) *r=0x00; 		//force asciz at end of line
		if ((r=strstr(lb,"="))!=NULL)					//it may be valid
		{  *r=0x00;										//force leading part to be asciz
			r++;
			if (stricmp(lb,"ourpeername")==0)
				strncpy(OurPeername,r,24);
			else if (stricmp(lb,"ourvendorname")==0)
				strncpy(OurVendorName,r,64);
			else if (stricmp(lb,"ourvendorid")==0)
				OurVendorID=(uint) atoi(r);		   		
			else if (stricmp(lb,"ourinstance")==0)
				OurInstance=(uint) atoi(r);		   		
			else if (stricmp(lb,"minimizestatus")==0)
				MinimizeStatus=YesNo(r);
			else if (stricmp(lb,"inhibittx")==0)		//							***013
				InhibitTX=YesNo(r);						//							***013
			else if (stricmp(lb,"useptp")==0)			//							***212
				UsePTP=YesNo(r);						//							***212
			else if (stricmp(lb,"usemstp")==0)			//							***217
				UseMSTP=YesNo(r);						//							***217
			else if (stricmp(lb,"whoisiaminterval")==0)
				WhoIsIAmInterval=atoi(r);
			else if (stricmp(lb,"timesynchinterval")==0)	//						***016
				TimeSynchInterval=atoi(r);					//						***016
			else if (stricmp(lb,"nalsdir")==0)
			{	strcpy(nalsdir,r);							//first copy the directory path
				if ((i=strlen(r))>0)						//if string is not null
					if (r[i-1]!='\\') strcat(nalsdir,"\\");
			}
			else if (stricmp(lb,"bacnettseg")==0)
				Tseg=(uint) atoi(r)*10;					//in 100 milliseconds		   		
			else if (stricmp(lb,"bacnettout")==0)
				Tout= (uint) atoi(r)*10;					//in 100 milliseconds		   		
			else if (stricmp(lb,"bacnetnretry")==0)
				Nretry=(uint) atoi(r);		   		
			else if (stricmp(lb,"maxsizealapdu")==0)
				MaxSizeALapdu= (uint) atol(r);		   		
			else if (stricmp(lb,"maxtxapdulen")==0)
				MaxTXapduLen= (uint) atol(r);		   		
			else if (stricmp(lb,"maxrxapdulen")==0)
				MaxRXapduLen= (uint) atol(r);		   		
			else if (stricmp(lb,"windowsize")==0)
				WindowSize= (uint) atol(r);		   		
			else if (stricmp(lb,"segmentedrx")==0)
				SegmentedRX=YesNo(r);
			else if (stricmp(lb,"segmentedtx")==0)
				SegmentedTX=YesNo(r);
			else if (stricmp(lb,"passive")==0)		//								***216
				passive=YesNo(r);					//								***216
			else if (stricmp(lb,"promiscuous")==0)
				ispromiscuous=YesNo(r);
			else if (stricmp(lb,"ptpsnet")==0)			//							***212 Begin
				PTPsnet=(word)atoi(r);
			else if (stricmp(lb,"ptpslen")==0)
				PTPslen=(word)atoi(r);
			else if (stricmp(lb,"ptpsadr")==0)
				cvhexn(r,PTPsadr,PTPslen);				//							***212 End
		 }
	}
	fclose(f);
	//																					***005 Begin
	us.apdu_segment_timeout=((dword)Tseg)*100;		//now set up us
	us.apdu_timeout=((dword)Tout)*100;
	us.number_apdu_retries=Nretry;
	us.go.object_id=OurInstance;
	us.vendor_id=OurVendorID;
	for (i=0;i<=sizeof (us.vendor_name)-1;i++)		//									***008
	{	us.vendor_name[i]=OurVendorName[i];
		if (us.vendor_name[i]==0)						//zero fill
		{	for (j=i+1;j<=sizeof (us.vendor_name)-1;j++)	//							***008
			   us.vendor_name[j]=0;
			break;
		}
	}
	us.max_apdu_length_accepted=MaxRXapduLen;
	if (SegmentedRX==1)
	{
	if (SegmentedTX==1)
		us.segmentation_supported=SEGMENTED_BOTH;
	else
		us.segmentation_supported=SEGMENTED_RECEIVE;
	}
	else if (SegmentedTX==1)
		us.segmentation_supported=SEGMENTED_TRANSMIT;
	else
		us.segmentation_supported=NO_SEGMENTATION;
//																						***005 End
	strcpy(HelpFile,nalsdir);
	strcat(HelpFile,hlpfile);
}
//																						***009 Begin
///////////////////////////////////////////////////////////////////////
//	Read Peerlist from NALSERV.PER. Build peerlist and peernet.
//
// line=p,nnnnnnnn,l,aaaaaaaaaaaa,i,m,s,v
//
//	p=port/logical board (1-4)
//	nnnn=network (in dec or hex)
//	l=address length (1-8)
//	aaaaaaaaaaaa=address (in hex)
//	i=instance (0-(2**22)-1)
//	m=max apdu length supported (1-65535)
//	s=segmentation supported (B=both,T=transmit,R=receive,N=none)
//	v=vendor id(0-65535)
//
void ReadPeerList ()
{	char WinDir[144],lb[128];
	char *r,*ll,c;
	int i,j;
	FILE *f;
  
	WinDir[0]=0x00;
	GetWindowsDirectory(WinDir, 144);					//make path in windows directory
	strcat(WinDir,peerfile);
	if ((f=fopen(WinDir,"r"))==NULL) return;
	while (feof(f)==0)
	{	fgets(lb,128,f);								//read a line from the file 
		if (lb[0]!=';')									//skip comments
		{   if ((r=strstr(lb,"\n"))!=NULL)
		    	*r=0x00; 									//force asciz at end of line
		    for (i=0;i<maxpeers;i++)						//find free entry in peerlist and build new peerlist entry
		    {	if (peerlist[i].nalspLport==255)
		    	{   j=0;
		    		if ((ll=Nxt(&lb[j]))==NULL)
		    			break;
		    		peerlist[i].nalspLport=(octet)atoi(&lb[j]);
		    		j=ll-lb;
		    		if ((ll=Nxt(&lb[j]))==NULL)
		    			break;
		     		if ((lb[j+1]=='x')||(lb[j+1]=='X'))
		      			cvhex4be(&lb[j+2],&peerlist[i].nalspNet);     		
		     		else
		   				peerlist[i].nalspNet=(uint)atol(&lb[j]);     		
		    		j=ll-lb;
		    		if ((ll=Nxt(&lb[j]))==NULL)
		    			break;
		    		peerlist[i].nalspAlen=(octet)atoi(&lb[j]);
		    		j=ll-lb;
		    		if ((ll=Nxt(&lb[j]))==NULL)
		    			break;
		    		cvhexn(&lb[j],&peerlist[i].nalspAdr[0],peerlist[i].nalspAlen);
		    		j=ll-lb;
		    		if ((ll=Nxt(&lb[j]))==NULL)
		    			break;
		    		peerlist[i].nalspInstance=atol(&lb[j]);
		    		j=ll-lb;
		    		if ((ll=Nxt(&lb[j]))==NULL)
		    			break;
		    		peerlist[i].nalspMaxapdulen=atoi(&lb[j]);
		    		j=ll-lb;
		    		if ((ll=Nxt(&lb[j]))==NULL)
		    			break;                              	
		    		c=lb[j]&0xDF;										//capitalize
		    		switch(c)
		    		{
		    		case 'B':
		    			peerlist[i].nalspSegsupported=SEGMENTED_BOTH;
		    			break;    			
		    		case 'N':
		    			peerlist[i].nalspSegsupported=NO_SEGMENTATION;
		    			break;   			
		    		case 'T':
		    			peerlist[i].nalspSegsupported=SEGMENTED_TRANSMIT;
		    			break;   			
		    		case 'R':
		    			peerlist[i].nalspSegsupported=SEGMENTED_RECEIVE;
		    			break;
		    		}	
		    		j=ll-lb;
		    		if ((ll=Nxt(&lb[j]))!=NULL)
		    			break;
		    		peerlist[i].nalspVendorid=(octet)atoi(&lb[j]);
		    		break;
		    	} 
		    }
		}
	}
  fclose(f);
}
//																			***009 End

///////////////////////////////////////////////////////////////////////
//	Save Preferences to NALSERV.INI

void SavePreferences ()
{	char	WinDir[144],*p;
	static char *yes="Yes",*no="No";
	int		i;
	FILE	*f;
	
	WinDir[0]=0x00;
	GetWindowsDirectory(WinDir, 144);				//make path in windows directory
	strcat(WinDir,inifile);
	if ((f=fopen(WinDir,"w"))==NULL) return;
	fprintf(f,"[General]\n");						//									***212
	fprintf(f,"nalsdir=%s\n",nalsdir);
	fprintf(f,"MinimizeStatus=%s\n",(MinimizeStatus?yes:no));
	fprintf(f,"InhibitTX=%s\n",(InhibitTX?yes:no));	//									***013
	fprintf(f,"UsePTP=%s\n",(UsePTP?yes:no));		//									***212
	fprintf(f,"UseMSTP=%s\n",(UseMSTP?yes:no));		//									***217
	fprintf(f,"OurPeername=%s\n",OurPeername);
	fprintf(f,"OurVendorName=%s\n",OurVendorName);
	fprintf(f,"OurVendorID=%d\n",OurVendorID);
	fprintf(f,"OurInstance=%d\n",OurInstance);
	fprintf(f,"WhoIsIAmInterval=%d\n",WhoIsIAmInterval);
	fprintf(f,"TimeSynchInterval=%d\n",TimeSynchInterval);	//							***016
	fprintf(f,"BACnetTseg=%d\n",Tseg/10);
	fprintf(f,"BACnetTout=%d\n",Tout/10);
	fprintf(f,"BACnetNretry=%d\n",Nretry);
	fprintf(f,"MaxSizeALapdu=%d\n",MaxSizeALapdu);
	fprintf(f,"MaxTXapduLen=%d\n",MaxTXapduLen);
	fprintf(f,"MaxRXapduLen=%d\n",MaxRXapduLen);
	fprintf(f,"WindowSize=%d\n",WindowSize);
	p=(SegmentedRX?yes:no);
	fprintf(f,"SegmentedRX=%s\n",p);
	p=(SegmentedTX?yes:no);
	fprintf(f,"SegmentedTX=%s\n",p);
	p=(passive?yes:no);							//										***216
	fprintf(f,"Passive=%s\n",p);				//										***216
	p=(ispromiscuous?yes:no);
	fprintf(f,"PTPsnet=%u\n",PTPsnet);				//									***212 Begin
	fprintf(f,"PTPslen=%u\n",PTPslen);
	fprintf(f,"PTPsadr=");
	for(i=0;i<8;i++) fprintf(f,"%.2X",PTPsadr[i]);
	fprintf(f,"\n");								//									***212 End
	fclose(f);
	PrefsDirty=FALSE;
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

/////////////////////////////////////////////////////////////////////// 
//	Convert 8 HEX chars to binary dword (big endian)
//in:	src		points to 8 hex chars
//		dst		points to where dword value goes
void cvhex8be(char *src,dword *dst)
{	union {dword dw; char c[4];}u;

	cvhex(cvhex(cvhex(cvhex(src,&u.c[0]),&u.c[1]),&u.c[2]),&u.c[3]);
	*dst=u.dw;
	return;
}

/////////////////////////////////////////////////////////////////////// 
//	Convert 4 HEX chars to binary dword (big endian)
//in:	src		points to 4 hex chars
//		dst		points to where word value goes
void cvhex4be(char *src,word *dst)
{	union {word w; char c[2];}u;

	cvhex(cvhex(src,&u.c[0]),&u.c[1]);
	*dst=u.w;
	return;
}

/////////////////////////////////////////////////////////////////////// 
//	Convert n*2 HEX chars to binary byte [n] array (little endian)			//	***009 Begin
//in:	src		points to n*2 hex chars
//		dst		points to where n byte value goes
void cvhexn(char *src,byte *dst,int n)
{	int	i;
	for (i=0;i<n;i++)
		src=cvhex(src,&dst[i]);
	return;																	//	***009 End
}

/////////////////////////////////////////////////////////////////////// 
//	Convert HEX chars to binary dword (little endian)
//in:	src		points to hex chars
//out:	dword value
dword cvhexdw(char *src)
{	dword dw;
	
	dw=0;
	while (isxdigit(*src))
	{	dw<<=4;
		if (isdigit(*src))
			dw+=(dword)(*src-'0');
		else
			dw+=(dword)((*src&0xDF)-55);
		src++;
	}
	return dw;
}

/////////////////////////////////////////////////////////////////////// 
//	Convert Big Endian Word to little endian word
//in:	bw		points to big endian word
//out:	little endian word

word wpbige(word *bw)					//									***003 Begin
{	return wbige(*bw); }

word wbige(word bw)
{	byte	t;
	union { word w; byte b[2];} u;
	u.w=bw;								//									***003 End
	t=u.b[0];
	u.b[0]=u.b[1];
	u.b[1]=t;
	return u.w;
}

//																			***009 Begin
////////////////////////////////////////////////////////////////////// 
//	Transmit an Iam APDU
//in:	lport 		is the logical destination port
//		dnet		is the destination network
//		radr		is the router address or NULL										***214
//		rlen		is the size in octets of radr (if rlen==0 then broadcast)			***214
//out:	error code or NALSE_Success

int APIENTRY NALSAPItransmitIam(octet lport,dword dnet,octet *radr,octet rlen) //		***214

{	NALSpacket	np;
	octet		pdu[16];

	if (BadPort(lport)) return NALSE_InvalidPort;	//									***212
	BuildNALSpacket(&np,lport,dnet,radr,rlen,false,NORMAL_MESSAGE);	//					***214
	np.nalsAPDU=&pdu[0]; 
	np.nalsAlen=(uint)eIAmService(&pdu[0],us.go.object_id,us.max_apdu_length_accepted,	//	***015
									(word)us.segmentation_supported,us.vendor_id);
	return NALSAPIrequest(&np);
}
////////////////////////////////////////////////////////////////////// 
//	Transmit a Whois APDU
//in:	lport 		is the logical destination port
//		dnet		is the destination network
//		radr		is the router address or NULL										***214
//		rlen		is the size in octets of radr (if rlen==0 then broadcast)			***214
//out:	error code or NALSE_Success

int APIENTRY NALSAPItransmitWhois(octet lport, dword dnet,octet *radr,octet rlen, //	***214
								  word hasrange, dword rangelow, dword rangehi)	//***015***212

{	NALSpacket	np;
	octet		pdu[16];

	if (BadPort(lport)) return NALSE_InvalidPort;	//									***212
	BuildNALSpacket(&np,lport,dnet,radr,rlen,false,NORMAL_MESSAGE);	//					***214
	np.nalsAPDU=&pdu[0]; 
	np.nalsAlen=eWhoIsService(&pdu[0],hasrange,rangelow,rangehi);	//					***015
	return NALSAPIrequest(&np);
//																						***009 End
}

//																						***016 Begin
////////////////////////////////////////////////////////////////////// 
//	Transmit an TimeSync APDU
//in:	lport 		is the logical destination port
//		dnet		is the destination network
//		radr		is the router address or NULL										***214
//		rlen		is the size in octets of radr (if rlen==0 then broadcast)			***214
//out:	error code or NALSE_Success

int APIENTRY NALSAPItransmitTimeSynch(octet lport,dword dnet,octet *radr,octet rlen) //	***214

{	NALSpacket	np;
	octet		pdu[16];
	struct tm 	*nt;
	time_t		long_time;					//time in seconds since 1970

	if (BadPort(lport)) return NALSE_InvalidPort;	//									***212
	BuildNALSpacket(&np,lport,dnet,radr,rlen,false,NORMAL_MESSAGE);	//					***214
	np.nalsAPDU=&pdu[0];
	time(&long_time);                     	//Get time as long integer
	nt=localtime(&long_time);      			//Convert to local time
	if (nt->tm_wday==0)
		nt->tm_wday=7;						//convert 1=Mon...7=Sun	
	np.nalsAlen=(uint)eTimeSynchService(&pdu[0],(word)((nt->tm_year)+1900),(octet) ((nt->tm_mon)+1),
										(octet) nt->tm_mday,(octet) nt->tm_wday,(octet) nt->tm_hour,
										(octet) nt->tm_min,(octet) nt->tm_sec,0);
	return NALSAPIrequest(&np);
}
////////////////////////////////////////////////////////////////////// 
//	Build a NALSpacket for transmission
//in:	np			points to the NALSpacket
//		lport 		is the logical destination port
//		dnet		is the destination network
//		radr		is the router address or NULL										***214
//		rlen		is the size in octets of radr (if rlen==0 then broadcast)			***214
//		expextreply	true/false
//		priority	is the message priority
//out:	error code or NALSE_Success

void BuildNALSpacket(NALSpacket *np,octet lport,dword dnet,octet far *radr,octet rlen, //	***214
					BOOL expectreply,int priority)

{	np->nalsExpectReply=expectreply;
	np->nalsPri=priority;
	np->nalsLport=lport;
	np->nalsDnet=(uint)dnet;
	np->nalsDlen=0;									//always broadcast					***214 Begin
	np->nalsRlen=rlen;
	if (rlen!=0)
		memcpy(np->nalsRadr,radr,rlen); 			//send to router					***214 End
	np->nalsSnet=0;									//don't include source routing stuff ***023
	np->nalsSlen=0;									//									 ***023
	return;
//																			***016 End
}

//////////////////////////////////////////////////////////////////////		***100 Begin
//	Allocate some space
//	in:	size	amount to allocate
//	out:	pointer or NULL
//

LPVOID GMALLOC (dword size)
{	//return GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT,size);			//allocate space	***213
	return mymalloc(size);													//					***213
}

//////////////////////////////////////////////////////////////////////
//	Free some space that was previously Allocated
//	in:	size	amount to allocate
//	out:	pointer or NULL
//
void GFREE (LPVOID mp)
{	//GlobalFreePtr(mp);												//						***213
	myfree(mp);															//						***213
}																		//	***100 End
//																		//	***100 Begin
//	*** MAC layer interface routines ***
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// Initialize the MAC layer Interface
//
//	out:	MAE_Success or MAC_*** (error)

word MACPInit()
{	ULONG	NameLength=128;					//						***210 Begin
	TCHAR	AdapterNames[128];				//the UNICODE NT adapter names \Device\bacmac1(0x0)\Device\bacmac2(0x0)...(0x0)(0x0)
	TCHAR	MacDriverNames[128];			//the UNICODE NT mac driver names \Device\DC21X41(0x0)\Device\COM200202(0x0)...(0x0)(0x0)
	char	p[128],b[128];
	TCHAR	*ap,*dp;						//						***210 End
	
	ULONG	Filter;
	int		i,j;												//	***110***210
	long	n;													//	***200

//	Get Device name													***210 Begin
	Packet32GetDeviceNames(AdapterNames,&NameLength,export);	//this will be \Device\bacmac1...	

//	Get MacDriver name
	NameLength=128;												//	***200
	Packet32GetDeviceNames(MacDriverNames,&NameLength,xbind);	//this will be \Device\DC21X41... (eg.)

//	Determine the number of bound adapters from asciiz AdapterNames=name0 name1 ... namex 0
	boundadapters=0;
	ap=AdapterNames;
	dp=MacDriverNames;

//	Open the adapters
	for (i=0;i<MAXADAPTERS;i++)
	{	if ((*ap==0)||(*dp==0)) break;
//  ...there's probably an easier way to do this without all this casting
		wcsncpy ((wchar_t *)&Adapter[i].AdapterName[0],(wchar_t *)ap,wcslen((wchar_t *)ap));
		ap+=(wcslen((wchar_t *)ap)+1)*sizeof (wchar_t);
		wcsncpy ((wchar_t *)&Adapter[i].MacDriverName[0],(wchar_t *)dp,wcslen((wchar_t *)dp));
		dp+=(wcslen((wchar_t *)dp)+1)*sizeof (wchar_t);
		Adapter[i].hFile=Packet32OpenAdapter(Adapter[i].AdapterName);
		if (Adapter[i].hFile == NULL)
		{	CloseAdapters(boundadapters,0);
			return MAE_OpenAdapterFailed;
		}
		boundadapters++;
	}

//	Initialize Adapter parameters and get some memory
	for (i=0;i<boundadapters;i++)
	{
		Adapter[i].BufferSize=1514;									//get max for ethernet always!
		for (j=0;j<MAXREADREQS;j++)									//			***110 Begin
		{	Adapter[i].hMem[j]=GlobalAlloc(GMEM_MOVEABLE,1514);	//allocate and lock rx buffers
			Adapter[i].lpMem[j]=GlobalLock(Adapter[i].hMem[j]);
		}														//			***110 End
		Adapter[i].hMem2=GlobalAlloc(GMEM_MOVEABLE,1514);	//allocate and lock tx buffer
		Adapter[i].lpMem2=GlobalLock(Adapter[i].hMem2);
		Adapter[i].Alen=MACPgetaddr((word)i,&Adapter[i].Adr[0]);
		if (Adapter[i].Alen==6)
			Adapter[i].AdapterType=ETH_802_3_ADAPTER;	
		else
			Adapter[i].AdapterType=ARCNET_ADAPTER;

		UnicodeToAscii(b,(wchar_t *)&Adapter[i].MacDriverName[0],sizeof (b));
		sprintf(p,"Logical Bound Adapter[%u] = %s",i,b);
		MakeLog (p,false);
///	Set packet filters
		Filter=((ispromiscuous)&&(Adapter[i].AdapterType==ETH_802_3_ADAPTER))?NDIS_PACKET_TYPE_PROMISCUOUS:NDIS_PACKET_TYPE_DIRECTED|NDIS_PACKET_TYPE_BROADCAST;	//	***NALS***200***210
		Packet32SetFilter(Adapter[i].hFile,Filter);

//	Now Start Some Reads						//							***110 Begin
		for (j=0;j<MAXREADREQS;j++)
		{	Adapter[i].PacketRX[j]=Packet32AllocatePacket(Adapter[i].hFile,true);	//allocate and create event	***140
			if (Adapter[i].PacketRX[j]!=NULL)
			{	if (Adapter[i].AdapterType==ETH_802_3_ADAPTER)	//		***200 Begin
					n=sizeof(PKT8022);			
				else
					n=sizeof(PKTARC);												//(514 without check bytes)
				Packet32InitPacket(Adapter[i].PacketRX[j],Adapter[i].lpMem[j],n);	//	***200 End
				RXEvents[(i*MAXREADREQS)+j]=((LPPACKET)Adapter[i].PacketRX[j])->OverLapped.hEvent;	//save event handle		***140
				Packet32ReceivePacket(Adapter[i].hFile,Adapter[i].PacketRX[j],FALSE,&Adapter[i].PacketLength);	//start the read..don't wait
			}
			else
			{	CloseAdapters(boundadapters,i+1);
				return MAE_NoFreeBuffers;
			}
		}													//							***110 End
		Adapter[i].PacketTX=NULL;							//transmit not in progress
	}														//							***210 End
	if ((hListener=_beginthread(Listener,0,NULL))==-1)	//start a thread for listening	***140								
	{	CloseAdapters(boundadapters,boundadapters);		//								***210
		return MAE_CreateThreadFailed;					//									***140
	}
	if(UsePTP)											//need to start PTP listener	***212 Begin
	{	if((i=ptpInit())!=ptpERSuccess) return i;		//failed to init ptp
		ptpinitialized=true;
	}													//								***212 End
	if(UseMSTP)											//need to start MSTP			***217 Begin
	{	if((i=mstpInit())!=mstpERSuccess) return i;		//failed to init mstp
		mstpVarMaxInfoFrames (&i,false);				//read this from the mstp dll
		us.max_info_frames=(word)i;
		mstpVarMaxMaster (&i,false);					//read this from the mstp dll
		us.max_master=(word)i;
		mstpinitialized=true;
	}													//								***217 End
	return MAE_Success;
}

///////////////////////////////////////////	***210 Begin
///////////////////////////////////////////////////////////////////////////////////////////////////	
// Convert a Unicode String to an Ascii string
//	in:	ap		points to the ascii string
//		up		points to a unicode string
//		max		has the max to convert
//	out:			the number actually converted (excluding the 0)

word UnicodeToAscii (char *ap,wchar_t *up,word max)
{	word	n=0;
	
	while ((*up!=0)&&(n<max-1))
	{	*ap++=(char)(*up++);
		n++;
	}
	*ap=0;
	return n;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// Close Adapters and freeup memory
//	in:	nadapters	number to close
//		ntofree		number with allocated read/write
//
void CloseAdapters(int nadapters,int ntofree)
{	int		i,j;										//								***110

	for (i=0;i<nadapters;i++)
	{	if (i<ntofree)
//	Free up the memory we got
		{	for (j=0;j<MAXREADREQS;j++)
			{	GlobalUnlock(Adapter[i].hMem[j]);		//unlock and free rx buffer	and close the event					
				GlobalFree(Adapter[i].hMem[j]);
			}
			GlobalUnlock(Adapter[i].hMem2);				//unlock and free tx buffer
			GlobalFree(Adapter[i].hMem2);
		}

//	Close it up
		Packet32CloseAdapter(Adapter[i].hFile);
	}
}														//								***210 End

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// Deinitialize the MAC layer Interface
//
void MACPDeinit()
{	
	oktolisten=0;										//signal Listener to terminate	***140 Begin
	while(oktolisten==0)								//wait for handshake
		Sleep(100L);									//								***140 End

//	Reset the Adapter
//	Packet32ResetAdapter(Adapter[0].hFile);				//shut down events??			***128

//	Close the Adapters
	CloseAdapters(boundadapters,boundadapters);			//								***210
	if(ptpinitialized) ptpDeinit();						//must shut it down too			***212
	if(mstpinitialized) mstpDeinit();					//must shut it down too			***217
}

///////////////////////////////////////////////////////////////////////					//	***140 Begin
///////////////////////////////////////////////////////////////////////
// Poll for a Received Packet
//
//	in:		mp points to a mac packet
//	out:	true if we have a packet to process
//
BOOL MACPreceive(MApacket far *mp)
{	MApacket	*xmp;		//,*ymp;
	
	if(mstpinitialized)									//								***217 Begin
		if(mstpReceive(mp)) return true;				//got one from mstp				***217 End
	if(ptpinitialized&&checkptp)						//								***212 Begin
		if(ptpReceive(mp)) return true;					//got one from ptp
	checkptp=!checkptp;
	if (usedma==NULL)									//none available from NDIS
	{	if(ptpinitialized)
			if(ptpReceive(mp)) return true;				//got one from ptp
		return false;									//nothing to do
	}													//								***212 End
	mydisable;											//								***142
	xmp=usedma;											//pick one off the used list
	usedma=xmp->maLink;
	xmp->maLink=NULL;
	maused--;											//								***141
	myenable;											//								***142
	memcpy(mp,xmp,sizeof(MApacket));					//transfer data
	GlobalFreePtr(xmp);									//free it
	return true;										//we got one
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//I Am A Thread---I Am A Thread---I Am A Thread---I Am A Thread---I Am A Thread---
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//	Handle incoming received frames
//
void Listener(void *dummy)
{	MApacket	*mp,*xmp;
	dword		BytesReceived,Result,dw;
	BOOL		done;
	PKT8022		*pkt;
	PKTARC		*apkt;					//										***200 Begin
	byte		*bp;
	dword		n;						//										***200 End
	word		i;
	LPVOID		lpb;
	char		b[1024];
	int			maxevents,lport,levent;	//										***210

	while(oktolisten)		//loop forever (until shutdown by DeinitPort)	//	***210 Begin
	{	maxevents=boundadapters*MAXREADREQS;
		dw=WaitForMultipleObjects(maxevents,RXEvents,false,ListenTime);	//block until one of our ports gets something(but no longer than ListenTime
		if ((dw>=WAIT_OBJECT_0)&&(dw<(WAIT_OBJECT_0+maxevents))) //got something
		{	NEXTREAD=dw-WAIT_OBJECT_0;
			done=false;
			while (!done)										//get 'em all
			{	done=true;										//default case (will be true only when no more events to process)
				for (i=0;i<maxevents;i++)
				{	lport=NEXTREAD/MAXREADREQS;					//calc the logical port (adapter)
					levent=NEXTREAD-(lport*MAXREADREQS);		//calc the logical event
					if ((Adapter[lport].PacketRX[levent]==NULL)||
						((Result=Packet32WaitPacket(Adapter[lport].hFile,Adapter[lport].PacketRX[levent],&BytesReceived,FALSE))==ERROR_IO_INCOMPLETE))	//***131
					{	if ((++NEXTREAD)>=maxevents) NEXTREAD=0;
						continue;
					}
					ResetEvent(RXEvents[NEXTREAD]);				//reset the event signal manually
					switch(Result)
					{
					case ERROR_SUCCESS:							//we got a live one
//		assume it's for us or a broadcast!!
//																					***200 Begin
						if (Adapter[lport].AdapterType==ETH_802_3_ADAPTER)
						{	pkt=(PKT8022 *)Adapter[lport].lpMem[levent];
							if (IsEcho(0,pkt->pktSADR)) break;	//it's an echo(always lport=0)
						}
						else
						{	apkt=(PKTARC *)Adapter[lport].lpMem[levent];
							if (IsEcho(0,&apkt->ApktSID)) break;	//it's an echo (always lport=0)
						}
///		Note: we hear only broadcast echos
//		it's for us and it's BACnet
						if (maused>=XMAXMAS)
						{	RxNoMAs++;
							break;							//toss it!!!
						}
						if ((mp=GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT,sizeof(MApacket)))==NULL)
						{	RxNoMAs++;
							break;							//toss it!!!
						}
						if (Adapter[lport].AdapterType==ETH_802_3_ADAPTER)	//Ethernet
						{	mp->maLsap=pkt->pktLLC[0];				//get lsap
							mp->maBroadcast=1;						//assume it's a broadcast
							for (i=0;i<6;i++)						//setup source & destination addresses
							{	if (pkt->pktDADR[i]!=0xFF) mp->maBroadcast=0;	//it's directed
								mp->maDadr[i]=pkt->pktDADR[i];		//destination address
								mp->maSadr[i]=pkt->pktSADR[i];		//source address
							}
							mp->maLport=(octet)lport;					//logical board number
							mp->maAlen=6;							//address length for media type (always 6)
							i=(pkt->pktLEN[0]<<8)+(pkt->pktLEN[1]&0xFF)-3;	//this is the NPDU length	***123
							mp->maNlen=i;
							memcpy(mp->maNPDU,pkt->pktNPDU,i);
							n=sizeof(PKT8022);
						}
						else												//ARCNET
						{	mp->maBroadcast=(apkt->ApktDID==0)?1:0;		//DID==0 means broadcast
							mp->maDadr[0]=apkt->ApktDID;				//destination address
							mp->maSadr[0]=apkt->ApktSID;				//source address
							mp->maLport=(octet)lport;					//logical board number (always 0)
							mp->maAlen=1;								//address length for media type
							mp->maLsap=apkt->ApktLLC[0];				//get lsap
							bp=apkt->ApktNPDU;
							i=(word)(BytesReceived-6);					//account for SID,DID,SC and LLC
							mp->maNlen=i;
							memcpy(mp->maNPDU,bp,i);
							n=sizeof(PKTARC);
						}
						mp->maLink=NULL;
						mydisable;							//							***143
						maused++;
						if (maxmaused<maused) maxmaused=maused;
						if (usedma!=NULL)
						{	xmp=usedma;
							while (xmp->maLink!=NULL)			//put it on the used list
								xmp=xmp->maLink;
							xmp->maLink=mp;
						}
						else
							usedma=mp;
						myenable;							//							***143

 /*						pkt=(PKT8022 *)Adapter[0].lpMem[NEXTREAD];
						if (!IsEcho(0,pkt->pktSADR))			//it's not an echo(always lport=0)
//		Note: we hear only broadcast echos
//		it's for us and it's BACnet
						{
							if (maused>=XMAXMAS)
							{	RxNoMAs++;
								break;							//toss it!!!
							}
							if ((mp=GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT,sizeof(MApacket)))==NULL)
							{	RxNoMAs++;
								break;							//toss it!!!
							}
							mp->maLsap=pkt->pktLLC[0];			//get lsap
							mp->maBroadcast=1;					//assume it's a broadcast
							for (i=0;i<6;i++)					//setup source & destination addresses
							{	if (pkt->pktDADR[i]!=0xFF) mp->maBroadcast=0;	//it's directed
								mp->maDadr[i]=pkt->pktDADR[i];	//destination address
								mp->maSadr[i]=pkt->pktSADR[i];	//destination address
							}
							mp->maLport=0;						//logical board number (always 0)
							mp->maAlen=6;						//address length for media type (always 6)
							i=(pkt->pktLEN[0]<<8)+(pkt->pktLEN[1]&0xFF)-3;	//this is the NPDU length	***123
							mp->maNlen=i;
							memcpy(mp->maNPDU,pkt->pktNPDU,i);
							mp->maLink=NULL;
							mydisable;							//							***143
							maused++;
							if (maxmaused<maused) maxmaused=maused;
							if (usedma!=NULL)
							{	xmp=usedma;
								while (xmp->maLink!=NULL)			//put it on the used list
									xmp=xmp->maLink;
								xmp->maLink=mp;
							}
							else
								usedma=mp;
							myenable;							//							***143
						}
*/																//							***200 End
						break;
					default:									//some other error
						break;
					}
//		Here we've received a packet. We may be tossing it or keeping it.		
//		Free the last event
					Packet32FreePacket(Adapter[lport].PacketRX[levent]);
//		Start another receive event
					Adapter[lport].PacketRX[levent]=Packet32AllocatePacket(Adapter[lport].hFile,true);
					if (Adapter[lport].PacketRX[levent]!=NULL)
					{	Packet32InitPacket(Adapter[lport].PacketRX[levent],Adapter[lport].lpMem[levent],n);	//***200
						RXEvents[NEXTREAD]=((LPPACKET)Adapter[lport].PacketRX[levent])->OverLapped.hEvent;
						Packet32ReceivePacket(Adapter[lport].hFile,Adapter[lport].PacketRX[levent],FALSE,&Adapter[lport].PacketLength); //don't wait
					}
					if ((++NEXTREAD)>=maxevents) NEXTREAD=0;						//	***210 End
					done=false;
				}
			}
		}
		else
		{	if (dw==WAIT_FAILED)				//for some reason we couldn't do the wait
			{	lpb=GetSystemError();
				b[0]=0;
				strcpy(b,"Listener thread terminated because: ");
				strcat("Listener thread terminated because: ",lpb);
				MakeLog(b,TRUE);
				LocalFree(lpb);
			}
			if (dw!=WAIT_TIMEOUT) return;		//this will terminate the thread!
		}
	}											//go back and wait again
	oktolisten=2;								//handshake with main process
}

///////////////////////////////////////////////////////////////////////
//	Get a buffer containing the last system error message
//out:	far pointer to a buffer containing the message
//		note: caller must release this buffer using
//		LocalFree(lpMsgBuf);

LPVOID GetSystemError()
{	LPVOID	lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,NULL,
		GetLastError(),MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,0,NULL);
	return lpMsgBuf;
}
//																							***140 End    
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// Transmit a Packet
//
//	in:		mp points to a mac packet
//	out:	always assume success
//
void MACPtransmit(MApacket far *mp)
{	byte	*bp;									//for easy access
	word	n;
	int		i,lport;								//									***210
	dword	BytesReceived;							//									***131 Begin
	
	lport=(int)mp->maLport;							//									***210 Begin					
	if(lport==mstpport)								//handle mstp specially				***217 Begin
	{	mstpTransmit(mp);
		return;
	}												//									***217 End					
	if(lport==ptpport)								//handle ptp specially				***212 Begin
	{	ptpTransmit(mp);
		return;
	}												//									***212 End					
	if (Adapter[lport].PacketTX!=NULL)								//we have a transmit in progress
	{	Packet32WaitPacket(Adapter[lport].hFile,Adapter[lport].PacketTX,&BytesReceived,TRUE);	//wait for completion***145
		Packet32FreePacket(Adapter[lport].PacketTX);
	}
	Adapter[lport].PacketTX=Packet32AllocatePacket(Adapter[lport].hFile,false);	//		***140
	if (Adapter[lport].PacketTX == NULL) return;	//we couldn't allocate a packet		***131 End
//	build the packet to send
	bp=Adapter[lport].lpMem2;
	if (Adapter[lport].AdapterType==ETH_802_3_ADAPTER)				//					***200
	{	n=mp->maAlen;
		memcpy(bp,mp->maDadr,n);				//dest addr
		bp+=n;
		memcpy(bp,mp->maSadr,n);				//source addr
		bp+=n;
		n=mp->maNlen+3;									//must account for LLC
		*bp++=(n>>8)&0xFF;								//MS len
		*bp++=n&0xFF;									//LS len
		*bp++=mp->maLsap;								//DSAP
		*bp++=mp->maLsap;								//SSAP
		*bp++=3;										//UI
		memcpy(bp,mp->maNPDU,mp->maNlen);				//NPDU
		bp+=mp->maNlen;
		i=(64-17)-mp->maNlen;							//see if we're smaller than ethernet min
		if (i>0)										//we must pad
		{	memset(bp,0,i);
			bp+=i;
		}
	}
	else												//								***200 Begin
	{	*bp++=mp->maSadr[0];							//source addr
		if (mp->maBroadcast)
			*bp++=0;									//dest addr (0=ARCnet Broadcast)
		else
			*bp++=mp->maDadr[0];						//dest addr
		*bp++=0xCD;										//BACnet SC
		*bp++=mp->maLsap;								//DSAP
		*bp++=mp->maLsap;								//SSAP
		*bp++=3;										//UI
		memcpy(bp,mp->maNPDU,mp->maNlen);				//NPDU
		bp+=mp->maNlen;
	}													//								***200 End

	n=bp-Adapter[lport].lpMem2;							//n is the number of bytes to send

	Packet32InitPacket(Adapter[lport].PacketTX,Adapter[lport].lpMem2,n);	//init pointer and size
//	ship it
	Packet32SendPacket(Adapter[lport].hFile,Adapter[lport].PacketTX,FALSE);	//no WAIT for COMPLETION	***110***131
//																						***210 End
//	Packet32FreePacket(PacketTX);														***131
}

///////////////////////////////////////////////////////////////////////
// Get the address of a port
//	in:		lport	is the logical port
//			paddr	points to buffer to put address						
//	out:	the size of the address
//		paddr contains the address

word MACPgetaddr(word lport,octet *paddr)			//									***212
{	word	n;										

	if(lport==ptpport) return 0;					//ptp is special					***212
	if(lport==mstpport)								//mstp is also special				***217 Begin
	{	*paddr=mstpGetTS();
		return 1;
	}												//									***217 End
	n=Packet32GetAddress(Adapter[lport].hFile,(PBYTE)paddr);	//						***210 
	return n;									//										***200

}	
///////////////////////////////////////////////////////////////////////
// Get the rx/tx stats.maximum size of an NPDU.
//	in:		lport		the logical port
//	out:	the maximum NPDU size

word MACPmaxNPDUlength(word lport)
{	word	n;										//									***200 Begin

	if(lport==ptpport||lport==mstpport) return 501;			//ptp/mstp is special		***212***217
	n=Packet32GetMaxFrameSize(Adapter[lport].hFile)-3;		//							***210
	return n;										//									***200 End
}

///////////////////////////////////////////////////////////////////////
// Get the rx/tx stats.
// 
//	in:		pstats		points to place to put stats
//	out:	pstats		has the stats

void MACPgetstats(MACStatTable *pstats)				//									***212
{
	pstats->MTotalTxPackets=TxCounter;
	pstats->MTotalRxPackets=RxCounter;
	pstats->MIgnoredRxPackets=RxToss;
	pstats->MTimeouts=RxTO;
	pstats->MNoMAs=RxNoMAs;												//				***140
	pstats->MErrors=RxError;											//				***NALS
}
//																						***NALS Begin
///////////////////////////////////////////////////////////////////////
//	Get MAC Layer Length of Addresses on a Logical Board

word MACPgetlen(word lbd)
{	if(lbd==ptpport)						//treat PTP specially here					***212
		return 0;							//											***212
	if(lbd==mstpport)						//treat MSTP specially here					***217
		return 1;							//											***217
	return (word)Adapter[lbd].Alen;										//				***210
}	

///////////////////////////////////////////////////////////////////////
//	Get List of Logical Boards
word MACPgetportlist(char *blist)
{	char	b[4096],*bp;					//											***210
	char	lpt[]={"000 "};					//											***210
	word	n;

	b[0]=0;									//											***210 Begin
	bp=b;
	for (n=0;n<boundadapters;n++)
	{	strcat(b,lpt);
		bp=&b[0]+strlen(b);
		UnicodeToAscii(bp,(wchar_t *)&Adapter[n].MacDriverName[0],sizeof (b));
		strcat(b,"\r\n");					//crlf
		lpt[2]++;
	}										//											***210 End
	if(ptpinitialized)						//get PTP port name							***212
		ptpCatName(b);						//											***212
	if(mstpinitialized)						//get MSTP port name						***217
		mstpCatName(b);						//											***217
	n=strlen(b);
	if (n<=512) strcpy(blist,b);
	else
	{	n=512;
		strncpy(blist,b,512);
	}
	return n;					
}	
//																						***NALS End
//																						***100 End
//																						***211 Start
///////////////////////////////////////////////////////////////////////
//	Create our icon in the systray

void CreateTrayIcon()
{	NOTIFYICONDATA	tnd;

  strcpy(tnd.szTip,version);
  tnd.uFlags=NIF_MESSAGE + NIF_ICON + NIF_TIP;
  tnd.uID=100;
  tnd.cbSize=sizeof(tnd);
  tnd.hWnd=hStatus;
  tnd.uCallbackMessage = WM_SYSTRAY;
  tnd.hIcon = hIcon;
  Shell_NotifyIcon(NIM_ADD, &tnd);
}

///////////////////////////////////////////////////////////////////////
//	Delete our icon from the systray

void DeleteTrayIcon()
{	NOTIFYICONDATA	tnd;

  tnd.uID = 100;
  tnd.cbSize = sizeof(tnd);
  tnd.hWnd = hStatus;
  tnd.uCallbackMessage = WM_SYSTRAY;
  Shell_NotifyIcon(NIM_DELETE, &tnd);
}
//																			***211 End