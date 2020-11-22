//STDOBJPR.H
//property definitions for standard objects
#ifndef __STDOBJPR_H_INCLUDED
#define __STDOBJPR_H_INCLUDED

#include "propid.h"
#include "props.h"

typedef struct {
	word	propes;								//proprietary enumeration begin at this value
	dword	propmaxes;							//max value for proprietary+1
	word	nes;								//number of string pointers which follow
	char	*estrings[];						//table of pointers to strings
	} etable;
#define _etable 0
	
//------------------------------------------------------
//	Special Enumeration Tables
static etable etObjectTypes={
			128,1024,18,
			"analog-input",
			"analog-output",       
			"analog-value",     
			"binary-input",     
			"binary-output",   
			"binary-value",
			"calendar",
			"command",
			"device",
			"event-enrollment",
			"file",
			"group",
			"loop",
			"multi-state-input",
			"multi-state-output",
			"notification-class",
			"program",
			"schedule"
			};
static etable etTF={0,0,2,"False","True"};
static etable etYN={0,0,2,"No","Yes"};
static etable etReli={
			64,65536,9,
			"no-fault-detected",
			"no-sensor",
			"over-range",
			"under-range",
			"open-loop",
			"shorted-loop",
			"no-output",
			"unreliable-other",
			"process-error"
			};
static etable etEvState={
			64,65536,5,
			"normal",
			"fault",
			"offnormal",
			"high-limit",
			"low-limit"
			};

static etable etEU={
			256,65536,142,
		//Area   
			"square-meters",                     //	 0
			"square-feet",                       //	 1
		//Electrical
			"milliamperes",                      //	 2
			"amperes",                           //	 3
			"ohms",                              //	 4
			"volts",                             //	 5
			"kilovolts",                         //	 6
			"megavolts",                         //	 7
			"volt-amperes",                      //	 8
			"kilovolt-amperes",                  //	 9
			"megavolt-amperes",                  //	10
			"volt-amperes-reactive",             //	11
			"kilovolt-amperes-reactive",         //	12
			"megavolt-amperes-reactive",         //	13
			"degrees-phase",                     //	14
			"power-factor",                      //	15
		//Energy
			"joules",                            //	16
			"kilojoules",                        //	17
			"watt-hours",                        //	18
			"kilowatt-hours",                    //	19
			"btus",                              //	20
			"therms",                            //	21
			"ton-hours",                         //	22
		//Enthalpy
			"joules-per-kilogram-dry-air",       //	23
			"btus-per-pound-dry-air",            //	24
		//Frecuency
			"cycles-per-hour",                   //	25
			"cycles-per-minute",                 //	26
			"hertz",                             //	27
		//Humidity
			"grams-of-water-per-kilogram-dry-air",//28
			"percent-relative-humidity",         //	29
		//Length
			"millimeter",                        //	30
			"meters",                            //	31
			"inches",                            //	32
			"feet",                              //	33
		//Light
			"watts-per-square-foot",             //	34
			"watts-per-square-meter",            //	35
			"lumen",                             //	36
			"lux",                               //	37
			"foot-candles",                      //	38
		//Mass
			"kilograms",                         //	39
			"pounds-mass",                       //	40
			"tons",                              //	41
		//Mass Flow
			"kilograms-per-second",              //	42
			"kilograms-per-minute",              //	43
			"kilograms-per-hour",                //	44
			"pounds-mass-per-minute",            //	45
			"pounds-mass-per-hour",              //	46
		//Power
			"watts",                             //	47
			"kilowatts",                         //	48
			"megawatts",                         //	49
			"btus-per-hour",                     //	50
			"horsepower",                        //	51
			"tons-refrigeration",                //	52
		//Pressure
			"pascals",                           //	53
			"kilopascals",                       //	54
			"bars",                              //	55
			"pounds-force-per-square-inch",      //	56
			"centimeters-of-water",              //	57
			"inches-of-water",                   //	58
			"millimeters-of-mercury",            //	59
			"centimeters-of-mercury",            //	60
			"inches-of-mercury",                 //	61
		//Temperature
			"degrees-celsius",                   //	62
			"degrees-kelvin",                    //	63
			"degrees-fahrenheit",                //	64
			"degree-days-celsius",               //	65
			"degree-days-fahrenheit",            //	66
		//Time
			"years",                             //	67
			"months",                            //	68
			"weeks",                             //	69
			"days",                              //	70
			"hours",                             //	71
			"minutes",                           //	72
			"seconds",                           //	73
		//Velocity
			"meters-per-second",                 //	74
			"kilometers-per-hour",               //	75
			"feet-per-second",                   //	76
			"feet-per-minute",                   //	77
			"miles-per-hour",                    //	78
		//Volume
			"cubic-feet",                        //	79
			"cubic-meters",                      //	80
			"imperial-gallons",                  //	81
			"liters",                            //	82
			"us-gallons",                        //	83
		//Volumetric Flow
			"cubic-feet-per-minute",             //	84
			"cubic-meters-per-second",           //	85
			"imperial-gallons-per-minute",       //	86
			"liters-per-second",                 //	87
			"liters-per-minute",                 //	88
			"us-gallons-per-minute",             //	89
		//Others
			"degrees-angular",                   // 90
			"degrees-celsius-per-hour",          // 91
			"degrees-celsius-per-minute",        // 92
			"degrees-fahrenheit-per-hour",       // 93
			"degrees-fahrenheit-per-minute",     // 94
			"no-units",                          // 95
			"parts-per-million",                 // 96
			"parts-per-billion",                 // 97
			"percent",                           // 98
			"percent-per-second",                // 99
			"per-minute",                        //100
			"per-second",                        //101
			"psi-per-degree-fahrenheit",         //102
			"radians",                           //103
			"revolutions-per-minute",            //104
		//Currency
			"currency1",						 //105
			"currency2",						 //106
			"currency3",						 //107
			"currency4",						 //108
			"currency5",						 //109
			"currency6",						 //110
			"currency7",						 //111
			"currency8",						 //112
			"currency9",						 //113
			"currency10",						 //114
		//Free
			NULL,								 //115
			NULL,								 //116
			NULL,								 //117
			NULL,								 //118
			NULL,								 //119
			NULL,								 //120
			NULL,								 //121
		//Added after second review:
			"kilohms",                           //122
			"megohms",							 //123
			"millivolts",						 //124
			"kilojoules-per-kilogram",			 //125
			"megajoules",						 //126
			"joules-per-degree-kelvin",			 //127
			"joules-per-kilogram-degree-kelvin", //128
			"kilohertz",						 //129
			"megahertz",						 //130
			"per-hour",							 //131
			"milliwatts",						 //132
			"hectopascals",						 //133
			"millibars",						 //134
			"cubic-meters-per-hour",			 //135
			"liters-per-hour",					 //136
			"kilowatt-hours-per-square-meter",	 //137
			"kilowatt-hours-per-square-foot",	 //138
			"megajoules-per-square-meter",		 //139
			"megajoules-per-square-foot",		 //140
			"watts-per-square-meter-degree-kelvin" //141
			};
static etable etNT={0,0,3,"alarm","event","ack-notification"};
static etable etBPV={0,0,2,"inactive","active"};
static etable etBPVn={0,0,3,"inactive","active","null"};
static etable etPolar={0,0,2,"normal","reverse"};
static etable etDS={
			64,65536,5,
			"operational",
			"operational-read-only",
			"download-required",
			"download-in-progress",
			"non-operational"
			};
