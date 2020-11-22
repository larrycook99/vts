
#include "stdafx.h"
#include "VTS.h"

#include "ScriptBase.h"
#include "ScriptKeywords.h"

//  HashCode of string

//
//	BIP Message Types
//

ScriptTranslateTable ScriptBIPMsgTypeMap[] =
	{ { 0x7AD9DDA9,  0 }					// BVLC-RESULT
	, { 0xDB2C653F,  1 }					// WRITE-BROADCAST-DISTRIBUTION-TABLE
	, { 0xEABBFFDD,  1 }					// WRITE-BDT
	, { 0xBE1E123D,  2 }					// READ-BROADCAST-DISTRIBUTION-TABLE
	, { 0x22846274,  2 }					// READ-BDT
	, { 0xAB90D4E9,  3 }					// READ-BROADCAST-DISTRIBUTION-TABLE-ACK
	, { 0xAE10BC2C,  3 }					// READ-BDT-ACK
	, { 0xA34641EF,  4 }					// FORWARDED-NPDU
	, { 0xF3BF47A9,  5 }					// REGISTER-FOREIGN-DEVICE
	, { 0x44BFC4B4,  6 }					// READ-FOREIGN-DEVICE-TABLE
	, { 0xF2022D0C,  6 }					// READ-FDT
	, { 0x0ACBEA40,  7 }					// READ-FOREIGN-DEVICE-TABLE-ACK
	, { 0x7D8E86C4,  7 }					// READ-FDT-ACK
	, { 0x5365DF07,  8 }					// DELETE-FOREIGN-DEVICE-TABLE-ENTRY
	, { 0x9DBD6C6A,  8 }					// DELETE-FDT-ENTRY
	, { 0x2BCD74BF,  9 }					// DISTRIBUTE-BROADCAST-TO-NETWORK
	, { 0x5C99962D, 10 }					// ORIGINAL-UNICAST-NPDU
	, { 0x911075CC, 11 }					// ORIGINAL-BROADCAST-NPDU
	, { 0, 0 }
	};

//
//	NL Message Types
//

ScriptTranslateTable ScriptNLMsgTypeMap[] =
	{ { 0x294E7816, 0 }						// WHO-IS-ROUTER-TO-NETWORK
	, { 0x0154EC4F, 0 }						// WHOS-RTN
	, { 0x89F9A2D5, 1 }						// I-AM-ROUTER-TO-NETWORK
	, { 0x53C1FED2, 1 }						// IM-RTN
	, { 0xF568D0CC, 2 }						// I-COULD-BE-ROUTER-TO-NETWORK
	, { 0xDEDA7E5F, 2 }						// I-CLD-BE-RTN
	, { 0x4ED9703F, 3 }						// REJECT-MESSAGE-TO-NETWORK
	, { 0xC5D299CB, 3 }						// REJ-MTN
	, { 0xCBCA6F6C, 4 }						// ROUTER-BUSY-TO-NETWORK
	, { 0x39B8FAFE, 4 }						// RBTN
	, { 0x5EEA137B, 5 }						// ROUTER-AVAILABLE-TO-NETWORK
	, { 0x668F275C, 5 }						// RATN
	, { 0x4B02A6C9, 6 }						// INITIALIZE-ROUTING-TABLE
	, { 0xEF02FDF7, 6 }						// IRT
	, { 0x0C092241, 7 }						// INITIALIZE-ROUTING-TABLE-ACK
	, { 0xE6F311EB, 7 }						// IRT-ACK
	, { 0xA35B5DA7, 8 }						// ESTABLISH-CONNECTION-TO-NETWORK
	, { 0x080256D5, 8 }						// ECTN
	, { 0xFDDDF574, 9 }						// DISCONNECT-CONNECTION-TO-NETWORK
	, { 0x07A24D9E, 9 }						// DCTN
	, { 0, 0 }
	};


//
//	AL Message Types
//

ScriptTranslateTable ScriptALMsgTypeMap[] =
	{ { 0x566b432b, 0 }						// CONFIRMED-REQUEST
	, { 0xa17e3788, 1 }						// UNCONFIRMED-REQUEST
	, { 0x324dc228, 2 }						// SIMPLEACK
	, { 0x38a36471, 3 }						// COMPLEXACK
	, { 0x6c7c0630, 4 }						// SEGMENTACK
	, { 0x71ad2a43, 5 }						// ERROR
	, { 0xa9a5413f, 6 }						// REJECT
	, { 0xa1e0029a, 7 }						// ABORT
	, { 0, 0 }
	};

//
//	AL Service Choices
//

// Built at runtime from the StringTable
ScriptTranslateTable *ScriptALConfirmedServiceMap;

