//DB.H
//Base Datatype Defitions for BACnet
#ifndef __DB_H_INCLUDED
#define __DB_H_INCLUDED

#define	true				1
#define	false				0

typedef unsigned char boolean;
typedef unsigned char octet;
typedef unsigned long dword;
typedef unsigned short word;

/*   ---------- BACnet base types --------------- */

enum BACnetAction {DIRECT, REVERSE};
enum BACnetBinaryPV {INACTIVE, ACTIVE};

#define	NotAnArray		0xFFFF					//value for pa_index fields if not an array

enum ActionValueType {BPV,UNS,FLT};				//binaryPV, unsigned word or float 

typedef struct tagActionCommand {         
struct tagActionCommand *next;
	dword				device_id;
	dword				object_id;
	dword				property_id;
	word				pa_index;
enum ActionValueType	value_type;
	union {
		enum BACnetBinaryPV	bproperty_value; 	//binary value
		word				uproperty_value;	//unsigned word
		float				fproperty_value;	//float
		}				av;
	octet				priority;
	word				post_delay;
	boolean				quit_on_failure;
	boolean				write_successful;
	} BACnetActionCommand;

typedef struct tagTextList {
struct tagTextList	*next;
	char			text[132];
	} TextList;

typedef struct {
   word		network_number;
   octet	mac_address[8];
   word		address_size;
   } BACnetAddress;

typedef struct tagAddressBinding {
struct tagAddressBinding	*next;
	dword					device_object_id;
	BACnetAddress			device_address;
   } BACnetAddressBinding;

typedef struct tagAlarmValues {
struct tagAlarmValues	*next;
	word 				value;
	} BACnetAlarmValues;

#define dontcare			0xFF				//for date and time fields

typedef struct {
//each field is one octet to match encoding, 0xFF means unknown or don't care
	octet	year;								//year - 1900
	octet	month;            					//January = 1
	octet	day_of_month;  
	octet	day_of_week;						//Sunday = 1
	} BACnetDate;

typedef struct {
//each field is one octet to match encoding, 0xFF means unknown or don't care
	octet	hour;								//24 hr clock
	octet	minute;
	octet	second;
	octet	hundredths;
   } BACnetTime;

typedef struct {
	BACnetDate date;
	BACnetTime time;
	} BACnetDateTime;

typedef struct {
	BACnetDate start_date;
	BACnetDate end_date;
	} BACnetDateRange;

typedef struct {
//each field is one octet to match encoding, 0xFF means unknown or don't care
	octet	month;								//January = 1
	octet	week;								//week 1 = days numbered 1-7 and so on
	octet	day;								//Monday = 1
	} BACnetWeekNDay;

typedef struct tagCalendarEntry {
struct tagCalendarEntry *next;
	octet					choice;
	union {
		BACnetDate			date;
		BACnetDateRange		date_range;
		BACnetWeekNDay		weekNday;
		}					u;
	} BACnetCalendarEntry;

typedef struct tagRecipient {
struct tagRecipient 	*next;
	octet					choice;    			//0 = device, 1 = address
	union {
		dword				device;
		BACnetAddress		address;
		}					u;
	} BACnetRecipient;

typedef struct tagDestination {
struct tagDestination	*next;
	octet				valid_days;
	BACnetTime			from_time;
	BACnetTime			to_time;
	BACnetRecipient		recipient;
	word				process_id;
	boolean				notification;
	octet				transitions;
	} BACnetDestination;

typedef struct tagDeviceList {
struct tagDeviceList	*next;
	char				device[32];
	} BACnetDeviceList;

enum BACnetDeviceStatus
	{	OPERATIONAL,              //0
		OPERATIONAL_READ_ONLY,    //1
		DOWNLOAD_REQUIRED,        //2
		DOWNLOAD_IN_PROGRESS,     //3
		NON_OPERATIONAL           //4
	};

