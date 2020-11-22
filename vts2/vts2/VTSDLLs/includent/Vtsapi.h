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

#ifndef _etable
typedef struct {
	word	propes;								//proprietary enumerations begin at this value
	dword	propmaxes;							//max value for proprietary+1
	word	nes;								//number of string pointers which follow
	char	*estrings[];						//table of pointers to strings
	} etable;
#define _etable 0
#endif

typedef struct {
	char	*name;
	dword	dwcons;
	} namedw;

typedef struct {
	char	*name;
	octet	octetcons;
	} nameoctet;

typedef	struct {
	char	VendorName[64];
	char	ProductName[128];
	char	ProductModelNumber[64];
	char	ProductDescription[128];
	word	BACnetConformanceClass;
	word	MaxAPDUSize;
	octet	SegmentedRequestWindow;
	octet	SegmentedResponseWindow;
	word	RouterFunctions;
	dword	BACnetFunctionalGroups;				//bitmap of functional groups supported
	octet	BACnetCharsets;						//bitmap of std charactersets supported
	octet	BACnetStandardServices[35];			//array of standard services supported
	octet	BACnetStandardObjects[18];			//array of standard objects supported
	word	DataLinkLayerOptions;
	dword	MSTPmasterBaudRates[16];
	dword	MSTPslaveBaudRates[16];
	dword	PTP232BaudRates[16];
	dword	PTPmodemBaudRates[16];
	dword	PTPAutoBaud[2];
	generic_object far *Database;
	} PICSdb;

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
#define		dlISO88023_10BASE5		0x0001
#define		dlISO88023_10BASE2		0x0002
#define		dlISO88023_10BASET		0x0004
#define		dlISO88023_fiber		0x0008
#define		dlARCNETcoaxstar		0x0010
#define		dlARCNETcoaxbus			0x0020
#define		dlARCNETtpstar			0x0040
#define		dlARCNETtpbus			0x0080
#define		dlARCNETfiberstar		0x0100
#define		dlMSTPmaster			0x0200
#define		dlMSTPslave				0x0400
#define		dlPTP232				0x0800
#define		dlPTPmodem				0x1000
#define		dlPTPautobaudmodem		0x2000
#define		dlLonTalk				0x4000
#define		dlOther					0x8000

//bits in BACnetCharsets
#define		csANSI					0x01		//ANSI X3.4
#define		csDBCS					0x02		//IBM/Microsoft DBCS
#define		csJIS					0x04		//JIS C 6226
#define		csUCS4					0x08		//ISO 10646 (UCS-4)
#define		csUCS2					0x10		//ISO 10646 (UCS-2)

#ifdef __cplusplus					//so this header can be used with C++
extern "C" {
#endif
dword APIENTRY VTSAPIgetpropinfo(word,word,char *,word *,word *,word *,word *);
word  APIENTRY VTSAPIgetenumtable(word,word,word *,dword *,char *);
word  APIENTRY VTSAPIgetdefaultparsetype(dword,HWND);
BOOL  APIENTRY VTSAPIgetdefaultpropinfo(word,dword,word *,word *);
word  APIENTRY VTSAPIgetpropertystates(word,HWND);
word  APIENTRY VTSAPIgetpropertystate(word,word,char *);
void  APIENTRY ReadTextPICS(char *,PICSdb *,int *);
void  APIENTRY DeletePICSObject(generic_object *);
#ifdef __cplusplus					//end of extern "C" declarations
}
#endif
#endif //__VTSAPI_H_INCLUDED