// Built at runtime from the StringTable
ScriptTranslateTable *ScriptALUnconfirmedServiceMap;

//
//	Error and Abort Reasons
//

// Built at runtime from the StringTable
ScriptTranslateTable *ScriptALRejectReasonMap;

// Built at runtime from the StringTable
ScriptTranslateTable *ScriptALAbortReasonMap;

// Object Type Keywords, built at runtime from the StringTable
// and the alias list below.
ScriptTranslateTable *ScriptObjectTypeMap;

// Alias list for object types.
// These are largely historical, and the newer objects tend to be
// obscure enough to argue against short aliases.
// But AI/AO/AV etc to the point objects are convenient.
const static ScriptTranslateTable ScriptObjectTypeMap_Aliases[] =
{     { 0x4F4FB029,  0 }					// AI
	, { 0x424AA5F5,  1 }					// AO
	, { 0x086F6F63,  2 }					// AV
	, { 0x4FAFB960,  3 }					// BI
	, { 0x42AAAF2C,  4 }					// BO
	, { 0x08CF789A,  5 }					// BV
	, { 0x80DDC343,  6 }					// CAL
	, { 0x59EB1404,  7 }					// COM
	, { 0xCD8A8E14,  8 }					// DEV
	, { 0xBAB1FB08,  9 }					// EVE
	, { 0x53D01EBD, 13 }					// MI
	, { 0x46CB1489, 14 }					// MO
	, { 0x61353228, 15 }					// NC
	, { 0x71D084EF, 16 }					// PRO
	, { 0xFA230323, 17 }					// SCH
	, { 0x52B95396, 18 }					// AVG
	, { 0x5D3473DF, 19 }					// MSV
	, { 0xC2E8CFF0, 20 }					// TR
	, { 0, 0 }
};

//
//	Application Layer Keywords
//

ScriptTranslateTable ScriptALMap[] =
	{ { kwNULL,					 1 }
	, { kwBOOL,					 2 }
	, { kwBOOLEAN,				 2 }
	, { kwUNSIGNED,				 3 }
	, { kwUNSIGNED8,			 3 }
	, { kwUNSIGNED16,			 3 }
	, { kwUNSIGNED32,			 3 }
	, { kwINTEGER,				 4 }
	, { kwREAL,					 5 }
	, { kwSINGLE,				 5 }
	, { kwDOUBLE,				 6 }
	, { kwOCTETSTRING,			 7 }
	, { kwCHARSTRING,			 8 }
	, { kwCHARACTERSTRING,		 8 }
	, { kwBITSTRING,			 9 }
	, { kwENUM,					10 }
	, { kwENUMERATED,			10 }
	, { kwDATE,					11 }
	, { kwTIME,					12 }
	, { kwOBJECT,				13 }
	, { kwOBJECTIDENTIFIER,		13 }
	, { kwDEVICE,				14 }
	, { kwDEVICEIDENTIFIER,		14 }
	, { kwPROPERTY,				15 }
	, { kwPROPERTYIDENTIFIER,	15 }
	, { kwOPEN,					16 }
	, { kwOPENTAG,				16 }
	, { kwOPENINGTAG,			16 }
	, { kwCLOSE,				17 }
	, { kwCLOSETAG,				17 }
	, { kwCLOSINGTAG,			17 }
	, { kwANY,					18 }
	, { 0, 0 }
	};

//
//	Boolean Value Keywords
//

ScriptTranslateTable ScriptBooleanMap[] =
	{ { 0xEC9BC233, 0 }						// FALSE
	, { 0x1A42850A, 0 }						// F
	, { 0x472B1DC0, 0 }						// NO
	, { 0x1D42CEC2, 0 }						// N
	, { 0x0E3DFF9D, 1 }						// TRUE
	, { 0x1F83060C, 1 }						// T
	, { 0xEF201378, 1 }						// YES
	, { 0x2163341F, 1 }						// Y
	, { 0, 0 }
	};

//
//	Character String Keywords
//

ScriptTranslateTable ScriptCharacterTypeMap[] =
	{ { 0x34504ea6, 0 }						// ANSI
	, { 0xd5c5606c, 0 }                     // ANSI-X3.4
	, { 0x5329d445, 0 }                     // UTF-8
    , { 0x998a0bda, 0 }                     // ISO-10646-UTF-8
	, { 0xa30b8ce4, 1 }						// IBM
	, { 0x3bc0262e, 1 }						// MICROSOFT
	, { 0xb3b30c8b, 1 }						// DBCS
	, { 0x06118d13, 1 }						// IBM-MICROSOFT-DBCS
	, { 0x3626d7c7, 2 }						// JIS
	, { 0xd6cfdb32, 2 }						// JIS-C-6226
	, { 0xbb2f3de8, 3 }						// UCS-4
	, { 0x43f12c59, 3 }						// ISO-10646-UCS-4
	, { 0xff1d2202, 4 }						// UCS-2
	, { 0x32cdf787, 4 }						// ISO-10646-UCS-2
	, { 0x632522b6, 5 }						// ISO-8859-1
	, { 0, 0 }
	};

