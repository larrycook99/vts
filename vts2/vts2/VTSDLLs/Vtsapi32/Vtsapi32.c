/*--------------------------------------------------------------------------------
	©1995-97, PolarSoft® Inc. and National Institute for Standards and Technology
	
module:		VTSAPI32.C
desc:		BACnet Standard Objects DLL v2.15
authors:	David M. Fisher, Jack R. Neyer
last edit:	09-Dec-97 [015] DMF revise for 32 bit from v0.14
----------------------------------------------------------------------------------			
			27-Aug-96 [014] DMF fix 0.13 item 3.
			20-Aug-96 [013] DMF 
				1. add support for propflags in generic_object handling
				2. fix Exception Schedule parse for WeekNDay numeric form of days
				3. fix MSO, and AO parse after NULL in priority array
				4. fix whitespace tolerance in recipient lists
				5. fix parse of days of week
				6. fix daterange when (date .. date) i.e. space after date but not a day of week
				7. fix parse of timevalues
			05-Jul-96 [012] DMF
				1. fix parse of recipients, setpoint references
				2. objectpropertyrefs
				3. string items (setstring)
				4. unknown object-types
				5. parse of booleans
			30-Jun-96 [011] DMF fix parse of priority arrays
			13-Jun-96 [010] DMF really fix 0.09
			12-Jun-96 [009] DMF fix problem with marking std props as present
			28-May-96 [008] DMF support T/F in ParseBitstring
								fix parsing for ActionCommands
			25-May-96 [007] DMF add VTSAPIgetpropertystate
			23-May-96 [006] DMF add rtrim
			16-May-96 [005] DMF add VTSAPIgetdefaultpropinfo
			14-May-96 [004] DMF add VTSAPIgetpropertystates
			04-Dec-95 [003] JN  add VTSgetdefaultparsetype
			22-Sep-95 [002] DMF add array flag, change enum tables
			18-Aug-95 [001] DMF First Cut
-----------------------------------------------------------------------------*/ 

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <ctype.h>
#include <sys\stat.h>
#include <sys\types.h>
#include "db.h"
#include "stdobj.h"
#include "stdobjpr.h"
#include "vtsapi.h"
///////////////////////////////////////////////////////////////////////
//	function prototypes
BOOL ReadFunctionalGroups(PICSdb *);	//										***008 Begin
BOOL ReadStandardServices(PICSdb *);
BOOL ReadStandardObjects(PICSdb *);
BOOL ReadDataLinkOptions(PICSdb *);
BOOL ReadSpecialFunctionality(PICSdb *);
BOOL ReadObjects(PICSdb *);
BOOL ReadCharsets(PICSdb *);			//										***008 End
BOOL ParseObjectList(BACnetObjectIdentifier **,word *);
BOOL ParseRASlist(BACnetReadAccessSpecification **);
BOOL ParseCalist(BACnetCalendarEntry **);
BOOL ParseTVList(BACnetTimeValue **);
BOOL ParseWeekdaySchedule(BACnetTimeValue *wp[]);
BOOL ParseExceptionSchedule(BACnetExceptionSchedule *);
BOOL ParseProperty(char *,generic_object *,word);
BOOL ParseDateTime(BACnetDateTime *);
BOOL ParseDate(BACnetDate *);
BOOL ParseTime(BACnetTime *);
BOOL ParseBitstring(octet *,word,octet *);
BOOL ParseOctetstring(octet *,word,word *);
BOOL ParseVTClassList(BACnetVTClassList **);
BOOL ParseAddressList(BACnetAddressBinding **);
BOOL ParseDestinationList(BACnetDestination **);
BOOL ParseRecipientList(BACnetRecipient **);
BOOL ParseEventParameter(BACnetEventParameter *);
BOOL ParseSessionKeyList(BACnetSessionKey **);
BOOL ParseRefList(BACnetObjectPropertyReference	**);
BACnetRecipient *ParseRecipient(BACnetRecipient *);
BACnetObjectPropertyReference *ParseReference(BACnetObjectPropertyReference	*);
BACnetActionCommand *ReadActionCommands(void);
BOOL setstring(char *,word,char *);
char *ReadNext(void);						//										***008
void skipwhitespace(void);					//										***008
char *openarray(char *);
char *strdelim(char *);						//										***008
dword ReadDW(void);
octet ReadB(octet,octet);
word  ReadW(void);
octet ReadBool(void);
word ReadEnum(etable *);
dword ReadObjID(void);
int CreateTextPICS(char *);
void readline(char *,int);					//										***008
char *Nxt(char *);
char *cvhex(char *,octet *);
BOOL MustBe(char);
BOOL tperror(char *,BOOL);
int ClearCBList(HWND);						//										***003 Begin
int AddCBListText(HWND,char *);
int SelectCBListItem(HWND,int);				//										***003 End
void rtrim(char *);							//										***006
///////////////////////////////////////////////////////////////////////
//	Module Constants
#define	tab							0x09
#define	space						0x20
#define	cr							0x0D
#define	lf							0x0A
#define	doublequote					0x22
#define	singlequote					0x27
#define	accentgrave					0x60
#define	badobjid					0xFFFFFFFF	//we presume that no one will use this as a valid object identifier
///////////////////////////////////////////////////////////////////////
// Global variables
BOOL		cancel=false;						//global cancel flag
char		lb[256];							//line buffer (current line of input file)
char		*lp;								//pointer into lb
word		lc;									//current 1-based line in file
int			lerrc;								//count of errors
FILE		*ifile;								//current input file				***008
//---------------------------------------------------------------------
//  Large Static Tables
static char	picshdr[]="PICS 0\x0D\x0A";
static char	EndPics[]="End of BACnet Protocol Implementation Conformance Statement";
#define		BeginPics	&EndPics[7]
static char *SectionNames[]={
			"Vendor Name",										//0
			"Product Name",										//1
			"Product Model Number",								//2
			"Product Description",								//3
			"BACnet Conformance Class Supported",				//4
			"BACnet Functional Groups Supported",				//5
			"BACnet Standard Application Services Supported",	//6
			"Standard Object-Types Supported",					//7
			"Data Link Layer Option",							//8
			"Special Functionality",							//9
			"List of Objects in test device",					//10
			"Character Sets Supported"							//11				***006
			};
static namedw FunctionalGroups[]={
			"HHWS",							fgHHWS,
			"PCWS",							fgPCWS,
			"COV Event Initiation",			fgCOVEventInitiation,
			"COV Event Response",			fgCOVEventResponse,
			"Event Initiation",				fgEventInitiation,
			"Event Response",				fgEventResponse,
			"Clock",						fgClock,
			"Device Communications",		fgDeviceCommunications,
			"Files",						fgFiles,
			"Time Master",					fgTimeMaster,
			"Virtual Operator Interface",	fgVirtualOPI,
			"Reinitialize",					fgReinitialize,
			"Virtual Terminal",				fgVirtualTerminal
			};
static char *StandardServices[]={
			"AcknowledgeAlarm",
			"ConfirmedCOVNotification",       
			"ConfirmedEventNotification",     
			"GetAlarmSummary",
			"GetEnrollmentSummary",
			"SubscribeCOV",
			"AtomicReadFile",
			"AtomicWriteFile",
			"AddListElement",
			"RemoveListElement",
			"CreateObject",
			"DeleteObject",
			"ReadProperty",
			"ReadPropertyConditional",
			"ReadPropertyMultiple",
			"WriteProperty",
			"WritePropertyMultiple",
			"DeviceCommunicationControl",
			"ConfirmedPrivateTransfer",
			"ConfirmedTextMessage",
			"ReinitializeDevice",
			"VT-Open",
			"VT-Close",
			"VT-Data",
			"Authenticate",
			"RequestKey",
			"I-AM",
			"I-Have",
			"UnConfirmedCOVNotification",     
			"UnConfirmedEventNotification",   
			"UnConfirmedPrivateTransfer",
			"UnconfirmedTextMessage",
			"TimeSynchronization",
			"Who-Has",
			"Who-Is"
			};
static char *StandardObjects[]={
			"Analog Input",
			"Analog Output",       
			"Analog Value",     
			"Binary Input",     
			"Binary Output",   
			"Binary Value",
			"Calendar",
			"Command",
			"Device",
			"Event Enrollment",
			"File",
			"Group",
			"Loop",
			"Multi-state Input",
			"Multi-state Output",
			"Notification Class",
			"Program",
			"Schedule"
			};
static namedw StandardDataLinks[]={
			"ISO 8802-3, 10BASE5",					dlISO88023_10BASE5,
			"ISO 8802-3, 10BASE2",					dlISO88023_10BASE2,
			"ISO 8802-3, 10BASET",					dlISO88023_10BASET,
			"ISO 8802-3, fiber",  					dlISO88023_fiber,
			"ARCNET, coax star",  					dlARCNETcoaxstar,
			"ARCNET, coax bus",   					dlARCNETcoaxbus,
			"ARCNET, twisted pair star",			dlARCNETtpstar,
			"ARCNET, twisted pair bus",				dlARCNETtpbus,
			"ARCNET, fiber star",					dlARCNETfiberstar,
			"MS/TP master. Baud rate(s)",			dlMSTPmaster,
			"MS/TP slave. Baud rate(s)",			dlMSTPslave,
			"Point-To-Point. EIA232, Baud rate(s)",	dlPTP232,
			"Point-To-Point. Modem, Baud rate(s)",	dlPTPmodem,
			"Point-To-Point. Modem, Autobaud range",dlPTPautobaudmodem,
			"LonTalk",								dlLonTalk,
			"Other",								dlOther
			};
static char *SpecialFunctionality[]={
			"Maximum APDU size in octets",
			"Segmented Requests Supported, window size",
			"Segmented Responses Supported, window size",
			"Router"
			};
static nameoctet Charsets[]={						//								***006 Begin
			"ANSI X3.4",							csANSI,
			"IBM/Microsoft DBCS",					csDBCS,
			"JIS C 6226",							csJIS,
			"ISO 10646 (UCS-4)",					csUCS4,
			"ISO 10646 (UCS-2)",					csUCS2
			};										//								***006 End
static char *MonthNames[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
static char *DOWNames[]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};

// begin by MAG
void mprintf(char *ipl){
#ifdef MDEBUG	
	FILE *op;  // comment out for release versions

	if(ipl == NULL) return;
	op = fopen("d:\\vtsapiout.txt","a");
	if(op == NULL) return;
	fprintf(op,"%s",ipl);
	fclose(op);
#endif
	return;
}
// end by MAG

///////////////////////////////////////////////////////////////////////				***007 Begin
//	Get Enumeration string for a given choice of BACnetPropertyStates
//in:	pschoice	the choice of which BACnetPropertyStates enumeration
//		psenum		the enumeration value
//		pbuf		a string buffer to put the description in (at least 32 long)
//out:	>0			the number of standard enumerations
//		=0			the choice or enumeration are invalid

word  APIENTRY VTSAPIgetpropertystate(word pschoice,word psenum, char *pbuf)
{	etable *pet;

	if (pschoice>=(sizeof(PropertyStates)/sizeof(PropertyStates[0])))
		return 0;
	pet=PropertyStates[pschoice];
	if (psenum>=pet->nes) return 0;						//enumeration is too big
	if (pet->estrings[psenum])
		strcpy(pbuf,pet->estrings[psenum]);
	else
		*pbuf=0;										//just return empty string
	return pet->nes;
}														//							***007 End

///////////////////////////////////////////////////////////////////////				***004 Begin
//	Get Enumeration Table for a given choice of BACnetPropertyStates
//in:	pschoice	the choice of which BACnetPropertyStates enumeration
//		plist		the window handle for a combo box to be filled with enumerations
//out:	>0			the number of standard enumerations
//		=0			the choice is invalid

word  APIENTRY VTSAPIgetpropertystates(word pschoice,HWND plist)
{	etable *pet;
	word i;

	char junk[300];

	sprintf(junk,"GPS: Enter gps pschoice = %d hwnd = %d\n",pschoice,plist);
	mprintf(junk);
	if (pschoice>=(sizeof(PropertyStates)/sizeof(PropertyStates[0])))
		return 0;
	pet=PropertyStates[pschoice];
	mprintf("GPS: about to ClearCBList\n");
	ClearCBList(plist);
	for (i=0;i<pet->nes;i++)
		if (pet->estrings[i])
			AddCBListText(plist,pet->estrings[i]);
		else
			AddCBListText(plist,"");
	sprintf(junk,"GPS: return value %d\n",pet->nes);
	mprintf(junk);
	return pet->nes;
}														//							***004 End

///////////////////////////////////////////////////////////////////////
//	Get Property Table information based on standard object type
//in:	objtype		the desired object type (using the standard enumeration)
//		propindex	the property table index, or 0xFFFF meaning get num properties
//		pname		points to a buffer to contain the property name
//out:	if objtype is invalid, or propindex is invalid then 0xFFFFFFFF
//		if propindex was 0xFFFF then return the number of properties for that object type
//		else		return the property ID for that slot
//					in this case, also return the property name in the buffer pname

dword APIENTRY VTSAPIgetpropinfo(word objtype,word propindex,
											char *pname,word *ptype,
											word *pgroup,word *pflags,
											word *pet)
{	propdescriptor	*pt;
	word			np;

	if (objtype>=etObjectTypes.nes) return 0xFFFFFFFF;	//not a valid object type
	pt=StdObjects[objtype].sotProps;					//point to table of properties for this guy
	np=1;												//always at least one property
	while((pt->PropGroup&Last)==0) {np++;pt++;}			//count num props
	if (propindex==0xFFFF) return (dword)np;			//just say how many properties there are
	if (propindex>=np) return 0xFFFFFFFF;				//invalid property index
	pt=StdObjects[objtype].sotProps;					//point to table of properties for this guy
	strcpy(pname,pt[propindex].PropertyName);			//return the name
	if (ptype) *ptype=(word)pt[propindex].ParseType;
	if (pgroup) *pgroup=(word)pt[propindex].PropGroup;
	if (pflags) *pflags=pt[propindex].PropFlags;
	if (pet) *pet=pt[propindex].PropET;
	return pt[propindex].PropID;						//and property ID
}

///////////////////////////////////////////////////////////////////////
//	Get Enumeration Table information
//in:	etindex		index to the enumeration table
//		index		the table index (use 0 to find out how many there are)
//		propstart	if not null, points to word var to receive value of where proprietary enumerations begin (0=none)
//		propmax		if not null, points to dword var to receive max value for proprietary enumerations+1
//		ptext		points to a buffer to contain the enumeration text
//out:	>0			the number of standard enumerations
//		=0			the index is invalid

word APIENTRY VTSAPIgetenumtable(word etindex,word index,word *propstart,dword *propmax,char *ptext)
{	etable *pet;

	if (etindex==0||etindex>(sizeof(AllETs)/sizeof(AllETs[0])))
		return 0;
	pet=AllETs[etindex-1];
	if (index>=pet->nes) return 0;						//invalid index
	if (propstart) *propstart=pet->propes;				//return start of proprietary enumerations
	if (propmax) *propmax=pet->propmaxes;				//return max proprietary enumerations
	if (pet->estrings[index])
		strcpy(ptext,pet->estrings[index]);			//return the name
	else
		*ptext=0;
	return pet->nes;
}

///////////////////////////////////////////////////////////////////////				***005 Begin
//	Get Default Property information based on property id
//
//note:	Only Matches the FIRST propid found!
//in:	objtype		the object type (0xFFFF if unknown)
//		propid		the property id 
//		ptype		pointer to a word to be filled with parsetype, or NULL
//		pet			pointer to word to be filled with enum table index, or NULL
//out:	false		if no default info available for property id, 
//		true		if default info was available, parameters filled in accordingly

