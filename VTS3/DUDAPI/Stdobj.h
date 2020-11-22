//STDOBJ.H

/* 
   C structures to hold data in the EPICS point database.
   The structs are manipuated (almost) entirely via the propdescriptor tables.
*/

//BACnet Standard Object Structures
#ifndef __STDOBJ_H_INCLUDED
#define __STDOBJ_H_INCLUDED

// NOTE: various places below use (or formerly used) enums such as BACnetBinaryPV,
// BACnetNotifyType, etc.  But the propdescriptor-driven code in Vtsapi32.cpp
// and DUDTOOL.CPP assumes that enumerations are 16-bit (word).
// This code was written assuming 16-bit ints, so that enums WERE 16 bits.
// But it isn't 1995 anymore.
// So most if not all of these should be "word" not "enum XXX".
//
// The good news is that if we change to word, and some code DID access the item
// by name (rather than by address via the propdescriptor), a compile error
// is likely due to the change in datatype.
//
// TODO: SOME of the objects have been changed.  More work here, unless we
// go the other way change enums to 32 bit, but that seems more complicated.

//equates for priority arrays
#define fpaNULL            FLT_MIN        //too bad we can't use NAN
#define bpaNULL            2              //binary present values are 0 and 1
#define dvaNULL            4              // DoorValue Enum NULL value
#define upaNULL            0xFFFF         //can't use more than 65534 enumerations, too bad
#define ipaNULL            0x80000000     //most negative 32-bit signed int, as a dword (sorry, wasn't MY idea)

//equates for standard properties
//these are always in this order for every object type:
#define  idProp            0              //object_id property
#define  nameProp          1              //object_name property
#define  typeProp          2              //object_type property
#define  NUM_PROP_FLAGS  100              // maximum number of properties in a propdescriptor[] table (check DVprops)

//equates for propflags bits
#define  PropIsPresent     1              //0=not present, 1=present in this instance
#define  PropIsWritable    2              //0=R, 1=W
#define  ValueUnknown      4              //0=have a value, 1=value is ?

// Any (well, MOST) primitive value
struct AnyPrimitive
{
   int            choice;           // Primitive tag type (PRIM_xxx as defined in VTS.h)
   int            helper;           // choice-dependent data: enumtable index, string length etc.
   union
   {
      bool           booleanValue;
      UINT           unsignedValue;
      int            signedValue;
      float          realValue;
      unsigned char  *pOctetStringValue;
      char           *pCharStringValue;
      unsigned char  *pBitStringValue;
      word           enumValue;
      BACnetDate     dateValue;
      BACnetTime     timeValue;
      dword          objectIdValue;
   };

   // Delete any allocated data, set choice to NULL
   void Flush();
};

#define PRIM_DOUBLE             5
#define PRIM_OCTET_STRING       6
#define PRIM_CHARACTER_STRING   7
#define PRIM_BIT_STRING         8
#define PRIM_ENUMERATED         9
#define PRIM_DATE              10
#define PRIM_TIME              11
#define PRIM_OBJECT_IDENTIFIER 12



typedef struct
{
// BACnetAnyValue minValue;
// BACnetAnyValue maxValue;
// BACnetAnyValue resolution;
   int            maxLengthStr;
   int            maxLengthList;
   int            maxLengthArray;
   int            allowedValues;
} ObjPropValueLimit; //Default property value restriction

//Generic Object type structure common to all Std Objects
//-------------------------------------------------------
typedef struct {
   void             *next;
   dword            object_id;
   char             object_name[MAX_TEXT_STRING];
   word             object_type;
   char             description[MAX_TEXT_STRING];
   octet            propflags[NUM_PROP_FLAGS];    //flags, indexed by position in property table
   char             profile_name[MAX_TEXT_STRING];
   BACnetEnumList   *property_list;   // List of BACnetPropertyIdentifier

   // These properties don't exist in ALL object, but the occur in enough that
   // it saves significant code to include them here.
   octet             status_flags;
   word              event_state;   // WAS enum BACnetEventState.  See NOTE above
   word              reliability;   // WAS enum BACnetReliability.  See NOTE above
   bool              out_of_service;

   word              time_delay;
   word              notification_class;

   octet             limit_enable;
   octet             event_enable;
   octet             acked_transitions;
   word              notify_type;   // WAS enum BACnetNotifyType.  See NOTE above
   BACnetTimeStamp   *event_time_stamps[3];

   char              *event_message_texts[3];
   char              *event_message_texts_config[3];
   bool              event_detection_enable;
   bool              event_algorithm_inhibit;
   bool              reliability_evaluation_inhibit;
   BACnetObjectPropertyReference  event_algorithm_inhibit_ref;
   dword             time_delay_normal;
} generic_object;

