/*--------------------------------------------------------------------------------
	©1995-2000, PolarSoft® Inc. and National Institute for Standards and Technology
	
module:		BACPRIM32.C
desc:		BACnet Encoding/Decoding Primitives DLL v2.37
authors:	David M. Fisher, Jack R. Neyer
last edit:	03-Mar-00 [237] DMF fix empty bitstring case for atoeBITSTRING
			15-Dec-99 [236] DMF 
							1.	change eEventNotification to encode real values for Analog types
							2.	add eWORD and fix eRECIPIENT to use it for network numbers
							3.	allow COV subscription lifetime to be dword (from word)
							4.	eGetEnrollmentSummary takes network as dword to allow VB calling
							5.	fix non-optional character string cases of zero length
							6.	add safestrlen
			12-Oct-98 [235] JN  fix characterstring encoding for VB4-VB5
								add service to eError
			08-Apr-98 [234] JN  add eSimpleAckPDU and eErrorPDU
			26-Feb-98 [233] JN  fix integer encoding
			09-Dec-97 [232]	DMF	revise for 32 bit DLL from v0.31
----------------------------------------------------------------------------------			
			30-Sep-96 [031] JN  made eDWORD external fixed dREAL	
			28-May-96 [030] DMF fix eAny for case of Xsched
			25-May-96 [029] DMF fix eEventNotification PropertyStates handling
			16-May-96 [028] JN  add list capabilities to raslist in eAny
								add wsched, xsched, evparm to eAny
								add	atotimeval, atoprim
			15-May-96 [027] JN  add list capabilities to skeys, tsrecip, dabind, reciplist, stavals in eAny
			14-May-96 [026] JN  fix reciplist check of opmax in eAny, fix atoeBITSTRING for empty strings
			10-May-96 [025] JN  add encoding for ttime, ddate, dtrange recip, setref in eAny 
			07-May-96 [024] JN  eliminate open/close tags for propref,lopref,calist,raslist in eAny, object id in eRECIPIENT
			06-May-96 [023] DMF fix lifetime in eSubscribeCOVService
								add enumtable support to eSelCrit
								add ePrivateTransferService
								add eGetEnrollmentSummaryService
								add eEventNotificationService
			25-Jan-96 [022] DMF add vbeREAL, vbeDOUBLE
			07-Dec-95 [021] JN  add raslist,lopref,reciplist,calist,dt to eAny, fix propref for array indices
			07-Dec-95 [020] JN  add relinquish control for commandable properties
			05-Dec-95 [019] JN  add object_id,bits,vtcl,propref,dabind, skeys, tsrecip to eAny
			04-Dec-95 [018] DMF add eListElementService
			30-Nov-95 [017] JN  add eVTOpenService,eVTDataService,eVTCloseService
			30-Nov-95 [016] DMF add ebool support
			29-Nov-95 [015] JN  add eReadPropConditionalService
			28-Nov-95 [014] DMF add eAtomicReadFileService, eAtomicWriteFileService, eDeleteObjectService, eCreateObjectService
			27-Nov-95 [013] DMF add eSubscribeCOVService,eCOVNotificationService
			21-Nov-95 [012] DMF add eWritePropService, eWritePropMultiple Service, ePropVal, eAny
			13-Oct-95 [011] DMF add eReadPropMultipleService
			21-Sep-95 [010] DMF add vbOBJECTID, eAcknowledgeAlarmService,eTextMessageService,eTIMESTAMP
			20-Sep-95 [009] DMF fix length encoding in eCHARSTRING
			13-Sep-95 [008] DMF add vbXXXX primitives 
			12-Sep-95 [007] JN  add remaining decoders
			11-Sep-95 [006] DMF change dUINT,dENUM,dOBJECTID,dCONTEXT to use single pointer, add dbeDWORD
			07-Sep-95 [005] DMF add eReadPropService,eReinitDeviceService,eTimeSynchService,eDeviceCommControlService,eGetAlarmSummaryService
			05-Sep-95 [004] DMF add eIAmService, eIHaveService, eWhoIsService,eWhoHasService
			30-Aug-94 [003] JN  added decoding
			30-Aug-95 [002] DMF fix unsigned loop counters
			22-Aug-95 [001] DMF First Cut (moved from NALSERV)
-----------------------------------------------------------------------------*/ 
#include <windows.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
/////////////////////////////////////////////////////////////////////// 
//	Local Function Prototypes

octet * atoeBITSTRING (octet *,char *,word);			//								***021
octet * atoeDATESTRING(octet *,char *);
octet * atoeTIMESTRING(octet *,char *);					//								***021
octet * atooct (octet *,char *);
octet * atoprim (octet *,word,char *);					//								***028
octet * atotimeval (octet *,word,char *);				//								***028
char *  cvhex(char *,byte *);
void    cvhexn(char *,byte *,int);						//								***019 Begin
dword   dDWORD (octet **,BOOL);							//								***006
octet * eAny (octet *,word,word,word,word,char *); 		//								***012 Begin
octet * eCONFREQ(octet *,octet);						//								***005 Begin
octet * eepENUM (octet *,char *,word);					//								***019 End
octet * eObjId (octet *,char *,word *);
word    ePropVal(octet *,word,word,octet,char *);
octet * eRECIPIENT(octet *,char *);						//	
word    eSelCrit(octet *,word,char *);					//								***015
octet * etagREAL(octet *, octet, float);				//								***023
octet * eTIMESTAMP(octet *,etimestamp *,octet);			//								***010
int     GetListCount(HWND);
word    GetListData(HWND,octet *,word,BOOL);			//								***014
int     GetListText(HWND,int,char *);					//								***012 End
word	safestrlen(char *);								//								***236
octet * APIENTRY eWORD (octet *, word ,octet );


///////////////////////////////////////////////////////////////////////					***236 Begin
//	get the length of a string without croaking on NULL

word safestrlen(char *m)
{
	if(m==NULL) return 0;						//now why doesn't strlen do this?
	return (word)strlen(m);
}												//										***236 End

///////////////////////////////////////////////////////////////////////					***012 Begin
//	get the list count from a List Box

int GetListCount(HWND hWnd)
{
	return (int) SendMessage(hWnd,LB_GETCOUNT,0, 0L);
}

///////////////////////////////////////////////////////////////////////
//	get the text from a list item in a List Box

int GetListText(HWND hWnd,int nIndex,char *p)
{
	return (int) SendMessage(hWnd,LB_GETTEXT,nIndex,(LPARAM) ((LPSTR) p));
}												//										***012 End

///////////////////////////////////////////////////////////////////////
//	get the text from a list item in a List Box and convert it to
//  an octet string
//in:	hWnd	handle to list box
//		os		octet string destination
//		nIndex	the list box index
//		hd		true if hex pairs, false if ascii
//out:	size of octet string in octets

word GetListData(HWND hWnd,octet *os,word nIndex,BOOL hd)
{	char	b[128];
	word	lb;

	GetListText(hWnd,(int)nIndex,b);			//first get the text
	lb=safestrlen(b);							//										***236
	if (hd)										//hex pairs
	{	lb>>=1;
		cvhexn(b,os,lb);
	}
	else										//straight ascii
		memcpy(os,b,lb);
	return lb;
}												//										***012 End

/////////////////////////////////////////////////////////////////////// 
//	Encode a Confirmed Request Header
//in:	op			points to a buffer to encode into
//		sc			service choice

octet *eCONFREQ(octet *op,octet sc)
{
	*op++=CONF_REQ_PDU<<4;
	*op++=0;										//max response filled in by NALSERV
	*op++=0;										//invokeid filled in by NALSERV
	*op++=sc;
	return op;
}													//									***005 End

///////////////////////////////////////////////////////////////////////					***234 Begin
//	Encode a Simple Ack PDU
//in:	op		points to buffer to encode into
//		id		invoke id
//		service	service ack choice
//out:	return	the number of octets encoded
//				or 0 meaning too complex
word APIENTRY eSimpleAckPDU (octet *op,octet id,octet service)
{	octet	*iop;

	iop=op;
	*op++=SIMPLE_ACK_PDU<<4;
	*op++=id;
	*op++=service;
	return (op-iop);
}
	///////////////////////////////////////////////////////////////////////					***234 Begin
//	Encode an Error PDU
//in:	op			points to buffer to encode into
//		id			invoke id
//		errorclass	the error class
//		errorcode	the errorcode
//		service		the service code													//***235
//out:	return	the number of octets encoded
//				or 0 meaning too complex
word APIENTRY eErrorPDU (octet *op,octet id,dword errorclass,dword errorcode,dword service)	//***235
{	octet	*iop;

	iop=op;
	*op++=ERROR_PDU<<4;
	*op++=id;
	*op++=(byte)service;																//***235
	op=eENUM(op,errorclass);
	op=eENUM(op,errorcode);
	return (op-iop);
}
//																						***234 End
///////////////////////////////////////////////////////////////////////					***023 Begin 
//	Encode an EventNotification Service
//in:	op		points to buffer to encode into
//		conf	0=unconfirmed, 1=confirmed 
//		procid	process identifier
//		devinst	device instance
//		evobjid	event object identifier
//		ts		timestamp
//		nclass	notification class
//		pri		priority
//		evtype	event type
//		ntype	notification type
//		msg		pointer to message to send (zero length means don't send it)
//		ackreq	0=no, 1=ack required
//		fromst	from event state
//		tost	to event state
//		nv		notification value
//		pstype	PropertyStates type (only used for evtype=1 (change-of-state))			***029
//out:	return	the number of octets encoded
//				or 0 meaning too complex

word  APIENTRY eEventNotificationService(octet *op,word conf,dword procid,dword devinst,
								dword evobjid,etimestamp *ts,word nclass,word pri,word evtype,
								word ntype,char *msg,word ackreq,word fromst,word tost,
								notifyvalue *nv,word pstype)	//						***029
{	octet *iop;
	word	objecttype,ml;							//									***236

	iop=op;											//remember starting point
	if (conf)
		op=eCONFREQ(op,confirmedEventNotification);
	else
	{	*op++=UNCONF_REQ_PDU<<4;
		*op++=unconfirmedEventNotification;
	}
	objecttype=(word)(evobjid>>22);					//remember the object type			***236
	op=eDWORD(op,procid,0x08,FALSE);				//process identifier
	op=etagOBJECTID(op,0x18,vbOBJECTID(DEVICE,devinst));
	op=etagOBJECTID(op,0x28,evobjid);
	op=eTIMESTAMP(op,ts,3);
	op=eDWORD(op,(dword)nclass,0x48,FALSE);
	op=eDWORD(op,(dword)pri,0x58,FALSE);
	op=eDWORD(op,(dword)evtype,0x68,FALSE);

	if ((ml=safestrlen(msg))>0)						//									***236
		op=eCHARSTRING(op,msg,0x78,ml,ANSI,0);		//									***236
	op=eDWORD(op,(dword)ntype,0x88,FALSE);
	if (ntype!=2)
	{	op=eDWORD(op,(dword)(ackreq?1:0),0x98,FALSE);
		op=eDWORD(op,(dword)fromst,0xA8,FALSE);
		op=eDWORD(op,(dword)tost,0xB8,FALSE);
		if (evtype>6) evtype=6;
		*op++=0xCE;									//open tag 12
		*op++=0x0E+(octet)(evtype<<4);				//open tag 'evtype'
		switch(evtype)
		{
		case 0:										//change-of-bitstring
			op=eBITSTRING(op,&nv->nvBits[0],0x08,nv->nvNumBits);
			op=eBITSTRING(op,(byte *)(&nv->nvFlags),0x18,4);
			break;
		case 1:										//change-of-state
			*op++=0x0E;								//new-state open tag		***029 Begin
			op=eDWORD(op,(dword)nv->nvEnum[0],(octet)(0x08+(pstype<<4)),FALSE);
			*op++=0x0F;								//new-state close tag		***029 End
			op=eBITSTRING(op,(byte *)(&nv->nvFlags),0x18,4);
			break;
		case 2:										//change-of-value
			*op++=0x0E;								//open tag 0
			if (nv->nvNumBits)						//it's a bitstring
				op=eBITSTRING(op,&nv->nvBits[0],0x08,nv->nvNumBits);
			else									//it's a value
				op=etagREAL(op,0x1C,nv->nvReal[0]);
			*op++=0x0F;								//close tag 0
			op=eBITSTRING(op,(byte *)(&nv->nvFlags),0x18,4);
			break;
		case 3:										//command-failure			***236 Begin
			if(objecttype==ANALOG_INPUT||
			   objecttype==ANALOG_OUTPUT||
			   objecttype==ANALOG_VALUE)			//analog types have real values
			{
				op=etagREAL(op,0x0C,nv->nvReal[0]);
				op=eBITSTRING(op,(byte *)(&nv->nvFlags),0x18,4);
				op=etagREAL(op,0x2C,nv->nvReal[1]);
			}
			else									//other types have enums
			{
				op=eDWORD(op,(dword)nv->nvEnum[0],0x08,FALSE);
				op=eBITSTRING(op,(byte *)(&nv->nvFlags),0x18,4);
				op=eDWORD(op,(dword)nv->nvEnum[1],0x28,FALSE);
			}
			break;									//							***236 End
		case 4:										//floating-limit
		case 5:										//out-of-range
			op=etagREAL(op,0x0C,nv->nvReal[0]);
			op=eBITSTRING(op,(byte *)(&nv->nvFlags),0x18,4);
			op=etagREAL(op,0x2C,nv->nvReal[1]);
			op=etagREAL(op,0x3C,nv->nvReal[2]);
			break;
//		default:									//complex-event-type
		}
		*op++=0x0F+(octet)(evtype<<4);				//close tag 'evtype'
		*op++=0xCF;									//close tag 12
	}
	
	return (op-iop);
}													//									***023 End

///////////////////////////////////////////////////////////////////////					***023 Begin 
//	Encode a GetEnrollmentSummary Service
//in:	op		points to buffer to encode into
//		wack	0=all, 1=acked, 2=notacked 
//		dvi		-1=no enrollment filter
//				-2=enrollment with address
//			net		is the network number
//			mlen	mac address length
//			maddr	mac address
//				else dvi is the device instance and net,mlen,maddr not used
//		procid	process identifier if dvi!=-1
//		evs		-1=no eventstate filter, else eventstate to filter
//		evt		-1=no eventtype filter, else eventtype to filter
//		pfn		-1=no priority filter, else pfn..pfx is the range of priorities
//		nf		-1=no notificationclass filter, else notification class
//out:	return	the number of octets encoded
//				or 0 meaning too complex

word  APIENTRY eGetEnrollmentSummaryService(octet *op,word wack,dword dvi,dword net,	//***236
							word mlen,octet *maddr,dword procid,word evs,word evt,word pfn,word pfx,word nf)
{	octet *iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,getEnrollmentSummary);
	op=eDWORD(op,(dword)wack,0x08,FALSE);			//0=all, 1=acked, 2=notacked
	if (dvi!=0xFFFFFFFF)							//enrollment filter
	{	*op++=0x1E;									//open context tag 1
			*op++=0x0E;								//open context tag 0
			if (dvi==0xFFFFFFFE)					//address
			{	*op++=0x1E;							//open context tag 1
				op=eUINT(op,net&0xFFFF);			//									***236
			    op=eOCTETSTRING(op,maddr,mlen);
				*op++=0x1F;							//close context tag 1
			}
			else									//device object
				op=etagOBJECTID(op,0x08,vbOBJECTID(DEVICE,dvi));
			*op++=0x0F;								//close context tag 0
			op=eDWORD(op,procid,0x18,FALSE);
		*op++=0x1F;									//close context tag 1
	}
	if (evs!=0xFFFF)
		op=eDWORD(op,(dword)evs,0x28,FALSE);		//event state
	if (evt!=0xFFFF)
		op=eDWORD(op,(dword)evt,0x38,FALSE);		//event type
	if (pfn!=0xFFFF)
	{	*op++=0x4E;									//open context tag 4
		op=eDWORD(op,(dword)pfn,0x08,FALSE);		//minpriority
		op=eDWORD(op,(dword)pfx,0x18,FALSE);		//maxpriority
		*op++=0x4F;									//close context tag 4
	}
	if (nf!=0xFFFF)
		op=eDWORD(op,(dword)nf,0x58,FALSE);			//event type
	return (op-iop);
}													//									***023 End

