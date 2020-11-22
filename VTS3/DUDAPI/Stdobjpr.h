//STDOBJPR.H
/* Revision: 16 September 2015
   Updated to ANSI/ASHRAE Standard 135-2012
*/

//property definitions for standard objects
#ifndef __STDOBJPR_H_INCLUDED
#define __STDOBJPR_H_INCLUDED

#include "propid.h"
#include "props.h"

// Property definition table entries
typedef struct
{
   dword PropID;
   word  StrucOffset;
   octet ParseType;
   octet PropGroup;
   word  PropET;     // One of the property table defines (eg, eiEvState) or else (fixed max) array size for select properties
   word  PropFlags;
   word  firstRevision;   // First protocol revision in which the property appeared
// word  lastRevision;    // Last protocol revision in which the property appeared (deprecated properties)
} propdescriptor;

typedef struct
{
   word           sotSize;             //size of one of these objects in bytes
   propdescriptor *sotProps;           //pointer to table of property descriptors
} stdobjtype;

#if _DoStaticPropDescriptors

// TODO: These tables really should be moved to a CPP.  In addition to being
// icky to have data in a .h file, it also thwarts Dev Studio's hover showing values.

// If the columns get wobbly, you can use the VTS utility ProperTable to clean them up.

// Macro for clarity in propdescriptor tables.
#define xxoo(typ,m)   (size_t)( (char *)&(((typ*)0)->m) - (char *)0 )
// The macro oo is redefined for each object type, to permit us to omit the struct name
// from each usage. That makes it much easier to copy and paste new properties into
// multiple tables without needing to edit them for each table.