//Analog Input Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    float               pv;
    char                device_type[MAX_TEXT_STRING];
    word                update_interval;
    word                units;   // WAS enum BACnetEngineeringUnits.  See NOTE above
    float               min_pres_value;
    float               max_pres_value;
    float               resolution;
    float               cov_increment;
    float               high_limit;
    float               low_limit;
    float               deadband;
} ai_obj_type;

//Analog Output Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    float               pv;
    char                device_type[MAX_TEXT_STRING];
    word                units;   // WAS enum BACnetEngineeringUnits.  See NOTE above
    float               min_pres_value;
    float               max_pres_value;
    float               resolution;
    float               priority_array[16];
    float               relinquish_default;
    float               cov_increment;
    float               high_limit;
    float               low_limit;
    float               deadband;
} ao_obj_type;

//Analog Value Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    float               pv;
    word                units;   // WAS enum BACnetEngineeringUnits.  See NOTE above
    float               min_pres_value;
    float               max_pres_value;
    float               resolution;
    float               priority_array[16];
    float               relinquish_default;
    float               cov_increment;
    float               high_limit;
    float               low_limit;
    float               deadband;
} av_obj_type;

//Binary Input Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    word                pv;  // WAS enum BACnetBinaryPV.  See NOTE above
    char                device_type[MAX_TEXT_STRING];
    word                polarity;  // WAS enum BACnetBinaryPV.  See NOTE above
    char                inactive_text[MAX_TEXT_STRING];
    char                active_text[MAX_TEXT_STRING];
    BACnetDateTime      cos_time;
    word                cos_count;
    BACnetDateTime      time_of_state_count_reset;
    dword               elapsed_active_time;
    BACnetDateTime      time_of_active_time_reset;
    word                alarm_value;  // WAS enum BACnetBinaryPV.  See NOTE above
} bi_obj_type;

//Binary Output Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    word                pv;  // WAS enum BACnetBinaryPV.  See NOTE above
    char                device_type[MAX_TEXT_STRING];
    word                polarity;  // WAS enum BACnetBinaryPV.  See NOTE above
    char                inactive_text[MAX_TEXT_STRING];
    char                active_text[MAX_TEXT_STRING];
    BACnetDateTime      cos_time;
    word                cos_count;
    BACnetDateTime      time_of_state_count_reset;
    dword               elapsed_active_time;
    BACnetDateTime      time_of_active_time_reset;
    dword               min_off_time;
    dword               min_on_time;
    word                priority_array[16];  // WAS enum BACnetBinaryPV.  See NOTE above
    word                relinquish_default;  // WAS enum BACnetBinaryPV.  See NOTE above
    word                feedback_value;      // WAS enum BACnetBinaryPV.  See NOTE above
} bo_obj_type;

//Binary Value Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    word                pv;  // WAS enum BACnetBinaryPV.  See NOTE above
    char                inactive_text[MAX_TEXT_STRING];
    char                active_text[MAX_TEXT_STRING];
    BACnetDateTime      cos_time;
    word                cos_count;
    BACnetDateTime      time_of_state_count_reset;
    dword               elapsed_active_time;
    BACnetDateTime      time_of_active_time_reset;
    dword               min_off_time;
    dword               min_on_time;
    word                priority_array[16];  // WAS enum BACnetBinaryPV.  See NOTE above
    word                relinquish_default;  // WAS enum BACnetBinaryPV.  See NOTE above
    word                alarm_value;         // WAS enum BACnetBinaryPV.  See NOTE above
} bv_obj_type;

//Calendar Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    bool                pv;
    BACnetCalendarEntry  *date_list;
} calendar_obj_type;

//Notification Class Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    word                priority[3];
    octet               ack_required;
    BACnetDestination   *recipient_list;
} nc_obj_type;

#define MAX_ACTION_TEXTS 32
//Command Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    word                pv;
    bool                in_process;
    bool                all_writes_successful;
    word                num_actions;
    BACnetActionCommand *action[MAX_ACTION_TEXTS];
    char                *action_text[MAX_ACTION_TEXTS];
} command_obj_type;

//Device Object
//-------------------------------------------------------
// Define maximum possible lengths
// for protocol_services_supported and object_types_supported.
// Still some work to do to prevent breakage if these bitstrings
// are longer than expected.
#define MAX_BITSTRING   80

