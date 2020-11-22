/*---------------------------------------------------------------------------
	©1998, PolarSoft Inc. and National Institute for Standards and Technology
	
module:		NALDUM.C
desc:		Dummy BACnet Network & Application Layer Server DLL v0.50
authors:	David M. Fisher, Jack R. Neyer
last edit:	22-Jan-98 [001] DMF First Cut
-----------------------------------------------------------------------------*/ 

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "nalsapi.h"
#include "resource.h"
///////////////////////////////////////////////////////////////////////
//	Module Constants
#define		TimerID				1					//main timer identifier
#define		TimerInterval		100					//100 ms
#define		WM_USER_INITIATE	(WM_USER+1)
#define		BACnetLSAP			0x82
#define		InitialHopCount		255
///////////////////////////////////////////////////////////////////////
// Global variables
const char	szAppName[]="NALSERV";
const char	version[] = "Dummy BACnet NL&AL Server DLL v0.50";
const octet	fakeport[6]= {0x11,0x22,0x33,0x44,0x55,0x66};

#define		vstring		"v0.50"
#define		ver100		50							//version*100

HWND		hStatus;								//handle of our Status Dialog
HANDLE		hDLLInst; 
HBITMAP		hRed,hGreen;
int			DLLusers=0;

//---------------------------------------------------------------------
long APIENTRY MainWndProc(HWND, UINT, UINT, LONG);
void StartUp(void);
void ClearItems(HWND hWnd,int nDlgItem);
int GetListCount(int nDlgItem);
void RemoveItem(int nDlgItem,int nIndex);
int AddItem(HWND hWnd,int nDlgItem,const char *p);
int EnterItem(int nDlgItem,const char *p);
void MakeLog (char *what, int needstimestamp);
///////////////////////////////////////////////////////////////////////
//	DLL entrypoint called from Windows

BOOL APIENTRY DllMain(HANDLE hInst, DWORD ul_reason_being_called, LPVOID lpReserved)
{
    hDLLInst = hInst;
    return 1;
	UNREFERENCED_PARAMETER(ul_reason_being_called);
	UNREFERENCED_PARAMETER(lpReserved);
}

//-------------------------- Begin API Functions ----------------------
///////////////////////////////////////////////////////////////////////
//	DLL initialization called from Application

int APIENTRY NALSAPIinit(word *vp)
{   WNDCLASS	wc;

	*vp=ver100;									//return our version
	if (DLLusers++>0) return NALSE_Success;		//just return if already inited
	
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = DLGWINDOWEXTRA;
    wc.hInstance = hDLLInst;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = szAppName;
    if (!RegisterClass(&wc))
    	return NALSE_FailedClassRegistration;	//failed
    
//	create the windows we need and allocate a timer
    hStatus = CreateDialog(hDLLInst,"NALDUM",0,NULL);
    if (!hStatus) return NALSE_CantCreateWindow;
    
    SendMessage (hStatus,WM_USER_INITIATE,0,0L); //Get things going
    return NALSE_Success;
}

///////////////////////////////////////////////////////////////////////
//	DLL deinitialization called from Application

void APIENTRY NALSAPIdeinit()
{	
	if (--DLLusers==0)
	{
		DestroyWindow(hStatus);
		UnregisterClass(szAppName,hDLLInst);
	}
}    
    
///////////////////////////////////////////////////////////////////////
//	Get MAC Layer Port List, called from Application

word APIENTRY NALSAPIgetportlist(char *pl)
{	strcpy(pl,"Dummy");
	return 1;
}

///////////////////////////////////////////////////////////////////////
//	Get MAC Layer Length of Addresses on a Logical Board, called from Application

word APIENTRY NALSAPIgetlen(word lbd)
{	return 6;										//like ethernet
}

///////////////////////////////////////////////////////////////////////
//	Get MAC Layer Address of a Logical Board, called from Application
// in:	lport 	has the logical port
//		ap		points to buffer to fill in with address
// out:	0		invalid logical port
//		!=0		address length
word APIENTRY NALSAPIgetaddr(word lport, octet *ap)
{	memcpy(ap,fakeport,6);	
	return 6;
}

