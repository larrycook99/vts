/*--------------------------------------------------------------------------------
   Original code (c)1995-97, PolarSoft(r) Inc. and National Institute for Standards and Technology
   Many later changes made by the VTS community and released into the public domain.

module:     VTSAPI32.C

-- NOTE: 135.1 doesn't say how to encode missing OPTIONAL items in a SEQUENCE:
-- should there be an empty pair of commas, or should the item and any comma be
-- omitted?  I believe that the comma MUST be maintained - else there is no
-- way to tell whether the item is present or not, unless it may be discerned
-- from the following item (if any), which isn't always possible.
-- If this interpretation is wrong, this decoder and the samepl EPICS
-- may need to be changed.

135.1-2013 clause 4.4 says
- constructed items are enclosed by curly brackets { } with elements separated by commas
- CHOICE of application-tagged items are represented as such
- CHOICE of context-tagged items are represented by [n] followed by the item
- SEQUENCE-OF (list) are enclosed in parenthesis, with items separated by commas
- Array values are enclosed in curly brackets, with items separated by commas
135.1-2013 Annex A SOMETIMES follows those rules...
VTS has traditionally varied widely from them.
The classes EpicsSequence and EpicsListOf accept either

TODO: various places write into the line buffer, usually temporary nulls to
ease parsing.  This is REALLY fragile: if you do this, and then call some
parser function like skipwhitespace(), MustBe(), etc., and they
run into the null, they will READ THE NEXT LINE FROM THE FILE, bollixing up
parsing thereafter.
This is widespread enough to be a pain to fix all at once, but do you part
when you encounter it.

-----------------------------------------------------------------------------*/

#include "stdafx.h"

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <ctype.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <assert.h>

#include "StringTables.h"

// TODO: Function currently in DUDTOOL.CPP, but soon (we hope) to be moved
#include "Propid.h"
#include "VTS.h"
const BACnetPropertyIdentifier* GetObjectTable( BACnetObjectType objType, int &nProps);

// Define this to enable debug messages to file in the debug build
#ifdef DEBUG
  #define MDEBUG
#endif


namespace PICS {

#define _DoStaticPropDescriptors 0

#include "db.h"
#include "stdobj.h"
#include "stdobjpr.h"
#include "vtsapi.h"
#include "VTS.h"
#include "EPICSConsCheck.h"
#include "dudapi.h"

///////////////////////////////////////////////////////////////////////
// function prototypes
static bool ReadStandardServices(PICSdb *);
static bool ReadStandardObjects(PICSdb *);
static bool ReadDataLinkOptions(PICSdb *);
static bool ReadSpecialFunctionality(PICSdb *);
static bool ReadObjects(PICSdb *);
static bool ReadCharsets(PICSdb *);
static bool ReadFailTimes(PICSdb *);
static bool ReadBIBBSupported(PICSdb *);
static bool ReadDefaultPropertyValueRestrictions(PICSdb *);

//static bool ParseLogRec(BACnetLogRecord *);
static bool ParseTimeStamp(BACnetTimeStamp *);
static bool ParseTimeStampArray(BACnetTimeStamp **parray, int arraycount);
static bool ParseObjectList(BACnetObjectIdentifier **,word *);
static bool ParseRASlist(BACnetReadAccessSpecification **);
static bool ParseCalist(BACnetCalendarEntry **);
static bool ParseTVList(BACnetTimeValue **);
static bool ParseWeekdaySchedule(BACnetTimeValue *wp[]);
static bool ParseExceptionSchedule(BACnetExceptionSchedule *);
static bool ParseProperty(const char *,generic_object *,word);
static bool ParseDateTime(BACnetDateTime *);
static bool ParseDate(BACnetDate *);
static bool ParseTime(BACnetTime *);
static bool ParseDateRange(BACnetDateRange *);
static bool ParseBitstring(octet *,UINT,UINT*);
static bool ParseOctetstring(octet *,word,word *);
static bool ParseVTClassList(BACnetVTClassList **);
static bool ParseVTSessionList(BACnetVTSession **);
static bool ParseAddressList(BACnetAddressBinding **);
static bool ParseBACnetAddress(BACnetAddress *);
static bool ParseDestinationList(BACnetDestination **);
static bool ParseRecipientList(BACnetRecipient **);
static bool ParseEventParameter(BACnetEventParameter *);
static bool ParseSessionKeyList(BACnetSessionKey **);
static bool ParseRefList(BACnetDeviceObjectPropertyReference **);
static bool ParseDevObjList(BACnetDeviceObjectReference **, bool isArray );
static bool ParsePrescale(BACnetPrescale* pt);
static bool ParseAccumulatorRecord(BACnetAccumulatorRecord* pt);
static BACnetRecipient *ParseRecipient(BACnetRecipient *);
static BACnetObjectPropertyReference *ParseReference(BACnetObjectPropertyReference*);
static bool ParseCOVSubList(BACnetCOVSubscription **);
static BACnetCOVSubscription *ParseCOVSubscription();
static bool ParseEnumList(BACnetEnumList **, const NetworkSniffer::BACnetStringTable *);
static bool ParsePropertyList(BACnetEnumList **);
static bool ParseUnsignedList( UnsignedList **elp );
static bool ParseBooleanList( BooleanList **elp );
static BACnetDeviceObjectPropertyReference *ParseDevObjPropReference(BACnetDeviceObjectPropertyReference *);
static BACnetDeviceObjectReference *ParseDevObjReference(BACnetDeviceObjectReference *);


// 135.1-2013 EPICS consistency checks added
static void ExpandBitstring(octet *pExpandedResult, octet *pBitstring, int nBits);
static void CheckPICSConsistency2013(PICSdb *); // runs all checks
static void CheckPICS_BIBB_Cross_Dependency(PICSdb *pd, int iSupportedBIBB, int iDependentBIBB);
static void CheckPICSConsProperties(PICSdb *pd, generic_object *obj);
static void CheckPICSCons2013A(PICSdb *pd);
static void CheckPICSCons2013D(PICSdb *pd);
static void CheckPICSCons2013E(PICSdb *pd);
static void CheckPICSCons2013F(PICSdb *pd);
static void CheckPICSCons2013G(PICSdb *pd);
static void CheckPICSCons2013H(PICSdb *pd);
static void CheckPICSCons2013I(PICSdb *pd);
static void CheckPICSCons2013K(PICSdb *pd);
static void CheckPICSCons2013L(PICSdb *pd);

/////////////////////////////////////////////////////////////

static BACnetActionCommand *ReadActionCommands(void);
static bool setstring(char *,word,char *);
static char *ReadNext(void);
static void skipwhitespace(void);
static bool ReadB(octet*,UINT,UINT);
static octet whatTimestampChoice(char str[]);
static void readline();
static bool PeekNextIs(char);  // non-consuming test
static bool NextIs(char);      // optional consuming test
static bool NextIs(const char*); // optional consuming string test
static bool NextIsWildcard();  // optional consuming test
static bool MustBe(char);      // required consuming test
static bool NextInBufferIs(char); // optional consuming test, do not read more
static bool tperror(const char *, bool);
static void Message(const char *, bool);
static void rtrim(char *);
static void preprocstr(char *str);

static bool IsNextNumber();
static bool ReadBool(bool *pValue, char mustBe = 0);
static bool ReadFloat(float *pVal, char mustBe = 0);
static bool ReadSigned(int *pVal, char mustBe = 0);
static bool ReadAlphaString(CString *pVal, char mustBe = 0);
static bool ReadEnumString(CString *pVal, char mustBe = 0);
static void SkipAlphaString();
static bool IsAlphaString( const char *pTestValue );
static bool ReadDW(dword *pVal, char mustBe = 0);
static bool ReadW(word *pVal, char mustBe = 0);
static bool ReadEnum(word *pVal, const NetworkSniffer::BACnetStringTable *etp);
static bool ReadObjectID(dword *pVal, char mustBe = 0);
static bool ReadChoiceTag(octet *pChoice, octet maxChoice, bool isRequired = false);

propdescriptor* GetPropDescriptorTable( word objtype );

///////////////////////////////////////////////////////////////////////
// Module Constants
#define  TAB                  0x09
#define  SPACE                0x20
#define  CR                   0x0D
#define  LF                   0x0A
#define  DOUBLEQUOTE          0x22
#define  SINGLEQUOTE          0x27
#define  ACCENTGRAVE          0x60
#define  BAD_OBJID            0xFFFFFFFF  //we presume that no one will use this as a valid object identifier

// String of digits for testing
static const char s_digits[] = "0123456789";

//=============================================================================
// Class to assist with bitstring or SEQUENCE parsing
class EpicsSequence
{
public:
   EpicsSequence();
   ~EpicsSequence();

   // Get opening parenthesis or bracket, else log an error.
   // Return true to abort
   bool Begin();

   // Get OPTIONAL opening parenthesis or bracket.
   // If neither is found, End() will do nothing.
   void BeginOptional();

   // Get closing parenthesis or bracket, else log an error.
   // Return true to abort
   bool End();

   // Return false if the next non-blank is comma (indicating a missing optional
   // value), or the closing terminator (indicating one or more missing values)
   // Thus, will return true if an optional item is present.
   // Comma is eaten, closing terminator is not.
   bool HasOption();

   // Return the terminating character for the sequence
   char Terminator() const { return m_terminator; }

protected:
   char m_terminator;
};

//=============================================================================
// Class to assist with loop processing
class EpicsListOf
{
public:
   EpicsListOf();
   ~EpicsListOf();

   // Return true if the loop should continue, else false
   bool HasMore();

   // Return the number of elements found thus far
   UINT Count() const { return m_count; }

   // Return the terminating character for the sequence
   char Terminator() const { return m_terminator; }

protected:
   char m_terminator;
   UINT m_count;
};

//=============================================================================
// Initialize string pointers to an empty string,
// in case the values are referenced without being set.
PICSdb::PICSdb()
: VendorName(&dummy)
, ProductName(&dummy)
, ProductModelNumber(&dummy)
, ProductDescription(&dummy)
, dummy(0)
, pDeviceObject(NULL)
{
}

PICSdb::~PICSdb()
{
   // Free any allocated string memory
   if (VendorName != &dummy)
   {
      delete[] VendorName;
   }

   if (ProductName != &dummy)
   {
      delete[] ProductName;
   }

   if (ProductModelNumber != &dummy)
   {
      delete[] ProductModelNumber;
   }

   if (ProductDescription != &dummy)
   {
      delete[] ProductDescription;
   }
}

///////////////////////////////////////////////////////////////////////
// global variables
UINT EPICSLengthProtocolServicesSupportedBitstring;    // used by test 135.1-2013 5(k)
UINT EPICSLengthProtocolObjectTypesSupportedBitstring; // used by test 135.1-2013 5(l)
///////////////////////////////////////////////////////////////////////
// local variables
static bool    s_cancel = false;          //global cancel flag.  Set true to abort EPICS parsing
static bool    s_error  = false;          //global error flag.  Set true to abort processing of a single property or other item
static char    *lb;                       //line buffer (current line of input file)
static UINT    lpSize;                    //size of allocated line buffer
static char    *lp;                       //pointer into lb
static word    lc;                        //current 1-based line in file
static int     lerrc;                     //count of errors
static FILE    *ifile;                    //current input file
static int     lPICSErr;                  //Count of EPICS consistency errors, -1 during EPICS parse

// TODO: these are currently used by the Schedule object to allow the datatype of
// the first referenced property be used to REPARSE present-value and schedule-default
// according to that datatype.  However,
// 1) it only works for ONE of present-value and schedule-default, since there is only
//    a single set to NoneTypePropName/NoneTypeValue
// 2) Doesn't affect weekly-schedule and exception-schedule, both of which have
//    elements of exactly the same datatype.
// 3) If there is a parse error in the saved value, the error message credits the line number
//    of the referenced property
// Personally, I think a more robust "any primitive type" parser would serve better, perhaps
// using the datatype of the first reference during a pass two consistency check.
static octet   newParseType;
static word    newPropET;
static boolean bNeedReset = false, bHasReset = false, bHasNoneType = false;
static char    NoneTypeValue[512], NoneTypePropName[MAX_TEXT_STRING];   // for store temp address of lp

static unsigned int cConsistencyErrors;

// Array of expected bitstring lengths for ProtocolServicesSupported
// based on the Protocol_Revision property as the index.
static octet aCorrectLengthProtocolServicesSupportedBitstring[] =
{
   35,  // Protocol_Revision = 0
   37,  // Protocol_Revision = 1  135-1995b (ReadRange, UTCTimeSynch)
   40,  // Protocol_Revision = 2  135-1995c (LifeSafetey, SubscribeCOV, GetEventInfo)
   40,  // Protocol_Revision = 3
   40,  // Protocol_Revision = 4  135-2004
   40,  // Protocol_Revision = 5
   40,  // Protocol_Revision = 6
   40,  // Protocol_Revision = 7  135-2008
   40,  // Protocol_Revision = 8
   40,  // Protocol_Revision = 9
   40,  // Protocol_Revision = 10
   40,  // Protocol_Revision = 11
   40,  // Protocol_Revision = 12 135-2010
   40,  // Protocol_Revision = 13
   41,  // Protocol_Revision = 14 135-2010aa (WriteGroup) 135-1012 (rev 14)
};

// Array of expected bitstring lengths for ProtocolObjectTypesSupported
// based on the Protocol_Revision property as the index.
static octet aCorrectLengthProtocolObjectTypesSupportedBitstring[] =
{
   18,  // Protocol_Revision = 0
   21,  // Protocol_Revision = 1
   23,  // Protocol_Revision = 2
   23,  // Protocol_Revision = 3
   25,  // Protocol_Revision = 4  135-2004 added objects 23 and 24
   30,  // Protocol_Revision = 5
   31,  // Protocol_Revision = 6
   31,  // protocol_revision = 7  135-2008
   31,  // protocol_revision = 8
   38,  // protocol_revision = 9  135-2008j added objects 32-37
   51,  // protocol_revision = 10 135-2008w added 39-50
   51,  // protocol_revision = 11
   51,  // protocol_revision = 12 135-2010
   53,  // protocol_revision = 13 135-2010af added 51 and 52
   55,  // protocol_revision = 14 135-2012 added 53 and 54
};

//---------------------------------------------------------------------
//  Large Static Tables
static const char picshdr[]   = "PICS 0\x0D\x0A";
static const char BeginPics[] = "BACnet Protocol Implementation Conformance Statement";
static const char EndPics[]   = "End of BACnet Protocol Implementation Conformance Statement";

typedef struct {
    char                  InitExec;                      // flag.  If zero, marks end of list.
    enum                  BACnetApplServ ApplServ;       // number of application service
} bibb_service;

#define MAX_SERVICES_PER_BIBB 8  // maximum number of services required per BIBB

// Structure to define each BIBB
typedef struct {
   char  *name;    // BIBB name
   bibb_service  aBIBB_Service[MAX_SERVICES_PER_BIBB];  // array of services required for each BIBB
} bibbdef;


// These are used by the parsing routines as well as the BIBB consistency checks.
// The order MUST match the order of the enum BIBB in Db.h, as those values are
// used as indexes into this array
static bibbdef BIBBs[]={
         "DS-RP-A",
             { { ssInitiate, asReadProperty,  }
             },  // (An InitExec value of zero marks the end of each list)
         "DS-RP-B",
             { { ssExecute, asReadProperty    }
             },
         "DS-RPM-A",
             { { ssInitiate, asReadPropertyMultiple }
             },
         "DS-RPM-B",
             { { ssExecute, asReadPropertyMultiple  }
             },
         "DS-RPC-A",
             { { ssInitiate, asReadPropertyConditional  }
             },
         "DS-RPC-B",
             { { ssExecute, asReadPropertyConditional  }
             },
         "DS-WP-A",
             { { ssInitiate, asWriteProperty  }
             },
         "DS-WP-B",
             { { ssExecute, asWriteProperty  }
             },
         "DS-WPM-A",
             { { ssInitiate, asWritePropertyMultiple }
             },
         "DS-WPM-B",
             { { ssExecute, asWritePropertyMultiple  }
             },
         "DS-COV-A",
             { { ssInitiate, asSubscribeCOV                },
               { ssExecute,  asConfirmedCOVNotification    },
               { ssExecute,  asUnconfirmedCOVNotification  }
             },
         "DS-COV-B",
             { { ssExecute,  asSubscribeCOV               },
               { ssInitiate, asConfirmedCOVNotification   },
               { ssInitiate, asUnconfirmedCOVNotification }
             },
         "DS-COVP-A",
             { { ssInitiate, asSubscribeCOVProperty       },
               { ssExecute,  asConfirmedCOVNotification   },
               { ssExecute,  asUnconfirmedCOVNotification }
             },
         "DS-COVP-B",
             { { ssExecute,  asSubscribeCOVProperty       },
               { ssInitiate, asConfirmedCOVNotification   },
               { ssInitiate, asUnconfirmedCOVNotification }
             },
         "DS-COVU-A",
             { { ssExecute,  asUnconfirmedCOVNotification }
             },
         "DS-COVU-B",
             { { ssInitiate, asUnconfirmedCOVNotification }
             },
         "AE-N-A",
             { { ssExecute,  asConfirmedEventNotification   },
               { ssExecute,  asUnconfirmedEventNotification }
             },
         "AE-N-I-B",
             { { ssInitiate, asConfirmedEventNotification    },
               { ssInitiate, asUnconfirmedEventNotification  }
             },
         "AE-N-E-B",
             { { ssInitiate, asConfirmedEventNotification    },
               { ssInitiate, asUnconfirmedEventNotification  }
             },
         "AE-EL-I-B",
             { { ssExecute,  asReadRange }
             },
         "AE-EL-E-B",
             { { ssExecute, asConfirmedEventNotification    },
               { ssExecute, asUnconfirmedEventNotification  },
               { ssExecute,  asReadRange }
             },
         "AE-ELV-A",
             { { ssInitiate,  asReadRange }
             },
         "AE-ELVM-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty },
               { ssInitiate, asReadRange }
             },
         "AE-ACK-A",
             { { ssInitiate, asAcknowledgeAlarm }
             },
         "AE-ACK-B",
             { { ssExecute,  asAcknowledgeAlarm }
             },
         "AE-ASUM-A",
             { { ssInitiate, asGetAlarmSummary  }
             },
         "AE-ASUM-B",
             { { ssExecute,  asGetAlarmSummary  }
             },
         "AE-ESUM-A",
             { { ssInitiate, asGetEnrollmentSummary }
             },
         "AE-ESUM-B",
             { { ssExecute,  asGetEnrollmentSummary }
             },
         "AE-INFO-A",
             { { ssInitiate, asGetEventInformation }
             },
         "AE-INFO-B",
             { { ssExecute,  asGetEventInformation }
             },
         "AE-LS-A",
             { { ssInitiate, asLifeSafetyOperation }
             },
         "AE-LS-B",
             { { ssExecute,  asLifeSafetyOperation }
             },
         "SCHED-A",   /* no specific services required, other requirements are in the code */
             { { 0 }
             },
         "SCHED-I-B", // no specific services required, other requirements are in the code
             { { ssExecute, asReadProperty },
               { ssExecute, asWriteProperty },
             // these are an OR requirement and handled later
             //{ ssExecute, asTimeSynchronization },
             //{ ssExecute, asUTCTimeSynchronization }
             },
         "SCHED-E-B", // no specific services required, other requirements are in the code
             { { ssExecute, asReadProperty },
               { ssExecute, asWriteProperty },
               { ssInitiate, asWriteProperty },
             // these are an OR requirement and handled later
             //{ ssExecute, asTimeSynchronization },
             //{ ssExecute, asUTCTimeSynchronization }
             },
         "T-VMT-A",
             { { ssInitiate, asReadRange }
             },
         "T-VMT-I-B",
             { { ssExecute,  asReadRange }
             },
         "T-VMT-E-B",  // no specific services required, other requirements are in the code
             { { ssExecute, asReadRange },
               { ssInitiate, asReadProperty }
             },
         "T-ATR-A",
             { { ssExecute,  asConfirmedEventNotification },
               { ssExecute,  asUnconfirmedEventNotification },
               { ssInitiate, asReadRange                  }
             },
         "T-ATR-B",
             { { ssInitiate, asConfirmedEventNotification },
               { ssInitiate, asUnconfirmedEventNotification },
               { ssExecute,  asReadRange                  }
             },
         "DM-DDB-A",
             { { ssInitiate, asWho_Is },
               { ssExecute,  asI_Am   }
             },
         "DM-DDB-B",
             { { ssExecute,  asWho_Is },
               { ssInitiate, asI_Am   }
             },
         "DM-DOB-A",
             { { ssInitiate, asWho_Has  },
               { ssExecute,  asI_Have   }
             },
         "DM-DOB-B",
             { { ssExecute,  asWho_Has },
               { ssInitiate, asI_Have  }
             },
         "DM-DCC-A",
             { { ssInitiate, asDeviceCommunicationControl }
             },
         "DM-DCC-B",
             { { ssExecute,  asDeviceCommunicationControl }
             },
         "DM-PT-A",
             { { ssInitiate, asConfirmedPrivateTransfer     },
               { ssInitiate, asUnconfirmedPrivateTransfer   }
             },
         "DM-PT-B",
             { { ssExecute, asConfirmedPrivateTransfer     },
               { ssExecute, asUnconfirmedPrivateTransfer   }
             },
         "DM-TM-A",
             { { ssInitiate, asConfirmedTextMessage      },
               { ssInitiate, asUnconfirmedTextMessage    }
             },
         "DM-TM-B",
             { { ssExecute, asConfirmedTextMessage       },
               { ssExecute, asUnconfirmedTextMessage     }
             },
         "DM-TS-A",
             { { ssInitiate, asTimeSynchronization }
             },
         "DM-TS-B",
             { { ssExecute,  asTimeSynchronization }
             },
         "DM-UTC-A",
             { { ssInitiate, asUTCTimeSynchronization }
             },
         "DM-UTC-B",
             { { ssExecute,  asUTCTimeSynchronization }
             },
         "DM-RD-A",
             { { ssInitiate, asReinitializeDevice }
             },
         "DM-RD-B",
             { { ssExecute,  asReinitializeDevice }
             },
         "DM-BR-A",
             { { ssInitiate, asAtomicReadFile      },
               { ssInitiate, asAtomicWriteFile     },
               { ssInitiate, asCreateObject        },
               { ssInitiate, asReinitializeDevice  }
             },
         "DM-BR-B",
             { { ssExecute,  asAtomicReadFile      },
               { ssExecute,  asAtomicWriteFile     },
               { ssExecute,  asReinitializeDevice  }
             },
         "DM-R-A",
             { { ssExecute,  asUnconfirmedCOVNotification }
             },
         "DM-R-B",
             { { ssInitiate, asUnconfirmedCOVNotification }
             },
         "DM-LM-A",
             { { ssInitiate, asAddListElement       },
               { ssInitiate, asRemoveListElement    }
             },
         "DM-LM-B",
             { { ssExecute,  asAddListElement       },
               { ssExecute,  asRemoveListElement    }
             },
         "DM-OCD-A",
             { { ssInitiate, asCreateObject       },
               { ssInitiate, asDeleteObject       }
             },
         "DM-OCD-B",
             { { ssExecute,  asCreateObject       },
               { ssExecute,  asDeleteObject       }
             },
         "DM-VT-A",
             { { ssInitiate, asVT_Open             },
               { ssInitiate, asVT_Close            },
               { ssInitiate, asVT_Data             },
               { ssExecute,  asVT_Close            },
               { ssExecute,  asVT_Data             }
             },
         "DM-VT-B",
             { { ssExecute,  asVT_Open             },
               { ssInitiate, asVT_Close            },
               { ssInitiate, asVT_Data             },
               { ssExecute,  asVT_Close            },
               { ssExecute,  asVT_Data             }
             },
         "NM-CE-A",  // network services are not specified in EPICS
             { { 0 }
             },
         "NM-CE-B",  // network services are not specified in EPICS
             { { 0 }
             },
         "NM-RC-A",  // network services are not specified in EPICS
             { { 0 }
             },
         "NM-RC-B",  // network services are not specified in EPICS
             { { 0 }
             },
         // Added Workstation BIBBs 12/5/2007 LJT
         "DS-V-A",
             { { ssInitiate, asReadProperty }
             },
         "DS-AV-A",
             { { ssInitiate, asReadProperty }
             },
         "DS-M-A",
             { { ssInitiate, asWriteProperty }
             },
         "DS-AM-A",
             { { ssInitiate, asWriteProperty }
             },
         "AE-VN-A",
             { { ssExecute, asConfirmedEventNotification },
               { ssExecute, asUnconfirmedEventNotification }
             },
         "AE-AVN-A",
             { { ssExecute, asConfirmedEventNotification },
               { ssExecute, asUnconfirmedEventNotification }
             },
         "AE-VM-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty }
             },
         "AE-AVM-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty },
               { ssInitiate, asCreateObject },
               { ssInitiate, asDeleteObject }
             },
         "AE-AS-A",
             { { ssInitiate, asGetEventInformation },
               { ssInitiate, asGetAlarmSummary }
             },
         "SCHED-AVM-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty },
               { ssInitiate, asCreateObject },
               { ssInitiate, asDeleteObject }
             },
         "SCHED-VM-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty }
             },
         "SCHED-WS-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty }
             },
         "SCHED-WS-I-B",
             { { ssExecute, asReadProperty },
               { ssExecute, asWriteProperty },
               { ssExecute, asTimeSynchronization },
               { ssExecute, asUTCTimeSynchronization }
             },
         "SCHED-R-B",
            { { ssExecute, asReadProperty },
              { ssExecute, asTimeSynchronization },
              { ssExecute, asUTCTimeSynchronization }
            },
         "T-AVM-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty },
               { ssInitiate, asCreateObject },
               { ssInitiate, asDeleteObject },
               { ssInitiate, asReadRange }
             },
         "T-VM-I-B",
             { { ssExecute,  asReadRange }
             },
         "T-VM-E-B",
             { { ssExecute, asReadRange },
               { ssInitiate, asReadProperty }
             },
         "T-V-A",
             { { ssInitiate, asReadRange }
             },
         "T-A-A",
             { { ssInitiate, asReadRange },
               { ssExecute, asConfirmedEventNotification }
             },
         "DM-ANM-A",
            { { ssInitiate, asWho_Is },
              { ssExecute,  asI_Am   },
              { ssExecute,  asWho_Is },
              { ssInitiate, asI_Am   }
            },
         "DM-ADM-A",
             { { ssInitiate, asReadProperty }
             },
         "DM-ATS-A",
             { { ssInitiate, asTimeSynchronization },
               { ssInitiate, asUTCTimeSynchronization }
             },
         "DM-MTS-A",
             { { ssInitiate, asTimeSynchronization },
               { ssInitiate, asUTCTimeSynchronization }
             },
         // These names have been deprecated and could be removed
         "SCH-AVM-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty },
               { ssInitiate, asCreateObject },
               { ssInitiate, asDeleteObject }
             },
         "SCH-VM-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty }
             },
         "SCH-WS-A",
             { { ssInitiate, asReadProperty },
               { ssInitiate, asWriteProperty }
             },
         "SCH-WS-I-B",
             { { ssExecute, asReadProperty },
               { ssExecute, asWriteProperty },
               { ssExecute, asTimeSynchronization },
               { ssExecute, asUTCTimeSynchronization }
             },
         "SCH-R-B",
            { { ssExecute, asReadProperty },
              { ssExecute, asTimeSynchronization },
              { ssExecute, asUTCTimeSynchronization }
            },
         "SCH-I-B", // no specific services required, other requirements are in the code
             { { ssExecute, asReadProperty },
               { ssExecute, asWriteProperty },
               { ssExecute, asTimeSynchronization },
               { ssExecute, asUTCTimeSynchronization }
             },
         "SCH-E-B", // no specific services required, other requirements are in the code
             { { ssExecute, asReadProperty },
               { ssExecute, asWriteProperty },
               { ssInitiate, asWriteProperty },
               { ssExecute, asTimeSynchronization },
               { ssExecute, asUTCTimeSynchronization }
             },

         // Added 26 August 2014
         // TODO: bogus service lists yet
         "T-VMMV-A",  // Deprecated in 135-2012
             { { ssInitiate, asReadRange }
             },
         "T-VMMV-I-B",
             { { ssExecute, asReadRange }
             },
         "T-VMMV-E-B",
             { { ssExecute, asReadRange },
               { ssInitiate, asReadPropertyMultiple }
             },
         "T-AMVR-A",
             { { ssExecute,  asConfirmedEventNotification },
               { ssExecute,  asUnconfirmedEventNotification },
               { ssInitiate, asReadRange                  }
             },
         "T-AMVR-B",
             { { ssInitiate, asConfirmedEventNotification },
               { ssInitiate, asUnconfirmedEventNotification },
               { ssExecute,  asReadRange                  }
             },
         };

// TODO: Duplicate of StringTables.cpp BACnetServicesSupported
// The order and position of elements in this array is important!
// It must correspond with the bit positions defined by BACnetServicesSupported
// in section 21 of the BACnet standard.
static const char *StandardServices[]={
         "AcknowledgeAlarm",                       //0
         "ConfirmedCOVNotification",               //1
         "ConfirmedEventNotification",             //2
         "GetAlarmSummary",                        //3
         "GetEnrollmentSummary",                   //4
         "SubscribeCOV",                           //5
         "AtomicReadFile",                         //6
         "AtomicWriteFile",                        //7
         "AddListElement",                         //8
         "RemoveListElement",                      //9
         "CreateObject",                           //10
         "DeleteObject",                           //11
         "ReadProperty",                           //12
         "ReadPropertyConditional",                //13
         "ReadPropertyMultiple",                   //14
         "WriteProperty",                          //15
         "WritePropertyMultiple",                  //16
         "DeviceCommunicationControl",             //17
         "ConfirmedPrivateTransfer",               //18
         "ConfirmedTextMessage",                   //19
         "ReinitializeDevice",                     //20
         "VT-Open",                                //21
         "VT-Close",                               //22
         "VT-Data",                                //23
         "Authenticate",                           //24
         "RequestKey",                             //25
         "I-Am",                                   //26   madanner 6/03: "I-AM"
         "I-Have",                                 //27
         "UnconfirmedCOVNotification",             //28   msdanner 9/04: was "UnConfirmed..."
         "UnconfirmedEventNotification",           //29   msdanner 9/04: was "UnConfirmed..."
         "UnconfirmedPrivateTransfer",             //30   msdanner 9/04: was "UnConfirmed..."
         "UnconfirmedTextMessage",                 //31   msdanner 9/04: was "UnConfirmed..."
         "TimeSynchronization",                    //32
         "Who-Has",                                //33
         "Who-Is",                                 //34
         "ReadRange",                              //35   madanner 6/03: "Read-Range"
         "UTCTimeSynchronization",                 //36   madanner 6/03: "UTC-Time-Synchronization"
         "LifeSafetyOperation",                    //37
         "SubscribeCOVProperty",                   //38
         "GetEventInformation",                    //39   last in 135-2008 (rev 7) through 135-2010 (rev 12)
         "WriteGroup"                              //40   last in 135-2012 (rev 14)
   // TODO: Also update StringTables.cpp BACnetServicesSupported
         };