typedef struct {
   generic_object      go;
   word                system_status;  // WAS enum BACnetDeviceStatus.  See NOTE above
   char                vendor_name[MAX_TEXT_STRING];
   word                vendor_id;
   char                model_name[MAX_TEXT_STRING];
   char                firmware_rev[MAX_TEXT_STRING];
   char                application_software_ver[MAX_TEXT_STRING];
   char                location[MAX_TEXT_STRING];
   word                protocol_ver;
   word                protocol_rev;
   word                protocol_conf_class;
   octet               protocol_services_supported[MAX_BITSTRING/8]; //bit string, one bit per service
   octet               object_types_supported[MAX_BITSTRING/8];     //bit string, one bit per object type
   word                num_objects;
   BACnetObjectIdentifier *object_list;           //points to our object list
   word                max_apdu_length_accepted;
   word                segmentation_supported;  // WAS enum BACnetSegmentation.  See NOTE above
   dword               max_segments_accepted;
   BACnetVTClassList   *vt_classes_supported;
   BACnetVTSession     *active_vt_sessions;
   BACnetDate          local_date;
   BACnetTime          local_time;                //time and date are read from system clock
   int                 utc_offset;
   bool                day_savings_status;
   dword               apdu_segment_timeout;
   dword               apdu_timeout;
   word                number_apdu_retries;
   BACnetSessionKey    *list_session_keys;
   BACnetRecipient     *time_synch_recipients;
   word                max_master;
   word                max_info_frames;
   BACnetAddressBinding *device_add_binding;
   dword                database_revision;
   BACnetObjectIdentifier  *configuration_files;      // points to configuration files
   BACnetTimeStamp      last_restore_time;
   word                 backup_failure_timeout;
   BACnetCOVSubscription *active_cov_subscriptions;
   BACnetAddressBinding *manual_slave_add_bind;
   BACnetAddressBinding *slave_add_bind;
   BooleanList          *auto_slave_disc;    // SEQ of boolean
   BooleanList          *slave_proxy_enable; // SEQ of boolean
   BACnetObjectIdentifier *structured_object_list;   //object list of Structured Views
   word                 backup_preparation_time;
   word                 restore_preparation_time;
   word                 restore_completion_time;
   word                 backup_and_restore_state;
   word                 last_restart_reason;
   BACnetTimeStamp      time_of_device_restart;
   BACnetRecipient      *restart_notification_recipients;
   BACnetRecipient      *utc_time_synchronization_recipients;
   word                 time_synchronization_interval;
   bool                 align_intervals;
   word                 interval_offset;
   char                 serial_number[MAX_TEXT_STRING];
} device_obj_type;

//Event Enrollment Object
//-------------------------------------------------------
typedef struct {
    generic_object         go;
//Note that event_type is part of parameter_list structure in this implementation!
    BACnetEventParameter   parameter_list;
    BACnetDeviceObjectPropertyReference obj_prop_ref;
    BACnetRecipient        recipient;          // removed for revision 4
    word                   process_id;         // removed for revision 4
    word                   priority;           // removed for revision 4
    bool                   issue_conf_notifications; // removed for revision 4
} ee_obj_type;

//File Object
//-------------------------------------------------------
typedef struct {
    generic_object         go;
    char                   file_type[MAX_TEXT_STRING];
    dword                  file_size;
    BACnetDateTime         mod_date;
    bool                   archive;
    bool                   read_only;
    dword                  record_count;
enum BACnetFileAccessMethod access_method;
} file_obj_type;

//Group Object
//-------------------------------------------------------
typedef struct {
    generic_object         go;
    BACnetReadAccessSpecification*list_of_group_members;
   //get present value by reading the relevant objects
} group_obj_type;

//Loop Object
//-------------------------------------------------------
typedef struct {
    generic_object         go;
    float                  pv;
    word                   update_interval;
    word                   output_units; // WAS enum BACnetEngineeringUnits.  See NOTE above
    int                    output_value;
    BACnetObjectPropertyReference man_var_ref;
    BACnetObjectPropertyReference cont_var_ref;
    float                  cont_var_value;
    word                   cont_var_units; // WAS enum BACnetEngineeringUnits.  See NOTE above
    BACnetObjectPropertyReference*setpoint_ref;
    float                  setpoint;
    word                   action;                 // WAS enum BACnetEngineeringUnits.  See NOTE above
    float                  proportional_const;
    word                   proportional_const_units; // WAS enum BACnetEngineeringUnits.  See NOTE above
    float                  integral_const;
    word                   integral_const_units;   // WAS enum BACnetEngineeringUnits.  See NOTE above
    float                  derivative_const;
    word                   derivative_const_units; // WAS enum BACnetEngineeringUnits.  See NOTE above
    float                  bias;
    float                  max_output;
    float                  min_output;
    word                   priority_for_writing;
    float                  cov_increment;
    float                  error_limit;
    float                  deadband;
} loop_obj_type;

