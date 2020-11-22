//Contents of VTSAPI.H
#ifndef __VTSAPI_H_INCLUDED
#define __VTSAPI_H_INCLUDED

typedef unsigned char byte;
typedef unsigned char octet;
typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned short uint;

#include	"db.h"
#include	"stdobj.h"
#include    "VTS.h"

typedef struct {
	char	*name;
	dword	dwcons;
	} namedw;

typedef struct {
	char	*name;
	octet	octetcons;
	} nameoctet;

// msdanner 9/04 added:  
// structure to hold default property value restrictions defined in EPICS
typedef struct {
	double long dlUnsignedMinimum;
	double long dlUnsignedMaximum;
	double long dlSignedMinimum;
	double long dlSignedMaximum;
	float  fRealMinimum;
	float  fRealMaximum;
	float  fRealResolution;
	double dDoubleMinimum;
	double dDoubleMaximum;
	double dDoubleResolution;
	BACnetDate dateMinimum;
	BACnetDate dateMaximum;
	double long dlMaxOctetStringLength;
	double long dlMaxCharacterStringLength;
	double long dlMaxListLength;
	double long dlMaxVariableLengthArray;
} defaultrangelimits;


struct PICSdb {
	PICSdb();
	~PICSdb();

	char	*VendorName;
	char	*ProductName;
	char	*ProductModelNumber;
	char	*ProductDescription;
	char    dummy;
	
	word	BACnetConformanceClass;
	word	MaxAPDUSize;
	octet	SegmentedRequestWindow;
	octet	SegmentedResponseWindow;
	word	RouterFunctions;
	dword	BACnetFunctionalGroups;				//bitmap of functional groups supported
	octet	BACnetCharsets;						//bitmap of std charactersets supported
	octet	BACnetStandardServices[MAX_SERVS_SUPP];			//array of standard services supported
	octet	BACnetStandardObjects[MAX_DEFINED_OBJ];			//array of standard objects supported
	octet	DataLinkLayerOptions[MAX_DATALINK_OPTIONS];     //array of data link layers supported
	dword	MSTPmasterBaudRates[16];
	dword	MSTPslaveBaudRates[16];
	dword	PTP232BaudRates[16];
	dword	PTPmodemBaudRates[16];
	dword	PTPAutoBaud[2];
	dword	BACnetFailTimes[MAX_FAIL_TIMES];                    //array of Fail Times           29/12/2003  GJB
	octet	BIBBSupported[MAX_BIBBS];  	//msdanner 9/2004: array of booleans indicating support for each BIBB,    
	defaultrangelimits defaultlimits;	//msdanner 9/2004: default limits section,  
	generic_object far *Database;
	octet   BBMD;                       //msdanner 9/2004: indicates support for BBMD functionality
	device_obj_type *pDeviceObject;     //msdanner 9/2004: points to parsed Device Object.  Only valid after parsing database.
};

//bits in RouterFunctions
#define		rfNotSupported			0
#define		rfSupported				1

//bits in BACnetFunctionalGroups
#define		fgHHWS					0x00000001
#define		fgPCWS					0x00000002
#define		fgCOVEventInitiation	0x00000004
#define		fgCOVEventResponse		0x00000008
#define		fgEventInitiation		0x00000010
#define		fgEventResponse			0x00000020
#define		fgClock					0x00000040
#define		fgDeviceCommunications	0x00000080
#define		fgFiles					0x00000100
#define		fgTimeMaster			0x00000200
#define		fgVirtualOPI			0x00000400
#define		fgReinitialize			0x00000800
#define		fgVirtualTerminal		0x00001000

//bits in BACnetStandardServices
#define		ssNotSupported			0
#define		ssInitiate				1
#define		ssExecute				2

//bits in BACnetStandardObjects
#define		soNotSupported			0
#define		soSupported				1
#define		soCreateable			2
#define		soDeleteable			4

//bits in DataLinkLayerOptions
//#define		dlISO88023_10BASE5		0x0001
//#define		dlISO88023_10BASE2		0x0002
//#define		dlISO88023_10BASET		0x0004
//#define		dlISO88023_fiber		0x0008
//#define		dlARCNETcoaxstar		0x0010
//#define		dlARCNETcoaxbus			0x0020
//#define		dlARCNETtpstar			0x0040
//#define		dlARCNETtpbus			0x0080
//#define		dlARCNETfiberstar		0x0100
//#define		dlMSTPmaster			0x0200
//#define		dlMSTPslave				0x0400
//#define		dlPTP232				0x0800
//#define		dlPTPmodem				0x1000
//#define		dlPTPautobaudmodem		0x2000
//#define		dlLonTalk				0x4000
//#define		dlOther					0x8000

//bits in BACnetCharsets
#define		csANSI					0x01		//ANSI X3.4
#define		csDBCS					0x02		//IBM/Microsoft DBCS
#define		csJIS					0x04		//JIS C 6226
#define		csUCS4					0x08		//ISO 10646 (UCS-4)
#define		csUCS2					0x10		//ISO 10646 (UCS-2)
#define     cs8859                  0x20        //ISO 8859-1

//bits in Fail Times
#define		ftNotSupported			0           //29/12/2003        GJB

#ifdef __cplusplus					//so this header can be used with C++
extern "C" {
#endif
dword APIENTRY VTSAPIgetpropinfo(word,word,char *,word *,word *,word *,word *);
bool  APIENTRY ReadTextPICS(const char *,PICSdb *,int *,int *);
void  APIENTRY DeletePICSObject(generic_object *);
int   GetStandardServicesSize(void);
const char *GetStandardServicesName(int i);
int   GetBIBBSize(void);
const char *GetBIBBName(int i);
int   GetObjectTypeSize(void);
const char *GetObjectTypeName(int i);
int   GetDataLinkSize(void);
void  GetDataLinkString(int i, PICSdb *pd, char *pstrResult);
int   GetCharsetSize(void);
const char *GetCharsetName(octet csTag);
int   GetSpecialFunctionalitySize(void);
const char *GetSpecialFunctionalityName(int i);
bool cvhex(char *buf,octet *result);

// Global variables
extern UINT EPICSLengthProtocolServicesSupportedBitstring;    // used by test 135.1-2003 (k)
extern UINT EPICSLengthProtocolObjectTypesSupportedBitstring; // used by test 135.1-2003 (l)

#ifdef __cplusplus					//end of extern "C" declarations
}
#endif
#endif //__VTSAPI_H_INCLUDED