BOOL APIENTRY VTSAPIgetdefaultpropinfo(word objtype,dword propid,word *ptype,word *pet)
{	word			i;
	propdescriptor	*pt;

	if (objtype<etObjectTypes.nes)						//known object type
	{	pt=StdObjects[objtype].sotProps;				//point to table of properties for this guy
		do
		{	if (pt->PropID==propid)						//found our man
			{   if (ptype) *ptype=(word)pt->ParseType;
				if (pet) *pet=pt->PropET;
				return true;
			}
			if (pt->PropGroup&Last) break;
			pt++;										//advance to next one 
		} while(true);
	}
//unknown object type
	for (i=0;i<dParseTypes.npt;i++)
	{	if (propid==dParseTypes.dpt[i].dptpropid)
		{   if (ptype) *ptype=dParseTypes.dpt[i].dptparsetype;
			if (pet) *pet=dParseTypes.dpt[i].dptpet;
			return true;
		}
	}                                                      
	return false;
}														//							***005 End

///////////////////////////////////////////////////////////////////////				***003 Begin
//	Get Default Parse Type based on property id
//in:	propid		the property id 
//		hWnd		the handle to a list control into which parse type(s) are built
//out:	0 			if invalid property id, 
//					else number added to list
//					       parse type text /t parsetype /t eb index /t pflags eg.
//						   "bits/t03/t18/t00"

word APIENTRY VTSAPIgetdefaultparsetype(dword propid,HWND hWnd)
{	word	i,n=0,pt;
	char	pstr[128],t[32];

	ClearCBList(hWnd);
	for (i=0;i<dParseTypes.npt;i++)
	{	if (propid==dParseTypes.dpt[i].dptpropid)
		{   pt=dParseTypes.dpt[i].dptparsetype;
			if (pt==none) strcpy(pstr,"none");
			else strcpy(pstr,stParseTypes.estrings[pt]);	//"parse type"
			strcat(pstr,"\t"); 							//tab
			_itoa(pt,t,10);
			strcat(pstr,t);								//parse type
			strcat(pstr,"\t");							//tab
			_itoa(dParseTypes.dpt[i].dptpet,t,10);
			strcat(pstr,t);								//parse type
			strcat(pstr,"\t");							//tab
			_itoa(dParseTypes.dpt[i].dptflags,t,10);
			strcat(pstr,t);								//parse type
            AddCBListText(hWnd,pstr);
			n++;
			if ((i==dParseTypes.npt-1)||(propid!=dParseTypes.dpt[i+1].dptpropid)) break;
		}                                                      
	}
	if (n!=0) SelectCBListItem(hWnd,0);					//select first item
	return n;
}

///////////////////////////////////////////////////////////////////////
//	Clear all items from a List Box

int ClearCBList(HWND hWnd)
{ return (int) SendMessage(hWnd,CB_RESETCONTENT,0, 0L);
}

///////////////////////////////////////////////////////////////////////
//	Add text to a list item in a Combo Box

int AddCBListText(HWND hWnd,char *p)
{ return (int) SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM) ((LPSTR) p));
}

///////////////////////////////////////////////////////////////////////
//	Select an item in a List Box

int SelectCBListItem(HWND hWnd,int index)
{ return (int) SendMessage(hWnd,CB_SETCURSEL,(WPARAM) index, 0L);
}
//																				***003 End


///////////////////////////////////////////////////////////////////////			***008 Begin
//	Delete an object created by ReadTextPICS
//in:	p		points to the object
void  APIENTRY DeletePICSObject(generic_object *p)
{	word	i;
	void *vp,*vq,*vr;

	switch(p->object_type)						//release this object's special fields
	{
	case CALENDAR:
		vp=((calendar_obj_type *)p)->date_list;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetCalendarEntry *)vp)->next;
			free(vq);
		}
		break; 
	case COMMAND:
		for (i=0;i<32;i++)
		{	vp=((command_obj_type *)p)->action[i];
			while(vp!=NULL)
			{	vq=vp;
				vp=((BACnetActionCommand *)vp)->next;
				free(vq);
			}
			if (((command_obj_type *)p)->action_text[i]!=NULL)
				free(((command_obj_type *)p)->action_text[i]);
		}
		break; 
	case DEVICE: 
		vp=((device_obj_type *)p)->object_list;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetObjectIdentifier *)vp)->next;
			free(vq);
		}
		vp=((device_obj_type *)p)->vt_classes_supported;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetVTClassList *)vp)->next;
			free(vq);
		}
		vp=((device_obj_type *)p)->active_vt_sessions;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetVTSession *)vp)->next;
			free(vq);
		}
		vp=((device_obj_type *)p)->list_session_keys;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetSessionKey *)vp)->next;
			free(vq);
		}
		vp=((device_obj_type *)p)->time_synch_recipients;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetRecipient *)vp)->next;
			free(vq);
		}
		vp=((device_obj_type *)p)->device_add_binding;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetAddressBinding *)vp)->next;
			free(vq);
		}
		break; 
	case EVENT_ENROLLMENT: 
		vp=((ee_obj_type *)p)->parameter_list.list_bitstring_value;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetListBitstringValue *)vp)->next;
			free(vq);
		}
		vp=((ee_obj_type *)p)->parameter_list.list_of_value;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetPropertyStates *)vp)->next;
			free(vq);
		}
		break; 
	case GROUP:
		vr=((group_obj_type *)p)->list_of_group_members;
		while(vr!=NULL)
		{	vp=((BACnetReadAccessSpecification *)vr)->list_of_prop_ref;
			while(vp!=NULL)
			{	vq=vp;
				vp=((BACnetPropertyReference *)vp)->next;
				free(vq);
			}
			vq=vr;
			vr=((BACnetReadAccessSpecification *)vr)->next;
			free(vq);
		}
		break; 
	case LOOP: 
		vp=((loop_obj_type *)p)->setpoint_ref;
		if (vp!=NULL) free(vp);
		break; 
	case MULTI_STATE_INPUT: 
		for (i=0;i<32;i++)
		{	if (((mi_obj_type *)p)->state_text[i]!=NULL)
				free(((mi_obj_type *)p)->state_text[i]);
		}
		break; 
	case MULTI_STATE_OUTPUT: 
		for (i=0;i<32;i++)
		{	if (((mo_obj_type *)p)->state_text[i]!=NULL)
				free(((mo_obj_type *)p)->state_text[i]);
		}
		break; 
	case NOTIFICATIONCLASS: 
		vp=((nc_obj_type *)p)->recipient_list;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetDestination *)vp)->next;
			free(vq);
		}
		break; 
	case SCHEDULE:
		for (i=0;i<7;i++)
		{	vp=((schedule_obj_type *)p)->weekly_schedule[i];
			while(vp!=NULL)
			{	vq=vp;
				vp=((BACnetTimeValue *)vp)->next;
				free(vq);
			}
		}
		vr=((schedule_obj_type *)p)->exception_schedule.special_event;
		while(vr!=NULL)
		{	vp=((BACnetSpecialEvent *)vr)->list_of_time_values;
			while(vp!=NULL)
			{	vq=vp;
				vp=((BACnetTimeValue *)vp)->next;
				free(vq);
			}
			vq=vr;
			vr=((BACnetSpecialEvent *)vr)->next;
			free(vq);
		}
		vp=((schedule_obj_type *)p)->list_obj_prop_ref;
		while(vp!=NULL)
		{	vq=vp;
			vp=((BACnetObjectPropertyReference *)vp)->next;
			free(vq);
		}
		break; 
	}
	free(p);									//and the object itself					***008 End
}

///////////////////////////////////////////////////////////////////////
//	Open and Read a TPI file
//in:	tp		points to file name string
//		errc	pointer to an error counter
//out:	ptr to the created list of database objects

void APIENTRY ReadTextPICS(char *tp,PICSdb *pd,int *errc)
{	int			i,j;
	char opj[300]; //line added by MAG
	generic_object *pd2;  // line added by MAG for debug only
	
	pd->Database=NULL;

	if(strlen(tp) < 275){
		sprintf(opj,"RTP: open file '%s'\n",tp);
		mprintf(opj);
	} else mprintf("RTP: open file <name too long>\n");
	lerrc=0;									//no errors yet
	if ((ifile=fopen(tp,"r"))==NULL)			//open input file						***008
	{	tperror(_strerror(tp),false);
		goto rtpexit;
	}
	
	lc=0;
	readline(lb,8);								//read header line from the file		***008
	if (strnicmp(lb,picshdr,6))				//invalid signature
	{	tperror("This file does not contain a supported form of Text PICS!",false);
		goto rtpclose;
	}
	sprintf(opj,"RTP: Read line '%s'\n",lb);		// MAG
	mprintf(opj);							// MAG

	readline(lb,sizeof(lb));					//read a line from the file				***008
	lp=&lb[0];
	sprintf(opj,"RTP: rl1: Read line '%s'\n",lb);		// MAG
	mprintf(opj);							// MAG

	if (stricmp(lb,BeginPics))				//invalid signature
	{	tperror("Invalid Text PICS signature.",false);
		goto rtpclose;
	}

	while (feof(ifile)==0&&!cancel)				//										***008
	{	readline(lb,sizeof(lb));				//read a line from the file 			***008
		sprintf(opj,"RTP: rl2: Read line '%s'\n",lb);		// MAG
		mprintf(opj);							// MAG

		if (lb[0])								//not a blank line
		{	if (stricmp(lb,EndPics)==0)		//found the end
				break;							//we're done
			if ((lp=strchr(lb,':'))==NULL)		//must have a section name
			{	lp=&lb[0];
	         	if (tperror("Expected section name here...",true))
					goto rtpclose;
			}
			else
			{	*lp++=0;						//make asciz section name and lp points to args
				for (i=0;i<(sizeof(SectionNames)/sizeof(SectionNames[0]));i++)
				  if (stricmp(lb,SectionNames[i])==0)	//we found a matching section name
				  {	switch(i)
					{
					case 0:
						if (setstring(pd->VendorName,sizeof(pd->VendorName),lp)) goto rtpclose;
						break;
					case 1:
						if (setstring(pd->ProductName,sizeof(pd->ProductName),lp)) goto rtpclose;
						break;
					case 2:
						if(setstring(pd->ProductModelNumber,sizeof(pd->ProductModelNumber),lp)) goto rtpclose;
						break;
					case 3:
						if (setstring(pd->ProductDescription,sizeof(pd->ProductDescription),lp)) goto rtpclose;
						break;
					case 4:
						skipwhitespace();							//			***008
						j=atoi(lp);				//get BACnet Conformance Class Supported
						if (j<1||j>6)
						{	if(tperror("Invalid conformance class",true)) goto rtpclose;}
						else
							pd->BACnetConformanceClass=j;
						break;
					case 5:
						if (ReadFunctionalGroups(pd)) goto rtpclose;	//			***008
						break;
					case 6:
						if (ReadStandardServices(pd)) goto rtpclose;	//			***008
						break;
					case 7:
						if (ReadStandardObjects(pd)) goto rtpclose;		//			***008
						break;
					case 8:
						if (ReadDataLinkOptions(pd)) goto rtpclose;		//			***008
						break;
					case 9:
						if (ReadSpecialFunctionality(pd)) goto rtpclose;	//		***008
						break;
					case 10:
						if (ReadObjects(pd)) goto rtpclose;	//						***008
						break;					//									***006 Begin
					case 11:
						if (ReadCharsets(pd)) goto rtpclose;	//					***006 End
					}
					i=0;						//remember that we found one
					break;
				  };
				if (i)							//couldn't find this one
				{	lp[-1]=':';
					if (tperror("Unknown section name",true))
						goto rtpclose;
				}
			}
		}
	};
rtpclose:
	fclose(ifile);								//									***008

rtpexit:
	*errc=lerrc;

	pd2 = pd->Database;
	mprintf("Begin database printout.\n");
	while(pd2 != NULL){
		sprintf(opj,"object id %d  object name = '%s' \n",pd2->object_id,pd2->object_name);
		mprintf(opj);
		pd2 = pd2->next;
	}
	mprintf("End database printout.\n");

	return;
}

///////////////////////////////////////////////////////////////////////
//	Read Functional Groups section of a TPI file
//in:	ifile		file stream
//		pd			database structure to be filled in from PICS
//out:	true		if cancel selected

BOOL ReadFunctionalGroups(PICSdb *pd)
{	int			i;

	ReadNext();									//point to next token				***008
	if (lp==NULL||*lp++!='{')					//no open token
		return tperror("Expected { here",true);
		
	pd->BACnetFunctionalGroups=0;				//default is none
	while (lp!=NULL)
	{	ReadNext();								//point to next token				***008
		if (*lp=='}'||lp==NULL) break;			//return, we're done with these
 		for (i=0;i<(sizeof(FunctionalGroups)/sizeof(FunctionalGroups[0]));i++)
		  if (stricmp(lp,FunctionalGroups[i].name)==0) //found it
		  {	pd->BACnetFunctionalGroups|=FunctionalGroups[i].dwcons;
			i=0;
			break;
		  }
		if (i)
		{	if (tperror("Unknown Functional Group",true))
				return true;
		}
	}
	return false;		
}

///////////////////////////////////////////////////////////////////////				***006 Begin
//	Read Charactersets section of a TPI file
//in:	ifile		file stream
//		pd			database structure to be filled in from PICS
//out:	true		if cancel selected

BOOL ReadCharsets(PICSdb *pd)
{	int			i;

	ReadNext();									//point to next token				***008
	if (lp==NULL||*lp++!='{')					//no open token
		return tperror("Expected { here",true);
		
	pd->BACnetCharsets=0;						//default is none
	while (lp!=NULL)
	{	ReadNext();								//point to next token				***008
		rtrim(lp);
		if (*lp=='}'||lp==NULL) break;			//return, we're done with these
 		for (i=0;i<(sizeof(Charsets)/sizeof(Charsets[0]));i++)
		  if (stricmp(lp,Charsets[i].name)==0) //found it
		  {	pd->BACnetCharsets|=Charsets[i].octetcons;
			i=0;
			break;
		  }
		if (i)
		{	if (tperror("Unknown Character Set",true))
				return true;
		}
	}
	return false;		
}												//									***006 End

///////////////////////////////////////////////////////////////////////
//	Read Standard Services section of a TPI file
//in:	ifile		file stream
//		pd			database structure to be filled in from PICS
//out:	true		if cancel selected

BOOL ReadStandardServices(PICSdb *pd)
{	int			i;
	char		*p;

	ReadNext();									//point to next token				***008
	if (lp==NULL||*lp++!='{')					//no open token
		return tperror("Expected { here",true);
		
	memset(pd->BACnetStandardServices,ssNotSupported,sizeof(pd->BACnetStandardServices));	//default is not supported
	while (lp!=NULL)
	{	ReadNext();								//point to next token				***008
		if (*lp=='}'||lp==NULL) break;			//return, we're done with these
		i=-1;									//assume there is a problem
		if ((p=strchr(lp,space))!=NULL)			//find the delimiter for supported stuff
		{	*p++=0;								//make service name be asciz
	 		for (i=0;i<(sizeof(StandardServices)/sizeof(StandardServices[0]));i++)
			  if (stricmp(lp,StandardServices[i])==0) //found it
			  {	if (strstr(p,"Initiate")!=NULL)	//supports initiate
			  		pd->BACnetStandardServices[i]|=ssInitiate;
				if (strstr(p,"Execute")!=NULL)	//supports execute
			  		pd->BACnetStandardServices[i]|=ssExecute;
			  	i=(pd->BACnetStandardServices[i]==ssNotSupported)?-1:0;
				break;
			  }
		}
		if (i<0)
		//no delimiter after the service, or couldn't find initiate or execute
		{	p=strchr(lp,0);
			if (tperror("Expected 'Initiate' or 'Execute' here",true))
				return true;
		}
		else if (i>0)
		{	if (tperror("Unknown Standard Service",true))
				return true;
		}
	}
	return false;		
}