///////////////////////////////////////////////////////////////////////					***023 Begin 
//	Encode a PrivateTransfer Service
//in:	op		points to buffer to encode into
//		opmax	max number of octets op can hold 
//		conf	true if issue confirmed tyransfers, false if unconfirmed
//		vendid	vendor identifier
//		svcnum	service number
//		nparms	number of bytes of parameters
//		parms	hex string of parameters
//out:	return	the number of octets encoded
//				or 0 meaning too complex

word  APIENTRY ePrivateTransferService(octet *op,word opmax,word conf,word vendid,
												  word svcnum,word nparms,char *parms)
{	octet *iop;

	iop=op;											//remember starting point
	if (conf)
		op=eCONFREQ(op,confirmedPrivateTransfer);
	else
	{	*op++=UNCONF_REQ_PDU<<4;
		*op++=unconfirmedPrivateTransfer;
	}
	op=eDWORD(op,(dword)vendid,0x08,FALSE);
	op=eDWORD(op,(dword)svcnum,0x18,FALSE);
	if (nparms)
	{
		if ((word)((op-iop)+nparms)>opmax) return 0;
		*op++=0x2E;									//open context tag 2
		cvhexn(parms,op,nparms);					//convert parms as hex bytes
		op+=nparms;									//skip past them
		*op++=0x2F;									//close context tag 2
	}
	return (op-iop);
}													//									***023 End

///////////////////////////////////////////////////////////////////////					***018 Begin 
//	Encode an Add or RemoveListElement Service
//in:	op			points to buffer to encode into
//		opmax		max length of op
//		svc			0=add, 1=remove
//		objid		the object identifier
//		propid		the property identifier
//		havearray 	is true if an array index is needed
//		arrayindex	is the index
//		ptype		is the parse type of the property (from VTSAPI)
//		pflags		flags from VTSAPI
//		ep			enumeration index from VTSAPI
//		arlist		the handle of a list box containing list elements to send 
//out:	return		the number of octets encoded
//					or 0 meaning too complex

word  APIENTRY eListElementService(octet *op,word opmax,word svc,dword objid,dword propid,
											  word havearray,dword arrayindex,word ptype,word pflags,word ep,HWND arlist)
{	octet *iop;
	octet	p[128];
	word	nv,i;

	iop=op;											//remember starting point
	op=eCONFREQ(op,(octet)(svc?removeListElement:addListElement));
	op=etagOBJECTID(op,0x08,objid);
	op=eDWORD(op,propid,0x18,FALSE);
	if (havearray)	
		op=eDWORD(op,arrayindex,0x28,FALSE);
	
	nv=(word)GetListCount(arlist);					//how many properties?
	*op++=0x3E;										//open tag 3
	for (i=0;i<nv;i++)
	{	GetListText(arlist,i,p);					//get the text value to encode
		if ((op=eAny(op,(word)(opmax-(op-iop)),ptype,pflags,ep,p))==0)
			return 0;								//can't encode it		
	}
	*op++=0x3F;										//close tag 3
	return (op-iop);
}													//									***018 End

///////////////////////////////////////////////////////////////////////					***014 Begin 
//	Encode a DeleteObject Service
//in:	op		points to buffer to encode into
//		objid	the object to delete
//out:	return	the number of octets encoded
//				or 0 meaning too complex

word  APIENTRY eDeleteObjectService(octet *op,dword objid)
{	octet *iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,deleteObject);
	op=etagOBJECTID(op,0xC0,objid);
	return (op-iop);
}													//									***014 End

///////////////////////////////////////////////////////////////////////					***014 Begin 
//	Encode a CreateObject Service
//in:	op		points to buffer to encode into
//		opmax	max llength of op
//		objid	the object identifier
//		useinst	true if objid has the instance we want, false if use object type only
//		dlist	the handle of a list box containing records to send 
//out:	return	the number of octets encoded
//				or 0 meaning too complex
#include<stdio.h> //MAG
word  APIENTRY eCreateObjectService(octet *op,word opmax,dword objid,BOOL useinst,HWND dlist)
{	octet *iop;
	octet	p[128];
	word	c,nv,i,objtype;
	//FILE *op2;  // MAG

	//op2 = fopen("c:\\temp\\bacprim32out.txt","a"); //MAG
	//if(op2 == NULL) return(42);
	//fprintf(op2,"enter eCreateObjectService\n"); //MAG
	//fflush(op2);  //MAG
	iop=op;											//remember starting point
	op=eCONFREQ(op,createObject);
	//fprintf(op2,"after eCONFREQ\nuseinst is %d",useinst); //MAG
	//fflush(op2);  //MAG
	*op++=0x0E;										//open tag 0
	objtype=(word)(objid>>22);
	if (useinst)
		op=etagOBJECTID(op,0x18,objid);
	else
		op=eDWORD(op,(dword)objtype,0x08,FALSE);
	//fprintf(op2,"after if else\n"); //MAG
	//fflush(op2);  //MAG
	*op++=0x0F;										//close tag 0
	nv=(word)GetListCount(dlist);					//how many properties?
	//fprintf(op2,"after GetListCount, nv = %d\n",nv); //MAG
	//fflush(op2);  //MAG
	if (nv)											//got some
	{	*op++=0x1E;									//open tag 1
		for (i=0;i<nv;i++){
			//fprintf(op2,"about to GetListText %d\n",nv); //MAG
			//fflush(op2);  //MAG
			GetListText(dlist,i,p);					//get the text value to encode
			if (c=ePropVal(op,(word)(opmax-(op-iop)),objtype,0x08,p))
				op+=c;
			else{
				//fprintf(op2,"exit at else\n"); //MAG
				//fclose(op2);				//MAG
				return 0;							//can't encode it		
			}
		}
		*op++=0x1F;									//close tag 1
	}
	//fprintf(op2,"exit normal\n"); //MAG
	//fclose(op2);				//MAG
	return (op-iop);
}													//									***014 End

///////////////////////////////////////////////////////////////////////					***014 Begin 
//	Encode an AtomicReadFile Service
//in:	op		points to buffer to encode into
//		fins	file instance
//		recacc	true if record access, false if stream access
//		fstart	start position or record (really 0..max)
//		cnt		octet or record count 
//out:	return	the number of octets encoded
//				or 0 meaning too complex

word  APIENTRY eAtomicReadFileService(octet *op,dword fins,word recacc,long fstart,dword cnt)
{	octet *iop;
	octet	tag;

	iop=op;											//remember starting point
	op=eCONFREQ(op,atomicFileRead);
	op=eOBJECTID(op,FILE_O,fins);					//file object
	if (recacc)										//record access
		tag=0x1E;									//open tag 1
	else
		tag=0x0E;									//open tag 0
	*op++=tag;										//stuff opening tag
	op=eINT(op,fstart);
	op=eUINT(op,cnt);
	*op++=tag+1;									//stuff closing tag
	return (op-iop);
}													//									***014 End

///////////////////////////////////////////////////////////////////////					***014 Begin 
//	Encode an AtomicWriteFile Service
//in:	op		points to buffer to encode into
//		opmax	max llength of op
//		fins	file instance
//		recacc	true if record access, false if stream access
//		fstart	start position or record (-1=eof)
//		hexdata	true if list contains hex ascii pairs, false if plain ascii
//		dlist	the handle of a list box containing records to send 
//out:	return	the number of octets encoded
//				or 0 meaning too complex

word  APIENTRY eAtomicWriteFileService(octet *op,word opmax,dword fins,word recacc,
												  long fstart,BOOL hexdata,HWND dlist)
{	octet *iop;
	octet	tag,p[128];
	word	c,nv,i;

	iop=op;											//remember starting point
	op=eCONFREQ(op,atomicFileWrite);
	op=eOBJECTID(op,FILE_O,fins);					//file object
	if (recacc)										//record access
		tag=0x1E;									//open tag 1
	else
		tag=0x0E;									//open tag 0
	*op++=tag;										//stuff opening tag
	op=eINT(op,fstart);
	if (recacc)
	{	nv=(word)GetListCount(dlist);				//how many records?
		op=eUINT(op,nv);
		for (i=0;i<nv;i++)
		{	c=GetListData(dlist,p,i,hexdata);		//get and convert list data
			if (((op-iop)+c)>opmax) return 0;
			op=eOCTETSTRING(op,p,c);
		}
	}
	else
	{	c=GetListData(dlist,p,0,hexdata);			//get and convert list data
		if (((op-iop)+c)>opmax) return 0;
		op=eOCTETSTRING(op,p,c);
	}
	*op++=tag+1;									//stuff closing tag
	return (op-iop);
}													//									***014 End

///////////////////////////////////////////////////////////////////////					***013 Begin 
//	Encode a COVNotification Service
//in:	op		points to buffer to encode into
//		opmax	max number of octets op can hold 
//		conf	true if issue confirmed notifications, false if unconfirmed
//		procid	subscriber process identifier
//		devinst	initiating device instance
//		objid	monitored object identifier
//		timeleft remaining time in the subscription
//		vlist	window handle of a list box containing values to report
//out:	return	the number of octets encoded
//				or 0 meaning too complex

word  APIENTRY eCOVNotificationService(octet *op,word opmax,word conf,dword procid,
												  dword devinst,dword objid,word timeleft,HWND vlist)
{	octet *iop;
	int		i,nv;
	word	c,objtype;
	char	p[128];

	iop=op;											//remember starting point
	if (conf)
		op=eCONFREQ(op,confirmedCOVNotification);
	else
	{	*op++=UNCONF_REQ_PDU<<4;
		*op++=unconfirmedCOVNotification;
	}
	op=eDWORD(op,procid,0x08,FALSE);
	op=etagOBJECTID(op,0x18,vbOBJECTID(DEVICE,devinst));
	op=etagOBJECTID(op,0x28,objid);
	op=eDWORD(op,(dword)timeleft,0x38,FALSE);
	if ((word)(op-iop)>opmax) return 0;
	nv=GetListCount(vlist);
	*op++=0x4E;										//open context tag 4
	objtype=(word)(objid>>22);
	for (i=0;i<nv;i++)
	{	GetListText(vlist,i,p);						//get the text value to encode
		if (c=ePropVal(op,(word)(opmax-(op-iop)),objtype,0x08,p))
			op+=c;
		else
			return 0;								//can't encode it		
	}
	*op++=0x4F;										//close context tag 4
	return (op-iop);
}

/////////////////////////////////////////////////////////////////////// 
//	Encode a SubscribeCOV Service
//in:	op		points to buffer to encode into 
//		procid	subscriber process identifier
//		objid	monitored object identifier
//		cancel	true if cancel subscription, false if (re)subscribe
//		conf	true if issue confirmed notifications, false if unconfirmed
//		lifetime the lifetime in seconds of the subscription (0=permanent)
//out:	return	the number of octets encoded

word  APIENTRY eSubscribeCOVService(octet *op,dword procid,dword objid,
											   word cancel,word conf,dword lifetime) //	***236
{	octet *iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,subscribeCOV);
	op=eDWORD(op,procid,0x08,FALSE);
	op=etagOBJECTID(op,0x18,objid);
	if (cancel==0)									//(re)subscribe
	{	op=eDWORD(op,(conf?1:0),0x28,FALSE);
		op=eDWORD(op,lifetime,0x38,FALSE);			//always use lifetime				***023
	}
	return (op-iop);
}													//									***013 End

/////////////////////////////////////////////////////////////////////// 				***012 Begin
//	Encode a Property Value (as used in WriteMultiple, Write etc.)
//in:	op			points to buffer to encode into 
//		opmax		length of available buffer op points to
//		objtype		the object type to assume
//		btag		the base tag (ie. 0x08 for WriteMultiple, 0x18 for Write)
//		pstr		points to a string like "propid=value" to be encoded after parsing
//					syntax forms for the string are loosely:
//					propid=value
//					propid[arrayindex]=value
//					propid=value@priority
//					propid[arrayindex]=value@priority
//out:	=0			the message can't be encoded in opmax octets
//		>0			the number of octets encoded
//		Note: the buffer pstr points to is trashed by this routine!


word ePropVal(octet *op,word opmax,word objtype,octet btag,char *pstr)
{	octet *iop;
	word	n,ptype,pflags,ep,i;
	dword	pid,aryindex;
	char	pn[64];
	char *cp;
	char *vp;
	octet	priority=0;
	BOOL	ha=FALSE;								//have array...

	//FILE *op2;//MAG
	//octet *iop2; // MAG
	//word j;
    
	//op2 = fopen("c:\\temp\\bacprim32outvbo.txt","a");
	//fprintf(op2,"ePropVal receive values %d %d '%s'\n",opmax,objtype,pstr);
	//fclose(op2);

	iop=op;											//remember starting point
	if (cp=strchr(pstr,'@'))						//priority was specified
	{	priority=atoi(cp+1);						//parse out priority
		*cp=0;										//make it asciz there
	}
    vp=strchr(pstr,'=');							//point to =
    *vp++=0;										//vp now points to the value string
    if (cp=strchr(pstr,'['))						//we have an array index
    {	*cp++=0;									//make pstr the propid
    	aryindex=atol(cp);							//get array index
    	ha=TRUE;
    }
    n=(word)VTSAPIgetpropinfo(objtype,0xFFFF,pn,NULL,NULL,NULL,NULL);	//determine number of properties this type
    if (n==0) return 0;													//should never happen
    for (i=0;i<n;i++)
	{	pid=VTSAPIgetpropinfo(objtype,i,pn,&ptype,NULL,&pflags,&ep);	//get next property
		//fprintf(op2,"ePV: check property %d '%s'\n",i,pn);
		if(stricmp(pn,pstr)==0)
		{	//fprintf(op2,"ePV: find matching property ptype = %d.\n",ptype);
			if ((word)(op-iop+2)>=opmax){ 
				//fprintf(op2,"ePV: opmax size violation- return\n"); fclose(op2); 
				return 0;}
			op=eDWORD(op,pid,btag,FALSE);
			//fprintf(op2,"ePV: after eDWORD length %d encoded sequence: ",op-iop);
			//for(j=0;j<op-iop;j++) fprintf(op2,"%02X ",iop[j]);
			//fprintf(op2,"\n");

			if (ha&&(pflags&IsArray))	
			{	//fprintf(op2,"ePV: find IsArray\n");
				if ((word)(op-iop+5)>=opmax) return 0;
				op=eDWORD(op,aryindex,(octet)(btag+0x10),FALSE);
			}
			//fprintf(op2,"ePV: pre opmax test 2\n");
			if ((word)(op-iop+2)>=opmax) { 
				//fprintf(op2,"ePV: fail opmax test 2- return\n"); fclose(op2); 
				return 0;}	//account for outer tags			
			*op++=(btag+0x20)|0x0E;
			//fprintf(op2,"ePV: after op++ length %d encoded sequence: ",op-iop);
			//for(j=0;j<op-iop;j++) fprintf(op2,"%02X ",iop[j]);
			//fprintf(op2,"\n");

			if (stricmp(vp,nullstring)==0){ ptype=enull; /*fprintf(op2,"ePV: ptype = null\n");*/ }
			op=eAny(op,(word)(opmax-(op-iop)),ptype,pflags,ep,vp);
			
			//fprintf(op2,"ePV: after op=eAny length %d encoded sequence: ",op-iop);
			//for(j=0;j<op-iop;j++) fprintf(op2,"%02X ",iop[j]);
			//fprintf(op2,"\n");

			if (op==0) { 
				//fprintf(op2,"ePV: fail op test- return\n"); fclose(op2); 
				return 0;}
			*op++=(btag+0x20)|0x0F;
			//fprintf(op2,"ePV: after op++=btag length %d encoded sequence: ",op-iop);
			//for(j=0;j<op-iop;j++) fprintf(op2,"%02X ",iop[j]);
			//fprintf(op2,"\n");

			if ((word)(op-iop)>=opmax) return 0;
			if (priority)
			{	//fprintf(op2,"ePV: find priority\n");
				if ((word)(op-iop+5)>=opmax) return 0;			
 				op=eDWORD(op,priority,(octet)(btag+0x30),FALSE);
			}
			//fprintf(op2,"ePV: Normal exit, return %d\nencoded sequence: ",op-iop);
			//for(j=0;j<op-iop;j++) fprintf(op2,"%02X ",iop[j]);
			//fprintf(op2,"\n");
			//fclose(op2);
			return (op-iop);
		}
	}   
    return 0;															//can't buffer it all
}

