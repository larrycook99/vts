//DB.H
//Base Datatype Defitions for BACnet
#ifndef __DB_H_INCLUDED
#define __DB_H_INCLUDED

#define  true           1
#define  false          0

typedef bool boolean;
typedef unsigned char octet;
typedef unsigned long dword;
typedef unsigned short word;

/*   ---------- BACnet base types --------------- */
typedef word BACnetStatusFlags[2];

enum BACnetAction {DIRECT, REVERSE};
enum BACnetBinaryPV {INACTIVE, ACTIVE};

#define  NotAnArray     0xFFFF               //value for pa_index fields if not an array

enum ActionValueType {BPV,UNS,FLT};          //binaryPV, unsigned word or float 

typedef struct tagActionCommand {
struct tagActionCommand *next;
   dword          device_id;
   dword          object_id;
   dword          property_id;
   word           pa_index;
enum ActionValueType value_type;
   union {
      enum BACnetBinaryPV  bproperty_value;  //binary value
      word           uproperty_value;  //unsigned word
      float          fproperty_value;  //float
   }              av;
   octet          priority;
   word           post_delay;
   bool        quit_on_failure;
   bool        write_successful;
   } BACnetActionCommand;

typedef struct tagTextList {
struct tagTextList   *next;
   char        text[MAX_TEXT_STRING];
   } TextList;

typedef struct {
   word     network_number;
   octet mac_address[8];
   word     address_size;
   } BACnetAddress;

typedef struct tagAddressBinding {
struct tagAddressBinding   *next;
   dword             device_object_id;
   BACnetAddress        device_address;
   } BACnetAddressBinding;

typedef struct tagAlarmValues {
struct tagAlarmValues   *next;
   word           value;
   } BACnetAlarmValues;

#define dontcare        0xFF           //for date and time fields
#define DONT_CARE       0xFF           //for date and time fields

typedef struct {
//each field is one octet to match encoding, 0xFF means unknown or don't care
   octet year;                         //year - 1900
   octet month;                        //January = 1
   octet day_of_month;  
   octet day_of_week;                  //Sunday = 1
   } BACnetDate;

typedef struct {
//each field is one octet to match encoding, 0xFF means unknown or don't care
   octet hour;                      //24 hr clock
   octet minute;
   octet second;
   octet hundredths;
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
   octet month;                        //January = 1
   octet week;                      //week 1 = days numbered 1-7 and so on
   octet day;                       //Monday = 1
   } BACnetWeekNDay;

typedef struct tagCalendarEntry 
  {
    struct tagCalendarEntry *next;
   octet             choice;
   union {
      BACnetDate        date;
      BACnetDateRange   date_range;
      BACnetWeekNDay    weekNday;
      }              u;
  } BACnetCalendarEntry;

typedef struct tagTimeStamp
 {
//   struct tagTimeStamp   *next;
   octet                choice;
   union
   {
    BACnetTime       time;
    word             sequence_number; // BACnetTimestamp defines this as 0-65535
    BACnetDateTime   date_time;
   }u;
 } BACnetTimeStamp;

typedef struct 
 {
   octet            choice;
   union
   {
    float         floatScale;
    int           integerScale;   
   }u;
 } BACnetScale;

//Shiyuan Xiao. 7/14/2005
typedef struct 
{
   word multiplier;
    word moduloDivide;
} BACnetPrescale;

//Shiyuan Xiao. 7/14/2005
typedef struct 
{
   BACnetDateTime timestamp;
   word           presentValue;
   word           accumulatedValue;
   word           accumulatorStatus;
} BACnetAccumulatorRecord;

typedef struct tagRecipient {
struct tagRecipient  *next;
   octet             choice;           //0 = device, 1 = address
   union {
      dword          device;
      BACnetAddress  address;
      }              u;
   } BACnetRecipient;

typedef struct tagDestination {
struct tagDestination   *next;
   octet             valid_days;
   BACnetTime        from_time;
   BACnetTime        to_time;
   BACnetRecipient   recipient;
   word              process_id;
   bool              notification;
   octet             transitions;
   } BACnetDestination;

typedef struct tagDeviceList {
struct tagDeviceList *next;
   char           device[32];
   } BACnetDeviceList;

enum BACnetDeviceStatus
   {
      OPERATIONAL,              //0
      OPERATIONAL_READ_ONLY,    //1
      DOWNLOAD_REQUIRED,        //2
      DOWNLOAD_IN_PROGRESS,     //3
      NON_OPERATIONAL,          //4
      BACKUP_IN_PROGRESS        //5    Added by Jingbo Gao 09/09/2004
   };