// The order and position of elements in this array is important!
// It must correspond with the definition of BACnetObjectType in section 21
// of the BACnet standard.
static const char *StandardObjects[]={
         "Analog Input",                //0
         "Analog Output",               //1
         "Analog Value",                //2
         "Binary Input",                //3
         "Binary Output",               //4
         "Binary Value",                //5
         "Calendar",                    //6
         "Command",                     //7
         "Device",                      //8
         "Event Enrollment",            //9
         "File",                        //10
         "Group",                       //11
         "Loop",                        //12
         "Multi-state Input",           //13
         "Multi-state Output",          //14
         "Notification Class",          //15
         "Program",                     //16
         "Schedule",                    //17
         "Averaging",                   //18
         "Multi-state Value",           //19
         "Trend Log",                   //20  madanner 6/03: "Trend-Log"
         "Life Safety Point",           //21  msdanner 9/04: added
         "Life Safety Zone",            //22  msdanner 9/04: added
         "Accumulator",                 //23  Shiyuan Xiao
         "Pulse Converter",             //24  Shiyuan Xiao
         "Event Log",
         "Global Group",
         "Trend Log Multiple",
         "Load Control",
         "Structured View",
         "Access Door",
         "Lighting Output",
         "Access Credential",       /* Addendum 2008-j */
         "Access Point",
         "Access Rights",
         "Access User",
         "Access Zone",
         "Credential Data Input",
         "Network Security",           /* addendum 2008-g */
         "BitString Value",            /* addendum 2008-w */
         "CharacterString Value",
         "Date Pattern Value",
         "Date Value",
         "DateTime Pattern Value",
         "DateTime Value",
         "Integer Value",
         "Large Analog Value",
         "OctetString Value",
         "Positive Integer Value",
         "Time Pattern Value",
         "Time Value",              // 50
         "Notification Forwarder",  // 51
         "Alert Enrollment",        // 52
         "Channel",                 // 53,
         "Lighting Output"          // 54 Max in 135-2012 BACNET_PROTOCOL_REVISION = 14
   // TODO: see comments at the end of StringTables.cpp BACnetObjectType regarding
   // actions to be taken when an object type is added
};


// The order of these is important.  New ones should be added to the end, or
// at least inserted after the last "Point-To-Point" entry.
// Maximum is specified by MAX_DATALINK_OPTIONS in vts.h
static const char *StandardDataLinks[]={
         "ISO 8802-3, 10BASE5",                          //0
         "ISO 8802-3, 10BASE2",                          //1
         "ISO 8802-3, 10BASET",                          //2
         "ISO 8802-3, fiber",                            //3
         "ARCNET, coax star",                            //4
         "ARCNET, coax bus",                             //5
         "ARCNET, twisted pair star",                    //6
         "ARCNET, twisted pair bus",                     //7
         "ARCNET, fiber star",                           //8
         "MS/TP master. Baud rate(s)",                   //9
         "MS/TP slave. Baud rate(s)",                    //10
         "Point-To-Point. EIA 232, Baud rate(s)",        //11
         "Point-To-Point. Modem, Baud rate(s)",          //12
         "Point-To-Point. Modem, Autobaud range",        //13
         "LonTalk",                                      //14
         "BACnet/IP, 'DIX' Ethernet",                    //15
         "BACnet/IP, PPP",                               //16
         "Other"                                         //17
         };

// Position is important.  Must preserve index numbers.
static const char *SpecialFunctionality[]={
         "Maximum APDU size in octets",                  //0
         "Segmented Requests Supported, window size",    //1
         "Segmented Responses Supported, window size",   //2
         "Router",                                       //3
         "BACnet/IP BBMD"                                //4
         };

static nameoctet Charsets[]={
         "ANSI X3.4",                     csANSI,  // As specified in 135.1-2013, which hasn't learned about UTF-8
         "ANSI X3.4/UTF-8",               csANSI,
         "ANSI X3.4 / UTF-8",             csANSI,
         "UTF-8",                         csANSI,  // Not in 135.1-2013, but seen in an EPICS from BTF
         "IBM/Microsoft DBCS",            csDBCS,  // As specified in 135.1-2013
         "JIS C 6226",                    csJIS,   // As specified in 135.1-2013
         "ISO 10646 (UCS-4)",             csUCS4,  // As specified in 135.1-2013
         "ISO 10646 (UCS-2)",             csUCS2,  // As specified in 135.1-2013
         "ISO 8859-1",                    cs8859   // As specified in 135.1-2013
         };

static const char *FailTimes[MAX_FAIL_TIMES]={     // as defined by 135.1-2013 4.5.9
         "Notification Fail Time",           // 0
         "Internal Processing Fail Time",    // 1
         "Minimum ON/OFF Time",              // 2
         "Schedule Evaluation Fail Time",    // 3
         "External Command Fail Time",       // 4
         "Program Object State Change Fail Time",// 5
         "Acknowledgement Fail Time",        // 6
         "Slave Proxy Confirm Interval",     // 7
         "Unconfirmed Response Fail Time"    // 8
         };

static const char *MonthNames[] = {
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
   "December",
   "Odd",
   "Even"
};

static const char *DOWNames[]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};

static void print_debug(char *fmt, ...)
{
#ifdef MDEBUG
   static FILE *pFile = NULL;
   static char path[MAX_PATH];

   if (fmt)
   {
      if (pFile == NULL)
      {
         // First time: create a new debug file
         // (Use GetCurrentDirectory only so we can SHOW where the file is)
         GetCurrentDirectory( sizeof(path), path );
         strcat( path, "\\vtsepics.txt" );
         pFile = fopen(path,"w");
      }

      if (pFile)
      {
         va_list ap;
         va_start(ap,fmt);
         vfprintf(pFile,fmt,ap);
      }
   }
   else if (pFile)
   {
      // NULL used as a flag to close the file
      fclose(pFile);
      pFile = NULL;

      CString str;
      str.Format( "Debug file is in %s", path );
      tperror( str, false );
   }
#else
   (void)fmt;
#endif
}

//======================
// Setup global error file:
static FILE * pfileError = NULL;


///////////////////////////////////////////////////////////////////////
// Get Property Table information based on standard object type
//in: objtype     the desired object type (using the standard enumeration)
//    propindex   the property table index, or 0xFFFF meaning get num properties
//    pname    points to a buffer to contain the property name
//out:   if objtype is invalid, or propindex is invalid then 0xFFFFFFFF
//    if propindex was 0xFFFF then return the number of properties for that object type
//    else     return the property ID for that slot
//             in this case, also return the property name in the buffer pname

dword APIENTRY VTSAPIgetpropinfo(word objtype, word propindex,
                                 char *pname, word *ptype,
                                 word *pgroup, word *pflags,
                                 word *pet)
{
   const propdescriptor *pt, *ptx;
   pt = ptx = GetPropDescriptorTable( objtype );
   word np = 1;
   while((ptx->PropGroup & LAST) == 0)
   {
      np++;
      ptx++;
   }
   if (propindex == 0xFFFF) return (dword)np;      //just say how many properties there are
   if (propindex >= np) return 0xFFFFFFFF;         //invalid property index

   strcpy( pname, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( pt[propindex].PropID ) );
   if (ptype) *ptype = (word)pt[propindex].ParseType;
   if (pgroup) *pgroup = (word)pt[propindex].PropGroup;
   if (pflags) *pflags = pt[propindex].PropFlags;
   if (pet) *pet = pt[propindex].PropET;
   return pt[propindex].PropID;                 //and property ID
}


///////////////////////////////////////////////////////////////////////
// Delete an object created by ReadTextPICS
//in: p     points to the object
void  APIENTRY DeletePICSObject(generic_object *p)
{
   word  i;
   void *vp,*vq,*vr;
   CString errMsg;

   switch (p->object_type)                 //release this object's special fields
   {
   case OBJ_CALENDAR:
      vp=((calendar_obj_type *)p)->date_list;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetCalendarEntry *)vp)->next;
         free(vq);
      }
      break;

   case OBJ_COMMAND:
      for (i=0;i<MAX_ACTION_TEXTS;i++)
      {
         vp=((command_obj_type *)p)->action[i];
         while(vp!=NULL)
         {
            vq=vp;
            vp=((BACnetActionCommand *)vp)->next;
            free(vq);
         }
         if (((command_obj_type *)p)->action_text[i]!=NULL)
            free(((command_obj_type *)p)->action_text[i]);
      }
      break;

   case OBJ_DEVICE:
      vp=((device_obj_type *)p)->object_list;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetObjectIdentifier *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->vt_classes_supported;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetVTClassList *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->active_vt_sessions;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetVTSession *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->list_session_keys;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetSessionKey *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->time_synch_recipients;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetRecipient *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->device_add_binding;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetAddressBinding *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->configuration_files;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetObjectIdentifier *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->active_cov_subscriptions;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetCOVSubscription *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->manual_slave_add_bind;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetAddressBinding *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->slave_add_bind;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetAddressBinding *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->auto_slave_disc;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BooleanList *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->slave_proxy_enable;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BooleanList *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->structured_object_list;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetObjectIdentifier *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->restart_notification_recipients;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetRecipient *)vp)->next;
         free(vq);
      }
      vp=((device_obj_type *)p)->utc_time_synchronization_recipients;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetRecipient *)vp)->next;
         free(vq);
      }
      break;

   case OBJ_EVENT_ENROLLMENT:
      vp=((ee_obj_type *)p)->parameter_list.list_bitstring_value;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetListBitstringValue *)vp)->next;
         free(vq);
      }
      vp=((ee_obj_type *)p)->parameter_list.list_of_value;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetPropertyStates *)vp)->next;
         free(vq);
      }
      break;

   case OBJ_FILE:
      break;

   case OBJ_GROUP:
      vr=((group_obj_type *)p)->list_of_group_members;
      while(vr!=NULL)
      {
         vp=((BACnetReadAccessSpecification *)vr)->list_of_prop_ref;
         while(vp!=NULL)
         {
            vq=vp;
            vp=((BACnetPropertyReference *)vp)->next;
            free(vq);
         }
         vq=vr;
         vr=((BACnetReadAccessSpecification *)vr)->next;
         free(vq);
      }
      break;

   case OBJ_LOOP:
      vp=((loop_obj_type *)p)->setpoint_ref;
      if (vp!=NULL) free(vp);
      break;

   case OBJ_MULTI_STATE_INPUT:
      for (i=0;i<MAX_STATE_TEXTS;i++)
      {
         if (((mi_obj_type *)p)->state_text[i]!=NULL)
            free(((mi_obj_type *)p)->state_text[i]);
      }
      vp=((mi_obj_type *)p)->alarm_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((UnsignedList *)vp)->next;
         free(vq);
      }
      vp=((mi_obj_type *)p)->fault_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((UnsignedList *)vp)->next;
         free(vq);
      }
      break;

   case OBJ_MULTI_STATE_OUTPUT:
      for (i=0;i<MAX_STATE_TEXTS;i++)
      {
         if (((mo_obj_type *)p)->state_text[i]!=NULL)
            free(((mo_obj_type *)p)->state_text[i]);
      }
      break;

   case OBJ_PROGRAM:
      break;

   case OBJ_NOTIFICATIONCLASS:
      vp=((nc_obj_type *)p)->recipient_list;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetDestination *)vp)->next;
         free(vq);
      }
      break;

   case OBJ_SCHEDULE:
      for (i=0;i<7;i++)
      {
         vp=((schedule_obj_type *)p)->weekly_schedule[i];
         while(vp!=NULL)
         {
            vq=vp;
            vp=((BACnetTimeValue *)vp)->next;
            free(vq);
         }
      }
      vr=((schedule_obj_type *)p)->exception_schedule.special_event;
      while(vr!=NULL)
      {
         vp=((BACnetSpecialEvent *)vr)->list_of_time_values;
         while(vp!=NULL)
         {
            vq=vp;
            vp=((BACnetTimeValue *)vp)->next;
            free(vq);
         }
         vq=vr;
         vr=((BACnetSpecialEvent *)vr)->next;
         free(vq);
      }
      vp=((schedule_obj_type *)p)->list_obj_prop_ref;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetObjectPropertyReference *)vp)->next;
         free(vq);
      }
      break;

   case OBJ_AVERAGING:
        break;

   case OBJ_ANALOG_INPUT:
      break;

   case OBJ_ANALOG_OUTPUT:
      break;

   case OBJ_ANALOG_VALUE:
      break;

   case OBJ_BINARY_INPUT:
      break;

   case OBJ_BINARY_OUTPUT:
      break;

   case OBJ_BINARY_VALUE:
      break;

   case OBJ_ACCUMULATOR:
      break;

   case OBJ_MULTI_STATE_VALUE: // msdanner 9/2004
      for (i=0;i<MAX_STATE_TEXTS;i++)
      {
         if (((msv_obj_type *)p)->state_text[i]!=NULL)
            free(((msv_obj_type *)p)->state_text[i]);
      }
      vp=((msv_obj_type *)p)->alarm_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((UnsignedList *)vp)->next;
         free(vq);
      }
      vp=((msv_obj_type *)p)->fault_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((UnsignedList *)vp)->next;
         free(vq);
      }
      break;

   case OBJ_TREND_LOG:
      break;

      case OBJ_LIFE_SAFETY_POINT:
      vp=((lifesafetypoint_obj_type *)p)->member_of;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetDeviceObjectReference *)vp)->next;
         free(vq);
      }
      vp=((lifesafetypoint_obj_type *)p)->accepted_modes;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetEnumList *)vp)->next;
         free(vq);
      }
      vp=((lifesafetypoint_obj_type *)p)->life_safety_alarm_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetEnumList *)vp)->next;
         free(vq);
      }
      vp=((lifesafetypoint_obj_type *)p)->alarm_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetEnumList *)vp)->next;
         free(vq);
      }
      vp=((lifesafetypoint_obj_type *)p)->fault_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetEnumList *)vp)->next;
         free(vq);
      }
      break;

   case OBJ_LIFE_SAFETY_ZONE:
      vp=((lifesafetyzone_obj_type *)p)->member_of;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetDeviceObjectReference *)vp)->next;
         free(vq);
      }
      vp=((lifesafetyzone_obj_type *)p)->zone_members;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetDeviceObjectReference *)vp)->next;
         free(vq);
      }
      vp=((lifesafetyzone_obj_type *)p)->accepted_modes;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetEnumList *)vp)->next;
         free(vq);
      }
      vp=((lifesafetyzone_obj_type *)p)->life_safety_alarm_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetEnumList *)vp)->next;
         free(vq);
      }
      vp=((lifesafetyzone_obj_type *)p)->alarm_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetEnumList *)vp)->next;
         free(vq);
      }
      vp=((lifesafetyzone_obj_type *)p)->fault_values;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetEnumList *)vp)->next;
         free(vq);
      }
        break;

   case OBJ_PULSE_CONVERTER:
      break;

   case OBJ_STRUCTURED_VIEW:
      vp=((sv_obj_type *)p)->subordinate_list;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetDeviceObjectReference *)vp)->next;
         free(vq);
      }

      for (i=0; i < MAX_SV_ANNOTATIONS; i++)
      {
         if (((sv_obj_type *)p)->subordinate_annotations[i]!=NULL)
            free(((sv_obj_type *)p)->subordinate_annotations[i]);
      }
      break;

    case OBJ_TREND_LOG_MULTIPLE:
      break;

    case OBJ_EVENT_LOG:
      break;

    case OBJ_GLOBAL_GROUP:
      vp=((global_group_obj_type *)p)->covu_recipients;
      while(vp!=NULL)
      {
         vq=vp;
         vp=((BACnetRecipient *)vp)->next;
         free(vq);
      }
      break;

    case OBJ_LOAD_CONTROL:
      for(i=0; i<MAX_SHED_LEVELS; i++)
      {
         if (((lc_obj_type *)p)->shed_level_descriptions[i]!=NULL)
            free(((lc_obj_type *)p)->shed_level_descriptions[i]);
      }

      vp = ((lc_obj_type *)p)->shed_levels;
      while (vp != NULL)
      {
         vq = vp;
         vp = ((UnsignedList *)vp)->next;
         free(vq);
      }
      break;

   case OBJ_ACCESS_DOOR:
      vp = ((ad_obj_type *)p)->alarm_values;
      while (vp != NULL)
      {
         vq = vp;
         vp = ((BACnetEnumList*)vp)->next;
         free(vq);
      }

      vp = ((ad_obj_type *)p)->fault_values;
      while (vp != NULL)
      {
         vq = vp;
         vp = ((BACnetEnumList*)vp)->next;
         free(vq);
      }

      vp = ((ad_obj_type *)p)->door_members;
      while (vp != NULL)
      {
         vq = vp;
         vp = ((BACnetDeviceObjectReference*)vp)->next;
         free(vq);
      }

      vp = ((ad_obj_type *)p)->masked_alarm_values;
      while (vp != NULL)
      {
         vq = vp;
         vp = ((BACnetEnumList*)vp)->next;
         free(vq);
      }
      break;

   case OBJ_CHARACTERSTRING_VALUE:
      for(i=0; i<MAX_FAULT_STRINGS; i++)
      {
         if ( ((charstring_obj_type *)p)->alarm_values[i]!=NULL)
            free(((charstring_obj_type *)p)->alarm_values[i]);
      }
      for(i=0; i<MAX_FAULT_STRINGS; i++)
      {
         if ( ((charstring_obj_type *)p)->fault_values[i]!=NULL)
            free(((charstring_obj_type *)p)->fault_values[i]);
      }
      break;

   case OBJ_INTEGER_VALUE:
      break;

   case OBJ_POSITIVE_INTEGER_VALUE:
      break;

   case OBJ_DATETIME_VALUE:
      // No allocations to clean up
      break;

   default:
      // Someone forgot to implement delete code for this object type
      errMsg.Format( "**WARNING**: No delete code for this object %s (type = %d), will probably leak \n",
                     p->object_name, p->object_type );
      OutputDebugString( errMsg );
      // Attempt the deletion anyway
      break;
   }

   // Clean up the base object
   for (i=0; i<3; i++)
   {
      if (p->event_time_stamps[i]!=NULL)
        free(p->event_time_stamps[i]);
   }

   for(i=0; i<3; i++)
   {
      if ( p->event_message_texts[i] != NULL)
         free( p->event_message_texts[i] );
      if ( p->event_message_texts_config[i] != NULL)
         free( p->event_message_texts_config[i] );
   }

   vp = p->property_list;
   while (vp != NULL)
   {
      vq = vp;
      vp = ((BACnetEnumList*)vp)->next;
      free(vq);
   }

   free(p);                         //and the object itself
}

///////////////////////////////////////////////////////////////////////
// Open and Read a TPI file (i.e. EPICS text file)
///////////////////////////////////////////////////////////////////////
bool APIENTRY ReadTextPICS(
        const char *tp, // filename string
        PICSdb *pd,     // EPICS database
        int *errc,      // error counter
        int *errPICS)   // returns the error
{
   // Flag parsing (vs consistency checking)
   lPICSErr = -1;

   //Set true to stop reading
   s_cancel = false;
   s_error = false;

   // Initial buffer, which may grow if EPICS file has long lines
   lpSize = 512;
   lb = (char*)malloc( lpSize );

   CString fileName;
   GetTempPath( MAX_PATH, fileName.GetBuffer( MAX_PATH ) );
   fileName.ReleaseBuffer();
   fileName += FILE_CHECK_EPICS_CONS;

   pfileError = fopen( fileName, "w");
   TRACE( "Errors logged to %s \n", fileName );

   pd->Database=NULL;
   memset(pd->BACnetStandardObjects,soNotSupported,sizeof(pd->BACnetStandardObjects));
   memset(pd->BACnetStandardServices,ssNotSupported,sizeof(pd->BACnetStandardServices));

   // Verify that the BIBBSupported is large enough to hold all the BIBBS
   assert( (sizeof(pd->BIBBSupported)/sizeof(pd->BIBBSupported[0]) >= (sizeof(BIBBs)/sizeof(BIBBs[0]))) );
   // initialize to no BIBBs supported
   memset(pd->BIBBSupported,0,sizeof(pd->BIBBSupported));   //default is not supported

   memset(pd->BACnetFailTimes,ftNotSupported,sizeof(pd->BACnetFailTimes)); //default is not supported
   pd->BACnetFunctionalGroups=0;          //default is none
    // default is no data links supported
   memset(pd->DataLinkLayerOptions, 0, sizeof(pd->DataLinkLayerOptions)); //default is none

   // Initialize Special Functionality items, in case this section is omitted in EPICS
   pd->RouterFunctions=rfNotSupported;       //default is none
   pd->SegmentedRequestWindow=pd->SegmentedResponseWindow=0;
   pd->MaxAPDUSize=50;
   pd->BBMD=0;  // default is no BBMD support

   print_debug("RTP: open file '%.275s'\n",tp);
   lerrc = 0;                       //no errors yet
   if ((ifile=fopen(tp,"r"))==NULL) //open input file
   {
      tperror(_strerror(tp),false);
      goto rtpexit;
   }

   lc=0;
   readline();                            //read header line from the file
   if (_strnicmp(lb,picshdr,6))           //invalid signature
   {
      tperror("This file does not contain a supported form of EPICS!",false);
      s_cancel = true;
   }
   else
   {
      print_debug("RTP: Read line '%s'\n",lb);

      readline();                         //read a line from the file
      lp = &lb[0];
      print_debug("RTP: rl1: Read line '%s'\n",lb);

      if (_stricmp(lb,BeginPics))            //invalid signature
      {
         tperror("Invalid Text PICS signature.",false);
         s_cancel = true;
      }
   }

   while (!s_cancel && feof(ifile)==0)
   {
      readline();                         //read a line from the file
      print_debug("RTP: rl2: Read line '%s'\n",lb);

      if (lb[0])                          //not a blank line
      {
         if (_stricmp(lb,EndPics)==0)     //found the end
            break;                        //we're done

         if ((lp=strchr(lb,':'))==NULL)   //must have a section name
         {
            lp = &lb[0];
            s_cancel = tperror("Expected section name here...",true);
         }
         else
         {
            int nameLen;
            *lp++ = 0;               // make asciz section name and lp points to args
            preprocstr(lb);          // remove score and underscore
            if (_stricmp(lb,"Vendor Name")==0)
            {
               // Allocate as much space as we need (actually, more than enough due to quoting)
               nameLen = strlen(lp) + 1;
               pd->VendorName = new char[nameLen];
               s_cancel = setstring(pd->VendorName,nameLen,lp);
            }
            else if (_stricmp(lb,"Product Name")==0)
            {
               nameLen = strlen(lp) + 1;
               pd->ProductName = new char[nameLen];
               s_cancel = setstring(pd->ProductName,nameLen,lp);
            }
            else if (_stricmp(lb,"Product Model Number")==0)
            {
               nameLen = strlen(lp) + 1;
               pd->ProductModelNumber = new char[nameLen];
               s_cancel = setstring(pd->ProductModelNumber,nameLen,lp);
            }
            else if (_stricmp(lb,"Product Description")==0)
            {
               nameLen = strlen(lp) + 1;
               pd->ProductDescription = new char[nameLen];
               s_cancel = setstring(pd->ProductDescription,nameLen,lp);
            }
            else if (_stricmp(lb,"BIBBs Supported")==0)
            {
               s_cancel = ReadBIBBSupported(pd);
            }
            else if (_stricmp(lb,"BACnet Standard Application Services Supported")==0)
            {
               s_cancel = ReadStandardServices(pd);
            }
            else if (_stricmp(lb,"Standard Object Types Supported")==0)
            {
               s_cancel = ReadStandardObjects(pd);
            }
            else if (_stricmp(lb,"Data Link Layer Option")==0)
            {
               s_cancel = ReadDataLinkOptions(pd);
            }
            else if (_stricmp(lb,"Character Sets Supported")==0)
            {
               s_cancel = ReadCharsets(pd);
            }
            else if (_stricmp(lb,"Special Functionality")==0)
            {
               s_cancel = ReadSpecialFunctionality(pd);
            }
            else if (_stricmp(lb,"Default Property Value Restrictions")==0)
            {
               s_cancel = ReadDefaultPropertyValueRestrictions(pd);
            }
            else if (_stricmp(lb,"Fail Times")==0)
            {
               s_cancel = ReadFailTimes(pd);
            }
            else if (_stricmp(lb,"List of Objects in Test Device")==0)
            {
               s_cancel = ReadObjects(pd);
            }
            else
            {
               lp[-1]=':';
               s_cancel = tperror("Unknown section name",true);
            }
         }
      }
   }
   fclose(ifile);

   // EPICS consistency Check
   if (!s_cancel)
   {
      // New 135.1-2013 checks
      if (lerrc == 0)  // if no syntax errors, proceed with EPICS consistency tests
      {
         lPICSErr = 0;                    // enables counting of consistency errors
         CheckPICSConsistency2013(pd);    // EPICS consistency checks specified by 135.1-2013
         *errPICS = lPICSErr;             // return count of consistency errors
      }
   }

    //clear the global variables
   memset(ObjInTestDB,0,sizeof(ObjInTestDB));
   memset(DevObjList,0,sizeof(DevObjList));

rtpexit:
   *errc = lerrc;

   generic_object *pd2 = pd->Database;
   print_debug("Begin database printout.\n");
   while(pd2 != NULL)
   {
      print_debug("object id (%d, %d) object name = '%s' desc = '%s'\n",
                   (pd2->object_id)>>22, pd2->object_id && 0x3FFFFF,
                   pd2->object_name, pd2->description);
      pd2 = (generic_object *)pd2->next;
   }
   print_debug("End database printout.\n");
   print_debug(NULL);            // Special call to close any debug file

   if (pfileError != NULL)       // Close main error file
   {
      fflush( pfileError );
      fclose(pfileError);
   }
   pfileError = NULL;

   free( lb );
   lb = NULL;

   return !s_cancel;
}

///////////////////////////////////////////////////////////////////////
// Read Charactersets section of a TPI file
//in: ifile    file stream
//    pd       database structure to be filled in from PICS
//out:   true     if s_cancel selected

bool ReadCharsets(PICSdb *pd)
{
   int         i;
   ReadNext();                            //point to next token
   if (lp==NULL || *lp++!='{')            //no open token
      return tperror("Expected { here",true);

   pd->BACnetCharsets = 0;                //default is none
   while (lp!=NULL)
   {
      ReadNext();                         //point to next token
      rtrim(lp);
      if (*lp=='}' || lp==NULL) break;    //return, we're done with these

      for (i=0;i<(sizeof(Charsets)/sizeof(Charsets[0]));i++)
      {
         if (_stricmp(lp,Charsets[i].name)==0) //found it
         {
            pd->BACnetCharsets|=Charsets[i].octetcons;
            i=0;
            break;
         }
      }

      if (i)
      {
         if (tperror("Unknown Character Set",true))
            return true;
      }
   }
   return false;
}

///////////////////////////////////////////////////////////////////////
// Read BIBBs Supported section of a TPI file
//in: ifile    file stream
//    pd       database structure to be filled in from PICS
//out:   true     if cancel selected

bool ReadBIBBSupported(PICSdb *pd)
{
   int         i;
   ReadNext();                         //point to next token
   if (lp==NULL || *lp++!='{')              //no open token
      return tperror("Expected { here",true);

   // initialize to no BIBBs supported
   memset(pd->BIBBSupported,0,sizeof(pd->BIBBSupported));   //default is not supported
   while (lp!=NULL)
   {
      ReadNext();                      //point to next token
      rtrim(lp);
      if (*lp=='}' || lp==NULL) break;         //return, we're done with these

      for (i=0;i<(sizeof(BIBBs)/sizeof(BIBBs[0]));i++)
      {
         if (_stricmp(lp,BIBBs[i].name)==0) //found it
         {
            pd->BIBBSupported[i] = 1;     // Mark this BIBB supported
            i=0;
            break;
         }
      }

      if (i)
      {
         if (tperror("Unknown BIBB",true))
            return true;
      }
   }
   return false;
}

///////////////////////////////////////////////////////////////////////
// Read Standard Services section of a TPI file
//in: ifile    file stream
//    pd       database structure to be filled in from PICS
//out:   true     if cancel selected

bool ReadStandardServices(PICSdb *pd)
{
   int      i;
   char     *p;

   ReadNext();                         //point to next token
   if (lp==NULL || *lp++!='{')         //no open token
      return tperror("Expected { here",true);

   memset(pd->BACnetStandardServices,ssNotSupported,sizeof(pd->BACnetStandardServices));  //default is not supported
   while (lp!=NULL)
   {
      ReadNext();                      //point to next token
      if (*lp=='}' || lp==NULL) break; //return, we're done with these
      i = -1;                          //assume there is a problem
      if ((p=strchr(lp,SPACE))!=NULL)  //find the delimiter for supported stuff
      {
         *p++ = 0;                     //make service name be asciz
         for (i=0;i<(sizeof(StandardServices)/sizeof(StandardServices[0]));i++)
         {
            if (_stricmp(lp,StandardServices[i])==0) //found it
            {
               if (strstr(p,"Initiate")!=NULL)  //supports initiate
                  pd->BACnetStandardServices[i]|=ssInitiate;
               if (strstr(p,"Execute")!=NULL)   //supports execute
                  pd->BACnetStandardServices[i]|=ssExecute;
               i = (pd->BACnetStandardServices[i]==ssNotSupported) ? -1 : 0;
               break;
            }
         }
      }

      if (i<0)
      {
         //no delimiter after the service, or couldn't find initiate or execute
         p=strchr(lp,0);
         if (tperror("Expected 'Initiate' or 'Execute' here",true))
            return true;
      }
      else if (i>0)
      {
         if (tperror("Unknown Standard Service",true))
            return true;
      }
   }
   return false;
}

///////////////////////////////////////////////////////////////////////
// Read Standard Objects section of a TPI file
//in: ifile    file stream
//    pd       database structure to be filled in from PICS
//out:   true     if cancel selected

bool ReadStandardObjects(PICSdb *pd)
{
   int      i;
   char     *pcre,*pdel;
   octet    sup;

   ReadNext();                         //point to next token
   if (lp==NULL || *lp++!='{')         //no open token
      return tperror("Expected { here",true);

   memset(pd->BACnetStandardObjects,soNotSupported,sizeof(pd->BACnetStandardObjects)); //default is not supported
   while (lp!=NULL)
   {
      ReadNext();                      //point to next token
      if (*lp=='}' || lp==NULL) break; //return, we're done with these
      sup = soSupported;
      if ((pcre=strstr(lp,"Createable"))!=NULL) //supports create
         sup |= soCreateable;
      if ((pdel=strstr(lp,"Deleteable"))!=NULL) //supports delete
         sup |= soDeleteable;
      if (pcre != NULL) pcre[-1] = 0;  //cheesy way to "remove" this from the string
      if (pdel != NULL) pdel[-1] = 0;  //cheesy way to "remove" this from the string

      // Search "Traditional" object type names with spaces
      for (i=0;i<(sizeof(StandardObjects)/sizeof(StandardObjects[0]));i++)
      {
         if (_stricmp(lp,StandardObjects[i])==0) //found it
         {
            pd->BACnetStandardObjects[i] = sup;
            i = 0;
            break;
         }
      }

      if (i>0)
      {
         // Check for "enum-style" object type name with dashes or underbars
         i = NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumValue( lp );
         if ((i >= 0) && (i < sizeof(pd->BACnetStandardObjects)))
         {
            pd->BACnetStandardObjects[i] = sup;
         }
         else
         {
            if (tperror("Unknown Standard Object",true))
               return true;
         }
      }
   }

   if (pd->BACnetStandardObjects[OBJ_DEVICE]==soNotSupported)
      return tperror("The Device Object was not present in the list of Standard Objects!",true);
   return false;
}

///////////////////////////////////////////////////////////////////////
// Read Data Link Layer Options section of a TPI file
//in: ifile    file stream
//    pd       database structure to be filled in from PICS
//out:   true     if cancel selected

