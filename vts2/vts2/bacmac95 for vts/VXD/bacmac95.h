//include file for bacmac95
// 07-Oct-99 [004] Jack Neyer -- bump up MAX_REQUESTS (from 4)
// 24-Sep-99 [003] Jack Neyer -- add diagnostic stats, bump up TRANSMIT_PACKETS
// 01-Oct-98 [002] Jack Neyer -- support for multiple adapters 
// 15-Dec-97 [001] William Ingle, Jack Neyer

#define  MAX_REQUESTS   32							//***004

#ifdef DEBUG
#define	PACKETASSERT(a)		if( !(a) ) { DbgPrint( "Packet: ASSERTION FAILED!\r\n" ); DbgBreakPoint(); }
#else
#define PACKETASSERT(a)
#endif

typedef struct _PACKET_RESERVED 
{	LIST_ENTRY	ListElement;

	char*		lpBuffer;
	DWORD		cbBuffer;
	DWORD*		lpcbBytesReturned;
	OVERLAPPED*	lpoOverlapped;
	DWORD		hDevice;
	DWORD		tagProcess;
}  
PACKET_RESERVED, *PPACKET_RESERVED;


typedef struct _INTERNAL_REQUEST 
{	PACKET_RESERVED Reserved;
	NDIS_REQUEST	 Request;
} 
INTERNAL_REQUEST, *PINTERNAL_REQUEST;

// Port device extension.

typedef struct _OPEN_INSTANCE 
{	LIST_ENTRY			ListElement;

	NDIS_STATUS			Status;
	NDIS_HANDLE			AdapterHandle;
	NDIS_HANDLE			PacketPool;
	NDIS_HANDLE			BufferPool;
	NDIS_HANDLE			BindAdapterContext;
	NDIS_SPIN_LOCK		RcvQSpinLock;
	LIST_ENTRY			RcvList;
	NDIS_SPIN_LOCK		RequestSpinLock;
	LIST_ENTRY			RequestList;
	NDIS_SPIN_LOCK		ResetSpinLock;
	LIST_ENTRY			ResetIrpList;
	INTERNAL_REQUEST  Requests[MAX_REQUESTS];
	NDIS_MEDIUM			NdisMedium;					//the underlying NIC medium		***002
	ULONG				RxCount;					//the count of BACnet packets ***003
	ULONG				TxCount;					//the count of BACnet packets (may include Broadcast echos) ***003
	ULONG				RxNonBACnet;				//tossed non-BACnet			***004 Begin
	ULONG				RxNoUserBuff;				//tossed because we had no place to put them
	ULONG				RxNoNDISBuff;				//tossed because of no NDIS resources
	ULONG				TxNoNDISBuff;				//ditto						***004 End
} 
OPEN_INSTANCE, *POPEN_INSTANCE;

#define MAX_DEVICES		16							//number of bindings									***002

typedef struct _DEVICE_EXTENSION 
{	PDRIVER_OBJECT		DriverObject;
	NDIS_HANDLE			NdisProtocolHandle;
	LIST_ENTRY			OpenList;
	DWORD				hDevice[MAX_DEVICES];		//device handles bound to protocol ***002
	WORD				AdapterIndex[MAX_DEVICES];	//the corresponding adapter index	***002
} 
DEVICE_EXTENSION, *PDEVICE_EXTENSION;

extern PDEVICE_EXTENSION GlobalDeviceExtension;

#define  ETHERNET_HEADER_LENGTH   14
#define RESERVED(_p) ((PPACKET_RESERVED)((_p)->ProtocolReserved))
#define  TRANSMIT_PACKETS    32		//(actually read/write packets per binding)	//***003

extern void YieldExecution( void );

PLIST_ENTRY PacketRemoveHeadList(IN PLIST_ENTRY pListHead);

VOID NDIS_API PacketBindAdapterComplete(
								IN NDIS_HANDLE  ProtocolBindingContext,
								IN NDIS_STATUS  Status,
								IN NDIS_STATUS  OpenErrorStatus);

VOID NDIS_API PacketUnbindAdapterComplete(
								IN NDIS_HANDLE  ProtocolBindingContext,
								IN NDIS_STATUS  Status);

NDIS_STATUS NDIS_API PacketReceiveIndicate(
								IN NDIS_HANDLE ProtocolBindingContext,
								IN NDIS_HANDLE MacReceiveContext,
								IN PVOID HeaderBuffer,
								IN UINT HeaderBufferSize,
								IN PVOID LookAheadBuffer,
								IN UINT LookaheadBufferSize,
								IN UINT PacketSize);

VOID NDIS_API PacketReceiveComplete(
								IN NDIS_HANDLE  ProtocolBindingContext);