/////////////////////////////////////////////////////////////////////// 				***015 Begin
//	Encode a Selection Criterium as used in ReadConditional
//in:	op			points to buffer to encode into 
//		opmax		length of available buffer op points to
//		pstr		points to a string like "propid rel value|parsetype|enumtableindex" to be encoded after parsing
//					syntax forms for the string are loosely:
//					propid rel value|parsetype|enumtableindex
//					propid[arrayindex] rel value|parsetype|enumtableindex
//out:	=0			the message can't be encoded in opmax octets
//		>0			the number of octets encoded
//		Note: the buffer pstr points to is trashed by this routine!


word eSelCrit(octet *op,word opmax,char *pstr)
{	octet *iop;
	word	ptype,reltype,i,ep;						//									***023
	dword	eval,aryindex;
	char *cp;
	char *vp;
	char	fc;
	BOOL	ha=FALSE;

	iop=op;											//remember starting point
	if (cp=strchr(pstr,'|'))						//find parse type
	{	*cp++=0;									//make it asciz there
		if (vp=strchr(cp,'|'))					//find enum table index				***023 Begin
		{	*vp++=0;								//make it asciz there
			ep=atoi(vp);
		}
		else return 0;								//									***023 End
		for (i=0;i<stParseTypes.nes;i++)
		{	if (stricmp(stParseTypes.estrings[i],cp)==0)
			{	ptype=i;							//parse type found
				break;
			}
		}
	}
	else return 0;
	reltype=0;										//default
	for (i=1;i<stRelationTypes.nes;i++)
	{	if (cp=strchr(pstr,*stRelationTypes.estrings[i]))
		{	if (*(cp+1)==*(stRelationTypes.estrings[i]+1))
			{	reltype=i;							//relational specifier found
				break;
			}
		}			
	}
	switch (reltype)
	{
	case 0:
		fc='=';
		goto getvp;
	case 1:
		fc='!';
		goto getvp;
	case 2:
	case 4:
		fc='<';
		goto getvp;
	case 3:
	case 5:
		fc='>';
getvp:	vp=strchr(pstr,fc);						//find first char of relation
		*vp++=0;									//asciiz property
		*vp++;										//vp points to value
	}
	
    if (cp=strchr(pstr,'['))						//we have an array index
    {	*cp++=0;									//make pstr the propid
    	aryindex=atol(cp);							//get array index
    	ha=TRUE;
    }
    for (i=0;i<stPropIDs.nes;i++)
	{	if (stricmp(pstr,stPropIDs.estrings[i])==0)
		{	if ((word)(op-iop+2)>=opmax) return 0;			
			op=eDWORD(op,i,0x08,FALSE);				//context tag 0 property id
			if (ha)	
			{	if ((word)(op-iop+5)>=opmax) return 0;
				op=eDWORD(op,aryindex,0x18,FALSE);	//optional context tag 1 array index
			}			
			if ((word)(op-iop+2)>=opmax) return 0;
			op=eDWORD(op,reltype,0x28,FALSE);		//context tag 2 relational specifier
			if ((word)(op-iop+2)>=opmax) return 0;	//account for outer tags			
			*op++=0x3E;								//open tag 3 comparison value
			if ((ptype==et)||(ptype==pab))			//an enumeration table
			{	if (opmax<5) return 0;
				eval=atol(vp);
				op=eDWORD(op,eval,0x90,FALSE);
			}
			else 
			{	op=eAny(op,(word)(opmax-(op-iop)),ptype,0,ep,vp);	//								***023
				if (op==0) return 0;
			}
			*op++=0x3F;								//close tag 3 comparison value
			if ((word)(op-iop)>=opmax) return 0;
			return (op-iop);
		}
	}   
    return 0;										//can't buffer it all
}
//																							***015 End
///////////////////////////////////////////////////////////////////////
//	Encode Any Value
//in:	op			points to buffer to encode into 
//		opmax		length of available buffer op points to
//		ptype		parse type
//		pflags		property flags
//		ep			enumeration pointer
//		pstr		points to a string value to be encoded after parsing
//out:	updated op 	if ok
//		0			if unable to encode