bool ReadDataLinkOptions(PICSdb *pd)
{
   int      i,j;
   bool     got9600;
   char     *p;
   dword    *dp;

   ReadNext();                         //point to next token
   if (lp==NULL || *lp++!='{')         //no open token
      return tperror("Expected { here",true);

   memset(pd->DataLinkLayerOptions, 0, sizeof(pd->DataLinkLayerOptions)); //default is none
   pd->PTPAutoBaud[0]=pd->PTPAutoBaud[1] = 0;
   for (i=0; i<16; i++)
   {
      pd->MSTPmasterBaudRates[i]=0;
      pd->MSTPslaveBaudRates[i]=0;
      pd->PTP232BaudRates[i]=0;
      pd->PTPmodemBaudRates[i]=0;
   }

   while (lp!=NULL)
   {
      ReadNext();                         //Read a line
      if (*lp=='}' || lp==NULL) break;    //return, we're done with these
      if ((p=strchr(lp,':')) != NULL)     //colon in this one
         *p = 0;                          //make it asciz there
      rtrim(lp);                          //trim trailing blanks
      for (i=0; i<(sizeof(StandardDataLinks)/sizeof(StandardDataLinks[0])); i++)
      {
         if (_stricmp(lp,StandardDataLinks[i])==0) //found it
         {
            pd->DataLinkLayerOptions[i] = 1;    //mark this data link supported
            if (p != NULL)
            {
               *p++ = ':';
               lp = p;                    //point to argument(s)
            }
            switch(i)                     //some of these need extra handling
            {
            case 9:                       //MS/TP master
               dp = &pd->MSTPmasterBaudRates[0];
               goto rdlorates;
            case 10:                      //MS/TP slave
               dp = &pd->MSTPslaveBaudRates[0];
               goto rdlorates;
            case 11:                      //PTP 232
               dp = &pd->PTP232BaudRates[0];
               goto rdlorates;
            case 12:                      //PTP modem, fixed baud rates
               dp = &pd->PTPmodemBaudRates[0];
rdlorates:     got9600 = false;
               for (j=0; j<16; j++)       //read up to 16 baudrates
               {
                  if (*lp == 0) break;    // end of line: baud rates MUST be on the same line.
                  if (ReadDW( &dp[j] )) break;
                  if (dp[j]==9600)
                     got9600 = true;         //remember if we find 9600 baud
                  if (*lp++ != ',') break;   // Can't use NextIs, as it may read another line
               }

               if ((i==9 || i==10) && got9600==false)  //MS/TP must include 9600 baud
               {
                  if (tperror("MS/TP devices must support 9600 baud!",true))
                     return true;
               }
               break;
            case 13:                   //PTP autobaud
               {
                  UINT from, to;
                  if (sscanf( lp, "%u to %u", &from, &to ) == 2)
                  {
                     pd->PTPAutoBaud[0] = from;
                     pd->PTPAutoBaud[1] = to;
                  }
                  else
                  {
                     tperror("Expected Autobaud range 'XX to YY' here",true);
                     return true;
                  }
               }
               break;
            }
            i=0;
            break;
         }
      }

      if (i)
      {
         if (tperror("Unknown Data Link Layer Option",true))
            return true;
      }
   }
   return false;
}

///////////////////////////////////////////////////////////////////////
// Read Special Functionality Options section of a TPI file
//in: ifile    file stream
//    pd       database structure to be filled in from PICS
//out:   true     if cancel selected

bool ReadSpecialFunctionality(PICSdb *pd)
{
   int      i;
   char     *p;
   octet    *wp;
   dword    d;

   ReadNext();                         //point to next token
   if (lp==NULL || *lp++!='{')         //no open token
      return tperror("Expected { here",true);

   // Initialize Special Functionality items
   pd->RouterFunctions = rfNotSupported;       //default is none
   pd->SegmentedRequestWindow = pd->SegmentedResponseWindow = 0;
   pd->MaxAPDUSize = 50;
   pd->BBMD = 0;  // default is no BBMD support
   while (lp!=NULL)
   {
      ReadNext();                         //point to next token
      if (*lp=='}'||lp==NULL) break;      //return, we're done with these
      if ((p=strchr(lp,':'))!=NULL)       //colon in this one
         *p = 0;                          //make it asciz there
      for (i=0;i<(sizeof(SpecialFunctionality)/sizeof(SpecialFunctionality[0]));i++)
      {
         if (_stricmp(lp,SpecialFunctionality[i])==0) //found it
         {
            if (p != NULL)
            {
               *p++ = ':';
               lp = p;                       //point to argument(s)
            }

            switch(i)
            {
            case 0:                       //max apdu size
               if (ReadDW(&d) || (d<50) || (d>1476))
               {
                  if (tperror("APDU sizes must be 50..1476!",true))
                     return true;
               }
               pd->MaxAPDUSize = (word)d;
               break;
            case 1:                       //request window
               wp = &pd->SegmentedRequestWindow;
               goto rsfwin;
            case 2:                       //response window
               wp = &pd->SegmentedResponseWindow;
rsfwin:        if (ReadDW(&d) || (d<1) || (d>127))
               {
                  if (tperror("Window sizes must be 1..127!",true))
                     return true;
               }
               *wp = (octet)d;
               break;
            case 3:                       //Router
               pd->RouterFunctions = rfSupported;
               break;
            case 4:
               pd->BBMD = 1;                //BBMD functionality is supported
               break;
            }
            i=0;
            break;
         }
      }

      if (i)
      {
         if (tperror("Unknown Special Functionality Option",true))
            return true;
      }
   }
   return false;
}