// Use this until someone fleshes out the real properties for a new object type.
// The generic object properties - always supported
#undef oo
#define oo(m)  xxoo(placeholder_obj_type,m)
propdescriptor PlaceholderProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { LIMIT_ENABLE,                   oo(go.limit_enable),               bits,        Intr,     eiLimEn,            O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   // <More properties here>
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(proprietary_obj_type,m)
propdescriptor ProprietaryObjProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevisio
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(ai_obj_type,m)
propdescriptor AIprops[]=
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            flt,         0,        0,                  R|Woutofservice|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DEVICE_TYPE,                    oo(device_type),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { UPDATE_INTERVAL,                oo(update_interval),               uw,          0,        0,                  O },
   { UNITS,                          oo(units),                         et,          0,        eiEU,               R },
   { MIN_PRES_VALUE,                 oo(min_pres_value),                flt,         0,        0,                  O },
   { MAX_PRES_VALUE,                 oo(max_pres_value),                flt,         0,        0,                  O },
   { RESOLUTION,                     oo(resolution),                    flt,         0,        0,                  O },
   { COV_INCREMENT,                  oo(cov_increment),                 flt,         COV,      0,                  O|WithService },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { HIGH_LIMIT,                     oo(high_limit),                    flt,         Intr,     0,                  O|WithService },
   { LOW_LIMIT,                      oo(low_limit),                     flt,         Intr,     0,                  O|WithService },
   { DEADBAND,                       oo(deadband),                      flt,         Intr,     0,                  O|WithService },
   { LIMIT_ENABLE,                   oo(go.limit_enable),               bits,        Intr,     eiLimEn,            O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(ao_obj_type,m)
propdescriptor AOprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            flt,         0,        0,                  W|IsCommandable|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DEVICE_TYPE,                    oo(device_type),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { UNITS,                          oo(units),                         et,          0,        eiEU,               R },
   { MIN_PRES_VALUE,                 oo(min_pres_value),                flt,         0,        0,                  O },
   { MAX_PRES_VALUE,                 oo(max_pres_value),                flt,         0,        0,                  O },
   { RESOLUTION,                     oo(resolution),                    flt,         0,        0,                  O },
   { PRIORITY_ARRAY,                 oo(priority_array),                paf,         0,        0,                  R|IsArray|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            flt,         0,        0,                  R },
   { COV_INCREMENT,                  oo(cov_increment),                 flt,         COV,      0,                  O|WithService },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { HIGH_LIMIT,                     oo(high_limit),                    flt,         Intr,     0,                  O|WithService },
   { LOW_LIMIT,                      oo(low_limit),                     flt,         Intr,     0,                  O|WithService },
   { DEADBAND,                       oo(deadband),                      flt,         Intr,     0,                  O|WithService },
   { LIMIT_ENABLE,                   oo(go.limit_enable),               bits,        Intr,     eiLimEn,            O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(av_obj_type,m)
propdescriptor AVprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            flt,         0,        0,                  W|IsCommandable },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O|QVAL },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { UNITS,                          oo(units),                         et,          0,        eiEU,               R },
   { PRIORITY_ARRAY,                 oo(priority_array),                paf,         1,        0,                  O|WithGroup|IsArray|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            flt,         1,        0,                  O|WithGroup },
   { COV_INCREMENT,                  oo(cov_increment),                 flt,         COV,      0,                  O|WithService },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { HIGH_LIMIT,                     oo(high_limit),                    flt,         Intr,     0,                  O|WithService },
   { LOW_LIMIT,                      oo(low_limit),                     flt,         Intr,     0,                  O|WithService },
   { DEADBAND,                       oo(deadband),                      flt,         Intr,     0,                  O|WithService },
   { LIMIT_ENABLE,                   oo(go.limit_enable),               bits,        Intr,     eiLimEn,            O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { MIN_PRES_VALUE,                 oo(min_pres_value),                flt,         0,        0,                  O,        14 },
   { MAX_PRES_VALUE,                 oo(max_pres_value),                flt,         0,        0,                  O,        14 },
   { RESOLUTION,                     oo(resolution),                    flt,         0,        0,                  O,        14 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(bi_obj_type,m)
propdescriptor BIprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            et,          0,        eiBPV,              R|Woutofservice|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DEVICE_TYPE,                    oo(device_type),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { POLARITY,                       oo(polarity),                      et,          0,        eiPolar,            R },
   { INACTIVE_TEXT,                  oo(inactive_text),                 ch_string,   1,        0,                  O|WithGroup },
   { ACTIVE_TEXT,                    oo(active_text),                   ch_string,   1,        0,                  O|WithGroup },
   { CHANGE_OF_STATE_TIME,           oo(cos_time),                      dt,          2,        0,                  O|WithGroup|QVAL },
   { CHANGE_OF_STATE_COUNT,          oo(cos_count),                     uw,          2,        0,                  O|WithGroup|QVAL },
   { TIME_OF_STATE_COUNT_RESET,      oo(time_of_state_count_reset),     dt,          2,        0,                  O|WithGroup },
   { ELAPSED_ACTIVE_TIME,            oo(elapsed_active_time),           ud,          3,        0,                  O|WithGroup|QVAL },
   { TIME_OF_ACTIVE_TIME_RESET,      oo(time_of_active_time_reset),     dt,          3,        0,                  O|WithGroup },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { ALARM_VALUE,                    oo(alarm_value),                   et,          Intr,     eiBPV,              O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(bo_obj_type,m)
propdescriptor BOprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            et,          0,        eiBPV,              W|IsCommandable|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DEVICE_TYPE,                    oo(device_type),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { POLARITY,                       oo(polarity),                      et,          0,        eiPolar,            R },
   { INACTIVE_TEXT,                  oo(inactive_text),                 ch_string,   1,        0,                  O|WithGroup },
   { ACTIVE_TEXT,                    oo(active_text),                   ch_string,   1,        0,                  O|WithGroup },
   { CHANGE_OF_STATE_TIME,           oo(cos_time),                      dt,          2,        0,                  O|WithGroup|QVAL },
   { CHANGE_OF_STATE_COUNT,          oo(cos_count),                     uw,          2,        0,                  O|WithGroup|QVAL },
   { TIME_OF_STATE_COUNT_RESET,      oo(time_of_state_count_reset),     dt,          2,        0,                  O|WithGroup },
   { ELAPSED_ACTIVE_TIME,            oo(elapsed_active_time),           ud,          3,        0,                  O|WithGroup|QVAL },
   { TIME_OF_ACTIVE_TIME_RESET,      oo(time_of_active_time_reset),     dt,          3,        0,                  O|WithGroup },
   { MINIMUM_OFF_TIME,               oo(min_off_time),                  ud,          0,        0,                  O },
   { MINIMUM_ON_TIME,                oo(min_on_time),                   ud,          0,        0,                  O },
   { PRIORITY_ARRAY,                 oo(priority_array),                pab,         0,        eiBPVn,             R|IsArray|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            et,          0,        eiBPV,              R },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { FEEDBACK_VALUE,                 oo(feedback_value),                et,          Intr,     eiBPV,              O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(bv_obj_type,m)
propdescriptor BVprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            et,          0,        eiBPV,              R|Woutofservice|IsCommandable|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { INACTIVE_TEXT,                  oo(inactive_text),                 ch_string,   1,        0,                  O|WithGroup },
   { ACTIVE_TEXT,                    oo(active_text),                   ch_string,   1,        0,                  O|WithGroup },
   { CHANGE_OF_STATE_TIME,           oo(cos_time),                      dt,          2,        0,                  O|WithGroup|QVAL },
   { CHANGE_OF_STATE_COUNT,          oo(cos_count),                     uw,          2,        0,                  O|WithGroup|QVAL },
   { TIME_OF_STATE_COUNT_RESET,      oo(time_of_state_count_reset),     dt,          2,        0,                  O|WithGroup },
   { ELAPSED_ACTIVE_TIME,            oo(elapsed_active_time),           ud,          3,        0,                  O|WithGroup|QVAL },
   { TIME_OF_ACTIVE_TIME_RESET,      oo(time_of_active_time_reset),     dt,          3,        0,                  O|WithGroup },
   { MINIMUM_OFF_TIME,               oo(min_off_time),                  ud,          0,        0,                  O },
   { MINIMUM_ON_TIME,                oo(min_on_time),                   ud,          0,        0,                  O },
   { PRIORITY_ARRAY,                 oo(priority_array),                pab,         4,        eiBPVn,             O|WithGroup|IsArray|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            et,          4,        eiBPV,              O|WithGroup },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { ALARM_VALUE,                    oo(alarm_value),                   et,          Intr,     eiBPV,              O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(calendar_obj_type,m)
propdescriptor CLprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            ebool,       0,        eiTF,               R|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DATE_LIST,                      oo(date_list),                     calist,      0,        0,                  R },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(command_obj_type,m)
propdescriptor CMprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { PRESENT_VALUE,                  oo(pv),                            uw,          0,        0,                  W|QVAL },
   { IN_PROCESS,                     oo(in_process),                    ebool,       0,        eiTF,               R|QVAL },
   { ALL_WRITES_SUCCESSFUL,          oo(all_writes_successful),         ebool,       0,        eiTF,               R|QVAL },
   { ACTION,                         oo(action),                        act,         0,        MAX_ACTION_TEXTS,   R|IsArray },
   { ACTION_TEXT,                    oo(action_text),                   actext,      0,        MAX_ACTION_TEXTS,   O|IsArray },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(device_obj_type,m)
propdescriptor DVprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { SYSTEM_STATUS,                  oo(system_status),                 et,          0,        eiDS,               R|QVAL },
   { VENDOR_NAME,                    oo(vendor_name),                   ch_string,   0,        0,                  R },
   { VENDOR_IDENTIFIER,              oo(vendor_id),                     uw,          0,        0,                  R },
   { MODEL_NAME,                     oo(model_name),                    ch_string,   0,        0,                  R },
   { FIRMWARE_REVISION,              oo(firmware_rev),                  ch_string,   0,        0,                  R },
   { APPLICATION_SOFTWARE_VERSION,   oo(application_software_ver),      ch_string,   0,        0,                  R },
   { LOCATION,                       oo(location),                      ch_string,   0,        0,                  O },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { PROTOCOL_VERSION,               oo(protocol_ver),                  uw,          0,        0,                  R },
   { PROTOCOL_REVISION,              oo(protocol_rev),                  uw,          0,        0,                  R },
   { PROTOCOL_CONFORMANCE_CLASS,     oo(protocol_conf_class),           uw,          0,        0,                  O },  // TODO: was required, now deprecated...
   { PROTOCOL_SERVICES_SUPPORTED,    oo(protocol_services_supported),   pss,         0,        0,                  R },
   { PROTOCOL_OBJECT_TYPES_SUPPORTED, oo(object_types_supported),       pos,         0,        0,                  R },
   { OBJECT_LIST,                    oo(object_list),                   looref,      0,        0,                  R|IsArray },
   { STRUCTURED_OBJECT_LIST,         oo(structured_object_list),        looref,      0,        0,                  O|IsArray, 7 },
   { MAX_APDU_LENGTH_ACCEPTED,       oo(max_apdu_length_accepted),      uw,          0,        0,                  R },
   { SEGMENTATION_SUPPORTED,         oo(segmentation_supported),        et,          0,        eiSegOpt,           R },
   { MAX_SEGMENTS_ACCEPTED,          oo(max_segments_accepted),         ud,          0,        0,                  O },
   { VT_CLASSES_SUPPORTED,           oo(vt_classes_supported),          vtcl,        VT,       0,                  O|WithService|WithGroup },
   { ACTIVE_VT_SESSIONS,             oo(active_vt_sessions),            vtse,        VT,       0,                  O|WithService|WithGroup },
   { LOCAL_TIME,                     oo(local_time),                    ttime,       0,        0,                  O|QVAL },
   { LOCAL_DATE,                     oo(local_date),                    ddate,       0,        0,                  O|QVAL },
   { UTC_OFFSET,                     oo(utc_offset),                    ssint,       0,        0,                  O },
   { DAYLIGHT_SAVINGS_STATUS,        oo(day_savings_status),            ebool,       0,        eiTF,               O|QVAL },
   { APDU_SEGMENT_TIMEOUT,           oo(apdu_segment_timeout),          ud,          SEGSVC,   0,                  O|WithService },
   { APDU_TIMEOUT,                   oo(apdu_timeout),                  ud,          0,        0,                  R },
   { NUMBER_OF_APDU_RETRIES,         oo(number_apdu_retries),           uw,          0,        0,                  R },
   { LIST_OF_SESSION_KEYS,           oo(list_session_keys),             skeys,       0,        0,                  O|QVAL }, // TODO: Deprecated
   { TIME_SYNCHRONIZATION_RECIPIENTS, oo(time_synch_recipients),        tsrecip,     TMASTER,  0,                  O|WithService },
   { MAX_MASTER,                     oo(max_master),                    u127,        MSTPMASTER, 0,                O|WithService },
   { MAX_INFO_FRAMES,                oo(max_info_frames),               uw,          MSTPMASTER, 0,                O|WithService },
   { DEVICE_ADDRESS_BINDING,         oo(device_add_binding),            dabind,      0,        0,                  R|QVAL },
   { DATABASE_REVISION,              oo(database_revision),             ud,          0,        0,                  R|QVAL },
   { CONFIGURATION_FILES,            oo(configuration_files),           looref,      0,        0,                  O|IsArray },
   { LAST_RESTORE_TIME,              oo(last_restore_time),             TSTMP,       0,        0,                  O|QVAL },
   { BACKUP_FAILURE_TIMEOUT,         oo(backup_failure_timeout),        uw,          0,        0,                  O },
   { BACKUP_PREPARATION_TIME,        oo(backup_preparation_time),       uw,          0,        0,                  O,        10 },
   { RESTORE_PREPARATION_TIME,       oo(restore_preparation_time),      uw,          0,        0,                  O,        10 },
   { RESTORE_COMPLETION_TIME,        oo(restore_completion_time),       uw,          0,        0,                  O,        10 },
   { BACKUP_AND_RESTORE_STATE,       oo(backup_and_restore_state),      et,          0,        eiBackupState,      O|QVAL,   10 },
   { ACTIVE_COV_SUBSCRIPTIONS,       oo(active_cov_subscriptions),      lCOVSub,     0,        0,                  O|IsArray|QVAL },
   { SLAVE_PROXY_ENABLE,             oo(slave_proxy_enable),            eboollist,   0,        0,                  O },
   { MANUAL_SLAVE_ADDRESS_BINDING,   oo(manual_slave_add_bind),         dabind,      0,        0,                  O },
   { AUTO_SLAVE_DISCOVERY,           oo(auto_slave_disc),               eboollist,   0,        0,                  O },
   { SLAVE_ADDRESS_BINDING,          oo(slave_add_bind),                dabind,      0,        0,                  O|QVAL },
   { LAST_RESTART_REASON,            oo(last_restart_reason),           et,          0,        eiRestartReason,    O|QVAL,   7 },
   { TIME_OF_DEVICE_RESTART,         oo(time_of_device_restart),        TSTMP,       0,        0,                  O|QVAL,   7 },
   { RESTART_NOTIFICATION_RECIPIENTS, oo(restart_notification_recipients), tsrecip,  0,        0,                  O,        7 },
   { UTC_TIME_SYNCHRONIZATION_RECIPIENTS, oo(utc_time_synchronization_recipients), tsrecip, TMASTER, 0,            O|WithService, 7 },
   { TIME_SYNCHRONIZATION_INTERVAL,  oo(time_synchronization_interval), uw,          0,        0,                  O,        7 },
   { ALIGN_INTERVALS,                oo(align_intervals),               ebool,       0,        eiTF,               O,        7 },
   { INTERVAL_OFFSET,                oo(interval_offset),               uw,          0,        0,                  O,        7 },
   { SERIAL_NUMBER,                  oo(serial_number),                 ch_string,   0,        0,                  O,        14 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(ee_obj_type,m)
propdescriptor EEprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { EVENT_TYPE,                     oo(parameter_list.event_type),     et,          0,        eiEvType,           R },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          0,        eiNT,               R },
   { EVENT_PARAMETERS,               oo(parameter_list),                evparm,      0,        0,                  R },
   { OBJECT_PROPERTY_REFERENCE,      oo(obj_prop_ref),                  devobjpropref, 0,      0,                  R },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        0,        eiEvTr,             R },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        0,        eiEvTr,             R|QVAL },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          1,        0,                  R|NotWithGroup2 },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    0,        0,                  R|IsArray|QVAL },
   // Note these properties were removed for revision 4
   { RECIPIENT,                      oo(recipient),                     recip,       2,        0,                  O|NotWithGroup1 },
   { PROCESS_IDENTIFIER,             oo(process_id),                    uw,          2,        0,                  O|NotWithGroup1 },
   { PRIORITY,                       oo(priority),                      uw,          2,        0,                  O|NotWithGroup1 },
   { ISSUE_CONFIRMED_NOTIFICATIONS,  oo(issue_conf_notifications),      ebool,       2,        eiTF,               O|NotWithGroup1 },
   // Note the above properties were removed for revision 4
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       0,        0,                  R,        13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R,        13 },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             R,        13 },
   // TODO: need to implement these