octet  *eAny (octet *op,word opmax,word ptype,word pflags,word ep,char *pstr)
{	octet		tag,os[3];
	long		max,min,val;

	dword		dmax,dmin,dval,dmask,pid;
	word		i,j,k,n,nc,msize,objtype;
	word		eitbl[3]={eiMon,eiWOM,eiAnyDOW};
	char 		ptext[64];
	char		*cp,*np,*vp,*tp, *lp;			//										***024***028
	BOOL		last=FALSE;
	byte		btemp;
	union { dword dw; byte b[4];} u;
	union { short int si; byte b[2]; } u2;
	
	switch(ptype)
	{
//																						***019 Begin
	case ssint:
		if((atoi(pstr) < 127)&&(atoi(pstr) >= 0)){
			*op++= 0x31;
			*op++= atoi(pstr);
			break;
		}
		{
			  // adapted from eWORD MAG 13 FEB 2001
			u2.si=atoi(pstr);										//so we can get at individual bytes
			*op++=0x30+0x02;								//tag (utag), assume 2 octets (word)
			*op++=u2.b[1];									//msb first
			*op++=u2.b[0];
		}
		break;
	case ob_id:									//an object identifier "(OBJECT_ID,instance)"
		if (opmax<5) return 0;
		op=eObjId(op,pstr,&objtype);
		break; 

//																						***021 Begin
	case dt:									//date/time "DateTime,date,month,year,dow,hh:mm:ss.hd"
		if (opmax<10) return 0;					//can't fit date,time
		cp=strchr(pstr,',');
		cp++;
		np=cp;
		for (i=0;i<4;i++)
		{	np=strchr(np,',');
			np++;
		}
		np--;
		*np++=0;
		op=atoeDATESTRING(op,cp);				//encode date
		if (op==0) return 0;
		cp=np;
		op=atoeTIMESTRING(op,cp);				//encode time
		break;

	case ddate:									//date "date,month,year,dow"		***025 Begin
		if (opmax<5) return 0;					//can't fit date
		op=atoeDATESTRING(op,pstr);				//encode date
		break;

	case ttime:									//time "hh:mm:ss.hd"
		if (opmax<5) return 0;					//can't fit time
		op=atoeTIMESTRING(op,pstr);				//encode time
		break;

	case dtrange:								//range of dates "DateRange,date,month,year,dow,date,month,year,dow"
		if (opmax<10) return 0;					//can't fit two dates
		np=pstr;
		for (i=1;i<=5;i++) 						//seek fifth ','
		{	np=strchr(np,',');
			if (np==0) break;
			np++;
			if (i==1) cp=np;					//so we skip "DateRange"
		}
		np--;									//backup
		*np++=0;								//make it asciiz
		op=atoeDATESTRING(op,cp);				//encode first date
		op=atoeDATESTRING(op,np);				//encode second date
		break;									//									***025 End

	case calist:								//list of calendarentry 
												//	"Date,date,month,year,dow| "
												//	"DateRange,date,month,year,dow,date,month,year,dow| "
												//	"WeekNDay,month,weekofmonth,dow| "
		cp=pstr;
		n=VTSAPIgetenumtable(eiCalType,0,NULL,NULL,ptext);	//get number of enumerations this pointer
		for(;;) 
		{	np=strchr(cp,'|');
			if (np==0) break;
			*np++=0;
			np=strchr(np,' ');
			np++;
			vp=strchr(cp,',');
			*vp++=0;
			for (i=0;i<n;i++)
			{	n=VTSAPIgetenumtable(eiCalType,i,NULL,NULL,ptext);	//get text for enumeration
				if(stricmp(ptext,cp)==0) break;			//we got it
			}
			cp=vp;
			if (i>=n) return 0;
			switch(i)
			{
			case 0:								//Date
				if (opmax<5) return 0;			//can't fit date 				***024 Begin
//				*op++=0x0E;						//open context tag 0
				tp=op;
				op=atoeDATESTRING(op,cp);
				*tp=0x0C;						//SD tag 0, L=4
				if (op==0) return 0;
//				*op++=0x0F;						//close context tag 0			***024 End
				break;
			case 1:								//DateRange
				if (opmax<12) return 0;			//can't fit date range
				*op++=0x1E;						//open context tag 1
				vp=cp;
				for (j=0;j<4;j++)
				{	vp=strchr(vp,',');
					vp++;
				}
				vp--;
				*vp++=0;						//make first date asciiz
				op=atoeDATESTRING(op,cp);		//encode first date
				if (op==0) return 0;
				cp=vp;
				op=atoeDATESTRING(op,cp);		//encode second date
				if (op==0) return 0;
				*op++=0x1F;						//close context tag 1
				break;
			case 2:                             //WeekNDay
				if (opmax<5) return 0;			//can't fit date				***024 Begin 
				vp=cp;
//				*op++=0x2E; 					//open context tag 2
				for (j=0;j<3;j++)
				{	vp=strchr(vp,',');
					if (vp!=0) *vp++=0;					
					nc=VTSAPIgetenumtable(eitbl[j],0,NULL,NULL,ptext);	//get number of enumerations this pointer
					for (k=0;k<nc;k++)
					{	n=VTSAPIgetenumtable(eitbl[j],k,NULL,NULL,ptext); //get text for enumeration
						if(stricmp(ptext,cp)==0)				//we got it
						{	if (k!=0) os[j]=(octet)k;
							else os[j]=0xFF;
							break;
						}
					}
					cp=vp;
				}
				tp=op;
				op=eOCTETSTRING (op,os,3);
				*tp=0x2B;						//SD tag 2, L=3
				if (op==0) return 0;
//				*op++=0x2F;						//close context tag 2			***024 End
				break;
			}
			cp=np;
		}
		break;

	case reciplist:								//list of BACnetDestination		***027 Begin 
												// eg. "(Recipient,(Monday,Tuesday,etc.),(xx:xx:xx.xx,xx:xx:xx.xx),Device,2,3,Yes,(to-offnormal,etc.)|Recipient...)"
		np=strchr(pstr,'(');
		if (np==0) break;
		np++;
		for (;;)
		{	if (opmax<6) return 0;					//can't fit bitstring
			cp=strchr(np,'(');					//find DOW list
			cp++;									//cp points to validDays
			np=strchr(cp,')');
			*np++=0;
			tp=atoeBITSTRING(op,cp,eiDOW);			//encode valid days					***026 Begin
			if (tp==0) return 0;
			opmax-=tp-op;
			op=tp;
			if (opmax<10) return 0;					//can't fit fromTime and toTime		***026 End
			cp=strchr(np,'(');					
			cp++;									//cp points to fromTime  
			tp=atoeTIMESTRING(op,cp);				//encode fromTime					***026 Begin
			if (tp==0) return 0;
			opmax-=tp-op;
			op=tp;									//									***026 End
			cp=strchr(cp,',');					
			cp++;									//cp points to toTime  
			tp=atoeTIMESTRING(op,cp);				//encode toTime		     			***026 Begin
			if (tp==0) return 0;
			opmax-=tp-op;
			op=tp;									//									***026 End
			cp=strchr(cp,',');
			cp++;									//cp points to recipient ("Device,inst,etc." or "Address,net,macaddr,etc.")
			np=strchr(cp,',');
			np++;
			np=strchr(np,',');
			if ((*cp=='D')||(*cp=='d'))				//we have a device 
			{	if (opmax<7) return 0;				//can't accomodate device,open/close tags
			}
			else									//we have an address
			{	if (opmax<16) return 0;				//can't do network.mac address,open/close tags
				np++;
				np=strchr(np,',');
			}
			*np++=0;								//make recipient asciiz
			tp=eRECIPIENT (op,cp);					//									***026 Begin
			if (tp==0) return 0; 
			opmax-=tp-op;
			op=tp;									//									***026 End
			if (opmax<12) return 0;					//can't fit processIdentifier,issueConfirmedNotifications and transitions
			cp=np;									//cp points to processIdentifier
			np=strchr(cp,',');
			dval=strtoul(cp,NULL,10);
			op=eDWORD(op,dval,0x20,FALSE);			//encode processIdentifier
			cp=++np;
			np=strchr(cp,',');
			*np++=0;
			n=VTSAPIgetenumtable(eiYN,0,NULL,NULL,ptext);		//get number of enumerations this pointer (n will always be 2!)
			for (i=0;i<n;i++)
			{	n=VTSAPIgetenumtable(eiYN,i,NULL,NULL,ptext);	//get text for enumeration
				if(stricmp(ptext,cp)==0)				//we got it
				{	op=eBOOL(op,(BOOL)i);
					break;
				}
			}
			cp=strchr(np,'(');
			cp++;									//cp points to transitions
			np=strchr(cp,')');
			*np++=0;
			tp=op;
			op=atoeBITSTRING(op,cp,eiEvTr);			//encode event transitions
			opmax-=op-tp;
			np=strchr(np,'|');
			if (np==0) break;
			np++;
		}
        break;									//											***027 End

//																							***028 Begin
		case raslist:							//list of readaccessspecs "((OBJECT_ID,instance),(Property_id,Property_id[opt.array index]))"	
		cp=strchr(pstr,'(');
		cp++;
		for (;;)
		{	if (opmax<7) return 0;					//can't save object id, and one sets open/close context tags   ***024
//			*op++=0x0E;								//open context tag 0						***024 Begin
			tp=op;
			op=eObjId(op,cp,&objtype);
			*tp=0x0C;								//add SD tag 0
			if (op==0) return 0;
//			*op++=0x0F;								//close context tag 0						***024 End
			cp=strchr(cp,')');
			cp=strchr(cp,',');
			cp=strchr(cp,'(');
			cp++;									//cp now points at property id(s)
			lp=strchr(cp,')');
			*lp++=0x00;								//make it asciiz
			*op++=0x1E;								//open context tag 1
		    n=(word)VTSAPIgetpropinfo(objtype,0xFFFF,ptext,NULL,NULL,NULL,NULL);	//determine number of properties this type
		    if (n==0) return 0;						//should never happen
		    for (;;)
	   		{	if (opmax<7) return 0;				//can't save property id, array indices and context tags	***024
            	np=strchr(cp,',');				//do next property
            	if (np!=0) *np++=0x00;				//make it asciiz
           		vp=strchr(cp,'[');
            	if (vp!=0) *vp++=0x00;				//we have an array index
				for (i=0;i<n;i++)
				{	pid=VTSAPIgetpropinfo(objtype,i,ptext,NULL,NULL,NULL,NULL);	//get next property
					if(stricmp(ptext,cp)==0)
					{	
//						*op++=0x0E;					//open context tag 0						***024 Begin
//						op=eDWORD(op,pid,0x90,FALSE);
						tp=op;
						op=eDWORD(op,pid,0x08,FALSE);	//SD context tag 0
//						*op++=0x0F;					//close context tag 0
						if (vp!=0)					//we have an array index
						{  	nc=atoi(vp);			//get array index
//							*op++=0x1E;				//open context tag 1
//							op=eDWORD(op,nc,0x20,FALSE);
							op=eDWORD(op,nc,0x18,FALSE);	//SD context tag 1
//							*op++=0x1F;				//close context tag 1						***024 End
						}
						opmax-=op-tp;
						break;
					}
				}
				if (i>=n) return 0;
				if (np==0) break;
				cp=np;
			}
			*op++=0x1F;								//close context tag 1
			np=strchr(lp,'|');
			if (np==0) break;
			np++;
			cp=np;
		}		
		break;

	case wsched:							    //list of time vals for week schedules (time,(tag)value|...)
		if (opmax<2) return 0;					//no hope
		*op++=0x0E;								//open context tag
		op=atotimeval (op,opmax,pstr);
		if (op!=0) *op++=0x0F;					//close context tag
		return op;

	case xsched:								//exception schedule: array[] of specialevent
												//	"Date,date,month,year,dow|(time,(tag)value|...)@priority "
												//	"DateRange,date,month,year,dow,date,month,year,dow|(time,(tag)value|...)@priority "
												//	"WeekNDay,month,weekofmonth,dow|(time,(tag)value|...)@priority "
												//	"ObjectID,(OBJECT_ID,instance)|dow,(time,(tag)value|...)@priority "

		cp=pstr;
		np=strchr(cp,',');
		if (np==0) return 0;
		*np++=0;
		n=VTSAPIgetenumtable(eiCalType,0,NULL,NULL,ptext);	//get number of enumerations this pointer
		for (i=0;i<n;i++)
		{	n=VTSAPIgetenumtable(eiCalType,i,NULL,NULL,ptext);	//get text for enumeration
			if(stricmp(ptext,cp)==0) break;			//we got it
		}
		cp=np;
		np=strchr(cp,'|');
		if (np==0) return 0;
		*np++=0;
		if (i<n)								//it's a Calendar entry
		{	
			if (opmax<2) return 0;				//there's no hope
			opmax-=2;							//account for open/close tags
			*op++=0x0E;							//open context tag
			switch(i)
			{
			case 0:								//Date
				if (opmax<5) return 0;			//can't fit date
				tp=op;
				op=atoeDATESTRING(op,cp);
				*tp=0x0C;						//SD tag 0, L=4
				if (op==0) return 0;
				break;
			case 1:								//DateRange
				if (opmax<12) return 0;			//can't fit date range
				*op++=0x1E;						//open context tag 1				***030
				vp=cp;
				for (j=0;j<4;j++)
				{	vp=strchr(vp,',');
					vp++;
				}
				vp--;
				*vp++=0;						//make first date asciiz
				op=atoeDATESTRING(op,cp);		//encode first date
				if (op==0) return 0;
				cp=vp;
				op=atoeDATESTRING(op,cp);		//encode second date
				if (op==0) return 0;
				*op++=0x1F;						//close context tag 1				***030
				break;
			case 2:                             //WeekNDay
				if (opmax<5) return 0;			//can't fit date
				vp=cp;
				for (j=0;j<3;j++)
				{	vp=strchr(vp,',');
					if (vp!=0) *vp++=0;					
					nc=VTSAPIgetenumtable(eitbl[j],0,NULL,NULL,ptext);	//get number of enumerations this pointer
					for (k=0;k<nc;k++)
					{	n=VTSAPIgetenumtable(eitbl[j],k,NULL,NULL,ptext); //get text for enumeration
						if(stricmp(ptext,cp)==0)				//we got it
						{	if (k!=0) os[j]=(octet)k;
							else os[j]=0xFF;
							break;
						}
					}
					cp=vp;
				}
				tp=op;
				op=eOCTETSTRING (op,os,3);
				*tp=0x2B;						//SD tag 2, L=3						***030
				if (op==0) return 0;
				break;
			}
			*op++=0x0F;							//close context tag
			cp=np;
		}
        else
        {
			if (opmax<5) return 0;					//can't fit object id
			tp=op;
			op=eObjId(op,cp,&objtype);				//encodes C4,xx,xx,xx,xx
			*tp=0x1C;								//SD tag 1, L=4
		}
		
		if (op==0) return 0;
		cp=strchr(np,'#');
		cp++;
		if (opmax<2) return 0;					//no hope
		*op++=0x2E;								//open context tag
		op=atotimeval (op,opmax,np);
		if (op!=0) *op++=0x2F;					//close context tag

		nc=atoi(cp);							//get priority
	    return eDWORD(op,nc,0x38,FALSE);

	case act:									//action array
												// eg. "((DEVICE,inst)|(OBJECT ID,instance),propertyid[index]=(prim type)value,priority,postdelay,failure(yes/no),writesuccess(yes/no),.....)"
		cp=strchr(pstr,'(');
		cp++;
		for (;;)
		{	if (opmax<7) return 0;					//can't save one set and object id open/close context tags
			*op++=0x0E;								//open context tag
			np=strchr(pstr,'|');					//do we have a device object ref
			if (np!=0)
			{	if (opmax<5) return 0;
				tp=op;
				op=eObjId(op,cp,&objtype);
				*tp=0x0C;							//add SD tag 0
				if (op==0) return 0;
				np++;
				cp=np;
				opmax-=5;
			}
			tp=op;
			op=eObjId(op,cp,&objtype);
			*tp=0x1C;								//add SD tag 1
			if (op==0) return 0;
			opmax-=7;
			cp=strchr(cp,')');
			cp=strchr(cp,',');
			cp++;									//cp now points at property id(s)
			np=strchr(cp,'=');
			*np++=0x00;								//make it asciiz
		    n=(word)VTSAPIgetpropinfo(objtype,0xFFFF,ptext,NULL,NULL,NULL,NULL);	//determine number of properties this type
		    if (n==0) return 0;						//should never happen
	   		if (opmax<5) return 0;					//can't save property id, array index
         	vp=strchr(cp,'[');
            if (vp!=0) *vp++=0x00;					//we have an array index
			for (i=0;i<n;i++)
			{	pid=VTSAPIgetpropinfo(objtype,i,ptext,NULL,NULL,NULL,NULL);	//get next property
				if(stricmp(ptext,cp)==0)
				{	
					tp=op;
					op=eDWORD(op,pid,0x28,FALSE);	//SD context tag 2
					if (vp!=0)						//we have an array index
					{  	nc=atoi(vp);				//get array index
						op=eDWORD(op,nc,0x38,FALSE);	//SD context tag 3
					}
					opmax-=op-tp;
					break;
				}
			}
			if (i>=n) return 0;
			cp=np;									//cp points to the value
			np=strchr(cp,',');
			*np++=0;								//np points to post delay
			if (opmax<5) return 0;					//can't save least max primative encode except strings
			tp=op;
			op=atoprim (op,opmax,cp);
			if (op==0) return 0;
			*tp=(*tp&0x0E)+0x48;					//SD tag 4
			cp=np;
			if (*np!=',')							//we have a priority
			{	if (opmax<2) return 0;				//can't fit priority
				cp=strchr(cp,',');
				*cp++=0;
				tp=op;
				nc=atoi(np);						//get priority
				op=eDWORD(op,nc,0x58,FALSE);		//SD context tag 5
				opmax-=op-tp;
			}
			else
			{	cp=strchr(cp,',');
				cp++;
			}
			np=cp;
			if (*np!=',')							//we have a post delay
			{	if (opmax<5) return 0;				//can't fit post delay
				cp=strchr(cp,',');
				*cp++=0;
				tp=op;
				nc=atoi(np);						//get post delay
				op=eDWORD(op,nc,0x68,FALSE);		//SD context tag 6
				opmax-=op-tp;
			}
			else
			{	cp=strchr(cp,',');
				cp++;
			}
			if (opmax<4) return 0;					//can't fit two bools				***030
			np=strchr(cp,',');					//cp points to YES/NO of quit on failure 
			*np++=0;
			n=VTSAPIgetenumtable(eiYN,0,NULL,NULL,ptext);		//get number of enumerations this pointer (n will always be 2!)
			for (i=0;i<n;i++)
			{	n=VTSAPIgetenumtable(eiYN,i,NULL,NULL,ptext);	//get text for enumeration
				if(stricmp(ptext,cp)==0)				//we got it
				{	*op++=0x79;						//SD tag 7, always len 1			***030
					*op++=(((BOOL)i) ?1:0);			//boolean value						***030
					break;
				}
			}
			
			cp=strchr(np,',');
			if (cp==0) 
			{   cp=strchr(np,')');
				if (cp==0) break;
				last=TRUE;
			}
			*cp++=0;									//np points to YES/NO of write successful
			n=VTSAPIgetenumtable(eiYN,0,NULL,NULL,ptext);		//get number of enumerations this pointer (n will always be 2!)
			for (i=0;i<n;i++)
			{	n=VTSAPIgetenumtable(eiYN,i,NULL,NULL,ptext);	//get text for enumeration
				if(stricmp(ptext,np)==0)				//we got it
				{	*op++=0x89;						//SD tag 8, always len 1			***030
					*op++=(((BOOL)i) ?1:0);			//boolean value						***030
					break;
				}
			}

			*op++=0x0F;								//close context tag 0
			if (last) break;
		}		
		break;
	case evparm:								//event parameters
												//'change-of-bitstring,time delay,bitmask,bitstring,bitstring,...'
												//'change-of-state,time delay,(state type)state type-text/(state)state-text,...
												//'change-of-value,time delay,(8)x/bitmask'
												//'change-of-value,time delay,(4)real value'
												//'command-failure,time delay,(OBJECT TYPE, instance),property[index]'
												//'floating-limit,time delay,(OBJECT TYPE, instance),property[index],low limit,high limit,deadband'
												//'out-of-range,time delay,low limit,high limit,deadband'
		if (opmax<7) return 0;					//can't fit time delay, open/close tags
		np=strchr(pstr,',');
		*np++=0;
		cp=strchr(np,',');
		*cp++=0;
		j=atoi(np);								//get time delay
		n=VTSAPIgetenumtable(eiEvType,0,NULL,NULL,ptext);		//get number of enumerations this pointer (n will always be 2!)
		for (i=0;i<n;i++)
		{	n=VTSAPIgetenumtable(eiEvType,i,NULL,NULL,ptext);	//get text for enumeration
			if(stricmp(ptext,pstr)==0) break;				//we got it
		}
		*op++=0x0E+(octet)(i<<4);				//open tag
		tp=op;
		op=eDWORD(op,j,0x08,FALSE);				//SD context tag 0
		opmax-=op-tp;
		np=cp;									//np points to first param after td
// Do NOT!! destroy i
		switch (i)
		{
		case 0:									//change-of-bitstring
			cp=strchr(cp,',');					
			*cp++=0;
			if (opmax<5) return 0;				//can't fit bitmask
			tp=op;
			op=atoprim(op,opmax,np);
			if (op==0) return 0;
			*tp=(*tp&0x0E)+0x18;				//SD tag 1 
			opmax-=op-tp;
			if (opmax<2) return 0;				//can't fit open/close tags
			opmax-=2;
			*op++=0x2E;							//open tag 2
			do
			{	np=cp;
				cp=strchr(cp,',');
				if (cp==0) last=TRUE;
				else *cp++=0;
				tp=op;
				op=atoprim(op,opmax,np);
				if (op==0) return 0;
				opmax-=op-tp;
			}
			while (last==FALSE);
			*op++=0x2F;							//close tag 2
			break;
		case 1:									//change-of-state
			if (opmax<2) return 0;				//can't fit open/close tag
			*op++=0x1E;							//open tag 1
			do
			{	if (opmax<3) return 0;
				tp=strchr(np,'(');
				tp++;							//tp points to a tag
				cp=strchr(tp,')');
				*cp++=0;
				np=strchr(cp,'(');
				np++;							//np points to a state
				cp=strchr(np,')');
				*cp++=0;						//np points to an enumerated value
				tag=(((octet)atoi(tp))<<4)+0x08;
				n=atoi(np);
				tp=op;				
				op=eDWORD(op,(dword)n,tag,FALSE);
				opmax-=op-tp;
				np=strchr(cp,',');
			}
			while (np!=0);
			*op++=0x1F;							//close tag 1
			break;
		case 2:									//change-of-value
			if (opmax<7) return 0;				//can't save least max primative encode except strings and open/close tags
			cp=np;
			cp++;
			*op++=0x1E;							//open tag 1
			tp=op;
			op=atoprim(op,opmax,np);
			if (op==0) return 0;
			if (*cp=='8') *tp=(*tp&0x0E)+0x08;
			else *tp=(*tp&0x0E)+0x18;		//SD tag 0 or 1
			*op++=0x1F;							//close tag 1
			break;
		case 3:									//command-failure
		case 4:									//floating-limit
			if (opmax<14) return 0;				//can't fit it all
			cp=strchr(cp,')');
			cp=strchr(cp,',');
			*cp++=0;
			*op++=0x1E;							//open tag
			tp=op;
			op=eObjId(op,np,&objtype);			//encodes C4,xx,xx,xx,xx
			*tp=0x0C;							//SD tag 0, L=4
		    n=(word)VTSAPIgetpropinfo(objtype,0xFFFF,ptext,NULL,NULL,NULL,NULL);	//determine number of properties this type
		    if (n==0) return 0;						//should never happen
           	vp=strchr(cp,'[');
            lp=cp;
            if (vp!=0)
            {	*vp++=0x00;						//we have an array index
            	lp=vp;
            }
            lp=strchr(lp,',');
            if (lp!=0) *lp++=0;
			for (j=0;j<n;j++)
			{	pid=VTSAPIgetpropinfo(objtype,j,ptext,NULL,NULL,NULL,NULL);	//get next property
				if(stricmp(ptext,cp)==0)
				{	
					tp=op;
					op=eDWORD(op,pid,0x18,FALSE);		//SD context tag 1
					if (vp!=0)							//we have an array index
					{  	nc=atoi(vp);					//get array index
						op=eDWORD(op,nc,0x28,FALSE);	//SD context tag 2
					}
					opmax-=op-tp;
					break;
				}
			}
			if (j>=n) return 0;
			*op++=0x1F;							//close tag 1
			if (i==3) break;					//all done for command-failure
			cp=lp;
			tag=0x2C;							//base tag for floating-limit
dolims:		if (opmax<15) return 0;				//can't fit 3 floats
			np=strchr(cp,',');
			*np++=0;	
			tp=op;
			op=eREAL(op,(float)atof(cp));		//converts to tag=44
			*tp=tag;
			tag+=0x10;
			cp=strchr(np,',');
			*cp++=0;	
			tp=op;
			op=eREAL(op,(float)atof(np));		//converts to tag=44
			*tp=tag;
			tag+=0x10;
			tp=op;
			op=eREAL(op,(float)atof(cp));		//converts to tag=44
			*tp=tag;
			break;
		case 5:									//out-of-range
			tag=0x1C;							//base tag
			goto dolims;
		default:								//invalid event type
			return 0;
		}
		*op++=0x0F+(octet)(i<<4);				//close tag
		break;
 ;
//																							***028 End 

	case lopref:								//list of object prop refs
	case propref:								//obj property reference "((OBJECT_ID,instance),Property_id[opt. array index])"
		cp=strchr(pstr,'(');
		cp++;									//eliminate first (
		np=strrchr(cp,')');
		*np=0x00;								//eliminate last )
		for (;;)
		{	if (ptype==setref)					//									***025 Begin
			{	if (opmax<7) return 0;			//can't fit open/close tags and object id
				*op++=0x0E;						//open context tag 0				
			}
			else 	
				if (opmax<5) return 0;			//can't fit object id
//																					***025 End
//			*op++=0x0E;								//open context tag 0				***024 Begin
			tp=op;
			op=eObjId(op,cp,&objtype);				//encodes C4,xx,xx,xx,xx
			*tp=0x0C;								//SD tag 0, L=4
			if (op==0) return 0;
//			*op++=0x0F;								//close context tag 0				***024 End
			cp=strchr(cp,')');
			cp++;
			cp=strchr(cp,',');
			cp++;              						//cp now points at property id
			np=strchr(cp,',');
			if (np!=0) *np++=0x00;
		    vp=strchr(cp,'[');
		    if (vp!=0)								//we have an array index
		    {	if (opmax<7) return 0;				//can't fit property id and array index  ***024
		    	*vp++=0x00;
		    }
		    else if (opmax<2) return 0;				//can't fit property id				***024
		    n=(word)VTSAPIgetpropinfo(objtype,0xFFFF,ptext,NULL,NULL,NULL,NULL);	//determine number of properties this type
		    if (n==0) return 0;						//should never happen
			for (i=0;i<n;i++)
			{	pid=VTSAPIgetpropinfo(objtype,i,ptext,NULL,NULL,NULL,NULL);	//get next property
				if(stricmp(ptext,cp)==0)
				{	
//					*op++=0x1E;						//open context tag 1				***024 Begin
//					op=eDWORD(op,pid,0x90,FALSE);
//					*op++=0x1F;						//close context tag 1
					op=eDWORD(op,pid,0x18,FALSE);
					if (vp!=0)						//we have an array index
					{  	nc=atoi(vp);				//get array index
//						*op++=0x2E;					//open context tag 2
//						op=eDWORD(op,nc,0x20,FALSE);
//						*op++=0x2F;					//close context tag 2
					    op=eDWORD(op,nc,0x28,FALSE);	//								***024 End
					}
					break;
				}
			}
			if (i>=n) return 0;
			if (np==0)
			{	if (ptype==setref) *op++=0x0F;		//close context tag 0				***025
				break;
			}
			cp=np;									//we have another ref to deal with (lopref only)
		}
		break; 
 //																						***021 End

	case vtcl:									//vt classes
		n=VTSAPIgetenumtable(eiVTCls,0,NULL,NULL,ptext);	//get number of enumerations this pointer 
		for (;;)
		{	if (opmax<2) return 0;
			cp=strchr(pstr,',');							//next comma-sep value
	    	if (cp==NULL)
	    	{	cp=strchr(pstr,0x00);						//last on in the list
	    		last=TRUE;
	    	}
	    	nc=cp-pstr;
			for (i=0;i<n;i++)
			{	n=VTSAPIgetenumtable(eiVTCls,i,NULL,NULL,ptext);	//get text for enumeration
				if(strnicmp(ptext,pstr,nc)==0)			//we got it
				{	op=eDWORD(op,(dword)i,0x90,FALSE);
					if (last) return op;
					pstr=++cp;								//next comma-sep value
					break;
				}
			}
			if (i>=n) return 0;								//could not be encoded
		}

	case bits:									//octet of T or F flags
//			for now handle up to 4 octets!!
		if (opmax<6) return 0;
		dval=strtoul(pstr,&cp,16);
		n=VTSAPIgetenumtable(ep,0,NULL,NULL,ptext);		//get number of significant bits
		dmask=0x80000000;								//bits are flipped in BACnet
		u.dw=0;
		for (i=0;i<n;i++)
		{	if (dval&0x01) u.dw|=dmask;
			dmask>>=1;
			dval>>=1;
		}
		btemp=u.b[0];									//put them in BACnet order
		u.b[0]=u.b[3];
		u.b[3]=btemp;
		btemp=u.b[1];
		u.b[1]=u.b[2];
		u.b[2]=btemp;
		op=eBITSTRING(op,&u.b[0],0x80,n);	
		break;

	case recip:									//recipient either "Device,inst" or "Address,net,macaddress" ***025 Begin
		if ((*pstr=='D')||(*pstr=='d'))			//we have a device 
		{	if (opmax<5) return 0;				//can't accomodate device
		}
		else									//we have an address
			if (opmax<14) return 0;				//can't do network.mac address
		return eRECIPIENT (op,pstr);			//															***025 End 

//																								***027 Begin
	case tsrecip:								//time synch recipients (x|x...) where x = either "Device,inst" or "Address,net,macaddress"
		np=strchr(pstr,'(');
		if (np==0) break;
		np++;
		for (;;)
		{	cp=strchr(np,'|');
			if (cp==0) 
			{   cp=strchr(np,')');
				if (cp==0) break;
				last=TRUE;
			}
			*cp++=0;
			if ((*np=='D')||(*np=='d'))				//we have a device 
			{	if (opmax<5) return 0;				//can't accomodate device
			}
			else									//we have an address
				if (opmax<14) return 0;				//can't do network.mac address
			tp=op;
			op=eRECIPIENT (op,np);
			if (last) break;
			opmax-=op-tp;
			np=cp;
		}
		break; 

	case skeys:									//session keys "(Sessionkey,key,network,macaddress|Sessionkey, ...)"
		np=strchr(pstr,'(');
		if (np==0) break;
		np++;
		for (;;)
		{	if (opmax<22) return 0;					//can't fit key and network
			tp=op;
			np=strchr(np,',');
			np++;
			cp=strchr(np,',');
			*cp++=0x00;
			op=atooct(op,np);						//encode key
			if (op==0) return 0;
			np=cp;
		 	dval=atol(np);							//get network number 		
 			op=eDWORD(op,dval,0x20,FALSE);			//minimal encode using tag 2
			opmax-=op-tp;
			np=strchr(np,',');
			np++;
			cp=strchr(np,'|');
			if (cp==0) 
			{   cp=strchr(np,')');
				if (cp==0) break;
				last=TRUE;
			}
			*cp++=0;
			if (opmax<10) return 0;					//can't fit mac address
			tp=op;
			op=atooct(op,np);
			if (last) break;
			opmax-=op-tp;
			np=cp;
		}
		break;

	case dabind:								//device address bindings "(Device,inst,Address,net,macaddress|Device,...)"
		np=strchr(pstr,'(');
		if (np==0) break;
		np++;
		for (;;)
		{	if (opmax<9) return 0;					//can't accomodate device and network
			ptext[0]='(';							//form string "(device,inst)
			cp=strchr(np,',');					//skip past "Address,
			cp++;
			cp=strchr(cp,',');
			nc=cp-np;
			cp++;
			strncpy(&ptext[1],np,nc);				//copy "device,inst"
			np=cp;
			ptext[nc+1]=')';                      
			ptext[nc+2],0x00;
			tp=op;
			op=eObjId (op,ptext,&objtype);			//get object-id part
			if (op==0) return 0;
			opmax-=op-tp;
			np=strchr(np,',');
			np++;
			dval=atol(np);							//get network number 		
 			tp=op;
 			op=eDWORD(op,dval,0x20,FALSE);			//minimal encode using tag 2
 			opmax-=op-tp;
			np=strchr(np,',');
			np++;
			cp=strchr(np,'|');
			if (cp==0) 
			{   cp=strchr(np,')');
				if (cp==0) break;
				last=TRUE;
			}
			*cp++=0;
			if (opmax<10) return 0;					//can't fit mac address
			op=atooct(op,np);
			if (last) break;
			opmax-=op-tp;
			np=cp;
		}
		break;
//																						***019 End

	case stavals:								//list of unsigned ints (ui|ui...)
		np=strchr(pstr,'(');
		if (np==0) break;
		np++;
		for (;;)                                
		{	if (opmax<=5) return 0;				//can't fit encoded value
			cp=strchr(np,'|');				//next comma-sep value
			if (cp==0) 
			{   cp=strchr(np,')');
				if (cp==0) break;
				last=TRUE;
			}
			*cp++=0;
			tp=op;
			op=eDWORD(op,atoi(np),0x20,TRUE);	//minimal encode using tag
			if (last) break;;
 			opmax-=op-tp;
			np=cp;								//next value
		}
		break;
//																						***027 End
	case ebool:									//a boolean								***016 Begin
		if (opmax<5) return 0;
		n=VTSAPIgetenumtable(ep,0,NULL,NULL,ptext);		//get number of enumerations this pointer (n will always be 2!)
		for (i=0;i<n;i++)
		{	n=VTSAPIgetenumtable(ep,i,NULL,NULL,ptext);	//get text for enumeration
			if(stricmp(ptext,pstr)==0)				//we got it
			{	op=eBOOL(op,(BOOL)i);
				return op;
			}
		}
		return 0;								//cound not be encoded					***016 End

	case et:									//an enumeration table
	case pab:									//priority array bpv
		if (opmax<5) return 0;
		op=eepENUM(op,pstr,ep);					//										***019
		return op;								//										***019
	case paf:									//priority array flt
	case flt:									//float
		if (opmax<5) return 0;
		op=eREAL(op,(float)atof(pstr));
		break;
	case u127:									//1..127
		max=127;
		min=1;
		msize=2;
		tag=0x20;
		goto svals;
	case u16:									//1..16
		max=16;
		min==0;
		msize=2;
		tag=0x20;
		goto svals;
	case pau:									//priority array uw
	case uw:									//unsigned word
		dmax=65535;
		dmin=0;
		msize=3;
		goto dvals;
	case ud:									//unsigned dword
		dmax=0;
		msize=5;
dvals:	if (opmax<msize) return 0;
		dval=strtoul(pstr,NULL,10);
		if (dmax!=0)
			if ((dval>dmax)||(dval<dmin)) return 0;
		op=eDWORD(op,dval,0x20,FALSE);			//minimal encode using tag 2
		break;
	case sw:									//signed word
		max=32767;
		min=-32768;
		msize=3;
		tag=0x30;
svals:	if (opmax<msize) return 0;
		val=atoi(pstr);
		if ((val>max)||(val<min)) return 0;
		op=eDWORD(op,val,tag,TRUE);				//minimal encode using tag
		break;		
	case statext:								//state text array
	case actext:								//action_text array
		msize=0;
		goto sstring;
	case s10:									//char [10]
		msize=10;
		goto sstring;
	case s32:									//char [32]
		msize=32;
		goto sstring;
	case s64:									//char [64]
		msize=64;
		goto sstring;
	case s132:									//char [132]
		msize=132;
sstring: 
		if ((msize!=0)&&(opmax<msize)) return 0;
		else if ((msize==0)&&(opmax<128)) return 0;
		n=safestrlen(pstr);						//										***236
		if ((n>msize)&&(msize!=0)) n=msize;
		op=eCHARSTRING(op,pstr,0x70,n,ANSI,	0);	//										***232
		break;
//																						***020 Begin
	case enull:
		if (opmax<1) return 0;
		op=eNULL (op);
		break;
//																						***020 End	

// --below this line cannot be modified!
	case pss:									//protocol services supported bitstring
	case pos:									//protocol objects supported bitstring
	case none:									//no parsing required, runtime generated value
	default:
		return 0;
	}
	return op;
}												//										***012 End