//
//	Weekday Keywords
//

ScriptTranslateTable ScriptWeekdayMap[] =
	{ { 0xAA6F2FAF, 1 }						// MON
	, { 0x5CE90128, 1 }						// MONDAY
	, { 0xED28B19F, 2 }						// TUE
	, { 0xB39B56E6, 2 }						// TUESDAY
	, { 0x6EE7D46F, 3 }						// WED
	, { 0x7A3908DB, 3 }						// WEDNESDAY
	, { 0x0044DDB5, 4 }						// THU
	, { 0xEFE97FD7, 4 }						// THURSDAY
	, { 0xA179B08B, 5 }						// FRI
	, { 0x53F38204, 5 }						// FRIDAY
	, { 0xECFC4C5B, 6 }						// SAT
	, { 0x0D213816, 6 }						// SATURDAY
	, { 0x9FAA5CC5, 7 }						// SUN
	, { 0x52242E3E, 7 }						// SUNDAY
	, { 0, 0 }
	};

//
//	For optional values
//

ScriptTranslateTable ScriptNONEMap[] =
	{ { 0x7926167A, 9999 }					// NONE
	, { 0, 0 }
	};

//
//	Month Keywords
//

ScriptTranslateTable ScriptMonthMap[] =
	{ { 0x1D05812E,  1 }					// JAN
	, { 0xC4E47EAA,  1 }					// JANUARY
	, { 0xCEFFBA5E,  2 }					// FEB
	, { 0x76DEB7DA,  2 }					// FEBUARY
	, { 0x513497A7,  3 }					// MAR
	, { 0x88667C8B,  3 }					// MARCH
	, { 0xAC278F91,  4 }					// APR
	, { 0x78905B59,  4 }					// APRIL
	, { 0x6A8ECE9A,  5 }					// MAY
	, { 0x9C4A09D6,  6 }					// JUN
	, { 0x6AA0E76A,  6 }					// JUNE
	, { 0x02C28C6C,  7 }					// JUL
	, { 0x32025C50,  7 }					// JULY
	, { 0x7F8F7FF4,  8 }					// AUG
	, { 0x1DAEAA09,  8 }					// AUGUST
	, { 0x0694A00F,  9 }					// SEP
	, { 0xEDDC4E24,  9 }					// SEPTEMBER
	, { 0x91CFCEC3, 10 }					// OCT
	, { 0x2649B215, 10 }					// OCTOBER
	, { 0x10ED2E8E, 11 }					// NOV
	, { 0x1EB5E48A, 11 }					// MOVEMBER
	, { 0x1B0366A5, 12 }					// DEC
	, { 0x28CC1CA1, 12 }					// DECEMBER
	, { 0, 0 }
	};

// Property Keywords, built at runtime from StringTable
ScriptTranslateTable *ScriptPropertyMap;