enum BACnetEngineeringUnits
	{
	//Area   
		SQUARE_METERS,                       // 0
		SQUARE_FEET,                         // 1
	//Electrical
		MILLIAMPERES,                        // 2
		AMPERES,                             // 3
		OHMS,                                // 4
		VOLTS,                               // 5
		KILOVOLTS,                           // 6
		MEGAVOLTS,                           // 7
		VOLT_AMPERES,                        // 8
		KILOVOLT_AMPERES,                    // 9
		MEGAVOLT_AMPERES,                    //10
		VOLT_AMPERES_REACTIVE,               //11
		KILOVOLT_AMPERES_REACTIVE,           //12
		MEGAVOLT_AMPERES_REACTIVE,           //13
		DEGREES_PHASE,                       //14
		POWER_FACTOR,                        //15
	//Energy
		JOULES,                              //16
		KILOJOULES,                          //17
		WATT_HOURS,                          //18
		KILOWATT_HOURS,                      //19
		BTU,                                 //20
		THERM,                               //21
		TON_HOR,                             //22
	//Enthalpy
		JOULES_PER_KILOGRAM_DRY_AIR,         //23
		BTU_PER_POUND_DRY_AIR,               //24
	//Frecuency
		CYCLES_PER_HOUR,                     //25
		CYCLES_PER_MINUTE,                   //26
		HERTZ,                               //27
	//Humidity
		GRAMS_OF_WATER_PER_KILOGRAM_DRY_AIR, //28
		PERCENT_RELATIVE_HUMIDITY,           //29
	//Length
		MILLIMETER,                          //30
		METERS,                              //31
		INCH,                                //32
		FEET,                                //33
	//Light
		WATTS_PER_SQUARE_FOOT,               //34
		WATTS_PER_SQUARE_METER,              //35
		LUMEN,                               //36
		LUX,                                 //37
		FOOT_CANDLES,                        //38
	//Mass
		KILOGRAMS,                           //39
		POUNDS_MASS,                         //40
		TONS,                                //41
	//Mass Flow
		KILOGRAMS_PER_SECOND,                //42
		KILOGRAMS_PER_MINUTE,                //43
		KILOGRAMS_PER_HOUR,                  //44
		POUNDS_MASS_PER_MINUTE,              //45
		POUNDS_MASS_PER_HOUR,                //46
	//Power
		WATTS,                               //47
		KILOWATTS,                           //48
		MEGAWATTS,                           //49
		BTU_PER_HOUR,                        //50
		HORSEPOWER,                          //51
		TONS_REFRIGERATIONS,                 //52
	//Pressure
		PASCALS,                              //53
		KILOPASCAL,                          //54
		BAR,                                 //55
		POUNDS_FORCE_PER_SQUARE_INCH,        //56
		CENTIMETERS_OF_WATER,                //57
		INCHES_OF_WATER,                     //58
		MILLIMETERS_OF_MERCURY,              //59
		CENTIMETERS_OF_MERCURY,              //60
		INCHES_OF_MERCURY,                   //61
	//Temperature
		DEGREES_CELSIUS,                     //62
		DEGREES_KELVIN,                      //63
		DEGREES_FAHRENHEIT,                  //64
		DEGREE_DAYS_CELSIUS,                 //65
		DEGREE_DAYS_FAHRENHEIT,              //66
	//Time
		YEAR,                                //67
		MONTH,                               //68
		WEEK,                                //69
		DAY,                                 //70
		HOUR,                                //71
		MINUTE,                              //72
		SECOND,                              //73
	//Velocity
		METERS_PER_SECOND,                   //74
		KILOMETERS_PER_HOUR,                 //75
		FEET_PER_SECOND,                     //76
		FEET_PER_MINUTE,                     //77
		MILES_PER_HOUR,                      //78
	//Volume
		CUBIC_FEET,                          //79
		CUBIC_METERS,                        //80
		IMPERIAL_GALLONS,                    //81
		LITERS,                              //82
		US_GALLONS,                          //83
	//Volumetric Flow
		CUBIC_FEET_PER_MINUTE,               //84
		CUBIC_METERS_PER_SECOND,             //85
		IMPERIAL_GALLONS_PER_MINUTE,         //86
		LITERS_PER_SECOND,                   //87
		LITERS_PER_MINUTE,                   //88
		US_GALLONS_PER_MINUTE,               //89
	//Others
		DEGREES_ANGULAR,                     // 90
		DEGREES_CELSIUS_PER_HOUR,            // 91
		DEGREES_CELSIUS_PER_MINUTE,          // 92
		DEGREES_FAHRENHEIT_PER_HOUR,         // 93
		DEGREES_FAHRENHEIT_PER_MINUTE,       // 94
		NO_UNITS,                            // 95
		PARTS_PER_MILLION,                   // 96
		PARTS_PER_BILLION,                   // 97
		PERCENT,                             // 98
		PERCENT_PER_SECOND,                  // 99
		PER_MINUTE,                          //100
		PER_SECOND,                          //101
		PSI_PER_DEGREE_FAHRENHEIT,           //102
		RADIANS,                             //103
		REVOLUTIONS_PER_MINUTE               //104
	};