static etable etSegOpt={0,0,4,"segmented-both","segmented-transmit","segmented-receive","no-segmentation"};
static etable etEvType={
			64,65536,6,
			"change-of-bitstring",
			"change-of-state",
			"change-of-value",
			"command-failure",
			"floating-limit",
			"out-of-range"
			};
static etable etFAM={0,0,3,"record-access","stream-access","record-and-stream-access"};
static etable etLoopAct={0,0,2,"direct","reverse"};
static etable etPrErr={
			64,65536,5,
			"normal",
			"load-failed",
			"internal",
			"program",
			"other"
			};
static etable etPrState={
			0,0,6,
			"idle",
			"loading",
			"running",
			"waiting",
			"halted",
			"unloading"
			};
static etable etPrChg={
			0,0,6,
			"ready",
			"load",
			"run",
			"halt",
			"restart",
			"unload"
			};
static etable etStatusFlags={
			0,0,4,
			"in-alarm",
			"fault",
			"overridden",
			"out-of-service"
			};
static etable etEventTransitions={
			0,0,3,
			"to-offnormal",
			"to-fault",
			"to-normal"
			};
static etable etLimitEnable={
			0,0,2,
			"lowLimitEnable",
			"highLimitEnable"
			};
static etable etVTClasses={
			0,0,7,
			"default-terminal",
			"ansi-x3.64",
			"dec-vt52",
			"dec-vt100",
			"dec-vt220",
			"hp-700/94",
			"ibm-3130"
			};
			
static etable etDaysofWeek={
			0,0,7,
			"monday",
			"tuesday",
			"wednesday",
			"thursday",
			"friday",
			"saturday",
			"sunday"
			};
			
static etable etCalType={
			0,0,3,
			"Date",
			"DateRange",
			"WeekNDay"
			};
			
static etable etMonths={
			0,0,13,
			"Any",
			"January",
			"February",
			"March",
			"April",
			"May",
			"June",
			"July",
			"August",
			"September",
			"October",
			"November",
			"December"
			};
			
static etable etWeekofMonth={
			0,0,7,
			"any",
			"1st-7th",
			"8th-14th",
			"15th-21st",
			"22nd-28th",
			"29th-31st",
			"Last 7 Days"
			};

static etable etAppDataTypes={
			0,0,16,
			"NULL",
			"Boolean",
			"Unsigned",
			"Unsigned8",
			"Unsigned16",
			"Unsigned32",
			"INTEGER",
			"REAL",
			"Double",
			"OCTET STRING",
			"CharacterString",
			"BIT STRING",
			"ENUMERATED",
			"Date",
			"Time",
			"BACnetObjectIdentifier"
			};

//There must be a one-to-one correspondence between this list and the application datatypes
#define	apdtNULL					0
#define	apdtBoolean					1
#define	apdtUnsigned				2
#define	apdtUnsigned8				3
#define	apdtUnsigned16				4
#define	apdtUnsigned32				5
#define	apdtINTEGER					6
#define	apdtREAL					7
#define	apdtDouble					8
#define	apdtOCTET STRING			9
#define	apdtCharacterString			10
#define	apdtBIT STRING				11
#define	apdtENUMERATED				12
#define	apdtDate					13
#define	apdtTime					14
#define	apdtBACnetObjectIdentifier	15

static etable etAnyDayofWeek={
			0,0,8,
			"Any",
			"Mon",
			"Tue",
			"Wed",
			"Thu",
			"Fri",
			"Sat",
			"Sun"
			};

static etable *AllETs[]={
			&etTF,
			&etReli,
			&etEvState,
			&etEU,
			&etNT,
			&etBPV,
			&etPolar,
			&etDS,
			&etSegOpt,
			&etEvType,
			&etFAM,
			&etLoopAct,
			&etPrErr,
			&etPrState,
			&etPrChg,
			&etObjectTypes,
			&etStatusFlags,
			&etEventTransitions,
			&etLimitEnable,
			&etVTClasses,
			&etDaysofWeek,
			&etYN,
			&etCalType,
			&etMonths,
			&etWeekofMonth,
			&etAnyDayofWeek,
			&etBPVn,
			&etAppDataTypes
			};
//NOTE: the ei defs have been moved to PROP.H

//Note:	The following table tracks the BACnetPropertyStates choices
static etable *PropertyStates[]={
			&etTF,
			&etBPV,
			&etEvType,
			&etPolar,  
			&etPrChg,
			&etPrState,
			&etPrErr,
			&etReli,
			&etEvState,
			&etDS,
			&etEU
			};
						
typedef struct {
	char	*PropertyName;
	dword	PropID;
	word	StrucOffset;
	octet	ParseType;
	octet	PropGroup;
	word	PropET;
	word	PropFlags;
	} propdescriptor;


