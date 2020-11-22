
//
//	Script Prefix Keywords
//

#define		kwB						0x18C2602E		// binary string
#define		kwD						0x1982729C		// decimal string
#define		kwO						0x1DA2D7F9		// octal string
#define		kwX						0x21032AE8		// hex string

//
//	Script Language Statements
//

#define		kwSETUP					0xB855FCA7
#define		kwSECTION				0xEE506921
#define		kwTEST					0x844578B5
#define		kwDEPS					0x132136A2
#define		kwDEPENDENCIES			0xDCA1F3BD
#define		kwREF					0x068F23C6
#define		kwREFERENCE				0x0F4ABD9F
#define		kwSEND					0xB68B8DB5
#define		kwTRANSMIT				0x72CC912D
#define		kwAFTER					0xF34A1E11
#define		kwEXPECT				0x1D75F6B8
#define		kwRECEIVE				0x625A643F
#define		kwBEFORE				0x997A45A6
#define		kwAND					0xD31F7A67
#define		kwOR					0xC108A1DD
#define		kwCASE					0x688ED7CA
#define		kwENDCASE				0x011E6F49
#define		kwCOMMENT				0xCDAB06AF
#define     kwIFDEF					0xD8D27EFB	// the actual keyword is IF
#define		kwELSEDEF				0x7C1B022E	// the actual keyword is ELSE
#define     kwELSEIFDEF				0x7BC955DD	// the actual keyword is ELSEIF
#define     kwENDIF					0x6CD21759
#define		kwMAKE					0x06313E48
#define		kwCHECK					0x7EF5647B
#define		kwINCLUDE				0x30143b56

#define     kwNOT					0x7765B124	//Added by Zhu Zhenhua, 2003-11-25
#define     kwASSIGN				0x36B93DC9 //Added by Zhu Zhenhua, 2003-12-24, to ASSIGN statement
#define     kwWAIT					0x3918D2C0 //Added by Zhu Zhenhua, 2003-12-31, to WAIT statement
//
//	General Keywords
//

#define		kwNET					0x37C36CD0
#define		kwNETWORK				0xA9CCC570
#define		kwRESERVED				0xE015F73A
#define		kwVENDOR				0x5A09B083

//
//	DLC Keywords
//	

#define		kwDA					0xB7212EBE
#define		kwSA					0xBCC1B8F7
#define		kwLSAP					0x94698F0F

//
//	BPI Keywords
//

#define		kwBVLCI					0x6827092D

//
//	NL Keywords
//

#define		kwVERSION				0xEA103EDD
#define		kwDNET					0xB808E811
#define		kwDADR					0xA874851A
#define		kwSNET					0xBDA9724A
#define		kwSADR					0xAE150F53
#define		kwHOPCNT				0xB2D05614
#define		kwHOPCOUNT				0x9BD32ABB
#define		kwMSG					0xDDBC4744
#define		kwMESSAGE				0xF62C3D62
#define		kwPRIO					0xA405636D
#define		kwPRIORITY				0x45B0B875
#define		kwDER					0x9A7B9340
#define		kwVENDORID				0xEB537F5C
#define		kwNL					0xCDADA2DA
#define		kwNLDATA				0x73560E54

//
//	AL Keywords
//

#define		kwPDU					0xB21D6A51
#define		kwPDUTYPE				0xFC1A5A6F		// PDU-TYPE

#define		kwSEGMSG				0xa8B5AF39		// SEGMSG
#define		kwSEGMENTEDMESSAGE		0x2B55410F		// SEGMENTED-MESSAGE
#define		kwMOREFOLLOWS			0xE2F8750A		// MORE-FOLLOWS
#define		kwSEGRESP				0xEB08D39B		// SEGRESP
#define		kwSEGRESPACCEPTED		0x90EEB1F7		// SEGMENTED-RESPONSE-ACCEPTED
#define		kwMAXRESP				0xB520F7CE
#define		kwMAXRESPONSE			0x0BAA02D7
#define		kwMAXSIZE				0x73359A4D
#define		kwNEGATIVEACK			0xBE50150B		// NEGATIVE-ACK
#define		kwSERVER				0x36D6117C
#define		kwINVOKEID				0xD3D94F5D
#define		kwSEQUENCENR			0x11A73EAC
#define		kwWINDOWSIZE			0xF4A61B43
#define		kwAL					0xC8CD2B0F
#define		kwALDATA				0x6E759689
#define		kwSERVICE				0x5AFCB283
#define		kwERROR					0x71AD2A43
#define		kwERRORCHOICE			0xF59843C2		// ERROR-CHOICE
#define		kwREJECTREASON			0x7BBAF9E6		// REJECT-REASON
#define		kwABORTREASON			0x07930C3A		// ABORT-REASON
#define		kwMAXSEGS				0x05812D5D		// MAXSEGS