///////////////////////////////////////////////////////////////////////					***019 Begin
//Test for received APDU, called from application
//in:	np			points to a NALSpacket which will be filled in
//		apmax		size of the buffer ap ppoints to
//		ap			points to a buffer which the APDU will be copied into
//out:	NALSE_Success					there was an APDU, and it was copied successfully
//		NALSE_ReceivedPacketTooLarge	can't fit the packet into apmax
//										In this case, np is filled in, but ap is not...
//		NALSE_NoPacketsAvailable		no packets in the queue

int APIENTRY NALSAPIgetapdu(NALSpacket *np,word apmax,octet *ap)
{
		return NALSE_NoPacketsAvailable;
}

///////////////////////////////////////////////////////////////////////
//	Get Next APDU, called from Application.
//
//         Note: Is not promiscuous!!
// out:	TRUE  if packet is available, caller must free the APDU packet
//		FALSE if no packet in the queue

BOOL APIENTRY NALSAPIgetpacket(NALSpacket *np)
{
	return FALSE;
}


///////////////////////////////////////////////////////////////////////
//Transmit APDU, called from application
//in:	np			points to a NALSpacket containing description of the packet to send
//					except that the np->nalsAPDU field is not initialized!
//		ap			points to the APDU to be copied
//out:	error code or NALSE_Success

int APIENTRY NALSAPIcopyrequest(NALSpacket *np,octet *ap)
{
	return NALSE_Success;
}

///////////////////////////////////////////////////////////////////////
//	Transmit APDU, called from Application
//
//in:	np				points to NALSpacket to control transmit
//		np->nalsDlen	=0 if broadcast
//		np->nalsSlen	=0 normally									***019
//						>0 if source routing to be added here		***019
//out:	error code or NALSE_Success