///////////////////////////////////////////////////////////////////////
//	Read Standard Objects section of a TPI file
//in:	ifile		file stream
//		pd			database structure to be filled in from PICS
//out:	true		if cancel selected

BOOL ReadStandardObjects(PICSdb *pd)
{	int			i;
	char		*pcre,*pdel;
	octet		sup;

	ReadNext();									//point to next token				***008
	if (lp==NULL||*lp++!='{')					//no open token
		return tperror("Expected { here",true);
		
	memset(pd->BACnetStandardObjects,soNotSupported,sizeof(pd->BACnetStandardObjects));	//default is not supported
	while (lp!=NULL)
	{	ReadNext();								//point to next token				***008
		if (*lp=='}'||lp==NULL) break;			//return, we're done with these
		sup=soSupported;
		if ((pcre=strstr(lp,"Createable"))!=NULL) //supports create
			sup|=soCreateable;
		if ((pdel=strstr(lp,"Deleteable"))!=NULL) //supports delete
			sup|=soDeleteable;
		if (pcre!=NULL)	pcre[-1]=0;				//cheesy way to "remove" this from the string
		if (pdel!=NULL)	pdel[-1]=0;				//cheesy way to "remove" this from the string

 		for (i=0;i<(sizeof(StandardObjects)/sizeof(StandardObjects[0]));i++)
			if (stricmp(lp,StandardObjects[i])==0) //found it
			{	pd->BACnetStandardObjects[i]=sup;
				i=0;
				break;
			}
		if (i>0)
		{	if (tperror("Unknown Standard Object",true))
				return true;
		}
	}
	if (pd->BACnetStandardObjects[DEVICE]==soNotSupported)
		return tperror("The Device Object was not present in the list of Standard Objects!",true);
	return false;		
}

///////////////////////////////////////////////////////////////////////
//	Read Data Link Layer Options section of a TPI file
//in:	ifile		file stream
//		pd			database structure to be filled in from PICS
//out:	true		if cancel selected

BOOL ReadDataLinkOptions(PICSdb *pd)
{	int			i,j;
	BOOL		got9600;
	char		*p;
	dword		*dp;

	ReadNext();									//point to next token				***008
	if (lp==NULL||*lp++!='{')					//no open token
		return tperror("Expected { here",true);
		
	pd->DataLinkLayerOptions=0;					//default is none
	pd->PTPAutoBaud[0]=pd->PTPAutoBaud[1]=0;
	for (i=0;i<16;i++)
	{	
		pd->MSTPmasterBaudRates[i]=0;
		pd->MSTPslaveBaudRates[i]=0;
		pd->PTP232BaudRates[i]=0;
		pd->PTPmodemBaudRates[i]=0;
	}
	while (lp!=NULL)
	{	ReadNext();								//point to next token				***008
		if (*lp=='}'||lp==NULL) break;			//return, we're done with these
		if ((p=strchr(lp,':'))!=NULL)			//colon in this one
			*p++=0;								//make it asciz there
		rtrim(lp);								//trim trailing blanks				***006
 		for (i=0;i<(sizeof(StandardDataLinks)/sizeof(StandardDataLinks[0]));i++)
		  if (stricmp(lp,StandardDataLinks[i].name)==0) //found it
		  {	pd->DataLinkLayerOptions|=LOWORD(StandardDataLinks[i].dwcons);
		  	switch(i)							//some of these need extra handling
		  	{
		  	case 9:								//MS/TP master
		  		dp=&pd->MSTPmasterBaudRates[0];
		  		goto rdlorates;
		  	case 10:							//MS/TP slave
				dp=&pd->MSTPslaveBaudRates[0];
		  		goto rdlorates;
		  	case 11:							//PTP 232
				dp=&pd->PTP232BaudRates[0];
		  		goto rdlorates;
		  	case 12:							//PTP modem, fixed baud rates
				dp=&pd->PTPmodemBaudRates[0];
rdlorates:		got9600=false;
				lp=p;							//point to argument(s)
				p[-1]=':';
				for (j=0;j<16;j++)				//read up to 16 baudrates
					if ((dp[j]=ReadDW())==0)
						break;					//stop as soon as we fail to find another one
					else if (dp[j]==9600)
						got9600=true;			//remember if we find 9600 baud
						
				if ((i==9||i==10)&&got9600==false)	//MS/TP must include 9600 baud
				{	if (tperror("MS/TP devices must support 9600 baud!",true))
						return true;
				}
				break;
		  	case 13:							//PTP autobaud
				lp=p;							//point to argument(s)
				p[-1]=':';
		  		if ((pd->PTPAutoBaud[0]=ReadDW())==0)
				{	if (tperror("Expected Autobaud range 'from baudrate' here!",true))
						return true;
				}
				else
				{	skipwhitespace();
					if (strnicmp(lp,"to",2)==0)
					{	lp+=2;					//skip over the 'to'
						if ((pd->PTPAutoBaud[1]=ReadDW())==0)
						{	if (tperror("Expected Autobaud range 'to baudrate' here!",true))
							return true;
						}
					}
					else
					{	if (tperror("Expected 'to' here!",true))
							return true;
					}
				};
		  	}
			i=0;
			break;
		  }
		if (i)
		{	if (tperror("Unknown Data Link Layer Option",true))
				return true;
		}
	}
	return false;		
}

///////////////////////////////////////////////////////////////////////
//	Read Special Functionality Options section of a TPI file
//in:	ifile		file stream
//		pd			database structure to be filled in from PICS
//out:	true		if cancel selected

BOOL ReadSpecialFunctionality(PICSdb *pd)
{	int			i;
	char		*p;
	octet		*wp;
	dword		d;

	ReadNext();									//point to next token				***008
	if (lp==NULL||*lp++!='{')					//no open token
		return tperror("Expected { here",true);
		
	pd->RouterFunctions=rfNotSupported;			//default is none
	pd->SegmentedRequestWindow=pd->SegmentedResponseWindow=0;
	pd->MaxAPDUSize=50;
	while (lp!=NULL)
	{	ReadNext();								//point to next token				***008
		if (*lp=='}'||lp==NULL) break;			//return, we're done with these
		if ((p=strchr(lp,':'))!=NULL)			//colon in this one
			*p++=0;								//make it asciz there
 		for (i=0;i<(sizeof(SpecialFunctionality)/sizeof(SpecialFunctionality[0]));i++)
		  if (stricmp(lp,SpecialFunctionality[i])==0) //found it
		  {	switch(i)
		  	{
		  	case 0:								//max apdu size
				lp=p;							//point to argument(s)
				p[-1]=':';
				d=ReadDW();						//parse a window argument
				if (d<50||d>1470)				//APDU size must be 50-1470
				{	if (tperror("APDU sizes must be 50..1470!",true))
						return true;
				}
				pd->MaxAPDUSize=LOWORD(d);
				break;
		  	case 1:								//request window
				wp=&pd->SegmentedRequestWindow;
		  		goto rsfwin;
		  	case 2:								//response window
				wp=&pd->SegmentedResponseWindow;
rsfwin:			lp=p;							//point to argument(s)
				p[-1]=':';
				d=ReadDW();						//parse a window argument
				if (d<1||d>127)					//window size must be 1-127
				{	if (tperror("Window sizes must be 1..127!",true))
						return true;
				}
				*wp=LOBYTE(d);
				break;
		  	case 3:								//Router
		  		pd->RouterFunctions=rfSupported;
		  	}
			i=0;
			break;
		  }
		if (i)
		{	if (tperror("Unknown Special Functionality Option",true))
				return true;
		}
	}
	return false;		
}

///////////////////////////////////////////////////////////////////////
//	Read Object Database section of a TPI file
//in:	ifile		file stream
//		pd			database structure to be filled in from PICS
//out:	true		if cancel selected

BOOL ReadObjects(PICSdb *pd)
{		char	*pn;							//property name pointer
		char	objname[32];
		BOOL	WeKnowObjectType;
		word	objtype;						//enumeration value for object type
		dword	objid;							//object identifier
		octet	fType,fID,fName;				//										***014
generic_object	*pobj,*po;					//pointers to objects

	ReadNext();									//point to next token					***008
	if (lp==NULL||*lp++!='{')					//no open token
		return tperror("Expected { here",true);

	while (lp!=NULL&&!cancel)
	{
nextobject:										//										***012
		ReadNext();								//point to next token					***008
		if (*lp=='}'||lp==NULL) break;			//return, we're done with these
		if (*lp)								//not a blank line
		{	if (*lp=='{')						//begin a new object
			{	WeKnowObjectType=false;			//don't know what kind yet
				objtype=0;						//no object found yet
				objname[0]=0;
				fType=fID=fName=0;				//										***014
				pobj=NULL;
				while (true)					//(lp!=NULL)							***006
				{	ReadNext();					//point to next token					***008
					if (*lp=='}'||lp==NULL) break;	//done with this object
					if (objtype==0xFFFF)		//										***012
						goto nextobject;		//once we find a bogus object type, we skip the rest of the object def
					if (*lp)					//ignore blank lines
					{	skipwhitespace();		//point to first char of name
						pn=lp;
						if ((lp=strchr(pn,':'))==NULL)		//find its end
						{	lp=strchr(pn,0);	//point to the end
							if (tperror("Expected : after property name here!",true))
								return true;
						}
						*lp++=0;				//make property name asciz
						if (WeKnowObjectType)
						{	if (stricmp(pn,"object-type")==0)
							{	lp[-1]=':';
								if (objtype!=ReadEnum(&etObjectTypes))
								{	if (tperror("The object-type does not agree with the object-identifier!",true))
										return true;
									objtype=0xFFFF;		//pretend objid was bad
								}
								pobj->propflags[typeProp]|=PropIsPresent; //remember we saw object type		***014 Begin
								while (*lp==space||*lp==',') lp++;	//skip any more whitespace or commas
								if (*lp=='W'||*lp=='w')				//prop is writeable
									pobj->propflags[typeProp]|=PropIsWritable; //							***014 End
								continue;
							}
							else if (stricmp(pn,"object-identifier")==0)
							{	lp[-1]=':';
								objid=ReadObjID();
								if (objtype!=(word)(objid>>22))
								{	if (tperror("The object-type does not agree with the object-identifier!",true))
										return true;
									objtype=0xFFFF;		//pretend objid was bad
								}
								pobj->propflags[idProp]|=PropIsPresent;	//remember we saw ID				***014 Begin
								while (*lp==space||*lp==',') lp++;	//skip any more whitespace or commas
								if (*lp=='W'||*lp=='w')				//prop is writeable
									pobj->propflags[idProp]|=PropIsWritable; //								***014 End
								continue;
							}
							mprintf("RO: About to PP\n");
							if (ParseProperty(pn,pobj,objtype)) return true;
							mprintf("RO: Done PP'ing\n");
						}
						else							//don't know what kind of object this is yet
						{	if (stricmp(pn,"object-type")==0)
							{	lp[-1]=':';
								if ((objtype=ReadEnum(&etObjectTypes))!=0xFFFF)
								{	WeKnowObjectType=true;
									objid=((dword)objtype)<<22;
									fType|=PropIsPresent; //remember we have the type						***014 Begin
									while (*lp==space||*lp==',') lp++;	//skip any more whitespace or commas
									if (*lp=='W'||*lp=='w')				//prop is writeable
										fType|=PropIsWritable;			//									***014 End
								}
							}
							else if (stricmp(pn,"object-identifier")==0)
							{	lp[-1]=':';
								if ((objid=ReadObjID())!=badobjid)
								{	WeKnowObjectType=true;
									objtype=(word)(objid>>22);
									fID|=PropIsPresent;	//remember we had the id							***014 Begin
									while (*lp==space||*lp==',') lp++;	//skip any more whitespace or commas
									if (*lp=='W'||*lp=='w')				//prop is writeable
										fID|=PropIsWritable;			//									***014 End
								}
								else
									objtype=0xFFFF;		//object identifier was bogus
							}
							else if (stricmp(pn,"object-name")==0)
							{	lp[-1]=':';
								if (setstring(objname,sizeof(objname),lp)) return true;
								fName|=PropIsPresent;	//remember we have the name							***014 Begin
								while (*lp==space||*lp==',') lp++;	//skip any more whitespace or commas
								if (*lp=='W'||*lp=='w')				//prop is writeable
									fType|=PropIsWritable;			//										***014 End
							}
							else
							{	lp[-1]=':';
								if (tperror("Must identify the object-identifier or object-type before defining this property!",true))
									return true;
							}
							if (WeKnowObjectType)		//just found out what type it is
							{	if (objtype>=etObjectTypes.propes)	//this is a proprietary object type
								{	tperror("Sorry, this version does not support Proprietary Objects in TextPICS!",true);
									objtype=0xFFFF;		//pretend objid was bad
									continue;
								}
								if ((pobj=malloc(StdObjects[objtype].sotSize))==NULL)		//can't allocate space for it
								{	tperror("Can't allocate space for this object...",true);
									objtype=0xFFFF;		//pretend objid was bad
									continue;
								}
								memset(pobj,0,StdObjects[objtype].sotSize);	//zero it out first
								pobj->next=NULL;
								pobj->object_id=objid;
								pobj->object_type=objtype;
								memcpy(&pobj->object_name[0],&objname[0],32);
								pobj->propflags[typeProp]|=fType;	//found type	***014 Begin
								pobj->propflags[idProp]|=fID;		//found id
								pobj->propflags[nameProp]|=fName;	//found name	***014 End
							}
						}
					}
				}
				//here we've found the end of the object definition
				if (pobj)						//									***012 Begin
				{	if (pobj->object_name[0]==0) //make sure object is named
					{	tperror("Object must have a name...",true);
						objtype=0xFFFF;			//pretend objid was bad
					}
					if (objtype==0xFFFF)		//something was wrong with it
					{	free(pobj);			//toss allocated object
						return true;			//fail
					}
					po=pd->Database;			//check for uniqueness of objid
					while (po!=NULL)
					{	if (objid==po->object_id)	//oops, we already have this one!
						{	tperror("Object Identifier is not unique!",true);
							free(pobj);		//toss allocated object
							return true;		//fail
						}
						po=po->next;			//try next one
					}
					pobj->next=pd->Database;	//link it in
					pd->Database=pobj;
				}								//									***012 End
			}
			else								//anything else is junk
			{	if (tperror("Expected '{' to begin an object definition here!",true))
					return true;
			}
		}
	}
	return false;		
}

///////////////////////////////////////////////////////////////////////
//	parse a property value
//in:	pn		points to property name string
//		pobj	points to an object structure of type objtype
//		objtype	is the object type
//
//out:	true	if cancel selected