#define MAX_STATE_TEXTS 128
//Multi-state Input Object
//-------------------------------------------------------
typedef struct {
    generic_object         go;
    word                   pv;
    char                   device_type[MAX_TEXT_STRING];
    word                   num_of_states;
    char                   *state_text[MAX_STATE_TEXTS];
    UnsignedList           *alarm_values;
    UnsignedList           *fault_values;
} mi_obj_type;

//Multi-state Output Object
//-------------------------------------------------------
typedef struct {
    generic_object   go;
    word             pv;
    char             device_type[MAX_TEXT_STRING];
    word             num_of_states;
    char             *state_text[MAX_STATE_TEXTS];
    word             priority_array[16];
    word             relinquish_default;
    word             feedback_value;
} mo_obj_type;

//Program Object
//-------------------------------------------------------
typedef struct {
   generic_object    go;
   word              prog_state;       // WAS enum BACnetProgramState.  See NOTE above
   word              prog_change;      // WAS enum BACnetProgramRequest.  See NOTE above
   word              reason_for_halt;  // WAS enum BACnetProgramError.  See NOTE above
   char              description_of_halt[MAX_TEXT_STRING];
   char              prog_location[MAX_TEXT_STRING];
   char              instance_of[MAX_TEXT_STRING];
} program_obj_type;

//Schedule Object
//-------------------------------------------------------
typedef struct {
   generic_object      go;
   dword               value_type;
   // TODO: what about the NULL, Boolean, enum, and perhaps other choices?
   // Also, why not make this a typedef, shared with schedule_default.
   // And also WeeklySchedule, ExceptionSchedule, and Command's ActionList
   // and anywhere else we need a primitive ANY.
   //
   // TODO: WeeklySchedule and ExceptionSchedule use ActionValueType to specify type,
   // but code in Vtsapi32 and DUDTOOL use ParseType to value_type, tangled up
   // with the "none" hack in Vtsapi32 to tweak value_type based on the referenced property.
   // This is pretty much guaranteed to be broken.
   union {
        enum BACnetBinaryPV   bproperty_value;  //binary value
        word            uproperty_value;  //unsigned word
        float           fproperty_value;  //float
        }               pv;
   BACnetDateRange     effective_period;
   BACnetTimeValue     *weekly_schedule[7];
   BACnetExceptionSchedule   exception_schedule;
   BACnetDeviceObjectPropertyReference*list_obj_prop_ref;
   word                priority_for_writing;

   union {
        enum BACnetBinaryPV   bproperty_value;  //binary value
        word            uproperty_value;  //unsigned word
        float           fproperty_value;  //float
        }               schedule_default;
} schedule_obj_type;

//Averaging Object
//-------------------------------------------------------
typedef struct
{
    generic_object                  go;
    float                           minimum_value;
    BACnetDateTime                  minimum_value_timestamp;
    float                           average_value;
    float                           variance_value;
    float                           maximum_value;
    BACnetDateTime                  maximum_value_timestamp;
    word                            attempted_samples;
    word                            valid_samples;
    BACnetDeviceObjectPropertyReference   obj_prop_ref;
    word                           window_interval;
    word                           window_samples;
} avg_obj_type;

// --------------------------------------------------------
//Multi-state Value Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    word                present_value;
    word                number_of_states;
    char                *state_text[MAX_STATE_TEXTS];
    word                priority_array[16];
    word                relinquish_default;
    UnsignedList        *alarm_values;
    UnsignedList        *fault_values;
} msv_obj_type;

// --------------------------------------------------------
// TrendLog Object
// --------------------------------------------------------
typedef struct
{
    generic_object                  go;
    bool                            log_enable;
    BACnetDateTime                  start_time;
    BACnetDateTime                  stop_time;
    BACnetDeviceObjectPropertyReference    log_device_object_property;
    dword                           log_interval;
    word                            cov_resubscription_interval;
    float                           client_cov_increment;
    bool                            stop_when_full;
    bool                            align_intervals;
    bool                            trigger;
    word                            logging_type;
    dword                           interval_offset;
    dword                           buffer_size;
    BACnetLogRecord                 log_buffer;
    dword                           record_count;
    dword                           total_record_count;
    dword                           notification_threshold;
    dword                           records_since_notification;
    word                            alarm_value;
    word                            fault_values;
    dword                           last_notify_record;
} trend_obj_type;