// Built at runtime from the StringTable
ScriptTranslateTable *scriptStandardServicesMap;

	
	ScriptTranslateTable scriptStandardServicesRequirementMap[] =
	{ { 0xED940AA5,		 1 }			//	INITIATE
	, { 0x9A18FBF4,		 2 }			//	EXECUTE
	, { 0, 0 }
	};

	// Strings defined by 135.1-2009 4.5.9
	// Turned into keywords by removing spaces.  Really!
	ScriptTranslateTable scriptFailTimesMap[] = // 
	{ { 0xCEE49250,		 0 }		// NotificationFailTime
	, { 0x97AC3FDB,		 1 }		// InternalProcessingFailTime
	, { 0xE6E9C002,		 2 }		// MinimumON/OFFTime
	, { 0x95DAF86D,		 3 }		// ScheduleEvaluationFailTime
	, { 0x43ED7A24,		 4 }		// ExternalCommandFailTime
	, { 0xAA4790D0,		 5 }		// ProgramObjectStateChangeFailTime
	, { 0x3D824098,		 6 }		// AcknowledgementFailTime
	, { 0x8F83F96F,		 7 }		// SlaveProxyConfirmInterval
	, { 0, 0 }
	};

	ScriptTranslateTable scriptGenKeywordMap[] =
	{ { kwNET,			 1 }
	, { kwNETWORK,		 2 }
	, { kwRESERVED,		 3 }
	, { kwVENDOR,		 4 }
	, { 0, 0 }
	};


	ScriptTranslateTable scriptDLCKeywordMap[] =
	{ { kwDA,			 1 }
	, { kwSA,			 2 }
	, { kwLSAP,			 3 }
	, { kwBVLCI,		 4 }
	, { 0, 0 }
	};
	
	ScriptTranslateTable scriptNLKeywordMap[] =
	{ { kwVERSION,			 1 }
	, { kwDNET,				 2 }
	, { kwDADR,				 3 }
	, { kwSNET,				 4 }
	, { kwSADR,				 5 }
	, { kwHOPCNT,			 6 }
	, { kwHOPCOUNT,			 6 }
	, { kwMSG,				 7 }
	, { kwMESSAGE,			 7 }
	, { kwPRIO,				 8 }
	, { kwPRIORITY,			 8 }
	, { kwDER,				 9 }
	, { kwVENDORID,			 10 }
	, { kwNL,				 11 }
	, { kwNLDATA,			 11 }
	, { 0, 0 }
	};

	ScriptTranslateTable scriptALKeywordMap[] =
	{ { kwPDU ,					 1 }
	, { kwPDUTYPE,				 1 }
	, { kwSEGMSG,				 2 }
	, { kwSEGMENTEDMESSAGE,		 3 }
	, { kwMOREFOLLOWS,			 4 }
	, { kwSEGRESP,				 5 }
	, { kwSEGRESPACCEPTED,		 5 }
	, { kwMAXRESP,				 6 }
	, { kwMAXRESPONSE,			 6 }
	, { kwMAXSIZE,				 7 }
	, { kwNEGATIVEACK,			 8 }
	, { kwSERVER,				 9 }
	, { kwINVOKEID,			     10 }
	, { kwSEQUENCENR,			 11 }
	, { kwWINDOWSIZE,			 12 }
	, { kwAL,				     13 }
	, { kwALDATA,			     13 }
	, { kwSERVICE,				 14 }
	, { kwERROR,				 14 }
	, { kwERRORCHOICE,		     14 }
	, { kwREJECTREASON,		 	 15 }
	, { kwABORTREASON,		     16 }
	, { kwMAXSEGS,		         17 }
	, { 0, 0 }
	};

	
	ScriptTranslateTable scriptDataKeywordMap[] =
	{ { kwOBJECT,					 1 }
	, { kwOBJECTIDENTIFIER,			 2 }
	, { kwDEVICE,					 4 }
	, { kwDEVICEIDENTIFIER,		     4 }
	, { kwPROPERTY,			         5 }
	, { kwPROPERTYIDENTIFIER,	 	 5 }
	, { kwOPEN,						 6 }
	, { kwOPENTAG,					 6 }
	, { kwOPENINGTAG,				 6 }
	, { kwCLOSE,					 7 }
	, { kwCLOSETAG,					 7 }
	, { kwCLOSINGTAG,				 7 }
	, { kwNULL,						 8 }
	, { kwBOOL,						 9 }
	, { kwBOOLEAN,					 9 }
	, { kwUNSIGNED,					 10 }
	, { kwUNSIGNED8,				 10 }
	, { kwUNSIGNED16,				 10 }
	, { kwUNSIGNED32,				 10 }
	, { kwINTEGER,					 11 }
	, { kwREAL,						 12 }
	, { kwSINGLE,					 12 }
	, { kwDOUBLE,					 12 }
	, { kwOCTETSTRING,				 13 }
	, { kwCHARSTRING,				 14 }
	, { kwCHARACTERSTRING,			 14 }
	, { kwBITSTRING,				 15 }
	, { kwENUM,						 16 }
	, { kwENUMERATED,				 16 }
	, { kwDATE,						 17 }
	, { kwTIME,						 18 }
	, { kwANY,						 19 }
	, { 0, 0 }
	};	


// In order to avoid the maintenance hassles of multiple, supposed-to-be-identical names
// and hash values, we generate the token tables at runtime from the string tables in StringTables.cpp.
//
namespace PICS {
	#include "db.h" 
	#include "Stdobjpr.h"	// definition to etable, if needed
}