enum BACnetEngineeringUnits
   {
      // This enumeration type is used in various functions etc., but none
      // of the enumerated VALUES are ever used: we just cast to and from integers.
      // Thus, to eliminate pointless maintenance, we have removed the values here.
      // Tables of string may be found in StringTables.cpp.
      NOTHING = 0
   };

enum BACnetEventType
   {
      CHANGE_OF_BITSTRING,          //0
      CHANGE_OF_STATE,              //1
      CHANGE_OF_VALUE,              //2
      COMMAND_FAILURE,              //3
      FLOATING_LIMIT,               //4
      OUT_OF_RANGE,                 //5
      COMPLEX_EVENT_TYPE,           //6
      CHANGE_OF_LIFE_SAFETY = 8,    //8 (7 is deprecated)
      EXTENDED,                     //9
      BUFFER_READY                  //10
   };

enum BACnetPolarity {NORM, REV};

enum BACnetProgramRequest {
   READY,        //0
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
   bool boolean_value;
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
   struct tagPropertyStates   *next;
   int                  enum_value;
} BACnetPropertyStates;

typedef struct tagObjectPropertyReference {
   struct tagObjectPropertyReference   *next;
   dword                   object_id;
   dword                   property_id;
   word                    pa_index;
} BACnetObjectPropertyReference;

typedef struct tagDeviceObjectPropertyReference {
   struct tagDeviceObjectPropertyReference   *next;
   dword    Objid;
   dword    wPropertyid;
   word     ulIndex;
   dword    DeviceObj;
} BACnetDeviceObjectPropertyReference;

typedef struct tagDeviceObjectReference {
   struct tagDeviceObjectReference *next;
   dword DeviceObj;
   dword Objid;
} BACnetDeviceObjectReference;

typedef struct tagListBitstringValue {
   struct tagListBitstringValue  *next;
   UINT                    bitstring_length;
   octet                   bitstring_value[4];
} BACnetListBitstringValue;

typedef struct tagLogRecord
{
   struct tagLogRecord *next;
   BACnetDateTime     timestamp;
   octet              oLenBitString;
   octet              choice;
   dword              logDatum;
   BACnetStatusFlags  statusFlags;
} BACnetLogRecord;

typedef struct {
   enum BACnetEventType       event_type;
   BACnetListBitstringValue   bitmask;
   BACnetListBitstringValue   *list_bitstring_value;
   BACnetPropertyStates       *list_of_value;
   BOOL                       use_property_increment;
   float                      ref_property_increment;
   word                       time_delay;
   BACnetObjectPropertyReference       feed_prop_ref;
   BACnetObjectPropertyReference       setpoint_ref;
   BACnetDeviceObjectPropertyReference device_prop_ref;
   float                      deadband;
   float                      high_diff_limit;
   float                      low_diff_limit;
   float                      high_limit;
   float                      low_limit;
   dword                      notification_threshold;
   dword                      previous_notification_count;
} BACnetEventParameter;

typedef struct tagObjectIdentifier {
   struct tagObjectIdentifier *next;
   dword             object_id;
} BACnetObjectIdentifier;

typedef struct tagTimeValue {
struct tagTimeValue     *next;
   BACnetTime        time;
enum ActionValueType value_type;
   union {
      enum BACnetBinaryPV  bproperty_value;  //binary value
      word           uproperty_value;  //unsigned word
      float          fproperty_value;  //float
      }           av;
   } BACnetTimeValue; 

typedef struct tagSpecialEvent {   //only one of date, date_range, weekNday, calendar_ref should be used at a time
struct tagSpecialEvent  far   *next;
   octet             choice;
   union {
      BACnetDate           date;
      BACnetDateRange         date_range;
      BACnetWeekNDay       weekNday;
      BACnetObjectIdentifier  calendar_ref;
      }              u;
   BACnetTimeValue      far   *list_of_time_values;
   word              event_priority;
   } BACnetSpecialEvent; 

typedef struct {
   word              size;
   BACnetSpecialEvent   far   *special_event;
   } BACnetExceptionSchedule;

enum BACnetFileAccessMethod {
   RECORD_ACCESS,            //0
   STREAM_ACCESS,            //1
   RECORD_AND_STREAM_ACCESS  //2
   };

enum BACnetNotifyType {ALARM, EVENT};