//#define		kwDEST					0x7E44E545
//#define		kwDESTINATION			0xE1A73108
//#define		kwROUTER				0x52B2A3B0
//#define		kwFLAGS					0x9D580D7C
//#define		kwPDUFLAGS				0xAAD15665		// PDU-FLAGS

//
//	Address Type Parameter Keywords
//

#define		kwUNKNOWN				0x77C61BB3
#define		kwBROADCAST				0xF35D58A4
#define		kwLOCALBROADCAST		0x396F4805		// LOCAL-BROADCAST
#define		kwREMOTEBROADCAST		0xCF252FF7		// REMOTE-BROADCAST
#define		kwGLOBAL_BROADCAST		0x5C235A3F		// GLOBAL-BROADCAST
#define		kwGLOBALBROADCAST		0x99C24AB5		// GLOBALBROADCAST

//
//	Network Type Parameter Keywords
//

#define		kwETHERNET				0xDF12BF76
#define		kwARCNET				0xAAB84F69
#define		kwMSTP					0x4750BFB5
#define		kwLONTALK				0x31D3B45E

//
//	Data Keywords
//

#define		kwOBJECT				0x2F07AAB4
#define		kwOBJECTIDENTIFIER		0x0E447381
#define		kwDEVICE				0xC6DBC30F
#define		kwDEVICEIDENTIFIER		0xA6188BDC
#define		kwPROPERTY				0x2384F5B6
#define		kwPROPERTYIDENTIFIER	0x51CFD715
#define		kwOPEN					0x456A459A
#define		kwOPENTAG				0x7E39D853
#define		kwOPENINGTAG			0x38EA9A85
#define		kwCLOSE					0xE492FAA3
#define		kwCLOSETAG				0xE6221E84
#define		kwCLOSINGTAG			0x7FA28D0F
#define		kwANY					0x1F2E1F40
#define		kwNULL					0x7484B078
#define		kwBOOL					0x63548837
#define		kwBOOLEAN				0x4B9EF04A
#define		kwUNSIGNED				0xB7B11F00
#define		kwUNSIGNED8				0x46447D48
#define		kwUNSIGNED16			0x9DEB695B
#define		kwUNSIGNED32			0xF1E2F211
#define		kwINTEGER				0xDC8E5F58
#define		kwREAL					0xF6FE696D
#define		kwSINGLE				0xF7A63EAB
#define		kwDOUBLE				0x6620A49D
#define		kwOCTETSTRING			0x3AC126B7
#define		kwCHARSTRING			0x798DEB65
#define		kwCHARACTERSTRING		0x86E99001
#define		kwBITSTRING				0x3848852A
#define		kwENUM					0xE2B9BAFC
#define		kwENUMERATED			0x413197CB
#define		kwDATE					0xB5B29FB6
#define		kwTIME					0x3BA79943

//
//	Translate Tables
//

extern ScriptTranslateTable ScriptBIPMsgTypeMap[];
extern ScriptTranslateTable ScriptNLMsgTypeMap[];
extern ScriptTranslateTable ScriptALMsgTypeMap[];
extern ScriptTranslateTable *ScriptALConfirmedServiceMap;
extern ScriptTranslateTable *ScriptALUnconfirmedServiceMap;
extern ScriptTranslateTable *ScriptALRejectReasonMap;
extern ScriptTranslateTable *ScriptALAbortReasonMap;
extern ScriptTranslateTable *ScriptObjectTypeMap;
extern ScriptTranslateTable ScriptALMap[];
extern ScriptTranslateTable ScriptBooleanMap[];
extern ScriptTranslateTable ScriptCharacterTypeMap[];
extern ScriptTranslateTable ScriptWeekdayMap[];
extern ScriptTranslateTable ScriptMonthMap[];
extern ScriptTranslateTable *ScriptPropertyMap;
extern ScriptTranslateTable ScriptNONEMap[];
extern ScriptTranslateTable *scriptStandardServicesMap;
extern ScriptTranslateTable scriptStandardServicesRequirementMap[];
extern ScriptTranslateTable scriptFailTimesMap[];


extern ScriptTranslateTable scriptGenKeywordMap[];
extern ScriptTranslateTable scriptDLCKeywordMap[];
extern ScriptTranslateTable scriptNLKeywordMap[];
extern ScriptTranslateTable scriptALKeywordMap[];
extern ScriptTranslateTable scriptDataKeywordMap[];

