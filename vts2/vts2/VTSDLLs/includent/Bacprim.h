//Contents of BACPRIM.H
#ifndef __BACPRIM_H_INCLUDED
#define __BACPRIM_H_INCLUDED

typedef unsigned char byte;
typedef unsigned char octet;
typedef unsigned long dword;
typedef unsigned short word;

typedef struct {	//etimestamp
	word	tsType;								//0=time, 1=sequence number, 2=time/date
	word	tsYear;								//e.g. 1995, also used as sequence number
	word	tsMonth;							//1-12 or 255
	word	tsDay;								//1-31 or 255
	word	tsDow;								//1=Mon, 7=Sun or 255
	word	tsHour;
	word	tsMinute;
	word	tsSecond;
	word	tsHundredth;
	} etimestamp;
	
typedef struct {	//notifyvalue
	word	nvFlags;
	word	nvNumBits;
	word	nvEnum[2];
	byte	nvBits[4];
	float	nvReal[3];
	} notifyvalue;
	
typedef struct {	//proplist
	dword	plObjectID;
	dword	plPropID;
	dword	plArrayIndex;
	char	plPropVal[256];
	} proplist;
	
#ifdef __cplusplus					//so this header can be used with C++
extern "C" {
#endif
octet *APIENTRY eNULL(octet *);
octet *APIENTRY eBOOL(octet *,BOOL);
octet *APIENTRY eUINT(octet *,dword);
octet *APIENTRY eINT(octet *,long);
octet *APIENTRY eREAL(octet *,float);
octet *APIENTRY eDOUBLE(octet *,double);
octet *APIENTRY eOCTETSTRING(octet *,octet *,word);
octet *APIENTRY eDWORD (octet *,dword,octet,BOOL);
octet *APIENTRY eCHARSTRING(octet *,char *,octet,word,octet,word);
octet *APIENTRY eBITSTRING(octet *,byte *,octet,word);
octet *APIENTRY eENUM(octet *,dword);
octet *APIENTRY eDATESTRING(octet *,word,octet,octet,octet);
octet *APIENTRY eTIMESTRING(octet *,octet,octet,octet,octet);
octet *APIENTRY eOBJECTID(octet *,word,dword);
octet *APIENTRY etagOBJECTID(octet *,octet,dword);
word  APIENTRY eDeleteObjectService(octet *,dword);
word  APIENTRY eCreateObjectService(octet *,word,dword,BOOL,HWND);
word  APIENTRY eAtomicReadFileService(octet *,dword,word,long,dword);
word  APIENTRY eAtomicWriteFileService(octet *,word,dword,word,long,BOOL,HWND);
word  APIENTRY eCOVNotificationService(octet *,word,word,dword,dword,dword,word,HWND);
word  APIENTRY eEventNotificationService(octet *,word,dword,dword,dword,etimestamp *,
								word,word,word,word,char *,word,word,word,notifyvalue *,word);
word  APIENTRY eSubscribeCOVService(octet *,dword,dword,word,word,word);
word  APIENTRY eIAmService(octet *,dword,word,word,word);
word  APIENTRY eWhoIsService(octet *,word,dword,dword);
word  APIENTRY eIHaveService(octet *,dword,dword,char *);
word  APIENTRY eWhoHasService(octet *,word,dword,dword,dword,char *);
word  APIENTRY eReadPropService(octet *,dword,dword,word,dword);
word  APIENTRY eReadPropMultipleService(octet *,word,proplist *);
word  APIENTRY eWritePropService(octet *,word,word,dword,char *);
word  APIENTRY eWritePropMultipleService(octet *,word,word,proplist *);
word  APIENTRY eReadPropConditionalService(octet *,word,word,HWND,HWND);
word  APIENTRY eReinitDeviceService(octet *,word,char *);
word  APIENTRY eTimeSynchService(octet *,word,octet,octet,octet,octet,octet,octet,octet);
word  APIENTRY eDeviceCommControlService(octet *,word,word,char *);
word  APIENTRY eGetAlarmSummaryService(octet *);
word  APIENTRY eAcknowledgeAlarmService(octet *,dword,dword,word,
													etimestamp *,char *,etimestamp *);
word  APIENTRY eTextMessageService(octet *,word,word,dword,word,char *,char *);
word  APIENTRY eVTOpenService(octet *,word,word);
word  APIENTRY eVTDataService(octet *,word,word,word,word,octet *);
word  APIENTRY eVTCloseService(octet *,word);
word  APIENTRY eListElementService(octet *,word,word,dword,dword,word,dword,word,word,word,HWND);
word  APIENTRY ePrivateTransferService(octet *,word,word,word,word,word,char *);
word  APIENTRY eGetEnrollmentSummaryService(octet *,word,dword,word,word,octet *,dword,word,word,word,word,word);
word  APIENTRY eSimpleAckPDU (octet *,octet ,octet);					//***234
word  APIENTRY eErrorPDU (octet *,octet,dword,dword,dword);				//***234***235

dword APIENTRY dUINT(octet **);
dword APIENTRY dENUM(octet **);
dword APIENTRY dCONTEXT(octet **,BOOL);
dword APIENTRY dOBJECTID (octet **,word *);
long  APIENTRY dINT(octet **);
octet APIENTRY dNULL (octet **);
BOOL  APIENTRY dBOOL (octet **);
float APIENTRY dREAL (octet **);
double APIENTRY dDOUBLE (octet **);
word  APIENTRY dOCTETSTRING (octet **,octet *);
word  APIENTRY dCHARSTRING(octet **,char *,octet *,word *);
word  APIENTRY dBITSTRING (octet **,octet *);
void  APIENTRY dDATESTRING (octet **,word *,octet *,octet *,octet *);
void  APIENTRY dTIMESTRING (octet **,octet *,octet *,octet *,octet *);

dword APIENTRY vbDWORD(octet *,word);
float APIENTRY vbREAL(octet *);
double APIENTRY vbDOUBLE(octet *);
word  APIENTRY vbWORD(octet *);
long  APIENTRY vbLONG(octet *,word);
dword APIENTRY vbOBJECTID(word,dword);
void  APIENTRY vbeDOUBLE(octet *,double);
void  APIENTRY vbeREAL(octet *,float);
#ifdef __cplusplus					//end of extern "C" declarations
}
#endif
#endif //__BACPRIM_H_INCLUDED