int  APIENTRY NALSAPIrequest(NALSpacket *np)
{
	return NALSE_Success;
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

word APIENTRY NALSAPIgetmaxresponse(word Lport)
{
	return 5;
}

///////////////////////////////////////////////////////////////////////
//	DLL statistics called from Application

void APIENTRY NALSAPIgetstatistics(NALSstats *ps)
{	return;
}

///////////////////////////////////////////////////////////////////////
//	turn receive filtering on/off

void APIENTRY NALSAPIreceivefilter(BOOL rf)
{	return;
}

///////////////////////////////////////////////////////////////////////
//	add a string to the status log list
//in:	ls		points to the asciz string to add

void APIENTRY NALSAPIputlog(char *ls)
{	MakeLog(ls,FALSE);
}

////////////////////////////////////////////////////////////////////////
//	Transmit NPDU, called from Network Layer
//
//in:	mp			points to NALSpacket to control transmit
//out:	MACAPI error code or MAE_Success

int  APIENTRY NALSAPItransmit(MApacket *mp)
{
	return MAE_Success;
}

////////////////////////////////////////////////////////////////////// 
//	Transmit an Iam APDU
//in:	lport 		is the logical destination port
//		dnet, dadr	is the destination network and address
//		alen		is the size in octets of dadr (if alen==0 then broadcast)
//out:	error code or NALSE_Success

int  APIENTRY NALSAPItransmitIam(octet lport,dword dnet,octet *dadr,octet alen)
{
	return NALSE_Success;
}

////////////////////////////////////////////////////////////////////// 
//	Transmit a Whois APDU
//in:	lport 		is the logical destination port
//		dnet, dadr	is the destination network and address
//		alen		is the size in octets of dadr (if alen==0 then broadcast)
//out:	error code or NALSE_Success

int  APIENTRY NALSAPItransmitWhois(octet lport, dword dnet,octet *dadr,octet alen,word hasrange, dword rangelow, dword rangehi)
{
	return NALSE_Success;
}

////////////////////////////////////////////////////////////////////// 
//	Transmit an TimeSync APDU
//in:	lport 		is the logical destination port
//		dnet, dadr	is the destination network and address
//		alen		is the size in octets of dadr (if alen==0 then broadcast)
//out:	error code or NALSE_Success

int  APIENTRY NALSAPItransmitTimeSynch(octet lport,dword dnet,octet *dadr,octet alen)
{
	return NALSE_Success;
}

////////////////////////////////////////////////////////////////////// 
//	Find out the maximum length of an NPDU on a port
//in:	lport 		is the logical destination port
//out:	length in octets

word APIENTRY NALSAPImaxNPDUlength(word lport)
{
	return 1497;
}

//////////////////////////////////////////////////////////////////////
//	get a string from the status log list
//in:	ls		points to an 81 byte buffer we can put the string in
//out:	true	if a string was returned in ls
//		false	if none available (ls is stuffed with asciz)

BOOL APIENTRY NALSAPIgetlog(char *ls)
{
		return FALSE;
}

///////////////////////////////////////////////////////////////////////
//	register a Network Layer server callback
//in:	srv		points to a callback function (or NULL)
//out:	0		if server was registered successfully
//		!0		an error occurred

int APIENTRY NALSAPIregisterNLserver(FARPROC srv)
{
	return NALSE_Success;
}

///////////////////////////////////////////////////////////////////////
//	register an Application Layer server callback
//in:	srv		points to a callback function (or NULL)
//out:	0		if server was registered successfully
//		!0		an error occurred

int APIENTRY NALSAPIregisterALserver(FARPROC srv)
{
	return NALSE_Success;
}

///////////////////////////////////////////////////////////////////////
//	Display DLL Configuration Dialog called from Application

void APIENTRY NALSAPIconfigure()
{
}

///////////////////////////////////////////////////////////////////////
//	Main Window Message Processor

long APIENTRY MainWndProc(HWND hWnd,UINT message,UINT wParam,LONG lParam)
{   
    switch (message)
    {
	  case WM_USER_INITIATE:
	  	StartUp();
        return 0;
		
      case WM_CREATE:
       	return 0;

      case WM_DESTROY:
       	return 0;
       	
    }
   	return (DefWindowProc(hWnd, message, wParam, lParam));
}

///////////////////////////////////////////////////////////////////////
//	Start Up Initialization (after initial window display)

void StartUp()
{	char	b[64];
   	strcpy(b,version);
   	strcat(b," Restarted");
   	MakeLog(b,TRUE);
	ShowWindow(hStatus,SW_SHOWNORMAL);
}

///////////////////////////////////////////////////////////////////////
//	remove all list items from one of the lists in a dialog

void ClearItems(HWND hWnd,int nDlgItem)
{ SendDlgItemMessage(hWnd,nDlgItem,LB_RESETCONTENT,0, 0L);
}

///////////////////////////////////////////////////////////////////////
//	get the list count from one of the lists in Status Dialog

int GetListCount(int nDlgItem)
{ return (int) SendDlgItemMessage(hStatus,nDlgItem,LB_GETCOUNT,0, 0L);
}

///////////////////////////////////////////////////////////////////////
//	remove a list item from one of the lists in Status Dialog

void RemoveItem(int nDlgItem,int nIndex)
{ SendDlgItemMessage(hStatus,nDlgItem,LB_DELETESTRING,nIndex, 0L);
}

///////////////////////////////////////////////////////////////////////
//	add a list item to one of the lists in a dialog

int AddItem(HWND hWnd,int nDlgItem,const char *p)
{	int	r;
	r=(int) SendDlgItemMessage(hWnd,nDlgItem,LB_ADDSTRING,0,(LPARAM) ((LPSTR) p));
	return r;
}

///////////////////////////////////////////////////////////////////////
//	add and select a list item to one of the lists in Status Dialog

int EnterItem(int nDlgItem,const char *p)
{	int	r;
	r=(int) SendDlgItemMessage(hStatus,nDlgItem,LB_ADDSTRING,0,(LPARAM) ((LPSTR) p));
	SendDlgItemMessage(hStatus,nDlgItem,LB_SETCURSEL,r, 0L);
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