///////////////////////////////////////////////////////////////////////
// Read Default Property Value Restrictions section of a TPI file
//in: ifile    file stream
//    pd       database structure to be filled in from PICS
//out:   true     if cancel selected
bool ReadDefaultPropertyValueRestrictions(PICSdb *)
{
   char     *p;

   ReadNext();                         //point to next token
   if (lp==NULL||*lp++!='{')           //no open token
      return tperror("Expected { here",true);

   // We currently read and IGNORE these...
   while (lp!=NULL)
   {
      ReadNext();
      if (*lp=='}'||lp==NULL) break;
      if ((p=strchr(lp,':'))==NULL){         //colon in this one
         return tperror("Missing colon and restriction value",true);
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////
// Read Fail Times section of a TPI file
//in: ifile    file stream
//    pd       database structure to be filled in from PICS
//out:   true     if cancel selected

bool ReadFailTimes(PICSdb *pd)
{
   int      i;
   char     *p;
   dword    d;
   bool     flag;

   ReadNext();                         //point to next token
   if (lp==NULL||*lp++!='{')           //no open token
      return tperror("Expected { here",true);

   memset(pd->BACnetFailTimes,ftNotSupported,sizeof(pd->BACnetFailTimes)); //default is not supported
   while (lp!=NULL)
   {
      ReadNext();
      if (*lp=='}'||lp==NULL) break;
      if ((p=strchr(lp,':'))==NULL){         //colon in this one
         return tperror("Missing colon and fail time",true);
      }
      *p = 0;                              // TODO: writing into buffer.  fixme
      flag = false;
      for (i=0; i<(sizeof(FailTimes)/sizeof(FailTimes[0])); i++)
      {
         if (_stricmp(lp,FailTimes[i])==0)   //found Fail Time
         {
            *p++ = ':';
            lp = p;                   //point to argument(s)
            if (ReadDW(&d)) return true;
            pd->BACnetFailTimes[i] = d;
            flag = true;
            break;
         }
      }
      if (flag == false) {
         if (tperror("Unknown Fail Time Option",true))
            return true;
      }
   }

   return false;
}

// Set common property flags
void SetPropertyFlags( generic_object *pobj, UINT pindex )
{
   pobj->propflags[pindex] |= PropIsPresent;    //remember we saw this property

   // DO NOT call skipwhitespace, as it may read another line - which may be a new property
   // Of course, this only works if the W and any <restrictions> are ON THE SAME LINE.
   while (*lp == SPACE) lp++;

   if (*lp=='W' || *lp=='w')
   {
      //prop is writeable
      lp += 1;
      pobj->propflags[pindex] |= PropIsWritable;
   }

   // TODO: should we also eat "C" if present?

   // Eat any <restrictions>, assuming they are on a single line.
   // TODO: might be nice to USE these someday...
   // The EPICS view has a column for them.
   while (*lp == SPACE) lp++;
   if (*lp=='<')
   {
      lp += 1;
      while ((*lp != 0) && (*lp++ != '>'))
      {
      }
   }

   while (*lp==SPACE) lp++;                     //skip whitespace, but DO NOT read another line
   if (*lp != 0)
   {
      tperror("Unexpected data after property value",true);
   }
}

///////////////////////////////////////////////////////////////////////
// Read Object Database section of a TPI file
//in: ifile    file stream
//    pd       database structure to be filled in from PICS
//out:   true     if cancel selected

bool ReadObjects(PICSdb *pd)
{
   char  objname[MAX_TEXT_STRING];     //size matches object_name fields
   bool  weKnowObjectType;
   word  objtype;                      //enumeration value for object type
   dword objid = 0;                    //object identifier
   octet fType,fID,fName;
   generic_object *pobj,*po,*polast;   //pointers to objects
   int    i;

   ReadNext();                         //point to next token
   if (lp==NULL || *lp++!='{')         //no open token
      return tperror("Expected { here",true);

   // Loop on objects
   while (lp!=NULL && !s_cancel)
   {
nextobject:
      bNeedReset = false;
      bHasNoneType = false;
      bHasReset = false;

      lp = ReadNext();                 //point to next token
      if (lp == NULL || *lp=='}') break;        //return, we're done with these
      if (*lp)                         //not a blank line
      {
         if (*lp=='{')                 //begin a new object
         {
            weKnowObjectType = false;  //don't know what kind yet
            objtype = 0;               //no object found yet
            objname[0] = 0;
            fType = fID = fName = 0;
            pobj = NULL;

            // Loop on properties
            bool skipToNextProperty = false;
            while (!s_cancel)
            {
               s_error = false;        // set true to abort parsing one property
               lp = ReadNext();        // read a line, point to next token
               if ((lp==NULL) || (*lp=='}')) break;   //done with this object
               if (objtype==0xFFFF)
                  goto nextobject;     //once we find a bogus object type, we skip the rest of the object def

               if (*lp)                //ignore blank lines
               {
                  CString propertyName;
                  if (ReadEnumString( &propertyName ) || !NextIs(':'))
                  {
                     // Line is not property-name:
                     if (skipToNextProperty)
                     {
                        // We are recovering from a parse failure: keep reading
                        tperror("Skipping line, looking for next property",true);
                        continue;
                     }

                     // Not much reason to continue if we are this messed up
                     tperror("Expected property name and colon here!",true);
                     return true;
                  }

                  print_debug("\nProperty %s value %s\n", (LPCTSTR)propertyName, lp);
                  if (weKnowObjectType)
                  {
                     if (propertyName.CompareNoCase("object-type")==0)
                     {
                        word type;
                        if (ReadEnum(&type, &NetworkSniffer::BAC_STRTAB_BACnetObjectType)) return true;
                        if ((objtype < NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nReserved) && (objtype != type))
                        {
                           if (tperror("The object-type property does not agree with the object-identifier!",true))
                              return true;
                           objtype = 0xFFFF;      //pretend objid was bad
                        }
                        if (objtype < MAX_DEFINED_OBJ)
                        {
                           ObjInTestDB[objtype].object_type = objtype;
                           ObjInTestDB[objtype].ObjIDSupported |= soSupported;
                           i = ObjInTestDB[objtype].ObjInstanceNum;
                           ObjInTestDB[objtype].ObjInstanceNum++;
                           ObjInTestDB[objtype].ObjInstance[i] = objid & 0x003fffff;
                        }

                        SetPropertyFlags( pobj, typeProp );
                        continue;
                     }
                     else if (propertyName.CompareNoCase("object-identifier")==0)
                     {
                        if (ReadObjectID( &objid ) || (objtype != (word)(objid>>22)))
                        {
                           objtype = 0xFFFF;      //pretend objid was bad
                           if (tperror("The object-identifier property does not agree with the object-identifier!",true))
                              return true;
                        }
                        if (objtype < MAX_DEFINED_OBJ)
                        {
                           ObjInTestDB[objtype].object_type = objtype;
                           ObjInTestDB[objtype].ObjIDSupported |= soSupported;
                           i = ObjInTestDB[objtype].ObjInstanceNum;
                           ObjInTestDB[objtype].ObjInstanceNum++;
                           ObjInTestDB[objtype].ObjInstance[i] = objid & 0x003fffff;
                        }

                        SetPropertyFlags( pobj, idProp );
                        continue;
                     }

                     // ParseProperty returns true for a parse error, which causes us to
                     // skip to the nex line with a property identifier.
                     // If cancel was indicated, exit here.
                     skipToNextProperty = ParseProperty(propertyName, pobj, objtype);
                     if (s_cancel) return true;

                     if (bNeedReset & bHasNoneType)
                     {
                        // propertyName contained information to help us re-parse
                        // the most recent property with "none" parseType.
                        // Reset the value pointer, and pray it is a one-line value...
                        lp = &NoneTypeValue[0];
                        ParseProperty(NoneTypePropName, pobj, objtype);
                        if (s_cancel) return true;
                     }
                  }
                  else                    //don't know what kind of object this is yet
                  {
                     if (propertyName.CompareNoCase("object-type")==0)
                     {
                        if (ReadEnum(&objtype, &NetworkSniffer::BAC_STRTAB_BACnetObjectType)) return true;
                        weKnowObjectType = true;
                        objid = ((dword)objtype)<<22;

                        fType |= PropIsPresent;             //remember we have the type
                        while (*lp==SPACE||*lp==',') lp++;  //skip any more whitespace or commas
                        if (*lp=='W' || *lp=='w')           //prop is writeable
                           fType |= PropIsWritable;
                     }
                     else if (propertyName.CompareNoCase("object-identifier")==0)
                     {
                        if (ReadObjectID( &objid ) || (objid == BAD_OBJID))
                        {
                           objtype = 0xFFFF;                   //object identifier was bogus
                        }
                        else
                        {
                           weKnowObjectType = true;
                           objtype = (word)(objid>>22);

                           fID |= PropIsPresent;               //remember we had the id
                           while (*lp==SPACE||*lp==',') lp++;  //skip any more whitespace or commas
                           if (*lp=='W'||*lp=='w')             //prop is writeable
                              fID |= PropIsWritable;
                        }
                     }
                     else if (propertyName.CompareNoCase("object-name")==0)
                     {
                        if (setstring(objname,sizeof(objname),lp)) return true;

                        fName |= PropIsPresent;                //remember we have the name
                        while (*lp==SPACE||*lp==',') lp++;     //skip any more whitespace or commas
                        if (*lp=='W'||*lp=='w')                //prop is writeable
                           fName |= PropIsWritable;
                     }
                     else
                     {
                        tperror("Must identify the object-identifier or object-type before defining this property!",true);
                        return true;
                     }
                     if (weKnowObjectType)      //just found out what type it is
                     {
//                      if (objtype >= etObjectTypes.propes)   //this is a proprietary object type
//                      {
//                         tperror("Sorry, this version does not support Proprietary Objects in TextPICS!",true);
//                         objtype=0xFFFF;      //pretend objid was bad
//                         continue;
//                      }

                        if (objtype >= NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nReserved)
                        {
                           if ((pobj=(generic_object *)malloc(sizeof(proprietary_obj_type)))==NULL)      //can't allocate space for it
                           {
                              tperror("Can't allocate space for this object...",true);
                              objtype=0xFFFF;      //pretend objid was bad
                              continue;
                           }
                           memset(pobj, 0, sizeof(proprietary_obj_type));  //zero it out first
                           pobj->next = NULL;
                           pobj->object_id = objid;
                           pobj->object_type = objtype;
                           strncpy(pobj->object_name, objname, sizeof(pobj->object_name));

                           pobj->propflags[typeProp] |= fType;   //found type
                           pobj->propflags[idProp]   |= fID;     //found id
                           pobj->propflags[nameProp] |= fName;   //found name
                           continue;
                        }

                        if ((pobj=(generic_object *)malloc(StdObjects[objtype].sotSize))==NULL)    //can't allocate space for it
                        {
                           tperror("Can't allocate space for this object...",true);
                           objtype = 0xFFFF;      //pretend objid was bad
                           continue;
                        }

                        memset(pobj,0,StdObjects[objtype].sotSize);  //zero it out first
                        pobj->next = NULL;
                        pobj->object_id = objid;
                        pobj->object_type = objtype;
                        strncpy(pobj->object_name, objname, sizeof(pobj->object_name));
                        pobj->propflags[typeProp] |= fType;   //found type
                        pobj->propflags[idProp]   |= fID;     //found id
                        pobj->propflags[nameProp] |= fName;   //found name

                        // remember pointer to Device Object for consistency checks
                        if (objtype == OBJ_DEVICE)
                           pd->pDeviceObject = (device_obj_type *)pobj;
                     }
                  }
               }
            }           // End loop on properties

            //here we've found the end of an object definition
            if (pobj)
            {
               if (pobj->object_name[0]==0) //make sure object is named
               {
                  tperror("Object must have a name...",true);
                  objtype = 0xFFFF;         //pretend objid was bad
               }
               if (objtype == 0xFFFF)    //something was wrong with it
               {
                  free(pobj);          //toss allocated object
                  return true;         //fail
               }
               po=pd->Database;        //check for uniqueness of objid
               polast = NULL;
               while (po!=NULL)
               {
                  if (objid==po->object_id)  //oops, we already have this one!
                  {
                     tperror("Object Identifier is not unique!",true);
                     free(pobj);    //toss allocated object
                     return true;      //fail
                  }
                  polast = po;  // msdanner 9/2004 - remember the last object in the list
                  po=(generic_object *)po->next;         //try next one
               }
               // msdanner 9/2004 - add new objects to the end instead of the beginning
               //pobj->next=pd->Database; //link it in
               //pd->Database=pobj;
               if (polast)
                  polast->next = pobj;  // add new object after the last one
               else
                  pd->Database = pobj;  // This is the first object, so set head pointer.
            }
         }
         else                       //anything else is junk
         {
            if (tperror("Expected '{' to begin an object definition here!",true))
               return true;
         }
      }
   }
   return false;
}

///////////////////////////////////////////////////////////////////////
// parse a property value
//in: pn    points to property name string
//    pobj  points to an object structure of type objtype
//    objtype  is the object type
//
//out:   true  if an error occurred while parsing the value
//
bool ParseProperty(const char *pn, generic_object *pobj, word objtype)
{
   propdescriptor *pd,*newpd;
   word        pindex,ub,i;
   void        *pstruc;
   dword       dw;
   char        b[512];
   const NetworkSniffer::BACnetStringTable *ep;
   float       *fp;
   word        *wp;
   char        **cp;
   octet       oParseType;
   word        parseET;
   BACnetActionCommand **acp;
   void        *pvalue_type;
   int         iVal;

   // Get the property ID for the specified name
   int propID = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumValue( pn );
   if (propID < 0)
   {
      if (tperror("Unknown property identifier",true)) return true;
   }

   pd = GetPropDescriptorTable( objtype );
   pindex = 0;
   while (true)
   {
      if (pd->PropID == propID)  //found this property
      {
         print_debug("ParseProperty: '%s' objtype %u parseType=%u\n",pn,objtype,pd->ParseType);
         pstruc = (octet *)pobj + pd->StrucOffset;  //make pointer to where the value is stored

         //Save the property name which parse type is none.
         if (pd->ParseType == none)
         {
            print_debug("NONE: Saving %s as NoneTypePropName\n",pn);
            strncpy(NoneTypePropName,pn,sizeof(NoneTypePropName));
         }

         // 135.1 4.5(c) says that '?' means "value unspecified", but this
         // code formerly accepted both '?' and '*'.
         // But '*' for Date and Time may indicate a partial or complete wildcard
         // or FIELD unspecified, which is distinct from "value unspecified",
         // and that should be parsed.
         skipwhitespace();             //point to where the value should be
         if (*lp == '?')
         {
            //property value is unspecified
            pobj->propflags[pindex] |= ValueUnknown; //we don't know what the value is
            if (pd->ParseType == none)
            {
               print_debug("NONE: Saving value %s\n",lp);
               strncpy( NoneTypeValue, lp, sizeof(NoneTypeValue) );   // store the value of property with ParseType none
            }
            lp++;                   //skip ?
         }
         else                       //has a property value
         {
            // Copy parseType and ET to avoid writing into the property table
            oParseType = pd->ParseType;
            parseET    = pd->PropET;
            if (bNeedReset & bHasNoneType)
            {
               print_debug("NONE: changing parsetype from %u to %u\n", oParseType, newParseType );
               oParseType = newParseType;
               parseET = newPropET;
               bHasReset = true;
            }

            switch (oParseType)
            {
            case evparm:               //event parameter
               if (ParseEventParameter((BACnetEventParameter *)pstruc)) return true;
               break;
            case setref:               //setpoint reference
               // BACnetSetpointReference, which is an OPTIONAL BACnetObjectPropertyReference.
               // ParseReference will return NULL on error, but also for an intentional
               // "no reference".  So we check the error flag
               *(BACnetObjectPropertyReference **)pstruc = ParseReference(NULL);
               if (s_error) return true;
               break;
            case propref:              //obj property reference
               ParseReference((BACnetObjectPropertyReference *)pstruc);
               break;
            case devobjpropref:        //device obj property reference
               ParseDevObjPropReference((BACnetDeviceObjectPropertyReference *)pstruc);
               break;
            case lodoref:
               ParseDevObjList((BACnetDeviceObjectReference **)pstruc, (pd->PropFlags & IsArray) != 0);
               break;
            case recip:                //recipient
               ParseRecipient((BACnetRecipient *)pstruc);
               break;
            case skeys:                //session keys
               if (ParseSessionKeyList((BACnetSessionKey **)pstruc)) return true;
               break;
            case wsched:               //weekly schedule: array[7] of list of timevalue
               if (ParseWeekdaySchedule((BACnetTimeValue **)pstruc)) return true;
               break;
            case xsched:               //exception schedule: array[] of specialevent
               if (ParseExceptionSchedule((BACnetExceptionSchedule *)pstruc)) return true;
               break;
            case reciplist:            //list of BACnetDestination
               if (ParseDestinationList((BACnetDestination **)pstruc)) return true;
               break;
            case tsrecip:              //time synch recipients
               if (ParseRecipientList((BACnetRecipient **)pstruc)) return true;
               break;
            case dabind:               //device address bindings
               if (ParseAddressList((BACnetAddressBinding **)pstruc)) return true;
               break;
            case raslist:              //read address specifications
               if (ParseRASlist((BACnetReadAccessSpecification **)pstruc)) return true;
               break;
            case calist:               //list of calendarentry
               if (ParseCalist((BACnetCalendarEntry **)pstruc)) return true;
               break;
            case looref:               //list of object ids
               if (ParseObjectList(((BACnetObjectIdentifier **)pstruc), &((device_obj_type *)pobj)->num_objects)) return true;
               break;
            case lopref:               //list of DEVICE objectpropertyreferences
               if (ParseRefList((BACnetDeviceObjectPropertyReference **)pstruc)) return true;

               //Add for resetting value of present-value,
               if (objtype == OBJ_SCHEDULE)
               {
                  // Schedule object: reset value_type to referenced property's type
                  BACnetDeviceObjectPropertyReference *temp;
                  temp = *(BACnetDeviceObjectPropertyReference **)pstruc;
                  if (temp==NULL) break;
                  dword refobjid = temp->Objid;
                  dword refpropid = temp->wPropertyid;
                  newpd = GetPropDescriptorTable(refobjid>>22); //point to property descriptor table for this object type
                  do
                  {
                     if (refpropid == newpd->PropID)
                     {
                        newPropET = newpd->PropET;
                        newParseType = newpd->ParseType;

                        pvalue_type = (octet*)pobj + (size_t)((char*)&(((schedule_obj_type*)0)->value_type) - (char*)0 );
                        *(dword*)pvalue_type = newParseType;

                        bNeedReset = true;
                        break;
                     }
                     if (newpd->PropGroup & LAST)
                        return tperror("Can't determine datatype of schedule's property reference",true);
                     newpd++;                         //advance to next table entry
                  } while(true);
               }
               break;
            case vtcl:                 //vt classes
               if (ParseVTClassList((BACnetVTClassList **)pstruc)) return true;
               break;
            case vtse:                 // list of active  vt sessions
               if (ParseVTSessionList((BACnetVTSession **)pstruc)) return true;
               break;
            case pss:                  //protocol services supported bitstring
               // Handles bitstrings up to MAX_BITSTRING,
               // regardless of the number of defined protocol services
               if (ParseBitstring((octet *)pstruc,
                                  MAX_BITSTRING,                                   /* max bits to parse */
                                  &EPICSLengthProtocolServicesSupportedBitstring)) /* how many bits were parsed? */
                   return true;
//             ProtocolServSup.ObjServNum=sizeof(StandardServices)/sizeof(StandardServices[0]);
//             ProtocolServSup.PropSupValue=(octet *)pstruc;
               break;
            case pos:                  //protocol objects supported bitstring
               // Handles bitstrings up to MAX_BITSTRING,
               // regardless of the number of defined protocol object types
               if (ParseBitstring((octet *)pstruc,
                                  MAX_BITSTRING,                                      /* max bits to parse */
                                  &EPICSLengthProtocolObjectTypesSupportedBitstring)) /* how many bits were parsed? */
                   return true;
//             ProtocolObjSup.PropSupValue=(octet *)pstruc;
//             ProtocolObjSup.ObjServNum=sizeof(StandardObjects)/sizeof(StandardObjects[0]);
               break;
            case dt:                //date/time
               if (ParseDateTime((BACnetDateTime *)pstruc)) return true;
               break;
            case dtrange:           //daterange
               if (ParseDateRange((BACnetDateRange *)pstruc)) return true;
               break;
            case ddate:
               if (ParseDate((BACnetDate *)pstruc)) return true;
               break;
            case ttime:
               if (ParseTime((BACnetTime *)pstruc)) return true;
               break;
            case act:               //action array
               {
                  acp = (BACnetActionCommand **)pstruc;
                  // Here, the PropET table property gives the number of array entries
                  assert( parseET > 0 );
                  for (i=0; i<parseET; i++)
                     acp[i] = NULL;   //init all slots to NULL values

                  i = 0;
                  EpicsListOf actionArray;
                  while (actionArray.HasMore() && (i < parseET))
                  {
                     if ((acp[i]=ReadActionCommands())==NULL) return true; //failed for some reason
                     i++;
                  }
                  ((command_obj_type *)pobj)->num_actions = i;
               }
               break;
            case eboollist:         // list of booleans
               if (ParseBooleanList( (BooleanList **)pstruc )) return true;
               break;
            case stavals:           // list of unsigned
               if (ParseUnsignedList( (UnsignedList **)pstruc )) return true;
               break;
            case statext:           //state text array
            case actext:            //action_text array
               {
                  // Here, the PropET table property gives the number of array entries
                  cp = (char **)pstruc;
                  assert( parseET > 0 );
                  for (i = 0; i < parseET; i++)
                     cp[i] = NULL; //init all slots to NULL values

                  i = 0;
                  EpicsListOf actionArray;
                  while (actionArray.HasMore() && (i < parseET))
                  {
                     if (setstring(b,sizeof(b),lp)) return true;   //put string in buffer b
                     if (b[0])            //if string isn't null
                     {
                        ub=strlen(b)+1;   //reqd length
                        if ((cp[i] = (char*)malloc(ub)) == NULL)
                        {
                           tperror("Can't Get String Space!",true);
                           return true;
                        }
                        strcpy(cp[i], b);  //copy it
                     }
                     i++;
                  }
               }
               break;
            case pae:                  // priority array enums
            case pab:                  // priority array bpv
               {
                  wp = (word *)pstruc;
                  for (i=0; i<16 ;i++)
                     wp[i] = bpaNULL; //init all slots to NULL values

                  i = 0;
                  EpicsListOf actionArray;
                  while (actionArray.HasMore() && (i <16))
                  {
                     // This assumes that the string table contains a value for NULL
                     // There will generally be two versions of such string tables: with and without null.
                     ep = NetworkSniffer::GetEnumStringTable(parseET); //point to enumeration table for this guy
                     if (ReadEnum(&wp[i], ep)) return true;
                     i++;
                  }
               }
               break;
            case pau:                  //priority array uw
               {
                  wp = (word *)pstruc;
                  for (i=0; i<16 ;i++) 
                     wp[i] = upaNULL; //init all slots to NULL values

                  i = 0;
                  EpicsListOf actionArray;
                  while (actionArray.HasMore() && (i <16))
                  {
                     skipwhitespace();
                     if (*lp=='n'||*lp=='N') //we'll assume he means 'NULL'
                     {
                        wp[i] = upaNULL;
                        SkipAlphaString();
                     }
                     else              //we'll assume there's a number here
                     {
                        if (ReadW( &wp[i] )) return true;
                     }
                     i++;
                  }
               }
               break;

            case ptPai:                //priority array int32
               {
                  int *pInt = (int*)pstruc;
                  for (i=0; i<16 ;i++)
                     pInt[i] = ipaNULL; //init all slots to NULL values.

                  i = 0;
                  EpicsListOf actionArray;
                  while (actionArray.HasMore() && (i <16))
                  {
                     skipwhitespace();
                     if (*lp=='n'||*lp=='N') //we'll assume he means 'NULL'
                     {
                        pInt[i] = ipaNULL;
                        SkipAlphaString();
                     }
                     else              //we'll assume there's a number here
                     {
                        if (ReadSigned( &pInt[i] )) return true;
                     }
                     i++;
                  }
               }
               break;

            case paf:                  //priority array flt
               {
                  fp = (float *)pstruc;
                  for (i=0; i<16 ;i++)
                     fp[i] = fpaNULL; //init all slots to NULL values

                  i = 0;
                  EpicsListOf actionArray;
                  while (actionArray.HasMore() && (i <16))
                  {
                     skipwhitespace();
                     if (*lp=='n' || *lp=='N') //we'll assume he means 'NULL'
                     {
                        fp[i] = fpaNULL;
                        SkipAlphaString();
                     }
                     else
                     {
                        //we'll assume there's a number here
                        if (ReadFloat( &fp[i] )) return true;
                     }
                     i++;
                  }
               }
               break;
            case ob_id:                //an object identifier
               if (ReadObjectID( (dword*)pstruc )) return true;
               break;
            case ebool:                //boolean value
               if (ReadBool( (bool*)pstruc )) return true;
               break;
            case etl:
               ep = NetworkSniffer::GetEnumStringTable(parseET);
               if (ParseEnumList((BACnetEnumList **)pstruc, ep )) return true;
               break;
            case PT_PROPLIST:
               // Almost "etl", except we don't have an appropriate property table
               if (ParsePropertyList((BACnetEnumList **)pstruc)) return true;
               break;
            case et:                //an enumeration table
               // TODO: Note that some enumeration members are defined in Stdobj.j as word,
               // but some are defined in stdobj.h AS enums, such as BACnetEventState, BACnetReliability etc.
               // and these will have size FOUR BYTES on modern x86.
               // This code works, so long as our processor is little-endian and the struct
               // is cleared to zero.  But the tables should be changed to "word", or else
               // to real enumerations and dword.
               ep = NetworkSniffer::GetEnumStringTable(parseET);  //point to enumeration table for this guy
               if (ReadEnum( (word*)pstruc, ep )) return true;
               break;
            case bits:                 //octet(s) of T or F flags
               // Number of bits is specified by the enumeration table's element count
               ep = NetworkSniffer::GetEnumStringTable(parseET);  //point to enumeration table for this guy
               if (ParseBitstring( (octet*)pstruc, ep->m_nStrings, NULL )) return true;
               break;
            case sw:                   //signed word
               // "sw" isn't used by any of the property tables
               if (ReadSigned( &iVal )) return true;
               *(short int *)pstruc = iVal;
               break;
            case ssint:                // signed int
               // TODO: PROPS.H says ssint is "short int"
               // "ssint" used in the property tables by UTC_OFFSET, stored as int
               if (ReadSigned( &iVal )) return true;
               *(int *)pstruc = iVal;
               break;
            case ptInt32:              // signed int
               if (ReadSigned( &iVal )) return true;
               *(int *)pstruc = iVal;
               break;
            case flt:                  //float (
               if (ReadFloat( (float *)pstruc )) return true;
               break;
            case uwarr:             // Array of three word: TODO: or put the size in the property table?  Useful anywhere else?
               {
                  wp = (word *)pstruc;
                  for (i=0; i<3 ;i++)
                     wp[i] = 0;

                  i = 0;
                  EpicsListOf actionArray;
                  while (actionArray.HasMore() && (i < 3))
                  {
                     if (ReadW( &wp[i] ))
                        return true;
                     i++;
                  }
               }
               break;
            case uw:                   //unsigned word
               if (ReadW( (word *)pstruc ))
                  return true;
               break;
            case ud:                   //unsigned dword
               if (ReadDW( (dword *)pstruc ))
                  return true;
               break;
            case u16:                  //1..16
               ub=16;
               goto ppub;
            case u127:                 //1..127
               ub=127;
   ppub:       if (ReadDW( &dw ))
                  return true;
               if (dw==0 || dw>ub)     //it's out of range
               {
                  sprintf(b,"Must use an Unsigned Integer 1..%u here!",ub);
                  if (tperror(b,true)) return true;
                  dw=(dword) ub;       //stick at upper bound
               }
               *(word *)pstruc=(word)dw;
               break;
            case ch_string:            //char [132MAX_TEXT_STRING
               if (setstring((char *)pstruc,MAX_TEXT_STRING,lp)) return true;
               break;

// TODO: Implement me
// On the other hand, when would this NOT be "?" in an EPICS, since
// Trends are likely to acquire additional records over time, etc.
// Since log records can't be read by ReadProperty, only by ReadRange,
// the check-against-EPICS wouldn't be done even if we DID decode this
//          case LOGREC:
//             if (ParseLogRec((BACnetLogRecord *)pstruc)) return true;
//             break;
            case TSTMP:
               if (ParseTimeStamp((BACnetTimeStamp *)pstruc)) return true;
               break;
            case TSTMParr:
               if (ParseTimeStampArray((BACnetTimeStamp **)pstruc, 3)) return true;
               break;
            case lCOVSub:     //List of BACnetCOVSubcription
               if (ParseCOVSubList((BACnetCOVSubscription **)pstruc)) return true;
               break;
            case escale:
               {
                  // BACnetScale is a CHOICE
                  //   floatScale     [0] REAL,
                  //   integerScale   [1] INTEGER
                  BACnetScale* pscale = (BACnetScale *)pstruc;
                  if (ReadChoiceTag( &pscale->choice, 1 )) return true;
                  float val;
                  if (ReadFloat(&val)) return true;
                  switch (pscale->choice)
                  {
                  case 0:
                     pscale->choice = 0;
                     pscale->u.floatScale = val;
                     break;
                  case 1:
                     pscale->choice = 1;
                     pscale->u.integerScale = (int)(val + 0.5);
                     break;
                  default:
                     // If you need to specify floating 2.0, use a tag
                     if (val > (float)((int)val))
                     {
                        pscale->choice = 0;
                        pscale->u.floatScale = val;
                     }
                     else
                     {
                        pscale->choice = 1;
                        pscale->u.integerScale = (int)(val + 0.5);
                     }
                     break;
                  }
               }
               break;
            case eprescl: //BACnetPrescale
               if (ParsePrescale((BACnetPrescale *)pstruc))
                  return true;
               break;
            case eaclr: //BACnetAccumulatorRecord
               if (ParseAccumulatorRecord((BACnetAccumulatorRecord *)pstruc))
                  return true;
               break;
            case none:
               // Hack to reset the datatype of present value based on the
               // value of another property.  Used by the Schedule object,
               // where the datatype of the first object-property-reference
               // is used to set the datatype of present-value and schedule-default.
               // Of course, it SHOULD also set the type of values within
               // weekly-schedule and exception-schedule, but...
               print_debug("NONE: Setting NoneTypeValue %s\n",lp);
               bHasNoneType = true;
               // Save the value for later.  CAUTION: assumes value is complete on one line.
               for (i = 0; ;i++)
               {
                  NoneTypeValue[i] = *lp;
                  if (*lp==0) break;
                  lp++;
               }
               break;
            case shedlevel:
               {
                  // BACnetShedLevel is a CHOICE of
                  //   percent  [0] Unsigned,
                  //   level    [1] Unsigned,
                  //   amount   [2] REAL
                  BACnetShedLevel* pshedlevel = (BACnetShedLevel *)pstruc;
                  if (ReadChoiceTag( &pshedlevel->choice, 2 )) return true;
                  float val;
                  if (ReadFloat(&val)) return true;
                  switch (pshedlevel->choice)
                  {
                  case 0:
                  case 1:
                     pshedlevel->sl.uproperty_value = (int)(val + 0.5);
                     break;
                  case 2:
                     pshedlevel->sl.fproperty_value = val;
                     break;
                  default:
                     // If you need to specify floating 2.0, use a tag
                     if (val > (float)((int)val))
                     {
                        pshedlevel->choice = 0;
                        pshedlevel->sl.uproperty_value = (int)(val + 0.5);
                     }
                     else
                     {
                        pshedlevel->choice = 2;
                        pshedlevel->sl.fproperty_value = val;
                     }
                     break;
                  }
               }
               break;
            default:
               // No handler for this ParseType
               return tperror( "This version of VTS has no decoder for this datatype", true );
               break;
            }

            if (bHasReset)
            {
               print_debug("NONE: resetting none flags\n");
               bHasReset = false;
               bNeedReset = false;
               bHasNoneType =false;
            }
         }

         SetPropertyFlags( pobj, pindex );
         return s_error;                 //we're done parsing.  Return the parse results
      }

      if (pd->PropGroup & LAST)
      {
         print_debug("ParseProperty: unknown property'%s' objtype %d\n",pn,objtype);
         if (objtype < NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nReserved)
         {
            if ((_strnicmp(pn, "vendor", 6) == 0) || (_strnicmp(pn, "proprietary", 11) == 0 ))
            {
               // Proprietary property: return true to skip parsing the value.
               Message( "Skipping proprietary property", true );
               return true;
            }
            else
            {
               return tperror( "Invalid or unknown property", true );
            }
         }
         else
         {
            // Ignore unknown property of proprietary objects
            Message( "Skipping property of proprietary object", true );
            return true;
         }
      }
      pd++;                         //advance to next table entry
      pindex++;
   }
}

///////////////////////////////////////////////////////////////////////
// read a BACnetEventParameter from the buffer lp points to
//
//note: (bitstring) may be B'101010' or (T,F...)
//
// (timedelay,(bitmask),(bitstring),(bitstring)... or
// (timedelay,state,state,state...) or
// (timedelay,(bitstring)) or (timedelay,refpropincrement) or
// (timedelay,((objtype,instance),propname)) or
// (timedelay,((objtype,instance),propname),lowdiff,hidiff,deadband) or
// (timedelay,low,hi,deadband)
//in: lp    points to current position in buffer lb
//    evp      points to the BACnetEventParameter to be filled in
//    Note: evp->event_type MUST be initialized before calling this function!
//out:   true  if an error occurred
//    lp    points past the delimiter )

bool ParseEventParameter(BACnetEventParameter *evp)
{
   // We assume that type was filled in by a previous EPICS line specifying
   // event_type property.  Or we COULD allow the tag to override the event_type
   //
   // Optional tag must match the event type
   octet eventTypeTag;
   if (ReadChoiceTag( &eventTypeTag, 20 )) return true;
   if ((eventTypeTag != 0xFF) && (eventTypeTag != evp->event_type))
   {
      return tperror( "Context tag must match the event-type", true );
   }
   eventTypeTag = evp->event_type;

   EpicsSequence eventParm;
   if (eventParm.Begin()) return true;

   switch (eventTypeTag)
   {
   case CHANGE_OF_BITSTRING:                 //0
      {
         if (ReadW( &evp->time_delay, ',' )) return true;
         if (ParseBitstring(&evp->bitmask.bitstring_value[0],
                            sizeof(evp->bitmask.bitstring_value)*8,
                            &evp->bitmask.bitstring_length)) return true;
         if (MustBe(',')) return true;

         BACnetListBitstringValue   **ppbv = &evp->list_bitstring_value, *qbv = NULL;
         evp->list_bitstring_value = NULL;   //no bitstrings initially
         EpicsListOf bitstrings;             // list-of-bitstring-values
         while (bitstrings.HasMore())
         {
            //here we have (bitstring)...
            if ((qbv=(BACnetListBitstringValue*)malloc(sizeof(BACnetListBitstringValue)))==NULL)
            {
               tperror("Can't Get Object Space!",true);
               break;
            }

            if (ParseBitstring(&qbv->bitstring_value[0], sizeof(qbv->bitstring_value)*8, &qbv->bitstring_length))
               break;

            qbv->next = NULL;                     //link onto the list
            *ppbv = qbv;
            ppbv = &qbv->next;
            qbv = NULL;
         }

         if (qbv!=NULL) free(qbv);
      }
      break;

   case CHANGE_OF_STATE:                  //1
      {
         if (ReadW( &evp->time_delay, ',' )) return true;

         BACnetPropertyStates       **ppv = &evp->list_of_value, *qv = NULL;
         evp->list_of_value = NULL;          //no values initially
         EpicsListOf states;                 // list of state values
         while (states.HasMore())
         {
            // TODO: coupla problems here
            // - 135.1 says enums should be STRINGS, not integers
            // - BACnetPropertyStates is a massive CHOICE, so we would need to use [tag] to tell WHICH enumeration table
            // We might use the undefined:123 syntax used elsewhere
            word val;
            if (ReadW( &val )) break;

            if ((qv=(tagPropertyStates *)malloc(sizeof(BACnetPropertyStates)))==NULL)
            {
               tperror("Can't Get Object Space!",true);
               break;
            }

            qv->enum_value = val;
            qv->next = NULL;                //link onto the list
            *ppv = qv;
            ppv = &qv->next;
            qv = NULL;
         }
      }
      break;

   case CHANGE_OF_VALUE:                  //2
      if (ReadW( &evp->time_delay, ',' )) return true;
      // TODO: 135.1 says we should have CHOICE tag [0] for bitstring, 1 for REAL
      // 135.1 says bitstrings are in {}, not ().  We accept both for historical reasons
      skipwhitespace();
      if (*lp=='(' || *lp=='{' || *lp=='B' || *lp=='b')      //cov-criteria is a bitmask
      {
         // [0] bitstring
         evp->use_property_increment=false;
         if (ParseBitstring(&evp->bitmask.bitstring_value[0],
                            sizeof(evp->bitmask.bitstring_value)*8,
                            &evp->bitmask.bitstring_length)) return true;
      }
      else                          //cov-criteria is a property-increment
      {
         // [1] REAL
         evp->use_property_increment=true;
         if (ReadFloat( &evp->ref_property_increment )) return true;
      }
      break;

   case COMMAND_FAILURE:                  //3
      if (ReadW( &evp->time_delay, ',' )) return true;
      if (ParseDevObjPropReference(&evp->device_prop_ref) == NULL) return true;
      break;

   case FLOATING_LIMIT:                   //4
      if (ReadW( &evp->time_delay, ',' )) return true;
      if (ParseDevObjPropReference(&evp->device_prop_ref) == NULL) return true;
      if (MustBe(',')) return true;
      if (ReadFloat( &evp->low_diff_limit, ',' )) return true;
      if (ReadFloat( &evp->high_diff_limit, ',')) return true;
      if (ReadFloat( &evp->deadband )) return true;
      break;

   case OUT_OF_RANGE:                     //5
      if (ReadW( &evp->time_delay, ',' )) return true;
      if (ReadFloat( &evp->low_limit, ',')) return true;
      if (ReadFloat( &evp->high_limit, ',')) return true;
      if (ReadFloat( &evp->deadband )) return true;
      break;

   case BUFFER_READY:                  //10
      if (ReadDW( &evp->notification_threshold, ',' )) return true;
      if (ReadDW( &evp->previous_notification_count )) return true;
      break;

   case CHANGE_OF_LIFE_SAFETY:
   case EXTENDED:
   default:
      // TODO: implement me (and various other event types)
      tperror("Event parameter choice has not been implemented!",true);
      return true;
   }

   return eventParm.End();
}

// Return false for good parse, true for error
// Assumes either
// - "property-identifier" if no index is specifed (i.e., not an array, or "all elements)
// - "property-identifier,index" if an index is specified, or
// - "property-identifier,delim" if no index is specifed (skipped) in a larger sequence
// See the NOTE at the top of this file for a discussion of this encoding, with
// the comma REQUIRED to encode the presence or absence of the optional index
//
// If ppPropDesc is provided, it is set to point at the propdescriptor for the property.
// If no property is found, ppPropDesc will be NULL and the function will return true
//
// Called from
// - ParseRASlist (BACnetReadAccessSpecifications) - all-elements is quite legal
// - ParseReference (BACnetObjectPropertyReference) - all-elements is unlikely
// - ParseDevObjPropReference (BACnetDeviceObjectPropertyReference) - all-elements is unlikely
// - ReadActionCommands (BACnetActionCommand) - all-elements is unlikely
//
bool validatePropertyNameAndIndexCode(dword dw,
                                      unsigned long *propId,
                                      unsigned short *index,
                                      const propdescriptor **ppPropDesc = NULL)
{
   // Get the property name
   CString name;
   if (ReadEnumString( &name )) return true;

   // Get the property ID for the specified name
   int propID = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumValue( name );
   if (propID < 0)
   {
      if (tperror("Unknown property identifier",true)) return true;
   }

   *propId = propID;
   bool arrayOK = true;
   if (ppPropDesc != NULL)
   {
      // Caller needs property type info: property MUST be in our table
      *ppPropDesc = NULL;

      const propdescriptor *pd = GetPropDescriptorTable((word)(dw>>22));  //point to property descriptor table for this object type
      while (true)
      {
         if (pd->PropID == propID)  //found this property
         {
            // Return a pointer to the property's data
            *ppPropDesc = pd;
            arrayOK = (pd->PropFlags & IsArray) != 0;
            break;
         }
         if (pd->PropGroup & LAST)
         {
            tperror("Unknown Property",true);
            return true;            // return NOT FOUND
         }
         pd++;                      //advance to next table entry
      }
   }
   else
   {
      // Caller just wants a valid spec.  Don't vet with the property table, else
      // User can't specify a proprietary property, or the "unused" object ID
   }

   *index = NotAnArray;
   if (NextIs(','))
   {
      // Comma must be followed by an index, or by another separator
      // if the index is omitted
      if (IsNextNumber())
      {
         if (ReadW( index )) return NULL;

         // It is (at least in some cases) OK to specify an array without an index
         // to denote the entire array.
         // It is NOT OK to specify an index for a non-array
         if (!arrayOK)
         {
            tperror("Index specified for non-array property",true);
            return NULL;
         }
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////
// read a list BACnetReadAccessSpecifications from the buffer lp points to
//   (((objtype,instance),propid,,propid,arindex),((objtype,instance),propid,,propid,),((objtype,instance),propid,,propid,)...)
//  See the NOTE at the top of this file for a discussion of this encoding, with
//  the comma REQUIRED to encode the presence or absence of the optional index
//in: lp    points to current position in buffer lb
//    rasp  points to a variable which should point to a list of BACnetReadAccessSpecifications
//out:   true  if an error occurred
//    else  *rasp points to a list of BACnetReadAccessSpecifications
//    lp    points past the delimiter )

bool ParseRASlist(BACnetReadAccessSpecification **rasp)
{
   BACnetReadAccessSpecification *pFirstRAS = NULL, **ppRAS = &pFirstRAS, *qRAS = NULL;
   *rasp = NULL;                         //initially there is no list

   BACnetPropertyReference  **ppPR = NULL, *qPR = NULL;
   bool   rasfail = true;

   // List is enclosed by (), but we accept {} as well
   EpicsListOf listOf;
   while (listOf.HasMore())
   {
      //here lp must point to the beginning of a new BACnetReadAccessSpecification
      EpicsSequence ras;
      if (ras.Begin()) goto rasx; // 135.1

      //here we have (objtype,instance),propid,propid...)...
      if ((qRAS=(tagReadAccessSpecification *)malloc(sizeof(BACnetReadAccessSpecification)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         goto rasx;
      }

      if (ReadObjectID( &qRAS->object_id, ',' )) goto rasx;
      qRAS->list_of_prop_ref = NULL;

      // Read the list of properties for this object
      ppPR = &qRAS->list_of_prop_ref;
      EpicsListOf properties;
      while (properties.HasMore())
      {
         if ((qPR = (tagPropertyReference *)malloc(sizeof(BACnetPropertyReference))) == NULL)
         {
            tperror("Can't Get Object Space!",true);
            goto rasx;
         }

         EpicsSequence propSpec;
         if (propSpec.Begin()) goto rasx;
         // validate property name and optional index code here
         if (validatePropertyNameAndIndexCode(qRAS->object_id, &(qPR->property_id), &(qPR->pa_index)))
            goto rasx;  // error end here
         if (propSpec.End()) goto rasx;

         qPR->next = NULL;                   //link property onto the list
         *ppPR = qPR;
         ppPR = &qPR->next;
         qPR = NULL;
      }
      if (ras.End()) goto rasx;

      qRAS->next = NULL;                     //link ReadAccessSpec onto the list
      *ppRAS = qRAS;
      ppRAS = &qRAS->next;
      qRAS = NULL;
   }
   rasfail = false;

rasx:
   *rasp = pFirstRAS;
   if (qRAS!=NULL) free(qRAS);               //don't lose this block!
   if (qPR!=NULL) free(qPR);                 //don't lose this block!
   return rasfail;
}

///////////////////////////////////////////////////////////////////////
// read an array of (BACnetSpecialEvent) from the buffer lp points to
// {((t,v),(t,v)),((t,v),(t,v)),...}
//in: lp    points to current position in buffer lb
//    xp    pointer to exception schedule to be initialized
//out:   true  if an error occurred
//    lp    points past the delimiter ) unless it was the end of the buffer

bool ParseExceptionSchedule(BACnetExceptionSchedule *xp)
{
   BACnetSpecialEvent   *fp = NULL, **pp = &fp, *q = NULL;
   bool retval = false;

   xp->special_event = NULL;                //initially there are no lists
   xp->size = 0;

   // Array is enclosed by {}, but we accept () as well
   EpicsListOf listOf;
   while (listOf.HasMore())
   {
      // here lp must point to a BACnetSpecialEvent in {}, but we accept () as well
      // (dateoption),(timevaluelist),eventpriority),...)
      EpicsSequence special;
      if (special.Begin()) break;

      if ((q=(tagSpecialEvent *)malloc(sizeof(BACnetSpecialEvent)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      // period is a CHOICE of
      //   calendarEntry     [0] BACnetCalendarEntry,
      //   calendarReference [1] BACnetObjectIdentifier
      //
      // BACnetCalendarEntry is a CHOICE of
      //   date        [0] Date,
      //   dateRange   [1] BACnetDateRange,
      //   weekNDay    [2] BACnetWeekNDay
      //
      // 135.1 clause 4 would encode it like this (I think)
      //   exception-schedule: {
      //       {[0][0](MON,23-DEC-1995),                     ({0:0:0.0,unknown:0},{8:0:0.0,unknown:1},{23:59:59.0,unknown:0}), 4},
      //       {[0][1]{(MON,23-DEC-1995),(FRI,3-JAN-1996)},  ({0:0:0.0,unknown:0},{8:0:0.0,unknown:1},{23:59:59.0,unknown:0}), 4},
      //       {[0][2]X'FFFF03',                             ({0:0:0.0,unknown:0},{8:0:0.0,unknown:1},{23:59:59.0,unknown:0}), 4},
      //       {[1](calendar,1),                             ({0:0:0.0,unknown:0},{8:0:0.0,unknown:1},{23:59:59.0,unknown:0}), 4}
      //   }
      //
      // 135.1 Annex A has (omitting day-of-week in Dates, using dash in DateRange, and not showing a WeekNDay)
      //   exception-schedule: {
      //       {(23-November-1995),                              ({0:00,INACTIVE}), 10},
      //       {(calendar, 1),                                   ({0:00,INACTIVE}), 11},
      //       {{(5-March-1996)-(7-March-1996)},                 ({9:00,ACTIVE}, {14:00,INACTIVE}), 6}
      //   }

      // Get optional choice tag for "period"
      octet choiceTag;
      if (ReadChoiceTag( &choiceTag, 1)) break;

      //here, lp points to one of 4 formats:
      // Calendar reference as (Calendar,instance)
      // Date as (Date)
      // DateRange as {(Date),(Date)}
      // weekNDay as X'MWD'
      //
      if ((choiceTag == 1)
            ||
          ((choiceTag == 0xFF) &&
           ((_strnicmp(lp,"(Calendar",8)==0) || (_strnicmp(lp,"( Calendar",9)==0))))     //it's a calendar reference
           // (our pre-parse turns multiple spaces into single, so this hack lets us handle
           // an arbitrary number of spaces between parenthesis and Calendar.)
      {
         q->choice = 3;
         if (ReadObjectID( &q->u.calendar_ref.object_id )) break;
         q->u.calendar_ref.next = NULL;
      }
      else
      {
         // Get optional choice tag for "calendarEntry"
         if (ReadChoiceTag( &choiceTag, 2)) break;

         if (((choiceTag == 2) || (choiceTag == 0xFF)) &&
             (NextIs('X') && (NextIs('`') || NextIs('\''))))
         {
            // WeekNDay as octet string
            q->choice = 2;
            q->u.weekNday.month = DONT_CARE;
            q->u.weekNday.day = DONT_CARE;
            q->u.weekNday.week = DONT_CARE;

            if (cvhex(lp,&q->u.weekNday.month) ||
                cvhex(lp+2,&q->u.weekNday.week) ||
                cvhex(lp+4,&q->u.weekNday.day))
            {
               tperror("Expected 6 hex characters for WeekNDay",true);
               break;
            }
            lp += 6;
            if (MustBe('\'')) break;
         }
         else if (((choiceTag == 1) || (choiceTag == 0xFF)) && PeekNextIs('{'))
         {
            // Assume that {} wraps two Dates of a DateRange (PeekNextIs leaves the "{" at *lp)
            q->choice = 1;
            if (ParseDateRange(&q->u.date_range)) break;
         }
         else if ((choiceTag == 0) || (choiceTag == 0xFF))
         {
            // Assume Date
            q->choice = 0;
            if (ParseDate(&q->u.date)) break;
         }
         else
         {
            tperror("Invalid CHOICE for BACnetCalendarEntry",true);
            break;
         }
      }
      if (MustBe(',')) break;

      if (ParseTVList(&q->list_of_time_values)) break;
      if (MustBe(',')) break;

      if (ReadW( &q->event_priority )) break;
      if (special.End()) break;

      xp->size++;

      q->next = NULL;                    //link onto the list
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   xp->special_event = fp;
   if (q!=NULL)
   {
      // Coming here with pointer implies parse error
      free(q);
      retval = true;
   }
   return retval;
}

///////////////////////////////////////////////////////////////////////
// read an array of (list of BACnetTimeValue) from the buffer lp points to
// {((t,v),(t,v)),((t,v),(t,v)),...}
//in: lp    points to current position in buffer lb
//    wp    an array of pointer variables to be
//          initialized to point to the created lists of BACnetTimeValues
//out:   true  if an error occurred
//    lp    points past the delimiter ) unless it was the end of the buffer

bool ParseWeekdaySchedule(BACnetTimeValue *wp[])
{
   int  i;
   for (i=0; i<7; i++) wp[i]=NULL;          //initially there are no lists

   // List is enclosed by (), but we accept {} as well
   i = 0;
   EpicsListOf listOf;
   while (listOf.HasMore())
   {
      //here lp must point to a list of BACnetTimeValue in the array
      if (i>6)
      {
         tperror("Weekly Schedule cannot contain more than 7 days!",true);
         break;
      }

      //here we have (list of timevalues),...
      if (ParseTVList(&wp[i])) break;
      i++;
   }
   return s_error;
}

///////////////////////////////////////////////////////////////////////
// read a list BACnetTimeValues from the buffer lp points to
// ({time,value},{time,value}...)
//in: lp    points to current position in buffer lb
//    tvp      points to a variable which should point to a list of BACnetTimeValues
//out:   true  if an error occurred
//    else  *tvp points to a list of BACnetTimeValues
//    lp    points past the delimiter ) unless it was the end of the buffer

bool ParseTVList(BACnetTimeValue **tvp)
{
   BACnetTimeValue   **pp = tvp, *q = NULL;
   *tvp = NULL;                          //initially there is no list

   char  *ep;

   // List is enclosed by (), but we accept {} as well
   EpicsListOf listOf;
   while (listOf.HasMore())
   {
      //here lp must point to a BACnetTimeValue in the list
      // SEQUENCE is enclosed by {}, but we accept () as well
      EpicsSequence btv;
      if (btv.Begin()) break;

      //here we have ({time,value},...)
      if ((q=(tagTimeValue *)malloc(sizeof(BACnetTimeValue)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      if (ParseTime(&q->time)) break;
      if (MustBe(',')) break;

      // 135.1 says that primitive CHOICEs are "encoded as described in 4.4".
      // Of course, that means we can't distinguish signed from unsigned integer.
      // Try to guess the value
      // TODO: You can put NULL values in a schedule to do priority release
      //    But that is PER VALUE, NOT based on or determining value_type
      // TODO: this only works if the value is on one line.
      // - Active/Inactive for Binary point
      // - REAL for Analog point
      // - integer, IF we can assume unsigned for Multistate point
      // TODO: What about Boolean?  Could check True/False
      // TODO: What about SIGNED integer?
      //    we might demand a leading + or - on signed values
      // TODO: What about other odd stuff that can technically be scheduled such as character strings?
      // Should we add a ReadAlphaString-style method to parse this primitive ANY?
      if ((ep = strchr(lp, btv.Terminator()))==NULL)
      {
         lp = strchr(lp,0);
         tperror("Expected } at end of {time,value}",true);
         break;
      }
      *ep = 0;                               //make "value" asciz  TODO: writing to text buffer.  fixme

      skipwhitespace();
      if (*lp=='a' || *lp=='A')              //Assume BPV Active
      {
         q->value_type = BPV;
         q->av.bproperty_value = ACTIVE;
         SkipAlphaString();
      }
      else if (*lp=='i' || *lp=='I')         //Assume BPV Inactive
      {
         q->value_type = BPV;
         q->av.bproperty_value = INACTIVE;
         SkipAlphaString();
      }
      else if (strstr( lp, "unknown" ) != NULL)
      {
         // TODO: nice as this is, it isn't 135.1
         // enumerated unknown value
         lp += 8;  // skip the text 'unknown:'
         q->value_type = BPV;
         if (ReadW( &q->av.uproperty_value )) break;
      }
      // TODO: this is the only case that needs the null terminator above
      else if (strchr(lp,'.')==NULL)   //doesn't have a dot, assume unsigned
      {
         q->value_type = UNS;
         if (ReadW( &q->av.uproperty_value )) break;
      }
      else                             //assume float
      {
         q->value_type = FLT;
         if (ReadFloat( &q->av.fproperty_value )) break;
      }

      *ep = btv.Terminator();
      lp = ep;
      if (btv.End()) break;

      q->next = NULL;                    //link onto the list
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   if (q!=NULL)
   {
      free(q);
   }
   return s_error;
}

// isArray argument distinguishes between Lists and Arrays when parsing
// The handling of BACnetListOfDeviceObjectReference[] is implemented the same
// here whether a list or array, so we can get away with this as long as we cover it
// in parsing.
// (This would only be needed if we enforced () for lists and {} for arrays.)
bool ParseDevObjList(BACnetDeviceObjectReference **refp, bool /*isArray*/)
{
   BACnetDeviceObjectReference **pp = refp, *q = NULL;
   *refp = NULL;                         //initially there is no list

   // List is enclosed by (), but we accept {} as well for arrays
   EpicsListOf listOf;
   while (listOf.HasMore())
   {
      //here lp must point to a BACnetObjectPropertyReference in the list
      if ((q = ParseDevObjReference(NULL))==NULL) break;

      q->next = NULL;                    //link onto the list
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   return s_error;
}

///////////////////////////////////////////////////////////////////////
// read a list BACnetObjectPropertyReferences from the buffer lp points to
// (((objtype,instance),propertyname[arrayindex]),((objtype,instance),propertyname[arrayindex])...)
//in: lp    points to current position in buffer lb
//    refp  points to a variable which should point to a list of BACnetObjectPropertyReferences
//out:   true  if an error occurred
//    else  *refp points to a list of BACnetObjectPropertyReferences
//    lp    points past the delimiter ) unless it was the end of the buffer

bool ParseRefList(BACnetDeviceObjectPropertyReference **refp)
{
   BACnetDeviceObjectPropertyReference **pp = refp, *q = NULL;
   *refp = NULL;                         //initially there is no list

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to a BACnetObjectPropertyReference in the list
      if ((q=ParseDevObjPropReference(NULL))==NULL)
      {
         tperror("Invalid List of DevObjPropReferences",true);
         break;
      }

      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   return s_error;
}

///////////////////////////////////////////////////////////////////////
// Read a BACnetObjectPropertyReference from the buffer lp points to
//   {(objtype,instance),propertyname,arrayindex} or
//   {(objtype,instance),propertyname} or
//   {(objtype,instance),propertyname,} or
//   ?
//  See the NOTE at the top of this file for a discussion of this encoding, with
//  the comma REQUIRED to encode the presence or absence of the optional index
//in: lp    points to current position in buffer lb
//    inq   points to a BACnetObjectPropertyReference to be filled in,
//          or NULL for a BACnetSetpointReference which may be unused.
//out:   NULL  if an error occurred
//    else  pointer to newly created BACnetObjectPropertyReference
//    lp    points past the delimiter ) unless it was the end of the buffer

BACnetObjectPropertyReference* ParseReference(BACnetObjectPropertyReference *inq)
{
   BACnetObjectPropertyReference *q = NULL;

   if (inq != NULL)
   {
      // Default value in case we return early for parse error or unspecified reference
      inq->next = NULL;
      inq->object_id = BAD_OBJID;
      inq->property_id = 0xffffffff;
      inq->pa_index = NotAnArray;
   }

   // allow empty value. (ParseProperty does this, but this MAY be useful when the reference
   // is part of a larger element)
   if (NextIs('?')) return NULL;

   // BACnetObjectPropertyReference enclosed in {} or ()
   EpicsSequence opr;
   if (opr.Begin()) return NULL;

   // Empty brackets means "not used"
   if (NextIs( opr.Terminator() )) return NULL;

   // TODO:
   // 135.1 Annex A shows setpoint-reference in a single set of brackets, and
   // doesn't show the "unused" case, although {} would encode it unambiguously
   //     setpoint-reference: {(analog-value,7),present-value}
   // allTestEpics.txt traditionally had double wrapping, which is probably closer to the rules in 135.1 4.4
   //     setpoint-reference: {{(analog-value,1),present-value}}
   //
   // For backward compatibility, we allow, but do not require, an extra { for the reference.
   // This assumes that the objectID is wrapped in () per 135.1, so this code is not () {} agnostic
   bool doubleWrap = ((inq == NULL) && NextIs('{'));

   if (inq==NULL)
   {
      if ((q=(tagObjectPropertyReference *)malloc(sizeof(BACnetObjectPropertyReference)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         return NULL;
      }
   }
   else
   {
      q = inq;
   }

   // initial values
   q->object_id = BAD_OBJID;
   q->property_id = 0xffffffff;
   q->pa_index = NotAnArray;
   q->next = NULL;

   if (ReadObjectID( &q->object_id, ',' )) goto oprfail;

   // validate property name and optional index
   if (validatePropertyNameAndIndexCode( q->object_id, &q->property_id, &q->pa_index ))
      goto oprfail;

   if (doubleWrap)
      if (MustBe('}')) goto oprfail;

   if (opr.End()) goto oprfail;

   return q;

oprfail:
   if (inq == NULL) free(q);
   return NULL;
}

///////////////////////////////////////////////////////////////////////
// Read a BACnetDeviceObjectPropertyReference from the buffer lp points to
//   {(objtype,instance),propertyname,arrayindex,(device,instance)} or
//   {(objtype,instance),propertyname,arrayindex}
//   {(objtype,instance),propertyname,,(device,instance)} or
//   {(objtype,instance),propertyname} or
//   ?
//  See the NOTE at the top of this file for a discussion of this encoding, with
//  the comma REQUIRED to encode the presence or absence of the optional index
//in: lp    points to current position in buffer lb
//    inq      points to a BACnetDeviceObjectPropertyReference to be filled in (or NULL)
//out:   NULL  if an error occurred
//    else  pointer to newly created BACnetObjectPropertyReference
//    lp    points past the delimiter ) unless it was the end of the buffer

BACnetDeviceObjectPropertyReference* ParseDevObjPropReference(BACnetDeviceObjectPropertyReference  *inq)
{
   BACnetDeviceObjectPropertyReference *q = NULL;

   if (inq != NULL)
   {
      inq->next = NULL;
      inq->DeviceObj = BAD_OBJID;
      inq->Objid = BAD_OBJID;
      inq->wPropertyid = 0xffffffff;
      inq->ulIndex = NotAnArray;
   }

   // allow empty value. (ParseProperty does this, but this MAY be useful when the reference
   // is part of a larger element)
   if (NextIs('?')) return NULL;

   // BACnetDeviceObjectPropertyReference enclosed in {} or ()
   EpicsSequence dopr;
   if (dopr.Begin()) return NULL;

   // Empty brackets means "not used"
   if (NextIs( dopr.Terminator() )) return NULL;

   if (inq==NULL)
   {
      if ((q=(BACnetDeviceObjectPropertyReference*)malloc(sizeof(BACnetDeviceObjectPropertyReference)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         return NULL;
      }
   }
   else
   {
      q = inq;
   }

   // initial values
   q->next = NULL;
   q->DeviceObj = BAD_OBJID;
   q->Objid = BAD_OBJID;
   q->wPropertyid = 0xffffffff;
   q->ulIndex = NotAnArray;

   if (ReadObjectID( &q->Objid, ',' )) goto oprfail;

   // validate property name and optional index
   if (validatePropertyNameAndIndexCode(q->Objid, &q->wPropertyid, &q->ulIndex ))
      goto oprfail;

   // Read optional device ObjId
   if (NextIs(',') && dopr.HasOption())
   {
      if (ReadObjectID( &q->DeviceObj)) goto oprfail;
   }

   if (dopr.End()) goto oprfail;
   return q;

oprfail:
   if (inq == NULL) free(q);
   return NULL;
}

////////////////////////////////////////////////////////////////////
BACnetDeviceObjectReference* ParseDevObjReference(BACnetDeviceObjectReference *inq)
{
   BACnetDeviceObjectReference   *q=NULL;
   dword dw;

   if (inq != NULL)
   {
      inq->next = NULL;
      inq->DeviceObj = BAD_OBJID;
      inq->Objid = BAD_OBJID;
   }

   // allow empty value. (ParseProperty does this, but this MAY be useful when the reference
   // is part of a larger element)
   if (NextIs('?')) return NULL;

   // BACnetDeviceObjectReference enclosed in {} or ()
   EpicsSequence dor;
   if (dor.Begin()) return NULL;

   // Empty brackets means "not used"
   if (NextIs( dor.Terminator() )) return NULL;

   if (inq==NULL)
   {
      if ((q=(BACnetDeviceObjectReference *)malloc(sizeof(BACnetDeviceObjectReference)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         return NULL;
      }
   }
   else
   {
      q = inq;
   }

   //initial the Values
   q->next = NULL;
   q->DeviceObj = BAD_OBJID;
   q->Objid = BAD_OBJID;

   if (dor.HasOption())
   {
      // Get DeviceIdentifier (comma would signify its absence)
      if (ReadObjectID(&dw, ',')) goto oprfail;
      if (dw>>22 != OBJ_DEVICE)
      {
         tperror("Must use a Device Object Identifier here!",true);
         goto oprfail;
      }
      q->DeviceObj = dw;
   }

   // Get objectID
   if (ReadObjectID(&q->Objid)) goto oprfail;

   if (dor.End()) goto oprfail;
   return q;

oprfail:
   if (inq == NULL) free(q);
   return NULL;
}

///////////////////////////////////////////////////////////////////////
// read a list BACnetCalendarEntrys from the buffer lp points to
// ((m/d/y dow),(d-m-y dow),(m,wom,dow),(date..date)...)
//  ((dow, d-m-y), (date..date)...) Shiyuan Xiao. According to standard 135.1-2013.
//
//in: lp    points to current position in buffer lb
//    calp  points to a variable which should point to a list of BACnetCalendarEntrys
//out:   true  if an error occurred
//    else  *calp points to a list of BACnetCalendarEntrys
//    lp    points past the closing delimiter

bool ParseCalist(BACnetCalendarEntry **calp)
{
   BACnetCalendarEntry  **pp = calp, *q = NULL;
   *calp = NULL;                         //initially there is no list

   // List is enclosed by (), but we accept {} as well
   EpicsListOf listOf;
   while (listOf.HasMore())
   {
      //Here lp must point to a BACnetCalendarEntry in the list
      // TODO: allTestEpics.tpi wraps each element in {}
      //   but
      // 135.1 4.4.1(b) says CHOICE of context-tagged items should have a tag, but
      // says nothing about extra wrapping (DateRange would be {wrapped} as a SEQUENCE)
      // 135.1 Annex A does NOT have extra wrapping
      //
      // So we ALLOW a {wrap} on WeekNDay and Date, and REQUIRE it on DateRange
      // Can't allow (wrap), since the Date will be ()
      bool wrapped = NextIs( '{' );

      //One of 3 formats:
      // BACnetWeekNDay - month,weekofmonth,dayofweek. but here as octet-string X'xxxxxxxx'
      // DateRange - {Date,Date}
      // Date
      if ((q=(tagCalendarEntry *)malloc(sizeof(BACnetCalendarEntry)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      if (NextIs('X') && ((*lp =='`') || (*lp == '\'')))
      {
         //Octet string: assume WeekNDay
         q->choice = 2;
         lp += 1;
         q->u.weekNday.month = DONT_CARE;
         q->u.weekNday.day = DONT_CARE;
         q->u.weekNday.week = DONT_CARE;

         if (cvhex(lp,&q->u.weekNday.month) ||
             cvhex(lp+2,&q->u.weekNday.week) ||
             cvhex(lp+4,&q->u.weekNday.day))
         {
            tperror("Expected 6 hex characters for WeekNDay",true);
            break;
         }
         lp += 6;
         if (MustBe('\'')) break;
      }
      // Don't EAT the ( - leave it for ParseDate
      else if (PeekNextIs('('))
      {
         // Either date or daterange
         q->choice = 0;                  // assume date for now
         if (ParseDate(&q->u.date)) break;

         // DateRange MUST be wrapped, Date MAY be in traditional format
         if (wrapped && (NextIs(',') || NextIs('-')))
         {
            // Comma (per 135.1 clause 4) or dash (per 135.1 Annex A)
            // means this is a DateRange (two dates)
            q->choice = 1;                  //DateRange
            q->u.date_range.start_date = q->u.date;
            if (ParseDate(&q->u.date_range.end_date)) break;
         }
      }
      else
      {
         // invalid entry
         tperror("Unknown BACnetCalendarEntry CHOICE",true);
         break;
      }

      if (wrapped)
      {
         if (MustBe( '}' )) break;
      }

      q->next = NULL;        //link onto the list
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   if (q!=NULL) free(q);
   return s_error;
}

////////////////////////////////////////////////////////////////////
// This function is called to determine the type of the values in the
// event-time-stamp.
////////////////////////////////////////////////////////////////////
octet whatTimestampChoice()
{
   octet status = 0xFF; // return value - set to be invalid
   int nDigits = 0;

   // time           [0] Time,                   as 08:00:00.00
   // sequenceNumber [1] Unsigned (0..65535),    as 1234
   // dateTime       [2] BACnetDateTime          as {(Wednesday, 18-June-2014), 08:00:00.00)
   //                                            or (9/29/1989, Friday), 01:00:00.0
   char ch;
   UINT ucindex = 0;
   skipwhitespace();
   while ((ch = lp[ucindex++]) != 0)
   {
      if (ch == ':')
      {
         // First delimiter seen is Time, so assume that
         status = 0;
         break;
      }

      if ((ch == '/') || (ch == '-') || (ch == ','))
      {
         // First delimiter seen is Date, so assume DateTime
         status = 2;
         break;
      }

      if (IsDigit(ch))
      {
         // Count digits of a timestamp integer
         nDigits += 1;
      }
      else if (!IsAlpha(ch))
      {
         if (nDigits > 0)
         {
            // Have at least one digit, so assume sequenceNumber
            status = 1;
            break;
         }
      }
   }

   return status;
}

///////////////////////////////////////////////////////////////////
//  This function parses the Time Stamp data presented in the PICS
//  Format: DateTime ,Time & Sequence number -> {(6/21/2001,11:32:43.0),(12:14:01.05),(65535)}
//  DateTime can also be written as: (23-MAR-95, 18:50:21.2)
//  The sequence number may be assigned any value from 0 - 65535 or the default "?".
//
// 135.1 Annex A
//   event-time-stamps: {{(Monday, 1-January-2007),18:50:21.02},
//                       {(*-*-*),*:*:*.*},
//                       {(Monday, 1-January-2007), 18:51:34.0}}
//   last-restore-time: {(29-September-1989),01:00:00.00}
//   minimum-value-timestamp: (16-December-1999,13:15:07.32)
//      Note that the date is missing day-of-week, and time is inside the date's () wrapping
//   Annex A contains no Time or Unsigned timestamp CHOICE.
//
// allTestEpics.txt
//   Event-Time-Stamps: {{13},{(12-JUL-01,2),18:50:21.2},{19:01:34.0}}
//      note wrapping of UINT and Time
//   last-restore-time: (9/29/1989,Friday), 01:00:00.0
//      note slashes in date, trailing day and lack of {} wrapping
//
// in:   lp    points to current position in buffer lb
//       inq      points to a BACnetObjectPropertyReference to be filled in (or NULL)
// out:  NULL  if an error occurred
//       else  pointer to newly created BACnetObjectPropertyReference
//       lp    points past the delimiter ) unless it was the end of the buffer
///////////////////////////////////////////////////////////////
bool ParseTimeStamp(BACnetTimeStamp *ptstamp)
{
   octet      tagVal;

   // Check for optional [n] CHOICE tag
   // time           [0] Time,
   // sequenceNumber [1] Unsigned (0..65535),
   // dateTime       [2] BACnetDateTime
   if (ReadChoiceTag( &tagVal, 2 )) return true;
   if (tagVal == 0xFF)
   {
      // No explicit tag: try to parse it out
      tagVal = whatTimestampChoice();
   }

   switch (tagVal)
   {
      case 0:   // The choice is time.
         ptstamp->choice = 0;
         if (ParseTime(&ptstamp->u.time))
         {
            tperror("Invalid time format",true);
            return true;
         }
         break;
      case 1:    // The choice is sequence number 0-65535.
         ptstamp->choice = 1;
         if (ReadW( &ptstamp->u.sequence_number )) return true;
         break;
      case 2:    // The choice is Date-Time.
         ptstamp->choice = 2;
         if (ParseDateTime(&ptstamp->u.date_time))
         {
            tperror("Invalid Date-Time format.", true);
            return true;
         }
         break;
      default:
         tperror("Cannot decode Event-Time-Stamp.",true);
         return true;
   }
   return false;
}

bool ParseTimeStampArray(BACnetTimeStamp *parray[], int arraycount)
{
   BACnetTimeStamp *q = NULL;

   int i = 0;
   EpicsListOf tsa;
   while (tsa.HasMore())
   {
      if (i >= arraycount)
      {
         tperror("Too many timestamps",true);
         break;
      }

      // TODO: DateTime should be wrapped, but according to 135.1 not Time or Unsigned.
      // But testAllEpics.txt wraps all three.  Make it optional?
      if (MustBe('{')) return true;  // each element surrounded by '{'

      if ((q=(tagTimeStamp *)malloc(sizeof(BACnetTimeStamp)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      if (ParseTimeStamp(q))
      {
         char tmp[50];
         sprintf(tmp,"Parse error in event-time-stamps, index=%d",i+1);
         tperror(tmp,true);
         free(q);
         break;  // error occurred
      }
      if (MustBe('}'))
         break;   // must close each array item

      parray[i] = q;
   }

   return s_error;
}

///////////////////////////////////////////////////////////////////////
// parse a date/time value
//in: lp    points to (date,time)
//    dtp      points to structure to initialize
//out:   true  if cancel selected

/*
Any of the following should be valid ...
   (5,29-SEP-1989), 01:00:00.0
   (Friday,29-SEP-1989), 01:00:00.0
   (29-SEP-1989, 5), 01:00:00.0
   (29-SEP-1989, Friday), 01:00:00.0
   (9/29/1989, Friday), 01:00:00.0
*/
bool ParseDateTime(BACnetDateTime *dtp)
{
   // 135.1 4.4(m) says DateTime is a constructed type and should be wrapped
   // in {}.  Historically, VTS has ignored this.
   // Since Date is wrapped as well, we can't be "bracket agnostic", but
   // we at least allow optional {}
   bool wrapped = NextIs('{');

   if (ParseDate(&dtp->date)) return true;
   if (MustBe(',')) return true;  // ',' separates the date and time text
   if (ParseTime(&dtp->time)) return true;

   if (wrapped)
   {
      if (MustBe('}')) return true;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////
// parse a DateRange value
//in: lp    points to (date,date)
//    pDR   points to structure to initialize
//out:   true  if parse error
bool ParseDateRange(BACnetDateRange *pDR)
{
   // 135.1 clause 4 would encode a BACnetDateRange SEQUENCE as {} with two comma-separated Dates:
   //   {(5-September-1995),(10-June-1996)}
   // 135.1 Annex A shows a dash instead of a comma:
   //   effective-period: {(5-September-1995)-(10-June-1996)}
   // allTestEpics.txt uses a comma, but no overall wrapping:
   //   effective-period: (1/2/2005,1),(2/1/2005,5)
   // VTS has historically allowed .. to separate the dates as well

   // Make wrapping optional.  To avoid confusion with (Date) wrapping, allow
   // only {} wrapping (per 135.1)
   bool wrapped = NextIs('{');

   if (ParseDate(&pDR->start_date)) return true;

   // Allow dates separated by dot-dot, dash, or comma
   if (NextIs('-'))
   {
   }
   else if (*lp=='.' && *(lp+1)=='.')
   {
      lp += 2;
   }
   else
   {
      // Using MustBe here means that any error message will encourage the 135.1-specified syntax
      if (MustBe(',')) return true;
   }

   if (ParseDate(&pDR->end_date)) return true;
   if (wrapped)
   {
      if (MustBe('}')) return true;
   }

   return false;
}

// Read day of week as text, integer 1 to 7, or *.
bool ReadDayOfWeek(octet *pVal)
{
   skipwhitespace();
   if (IsAlpha(*lp))
   {
      for (int i=0; i<7; i++)
      {
         if (_strnicmp(lp, DOWNames[i], 3) == 0)
         {
            SkipAlphaString();
            *pVal = i+1;  // day of week is 1 - 7
            return false;
         }
      }

      tperror("Invalid day of week",true);
      return true;
   }

   // Read as integer or *
   return ReadB( pVal, 1,7 );
}

// Read numeric day of month or day of week, LAST, EVEN, ODD, or *
// Return 0 for good value, 1 for error, or -1 for "not one of these"
int ReadDayOrMonth( word *pVal )
{
   int retval = -1;
   if (NextIsWildcard())
   {
      *pVal = DONT_CARE;
      retval = 0;
   }
   else if (IsNextNumber())
   {
      // Number may be month in / format, or day-of-month in - format
      retval = (ReadW(pVal)) ? 1 : 0;
   }
   else if (_strnicmp(lp,"LAST",4) == 0)
   {
      // LAST day-of-month
      lp += 4;
      *pVal = 32;
      retval = 0;
   }
   else if (_strnicmp(lp,"ODD",3) == 0)
   {
      // ODD day-of-month or ODD month
      lp += 3;
      *pVal = (PeekNextIs('-')) ? 33 : 13;
      retval = 0;
   }
   else if (_strnicmp(lp,"EVEN",4) == 0)
   {
      // EVEN day-of-month or EVEN month
      lp += 4;
      *pVal = (PeekNextIs('-')) ? 34 : 14;
      retval = 0;
   }

   return retval;
}

// Given the first value in a date, and lp pointing at the next character, 
// parse year, month, day of month, and day-of-week as required.
bool ParseDateHelper(BACnetDate *dtp, word val, bool allowTrailingDOW )
{
   if (NextIs('/'))
   {
      // Month
      if ((val != DONT_CARE) && ((val == 0) || (val > 14)))
      {
         tperror("Month must be 1-12, 13 for odd, 14 for even month, or *",true);
         return true;
      }
      dtp->month = (octet)val;

      // We do NOT (yet) support LAST, EVEN, or ODD strings in this format...
      if (ReadB( &dtp->day_of_month, 1,34 ))
         return true;
      if (MustBe('/'))
         return true;
      // Get numeric year below
   }
   else if (NextIs('-'))
   {
      // Day of month
      if ((val != DONT_CARE) && ((val == 0) || (val > 34)))
      {
         tperror("Day of month must be 1-31, LAST (or 32), odd (or 33), EVEN (or 34), or *",true);
         return true;
      }
      dtp->day_of_month = (octet)val;

      // Month
      if (NextIsWildcard())
      {
         dtp->month = DONT_CARE;
      }
      else
      {
         for (int i=0; i< (sizeof(MonthNames)/sizeof(MonthNames[0])); i++)
         {
            if (_strnicmp(lp,MonthNames[i],3)==0)  // only look at first 3 chars
            {
               SkipAlphaString();
               dtp->month = (octet)(i+1);        //months are 1-14 (including Odd and Even)
               break;
            }
         }

         // TODO: the old parser accidentally accepted ANY alpha string followed by dash
         // as DONT_CARE.  That seems like a bad idea, but we want to load old EPICS
         // with things like 255-ANY-2154 instead of *-*-*
         if (dtp->month == DONT_CARE)
         {
            SkipAlphaString();
//          return tperror("Invalid month name",true);
         }
      }
      if (MustBe('-'))
         return true;
      // Get numeric year below
   }
   else if (NextIs(','))
   {
      // Day of week as an integer or don't care
      if ((val != DONT_CARE) && ((val == 0) || (val > 7)))
      {
         tperror("Day of week must be MONday-SUNday, 1-7 or *",true);
         return true;
      }
      dtp->day_of_week = (octet)val;

      // Get next token: *, numeric month, numeric day-of-month, LAST, EVEN, ODD.
      // Then do recursive call to get date portion, but no trailing DOW
      int status = ReadDayOrMonth( &val );
      if (status > 0)
         return true;

      if (status == 0)
      {
         // Process the day or month, and recurse
         return ParseDateHelper(dtp, val, false);
      }
      else
      {
         tperror("Unknown Date format",true);
      return true;
      }
   }
   else
   {
      // Bad news
      tperror("Unknown Date format",true);
      return true;
   }

   // Get the year or wildcard
   if (NextIsWildcard())
   {
      dtp->year = DONT_CARE;
   }
   else
   {
      if (ReadW(&val))
         return true;

      if (val > 2154)                      //can't represent this date
      {
         tperror("Can't represent dates beyond 2154!",true);
         return true;
      }

      if (val>254 && val<1900)
      {
         tperror("Can't represent this year!",true);
         return true;
      }

      // Adjust so that 01 to 68 means 2001 to 2068
      // 69 to 99 means 1969 to 1999 (allows time_t 0 to show 1970)
      if (val < 69)
         val += 2000;

      if (val >= 1900)
         val -= 1900;

      dtp->year = (octet)val;
   }

   if (allowTrailingDOW)
   {
      if (NextIs(','))
      {
         // VTS format allows trailing DOW as number or string
         if (ReadDayOfWeek(&dtp->day_of_week))
            return true;
      }
      else
      {
         // TODO: should we COMPUTE a non-specified DOW from non-wildcard Y-M-D?
         // OMITTING DOW (versus *) in convenient...
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////
// parse a date value
//in: lp    points to date
//    dtp      points to structure to initialize
//out:   true  if cancel selected
/*
Any of the following should be valid ...
   (Friday,29-SEP-1989)  -- 135.1 format with day of week
   (29-SEP-1989)         -- 135.1 format without day of week
   (5,29-SEP-1989)
   (29-SEP-1989, 5)
   (29-SEP-1989, Friday)
   (9/29/1989, Friday)
*/
bool ParseDate(BACnetDate *dtp)
{
   word val;
   if (MustBe('(')) return true;  // date is surrounded by ()

   // fill initially with ANY values
   dtp->year = DONT_CARE;
   dtp->month = DONT_CARE;
   dtp->day_of_month = DONT_CARE;
   dtp->day_of_week = DONT_CARE;

   if (NextIsWildcard())
   {
      if (PeekNextIs(')'))
      {
         // Interpret "(*)" as a complete wildcard (the most common case)
         // (Don't return here - go through print_debug below)
      }
      else
      {
         // Wildcard of first element:
         // may be day of week in 135.1 format, month in / format, or day-of-month in - format
         if (ParseDateHelper(dtp, DONT_CARE, true))
            return true;
      }
   }
   else
   {
      int status = ReadDayOrMonth( &val );
      if (status > 0)
         return true;

      if (status == 0)
      {
         // Process the day or month, and continue
         if (ParseDateHelper(dtp, val, true))
            return true;
      }
      else
      {
         // Must be Day of week followed by comma
         octet dow;
         if (ReadDayOfWeek(&dow))
            return true;
         if (ParseDateHelper(dtp, dow, false))
            return true;
      }
   }

   print_debug( "  Date Y=%u M=%u D=%u dow=%u\n", dtp->year, dtp->month, dtp->day_of_month, dtp->day_of_week );
   return MustBe(')');
}

///////////////////////////////////////////////////////////////////////
// parse a time value
//in: lp    points to time
//    tp    points to structure to initialize
//out:   true  if cancel selected
//
// Accepts
//    10:31:13.95    Fully specified
//    10:30:12       Hundredths set to 0
//    10:29          Seconds and Hundredths set to 0
//    *              All set to DONT_CARE
//    *:*:*.*        All set to DONT_CARE
//    10:31:13.*     Any or all fields set to DONT_CARE
//    10:*:13.*
//    etc.
//
bool ParseTime(BACnetTime *tp)
{
   tp->hour = DONT_CARE;
   tp->minute = DONT_CARE;
   tp->second = DONT_CARE;
   tp->hundredths = DONT_CARE;

   if (ReadB( &tp->hour, 0,23 )) return true;   // Accepts 0-23, or * for DONT_CARE
   if (NextInBufferIs(':'))            // May be at end of buffer, so don't call NextIs
   {
      if (ReadB( &tp->minute, 0,59)) return true;
      if (NextInBufferIs(':'))         // May be at end of buffer, so don't call NextIs
      {
         // Optional seconds
         if (ReadB( &tp->second, 0,59)) return true;
         if (NextInBufferIs('.'))      // May be at end of buffer, so don't call NextIs
         {
            // Optional hundredths
            if (ReadB( &tp->hundredths, 0,99)) return true;
         }
         else
         {
            tp->hundredths = 0;        // omitted hundredths means 0.
         }
      }
      else
      {
         tp->second = 0;               // omitted seconds means 0.
         tp->hundredths = 0;           // omitted hundredths means 0.
      }
   }

   print_debug( "  Time H=%u M=%u S=%u h=%u\n", tp->hour, tp->minute, tp->second, tp->hundredths );
   return false;
}

///////////////////////////////////////////////////////////////////////
// parse a bitstring value as either of two forms:
//    B'11011...1011'
//  or
//    ( T,F,T,F,...)    i.e. T or True or F or False in a list
//note:  after the initial B' or ( the list may span multiple lines
//
//in: lp    points to B'11011...1011' or (...)
//    bsp      points to structure to initialize
//    nbits how many bits maximum
//    nbf      if not NULL, points to UINT to update with number of bits found
//out:   true  if cancel selected

bool ParseBitstring(octet *bsp, UINT nbits, UINT *nbf)
{
   octet db = 0x80;

   memset(bsp,0,(nbits+7)/8);          //initialize to 0
   if (nbf) *nbf = 0;                  //no bits found yet

   skipwhitespace();
   if ((*lp=='b' || *lp=='B') && (lp[1]=='\'' || lp[1]=='`'))
   {
      // it's a bitstring in old-school format
      lp += 2;                          //skip over B'
      while (*lp != '\'')
      {
         if (nbits == 0)
         {
            tperror("Expected end of bitstring here",true);
            return true;
         }
         if (feof(ifile) || ((*lp==0) && (ReadNext()==NULL)))
         {
            tperror("End of file within bitstring",true);
            return true;
         }

         if (*lp == '1')
         {
            *bsp |= db;
         }
         else if (*lp != '0')
         {
            tperror("Bitstring must contain 0s or 1s",true);
            return true;
         }
         lp++;                   //skip 0 or 1

         if ((db>>=1)==0)
         {
            db = 0x80;
            bsp++;
         }
         if (nbf) *nbf += 1;             //update num bits found
         nbits--;
      }
      lp++; // skip trailing quote
   }
   // 135.1 say this should be {}, but () seems to be traditional in sample tpi files.
   // TODO: 135.1 4.4(h) says that we are supposed to accept "?" as a don't-care bit.
   // Alas, we have no way to STORE that in our simple string of bytes.
   // So we COULD parse it, and turn "?" (and probably "*") into FALSE, but that
   // seems of dubious utility.
   // Note that the BACnetBitString can't parse or store "?" either.
   else if ((*lp=='(') || (*lp=='{'))
   {
      // it's a bitstring in 135.1 format
      EpicsListOf bitsy;
      while (bitsy.HasMore())
      {
         if (nbits == 0)
         {
            tperror("Expected end of bitstring here",true);
            return true;
         }

         CString value;
         if (ReadAlphaString( &value ))
            return true;

         if ((value[0] == 't') || (value[0]=='T'))
         {
            *bsp |= db;
         }
         else if ((value[0] != 'f') && (value[0] != 'F'))
         {
            tperror("Expected true, false, T, or F",true);
            return true;
         }

         if ((db>>=1)==0)
         {
            db = 0x80;
            bsp++;
         }
         if (nbf) *nbf += 1;             //update num bits found
         nbits--;
      }
   }
   else
   {
      tperror("Expected bitstring as {T,F...} or B'bitstring'",true);
      return true;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////
// parse an octetstring value as X'hexchars'
//in: lp    points to X'hexchars'
//    osp      points to structure to initialize
//    nmax  how many octets maximum
//    ncount   if not NULL, points to word to receive string size in octets
//out:   true  if cancel selected

bool ParseOctetstring(octet *osp,word nmax,word *ncount)
{
   memset(osp,0,nmax);                 //initialize to 0
   if (ncount != NULL) *ncount=0;

   skipwhitespace();
   if ((*lp!='x' && *lp!='X') || (lp[1]!='\'' && lp[1]!='`'))
   {
      tperror("Expected X'octetstring' here!",true);
      return true;
   }
   lp += 2;                              //skip over X'

   while (*lp && (*lp != '\''))
   {
      if (nmax)
      {
         if (cvhex(lp,osp))
         {
            tperror("Expected hex characters",true);
            return true;
         }
         lp  += 2;
         osp += 1;
         if (ncount != NULL) (*ncount)++;
         nmax--;
      }
      else
      {
         tperror("Expected ' for end of octetstring here!",true);
         return true;
      }
   }
   if (*lp)
   {
      lp++;
   }
   return false;
}

bool ParsePrescale(BACnetPrescale* pt)
{
   EpicsSequence prescale;
   if (prescale.Begin()) return true;

   if (ReadW( &pt->multiplier, ',' )) return true;
   if (ReadW( &pt->moduloDivide )) return true;

   return prescale.End();
}

bool ParseAccumulatorRecord(BACnetAccumulatorRecord* pt)
{
   EpicsSequence accRecord;
   if (accRecord.Begin()) return true;

   ParseDateTime(&pt->timestamp);
   if (MustBe(',')) return true;

   if (ReadW( &pt->presentValue, ',' )) return true;
   if (ReadW( &pt->accumulatedValue, ',' )) return true;

   if (ReadEnum(&pt->accumulatorStatus, &NetworkSniffer::BAC_STRTAB_BACnetAccumulatorStatus)) return true;
   return accRecord.End();
}

///////////////////////////////////////////////////////////////////////
// read an array of BACnetActionCommands from the buffer lp points to
//  ({actioncommand},{actioncommand},{actioncommand}...)
//in: lp    points to current position in buffer lb
//out:   NULL  if an error occurred
//    else  pointer to a list of BACnetActionCommands
//    lp    points past the delimiter } unless it was the end of the buffer
//
// - BACnetActionList is a list of BACnetActionCommand
// - 135.1 4.4.1 c says that lists are enclosed in parenthesis, with
//   elements separated by commas
// - each BACnetActionCommand is a SEQUENCE
// - 135.1 4.4 says that SEQUENCE are enclosed curly brackets
//
BACnetActionCommand *ReadActionCommands()
{
   BACnetActionCommand  *firstp = NULL, **pp = &firstp, *q = NULL;
   const propdescriptor *pd;
   dword          dw;

   // List is enclosed by (), but we accept {} as well
   EpicsListOf listOf;
   while (listOf.HasMore())
   {
      // Here lp must point to a BACnetActionCommand.
      // Sequence should be {}, but we accept () as well
      EpicsSequence action;
      if (action.Begin()) break;

      // here we have (BACnetActionCommand)...
      if ((q = (tagActionCommand *)malloc(sizeof(BACnetActionCommand))) == NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      // DeviceID is optional.
      if (action.HasOption())
      {
         if (ReadObjectID( &dw, ',' )) break;
         if ((dw>>22) != OBJ_DEVICE)
         {
            if (tperror("Expected a Device Object Identifier here!",true))
               break;
         }
         q->device_id = dw;
      }
      else
      {
         // Use BAD_OBJID as to mean "no deviceID specified"
         q->device_id = BAD_OBJID;
      }

      if (ReadObjectID( &q->object_id, ',' )) break;

      // validate property name and optional index here
      //
      skipwhitespace();
      if (validatePropertyNameAndIndexCode(q->object_id, &q->property_id, &q->pa_index, &pd))
         break;  // Error already output

      if (MustBe(',')) break;

      // ASN.1 says propertyValue is an ANY, but we decode based on
      // the specified object type and property.
      if (pd->ParseType == flt)
      {
         // REAL
         q->value_type = FLT;
         if (ReadFloat( &q->av.fproperty_value, ',')) break;
      }
      else if ( (pd->ParseType == et) && (pd->PropET == eiBPV))
      {
         // BinaryPV
         q->value_type = BPV;
         q->av.bproperty_value = INACTIVE;     //assume inactive
         CString val;
         if (ReadAlphaString( &val, ',' )) break;
         if (val[0] =='a' || val[0]=='A')
            q->av.bproperty_value = ACTIVE;
      }
      else
      {
         // Unsigned
         q->value_type = UNS;
         if (ReadW( &q->av.uproperty_value, ',')) break;
      }

      // The ASN.1 says priority is "OPTIONAL, --used only when property is commandable"
      // But that isn't clear whether it is REQUIRED to be present in that case.
      // Clause 19.2.1 says priority ISN'T required during a write, defaulting to 16.
      if (action.HasOption())
      {
         if (ReadB( &q->priority, 1,16 ) || MustBe(',')) break;
      }
      else
      {
         // No priority provided.  Use default
         q->priority = 16;

         // See comment above
//         if (pd->PropFlags & IsCommandable)
//         {
//            if (tperror("Commandable property needs priority here!",true)) break;
//         }
      }

      // postDelay is optional
      if (action.HasOption())
      {
         if (ReadW( &q->post_delay, ',' )) break;
      }
      else
      {
         q->post_delay = 0;
      }

      if (ReadBool( &q->quit_on_failure, ',' )) break;
      if (ReadBool( &q->write_successful )) break;

      // Check closing paren or bracket
      if (action.End()) break;

      q->next = NULL;                    //link onto the list
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   if (q!=NULL)
   {
      free(q);               //don't lose this block!
   }
   return firstp;
}

bool ParseBooleanList( BooleanList **elp )
{
   BooleanList **pp = elp, *q = NULL;
   *elp = NULL;                          //initially there is no list
   bool value;

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to a Boolean

      // TODO: what gives with wrapped Booleans?  testAllEpics has this for auto-slave-discovery.
      // Eat it for now.
      bool wrapped = NextIs('{');
      if (ReadBool( &value )) break;
      if (wrapped) NextIs('}');

      if ((q = (tagBooleanList *)malloc(sizeof(BooleanList))) == NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      q->value = value;
      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   return s_error;
}

bool ParseUnsignedList( UnsignedList **elp )
{
   UnsignedList **pp = elp, *q = NULL;
   *elp = NULL;                          //initially there is no list
   dword value;

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to an unsigned integer
      if (ReadDW(&value)) break;

      if ((q = (tagUnsignedList *)malloc(sizeof(UnsignedList))) == NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      q->value = value;
      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   return s_error;
}

bool ParseEnumList(BACnetEnumList **elp, const NetworkSniffer::BACnetStringTable *etbl)
{
   BACnetEnumList **pp = elp, *q=NULL;
   *elp = NULL;                          //initially there is no list
   word value;

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to an enumerated value
      if (ReadEnum(&value, etbl)) break;

      if ((q = (tagEnumList *)malloc(sizeof(BACnetEnumList))) == NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      q->value=value;
      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   return s_error;
}

bool ParsePropertyList(BACnetEnumList **elp)
{
   BACnetEnumList **pp = elp, *q = NULL;
   *elp = NULL;                          //initially there is no list

   EpicsListOf list;
   while (list.HasMore())
   {
      // Here lp must point to the beginning of a new BACnetPropertyIdentifier in the array
      CString propName;
      if (ReadEnumString( &propName ))
         break;
      int value = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumValue( propName );
      if (value < 0)
      {
         tperror("Unknown property identifier",true);
         break;
      }

      if ((q = (tagEnumList *)malloc(sizeof(BACnetEnumList))) == NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      q->value = value;
      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   return s_error;
}

///////////////////////////////////////////////////////////////////////
// read a list of BACnetVTClasses from the buffer lp points to
// (class,class,class...)
//in: lp    points to current position in buffer lb
//    vtclp points to a BACnetVTClassList pointer variable to be
//          initialized to point to the created list of BACnetVTClasses
//out:   true  if an error occurred
//    lp    points past the delimiter ) unless it was the end of the buffer

// TODO: isn't this just a list of enums?  Why not use ParseEnumList?
bool ParseVTClassList(BACnetVTClassList **vtclp)
{
   BACnetVTClassList **pp = vtclp, *q = NULL;
   *vtclp = NULL;                           //initially there is no list
   word  vtc;

   EpicsListOf list;
   while (list.HasMore())
   {
      // Here lp must point to a BACnetVTClass enumeration
      if (ReadEnum(&vtc, &NetworkSniffer::BAC_STRTAB_BACnetVTClass)) break;

      if ((q = (tagVTClassList *)malloc(sizeof(BACnetVTClassList))) == NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      q->vtclass = (BACnetVTClass)vtc;
      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   return s_error;
}

///////////////////////////////////////////////////////////////////////
// read a list of BACnetVTSession from the buffer lp points to
// (class,class,class...)
//in: lp    points to current position in buffer lb
//    vtslp points to a BACnetVTSession pointer variable to be
//          initialized to point to the created list of BACnetVTClasses
//out:   true  if an error occurred
//    lp    points past the delimiter ) unless it was the end of the buffer
bool ParseVTSessionList(BACnetVTSession **vtslp)
{
   BACnetVTSession **pp = vtslp, *q = NULL;
   *vtslp = NULL;                           //initially there is no list

   EpicsListOf list;
   while (list.HasMore())
   {
      // Here lp must point to a BACnetVTSession
      if ((q = (BACnetVTSession*)malloc(sizeof(BACnetVTSession))) == NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      EpicsSequence vts;
      if (vts.Begin()) break;
      if (ReadW( &q->local_session_id, ',')) break;
      if (ReadW( &q->remote_session_id, ',')) break;
      if (ParseBACnetAddress( &q->remote_address)) break;
      if (vts.End()) break;

      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   if (q != NULL)
   {
      free(q);
   }

   return s_error;
}


///////////////////////////////////////////////////////////////////////
// read an array of BACnetObjectIdentifiers from the buffer lp points to
// {(objtype,instance),(objtype,instance),...}
//in: lp    points to current position in buffer lb
//    dalp  points to a BACnetObjectIdentifier pointer variable to be
//          initialized to point to the created list of BACnetObjectIdentifiers
//    nump  points to a word counter for the number of items found
//out:   true  if an error occurred
//    lp    points past the delimiter ) unless it was the end of the buffer

bool ParseObjectList(BACnetObjectIdentifier **dalp,word *nump)
{
   BACnetObjectIdentifier **pp = dalp, *q = NULL;
   *dalp = NULL;                         //initially there is no list
   *nump = 0;

   word  objtype;
   dword objectInstance;
   int i;

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to an objectID in parenthesis
      if (ReadObjectID( &objectInstance )) break;
      objtype = (word)(objectInstance >> 22);
      if (objtype < MAX_DEFINED_OBJ)
      {
         // don't add proprietary objects here
         DevObjList[objtype].object_type = objtype;
         DevObjList[objtype].ObjIDSupported |= soSupported;
         i = DevObjList[objtype].ObjInstanceNum;
         DevObjList[objtype].ObjInstanceNum++;
         DevObjList[objtype].ObjInstance[i] = objectInstance & 0x003fffff;
      }

      if ((q = (tagObjectIdentifier *)malloc(sizeof(BACnetObjectIdentifier)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      q->object_id = objectInstance;
      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
      *nump += 1;
   }
   return s_error;
}

///////////////////////////////////////////////////////////////////////
// read a list of BACnetAddressBinding from the buffer lp points to
// (((device,instance),network,macaddr),((device,instance),network,macaddr),((device,instance),network,macaddr)...)
//in: lp    points to current position in buffer lb
//    dalp  points to a BACnetAddressBinding pointer variable to be
//          initialized to point to the created list of BACnetAddressBindings
//out:   true  if an error occurred
//    lp    points past the delimiter ) unless it was the end of the buffer

bool ParseAddressList(BACnetAddressBinding **dalp)
{
   BACnetAddressBinding **pp = dalp, *q = NULL;
   *dalp = NULL;                         //initially there is no list
   dword                dw;

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to a BACnetAddressBinding
      //{(device,instance),network,macaddr)}, {...
      if ((q=(tagAddressBinding *)malloc(sizeof(BACnetAddressBinding)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      EpicsSequence bab;
      if (bab.Begin()) break;

      if (ReadObjectID( &dw, ',' )) break;
      if ((dw>>22)!=OBJ_DEVICE)
      {
         tperror("Must use a Device Object Identifier here!",true);
         break;
      }
      q->device_object_id = dw;

      if (ParseBACnetAddress( &q->device_address )) break;

      if (bab.End()) break;

      q->next = NULL;                       //link onto the list
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   if (q != NULL)
      free(q);
   return s_error;
}

///////////////////////////////////////////////////////////////////////
// read a list of BACnetSessionKeys from the buffer lp points to
// ((key,network,macaddr),(key,network,macaddr),(key,network,macaddr)...)
//in: lp    points to current position in buffer lb
//    dalp  points to a BACnetSessionKey pointer variable to be
//          initialized to point to the created list of BACnetSessionKeys
//out:   true  if an error occurred
//    lp    points past the delimiter ) unless it was the end of the buffer

bool ParseSessionKeyList(BACnetSessionKey **dalp)
{
   BACnetSessionKey  **pp = dalp, *q = NULL;
   *dalp = NULL;                         //initially there is no list

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to a BACnetSessionKey in the list
      if ((q=(tagSessionKey *)malloc(sizeof(BACnetSessionKey)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      EpicsSequence bsk;
      if (bsk.Begin()) break; // each list item is surrounded by {}
      //here we have key,network,macaddr}...

      if (ParseOctetstring(&q->session_key[0],
                           sizeof(q->session_key),NULL)) break;
      if (MustBe(',')) break;
      if (ParseBACnetAddress( &q->peer_address )) break;

      if (bsk.End()) break; // each list item is surrounded by {}

      q->next = NULL;                       //link onto the list
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   if (q != NULL)
      free(q);
   return s_error;
}

// Return true if the string contains a bracketed day of week name
bool hasDOW(char *src)
{
   bool found = false;
   if ((*src == '(') || (*src == '{'))
   {
      while (*++src == ' ') ;
      for (int i=0; i<7; i++)
      {
         // Just look for a case-insensitive match on the first three characters
         if (_strnicmp(src, DOWNames[i], 3)==0)
         {
            found = true;
            break;
         }
      }
   }

   return found;
}

///////////////////////////////////////////////////////////////////////
// read a list of BACnetAddress from the buffer lp points to
//  {network,X'macaddress'}
//in: lp    points to current position in buffer lb
//    pAddr  points to a BACnetAddress
//out:   true  if an error occurred
//    lp    points past the closing bracket if wrapped, else past the address
bool ParseBACnetAddress(BACnetAddress *pAddr)
{
   // TODO: 135.1 clause 4 would wrap BACnetAddress in {}
   // 135.1 Annex A shows bindings wrapped
   // But allTestEpics.txt has historically NOT wrapped in SOME cases.
   // Accept optional wrapping
   EpicsSequence bna;
   bna.BeginOptional();

   if (ReadW( &pAddr->network_number, ',' )) return true;
   if (ParseOctetstring(&pAddr->mac_address[0],
                        sizeof(pAddr->mac_address),
                        &pAddr->address_size)) return true;
   return bna.End();
}

///////////////////////////////////////////////////////////////////////
// read a list of BACnetDestination from the buffer lp points to
//  ((days,from,to,(recipient),procid,conf,transitions),(days,from,to,(recipient),procid,conf,transitions)...)
//in: lp    points to current position in buffer lb
//    dalp  points to a BACnetDestination pointer variable to be
//          initialized to point to the created list of BACnetDestinations
//out:   true  if an error occurred
//    lp    points past the delimiter )

bool ParseDestinationList(BACnetDestination **dalp)
{
   BACnetDestination **pp = dalp, *q = NULL;
   *dalp = NULL;                         //initially there is no list

   word  i;
   bool found_day;

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to a BACnetDestination in the list
      EpicsSequence bdest;
      if (bdest.Begin()) break;

      //here we have days,from,to,(recipient),procid,conf,transitions),...
      if ((q=(tagDestination *)malloc(sizeof(BACnetDestination)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         break;
      }

      //look for (days of week)
      q->valid_days = 0;

      // 135.1 says bitstring in {}, and there is no mention of the DOW version
      // For now, accept both.
      // test if days of week are entered as (T,F,T,F,F,T,T) or (MON,WED,SAT,SUN)
      skipwhitespace();
      if (!hasDOW(lp))
      {
         if (ParseBitstring(&q->valid_days, 7, NULL)) break;
      }
      else
      {
         // Special bitstring handling for days of week
         EpicsListOf dow;
         while (dow.HasMore())
         {
            skipwhitespace();
            found_day = false;
            for (i=0; i<7; i++)
            {
               if (_strnicmp(lp, DOWNames[i], 3)==0)
               {
                  SkipAlphaString();
                  q->valid_days |= (octet)(0x80>>i); //Monday is 80, Sunday is 2
                  found_day = true;
                  break;
               }
            }
            if (!found_day)
               return tperror("Expected Day of Week Here",true);
         }
      }
      if (MustBe(',')) break;

      if (ParseTime(&q->from_time)) break;
      if (MustBe(',')) break;

      if (ParseTime(&q->to_time)) break;
      if (MustBe(',')) break;

      if (ParseRecipient(&q->recipient)==NULL) break;
      if (MustBe(',')) break;

      if (ReadW( &q->process_id, ',' )) break;

      if (ReadBool( &q->notification, ',')) break;

      if (ParseBitstring(&q->transitions, 3, NULL)) break;

      if (bdest.End()) break;

      q->next = NULL;                       //link onto the list
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   if (q!=NULL)
   {
      free(q);
   }
   return s_error;
}

///////////////////////////////////////////////////////////////////////
// read a list of BACnetRecipient from the buffer lp points to
// ((device,instance) or (network,macaddr),...)
//in: lp    points to current position in buffer lb
//    dalp  points to a BACnetRecipient pointer variable to be
//          initialized to point to the created list of BACnetRecipients
//out:   true  if an error occurred
//    lp    points past the delimiter ) unless it was the end of the buffer

bool ParseRecipientList(BACnetRecipient **dalp)
{
   BACnetRecipient **pp = dalp, *q = NULL;
   *dalp = NULL;                         //initially there is no list

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to a BACnetRecipient in the list
      if ((q = ParseRecipient(NULL))==NULL)
         break;

      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   return s_error;
}

///////////////////////////////////////////////////////////////////////
// read a BACnetRecipient from the buffer lp points to
// (device,instance) or (network,macaddr)
//in: lp    points to current position in buffer lb
//    inq   points at BACnetRecipient, or if null, allocate memory for it.
//out:   NULL  if an error occurred
//    else  pointer to BACnetRecipient
//    lp    points past end of BACnetRecipient wrapper

BACnetRecipient *ParseRecipient(BACnetRecipient *inq)
{
   BACnetRecipient   *q = NULL;
   dword             dw;
   bool wrapped = false;   // Declared here to preserve VC++ 6.0 compatibility

   // 135.1 clause 4 says there should be a CHOICE tag, followed by BACnetAddress in {} or ObjectID in ()
   // 135.1 Annex A has
   //   recipient-list: ( {{Monday, Tuesday, Wednesday, Thursday, Friday}, 6:00, 20:00,(device, 12), 21, TRUE, {TRUE, TRUE,TRUE}} )
   // containing the unwrapped objectID (device, 12), which matches clause 4 except for the tag
   //
   // allTestEpics.txt has
   //   time-synchronization-recipients: ({(Device,18)},{(Device,20)},{4,X'04'}) w
   //   recipient-list: ({(MON,WED,SAT,SUN),0:0:0.0,23:59:59.99,(device,481699),1,true,(T, T, T)})
   //   active-cov-subscriptions: (
   //        {{(Device, 12), 30}, {(Analog-Input, 2),Present-Value},TRUE,100,1.0},
   //        {{55, X'01', 30}, {(Analog-Input, 1) , Present_Value},TRUE,100} )
   // The first wraps the ObjectID as {(Device,20)}; the second does not as (device,481699); 
   // the third does not as (Device, 12)

   // Allow an optional CHOICE tag
   octet choice;
   if (ReadChoiceTag( &choice, 1 )) goto brfail;

   // For 135.1 strict, move the wrapper as an EpicsSequence into the Address choice.
   // We allow OPTIONAL {} wrapping, so as not to eat the () on a naked ObjectID
   wrapped = NextIs( '{' );
   if (inq==NULL)
   {
      if ((q=(tagRecipient *)malloc(sizeof(BACnetRecipient)))==NULL)
      {
         tperror("Can't Get Object Space!",true);
         return NULL;
      }
   }
   else
   {
      q = inq;
   }
   q->next = NULL;

   if (choice == 0xFF)
   {
      // CHOICE not tagged.  Assume that DeviceID is wrapped in (), and
      // that a digit indicates the network number of a BACnetAddress
      choice = (IsNextNumber()) ? 1 : 0;
   }

   switch (choice)
   {
   case 0:
      q->choice = 0;                  //deviceID
      if (ReadObjectID(&dw)) goto brfail;
      if ((dw>>22) != OBJ_DEVICE)
      {
         tperror("Must use a Device Object Identifier here!",true);
         goto brfail;
      }
      q->u.device = dw;
      break;

   case 1:
      q->choice = 1;                  //address
      if (ParseBACnetAddress( &q->u.address )) goto brfail;
      break;

   default:
      tperror("Invalid BACnetRecipient choice!",true);
      goto brfail;
      break;
   }

   if (wrapped && MustBe('}'))
      goto brfail;

   return q;

brfail:
   if (inq==NULL) free(q);          //don't release unless we malloc'd it
   return NULL;
}


///////////////////////////////////////////////////////////////////////
// read a list of BACnetCOVSubscription from the buffer lp
//in: lp    points to current position in buffer lb
//    dalp  points to a BACnetCOVSubscription pointer variable to be
//          initialized to point to the created list of BACnetCOVSubscription
//out:   true  if an error occurred
//    lp    points past the delimiter ) unless it was the end of the buffer
bool ParseCOVSubList(BACnetCOVSubscription **covsub)
{
   BACnetCOVSubscription  **pp = covsub, *q = NULL;
   *covsub = NULL;                          //initially there is no list

   EpicsListOf list;
   while (list.HasMore())
   {
      //here lp must point to a BACnetCOVSubscription in the list
      if ((q=ParseCOVSubscription()) == NULL) break;

      q->next = NULL;
      *pp = q;
      pp = &q->next;
      q = NULL;
   }

   return s_error;
}

///////////////////////////////////////////////////////////////////////
// read a BACnetCOVSubscription from the buffer lp points to
// (RecipientProcess, ObjectPropertyReference, Boolean, Unsigned)
//in: lp    points to current position in buffer lb
//out:   NULL  if an error occurred
//    else  pointer to newly created BACnetCOVSubscription
//    lp    points past the delimiter ) unless it was the end of the buffer
BACnetCOVSubscription *ParseCOVSubscription()
{
   BACnetCOVSubscription   *q = NULL;
   // (((0, (Device, Instance 12)), 300), ((Analog Input, 1), Present-Value), TRUE, 100, 1.0)
   //here we have (RecipientProcess, ObjectPropertyReference, Boolean, Unsigned) ...
   EpicsSequence covs;
   if (covs.Begin()) return NULL;

   if ((q=(tagCOVSubscription *)malloc(sizeof(BACnetCOVSubscription)))==NULL)
   {
      tperror("Can't Get Object Space!",true);
      return NULL;
   }
   q->next = NULL;

   //parse BACnetRecipientProcess: (BACnetRecipient, Unsigned32)
   // (((0, (Device, Instance 12)), 300)
   EpicsSequence brp;
   if (brp.Begin()) goto brfail;
   if (ParseRecipient(&(q->recipient.recipient)) == NULL) goto brfail;
   if (MustBe(',')) goto brfail;
   if (ReadW( &q->recipient.process_id )) goto brfail;
   if (brp.End()) goto brfail;
   if (MustBe(',')) goto brfail;

   // parse ObjectPropertyReference
   // ((Analog Input, 1), Present-Value)
   if ((ParseReference(&(q->monitoredPropertyReference)))==NULL)  goto brfail;
   if (MustBe(',')) goto brfail;

   // parse boolean (issue confirmed notifications)
   if (ReadBool( &q->notification, ',' )) goto brfail;

   // parse unsigned (timeRermaining)
   if (ReadW( &q->timeRemaining )) goto brfail;

   // parse optional real
   if (NextIs(','))
   {
      if (ReadFloat( &q->covIncrement )) goto brfail;
   }

   if (covs.End()) goto brfail;
   return q;

brfail:
   if (q==NULL) free(q);
   return NULL;
}


///////////////////////////////////////////////////////////////////////
// parse a string parameter
// in: p     points to string variable to contain the result
//     ps    max size of p
//     param points to parameter to be parsed
//
// The param pointer should point to a string like:
//     "some string"
// or  'some string'
// or  `somestring'
// out:   true     if no string, and cancel selected

bool setstring(char *p,word ps, char *param)
{
   char  q;
   word  i;

   lp = param;
   skipwhitespace();
   q = *lp++;
   if ((q == DOUBLEQUOTE) || (q == SINGLEQUOTE) || (q == ACCENTGRAVE))
   {
      // 135.1 clause 4.3 says that strings
      // - may be enclosed by double quotes
      // - may be enclosed by single quotes
      // - may be enclosed between an accent grave and a single quote
      // - ends at the end of the line, regardless of quoting
      if (q == ACCENTGRAVE)
         q = SINGLEQUOTE;

      // Eat the entire string, whether we can store it all or not
      ps -= 1; // room for the null
      for (i = 0; (*lp != 0) && (*lp != 0x0A) && (*lp != q); lp++)
      {
         if (i < ps)
         {
            p[i++] = *lp;
         }
      }

      p[i] = 0;        // mark end with asciz

      if (*lp != 0)
      {
         lp += 1;    // skip trailing quote
      }

      return false;
   }

   return tperror("Expected string parameter here",true);
}


///////////////////////////////////////////////////////////////////////
// read a one-byte integer from the buffer lp points to
//in: lp    points to current position in buffer lb
//    lb,ub range of acceptable values
//out: return false if a valid value or "*" for DONT_CARE; else true.
//    *pValue gets value
//    lp    points to the character after the number

bool ReadB(octet *pValue, UINT lb, UINT ub)
{
   *pValue = DONT_CARE;    // Returned for non-numeric data

   if (NextIsWildcard())
   {
      // Accept as wildcard value (*pValue is DONT_CARE)
      return false;
   }

   UINT d = 0;
   while (IsDigit(*lp))
   {
      d = (d*10) + (*lp++ - '0');
   }

   // TODO: the 255 sucks - "*" is nicer syntax - but we want to maintain backward compatibility.
   if ((d != 255) && (d<lb || d>ub))
   {
      CString err;
      err.Format("Must use an Unsigned Integer %u..%u here!",lb,ub);
      tperror(err,true);
      return true;   // (*pValue is DONT_CARE)
   }

   *pValue = (octet)d;
   return false;
}

///////////////////////////////////////////////////////////////////////
// Read an enumeration value from the buffer lp points to
// in:  lp       points to current position in buffer lb
//      etp      table of enumeration names
// out: 0xFFFF   if end of buffer or invalid enumeration
//      else     the enumeration (note that ? results in enumeration 0)
//      lp       points at the delimiter after the enumeration.
//
bool ReadEnum(word *pVal, const NetworkSniffer::BACnetStringTable *etp)
{
   char  c, e[512];

   // Parse out the value, converting any underbars to dashs to match our tables.
   // NOTE: this code formerly converted SPACES to dashes.  This caused hassles
   // with the expected W and <...> decorations, and isn't allowed by the EPICS format,
   // so we have removed it.
   skipwhitespace();
   int len = 0;
   while ((c = *lp) != 0)              //until the end of the buffer
   {
      if (c == '_')
         c = '-';                      //convert underscore to dash

      if (c=='-' || IsAlnum(c))        //if its a valid part of an enumeration name
      {
         if (len < sizeof(e)-1)
            e[len++] = c;              //save this character
         lp++;                         //advance scan
      }
      else
      {
         break;                        // else exit loop
      }
   }

   e[len] = 0;
   if (len > 0)
   {
      // We COULD call EnumValue, which would also accept
      // proprietary{xxx}123, vendor{xxx}123, reserved{xxx}123, or naked integer.
      // Since these aren't "EPICS legal" we do it by hand.
      word ix;
      for (ix=0; ix < etp->m_nStrings; ix++)
      {
         if (etp->m_pStrings[ix])
         {
            if (_stricmp(e,etp->m_pStrings[ix])==0)
            {
               //matching enumeration
               *pVal = ix;
               return false;
            }
         }
      }

      // The 135.1-2013 EPICS definition says
      //   "Proprietary values are shown as a named text with no whitespace and
      //   ending in a non-negative decimal numeric. Each must start with the word
      //   "proprietary": Object_Type, proprietary-object-type-653;"
      // But this means that we COPIED the digits into buffer e.
      // To call ReadDW, we need to back up the cursor to the first digit.
      if (_strnicmp(e,"proprietary",11)==0)
      {
         if (etp->m_nMax <= etp->m_nReserved)
         {
            tperror("This is not an extensible enumeration!",true);
         }
         else
         {
            // Back up the cursor looking for digits
            bool hasDigits = false;
            while (IsDigit(lp[-1]))
            {
               hasDigits = true;
               lp -= 1;
            }

            if (hasDigits)
            {
               if (!ReadW( pVal ))
               {
                  if (*pVal >= etp->m_nReserved)
                  {
                     // Valid proprietary value
                     return false;
                  }
                  tperror("Proprietary enumeration cannot use the reserved range",true);
               }
            }
            else
            {
               tperror("Proprietary enumeration must specify a numeric value",true);
            }
         }
         return true;
      }

      // "unknown" isn't allowed by the 135.1-2013 EPICS definition, but
      // it is part of the inherited code and used in Docs/allTestEpics.tpi,
      // so we retain it.  Syntax is unknown:123
      //
      // Something like this seems NECESSARY for things like Schedule, which
      // can reference an enumerated value of unknown type.
      // Not crazy about the colon (versus unknown_123, like proprietary), but
      // we want to be backward compatible, at least for now
      if ((_stricmp(e,"unknown")==0) && (*lp == ':'))
      {
         lp++;
         return ReadW( pVal );
      }
   }

   tperror("Expected an Enumeration Name here!",true);
   return true;
}

///////////////////////////////////////////////////////////////////////
// find the next non-whitespace line from the file
//in: lp    points to current position in buffer lb
//    ifile file stream
//out:   NULL  if eof,
//    else  pointer to next non-whitespace
//          (in this case lp also points to it)

char *ReadNext()
{
   do
   {
      if (feof(ifile)) return NULL;    //end of file
      readline();                      //read a line from the file
      lp = &lb[0];
      skipwhitespace();
   } while (*lp==0);                   //this was a blank line
   return lp;
}

///////////////////////////////////////////////////////////////////////
// find the end of whitespace in a string
//in: lp    points to string
//out:   lp    points to first non-whitespace char

void skipwhitespace()
{
   while (*lp==SPACE) lp++;
   if (*lp==0) ReadNext();
}

///////////////////////////////////////////////////////////////////////
// Read a line from a text file, squishing out redundant white space and comments
//in: lb    points to a line buffer
//    ifile file stream to read from
//out: lb    filled with the line, ends in 0
//    lc    updated

static void readline()
{
   char  *dp,*sp,c;
   bool  HaveNonWS=false;
   int len;

   lb[0] = 0;
   dp = fgets(lb, lpSize, ifile);                //get a line from the file

   // Check for buffer overflow
   len = strlen(lb);
   while ((dp != NULL) && (len == lpSize-1) && (lb[lpSize-2] != '\n'))
   {
      // Buffer is full and last byte isn't newline: need to read more.
      // Double the buffer size and append
      lb = (char*)realloc( lb, 2*lpSize );

      // Read into buffer, covering the null
      dp = fgets(&lb[lpSize-1], lpSize+1, ifile);
      lpSize *= 2;
      len = strlen(lb);
   }

   dp = sp = lb;
   while (*sp)
   {
      switch(c=*sp++)
      {
      case SPACE:
      case TAB:
      case CR:
      case LF:
         while ((c=*sp) != 0)
         {
            if (c==SPACE||c==TAB||c==CR||c==LF)
               sp++;                      //skip white space
            else
               break;
         }
         if (*sp==0) goto rlexit;         //we're done
         if (HaveNonWS)
            *dp++ = SPACE;                //convert a white space sequence to just a single space
         break;
      case '-':                           //comment?
         if (*sp=='-') goto rlexit;       //yes, ignore to the end
         *dp++ = c;
         HaveNonWS = true;
         break;
      case ACCENTGRAVE:
         c = SINGLEQUOTE;                 //replace with singlequote
         // FALL THROUGH to get string with closing single-quote
      case DOUBLEQUOTE:
      case SINGLEQUOTE:
         *dp++ = c;
         while (*sp && (*sp != c))
            *dp++ = *sp++;
         *dp++ = c;
         if (*sp) sp++;                //consume closing quote unless it's the end of the line
         HaveNonWS = true;
         break;
      default:
         *dp++ = c;
         HaveNonWS=true;
      }
   }
rlexit:
   *dp=0;                              //mark the end with asciz

   // Problem leaving an extra space on the valid line if
   // a comment exists further down the line... so just trim it off
   lp = lb;
   rtrim(lp);

   lc++;                            //bump line count
}

///////////////////////////////////////////////////////////////////////
// Trim whitespace off the end of a string
//in: p  points to the beginning of the string to look in

void rtrim(char *p)
{
   for ( char * q = p + strlen(p) - 1; q >= p && (*q == ' ' || *q == 0x09); q-- )
      *q = 0;
}

///////////////////////////////////////////////////////////////////////
// Preprocess a string before parsing,
//  Currently replaces score or underscore with space, as for object types
//  in early sections of the EPICS
//
//  other processes can be added if necessary
//in: str      points to the beginning of the string to be modified
void preprocstr(char *str)
{
   char* p;
   while ((p = strchr(str, '-')) != NULL)
   {
      *p = ' ';
   }
   while ((p = strchr(str, '_')) != NULL)
   {
      *p = ' ';
   }
}

///////////////////////////////////////////////////////////////////////
// Convert 2 hex characters to binary octet
//in: src      points to 2 hex chars
//    dst      points to octet to receive the value
//out:   false if a proper value was found.  True if either character is non-hex
// Does not log an error (MAY be used by Bacprim32.cpp if that still exists)

bool cvhex(char *src,octet *dst)
{
   int ch = src[0];
   if (!IsXDigit(ch))
   {
      *dst = 0;                          //set stable value
      return true;
   }
   if (IsDigit(ch))
      *dst = ch - '0';
   else
      *dst = (ch & 0xDF) - 55;

   ch = src[1];
   if (!IsXDigit(ch)) return true;
   if (IsDigit(ch))
      *dst = (*dst << 4) + (ch - '0');
   else
      *dst = (*dst << 4) + ((ch & 0xDF) - 55);

   return false;
}

///////////////////////////////////////////////////////////////////////
// non-consuming check for a character during parsing
// For convenience, strips blanks before testing.
//in: c        the expected character
//    lp       points to the next buffer character
//out:   true  the character was found, lp points at it.
//       else false and lp points at non-blank
bool PeekNextIs(char c)
{
   skipwhitespace();
   return (*lp == c);
}

///////////////////////////////////////////////////////////////////////
// check for a character during parsing
// For convenience, strips blanks before testing.
//in: c        the expected character
//    lp       points to the next buffer character
//out:   true  the character was found, lp points after it.
//       else false and lp points at non-blank
bool NextIs(char c)
{
   skipwhitespace();
   bool retval = (*lp == c);
   if (retval)
   {
      lp += 1;
   }
   return retval;
}

///////////////////////////////////////////////////////////////////////
// check for a character string during parsing.
// For convenience, strips blanks before testing.
// Checks only characters in the buffer after blank stripping.
//in: pStr     the expected string
//    lp       points to the next buffer character
//out:   true  the string was found, lp points after it.
//       else false and lp points at non-blank
bool NextIs(const char *pStr)
{
   skipwhitespace();

   int len = strlen( pStr );
   bool retval = _strnicmp( lp, pStr, len ) == 0;
   if (retval)
   {
      // Bypass the matched characters
      lp += len;
   }

   return retval;
}

///////////////////////////////////////////////////////////////////////
// check for one or more consecutive "*" or "?" characters during parsing.
// For convenience, strips blanks before testing.
//in: lp       points to the next buffer character
//out: true  one or more * were found, lp points after it/them
//     else false and lp points at non-blank
bool NextIsWildcard()
{
   // TODO: Multiple wildcard seems lame, but we have legacy EPICS
   // with things like **:**:** for time
   // TODO: 135.1 says "*", but we have legacy EPICS that use '?' within Dates etc.
   bool retval = NextIs( '*' ) || NextIs( '?' );
   while ((*lp == '*') || (*lp == '?'))
   {
      lp += 1;
   }
   return retval;
}

///////////////////////////////////////////////////////////////////////
// check for a character during parsing
// For convenience, strips blanks before testing, but DOES NOT read a new buffer
//in: c        the expected character
//    lp       points to the next buffer character
//out:   true  the character was found, lp points after it.
//       else false and lp points at non-blank
bool NextInBufferIs(char c)
{
   while (*lp == ' ') lp++;
   bool retval = (*lp == c);
   if (retval)
   {
      lp += 1;
   }
   return retval;
}

///////////////////////////////////////////////////////////////////////
// check for a character during parsing and complain if not found.
// For convenience, strips blanks before and after.
//in: c        the expected character
//    lp       points to the next buffer character
//out:   true     if error and cancel selected (also sets cancel=true)
bool MustBe(char c)
{
   skipwhitespace();
   if (*lp++ != c)
   {
      char  b[20];
      lp--;
      sprintf(b,"Expected %c here!",c);
      return tperror(b,true);
   }
   return false;
}

///////////////////////////////////////////////////////////////////////
// Body of error logging
// TODO: This function highlights the character at lp as the source of
// the error, which it often is NOT - for example an invalid enumeration.
// Ideally, would pass in begin and end pointers for the offending
// string.  We dream of a parser object to eat whitespace, and have
// pointers to start of token as well as cursor...
void LoggingGuts(CString &msg, const char *mp, bool showline)
{
   // Avoid buffer overflow headaches
   CString  str;
   char     c;

   if (showline)
   {
      str.Format("Line %u: ", lc);     // add line number
      msg += str;
   }

   if ( pfileError == NULL || !showline )
      str.Format("%s\n",mp);
   else
      str.Format("%s:  ",mp);
   msg += str;

   if (showline)
   {
      c = *lp;                         //save the character we "broke" on
      if (c != 0)
      {
         *lp = 0;                      //make it asciz there temporarily
         str.Format("%s<%c>%s\n", lb, c, lp+1);
         *lp = c;                      //restore the character we "broke" on
      }
      else
      {
         str.Format("%s\n",lb);
      }
      msg += str;
   }

   if (pfileError == NULL)
   {
      s_cancel = AfxMessageBox(msg, MB_ICONEXCLAMATION | (showline ? MB_OKCANCEL : MB_OK)) == IDCANCEL;
   }
   else
   {
      fputs(msg, pfileError);
   }
}

///////////////////////////////////////////////////////////////////////
// Display an error message in the file or dialog and update the error count
//in: mp       points to specific message
//    showline true if need to show source line and position
//out:   true  always, and s_error true
//             Formerly returned true only if cancel was selected
//             in a dialog that hasn't been shown for years.
//             The old way caused parse errors to continue trying
//             to parse, which seldom worked.
//             The new method allows parsing to abort, and allows
//             ParseProperty to skip any additional value lines until
//             the next property.
// (also sets cancel=true)
//
bool tperror(const char *mp, bool showline)
{
   CString str, msg;

   // If displaying in a dialog, add error title
   if (pfileError == NULL)
      msg +="Read Text PICS Error:\n\n";

   if (lPICSErr == -1)
   {
      // Not doing consistency checks: count a parse error
      lerrc++;
   }
   else
   {
      // Count a PICS consistency error
      lPICSErr++;
      str.Format("%d) ", lPICSErr);
      msg += str;
   }

   LoggingGuts( msg, mp, showline );

   // Circuit breaker: don't parse total junk
   if (lerrc > 200)
   {
      s_cancel = true;
   }

   s_error = true;
   return true;
}

///////////////////////////////////////////////////////////////////////
// Display a message in the file or dialog and update the error count
//in: mp       points to specific message
//    showline true if need to show source line and position
void Message(const char *mp, bool showline)
{
   CString str;
   LoggingGuts( str, mp, showline );
}


// Returns the maximum number of standard services
int GetStandardServicesSize()
{
   return sizeof(StandardServices)/sizeof(StandardServices[0]);
}

// Returns a string representing the standard service indexed by i
const char *GetStandardServicesName(int i)
{
   return (i < sizeof(StandardServices)/sizeof(StandardServices[0]))
          ? StandardServices[i]
          : "unknown-service";
}

// Returns the maximum number of potential BIBBs
int GetBIBBSize()
{
   return MAX_BIBBS;
}

// Returns a string representing the BIBB corresponding to index i
const char *GetBIBBName(int i)
{
   return BIBBs[i].name;
}

// Returns the maximum number of potential Object Types
int GetObjectTypeSize()
{
   return sizeof(StandardObjects)/sizeof(StandardObjects[0]);
}

// Returns a string representing the Object Type indexed by i
const char *GetObjectTypeName(int i)
{
   return StandardObjects[i];
}

// Returns the maximum number of potential Data Link Options
int GetDataLinkSize()
{
   return  sizeof(StandardDataLinks)/sizeof(StandardDataLinks[0]);
}

// Returns a string representing the Data Link Option indexed by i
// and any baud rate options.
// Caller must allocate the memory to hold the resulting string
// and pass in a pointer to the allocation in pstrResult.
// Allocation should be at least 200 bytes
void GetDataLinkString(int i, PICSdb *pd, char *pstrResult)
{
   int j;
   char *pz;
   dword *dp = NULL;

   if (!pstrResult)
      return;

   // Construct the first part of the string, the data link name
   pz = pstrResult;
   pz += sprintf(pz, "%s", StandardDataLinks[i]);   // get Data Link name

   // append optional baud rates
   switch (i)
   {
   case 9:                    //MS/TP master
      dp=&pd->MSTPmasterBaudRates[0];
      break;
   case 10:                   //MS/TP slave
      dp=&pd->MSTPslaveBaudRates[0];
      break;
   case 11:                   //PTP 232
      dp=&pd->PTP232BaudRates[0];
      break;
   case 12:                   //PTP modem, fixed baud rates
      dp=&pd->PTPmodemBaudRates[0];
      break;
   }

   // If list of fixed baud rates, append them to the string
   if ( (i >= 9) && (i <= 12) )
   {
      for (j=0; (j<16) && dp[j]; j++)
      {
         // If this is the first one
         if ( j == 0 )
            pz += sprintf(pz, ": %u", dp[j]);  // add first baud rate
         else
              pz += sprintf(pz, ", %u", dp[j]);  // else, add another one
      }
   }
   else if ( i == 13 )  // PTP auto-baud range - only 2 baud rates to add
   {
      pz += sprintf(pz, ": %u to %u",
         pd->PTPAutoBaud[0],
         pd->PTPAutoBaud[1] );
   }
   return;
}

// Returns the maximum number of potential Character Sets
int GetCharsetSize()
{
   return  sizeof(Charsets)/sizeof(Charsets[0]);
}

// Returns a string representing the Charset matching csTag
const char *GetCharsetName(octet csTag)
{
   int j;
   for (j=0;j<(sizeof(Charsets)/sizeof(Charsets[0]));j++)
   {
      if ((int)(Charsets[j].octetcons) == csTag)
         return Charsets[j].name;
   }
   return NULL;
}

// function to expand a packed bitstring into an array
// of ocetes, with each octet being a 0 or 1 depending on the bitstring
void ExpandBitstring(octet *pExpandedResult, octet *pBitstring, int nBits)
{
   octet mask;
   while (nBits)
   {
      for (mask = 0x80; mask && nBits; mask>>=1, nBits--)
      {
         *pExpandedResult++ = ((*pBitstring & mask) != 0);
      }
      pBitstring++;  // next 8 bits
   }
}


// Returns the maximum number of Special Functionality choices
int GetSpecialFunctionalitySize()
{
   return  sizeof(SpecialFunctionality)/sizeof(SpecialFunctionality[0]);
}

// Returns a Special Functionality string at index i
const char *GetSpecialFunctionalityName(int i)
{
   return SpecialFunctionality[i];
}

//////////////////////////////////////////////////////////////////////////////
// Run all EPICS Consistency checks specified by standard 135.1-2013.
//
// 5.  EPICS CONSISTENCY TESTS
//
// Each implementation shall be tested to ensure consistency among
// interrelated data elements. These tests shall include:
//
// (a) All object types required by the specified BIBBs shall be indicated
// as supported in the Standard Object Types Supported section of the
// EPICS.
//
// (b) A minimum of one instance of each object type required by the
// specified BIBBs shall be included in the test database.
//
// (c) The Object_Types_Supported property of the Device object in the test
// database shall indicate support for each object type required by the
// supported BIBBs.
//
// (d) All application services required by the supported BIBBs shall be
// indicated as supported in the BACnet Standard Application Services
// Supported section of the EPICS with Initiate and Execute indicated as
// required by the supported BIBBs.
//
// (e) The Application_Services_Supported property of the Device object in
// the test database shall indicate support for each application service
// for which the supported BIBBs requires support for execution of the
// service.
//
// (f) The object types listed in the Standard Object Types Supported
// section of the EPICS shall have a one-to-one correspondence with object
// types listed in the Object_Types_Supported property of the Device object
// contained in the test database.
//
// (g) For each object type listed in the Standard Object Types Supported
// section of the EPICS there shall be at least one object of that type in
// the test database.
//
// (h) There shall be a one-to-one correspondence between the objects
// listed in the Object_List property of the Device object and the objects
// included in the test database. The Object_List property and the test
// database shall both include all proprietary objects. Properties of
// proprietary objects that are not required by BACnet Clause 23.4.3 need
// not be included in the test database.
//
// (i) For each object included in the test database, all required
// properties for that object as defined in Clause 12 of BACnet shall be
// present. In addition, if any of the properties supported for an object
// require the conditional presence of other properties, their presence
// shall be verified.
//
// (j) For each property that is required to be writable, that property
// shall be marked as writable in the EPICS.
//
// (k) The length of the Protocol_Services_Supported bitstring shall have
// the number of bits defined for BACnetProtocolServicesSupported for the
// IUT's declared protocol revision.
//
// (l) The length of the Protocol_Object_Types_Supported bitstring shall
// have the number of bits defined for BACnetObjectTypesSupported for the
// IUT's declared protocol revision.
//
void CheckPICSConsistency2013(PICSdb *pd)
{
   cConsistencyErrors=0;  // Reset global PICS error count.

   // Make sure that each Object Type required by a BIBB
   // exists in the Standard Object Types Supported section,
   // as well as cross dependencies between BIBBs.
   // 135.1-2013, section 5.(a)
   CheckPICSCons2013A(pd);

   // 135.1-2013, section 5.(b) is covered by test (g) below

   // 135.1-2013, section 5.(c) is covered by test (f) below

   // 135.1-2013, section 5.(d) test
   // Make sure application services required by each BIBB are
   // listed in the Standard Application Services Supported section.
   CheckPICSCons2013D(pd);

   // 135.1-2013, section 5.(e) test
   // Make sure the services marked "Execute" in the
   // 'BACnet Standard Application Services Supported' section match
   // the services marked supported in the Application_Services_Supported
   // Property of the Device Object.
   // This is a two-way check.
   CheckPICSCons2013E(pd);

   // 135.1-2013, section 5.(f)
   // Make sure the Objects listed in the Standard Object Types section
   // match the bits in the Objects_Types_Supported property of the Device.
   CheckPICSCons2013F(pd);

   // Make sure each Object type in the Standard Object Types Supported section
   // is represented in the test database.
   // 135.1-2013, section 5.(b) & 5.(g)
   CheckPICSCons2013G(pd);

   // 135.1-2013, section 5.(h)
   CheckPICSCons2013H(pd);

   // 135.1-2013, section 5.(i)
   // 135.1-2013, section 5.(j)
   CheckPICSCons2013I(pd);

   // 135.1-2013, section 5.(k)
   CheckPICSCons2013K(pd);

   // 135.1-2013, section 5.(l)
   CheckPICSCons2013L(pd);

#if _DEBUG
   // TODO: Regression test: compare strdobjpr tables to DUDTOOL tables
   // to find unimplemented properties and object types.
   CString str;
   for (int objType = 0; objType < MAX_DEFINED_OBJ; objType++)
   {
      const char *pTypeName = NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString( objType );
      int nProps;
      const BACnetPropertyIdentifier *pPropList = GetObjectTable( (BACnetObjectType)objType, nProps );
      if (pPropList == NULL)
      {
         str.Format( "TODO: Missing BACnetPropertyIdentifier list for type %s", pTypeName );
         tperror(str, false);
      }
      else
      {
         const propdescriptor *pDesc = GetPropDescriptorTable( objType );
         if (pDesc == NULL)
         {
            str.Format( "TODO: Missing propdescriptor table for type %s", pTypeName );
            tperror(str, false);
         }
         else if (pDesc == PlaceholderProps)
         {
            str.Format( "TODO: Has only placeholder propdescriptor for type %s", pTypeName );
            tperror(str, false);
         }
         else
         {
            for (int ix = 0; ix < nProps; ix++)
            {
               BACnetPropertyIdentifier propID = pPropList[ix];
               const char *pPropName = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( propID );

               // Is this property in the mapping table?
               bool found = false;
               const propdescriptor *pD = pDesc;
               while (true)
               {
                  if (pD->PropID == propID)
                  {
                     found = true;
                     break;
                  }

                  if (pD->PropGroup & LAST)
                     break;

                  pD++;
               }

               if (!found)
               {
                  str.Format( "TODO: %s propdescriptor is missing %s", pTypeName, pPropName );
                  tperror(str, false);
               }
            }
         }
      }
   }
#endif
}


/////////////////////////////////////////////////////////////////////////////
// Some BIBBs require support for other BIBBs.
// This is a helper function to perform that cross check.
// iSupportedBIBB = the BIBB that requires another BIBB.
// iDependentBIBB = the BIBB that iSupportedBIBB requires.
//
void CheckPICS_BIBB_Cross_Dependency(PICSdb *pd, int iSupportedBIBB, int iDependentBIBB)
{
   char opj[200];
   if ( !pd->BIBBSupported[iDependentBIBB] )
   {
     sprintf(opj,"BIBB %s requires support for BIBB %s.\n",
             BIBBs[iSupportedBIBB].name,
             BIBBs[iDependentBIBB].name );
     tperror(opj,false);
   }
   return;
}


///////////////////////////////////////////////////////////////////////////////
// EPICS Consistency test specified by 135.1-2013, section 5(a)
//
// (a) All object types required by the specified BIBBs shall be indicated
// as supported in the Standard Object Types Supported section of the
// EPICS. (Similar to the old test "A", but using BIBBs instead of
// conformance class.)
//
// This function also runs checks for cross-BIBB dependencies,
// which is not a test that is specified in 135.1-2013 section 5,
// but is clearly stated in the BIBB definition section of the BACnet standard.
//
// In addition, this function also checks for specific properties
// that must be present for certain BIBBs. TODO
//
void CheckPICSCons2013A(PICSdb *pd)
{
   int i;
   char opj[300];
   for (i = 0; i < MAX_BIBBS; i++ )
   {
      // scan for supported BIBBs
      if (pd->BIBBSupported[i])
      {
         switch (i)
         {
            case bibbAE_N_I_B:  // requires support for either NC or EE Objects
               if ( (pd->BACnetStandardObjects[OBJ_NOTIFICATIONCLASS]==soNotSupported) &&
                    (pd->BACnetStandardObjects[OBJ_EVENT_ENROLLMENT]==soNotSupported) )
               {
                 sprintf(opj,"135.1-2013 5.(a): "
                   "BIBB AE-N-I-B requires support for Intrinsic or Algorithmic reporting, "
                   "which implies support for the Event Enrollment or Notification Class object "
                   "in the \"Standard Object Types Supported\" section.\n");
                 tperror(opj,false);
               }
               break;

            case bibbAE_N_E_B: // requires support for AE-N-I-B & DS-RP-A & EE Object
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbAE_N_I_B);
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbDS_RP_A);
               if ( pd->BACnetStandardObjects[OBJ_EVENT_ENROLLMENT]==soNotSupported )
               {
                 sprintf(opj,"135.1-2013 5.(a): "
                   "BIBB AE-N-E-B requires support for the Event Enrollment object "
                   "in the \"Standard Object Types Supported\" section.\n");
                 tperror(opj,false);
               }
               break;

            case bibbAE_LS_B: // requires support for Life Safety Point or Life Safety Zone object
               if ( (pd->BACnetStandardObjects[OBJ_LIFE_SAFETY_POINT]==soNotSupported) &&
                    (pd->BACnetStandardObjects[OBJ_LIFE_SAFETY_ZONE]==soNotSupported) )
               {
                 sprintf(opj,"135.1-2013 5.(a): "
                   "BIBB AE-LS-B requires support for the Life Safety Point or Life Safety Zone object "
                   "in the \"Standard Object Types Supported\" section.\n");
                 tperror(opj,false);
               }
               break;

            case bibbSCHED_A:  // requires support for DS-RP-A and DS-WP-A
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbDS_WP_A);
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbDS_RP_A);
               break;

            case bibbSCHED_I_B:  // requires DS-RP-B, DS-WP-B, Calendar & Schedule objects
                                 // and either DM-TS-B or DM-UTC-B
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbDS_RP_B);
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbDS_WP_B);
               if ( pd->BACnetStandardObjects[OBJ_CALENDAR]==soNotSupported )
               {
                 sprintf(opj,"135.1-2013 5.(a): "
                   "BIBB SCHED-I-B requires support for the Calendar object "
                   "in the \"Standard Object Types Supported\" section.\n");
                 tperror(opj,false);
               }
               if ( pd->BACnetStandardObjects[OBJ_SCHEDULE]==soNotSupported )
               {
                 sprintf(opj,"135.1-2013 5.(a): "
                   "BIBB SCHED-I-B requires support for the Schedule object "
                   "in the \"Standard Object Types Supported\" section.\n");
                 tperror(opj,false);
               }
               if ( !pd->BIBBSupported[bibbDM_TS_B] &&
                    !pd->BIBBSupported[bibbDM_UTC_B] )
               {
                 sprintf(opj,"BIBB SCHED-I-B requires support for either DM-TS-B or DM-UTC-B.\n");
                 tperror(opj,false);
               }
               break;

            case bibbSCHED_E_B:  // requires support for SCHED-I-B and DS-WP-A
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbSCHED_I_B);
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbDS_WP_A);
               break;

            case bibbSCH_E_B:
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbSCH_I_B);
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbDS_WP_A);
               break;

            case bibbT_VMT_I_B:  // both require support for TREND_LOG object
            case bibbT_ATR_B:
               if ( pd->BACnetStandardObjects[OBJ_TREND_LOG]==soNotSupported )
               {
                 sprintf(opj,"135.1-2013 5.(a): "
                   "BIBB %s requires support for the Trend Log object "
                   "in the \"Standard Object Types Supported\" section.\n",
                   BIBBs[i].name );
                 tperror(opj,false);
               }
               break;

            case bibbT_VMT_E_B:  // requires support for T-VMT-I-B and DS-RP-A
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbT_VMT_I_B);
               CheckPICS_BIBB_Cross_Dependency(pd,i,bibbDS_RP_A);
               break;

            case bibbDM_TS_A:
            case bibbDM_UTC_A:
               // requires presence of Time_Synchronization_Recipients
               // property in the Device Object.
               // TODO: implement me
               break;

            case bibbDM_UTC_B: // Same requirements as DM_TS_B, but adds
                               // the requirement for the presence of UTC_Offset
                               // and Daylight_Saving_Status in the Device Object.
                               // (deliberate fall-through to next case)
            case bibbDM_TS_B:  // requires the presence of the Local_Time and Local_Date
                               // properties of the Device Object.
                               // TODO
               break;

            case bibbDM_R_B:   // requires the presence of the Time_Of_Device_Restart
                               // and Last_Restart_Reason properties
                               // in the Device Object.
                               // TODO
               break;

         // LJT Updated this list with the new Workstation Bibbs and their dependencies
         case bibbDS_V_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_A);
            break;
         case bibbAE_VN_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbAE_N_A);
            break;
         case bibbAE_AVN_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbAE_VN_A);
            break;
         case bibbAE_VM_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_A);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_WP_A);
            break;
         case bibbAE_AVM_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_A);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_WP_A);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDM_OCD_A);
            break;
         case bibbSCH_AVM_A:
         case bibbSCHED_AVM_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_A);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_WP_A);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDM_OCD_A);
            break;
         case bibbSCH_VM_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_A);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_WP_A);
            break;
         case bibbSCH_WS_A:
         case bibbSCHED_WS_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_A);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_WP_A);
            break;
         case bibbSCH_WS_I_B:
         case bibbSCHED_WS_I_B:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_B);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_WP_B);
            if ( !pd->BIBBSupported[bibbDM_TS_B] &&
                !pd->BIBBSupported[bibbDM_UTC_B] )
            {
               sprintf(opj,"BIBB SCHED-WS-I-B requires support for either DM-TS-B or DM-UTC-B.\n");
               tperror(opj,false);
            }
            if ( pd->BACnetStandardObjects[OBJ_SCHEDULE]==soNotSupported )
            {
               sprintf(opj,"135.1-2013 5.(a): "
                       "BIBB SCHED-WS-I-B requires support for the Schedule object "
                       "in the \"Standard Object Types Supported\" section.\n");
                tperror(opj,false);
            }
            break;
         case bibbSCH_R_B:
         case bibbSCHED_R_B:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_B);
            if ( !pd->BIBBSupported[bibbDM_TS_B] &&
                 !pd->BIBBSupported[bibbDM_UTC_B] )
            {
               sprintf(opj,"BIBB SCHED-R-B requires support for either DM-TS-B or DM-UTC-B.\n");
               tperror(opj,false);
            }
            if ( pd->BACnetStandardObjects[OBJ_SCHEDULE]==soNotSupported )
            {
               sprintf(opj,"135.1-2013 5.(a): "
                       "BIBB SCHED-R-B requires support for the Schedule object "
                      "in the \"Standard Object Types Supported\" section.\n");
                tperror(opj,false);
            }
            break;
         case bibbT_AVM_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_A);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_WP_A);
            break;
         case bibbT_A_A:
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbT_ATR_A);
            break;
         case bibbDM_ANM_A:
            // 17-May-2010 DDO removed erroneous Dependency upon DM_DDB_B
            // CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDM_DDB_B);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDM_DDB_A);
            break;
         case bibbDM_ADM_A:
            // 17-May-2010 DDO removed erroneous Dependency upon DS_AV_A
            // replacing it with correct Dependency upon DS_RP_A
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_A);
            break;

         case bibbT_VMMV_A:  // Deprecated in 135-2012
            break;
         case bibbT_VMMV_I_B:
            // Must support Trend-multiple object
            if ( pd->BACnetStandardObjects[OBJ_TREND_LOG_MULTIPLE]==soNotSupported )
            {
               sprintf(opj,"135.1-2013 5.(a): "
                       "BIBB T-VMMV-I-B requires support for the Trend Multiple object "
                      "in the \"Standard Object Types Supported\" section.\n");
                tperror(opj,false);
            }
            break;
         case bibbT_VMMV_E_B:
            if ( pd->BACnetStandardObjects[OBJ_TREND_LOG_MULTIPLE]==soNotSupported )
            {
               sprintf(opj,"135.1-2013 5.(a): "
                       "BIBB T-VMMV-E-B requires support for the Trend Multiple object "
                      "in the \"Standard Object Types Supported\" section.\n");
                tperror(opj,false);
            }
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbT_VMMV_I_B);
            CheckPICS_BIBB_Cross_Dependency(pd, i, bibbDS_RP_A);
            break;
         case bibbT_AMVR_A:
            break;
         case bibbT_AMVR_B:
            if ( pd->BACnetStandardObjects[OBJ_TREND_LOG_MULTIPLE]==soNotSupported )
            {
               sprintf(opj,"135.1-2013 5.(a): "
                       "BIBB T-AMVR-B requires support for the Trend Multiple object "
                      "in the \"Standard Object Types Supported\" section.\n");
                tperror(opj,false);
            }
            break;
         }
      }
   }

   return;
}


///////////////////////////////////////////////////////////////////////////////
// EPICS Consistency test specified by 135.1-2013, section 5(d)
//
// (d) All application services required by the supported BIBBs shall be
// indicated as supported in the BACnet Standard Application Services
// Supported section of the EPICS with Initiate and Execute indicated as
// required by the supported BIBBs. (Similar to the old test "D", but using
// BIBBs instead of conformance class.)
//
void CheckPICSCons2013D(PICSdb *pd)
{
   int i;
   char opj[300];
   char *errmsg;
   bibb_service *pBibb_rule;  // pointer to each bibb_service rule in the BIBB definition
   for (i = 0; i < MAX_BIBBS; i++ )
   {
      // Scan for supported BIBBs, cross checking the services supported section
      // when support id found for a BIBB
      if (pd->BIBBSupported[i])
      {
         pBibb_rule = BIBBs[i].aBIBB_Service;  // point to first BIBB rule
         while (pBibb_rule->InitExec) // while not at end of this BIBB's rules
         {
            // If the BIBB rule requires a service that has not been
            // defined in the StandardServices section, this is an error.
            if ((pd->BACnetStandardServices[pBibb_rule->ApplServ] & pBibb_rule->InitExec)==0)
            {
               if (pBibb_rule->InitExec == ssInitiate)
                  errmsg = "Initiate";
               else
                  errmsg = "Execute";

               sprintf(opj,"135.1-2013 5.(d): "
                       "BIBB %s requires the Device to %s the %s application service,"
                       " and support for this service has not been included in the "
                       "\"BACnet Standard Application Services Supported\" section.\n",
                       BIBBs[i].name,
                       errmsg,
                       GetStandardServicesName( pBibb_rule->ApplServ ));
               tperror(opj,false);
            }
            pBibb_rule++;  // next rule please
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
// EPICS Consistency test specified by 135.1-2013, section 5(e)
//
// (e) The Application_Services_Supported property of the Device object in
// the test database shall indicate support for each application service
// for which the supported BIBBs requires support for execution of the
// service. (Similar to the old test "E", but using BIBBs instead of
// conformance class.)
//
// This function does not actually use the BIBBs for reference since
// the consistency check between the BIBBS and the Application Services Supported
// section has already been done. It runs a consistency check between the
// Application Services Supported section and the Application_Services_Supported
// property of the Device Object, but only for the "Execute" services.
//
void CheckPICSCons2013E(PICSdb *pd)
{
   int i;
   char opj[300];
   octet Application_Services_Supported[MAX_BITSTRING]; // one byte per bit
   octet InStandardAppSection;
   int  iNumStandardServices;
   memset(Application_Services_Supported,0,MAX_BITSTRING);

   iNumStandardServices = GetStandardServicesSize();
   if (!pd->pDeviceObject)
      return;
   ExpandBitstring(Application_Services_Supported,pd->pDeviceObject->protocol_services_supported, iNumStandardServices);

   for (i=0; i<iNumStandardServices; i++)
   {
      InStandardAppSection = ((pd->BACnetStandardServices[i] & ssExecute) != 0);
      // Exclusive OR test between the property and the EPICS section.
      // If either is set and not the other, this is an error.
      if( (InStandardAppSection ^ Application_Services_Supported[i]) )  // XOR
      {
         sprintf(opj,"135.1-2013 5.(e): "
                 "Support for execution of the %s application service is not consistent "
                 "between the \"BACnet Standard Application Services Supported\" "
                 "section and the Application_Services_Supported Property of the Device Object.\n",
                 GetStandardServicesName( i ));
         tperror(opj,false);
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
// EPICS Consistency test specified by 135.1-2013, section 5(c) & 5(f)
//
// Since the 2013A test covers which Objects should be in the
// Standard Object Types Supported section for each BIBB,
// this function just needs to check that each object listed in that
// section is indeed defined in the Object_Types_Supported property
// of the Device Object in the database.  This covers both of the
// following consistency checks in 135.1-2013:
//
// (c) The Object_Types_Supported property of the Device object in the test
// database shall indicate support for each object type required by the
// supported BIBBs.  (Similar to the old test "C", but using BIBBs instead
// of conformance class.)
//
// (f) The object types listed in the Standard Object Types Supported
// section of the EPICS shall have a one-to-one correspondence with object
// types listed in the Object_Types_Supported property of the Device object
// contained in the test database.  (Identical to the old test "K".)
//
void CheckPICSCons2013F(PICSdb *pd)
{
   char errMsg[300];
   int  i, iNumStandardObjects;
   octet InStandardObjectSection;
   octet ProtocolObjectSup[MAX_BITSTRING];

   memset(ProtocolObjectSup,0,MAX_BITSTRING);
   iNumStandardObjects = GetObjectTypeSize();
   if (!pd->pDeviceObject)
      return;
   ExpandBitstring(ProtocolObjectSup,pd->pDeviceObject->object_types_supported, iNumStandardObjects);

   for(i=0;i<MAX_DEFINED_OBJ;i++)
   {
      InStandardObjectSection = (pd->BACnetStandardObjects[i] != 0); // convert to boolean
      // Exclusive OR test between the property and the EPICS section.
      // If either is set and not the other, this is an error.
      if( (InStandardObjectSection ^ ProtocolObjectSup[i]) )  // XOR
      {
         sprintf(errMsg,"135.1-2013 5.(f): "
            "Object type %s is not consistent between the \"Standard Object Types Supported\" "
            "section and the Object_Types_Supported Property of the Device Object.\n", StandardObjects[i]);
         tperror(errMsg,false);
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
// EPICS Consistency test specified by 135.1-2013, section 5(b) & 5(g)
//
// Since the 2013A test covers which Objects should be in the
// Standard Object Types Supported section for each BIBB,
// this function just needs to check that each object listed in that
// section is indeed in the database.  This covers both of the
// following consistency checks in 135.1-2013:
//
// (b)   A minimum of one instance of each object type required by the
// specified BIBBs shall be included in the test database. (Similar to the
// old test "B", but using BIBBs instead of conformance class.)
//
// (g) For each object type listed in the Standard Object Types Supported
// section of the EPICS there shall be at least one object of that type in
// the test database. (Identical to the old test "L".)
//
void CheckPICSCons2013G(PICSdb *pd)
{
   char errMsg[300];
   for(int i=0;i<MAX_DEFINED_OBJ;i++)
   {
      if( (pd->BACnetStandardObjects[i]) && !(ObjInTestDB[i].ObjIDSupported))
      {
         sprintf(errMsg,"135.1-2013 5.(g): "
            "Object type %s is listed in the \"Standard Object Types Supported\" section "
            "but no Objects of that type are defined in the test database.\n",StandardObjects[i]);
         tperror(errMsg,false);
      }
   }
}

// Scans the test database looking for an object with
// a matching ObjectID.  If found, returns a pointer to the Object.
// If not found, returns NULL.
generic_object *FindObjectInDatabase(PICSdb *pd, dword ObjectID)
{
   generic_object *po;
   // pointer to the start of objects in the test database
   po = pd->Database;
   while (po)
   {
      if (po->object_id == ObjectID)
         return po;
      po=(generic_object *)po->next;
   }
   return NULL;
}

// Finds an Object matching the ObjectID in the Object_List
// propery of the Device Object defined in the test database.
// If found, returns true.
// If not found, returns false.
bool FindObjectInObjectList(PICSdb *pd, dword ObjectID)
{
   BACnetObjectIdentifier *pObjectID;
   if (pd->pDeviceObject == NULL)
      return false; // Device Object not parsed yet

   // Pointer to the parsed Object_List property of the Device Object
   pObjectID = pd->pDeviceObject->object_list;

   while (pObjectID)
   {
      if (pObjectID->object_id == ObjectID)
         return true;
      pObjectID = pObjectID->next;
   }
   // TODO: Add check of structured_object_list here?
   return false;
}

///////////////////////////////////////////////////////////////////////////////
// EPICS Consistency test specified by 135.1-2013, section 5(h)
//
// (h) There shall be a one-to-one correspondence between the objects
// listed in the Object_List property of the Device object and the objects
// included in the test database. The Object_List property and the test
// database shall both include all proprietary objects. Properties of
// proprietary objects that are not required by BACnet Clause 23.4.3 need
// not be included in the test database. (Identical to the old test "M".)
//
// This test first steps through the Object_List and verifies that
// every Object found in the Object_List has been defined in the test database.
// Then it does a reverse lookup by stepping through the test database
// and making sure that every Object found is also in the Object_List.
// This strategy should uncover any and all mismatches.
//
void CheckPICSCons2013H(PICSdb *pd)
{
   char errMsg[300];
   generic_object *po;
   BACnetObjectIdentifier *pObjectID;
   dword id, objinstance; // temps
   word  objtype;

   dword dwMaxStdObj = sizeof(StandardObjects)/sizeof(StandardObjects[0]);

   // pointer to the start of objects in the test database
   po = pd->Database;
   if (po == NULL)
      return;
   // Pointer to the parsed Object_List property of the Device Object
   if (pd->pDeviceObject == NULL)
      return;
   pObjectID = pd->pDeviceObject->object_list;

   // If Object_List has not been parsed, we cannot do this test.
   if (!pObjectID)
      return;

   // Step through the Object_List
   while (pObjectID)
   {
      id = pObjectID->object_id;
      if (FindObjectInDatabase(pd, id) == NULL)
      {
         SplitObjectId(id, &objtype, &objinstance);
         if (objtype < dwMaxStdObj)
         {
            sprintf(errMsg,"135.1-2013 5.(h): "
              "(%s,%u) is defined in the Object_List but does not appear in the test database.\n",
              StandardObjects[objtype], objinstance );
         }
         else
         {
            sprintf(errMsg,"135.1-2013 5.(h): "
              "(proprietary %u,%u) is defined in the Object_List but does not appear in the test database.\n",
              objtype, objinstance );
         }
         tperror(errMsg,false);
      }
      pObjectID = pObjectID->next; // next in the Object_List
   }

   // Now step through the test database and verify that every object in the database
   // is also in the Object_List.
   while (po)
   {
      id=po->object_id;
      if (!FindObjectInObjectList(pd, id))
      {
         SplitObjectId(id, &objtype, &objinstance);
         if (objtype < dwMaxStdObj)
         {
            sprintf(errMsg,"135.1-2013 5.(h): "
              "(%s,%u) is defined in the test database but does not appear in the Object_List.\n",
              StandardObjects[objtype], objinstance );
         }
         else
         {
            sprintf(errMsg,"135.1-2013 5.(h): "
              "(proprietary %u,%u) is defined in the test database but does not appear in the Object_List.\n",
              objtype, objinstance );
         }
         tperror(errMsg,false);
      }
      po=(generic_object *)po->next;
   }
   return;
}

// Helper function for CheckPICSCons2013I.
// This function is called for each object in the test database
// to check if required (and writable) properties are present.
// This function also handles the special case of the Device Object,
// which applies conditions that are outside the scope of
// the Device Object itself.
void CheckPICSConsProperties(PICSdb *pd, generic_object *obj)
{
   char   errMsg[300];
   word   objtype;
   dword  objInstance;
   const  propdescriptor *propdesc;
   octet  groupRequired[128]; // flags corresponding to PropGroup
   int    i = 0;
   octet  group = 0;

   // We need to know DUT's revision.  If known, use 0
   word dutRevision = 0;
   if (pd->pDeviceObject != NULL)
   {
      dutRevision = pd->pDeviceObject->protocol_rev;
   }

   // Clear group detection flags
   memset(groupRequired, 0, sizeof(groupRequired));

   // derive object type and object instance number from object ID
   SplitObjectId(obj->object_id, &objtype, &objInstance);

   // set property descriptor to point to property descriptors for this object type
   propdesc = GetPropDescriptorTable( objtype );

   // loop through the standard properties for this object type
   while (1)
   {
      // If property is required, and it was not parsed in EPICS, log an error.
      if ( (propdesc->PropFlags & R) && !(obj->propflags[i] & PropIsPresent) &&
           (dutRevision >= propdesc->firstRevision) )
      {
         sprintf(errMsg,
                 "135.1-2013 5.(i): (%s,%u) must contain required property \"%s\".\n",
                 StandardObjects[objtype], objInstance,
                 NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( propdesc->PropID ));
         tperror(errMsg,false);
      }
      // If the property is present, and the property is required to be writable,
      // but is not marked with a 'W' in EPICS, log an error.
      // We exclude Commandable properties from this check and properties that
      // are only writable when Out_Of_Service is set.
      else if ( (propdesc->PropFlags & W) &&
                !(propdesc->PropFlags & (IsCommandable | Woutofservice)) &&
                !(obj->propflags[i] & PropIsWritable) &&
                (dutRevision >= propdesc->firstRevision) )
      {
         sprintf(errMsg,
                 "135.1-2013 5.(j): (%s,%u) must have property \"%s\" marked writable.\n",
                 StandardObjects[objtype], objInstance,
                 NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( propdesc->PropID ));
         tperror(errMsg,false);
      }
      else if ( propdesc->PropFlags & AtLeast1 )
      {
         // TODO: special property here -- need to find at least 1 of these when we do we can stop looking.
      }

      // Test for the presence of properties that are marked with 'footnotes'
      // that cause other properties to be present if this property is present.
      // These are remembered for a second pass through the properties to
      // check for missing properties.
      group = propdesc->PropGroup & ~LAST; // mask off the "Last" indicator bit
      // If this property is in a "group" (footnote), and it is present in the database,
      // mark this whole group as required.
      if (group && (obj->propflags[i] & PropIsPresent) ) //&& !(propdesc->PropFlags&&AtLeast1 == AtLeast1))  // LJT 4/17/2008 added AtLeast1Check
         groupRequired[group] = 1;

      if (propdesc->PropGroup & LAST)
         break;  // if this is the last property definition, exit loop
      propdesc++; // next propertydefinition for this object
      i++;  // next index into propflags
   }

   ///////////////////
   // !! 2nd pass !!
   //////////////////

   // reset property descriptor to point to property descriptors for this object type
   propdesc = StdObjects[objtype].sotProps;
   i = 0;

   // Make a second pass through the standard properties, looking for
   // missing "footnoted" properties that belong to groups that
   // were detected during the first pass.
   if (objtype != OBJ_DEVICE)  // don't do these checks on the Device Object (see else below)
   {
      while (1)
      {
         group = propdesc->PropGroup & ~LAST;
         // If property belongs to a group, and another property was detected
         // in this same group in the first pass, and this property
         // is not present in the database,log an error.
         if ( group && groupRequired[group] && !(obj->propflags[i] & PropIsPresent) &&
              (dutRevision >= propdesc->firstRevision) )
         {
            sprintf(errMsg,
                    "135.1-2013 5.(i): (%s,%u) must contain conditionally required property \"%s\".\n",
                    StandardObjects[objtype], objInstance,
                    NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( propdesc->PropID ));
            tperror(errMsg,false);
         }

         // Are we done?
         if (propdesc->PropGroup & LAST)
            break;  // if this is the last property definition, exit loop
         propdesc++; // next propertydefinition for this object
         i++;  // next index into propflags
      }
   }
   else // objtype == OBJ_DEVICE
   {
      // These are special tests run on the Device Object to check for properties that
      // are conditionally required based on support for certain *services* or
      // data link options.
      while (1)
      {
         int required;
         required = 0;
         switch (propdesc->PropID)
         {
            case MAX_SEGMENTS_ACCEPTED:
            case APDU_SEGMENT_TIMEOUT:
               // If segmentation is supported, these are required.
               if ( pd->SegmentedRequestWindow || pd->SegmentedResponseWindow )
                  required = R;
               break;

            case VT_CLASSES_SUPPORTED:
            case ACTIVE_VT_SESSIONS:
               // If VT services are supported, these are required.
               if ( (pd->BACnetStandardServices[asVT_Open] & ssExecute) ||
                    (pd->BACnetStandardServices[asVT_Close] & ssExecute) ||
                    (pd->BACnetStandardServices[asVT_Data] & ssExecute) )
                   required = R;
               break;

            case LOCAL_TIME:
            case LOCAL_DATE:
               // If the device supports the execution of TimeSynchronization
               // or UTCTimeSynchronization, these are required.
               if ( pd->BACnetStandardServices[asTimeSynchronization] & ssExecute )
                   required = R;
               // !!! WARNING !!! - deliberate fall-through to the next case - no break!
            case UTC_OFFSET:
            case DAYLIGHT_SAVINGS_STATUS:
                // If the device supports the execution of UTCTimeSynchronization, these are required.
               if ( pd->BACnetStandardServices[asUTCTimeSynchronization] & ssExecute )
                   required = R;
               break;

            case TIME_SYNCHRONIZATION_RECIPIENTS:
               // If supports DM-TS-A or DM-UTC-A, this must be present & WRITABLE!
               // If supports DM-ATS-A (but not for DM-MTS-A), also requires this present & WRITABLE!
               if ( pd->BIBBSupported[bibbDM_TS_A] || pd->BIBBSupported[bibbDM_UTC_A]
                    || pd->BIBBSupported[bibbDM_ATS_A] )
                  required = W;
               break;

            case MAX_MASTER:
            case MAX_INFO_FRAMES:
               // If MS/TP Master, these are required
               if ( pd->DataLinkLayerOptions[9] )  // ugly hard-coded 9
                  required = R;
               break;

            case CONFIGURATION_FILES:
            case LAST_RESTORE_TIME:
               // If Backup and restore is supported, these must be present.
               if ( pd->BIBBSupported[bibbDM_BR_B] )
                  required = R;
               break;

            case BACKUP_FAILURE_TIMEOUT:
               // If Backup and restore is supported, this must be present & WRITABLE!
               if ( pd->BIBBSupported[bibbDM_BR_B] )
                  required = W;
               break;

            case ACTIVE_COV_SUBSCRIPTIONS:
                // If the device supports the execution of SubscribeCOV or SubscribeCOVProperty,
                // this property is required.
               if  ( (pd->BACnetStandardServices[asSubscribeCOV]         & ssExecute) ||
                     (pd->BACnetStandardServices[asSubscribeCOVProperty] & ssExecute) )
                   required = R;
               break;
         }

         // If property is required, and it was not parsed in EPICS, log an error.
         if (required && !(obj->propflags[i] & PropIsPresent) &&
             (dutRevision >= propdesc->firstRevision))
         {
            sprintf(errMsg,
                    "135.1-2013 5.(i): (%s,%u) must contain conditionally required property \"%s\".\n",
                    StandardObjects[objtype],
                    objInstance,
                    NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( propdesc->PropID ));
            tperror(errMsg,false);
         }
         // If property must be writable and is not, log an error.
         else if ( (required & W) && !(obj->propflags[i] & PropIsWritable) &&
                   (dutRevision >= propdesc->firstRevision) )
         {
            sprintf(errMsg,
                    "135.1-2013 5.(i): (%s,%u) must have property \"%s\" marked writable.\n",
                    StandardObjects[objtype],
                    objInstance,
                    NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( propdesc->PropID ));
            tperror(errMsg,false);
         }

         // Are we done?
         if (propdesc->PropGroup & LAST)
            break;  // if this is the last property definition, exit loop
         propdesc++; // next propertydefinition for this object
         i++;  // next index into propflags
      }
   }

   return;
}


///////////////////////////////////////////////////////////////////////////////
// EPICS Consistency test specified by 135.1-2013, section 5(i) & (j)
//
// (i) For each object included in the test database, all required
// properties for that object as defined in Clause 12 of BACnet shall be
// present. In addition, if any of the properties supported for an object
// require the conditional presence of other properties, their presence
// shall be verified. (Identical to the old test "N".)
//
// (j) For each property that is required to be writable, that property
// shall be marked as writable in the EPICS. (New test.)
//
void CheckPICSCons2013I(PICSdb *pd)
{
   generic_object *obj;
   obj=pd->Database;
   // Loop through the entire database ...
   while(obj)
   {
      // TODO: check for required, conditionally required, and mandatory writable properties

     // Ignore proprietary object
     if(obj->object_type < NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nReserved)
        CheckPICSConsProperties(pd, obj);

     obj=(generic_object *)obj->next;
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////
// EPICS Consistency test specified by 135.1-2013, section 5(k)
//
// (k) The length of the Protocol_Services_Supported bitstring shall have
// the number of bits defined for BACnetProtocolServicesSupported for the
// IUT's declared protocol revision.  (New test.)
//
void CheckPICSCons2013K(PICSdb *pd)
{
   char   errMsg[300];
   UINT ExpectedLength;
   device_obj_type *pDevice = pd->pDeviceObject;
   // Make sure we have a pointer to the Device Object.
   if (!pDevice)
      return;

   // Bounds check.  If protocol_revision is a higher number than what we know about,
   // skip this test.
   if (pDevice->protocol_rev >= (sizeof(aCorrectLengthProtocolServicesSupportedBitstring)/
                                sizeof(aCorrectLengthProtocolServicesSupportedBitstring[0])) )
      return;

   ExpectedLength = aCorrectLengthProtocolServicesSupportedBitstring[pDevice->protocol_rev];
   if (EPICSLengthProtocolServicesSupportedBitstring != ExpectedLength)
   {
      sprintf(errMsg,"135.1-2013 5.(k): "
         "For Protocol_Revision %d, the length of Protocol_Services_Supported "
         "must be %d bits, but it is %d bits.\n",
         pDevice->protocol_rev,
         ExpectedLength,
         EPICSLengthProtocolServicesSupportedBitstring);
      tperror(errMsg,false);
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////
// EPICS Consistency test specified by 135.1-2013, section 5(l)
//
// (l) The length of the Protocol_Object_Types_Supported bitstring shall
// have the number of bits defined for BACnetObjectTypesSupported for the
// IUT's declared protocol revision. (New test.)
//
void CheckPICSCons2013L(PICSdb *pd)
{
   char   errMsg[300];
   UINT ExpectedLength;
   device_obj_type *pDevice = pd->pDeviceObject;
   // Make sure we have a pointer to the Device Object.
   if (!pDevice)
      return;

   // Bounds check.  If protocol_revision is a higher number than what we know about,
   // skip this test.
   if (pDevice->protocol_rev >= (sizeof(aCorrectLengthProtocolObjectTypesSupportedBitstring)/
                                sizeof(aCorrectLengthProtocolObjectTypesSupportedBitstring[0])) )
      return;

   ExpectedLength = aCorrectLengthProtocolObjectTypesSupportedBitstring[pDevice->protocol_rev];
   if (EPICSLengthProtocolObjectTypesSupportedBitstring != ExpectedLength)
   {
      sprintf(errMsg,"135.1-2013 5.(l): "
         "For Protocol_Revision %d, the length of Protocol_Object_Types_Supported "
         "must be %d bits, but it is %d bits.\n",
         pDevice->protocol_rev,
         ExpectedLength,
         EPICSLengthProtocolObjectTypesSupportedBitstring);
      tperror(errMsg,false);
   }
   return;
}

// Read a character string consisting of upper and lower case letters.
// Advance lp to the character after the token.
// if mustBe is non-zero, call MustBe.
// Logs an error if no token is present.
// Return true on error with cancel.
bool ReadAlphaString( CString *pVal, char mustBe )
{
   bool retval = false;
   pVal->Empty();

   skipwhitespace();
   int ch;
   while ((ch = *lp) != 0)
   {
      if (!IsAlpha(ch))
      {
         break;
      }
      *pVal += (char)ch;
      lp += 1;
   }

   if (pVal->IsEmpty())
   {
      retval = tperror( "Expected string of letters here", true );
   }
   else if (mustBe != 0)
   {
      retval = MustBe( mustBe );
   }

   return retval;
}

// Skip over a character string consisting of upper and lower case letters.
// At entry, lp must point at first character of token.
// Advances lp to the character after the token.
void SkipAlphaString()
{
   int ch;
   while ((ch = *lp) != 0)
   {
      if (!IsAlpha(ch))
      {
         break;
      }
      lp += 1;
   }
}

// Return true if the next characters are a case-insensitive
// match for the string pointed to by pTestValue, followed by
// a non-alpha character.
// If return is true, updates lp to point to the non-alpha.
// If return is false, lp is untouched
// DOES NOT skip leading whitespace, or otherwise affect buffer contents.
bool IsAlphaString( const char *pTestValue )
{
   int len = strlen( pTestValue );
   bool retval = (_strnicmp( lp, pTestValue, len ) == 0) && !IsAlpha( lp[len] );
   if (retval)
   {
      lp += len;
   }

   return retval;
}

// Read a character string suitable for use as a BACnet enumeration name:
// until the first character that isn't A-Z, a-z, 0-9, _ or -.
// Convert _ to -
// Advance lp to the character after the token.
// if mustBe is non-zero, call MustBe.
// Logs an error if no token is present.
// Return true on error with cancel.
bool ReadEnumString( CString *pVal, char mustBe )
{
   bool retval = false;
   pVal->Empty();

   skipwhitespace();
   int ch;
   while ((ch = *lp) != 0)
   {
      if (ch == '_') ch = '-';
      if (!IsAlnum(ch) && (ch != '-'))
      {
         break;
      }
      *pVal += (char)ch;
      lp += 1;
   }

   if (pVal->IsEmpty())
   {
      retval = tperror( "Expected string token here", true );
   }
   else if (mustBe != 0)
   {
      retval = MustBe( mustBe );
   }

   return retval;
}

///////////////////////////////////////////////////////////////////////
// Read a boolean value from the stream.
// Advance lp to the character after the value.
// if mustBe is non-zero, call MustBe.
// Return true on error with cancel.
bool ReadBool(bool *pValue, char mustBe)
{
   *pValue = false;     // default

   CString str;
   bool retval = ReadAlphaString( &str );
   if (!retval)
   {
      int ch = str[0];
      if ((ch == 'T') || (ch == 't'))
      {
         *pValue = true;
      }
      else if ((ch != 'F') && (ch != 'f'))
      {
         tperror( "Expected True or False value here", true );
         retval = true;
      }

      if (!retval && (mustBe != 0))
      {
         retval = MustBe( mustBe );
      }
   }

   return retval;
}

// Read a floating point number from the stream.
// Advance lp to the character after the number.
// if mustBe is non-zero, call MustBe.
// Return true on error with cancel.
bool ReadFloat(float *pVal, char mustBe)
{
   bool retval = false;

   skipwhitespace();
   int nChar = StringToFloat( lp, *pVal );
   if (nChar == 0)
   {
      // Failed to get a value
      retval = true;
   }
   else
   {
      // Skip the value
      lp += nChar;

      if (mustBe != 0)
      {
         retval = MustBe( mustBe );
      }
   }
   return retval;
}

// Read a signed integer from the stream.
// Advance lp to the character after the number.
// if mustBe is non-zero, call MustBe.
// Return true on error with cancel.
bool ReadSigned(int *pVal, char mustBe)
{
   bool retval = false;
   *pVal = 0;

   // Get an integer with optional leading whitespace, count characters
   int nChar;
   if (sscanf( lp, " %d%n", pVal, &nChar ) != 1)
   {
      retval = tperror("Expected signed integer here...",true);
   }
   else
   {
      lp += nChar;
      if (mustBe != 0)
      {
         retval = MustBe( mustBe );
      }
   }

   return retval;
}

// Return true if the next character is a digit.
// Skip any leading space, but does not otherwise affect lp.
bool IsNextNumber()
{
   skipwhitespace();
   return (IsDigit(lp[0]) != 0);
}

// Read an unsigned integer from the stream.
// Advance lp to the character after the number.
// if mustBe is non-zero, call MustBe.
// Return true on error with cancel.
bool ReadDW(dword *pVal, char mustBe)
{
   bool retval = false;
   dword d = 0;

   skipwhitespace();
   if (*lp == '?')
   {
      lp += 1;             // pretend ? is a valid digit
   }
   else if (IsDigit(*lp))
   {
      // Decimal integer
      while (IsDigit(*lp))
      {
         d = 10*d + (*lp++ - '0');
      }
   }
   else
   {
      retval = tperror("Expected Unsigned Integer here...",true);
   }

   if (!retval && (mustBe != 0))
   {
      retval = MustBe( mustBe );
   }

   *pVal = d;
   return retval;
}

// Read an unsigned integer word from the stream.
// Advance lp to the character after the number.
// if mustBe is non-zero, call MustBe.
// Return true on error with cancel.
bool ReadW(word *pVal, char mustBe)
{
   dword d;
   bool retval = ReadDW( &d, 0 );
   if (!retval)
   {
      if (d > 65535)
      {
         retval = tperror("Must use an Unsigned Integer 0..65535 here!",true);
         d = 0;
      }
      else if (mustBe != 0)
      {
         retval = MustBe( mustBe );
      }
   }

   *pVal = (word)d;
   return retval;
}

// Read an object identifier (objecttype,instance) from the stream
// Advance lp to the character after the number.
// if mustBe is non-zero, call MustBe.
// Return true on error with cancel.
bool ReadObjectID(dword *pVal, char mustBe )
{
   word  objtype;
   dword id;

   // Set a default, for naughty folks who don't check return value
   *pVal = BAD_OBJID;

   // We COULD use EpicsSequence to accept EITHER () or {}, but () seems pretty consistent
   if (!NextIs('('))
   {
      return tperror("Expected ( before (objecttype,instance) here!",true);
   }

   if (IsNextNumber())
   {
      // Integer object type (useful, even though not 135.1)
      if (ReadW(&objtype)) return true;
   }
   else if (_strnicmp(lp, "proprietary ", 12) == 0)
   {
      // Legacy VTS "proprietary NNN"
      lp += 12;
      if (ReadW(&objtype)) return true;
   }
   else if (_strnicmp(lp, "vendor-", 7) == 0)
   {
      lp += 7;
      if (ReadW(&objtype)) return true;
   }
   // ReadEnum also accepts 135.1 "proprietarywhatever-123" and VTS-style "unknown:123"
   else if (ReadEnum(&objtype, &NetworkSniffer::BAC_STRTAB_BACnetObjectType))
   {
      // Error shown by ReadEnum
      return true;
   }

   if (MustBe(',')) return true;

   skipwhitespace();
   if (_strnicmp(lp,"instance ",9)==0) //ignore instance here (non 135.1)
      lp += 9;

   if (ReadDW(&id, ')')) return true;
   if (id >= (1L<<22))
   {
      return tperror("Object Instance must be 0..4194303!",true);
   }

   //valid instance number
   *pVal = (((dword)objtype)<<22) + id;

   if (mustBe != 0)
   {
      return MustBe( mustBe );
   }

//   print_debug( "  Read Object ID (%u,%u)\n", objtype, id );
   return false;
}

// Read a square-bracketed CHOICE tag from the stream.
// If isRequired is true, then the tag is required; if false, the tag is optional.
// If present, the tag value must be between 0 and maxChoice.
// If the tag is absent or invalid, *pChoice will be set to 0xFF.
// Advance lp to the character after the tag, if any
//
// Return true on error with cancel.
bool ReadChoiceTag(octet *pChoice, octet maxChoice, bool isRequired)
{
   bool retval = false;
   *pChoice = 0xFF;
   if (NextIs( '[' ))
   {
      DWORD dw;
      retval = ReadDW( &dw, ']' );
      if (!retval)
      {
         if (dw > (dword)maxChoice)
         {
            CString str;
            str.Format( "Choice [%u] is outside expected range [0] to [%u]", dw, maxChoice );
            tperror( str,true );
            retval = true;
         }
         else
         {
            *pChoice = (octet)dw;
         }
      }
   }
   else if (isRequired)
   {
      CString str;
      str.Format( "Missing required CHOICE tag [0] to [%u]", maxChoice );
      tperror( str,true );
      retval = true;
   }

   return retval;
}

// Return a propdescriptor appropriate to the specified object type.
// If the type is unknown, returns the default table
// This SHOULD return const, except for one immoral usage in ParseProperty,
// which writes into the table under the control of global flags...
// For now we satisfy ourselves with const variables when possible.
//
propdescriptor* GetPropDescriptorTable( word objtype )
{
   return (objtype < MAX_DEFINED_OBJ) ? StdObjects[objtype].sotProps : ProprietaryObjProps;
}

//=============================================================================
// Class to assist with bitstring or SEQUENCE parsing
EpicsSequence::EpicsSequence()
: m_terminator(0)
{
}

EpicsSequence::~EpicsSequence()
{
}

// Get opening parenthesis or bracket, else log an error.
// Return true to abort
bool EpicsSequence::Begin()
{
   bool retval = false;
   if (NextIs('{'))
   {
      m_terminator = '}';
   }
   else if (NextIs('('))
   {
      m_terminator = ')';
   }
   else
   {
      retval = tperror("Expected ( or {",true);
   }

   return retval;
}

// Get OPTIONAL opening parenthesis or bracket.
// If neither is found, End() will do nothing.
void EpicsSequence::BeginOptional()
{
   if (NextIs('{'))
   {
      m_terminator = '}';
   }
   else if (NextIs('('))
   {
      m_terminator = ')';
   }
}

// Get closing parenthesis or bracket, else log an error.
// Return true to abort
bool EpicsSequence::End()
{
   bool retval = false;
   if ((m_terminator != 0) && !NextIs( m_terminator ))
   {
      CString err;
      err.Format("Expected %c", m_terminator );
      retval = tperror(err, true);
   }

   return retval;
}

// Return false if the next non-blank is comma (indicating a missing optional
// value), or the closing terminator (indicating one or more missing values)
// Thus, will return true if an optional item is present.
// Comma is eaten, closing terminator is not.
bool EpicsSequence::HasOption()
{
   bool retval = false;

   // Eat whitespace, check for comma.  If found, eat the comma
   if (!NextIs( ',' ))
   {
      // Not a comma.  Return true if non-null and not a terminator
      retval = (*lp != 0) && (*lp != m_terminator);
   }

   return retval;
}

//=============================================================================
EpicsListOf::EpicsListOf()
: m_terminator(0)
, m_count(0)
{
}

EpicsListOf::~EpicsListOf()
{
}

// Return true if the loop should continue, else false
bool EpicsListOf::HasMore()
{
   if (m_terminator == 0)
   {
      // First time
      if (NextIs('{'))
      {
         m_terminator = '}';
      }
      else if (NextIs('('))
      {
         m_terminator = ')';
      }
      else
      {
         tperror("Expected ( or { on list specification",true);
      }
   }

   if (m_terminator != 0)
   {
      // During a loop (including first time)
      if (NextIs( m_terminator ))
      {
         // End of list (with or without elements)
         m_terminator = 0;
      }
      else if (NextIs( ',' ))
      {
         if (m_count == 0)
         {
            tperror("Got comma, but expected element or end of list",true);
            m_terminator = 0;
         }
         else
         {
            // Get ready to parse the next element
            m_count += 1;
            skipwhitespace();
         }
      }
      else if (feof(ifile))
      {
         // End of file
         tperror("Unexpected end of file",true);
         m_terminator = 0;
      }
      else
      {
         // For rigorous parsing, SHOULD be here only the first time,
         // with commas before later iterations.
         // But "be liberal in what you accept", so we allow just
         // spaces between elements.  This also makes this class
         // compatible with old-style parse-past-the-comma code
//         if (m_count == 0)
//         {
            // Get ready to parse the first element
            m_count += 1;
            skipwhitespace();
//         }
//         else
//         {
//            tperror("Expected comma or end of list",true);
//            m_terminator = 0;
//         }
      }
   }

   return (m_terminator != 0);
}

// Any (well, MOST) primitive value

// Delete any allocated data, set choice to NULL
void AnyPrimitive::Flush()
{
   switch (choice)
   {
   case PRIM_OCTET_STRING:
      if (pOctetStringValue)
         free(pOctetStringValue);
      pOctetStringValue = NULL;
      break;

   case PRIM_CHARACTER_STRING:
      if (pCharStringValue)
         free(pCharStringValue);
      pCharStringValue = NULL;
      break;

   case PRIM_BIT_STRING:
      if (pBitStringValue) 
         free(pBitStringValue);
      pBitStringValue = NULL;
      break;

   default:
      // Other types have no memory allocations
      break;
   }
   choice = PRIM_NULL;
}

// Decode a CHOICE of any primitive value from lp.
// 135.1 clause 4 says we are supposed to determine the choice from the value.
// BUT:
// - can't tell a signed 42 from an unsigned 42.  We require a "+" or "-" on signed
// - can't tell double from real
// - can't tell what enumeration set to use.  We hack the common inactive/active for points
//   and rely on unknown:xxxx for anything else.
bool DecodeAnyPrimitive( AnyPrimitive *pVal )
{
   bool retval = false;
   int nDigits;

   skipwhitespace();
   char *pStr = lp;
   if (IsAlphaString( "null" ))
   {
      pVal->choice = PRIM_NULL;
   }
   else if (IsAlphaString( "true" ) || IsAlphaString( "t" ))
   {
      pVal->choice = PRIM_BOOLEAN;
      pVal->booleanValue = true;
   }
   else if (IsAlphaString( "false" ) || IsAlphaString( "f" ))
   {
      pVal->choice = PRIM_BOOLEAN;
      pVal->booleanValue = false;
   }
   else if (IsAlphaString( "active" ))
   {
      // TODO: set an enumTable?
      pVal->choice = PRIM_ENUMERATED;
      pVal->enumValue = 1;
   }
   else if (IsAlphaString( "inactive" ))
   {
      // TODO: set an enumTable?
      pVal->choice = PRIM_ENUMERATED;
      pVal->enumValue = 0;
   }
   else if (IsAlphaString( "unknown:" ))
   {
      // TODO: set an enumTable?
      pVal->choice = PRIM_ENUMERATED;
      retval = ReadW( &pVal->enumValue );
   }
   else if ((*lp == '"') || (*lp == '`') || (*lp == '\''))
   {
      // Character string
      char terms[2];
      terms[1] = 0;
      terms[0] = *lp++;
      if (terms[0] == '`')
         terms[0] = '\'';
      int len = strspn( lp, terms );
      pVal->choice = PRIM_CHARACTER_STRING;
      pVal->helper = len;
      pVal->pCharStringValue = (char*)malloc( len + 1 );
      strncpy( pVal->pCharStringValue, lp, len );
      pVal->pCharStringValue[len] = 0;
      lp += len+1;
   }
   else if (((*lp == 'X') || (*lp == 'x')) && ((lp[1] == '`') || (lp[1] == '\'')))
   {
      // Octet string
      // TODO: we COULD count octets, and allocate based on that.
      // But since memory is cheap and this code will probably never be run, we
      // allocate a fixed max
      pVal->choice = PRIM_OCTET_STRING;
      pVal->pOctetStringValue = (unsigned char*)malloc( 100 );
      word count;
      retval = ParseOctetstring( pVal->pOctetStringValue, 100, &count );
      pVal->helper = count;
   }
   else if ((*lp == '{')
               ||
            (((*lp=='B') || (*lp=='b')) && ((lp[1]=='`') || (lp[1]=='\''))))
   {
      // Bit string
      // TODO: we COULD count bits, and allocate based on that.
      // But since memory is cheap and this code will probably never be run, we
      // allocate a fixed max
      pVal->choice = PRIM_BIT_STRING;
      pVal->pBitStringValue = (unsigned char*)malloc( 100 );
      UINT count;
      retval = ParseBitstring( pVal->pBitStringValue, 8*100, &count );
      pVal->helper = (int)count;
   }
   else if ((*lp == '+') || (*lp == '-'))
   {
      // Signed Integer or Real
      pStr++;
      nDigits = strspn( pStr, s_digits );
      if (nDigits > 0)
      {
         if (pStr[nDigits] == '.')
         {
            // Float
            pVal->choice = PRIM_REAL;
            retval = ReadFloat( &pVal->realValue );
         }
         else
         {
            // Signed integer
            pVal->choice = PRIM_SIGNED;
            retval = ReadSigned( &pVal->signedValue );
         }
      }
      else
      {
         // Error
         tperror( "Expected real or signed integer here", true );
         retval = true;
      }
   }
   else if (*lp == '(')
   {
      // Date or ObjectID.
      // TODO: Distinguishing these is a pain because of all the Date options
      // that we support: Date with / is easy, but - and , both appear in at
      // least some ObjectIDs.
      // For now, assume Date, since there is a Date object, and no ObjectID object
      pVal->choice = PRIM_DATE;
      retval = ReadObjectID( &pVal->objectIdValue );
      if (retval)
      {
         // ObjectID as (obj-type,int)        pVal->choice = PRIM_OBJECT_IDENTIFIER;
         tperror( "ObjectID parsing not yet implemented", true );
      }
   }
   else
   {
      nDigits = strspn( pStr, s_digits );
      if (nDigits > 0)
      {
         if (pStr[nDigits] == ':')
         {
            // Time
            pVal->choice = PRIM_TIME;
            retval = ParseTime( &pVal->timeValue );
         }
         else if (pStr[nDigits] == '.')
         {
            // Float
            pVal->choice = PRIM_REAL;
            retval = ReadFloat( &pVal->realValue );
         }
         else
         {
            // Signed integer
            pVal->choice = PRIM_UNSIGNED;
            retval = ReadSigned( &pVal->signedValue );
         }
      }
      else
      {
         tperror( "Unable to parse as a primitive value", true );
         retval = true;
      }
   }

   return retval;
}

}  // end namespace PICS
