//PROPS.H
//property definitions for standard objects
#ifndef __PROPS_H_INCLUDED
#define __PROPS_H_INCLUDED

//ParseTypes
#define	ob_id				0					//an object identifier
#define	et					1					//an enumeration table
#define	flt					2					//float
#define	bits				3					//octet of T or F flags
#define	uw					4					//unsigned word
#define	ud					5					//unsigned dword
#define	sw					6					//signed word
#define	u127				7					//1..127
#define	s10					8					//char [10]
#define	s32					9					//char [32]
#define	s64					10					//char [64]
#define	s132				11					//char [132]
#define	propref				12					//obj property reference
#define	dt					13					//date/time
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
#define	pau					28					//priority array uw
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
#define ssint				42
#define enull				254					//null enumeration
#define	none				255					//no parsing required, runtime generated value


static char	nullstring[5]={'n','u','l','l',0};	//"null\0";			//	***001
static char anystring[4]={'a','n','y',0};		//"any\0";			//	***001

typedef struct {
	word	nes;								//number of string pointers which follow
	char	*estrings[];						//table of pointers to strings
	} stable;
//Special table for parse types

static stable stParseTypes={
				42,								//42 parse types						
				"object ID",
				"enumeration",
				"float",
				"bit flags",
				"unsigned16",
				"unsigned32",
				"signed16",
				"1..127",
				"char10",
				"char32",
				"char64",
				"char132",
				"prop ref",
				"date time",
				"calendar list",
				"action array",
				"action text",
				"prot services",
				"prot objects",
				"vt classes",
				"event parm",
				"session keys",
				"time recip",
				"devaddr binding",
				"recipient",
				"flt:pri array",
				"state text",
				"bpv:pri array",
				"uw:pri array",
				"dest list",
				"excep sched",
				"weekly sched",
				"1..16",
				"setpt ref",
				"Read list",
				"uns list",
				"date range",
				"obj list",
				"boolean",
				"date",
				"time",
				"obj ref list"
				};
//Special table for parse types
static stable stRelationTypes={
				6,								//6 relation types						
				"==",
				"!=",
				"< ",
				"> ",
				"<=",
				">="
				};
//Special table for property ids

static stable stPropIDs={
				124,
				"acked-transitions",
				"ack-required",
				"action",
				"action-text",
				"active-text",
				"active-vt-sessions",
				"alarm-value",
				"alarm-values",
				"all",
				"all-writes-successful",
				"apdu-segment-timout",
				"apdu-timout",
				"application-software-version",
				"archive",
				"bias",
				"change-of-state-count",
				"change-of-state-time",
				"notification-class",
				"reserved propertyID 18",
				"controlled-variable-reference",
				"controlled-variable-units",
				"controlled-variable-value",
				"cov-increment",
				"datelist",
				"daylight-savings-status",
				"deadband",
				"derivative-constant",
				"derivative-constant-units",
				"description",
				"description-of-halt",
				"device-address-binding",
				"device-type",
				"effective-period",
				"elapsed-active-time",
				"error-limit",
				"event-enabled",
				"event-state",
				"event-type",
				"exception-schedule",
				"fault-values",
				"feedback-value",
				"file-access-method",
				"file-size",
				"file-type",
				"firmware-revision",
				"high-limit",
				"inactive-text",
				"in-process",
				"instance-of",
				"integral-constant",
				"integral-constant-units",
				"issue-confirmed-notifications",
				"limit-enable",
				"list-of-group-members",
				"list-of-object-property-references",
				"list-of-session-keys",
				"local-date",
				"local-time",
				"location",
				"low-limit",
				"manipulated-variable-reference",
				"maximum-output",
				"max-apdu-length-accepted",
				"max-info-frames",
				"max-master",
				"max-pres-value",
				"minimum-off-time",
				"minimum-on-time",
				"minimum-output",
				"min-pres-value",
				"model-name",
				"modification-date",
				"notify-type",
				"number-of-apdu-retries",
				"number-of-states",
				"object-identifier",
				"object-list",
				"object-name",
				"object-property-reference",
				"object-type",
				"optional",
				"out-of-service",
				"output-units",
				"event-parameters",
				"polarity",
				"present-value",
				"priority",
				"priority-array",
				"priority-for-writing",
				"process-identifier",
				"program-change",
				"program-location",
				"program-state",
				"proportional-constant",
				"proportional-constant-units",
				"protocol-conformance-class",
				"protocol-object-types-supported",
				"protocol-services-supported",
				"protocol-version",
				"read-only",
				"reason-for-halt",
				"recipient",
				"recipient-list",
				"reliability",
				"relinquish-default",
				"required",
				"resolution",
				"segmentation-supported",
				"setpoint",
				"setpoint-reference",
				"state-text",
				"status-flags",
				"system-status",
				"time-delay",
				"time-of-active-time-reset",
				"time-of-state-count-reset",
				"time-synchronization-recipients",
				"units",
				"update-interval",
				"utc-offset",
				"vendor-identifier",
				"vendor-name",
				"vt-classes-supported",
				"weekly-schedule"
				};
//qualifiers in PropFlags
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
//services supported in PropGroup
#define	COV					127					//COV reporting
#define	Intr				126					//Instrinsic reporting
#define	VT					125					//VT services
#define	SEGSVC				124					//segmentation
#define	TMASTER				123					//time master, must also be writable
#define	MSTPMASTER			122					//MS/TP master node
//last definition in an object is marked in PropGroup with:
#define	Last		0x80

//NOTE: the following equates MUST represent the order in the AllETs table in STDOBJPR.H!
#define		eiTF			1
#define		eiReli			2
#define		eiEvState		3
#define		eiEU			4
#define		eiNT			5
#define		eiBPV			6
#define		eiPolar			7
#define		eiDS			8
#define		eiSegOpt		9
#define		eiEvType		10
#define		eiFAM			11
#define		eiLoopAct		12
#define		eiPrErr			13
#define		eiPrState		14
#define		eiPrChg			15
#define 	eiObjectTypes	16
#define		eiStF			17
#define		eiEvTr			18
#define		eiLimEn			19
#define		eiVTCls			20
#define		eiDOW			21
#define		eiYN			22
#define		eiCalType		23
#define		eiMon			24
#define		eiWOM			25
#define		eiAnyDOW		26
#define		eiBPVn			27
#define		eiAppDataTypes	28

#endif //__PROPS_H_INCLUDED
