//Contents of NALSAPI.H
#ifndef __NALSAPI_H_INCLUDED
#define __NALSAPI_H_INCLUDED

#define	NALS_MAX_STATUS		100					//max entries in status log
#define	Min_NPDU_Length		2
#define	BroadcastDNET		0xFFFF

typedef unsigned char byte;
typedef unsigned char octet;
typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned short uint;
typedef int		    ALTYPE;
#define CLIENT		    0
#define SERVER		    1

enum	{	Who_Is_Router_To_Network,			//0
			I_Am_Router_To_Network,				//1
			I_Could_Be_Router_To_Network,		//2
			Reject_Message_To_Network,			//3                                                           
			Router_Busy_To_Network,				//4
			Router_Available_To_Network,		//5
			Initialize_Routing_Table,			//6
			Initialize_Routing_Table_Ack,		//7
			Establish_Connection_To_Network,	//8
			Disconnect_Connection_To_Network	//9
		} BACnetNLMessageType;

		
enum	{	NORMAL_MESSAGE,
			URGENT_MESSAGE,
			CRITICAL_EQUIPMENT_MESSAGE,
			LIFE_SAFETY_MESSAGE
		} NALSpriority;

typedef struct {	//NPCI
	octet	npciVersion;
	octet	npciControl;
	uint	npciDNET;
	octet	npciDLEN;
	octet	npciDADR[8];
	} NPCI;
	
typedef struct {	//NSRC
	uint	SNET;
	octet	SLEN;
	octet	SADR[8];
	} NSRC;
	

#define	NSDUisNLmessage			0x80			//1==NL message, 0=APDU
//								0x40			//reserved
#define NSDUhasDNET				0x20			//1==DNET, DLEN, HopCount present
//								0x10			//reserved
#define NSDUhasSNET				0x08			//1==SNET, SLEN present
#define NSDUexpectsreply		0x04
#define NSDUpriority			0x03			//mask for priority field

typedef struct {	//ASDU
	octet	asduPDUheader;
	octet	asduInvokeID;
	octet	asduSequenceNumber;					//ComplexACK and SegmentedACK only
	octet	asduWindowSize;						//ComplexACK (ProposedWindowSize) and SegmnetedACK(ActualWindowSize) only
	octet	asduServiceAckChoice; 				//ComplexACK
	} ASDU;
	
typedef struct {	//ASDU for Confirmed Requests
	octet	asdureqPDUheader;
	octet	asdureqMaxResponse;
	octet	asdureqInvokeID;
	octet	asdureqSequenceNumber;
	octet	asdureqProposedWindowSize;
	octet	asdureqServiceChoice;
	} ASDUREQ;

#define	ASDUpdutype				0xF0			//mask for pdu type field
#define	ASDUpdutypeshr			4				//shift right this many times to get it
#define	ASDUissegmented			0x08			//all segmented types have this bit set
#define ASDUmoresegments		0x04			//1=more segments
#define ASDUsegmentrespaccepted 0x02			//1=segmented responses accepted
#define ASDUnak					0x02			//1=segmented NAK, 0=segmented ACK
#define ASDUserver				0x01			//1=from a server, 0=from a client
#define ASDUMaxResp				0x0F			//mask for max response
typedef struct {	//NALSstats
	dword	nalstAPDUcount;
	dword	nalstNPDUcount;
	} NALSstats;
	