//																						***019 Begin
///////////////////////////////////////////////////////////////////////
//	Parse a text "(OBJECT-ID,instance)" into a BACnet object id
//in:	op			points to buffer to encode into 
//		pstr		points to a string value to be encoded after parsing
//		&objtype	pointer to word to return object type
//out:	updated op 	if ok
//		0			if unable to encode


octet  *eObjId (octet *op,char *pstr,word *objtype)
{	dword	inst;
	word	i,n,nc;
	char 	ptext[64];
	char *cp,*np;
	
		cp=strchr(pstr,'(');
    	if (cp==NULL) return 0;
    	cp++;									//we have object identifier form (object-id,instance)
		np=strchr(cp,',');
		nc=np-cp;								//number of characters in object id
		np++;
		inst=atol(np);							//get instance
		n=VTSAPIgetenumtable(eiObjectTypes,0,NULL,NULL,ptext);		//get number of enumerations this pointer
		for (i=0;i<n;i++)
		{	n=VTSAPIgetenumtable(eiObjectTypes,i,NULL,NULL,ptext);	//get text for enumeration
			if(strnicmp(ptext,cp,nc)==0)							//we got it
			{	*objtype=i;
				op=etagOBJECTID(op,0xC0,vbOBJECTID(i,inst));
				return op;
			}
		}
		return 0;
}

///////////////////////////////////////////////////////////////////////
//	Encode an ENUM type based on a supplied ep
//in:	op			points to buffer to encode into                      
//		pstr		points to a string value to be encoded after parsing
//		ep			is the ei table index
//out:	updated op 	if ok
//		0			if unable to encode

octet  *eepENUM (octet *op,char *pstr,word ep)
{	word	i,n;
	char	ptext[64];
	
	n=VTSAPIgetenumtable(ep,0,NULL,NULL,ptext);		//get number of enumerations this pointer
	for (i=0;i<n;i++)
	{	n=VTSAPIgetenumtable(ep,i,NULL,NULL,ptext);	//get text for enumeration
		if(stricmp(ptext,pstr)==0)				//we got it
		{	op=eDWORD(op,(dword)i,0x90,FALSE);
			return op;
		}
	}
	return 0;								//could not be encoded
}
//																						***028 Begin
///////////////////////////////////////////////////////////////////////
//	Encode a list of Time Values
//in:	op			points to buffer to encode into 
//		opmax		length of available buffer op points to
//		pstr		points to a string value to be encoded after parsing
//out:	updated op 	if ok
//		0			if unable to encode


octet  *atotimeval (octet *op,word opmax,char *pstr)
{	char		*cp,*np;
	BOOL		last=FALSE;

										    	//list of time vals (time,(tag)value|...)
	cp=strchr(pstr,'(');
	cp++;
	for (;;)
	{	if (opmax<10) return 0;					//can't save time and at least max primative encode except strings
		np=cp;
		cp=strchr(cp,',');
		*cp++=0;
		op=atoeTIMESTRING(op,np);
		np=cp;
		opmax-=5;
		cp=strchr(np,'|');
		if (cp==0)
		{	last=TRUE;
			cp=strchr(np,')');
			cp++;
			cp=strchr(cp,')');
		}
		*cp++=0;								//cp points to next time
		op=atoprim (op,opmax,np);
		if (op==0) return 0;
		if (last) break;
	}
	return op;
}

///////////////////////////////////////////////////////////////////////
//	Encode a primative tag
//in:	op			points to buffer to encode into 
//		opmax		length of available buffer op points to
//		pstr		points to a string value to be encoded after parsing
//out:	updated op 	if ok
//		0			if unable to encode


octet  *atoprim (octet *op,word opmax,char *pstr)
{	octet		tag;
	dword		dval;
	word		objtype,n,i,bn;
	char		*cp,*np,*tp;
	byte 		bm,b[4];

										    	//list of time vals (tag)value
	cp=strchr(pstr,'(');
	cp++;
	np=strchr(cp,')');
	*np++=0;								//np points at value
	tag=(octet)atoi(cp);					//get tag
	tp=op;
	switch (tag)
	{
	case 0:									//null
		op=eNULL(op);
		break;				
	case 1:									//bool
		op=eBOOL(op,(BOOL)atoi(np));
		break;
	case 2:									//uint
		dval=strtoul(np,NULL,10);
		op=eDWORD(op,dval,(octet)(tag<<4),FALSE);
		break;
	case 3:									//sint									
		dval=strtoul(np,NULL,10);
		op=eDWORD(op,dval,(octet)(tag<<4),TRUE);
		break;
	case 4:									//real
	case 5:									//dbl
		op=eREAL(op,(float)atof(np));
		break;
	case 6:									//octet string
		n=safestrlen(np);					//											***236
		if (n>256) return 0;
		op=atooct(op,np);
		opmax-=(n/2)+1;
		break;
	case 7:									//char string
		if (opmax<129) return 0;
		n=safestrlen(np);					//											***236
		if (n>128) n=128;
		op=eCHARSTRING(op,np,0x70,n,ANSI,0);	//										***232
		break;
	case 8:									//bit string value=significant bits/0bvalue...
	    cp=strchr(np,'/');
		*cp++=0;
		n=atoi(np);							//significant bits
		cp+=2;								//skip 0b
		if (n>31) return 0;
		bn=0;
		bm=0x80;
		b[0]=0;
		for (i=0;i<n;i++)
		{	if (*cp=='1') b[bn]|=bm;
			cp++;
			bm>>=1;
			if (bm==0)
			{	bn++;
				bm=0x80;
				b[bn]=0;
			}
		}
		op=eBITSTRING(op,b,0x80,n);	
		break;
	case 9:									//enum
		dval=strtoul(np,NULL,10);
		op=eENUM(op, dval);
		break;
	case 10:								//date
		op=atoeDATESTRING(op,np);
		break;
	case 11:								//time
		op=atoeTIMESTRING(op,np);
		break;
	case 12:								//objid
		op=eObjId(op,np,&objtype);
		break;
	default:
		return 0;							//invalid tag
	}
	opmax-=op-tp;
	return op;
}		
//																						***028 End
///////////////////////////////////////////////////////////////////////
//	Encode an OCTET type from an ascii string
//in:	op			points to buffer to encode into                      
//		pstr		points to an asciiz string value to be encoded after parsing
//out:	updated op 	if ok
//		0			if unable to encode

octet *atooct (octet *op,char *pstr)
{	char	*ap,atext[256];						//										***028
	octet	obuf[128];							//										***028
	word	nc;

		nc=safestrlen(pstr);					//number of characters					***236
		if ((nc==0)||(nc>256)) return 0; 		//										***028
		ap=pstr;								//in case we have an even number of characters
		if (nc&1)								//odd number of characters
		{	ap=atext;
			*ap++='0';							//must add leading '0'
			strncpy(ap,pstr,nc);				//move remaining text into atext
		}
		nc=(nc+1)/2;							//number of octets
		cvhexn(ap,(byte *)obuf,nc);
		op=eOCTETSTRING (op,obuf,nc);		        
		return op;
}
//																						***019 End
//																						***021 Begin
///////////////////////////////////////////////////////////////////////
//	Encode an BITSTRING type based on a supplied ep and comma separated
//  list of ascii bits, eg, "Monday,Wednesday,Thursday" to build a D.O.W.
//	bit string.
//in:	op			points to buffer to encode into                      
//		pstr		points to a string value to be encoded after parsing
//		ep			is the ei table index or 0 meaning unknown							***237
//out:	updated op 	if ok
//		0			if unable to encode

