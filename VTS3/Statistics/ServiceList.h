#define TOTAL_MSG_NUMBER 65


int Service_Type_Index[]=
{8,10,10,2,10,12,3,3,4,3
};

/* BACnet packet groups */
CString group[]={
	"BVLL",
	"Network",
	"Alarm and event",
	"File Access",
	"Object Access",
	"Remote Device Management",
	"Virtual Terminal",
	"Security",
	"ACK",
	"Errors"
};

/* BVLL function */
CString BACnet_msgs[]={
	"BVLC-Result",
	"Write-Broadcast-Distribution-Table",
	"Read-Broadcast-Distribution-Table",
	"Read-Broadcast-Distribution-Table-Ack",
	"Register-Foreign-Device",
	"Read-Foreign-Device-Table",
	"Read-Foreign-Device-Table-Ack",
	"Delete-Foreign-Device-Table-Entry",

/* Nertwork Layer Services */
   "Who-Is-Router-To-Network",		
   "I-Am-Router-To-Network",		
   "I-Could-Be-Router-To-Network",	
   "Reject-Message-To-Network",	
   "Router-Busy-To-Network",		
   "Router-Available-To-Network",	
   "Initialize-Routing-Table",		
   "Initialize-Routing-Table-Ack",
   "Establish-Connection-To-Network",
   "Disconnect-Connection-To-Network",

/* Alarm and Event Services */
   "AcknowledgeAlarm",              /* 0 */
   "ConfirmedCOVNotification",      /* 1 */
   "ConfirmedEventNotification",    /* 2 */
   "GetAlarmSummary",               /* 3 */
   "GetEnrollmentSummary",          /* 4 */
   "SubscribeCOV",                  /* 5 */
   "UnconfirmedCOVNotification",    /* 6 */
   "UnconfirmedEventNotification",  /* 7 */
   "SubscribeCOVProperty",          /* 8 */ 
   "GetEventInformation",            /* 9 */
   
/* File Access Services */
   "AtomicReadFile",                /* 10 */
   "AtomicWriteFile",               /* 11 */

/* Object Access Services */
   "AddListElement",                /* 12 */
   "RemoveListElement",             /* 13 */
   "CreateObject",                  /* 14 */
   "DeleteObject",                  /* 15 */
   "ReadProperty",                  /* 16 */
   "ReadPropertyConditional",       /* 17 */
   "ReadPropertyMultiple",          /* 18 */
   "WriteProperty",                 /* 19 */
   "WritePropertyMultiple",         /* 20 */
   "ReadRange",                     /* 21 */

/* Remote Device Management Services */
   "DeviceCommunicationControl",    /* 22 */
   "ConfirmedPrivateTransfer",      /* 23 */
   "UnconfirmedPrivateTransfer",    /* 24 */ 
   "ReinitializeDevice",            /* 25 */
   "ConfirmedTextMessage",          /* 26 */
   "UnconfirmedTextMessage",        /* 27 */
   "TimeSynchronization",           /* 28 */
   "Who-Has",                       /* 29 */
   "I-Have",                        /* 30 */
   "Who-Is",                        /* 31 */
   "I-Am",                          /* 32 */
   "UtcTimeSynchronization"  ,      /* 33 */

/* Virtual Terminal Services */
   "VT-Open",                       /* 34 */
   "VT-Close",                      /* 35 */
   "VT-Data",                       /* 36 */

/*  Security Services */
   "Authenticate",                  /* 37 */
   "RequestKey",                    /* 38 */
   "LifeSafetyOperation",           /* 39 */  

/* Ack packets */
	"Complex ACK",
	"Simple ACK",
	"Segment ACK",
	"ReadProperty-ACK",

/* Error packets */
	"Error",
	"Reject",
	"Abort"
};