BOOL ParseProperty(char *pn,generic_object *pobj,word objtype)
{	propdescriptor *pd;
	word			pindex,ub,i;
	void 		far	*pstruc;
	dword			dw;
	char			b[64],q;
	octet			db,dm;
	etable 			*ep;
	float		far	*fp;
	word		far	*wp;
	char		**cp;
	octet		*op;
	BACnetActionCommand **acp;
	BACnetDateRange		*dtp;
	char opj[300];	// MAG

	sprintf(opj,"PP: Enter ParseProperty, search for '%s' objtype %d\n",pn,objtype);	//MAG
	mprintf(opj);	//MAG
	pd=StdObjects[objtype].sotProps;			//point to property descriptor table for this object type
	pindex=0;
    do
	{	if (stricmp(pn,pd->PropertyName)==0)	//found this property name
		{	pstruc=(octet *)pobj+pd->StrucOffset;	//make pointer to where the value is stored
			pn=strchr(pn,0);					//restore the ':' after the property name
			*pn=':';
			skipwhitespace();					//point to where the value should be					***013 Begin
			sprintf(opj,"PP: pd->ParseType == %d\n",pd->ParseType); //MAG
			mprintf(opj); //MAG
			if (*lp=='?')						//property value is unspecified
			{	pobj->propflags[pindex]|=ValueUnknown;	//we don't know what the value is
				lp++;							//skip ?												***014
			}
			else								//has a property value									***013 End
				switch(pd->ParseType)
				{
				case evparm:					//event parameter
					if (ParseEventParameter((BACnetEventParameter *)pstruc)) return true;
					break;
				case setref:					//setpoint reference
					//note: we can't tell the difference between a ParseReference which had an error
					//		and returned NULL, vs. an intentional *no reference* which also returns
					//		NULL.
					*(BACnetObjectPropertyReference **)pstruc=ParseReference(NULL);	//				***012
					break;
				case propref:					//obj property reference
					ParseReference((BACnetObjectPropertyReference *)pstruc);
					break;
				case recip:						//recipient
					ParseRecipient((BACnetRecipient *)pstruc);						//				***012
					break;
				case skeys:						//session keys
					if (ParseSessionKeyList((BACnetSessionKey **)pstruc)) return true;
					break;
				case wsched:					//weekly schedule: array[7] of list of timevalue
					if (ParseWeekdaySchedule((BACnetTimeValue **)pstruc)) return true;
					break;
				case xsched:					//exception schedule: array[] of specialevent
					if (ParseExceptionSchedule((BACnetExceptionSchedule *)pstruc)) return true;
					break;
				case reciplist:					//list of BACnetDestination
					if (ParseDestinationList((BACnetDestination **)pstruc)) return true;
					break;
				case tsrecip:					//time synch recipients
					if (ParseRecipientList((BACnetRecipient **)pstruc)) return true;
					break;
				case dabind:					//device address bindings
					if (ParseAddressList((BACnetAddressBinding **)pstruc)) return true;
					break;
				case raslist:					//read address specifications
					if (ParseRASlist((BACnetReadAccessSpecification **)pstruc)) return true;
					break;
				case calist:					//list of calendarentry
					if (ParseCalist((BACnetCalendarEntry **)pstruc)) return true;
					break;
				case looref:					//list of object ids
					if (ParseObjectList(((BACnetObjectIdentifier **)pstruc),&((device_obj_type *)pobj)->num_objects)) return true;
					break;
				case lopref:					//list of objectpropertyreferences
					if (ParseRefList((BACnetObjectPropertyReference **)pstruc)) return true;
					break;
				case vtcl:						//vt classes
					if (ParseVTClassList((BACnetVTClassList **)pstruc)) return true;
					break;
				case pss:						//protocol services supported bitstring
					if (ParseBitstring((octet *)pstruc,(sizeof(StandardServices)/sizeof(StandardServices[0])),NULL)) return true;
					break;
				case pos:						//protocol objects supported bitstring
					if (ParseBitstring((octet *)pstruc,(sizeof(StandardObjects)/sizeof(StandardObjects[0])),NULL)) return true;
					break;
				case dt:						//date/time
					if (ParseDateTime((BACnetDateTime *)pstruc)) return true;
					break;
				case dtrange:					//daterange
					skipwhitespace();
					if (MustBe('(')) return true;
					dtp=(BACnetDateRange *)pstruc;
					if (ParseDate(&dtp->start_date)) return true;
					lp=strstr(lp,"..")+2;
					if (ParseDate(&dtp->end_date)) return true;
					if (MustBe(')')) return true;
					break;
				case ddate:
					skipwhitespace();
					if (MustBe('(')) return true;
					if (ParseDate((BACnetDate *)pstruc)) return true;
					if (MustBe(')')) return true;
					break;
				case ttime:
					skipwhitespace();
					if (ParseTime((BACnetTime *)pstruc)) return true;
					break;
				case act:						//action array
					acp=(BACnetActionCommand **)pstruc;
					for (i=0;i<32;i++) acp[i]=NULL;	//init all slots to NULL values ***011
					i=0;
					if ((lp=openarray(lp))==NULL) return true;
					
					while (i<32)				//									***008 Begin
					{	if ((acp[i]=ReadActionCommands())==NULL) return true;	//failed for some reason	
						i++;					//									***011 Begin
						while (*lp==space||*lp==',') lp++;	//skip separation between list elements
						if (*lp==0) 
							if (ReadNext()==NULL) break;
						if (*lp=='}') 
						{	lp++;
							break;				//close this array out
						}						//									***011 End
					}
					((command_obj_type *)pobj)->num_actions=i;
					break;
				case stavals:					//list of states					***006 Begin
					op=(octet *)pstruc;
					for (i=0;i<32;i++) op[i]=0;	//									***011
					i=0;
					skipwhitespace();
					if (MustBe('(')) return true;
					
					while (*lp&&i<32)
					{	if ((db=ReadB(1,32))!=dontcare) op[i]=db;
						i++;					//									***011 Begin
						if(*(lp-1) == ')') lp--;  // MAG
						while (*lp==space||*lp==',') lp++;	//skip separation between list elements
						if (*lp==0) 
							if (ReadNext()==NULL) break;
						if (*lp==')') 
						{	lp++;
							break;				//close this array out
						}						//									***011 End
					}
					break;
				case statext:					//state text array
				case actext:					//action_text array
					cp=(char **)pstruc;
					for (i=0;i<32;i++) cp[i]=NULL;	//init all slots to NULL values	***011
					i=0;
					if ((lp=openarray(lp))==NULL) return true;
					
					while (*lp&&i<32)
					{	if (setstring(b,32,lp)) return true;	//put string in buffer b
						if (b[0])				//if string isn't null
						{	ub=strlen(b)+1;   //reqd length
							if ((cp[i]=malloc(ub))==NULL)
							{	tperror("Can't Get String Space!",true);
								return true;
							}
							strcpy(cp[i],b);	//copy it
						}
						i++;					//									***011 Begin
						while (*lp==space||*lp==',') lp++;	//skip separation between list elements
						if (*lp==0) 
							if (ReadNext()==NULL) break;
						if (*lp=='}') 
						{	lp++;
							break;				//close this array out
						}						//									***011 End
					}
					break;
				case pab:						//priority array bpv
					wp=(word *)pstruc;
					for (i=0;i<16;i++) wp[i]=bpaNULL; //init all slots to NULL values	***011
					i=0;
					if ((lp=openarray(lp))==NULL) return true;
					
					while (*lp&&i<16)
					{	ep=AllETs[pd->PropET-1]; //point to enumeration table for this guy
						if ((ub=ReadEnum(ep))==0xFFFF)
						{	tperror("Invalid Enumeration",true);
							return true;
						}
						*wp++=ub;
						i++;
						lp--;					//need to point to delimiter		***011 Begin
						while (*lp==space||*lp==',') lp++; //skip separation between list elements
						if (*lp==0) 
							if (ReadNext()==NULL) break;
						if (*lp=='}') 
						{	lp++;
							break;				//close this array out
						}						//									***011 End
					}
					break;
				case pau:						//priority array uw
					wp=(word *)pstruc;
					for (i=0;i<16;i++) wp[i]=upaNULL; //init all slots to NULL values	***011
					i=0;
					if ((lp=openarray(lp))==NULL) return true;
					
					while (*lp&&i<16)
					{	if (*lp=='n'||*lp=='N')	//we'll assume he means 'NULL'
						{	*wp++=upaNULL;		//									***013 Begin
							while (*lp&&*lp!=space&&*lp!=','&&*lp!='}') lp++; //skip rest of NULL ***014
						}						//									***013 End
						else					//we'll assume there's a number here
							*wp++=ReadW();
						i++;					//									***011 Begin
						while (*lp==space||*lp==',') lp++; //skip separation between list elements
						if (*lp==0) 
							if (ReadNext()==NULL) break;
						if (*lp=='}') 
						{	lp++;
							break;				//close this array out
						}						//									***011 End
					}
					break;
				case paf:						//priority array flt
					fp=(float *)pstruc;
					for (i=0;i<16;i++) fp[i]=fpaNULL; //init all slots to NULL values	***011
					i=0;						//									***011
					if ((lp=openarray(lp))==NULL) return true;
					
					while (*lp&&i<16)
					{	if (*lp=='n'||*lp=='N')	//we'll assume he means 'NULL'
						{	*fp++=fpaNULL;		//									***013 Begin
							while (*lp&&*lp!=space&&*lp!=','&&*lp!='}') lp++; //skip rest of NULL ***014
						}						//									***013 End
						else					//we'll assume there's a number here
							*fp++=(float)atof(lp);
						i++;					//									***011 Begin
						if ((strdelim(",}"))==NULL) break;
						if (lp[-1]=='}') break;	//									***011 End
					}
					break;
				case ob_id:						//an object identifier
					if ((dw=ReadObjID())==badobjid) return true;
					*(dword *)pstruc=dw;
					break;
				case ebool:						//boolean value 					***006 Begin
					*(octet *)pstruc=ReadBool();	//							***012
					break;						//									***006 End
				case et:						//an enumeration table
					ep=AllETs[pd->PropET-1];	//point to enumeration table for this guy
					if ((ub=ReadEnum(ep))==0xFFFF)
					{	tperror("Invalid Enumeration",true);
						return true;
					}
					*(word *)pstruc=ub;
					break;
				case bits:						//octet of T or F flags
					db=0;
					dm=0x80;
					while (q=*lp++)
					{	if (q==')') break;
						if (q==',') dm>>=1;
						if (q=='t'||q=='T') db|=dm;
					}
					*(octet *)pstruc=db;
					break;
				case sw:						//signed word
					*(int *)pstruc=atoi(lp);
					break;
				case ssint:						// case added for 2.27 MAG 13 FEB 2001
				case flt:						//float
					*(float *)pstruc=(float)atof(lp);
					break;
				case uw:						//unsigned word
				//Note: this is a hack to handle the case of NotificationClass Priority array (3 unsigned words)
					if (pd->PropFlags&IsArray)	//this is an array of words			***012 Begin
					{	wp=(word *)pstruc;
						i=0;
						if ((lp=openarray(lp))==NULL) return true;
						
						while (*lp&&i<3)		//no more than 3
						{	*wp++=ReadW();
							i++;
							if(*(lp-1) == '}') lp--;  // MAG  error fix for priority object
							while (*lp==space||*lp==',') lp++;	//skip separation between list elements
							if (*lp==0) 
								if (ReadNext()==NULL) break;
							if (*lp=='}') 
							{	lp++;
								break;			//close this array out
							}
						}
					}
					else						//just a word						***012 End
						*(word *)pstruc=ReadW();
					break;
				case ud:						//unsigned dword
					*(dword *)pstruc=ReadDW();
					break;
				case u16:						//1..16
					ub=16;
					goto ppub;
				case u127:						//1..127
					ub=127;
	ppub:			dw=ReadDW();
					if (dw==0||dw>ub)			//it's out of range
					{	sprintf(b,"Must use an Unsigned Integer 1..%u here!",ub);
						if (tperror(b,true)) return true;
						dw=(dword) ub;			//stick at upper bound
					}
					*(word *)pstruc=(word)dw;
					break;
				case s10:						//char [10]
					ub=10;
					goto ppstub;
				case s32:						//char [32]
					ub=32;
					goto ppstub;
				case s64:						//char [64]
					ub=64;
					goto ppstub;
				case s132:						//char [132]
					ub=132;
	ppstub:         if (setstring((char *)pstruc,ub,lp)) return true;
					break;
				default:
				//Note:	If we get here it is always an error because the default case means that
				//		this is not a supported parsetype and therefore must always be ?,i.e. an
				//		unknown value. As of ***013, ? values are handled before we get here, so
				//		if we got here then something other than ? was specified as the value.
					if (MustBe('?')) return true;
				}
			pobj->propflags[pindex]|=PropIsPresent;	//set the bit for this property	***013 Begin
			while (*lp==space||*lp==',') lp++;		//skip any more whitespace or commas ***014
			if (*lp=='W'||*lp=='w')				//prop is writeable
				pobj->propflags[pindex]|=PropIsWritable;	//						***013 End
			mprintf("PP: RETURN\n");
			return false;						//we're done parsing
		}
		if (pd->PropGroup&Last)
			return tperror("Invalid Property Name- Check Spelling",true); 
		pd++;									//advance to next table entry
		pindex++;
	}
	while(true);
}

///////////////////////////////////////////////////////////////////////
//	read a BACnetEventParameter from the buffer lp points to
//
//note: (bitstring) may be B'101010' or (T,F...)
//
//	(timedelay,(bitmask),(bitstring),(bitstring)... or
//	(timedelay,state,state,state...) or
//	(timedelay,(bitstring)) or (timedelay,refpropincrement) or
//	(timedelay,((objtype,instance),propname)) or
//	(timedelay,((objtype,instance),propname),lowdiff,hidiff,deadband) or
//	(timedelay,low,hi,deadband)
//in:	lp		points to current position in buffer lb
//		evp		points to the BACnetEventParameter to be filled in
//		Note:	evp->event_type MUST be initialized before calling this function!
//out:	true	if an error occurred
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseEventParameter(BACnetEventParameter *evp)
{	BACnetListBitstringValue 	*pbv=NULL, *qbv=NULL;
	BACnetPropertyStates 		*pv=NULL, *qv=NULL;

	skipwhitespace();
	if (MustBe('(')) return true;
	skipwhitespace();
	evp->time_delay=ReadW();
	switch(evp->event_type)
	{
	case CHANGE_OF_BITSTRING:					//0
		skipwhitespace();
		if (ParseBitstring(&evp->bitmask.bitstring_value[0],
							sizeof(evp->bitmask.bitstring_value)*8,
							&evp->bitmask.bitstring_length)) return true;
		if (MustBe(',')) return true;
		evp->list_bitstring_value=NULL;			//no bitstrings initially
		while(feof(ifile)==0)
		{   while (*lp==space||*lp==',') lp++;	//skip separation between list elements
			if (*lp==0)
				if (ReadNext()==NULL) break;	//									***008
			if (*lp==')') break;				//close this list out
			//here we have (bitstring)...
			if ((qbv=malloc(sizeof(BACnetListBitstringValue)))==NULL)
			{	tperror("Can't Get Object Space!",true);
				break;
			}
			if (ParseBitstring(&qbv->bitstring_value[0],
							sizeof(qbv->bitstring_value)*8,
							&qbv->bitstring_length)) break;
			qbv->next=NULL;							//link onto the list
			if (evp->list_bitstring_value==NULL)
				evp->list_bitstring_value=qbv;		//remember first guy we made
			else
				pbv->next=qbv;					//link new guy on the end of the list
			pbv=qbv;							//remember new guy is now the last guy
			qbv=NULL;
		}
		if (qbv!=NULL) free(qbv);
		break;
	case CHANGE_OF_STATE:						//1
		evp->list_of_value=NULL;				//no values initially
		while(feof(ifile)==0)
		{   while (*lp==space||*lp==',') lp++;	//skip separation between list elements
			if (*lp==0)
				if (ReadNext()==NULL) break;	//									***008
			if (*lp==')') break;				//close this list out
			//here we have state,state...
			if ((qv=malloc(sizeof(BACnetPropertyStates)))==NULL)
			{	tperror("Can't Get Object Space!",true);
				break;
			}
			qv->enum_value=ReadW();
			qv->next=NULL;						//link onto the list
			if (evp->list_of_value==NULL)
				evp->list_of_value=qv;			//remember first guy we made
			else
				pv->next=qv;					//link new guy on the end of the list
			pv=qv;								//remember new guy is now the last guy
			qv=NULL;
			if (lp[-1]==')') lp--;
		}
		if (qv!=NULL) free(qv);
		break;
	case CHANGE_OF_VALUE:						//2
		skipwhitespace();
		if (*lp=='('||*lp=='B'||*lp=='b')		//cov-criteria is a bitmask
		{	evp->use_property_increment=false;
			if (ParseBitstring(&evp->bitmask.bitstring_value[0],
								sizeof(evp->bitmask.bitstring_value)*8,
								&evp->bitmask.bitstring_length)) return true;
		}
		else									//cov-criteria is a property-increment
		{	evp->use_property_increment=true;
			evp->ref_property_increment=(float)atof(lp);
			if ((strdelim(")"))==NULL) return true;
			lp--;
		}
		break;
	case COMMAND_FAILURE:						//3
		ParseReference(&evp->feed_prop_ref);
		break;
	case FLOATING_LIMIT:						//4
		ParseReference(&evp->setpoint_ref);
		if (MustBe(',')) return true;
		evp->low_diff_limit=(float)atof(lp);
		if ((strdelim(","))==NULL) return true;
		evp->high_diff_limit=(float)atof(lp);
		if ((strdelim(","))==NULL) return true;
		evp->deadband=(float)atof(lp);
		if ((strdelim(")"))==NULL) return true;
		lp--;
		break;
	case OUT_OF_RANGE:							//5
		evp->low_limit=(float)atof(lp);
		if ((strdelim(","))==NULL) return true;
		evp->high_limit=(float)atof(lp);
		if ((strdelim(","))==NULL) return true;
		evp->deadband=(float)atof(lp);
		if ((strdelim(")"))==NULL) return true;
		lp--;
		break;
	}
	return MustBe(')');
}

