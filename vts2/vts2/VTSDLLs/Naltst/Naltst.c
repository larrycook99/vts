/*--------------------------------------------------------------------
module:		NALTST.C
desc:		test program to Start NALSERV
authors:	David M. Fisher, Jack R. Neyer
last edit:	7-Jun-95 [001] First Cut
----------------------------------------------------------------------*/
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nalsapi.h"
#include "service.h" 
#include "bacprim.h"
#include "db.h"

//#include <afxwin.h>  // MAG
///////////////////////////////////////////////////////////////////////
//	Module Constants
#define		TimerID				1				//main timer identifier
#define		TimerInterval		100				//100 ms
///////////////////////////////////////////////////////////////////////
// Global variables
const char szAppName[]="NALTST";
const char version[] = "NALSERV Tester v1.00";
int SendAPDUCounter=10;							//TEMP
BOOL SentAPDU=FALSE;							//TEMP
int SendNPDUCounter=10;							//TEMP
BOOL SentNPDU=TRUE;								//TEMP
#define		WM_USER_INITIATE		(WM_USER+1)
///////////////////////////////////////////////////////////////////////
//	function prototypes
int WINAPI WinMain(HANDLE, HANDLE, LPSTR, int);
long WINAPI MainWndProc(HWND, UINT, UINT, LONG);
//---------------------------------------------------------------------
void TimerEvent(void);
void MainLoop(void);
BOOL SendAPDU (int,octet,int,octet,octet);
BOOL SendNPDU (int);
void ALServerCallback (NALSpacket *);

///////////////////////////////////////////////////////////////////////
//	Main Window Procedure, Message Loop

