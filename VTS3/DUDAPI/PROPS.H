//PROPS.H
//property definitions for standard objects
#include "VTS.h"
#ifndef __PROPS_H_INCLUDED
#define __PROPS_H_INCLUDED

//ParseTypes
#define	ob_id				0					//an object identifier
#define	et					1					//an enumeration table
#define	flt					2					//float
#define	bits				3					//octet of T or F flags
#define	uw					4					//unsigned word (16 bits)
#define	ud					5					//unsigned dword (32 bits)
#define	sw					6					//signed word (16 bits)
#define	u127				7					//1..127
//#define	s10					8					//was char [10]
//#define	s32					9					//was char [32]
//#define	s64					10					//was char [64]
#define	ch_string				11					// string with max length
#define	propref				12					//obj property reference
#define	dt					13					//BACnetDateTime
#define	calist				14					//list of calendarentry
#define	act					15					//action array
#define	actext				16					//action_text array
#define	pss					17					//protocol services supported bitstring
#define	pos					18					//protocol objects supported bitstring
#define	vtcl				19					//vt classes
#define	evparm				20					//event parameter
#define	skeys				21					//session keys
#define	tsrecip				22					//time synch recipients
#define	dabind				23					//device address bindings
#define	recip				24					//recipient
#define	paf					25					//priority array flt
#define	statext				26					//state text array
#define	pab					27					//priority array bpv
#define	pau					28					//priority array uw (16 bits)
#define	reciplist			29					//list of BACnetDestination
#define	xsched				30					//exception schedule: array[] of specialevent
#define	wsched				31					//weekly schedule: array[7] of list of timevalue
#define	u16					32					//1..16
#define	setref				33					//setpoint reference
#define	raslist				34					//list of readaccessspecs
#define	stavals				35					//list of unsigned
#define	dtrange				36					//range of dates
#define	lopref				37					//list of object prop refs
#define ebool				38					//boolean enumeration
#define ddate				39					//date
#define ttime				40					//time
#define looref				41					//list of objectid references
#define ssint				42					//short (2-byte) signed integer  MAG 13 FEB 2001
#define LOGREC				43					//LogRecord
#define TSTMP               44             // BACnetTimeStamp
//Added  Parse Time by Liangping Xu,2002-9

#define	devobjpropref	    45					//dev obj property reference

#define calent				46					//single calendar entry, madanner 11/1/02
#define prival				47					//single priority value (could be b, flt, null, r), madanner 11/1/02

//Added by Zhu Zhen hua, 2003-9-4
#define dsofweek            48			//BACnetDaysofWeek
#define eventransbits		49			//BACnetEventTansitionBits
//#define recipt			50			//BACnetRecipient  //madanner 9/04 should be recip
#define destination         51			//BACnetDestination
#define sequenceof			52			//BACnetSequenceOf
#define listof				53			//BACnetListOf
#define arrayof				54			//BACnetArrayOf
#define lodoref				55          // List of BACnetDeviceObjectReference
#define devobjref			56          // BACnetDeviceObjectReference
#define eboollist			57          // list of boolean

#define escale				60   // BACnetScale    Shiyuan Xiao 7/15/2005
#define eprescl				61   // BACnetPrescale Shiyuan Xiao 7/15/2005
#define eaclr				62   // BACnetAccumulatorRecord Shiyuan Xiao 7/15/2005

#define ptInt32				63	// Signed Int32
#define ptPai				64	// Priority Array of Int32


#define ptDate				200  // parsetype BACnetDate
#define ptTime				201  // parsetype BACnetTime
#define lobj				202  // array of objects, (parse type)
#define vtse				203  // list of active  vt sessions (parse type)
#define uwarr				204  // array of uw (16 bits)

#define lCOVSub				205		// List of BACnetCOVSubcription,   added Gingbo Gao, 2003-9-1
#define TSTMParr			206	// array of BACnetTimeStamp  madanner 9/04
#define dabindelem			207	// single BACnetDeviceAddressBinding madanner 9/04
#define COVSub				208		// single BACnetCOVSubscription madanner 9/04
#define recipproc			209	// singled BACnetRecipientProcess madanner 9/04

#define etl					210         // enumeration table in a list ljt 9/05
#define shedlevel			211  // BACnetShedLevel parse type  LJT 3/3/2009
#define	pae					212		// priority-array of enum
#define	PT_PROPLIST			213		// list of propertyIDs