// --------------------------------------------------------
// Proprietary Object
// --------------------------------------------------------
typedef struct
{
   generic_object go;
} proprietary_obj_type;

typedef struct
{
    generic_object            go;
enum BACnetLifeSafetyState    present_value;
enum BACnetLifeSafetyState    tracking_value;
    char                      device_type[MAX_TEXT_STRING];
enum BACnetLifeSafetyMode     mode;
    BACnetEnumList            *accepted_modes; // List of BACnetLifeSafetyMode
    BACnetEnumList            *life_safety_alarm_values; // List of LifeSafetyState
    BACnetEnumList            *alarm_values; // List of LifeSafetyState
    BACnetEnumList            *fault_values; // List of LifeSafetyState
enum BACnetSilencedState      silenced; 
enum BACnetLifeSafetyOperation operation_expected;
enum BACnetMaintenance        maintenance_required;
    word                      setting;
    float                     direct_reading;
    word                      units;   // WAS enum BACnetEngineeringUnits.  See NOTE above
    BACnetDeviceObjectReference   *member_of;
} lifesafetypoint_obj_type;

typedef struct
{
    generic_object            go;
enum BACnetLifeSafetyState    present_value;
enum BACnetLifeSafetyState    tracking_value;
    char                      device_type[MAX_TEXT_STRING];
enum BACnetLifeSafetyMode     mode;
    BACnetEnumList            *accepted_modes; // List of BACnetLifeSafetyMode
    BACnetEnumList            *life_safety_alarm_values; // List of LifeSafetyState
    BACnetEnumList            *alarm_values; // List of LifeSafetyState
    BACnetEnumList            *fault_values; // List of LifeSafetyState
enum BACnetSilencedState      silenced; 
enum BACnetLifeSafetyOperation operation_expected;
     bool                      maintenance_required;
     BACnetDeviceObjectReference *zone_members; 
     BACnetDeviceObjectReference *member_of;  
} lifesafetyzone_obj_type;

typedef struct
{
   generic_object             go;
   word                       present_value;
   char                       device_type[MAX_TEXT_STRING];
   BACnetScale                scale;
   word                       units;   // WAS enum BACnetEngineeringUnits.  See NOTE above
   BACnetPrescale             prescale;
   word                       max_pres_value;
   BACnetDateTime             value_change_time;
   word                       value_before_change;
   word                       value_set;
   BACnetAccumulatorRecord    logging_record;
   BACnetObjectIdentifier     logging_device;
   word                       pulse_rate;
   word                       high_limit;
   word                       low_limit;
   word                       limit_monitoring_interval;
} accumulator_obj_type;

typedef struct
{
   generic_object              go;
   float                       present_value;
   BACnetObjectPropertyReference  input_reference;
   word                       units;   // WAS enum BACnetEngineeringUnits.  See NOTE above
   float                      scale_factor;
   float                      adjust_value;
   word                       count;
   BACnetDateTime             update_time;
   BACnetDateTime             count_change_time;
   word                       count_before_change;
   float                      cov_increment;
   word                       cov_period;
   float                      high_limit;
   float                      low_limit;
   float                      deadband;
} pulseconverter_obj_type;

#define MAX_SHED_LEVELS 255
typedef struct
{
   generic_object             go;
   BACnetShedLevel            requested_shed_level;
   DWORD                      shed_duration;
   DWORD                      duty_window;
   BACnetShedLevel            expected_shed_level;
   BACnetShedLevel            actual_shed_level;
   UnsignedList               *shed_levels;
   char                       *shed_level_descriptions[MAX_SHED_LEVELS];
   bool                       log_enable;
   BACnetDateTime             start_time;
   enum BACnetShedState       present_value;
   char                       state_description[MAX_TEXT_STRING];
   float                      full_duty_baseline;
} lc_obj_type;