// { FAULT_TYPE,     oo(fault_type),     et,      0,  eiFaultType, O,                              13 },
// { FAULT_PARAMETERS, oo(fault_parameters),  PT_FAULT_PARMS, 0,  0, O,                            13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(file_obj_type,m)
propdescriptor FLprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { FILE_TYPE,                      oo(file_type),                     ch_string,   0,        0,                  R },
   { FILE_SIZE,                      oo(file_size),                     ud,          0,        0,                  R },
   { MODIFICATION_DATE,              oo(mod_date),                      dt,          0,        0,                  R },
   { ARCHIVE,                        oo(archive),                       ebool,       0,        eiTF,               W },
   { READ_ONLY,                      oo(read_only),                     ebool,       0,        eiTF,               R },
   { FILE_ACCESS_METHOD,             oo(access_method),                 et,          0,        eiFAM,              R },
   { RECORD_COUNT,                   oo(record_count),                  ud,          0,        0,                  O },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(group_obj_type,m)
propdescriptor GRprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { LIST_OF_GROUP_MEMBERS,          oo(list_of_group_members),         raslist,     0,        0,                  R },
   { PRESENT_VALUE,                  0,                                 none,        0,        0,                  R|QVAL },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(loop_obj_type,m)
propdescriptor LPprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            flt,         0,        0,                  R|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { UPDATE_INTERVAL,                oo(update_interval),               uw,          0,        0,                  O },
   { OUTPUT_UNITS,                   oo(output_units),                  et,          0,        eiEU,               R },
   { MANIPULATED_VARIABLE_REFERENCE, oo(man_var_ref),                   propref,     0,        0,                  R },
   { CONTROLLED_VARIABLE_REFERENCE,  oo(cont_var_ref),                  propref,     0,        0,                  R },
   { CONTROLLED_VARIABLE_VALUE,      oo(cont_var_value),                flt,         0,        0,                  R|QVAL },
   { CONTROLLED_VARIABLE_UNITS,      oo(cont_var_units),                et,          0,        eiEU,               R },
   { SETPOINT_REFERENCE,             oo(setpoint_ref),                  setref,      0,        0,                  R },
   { SETPOINT,                       oo(setpoint),                      flt,         0,        0,                  R },
   { ACTION,                         oo(action),                        et,          0,        eiLoopAct,          R },
   { PROPORTIONAL_CONSTANT,          oo(proportional_const),            flt,         1,        0,                  O|WithGroup },
   { PROPORTIONAL_CONSTANT_UNITS,    oo(proportional_const_units),      et,          1,        eiEU,               O|WithGroup },
   { INTEGRAL_CONSTANT,              oo(integral_const),                flt,         2,        0,                  O|WithGroup },
   { INTEGRAL_CONSTANT_UNITS,        oo(integral_const_units),          et,          2,        eiEU,               O|WithGroup },
   { DERIVATIVE_CONSTANT,            oo(derivative_const),              flt,         3,        0,                  O|WithGroup },
   { DERIVATIVE_CONSTANT_UNITS,      oo(derivative_const_units),        et,          3,        eiEU,               O|WithGroup },
   { BIAS,                           oo(bias),                          flt,         0,        0,                  O },
   { MAXIMUM_OUTPUT,                 oo(max_output),                    flt,         0,        0,                  O },
   { MINIMUM_OUTPUT,                 oo(min_output),                    flt,         0,        0,                  O },
   { PRIORITY_FOR_WRITING,           oo(priority_for_writing),          u16,         0,        0,                  R },
   { COV_INCREMENT,                  oo(cov_increment),                 flt,         COV,      0,                  O|WithService },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { ERROR_LIMIT,                    oo(error_limit),                   flt,         Intr,     0,                  O|WithService },
   { DEADBAND,                       oo(deadband),                      flt,         Intr,     0,                  O|WithService, 7 },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(mi_obj_type,m)