enum BACnetEventType
	{	CHANGE_OF_BITSTRING,				//0
		CHANGE_OF_STATE,					//1
		CHANGE_OF_VALUE,					//2
		COMMAND_FAILURE,					//3
		FLOATING_LIMIT,						//4
		OUT_OF_RANGE						//5
	};

enum BACnetPolarity {NORM, REV};

enum BACnetProgramRequest {
   READY,         //0
   LOAD,         //1
   RUN,          //2
   HALT,         //3
   RESTART,      //4
   UNLOAD        //5
   };

enum BACnetProgramState {
   IDLE,          //0
   LOADING,       //1
   RUNNING,       //2
   WAITING,       //3
   HALTED,        //4
   UNLOADING      //5
   };

enum BACnetProgramError {
   P_NORMAL,      //0
   LOAD_FAILED,   //1
   INTERNAL,      //2
   PROGRAM_E,     //3
   OTHER          //4
   };

enum BACnetReliability {
   NO_FAULT_DETECTED,  //0
   NO_SENSOR,          //1
   OVER_RANGE,         //2
   UNDER_RANGE,        //3
   OPEN_LOOP,          //4
   SHORTED_LOOP,       //5
   NO_OUTPUT,          //6
   UNRELIABLE_OTHER    //7
   };

enum BACnetEventState {
   NORMAL,        //0
   FAULT,         //1
   OFFNORMAL,     //2
   E_HIGH_LIMIT,  //3
   E_LOW_LIMIT    //4
   };

/* version before Joe changed it 
struct BACnetPropertyStates {
   boolean boolean_value;
   enum BACnetBinaryPV binary_value;
   enum BACnetEventType event_type;
   enum BACnetPolarity polarity;
   enum BACnetProgramRequest program_change;
   enum BACnetProgramState program_state;
   enum BACnetProgramError reason_for_halt;
   enum BACnetReliability reliability;
   enum BACnetEventState state;
   enum BACnetDeviceStatus system_status;
   enum BACnetEngineeringUnits units;
   };
*/

typedef struct tagPropertyStates {
struct tagPropertyStates 	*next;
	int						enum_value;
	} BACnetPropertyStates;

typedef struct tagObjectPropertyReference {
struct tagObjectPropertyReference	*next;
	dword							object_id;
	dword							property_id;    
	word							pa_index;
	} BACnetObjectPropertyReference;

typedef struct tagListBitstringValue {
struct tagListBitstringValue	far	*next;
	octet							bitstring_length;
	octet							bitstring_value[4];
	} BACnetListBitstringValue;

typedef struct {
enum BACnetEventType				event_type;
	BACnetListBitstringValue		bitmask;
	BACnetListBitstringValue	*list_bitstring_value;
	BACnetPropertyStates		*list_of_value;
	BOOL							use_property_increment;
	float							ref_property_increment;
	word							time_delay;
	BACnetObjectPropertyReference	feed_prop_ref;
	BACnetObjectPropertyReference	setpoint_ref;
	float							deadband;
	float							high_diff_limit;
	float							low_diff_limit;
	float							high_limit;
	float							low_limit;
	} BACnetEventParameter;

typedef struct tagObjectIdentifier {
struct tagObjectIdentifier	*next;
	dword					object_id;
	} BACnetObjectIdentifier;

typedef struct tagTimeValue {
struct tagTimeValue		*next;
	BACnetTime			time;
enum ActionValueType	value_type;
	union {
		enum BACnetBinaryPV	bproperty_value; 	//binary value
		word				uproperty_value;	//unsigned word
		float				fproperty_value;	//float
		}				av;
   } BACnetTimeValue; 

typedef struct tagSpecialEvent {   //only one of date, date_range, weekNday, calendar_ref should be used at a time
struct tagSpecialEvent	far	*next;
	octet					choice;
	union {
		BACnetDate				date;
		BACnetDateRange			date_range;
		BACnetWeekNDay			weekNday;
		BACnetObjectIdentifier	calendar_ref;
		}					u;
	BACnetTimeValue		far	*list_of_time_values;
	word					event_priority;
   } BACnetSpecialEvent; 