////
//////////////////////////////////////////////
#define enull				254					//null enumeration
#define	none				255					//no parsing required, runtime generated value



static const char nullstring[5]={"null"};	//"null\0";			//	***001
static const char anystring[4]={"any"};		//"any\0";			//	***001

// TODO: Why is this table in an h file?  
// Used by Bacprim32.cpp PARSING READ-CONDITIONAL to convert string to parsetype.
//   This service is now deprecated
// Used by Vtsapi32.cpp VTSAPIgetdefaultparsetype using an unchecked index to get a string.
// As noted below, there are parsetypes #defined above past the end of this table...
// (VTSAPIgetdefaultparsetype may be obsolete)
static const char *stParseTypes[]={
				"object ID",      // 0
				"enumeration",
				"float",
				"bit flags",
				"unsigned16",
				"unsigned32",
				"signed16",
				"1..127",
				"char10",
				"char32",
				"char64",         // 10
				"char string",
				"prop ref",
				"date time",
				"calendar list",
				"action array",
				"action text",
				"prot services",
				"prot objects",
				"vt classes",
				"event parm",     // 20
				"session keys",
				"time recip",
				"devaddr binding",
				"recipient",
				"flt:pri array",
				"state text",
				"bpv:pri array",
				"uw:pri array",
				"dest list",
				"excep sched",    // 30
				"weekly sched",
				"1..16",
				"setpt ref",
				"Read list",
				"uns list",
				"date range",
				"obj list",
				"boolean",
				"date",
				"time",           // 40
				"obj ref list",
				"short signed int",  //MAG added 13 FEB 2001
            // TODO: values are #defined above up to 64, and then again from 200 to 212.  Should we worry?
				NULL /*always last*/
				};

//qualifiers in propdescriptor member PropFlags
#define	O					0x0000				//optional
#define	R					0x0001				//required
#define	W					0x0002				//required writable
#define	Woutofservice		0x0004				//req writable when OutOfService is true
#define	WithGroup			0x0008				//all in PropGroup must be present if any are
#define	NotWithGroup1		0x0010				//must not be present if any in PropGroup=1 are
#define	NotWithGroup2		0x0020				//must not be present if any in PropGroup=2 are
#define	WithService			0x0040				//required if service PropGroup is supported
#define	AtLeast1			0x0080				//at least one from PropGroup must be present
#define	IsArray				0x0100				//property is an array
#define IsCommandable		0x0200				//property is commandable
#define QVAL					0x0400				//Show as "?" in EPICS
//services supported in propdescriptor member PropGroup
#define	COV					127					//COV reporting
#define	Intr				126					//Instrinsic reporting
#define	VT					125					//VT services
#define	SEGSVC				124					//segmentation
#define	TMASTER				123					//time master, must also be writable
#define	MSTPMASTER			122					//MS/TP master node
//last definition in an object is marked in PropGroup with:
#define	LAST		0x80

// NOTE: the following values MUST represent the order in the s_enumStringTables table in StringTables.cpp!
enum EnumType
{
   eiNone = -1,      // used for non-enumerated parse types
   eiTF = 0,         // 0
   eiReli,
   eiEvState,
   eiEU,
   eiNT,
   eiBPV,
   eiPolar,
   eiDS,
   eiSegOpt,
   eiEvType,
   eiFAM,            // 10
   eiLoopAct,
   eiPrErr,
   eiPrState,
   eiPrChg,
   eiObjectTypes,
   eiStF,
   eiEvTr,
   eiLimEn,
   eiVTCls,
   eiDOW,            // 20
   eiYN,
   eiMon,
   eiBPVn,
   eiLifeSafetyState,
   eiSilencedState,
   eiLifeSafetyOperation,
   eiMaintenance,
   eiLifeSafetyMode,
   eiAccumulatorStatus,
   eiLoggingType,    // 30
   eiShedState,
   eiNodeType,
   eiDoorValue,
   eiDoorStatus,
   eiLockStatus,
   eiDoorSecuredStatus,
   eiDoorAlarmState,
   eiDoorValuen,
   eiBackupState,
   eiRestartReason   // 40
};

#endif //__PROPS_H_INCLUDED
