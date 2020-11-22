#ifndef __PACKET32
#define __PACKET32

#include <bacoid.h>					//									***010

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif /* __cplusplus */


#define     DOSNAMEPREFIX   TEXT("Packet_")

#define     MAX_LINK_NAME_LENGTH		64

#define		ETH_802_3_HEADER_LENGTH		14
#define		ETH_802_3_ADDRESS_LENGTH	6
//																		***005 Begin
#define		ARCNET_ADDRESS_LENGTH		1
#define		ARCNET_HEADER_LENGTH		3

#define		ETH_802_3_ADAPTER			0
#define		ARCNET_ADAPTER				1
//																		***005 End

typedef struct _PACKET					//moved							***006 
{
    HANDLE       hEvent;
    OVERLAPPED   OverLapped;
    PVOID        Buffer;
    UINT         Length;
	ULONG		 ulBytesReceived;
	BOOLEAN		 bIoComplete;
} 
PACKET, *LPPACKET;

#define		MAXADAPTERS					2				//				***006***011
#define		MAXREADREQS					16				//				***006***010
typedef struct _ADAPTER 
{
    HANDLE		hFile;

    TCHAR		SymbolicLink[ MAX_LINK_NAME_LENGTH ];
	TCHAR		CurrentAddress[ ETH_802_3_ADDRESS_LENGTH ];
	TCHAR		PermanentAddress[ ETH_802_3_ADDRESS_LENGTH ];
	WORD		AddressLength;					//						***006
	ULONG		ulMaxFrameSize;
	ULONG		ulMaxLookAheadSize;
	ULONG		ulGenMacOptions;				//						***003
	ULONG		ulAdapterType;					//						***005
	LPPACKET	PacketRX[MAXREADREQS];			//						***006 Begin
	LPPACKET	PacketTX;						//
	DWORD		commEnableTimer;				//a timer for re-enabling communications (in seconds)
	BOOL		commEnabled;					//true if communications are enabled
//																		***006 End

} 
ADAPTER, *LPADAPTER;


typedef struct _ETH_802_3_FRAME
{
	BYTE	DestinationAddress[ ETH_802_3_ADDRESS_LENGTH ];
	BYTE	SourceAddress[ ETH_802_3_ADDRESS_LENGTH ];
	BYTE	LengthCount[2];
	BYTE	PduBuffer[1];
}
ETH_802_3_FRAME, *PETH_802_3_FRAME;



LPADAPTER APIENTRY Packet32OpenAdapter(LPTSTR);
BOOLEAN APIENTRY Packet32SendPacket(LPADAPTER ,LPPACKET,BOOLEAN );
LPPACKET APIENTRY Packet32AllocatePacket(LPADAPTER,BOOLEAN);			//		***004
BOOLEAN APIENTRY Packet32InitPacket(LPPACKET,UINT);
VOID APIENTRY Packet32FreePacket(LPPACKET);
BOOLEAN APIENTRY Packet32ResetAdapter(LPADAPTER);
WORD APIENTRY Packet32GetAddress(LPADAPTER,PBYTE);						//		***006
BOOLEAN APIENTRY Packet32DeviceIoControl(LPADAPTER,LPPACKET,ULONG,BOOLEAN);
BOOLEAN APIENTRY Packet32Request(LPADAPTER,LPPACKET,BOOLEAN);
DWORD APIENTRY Packet32WaitPacket(LPADAPTER,LPPACKET,PULONG,BOOLEAN);	//		***002***004
BOOLEAN APIENTRY Packet32ReceivePacket(LPADAPTER,LPPACKET,BOOLEAN);
VOID APIENTRY Packet32CloseAdapter(LPADAPTER);
BOOLEAN APIENTRY Packet32SetFilter(LPADAPTER,ULONG );
BOOLEAN APIENTRY Packet32GetAdapterNames(LPADAPTER,LPPACKET);
ULONG APIENTRY Packet32GetRxCount(LPADAPTER);							//		***010
ULONG APIENTRY Packet32GetTxCount(LPADAPTER);							//		***010
VOID APIENTRY Packet32GetPerfStats(LPADAPTER,PERF_DATA *);				//		***010
VOID *APIENTRY mymalloc(size_t);
VOID APIENTRY myfree(void *p);
size_t APIENTRY mysize(void *);											//		***013

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __PACKET32 */