octet *atoeBITSTRING (octet *op,char *pstr,word ep)
{	word		i,n;
	dword		dmask;
	char 		*np,*cp;
	char		ptext[64];
	union 		{ dword dw; byte b[4];} u;
	byte		btemp;
	
//	for now handle up to 4 octets!!
	cp=pstr;
	u.dw=0;
	if(ep==0)									//bitstring 'type' is unknown			***237 Begin
		n=0;									//then make it "stretchy"
	else										//we know the type, so look up the size
		n=VTSAPIgetenumtable(ep,0,NULL,NULL,ptext);	//get number of significant bits

	if (*cp!=0x00)								//not an empty string					***237 End
	{
		u.dw=0;
		for (;;)
		{	np=strchr(cp,',');
			if (np!=0) *np++=0x00;				//not last one in a list so make it asciiz
			dmask=0x80000000;					//bits are flipped in BACnet
			for (i=0;i<n;i++)
			{	n=VTSAPIgetenumtable(ep,i,NULL,NULL,ptext);	//get text for enumeration
				if(stricmp(ptext,cp)==0)
				{	u.dw|=dmask;
					break;
				}
				dmask>>=1;
			}
			if (i>=n) break;					//should never happen
			if (np==0) break;					//last one
			cp=np;
		}
		btemp=u.b[0];							//put them in BACnet order
		u.b[0]=u.b[3];
		u.b[3]=btemp;
		btemp=u.b[1];
		u.b[1]=u.b[2];
		u.b[2]=btemp;
	}
	return eBITSTRING(op,&u.b[0],0x80,n);	
}

///////////////////////////////////////////////////////////////////////
//	Encode an TIMESTRING from ascii "hh:mm:ss.xx"
//in:	op			points to buffer to encode into                      
//		pstr		points to a string value to be encoded after parsing
//out:	updated op 	if ok
//		0			if unable to encode

octet *atoeTIMESTRING(octet *op,char *pstr)
{	octet hour=0xFF,min=0xFF,sec=0xFF,hun=0xFF;

	if (*pstr!='?') hour=(octet)atoi(pstr);
    pstr=strchr(pstr,':');
    pstr++;
    if (*pstr!='?') min=(octet)atoi(pstr);
    pstr=strchr(pstr,':');
    pstr++;
    if (*pstr!='?') sec=(octet)atoi(pstr);
    pstr=strchr(pstr,'.');
    pstr++;
    if (*pstr!='?') hun=(octet)atoi(pstr);
	return eTIMESTRING (op,hour,min,sec,hun);
}

///////////////////////////////////////////////////////////////////////
//	Encode an DATESTRING from ascii "date,month,year,dow"
//in:	op			points to buffer to encode into                      
//		pstr		points to a string value to be encoded after parsing
//out:	updated op 	if ok
//		0			if unable to encode

octet *atoeDATESTRING(octet *op,char *pstr)
{	octet		month=0xFF,day=0xFF,dow=0xFF;
	word		year=0xFF,i,n;
	char 	*cp,*np;
	char		ptext[64];
    
    cp=pstr;
    np=strchr(pstr,',');
    *np++=0;
	if(stricmp(cp,anystring)!=0) day=(octet)atoi(cp);	//get the day
	cp=np;
    np=strchr(np,',');
    *np++=0;
	n=VTSAPIgetenumtable(eiMon,0,NULL,NULL,ptext);		//get number of month strings
	for (i=0;i<n;i++)
	{	n=VTSAPIgetenumtable(eiMon,i,NULL,NULL,ptext);	//get text for enumeration
		if(stricmp(ptext,cp)==0)						//we found the month
		{	if (i!=0) month=(octet)i;
			break;
		}
	}
	cp=np;
    np=strchr(np,',');
    *np++=0;
	if(stricmp(cp,anystring)!=0) year=atoi(cp);		//get the year
	cp=np;
	n=VTSAPIgetenumtable(eiAnyDOW,0,NULL,NULL,ptext);	//get number of dow strings
	for (i=0;i<n;i++)
	{	n=VTSAPIgetenumtable(eiAnyDOW,i,NULL,NULL,ptext);	//get text for enumeration
		if(stricmp(ptext,cp)==0)						//we found the dow
		{	if (i!=0) dow=(octet)i;
			break;
		}
	}
	return eDATESTRING (op,year,month,day,dow);
}
///////////////////////////////////////////////////////////////////////
//	Encode an BACnetRecipient of the form:
//		"Device,object-type,instance" or
//		"Address,network,macaddress"
//in:	op			points to buffer to encode into                      
//		pstr		points to a string value to be encoded after parsing
//out:	updated op 	if ok
//		0			if unable to encode

octet *eRECIPIENT(octet *op,char *pstr)
{   word		nc,objtype;					//								***236
	char 		*cp;
	char		ptext[64];
	
	if ((*pstr=='D')||(*pstr=='d'))			//we have a device 
	{	ptext[0]='(';						//form string "(device,inst)
		nc=safestrlen(pstr);				//								***236
		strncpy(&ptext[1],pstr,nc);			//copy "device,inst"
		ptext[nc+1]=')';                      
		ptext[nc+2]=0x00;
//		*op++=0x0E;							//open context tag 0			***024 Begin
		cp=op;
		op=eObjId (op,ptext,&objtype);		//get object-id
		if (op==0) return 0;
		*cp=0x0C;							//SD tag 0, L=4
//		*op++=0x0F;							//close context tag 0			***024 End
		return op;
	}
	else									//we have an address
	{	pstr=strchr(pstr,',');
		pstr++;
		nc=(word)strtoul(pstr,NULL,10);		//get network number 			***236
 		*op++=0x1E;							//open context tag 1
		op=eWORD(op,nc,0x20);				//encode using tag 2			***236
		pstr=strchr(pstr,',');
		pstr++;
		cp=strchr(pstr,0x00);
//		nc=cp-pstr;							//								***236
		op=atooct(op,pstr);
		if (op==0) return 0;
		*op++=0x1F;							//close context tag 1
		return op;
	}
}
//																						***021 End
///////////////////////////////////////////////////////////////////////					***010 Begin 
//	Encode a Time Stamp choice
//in:	op			points to a buffer to encode into
//		ts			points to an etimestamp structure containing the choice info
//					choice is driven from tsType: 0=time, 1=sequence number, 2=date/time
//					for tsType=1, the sequence number is passed in the tsYear word
//		tag			PD tag tag number or 255=no PD tag required
//out:	pointer past encoded octets

octet * eTIMESTAMP(octet *op,etimestamp *ts,octet tag)
{
	if (tag!=255)									//we need a PD open tag
		*op++=(tag<<4)+0x0E;
	switch(ts->tsType)
	{
	case 0:											//time
		op=eTIMESTRING(op,(octet)ts->tsHour,(octet)ts->tsMinute,(octet)ts->tsSecond,(octet)ts->tsHundredth);
		op[-5]=0x0C;								//overwrite tag with context tag 0, length 4
		break;
	case 1:											//sequence number
		op=eDWORD(op,ts->tsYear,0x18,FALSE);
		break;
	case 2:											//date/time
		*op++=0x2E;									//PD open tag 2 
		op=eDATESTRING(op,ts->tsYear,(octet)ts->tsMonth,(octet)ts->tsDay,(octet)ts->tsDow);
		op=eTIMESTRING(op,(octet)ts->tsHour,(octet)ts->tsMinute,(octet)ts->tsSecond,(octet)ts->tsHundredth);
		*op++=0x2F;									//PD close tag 2
		break;
	}
	if (tag!=255)									//we need a PD close tag
		*op++=(tag<<4)+0x0F;
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode a Text Message Service
//in:	op			points to buffer to encode into 
//		opmax		length of available buffer op points to
//		confirmed	0=unconfirmed, else confirmed
//		devinst		device instance
//		priority	0=normal, 1=urgent
//		msgclass	points to a buffer which may be blank, or contain a number or a character class
//		msg			the actual text
//out:	=0			the message can't be encoded in opmax octets
//		>0			the number of octets encoded

word  APIENTRY eTextMessageService(octet *op,word opmax,word confirmed,dword devinst,
											  word priority,char *msgclass,char *msg)
{	octet *iop;
	word  msglen;
	char *cp;

	iop=op;											//remember starting point
	if (confirmed)
		op=eCONFREQ(op,confirmedTextMessage);
	else
	{	*op++=UNCONF_REQ_PDU<<4;
		*op++=unconfirmedTextMessage;
	}
	op=etagOBJECTID(op,0x08,vbOBJECTID(DEVICE,devinst));
	if ((msgclass)&&(*msgclass))					//we've got something for a message class***235
	{	*op++=0x1E;									//PD open tag 1
		cp=msgclass;								//first see if we have only digits
		while(*cp)
		{	if (isdigit(*cp))
				cp++;
			else
				break;
		}
		if (*cp)									//wasn't all digits
			op=eCHARSTRING(op,msgclass,0x18,safestrlen(msgclass),ANSI,0);	//			***236
		else										//all digits
			op=eDWORD(op,atol(msgclass),0x08,FALSE);
		*op++=0x1F;									//PD close tag 1
	}
	op=eDWORD(op,priority,0x28,FALSE);
	msglen=safestrlen(msg);							//									***236 Begin
	if (((op-iop)+3+msglen)>opmax)					//can't buffer it all
		return 0;
	op=eCHARSTRING(op,msg,0x38,msglen,ANSI,0);		//encode a regular string			***236 End
	return (op-iop);
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Acknowledge Alarm Service
//in:	op			points to buffer to encode into 
//		procid		acknowledging process ID
//		evobid		event object identifier
//		evsack		event state being acknowledged
//		timestamp	when it occurred
//		acksrc		who did it
//		acktimestamp when it was acked
//out:	return	the number of octets encoded

word  APIENTRY eAcknowledgeAlarmService(octet *op,dword procid,dword evobid,word evsack,
													etimestamp *timestamp,char *acksrc,etimestamp *acktimestamp)
{	octet *iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,acknowledgeAlarm);
	op=eDWORD(op,procid,0x08,FALSE);
	op=etagOBJECTID(op,0x18,evobid);
	op=eDWORD(op,evsack,0x28,FALSE);
	op=eTIMESTAMP(op,timestamp,3);
	op=eCHARSTRING(op,acksrc,0x48,safestrlen(acksrc),ANSI,0);	//encode a regular string ***236
	op=eTIMESTAMP(op,acktimestamp,5);
	return (op-iop);
}													//									***010 End

///////////////////////////////////////////////////////////////////////					***005 Begin 
//	Encode a Read Property Service
//in:	op			points to buffer to encode into 
//		obid		object identifier													***010
//		propid		property identifier
//		havearray	true if we need an array index
//		arrind		array index
//out:	return	the number of octets encoded

word  APIENTRY eReadPropService(octet *op,dword obid,
										   dword propid,word havearray,dword arrind)
{	octet *iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,readProperty);
	op=etagOBJECTID(op,0x08,obid);					//									***010
	op=eDWORD(op,propid,0x18,FALSE);
	if (havearray)	
		op=eDWORD(op,arrind,0x28,FALSE);
	return (op-iop);
}
										   
///////////////////////////////////////////////////////////////////////					***011 Begin 
//	Encode a Read Property Multiple Service
//in:	op			points to buffer to encode into 
//		nprops		number of object/property references pp points to
//		pp			array of proplist structures nprops long
//					note: pp->plArrayIndex is normally 0xFFFFFFFF meaning "no array index"
//						  values <0FFFFFFFE mean "include array index"
//out:	return	the number of octets encoded

word  APIENTRY eReadPropMultipleService(octet *op,word nprops,proplist *pp)
{	octet 	*iop;
	word		i;
	BOOL		closepending=FALSE;
	dword		prevobjid=0xFFFFFFFF;				//force object id out the first time

	iop=op;											//remember starting point
	op=eCONFREQ(op,readPropertyMultiple);
	for (i=0;i<nprops;i++)
	{	if (prevobjid!=pp->plObjectID)				//a new object id
		{	if (closepending)
			{	*op++=0x1F;							//PD close tag 1
				closepending=FALSE;
			}
			op=etagOBJECTID(op,0x08,pp->plObjectID);
			*op++=0x1E;								//PD open tag 1
			closepending=TRUE;
			prevobjid=pp->plObjectID;				//remember the new object ID
		}
		op=eDWORD(op,pp->plPropID,0x08,FALSE);		//output property id
		if (pp->plArrayIndex!=0xFFFFFFFF)	
			op=eDWORD(op,pp->plArrayIndex,0x18,FALSE);	//and array index
		pp++;
	}
	if (closepending)
		*op++=0x1F;									//PD close tag 1
	return (op-iop);
}													//									***011 End

///////////////////////////////////////////////////////////////////////					***012 Begin 
//	Encode a Write Property Service
//in:	op			points to buffer to encode into
//		opmax		maximum size buffer will handle 
//		obid		object identifier
//		pstr		points to a string like "propid=value" to be encoded after parsing
//					syntax forms for the string are loosely:
//					propid=value
//					propid[arrayindex]=value
//					propid=value@priority
//					propid[arrayindex]=value@priority
//		
//out:	return		the number of octets encoded
//					0=could not encode

word  APIENTRY eWritePropService(octet *op,word opmax,word objtype,dword objinst,char *pstr)
{	octet *iop;
	word 	n;
	
	iop=op;											//remember starting point
	op=eCONFREQ(op,writeProperty);
	op=etagOBJECTID(op,0x08,vbOBJECTID(objtype,objinst));
	n=ePropVal(op,(word)(opmax-(op-iop)),objtype,0x18,pstr);
	if (n==0) return 0;
	op+=n;
	return (op-iop);
}

///////////////////////////////////////////////////////////////////////
//	Encode a Write Property Multiple Service
//in:	op			points to buffer to encode into 
//		opmax		maximum size buffer will handle 
//		nprops		number of object/property references pp points to
//		pp			array of proplist structures nprops long
//out:	return	the number of octets encoded

word  APIENTRY eWritePropMultipleService(octet *op,word opmax,word nprops,proplist *pp)
{	octet 	*iop;
	word		i,n,objtype;
	BOOL		closepending=FALSE;
	dword		prevobjid=0xFFFFFFFF;				//force object id out the first time

	iop=op;											//remember starting point
	op=eCONFREQ(op,writePropertyMultiple);
	for (i=0;i<nprops;i++)
	{	if (prevobjid!=pp->plObjectID)				//a new object id
		{	if (closepending)
			{	*op++=0x1F;							//PD close tag 1
				closepending=FALSE;
			}
			op=etagOBJECTID(op,0x08,pp->plObjectID);
			*op++=0x1E;								//PD open tag 1
			closepending=TRUE;
			prevobjid=pp->plObjectID;				//remember the new object ID
		}
		objtype=(word)((pp->plObjectID>>22)&0x3FF);						//return device type
		n=ePropVal(op,(word)(opmax-(op-iop)),objtype,0x08,pp->plPropVal);
		if (n==0) return 0;
		op+=n;
		pp++;
	}
	if (closepending)
		*op++=0x1F;									//PD close tag 1
	return (op-iop);
}													//									***012 End
										   
//																						***015 Begin
///////////////////////////////////////////////////////////////////////
//	Encode a Read Property Conditional Service
//in:	op			points to buffer to encode into 
//		opmax		maximum size buffer will handle 
//		selectlogic	the criteria selection logic (0=AND, 1=OR, 2=ALL)
//		hcrit		handle to the criteria list
//		hrefs		handle to the property refs list (NULL=none)
//out:	return	the number of octets encoded

word  APIENTRY eReadPropConditionalService(octet *op,word opmax,word selectlogic,HWND hcrit,HWND hrefs)
{	octet 	*iop;
	word	nv,i,j,c;
	octet	p[128];

	iop=op;											//remember starting point
	op=eCONFREQ(op,readPropertyConditional);
	if ((word)(op-iop+4)>opmax) return 0;			//account for outer tags
	*op++=0x0E;										//opening tag 0 object selection criteria
	op=eDWORD(op,selectlogic,0x08,FALSE);			//context tag 0 selection logic
	if (selectlogic!=2)								//either AND or OR, we should have criteria
	{	if (hcrit==NULL) return 0;
		nv=(word)GetListCount(hcrit);				//how many properties?
		if (nv)										//got some
		{	if ((word)(op-iop+2)>opmax) return 0;	//account for outer tags
			*op++=0x1E;								//open tag 1 list of selection criteria
			for (i=0;i<nv;i++)
			{	GetListText(hcrit,i,p);				//get the selection criterium text to encode
				if (c=eSelCrit(op,(word)(opmax-(op-iop)),p))
					op+=c;
				else
					return 0;						//can't encode it		
			}
			*op++=0x1F;								//close tag 1 list of selection criteria
		}
		else return 0;								//better have some criteria
	}
	*op++=0x0F;										//closing tag 0 object selection criteria

	if (hrefs==NULL) return (op-iop);				//no property refs
	nv=(word)GetListCount(hrefs);					//how many property refs?
	if (nv)											//got some
	{	if ((word)(op-iop+2)>opmax) return 0;		//account for outer tags
		*op++=0x1E;									//open tag 1 list of property refs
		for (i=0;i<nv;i++)
		{	GetListText(hrefs,i,p);					//get the property reference text to encode
		    for (j=0;j<stPropIDs.nes;j++)
			{	if (stricmp(p,stPropIDs.estrings[j])==0)
				{	if ((word)(op-iop+2)>=opmax) return 0;			
					op=eDWORD(op,j,0x08,FALSE);		//context tag 0 property id
					break;
				}
			}
			if (j==stPropIDs.nes) return 0;   
		}
		*op++=0x1F;									//close tag 1 list of property refs
	}
	return (op-iop);
}
//																						***015 End
/////////////////////////////////////////////////////////////////////// 
//	Encode a Reinitialize Device Service
//in:	op			points to buffer to encode into 
//		rstate		type of reinit, 0=cold, 1=warm
//		password	0..20 char string
//out:	return	the number of octets encoded