/*typedef struct tagMApacket	{
struct  tagMApacket *maLink;					//linked list pointer		***140
dword	maTrace;								//packet sequence identifier
octet	maLport;								//the logical port
octet	maAlen;									//the length of sadr and dadr really used
octet	maLsap;									//the link service access point
octet	maBroadcast;							//0=no, 1=yes
octet	maReserved[11];							//reserved							***WIN95
octet	maDadr[8];								//octet string destination address
octet	maSadr[8];								//octet string source address
octet	mapad1;									//									***WIN95
word	maNlen;									//length of maNPDU in octets
octet	maNPDU[1497];							//max sized NPDU
octet	mapad2;									//									***WIN95
				} MApacket;
*/
typedef struct tagNALSpacket {	
	struct  tagNALSpacket far *nalsNext;		//linked list pointer
	BOOL	nalsExpectReply;
	BOOL	nalsRequestorisserver;				//TRUE if the requestor is a server;FALSE if client
	BOOL	nalsBroadcast;						//moved									***WIN95
	octet	nalsRxType;							//0=NL packet,1=broadcast or this node AL packet,2=other unsegmented packet
	octet	nalsInvokeID;						//invoke id assigned to request/from reply
	word	nalsRxFlag;							//a NALSAPI success/error code
	word	nalsPri;							//use NALSpriority  here				***WIN95
	word	nalsDnet;							//the destination network				***WIN95
	octet	nalsDlen;							//the length of dadr really used		
	octet	nalsDadr[8];						//octet string destination address
	octet	nalsRlen;							//the length of radr really used
	octet	nalsRadr[8];						//octet string router address
	word	nalsSnet;							//										***WIN95
	octet	nalsSlen;
	octet	nalsSadr[8];						//octet string source address
	octet	nalsLport;							//the logical port (moved)		//***WIN95						***WIN95
	word	nalsAlen;							//length of APDU in octets
	octet far *nalsAPDU;						//points to APDU
	} NALSpacket;

//!! CAUTION: Both BOOLs and enums are 32-bits
typedef struct {	//CLIENTid					//when we're a client
	byte	cidState;
	BOOL	cidRequestorisserver;				//TRUE if the requestor is a server;FALSE if client
	word	cidOctetCount;
	octet far *cidTxPacket;						//points to a tx packet
	word	cidNPCI_APCILen;						//so we can find where to put data in tx packet
	octet far *cidAPDUData;						//points to APDU data
	octet far *cidAPDUPacket;					//points to entire APDU packet received from A.L.
	word	cidMaxLen;							//maximum size of an APDU
	octet	cidWindowSize;
	octet	cidActualWindowSize;
	word	cidTimer;							//in 100ms increments
	uint	cidTries;							//Total number of times to try
	octet	cidSequence;
	octet	cidInitialSequence;
	BOOL	cidMore;							//TRUE = More
	uint	cidDnet;
	octet	cidDlen;
	octet	cidDadr[8];							//octet string destination address
	octet	cidLport;
	} CLIENTid;
	
typedef struct	{	//SERVERid					//when we're a server
	byte	sidState;
	octet	sidInvokeID;						//invoke ID from client
	word	sidOctetCount;
	octet far *sidTxPacket;						//points to a tx packet
	word	sidNPCI_APCILen;						//so we can find where to put data in tx packet
	octet far *sidAPDUData;						//points to APDU data
	octet far *sidAPDUPacket;					//points to entire APDU packet received from A.L.
	word	sidMaxLen;							//maximum size of an APDU
	octet	sidWindowSize;
	octet	sidActualWindowSize;
	word	sidTimer;							//in 100ms increments
	uint	sidTries;							//Total number of times to try
	octet	sidSequence;
	octet	sidInitialSequence;
	BOOL	sidMore;							//TRUE = More
	uint	sidSnet;
	octet	sidSlen;
	octet	sidSadr[8];							//octet string source address
	octet	sidLport;
	octet	sidMaxResponse;						//max response
	BOOL	sidSegmentAccepted;					//segmented responses accepted	
	} SERVERid;
	
enum	{	stateFree,							//0 Free/Idle
			stateSegmentedRequest,				//1 Client - Sending a Segmented Confirmed Request
												//  Server - Receiving a Segmented Confirmed Request
			stateAwaitConf,						//2 Client - Waiting for Confirmation to Confirmed Request
			stateSegmentedConf,					//3 Client - Receiving a Segmented Complex Ack
			stateSegmentedResponse,				//4 Server - Sending a Segmented Complex Ack
			stateAwaitResponse					//5 Server - Waiting for response from Application
		} BACnetStateMachineStates;
		 