int pascal WinMain(HANDLE hInstance,HANDLE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{   MSG			msg;
	HWND		hWnd;
	WNDCLASS	wc;
	//CButton		cbu; //MAG

	

    if (!hPrevInstance)
    {
	    wc.style = CS_HREDRAW | CS_VREDRAW;
	    wc.lpfnWndProc = MainWndProc;
	    wc.cbClsExtra = 0;
	    wc.cbWndExtra = 0;
	    wc.hInstance = hInstance;
	    wc.hIcon = LoadIcon(hInstance, "NALTST");
	    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	    wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
	    wc.lpszMenuName =  NULL;
	    wc.lpszClassName = szAppName;
    	if (!RegisterClass(&wc)) return (FALSE);
    }

	hWnd=CreateWindow(szAppName,version,
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT,CW_USEDEFAULT,
					CW_USEDEFAULT,CW_USEDEFAULT,
					NULL,NULL,hInstance,NULL);
    if (!hWnd) return (FALSE);

    if (!SetTimer(hWnd,TimerID,TimerInterval,NULL))
	{	DestroyWindow(hWnd);					//we don't need the window now
      	return (FALSE);
	}

	ShowWindow(hWnd, SW_SHOWMINIMIZED);
	UpdateWindow(hWnd);
    SendMessage (hWnd,WM_USER_INITIATE,0,0L); //Get things going (could this be done in-line??)
    
    while (GetMessage(&msg, NULL,0,0))
    {   
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
    return (msg.wParam);
}

///////////////////////////////////////////////////////////////////////
//	Main Window Message Processor

long WINAPI MainWndProc(HWND hWnd,UINT message,UINT wParam,LONG lParam)
{	word	v;
	char	b[64];
	int		i;
	
	switch (message)
    {    	 
      case WM_CREATE:
		return 0;
		
	  case WM_USER_INITIATE:
        if (i=NALSAPIinit(&v))
        {	sprintf(b,"NALSAPIinit failed with error %d",i);
        	MessageBox(hWnd,b,szAppName,MB_ICONSTOP);
         	DestroyWindow(hWnd);
        }
        else 
	        if (i=NALSAPIregisterALserver((FARPROC) ALServerCallback))	//register our server callback
	        {	sprintf(b,"NALSAPregisterALserver failed with error %d",i);
	        	MessageBox(hWnd,b,szAppName,MB_ICONSTOP);
	        }
        	NALSAPIreceivefilter(TRUE);				//we only want responses to our requests
       	return 0;

      case WM_TIMER:
        TimerEvent();								//perform timer stuff
        return 0;
        
      case WM_DESTROY:
		NALSAPIdeinit();
       	PostQuitMessage(0);
       	return 0;     
       	
    }
   	return (DefWindowProc(hWnd, message, wParam, lParam));
}
///////////////////////////////////////////////////////////////////////
//	Timer Event Handler (every 100ms)

void TimerEvent()
{	static int	InMainLoop=FALSE;			//lock flag						<--do we need this?
	static int	SecondsCounter=10;

	if ((SecondsCounter--)==0)				//a second has gone by
	{ SecondsCounter=10;					//reset downcounter
	  if (SendAPDUCounter>0) SendAPDUCounter--;		//update downcounters
	  if (SendNPDUCounter>0) SendNPDUCounter--;		//update downcounters
  	}
    if (InMainLoop) return;					//don't reenter this part yet	<--do we need this?
    InMainLoop=TRUE;						//lock out reentrancy			<--do we need this?
    MainLoop();								//perform main loop functions
    InMainLoop=FALSE;						//release reentrancy lock		<--do we need this?
}

///////////////////////////////////////////////////////////////////////
//	Main Loop (every 100ms)

void MainLoop()
{	NALSpacket np;

//TEMP
    if ((SendAPDUCounter==0) && (SentAPDU==FALSE))
    { 
	   	SendAPDU (CONF_REQ_PDU,0x1D,2000,0,0);	//NOTE: ID assigned by NALSERV
    	SentAPDU=TRUE;
    }
    if ((SendNPDUCounter==0) && (SentNPDU==FALSE))
    { 
    	SendNPDU (100);
    	SentNPDU=TRUE;
    }
//TEMP

	if (NALSAPIgetpacket (&np))				//Manage received packets to the CLIENT ie. responses to requests
	{
		if (np.nalsAPDU!=NULL)
			free(np.nalsAPDU);
	}    
    
    return;
}

////////////////////////////////////////////////////////////////////////
// The server callback interface to NALSERV
//
// in:	np pointer of a NALSpacket from NALSERV
//

void ALServerCallback (NALSpacket *np)
{
	ASDUREQ	*apr;
	octet	pdutype;

	apr=(ASDUREQ *)np->nalsAPDU;				//point to APDU header
	pdutype=((apr->asdureqPDUheader&ASDUpdutype)>>ASDUpdutypeshr); //get the pdu type field
	if (pdutype==CONF_REQ_PDU)					//send a CACK reply if confirmed request is received
	    SendAPDU (COMPLEX_ACK_PDU,apr->asdureqInvokeID,50,0,0);
	free(np->nalsAPDU);
	return;

}


////////////////////////////////////////////////////////////////////////
// Send an APDU
//
// in:	PDUtype = PDU type to send
//		id=invoke id
//		noctets = number of octets of data to be sent
//		o1 = PDU dependent
//		o2 = PDU dependent
// out:	TRUE=Successful, FALSE=Unsuccessful

BOOL SendAPDU (int PDUtype, octet id, int noctets, octet o1, octet o2)
{	int		i;
	NALSpacket np;
	ASDUREQ	*apr;
	ASDU	*ap;
	octet	*pp;
	octet	o;
	char	b[64];

	if ((apr=(ASDUREQ *)malloc(noctets+3))==NULL)		//allocate space for a big NPDU packet
	{
		sprintf(b,"NALSAPIinit failed -- No Free Space");
		MessageBox(NULL,b,szAppName,MB_ICONSTOP);
	 	return FALSE;
	 }
	ap=(ASDU *) apr;
	pp= (octet *) ap;
	np.nalsAPDU=pp;
	np.nalsPri=NORMAL_MESSAGE;
	np.nalsLport=1;

	np.nalsDnet=1;
	np.nalsRlen=0;								//no routing
	np.nalsRadr[0]=0x00;
	np.nalsRadr[1]=0x00;
	np.nalsRadr[2]=0x00;
	np.nalsRadr[3]=0x00;
	np.nalsRadr[4]=0x00;
	np.nalsRadr[5]=0x00;
	
	np.nalsDnet=0;
	np.nalsDlen=6;
	np.nalsDadr[0]=0x00;						//for now send to the SAGE
	np.nalsDadr[1]=0x40;
	np.nalsDadr[2]=0x05;
	np.nalsDadr[3]=0x11;
	np.nalsDadr[4]=0x11;
	np.nalsDadr[5]=0x11;
	np.nalsSnet=0;
	np.nalsSlen=0;								//no source routing
	switch(PDUtype)
	{
	case CONF_REQ_PDU:
		np.nalsExpectReply=TRUE;
		np.nalsRequestorisserver=FALSE;
		np.nalsAlen=noctets+4;					//APDU only (segmentation required)
		*pp++=0x00;								//Confirmed Request PDU
		*pp++=0x05;
		*pp++=0xff;								//invoke ID (filled in by NALSERV)
		*pp++=0x55;								//service choice
		o=0;
		for (i=1;i<=noctets;i++)				//fill APDU data with 0,1,...0xff,0,1...
		{	*pp++=o;
			o+=1;
		}
		break;
	case UNCONF_REQ_PDU:
		np.nalsExpectReply=FALSE;
		np.nalsRequestorisserver=FALSE;
		np.nalsAlen=2;							//APDU only
		*pp++=0x10;								//Unconfirmed Request PDU
		*pp++=0x08;								//who is
		break;
	case COMPLEX_ACK_PDU:
		np.nalsExpectReply=FALSE;
		np.nalsRequestorisserver=TRUE;
		np.nalsAlen=noctets+2;					//APDU only (segmentation required)
		*pp++=0x30;								//Complex ack PDU
		*pp++=id;								//invoke ID
		o=0;
		for (i=1;i<=noctets;i++)				//fill APDU data with 0,1,...0xff,0,1...
		{	*pp++=o;
			o+=1;
		}		
		break;
	default:
		free (apr);
		free (&np);
		sprintf(b,"NALSAPIinit failed -- Invalid PDU Type");
		MessageBox(NULL,b,szAppName,MB_ICONSTOP);
	 	return FALSE;
	 }
	
	if (i=NALSAPIrequest(&np))
	{	sprintf(b,"NALSAPIrequest failed with error %d",i);
		MessageBox(NULL,b,szAppName,MB_ICONSTOP);
		return FALSE;
	}
	else
		return TRUE;
		    	
}
////////////////////////////////////////////////////////////////////////
// Send an NPDU
//
// in:	noctets = number of octets of NPDU data to be sent
// out:	TRUE=Successful, FALSE=Unsuccessful

BOOL SendNPDU (int noctets)
{	int			i;
	MApacket 	*mp;
	octet		*pp,o;
	char		b[64];

	if ((mp = (MApacket *)malloc(sizeof (MApacket)))==NULL)	//allocate space for MA packet
	{
		sprintf(b,"NALSAPIinit failed -- No Free Space");
		MessageBox(NULL,b,szAppName,MB_ICONSTOP);
	 	return FALSE;
	 }
	mp->maLport=1;
	mp->maAlen=6;
	mp->maLsap=0x82;
	mp->maBroadcast=0;
	mp->maDadr[0]=0x00;						//for now send to the SAGE
	mp->maDadr[1]=0x40;
	mp->maDadr[2]=0x05;
	mp->maDadr[3]=0x11;
	mp->maDadr[4]=0x11;
	mp->maDadr[5]=0x11;
	mp->maSadr[0]=0x00;						//for now Jack's PC
	mp->maSadr[1]=0x00;
	mp->maSadr[2]=0x1B;
	mp->maSadr[3]=0x41;
	mp->maSadr[4]=0x79;
	mp->maSadr[5]=0x7A;

	pp=(octet *) &mp->maNPDU;
	
	*pp++=1;							//BACnet version
	*pp++=0x80;							//Control: no Sadr, Dadr, Pri=0, No response
	*pp++=5;							//message type: Router Available to Network
	mp->maNlen=noctets+3;
    o=0;
	for (i=1;i<=(noctets/2);i++)			//fill NPDU data with 0,0,0,1,0,2,0,3..0,ff,0,0,0.1..
	{	*pp++=0;
		*pp++=o;
		o+=1;
	}
	
	if (i=NALSAPItransmit(mp))
	{	sprintf(b,"NALSAPItransmit failed with error %d",i);
		MessageBox(NULL,b,szAppName,MB_ICONSTOP);
		return FALSE;
	}
	else
		return TRUE;
}