word  APIENTRY eReinitDeviceService(octet *op,word rstate,char *password)
{	octet *iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,reinitializeDevice);
	op=eDWORD(op,rstate,0x08,FALSE);
	if ((password)&&(*password))					//								***235
		op=eCHARSTRING (op,password,0x18,safestrlen(password),ANSI,0);	//encode a regular string ***236
	return (op-iop);
}

/////////////////////////////////////////////////////////////////////// 
//	Encode a Time Synchronization Service
//in:	op			points to buffer to encode into 
//		d			date
//		t			time
//out:	return	the number of octets encoded

word  APIENTRY eTimeSynchService(octet *op,word year, octet month, octet day, octet dow,
														  octet hour, octet min, octet sec, octet hun)
{	octet *iop;

	iop=op;											//remember starting point
	*op++=UNCONF_REQ_PDU<<4;
	*op++=timeSynchronization;
	op=eDATESTRING (op,year,month,day,dow);
	op=eTIMESTRING (op,hour,min,sec,hun);
	return (op-iop);
}

/////////////////////////////////////////////////////////////////////// 
//	Encode a Device Communication Control Service
//in:	op			points to buffer to encode into
//		duration	time to be quiet in seconds 
//		enable		type of reinit, 0=disable, 1=enable
//		password	0..20 char string
//out:	return	the number of octets encoded

word  APIENTRY eDeviceCommControlService(octet *op,word duration,word enable,char *password)
{	octet *iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,deviceCommunicationControl);
	if (duration)
		op=eDWORD(op,(dword)duration,0x08,FALSE);
	op=eDWORD(op,(dword)enable,0x18,FALSE);
	if ((password)&&(*password))					//									***235
		op=eCHARSTRING (op,password,0x28,safestrlen(password),ANSI,0);	//encode a regular string ***236
	return (op-iop);
}

/////////////////////////////////////////////////////////////////////// 
//	Encode a Get Alarm Summary Service
//in:	op			points to buffer to encode into
//out:	return	the number of octets encoded

word  APIENTRY eGetAlarmSummaryService(octet *op)
{	octet *iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,getAlarmSummary);
	return (op-iop);
}													//									***005 End

///////////////////////////////////////////////////////////////////////					***004 Begin 
//	Encode an IAm Service
//in:	op		points to buffer to encode into 
//		inst	device object instance
//		mplen	max APDU length
//		segsup	segmentation supported (0=both,1=tx,2=rx,3=none)
//		vid		vendor ID
//out:	return	the number of octets encoded