typedef struct {
   word              size;
   BACnetObjectIdentifier  object_list_id;
   } BACnetObjectList;

//Shiyuan Xiao. 7/14/2005
enum BACnetLifeSafetyState
{
   quiet,
   pre_alarm,
   alarm,
   fault,
   fault_pre_alarm,
   fault_alarm,
   not_ready,
   active,
   tamper,
   test_alarm,
   test_active,
   test_fault,
   test_fault_alarm,
   holdup,
   duress,
   tamper_alarm,
   abnormal,
   emergency_power,
   delayed,
   blocked,
   local_alarm,
   general_alarm,
   supervisory,
   test_supervisory
};

typedef struct tagEnumList
{
   struct tagEnumList *next;
   word  value;
} BACnetEnumList;

//Shiyuan Xiao. 7/21/2005
enum BACnetMaintenance
{
   None,
   Periodic_test,
   Need_Service_Operational,
   Need_Service_Inoperative
};

enum BACnetLifeSafetyMode
{
   off,
   on,
   test,
   manned,
   unmanned,
   armed,
   disarmed,
   prearmed,
   slow,
   fast,
   disconnected,
   enabled,
   disabled,
   automatic_release_disabled,
   _default
};

enum BACnetSilencedState
{
   Unsilenced,
   Audible_Silenced,
   Visible_Silenced,
   All_Silenced
};

enum BACnetLifeSafetyOperation
{
   none,
   silence,
   silence_audible,
   silence_visual,
   reset,
   reset_alarm,
   reset_fault
};

//Shiyuna Xiao
enum BACnetAccumulatorStatus
{
   normal,
   starting,
   recovered,
   abnormall,
   failed
};

// enumerated BIBBs
// WARNING - This enumeration must match exactly with the 
// definition of the BIBBs[] array in Vtsapi32.cpp because these
// values are used as in index into that array.
enum BIBB {
   bibbDS_RP_A = 0,  
   bibbDS_RP_B,
   bibbDS_RPM_A,
   bibbDS_RPM_B,
   bibbDS_RPC_A,
   bibbDS_RPC_B,
   bibbDS_WP_A,
   bibbDS_WP_B,
   bibbDS_WPM_A,
   bibbDS_WPM_B,
   bibbDS_COV_A,
   bibbDS_COV_B,
   bibbDS_COVP_A,
   bibbDS_COVP_B,
   bibbDS_COVU_A,
   bibbDS_COVU_B,
   bibbAE_N_A,
   bibbAE_N_I_B,
   bibbAE_N_E_B,
   bibbAE_EL_I_B,
   bibbAE_EL_E_B,
   bibbAE_ELV_A,
   bibbAE_ELVM_A,
   bibbAE_ACK_A,
   bibbAE_ACK_B,
   bibbAE_ASUM_A,
   bibbAE_ASUM_B,
   bibbAE_ESUM_A,
   bibbAE_ESUM_B,
   bibbAE_INFO_A,
   bibbAE_INFO_B,
   bibbAE_LS_A,
   bibbAE_LS_B,
   bibbSCHED_A,
   bibbSCHED_I_B,
   bibbSCHED_E_B,
   bibbT_VMT_A,
   bibbT_VMT_I_B,
   bibbT_VMT_E_B,
   bibbT_ATR_A,
   bibbT_ATR_B,
   bibbDM_DDB_A,
   bibbDM_DDB_B,
   bibbDM_DOB_A,
   bibbDM_DOB_B,
   bibbDM_DCC_A,
   bibbDM_DCC_B,
   bibbDM_PT_A,
   bibbDM_PT_B,
   bibbDM_TM_A,
   bibbDM_TM_B,
   bibbDM_TS_A,
   bibbDM_TS_B,
   bibbDM_UTC_A,
   bibbDM_UTC_B,
   bibbDM_RD_A,
   bibbDM_RD_B,
   bibbDM_BR_A,
   bibbDM_BR_B,
   bibbDM_R_A,
   bibbDM_R_B,
   bibbDM_LM_A,
   bibbDM_LM_B,
   bibbDM_OCD_A,
   bibbDM_OCD_B,
   bibbDM_VT_A,
   bibbDM_VT_B,
   bibbNM_CE_A,
   bibbNM_CE_B,
   bibbNM_RC_A,
   bibbNM_RC_B,
   // Added Workstation BIBBs 12/5/2007 LJT
   bibbDS_V_A,
   bibbDS_AV_A,
   bibbDS_M_A,
   bibbDS_AM_A,
   bibbAE_VN_A,
   bibbAE_AVN_A,
   bibbAE_VM_A,
   bibbAE_AVM_A,
   bibbAE_AS_A,
   bibbSCHED_AVM_A,
   bibbSCHED_VM_A,
   bibbSCHED_WS_A,
   bibbSCHED_WS_I_B,
   bibbSCHED_R_B,
   bibbT_AVM_A,
   bibbT_VM_I_B,
   bibbT_VM_E_B,
   bibbT_V_A,
   bibbT_A_A,
   bibbDM_ANM_A,
   bibbDM_ADM_A,
   bibbDM_ATS_A,
   bibb_DM_MTS_A,
   bibbSCH_AVM_A,
   bibbSCH_VM_A,
   bibbSCH_WS_A,
   bibbSCH_WS_I_B,
   bibbSCH_R_B,
   bibbSCH_I_B,
   bibbSCH_E_B,
   // Added to VTS 26 August 2014
   bibbT_VMMV_A,  // Deprecated in 135-2012
   bibbT_VMMV_I_B,
   bibbT_VMMV_E_B,
   bibbT_AMVR_A,
   bibbT_AMVR_B,
};