typedef struct {	//NALSpeers
	octet	nalspLport;							//logical port
	uint	nalspNet;							//network
	octet	nalspAlen;							//length of address
	octet	nalspAdr[8];						//address
	octet	nalspRlen;							//length of Radr			***170
	octet	nalspRadr[8];						//routing info				***170
	dword	nalspInstance;						//device instance
	uint	nalspMaxapdulen;					//maximum APDU length supported
	octet	nalspSegsupported;					//segmentation supported
	uint	nalspVendorid;    					//vendor ID
		} NALSpeers;

/*typedef struct {	//NALSlboards
	octet	nalslLboard;						//logical board (index into Adapter[x])
	word	nalslAlen;							//length of address
	octet	nalslAdr[8];						//address
	dword	nalslAdapterType;					//0=Ethernet, 1=ARCNET	***200
	} NALSlboards;
*/
//NALSAPI errors
#define	MAE_Success						0
#define	NALSE_Success					0
#define	NALSE_NoFreeTimers				1000
#define	NALSE_CantCreateWindow			1001
#define	NALSE_FailedClassRegistration	1002
#define NALSE_NLserverAlreadyRegistered	1003
#define NALSE_ALserverAlreadyRegistered	1004
#define	NALSE_MACPNotInstalled			1005		//		***100
#define	NALSE_UnknownPDUType			1006
#define	NALSE_InvalidPDUType			1007
#define	NALSE_InvalidRequestThisState 	1008
#define	NALSE_NoFreeSpaceAvailable		1009
#define NALSE_NoFreeInvokeIDs			1010
#define	NALSE_Timeout					1011
#define NALSE_ReceivedPacketTooLarge	1012
#define NALSE_TransmitError				1013
#define NALSE_InvalidDestinationAddr	1014
#define	NALSE_InvalidResponse			1015
#define NALSE_AbortReceived				1016
#define NALSE_RejectReceived			1017
#define NALSE_ErrorReceived				1018
#define NALSE_NoMoreSegments			1019
#define NALSE_CannotSendSegmentedCACK	1020 
#define NALSE_TransmitPacketTooLarge	1021
#define NALSE_NoPacketsAvailable		1022
#define NALSE_InvalidPort				1023
#define NALSE_InhibitTX					1998		//		***165

int  APIENTRY NALSAPIinit(word *);
void APIENTRY NALSAPIdeinit(void);
void APIENTRY NALSAPIgetstatistics(NALSstats *);
BOOL APIENTRY NALSAPIgetlog(char *);
void APIENTRY NALSAPIputlog(char *);
void APIENTRY NALSAPIreceivefilter(BOOL);
BOOL APIENTRY NALSAPIgetpacket(NALSpacket *);
int  APIENTRY NALSAPIgetapdu(NALSpacket *,word,octet *);
int  APIENTRY NALSAPIrequest(NALSpacket *);
int  APIENTRY NALSAPIcopyrequest(NALSpacket *,octet *);
int  APIENTRY NALSAPItransmit(MApacket *);
int  APIENTRY NALSAPIregisterNLserver(FARPROC);
int  APIENTRY NALSAPIregisterALserver(FARPROC);
word APIENTRY NALSAPIgetportlist(char *);
word APIENTRY NALSAPIgetaddr(word,octet *);
word APIENTRY NALSAPIgetlen(word);
word APIENTRY NALSAPIgetmaxresponse(word);
int  APIENTRY NALSAPItransmitIam(octet,dword,octet *,octet);
int  APIENTRY NALSAPItransmitWhois(octet,dword,octet *,octet,word,dword,dword);
int  APIENTRY NALSAPItransmitTimeSynch(octet,dword,octet *,octet alen);
word APIENTRY NALSAPImaxNPDUlength(word);
void APIENTRY NALSAPIanytobyte(void *,byte *,word);
void APIENTRY NALSAPIbytetoany(byte *,void *,word);

#endif //__NALSAPI_H_INCLUDED