word  APIENTRY eIAmService(octet *op,dword inst,word mplen,word segsup,word vid)
{	octet *iop;

	iop=op;											//remember starting point
	*op++=UNCONF_REQ_PDU<<4;
	*op++=I_Am;
	op=eOBJECTID(op,DEVICE,inst);
	op=eUINT(op,(dword)mplen);
	op=eENUM(op,(dword)segsup);	
	op=eUINT(op,(dword)vid);	
	return (op-iop);
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an WhoIs Service
//in:	op			points to buffer to encode into
//		haverange	true if range parameters should be included
//		rlow,rhigh	device instance numbers for range
//out:	return	the number of octets encoded

word  APIENTRY eWhoIsService(octet *op,word haverange,dword rlow,dword rhigh)
{	octet *iop;

	iop=op;											//remember starting point
	*op++=UNCONF_REQ_PDU<<4;
	*op++=who_Is;
	if (haverange)
	{
		op=eDWORD(op,rlow,0x08,FALSE);				//encode range as context tagged UINTs!
		op=eDWORD(op,rhigh,0x18,FALSE);
	}	
	return (op-iop);
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an IHave Service
//in:	op		points to buffer to encode into
//		devinst	device instance
//		obid	object identifier													***010
//		obname	object name
//out:	return	the number of octets encoded

word  APIENTRY eIHaveService(octet *op,dword devinst,dword obid,char *obname)
{	octet *iop;

	iop=op;											//remember starting point
	*op++=UNCONF_REQ_PDU<<4;
	*op++=I_Have;
	op=eOBJECTID(op,DEVICE,devinst);
	op=etagOBJECTID(op,0xC0,obid);					//								***010
	op=eCHARSTRING(op,obname,0x70,safestrlen(obname),ANSI,0);	//encode a regular string ***236
	return (op-iop);
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an WhoHas Service
//in:	op			points to buffer to encode into
//		hasrange	true if rlow,rhigh are used
//		rlow,rhigh	device instance range
//		obid		FFFFFFFF=use obname, else object identifier						***010
//		obname		name to use in case obtyp=FFFF
//out:	return	the number of octets encoded

word  APIENTRY eWhoHasService(octet *op,word hasrange,dword rlow,dword rhigh,
										 dword obid,char *obname)
{	octet *iop;

	iop=op;											//remember starting point
	*op++=UNCONF_REQ_PDU<<4;
	*op++=who_Has;
	if (hasrange)
	{
		op=eDWORD(op,rlow,0x08,FALSE);				//encode range as context tagged UINTs!
		op=eDWORD(op,rhigh,0x18,FALSE);
	}
	if (obid==0xFFFFFFFF)							//								***010
		op=eCHARSTRING(op,obname,0x38,safestrlen(obname),ANSI,0);	//encode a context tagged string ***236
	else
		op=etagOBJECTID(op,0x28,obid);				//								***010
	return (op-iop);
}													//								***004 End
                                                                            
//																					***017 Begin
///////////////////////////////////////////////////////////////////////
//	Encode a VT Open Service
//in:	op			points to buffer to encode into
//		vtclass		the vt class of the session we're opening 
//		localsession the local session we're opening
//out:	return	the number of octets encoded

word  APIENTRY eVTOpenService(octet *op,word vtclass,word localsession)
{	octet 	*iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,vtOpen);
	op=eDWORD(op,vtclass,0x90,FALSE);				//the vt class of the session we're opening
	op=eDWORD(op,localsession,0x20,FALSE);			//session to open
	return (op-iop);
}

///////////////////////////////////////////////////////////////////////
//	Encode a VT Data Service
//in:	op			points to buffer to encode into 
//		opmax		maximum size buffer will handle 
//		remotesession	the remote session to which the data is sent
//		dataflag	the VT data (sync) flag (0 or 1) for this data
//		noctets		number of octets in the data
//		vtdata		the data
//out:	return	the number of octets encoded

word  APIENTRY eVTDataService(octet *op,word opmax,word remotesession,word dataflag,word noctets,octet *vtdata)
{	octet 	*iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,vtData);
	if ((word)(op-iop+4+noctets)>opmax) return 0;	//account for session and data flag
	op=eDWORD(op,remotesession,0x20,FALSE);			//session for the data
	op=eOCTETSTRING (op,vtdata,noctets);			//the vt data
	op=eDWORD(op,dataflag,0x20,FALSE);				//the data flag
	return (op-iop);
}
///////////////////////////////////////////////////////////////////////
//	Encode a VT Close Service
//in:	op			points to buffer to encode into
//		vtclass		the vt class of the session we're opening 
//		remotesession the remote session we're closing
//out:	return	the number of octets encoded

word  APIENTRY eVTCloseService(octet *op,word remotesession)
{	octet 	*iop;

	iop=op;											//remember starting point
	op=eCONFREQ(op,vtClose);
	op=eDWORD(op,remotesession,0x20,FALSE);			//session to close
	return (op-iop);
}
//																					***017 End
/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Null Value
//in:	op		points to an octet buffer to put the results in
//out:	return	advanced pointer to buffer

octet * APIENTRY eNULL (octet *op)
{
	*op++=0;
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Boolean Value
//in:	op		points to an octet buffer to put the results in
//		val		has the boolean state (TRUE or FALSE) to encode
//out:	return	advanced pointer to buffer

octet * APIENTRY eBOOL (octet *op, BOOL val)
{
	*op++=((val) ? 0x11:0x10);						//tag 1, and boolean value
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Unsigned Integer Value
//in:	op		points to an octet buffer to put the results in
//		val		has the value to encode
//out:	return	advanced pointer to buffer

octet * APIENTRY eUINT (octet *op, dword val)
{	return eDWORD(op,val,0x20,FALSE);}				//minimal encode using tag 2

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Enumerated Value
//in:	op		points to an octet buffer to put the results in
//		eval	has the dword enumerated value to encode
//out:	return	advanced pointer to buffer

octet * APIENTRY eENUM(octet *op, dword eval)
{	return eDWORD(op,eval,0x90,FALSE);}				//minimal encode using tag 9

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Signed Integer Value
//in:	op		points to an octet buffer to put the results in
//		val		has the long value to encode
//out:	return	advanced pointer to buffer

octet * APIENTRY eINT (octet *op, long val)
{	return eDWORD(op,val,0x30,TRUE);}				//minimal encode using tag 3

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged 4 octet minimal encoding value
//in:	op		points to an octet buffer to put the results in
//		val		has the value to encode
//		utag	is the tag value to use, e.g. 0x20 is tag 2
//		sgnd	TRUE if signed, FALSE if unsigned
//out:	return	advanced pointer to buffer

octet * APIENTRY eDWORD (octet *op, dword val,octet utag,BOOL sgnd)		//***031
{	union { dword dw; byte b[4];} u;
	int		i;										//							***002
    BOOL 	ffndb=FALSE;							//found first non-discardable byte
    BOOL	db;										//true if this is a discardable byte
    
	u.dw=val;										//so we can get at individual bytes
	*op++=utag+0x04;								//tag (utag), assume 4 octets (dword)
	for (i=3;i>=0;i--)								//store all bytes from first non-0 ms byte, ms first, ls last
	{	db=(u.b[i]==0);								//if 0, it's discardable
		if (sgnd&&(u.b[i]==0xFF)) db=TRUE;			//also discardable if signed and it's FF
//																			***233 Begin
// The following code is added so that the following cases of SIGNED numbers are encoded properly:
//		0x00000080	encoded as 32 00 80
//		0xFFFFFF80	encoded as 31 80
//		0xFFFFFF7f	encoded as 32 FF 7F
//		etc.
		if (sgnd && db && !ffndb && (i!=0))							//it's signed and possibly discardable
		{	if ((u.b[i]==0) && ((u.b[i-1]&0x80) != 0)) db=FALSE;	//don't discard
			else if ((u.b[i]==0xFF) && ((u.b[i-1]&0x80) == 0)) db=FALSE;	//don't discard 
		}
//																			***233 End
		if (!db||ffndb||i==0)
		{	*op++=(octet)u.b[i];					//save byte
			ffndb=TRUE;								//we found first non-discardable byte
		}
		else
			op[-1]--;								//one less digit than assumed
	}			
	return op;
}

///////////////////////////////////////////////////////////////////////			***236 Begin 
//	Encode an Application Tagged 2 octet value
//in:	op		points to an octet buffer to put the results in
//		val		has the value to encode
//		utag	is the tag value to use, e.g. 0x20 is tag 2
//out:	return	advanced pointer to buffer

octet * APIENTRY eWORD (octet *op, word val,octet utag)
{	union { word w; byte b[2];} u;
    
	u.w=val;										//so we can get at individual bytes
	*op++=utag+0x02;								//tag (utag), assume 2 octets (word)
	*op++=u.b[1];									//msb first
	*op++=u.b[0];
	return op;
}													//							***236 End

///////////////////////////////////////////////////////////////////////			***023 Begin 
//	Encode an Application Tagged Real Value
//in:	op		points to an octet buffer to put the results in
//		val		has the fp value to encode
//out:	return	advanced pointer to buffer

octet * APIENTRY eREAL (octet *op, float val)
{	return etagREAL(op,0x44,val); }					//tag 4, 4 octets (float)

octet * etagREAL (octet *op, octet tag, float val)
{	union { float rv; byte b[4];} u;
	int		i;

	u.rv=val;										//so we can get at individual bytes
	*op++=tag;
	for (i=3;i>=0;i--)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];			
	return op;
}													//							***023 End

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Double Value
//in:	op		points to an octet buffer to put the results in
//		val		has the double fp value to encode
//out:	return	advanced pointer to buffer

octet * APIENTRY eDOUBLE (octet *op, double val)
{	union { double dv; byte b[8];} u;
	int		i;

	u.dv=val;										//so we can get at individual bytes
	*op++=0x55;										//tag 5, extended length
	*op++=8;										//8 octets (double)
	for (i=7;i>=0;i--)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];			
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Octet String
//in:	op		points to an octet buffer to put the results in
//		os		points to the octet string to encode
//		n		has the number of octets to encode (0-65535)
//out:	return	advanced pointer to buffer

octet * APIENTRY eOCTETSTRING (octet *op, octet *os, word n)
{	union { word w; byte b[2];} u;
	word	i;

	u.w=n;											//so we can get byte access
	if (n<=4)
		*op++=0x60+(octet)n;						//tag 6, length
	else
	{	*op++=0x65;									//tag 6, extended length
		if (n<=253)
			*op++=(octet)n;
		else										//assume n<=65535
		{	*op++=0xFE;
			*op++=(octet)u.b[1];					//msb of length first
			*op++=(octet)u.b[0];
		}	
	}
	for (i=0;i<n;i++)								//store all octets in order
		*op++=*os++;			
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Character String
//in:	op		points to an octet buffer to put the results in
//		cs		points to the character string to encode
//		tag		tag value to use, e.g. 0x70
//		n		has the number of octets to encode (0-65535)
//				NOTE: this is the length of the string NOT including the
//					  characterset or code page octets!
//		cscode	has the characterset code (see BCstring enumeration)
//		cpage	is the code page for string type MS_DBCS
//out:	return	advanced pointer to buffer

octet * APIENTRY eCHARSTRING(octet *op,char *cs,octet tag,word n,octet cscode,word cpage)
{	union { word w; byte b[2];} u;
	word	i,nc;

	nc=n+1;											//string length plus characterset octet
	if (cscode==MS_DBCS) nc+=2;						//DBCS has two octet code page as well
	if (nc<=4)										//								***009
		*op++=tag+(octet)nc;						//tag ?, length
	else
	{	*op++=tag+5;								//tag ?, extended length
		if (nc<=253)								//								***009
			*op++=(octet)nc;
		else										//assume n<=65535
		{	*op++=0xFE;
			u.w=nc;
			*op++=(octet)u.b[1];					//msb of length first
			*op++=(octet)u.b[0];
		}	
	}
	*op++=cscode;
	if (cscode==MS_DBCS)							//DBCS has two octet code page as well
	{	u.w=cpage;									//get code page
		*op++=(octet)u.b[1];						//msb of code page first
		*op++=(octet)u.b[0];
	}
	for (i=0;i<n;i++)								//store all characters in order
		*op++=(octet)*cs++;			
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Bit String
//in:	op		points to an octet buffer to put the results in
//		bp		points to the bit string to encode (MUST be BACnet-order bytes/bits)
//		tag		tag value to use, e.g. 0x80
//		nb		is the number of significant bits in the bit string
//out:	return	advanced pointer to buffer

octet * APIENTRY eBITSTRING (octet *op,byte *bp,octet tag,word nb)
{	union { word w; byte b[2];} u;
	word	i,n;

	n=((nb+7)/8)+1;									//n is the number of bytes+1
	if (n<=4)
		*op++=tag+(octet)n;							//tag ?, length
	else
	{	*op++=tag+5;								//tag ?, extended length
		if (n<=253)
			*op++=(octet)n;
		else										//assume n<=65535
		{	*op++=0xFE;
			u.w=n;
			*op++=(octet)u.b[1];					//msb of length first
			*op++=(octet)u.b[0];
		}	
	}
	i= ((n-1)*8)-nb;
	*op++=(octet)i;									//number of unused bits
	for (i=0;i<(n-1);i++)
		*op++=*bp++;	
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Date String
//
//NOTE:	Error cases generate *don't care* values!
//		Bad dates, e.g. Feb 30 and Feb 29 for non-leap years and
//		incorrect day-of-week for a given date are not checked!
//
//in:	op		points to an octet buffer to put the results in
//		year	the actual year, e.g. 1995, or FFFF means don't care
//				(values <255 are interpreted as 1900+year)
//		month	the month, 1=Jan or FF=don't care 
//		day		the day 1-31 or FF=don't care
//		dow		the day of week, 1=Mon, 7=Sun or FF=don't care
//out:	return advanced pointer to buffer

octet * APIENTRY eDATESTRING (octet *op, word year, octet month, octet day, octet dow)
{	*op++=0xA4;										//tag 10, 4 octets
	if (year>=1900) year-=1900;						//convert large year format
	if (year>254) year=0xFF;						//bad ones are don't cares
	*op++=(octet)year;
	if (month<1||month>12) month=0xFF;
	if (day<1||day>31) day=0xFF;
	if (dow<1||dow>7) dow=0xFF;
	*op++=month;
	*op++=day;
	*op++=dow;
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Time String
//
//	NOTE:	bad values generate *don't care* !
//
//in:	op 		points to an octet buffer to put the results in
//		hour	the hour 0..23 or FF=don't care
//		min		the minute 0..59 or FF=don't care
//		sec		the second 0..59 or FF=don't care
//		hun		hundredths 0..99 or FF=don't care
//out:	return	advanced pointer to buffer

octet * APIENTRY eTIMESTRING (octet *op, octet hour, octet min, octet sec, octet hun)
{	*op++=0xB4;						//tag 11, 4 octets
	if (hour>23) hour=0xFF;
	if (min>59) min=0xFF;
	if (sec>59) sec=0xFF;
	if (hun>99) hun=0xFF;
	*op++=hour;
	*op++=min;
	*op++=sec;
	*op++=hun;
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Object Identifier Value
//in:	op		points to an octet buffer to put the results in
//		type 	has the object type to encode
//		inst 	has the instance number
//out:	return	advanced pointer to buffer

octet * APIENTRY eOBJECTID (octet *op, word type, dword inst)
{	return etagOBJECTID(op,0xC0,vbOBJECTID(type,inst)); }	//							***010

octet * APIENTRY etagOBJECTID (octet *op,octet tag,dword objid)	//	***010
{	union { dword dw; byte b[4];} u;
	int		i;										//								***002
    
	u.dw=objid;										//so we can get at individual bytes ***010
	*op++=tag+4;									//tag ?, 4 octets
	for (i=3;i>=0;i--)								//store all bytes ms first, ls last
		*op++=(octet)u.b[i];						//save byte
	return op;
}

//																					***006 Begin
/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Unsigned Integer Value (tag 2)
//in:	np		is the address of the pointer to the tag in an octet buffer
//out:	return	dword value
//		np		advanced

dword APIENTRY dUINT (octet *(*np))	//argument is passed by reference, ie. the address of a pointer
{	return dDWORD(np,FALSE);}					//minimal decode

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Enumerated Value (tag 9)
//in:	np		is the address of the pointer to an octet buffer
//out:	return	dword value
//		np		advanced

dword APIENTRY dENUM (octet *(*np))	//argument is passed by reference, ie. the address of a pointer
{	return dDWORD(np,FALSE);}						//minimal decode

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Object Identifier Value (tag 12)
//in:	np		is the address of the pointer to the tag in an octet buffer
//		type 	returns the decoded object type
//		inst 	has the instance number
//out:	return	instance number
//		np		advanced

dword APIENTRY dOBJECTID (octet *(*np),word *type)	//argument is passed by reference, ie. the address of a pointer
{	union { dword dw; word w[2]; byte b[4];} u;
	int		i;

	(*np)++;										//skip C4
	for (i=3;i>=0;i--)								//store all bytes ms last, ms last
		u.b[i]=*(*np)++;
	*type=(u.w[1]>>6)&0x3FF;						//return device type
	return (u.dw&0x3FFFFF);							//return instance
}

/////////////////////////////////////////////////////////////////////// 
//	Decode a Context Specific Tagged Value
//in:	np		is the address of the pointer to the tag in an octet buffer
//		sgnd	TRUE if return value is to be signed
//out:	return	dword value
//		np		advanced

dword APIENTRY dCONTEXT (octet *(*np), BOOL sgnd)	//argument is passed by reference, ie. the address of a pointer
{	return dDWORD(np,sgnd);}						//minimal decode

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged 4 octet minimal encoded value
//in:	np		is the address of the pointer to the tag in an octet buffer
//		sgnd	TRUE if signed, FALSE if unsigned
//out:	return	the dword value
//		np		advanced

dword dDWORD (octet *(*np),BOOL sgnd)			//argument is passed by reference, ie. the address of a pointer
{	union { dword dw; byte b[4];} u;
	int		i,n;
    
	n=(*(*np)++)&7;
	u.dw=((sgnd)&&((*(*np)&0x80)!=0))?-1:0;			//initialize for signed or unsigned 
	for (i=n-1;i>=0;i--)							//store bytes ms last, ls first
		u.b[i]=*(*np)++;
	return u.dw;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Null Value (Tag 0)
//in:	np		is the address of the pointer to the tag in an octet buffer
//out:	return	octet value of null
//		np 		advanced

octet APIENTRY dNULL(octet *(*np))
{
	(*np)++;										//advance pointer, trust us
	return 0;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Boolean Value (Tag 1)
//in:	op		is the address of the pointer to the tag in an octet buffer
//out:	return	TRUE/FALSE
//		op		advanced

BOOL APIENTRY dBOOL (octet *(*np))
{
	return ((((*(*np)++)&7)==1) ? TRUE:FALSE);
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Signed Integer Value (tag 3)
//in:	np		is the address of the pointer to the tag in an octet buffer
//out:	return	dword value
//		np		advanced

long APIENTRY dINT (octet *(*np))	//argument is passed by reference, ie. the address of a pointer
{	return dDWORD(np,TRUE);}					//minimal decode

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Real Value (Tag 4)
//in:	np		is the address of the pointer to the tag in an octet buffer
//out:	return	fp real value
//		np		advanced

float APIENTRY dREAL (octet *(*np))
{	union { float rv; byte b[4];} u;                 
	int		i;

	(*np)++;										//advance pointer, trust us
	for (i=3;i>=0;i--)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;						//									***031
	return u.rv;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Double Value (Tag 5)
//in:	np		is the address of the pointer to the tag in an octet buffer
//out:	return	fp double value
//		np		advanced

double APIENTRY dDOUBLE (octet *(*np))
{	union { double dv; byte b[8];} u;
	int		i;
    
    (*np)+=2;										//skip tag, extended length (trust that it's =8)
	for (i=7;i>=0;i--)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;			
	return u.dv;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Octet String (Tag 6)
//in:	np		is the address of the pointer to the tag in an octet buffer
//		os		points to the octet string to decode into
//		n		has the number of octets to encode (0-65535)
//out:	return	number of octets decoded (0-65535)
//		np		advanced

word APIENTRY dOCTETSTRING(octet *(*np),octet *os)
{	union { word w; byte b[2];} u;
	word	i,n;

	if ((*(*np)&7)<=4)
		n=(*(*np)++)&7;								//tag 6, length
	else
	{	*(*np)++;									//tag 6, extended length
		if (*(*np)<=253)
			n=*(*np)++;
		else										//assume n<=65535
		{	*(*np)++;
			u.b[1]=(byte)*(*np)++;					//msb of length first
			u.b[0]=(byte)*(*np)++;  
			n=u.w;
		}	
	}
	for (i=0;i<n;i++)								//store all octets in order
		*os++=*(*np)++;
	return n;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Character String (Tag 7)
//in:	np		is the address of the pointer to the tag in an octet buffer
//		cs		points to the character string to decode into
//		cscode	points to the characterset code (see BCstring enumeration) to return
//		cpage	points to the code page for string type MS_DBCS to return
//out:	return	number of characters decoded (0-65535)
//		np		advanced

word APIENTRY dCHARSTRING (octet *(*np),char *cs,octet *cscode,word *cpage)
{	union { word w; byte b[2];} u;
	word	i,nc;

	if ((*(*np)&7)<=4)
		nc=(*(*np)++)&7;							//tag 6, length
	else
	{	*(*np)++;									//tag 6, extended length
		if (*(*np)<=253)
			nc=*(*np)++;
		else										//assume n<=65535
		{	*(*np)++;
			u.b[1]=(byte)*(*np)++;					//msb of length first
			u.b[0]=(byte)*(*np)++;  
			nc=u.w;
		}	
	}
	nc--;											//account for cs code
	*cscode=*(*np)++;
	if (*cscode==MS_DBCS)							//DBCS has two octet code page as well
	{	(octet)u.b[1]=*(*np)++;						//msb of code page first
		(octet)u.b[0]=*(*np)++;
		*cpage=u.w;									//get code page
		nc-=2;										//exclude from char count
	}
	for (i=0;i<nc;i++)								//store all characters in order
		*cs++=*(*np)++;
	return nc;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Bit String (Tag 8)
//in:	np		is the address of the pointer to the tag in an octet buffer
//		bs		points to the bit string string to decode into
//out:	return	number of bytes decoded (0-65535)
//		np		advanced

word APIENTRY dBITSTRING (octet *(*np),byte *bs)
{	union { word w; byte b[2];} u;
	word	i,nb;

	if ((*(*np)&7)<=4)
		nb=(*(*np)++)&7;							//tag 8, length
	else
	{	*(*np)++;									//tag 8, extended length
		if (*(*np)<=253)
			nb=*(*np)++;
		else										//assume n<=65535
		{	*(*np)++;
			u.b[1]=(byte)*(*np)++;					//msb of length first
			u.b[0]=(byte)*(*np)++;  
			nb=u.w;
		}	
	}
	nb--;											//account unused bits octet
	(*np)++;										//skip unused bits
	for (i=0;i<nb;i++)								//store all bytes in order
		*bs++=*(*np)++;
	return nb;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Date String (Tag 10)
//
//NOTE:	Error cases generate *don't care* values!
//		Bad dates, e.g. Feb 30 and Feb 29 for non-leap years and
//		incorrect day-of-week for a given date are not checked!
//
//in:	np		is the address of the pointer to the tag in an octet buffer
//		year	points to the actual year to retrun, e.g. 1995, or FFFF means don't care
//				(values <255 are interpreted as 1900+year)
//		month	points to the month to return, 1=Jan or FF=don't care 
//		day		points to the day to return 1-31 or FF=don't care
//		dow		points to the day of week to return, 1=Mon, 7=Sun or FF=don't care
//out:	return 	np advanced

void APIENTRY dDATESTRING (octet *(*np),word *year,octet *month,octet *day,octet *dow)
{	(*np)++;										//advance pointer, trust us
	*year=(word)*(*np)++;
	if (*year<1900) *year+=1900;					//convert large year format
	*month=*(*np)++;
	*day=*(*np)++;
	*dow=*(*np)++;
	return;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Time String (Tag 11)
//
//	NOTE:	bad values generate *don't care* !
//
//in:	np		is the address of the pointer to the tag in an octet buffer
//		hour	points to the hour 0..23 or FF=don't care
//		min		points to the minute 0..59 or FF=don't care
//		sec		points to the second 0..59 or FF=don't care
//		hun		points to hundredths 0..99 or FF=don't care
//out:	return	np advanced

void APIENTRY dTIMESTRING (octet *(*np),octet *hour,octet *min,octet *sec,octet *hun)
{	(*np)++;										//advance pointer, trust us
	*hour=*(*np)++;
	*min=*(*np)++;
	*sec=*(*np)++;
	*hun=*(*np)++;
	return;
}
//																					***006 End
/////////////////////////////////////////////////////////////////////// 			***008 Begin
//	Decode a minimal encoded big endian dword value
//in:	np		is the pointer to an octet buffer
//		nl		is the buffer length 1,2,3 or 4
//out:	the dword value

dword APIENTRY vbDWORD(octet *np,word nl)
{	union { dword dw; word w[2]; byte b[4];} u;
	int		i;
	u.dw=0;											//init to zero
	for (i=nl-1;i>=0;i--)							//store all bytes ms last, ms last
		u.b[i]=*np++;
	return (u.dw);									//return it
}

/////////////////////////////////////////////////////////////////////// 
//	Decode a minimal encoded big endian signed integer value
//in:	np		is the pointer to an octet buffer
//		nl		is the buffer length 1,2,3 or 4
//out:	the signed integer value

long APIENTRY vbLONG(octet *np,word nl)
{	union { long dw; word w[2]; byte b[4];} u;
	int		i;
	u.dw=((*np&0x80)!=0)?-1:0;						//init to zero or FF's
	for (i=nl-1;i>=0;i--)							//store all bytes ms last, ms last
		u.b[i]=*np++;
	return (u.dw);									//return it
}

/////////////////////////////////////////////////////////////////////// 
//	Decode a big endian word
//in:	np		is the address of the octet buffer
//out:	return	word value

word APIENTRY vbWORD (octet *np)
{	union { word w; octet b[2];} u;                 

	u.b[1]=*np++;			
	u.b[0]=*np++;			
	return u.w;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Real Value (Tag 4)
//in:	np		is the address of the octet buffer
//out:	return	fp real value

float APIENTRY vbREAL (octet *np)
{	union { float rv; octet b[4];} u;                 
	int		i;

	for (i=3;i>=0;i--)								//store all bytes, ls first, ms last
		u.b[i]=*np++;			
	return u.rv;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Double Value (Tag 5)
//in:	np		is the address of the octet buffer
//out:	return	fp double value

double APIENTRY vbDOUBLE (octet *np)
{	union { double rv; octet b[8];} u;                 
	int		i;

	for (i=7;i>=0;i--)								//store all bytes, ls first, ms last
		u.b[i]=*np++;			
	return u.rv;
}

//																					***008 End

/////////////////////////////////////////////////////////////////////// 			***010 Begin
//	Combine an Object Type and Instance into an ObjectID
//in:	otype		is the object type
//		oinst		is the object instance
//out:	return		an object id

dword APIENTRY vbOBJECTID(word otype,dword oinst)
{	union { dword dw; word w[2];} u;
	//FILE *op2;//MAG
    
	//op2 = fopen("c:\\temp\\bacprim32outvbo.txt","a");
	//fprintf(op2,"vbOBJECTID receive values %d %d\n",otype,oinst);
	//fclose(op2);

	u.w[0]=0;
	u.w[1]=(otype<<6)&0xFFC0;						//we need only 10 bits of the type
	u.dw|=(oinst&0x3FFFFF);							//so we can get at individual bytes
	return (u.dw);
}													//								***010 End

/////////////////////////////////////////////////////////////////////// 			***014 Begin
//	Convert HEX chars to binary byte
//in:	src		points to 2 hex chars
//		dst		points to byte to receive the value
//out:	ptr to 1st non-hex char, or 2 past src
char *cvhex(char *src,byte *dst)		//									***019
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
//	Convert n*2 HEX chars to binary byte [n] array (little endian)
//in:	src		points to n*2 hex chars
//		dst		points to where n byte value goes
void cvhexn(char *src,byte *dst,int n)		//								***019
{	int	i;
	for (i=0;i<n;i++)
		src=cvhex(src,&dst[i]);
	return;										//									***014 End
}

/////////////////////////////////////////////////////////////////////// 			***023 Begin
//	Encode a Real value into a buffer
//in:	op			points to a 4 octet buffer
//		rv			is the real value

void APIENTRY vbeREAL(octet *op,float rv)
{	union { float rv; octet b[4];} u;                 
	int		i;

	u.rv=rv;
	for (i=3;i>=0;i--)								//store all bytes, ls first, ms last
		*op++=u.b[i];			
	return;
}													//								***023 End

/////////////////////////////////////////////////////////////////////// 			***023 Begin
//	Encode a Double value into a buffer
//in:	op			points to an 8 octet buffer
//		dv			is the real value

void APIENTRY vbeDOUBLE(octet *op,double dv)
{	union { double rv; octet b[8];} u;                 
	int		i;

	u.rv=dv;
	for (i=7;i>=0;i--)								//store all bytes, ls first, ms last
		*op++=u.b[i];			
	return;
}													//								***023 End
