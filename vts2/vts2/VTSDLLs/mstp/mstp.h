//Contents of MSTP.H
#ifndef __MSTP_H_INCLUDED
#define __MSTP_H_INCLUDED

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

#define NPDUexpectsreply	4			//bit pattern for NPDU[1]

typedef struct pmmpkt{		//mmpkt
	struct pmmpkt	*next;
	octet			frametype;
	octet			destaddr;
	octet			sourceaddr;			//not required for outgoing
	int				length;
	octet			buf[512];
	} mmpkt;

typedef struct {			//pq
	mmpkt	*front;
	mmpkt	*rear;
	int		count;
	} pq;

#define		mstpport				11				//our pseudo port number

//MSTP frame types
#define	mftToken						0x00
#define	mftPollForMaster				0x01
#define	mftReplyToPollForMaster			0x02
#define	mftTestRequest					0x03
#define	mftTestResponse					0x04
#define	mftBACnetDataExpectingReply		0x05
#define	mftBACnetDataNotExpectingReply	0x06
#define	mftReplyPostponed				0x07
#define mftUnknown						0x08

#define mstpERSuccess					0
#define mstpERFailedClassRegistration	-200
#define	mstpERCantCreateWindow			-201
#define	mstpERCantCreateTimer			-202
#define mstpERNoFreeBuffers				-203
#define mstpERTransmitRequestTooLarge	-204
#define	mstpERSjack						-300

#ifdef __cplusplus					//so this header can be used with C++
extern "C" {
#endif

//put function prototypes here
int		APIENTRY	mstpInit(void);
void	APIENTRY	mstpDeinit(void);
int		APIENTRY	mstpTransmit(MApacket *);
int		APIENTRY	mstpTransmitTest(MApacket *);
BOOL	APIENTRY	mstpReceive(MApacket *);
void	APIENTRY	mstpCatName(char *);
void	APIENTRY	mstpVarMaxInfoFrames(int *,BOOL);
void	APIENTRY	mstpVarMaxMaster(int *,BOOL);
byte	APIENTRY	mstpGetTS(void);

#ifdef __cplusplus					//end of extern "C" declarations
}
#endif
#endif //__MSTP_H_INCLUDED