typedef struct tagPropertyReference {
struct tagPropertyReference      *next;
   dword                property_id;
   word                 pa_index;
   } BACnetPropertyReference;

typedef struct tagPropertyValue {
struct tagPropertyValue       *next;
   word                 property_id;
   word                 pa_index;
   octet                *property_value;
   int                     priority;
   } BACnetPropertyValue;

typedef struct {
   dword                object_id;
   word                 property_id;
   word                 pa_index;
   float                value;
   word                 property_value;
enum BACnetReliability        property_access_error;
   word                 property_value_id;
   char                 description[MAX_TEXT_STRING];
   } BACnetReadAccessResult;

typedef struct tagReadAccessSpecification {
struct tagReadAccessSpecification   *next;
   dword                   object_id;
   BACnetPropertyReference    far   *list_of_prop_ref;
   } BACnetReadAccessSpecification;

typedef struct tagRecipientProcess {
struct tagRecipientProcess    *next;
   BACnetRecipient            recipient;
   word                 process_id;
   } BACnetRecipientProcess;

enum BACnetSegmentation {
   SEGMENTED_BOTH,      //0
   SEGMENTED_TRANSMIT,  //1
   SEGMENTED_RECEIVE,   //2
   NO_SEGMENTATION      //3
   };

typedef struct tagSessionKey {
struct tagSessionKey       *next;
   octet                session_key[8];
   BACnetAddress           peer_address;
   } BACnetSessionKey;


enum BACnetVTClass {
   DEFAULT_TERMINAL,                   //0
   ANSI_X34,                           //1   real name is ANSI X3.64
   DEC_VT52,                           //2
   DEC_VT100,                          //3
   DEC_VT220,                          //4
   HP_700_94,                          //5   real name is HP 700/94
   IBM_3130                            //6
   };

typedef struct tagVTClassList{
struct tagVTClassList   *next;
enum BACnetVTClass      vtclass;
   } BACnetVTClassList;

typedef struct tagVTSession {
struct tagVTSession     *next;
   word           local_session_id;
   word           remote_session_id;
   BACnetAddress     remote_address;
   } BACnetVTSession;

typedef struct {
   dword             object_id;
   BACnetPropertyValue     list_property;
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
   {  ABORT_Other,                        //0
      ABORT_BufferOverflow,               //1
      ABORT_InvalidAPDUInThisState,       //2
      ABORT_PreemptedByHigherPriorityTask,//3
      ABORT_SegmentationNotSupported      //4
   };
//Xiao Shiyuan 2002-7-23
typedef struct tagCOVSubscription {
struct tagCOVSubscription     *next;
   BACnetRecipientProcess     recipient;
   BACnetObjectPropertyReference    monitoredPropertyReference;
   bool        notification;
   word        timeRemaining;
   float       covIncrement;
   } BACnetCOVSubscription;

typedef struct tagBooleanList {
   struct tagBooleanList *next;
   bool    value;
} BooleanList;

typedef struct tagUnsignedList {
   struct tagUnsignedList *next;
   unsigned value;
} UnsignedList;

typedef struct tagBACnetShedLevel {
   octet choice;  // context tag for this structure
   union {
      dword uproperty_value;  //unsigned dword
      float fproperty_value;  //float
   } sl;

} BACnetShedLevel;

#endif //__DB_H_INCLUDED
