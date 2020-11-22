//MACP.H Header file
#ifndef __BACIOCTL_H_INCLUDED
#define __BACIOCTL_H_INCLUDED

#define OID_BACMAC_GET_PERF_DATA	OID_GEN_RCV_NO_BUFFER+1

typedef struct _PERF_DATA{
	ULONG		RxCount;					//the count of BACnet packets
	ULONG		TxCount;					//the count of BACnet packets (may include Broadcast echos)
	ULONG		RxNonBACnet;				//tossed non-BACnet
	ULONG		RxNoUserBuff;				//tossed because we had no place to put them
	ULONG		RxNoNDISBuff;				//tossed because of no NDIS resources
	ULONG		TxNoNDISBuff;				//ditto
} PERF_DATA, *PPERF_DATA;
#endif //__BACIOCTL_H_INCLUDED
