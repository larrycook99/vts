//Contents of SJACK32.H
#ifndef __SJACK32_H_INCLUDED
#define __SJACK32_H_INCLUDED

typedef unsigned char byte;
typedef unsigned long dword;
typedef unsigned short word;

#define true				1
#define false				0

#define maxrcvqsize			2048				//circular receive queue size

typedef struct {
	byte	comport;							//1-4, 0=not used
	DCB		dcb;
	HANDLE	handle;								//port's driver handle
	OVERLAPPED ovTX;							//transmit semaphore
	int		rcvf;								//circular buffer front index
	int		rcvr;								//circular buffer's rear index
	int		rcvcount;							//count of bytes in circular queue
	short	rcv[maxrcvqsize];					//circular receive buffer
												//MS byte=0 if no error
												//MS byte=1 if framing, =2 if parity =3 if overrun
	} SJACKport;

#define	SJACK_Success					 0		//error codes
#define SJACK_InvalidParameter			-1
#define SJACK_AlreadyBound				-2
#define SJACK_CantCreateThread			-3
#define SJACK_BindingFailed				-4
#define SJACK_InvalidPort				-5
#define SJACK_CantCreateFile			-6
#define SJACK_TransmitterBusy			-7
#define	SJACK_TransmitError				-8

__declspec(dllexport) int SJACKinit(word);				//				***002 Begin
__declspec(dllexport) int SJACKbind(word,char *);
__declspec(dllexport) void SJACKdeinit(void);
__declspec(dllexport) int SJACKrx(word);
__declspec(dllexport) int SJACKtx(word,byte *,dword);	//				***002 End
__declspec(dllexport) int SJACKtxchar(word,byte);		//				***006
#endif //__SJACK32_H_INCLUDED