//handy macro for clarity in propdescriptor tables:
#define oo(s,m)   (size_t)( (char *)&(((s##_obj_type *)0)->m) - (char *)0 )
//-------------------------------------------------------------------------------------------------
//table of standard object's properties
propdescriptor	AIprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(ai,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(ai,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(ai,go.object_type),	et,		0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(ai,pv),				flt,	0,		   0,	R|Woutofservice,
    "description",			DESCRIPTION,		oo(ai,go.description),	s132,	0,	       0,	O,
    "device-type",			DEVICE_TYPE,		oo(ai,device_type),		s64,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(ai,status_flags),	bits,	0,	   eiStF,	R,
    "event-state",			EVENT_STATE,		oo(ai,state),			et,		0,eiEvState,	R,
    "reliability",			RELIABILITY,		oo(ai,reliability),		et,		0,	 eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(ai,out_of_service),	ebool,  0,	   eiTF,	R,
    "update-interval",		UPDATE_INTERVAL,	oo(ai,update_interval),	uw,		0,	       0,	O,
    "units",				UNITS,				oo(ai,units),			et,		0,	   eiEU,	R,
    "min-pres-value",		MIN_PRES_VALUE,		oo(ai,min_pres_value),	flt,	0,	 	   0,	O,				
    "max-pres-value",		MAX_PRES_VALUE,		oo(ai,max_pres_value),	flt,	0,	       0,	O,
    "resolution",			RESOLUTION,			oo(ai,resolution),		flt,	0,	       0,	O,
    "cov-increment",		COV_INCREMENT,		oo(ai,cov_increment),	flt,	COV,	   0,	O|WithService,
    "time-delay",			TIME_DELAY,			oo(ai,time_delay),		uw,		Intr,	   0,	O|WithService,
    "notification-class",	NOTIFICATION_CLASS,	oo(ai,notification_class),uw,	Intr,	   0,	O|WithService,
    "high-limit",			HIGH_LIMIT,			oo(ai,high_limit),		flt,	Intr,	   0,	O|WithService,
    "low-limit",			LOW_LIMIT,			oo(ai,low_limit),		flt,	Intr,	   0,	O|WithService,
    "deadband",				DEADBAND,			oo(ai,deadband),		flt,	Intr,	   0,	O|WithService,
    "limit-enable",			LIMIT_ENABLE,		oo(ai,limit_enable),	bits,	Intr,eiLimEn,	O|WithService,
    "event-enable",			EVENT_ENABLE,		oo(ai,event_enable),	bits,	Intr,eiEvTr,	O|WithService,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(ai,acked_transitions),bits,	Intr,eiEvTr,	O|WithService,
    "notify-type",			NOTIFY_TYPE,		oo(ai,notify_type),		et,Last|Intr,  eiNT,	O|WithService
	};

propdescriptor	AOprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(ao,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(ao,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(ao,go.object_type),	et,		0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(ao,pv),				flt,	0,	       0,	W|IsCommandable,
    "description",			DESCRIPTION,		oo(ao,go.description),	s132,	0,	       0,	O,
    "device-type",			DEVICE_TYPE,		oo(ao,device_type),		s64,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(ao,status_flags),	bits,	0,	   eiStF,	R,
    "event-state",			EVENT_STATE,		oo(ao,state),			et,		0,eiEvState,	R,
    "reliability",			RELIABILITY,		oo(ao,reliability),		et,		0,	 eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(ao,out_of_service),	ebool,  0,		eiTF,	R,
    "units",				UNITS,				oo(ao,units),			et,		0,	    eiEU,	R,
    "min-pres-value",		MIN_PRES_VALUE,		oo(ao,min_pres_value),	flt,	0,	 	   0,	O,				
    "max-pres-value",		MAX_PRES_VALUE,		oo(ao,max_pres_value),	flt,	0,	       0,	O,
    "resolution",			RESOLUTION,			oo(ao,resolution),		flt,	0,	       0,	O,
    "priority-array",		PRIORITY_ARRAY,		oo(ao,priority_array),	paf,	0,	       0,	R|IsArray,
    "relinquish-default",	RELINQUISH_DEFAULT,	oo(ao,relinquish_default), flt,	0,	       0,	R,
    "cov-increment",		COV_INCREMENT,		oo(ao,cov_increment),	flt,	COV,	   0,	O|WithService,
    "time-delay",			TIME_DELAY,			oo(ao,time_delay),		uw,		Intr,	   0,	O|WithService,
    "notification-class",	NOTIFICATION_CLASS,	oo(ao,notification_class),uw,	Intr,	   0,	O|WithService,
    "high-limit",			HIGH_LIMIT,			oo(ao,high_limit),		flt,	Intr,	   0,	O|WithService,
    "low-limit",			LOW_LIMIT,			oo(ao,low_limit),		flt,	Intr,	   0,	O|WithService,
    "deadband",				DEADBAND,			oo(ao,deadband),		flt,	Intr,	   0,	O|WithService,
    "limit-enable",			LIMIT_ENABLE,		oo(ao,limit_enable),	bits,	Intr,eiLimEn,	O|WithService,
    "event-enable",			EVENT_ENABLE,		oo(ao,event_enable),	bits,	Intr,eiEvTr,	O|WithService,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(ao,acked_transitions),bits,	Intr,eiEvTr,	O|WithService,
    "notify-type",			NOTIFY_TYPE,		oo(ao,notify_type),		et,Last|Intr,	eiNT,	O|WithService
	};

propdescriptor	AVprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(av,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(av,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(av,go.object_type),	et,		0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(av,pv),				flt,	0,	       0,	W|IsCommandable,
    "description",			DESCRIPTION,		oo(av,go.description),	s132,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(av,status_flags),	bits,	0,	   eiStF,	R,
    "event-state",			EVENT_STATE,		oo(av,state),			et,		0,eiEvState,	R,
    "reliability",			RELIABILITY,		oo(av,reliability),		et,		0,	 eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(av,out_of_service),	ebool,  0,		eiTF,	R,
    "units",				UNITS,				oo(av,units),			et,		0,	    eiEU,	R,
    "priority-array",		PRIORITY_ARRAY,		oo(av,priority_array),	paf,	1,		   0,	O|WithGroup|IsArray,
    "relinquish-default",	RELINQUISH_DEFAULT,	oo(av,relinquish_default), flt,	1,		   0,	O|WithGroup,
    "cov-increment",		COV_INCREMENT,		oo(av,cov_increment),	flt,	COV,	   0,	O|WithService,
    "time-delay",			TIME_DELAY,			oo(av,time_delay),		uw,		Intr,	   0,	O|WithService,
    "notification-class",	NOTIFICATION_CLASS,	oo(av,notification_class),uw,	Intr,	   0,	O|WithService,
    "high-limit",			HIGH_LIMIT,			oo(av,high_limit),		flt,	Intr,	   0,	O|WithService,
    "low-limit",			LOW_LIMIT,			oo(av,low_limit),		flt,	Intr,	   0,	O|WithService,
    "deadband",				DEADBAND,			oo(av,deadband),		flt,	Intr,	   0,	O|WithService,
    "limit-enable",			LIMIT_ENABLE,		oo(av,limit_enable),	bits,	Intr,eiLimEn,	O|WithService,
    "event-enable",			EVENT_ENABLE,		oo(av,event_enable),	bits,	Intr,eiEvTr,	O|WithService,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(av,acked_transitions),bits,	Intr,eiEvTr,	O|WithService,
    "notify-type",			NOTIFY_TYPE,		oo(av,notify_type),		et,Last|Intr,	eiNT,	O|WithService
	};

propdescriptor	BIprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(bi,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(bi,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(bi,go.object_type),	et,		0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(bi,pv),				et,		0,	   eiBPV,	R|Woutofservice,
    "description",			DESCRIPTION,		oo(bi,go.description),	s132,	0,	       0,	O,
    "device-type",			DEVICE_TYPE,		oo(bi,device_type),		s64,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(bi,status_flags),	bits,	0,	   eiStF,	R,
    "event-state",			EVENT_STATE,		oo(bi,state),			et,		0,eiEvState,	R,
    "reliability",			RELIABILITY,		oo(bi,reliability),		et,		0,	 eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(bi,out_of_service),	ebool,  0,		eiTF,	R,
    "polarity",				POLARITY,			oo(bi,polarity),		et,		0,	  eiPolar,	R,
    "inactive-text",		INACTIVE_TEXT,		oo(bi,inactive_text),	s64,	1,		   0,	O|WithGroup,
    "active-text",			ACTIVE_TEXT,		oo(bi,active_text),		s64,	1,		   0,	O|WithGroup,
    "change-of-state-time",	CHANGE_OF_STATE_TIME,oo(bi,cos_time),		dt,		2,		   0,	O|WithGroup,
    "change-of-state-count",CHANGE_OF_STATE_COUNT,oo(bi,cos_count),		uw,		2,		   0,	O|WithGroup,
"time-of-state-count-reset",TIME_OF_STATE_COUNT_RESET, oo(bi,
												time_of_state_count_reset),dt,	2,		   0,	O|WithGroup,
    "elapsed-active-time",	ELAPSED_ACTIVE_TIME,oo(bi,elapsed_active_time), ud,	3,		   0,	O|WithGroup,
"time-of-active-time-reset",TIME_OF_ACTIVE_TIME_RESET, oo(bi,
												time_of_active_time_reset),	dt,	3,		   0,	O|WithGroup,
    "time-delay",			TIME_DELAY,			oo(bi,time_delay),		uw,		Intr,	   0,	O|WithService,
    "notification-class",	NOTIFICATION_CLASS,	oo(bi,notification_class),uw,	Intr,	   0,	O|WithService,
    "alarm-value",			ALARM_VALUE,		oo(bi,alarm_value),		et,		Intr,  eiBPV,	O|WithService,
    "event-enable",			EVENT_ENABLE,		oo(bi,event_enable),	bits,	Intr,eiEvTr,	O|WithService,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(bi,acked_transitions),bits,	Intr,eiEvTr,	O|WithService,
    "notify-type",			NOTIFY_TYPE,		oo(bi,notify_type),		et,Last|Intr,	eiNT,	O|WithService
	};

propdescriptor	BOprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(bo,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(bo,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(bo,go.object_type),	et,		0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(bo,pv),				et,		0,	   eiBPV,	W|IsCommandable,
    "description",			DESCRIPTION,		oo(bo,go.description),	s132,	0,	       0,	O,
    "device-type",			DEVICE_TYPE,		oo(bo,device_type),		s64,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(bo,status_flags),	bits,	0,	   eiStF,	R,
    "event-state",			EVENT_STATE,		oo(bo,state),			et,		0, eiEvState,	R,
    "reliability",			RELIABILITY,		oo(bo,reliability),		et,		0,	  eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(bo,out_of_service),	ebool,  0,		eiTF,	R,
    "polarity",				POLARITY,			oo(bo,polarity),		et,		0,	  eiPolar,	R,
    "inactive-text",		INACTIVE_TEXT,		oo(bo,inactive_text),	s64,	1,		   0,	O|WithGroup,
    "active-text",			ACTIVE_TEXT,		oo(bo,active_text),		s64,	1,		   0,	O|WithGroup,
    "change-of-state-time",	CHANGE_OF_STATE_TIME,oo(bo,cos_time),		dt,		2,		   0,	O|WithGroup,
    "change-of-state-count",CHANGE_OF_STATE_COUNT,oo(bo,cos_count),		uw,		2,		   0,	O|WithGroup,
"time-of-state-count-reset",TIME_OF_STATE_COUNT_RESET, oo(bo,
												time_of_state_count_reset),dt,	2,		   0,	O|WithGroup,
    "elapsed-active-time",	ELAPSED_ACTIVE_TIME,oo(bo,elapsed_active_time), ud,	3,		   0,	O|WithGroup,
"time-of-active-time-reset",TIME_OF_ACTIVE_TIME_RESET, oo(bo,
												time_of_active_time_reset),dt,	3,		   0,	O|WithGroup,
    "minimum-off-time",		MINIMUM_OFF_TIME,	oo(bo,min_off_time), 	ud,		0,	       0,	O,
    "minimum-on-time",		MINIMUM_ON_TIME,	oo(bo,min_on_time),		ud,		0,	       0,	O,
    "priority-array",		PRIORITY_ARRAY,		oo(bo,priority_array),	pab,	0,	   eiBPVn,	R|IsArray,
    "relinquish-default",	RELINQUISH_DEFAULT,	oo(bo,relinquish_default), et,	0,	   eiBPV,	R,
    "time-delay",			TIME_DELAY,			oo(bo,time_delay),		uw,		Intr,	   0,	O|WithService,
    "notification-class",	NOTIFICATION_CLASS,	oo(bo,notification_class),uw,	Intr,	   0,	O|WithService,
    "feedback-value",		FEEDBACK_VALUE,		oo(bo,feedback_value),	et,		Intr,  eiBPV,	O|WithService,
    "event-enable",			EVENT_ENABLE,		oo(bo,event_enable),	bits,	Intr,eiEvTr,	O|WithService,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(bo,acked_transitions),bits,	Intr,eiEvTr,	O|WithService,
    "notify-type",			NOTIFY_TYPE,		oo(bo,notify_type),		et,Last|Intr,	eiNT,	O|WithService
	};

propdescriptor	BVprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(bv,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(bv,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(bv,go.object_type),	et,		0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(bv,pv),				et,		0,	   eiBPV,	R|Woutofservice|IsCommandable,
    "description",			DESCRIPTION,		oo(bv,go.description),	s132,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(bv,status_flags),	bits,	0,	  eiStF,	R,
    "event-state",			EVENT_STATE,		oo(bv,state),			et,		0, eiEvState,	R,
    "reliability",			RELIABILITY,		oo(bv,reliability),		et,		0,	  eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(bv,out_of_service),	ebool,  0,		eiTF,	R,
    "inactive-text",		INACTIVE_TEXT,		oo(bv,inactive_text),	s64,	1,		   0,	O|WithGroup,
    "active-text",			ACTIVE_TEXT,		oo(bv,active_text),		s64,	1,		   0,	O|WithGroup,
    "change-of-state-time",	CHANGE_OF_STATE_TIME,oo(bv,cos_time),		dt,		2,		   0,	O|WithGroup,
    "change-of-state-count",CHANGE_OF_STATE_COUNT,oo(bv,cos_count),		uw,		2,		   0,	O|WithGroup,
"time-of-state-count-reset",TIME_OF_STATE_COUNT_RESET, oo(bv,
												time_of_state_count_reset),dt,	2,		   0,	O|WithGroup,
    "elapsed-active-time",	ELAPSED_ACTIVE_TIME,oo(bv,elapsed_active_time), ud,	3,		   0,	O|WithGroup,
"time-of-active-time-reset",TIME_OF_ACTIVE_TIME_RESET, oo(bv,
												time_of_active_time_reset),dt,	3,		   0,	O|WithGroup,
    "minimum-off-time",		MINIMUM_OFF_TIME,	oo(bv,min_off_time), 	ud,		0,	       0,	O,
    "minimum-on-time",		MINIMUM_ON_TIME,	oo(bv,min_on_time),		ud,		0,	       0,	O,
    "priority-array",		PRIORITY_ARRAY,		oo(bv,priority_array),	pab,	4,	   eiBPVn,	O|WithGroup|IsArray,
    "relinquish-default",	RELINQUISH_DEFAULT,	oo(bv,relinquish_default), et,	4,	   eiBPV,	O|WithGroup,
    "time-delay",			TIME_DELAY,			oo(bv,time_delay),		uw,		Intr,	   0,	O|WithService,
    "notification-class",	NOTIFICATION_CLASS,	oo(bv,notification_class),uw,	Intr,	   0,	O|WithService,
    "alarm-value",			ALARM_VALUE,		oo(bv,alarm_value),		et,		Intr,  eiBPV,	O|WithService,
    "event-enable",			EVENT_ENABLE,		oo(bv,event_enable),	bits,	Intr,eiEvTr,	O|WithService,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(bv,acked_transitions),bits,	Intr,eiEvTr,	O|WithService,
    "notify-type",			NOTIFY_TYPE,		oo(bv,notify_type),		et,Last|Intr,	eiNT,	O|WithService
	};

propdescriptor	CLprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(calendar,go.object_id),ob_id,0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(calendar,go.object_name),s32,0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(calendar,go.object_type),et,	0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(calendar,pv),		ebool,  0,    	eiTF,	R,
    "description",			DESCRIPTION,		oo(calendar,go.description),s132,0,	       0,	O,
    "date-list",			DATE_LIST,			oo(calendar,date_list),	calist,	Last,	   0,	R
	};

propdescriptor	CMprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(command,go.object_id),ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(command,go.object_name),s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(command,go.object_type),et,	0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(command,pv),			uw,		0,	       0,	W,
    "description",			DESCRIPTION,		oo(command,go.description),s132,0,	       0,	O,
    "in-process",			IN_PROCESS,			oo(command,in_process),	ebool,  0,    	eiTF,	R,
    "all-writes-successful",ALL_WRITES_SUCCESSFUL,oo(command,
    											all_writes_successful),	ebool,  0,    	eiTF,	R,
    "action",				ACTION,				oo(command,action),		act,	0,	       0,	R|IsArray,
    "action-text",			ACTION_TEXT,		oo(command,action_text),actext,	Last,	   0,	O|IsArray
	};

propdescriptor	DVprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(device,go.object_id),ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(device,go.object_name),s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(device,go.object_type),et,	0,eiObjectTypes,R,
    "system-status",		SYSTEM_STATUS,		oo(device,system_status),et,	0,	    eiDS,	R,
    "vendor-name",			VENDOR_NAME,		oo(device,vendor_name),	s64,	0,	       0,	R,
    "vendor-identifier",	VENDOR_IDENTIFIER,	oo(device,vendor_id),	uw,		0,	       0,	R,
    "model-name",			MODEL_NAME,			oo(device,model_name),	s32,	0,	       0,	R,
    "firmware-revision",	FIRMWARE_REVISION,	oo(device,firmware_rev),s10,	0,	       0,	R,
"application-software-version", APPLICATION_SOFTWARE_VERSION,oo(device,
												application_software_ver),s32,	0,	       0,	R,
    "location",				LOCATION,			oo(device,location),	s64,	0,	       0,	O,
    "description",			DESCRIPTION,		oo(device,go.description),s132,	0,	       0,	O,
    "protocol-version",		PROTOCOL_VERSION,	oo(device,protocol_ver),uw,		0,	       0,	R,
"protocol-conformance-class",PROTOCOL_CONFORMANCE_CLASS,oo(device,
												protocol_conf_class),	uw,		0,	       0,	R,
"protocol-services-supported",PROTOCOL_SERVICES_SUPPORTED,oo(device,
												protocol_services_supported),pss,0,	       0,	R,
"protocol-object-types-supported",PROTOCOL_OBJECT_TYPES_SUPPORTED,oo(device,
												object_types_supported),pos,	0,	       0,	R,
    "object-list",			OBJECT_LIST,		oo(device,object_list),	looref,	0,	       0,	R|IsArray,
    "max-APDU-length-accepted", MAX_APDU_LENGTH_ACCEPTED,oo(device,
    											max_apdu_length_accepted),uw,	0,	       0,	R,
    "segmentation-supported", SEGMENTATION_SUPPORTED,oo(device,
    											segmentation_supported),et,		0,	eiSegOpt,	R,
    "vt-classes-supported",	VT_CLASSES_SUPPORTED,oo(device,
    											vt_classes_supported),	vtcl,	VT,		   0,	O|WithService|WithGroup,
    "active-vt-sessions",	ACTIVE_VT_SESSIONS,	0,						none,	VT,		   0,	O|WithService|WithGroup,
    "local-time",			LOCAL_TIME,			oo(device,local_time),	ttime,	0,	       0,	O,
    "local-date",			LOCAL_DATE,			oo(device,local_date),	ddate,	0,	       0,	O,
    "utc-offset",			UTC_OFFSET,			oo(device,utc_offset),	ssint,	0,	       0,	O,  //MAG 13 FEB 2001 change parse from flt to ssint
"daylight-savings-status",	DAYLIGHT_SAVINGS_STATUS,oo(device,
												day_savings_status),	ebool,  0,		eiTF,	O,
    "apdu-segment-timeout",	APDU_SEGMENT_TIMEOUT,oo(device,
    											apdu_segment_timeout),	uw,		SEGSVC,	   0,	O|WithService,
    "apdu-timeout",			APDU_TIMEOUT,		oo(device,apdu_timeout),uw,		0,	       0,	R,
"number-of-APDU-retries",	NUMBER_OF_APDU_RETRIES,oo(device,
												number_apdu_retries),	uw,		0,	       0,	R,
    "list-of-session-keys",	LIST_OF_SESSION_KEYS,oo(device,
    											list_session_keys),		skeys,	0,	       0,	O,
"time-synchronization-recipients",TIME_SYNCHRONIZATION_RECIPIENTS,oo(device,
    											time_synch_recipients),	tsrecip,TMASTER,   0,	O|WithService,
    "max-master",			MAX_MASTER,			oo(device,max_master),	u127,MSTPMASTER,   0,	O|WithService,
    "max-info-frames",		MAX_INFO_FRAMES,	oo(device,max_info_frames),uw,MSTPMASTER,  0,	O|WithService,
"device-address-binding",	DEVICE_ADDRESS_BINDING,oo(device,
												device_add_binding),	dabind,	Last,	   0,	R
	};

propdescriptor	EEprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(ee,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(ee,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(ee,go.object_type),	et,		0,eiObjectTypes,R,
    "description",			DESCRIPTION,		oo(ee,go.description),	s132,	0,	       0,	O,
    "event-type",			EVENT_TYPE,			oo(ee,
    											parameter_list.event_type),	et,	0,	eiEvType,	R,
    "notify-type",			NOTIFY_TYPE,		oo(ee,notify_type),		et,		0,	    eiNT,	R,
    "event-parameters",		EVENT_PARAMETERS,	oo(ee,parameter_list),	evparm,	0,	       0,	R,
"object-property-reference",OBJECT_PROPERTY_REFERENCE,oo(ee,obj_prop_ref),propref,0,       0,	R,
    "event-state",			EVENT_STATE,		oo(ee,state),			et,		0, eiEvState,	R,
    "event-enable",			EVENT_ENABLE,		oo(ee,event_enable),	bits,	0,	 eiEvTr,	R,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(ee,acked_transitions),bits,	0,	 eiEvTr,	R,
    "notification-class",	NOTIFICATION_CLASS,	oo(ee,notification_class),uw,	1,		   0,	O|NotWithGroup2,
    "recipient",			RECIPIENT,			oo(ee,recipient),		recip,	2,		   0,	O|NotWithGroup1,
    "process-identifier",	PROCESS_IDENTIFIER,	oo(ee,process_id),		uw,		2,		   0,	O|NotWithGroup1,
    "priority",				PRIORITY,			oo(ee,priority),		uw,		2,		   0,	O|NotWithGroup1,
"issue-confirmed-notifications",ISSUE_CONFIRMED_NOTIFICATIONS,oo(ee,
												issue_conf_notifications),ebool,Last|2,	eiTF,	O|NotWithGroup1
	};

propdescriptor	FLprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(file,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(file,go.object_name),s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(file,go.object_type),et,		0,eiObjectTypes,R,
    "description",			DESCRIPTION,		oo(file,go.description),s132,	0,	       0,	O,
    "file-type",			FILE_TYPE,			oo(file,file_type),		s32,	0,	       0,	R,
    "file-size",			FILE_SIZE,			oo(file,file_size),		ud,		0,	       0,	R,
    "modification-date",	MODIFICATION_DATE,	oo(file,mod_date),		dt,		0,	       0,	R,
    "archive",				ARCHIVE,			oo(file,archive),		ebool,  0,		eiTF,	W,
    "read-only",			READ_ONLY,			oo(file,read_only),		ebool,  0,		eiTF,	R,
    "file-access-method",	FILE_ACCESS_METHOD,	oo(file,access_method),	et,		Last,   eiFAM,	R
	};

propdescriptor	GRprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(group,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(group,go.object_name),s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(group,go.object_type),et,	0,eiObjectTypes,R,
    "description",			DESCRIPTION,		oo(group,go.description),s132,	0,	       0,	O,
    "list-of-group-members",LIST_OF_GROUP_MEMBERS,oo(group,
    											list_of_group_members),	raslist,0,	       0,	R,
    "present-value",		PRESENT_VALUE,		0,						none,	Last,	   0,	R
	};

propdescriptor	LPprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(loop,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(loop,go.object_name),s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(loop,go.object_type),et,		0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(loop,pv),			flt,	0,	       0,	R,
    "description",			DESCRIPTION,		oo(loop,go.description),s132,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(loop,status_flags),	bits,	0,	   eiStF,	R,
    "event-state",			EVENT_STATE,		oo(loop,state),			et,		0, eiEvState,	R,
    "reliability",			RELIABILITY,		oo(loop,reliability),	et,		0,	  eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(loop,out_of_service),ebool,  0,		eiTF,	R,
    "update-interval",		UPDATE_INTERVAL,	oo(loop,update_interval),uw,	0,	       0,	O,
    "output-units",			OUTPUT_UNITS,		oo(loop,output_units),	et,		0,	    eiEU,	R,
"manipulated-variable-reference",MANIPULATED_VARIABLE_REFERENCE,oo(loop,
												man_var_ref),			propref,0,	       0,	R,
"controlled-variable-reference",CONTROLLED_VARIABLE_REFERENCE,oo(loop,
												cont_var_ref),			propref,0,	       0,	R,
"controlled-variable-value",CONTROLLED_VARIABLE_VALUE,oo(loop,
												cont_var_value),		flt,	0,	       0,	R,
"controlled-variable-units",CONTROLLED_VARIABLE_UNITS,oo(loop,
												cont_var_units),		et,		0,	    eiEU,	R,
    "setpoint-reference",	SETPOINT_REFERENCE,	oo(loop,setpoint_ref),	setref,	0,	       0,	R,
    "setpoint",				SETPOINT,			oo(loop,setpoint),		flt,	0,	       0,	R,
    "action",				ACTION,				oo(loop,action),		et,		0,	eiLoopAct,	R,
"proportional-constant",	PROPORTIONAL_CONSTANT,oo(loop,
												proportional_const),	flt,	1,		   0,	O|WithGroup,
"proportional-constant-units",PROPORTIONAL_CONSTANT_UNITS,oo(loop,
												proportional_const_units),et,	1,		eiEU,	O|WithGroup,
"integral-constant",		INTEGRAL_CONSTANT,oo(loop,
												integral_const),		flt,	2,		   0,	O|WithGroup,
"integral-constant-units",	INTEGRAL_CONSTANT_UNITS,oo(loop,
												integral_const_units),	et,		2,		eiEU,	O|WithGroup,
"derivative-constant",		DERIVATIVE_CONSTANT,oo(loop,
												derivative_const),		flt,	3,		   0,	O|WithGroup,
"derivative-constant-units",DERIVATIVE_CONSTANT_UNITS,oo(loop,
												derivative_const_units),et,		3,		eiEU,	O|WithGroup,
    "bias",					BIAS,				oo(loop,bias),			flt,	0,	       0,	O,
    "maximum-output",		MAXIMUM_OUTPUT,		oo(loop,max_output),	flt,	0,	       0,	O,
    "minimum-output",		MINIMUM_OUTPUT,		oo(loop,min_output),	flt,	0,	       0,	O,
    "priority-for-writing",	PRIORITY_FOR_WRITING,oo(loop,
    											priority_for_writing),	uw,		0,	 	   0,	R,	
    "cov-increment",		COV_INCREMENT,		oo(loop,cov_increment),	flt,	COV,	   0,	O|WithService,
    "time-delay",			TIME_DELAY,			oo(loop,time_delay),	uw,		Intr,	   0,	O|WithService,
    "notification-class",	NOTIFICATION_CLASS,	oo(loop,notification_class),uw,	Intr,	   0,	O|WithService,
    "error-limit",			ERROR_LIMIT,		oo(loop,error_limit),	flt,	Intr,	   0,	O|WithService,
    "event-enable",			EVENT_ENABLE,		oo(loop,event_enable),	bits,	Intr,eiEvTr,	O|WithService,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(loop,acked_transitions),bits,Intr,eiEvTr,	O|WithService,
    "notify-type",			NOTIFY_TYPE,		oo(loop,notify_type),	et,Last|Intr,	eiNT,	O|WithService
	};

propdescriptor	MIprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(mi,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(mi,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(mi,go.object_type),	et,		0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(mi,pv),				uw,		0,		   0,	R|Woutofservice,
    "description",			DESCRIPTION,		oo(mi,go.description),	s132,	0,	       0,	O,
    "device-type",			DEVICE_TYPE,		oo(mi,device_type),		s64,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(mi,status_flags),	bits,	0,	  eiStF,	R,
    "event-state",			EVENT_STATE,		oo(mi,state),			et,		0, eiEvState,	R,
    "reliability",			RELIABILITY,		oo(mi,reliability),		et,		0,	  eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(mi,out_of_service),	ebool,  0,		eiTF,	R,
	"number-of-states",		NUMBER_OF_STATES,	oo(mi,num_of_states),	uw,		0,	       0,	R,
    "state-text",			STATE_TEXT,			oo(mi,state_text),		statext,0,	       0,	O|IsArray,
    "time-delay",			TIME_DELAY,			oo(mi,time_delay),		uw,		Intr,	   0,	O|WithService,
    "notification-class",	NOTIFICATION_CLASS,	oo(mi,notification_class),uw,	Intr,	   0,	O|WithService,
    "alarm-values",			ALARM_VALUES,		oo(mi,alarm_values),	stavals,Intr,	   0,	O|WithService,
    "fault-values",			FAULT_VALUES,		oo(mi,fault_values),	stavals,Intr,	   0,	O|WithService,
    "event-enable",			EVENT_ENABLE,		oo(mi,event_enable),	bits,	Intr,eiEvTr,	O|WithService,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(mi,acked_transitions),bits,	Intr,eiEvTr,	O|WithService,
    "notify-type",			NOTIFY_TYPE,		oo(mi,notify_type),		et,Last|Intr,	eiNT,	O|WithService
	};

  
propdescriptor	MOprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(mo,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(mo,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(mo,go.object_type),	et,		0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(mo,pv),				uw,		0,	       0,	W|IsCommandable,
    "description",			DESCRIPTION,		oo(mo,go.description),	s132,	0,	       0,	O,
    "device-type",			DEVICE_TYPE,		oo(mo,device_type),		s64,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(mo,status_flags),	bits,	0,	   eiStF,	R,
    "event-state",			EVENT_STATE,		oo(mo,state),			et,		0, eiEvState,	R,
    "reliability",			RELIABILITY,		oo(mo,reliability),		et,		0,	  eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(mo,out_of_service),	ebool,  0,		eiTF,	R,
	"number-of-states",		NUMBER_OF_STATES,	oo(mo,num_of_states),	uw,		0,	       0,	R,
    "state-text",			STATE_TEXT,			oo(mo,state_text),		statext,0,	       0,	O|IsArray,
    "priority-array",		PRIORITY_ARRAY,		oo(mo,priority_array),	pau,	0,	       0,	R|IsArray,
    "relinquish-default",	RELINQUISH_DEFAULT,	oo(mo,relinquish_default), uw,	0,	       0,	R,
    "time-delay",			TIME_DELAY,			oo(mo,time_delay),		uw,		Intr,	   0,	O|WithService,
    "notification-class",	NOTIFICATION_CLASS,	oo(mo,notification_class),uw,	Intr,	   0,	O|WithService,
    "feedback-value",		FEEDBACK_VALUE,		oo(mo,feedback_value),	uw,		Intr,	   0,	O|WithService,
    "event-enable",			EVENT_ENABLE,		oo(mo,event_enable),	bits,	Intr,eiEvTr,	O|WithService,
    "acked-transitions",	ACKED_TRANSITIONS,	oo(mo,acked_transitions),bits,	Intr,eiEvTr,	O|WithService,
    "notify-type",			NOTIFY_TYPE,		oo(mo,notify_type),		et,Last|Intr,	eiNT,	O|WithService
	};

propdescriptor	NCprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(nc,go.object_id),	ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(nc,go.object_name),	s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(nc,go.object_type),	et,		0,eiObjectTypes,R,
    "description",			DESCRIPTION,		oo(nc,go.description),	s132,	0,	       0,	O,
    "notification-class",	NOTIFICATION_CLASS,	oo(nc,notification_class), uw,	0,	       0,	R,
    "priority",				PRIORITY,			oo(nc,priority),		uw,		0,	       0,	R|IsArray,
    "ack-required",			ACK_REQUIRED,		oo(nc,ack_required),	bits,	0,	  eiEvTr,	R,
    "recipient-list",		RECIPIENT_LIST,		oo(nc,recipient_list),reciplist,Last,	   0,	R	
  	};

propdescriptor	PRprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(program,go.object_id),ob_id,	0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(program,go.object_name),s32,	0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(program,go.object_type),et,	0,eiObjectTypes,R,
    "program-state",		PROGRAM_STATE,		oo(program,prog_state),	et,		0, eiPrState,	R,
    "program-change",		PROGRAM_CHANGE,		oo(program,prog_change),et,		0,	 eiPrChg,	W,
    "reason-for-halt",		REASON_FOR_HALT,	oo(program,
    											reason_for_halt),		et,		1,	 eiPrErr,	O|WithGroup,
    "description-of-halt",	DESCRIPTION_OF_HALT,oo(program,
    											description_of_halt),	s64,	1,		   0,	O|WithGroup,
    "program-location",		PROGRAM_LOCATION,	oo(program,prog_location),s64,	0,	       0,	O,
    "description",			DESCRIPTION,		oo(program,go.description),s132,0,	       0,	O,
    "instance-of",			INSTANCE_OF,		oo(program,instance_of),s64,	0,	       0,	O,
    "status-flags",			STATUS_FLAGS,		oo(program,status_flags),bits,	0,	   eiStF,	R,
    "reliability",			RELIABILITY,		oo(program,reliability),et,		0,	  eiReli,	O,
    "out-of-service",		OUT_OF_SERVICE,		oo(program,out_of_service),ebool,Last,	eiTF,	R
  	};

propdescriptor	SCprops[]={
//	"property name",		property identifier,	struc offset,		parse,	group,	table,	qualifiers
	"object-identifier",	OBJECT_IDENTIFIER,	oo(schedule,go.object_id),ob_id,0,	       0,	R,
    "object-name",			OBJECT_NAME,		oo(schedule,go.object_name),s32,0,	       0,	R,
    "object-type",			OBJECT_TYPE,		oo(schedule,go.object_type),et,	0,eiObjectTypes,R,
    "present-value",		PRESENT_VALUE,		oo(schedule,pv),		et/*none*/,	0,	  	   eiBPV/*0*/,	R,  // MAG error- needs a parse type - trying et
    "description",			DESCRIPTION,		oo(schedule,go.description),s132,0,	       0,	O,
    "effective-period",		EFFECTIVE_PERIOD,	oo(schedule,
    											effective_period),		dtrange,0,		   0,	R,
    "weekly-schedule",		WEEKLY_SCHEDULE,	oo(schedule,
    											weekly_schedule),		wsched,	1,		   0,	O|AtLeast1|IsArray,
    "exception-schedule",	EXCEPTION_SCHEDULE,	oo(schedule,
    											exception_schedule),	xsched,	1,		   0,	O|AtLeast1|IsArray,
"list-of-object-property-references",
			LIST_OF_OBJECT_PROPERTY_REFERENCES,	oo(schedule,
												list_obj_prop_ref),		lopref,	0,		   0,	R,
    "priority-for-writing",	PRIORITY_FOR_WRITING,oo(schedule,
    											priority_for_writing),	u16,	Last,	   0,	R
  };

typedef struct {
	word			sotSize;					//size of one of these objects in bytes
	propdescriptor	*sotProps;					//pointer to table of property descriptors
	} stdobjtype;

stdobjtype	StdObjects[]={
	sizeof(ai_obj_type),					AIprops,
	sizeof(ao_obj_type),					AOprops,
	sizeof(av_obj_type),					AVprops,
	sizeof(bi_obj_type),					BIprops,
	sizeof(bo_obj_type),					BOprops,
	sizeof(bv_obj_type),					BVprops,
	sizeof(calendar_obj_type),				CLprops,
	sizeof(command_obj_type),				CMprops,
	sizeof(device_obj_type),				DVprops,
	sizeof(ee_obj_type),					EEprops,
	sizeof(file_obj_type),					FLprops,
	sizeof(group_obj_type),					GRprops,
	sizeof(loop_obj_type),					LPprops,
	sizeof(mi_obj_type),					MIprops,
	sizeof(mo_obj_type),					MOprops,
	sizeof(nc_obj_type),					NCprops,
	sizeof(program_obj_type),				PRprops,
	sizeof(schedule_obj_type),				SCprops
	};

typedef struct {
	dword	dptpropid;							//property id
	octet	dptparsetype;						//parse type
	word	dptpet;								//parse type et if applicable
	word	dptflags;							//only IsArray used 
	} defparsetypes; 

typedef struct {
	word	npt;								//number of string pointers which follow
	defparsetypes dpt[135];						//the default parse types
	} dpttable;

//Special table for default parse types
static dpttable dParseTypes={
				135,							//135 parse types						
				{{ACKED_TRANSITIONS,bits,eiEvTr,0},
				{ACK_REQUIRED,bits,eiEvTr,0},
				{ACTION,act,0,IsArray},
				{ACTION,et,eiLoopAct,0},
				{ACTION_TEXT,actext,0,IsArray},
				{ACTIVE_TEXT,s64,0},
				{ACTIVE_VT_SESSIONS,none,0,0},
				{ALARM_VALUE,et,eiBPV,0},
				{ALARM_VALUES,stavals,0,0},
				{ALL,none,0,0},
				{ALL_WRITES_SUCCESSFUL,ebool,eiTF,0},
				{APDU_SEGMENT_TIMEOUT,uw,0,0},
				{APDU_TIMEOUT,uw,0,0},
				{APPLICATION_SOFTWARE_VERSION,s32,0,0},
				{ARCHIVE,ebool,eiTF,0},
				{BIAS,flt,0,0},
				{CHANGE_OF_STATE_COUNT,uw,0,0},
				{CHANGE_OF_STATE_TIME,dt,0,0},
				{NOTIFICATION_CLASS,uw,0,0},
				{blank1,0,0,0},
				{CONTROLLED_VARIABLE_REFERENCE,propref,0,0},
				{CONTROLLED_VARIABLE_UNITS,et,eiEU,0},
				{CONTROLLED_VARIABLE_VALUE,flt,0,0},
				{COV_INCREMENT,flt,0,0},
				{DATE_LIST,calist,0,0},
				{DAYLIGHT_SAVINGS_STATUS,ebool,eiTF,0},
				{DEADBAND,flt,0,0},
				{DERIVATIVE_CONSTANT,flt,0,0},
				{DERIVATIVE_CONSTANT_UNITS,et,eiEU,0},
				{DESCRIPTION,s132,0,0},
				{DESCRIPTION_OF_HALT,s64,0,0},
				{DEVICE_ADDRESS_BINDING,dabind,0,0},
				{DEVICE_TYPE,s64,0,0},
				{EFFECTIVE_PERIOD,dtrange,0,0},
				{ELAPSED_ACTIVE_TIME,ud,0,0},
				{ERROR_LIMIT,flt,0,0},
				{EVENT_ENABLE,bits,eiEvTr,0},
				{EVENT_STATE,et,eiEvState,0},
				{EVENT_TYPE,et,eiEvType,0},
				{EXCEPTION_SCHEDULE,xsched,0,IsArray},
				{FAULT_VALUES,stavals,0,0},
				{FEEDBACK_VALUE,uw,0,0},
				{FEEDBACK_VALUE,et,eiBPV,0},
				{FILE_ACCESS_METHOD,et,eiFAM,0},
				{FILE_SIZE,ud,0,0},
				{FILE_TYPE,s32,0,0},
				{FIRMWARE_REVISION,s10,0,0},
				{HIGH_LIMIT,flt,0,0},
				{INACTIVE_TEXT,s64,0,0},
				{IN_PROCESS,ebool,eiTF,0},
				{INSTANCE_OF,s64,0,0},
				{INTEGRAL_CONSTANT,flt,0,0},
				{INTEGRAL_CONSTANT_UNITS,et,eiEU,0},
				{ISSUE_CONFIRMED_NOTIFICATIONS,ebool,eiTF,0},
				{LIMIT_ENABLE,bits,eiLimEn,0},
				{LIST_OF_GROUP_MEMBERS,raslist,0,0},
				{LIST_OF_OBJECT_PROPERTY_REFERENCES,lopref,0,0},
				{LIST_OF_SESSION_KEYS,skeys,0,0},
				{LOCAL_DATE,none,0,0},
				{LOCAL_TIME,none,0,0},
				{LOCATION,s64,0,0},
				{LOW_LIMIT,flt,0,0},
				{MANIPULATED_VARIABLE_REFERENCE,propref,0,0},
				{MAXIMUM_OUTPUT,flt,0,0},
				{MAX_APDU_LENGTH_ACCEPTED,uw,0,0},
				{MAX_INFO_FRAMES,uw,0,0},
				{MAX_MASTER,u127,0,0},
				{MAX_PRES_VALUE,flt,0,0},
				{MINIMUM_OFF_TIME,ud,0,0},
				{MINIMUM_ON_TIME,ud,0,0},
				{MINIMUM_OUTPUT,flt,0,0},
				{MIN_PRES_VALUE,flt,0,0},				
				{MODEL_NAME,s32,0,0},
				{MODIFICATION_DATE,dt,0,0},
				{NOTIFY_TYPE,et,eiNT,0},
				{NUMBER_OF_APDU_RETRIES,uw,0,0},
				{NUMBER_OF_STATES,uw,0,0},
				{OBJECT_IDENTIFIER,ob_id,0,0},
				{OBJECT_LIST,none,0,IsArray},
				{OBJECT_NAME,s32,0,0},
				{OBJECT_PROPERTY_REFERENCE,propref,0},
				{OBJECT_TYPE,et,eiObjectTypes,0}, 
				{ISOPTIONAL,none,0,0},
				{OUT_OF_SERVICE,ebool,eiTF,0},
				{OUTPUT_UNITS,et,eiEU,0},
				{EVENT_PARAMETERS,evparm,0,0},
				{POLARITY,et,eiPolar,0},
				{PRESENT_VALUE,flt,0,0},
				{PRESENT_VALUE,et,eiBPV,0},
				{PRESENT_VALUE,ebool,eiTF,0},
				{PRESENT_VALUE,uw,0,0},
				{PRESENT_VALUE,none,0,0},
				{PRIORITY,uw,0,IsArray},
				{PRIORITY_ARRAY,paf,0,IsArray},
				{PRIORITY_ARRAY,pab,eiBPV,IsArray},
				{PRIORITY_ARRAY,pau,0,0},
				{PRIORITY_FOR_WRITING,uw,0,0},
				{PRIORITY_FOR_WRITING,u16,0,0},
				{PROCESS_IDENTIFIER,uw,0,0},
				{PROGRAM_CHANGE,et,eiPrChg,0},
				{PROGRAM_LOCATION,s64,0,0},
				{PROGRAM_STATE,et,eiPrState,0},
				{PROPORTIONAL_CONSTANT,flt,0,0},
				{PROPORTIONAL_CONSTANT_UNITS,et,eiEU,0},
				{PROTOCOL_CONFORMANCE_CLASS,uw,0,0},
				{PROTOCOL_OBJECT_TYPES_SUPPORTED,pos,0,0},
				{PROTOCOL_SERVICES_SUPPORTED,pss,0,0},
				{PROTOCOL_VERSION,uw,0,0},
				{READ_ONLY,ebool,eiTF,0},
				{REASON_FOR_HALT,et,eiPrErr,0},
				{RECIPIENT,recip,0,0},
				{RECIPIENT_LIST,reciplist,0,0},
				{RELIABILITY,et,eiReli,0},
				{RELINQUISH_DEFAULT,flt,0,0},
				{RELINQUISH_DEFAULT,et,eiBPV,0},
				{RELINQUISH_DEFAULT,uw,0,0},
				{REQUIRED,none,0,0},
				{RESOLUTION,flt,0,0},
				{SEGMENTATION_SUPPORTED,et,eiSegOpt,0},
				{SETPOINT,flt,0,0},
				{SETPOINT_REFERENCE,setref,0,0},
				{STATE_TEXT,statext,0,IsArray},
				{STATUS_FLAGS,bits,0,0},
				{SYSTEM_STATUS,et,eiDS,0},
				{TIME_DELAY,uw,0,0},
				{TIME_OF_ACTIVE_TIME_RESET,dt,0,0},
				{TIME_OF_STATE_COUNT_RESET,dt,0,0},
				{TIME_SYNCHRONIZATION_RECIPIENTS,tsrecip,0,0},
				{UNITS,et,eiEU,0},
				{UPDATE_INTERVAL,uw,0,0},
				{UTC_OFFSET,sw,0,0},
				{VENDOR_IDENTIFIER,uw,0,0},
				{VENDOR_NAME,s64,0,0},
				{VT_CLASSES_SUPPORTED,vtcl,0,0},
				{WEEKLY_SCHEDULE,wsched,0,IsArray}}
				};
#endif //__STDOBJPR_H_INCLUDED