typedef struct {
	word					size;
	BACnetSpecialEvent	far	*special_event;
	} BACnetExceptionSchedule;

enum BACnetFileAccessMethod {
   RECORD_ACCESS,            //0
   STREAM_ACCESS,            //1
   RECORD_AND_STREAM_ACCESS  //2
   };

enum BACnetNotifyType {ALARM, EVENT};

typedef struct {
	word					size;
	BACnetObjectIdentifier	object_list_id;
	} BACnetObjectList;

enum  BACnetObjectType
	{
		ANALOG_INPUT,       					// 0 
		ANALOG_OUTPUT,      					// 1 
		ANALOG_VALUE,       					// 2 
		BINARY_INPUT,       					// 3 
		BINARY_OUTPUT,      					// 4 
		BINARY_VALUE,       					// 5 
		CALENDAR,           					// 6 
		COMMAND,            					// 7 
		DEVICE,             					// 8 
		EVENT_ENROLLMENT,   					// 9 
		FILE_O,             					//10 
		GROUP,              					//11 
		LOOP,               					//12 
		MULTI_STATE_INPUT,  					//13 
		MULTI_STATE_OUTPUT, 					//14 
		NOTIFICATIONCLASS,						//15 
		PROGRAM,            					//16 
		SCHEDULE            					//17 
   };

typedef struct tagPropertyReference {
struct tagPropertyReference		*next;
	dword						property_id;
	word						pa_index;
	} BACnetPropertyReference;

typedef struct tagPropertyValue {
struct tagPropertyValue			*next;
	word						property_id;
	word						pa_index;
	octet						*property_value;
	int							priority;
	} BACnetPropertyValue;

typedef struct {
	dword						object_id;
	word						property_id;
	word						pa_index;
	float						value;
	word						property_value;
enum BACnetReliability			property_access_error;
	word						property_value_id;
	char						description[32];
	} BACnetReadAccessResult;

typedef struct tagReadAccessSpecification {
struct tagReadAccessSpecification	*next;
	dword							object_id;
	BACnetPropertyReference		far	*list_of_prop_ref;
	} BACnetReadAccessSpecification;

typedef struct tagRecipientProcess {
struct tagRecipientProcess		*next;
	BACnetRecipient				recipient;
	word						process_id;
	} BACnetRecipientProcess;

enum BACnetSegmentation {
   SEGMENTED_BOTH,      //0
   SEGMENTED_TRANSMIT,  //1
   SEGMENTED_RECEIVE,   //2
   NO_SEGMENTATION      //3
   };

typedef struct tagSessionKey {
struct tagSessionKey			*next;
	octet						session_key[8];
	BACnetAddress				peer_address;
	} BACnetSessionKey;

typedef struct tagTimeStamp {
struct tagTimeStamp		*next;
	BACnetTime			time;
	dword				sequence_number;
	BACnetDateTime		date_time;
	} BACnetTimeStamp;

enum BACnetVTClass {
   DEFAULT_TERMINAL,            				//0
   ANSI_X34,    								//1	real name is ANSI X3.64
   DEC_VT52,                					//2
   DEC_VT100,               					//3
   DEC_VT220,               					//4
   HP_700_94,   								//5	real name is HP 700/94
   IBM_3130                						//6
   };

typedef struct tagVTClassList{
struct tagVTClassList	*next;
enum BACnetVTClass		vtclass;
	} BACnetVTClassList;

typedef struct tagVTSession {
struct tagVTSession		*next;
	word				local_session_id;
	word				remote_session_id;
	BACnetAddress		remote_address;
	} BACnetVTSession;

typedef struct {
	dword					object_id;
	BACnetPropertyValue		list_property;
	} BACnetWriteAccessSpecification;
	
enum BCstring {
	ANSI,
	MS_DBCS,
	JISC_6226,
	UCS4,
	UCS2,
	ISO8859
	};
	
enum BACnetAbortReason
	{	ABORT_Other,							//0
		ABORT_BufferOverflow,					//1
		ABORT_InvalidAPDUInThisState,			//2
		ABORT_PreemptedByHigherPriorityTask,	//3
		ABORT_SegmentationNotSupported			//4
	};

#endif //__DB_H_INCLUDED
