//Contents of PTP.H
#ifndef __PTP_H_INCLUDED
#define __PTP_H_INCLUDED

#ifndef byte
typedef unsigned char byte;
#endif
#ifndef octet
typedef unsigned char octet;
#endif
#ifndef dword
typedef unsigned long dword;
#endif
#ifndef word
typedef unsigned short word;
#endif
#ifndef false
#define false		FALSE
#endif
#ifndef true
#define true		TRUE
#endif

typedef struct pmpkt{		//mpkt
	struct pmpkt	*next;
	byte            frametype; //PTP frame type.Xiaoshiyuan 2002-5-16	
	word			length;
	byte            headerCRC;	//PTP frame header CRC.Xiaoshiyuan 2002-5-16	
	octet			buf[512];
    word            dataCRC; //PTP frame data CRC.Xiaoshiyuan 2002-5-16	
	} mpkt;

//PTP packet
//Xiao Shiyuan 2002-5-16
typedef struct tagPKTPTP {	
	unsigned short  pktPreamble;
	unsigned char   pktFrameType;	//frame type
	unsigned short  pktLen;		
	unsigned char   pktHeaderCRC;	//Header CRC
	unsigned char   pktData[512];
	unsigned short  pktDataCRC; 
	} PKTPTP;

typedef struct {			//pktq
	mpkt	*front;
	mpkt	*rear;
	int		count;
	} pktq;

#define		ptpport					10				//our pseudo port number

//PTP frame types
#define	ftHeartbeatXOFF				0x00
#define	ftHeartbeatXON				0x01
#define	ftData0						0x02
#define	ftData1						0x03
#define	ftDataAck0XOFF				0x04
#define	ftDataAck1XOFF				0x05
#define	ftDataAck0XON				0x06
#define	ftDataAck1XON				0x07
#define	ftDataNak0XOFF				0x08
#define	ftDataNak1XOFF				0x09
#define	ftDataNak0XON				0x0A
#define	ftDataNak1XON				0x0B
#define	ftConnectRequest			0x0C
#define	ftConnectResponse			0x0D
#define	ftDisconnectRequest			0x0E
#define	ftDisconnectResponse		0x0F
#define	ftTestRequest				0x14
#define	ftTestResponse				0x15

//state flags
#define ptpERSuccess					0
#define ptpERFailedClassRegistration	-200
#define	ptpERCantCreateWindow			-201
#define	ptpERNoFreeTimers				-202
#define	ptpERNotDisconnected			-203
#define	ptpERPasswordTooLong			-204
#define	ptpERNotConnected				-205
#define ptpERNoFreeBuffers				-206
#define ptpERTransmitRequestTooLarge	-207
#define	ptpERSjack						-300

//put function prototypes here
AFX_EXT_API int  ptpInit(const char *config);
AFX_EXT_API void ptpDeinit(void);
AFX_EXT_API int  ptpConnect(char *);
AFX_EXT_API int  ptpDisconnect(octet);
AFX_EXT_API int  ptpTransmit(byte *, int len);
AFX_EXT_API BOOL ptpReceive(PKTPTP*);
AFX_EXT_API void ptpCatName(char *);
AFX_EXT_API int  ptpGetFrame(PKTPTP pkt, byte *framebuf, int buflen, int *plen);
AFX_EXT_API int  ptpNPDUtoFrame(byte *npdu, int npdulen, byte *framebuf, int buflen, int *plen);

#endif //__PTP_H_INCLUDED