///////////////////////////////////////////////////////////////////////				***008 Begin
//	read a list BACnetReadAccessSpecifications from the buffer lp points to
//	(((objtype,instance),propid,propid[arindex]),((objtype,instance),propid,propid),((objtype,instance),propid,propid)...)
//in:	lp		points to current position in buffer lb
//		rasp	points to a variable which should point to a list of BACnetReadAccessSpecifications
//out:	true	if an error occurred
//		else	*rasp points to a list of BACnetReadAccessSpecifications
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseRASlist(BACnetReadAccessSpecification **rasp)
{	BACnetReadAccessSpecification	*fp=NULL,*p=NULL,*q=NULL;
	BACnetPropertyReference			*pp=NULL, *pq=NULL;
	propdescriptor		*pd;
	word				i;
	dword				dw;
	char				pn[40];
	BOOL				rasfail=true;				
				
	*rasp=NULL;									//initially there is no list
	skipwhitespace();
	if (MustBe('{')) return true;  //MAG change from '('
	while(true)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (	i.e. the beginning of a new BACnetReadAccessSpecification in the list
		//3. )				i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0) 
			if (ReadNext()==NULL) break;
		if (*lp=='}') // MAG was )
		{	lp++;
			break;								//close this list out
		}
		if (MustBe('(')) goto rasx;

		//here we have (objtype,instance),propid,propid...)...
		if ((q=malloc(sizeof(BACnetReadAccessSpecification)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			goto rasx;
		}
		dw=ReadObjID();
		if (dw==badobjid)
		{	if (tperror("Must use an Object Identifier here!",true))
				goto rasx;
		}
		q->object_id=dw;
		if ((strdelim(","))==NULL) goto rasrem;
		q->list_of_prop_ref=NULL;
		pp=NULL;
		//----------------------------------------------------
		while(feof(ifile)==0)
		{
			while (*lp==space||*lp==',') lp++;		//skip separation between list elements
			if (*lp==0) 
				if (ReadNext()==NULL) break;
			if (*lp==')') 
			{	//lp++;  //MAG
				lp +=2;  // MAG
				break;								//close this list out
			}
			if ((pq=malloc(sizeof(BACnetPropertyReference)))==NULL)
			{	tperror("Can't Get Object Space!",true);
				goto rasx;
			}
			i=0;
			if(*lp == '(') lp++; // MAG added to skip leading '('
			while(*lp&&*lp!=','&&*lp!=space&&*lp!=')'&&*lp!='['&&i<39)
			{	if (*lp=='_') *lp='-';				//change _ to -
				pn[i++]=*lp++;
			}
			pn[i]=0;								//force it to be asciz
			pd=StdObjects[(word)(dw>>22)].sotProps;	//point to property descriptor table for this object type
			do
			{	if (stricmp(pn,pd->PropertyName)==0)	//found this property name
		        {	pq->property_id=pd->PropID;
		        	break;
				}
				if (pd->PropGroup&Last)
				{	tperror("Invalid Property Name",true);
					goto rasx;
				}
				pd++;								//advance to next table entry
			}
			while(true);
	
			pq->pa_index=NotAnArray;
			if (pd->PropFlags&IsArray)
			{	if (MustBe('[')) goto rasx; 
				pq->pa_index=ReadW();
			}
	
			pq->next=NULL;							//link onto the list
			if (q->list_of_prop_ref==NULL)
				q->list_of_prop_ref=pq;				//remember first guy we made
			else
				pp->next=pq;						//link new guy on the end of the list
			pp=pq;									//remember new guy is now the last guy
			pq=NULL;
        }

		if (lp[-1]!=')')
rasrem:	{	tperror("Expected ) here!",true);
			goto rasx;
		}
		q->next=NULL;							//link onto the list
		if (fp==NULL)
			fp=q;								//remember first guy we made
		else
			p->next=q;							//link new guy on the end of the list
		p=q;									//remember new guy is now the last guy
		q=NULL;
	}
	rasfail=false;
rasx:
	*rasp=fp;
	if (q!=NULL) free(q);						//don't lose this block!
	if (pq!=NULL) free(pq);					//don't lose this block!
	return rasfail;								//									***008 End
}

///////////////////////////////////////////////////////////////////////				***008 Begin
//	read an array of (BACnetSpecialEvent) from the buffer lp points to
//	{((t,v),(t,v)),((t,v),(t,v)),...}
//in:	lp		points to current position in buffer lb
//		xp		pointer to exception schedule to be initialized
//out:	true	if an error occurred
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseExceptionSchedule(BACnetExceptionSchedule *xp)
{	BACnetSpecialEvent	*fp=NULL,*p=NULL,*q=NULL;
	BOOL				xfail=true;
	char				*ep;
	word				i;

	char junk[300]; // MAG

	xp->special_event=NULL;						//initially there are no lists
	xp->size=0;
	
	skipwhitespace();
	if (*lp=='?') return false;					//? means no list
	if (MustBe('{')) return true;
	while(feof(ifile)==0)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (			i.e. the beginning of a new BACnetSpecialEvent in the array
		//3. )			i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0)
			if (ReadNext()==NULL) return true;
		if (*lp=='}') 
		{	lp++;
			break;								//close this list out
		}
        
		if (MustBe('(')) goto xx;
		//here we have (dateoption),(timevaluelist),eventpriority),...)
		if ((q=malloc(sizeof(BACnetSpecialEvent)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			goto xx;
		}
		skipwhitespace();
		if (MustBe('(')) goto xx;
		if ((ep=strchr(lp,')'))==NULL)
		{	lp=strchr(lp,0);
			tperror("Expected ) here!",true);
			goto xx;
		}
		//here, lp points to one of 4 formats:
		//	Calendar,instance
		//	month,weekofmonth,dayofweek
		//	date..date
		//	date
		//
		//where date is in one of two formats:
		//	month/day/year dow			i.e. month is numeric
		//	day-mon-year dow			i.e. month is text like Jan
		//in either case dow must be separated from year by a space, or not be there at all
		//The .. between dates in a daterange is significant and must be literally 2 dots in a row
		
		if (strnicmp(lp,"Calendar",8)==0)		//it's a calendar reference
		{	q->choice=3;
			lp--;								//ReadObjID needs to point to the (
			q->u.calendar_ref.object_id=ReadObjID();
			q->u.calendar_ref.next=NULL;
		}
		else
		{	*ep=0;								//remember this end pointer
			if (strchr(lp,',')!=NULL)			//must be WeekNDay
			{	q->choice=2;					//WeekNDay
				skipwhitespace();
				q->u.weekNday.month=dontcare;
				q->u.weekNday.day=dontcare;
				q->u.weekNday.week=dontcare;
				if (isdigit(*lp))				//use numeric form of month
				    q->u.weekNday.month=ReadB(1,12);
				else							//use monthname
			    {	for (i=0;i<12;i++)
			    		if (strnicmp(lp,MonthNames[i],3)==0)
			    		{	q->u.weekNday.month=(octet)i+1;	//months are 1-12
			    			break;
			    		}
			    	strdelim(",");				//skip past comma
				}
				if (*lp=='l'||*lp=='L')			//Last week
				{	q->u.weekNday.week=6;
					strdelim(",");
				}
				else
					q->u.weekNday.week=ReadB(1,6);
				if (isdigit(*lp))				//use numeric form of dayofweek		***013 Begin
					q->u.weekNday.day=ReadB(1,7);
				else
			    {	for(i=0;i<7;i++)
			    		if (strnicmp(lp,DOWNames[i],3)==0)
			    		{	q->u.weekNday.day=(octet)i+1;		//days are 1-7
			    			break;
			    		}
			    	lp+=3;
			    }								//									***013 End
			}
			else if (strstr(lp,"..")!=NULL)	//must be daterange
			{	q->choice=1;					//DateRange
				if (ParseDate(&q->u.date_range.start_date)) goto xx;
				lp=strstr(lp,"..")+2;			//skip delimiter
				if (ParseDate(&q->u.date_range.end_date)) goto xx;
			}
			else								//must be date
			{	q->choice=0;					//Date
				if (ParseDate(&q->u.date)) goto xx;
			}
			lp=ep;
			*lp++=')';
		}
		sprintf(junk,"PES: About to strdelim lp = '%s'\n",lp);
		mprintf(junk);
		if (strdelim(",")==NULL) goto xx;

		sprintf(junk,"PES: About to PTVL lp = '%s'\n",lp);
		mprintf(junk);
		if (ParseTVList(&q->list_of_time_values)) goto xx;
		
		sprintf(junk,"PES: About to strdelim2 lp = '%s'\n",lp);
		mprintf(junk);
		if (strdelim(",")==NULL) goto xx;

		sprintf(junk,"PES: about to ReadW lp = '%s'\n",lp);
		mprintf(junk);
		q->event_priority=ReadW();

        xp->size++;
		sprintf(junk,"PES: ReadW returns %d\n",q->event_priority);
		mprintf(junk);

		q->next=NULL;							//link onto the list
		if (fp==NULL)
			fp=q;								//remember first guy we made
		else
			p->next=q;							//link new guy on the end of the list
		p=q;									//remember new guy is now the last guy
		q=NULL;
		if(( *(lp-1) == '(')&&(*lp == '(')) lp--;  // MAG
		sprintf(junk,"PES: end while loop, lp = '%s'\n",lp);
		mprintf(junk);

	}
	xfail=false;
xx:
	xp->special_event=fp;
	if (q!=NULL) free(q);						//don't lose this block!
	return xfail;								//									***008 End
}												//									***008 End

///////////////////////////////////////////////////////////////////////				***008 Begin
//	read an array of (list of BACnetTimeValue) from the buffer lp points to
//	{((t,v),(t,v)),((t,v),(t,v)),...}
//in:	lp		points to current position in buffer lb
//		wp		an array of pointer variables to be
//				initialized to point to the created lists of BACnetTimeValues
//out:	true	if an error occurred
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseWeekdaySchedule(BACnetTimeValue *wp[])
{	word	i;
				
	for (i=0;i<7;i++) wp[i]=NULL;				//initially there are no lists
	
	skipwhitespace();
	if (*lp=='?') return false;					//? means no list
	if (MustBe('{')) return true;
	i=0;
	while(feof(ifile)==0)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (			i.e. the beginning of a new BACnetTimeValue list in the array
		//3. )			i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0)
			if (ReadNext()==NULL) return true;
		if (*lp=='}') 
		{	lp++;
			break;								//close this list out
		}
        
        if (i>6)
        {	tperror("Weekly Schedule cannot contain more than 7 days!",true);
        	return true;
        }
        
		//here we have (list of timevalues),...
        if (ParseTVList(&wp[i])) return true;
        i++;
	}
	return false;
}												//									***008 End

///////////////////////////////////////////////////////////////////////				***008 Begin
//	read a list BACnetTimeValues from the buffer lp points to
//	((time,value),(time,value)...)
//in:	lp		points to current position in buffer lb
//		tvp		points to a variable which should point to a list of BACnetTimeValues
//out:	true	if an error occurred
//		else	*tvp points to a list of BACnetTimeValues
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseTVList(BACnetTimeValue **tvp)
{	BACnetTimeValue	*fp=NULL,*p=NULL,*q=NULL;
	BOOL				tvfail=true;
	char			*ep;				
				
	*tvp=NULL;									//initially there is no list
	skipwhitespace();
	if (MustBe('(')) return true;
	while(true)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (	i.e. the beginning of a new BACnetTimeValue in the list
		//3. )				i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0) 
			if (ReadNext()==NULL) break;
		if (*lp==')') 
		{	lp++;
			break;								//close this list out
		}

		if((*(lp-1) == '(')&&(*lp != '(')) lp--;  // MAG
		if (MustBe('(')) goto tvx;

		//here we have time,value),...)
		if ((q=malloc(sizeof(BACnetTimeValue)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			goto tvx;
		}
		if ((ep=strchr(lp,')'))==NULL)
		{	lp=strchr(lp,0);
			tperror("Expected ) at end of (time,value...",true);
			goto tvx;
		}
		*ep=0;									//make it "time,value" asciz
		if (ParseTime(&q->time)) goto tvx;
		if (strdelim(",")==NULL) goto tvx;
		skipwhitespace();
		if (*lp=='a'||*lp=='A')					//BPV Active
		{	q->value_type=BPV;
			q->av.bproperty_value=ACTIVE;
		}
		else if (*lp=='i'||*lp=='I')			//									***013
		{	q->value_type=BPV;
			q->av.bproperty_value=INACTIVE;
		}
		else if (strchr(lp,'.')==NULL)		//doesn't have a dot, must be unsigned
		{	q->value_type=UNS;
			q->av.uproperty_value=ReadW();
		}
		else									//must be float
		{	q->value_type=FLT;
			q->av.fproperty_value=(float)atof(lp);
		}

		q->next=NULL;							//link onto the list
		if (fp==NULL)
			fp=q;								//remember first guy we made
		else
			p->next=q;							//link new guy on the end of the list
		p=q;									//remember new guy is now the last guy
		q=NULL;
		*ep++=')';
		lp=ep;

		// MAG 25 JAN 01 added as bugfix for schedule:exception-schedule property
		if((lp[0] == ',') && (lp[1] != '(')){
			//sprintf(junk,"PTVL: Exit PTVL at repeat check lp = '%s'\n",lp);
			//mprintf(junk);
			if(*lp != ',') lp++;  // MAG 14 FEB 2001 fix parse error-add 'if()' part to line
			break;								//close this list out
		}
	}
	tvfail=false;
tvx:
	*tvp=fp;
	if (q!=NULL) free(q);						//don't lose this block!
	return tvfail;								//									***008 End
}

///////////////////////////////////////////////////////////////////////				***008 Begin
//	read a list BACnetObjectPropertyReferences from the buffer lp points to
//	(((objtype,instance),propertyname[arrayindex]),((objtype,instance),propertyname[arrayindex])...)
//in:	lp		points to current position in buffer lb
//		refp	points to a variable which should point to a list of BACnetObjectPropertyReferences
//out:	true	if an error occurred
//		else	*refp points to a list of BACnetObjectPropertyReferences
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseRefList(BACnetObjectPropertyReference **refp)
{	BACnetObjectPropertyReference	*fp=NULL,*p=NULL,*q=NULL;
	BOOL				reffail=true;				
				
	*refp=NULL;									//initially there is no list
	skipwhitespace();
	if (MustBe('(')) return true;
	while(true)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (	i.e. the beginning of a new BACnetObjectPropertyReference in the list
		//3. )				i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0) 
			if (ReadNext()==NULL) break;
		if (*lp==')') 
		{	lp++;
			break;								//close this list out
		}

		if ((q=ParseReference(NULL))==NULL)	goto refx;
		if((*lp == 0) && (*(lp-1) == ')')) lp--;  // MAG fix for parser error in schedule object 14 FEB 2001

		q->next=NULL;							//link onto the list
		if (fp==NULL)
			fp=q;								//remember first guy we made
		else
			p->next=q;							//link new guy on the end of the list
		p=q;									//remember new guy is now the last guy
		q=NULL;
	}
	reffail=false;
refx:
	*refp=fp;
	if (q!=NULL) free(q);						//don't lose this block!
	return reffail;								//								***008 End
}

///////////////////////////////////////////////////////////////////////
//	read a BACnetObjectPropertyReference from the buffer lp points to
//	((objtype,instance),propertyname[arrayindex])  or ?
//in:	lp		points to current position in buffer lb
//		inq		points to a BACnetObjectPropertyReference to be filled in (or NULL)
//out:	NULL	if an error occurred
//		else	pointer to newly created BACnetObjectPropertyReference
//		lp		points past the delimiter ) unless it was the end of the buffer

BACnetObjectPropertyReference *ParseReference(BACnetObjectPropertyReference	*inq)
{	BACnetObjectPropertyReference	*q=NULL;
	propdescriptor		*pd;
	dword	dw;
	word	i;
	char	pn[40];
	
	//here we have ((objtype,instance),propertyname[arrayindex])...
	skipwhitespace();
	if (*lp=='?') return NULL;
	if (MustBe('(')) return NULL;

	if (inq==NULL)
	{	if ((q=malloc(sizeof(BACnetObjectPropertyReference)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			return NULL;
		}
	}
	else
		q=inq;

	skipwhitespace();
	if((*lp != '(') && (*(lp-1) == '(')) lp--;  // MAG
	dw=ReadObjID();
	if (dw==badobjid)
	{	tperror("Must use an Object Identifier here!",true);	//					***006
		goto oprfail;
	}
	q->object_id=dw;
	if ((strdelim(","))==NULL) goto oprfail;
	skipwhitespace();						//										***006
	i=0;
	while(*lp&&*lp!=','&&*lp!=')'&&*lp!='['&&i<39)
		pn[i++]=*lp++;
	pn[i]=0;								//force it to be asciz
	rtrim(&pn[0]);							//										***006 Begin
	while(i)								//force _ and spaces to dashes
	{	if (pn[i]==space||pn[i]=='_') pn[i]='-';
		i--;
	}										//										***006 End
	pd=StdObjects[(word)(dw>>22)].sotProps;	//point to property descriptor table for this object type
	do
	{	if (stricmp(pn,pd->PropertyName)==0)	//found this property name
        {	q->property_id=pd->PropID;
        	break;
		}
		if (pd->PropGroup&Last)
		{	tperror("Invalid Property Name",true);
			break;
		}
		pd++;								//advance to next table entry
	}
	while(true);

	q->pa_index=NotAnArray;
	if (pd->PropFlags&IsArray)
	{	if (MustBe('[')) goto oprfail;		//require [arrayindex]					***012 Begin
		q->pa_index=ReadW();
	}
	if (MustBe(')')) return NULL;
	q->next=NULL;							//										***012 End
	return q;
	
oprfail:
	if (inq==NULL) free(q);
	return NULL;
}

///////////////////////////////////////////////////////////////////////				***008 Begin
//	read a list BACnetCalendarEntrys from the buffer lp points to
//	((m/d/y dow),(d-m-y dow),(m,wom,dow),(date..date)...)
//in:	lp		points to current position in buffer lb
//		calp	points to a variable which should point to a list of BACnetCalendarEntrys
//out:	true	if an error occurred
//		else	*calp points to a list of BACnetCalendarEntrys
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseCalist(BACnetCalendarEntry **calp)
{	BACnetCalendarEntry	*fp=NULL,*p=NULL,*q=NULL;
	word				i;
	char			*ep;
	BOOL				calfail=true;				
				
	*calp=NULL;									//initially there is no list
	skipwhitespace();
	if (MustBe('{')) return true;  // MAG was (
	while(true)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (	i.e. the beginning of a new BACnetCalendarEntry in the list
		//3. )				i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0) 
			if (ReadNext()==NULL) break;
		if (*lp=='}')  // MAG was )
		{	lp++;
			break;								//close this list out
		}
		if (MustBe('(')) goto calx;

		//here we have calentry)...     
		if ((ep=strchr(lp,')'))==NULL)
		{	lp=strchr(lp,0);
			tperror("Expected ) here!",true);
			goto calx;
		}
		*ep++=0;								//remember this end pointer
		//here, lp points to one of 3 formats:
		//	month,weekofmonth,dayofweek
		//	date..date
		//	date
		//
		//where date is in one of two formats:
		//	month/day/year dow			i.e. month is numeric
		//	day-mon-year dow			i.e. month is text like Jan
		//in either case dow must be separated from year by a space, or not be there at all
		//The .. between dates in a daterange is significant and must be literally 2 dots in a row
		
		if ((q=malloc(sizeof(BACnetCalendarEntry)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			goto calx;
		}
		if (strchr(lp,',')!=NULL)				//must be WeekNDay
		{	q->choice=2;						//WeekNDay
			skipwhitespace();
			q->u.weekNday.month=dontcare;
			q->u.weekNday.day=dontcare;
			q->u.weekNday.week=dontcare;
			if (isdigit(*lp))					//use numeric form of month
			    q->u.weekNday.month=ReadB(1,12);
			else								//use monthname
		    {	for (i=0;i<12;i++)
		    		if (strnicmp(lp,MonthNames[i],3)==0)
		    		{	q->u.weekNday.month=(octet)i+1;	//months are 1-12
		    			break;
		    		}
		    	strdelim(",");					//skip past comma
			}
			if (*lp=='l'||*lp=='L')				//Last week
			{	q->u.weekNday.week=6;
				strdelim(",");
			}
			else
				q->u.weekNday.week=ReadB(1,6);
	    	for(i=0;i<7;i++)
	    		if (strnicmp(lp,DOWNames[i],3)==0)
	    		{	q->u.weekNday.day=(octet)i+1;		//days are 1-7
	    			break;
	    		}
	    	lp+=3;
		}
		else if (strstr(lp,"..")!=NULL)		//must be daterange
		{	q->choice=1;						//DateRange
			if (ParseDate(&q->u.date_range.start_date)) goto calx;
			lp=strstr(lp,"..")+2;				//skip delimiter
			if (ParseDate(&q->u.date_range.end_date)) goto calx;
		}
		else									//must be date
		{	q->choice=0;						//Date
			if (ParseDate(&q->u.date)) goto calx;
		}
		lp=ep;
		ep[-1]=')';
		q->next=NULL;							//link onto the list
		if (fp==NULL)
			fp=q;								//remember first guy we made
		else
			p->next=q;							//link new guy on the end of the list
		p=q;									//remember new guy is now the last guy
		q=NULL;
	}
	calfail=false;
calx:
	*calp=fp;
	if (q!=NULL) free(q);						//don't lose this block!
	return calfail;								//								***008 End
}

///////////////////////////////////////////////////////////////////////
//	parse a date/time value
//in:	lp		points to (date,time)
//		dtp		points to structure to initialize
//out:	true	if cancel selected

BOOL ParseDateTime(BACnetDateTime *dtp)
{	skipwhitespace();
	if (MustBe('(')) return true;
    if (ParseDate(&dtp->date)) return true;
	if (MustBe(',')) return true;
    if (ParseTime(&dtp->time)) return true;
	return MustBe(')');
}

///////////////////////////////////////////////////////////////////////
//	parse a date value
//in:	lp		points to date
//		dtp		points to structure to initialize
//out:	true	if cancel selected

BOOL ParseDate(BACnetDate *dtp)
{	octet	db;
	word	i;
	char opj[300]; // MAG

	sprintf(opj,"PD: Enter ParseDate, lp = '%s'\n",lp);	//MAG
	mprintf(opj);									//MAG
	
	dtp->year=dontcare;
	dtp->month=dontcare;
	dtp->day_of_month=dontcare;
	dtp->day_of_week=dontcare;
	skipwhitespace();
	mprintf("PD: about to read first set\n");
    if ((db=ReadB(1,31))!=dontcare)				//not wild
    {	if (lp[-1]=='/')						//was it month/day/year?
    	{	mprintf("PD: find month first\n");
			if (db>12)							//yes
    		{	tperror("Month must be 1-12!",true);
				return true;
			}
			dtp->month=db;
    	}
    	else									//must be day-month-year
    	{	mprintf("PD: find day of month first\n");
			if (db>31)
    		{	tperror("Day of month must be 1-31!",true);
    			return true;
    		}
    		dtp->day_of_month=db;
    	}
    } else mprintf("PD: first value not specified\n");
	sprintf(opj,"PD: About to read second set, db = %d lp = '%s' lp[-1] = '%c' \n",db,lp,&lp[-1]);
	mprintf(opj);
    if (lp[-1]=='/')							//was it month/day/year?
    {	mprintf("PD: second set slash case\n");
		if ((db=ReadB(1,31))!=dontcare)			//we'll check for valid days later
    		dtp->day_of_month=db;
    }
    else if (lp[-1]=='-')						//day-month-year
    {	mprintf("PD: second set dash case\n");
		for (i=0;i<12;i++)
    		if (strnicmp(lp,MonthNames[i],3)==0)
    		{	dtp->month=(octet)i+1;			//months are 1-12
    			break;
    		}
    	if ((strdelim("-"))==NULL)
		{	tperror("Must use monthname-year here!",true);
			return true;
    	}
    }
    else if (lp[-1]!=',')
	{	tperror("Must use month/day/year or day-monthname-year here!",true);
		return true;
	}
    
	mprintf("PD: About to read third set\n");
    if (i=ReadW())								//not wild
    {	if (i>2154)								//can't represent this date
    	{	tperror("Can't represent dates beyond 2154!",true);
    		return true;
    	}
    	if (i>254&&i<1900)
    	{	tperror("Can't represent this year!",true);
    		return true;
    	}
    	if (i>=1900) i-=1900;// MAG fix bug here when date==1900 by change > to >= 08 FEB 2001
    	dtp->year=(octet)i;
    }
    if (lp[-1]==space)							//must have a day of week
    {	for(i=0;i<7;i++)
    		if (strnicmp(lp,DOWNames[i],3)==0)
    		{	dtp->day_of_week=(octet)i+1;	//days are 1-7
    			lp+=3;							//skip day name						***013
    			break;
    		}
    }
    else
    	lp--;									//point back to delimiter
	mprintf("PD: Exit ParseDate normal\n");
	return false;
}

///////////////////////////////////////////////////////////////////////
//	parse a time value
//in:	lp		points to time
//		tp		points to structure to initialize
//out:	true	if cancel selected

BOOL ParseTime(BACnetTime *tp)
{	
	tp->hour=dontcare;
	tp->minute=dontcare;
	tp->second=dontcare;
	tp->hundredths=dontcare;
	skipwhitespace();
    tp->hour=ReadB(0,23);
    if (lp[-1]==':')
    {	tp->minute=ReadB(0,59);
    	if (lp[-1]==':')
    	{	tp->second=ReadB(0,59);
    		if (lp[-1]=='.')
    			tp->hundredths=ReadB(0,99);
    	}
    }
    lp--;										//point back to delimiter
	return false;
}

///////////////////////////////////////////////////////////////////////
//	parse a bitstring value as either of two forms:
//		B'11011...1011'
//  or
//		( T,F,T,F,...)		i.e. T or True or F or False in a list					***008
//note:	after the initial B' or ( the list may span multiple lines
//
//in:	lp		points to B'11011...1011' or (...)
//		bsp		points to structure to initialize
//		nbits	how many bits maximum
//		nbf		points to octet to update with number of bits found (or NULL)
//out:	true	if cancel selected

BOOL ParseBitstring(octet *bsp,word nbits,octet *nbf)
{	octet	db;
	char	term[3];
	BOOL	isbits;								//									***008

	memset(bsp,0,nbits/8);					//initialize to 0
	if (nbf) *nbf=0;							//no bits found yet
	
	skipwhitespace();
	if ((*lp=='b'||*lp=='B')&&(lp[1]=='\''||lp[1]=='`'))	//						***008 Begin
	{//it's a bitstring
		isbits=true;
		term[0]='\'';							//terminator
		lp+=2;									//skip over B'
	}
	else if (*lp=='(')
	{//it's a flag list
		isbits=false;
		term[0]=')';							//terminator
		lp++;									//skip over (
	}
	else
	{	tperror("Expected B'bitstring' or (T,F...) bitflag list here!",true);
		return true;
	}
	term[1]=',';
	term[2]=0;									//i.e. term is asciz string
	db=0x80;
	while(feof(ifile)==0)
	{	if (*lp==0) 
			if (ReadNext()==NULL) break;
		if (*lp==term[0])
		{	lp++;
			break;								//found terminator
		}
		if (nbits)
		{	if (isbits)
			{	if (*lp=='1')
					*bsp|=db;
				else
					if (*lp!='0')
					{	tperror("Bitstring must contain 0s or 1s!",true);
						return true;
					}
				lp++;							//skip 0 or 1
			}
			else
			{	skipwhitespace();
				if (*lp=='t'||*lp=='T')
					*bsp|=db;
				if (strdelim(&term[0])==NULL) break;
				if (lp[-1]==term[0]) break;
			}										//								***008 End
			if ((db>>=1)==0)
			{	db=0x80;
				bsp++;
			}
			if (nbf) *nbf+=1;					//update num bits found
			nbits--;
		}
		else
		{	tperror("Expected end of bitstring here!",true);
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////
//	parse an octetstring value as X'hexchars'
//in:	lp		points to X'hexchars'
//		osp		points to structure to initialize
//		nmax	how many octets maximum
//		ncount	points to word to receive string size in octets (may be NULL)
//out:	true	if cancel selected

BOOL ParseOctetstring(octet *osp,word nmax,word *ncount)
{	memset(osp,0,nmax);						//initialize to 0
	if (ncount!=NULL) *ncount=0;
	skipwhitespace();
	if ((*lp!='x'&&*lp!='X')||lp[1]!='\'')
	{	tperror("Expected X'octetstring' here!",true);
		return true;
	}
	lp+=2;										//skip over X'
	while(*lp&&*lp!='\'')
	{	if (nmax)
		{   lp=cvhex(lp,osp);					//convert some chars
		    osp++;
		    if (ncount!=NULL) (*ncount)++;
			nmax--;
		}
		else
		{	tperror("Expected ' for end of octetstring here!",true);
			return true;
		}
	}
	lp++;										//skip trailing '
	return false;
}

///////////////////////////////////////////////////////////////////////
//	read a list BACnetActionCommands from the buffer lp points to
//	((actioncommand),(actioncommand),(actioncommand)...)
//in:	lp		points to current position in buffer lb
//out:	NULL	if an error occurred
//		else	pointer to a list of BACnetActionCommands
//		lp		points past the delimiter ) unless it was the end of the buffer

BACnetActionCommand *ReadActionCommands()
{	BACnetActionCommand	*firstp=NULL,*p=NULL,*q=NULL;
	propdescriptor		*pd;
	word				i;
	dword				dw;
	char				pn[40];				
				
	skipwhitespace();							//									***008
	if (MustBe('(')) return NULL;
	while(true)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (	i.e. the beginning of a new BACnetActionCommand in the list
		//3. )				i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0) 
			if (ReadNext()==NULL) break;		//									***008
		if (*lp==')') 
		{	lp++;
			break;								//close this list out
		}
		if (MustBe('(')) break;

		//here we have (BACnetActionCommand)...
		if ((q=malloc(sizeof(BACnetActionCommand)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			break;
		}
		q->device_id=badobjid;
		if (*lp!=',')							//									***008 Begin
		{	dw=ReadObjID();
			if ((word)(dw>>22)!=DEVICE)
			{	if (tperror("Must use a Device Object Identifier here!",true))
					break;
			}
			q->device_id=dw;
			if ((strdelim(","))==NULL) goto acprem;
		}
		else
			lp++;								//skip comma						***008 End
		dw=ReadObjID();
		if (dw==badobjid)
		{	if (tperror("Must use an Object Identifier here!",true))
				break;
		}
		q->object_id=dw;
		if ((strdelim(","))==NULL) goto acprem;
		skipwhitespace();					//									***008
		i=0;
		while(*lp&&*lp!=','&&*lp!=space&&*lp!=')'&&i<39)
		{	if (*lp=='_') *lp='-';				//change _ to -						***008
			pn[i++]=*lp++;
		}
		pn[i]=0;								//force it to be asciz
		pd=StdObjects[(word)(dw>>22)].sotProps;	//point to property descriptor table for this object type
		do
		{	if (stricmp(pn,pd->PropertyName)==0)	//found this property name
	        {	q->property_id=pd->PropID;
	        	break;
			}
			if (pd->PropGroup&Last)
			{	tperror("Invalid Property Name",true);
				break;
			}
			pd++;								//advance to next table entry
		}
		while(true);

		if ((strdelim(","))==NULL) goto acprem;
		q->pa_index=NotAnArray;
		if (pd->PropFlags&IsArray)
		{	if (MustBe('[')) break;				//								***012 Begin
			q->pa_index=ReadW();
		}
		if ((strdelim(","))==NULL) goto acprem;	//								***012 End
		if (pd->ParseType==flt)					//it's a floating value
		{	q->value_type=FLT;
			q->av.fproperty_value=(float)atof(lp);
			if ((strdelim(","))==NULL) goto acprem;
		}
		else if (pd->PropET==eiBPV)				//it's a BPV
		{	q->value_type=BPV;
			q->av.bproperty_value=INACTIVE;		//assume inactive
			if (*lp=='a'||*lp=='A')
				q->av.bproperty_value=ACTIVE;
			if ((strdelim(","))==NULL) goto acprem;
		}
		else									//must be unsigned
		{	q->value_type=UNS;
			q->av.uproperty_value=ReadW();
		}
		if (pd->PropFlags&IsCommandable)		//only need priority for commandables	***008 Begin
			q->priority=ReadB(0,16);
		else
			if ((strdelim(","))==NULL) goto acprem;	//							***008 End
		q->post_delay=ReadW();
		q->quit_on_failure=ReadBool();
		q->write_successful=ReadBool();
		if (lp[-1]!=')')
acprem:	{	tperror("Expected ) here!",true);
			break;
		}
		q->next=NULL;							//link onto the list			***008 Begin
		if (firstp==NULL)
			firstp=q;							//remember first guy we made
		else
			p->next=q;							//link new guy on the end of the list
		p=q;									//remember new guy is now the last guy
		q=NULL;
	}
	if (q!=NULL) free(q);						//don't lose this block!
	return firstp;								//								***008 End
}

///////////////////////////////////////////////////////////////////////
//	read a list of BACnetVTClasses from the buffer lp points to
//	(class,class,class...)
//in:	lp		points to current position in buffer lb
//		vtclp	points to a BACnetVTClassList pointer variable to be
//				initialized to point to the created list of BACnetVTClasses
//out:	true	if an error occurred
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseVTClassList(BACnetVTClassList **vtclp)
{	BACnetVTClassList	*p=NULL,*q=NULL;
	word				vtc;
				
	*vtclp=NULL;									//initially there is no list
	
	skipwhitespace();
	if (MustBe('(')) return true;
	while(feof(ifile)==0)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//2. the beginning of a new BACnetVTClass enumeration in the list
		//3. )				i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0)
			if (ReadNext()==NULL) break;		//									***008
		if (*lp==')') 
		{	lp++;
			break;								//close this list out
		}

		if ((q=malloc(sizeof(BACnetVTClassList)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			break;
		}

		if ((vtc=ReadEnum(&etVTClasses))!=0xFFFF)
		{	q->vtclass=vtc;
			q->next=p;							//link onto the list
			p=q;
		}
		else
			free(q);							//give this one up

		q=NULL;
		if (lp[-1]==')') break;					//list is done						***008
	}
	if (q!=NULL) free(q);						//don't lose this block!
	*vtclp=p;
	return false;
}

///////////////////////////////////////////////////////////////////////				***008 Begin
//	read an array of BACnetObjectIdentifiers from the buffer lp points to
//	{(objtype,instance),(objtype,instance),...}
//in:	lp		points to current position in buffer lb
//		dalp	points to a BACnetObjectIdentifier pointer variable to be
//				initialized to point to the created list of BACnetObjectIdentifiers
//		nump	points to a word counter for the number of items found
//out:	true	if an error occurred
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseObjectList(BACnetObjectIdentifier **dalp,word *nump)
{	BACnetObjectIdentifier	*firstp=NULL,*p=NULL,*q=NULL;
				
	*dalp=NULL;									//initially there is no list
	*nump=0;
	
	skipwhitespace();
	if (*lp=='?') return false;					//? means no list
	if (MustBe('{')) return true;
	while(feof(ifile)==0)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (			i.e. the beginning of a new BACnetObjectIdentifier in the list
		//3. )			i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0)
			if (ReadNext()==NULL) break;		//									***008
		if (*lp=='}') 
		{	lp++;
			break;								//close this list out
		}

		//here we have (objtype,instance),...

		if ((q=malloc(sizeof(BACnetObjectIdentifier)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			break;
		}
		q->object_id=ReadObjID();
		q->next=NULL;							//link onto the list
		if (firstp==NULL)
			firstp=q;							//remember first guy we made
		else
			p->next=q;							//link new guy on the end of the list
		p=q;									//remember new guy is now the last guy
		q=NULL;
		*nump+=1;
	}
	if (q!=NULL) free(q);						//don't lose this block!
	*dalp=firstp;
	return false;
}												//									***008 End

///////////////////////////////////////////////////////////////////////
//	read a list of BACnetAddressBindings from the buffer lp points to
//	(((device,instance),network,macaddr),((device,instance),network,macaddr),((device,instance),network,macaddr)...)
//in:	lp		points to current position in buffer lb
//		dalp	points to a BACnetAddressBinding pointer variable to be
//				initialized to point to the created list of BACnetAddressBindings
//out:	true	if an error occurred
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseAddressList(BACnetAddressBinding **dalp)
{	BACnetAddressBinding	*p=NULL,*q=NULL;
	dword						dw;
				
	*dalp=NULL;									//initially there is no list
	
	skipwhitespace();
	if (*lp=='?') return false;					//? means no list					***008
	if (MustBe('(')) return true;
	while(feof(ifile)==0)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (			i.e. the beginning of a new BACnetAddressBinding in the list
		//3. )			i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0)
			if (ReadNext()==NULL) break;		//									***008
		if (*lp==')') 
		{	lp++;
			break;								//close this list out
		}
		if (MustBe('(')) break;

		//here we have (device,instance),network,macaddr)...

		if ((q=malloc(sizeof(BACnetAddressBinding)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			break;
		}
		dw=ReadObjID();
		if ((word)(dw>>22)!=DEVICE)
		{	if (tperror("Must use a Device Object Identifier here!",true))
				break;
		}
		q->device_object_id=dw;
		if ((strdelim(","))==NULL) goto alprem;
        q->device_address.network_number=ReadW();
        if (ParseOctetstring(&q->device_address.mac_address[0],
        					 sizeof(q->device_address.mac_address),
        					 &q->device_address.address_size)) break;
		if (*lp++!=')')
alprem:	{	lp--;
			tperror("Expected ) here!",true);
			break;
		}
		q->next=p;								//link onto the list
		p=q;
		q=NULL;
	}
	if (q!=NULL) free(q);						//don't lose this block!
	*dalp=p;
	return false;
}

///////////////////////////////////////////////////////////////////////
//	read a list of BACnetSessionKeys from the buffer lp points to
//	((key,network,macaddr),(key,network,macaddr),(key,network,macaddr)...)
//in:	lp		points to current position in buffer lb
//		dalp	points to a BACnetSessionKey pointer variable to be
//				initialized to point to the created list of BACnetSessionKeys
//out:	true	if an error occurred
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseSessionKeyList(BACnetSessionKey **dalp)
{	BACnetSessionKey	*p=NULL,*q=NULL;
				
	*dalp=NULL;									//initially there is no list
	
	skipwhitespace();
	if (MustBe('(')) return true;
	while(feof(ifile)==0)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (			i.e. the beginning of a new BACnetSessionKey in the list
		//3. )			i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0)
			if (ReadNext()==NULL) break;		//									***008
		if (*lp==')') 
		{	lp++;
			break;								//close this list out
		}
		if (MustBe('(')) break;

		//here we have key,network,macaddr)...

		if ((q=malloc(sizeof(BACnetSessionKey)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			break;
		}
        if (ParseOctetstring(&q->session_key[0],
        					 sizeof(q->session_key),NULL)) break;
		if ((strdelim(","))==NULL) goto skprem;
        q->peer_address.network_number=ReadW();
        if (ParseOctetstring(&q->peer_address.mac_address[0],
        					 sizeof(q->peer_address.mac_address),
        					 &q->peer_address.address_size)) break;
		if (*lp++!=')')
skprem:	{	lp--;
			tperror("Expected ) here!",true);
			break;
		}
		q->next=p;								//link onto the list
		p=q;
		q=NULL;
	}
	if (q!=NULL) free(q);						//don't lose this block!
	*dalp=p;
	return false;
}

///////////////////////////////////////////////////////////////////////
//	read a list of BACnetDEstination from the buffer lp points to
//	((days,from,to,(recipient),procid,conf,transitions),(days,from,to,(recipient),procid,conf,transitions)...)
//in:	lp		points to current position in buffer lb
//		dalp	points to a BACnetDestination pointer variable to be
//				initialized to point to the created list of BACnetDestinations
//out:	true	if an error occurred
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseDestinationList(BACnetDestination **dalp)
{	BACnetDestination	*p=NULL,*q=NULL;
	char	c;
	octet	dm;
	word	i;
	char found_day;  // MAG

	char lpj[400];
				
	*dalp=NULL;									//initially there is no list
	
	mprintf("PDL: Enter ParseDestinationList\n");
	skipwhitespace();
	if (MustBe('(')) return true;
	while(feof(ifile)==0)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (			i.e. the beginning of a new BACnetDestination in the list
		//3. )			i.e. the closing part of the list
		sprintf(lpj,"PDL: start loop lp = '%s'\n",lp);
		mprintf(lpj);
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0)
			if (ReadNext()==NULL) break;		//									***008
		if (*lp==')') 
		{	lp++;
			break;								//close this list out
		}

		if (MustBe('(')) break;

		//here we have days,from,to,(recipient),procid,conf,transitions),...

		if ((q=malloc(sizeof(BACnetDestination)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			break;
		}
		//look for (days of week),
		skipwhitespace();						//									***013
		mprintf("PDL: About to check weekdays\n");
		if (MustBe('(')) break;
		q->valid_days=0;
		mprintf("PDL: pre while 1\n");
        while(*lp&&*lp!=')')
        {   while (*lp==space||*lp==',') lp++;	//skip separation between list elements
			sprintf(lpj,"PDL: post while 2 lp = '%s'\n",lp);
			mprintf(lpj);
	    	if (*lp==')') break;				//done								***013
			found_day = 0;  // MAG
	    	for(i=0;i<7;i++)
	    		if (strnicmp(lp,DOWNames[i],3)==0)
	    		{	q->valid_days|=(octet)(0x80>>i);	//Monday is 80, Sunday is 2
					while (*lp&&*lp!=' '&&*lp!=','&&*lp!=')') lp++;	//find delim	***014
					found_day = 1;
	    			break;
	    		}
			sprintf(lpj,"PDL: post while 3 lp = '%s'\n",lp);
			mprintf(lpj);
			if(!found_day)
				return tperror("Expected Day of Week Here",true);

		}
		mprintf("PDL: Past check weekdays\n");
		if (MustBe(')')) break;
		if (strdelim(",")==NULL) break;
    	q->from_time.hour=ReadB(0,23);
    	q->from_time.minute=dontcare;
    	q->from_time.second=dontcare;
    	q->from_time.hundredths=dontcare;
    	if (lp[-1]==':')
    	{	q->from_time.minute=ReadB(0,59);
    		if (lp[-1]==':')
    		{	q->from_time.second=ReadB(0,59);
    			if (lp[-1]=='.')
    				q->from_time.hundredths=ReadB(0,99);
    		}
    	}
		mprintf("PDL: Past min-sec-hsec\n");
		lp--;
		if (strdelim(",")==NULL) break;
    	q->to_time.hour=ReadB(0,23);
    	q->to_time.minute=dontcare;
    	q->to_time.second=dontcare;
    	q->to_time.hundredths=dontcare;
    	if (lp[-1]==':')
    	{	q->to_time.minute=ReadB(0,59);
    		if (lp[-1]==':')
    		{	q->to_time.second=ReadB(0,59);
    			if (lp[-1]=='.')
    				q->to_time.hundredths=ReadB(0,99);
    		}
    	}
		mprintf("PDL: Past min-sec-hsec 2\n");
		lp--;
		if (strdelim(",")==NULL) break;
		if (ParseRecipient(&q->recipient)==NULL) break;
		if (strdelim(",")==NULL) break;
		q->process_id=ReadW();
		q->notification=ReadBool();
		skipwhitespace();
		if (MustBe('(')) break;
		q->transitions=0;
		dm=0x80;
		while (c=*lp++)
		{	if (c==')') break;
			if (c==',') dm>>=1;
			if (c=='t'||c=='T') q->transitions|=dm;
		}
		skipwhitespace();
		if (MustBe(')')) break;
		q->next=p;								//link onto the list
		p=q;
		q=NULL;
		mprintf("PDL: End of loop\n");
	}
	if (q!=NULL) free(q);						//don't lose this block!
	*dalp=p;
	mprintf("PDL: Return\n");
	return false;
}

///////////////////////////////////////////////////////////////////////
//	read a list of BACnetRecipient from the buffer lp points to
//	((device,instance) or (network,macaddr),...)
//in:	lp		points to current position in buffer lb
//		dalp	points to a BACnetRecipient pointer variable to be
//				initialized to point to the created list of BACnetRecipients
//out:	true	if an error occurred
//		lp		points past the delimiter ) unless it was the end of the buffer

BOOL ParseRecipientList(BACnetRecipient **dalp)
{	BACnetRecipient	*p=NULL,*q=NULL;
				
	*dalp=NULL;									//initially there is no list
	
	skipwhitespace();
	if (MustBe('(')) return true;
	while(feof(ifile)==0)
	{   //here lp must point to:
		//1. a comma or whitespace which we ignore as a separator between list elements.
		//   Note that we require "empty" list elements to use proper syntax (...),(),(...)
		//   but (...),,(...) is treated the same as (...),(...)
		//2. (			i.e. the beginning of a new BACnetRecipient in the list
		//3. )			i.e. the closing part of the list
		while (*lp==space||*lp==',') lp++;		//skip separation between list elements
		if (*lp==0)
			if (ReadNext()==NULL) break;		//									***008
		if (*lp==')') 
		{	lp++;
			break;								//close this list out
		}

		if ((q=ParseRecipient(NULL))!=NULL)
		{	q->next=p;							//link onto the list
			p=q;
			q=NULL;
		}
	}
	if (q!=NULL) free(q);						//don't lose this block!
	*dalp=p;
	return false;
}

///////////////////////////////////////////////////////////////////////
//	read a BACnetRecipient from the buffer lp points to
//	(device,instance) or (network,macaddr)
//in:	lp		points to current position in buffer lb
//out:	NULL	if an error occurred
//		else	pointer to newly created BACnetRecipient
//		lp		points past the delimiter ) unless it was the end of the buffer

BACnetRecipient *ParseRecipient(BACnetRecipient *inq)
{	BACnetRecipient	*q=NULL;
	dword				dw;
	
	skipwhitespace();
	//here we have (device,instance) or (network,macaddr)...
	if (MustBe('(')) return NULL;

	if (inq==NULL)
	{   if ((q=malloc(sizeof(BACnetRecipient)))==NULL)
		{	tperror("Can't Get Object Space!",true);
			return NULL;
		}
	}
	else
		q=inq;
	q->next=NULL;
	skipwhitespace();
	if (isdigit(*lp))						//must be network,macaddress)
	{	q->choice=1;						//address
        q->u.address.network_number=ReadW();
        if (ParseOctetstring(&q->u.address.mac_address[0],
        					 sizeof(q->u.address.mac_address),
        					 &q->u.address.address_size)) goto brfail;
		if (*lp++!=')')
		{	lp--;
			tperror("Expected ) here!",true);
			goto brfail;
		}
	}
	else									//must be (device,instance)
	{	lp--;								//ReadObjID needs to see the (
		dw=ReadObjID();
		if ((word)(dw>>22)!=DEVICE)
		{	if (tperror("Must use a Device Object Identifier here!",true))
				goto brfail;
		}
		q->choice=0;						//device
		q->u.device=dw;
	}
	return q;

brfail:
	if (inq==NULL) free(q);				//don't release unless we malloc'd it
	return NULL;
}

///////////////////////////////////////////////////////////////////////
//	parse a string parameter
//in:	p		points to string variable to contain the result
//		ps		max size of p
//		param	points to parameter to be parsed
//
//The param pointer should point to a string like:
//		"some string"
//or	'some string'
//or	`somestring'
//out:	true		if cancel selected

BOOL setstring(char *p,word ps, char *param)
{	char	q;
	word	i;

	lp=param;									//									***008
	skipwhitespace();
	q=*lp++;
	if (q==singlequote||q==doublequote)
	{	for (i=0;i<(ps-1);i++)					//copy until end of line, end of string or ps chars copied
		  if (*lp==q)
		  {	lp++;								//skip trailing quote				***012 Begin
		  	break;
		  }
		  else if (*lp==0)						//									***012 End
			break;								//found end of line
		  else
		  	*p++=*lp++;
		*p=0;									//mark end with asciz
		return false;
	}
	return tperror("Expected string parameter here",true);
}

///////////////////////////////////////////////////////////////////////
//	read a non-zero dword from the buffer lp points to
//in:	lp		points to current position in buffer lb
//out:	0		if end of buffer
//		else	the number
//		lp		points past the delimiter unless it was the end of the buffer

dword ReadDW()
{	dword	d=0;
	char	c;

	skipwhitespace();
	while (isdigit(c=*lp++))
		d=(d*10L)+(c-'0');
	if (c=='?') c=*lp++;						//pretend ? is a valid digit
	if (c==0) lp--;								//stick at end of buffer
	return d;
}

///////////////////////////////////////////////////////////////////////
//	read a word from the buffer lp points to
//in:	lp		points to current position in buffer lb
//out:	0		if end of buffer
//		else	the number
//		lp		points past the delimiter unless it was the end of the buffer

word ReadW()
{	dword d;

	d=ReadDW();
	if (d>65535)
	{	lp--;
		tperror("Must use an Unsigned Integer 0..65535 here!",true);
		d=0;
	}
	return (word)d;
}                

///////////////////////////////////////////////////////////////////////
//	read a byte from the buffer lp points to
//in:	lp		points to current position in buffer lb
//		lb,ub	range of acceptable values
//out:	255		if out of range
//		else	the number
//		lp		points past the delimiter unless it was the end of the buffer

octet ReadB(octet lb,octet ub)
{	octet d=0;
	char b[64],c;

	mprintf("RB: Enter ReadB lp = '");
	mprintf(lp);
	mprintf("'\n");
	skipwhitespace();
	while (isdigit(c=*lp++))
		d=(d*10)+(c-'0');
	if (c=='?') 
	{	c=*lp++;								//pretend ? is a valid digit
		d=dontcare;
	}
	if (c==0) lp--;								//stick at end of buffer
	if (d!=dontcare)
	{	if(d<lb||d>ub)
		{	lp--;
			sprintf(b,"Must use an Unsigned Integer %u..%u here!",lb,ub);
			tperror(b,true);
			d=dontcare;
		}
	}
	return d;
}                

///////////////////////////////////////////////////////////////////////
//	read a boolean (true/false) from the buffer lp points to
//in:	lp		points to current position in buffer lb
//out:	0/1
//		lp		points past the delimiter unless it was the end of the buffer

octet ReadBool()
{	char	q;
	octet	v=0;
	
	while (q=*lp++)
	{	if (q==')'||q=='}'||q==',') break;
		if (q=='t'||q=='T') v=1;
	}
	return v;
}

///////////////////////////////////////////////////////////////////////
//	read an enumeration name from the buffer lp points to
//in:	lp			points to current position in buffer lb
//		enumtable	table of pointers to enumeration names (tsize of them)
//		tsize		number of names in the table
//		firstprop	the index of the first proprietary value, or 0 meaning not extensible
//out:	0xFFFF		if end of buffer or invalid enumeration
//		else		the enumeration (note that ? results in enumeration 0)
//		lp			points past the delimiter unless it was the end of the buffer

word ReadEnum(etable *etp)
{	char	c,e[33];
	word	i;
	int j;  // MAG
	char opj[200];

	mprintf("RE: Enter ReadEnum lp = '");
	mprintf(lp);
	mprintf("'\n");
	skipwhitespace();
	i=0;
	while (c=*lp)								//until the end of the buffer
	{	lp++;									//advance scan
		if (c=='_'||c==space) c='-';			//convert underscore or space to dash	***006
		if (c=='?'||c=='-'||isalnum(c))			//if its a valid part of an enumeration name
		{	if (i<32) e[i++]=c;}				//save this character
		else									//found the delimiter
			break;
	}
	mprintf("RE: find enum '");
	mprintf(e);
	mprintf("'\n");
	
	j = strlen(e);
	sprintf(opj,"RE: spot j-2 = %c spot j-1 = %c\n",e[j-2],e[j-1]);
	mprintf(opj);
	if((j > 3)&&(e[j-2] == '-')&&((e[j-1] = 'w')||(e[j-1] = 'W'))){ // MAG 3 MAR 2001 fix for string ending in '-w' (writeable)
		mprintf("RE: fix end string: was '");
		mprintf(e);
		mprintf("' now '");
		lp -=2;
		e[j-2] = 0;
		mprintf(e);
		mprintf("'\n");
	}

	if (i)
	{	e[i]=0;									//always leave asciz in buffer
		if (e[0]=='?') return 0;				//? defaults to enumeration 0
		for (i=0;i<etp->nes;i++)
		{	if (etp->estrings[i])				//make sure it's not null			***006
				if (stricmp(e,etp->estrings[i])==0){	//matching enumeration
					mprintf("RE: find match- return\n");
					return i;
				}
		}
		if (stricmp(e,"proprietary")==0)
		{	if (etp->propes)
			{	if ((i=(word)ReadDW())>=etp->propes)
					return i;
				tperror("Proprietary enumeration cannot use the reserved range for this property!",true);
			}
			else
				tperror("This is not an extensible enumeration!",true);
			return 0xFFFF;
		}
	}	
	mprintf("RE: Return w/o enumeration\n");
	tperror("Expected an Enumeration Name here!",true);
	return 0xFFFF;
}

///////////////////////////////////////////////////////////////////////
//	read an object identifier (objecttype,instance) from the buffer lp points to
//in:	lp			points to current position in buffer lb
//		objid		points to the dword to receive the object identifier
//out:	badobjid	if it was an invalid object identifier,
//		else		the object identifier
//		lp			points past the delimiter unless it was the end of the buffer

dword ReadObjID()
{	word	objtype;
	dword	id;

	mprintf("ROI: Enter ReadObjID\n");
	skipwhitespace();
	if (*lp++!='(')
	{	tperror("Expected ( before (objecttype,instance) here!",true);
		goto roidx;
	}
	if ((objtype=ReadEnum(&etObjectTypes))==0xFFFF)
		goto roidx;
	skipwhitespace();						//									***006 Begin
	if (strnicmp(lp,"instance ",9)==0)		//ignore instance here
		lp+=9;									//									***006 End
	id=ReadDW();
	if (lp[-1]==')')							//it ended with a closing paren, it's ok
	{	if (id<(1L<<22))						//valid instance number
			return (((dword)objtype)<<22)+id;	//save the object identifier as a dword
		tperror("Object Instance must be 0..4194303!",true);
		goto roidx;
	}	
	tperror("Expected ')' after instance here!",true);

roidx:
	return badobjid;
}

///////////////////////////////////////////////////////////////////////
//	find the next non-whitespace in a file
//in:	lp		points to current position in buffer lb
//		ifile	file stream 
//out:	NULL	if eof,
//		else	pointer to next non-whitespace
//				(in this case lp also points to it)

char *ReadNext()								//										***008
{
	do
	{	if (feof(ifile)) return NULL;			//end of file							***008
		readline(lb,sizeof(lb));				//read a line from the file 			***008
		lp=&lb[0];
		skipwhitespace();
	} while (*lp==0);							//this was a blank line
	return lp;
}

///////////////////////////////////////////////////////////////////////				***008 Begin
//	find the end of whitespace in a string
//in:	lp		points to string 
//out:	lp		points to first non-whitespace char

void skipwhitespace()
{	while (*lp==space) lp++;
	if (*lp==0) ReadNext();						//									***008 End
}

///////////////////////////////////////////////////////////////////////
//	find the first element of an array or list after whitespace{whitespace
//in:	p		points to string 
//out:	points to first non-whitespace char or NULL if no { was found

char *openarray(char *p)
{	BOOL	foundlb=false;

	while (*p==space||*p=='{')
	{	if (*p=='{')
			if (foundlb)
				break;							//treat second { as "first element"
			else
				foundlb=true;					//well, we found one
		p++;									//skip it
	}
	if (foundlb)
		return p;
	else
	{	tperror("Expected { here...",true);
		return NULL;							//didn't find {
	}
}

///////////////////////////////////////////////////////////////////////
//	Create a TPI file
//in:	tp		points to file name string
//out:	0		success
//		else	error code

int CreateTextPICS(char *tp)
{	FILE	*f;
	
	if ((f=fopen(tp,"w"))==NULL) return errno;	//return system error code for it
    return 0;
}

///////////////////////////////////////////////////////////////////////
//	Read a line from a text file, squishing out redundant white space and comments
//in:	lp		points to a line buffer
//		lps		size of buffer lp
//		ifile	file stream to read from
//out:	lp		filled with the line, ends in 0
//		lc		updated

void readline(char *lp,int lps)
{	char	*dp,*sp,c;
	BOOL	HaveNonWS=FALSE;

	fgets(lp,lps,ifile);						//get a line from the file			***008
	dp=sp=lp;
	while (*sp)
	{	switch(c=*sp++)
		{
		case space:
		case tab:
		case cr:
		case lf:
			while (c=*sp)
		  	  if (c==space||c==tab||c==cr||c==lf) 
		  		sp++; 							//skip white space
		  	  else
		  		break;
			if (*sp==0) goto rlexit;			//we're done
			if (HaveNonWS)
				*dp++=space;					//convert a white space sequence to just a single space
			break;
		case '-':								//comment?
			if (*sp=='-') goto rlexit;			//yes, ignore to the end
			*dp++=c;
			HaveNonWS=TRUE;
			break; 
		case accentgrave:
			c=singlequote;						//matching quote is singlequote
			goto rlquote;
		case doublequote:
		case singlequote:
rlquote:	*dp++=c;
			while (*sp&&c!=*sp)
				*dp++=*sp++;
			*dp++=c;
			if (*sp) sp++;						//consume closing quote unless it's the end of the line
			HaveNonWS=TRUE;
			break;
		default:
			*dp++=c;
			HaveNonWS=TRUE;
		}
	}
rlexit:
	*dp=0;										//mark the end with asciz
	lc++;										//bump line count
//	printf("%.3u:%s\n",lc,lp);					//*** DEBUG ***
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

///////////////////////////////////////////////////////////////////////			***008 Begin 
//	Find the next delimiter
//in:	lp	points to the beginning of the string to look in
//		d	points to the set of delimiter characters
//out:	returns pointer past delimiter if one was found or NULL if EOF
//		lp	is also that pointer

char *strdelim(char *d)
{	char *q,*dq;								//temp pointers

	while(feof(ifile)==0)
	{	dq=d;									//point to list of delimiters
		while (*dq&&*lp)						//for each delimiter character	***006
		{	if ((q=strchr(lp,*dq++))!=NULL)		//got a delimiter
			{	lp=q+1;
				return lp;						//return pointer past delimiter
			}
		}
		ReadNext();
	}
	return NULL;								//not found						***008 End
}

///////////////////////////////////////////////////////////////////////			***006 Begin 
//	Trim whitespace off the end of a string
//in:	p	points to the beginning of the string to look in

void rtrim(char *p)
{	char *q;									//temp pointer
	q=strchr(p,0);								//find the asciz
	while (p!=q)
	{	if(*q==' '||*q==0x09)
			*q--=0;
		else
		  return;
	}
}												//								***006 End

/////////////////////////////////////////////////////////////////////// 
//	Convert HEX chars to binary octet
//in:	src		points to 2 hex chars
//		dst		points to octet to receive the value
//out:	ptr to 1st non-hex char, or 2 past src
char *cvhex(char *src,octet *dst)
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
//	check for a character during parsing and complain if not found
//in:	c			the expected character
//		lp			points to where it should be
//out:	true		if error and cancel selected (also sets cancel=true)
BOOL MustBe(char c)
{	char	b[20];

	if (*lp++!=c)
	{	lp--;
		sprintf(b,"Expected %c here!",c);
		return tperror(b,true);
	}
	return false;
}

/////////////////////////////////////////////////////////////////////// 
//	display an error message dialog and update the error count
//in:	mp			points to specific message
//		showline	true if need to show source line and position
//out:	true		if cancel selected (also sets cancel=true)

BOOL tperror(char *mp,BOOL showline)
{	static char m[512];
	char 		*p,c;
	
	m[0]=0;
	lerrc++;
	if (showline)
		sprintf(m,"Line %u: ",lc);				//add line number first
	p=strchr(m,0);								//find asciz

	sprintf(p,"%s\n",mp);
	if (showline)
	{	p=strchr(p,0);							//find asciz
//		lp--;									//back up by one
		c=*lp;									//save the character we "broke" on
		*lp++=0;								//make it asciz there temporarily
		sprintf(p,"%s«%c»%s\n",lb,c,lp);
	}
	MessageBeep(MB_ICONEXCLAMATION);
	cancel=(MessageBox(NULL,m,"Read Text PICS Error",
					   MB_ICONEXCLAMATION|(showline?MB_OKCANCEL:MB_OK))==IDCANCEL)?TRUE:FALSE;
	return cancel;
}