// Compare the st
static void VetHash( NetworkSniffer::BACnetStringTable	&theStringTable,	// Master string table 
					 ScriptTranslateTable				*pTheTranslateTable,// has table  to vet
					 const char							*theStrings[] )		// optional string table to compare
{
	// Validate the strings
	for (int ix = 0; ix < theStringTable.m_nStrings; ix++)
	{
		const char *pStringT = theStringTable.m_pStrings[ix];
		const char *pString2 = (theStrings) ? theStrings[ix] : "no table";
		if ((pString2 != NULL) && (*pString2 != 0))
		{
			// Normalize the strings
			CString str1( pStringT );
			str1.MakeUpper();
			
			CString str2( pString2 );
			str2.MakeUpper();
			str2.Replace( '_', '-' );

			if (theStrings && (str1 != str2))
			{
				TRACE( "Val=%u: STRTAB=>%s< differs from string2=>%s<\n", ix, pStringT, pString2 );
			}

			int hash = ScriptToken::HashCode( (LPCTSTR)str1 );
			int value = ScriptToken::Lookup( hash, pTheTranslateTable );
			if (value < 0)
			{
				TRACE( "Val=%u: STRTAB=>%s< hash=%08X not in hash table\n", ix, pStringT, hash );
			}
			else if (value != ix)
			{
				TRACE( "Val=%u: STRTAB=>%s< hash=%08X table has value %u\n", ix, pStringT, hash, value );
			}
		}
		else
		{
			TRACE( "Val=%u: STRTAB=>%s< missing string2\n", ix, pStringT );
		}
	}
}

static void MakeHash( ScriptTranslateTable				**ppTheTranslateTable,	// build this table 
					  NetworkSniffer::BACnetStringTable &theStringTable,		// Master string table
					  const ScriptTranslateTable		*pTheAliasTable )		// optional alias values
{
	// Basic table is N strings, plus a final NULL entry
	int tableSize = theStringTable.m_nStrings + 1;

	if (pTheAliasTable != NULL)
	{
		// Count the aliases
		for (int ix = 0; pTheAliasTable[ix].scriptKeyword != 0; ix++)
		{
			tableSize += 1;
		}
	}

	// Allocate the hash table
	*ppTheTranslateTable = new ScriptTranslateTable[ tableSize ];

	// Add the hash versions of the string-table entries
	int hashIx = 0;
	for (int ix = 0; ix < theStringTable.m_nStrings; ix++)
	{
		// Normalize the string
		CString str( theStringTable.m_pStrings[ix] );
		str.MakeUpper();
		str.Replace( '_', '-' );

		int hash = ScriptToken::HashCode( (LPCTSTR)str );
		(*ppTheTranslateTable)[ hashIx   ].scriptKeyword = hash;
		(*ppTheTranslateTable)[ hashIx++ ].scriptValue   = ix;
	}

	if (pTheAliasTable != NULL)
	{
		// Add the aliases
		for (int ix = 0; pTheAliasTable[ix].scriptKeyword != 0; ix++)
		{
			(*ppTheTranslateTable)[ hashIx   ].scriptKeyword = pTheAliasTable[ix].scriptKeyword;
			(*ppTheTranslateTable)[ hashIx++ ].scriptValue   = pTheAliasTable[ix].scriptValue;
		}
	}

	// Final null entry
	(*ppTheTranslateTable)[ hashIx ].scriptKeyword = 0;
	(*ppTheTranslateTable)[ hashIx ].scriptValue   = 0;
}

// Called during initialization (VTSApp::InitInstance or equivalent) to build hash tables from enum strings.
void MakeHashTables()
{
	MakeHash( &ScriptPropertyMap,  NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, NULL );
	MakeHash( &ScriptObjectTypeMap, NetworkSniffer::BAC_STRTAB_BACnetObjectType, ScriptObjectTypeMap_Aliases );

	MakeHash( &ScriptALConfirmedServiceMap, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice, NULL );
	MakeHash( &ScriptALUnconfirmedServiceMap, NetworkSniffer::BAC_STRTAB_BACnetUnconfirmedServiceChoice, NULL );

	MakeHash( &ScriptALRejectReasonMap, NetworkSniffer::BAC_STRTAB_BACnetReject, NULL );
	MakeHash( &ScriptALAbortReasonMap, NetworkSniffer::BAC_STRTAB_BACnetAbort, NULL );
	MakeHash( &scriptStandardServicesMap, NetworkSniffer::BAC_STRTAB_BACnetServicesSupported, NULL );
}

// Called during shutdown (VTSApp::ExitInstance or equivalent) to free allocated hash tables
void FreeHashTables()
{
	delete[] ScriptPropertyMap;
	delete[] ScriptObjectTypeMap;
	delete[] ScriptALConfirmedServiceMap;
	delete[] ScriptALUnconfirmedServiceMap;

	delete[] ScriptALRejectReasonMap;
	delete[] ScriptALAbortReasonMap;
	delete[] scriptStandardServicesMap;
}