typedef struct
{
   generic_object       go;
   BACnetEnumList       *alarm_values;
   BACnetEnumList       *fault_values;

   enum BACnetDoorValue present_value;
   enum BACnetDoorValue relinquish_default;
   enum BACnetDoorValue priority_array[16];
   enum BACnetDoorStatus   door_status;
   enum BACnetLockStatus   lock_status;
   enum BACnetDoorSecuredStatus  secured_status;
   BACnetDeviceObjectReference*door_members;
   word              door_pulse_time;
   word              door_extended_pulse_time;
   word              door_unlock_delay_time;
   word              door_open_too_long_time;
   enum BACnetDoorAlarmState  door_alarm_state;
   BACnetEnumList      *masked_alarm_values; //enum BACnetDoorAlarmState 
   enum BACnetMaintenance  maintenance_required;
} ad_obj_type;

#define MAX_SV_ANNOTATIONS 255
// Structured-View
typedef struct
{
   generic_object       go;
   enum BACnetNodeType  node_type;
   char                 node_subtype[MAX_TEXT_STRING];
   BACnetDeviceObjectReference*subordinate_list;
   char                 *subordinate_annotations[MAX_SV_ANNOTATIONS];
} sv_obj_type;

typedef struct
{
   generic_object                  go;
   bool                            log_enable;
   bool                            stop_when_full;
   BACnetDateTime                  start_time;
   BACnetDateTime                  stop_time;
   dword                           buffer_size;
   BACnetLogRecord                 log_buffer;
   dword                           record_count;
   dword                           total_record_count;
   dword                           notification_threshold;
   dword                           records_since_notification;
   dword                           last_notify_record;
} el_obj_type;

typedef struct
{
   generic_object          go;
   dword                   update_interval;
   dword                   requested_update_interval;
   dword                   cov_resubscription_interval;
   dword                   covu_period;
   BACnetRecipient         *covu_recipients;
} global_group_obj_type;

typedef struct
{
   generic_object          go;
   bool                    log_enable;
   BACnetDateTime          start_time;
   BACnetDateTime          stop_time;
   // TODO: should be an ARRAY OF BACnetDeviceObjectPropertyReference
   // like the LIST OF in ther Schedule
   BACnetDeviceObjectPropertyReference    log_device_object_property;
   enum BACnetLoggingType  logging_type;
   bool                    align_intervals;
   word                    interval_offset;
   bool                    trigger;
   bool                    stop_when_full;
   dword                   log_interval;
   dword                   buffer_size;
   BACnetLogRecord         log_buffer;
   dword                   record_count;
   dword                   total_record_count;
   dword                   notification_threshold;
   dword                   records_since_notification;
   dword                   last_notify_record;
} tlm_obj_type;

// --------------------------------------------------------
// A placeholder until *someone* fleshes out the real
// properties of a new Object Type.
//-------------------------------------------------------
typedef struct {
   generic_object         go;
} placeholder_obj_type;

#define MAX_FAULT_STRINGS 8
// --------------------------------------------------------
//Characterstring Value Object
//-------------------------------------------------------
typedef struct {
   generic_object       go;
   char                 present_value[MAX_TEXT_STRING];
   // TODO: this needs to be an array of STRING/NULL choice
   word                 priority_array[16];
   char                 relinquish_default[MAX_TEXT_STRING];
   char                 *alarm_values[MAX_FAULT_STRINGS];
   char                 *fault_values[MAX_FAULT_STRINGS];
} charstring_obj_type;

//-------------------------------------------------------
//Integer Value Object INTEGER_VALUE
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    int                 present_value;
    word                units;   // WAS enum BACnetEngineeringUnits.  See NOTE above
    int                 priority_array[16];
    int                 relinquish_default;
    int                 cov_increment;
    int                 high_limit;
    int                 low_limit;
    unsigned int        deadband;
    int                 min_pres_value;
    int                 max_pres_value;
    int                 resolution;
} integer_obj_type;

//-------------------------------------------------------
//Positive Integer Value Object POSITIVE_INTEGER_VALUE
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    unsigned int        present_value;
    word                units;   // WAS enum BACnetEngineeringUnits.  See NOTE above
    unsigned int        priority_array[16];
    unsigned int        relinquish_default;
    int                 cov_increment;
    unsigned int        high_limit;
    unsigned int        low_limit;
    unsigned int        deadband;
    unsigned int        min_pres_value;
    unsigned int        max_pres_value;
    unsigned int        resolution;
} positive_integer_obj_type;

//-------------------------------------------------------
//DateTime Value Object
//-------------------------------------------------------
typedef struct {
    generic_object      go;
    BACnetDateTime      present_value;
    BACnetDateTime      priority_array[16];
    BACnetDateTime      relinquish_default;
    bool                is_utc;
   } datetimevalue_obj_type;

#endif //__STDOBJ_H_INCLUDED