propdescriptor MIprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            uw,          0,        0,                  R|Woutofservice|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DEVICE_TYPE,                    oo(device_type),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { NUMBER_OF_STATES,               oo(num_of_states),                 uw,          0,        0,                  R },
   { STATE_TEXT,                     oo(state_text),                    statext,     0,        MAX_STATE_TEXTS,    O|IsArray },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { ALARM_VALUES,                   oo(alarm_values),                  stavals,     Intr,     0,                  O|WithService },
   { FAULT_VALUES,                   oo(fault_values),                  stavals,     Intr,     0,                  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(mo_obj_type,m)
propdescriptor MOprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            uw,          0,        0,                  W|IsCommandable|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DEVICE_TYPE,                    oo(device_type),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { NUMBER_OF_STATES,               oo(num_of_states),                 uw,          0,        0,                  R },
   { STATE_TEXT,                     oo(state_text),                    statext,     0,        MAX_STATE_TEXTS,    O|IsArray },
   { PRIORITY_ARRAY,                 oo(priority_array),                pau,         0,        0,                  R|IsArray|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            uw,          0,        0,                  R },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { FEEDBACK_VALUE,                 oo(feedback_value),                uw,          Intr,     0,                  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(nc_obj_type,m)
propdescriptor NCprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          0,        0,                  R },
   { PRIORITY,                       oo(priority),                      uwarr,       0,        0,                  R|IsArray },
   { ACK_REQUIRED,                   oo(ack_required),                  bits,        0,        eiEvTr,             R },
   { RECIPIENT_LIST,                 oo(recipient_list),                reciplist,   0,        0,                  R },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(program_obj_type,m)
propdescriptor PRprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PROGRAM_STATE,                  oo(prog_state),                    et,          0,        eiPrState,          R|QVAL },
   { PROGRAM_CHANGE,                 oo(prog_change),                   et,          0,        eiPrChg,            W },
   { REASON_FOR_HALT,                oo(reason_for_halt),               et,          1,        eiPrErr,            O|WithGroup },
   { DESCRIPTION_OF_HALT,            oo(description_of_halt),           ch_string,   1,        0,                  O|WithGroup },
   { PROGRAM_LOCATION,               oo(prog_location),                 ch_string,   0,        0,                  O },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { INSTANCE_OF,                    oo(instance_of),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService, 13 },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService, 13 },
   { EVENT_STATE,                    oo(go.event_state),                et,          Intr,     eiEvState,          O|WithService, 13|QVAL },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService, 13|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService, 13 },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL, 13 },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(schedule_obj_type,m)
propdescriptor SCprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(pv),                            none,        0,        0,                  R|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { EFFECTIVE_PERIOD,               oo(effective_period),              dtrange,     0,        0,                  R },
   { WEEKLY_SCHEDULE,                oo(weekly_schedule),               wsched,      1,        0,                  O|AtLeast1|IsArray },
   { EXCEPTION_SCHEDULE,             oo(exception_schedule),            xsched,      1,        0,                  O|AtLeast1|IsArray },
   { SCHEDULE_DEFAULT,               oo(schedule_default),              none,        0,        0,                  R },
   { LIST_OF_OBJECT_PROPERTY_REFERENCES, oo(list_obj_prop_ref),         lopref,      0,        0,                  R },
   { PRIORITY_FOR_WRITING,           oo(priority_for_writing),          u16,         0,        0,                  R },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              O|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O|QVAL },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               O },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService, 13 },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService, 13 },
   { EVENT_STATE,                    oo(go.event_state),                et,          Intr,     eiEvState,          O|WithService, 13|QVAL },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService, 13|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService, 13 },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL, 13 },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(avg_obj_type,m)
