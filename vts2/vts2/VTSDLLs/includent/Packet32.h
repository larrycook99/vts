
#ifndef __PACKET32
#define __PACKET32

#define	DOSNAMEPREFIX   TEXT("Packet_")
#define drivername TEXT("BACMACNT")			//			***001

#define MAX_LINK_NAME_LENGTH   64
//														***004 Begin
#define ETH_802_3_ADDRESS_LENGTH	6
#define ARCNET_ADDRESS_LENGTH	1

#define		ETH_802_3_ADAPTER			0
#define		ARCNET_ADAPTER				1
//														***004 End
typedef struct _ADAPTER
{	HANDLE     hFile;
    TCHAR      SymbolicLink[MAX_LINK_NAME_LENGTH];
}ADAPTER, *LPADAPTER;

typedef struct _PACKET
{
	HANDLE       hEvent;				//this does not seem to be used!
	OVERLAPPED   OverLapped;
	PVOID        Buffer;
	UINT         Length;
}PACKET, *LPPACKET;

enum {export,xbind} DEVICETYPES;

__declspec(dllexport) PVOID Packet32OpenAdapter(LPTSTR);
__declspec(dllexport) BOOLEAN Packet32SendPacket(LPADAPTER,LPPACKET,BOOLEAN);
__declspec(dllexport) PVOID Packet32AllocatePacket(LPADAPTER,BOOLEAN);				//***002
__declspec(dllexport) VOID Packet32InitPacket(LPPACKET,PVOID,UINT);
__declspec(dllexport) VOID Packet32FreePacket(LPPACKET);
__declspec(dllexport) BOOLEAN Packet32ResetAdapter(LPADAPTER);
//__declspec(dllexport) WORD Packet32GetAddress(LPTSTR,PBYTE);			***004
__declspec(dllexport) WORD Packet32GetAddress(LPADAPTER,PBYTE);	//		***004
__declspec(dllexport) WORD Packet32GetMaxFrameSize(LPADAPTER);	//		***004
__declspec(dllexport) DWORD Packet32WaitPacket(LPADAPTER,LPPACKET,PULONG,BOOLEAN);	//***002
__declspec(dllexport) BOOLEAN Packet32ReceivePacket(LPADAPTER,LPPACKET,BOOLEAN,PULONG);
__declspec(dllexport) VOID Packet32CloseAdapter(LPADAPTER);
__declspec(dllexport) BOOLEAN Packet32SetFilter(LPADAPTER,ULONG);
//__declspec(dllexport) ULONG Packet32GetAdapterNames(PTSTR,PULONG);
__declspec(dllexport) ULONG Packet32GetDeviceNames(PTSTR,PULONG,enum DEVICETYPES);
__declspec(dllexport) void *mymalloc(size_t);					//		***214
__declspec(dllexport) void myfree(void *);						//		***214
__declspec(dllexport) size_t mysize(void *);						//		***214***251

#endif