DWORD PacketRequest(POPEN_INSTANCE		pOpen,
					DWORD  				FunctionCode,
					DWORD  				dwDDB,
					DWORD					hDevice,
					PDIOCPARAMETERS 	pDiocParms);

VOID NDIS_API PacketRequestComplete(
						IN NDIS_HANDLE   ProtocolBindingContext,
						IN PNDIS_REQUEST pRequest,
						IN NDIS_STATUS   Status);

VOID NDIS_API PacketSendComplete(
						IN NDIS_HANDLE   ProtocolBindingContext,
						IN PNDIS_PACKET  pPacket,
						IN NDIS_STATUS   Status);

VOID PacketReset(PNDIS_STATUS	pStatus,
				 POPEN_INSTANCE	pOpen );

VOID NDIS_API PacketResetComplete(
						IN NDIS_HANDLE  ProtocolBindingContext,
						IN NDIS_STATUS  Status);

VOID NDIS_API PacketStatus(
						IN NDIS_HANDLE   ProtocolBindingContext,
						IN NDIS_STATUS   Status,
						IN PVOID         StatusBuffer,
						IN UINT          StatusBufferSize);


VOID NDIS_API PacketStatusComplete(IN NDIS_HANDLE ProtocolBindingContext);

VOID PacketAllocatePacketBuffer( PNDIS_STATUS	pStatus,
								 POPEN_INSTANCE	pOpen,
								 PNDIS_PACKET	*lplpPacket,
								 PDIOCPARAMETERS pDiocParms,
								 DWORD			FunctionCode);

VOID NDIS_API PacketTransferDataComplete(
								IN NDIS_HANDLE	ProtocolBindingContext,
								IN PNDIS_PACKET	Packet,
								IN NDIS_STATUS	Status,
								IN UINT 		BytesTransferred);

VOID PacketRemoveReference(IN PDEVICE_EXTENSION DeviceExtension);

VOID PacketFreeResources(POPEN_INSTANCE Open);

VOID PacketCleanUp(PNDIS_STATUS	Status,POPEN_INSTANCE Open);

NTSTATUS NDIS_API PacketShutdown(IN PDEVICE_OBJECT DeviceObject);

VOID NDIS_API PacketUnload();

VOID NDIS_API PacketBindAdapter(OUT PNDIS_STATUS Status,
								IN  NDIS_HANDLE  BindAdapterContext,
								IN  PNDIS_STRING AdapterName,
								IN  PVOID        SystemSpecific1,
								IN  PVOID        SystemSpecific2);

VOID NDIS_API PacketUnbindAdapter(
								OUT PNDIS_STATUS	Status,
								IN NDIS_HANDLE	ProtocolBindingContext,
								IN NDIS_HANDLE	UnbindContext);

DWORD PacketWrite(	POPEN_INSTANCE	Open,
					DWORD  			dwDDB,
					DWORD  			hDevice,
					PDIOCPARAMETERS	pDiocParms);

DWORD PacketRead(	POPEN_INSTANCE	Open,
					DWORD  				dwDDB,
					DWORD  				hDevice,
					PDIOCPARAMETERS   pDiocParms);

DWORD _stdcall PacketIOControl(	DWORD  			dwService,
                                DWORD  			dwDDB,
                                DWORD  			hDevice,
                                PDIOCPARAMETERS lpDIOCParms );

// define wrapper for VWIN32_DIOCCompletionRoutine 

void VXDINLINE VWIN32_DIOCCompletionRoutine(DWORD hEvent)
{	_asm mov ebx, hEvent
	VxDCall( VWIN32_DIOCCompletionRoutine);
}

ULONG strlen(BYTE *s);

#define strcat(d, s)	NdisMoveMemory( d+strlen(d), s, strlen(s) )

// where is memset defined?  okay, let's make one!
//

#define memset( _S, _C, _N )\
{\
	UCHAR* _pS = _S;\
	ULONG  _I  = _N;\
	while ( _I-- )\
	{\
		*_pS++ = _C;\
	}\
}
//														***PSDBG Begin
#define DBGUART 	0x2F8			//com2

//UART registers

#define	THR         DBGUART + 0
#define	RBR         DBGUART + 0
#define	DLL         DBGUART + 0
#define	DLM         DBGUART + 1
#define	IER         DBGUART + 1
#define	FCR         DBGUART + 2
#define	LCR         DBGUART + 3
#define	MCR         DBGUART + 4
#define	LSR         DBGUART + 5

void psdInit();
//display a character on the debug port
void psdOutc(int);
//display a message
void psdPrint(char *);
//display a formatted message with an arg
void psdPrintf(char *,DWORD);
//														***PSDBG End
