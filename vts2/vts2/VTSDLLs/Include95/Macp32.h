//MACP.H Header file
#ifndef __MACP_H_INCLUDED
#define __MACP_H_INCLUDED

typedef unsigned char byte;
typedef unsigned char octet;
typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned short uint;

//Note:	21-May-99 DMF   this packet should be 1536 total length, but became 1540 by accident
//						Also, maReserved[13] is bad alignment for VB5. So I've changed
//						to this arrangement for fixing the length and VB5...
//		***PAD
typedef struct tagMApacket	{
struct  tagMApacket *maLink;					//linked list pointer		***140
dword	maTrace;								//packet sequence identifier
octet	maLport;								//the logical port within LSL to/from
octet	maAlen;									//the length of sadr and dadr really used
octet	maLsap;									//the link service access point
octet	maBroadcast;							//0=no, 1=yes
//octet	maReserved[13];							//reserved					***PAD deleted
octet	maReserved[6];							//reserved					***PAD added
octet	maDadr[8];								//octet string destination address
octet	maSadr[8];								//octet string source address
word	maNlen;									//length of maNPDU in octets
octet	maNPDU[1497];							//max sized NPDU
octet	maPad[3];								//max sized NPDU			***PAD added
				} MApacket;


typedef struct {						//Protocol Statistics Table
dword	MTotalTxPackets;
dword	MTotalRxPackets;
dword	MIgnoredRxPackets;
dword	MTimeouts;
dword	MNoMAs;							//									***140
dword	MErrors;
			} MACStatTable;

typedef struct {	//Eth8022 packet
	byte	pktDADR[6];		//destination address
	byte	pktSADR[6];		//source address
	byte	pktLEN[2];		//length ms/ls (bytes after these two)
	byte	pktLLC[3];		//LLC DSAP, SSAP, UI
	byte	pktNPDU[1497];	//the NPDU
	} PKT8022;

typedef struct {	//ARCNET NDIS packet				***170 Begin
	byte	ApktSID;		//source address
	byte	ApktDID;		//destination address
	byte	ApktSC;			//the BACnet Service Code
//	byte	ApktIL[2];		//length ms/ls (bytes after these two)(could only be 1)
	byte	ApktLLC[3];		//LLC DSAP, SSAP, UI
	byte	ApktNPDU[504];	//the NPDU
	} PKTARC;				//							***170 End

//#define SIMULTANEOUS_READS	8		//											***110***210
#define		MAXADAPTERS			2		//											***210***218
#define		MAXREADREQS			16		//											***210***218
// Caution: MAXADAPTERS*MAXREADREQS cannot exceed MAXIMUM_WAIT_OBJECTS=64!			***218
typedef struct _CONTROL_BLOCK {
	PVOID	hFile;					//a pointer to an adapter object (not a file handle)
	HANDLE	hEvent;					//not used
	TCHAR	AdapterName[64];		//the UNICODE NT adapter name \Device\bacpac1
	TCHAR	MacDriverName[64];		//the UNICODE NT MAC name \Device\DC21X41 (eg.) ***001
//																		***110 Begin
	HGLOBAL	hMem[MAXREADREQS];		//handle to an allocated block of memory (for RX)***218
	LPBYTE	lpMem[MAXREADREQS];		//pointer to the memory block					***218
//																		***110 End
	HGLOBAL	hMem2;					//handle to an allocated block of memory (for TX)
	LPBYTE	lpMem2;					//pointer to the memory block
	ULONG	PacketLength;			//the real length of a received packet

	ULONG	LastReadSize;			//not used
	UINT	BufferSize;				//the max size of a network packet (1514)
	} CONTROL_BLOCK,*PCONTROL_BLOCK;

//MACAPI Errors
#define	MAE_Success 	 			  0
#define	MAE_NoFreeBuffers 			 -1
#define MAE_InvalidPort				 -2
#define	MAE_OpenAdapterFailed		 -3
#define	MAE_LSLNotFound 			 -4
#define	MAE_InvalidService	 		 -5
#define	MAE_CantGetSelector 		 -6
#define MAE_InvalidPointer			 -7
#define	MAE_LSAPinUse 				 -8
#define	MAE_NotYetInitialized 		 -9
#define MAE_CreateThreadFailed		-10								//***140

__declspec(dllexport) word MACPInit(void);
__declspec(dllexport) void MACPDeinit(void);
__declspec(dllexport) BOOL MACPreceive(MApacket *);
__declspec(dllexport) word MACPgetlen(word);
__declspec(dllexport) word MACPgetaddr(word,octet *);
__declspec(dllexport) void MACPtransmit(MApacket *);
__declspec(dllexport) word MACPgetportlist(char *);
__declspec(dllexport) word MACPmaxNPDUlength(word);
__declspec(dllexport)void MACPgetstats(MACStatTable *);
#endif //__MACP_H_INCLUDED