propdescriptor AVGprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { MINIMUM_VALUE,                  oo(minimum_value),                 flt,         0,        0,                  R|QVAL },
   { MINIMUM_VALUE_TIMESTAMP,        oo(minimum_value_timestamp),       dt,          0,        0,                  O|QVAL },
   { AVERAGE_VALUE,                  oo(average_value),                 flt,         0,        0,                  R|QVAL },
   { VARIANCE_VALUE,                 oo(variance_value),                flt,         0,        0,                  O|QVAL },
   { MAXIMUM_VALUE,                  oo(maximum_value),                 flt,         0,        0,                  O|QVAL },
   { MAXIMUM_VALUE_TIMESTAMP,        oo(maximum_value_timestamp),       dt,          0,        0,                  O|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { ATTEMPTED_SAMPLES,              oo(attempted_samples),             uw,          0,        0,                  W },
   { VALID_SAMPLES,                  oo(valid_samples),                 uw,          0,        0,                  R|QVAL },
   { OBJECT_PROPERTY_REFERENCE,      oo(obj_prop_ref),                  devobjpropref, 0,      0,                  R },
   { WINDOW_INTERVAL,                oo(window_interval),               uw,          0,        0,                  W },
   { WINDOW_SAMPLES,                 oo(window_samples),                uw,          0,        0,                  W },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(msv_obj_type,m)
propdescriptor MVprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(present_value),                 uw,          0,        0,                  W|IsCommandable|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { NUMBER_OF_STATES,               oo(number_of_states),              uw,          0,        0,                  R },
   { STATE_TEXT,                     oo(state_text),                    statext,     0,        MAX_STATE_TEXTS,    O|IsArray },
   { PRIORITY_ARRAY,                 oo(priority_array),                pau,         3,        0,                  O|IsArray|WithGroup|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            uw,          3,        0,                  O|WithGroup },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { ALARM_VALUES,                   oo(alarm_values),                  stavals,     Intr,     0,                  O|WithService },
   { FAULT_VALUES,                   oo(fault_values),                  stavals,     Intr,     0,                  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(trend_obj_type,m)
propdescriptor TRprops[]={
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { ENABLE,                         oo(log_enable),                    ebool,       0,        eiTF,               W },
   { START_TIME,                     oo(start_time),                    dt,          0,        0,                  O },
   { STOP_TIME,                      oo(stop_time),                     dt,          0,        0,                  O },
   { LOG_DEVICE_OBJECT_PROPERTY,     oo(log_device_object_property),    devobjpropref, 0,      0,                  O },
   { LOG_INTERVAL,                   oo(log_interval),                  ud,          0,        0,                  O },
   { COV_RESUBSCRIPTION_INTERVAL,    oo(cov_resubscription_interval),   uw,          0,        0,                  O },
   { CLIENT_COV_INCREMENT,           oo(client_cov_increment),          flt,         0,        0,                  O },
   { STOP_WHEN_FULL,                 oo(stop_when_full),                ebool,       0,        eiTF,               R },
   { BUFFER_SIZE,                    oo(buffer_size),                   ud,          0,        0,                  R },
   { LOG_BUFFER,                     oo(log_buffer),                    LOGREC,      0,        0,                  R|QVAL },
   { RECORD_COUNT,                   oo(record_count),                  ud,          0,        0,                  W|QVAL },
   { TOTAL_RECORD_COUNT,             oo(total_record_count),            ud,          0,        0,                  R|QVAL },
   { LOGGING_TYPE,                   oo(logging_type),                  et,          0,        eiLoggingType,      R,        7 },
   { ALIGN_INTERVALS,                oo(align_intervals),               ebool,       0,        eiTF,               O,        7 },
   { INTERVAL_OFFSET,                oo(interval_offset),               ud,          0,        0,                  O,        7 },
   { TRIGGER,                        oo(trigger),                       ebool,       0,        eiTF,               O,        7 },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL,   7 },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O,        7 },
   { NOTIFICATION_THRESHOLD,         oo(notification_threshold),        ud,          0,        0,                  O|WithService },
   { RECORDS_SINCE_NOTIFICATION,     oo(records_since_notification),    ud,          0,        0,                  O|WithService|QVAL },
   { LAST_NOTIFY_RECORD,             oo(last_notify_record),            ud,          0,        0,                  O|WithService|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          0,        0,                  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        0,        eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        0,        eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          0,        eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    0,        0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(lifesafetypoint_obj_type,m)
propdescriptor LFSPProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(present_value),                 et,          0,        eiLifeSafetyState,  R|QVAL },
   { TRACKING_VALUE,                 oo(tracking_value),                et,          0,        eiLifeSafetyState,  O },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DEVICE_TYPE,                    oo(device_type),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             R },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { MODE,                           oo(mode),                          et,          0,        eiLifeSafetyMode,   W },
   { ACCEPTED_MODES,                 oo(accepted_modes),                etl,         0,        eiLifeSafetyMode,   O },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          0,        0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          0,        0,                  O|WithService },
   { LIFE_SAFETY_ALARM_VALUES,       oo(life_safety_alarm_values),      etl,         0,        eiLifeSafetyState,  O|WithService },
   { ALARM_VALUES,                   oo(alarm_values),                  etl,         0,        eiLifeSafetyState,  O|WithService },
   { FAULT_VALUES,                   oo(fault_values),                  etl,         0,        eiLifeSafetyState,  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        0,        eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        0,        eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          0,        eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    0,        0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { SILENCED,                       oo(silenced),                      et,          0,        eiSilencedState,    R },
   { OPERATION_EXPECTED,             oo(operation_expected),            et,          0,        eiLifeSafetyOperation, R },
   { MAINTENANCE_REQUIRED,           oo(maintenance_required),          et,          0,        eiMaintenance,      O },
   { SETTING,                        oo(setting),                       uw,          0,        0,                  O },
   { DIRECT_READING,                 oo(direct_reading),                flt,         0,        0,                  O },
   { UNITS,                          oo(units),                         et,          0,        eiEU,               O },
   { MEMBER_OF,                      oo(member_of),                     lodoref,     0,        0,                  O|IsArray },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(lifesafetyzone_obj_type,m)
propdescriptor LFSZProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(present_value),                 et,          0,        eiLifeSafetyState,  R|QVAL },
   { TRACKING_VALUE,                 oo(tracking_value),                et,          0,        eiLifeSafetyState,  O },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DEVICE_TYPE,                    oo(device_type),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             R },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { MODE,                           oo(mode),                          et,          0,        eiLifeSafetyMode,   W },
   { ACCEPTED_MODES,                 oo(accepted_modes),                etl,         0,        eiLifeSafetyMode,   O },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          0,        0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          0,        0,                  O|WithService },
   { LIFE_SAFETY_ALARM_VALUES,       oo(life_safety_alarm_values),      etl,         0,        eiLifeSafetyState,  O|WithService },
   { ALARM_VALUES,                   oo(alarm_values),                  etl,         0,        eiLifeSafetyState,  O|WithService },
   { FAULT_VALUES,                   oo(fault_values),                  etl,         0,        eiLifeSafetyState,  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        0,        eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        0,        eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          0,        eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    0,        0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { SILENCED,                       oo(silenced),                      et,          0,        eiSilencedState,    R },
   { OPERATION_EXPECTED,             oo(operation_expected),            et,          0,        eiLifeSafetyOperation, R },
   { MAINTENANCE_REQUIRED,           oo(maintenance_required),          ebool,       0,        0,                  O },
   { ZONE_MEMBERS,                   oo(zone_members),                  lodoref,     0,        0,                  R },
   { MEMBER_OF,                      oo(member_of),                     lodoref,     0,        0,                  O },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(accumulator_obj_type,m)
propdescriptor ACProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(present_value),                 uw,          0,        0,                  R|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { DEVICE_TYPE,                    oo(device_type),                   ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { SCALE,                          oo(scale),                         escale,      0,        0,                  R },
   { UNITS,                          oo(units),                         et,          0,        eiEU,               R },
   { PRESCALE,                       oo(prescale),                      eprescl,     0,        0,                  O },
   { MAX_PRES_VALUE,                 oo(max_pres_value),                uw,          0,        0,                  R },
   { VALUE_CHANGE_TIME,              oo(value_change_time),             dt,          0,        0,                  O|WithService|QVAL },
   { VALUE_BEFORE_CHANGE,            oo(value_before_change),           uw,          0,        0,                  O|WithService|QVAL },
   { VALUE_SET,                      oo(value_set),                     uw,          0,        0,                  O|WithService },
   { LOGGING_RECORD,                 oo(logging_record),                eaclr,       0,        0,                  O },
   { LOGGING_OBJECT,                 oo(logging_device.object_id),      ob_id,       0,        0,                  O },
   { PULSE_RATE,                     oo(pulse_rate),                    uw,          0,        0,                  O|WithService },
   { HIGH_LIMIT,                     oo(high_limit),                    uw,          Intr,     0,                  O|WithService },
   { LOW_LIMIT,                      oo(low_limit),                     uw,          Intr,     0,                  O|WithService },
   { LIMIT_MONITORING_INTERVAL,      oo(limit_monitoring_interval),     uw,          0,        0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          0,        0,                  O|WithService },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          0,        0,                  O|WithService },
   { LIMIT_ENABLE,                   oo(go.limit_enable),               bits,        0,        eiLimEn,            O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        0,        eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        0,        eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          0,        eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    0,        0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(pulseconverter_obj_type,m)
propdescriptor PCProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { PRESENT_VALUE,                  oo(present_value),                 flt,         0,        0,                  R|QVAL },
   { INPUT_REFERENCE,                oo(input_reference),               propref,     0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { UNITS,                          oo(units),                         et,          0,        eiEU,               R },
   { SCALE_FACTOR,                   oo(scale_factor),                  flt,         0,        0,                  R },
   { ADJUST_VALUE,                   oo(adjust_value),                  flt,         0,        0,                  W },
   { COUNT,                          oo(count),                         uw,          0,        0,                  R|QVAL },
   { UPDATE_TIME,                    oo(update_time),                   dt,          0,        0,                  R|QVAL },
   { COUNT_CHANGE_TIME,              oo(count_change_time),             dt,          0,        0,                  R|WithService|QVAL },
   { COUNT_BEFORE_CHANGE,            oo(count_before_change),           uw,          0,        0,                  R|QVAL },
   { COV_INCREMENT,                  oo(cov_increment),                 flt,         0,        0,                  O|WithService },
   { COV_PERIOD,                     oo(cov_period),                    uw,          0,        0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          0,        0,                  O|WithService },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          0,        0,                  O|WithService },
   { HIGH_LIMIT,                     oo(high_limit),                    flt,         0,        0,                  O|WithService },
   { LOW_LIMIT,                      oo(low_limit),                     flt,         0,        0,                  O|WithService },
   { DEADBAND,                       oo(deadband),                      flt,         0,        0,                  O|WithService },
   { LIMIT_ENABLE,                   oo(go.limit_enable),               bits,        0,        eiLimEn,            O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        0,        eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        0,        eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          0,        eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    0,        0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(lc_obj_type,m)
propdescriptor LCProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { PRESENT_VALUE,                  oo(present_value),                 et,          0,        eiShedState,        R|QVAL },
   { STATE_DESCRIPTION,              oo(state_description),             ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { REQUESTED_SHED_LEVEL,           oo(requested_shed_level),          shedlevel,   0,        0,                  W },
   { START_TIME,                     oo(start_time),                    dt,          0,        0,                  W },
   { SHED_DURATION,                  oo(shed_duration),                 ud,          0,        0,                  W },
   { DUTY_WINDOW,                    oo(duty_window),                   ud,          0,        0,                  W },
   { ENABLE,                         oo(log_enable),                    ebool,       0,        eiTF,               W },
   { FULL_DUTY_BASELINE,             oo(full_duty_baseline),            flt,         0,        0,                  O },
   { EXPECTED_SHED_LEVEL,            oo(expected_shed_level),           shedlevel,   0,        0,                  R },
   { ACTUAL_SHED_LEVEL,              oo(actual_shed_level),             shedlevel,   0,        0,                  R },
   { SHED_LEVELS,                    oo(shed_levels),                   stavals,     0,        0,                  W|IsArray },
   { SHED_LEVEL_DESCRIPTIONS,        oo(shed_level_descriptions),       statext,     0,        MAX_SHED_LEVELS,    R|IsArray },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(ad_obj_type,m)
propdescriptor ADProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PRESENT_VALUE,                  oo(present_value),                 et,          0,        eiDoorValue,        W|QVAL },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { PRIORITY_ARRAY,                 oo(priority_array),                pae,         0,        eiDoorValuen,       R|IsArray|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            et,          0,        eiDoorValue,        R },
   { DOOR_STATUS,                    oo(door_status),                   et,          0,        eiDoorStatus,       O|WithGroup|Woutofservice },
   { LOCK_STATUS,                    oo(lock_status),                   et,          0,        eiLockStatus,       O|Woutofservice },
   { SECURED_STATUS,                 oo(secured_status),                et,          0,        eiDoorSecuredStatus, O },
   { DOOR_MEMBERS,                   oo(door_members),                  lodoref,     0,        0,                  O|IsArray },
   { DOOR_PULSE_TIME,                oo(door_pulse_time),               uw,          0,        0,                  R },
   { DOOR_EXTENDED_PULSE_TIME,       oo(door_extended_pulse_time),      uw,          0,        0,                  R },
   { DOOR_UNLOCK_DELAY_TIME,         oo(door_unlock_delay_time),        uw,          0,        0,                  O },
   { DOOR_OPEN_TOO_LONG_TIME,        oo(door_open_too_long_time),       uw,          0,        0,                  R },
   { DOOR_ALARM_STATE,               oo(door_alarm_state),              et,          Intr,     eiDoorAlarmState,   O|Woutofservice|WithService },
   { MASKED_ALARM_VALUES,            oo(masked_alarm_values),           etl,         0,        eiDoorAlarmState,   O },
   { MAINTENANCE_REQUIRED,           oo(maintenance_required),          et,          0,        eiMaintenance,      O },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { ALARM_VALUES,                   oo(alarm_values),                  etl,         Intr,     eiDoorAlarmState,   O|WithService },
   { FAULT_VALUES,                   oo(fault_values),                  etl,         Intr,     eiDoorAlarmState,   O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(sv_obj_type,m)
propdescriptor SVProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { NODE_TYPE,                      oo(node_type),                     et,          0,        eiNodeType,         R },
   { NODE_SUBTYPE,                   oo(node_subtype),                  ch_string,   0,        0,                  O },
   // Even though subordinate-list is an array, re-use the "list of device object reference" type here; we'll catch the difference when parsing.
   { SUBORDINATE_LIST,               oo(subordinate_list),              lodoref,     0,        0,                  R|IsArray },
   { SUBORDINATE_ANNOTATIONS,        oo(subordinate_annotations),       statext,     0,        MAX_SV_ANNOTATIONS, O|IsArray },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(el_obj_type,m)
propdescriptor ELProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { ENABLE,                         oo(log_enable),                    ebool,       0,        eiTF,               W },
   { START_TIME,                     oo(start_time),                    dt,          0,        0,                  O|WithGroup },
   { STOP_TIME,                      oo(stop_time),                     dt,          0,        0,                  O|WithGroup },
   { STOP_WHEN_FULL,                 oo(stop_when_full),                ebool,       0,        eiTF,               R,        7 },
   { BUFFER_SIZE,                    oo(buffer_size),                   ud,          0,        0,                  R },
   { LOG_BUFFER,                     oo(log_buffer),                    LOGREC,      0,        0,                  R },
   { RECORD_COUNT,                   oo(record_count),                  ud,          0,        0,                  W|QVAL },
   { TOTAL_RECORD_COUNT,             oo(total_record_count),            ud,          0,        0,                  R|QVAL },
   { NOTIFICATION_THRESHOLD,         oo(notification_threshold),        ud,          0,        0,                  O|WithService },
   { RECORDS_SINCE_NOTIFICATION,     oo(records_since_notification),    ud,          0,        0,                  O|WithService|QVAL },
   { LAST_NOTIFY_RECORD,             oo(last_notify_record),            ud,          0,        0,                  O|WithService|QVAL },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          0,        0,                  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        0,        eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        0,        eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          0,        eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    0,        0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(global_group_obj_type,m)
propdescriptor GlobalGroupProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
// TODO:  GROUP_MEMBERS, array of BACnetDeviceObjectPropertyReference R
// TODO:  GROUP_MEMBER_NAMES, array of character strings O
// TODO:  PRESENT_VALUE, array of BACnetPropertyAccessResult R
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { UPDATE_INTERVAL,                oo(update_interval),               ud,          0,        0,                  O },
   { REQUESTED_UPDATE_INTERVAL,      oo(requested_update_interval),     ud,          0,        0,                  O },
   { COV_RESUBSCRIPTION_INTERVAL,    oo(cov_resubscription_interval),   ud,          0,        0,                  O },
// TODO:  CLIENT_COV_INCREMENT, BACnetClientCOV O
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   // TODO: the next two are O2: required present and W if sends unsubscribed COV
   { COVU_PERIOD,                    oo(covu_period),                   ud,          0,        0,                  O },
   { COVU_RECIPIENTS,                oo(covu_recipients),               tsrecip,     0,        0,                  O },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(tlm_obj_type,m)
propdescriptor TLMProps[] =
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { ENABLE,                         oo(log_enable),                    ebool,       0,        eiTF,               W },
   { START_TIME,                     oo(start_time),                    dt,          0,        0,                  O },
   { STOP_TIME,                      oo(stop_time),                     dt,          0,        0,                  O },
   { LOG_DEVICE_OBJECT_PROPERTY,     oo(log_device_object_property),    devobjpropref, 0,      0,                  R },
   { LOG_INTERVAL,                   oo(log_interval),                  ud,          0,        0,                  R },
   { LOGGING_TYPE,                   oo(logging_type),                  et,          0,        eiLoggingType,      R },
   { ALIGN_INTERVALS,                oo(align_intervals),               ebool,       0,        eiTF,               O },
   { INTERVAL_OFFSET,                oo(interval_offset),               uw,          0,        0,                  O },
   { TRIGGER,                        oo(trigger),                       ebool,       0,        eiTF,               O },
   { STOP_WHEN_FULL,                 oo(stop_when_full),                ebool,       0,        eiTF,               R },
   { BUFFER_SIZE,                    oo(buffer_size),                   ud,          0,        0,                  R },
   { LOG_BUFFER,                     oo(log_buffer),                    LOGREC,      0,        0,                  R|QVAL },
   { RECORD_COUNT,                   oo(record_count),                  ud,          0,        0,                  W|QVAL },
   { TOTAL_RECORD_COUNT,             oo(total_record_count),            ud,          0,        0,                  R|QVAL },
   { NOTIFICATION_THRESHOLD,         oo(notification_threshold),        ud,          0,        0,                  O|WithService },
   { RECORDS_SINCE_NOTIFICATION,     oo(records_since_notification),    ud,          0,        0,                  O|WithService|QVAL },
   { LAST_NOTIFY_RECORD,             oo(last_notify_record),            ud,          0,        0,                  O|WithService|QVAL },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          0,        0,                  O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        0,        eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        0,        eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          0,        eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    0,        0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(charstring_obj_type,m)
propdescriptor CharstringProps[]=
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { PRESENT_VALUE,                  oo(present_value),                 ch_string,   0,        0,                  W|IsCommandable|QVAL },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
// TODO: { PRIORITY_ARRAY,      oo(priority_array),       pa??,    3,          0,       O|IsArray|WithGroup|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            ch_string,   3,        0,                  O|WithGroup },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { ALARM_VALUES,                   oo(alarm_values),                  actext,      Intr,     MAX_FAULT_STRINGS,  O|IsArray|WithService },
   { FAULT_VALUES,                   oo(fault_values),                  actext,      Intr,     MAX_FAULT_STRINGS,  O|IsArray|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(integer_obj_type,m)
propdescriptor IntegerProps[]=
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { PRESENT_VALUE,                  oo(present_value),                 ptInt32,     0,        0,                  W|IsCommandable|QVAL },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { UNITS,                          oo(units),                         et,          0,        eiEU,               R },
   { PRIORITY_ARRAY,                 oo(priority_array),                ptPai,       1,        0,                  O|WithGroup|IsArray|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            ptInt32,     1,        0,                  O|WithGroup },
   { COV_INCREMENT,                  oo(cov_increment),                 ptInt32,     COV,      0,                  O|WithService },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { HIGH_LIMIT,                     oo(high_limit),                    ptInt32,     Intr,     0,                  O|WithService },
   { LOW_LIMIT,                      oo(low_limit),                     ptInt32,     Intr,     0,                  O|WithService },
   { DEADBAND,                       oo(deadband),                      ud,          Intr,     0,                  O|WithService },
   { LIMIT_ENABLE,                   oo(go.limit_enable),               bits,        Intr,     eiLimEn,            O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { MIN_PRES_VALUE,                 oo(min_pres_value),                ptInt32,     0,        0,                  O,        14 },
   { MAX_PRES_VALUE,                 oo(max_pres_value),                ptInt32,     0,        0,                  O,        14 },
   { RESOLUTION,                     oo(resolution),                    ptInt32,     0,        0,                  O,        14 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

// parse type "ud" is unsigned double word, eg what would be Uint32
#undef oo
#define oo(m)  xxoo(positive_integer_obj_type,m)
propdescriptor PositiveIntegerProps[]=
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { PRESENT_VALUE,                  oo(present_value),                 ud,          0,        0,                  W|IsCommandable|QVAL },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
   { UNITS,                          oo(units),                         et,          0,        eiEU,               R },
   { PRIORITY_ARRAY,                 oo(priority_array),                ptPai,       1,        0,                  O|WithGroup|IsArray|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            ud,          1,        0,                  O|WithGroup },
   { COV_INCREMENT,                  oo(cov_increment),                 ud,          COV,      0,                  O|WithService },
   { TIME_DELAY,                     oo(go.time_delay),                 uw,          Intr,     0,                  O|WithService },
   { NOTIFICATION_CLASS,             oo(go.notification_class),         uw,          Intr,     0,                  O|WithService },
   { HIGH_LIMIT,                     oo(high_limit),                    ud,          Intr,     0,                  O|WithService },
   { LOW_LIMIT,                      oo(low_limit),                     ud,          Intr,     0,                  O|WithService },
   { DEADBAND,                       oo(deadband),                      ud,          Intr,     0,                  O|WithService },
   { LIMIT_ENABLE,                   oo(go.limit_enable),               bits,        Intr,     eiLimEn,            O|WithService },
   { EVENT_ENABLE,                   oo(go.event_enable),               bits,        Intr,     eiEvTr,             O|WithService },
   { ACKED_TRANSITIONS,              oo(go.acked_transitions),          bits,        Intr,     eiEvTr,             O|WithService|QVAL },
   { NOTIFY_TYPE,                    oo(go.notify_type),                et,          Intr,     eiNT,               O|WithService },
   { EVENT_TIME_STAMPS,              oo(go.event_time_stamps),          TSTMParr,    Intr,     0,                  O|IsArray|WithService|QVAL },
   { EVENT_MESSAGE_TEXTS,            oo(go.event_message_texts),        statext,     0,        3,                  O|IsArray, 11 },
   { EVENT_MESSAGE_TEXTS_CONFIG,     oo(go.event_message_texts_config), statext,     0,        3,                  O|IsArray, 13 },
   { EVENT_DETECTION_ENABLE,         oo(go.event_detection_enable),     ebool,       Intr,     0,                  O|WithService, 13 },
   { EVENT_ALGORITHM_INHIBIT_REF,    oo(go.event_algorithm_inhibit_ref), propref,    0,        0,                  O,        13 },
   { EVENT_ALGORITHM_INHIBIT,        oo(go.event_algorithm_inhibit),    ebool,       0,        0,                  O,        13 },
   { TIME_DELAY_NORMAL,              oo(go.time_delay_normal),          ud,          0,        0,                  O,        13 },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { MIN_PRES_VALUE,                 oo(min_pres_value),                ud,          0,        0,                  O,        14 },
   { MAX_PRES_VALUE,                 oo(max_pres_value),                ud,          0,        0,                  O,        14 },
   { RESOLUTION,                     oo(resolution),                    ud,          0,        0,                  O,        14 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

#undef oo
#define oo(m)  xxoo(datetimevalue_obj_type,m)
propdescriptor DateTimeValueProps[]=
{
// property identifier,            struc offset,                      parse,       group,    table,              qualifiers, firstRevision
   { OBJECT_IDENTIFIER,              oo(go.object_id),                  ob_id,       0,        0,                  R },
   { OBJECT_NAME,                    oo(go.object_name),                ch_string,   0,        0,                  R },
   { OBJECT_TYPE,                    oo(go.object_type),                et,          0,        eiObjectTypes,      R },
   { DESCRIPTION,                    oo(go.description),                ch_string,   0,        0,                  O },
   { PRESENT_VALUE,                  oo(present_value),                 dt,          0,        0,                  W|IsCommandable|QVAL },
   { STATUS_FLAGS,                   oo(go.status_flags),               bits,        0,        eiStF,              R|QVAL },
   { EVENT_STATE,                    oo(go.event_state),                et,          0,        eiEvState,          R|QVAL },
   { RELIABILITY,                    oo(go.reliability),                et,          0,        eiReli,             O },
   { OUT_OF_SERVICE,                 oo(go.out_of_service),             ebool,       0,        eiTF,               R },
// TODO: { PRIORITY_ARRAY,   oo(priority_array),   ptPa??,  1,       0, O|WithGroup|IsArray|QVAL },
   { RELINQUISH_DEFAULT,             oo(relinquish_default),            dt,          1,        0,                  O|WithGroup },
   { IS_UTC,                         oo(is_utc),                        ebool,       0,        eiTF,               R },
   { RELIABILITY_EVALUATION_INHIBIT, oo(go.reliability_evaluation_inhibit), ebool,   0,        0,                  O,        13 },
   { PROPERTY_LIST,                  oo(go.property_list),              PT_PROPLIST, 0,        0,                  R,        14 },
   { PROFILE_NAME,                   oo(go.profile_name),               ch_string,   LAST,     0,                  O }
};

stdobjtype  StdObjects[MAX_DEFINED_OBJ]={
   sizeof(ai_obj_type),               AIprops,
   sizeof(ao_obj_type),               AOprops,
   sizeof(av_obj_type),               AVprops,
   sizeof(bi_obj_type),               BIprops,
   sizeof(bo_obj_type),               BOprops,
   sizeof(bv_obj_type),               BVprops,
   sizeof(calendar_obj_type),         CLprops,
   sizeof(command_obj_type),          CMprops,
   sizeof(device_obj_type),           DVprops,
   sizeof(ee_obj_type),               EEprops,
   sizeof(file_obj_type),             FLprops,
   sizeof(group_obj_type),            GRprops,
   sizeof(loop_obj_type),             LPprops,
   sizeof(mi_obj_type),               MIprops,
   sizeof(mo_obj_type),               MOprops,
   sizeof(nc_obj_type),               NCprops,
   sizeof(program_obj_type),          PRprops,
   sizeof(schedule_obj_type),         SCprops,
   sizeof(avg_obj_type),              AVGprops,
   sizeof(msv_obj_type),              MVprops,
   sizeof(trend_obj_type),            TRprops,
   sizeof(lifesafetypoint_obj_type),  LFSPProps,
   sizeof(lifesafetyzone_obj_type),   LFSZProps,
   sizeof(accumulator_obj_type),      ACProps,
   sizeof(pulseconverter_obj_type),   PCProps,
   sizeof(el_obj_type),               ELProps,
   sizeof(global_group_obj_type),     GlobalGroupProps,
   sizeof(tlm_obj_type),              TLMProps,
   sizeof(lc_obj_type),               LCProps,
   sizeof(sv_obj_type),               SVProps,
   sizeof(ad_obj_type),               ADProps,
   sizeof(placeholder_obj_type),      PlaceholderProps,   // unused-31 was lighting-output in an early review
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Access-Credential",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Access-Point",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Access-Rights",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Access-User",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Access-Zone",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Credential-Data-Input",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Network-Security",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "BitString-Value",
   sizeof(charstring_obj_type),       CharstringProps,    // "CharacterString-Value",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Date-Pattern-Value",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Date-Value",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "DateTime-Pattern-Value",
   sizeof(datetimevalue_obj_type),    DateTimeValueProps, // "DateTime-Value",
   sizeof(integer_obj_type),          IntegerProps,       // "Integer-Value",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Large-Analog-Value",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "OctetString-Value",
   sizeof(positive_integer_obj_type), PositiveIntegerProps,// "Positive-Integer-Value",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Time-Pattern-Value",
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Time-Value"
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Notification-Forwarder"
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Alert-Enrollment"
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Channel"
   sizeof(placeholder_obj_type),      PlaceholderProps,   // TODO: "Lighting-Output"
};

#else

extern stdobjtype StdObjects[];
extern propdescriptor ProprietaryObjProps[];

// TODO: debug only (used by EPICS view to show unfinished business)
extern propdescriptor PlaceholderProps[];

#endif

propdescriptor* GetPropDescriptorTable( word objtype );

/*
// TODO: This stuff isn't currently referenced anywhere.  Could it be useful?

typedef struct {
    dword   dptpropid;           //property id
    octet   dptparsetype;        //parse type
    word    dptpet;              //parse type et if applicable
    word    dptflags;            //only IsArray used
} defparsetypes;

typedef struct {
    word          npt;           //number of string pointers which follow
    defparsetypes dpt[147];      //the default parse types
} dpttable;

#if _DoStaticPropDescriptors

//Special table for default parse types
static dpttable dParseTypes={
                147,                   //147 parse types
                {
                {ACKED_TRANSITIONS,bits,eiEvTr,0},
                {ACK_REQUIRED,bits,eiEvTr,0},
                {ACTION,act,0,IsArray},
                {ACTION,et,eiLoopAct,0},
                {ACTION_TEXT,actext,0,IsArray},
                {ACTIVE_TEXT,ch_string,0},
                {ACTIVE_VT_SESSIONS,vtse,0,0},
                {ALARM_VALUE,et,eiBPV,0},
                {ALARM_VALUES,stavals,0,0},
                {ALL,none,0,0},
                {ALL_WRITES_SUCCESSFUL,ebool,eiTF,0},
                {APDU_SEGMENT_TIMEOUT,uw,0,0},
                {APDU_TIMEOUT,uw,0,0},
                {APPLICATION_SOFTWARE_VERSION,ch_string,0,0},
                {ARCHIVE,ebool,eiTF,0},
                {ATTEMPTED_SAMPLES,uw,0,0},
                {AVERAGE_VALUE,flt,0,0},
                {BIAS,flt,0,0},
                {CHANGE_OF_STATE_COUNT,uw,0,0},
                {CHANGE_OF_STATE_TIME,dt,0,0},
                {NOTIFICATION_CLASS,uw,0,0},
                {WAS_blank1,0,0,0},
                {CONTROLLED_VARIABLE_REFERENCE,propref,0,0},
                {CONTROLLED_VARIABLE_UNITS,et,eiEU,0},
                {CONTROLLED_VARIABLE_VALUE,flt,0,0},
                {COV_INCREMENT,flt,0,0},
                {DATE_LIST,calist,0,0},
                {DAYLIGHT_SAVINGS_STATUS,ebool,eiTF,0},
                {DEADBAND,flt,0,0},
                {DERIVATIVE_CONSTANT,flt,0,0},
                {DERIVATIVE_CONSTANT_UNITS,et,eiEU,0},
                {DESCRIPTION,ch_string,0,0},
                {DESCRIPTION_OF_HALT,ch_string,0,0},
                {DEVICE_ADDRESS_BINDING,dabind,0,0},
                {DEVICE_TYPE,ch_string,0,0},
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
                {FILE_TYPE,ch_string,0,0},
                {FIRMWARE_REVISION,ch_string,0,0},
                {HIGH_LIMIT,flt,0,0},
                {INACTIVE_TEXT,ch_string,0,0},
                {IN_PROCESS,ebool,eiTF,0},
                {INSTANCE_OF,ch_string,0,0},
                {INTEGRAL_CONSTANT,flt,0,0},
                {INTEGRAL_CONSTANT_UNITS,et,eiEU,0},
                {ISSUE_CONFIRMED_NOTIFICATIONS,ebool,eiTF,0},
                {LIMIT_ENABLE,bits,eiLimEn,0},
                {LIST_OF_GROUP_MEMBERS,raslist,0,0},
                {LIST_OF_OBJECT_PROPERTY_REFERENCES,lopref,0,0},
                {LIST_OF_SESSION_KEYS,skeys,0,0},
                {LOCAL_DATE,none,0,0},
                {LOCAL_TIME,none,0,0},
                {LOCATION,ch_string,0,0},
                {LOG_BUFFER,LOGREC,0,0},
                {LOW_LIMIT,flt,0,0},
                {MANIPULATED_VARIABLE_REFERENCE,propref,0,0},
                {MAXIMUM_OUTPUT,flt,0,0},
                {MAXIMUM_VALUE,flt,0,0},
                {MAX_APDU_LENGTH_ACCEPTED,uw,0,0},
                {MAXIMUM_VALUE_TIMESTAMP,dt,0,0},
                {MAX_INFO_FRAMES,uw,0,0},
                {MAX_MASTER,u127,0,0},
                {MAX_PRES_VALUE,flt,0,0},
                {MINIMUM_OFF_TIME,ud,0,0},
                {MINIMUM_ON_TIME,ud,0,0},
                {MINIMUM_OUTPUT,flt,0,0},
                {MINIMUM_VALUE,flt,0,0},
                {MINIMUM_VALUE_TIMESTAMP,dt,0,0},
                {MIN_PRES_VALUE,flt,0,0},
                {MODEL_NAME,ch_string,0,0},
                {MODIFICATION_DATE,dt,0,0},
                {NOTIFY_TYPE,et,eiNT,0},
                {NUMBER_OF_APDU_RETRIES,uw,0,0},
                {NUMBER_OF_STATES,uw,0,0},
                {OBJECT_IDENTIFIER,ob_id,0,0},
                {OBJECT_LIST,none,0,IsArray},
                {OBJECT_NAME,ch_string,0,0},
                {OBJECT_PROPERTY_REFERENCE,devobjpropref,0},
                {OBJECT_TYPE,et,eiObjectTypes,0},
                {PROP_OPTIONAL,none,0,0},
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
                {PRIORITY_FOR_WRITING,u16,0,0},
                {PROCESS_IDENTIFIER,uw,0,0},
                {PROGRAM_CHANGE,et,eiPrChg,0},
                {PROGRAM_LOCATION,ch_string,0,0},
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
                {STATUS_FLAGS,bits,eiStF,0},
                {SYSTEM_STATUS,et,eiDS,0},
                {TIME_DELAY,uw,0,0},
                {TIME_OF_ACTIVE_TIME_RESET,dt,0,0},
                {TIME_OF_STATE_COUNT_RESET,dt,0,0},
                {TIME_SYNCHRONIZATION_RECIPIENTS,tsrecip,0,0},
                {UNITS,et,eiEU,0},
                {UPDATE_INTERVAL,uw,0,0},
                {UTC_OFFSET,sw,0,0},
                {VALID_SAMPLES,uw,0,0,},
                {VARIANCE_VALUE,flt,0,0},
                {VENDOR_IDENTIFIER,uw,0,0},
                {VENDOR_NAME,ch_string,0,0},
                {VT_CLASSES_SUPPORTED,vtcl,0,0},
                {WEEKLY_SCHEDULE,wsched,0,IsArray},
                {WINDOW_INTERVAL,uw,0,0},
                {WINDOW_SAMPLES,uw,0,0},
                {STRUCTURED_OBJECT_LIST,none,0,IsArray} }
};
#endif
*/

#endif //__STDOBJPR_H_INCLUDED
