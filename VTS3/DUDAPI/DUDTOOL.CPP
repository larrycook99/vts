//
// Functions for creating C++ versions of objectsin the EPICS database, for display, encoding etc.

#include "stdafx.h"

#include <windows.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include "VTS.h"
#include "BACnet.hpp"
#include "propid.h"
namespace NetworkSniffer {
   extern char *BACnetPropertyIdentifier[];
}

namespace PICS {

#include "db.h"       // contains BACnet datatypes
#include "stdobjpr.h"
#include "vtsapi.h"
#include "bacprim.h"  // contains basic encoding functions

#include "dudapi.h"   // export interface

#include "props.h"

#define nPRIO 16    // size of priority arrays
#define nTEST 3     // number of test values
#define nTESTLIST 4 // number of test values for list properties


// test values for list properties:
static BACnetCalendarEntry far* gTestCalist[nTESTLIST];
static BACnetReadAccessSpecification far* gTestRaslist[nTESTLIST];
static BACnetVTClassList far* gTestVtcl[nTESTLIST];
static BACnetDeviceObjectPropertyReference far* gTestLodpref[nTESTLIST];
static BACnetObjectPropertyReference far* gTestLopref[nTESTLIST];
static BACnetSessionKey far* gTestSkeys[nTESTLIST];
static BACnetAddressBinding far* gTestDabind[nTESTLIST];
static BACnetRecipient far* gTestRecip[nTESTLIST];
static BACnetDestination far* gTestReciplist[nTESTLIST];
static BACnetVTSession far* gTestVtse[nTESTLIST];

static BACnetGenericArray * CreateAddressBindingList( PICS::BACnetAddressBinding * pAddrBinding );
static BACnetObjectIDList * CreateObjectList( PICS::BACnetObjectIdentifier * pobjidlist );
static BACnetTimeStampArray * CreateTimeStampArray( PICS::BACnetTimeStamp  * ptimestamp[3], int nMax );
static BACnetListOfCalendarEntry * CreateCalendarList( PICS::BACnetCalendarEntry * pcalentry );
static BACnetListOfVTClass * CreateVTClassesSupported( PICS::BACnetVTClassList * pVTClassList );
static BACnetListOfEnum * CreateEnumList( PICS::BACnetEnumList * penumList, int iTableIndex);
static BACnetListOfDestination * CreateListOfDestination( PICS::BACnetDestination * pdest );
static BACnetListOfRecipient * CreateRecipientList( PICS::BACnetRecipient * precip );
static BACnetListOfCOVSubscription * CreateCOVSubscriptionList( PICS::BACnetCOVSubscription * psub );
static BACnetListOfDeviceObjectPropertyReference * CreateDeviceObjectPropertyList( PICS::BACnetDeviceObjectPropertyReference * pdevobjlist );
static BACnetListOfDeviceObjectReference * CreateDeviceObjectList( PICS::BACnetDeviceObjectReference * pdevobjlist );

static bool CreateGenericProperty( generic_object far * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool CreateCommonProperty(  generic_object far * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );

static bool AI_CreateProperty( ai_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool AO_CreateProperty( ao_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool AV_CreateProperty( av_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool BI_CreateProperty( bi_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool BO_CreateProperty( bo_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool BV_CreateProperty( bv_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool CA_CreateProperty( calendar_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool CO_CreateProperty( command_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool DE_CreateProperty( device_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool EV_CreateProperty( ee_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool FI_CreateProperty( file_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool GR_CreateProperty( group_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool LO_CreateProperty( loop_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool MSI_CreateProperty( mi_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool MSO_CreateProperty( mo_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool MSV_CreateProperty( msv_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool NC_CreateProperty( nc_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool PR_CreateProperty( program_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool SC_CreateProperty( schedule_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool AVG_CreateProperty( avg_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool TR_CreateProperty( trend_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool LFSP_CreateProperty( lifesafetypoint_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue ); //Shiyuan Xiao 7/21/2005
static bool LFSZ_CreateProperty( lifesafetyzone_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue ); //Shiyuan Xiao 7/21/2005
static bool AC_CreateProperty( accumulator_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue ); //Shiyuan Xiao 7/15/2005
static bool PC_CreateProperty( pulseconverter_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool LC_CreateProperty( lc_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool AD_CreateProperty( ad_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool SV_CreateProperty( sv_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool EL_CreateProperty( el_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool TLM_CreateProperty( tlm_obj_type * pObj, BACnetPropertyIdentifier PropId, BACnetAnyValue * pbacnetAnyValue );
static bool CharStr_CreateProperty( charstring_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue );
static bool Integer_CreateProperty( integer_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue );
static bool Pos_Int_CreateProperty( positive_integer_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue );
static bool DateTime_CreateProperty( datetimevalue_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue );

static octet gTestStavals[nTESTLIST][4]= { {1,2,3,0}, {4,0,0,0}, {0,0,0,0}, {5,0,0,0} };

static int      gTestEnum[nTEST]= {0, 1, 2};
static float    gTestReal[nTEST]= {47.1F, 0.5F, 92.2F};

static octet    gTestBits[nTEST]= {0x55, 0x44, 0x33};
static int      gTestUsedBits= 5;

static int      gTestInt[nTEST]=     {13, 14, 15};

static char*    gTestString[nTEST]= {"abcd", "efgh", "ijkl"};

static BOOL     gTestBool[nTEST]= {TRUE, FALSE, TRUE};

static BACnetDateTime gTestDT[nTEST] = {
                           {{96,5,20,2},{9,28,0,0}},
                           {{96,4,21,3},{9,13,0,0}},
                           {{96,6,21,2},{8,29,0,0}}
                        };

//
static float gTestPAF[nTEST][nPRIO] =
{
   {1.1F,2.1F,3.1F,4.1F,5.1F,6.1F,7.1F,8.1F,9.1F,10.1F,11.1F,12.1F,13.1F,14.1F,15.1F,16.1F},
   {1.2F,2.2F,3.2F,4.2F,5.2F,6.2F,7.2F,8.2F,9.2F,10.2F,11.2F,12.2F,13.2F,14.2F,15.2F,16.2F},
   {1.3F,2.3F,3.3F,4.3F,5.3F,6.3F,7.3F,8.3F,9.3F,10.3F,11.3F,12.3F,13.3F,14.3F,15.3F,16.3F}
};

static enum BACnetBinaryPV gTestPAB[nTEST][nPRIO]=
        {
          { INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,
            INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE },
          { ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,
            ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE } ,
          { ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,
            INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE }
        };

static word gTestPAU[nTEST][nPRIO]= {
                        {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
                        {16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1},
                        {16,15,14,13,12,11,10,9,1,2,3,4,5,6,7,8}
                      };

static dword gTestPADW[nTEST][nPRIO]= {
                        {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
                        {16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1},
                        {16,15,14,13,12,11,10,9,1,2,3,4,5,6,7,8}
                      };

static BACnetDateRange gTestDTRange[nTEST]= {
                              { {96,5,20,0xFF}, {96,5,22,0xFF} },
                              { {95,4,19,0xFF}, {97,6,24,0xFF} },
                              { {95,6,21,0xFF}, {95,6,23,0xFF} }
                            };

static BACnetEventParameter gTestEvparm[nTEST]= {
    {OUT_OF_RANGE, {NULL,8,{0,0,0,0}}, NULL, NULL, 0, 0.0, 17, {NULL}, {NULL}, {NULL}, 1.5F, 1.6F, 1.7F, 1.8F, 1.9F },
    {OUT_OF_RANGE, {NULL,8,{0,0,0,0}}, NULL, NULL, 0, 0.0, 17, {NULL}, {NULL}, {NULL}, 1.1F, 1.2F, 1.3F, 1.4F, 1.5F },
    {OUT_OF_RANGE, {NULL,8,{0,0,0,0}}, NULL, NULL, 0, 0.0, 17, {NULL}, {NULL}, {NULL}, 1.3F, 4.2F, 7.3F, 11.4F, 51.5F }
  };

static BACnetObjectIdentifier gTestLObj[nTEST]= { {NULL,4711}, {NULL,4712}, {NULL,4713} };

static BACnetTimeValue gTestTimeValue[nTEST][7]= {
         {
           {NULL, {15,47,59,59}, UNS }, // 0
           {NULL, {15,47,59,59}, UNS }, // 1
           {NULL, {15,47,59,59}, UNS }, // 2
           {NULL, {15,47,59,59}, UNS }, // 3
           {NULL, {15,47,59,59}, UNS }, // 4
           {NULL, {15,47,59,59}, UNS }, // 5
           {NULL, {15,47,59,59}, UNS }  // 6
         },

         {
           {NULL, {14,47,59,59}, UNS }, // 0
           {NULL, {14,46,59,59}, UNS }, // 1
           {NULL, {14,45,59,59}, UNS }, // 2
           {NULL, {14,44,59,59}, UNS }, // 3
           {NULL, {14,43,59,59}, UNS }, // 4
           {NULL, {14,42,59,59}, UNS }, // 5
           {NULL, {14,41,59,59}, UNS }  // 6
         },

         {
           {NULL, {16,47,59,59}, UNS }, // 0
           {NULL, {16,46,59,59}, UNS }, // 1
           {NULL, {16,45,59,59}, UNS }, // 2
           {NULL, {16,44,59,59}, UNS }, // 3
           {NULL, {16,43,59,59}, UNS }, // 4
           {NULL, {16,42,59,59}, UNS }, // 5
           {NULL, {16,41,59,59}, UNS }  // 6
         }
    };


static BACnetTimeValue far* gTestWsched[nTEST][7];
static BACnetExceptionSchedule far* gTestXSched[nTEST];

#define nTESTUWNUM 3
static word gTestUWArr[nTEST][nTESTUWNUM]= { {1,2,3}, {3,2,1}, {2,3,1} };

static char far* gTestActext[nTEST][1]= { {"act1"}, {"act2"}, {"act3"} };

static BACnetActionCommand far* gTestAct[nTEST][1];

// ===========================================================================================================
//
// functions
//
// ===========================================================================================================


// ===========================================================================================================
// Functions for loading type of data and pointer to buffer for encoding later, checking now, etc.
// ===========================================================================================================

extern "C" void CreatePropertyFromEPICS( PICS::generic_object * pObj, int PropId, BACnetAnyValue * pbacnetAnyValue )
{
   bool handled = false;
   switch(pObj->object_type)
   {
   case OBJ_ANALOG_INPUT:
      handled = AI_CreateProperty( (ai_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_ANALOG_OUTPUT:
      handled = AO_CreateProperty( (ao_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_ANALOG_VALUE:
      handled = AV_CreateProperty( (av_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_BINARY_INPUT:
      handled = BI_CreateProperty( (bi_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_BINARY_OUTPUT:
      handled = BO_CreateProperty( (bo_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_BINARY_VALUE:
      handled = BV_CreateProperty( (bv_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_CALENDAR:
      handled = CA_CreateProperty( (calendar_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_COMMAND:
      handled = CO_CreateProperty( (command_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_DEVICE:
      handled = DE_CreateProperty( (device_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_EVENT_ENROLLMENT:
      handled = EV_CreateProperty( (ee_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_FILE:
      handled = FI_CreateProperty( (file_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_GROUP:
      handled = GR_CreateProperty( (group_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_LOOP:
      handled = LO_CreateProperty( (loop_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_MULTI_STATE_INPUT:
      handled = MSI_CreateProperty( (mi_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_MULTI_STATE_OUTPUT:
      handled = MSO_CreateProperty( (mo_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_MULTI_STATE_VALUE:
      handled = MSV_CreateProperty( (msv_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_NOTIFICATIONCLASS:
      handled = NC_CreateProperty( (nc_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_PROGRAM:
      handled = PR_CreateProperty( (program_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_SCHEDULE:
      handled = SC_CreateProperty( (schedule_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_AVERAGING:
      handled = AVG_CreateProperty( (avg_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_TREND_LOG:
      handled = TR_CreateProperty( (trend_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_LIFE_SAFETY_POINT:
      handled = LFSP_CreateProperty((lifesafetypoint_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_LIFE_SAFETY_ZONE:
      handled = LFSZ_CreateProperty((lifesafetyzone_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_ACCUMULATOR:
      handled = AC_CreateProperty( (accumulator_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_PULSE_CONVERTER:
      handled = PC_CreateProperty( (pulseconverter_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_EVENT_LOG:
      handled = EL_CreateProperty( (el_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_TREND_LOG_MULTIPLE:
      handled = TLM_CreateProperty( (tlm_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_LOAD_CONTROL:
      handled = LC_CreateProperty( (lc_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_STRUCTURED_VIEW:
      handled = SV_CreateProperty( (sv_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_ACCESS_DOOR:
      handled = AD_CreateProperty( (ad_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;

   // Until real object property handling is implemented, do nothing here.  Generic handling below
   case OBJ_LIGHTING_OUTPUT:
   case OBJ_ACCESS_CREDENTIAL:
   case OBJ_ACCESS_POINT:
   case OBJ_ACCESS_RIGHTS:
   case OBJ_ACCESS_USER:
   case OBJ_ACCESS_ZONE:
   case OBJ_CREDENTIAL_DATA_INPUT:
   case OBJ_NETWORK_SECURITY:
   case OBJ_BITSTRING_VALUE:
      break;

   case OBJ_CHARACTERSTRING_VALUE:
      handled = CharStr_CreateProperty( (charstring_obj_type *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;

   case OBJ_DATE_PATTERN_VALUE:
   case OBJ_DATE_VALUE:
   case OBJ_DATETIME_PATTERN_VALUE:
      break;

   case OBJ_DATETIME_VALUE:
      handled = DateTime_CreateProperty( (datetimevalue_obj_type far *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;
   case OBJ_INTEGER_VALUE:
      handled = Integer_CreateProperty( (integer_obj_type far *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue );
      break;

   case OBJ_LARGE_ANALOG_VALUE:
   case OBJ_OCTETSTRING_VALUE:
      break;

   case OBJ_POSITIVE_INTEGER_VALUE:
      handled = Pos_Int_CreateProperty( (positive_integer_obj_type far *) pObj, (BACnetPropertyIdentifier) PropId, pbacnetAnyValue ); break;

   case OBJ_TIME_PATTERN_VALUE:
   case OBJ_TIME_VALUE:
      break;

   default:
      //Proprietary objects
      break;
   }

   // Handle anything not done above (this lets object-specific code override default processing)
   if (!handled)
   {
      handled = CreateGenericProperty( pObj, (BACnetPropertyIdentifier)PropId, pbacnetAnyValue );
   }
   if (!handled)
   {
      handled = CreateCommonProperty( pObj, (BACnetPropertyIdentifier)PropId, pbacnetAnyValue );
   }
   if (!handled)
   {
      // no handler for this property
      ASSERT(FALSE);
   }
}

/** Handle the creation of generic properties for any Object type that uses the generic_object structure.
 * @return True if property was created, else False (someone else needs to handle it).
 */
bool CreateGenericProperty( generic_object far * pGenObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   bool bHandled = true;
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case EVENT_MESSAGE_TEXTS:
         pbacnetAnyValue->SetObject(statext, new BACnetTextArray(pGenObj->event_message_texts, 3));
         break;
      case EVENT_MESSAGE_TEXTS_CONFIG:
         pbacnetAnyValue->SetObject(statext, new BACnetTextArray(pGenObj->event_message_texts_config, 3));
         break;
      case EVENT_ALGORITHM_INHIBIT_REF:
         pbacnetAnyValue->SetObject(propref, new ::BACnetObjectPropertyReference(pGenObj->event_algorithm_inhibit_ref.object_id,
                                                                                 pGenObj->event_algorithm_inhibit_ref.property_id,
                                                                                 pGenObj->event_algorithm_inhibit_ref.pa_index));
         break;

      default:
         bHandled = false;    // If we get here, wasn't handled.
         break;
   }
   return bHandled;
}

/** Handle the creation of common properties for all Object types.
 * Just deals with the properties of known (fixed) data types.
 * @return True if property was created, else False (someone else needs to handle it).
 */
bool CreateCommonProperty( generic_object far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);
   bool bHandled = false;

   const NetworkSniffer::BACnetStringTable *petable = NULL;
   const propdescriptor *pd;
   unsigned char *pProp;
   word objType = pObj->object_type;
   if ( ( objType >= MAX_DEFINED_OBJ ) || ( objType < 0 ) )
   {
      return bHandled;
   }

   // First, check that this Object Type supports the property, and get its description
   pd = StdObjects[objType].sotProps;
   while (pd->PropID != propertyID)
   {
      if (pd->PropGroup & LAST)
         return bHandled;     // no luck finding it here
      pd += 1;
   }

   // Point to the data in our object structure
   pProp = (unsigned char *)pObj + (size_t) pd->StrucOffset;

   // Now see if this function handles the Property (assume True to start with)
   bHandled = true;
   switch (pd->ParseType)
   {
   case ob_id:
      pbacnetAnyValue->SetObject(ob_id, new ::BACnetObjectIdentifier( *(unsigned int*) pProp ));
      break;
   case et:
      // TODO: Note that some enumeration members are defined in Stdobj.j as word,
      // but some are defined AS enums, such as BACnetEventState, BACnetReliability etc.
      // and these will have size FOUR BYTES on modern x86.
      // This code works, so long as our processor is little-endian and the struct
      // is cleared to zero...
      // Please help us stamp out word/dword in our lifetime.  Thank you.
      petable = NetworkSniffer::GetEnumStringTable(pd->PropET);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( *(word *) pProp, *petable ));
      break;
   case flt:
      pbacnetAnyValue->SetObject(flt, new BACnetReal( *(float *) pProp));
      break;
   case bits:
      petable = NetworkSniffer::GetEnumStringTable(pd->PropET);
      pbacnetAnyValue->SetObject(bits, new BACnetBitString( petable->m_nStrings, pProp ));
      break;
   case u127:
      pbacnetAnyValue->SetObject(pd->ParseType, new BACnetUnsigned( *pProp));
      break;
   case u16:
   case uw:
      pbacnetAnyValue->SetObject(pd->ParseType, new BACnetUnsigned( *(word *) pProp));
      break;
   case ud:
      pbacnetAnyValue->SetObject(pd->ParseType, new BACnetUnsigned( *(dword *) pProp));
      break;
   case ebool:
      pbacnetAnyValue->SetObject(ebool, new BACnetBoolean( *(bool *)pProp));
      break;
   case ch_string:
      pbacnetAnyValue->SetObject(pd->ParseType, new BACnetCharacterString( (char*) pProp));
      break;
   case dt:
      pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime( *(PICS::BACnetDateTime*) pProp ));
      break;
   case paf:
      pbacnetAnyValue->SetObject(paf, new BACnetPriorityArray( (float*)pProp, nPRIO, fpaNULL ));
      break;
   case TSTMParr:
      pbacnetAnyValue->SetObject(TSTMParr, CreateTimeStampArray( (BACnetTimeStamp **) pProp, 3));
      break;
   case PT_PROPLIST:
      {
         // TODO: this could call CreateEnumList, except there is currently no enum spec for propertyID, and
         // the enumeration is too large to duplicate sanely.
         BACnetGenericArray * pList = new BACnetGenericArray();
         PICS::BACnetEnumList *pEnum = pObj->property_list;
         for ( ; pEnum != NULL; pEnum = pEnum->next )
         {
            pList->Add(new ::BACnetEnumerated( (int)pEnum->value, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier ));
         }
         pbacnetAnyValue->SetObject(PT_PROPLIST, pList );
      }
      break;

   case tsrecip:
      pbacnetAnyValue->SetObject(tsrecip, CreateRecipientList( *(BACnetRecipient**)pProp ));
      break;

   default:
      bHandled = false;
      break;
   }

   return bHandled;
}


bool AI_CreateProperty( ai_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   // All properties are handled by CreateCommonProperty based on ParseType and the property table
   return false;
}

bool AO_CreateProperty( ao_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   // All properties are handled by CreateCommonProperty based on ParseType and the property table
   return false;
}

bool AV_CreateProperty( av_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   // All properties are handled by CreateCommonProperty based on ParseType and the property table
   return false;
}

bool BI_CreateProperty( bi_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   // All properties are handled by CreateCommonProperty based on ParseType and the property table
   return false;
}

bool BO_CreateProperty( bo_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   // Most properties are handled by CreateCommonProperty based on ParseType and the property table.
   // More could be if you add some more ParseTypes
   switch ( propertyID )
   {
      case PRIORITY_ARRAY:
         pbacnetAnyValue->SetObject(pab, new BACnetPriorityArray(pObj->priority_array, nPRIO, bpaNULL, true));  // changed by Kare Sars
         break;
      case FEEDBACK_VALUE:
         pbacnetAnyValue->SetObject(et, new BACnetBinaryPriV((int) pObj->feedback_value));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}

bool BV_CreateProperty( bv_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   // Most properties are handled by CreateCommonProperty based on ParseType and the property table.
   // More could be if you add some more ParseTypes
   switch ( propertyID )
   {
      case PRIORITY_ARRAY:
         pbacnetAnyValue->SetObject(pab, new BACnetPriorityArray((unsigned short *) pObj->priority_array, nPRIO, bpaNULL, true));  // changed by Kare Sars
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}


bool CA_CreateProperty( calendar_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case DATE_LIST:
         pbacnetAnyValue->SetObject(calist, CreateCalendarList(pObj->date_list));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}


bool CO_CreateProperty( command_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case ACTION:
         pbacnetAnyValue->SetObject(act, new ::BACnetActionCommand());
           //msg->pv= Obj->action;
         //msg->pt= act;
         //msg->Num= Obj->num_actions;
         break;
      case ACTION_TEXT:
         pbacnetAnyValue->SetObject(actext, new BACnetTextArray(pObj->action_text, MAX_ACTION_TEXTS ));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}

bool DE_CreateProperty( device_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   // Most properties are handled by CreateCommonProperty based on ParseType and the property table.
   // More could be if you add some more ParseTypes
   switch ( propertyID )
   {
      // The ParseType in the property table is pss, not bits
      case PROTOCOL_SERVICES_SUPPORTED:
         pbacnetAnyValue->SetObject(bits, new BACnetBitString(EPICSLengthProtocolServicesSupportedBitstring, pObj->protocol_services_supported));
         break;
      // The ParseType in the property table is pos, not bits
      case PROTOCOL_OBJECT_TYPES_SUPPORTED:
         pbacnetAnyValue->SetObject(bits, new BACnetBitString(EPICSLengthProtocolObjectTypesSupportedBitstring, pObj->object_types_supported));
         break;
      case OBJECT_LIST:
         pbacnetAnyValue->SetObject(lobj, CreateObjectList(pObj->object_list));
         break;
      case VT_CLASSES_SUPPORTED:
         pbacnetAnyValue->SetObject(vtcl, CreateVTClassesSupported(pObj->vt_classes_supported));
         break;
      case ACTIVE_VT_SESSIONS:
         pbacnetAnyValue->SetObject(vtse, new ::BACnetVTSession());
         //msg->pv= Obj->active_vt_sessions;
         //msg->pt= vtse;
         break;
      case LOCAL_TIME:
         // This ignores any value in the EPICS, assuming it is ?
         // and uses the default constructor: current local time
         pbacnetAnyValue->SetObject(ptTime, new ::BACnetTime());
         break;
      case LOCAL_DATE:
         // This ignores any value in the EPICS, assuming it is ?
         // and uses the default constructor: current local time
         pbacnetAnyValue->SetObject(ptDate, new ::BACnetDate());
         break;
      case UTC_OFFSET:
         pbacnetAnyValue->SetObject(ssint, new BACnetInteger(pObj->utc_offset));
         break;
      case DAYLIGHT_SAVINGS_STATUS:
         pbacnetAnyValue->SetObject(ebool, new BACnetBoolean((int) pObj->day_savings_status));
         break;
      case LIST_OF_SESSION_KEYS:
         pbacnetAnyValue->SetObject(skeys, new ::BACnetSessionKey());
         //msg->pv= Obj->list_session_keys;
         //msg->pt= skeys;
         break;
      case DEVICE_ADDRESS_BINDING:
         pbacnetAnyValue->SetObject(dabind, CreateAddressBindingList(pObj->device_add_binding));
           break;
      case CONFIGURATION_FILES:
         pbacnetAnyValue->SetObject(lobj, CreateObjectList(pObj->configuration_files));
         break;
      case LAST_RESTORE_TIME:
         switch(pObj->last_restore_time.choice)
         {
         case 0:  pbacnetAnyValue->SetObject(TSTMP, new ::BACnetTimeStamp(new ::BACnetTime(pObj->last_restore_time.u.time.hour, pObj->last_restore_time.u.time.minute, pObj->last_restore_time.u.time.second, pObj->last_restore_time.u.time.hundredths)));
                  break;
         case 1:  pbacnetAnyValue->SetObject(TSTMP, new ::BACnetTimeStamp(new BACnetUnsigned(pObj->last_restore_time.u.sequence_number)));
                  break;
         case 2:  pbacnetAnyValue->SetObject(TSTMP, new ::BACnetTimeStamp(new ::BACnetDateTime(pObj->last_restore_time.u.date_time.date.year, pObj->last_restore_time.u.date_time.date.month, pObj->last_restore_time.u.date_time.date.day_of_month,
                              pObj->last_restore_time.u.date_time.time.hour, pObj->last_restore_time.u.date_time.time.minute, pObj->last_restore_time.u.date_time.time.second, pObj->last_restore_time.u.date_time.time.hundredths)));
                  break;
         }
//       pbacnetAnyValue->SetObject(TSTMP, new ::BACnetTimeStamp(pObj->last_restore_time.u.date_time.date.year, pObj->last_restore_time.u.date_time.date.month,
//                                     pObj->last_restore_time.u.date_time.date.day_of_month, pObj->last_restore_time.u.date_time.time.hour,
//                                     pObj->last_restore_time.u.date_time.time.minute, pObj->last_restore_time.u.date_time.time.second, 0));
         break;
     case ACTIVE_COV_SUBSCRIPTIONS:
         pbacnetAnyValue->SetObject(lCOVSub, CreateCOVSubscriptionList(pObj->active_cov_subscriptions));
         break;
     case SLAVE_ADDRESS_BINDING:
         pbacnetAnyValue->SetObject(dabind, CreateAddressBindingList(pObj->slave_add_bind));
         break;
     case MANUAL_SLAVE_ADDRESS_BINDING:
         pbacnetAnyValue->SetObject(dabind, CreateAddressBindingList(pObj->manual_slave_add_bind));
           break;
     case AUTO_SLAVE_DISCOVERY:
        pbacnetAnyValue->SetObject(eboollist, new BACnetBooleanArray(pObj->auto_slave_disc));
        break;
     case SLAVE_PROXY_ENABLE:
        pbacnetAnyValue->SetObject(eboollist, new BACnetBooleanArray(pObj->slave_proxy_enable));
        break;
     case STRUCTURED_OBJECT_LIST:
        pbacnetAnyValue->SetObject(lobj, CreateObjectList(pObj->structured_object_list));
        break;
     case TIME_OF_DEVICE_RESTART:
         switch(pObj->time_of_device_restart.choice)
         {
         case 0:  pbacnetAnyValue->SetObject(TSTMP, new ::BACnetTimeStamp(new ::BACnetTime(pObj->time_of_device_restart.u.time.hour, pObj->time_of_device_restart.u.time.minute, pObj->time_of_device_restart.u.time.second, pObj->time_of_device_restart.u.time.hundredths)));
                  break;
         case 1:  pbacnetAnyValue->SetObject(TSTMP, new ::BACnetTimeStamp(new BACnetUnsigned(pObj->time_of_device_restart.u.sequence_number)));
                  break;
         case 2:  pbacnetAnyValue->SetObject(TSTMP, new ::BACnetTimeStamp(new ::BACnetDateTime(pObj->time_of_device_restart.u.date_time.date.year, pObj->time_of_device_restart.u.date_time.date.month, pObj->time_of_device_restart.u.date_time.date.day_of_month,
                              pObj->time_of_device_restart.u.date_time.time.hour, pObj->time_of_device_restart.u.date_time.time.minute, pObj->time_of_device_restart.u.date_time.time.second, pObj->time_of_device_restart.u.date_time.time.hundredths)));
                  break;
         }
         break;
     default:
        // Not handled here
        return false;
   }

   return true;
}

// Event Enrollment
bool EV_CreateProperty( ee_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case EVENT_PARAMETERS:
         pbacnetAnyValue->SetObject(evparm, new ::BACnetEventParameter());
         //msg->pv= &Obj->parameter_list;
         //msg->pt= evparm;
         break;
      case OBJECT_PROPERTY_REFERENCE:
         // For those who come after !!!
         // As of 9/04, the EPICS parser is storing this property as a propref instead of the
         // devobjpropref it is supposed to be... For sanity's sake, continue to create the
         // BACnetObjectPropertyReference to reflect EPICS.
         // Good luck, should you choose to accept this mission.  This code has already self destructed.
         pbacnetAnyValue->SetObject(devobjpropref, new ::BACnetDeviceObjectPropertyReference(pObj->obj_prop_ref.Objid,
                  pObj->obj_prop_ref.wPropertyid,
                  pObj->obj_prop_ref.ulIndex,
                  pObj->obj_prop_ref.DeviceObj ));
         break;
      case RECIPIENT:
         if ( pObj->recipient.choice == 0 )
            pbacnetAnyValue->SetObject(recip, new ::BACnetRecipient(
                              new ::BACnetAddr( (int) pObj->recipient.u.address.network_number, (octet *) pObj->recipient.u.address.mac_address,
                              (int) pObj->recipient.u.address.address_size)));
         else
            pbacnetAnyValue->SetObject(recip, new ::BACnetRecipient(
                              new ::BACnetObjectIdentifier((unsigned int) pObj->recipient.u.device)));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}

bool FI_CreateProperty( file_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   return false;
}

bool GR_CreateProperty( group_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case LIST_OF_GROUP_MEMBERS:
         pbacnetAnyValue->SetObject(raslist, new ::BACnetReadAccessSpecification());
         //msg->pv= Obj->list_of_group_members;
         //msg->pt= raslist;
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}


bool LO_CreateProperty( loop_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case MANIPULATED_VARIABLE_REFERENCE:
         pbacnetAnyValue->SetObject(propref, new ::BACnetObjectPropertyReference(pObj->man_var_ref.object_id,
                                                               pObj->man_var_ref.property_id,
                                                               pObj->man_var_ref.pa_index));
          break;
      case CONTROLLED_VARIABLE_REFERENCE:
         pbacnetAnyValue->SetObject(propref, new ::BACnetObjectPropertyReference(pObj->cont_var_ref.object_id,
                                                               pObj->cont_var_ref.property_id,
                                                               pObj->cont_var_ref.pa_index));
         break;
      case SETPOINT_REFERENCE:
         // pbacnetAnyValue->SetObject(lopref, new ::BACnetListOfObjectPropertyReference(pObj->setpoint_ref));
         pbacnetAnyValue->SetObject(propref, new ::BACnetObjectPropertyReference(pObj->setpoint_ref[0].object_id,
                                                                     pObj->setpoint_ref[0].property_id,
                                                                     pObj->setpoint_ref[0].pa_index));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}


bool MSI_CreateProperty( mi_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case STATE_TEXT:
         pbacnetAnyValue->SetObject(statext, new BACnetTextArray(pObj->state_text, MAX_STATE_TEXTS));
         break;
      case ALARM_VALUES:
         pbacnetAnyValue->SetObject(stavals, new BACnetUnsignedArray(pObj->alarm_values));
         break;
      case FAULT_VALUES:
         pbacnetAnyValue->SetObject(stavals, new BACnetUnsignedArray(pObj->fault_values));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}


bool MSO_CreateProperty( mo_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case STATE_TEXT:
         pbacnetAnyValue->SetObject(statext, new BACnetTextArray(pObj->state_text, MAX_STATE_TEXTS));
         break;
      case PRIORITY_ARRAY:
         pbacnetAnyValue->SetObject(pau, new BACnetPriorityArray(pObj->priority_array, nPRIO, upaNULL, false));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}

bool MSV_CreateProperty( msv_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);
   switch ( propertyID )
   {
      case STATE_TEXT:
         pbacnetAnyValue->SetObject(statext, new BACnetTextArray(pObj->state_text, MAX_STATE_TEXTS));
         break;
      case PRIORITY_ARRAY:
         pbacnetAnyValue->SetObject(pau, new BACnetPriorityArray(pObj->priority_array, nPRIO, upaNULL, false));
         break;
      case ALARM_VALUES:               // array of words...  was 'uw' type (I think by mistake)
         pbacnetAnyValue->SetObject(stavals, new BACnetUnsignedArray(pObj->alarm_values));
         break;
      case FAULT_VALUES:               // array of words   was 'uw' type (I think by mistake)
         pbacnetAnyValue->SetObject(stavals, new BACnetUnsignedArray(pObj->fault_values));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}


bool NC_CreateProperty( nc_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case PRIORITY:
         pbacnetAnyValue->SetObject(uwarr, new BACnetUnsignedArray(pObj->priority, 3));
         break;
      case RECIPIENT_LIST:
         pbacnetAnyValue->SetObject(reciplist, CreateListOfDestination(pObj->recipient_list));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}


bool PR_CreateProperty( program_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   return false;
}

bool SC_CreateProperty( schedule_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case PRESENT_VALUE:
         // TODO: what about Boolean, other enum, and NULL?
         // value_type is the type of the REFERENCE - WON'T ever be null,
         // but PresentValue MAY be null.
         //
         // TODO: WeeklySchedule and ExceptionSchedule use ActionValueType to specify type,
         // but code here and in Vtsapi32 use ParseType to value_type, tangled up
         // with the "none" hack in Vtsapi32 to tweak value_type based on the referenced property.
         // This is pretty much guaranteed to be broken.
         switch(pObj->value_type)
         {
            case et: pbacnetAnyValue->SetObject(et, new BACnetBinaryPriV(pObj->pv.bproperty_value));
                     break;
            case uw: pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->pv.uproperty_value));
                     break;
            case flt: pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->pv.fproperty_value));
                     break;
            case ebool: pbacnetAnyValue->SetObject(ebool, new BACnetBoolean(pObj->pv.bproperty_value));
                     break;
            default: TRACE0("INVALID TYPE in SC_CreateProperty for PRESENT_VALUE"); ASSERT(0);
         }
         break;
      case EFFECTIVE_PERIOD:
         pbacnetAnyValue->SetObject(dtrange, new ::BACnetDateRange(pObj->effective_period.start_date.year, pObj->effective_period.start_date.month, pObj->effective_period.start_date.day_of_month,
                                                   pObj->effective_period.end_date.year, pObj->effective_period.end_date.month, pObj->effective_period.end_date.day_of_month));
         break;
      case WEEKLY_SCHEDULE:
         pbacnetAnyValue->SetObject(wsched, new ::BACnetTimeValue());
         //msg->pv= Obj->weekly_schedule;
         //msg->pt= wsched;
         break;
      case EXCEPTION_SCHEDULE:
         pbacnetAnyValue->SetObject(xsched, new ::BACnetExceptionSchedule());
         //msg->pv= &Obj->exception_schedule;
         //msg->pt= xsched;
         break;
      case LIST_OF_OBJECT_PROPERTY_REFERENCES:
         // These are actually DEVICE object-property-references
         pbacnetAnyValue->SetObject(lopref, CreateDeviceObjectPropertyList(pObj->list_obj_prop_ref));
         break;
      case SCHEDULE_DEFAULT:
         // TODO: what about Boolean, other enum, and NULL?
         // value_type is the type of the REFERENCE - WON'T ever be null,
         // but schedule-default MAY be null.
         //
         // TODO: WeeklySchedule and ExceptionSchedule use ActionValueType to specify type,
         // but code here and in Vtsapi32 use ParseType to value_type, tangled up
         // with the "none" hack in Vtsapi32 to tweak value_type based on the referenced property.
         // This is pretty much guaranteed to be broken.
         switch(pObj->value_type)
         {
            case et: pbacnetAnyValue->SetObject(et, new BACnetBinaryPriV(pObj->schedule_default.bproperty_value));
                     break;
            case uw: pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->schedule_default.uproperty_value));
                     break;
            case flt: pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->schedule_default.fproperty_value));
                     break;
            case ebool: pbacnetAnyValue->SetObject(ebool, new BACnetBoolean(pObj->pv.bproperty_value));
                     break;
            default: TRACE0("INVALID TYPE in SC_CreateProperty for SCHEDULE_DEFAULT"); ASSERT(0);
         }
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}


bool AVG_CreateProperty( avg_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case OBJECT_PROPERTY_REFERENCE:
         pbacnetAnyValue->SetObject(devobjpropref, new ::BACnetDeviceObjectPropertyReference(pObj->obj_prop_ref.Objid,
                                                               pObj->obj_prop_ref.wPropertyid,
                                                               pObj->obj_prop_ref.ulIndex, pObj->obj_prop_ref.DeviceObj));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}


bool TR_CreateProperty( trend_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case LOG_DEVICE_OBJECT_PROPERTY:
         pbacnetAnyValue->SetObject(devobjpropref, new ::BACnetDeviceObjectPropertyReference(pObj->log_device_object_property.Objid,
                                                               pObj->log_device_object_property.wPropertyid,
                                                               pObj->log_device_object_property.ulIndex, pObj->log_device_object_property.DeviceObj));
         break;
      case LOG_BUFFER:
         // Cannot read this with ReadProperty!
         //msg->pv = &Obj->log_buffer;
         //msg->pt = LOGREC;
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}

// TODO: objects after this point in the file have not been cleaned up: many properties could be handled
// by CreateCommonProperty based on ParseType and the property table.
// More could be if you add some more ParseTypes

bool LFSP_CreateProperty( lifesafetypoint_obj_type * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   const NetworkSniffer::BACnetStringTable *petable;
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
   case PRESENT_VALUE:
      petable = NetworkSniffer::GetEnumStringTable(eiLifeSafetyState);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int) pObj->present_value, *petable ));
      break;
   case TRACKING_VALUE:
      petable = NetworkSniffer::GetEnumStringTable(eiLifeSafetyState);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated(  (int) pObj->tracking_value, *petable ));
      break;
   case DEVICE_TYPE:
      pbacnetAnyValue->SetObject(ch_string, new BACnetCharacterString(pObj->device_type));
      break;
   case MODE:
      petable = NetworkSniffer::GetEnumStringTable(eiLifeSafetyMode);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated(  (int) pObj->mode, *petable ));
      break;
   case ACCEPTED_MODES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->accepted_modes, eiLifeSafetyMode ));
      break;
   case LIFE_SAFETY_ALARM_VALUES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->life_safety_alarm_values, eiLifeSafetyState ));
      break;
   case ALARM_VALUES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->alarm_values, eiLifeSafetyState ));
      break;
   case FAULT_VALUES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->fault_values, eiLifeSafetyState ));
      break;
   case SILENCED:
      petable = NetworkSniffer::GetEnumStringTable(eiSilencedState);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int) pObj->silenced, *petable ));
      break;
   case OPERATION_EXPECTED:
      petable = NetworkSniffer::GetEnumStringTable(eiLifeSafetyOperation);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int) pObj->operation_expected, *petable ));
      break;
   case MAINTENANCE_REQUIRED:
      petable = NetworkSniffer::GetEnumStringTable(eiMaintenance);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int) pObj->maintenance_required, *petable ));
      break;
   case SETTING:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->setting));
      break;
   case DIRECT_READING:
      pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->direct_reading));
      break;
   case MEMBER_OF:
      pbacnetAnyValue->SetObject(lodoref, CreateDeviceObjectList(pObj->member_of));
      break;

   default:
      // Not handled here
      return false;
   }

   return true;
}

bool LFSZ_CreateProperty( lifesafetyzone_obj_type * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   const NetworkSniffer::BACnetStringTable *petable;
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
   case PRESENT_VALUE:
      petable = NetworkSniffer::GetEnumStringTable(eiLifeSafetyState);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->present_value, *petable ));
      break;
   case TRACKING_VALUE:
      petable = NetworkSniffer::GetEnumStringTable(eiLifeSafetyState);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->tracking_value, *petable ));
      break;
   case DEVICE_TYPE:
      pbacnetAnyValue->SetObject(ch_string, new BACnetCharacterString(pObj->device_type));
      break;
   case MODE:
      petable = NetworkSniffer::GetEnumStringTable(eiLifeSafetyMode);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->mode, *petable ));
      break;
   case ACCEPTED_MODES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->accepted_modes, eiLifeSafetyMode ));
      break;
   case LIFE_SAFETY_ALARM_VALUES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->life_safety_alarm_values, eiLifeSafetyState ));
      break;
   case ALARM_VALUES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->alarm_values, eiLifeSafetyState ));
      break;
   case FAULT_VALUES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->fault_values, eiLifeSafetyState ));
      break;
   case SILENCED:
      petable = NetworkSniffer::GetEnumStringTable(eiSilencedState);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->silenced, *petable ));
      break;
   case OPERATION_EXPECTED:
      petable = NetworkSniffer::GetEnumStringTable(eiLifeSafetyOperation);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->operation_expected, *petable ));
      break;
   case MAINTENANCE_REQUIRED:
      pbacnetAnyValue->SetObject(ebool, new BACnetBoolean((int) pObj->maintenance_required));
      break;
   case ZONE_MEMBERS:
      pbacnetAnyValue->SetObject(lodoref, CreateDeviceObjectList(pObj->zone_members));
      break;
   case MEMBER_OF:
      pbacnetAnyValue->SetObject(lodoref, CreateDeviceObjectList(pObj->member_of));
      break;
   default:
      // Not handled here
      return false;
   }

   return true;
}

bool AC_CreateProperty( accumulator_obj_type * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
   case PRESENT_VALUE:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->present_value));
      break;
   case DEVICE_TYPE:
      pbacnetAnyValue->SetObject(ch_string, new BACnetCharacterString(pObj->device_type));
      break;
   case SCALE:
      {
         if(pObj->scale.choice == 0)
         {
            pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->scale.u.floatScale));
         }
         else
         {
            pbacnetAnyValue->SetObject(uw, new BACnetInteger(pObj->scale.u.integerScale));
         }
      }
      break;
   case PRESCALE:
      pbacnetAnyValue->SetObject(eprescl, new ::BACnetPrescale(pObj->prescale.multiplier,
                                 pObj->prescale.moduloDivide));
      break;
   case MAX_PRES_VALUE:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->max_pres_value));
      break;
   case VALUE_CHANGE_TIME:
      pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime(pObj->value_change_time.date.year, pObj->value_change_time.date.month,
                                 pObj->value_change_time.date.day_of_month, pObj->value_change_time.time.hour,
                                 pObj->value_change_time.time.minute, pObj->value_change_time.time.second));
      break;
   case VALUE_BEFORE_CHANGE:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->value_before_change));
      break;
   case VALUE_SET:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->value_set));
      break;
   case LOGGING_RECORD:
      pbacnetAnyValue->SetObject(eaclr, new ::BACnetAccumulatorRecord(pObj->logging_record.timestamp,
      pObj->logging_record.presentValue, pObj->logging_record.accumulatedValue,
      pObj->logging_record.accumulatorStatus));
      break;
   case LOGGING_OBJECT:
      pbacnetAnyValue->SetObject(ob_id, new ::BACnetObjectIdentifier((unsigned int) pObj->logging_device.object_id));
      break;
   case PULSE_RATE:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->pulse_rate));
      break;
   case HIGH_LIMIT:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->high_limit));
      break;
   case LOW_LIMIT:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->low_limit));
      break;
   case LIMIT_MONITORING_INTERVAL:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->limit_monitoring_interval));
      break;
   default:
      // Not handled here
      return false;
   }

   return true;
}

bool PC_CreateProperty( pulseconverter_obj_type * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue *pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
   case PRESENT_VALUE:
      pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->present_value));
      break;
   case INPUT_REFERENCE:
      pbacnetAnyValue->SetObject(propref, new ::BACnetObjectPropertyReference(pObj->input_reference.object_id,
      pObj->input_reference.property_id, pObj->input_reference.pa_index));
      break;
   case SCALE_FACTOR:
      pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->scale_factor));
      break;
   case ADJUST_VALUE:
      pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->adjust_value));
      break;
   case COUNT:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->count));
      break;
   case UPDATE_TIME:
      pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime(pObj->update_time.date.year, pObj->update_time.date.month,
                                 pObj->update_time.date.day_of_month, pObj->update_time.time.hour,
                                 pObj->update_time.time.minute, pObj->update_time.time.second));
      break;
   case COUNT_CHANGE_TIME:
      pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime(pObj->count_change_time.date.year, pObj->count_change_time.date.month,
                                 pObj->count_change_time.date.day_of_month, pObj->count_change_time.time.hour,
                                 pObj->count_change_time.time.minute, pObj->count_change_time.time.second));
      break;
   case COUNT_BEFORE_CHANGE:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->count_before_change));
      break;
   case COV_INCREMENT:
      pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->cov_increment));
      break;
   case COV_PERIOD:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->cov_period));
      break;
   case HIGH_LIMIT:
      pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->high_limit));
      break;
   case LOW_LIMIT:
      pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->low_limit));
      break;
   case DEADBAND:
      pbacnetAnyValue->SetObject(flt, new BACnetReal(pObj->deadband));
      break;
   default:
      // Not handled here
      return false;
   }

   return true;
}

bool LC_CreateProperty( lc_obj_type * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue *pbacnetAnyValue )
{
   const NetworkSniffer::BACnetStringTable *petable;
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
   case PRESENT_VALUE:
      petable = NetworkSniffer::GetEnumStringTable(eiShedState);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->present_value, *petable ));
      break;
   case STATE_DESCRIPTION:
      pbacnetAnyValue->SetObject(ch_string, new BACnetCharacterString(pObj->state_description));
      break;
   case ENABLE:
      pbacnetAnyValue->SetObject(ebool, new BACnetBoolean((int) pObj->log_enable));
      break;
   case REQUESTED_SHED_LEVEL:
      switch( pObj->requested_shed_level.choice )
      {
      // Removed warnings here about converting unsigned long to float
      // just by casting, since they end up as ushorts anyway (so there's enough precision)
      case 0:
         pbacnetAnyValue->SetObject(shedlevel, new ::BACnetShedLevel(pObj->requested_shed_level.choice, (float)pObj->requested_shed_level.sl.uproperty_value));
         break;
      case 1:
         pbacnetAnyValue->SetObject(shedlevel, new ::BACnetShedLevel(pObj->requested_shed_level.choice, (float)pObj->requested_shed_level.sl.uproperty_value));
         break;
      case 2:
         pbacnetAnyValue->SetObject(shedlevel, new ::BACnetShedLevel(pObj->requested_shed_level.choice, pObj->requested_shed_level.sl.fproperty_value));
         break;
      }
      break;
   case START_TIME:
      pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime(pObj->start_time.date.year, pObj->start_time.date.month,
                           pObj->start_time.date.day_of_month, pObj->start_time.time.hour,
                           pObj->start_time.time.minute, pObj->start_time.time.second));
      break;
   case SHED_DURATION:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->shed_duration));
      break;
   case DUTY_WINDOW:
      pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->duty_window));
      break;
   case FULL_DUTY_BASELINE:
      pbacnetAnyValue->SetObject(uw, new BACnetReal(pObj->full_duty_baseline));
      break;
   case EXPECTED_SHED_LEVEL:
      switch( pObj->expected_shed_level.choice )
      {
      case 0:
         pbacnetAnyValue->SetObject(shedlevel, new ::BACnetShedLevel(pObj->expected_shed_level.choice, (float)pObj->expected_shed_level.sl.uproperty_value));
         break;
      case 1:
         pbacnetAnyValue->SetObject(shedlevel, new ::BACnetShedLevel(pObj->expected_shed_level.choice, (float)pObj->expected_shed_level.sl.uproperty_value));
         break;
      case 2:
         pbacnetAnyValue->SetObject(shedlevel, new ::BACnetShedLevel(pObj->expected_shed_level.choice, pObj->expected_shed_level.sl.fproperty_value));
         break;
      }
      break;
   case ACTUAL_SHED_LEVEL:
      switch( pObj->actual_shed_level.choice )
      {
      case 0:
         pbacnetAnyValue->SetObject(shedlevel, new ::BACnetShedLevel(pObj->actual_shed_level.choice, (float)pObj->actual_shed_level.sl.uproperty_value));
         break;
      case 1:
         pbacnetAnyValue->SetObject(shedlevel, new ::BACnetShedLevel(pObj->actual_shed_level.choice, (float)pObj->actual_shed_level.sl.uproperty_value));
         break;
      case 2:
         pbacnetAnyValue->SetObject(shedlevel, new ::BACnetShedLevel(pObj->actual_shed_level.choice, pObj->actual_shed_level.sl.fproperty_value));
         break;
      }
      break;
   case SHED_LEVELS:
      pbacnetAnyValue->SetObject(stavals, new BACnetUnsignedArray(pObj->shed_levels));
      break;
   case SHED_LEVEL_DESCRIPTIONS:
      pbacnetAnyValue->SetObject(statext, new BACnetTextArray(pObj->shed_level_descriptions, 255));
      break;
   default:
      // Not handled here
      return false;
   }

   return true;
}

bool AD_CreateProperty( ad_obj_type * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue *pbacnetAnyValue )
{
   const NetworkSniffer::BACnetStringTable *petable;
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
   case PRESENT_VALUE:
      petable = NetworkSniffer::GetEnumStringTable(eiDoorValue);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->present_value, *petable ));
      break;
   case RELINQUISH_DEFAULT:
      petable = NetworkSniffer::GetEnumStringTable(eiDoorValue);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->relinquish_default, *petable ));
      break;
   case PRIORITY_ARRAY:
      pbacnetAnyValue->SetObject(pae, new BACnetPriorityArray((unsigned short *) pObj->priority_array, nPRIO, dvaNULL, true));
      break;
   case DOOR_STATUS:
      petable = NetworkSniffer::GetEnumStringTable(eiDoorStatus);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->door_status, *petable ));
      break;
   case LOCK_STATUS:
      petable = NetworkSniffer::GetEnumStringTable(eiLockStatus);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->lock_status, *petable ));
      break;
   case SECURED_STATUS:
      petable = NetworkSniffer::GetEnumStringTable(eiDoorSecuredStatus);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->secured_status, *petable ));
      break;
   case DOOR_MEMBERS:
      pbacnetAnyValue->SetObject(lodoref, CreateDeviceObjectList(pObj->door_members));
      break;
   case DOOR_PULSE_TIME:
      pbacnetAnyValue->SetObject(ud, new BACnetUnsigned((unsigned long) pObj->door_pulse_time));
      break;
   case DOOR_EXTENDED_PULSE_TIME:
      pbacnetAnyValue->SetObject(ud, new BACnetUnsigned((unsigned long) pObj->door_extended_pulse_time));
      break;
   case DOOR_UNLOCK_DELAY_TIME:
      pbacnetAnyValue->SetObject(ud, new BACnetUnsigned((unsigned long) pObj->door_unlock_delay_time));
      break;
   case DOOR_OPEN_TOO_LONG_TIME:
      pbacnetAnyValue->SetObject(ud, new BACnetUnsigned((unsigned long) pObj->door_open_too_long_time));
      break;
   case DOOR_ALARM_STATE:
      petable = NetworkSniffer::GetEnumStringTable(eiDoorAlarmState);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->door_alarm_state, *petable ));
      break;
   case MASKED_ALARM_VALUES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->masked_alarm_values, eiDoorAlarmState ));
      break;
   case MAINTENANCE_REQUIRED:
      petable = NetworkSniffer::GetEnumStringTable(eiMaintenance);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->maintenance_required, *petable ));
      break;
   case ALARM_VALUES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->alarm_values, eiDoorAlarmState ));
      break;
   case FAULT_VALUES:
      pbacnetAnyValue->SetObject(etl, CreateEnumList( pObj->fault_values, eiDoorAlarmState ));
      break;
   default:
      // Not handled here
      return false;
   }

   return true;
}

bool SV_CreateProperty( sv_obj_type * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue *pbacnetAnyValue )
{
   const NetworkSniffer::BACnetStringTable *petable;
   ASSERT(pbacnetAnyValue != NULL);
   switch ( propertyID )
   {
   case NODE_TYPE:
      petable = NetworkSniffer::GetEnumStringTable(eiNodeType);
      pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->node_type, *petable ));
      break;
   case NODE_SUBTYPE:
      pbacnetAnyValue->SetObject(ch_string, new BACnetCharacterString(pObj->node_subtype));
      break;
   case SUBORDINATE_LIST:
      pbacnetAnyValue->SetObject(lodoref, CreateDeviceObjectList(pObj->subordinate_list));
      break;
   case SUBORDINATE_ANNOTATIONS:
      pbacnetAnyValue->SetObject(statext, new BACnetTextArray(pObj->subordinate_annotations, MAX_SV_ANNOTATIONS));
      break;
   default:
      // Not handled here
      return false;
   }

   return true;
}

bool EL_CreateProperty( el_obj_type * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue *pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
      case ENABLE:
         pbacnetAnyValue->SetObject(ebool, new BACnetBoolean((int) pObj->log_enable));
         break;
      case START_TIME:
         pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime(pObj->start_time.date.year, pObj->start_time.date.month,
                                    pObj->start_time.date.day_of_month, pObj->start_time.time.hour,
                                    pObj->start_time.time.minute, pObj->start_time.time.second));
         break;
      case STOP_TIME:
         pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime(pObj->stop_time.date.year, pObj->stop_time.date.month,
                                    pObj->stop_time.date.day_of_month, pObj->stop_time.time.hour,
                                    pObj->stop_time.time.minute, pObj->stop_time.time.second));
         break;
      case BUFFER_SIZE:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->buffer_size));
         break;
      case LOG_BUFFER:
         // Cannot read this with ReadProperty!
         //msg->pv = &Obj->log_buffer;
         //msg->pt = LOGREC;
         break;
      case RECORD_COUNT:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->record_count));
         break;
      case TOTAL_RECORD_COUNT:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->total_record_count));
         break;
      case NOTIFICATION_THRESHOLD:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->notification_threshold));
         break;
      case RECORDS_SINCE_NOTIFICATION:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->records_since_notification));
         break;
      case LAST_NOTIFY_RECORD:
         pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->last_notify_record));
         break;
      default:
         // Not handled here
         return false;
   }

   return true;
}

bool TLM_CreateProperty( tlm_obj_type * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue *pbacnetAnyValue )
{
   const NetworkSniffer::BACnetStringTable *petable;
   ASSERT(pbacnetAnyValue != NULL);

   switch ( propertyID )
   {
   case ENABLE:
         pbacnetAnyValue->SetObject(ebool, new BACnetBoolean((int) pObj->log_enable));
         break;
   case START_TIME:
         pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime(pObj->start_time.date.year, pObj->start_time.date.month,
                                    pObj->start_time.date.day_of_month, pObj->start_time.time.hour,
                                    pObj->start_time.time.minute, pObj->start_time.time.second));
         break;
   case STOP_TIME:
         pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime(pObj->stop_time.date.year, pObj->stop_time.date.month,
                                    pObj->stop_time.date.day_of_month, pObj->stop_time.time.hour,
                                    pObj->stop_time.time.minute, pObj->stop_time.time.second));
         break;
   case LOG_DEVICE_OBJECT_PROPERTY:
         pbacnetAnyValue->SetObject(devobjpropref, new ::BACnetDeviceObjectPropertyReference(pObj->log_device_object_property.Objid,
                                                               pObj->log_device_object_property.wPropertyid,
                                                               pObj->log_device_object_property.ulIndex, pObj->log_device_object_property.DeviceObj));
         break;
   case LOGGING_TYPE:
         petable = NetworkSniffer::GetEnumStringTable(eiLoggingType);
         pbacnetAnyValue->SetObject(et, new BACnetEnumerated( (int)pObj->logging_type, *petable ));
         break;
   case LOG_INTERVAL:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->log_interval));
         break;
   case ALIGN_INTERVALS:
         pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->align_intervals));
         break;
   case INTERVAL_OFFSET:
         pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->interval_offset));
         break;
   case TRIGGER:
         pbacnetAnyValue->SetObject(uw, new BACnetBoolean(pObj->trigger));
         break;

   case STOP_WHEN_FULL:
         pbacnetAnyValue->SetObject(ebool, new BACnetBoolean((int) pObj->stop_when_full));
         break;
   case BUFFER_SIZE:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->buffer_size));
         break;
   case LOG_BUFFER:
         // Cannot read this with ReadProperty!
         //msg->pv = &Obj->log_buffer;
         //msg->pt = LOGREC;
         break;
   case RECORD_COUNT:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->record_count));
         break;
   case TOTAL_RECORD_COUNT:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->total_record_count));
         break;
   case NOTIFICATION_THRESHOLD:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->notification_threshold));
         break;
   case RECORDS_SINCE_NOTIFICATION:
         pbacnetAnyValue->SetObject(ud, new BACnetUnsigned(pObj->records_since_notification));
         break;
   case LAST_NOTIFY_RECORD:
         pbacnetAnyValue->SetObject(uw, new BACnetUnsigned(pObj->last_notify_record));
         break;
   default:
         // Not handled here
         return false;
   }

   return true;
}


bool CharStr_CreateProperty( charstring_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);
   switch ( propertyID )
   {
      case PRESENT_VALUE:
         pbacnetAnyValue->SetObject(ch_string, new BACnetCharacterString( pObj->present_value));
         break;
      case RELINQUISH_DEFAULT:
         pbacnetAnyValue->SetObject(ch_string, new BACnetCharacterString(pObj->relinquish_default));
         break;
//    case PRIORITY_ARRAY:
//       // TODO: make me an array of strings, please
//       pbacnetAnyValue->SetObject(pa??, new BACnetPriorityArray(pObj->priority_array, nPRIO, upaNULL, false));
//       break;
      case ALARM_VALUES:               // array of words...  was 'uw' type (I think by mistake)
         pbacnetAnyValue->SetObject(actext, new BACnetTextArray(pObj->alarm_values, MAX_FAULT_STRINGS));
         break;
      case FAULT_VALUES:               // array of words   was 'uw' type (I think by mistake)
         pbacnetAnyValue->SetObject(actext, new BACnetTextArray(pObj->fault_values, MAX_FAULT_STRINGS));
         break;

      default:
         // Not handled here
         return false;
   }

   return true;
}

bool Integer_CreateProperty( integer_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);
   switch ( propertyID )
   {
      case PRESENT_VALUE:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->present_value ));
         break;
//    case PRIORITY_ARRAY:
//       pbacnetAnyValue->SetObject(ptPai, new BACnetPriorityArray(pObj->priority_array, nPRIO, fpaNULL));
//       break;
      case RELINQUISH_DEFAULT:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->relinquish_default));
         break;
      case COV_INCREMENT:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->cov_increment));
         break;
      case HIGH_LIMIT:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->high_limit));
         break;
      case LOW_LIMIT:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->low_limit));
         break;
      case DEADBAND:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->deadband));
         break;

      default:
         // Not handled here
         return false;
   }

   return true;
}


bool Pos_Int_CreateProperty( positive_integer_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);
   switch ( propertyID )
   {
      case PRESENT_VALUE:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->present_value ));
         break;
//    case PRIORITY_ARRAY:
//       pbacnetAnyValue->SetObject(ptPai, new BACnetPriorityArray(pObj->priority_array, nPRIO, fpaNULL));
//       break;
      case RELINQUISH_DEFAULT:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->relinquish_default));
         break;
      case COV_INCREMENT:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->cov_increment));
         break;
      case HIGH_LIMIT:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->high_limit));
         break;
      case LOW_LIMIT:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->low_limit));
         break;
      case DEADBAND:
         pbacnetAnyValue->SetObject(ptInt32, new BACnetInteger( (int) pObj->deadband));
         break;

      default:
         // Not handled here
         return false;
   }

   return true;
}


bool DateTime_CreateProperty( datetimevalue_obj_type far * pObj, BACnetPropertyIdentifier propertyID, BACnetAnyValue * pbacnetAnyValue )
{
   ASSERT(pbacnetAnyValue != NULL);
   switch ( propertyID )
   {
      case PRESENT_VALUE:
         pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime( pObj->present_value ) );
         break;
//    case PRIORITY_ARRAY:
//       pbacnetAnyValue->SetObject(ptPai, new BACnetPriorityArray(pObj->priority_array, nPRIO, fpaNULL));
//       break;
      case RELINQUISH_DEFAULT:
         pbacnetAnyValue->SetObject(dt, new ::BACnetDateTime( pObj->relinquish_default ) );
         break;
      case IS_UTC:
         pbacnetAnyValue->SetObject(ebool, new BACnetBoolean( (int) pObj->is_utc));
         break;

      default:
         // Not handled here
         return false;
   }

   return true;
}


// Create separate function to hide the nasty internal EPICS structures until we can
// replace them with BACnet objects

BACnetListOfCalendarEntry * CreateCalendarList( PICS::BACnetCalendarEntry * pcalentry )
{
   BACnetListOfCalendarEntry * pbacnetCalList = new BACnetListOfCalendarEntry();

   for ( ; pcalentry != NULL; pcalentry = pcalentry->next )
   {
      ::BACnetCalendarEntry * pcalentryObj;

      switch(pcalentry->choice)
      {
      case 0:     pcalentryObj = new ::BACnetCalendarEntry(new ::BACnetDate(pcalentry->u.date.year, pcalentry->u.date.month, pcalentry->u.date.day_of_month));
                  break;
      case 1:     pcalentryObj = new ::BACnetCalendarEntry(new ::BACnetDateRange(pcalentry->u.date_range.start_date.year, pcalentry->u.date_range.start_date.month, pcalentry->u.date_range.start_date.day_of_month,
                                                                  pcalentry->u.date_range.end_date.year, pcalentry->u.date_range.end_date.month, pcalentry->u.date_range.end_date.day_of_month));
                  break;
      case 2:     pcalentryObj = new ::BACnetCalendarEntry(new ::BACnetWeekNDay(pcalentry->u.weekNday.month, pcalentry->u.weekNday.week, pcalentry->u.weekNday.day));
                  break;
      default:    ASSERT(0);
                  break;
      }
      pbacnetCalList->Add(pcalentryObj);
   }

   return pbacnetCalList;
}



BACnetTimeStampArray * CreateTimeStampArray( PICS::BACnetTimeStamp *ptimestamp[3], int nMax )
{
   BACnetTimeStampArray * pbacnetArray = new BACnetTimeStampArray();  // (nMax) don't set because puts empty array values and no way to set them

   for ( int i = 0; i < nMax; i++ )
   {
      // If the PICS doesn't specify timestamps, we may get an array of NULL pointers.
      if (ptimestamp[i] != NULL)
      {
         switch(ptimestamp[i]->choice)
         {
         case 0:  pbacnetArray->Add( new ::BACnetTimeStamp(new ::BACnetTime(ptimestamp[i]->u.time.hour, ptimestamp[i]->u.time.minute, ptimestamp[i]->u.time.second, ptimestamp[i]->u.time.hundredths)));
                  break;
         case 1:  pbacnetArray->Add( new ::BACnetTimeStamp(new BACnetUnsigned(ptimestamp[i]->u.sequence_number)));
                  break;
         case 2:  pbacnetArray->Add( new ::BACnetTimeStamp(new ::BACnetDateTime(ptimestamp[i]->u.date_time.date.year, ptimestamp[i]->u.date_time.date.month, ptimestamp[i]->u.date_time.date.day_of_month,
                                     ptimestamp[i]->u.date_time.time.hour, ptimestamp[i]->u.date_time.time.minute, ptimestamp[i]->u.date_time.time.second, ptimestamp[i]->u.date_time.time.hundredths)));
                  break;
         }
      }
      else
      {
         // Make a default timestamp.
         // We choose DateTime, with 0xFF in each position as specified in clause 12
         pbacnetArray->Add( new ::BACnetTimeStamp(new ::BACnetDateTime(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF)) );
      }
   }

   return pbacnetArray;
}

BACnetListOfDeviceObjectReference * CreateDeviceObjectList( PICS::BACnetDeviceObjectReference * pdevobjlist )
{
   BACnetListOfDeviceObjectReference * pbacnetList = new BACnetListOfDeviceObjectReference();
   for ( ; pdevobjlist != NULL; pdevobjlist = pdevobjlist->next )
      pbacnetList->Add(new ::BACnetDeviceObjectReference( pdevobjlist->Objid, pdevobjlist->DeviceObj));
   return pbacnetList;
}

BACnetListOfDeviceObjectPropertyReference * CreateDeviceObjectPropertyList( PICS::BACnetDeviceObjectPropertyReference * pdevobjlist )
{
   BACnetListOfDeviceObjectPropertyReference * pbacnetList = new BACnetListOfDeviceObjectPropertyReference();
   for ( ; pdevobjlist != NULL; pdevobjlist = pdevobjlist->next )
      pbacnetList->Add(new ::BACnetDeviceObjectPropertyReference( pdevobjlist->Objid, pdevobjlist->wPropertyid, pdevobjlist->ulIndex, pdevobjlist->DeviceObj));
   return pbacnetList;
}

BACnetObjectIDList * CreateObjectList( PICS::BACnetObjectIdentifier * pobjidlist )
{
   BACnetObjectIDList * pbacnetList = new BACnetObjectIDList();

   for ( ; pobjidlist != NULL; pobjidlist = pobjidlist->next )
      pbacnetList->Add(new ::BACnetObjectIdentifier((unsigned int) pobjidlist->object_id));

   return pbacnetList;
}


BACnetGenericArray * CreateAddressBindingList( PICS::BACnetAddressBinding * pAddrBinding )
{
   BACnetGenericArray * pbacnetArray = new BACnetGenericArray(dabindelem);

   for ( ; pAddrBinding != NULL; pAddrBinding = pAddrBinding->next )
      pbacnetArray->Add(new ::BACnetAddressBinding(pAddrBinding->device_object_id, pAddrBinding->device_address.network_number,
                                       pAddrBinding->device_address.mac_address, pAddrBinding->device_address.address_size));

   return pbacnetArray;
}

BACnetListOfEnum * CreateEnumList( PICS::BACnetEnumList * penumList, int iTableIndex )
{
   BACnetListOfEnum * pbacnetList = new BACnetListOfEnum(iTableIndex);

   const NetworkSniffer::BACnetStringTable *petable = NetworkSniffer::GetEnumStringTable(iTableIndex);
   for ( ; penumList != NULL; penumList = penumList->next )
      pbacnetList->Add(new ::BACnetEnumerated((int)penumList->value, *petable ));

   return pbacnetList;
}

BACnetListOfVTClass * CreateVTClassesSupported( PICS::BACnetVTClassList * pVTClassList )
{
   BACnetListOfVTClass * pbacnetArray = new BACnetListOfVTClass();

   // get pointer to the table describing the VTClasses enumeration
   const NetworkSniffer::BACnetStringTable *petable = NetworkSniffer::GetEnumStringTable(eiVTCls);
   for ( ; pVTClassList != NULL; pVTClassList = pVTClassList->next )
      pbacnetArray->Add(new ::BACnetEnumerated((int)pVTClassList->vtclass , *petable ));

   return pbacnetArray;
}


BACnetListOfRecipient * CreateRecipientList( PICS::BACnetRecipient * precip )
{
   BACnetListOfRecipient * pbacnetArray = new BACnetListOfRecipient();

   for ( ; precip != NULL; precip = precip->next )
   {
      ::BACnetRecipient * prc = new ::BACnetRecipient();

      if ( precip->choice == 0 )
         prc->SetObject( new ::BACnetObjectIdentifier((unsigned int) precip->u.device));
      else
         prc->SetObject( new ::BACnetAddr((unsigned int) precip->u.address.network_number, (BACnetOctet *) precip->u.address.mac_address, (unsigned int) precip->u.address.address_size));

      pbacnetArray->Add(prc);
   }

   return pbacnetArray;
}



BACnetListOfCOVSubscription * CreateCOVSubscriptionList( PICS::BACnetCOVSubscription * psub )
{
   BACnetListOfCOVSubscription * pbacnetArray = new BACnetListOfCOVSubscription();

   for ( ; psub != NULL; psub = psub->next )
   {
      ::BACnetRecipient    rc;

      if ( psub->recipient.recipient.choice == 0 )
         rc.SetObject( new ::BACnetObjectIdentifier((unsigned int) psub->recipient.recipient.u.device));
      else
         rc.SetObject( new ::BACnetAddr((unsigned int) psub->recipient.recipient.u.address.network_number, (BACnetOctet *) psub->recipient.recipient.u.address.mac_address, (unsigned int) psub->recipient.recipient.u.address.address_size));

      ::BACnetRecipientProcess rcp(rc, (unsigned int) psub->recipient.process_id);
      ::BACnetObjectPropertyReference opr(psub->monitoredPropertyReference.object_id,
                                 psub->monitoredPropertyReference.property_id,
                                 psub->monitoredPropertyReference.pa_index );

      pbacnetArray->Add(new ::BACnetCOVSubscription(rcp, opr, psub->notification != 0, psub->timeRemaining, psub->covIncrement));
   }

   return pbacnetArray;
}


BACnetListOfDestination * CreateListOfDestination( PICS::BACnetDestination * pdest )
{
   BACnetListOfDestination * pbacnetArray = new BACnetListOfDestination();

   for ( ; pdest != NULL; pdest = pdest->next )
   {
      ::BACnetTime         fromTime(pdest->from_time.hour, pdest->from_time.minute, pdest->from_time.second, pdest->from_time.hundredths );
      ::BACnetTime         toTime(pdest->to_time.hour, pdest->to_time.minute, pdest->to_time.second, pdest->to_time.hundredths );
      ::BACnetUnsigned     processID(pdest->process_id);
      ::BACnetBoolean      issueConfirmedNotifications(pdest->notification);

      ::BACnetRecipient    rc;

      if ( pdest->recipient.choice == 0 )
         rc.SetObject( new ::BACnetObjectIdentifier((unsigned int) pdest->recipient.u.device));
      else
         rc.SetObject( new ::BACnetAddr((unsigned int) pdest->recipient.u.address.network_number, (BACnetOctet *) pdest->recipient.u.address.mac_address, (unsigned int) pdest->recipient.u.address.address_size));

      ::BACnetDaysOfWeek      validDays(  (pdest->valid_days & (1<<7)) == (1<<7), //monday
                                 (pdest->valid_days & (1<<6)) == (1<<6), //tuesday
                                 (pdest->valid_days & (1<<5)) == (1<<5), //wednesday
                                 (pdest->valid_days & (1<<4)) == (1<<4), //thursday
                                 (pdest->valid_days & (1<<3)) == (1<<3), //friday
                                 (pdest->valid_days & (1<<2)) == (1<<2), //saturday
                                 (pdest->valid_days & (1<<1)) == (1<<1) ); // sunday

      ::BACnetEventTransitionBits transitions( (pdest->transitions & (1<<7)) == (1<<7), //offnormal
                                     (pdest->transitions & (1<<6)) == (1<<6), //fault
                                     (pdest->transitions & (1<<5)) == (1<<5) ); //normal

      pbacnetArray->Add(new ::BACnetDestination(validDays, fromTime, toTime, rc, processID, issueConfirmedNotifications, transitions));
   }

   return pbacnetArray;
}

} // end of PICS namespace

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: the remainder of this file has nothing to do with PICS.
// It should probably be combined with the property tables in stdobjpr.h into a new source file,
// ObjectTables.cpp.
// If we commit to making a FULL table for EVERY new object, the functions of these tables could be
// replaced by the property-id portion of the object tables.
//
// Or, we could beef up parseType and replace most of the gnarly EPICS parsing and object-storage code
// with simpler/clearer/shared C++ stuff using the BACnet.cpp objects.
//
// Check_Obj_Prop is currently used only by the property combo.
// Note that the combo-filling could more efficiently use the arrays of supported properties
// directly...

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier analogInput_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   DEVICE_TYPE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   UPDATE_INTERVAL,
   UNITS,
   MIN_PRES_VALUE,
   MAX_PRES_VALUE,
   RESOLUTION,
   COV_INCREMENT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   HIGH_LIMIT,
   LOW_LIMIT,
   DEADBAND,
   LIMIT_ENABLE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier analogOutput_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   DEVICE_TYPE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   UNITS,
   MIN_PRES_VALUE,
   MAX_PRES_VALUE,
   RESOLUTION,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   COV_INCREMENT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   HIGH_LIMIT,
   LOW_LIMIT,
   DEADBAND,
   LIMIT_ENABLE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME,
};

// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier analogValue_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   UNITS,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   COV_INCREMENT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   HIGH_LIMIT,
   LOW_LIMIT,
   DEADBAND,
   LIMIT_ENABLE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   MIN_PRES_VALUE,
   MAX_PRES_VALUE,
   RESOLUTION,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier binaryInput_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   DEVICE_TYPE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   POLARITY,
   INACTIVE_TEXT,
   ACTIVE_TEXT,
   CHANGE_OF_STATE_TIME,
   CHANGE_OF_STATE_COUNT,
   TIME_OF_STATE_COUNT_RESET,
   ELAPSED_ACTIVE_TIME,
   TIME_OF_ACTIVE_TIME_RESET,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   ALARM_VALUE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier binaryOutput_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   DEVICE_TYPE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   POLARITY,
   INACTIVE_TEXT,
   ACTIVE_TEXT,
   CHANGE_OF_STATE_TIME,
   CHANGE_OF_STATE_COUNT,
   TIME_OF_STATE_COUNT_RESET,
   ELAPSED_ACTIVE_TIME,
   TIME_OF_ACTIVE_TIME_RESET,
   MINIMUM_OFF_TIME,
   MINIMUM_ON_TIME,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   FEEDBACK_VALUE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier binaryValue_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   INACTIVE_TEXT,
   ACTIVE_TEXT,
   CHANGE_OF_STATE_TIME,
   CHANGE_OF_STATE_COUNT,
   TIME_OF_STATE_COUNT_RESET,
   ELAPSED_ACTIVE_TIME,
   TIME_OF_ACTIVE_TIME_RESET,
   MINIMUM_OFF_TIME,
   MINIMUM_ON_TIME,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   ALARM_VALUE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Calendar
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier calendar_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   DATE_LIST,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Command
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier command_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   IN_PROCESS,
   ALL_WRITES_SUCCESSFUL,
   ACTION,
   ACTION_TEXT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Device
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier device_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   SYSTEM_STATUS,
   VENDOR_NAME,
   VENDOR_IDENTIFIER,
   MODEL_NAME,
   FIRMWARE_REVISION,
   APPLICATION_SOFTWARE_VERSION,
   LOCATION,
   DESCRIPTION,
   PROTOCOL_VERSION,
   PROTOCOL_REVISION,
   PROTOCOL_SERVICES_SUPPORTED,
   PROTOCOL_OBJECT_TYPES_SUPPORTED,
   OBJECT_LIST,
   STRUCTURED_OBJECT_LIST,
   MAX_APDU_LENGTH_ACCEPTED,
   SEGMENTATION_SUPPORTED,
   MAX_SEGMENTS_ACCEPTED,
   VT_CLASSES_SUPPORTED,
   ACTIVE_VT_SESSIONS,
   LOCAL_TIME,
   LOCAL_DATE,
   UTC_OFFSET,
   DAYLIGHT_SAVINGS_STATUS,
   APDU_SEGMENT_TIMEOUT,
   APDU_TIMEOUT,
   NUMBER_OF_APDU_RETRIES,
   TIME_SYNCHRONIZATION_RECIPIENTS,
   MAX_MASTER,
   MAX_INFO_FRAMES,
   DEVICE_ADDRESS_BINDING,
   DATABASE_REVISION,
   CONFIGURATION_FILES,
   LAST_RESTORE_TIME,
   BACKUP_FAILURE_TIMEOUT,
   BACKUP_PREPARATION_TIME,
   RESTORE_PREPARATION_TIME,
   RESTORE_COMPLETION_TIME,
   BACKUP_AND_RESTORE_STATE,
   ACTIVE_COV_SUBSCRIPTIONS,
   SLAVE_PROXY_ENABLE,
   MANUAL_SLAVE_ADDRESS_BINDING,
   AUTO_SLAVE_DISCOVERY,
   SLAVE_ADDRESS_BINDING,
   LAST_RESTART_REASON,
   TIME_OF_DEVICE_RESTART,
   RESTART_NOTIFICATION_RECIPIENTS,
   UTC_TIME_SYNCHRONIZATION_RECIPIENTS,
   TIME_SYNCHRONIZATION_INTERVAL,
   ALIGN_INTERVALS,
   INTERVAL_OFFSET,
   SERIAL_NUMBER,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Event Enrollment
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier eventEnrollment_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   EVENT_TYPE,
   NOTIFY_TYPE,
   EVENT_PARAMETERS,
   OBJECT_PROPERTY_REFERENCE,
   EVENT_STATE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFICATION_CLASS,
   EVENT_TIME_STAMPS,
   // note these were removed for revision 4
   RECIPIENT,
   PROCESS_IDENTIFIER,
   PRIORITY,
   ISSUE_CONFIRMED_NOTIFICATIONS,
   // note the above were removed for revision 4
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   STATUS_FLAGS,
   RELIABILITY,
   FAULT_TYPE,
   FAULT_PARAMETERS,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// File
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier file_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   FILE_TYPE,
   FILE_SIZE,
   MODIFICATION_DATE,
   ARCHIVE,
   READ_ONLY,
   FILE_ACCESS_METHOD,
   RECORD_COUNT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Group
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier group_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   LIST_OF_GROUP_MEMBERS,
   PRESENT_VALUE,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Loop
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier loop_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   UPDATE_INTERVAL,
   OUTPUT_UNITS,
   MANIPULATED_VARIABLE_REFERENCE,
   CONTROLLED_VARIABLE_REFERENCE,
   CONTROLLED_VARIABLE_VALUE,
   CONTROLLED_VARIABLE_UNITS,
   SETPOINT_REFERENCE,
   SETPOINT,
   ACTION,
   PROPORTIONAL_CONSTANT,
   PROPORTIONAL_CONSTANT_UNITS,
   INTEGRAL_CONSTANT,
   INTEGRAL_CONSTANT_UNITS,
   DERIVATIVE_CONSTANT,
   DERIVATIVE_CONSTANT_UNITS,
   BIAS,
   MAXIMUM_OUTPUT,
   MINIMUM_OUTPUT,
   PRIORITY_FOR_WRITING,
   COV_INCREMENT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   ERROR_LIMIT,
   DEADBAND,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier multistateInput_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   DEVICE_TYPE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   NUMBER_OF_STATES,
   STATE_TEXT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   ALARM_VALUES,
   FAULT_VALUES,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier multistateOutput_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   DEVICE_TYPE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   NUMBER_OF_STATES,
   STATE_TEXT,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   FEEDBACK_VALUE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier multistateValue_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   NUMBER_OF_STATES,
   STATE_TEXT,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   ALARM_VALUES,
   FAULT_VALUES,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Notification Class
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier notificationClass_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   NOTIFICATION_CLASS,
   PRIORITY,
   ACK_REQUIRED,
   RECIPIENT_LIST,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Program
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier program_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PROGRAM_STATE,
   PROGRAM_CHANGE,
   REASON_FOR_HALT,
   DESCRIPTION_OF_HALT,
   PROGRAM_LOCATION,
   DESCRIPTION,
   INSTANCE_OF,
   STATUS_FLAGS,
   RELIABILITY,
   OUT_OF_SERVICE,
   EVENT_DETECTION_ENABLE,
   NOTIFICATION_CLASS,
   EVENT_ENABLE,
   EVENT_STATE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Schedule
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier schedule_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   EFFECTIVE_PERIOD,
   WEEKLY_SCHEDULE,
   EXCEPTION_SCHEDULE,
   SCHEDULE_DEFAULT,
   LIST_OF_OBJECT_PROPERTY_REFERENCES,
   PRIORITY_FOR_WRITING,
   STATUS_FLAGS,
   RELIABILITY,
   OUT_OF_SERVICE,
   EVENT_DETECTION_ENABLE,
   NOTIFICATION_CLASS,
   EVENT_ENABLE,
   EVENT_STATE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Averaging
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier averaging_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   MINIMUM_VALUE,
   MINIMUM_VALUE_TIMESTAMP,
   AVERAGE_VALUE,
   VARIANCE_VALUE,
   MAXIMUM_VALUE,
   MAXIMUM_VALUE_TIMESTAMP,
   DESCRIPTION,
   ATTEMPTED_SAMPLES,
   VALID_SAMPLES,
   OBJECT_PROPERTY_REFERENCE,
   WINDOW_INTERVAL,
   WINDOW_SAMPLES,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Trend
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier trend_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   ENABLE,
   START_TIME,
   STOP_TIME,
   LOG_DEVICE_OBJECT_PROPERTY,
   LOG_INTERVAL,
   COV_RESUBSCRIPTION_INTERVAL,
   CLIENT_COV_INCREMENT,
   STOP_WHEN_FULL,
   BUFFER_SIZE,
   LOG_BUFFER,
   RECORD_COUNT,
   TOTAL_RECORD_COUNT,
   LOGGING_TYPE,
   ALIGN_INTERVALS,
   INTERVAL_OFFSET,
   TRIGGER,
   STATUS_FLAGS,
   RELIABILITY,
   NOTIFICATION_THRESHOLD,
   RECORDS_SINCE_NOTIFICATION,
   LAST_NOTIFY_RECORD,
   EVENT_STATE,
   NOTIFICATION_CLASS,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Life Safety Point
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier lifeSafetyPoint_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   TRACKING_VALUE,
   DESCRIPTION,
   DEVICE_TYPE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   MODE,
   ACCEPTED_MODES,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   LIFE_SAFETY_ALARM_VALUES,
   ALARM_VALUES,
   FAULT_VALUES,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   SILENCED,
   OPERATION_EXPECTED,
   MAINTENANCE_REQUIRED,
   SETTING,
   DIRECT_READING,
   UNITS,
   MEMBER_OF,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Life Safety Zone
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier lifeSafetyZone_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   TRACKING_VALUE,
   DESCRIPTION,
   DEVICE_TYPE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   MODE,
   ACCEPTED_MODES,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   LIFE_SAFETY_ALARM_VALUES,
   ALARM_VALUES,
   FAULT_VALUES,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   SILENCED,
   OPERATION_EXPECTED,
   MAINTENANCE_REQUIRED,
   ZONE_MEMBERS,
   MEMBER_OF,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Accumulator
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier accumulator_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   DEVICE_TYPE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   SCALE,
   UNITS,
   PRESCALE,
   MAX_PRES_VALUE,
   VALUE_CHANGE_TIME,
   VALUE_BEFORE_CHANGE,
   VALUE_SET,
   LOGGING_RECORD,
   LOGGING_OBJECT,
   PULSE_RATE,
   HIGH_LIMIT,
   LOW_LIMIT,
   LIMIT_MONITORING_INTERVAL,
   NOTIFICATION_CLASS,
   TIME_DELAY,
   LIMIT_ENABLE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Pulse Converter
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier pulseConverter_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   INPUT_REFERENCE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   UNITS,
   SCALE_FACTOR,
   ADJUST_VALUE,
   COUNT,
   UPDATE_TIME,
   COUNT_CHANGE_TIME,
   COUNT_BEFORE_CHANGE,
   COV_INCREMENT,
   COV_PERIOD,
   NOTIFICATION_CLASS,
   TIME_DELAY,
   HIGH_LIMIT,
   LOW_LIMIT,
   DEADBAND,
   LIMIT_ENABLE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Event Log
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier eventLog_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PROPERTY_LIST,
   DESCRIPTION,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   ENABLE,
   START_TIME,
   STOP_TIME,
   STOP_WHEN_FULL,
   BUFFER_SIZE,
   LOG_BUFFER,
   RECORD_COUNT,
   TOTAL_RECORD_COUNT,
   NOTIFICATION_THRESHOLD,
   RECORDS_SINCE_NOTIFICATION,
   LAST_NOTIFY_RECORD,
   NOTIFICATION_CLASS,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROFILE_NAME
};

// structuredView Property List
static const BACnetPropertyIdentifier structuredView_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   NODE_TYPE,
   NODE_SUBTYPE,
   SUBORDINATE_LIST,
   SUBORDINATE_ANNOTATIONS,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Trend Log Multiple
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier trendLogMultiple_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   ENABLE,
   START_TIME,
   STOP_TIME,
   LOG_DEVICE_OBJECT_PROPERTY,
   LOGGING_TYPE,
   LOG_INTERVAL,
   ALIGN_INTERVALS,
   INTERVAL_OFFSET,
   TRIGGER,
   STOP_WHEN_FULL,
   BUFFER_SIZE,
   LOG_BUFFER,
   RECORD_COUNT,
   TOTAL_RECORD_COUNT,
   NOTIFICATION_THRESHOLD,
   RECORDS_SINCE_NOTIFICATION,
   LAST_NOTIFY_RECORD,
   NOTIFICATION_CLASS,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Access Door
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier accessDoor_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   DOOR_STATUS,
   LOCK_STATUS,
   SECURED_STATUS,
   DOOR_MEMBERS,
   DOOR_PULSE_TIME,
   DOOR_EXTENDED_PULSE_TIME,
   DOOR_UNLOCK_DELAY_TIME,
   DOOR_OPEN_TOO_LONG_TIME,
   DOOR_ALARM_STATE,
   MASKED_ALARM_VALUES,
   MAINTENANCE_REQUIRED,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   ALARM_VALUES,
   FAULT_VALUES,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Load Control
// Check whether the propertyID is one of the Object's standard properties
// Updated to 135-2012
static const BACnetPropertyIdentifier loadControl_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   STATE_DESCRIPTION,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   REQUESTED_SHED_LEVEL,
   START_TIME,
   SHED_DURATION,
   DUTY_WINDOW,
   ENABLE,
   FULL_DUTY_BASELINE,
   EXPECTED_SHED_LEVEL,
   ACTUAL_SHED_LEVEL,
   SHED_LEVELS,
   SHED_LEVEL_DESCRIPTIONS,
   NOTIFICATION_CLASS,
   TIME_DELAY,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// bitstringValue Property List
// Updated to 135-2012
static const BACnetPropertyIdentifier bitstringValue_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   BIT_TEXT,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   ALARM_VALUES,
   BIT_MASK,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// characterstringValue Property List
// Updated to 135-2012
static const BACnetPropertyIdentifier characterstringValue_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   ALARM_VALUES,
   FAULT_VALUES,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

//dateValue:   (all have the same Property List)
//    Updated to 135-2012
//datePatternValue:
//    Updated to 135-2012
//octetstringValue:
//    Updated to 135-2012
//timePatternValue:
//    Updated to 135-2012
//timeValue:
//    Updated to 135-2012
static const BACnetPropertyIdentifier dateValue_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

//datetimePatternValue: (both have the same Property List)
//    Updated to 135-2012
//datetimeValue:
//    Updated to 135-2012
static const BACnetPropertyIdentifier dateTimeValue_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   IS_UTC,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

//integerValue: (all have the same Property List)
//    Updated to 135-2012
//largeAnalogValue:
//    Updated to 135-2012
//positiveIntegerValue:
//    Updated to 135-2012
static const BACnetPropertyIdentifier integerValue_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   UNITS,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   COV_INCREMENT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   HIGH_LIMIT,
   LOW_LIMIT,
   DEADBAND,
   LIMIT_ENABLE,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   MIN_PRES_VALUE,
   MAX_PRES_VALUE,
   RESOLUTION,
   PROPERTY_LIST,
   PROFILE_NAME
};

// Global Group
//    Updated to 135-2012
static const BACnetPropertyIdentifier globalgroup_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   GROUP_MEMBERS,
   GROUP_MEMBER_NAMES,
   PRESENT_VALUE,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   UPDATE_INTERVAL,
   REQUESTED_UPDATE_INTERVAL,
   COV_RESUBSCRIPTION_INTERVAL,
   CLIENT_COV_INCREMENT,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   COVU_PERIOD,
   COVU_RECIPIENTS,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

//accessCredential
//    Updated to 135-2012
static const BACnetPropertyIdentifier accessCredential_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   GLOBAL_IDENTIFIER,
   STATUS_FLAGS,
   RELIABILITY,
   CREDENTIAL_STATUS,
   REASON_FOR_DISABLE,
   AUTHENTICATION_FACTORS,
   ACTIVATION_TIME,
   EXPIRY_TIME,
   CREDENTIAL_DISABLE,
   DAYS_REMAINING,
   USES_REMAINING,
   ABSENTEE_LIMIT,
   BELONGS_TO,
   ASSIGNED_ACCESS_RIGHTS,
   LAST_ACCESS_POINT,
   LAST_ACCESS_EVENT,
   LAST_USE_TIME,
   TRACE_FLAG,
   THREAT_AUTHORITY,
   EXTENDED_TIME_ENABLE,
   AUTHORIZATION_EXEMPTIONS,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

//accessPoint
//    Updated to 135-2012
static const BACnetPropertyIdentifier accessPoint_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   STATUS_FLAGS,
   EVENT_STATE,
   RELIABILITY,
   OUT_OF_SERVICE,
   AUTHENTICATION_STATUS,
   ACTIVE_AUTHENTICATION_POLICY,
   NUMBER_OF_AUTHENTICATION_POLICIES,
   AUTHENTICATION_POLICY_LIST,
   AUTHENTICATION_POLICY_NAMES,
   AUTHORIZATION_MODE,
   VERIFICATION_TIME,
   LOCKOUT,
   LOCKOUT_RELINQUISH_TIME,
   FAILED_ATTEMPTS,
   FAILED_ATTEMPT_EVENTS,
   MAX_FAILED_ATTEMPTS,
   FAILED_ATTEMPTS_TIME,
   THREAT_LEVEL,
   OCCUPANCY_UPPER_LIMIT_ENFORCED,
   OCCUPANCY_LOWER_LIMIT_ENFORCED,
   OCCUPANCY_COUNT_ADJUST,
   ACCOMPANIMENT_TIME,
   ACCESS_EVENT,
   ACCESS_EVENT_TAG,
   ACCESS_EVENT_TIME,
   ACCESS_EVENT_CREDENTIAL,
   ACCESS_EVENT_AUTHENTICATION_FACTOR,
   ACCESS_DOORS,
   PRIORITY_FOR_WRITING,
   MUSTER_POINT,
   ZONE_TO, ZONE_FROM,
   NOTIFICATION_CLASS,
   TRANSACTION_NOTIFICATION_CLASS,
   ACCESS_ALARM_EVENTS,
   ACCESS_TRANSACTION_EVENTS,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

//accessRights
//    Updated to 135-2012
static const BACnetPropertyIdentifier accessRights_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   GLOBAL_IDENTIFIER,
   STATUS_FLAGS,
   RELIABILITY,
   ENABLE,
   NEGATIVE_ACCESS_RULES,
   POSITIVE_ACCESS_RULES,
   ACCOMPANIMENT,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

//accessUser
//    Updated to 135-2012
static const BACnetPropertyIdentifier accessUser_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   GLOBAL_IDENTIFIER,
   STATUS_FLAGS,
   RELIABILITY,
   USER_TYPE,
   USER_NAME,
   USER_EXTERNAL_IDENTIFIER,
   USER_INFORMATION_REFERENCE,
   MEMBERS,
   MEMBER_OF,
   CREDENTIALS,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

//accessZone
//    Updated to 135-2012
static const BACnetPropertyIdentifier accessZone_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME, OBJECT_TYPE,
   DESCRIPTION,
   GLOBAL_IDENTIFIER,
   OCCUPANCY_STATE,
   STATUS_FLAGS,
   EVENT_STATE, RELIABILITY,
   OUT_OF_SERVICE,
   OCCUPANCY_COUNT,
   OCCUPANCY_COUNT_ENABLE,
   ADJUST_VALUE,
   OCCUPANCY_UPPER_LIMIT,
   OCCUPANCY_LOWER_LIMIT,
   CREDENTIALS_IN_ZONE,
   LAST_CREDENTIAL_ADDED,
   LAST_CREDENTIAL_ADDED_TIME,
   LAST_CREDENTIAL_REMOVED,
   LAST_CREDENTIAL_REMOVED_TIME,
   PASSBACK_MODE,
   PASSBACK_TIMEOUT,
   ENTRY_POINTS,
   EXIT_POINTS,
   TIME_DELAY,
   NOTIFICATION_CLASS,
   ALARM_VALUES,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_DETECTION_ENABLE,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   TIME_DELAY_NORMAL,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME,
};

//credentialDataInput
//    Updated to 135-2012
static const BACnetPropertyIdentifier credentialDataInput_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   DESCRIPTION,
   STATUS_FLAGS,
   RELIABILITY,
   OUT_OF_SERVICE,
   SUPPORTED_FORMATS,
   SUPPORTED_FORMAT_CLASSES,
   UPDATE_TIME,
   EVENT_DETECTION_ENABLE,
   NOTIFICATION_CLASS,
   EVENT_ENABLE,
   EVENT_STATE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME,
};

//networkSecurity
//    Updated to 135-2012
static const BACnetPropertyIdentifier networkSecurity_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   BASE_DEVICE_SECURITY_POLICY,
   NETWORK_ACCESS_SECURITY_POLICIES,
   SECURITY_TIME_WINDOW,
   PACKET_REORDER_TIME,
   DISTRIBUTION_KEY_REVISION,
   KEY_SETS,
   LAST_KEY_SERVER,
   SECURITY_PDU_TIMEOUT,
   UPDATE_KEY_SET_TIMEOUT,
   SUPPORTED_SECURITY_ALGORITHMS,
   DO_NOT_HIDE,
   PROPERTY_LIST,
   PROFILE_NAME
};

// notificationForwarder
//    Updated to 135-2012
static const BACnetPropertyIdentifier notificationForwarder_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   STATUS_FLAGS,
   RELIABILITY,
   OUT_OF_SERVICE,
   RECIPIENT_LIST,
   SUBSCRIBED_RECIPIENTS,
   PROCESS_IDENTIFIER_FILTER,
   PORT_FILTER,
   LOCAL_FORWARDING_ONLY,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// alertEnrollment
//    Updated to 135-2012
static const BACnetPropertyIdentifier alertEnrollment_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   EVENT_STATE,
   EVENT_DETECTION_ENABLE,
   NOTIFICATION_CLASS,
   EVENT_ENABLE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   EVENT_ALGORITHM_INHIBIT_REF,
   EVENT_ALGORITHM_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// channel
//    Updated to 135-2012
static const BACnetPropertyIdentifier channel_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   DESCRIPTION,
   PRESENT_VALUE,
   LAST_PRIORITY,
   WRITE_STATUS,
   STATUS_FLAGS,
   RELIABILITY,
   OUT_OF_SERVICE,
   LIST_OF_OBJECT_PROPERTY_REFERENCES,
   EXECUTION_DELAY,
   ALLOW_GROUP_DELAY_INHIBIT,
   CHANNEL_NUMBER,
   CONTROL_GROUPS,
   EVENT_DETECTION_ENABLE,
   NOTIFICATION_CLASS,
   EVENT_ENABLE,
   EVENT_STATE,
   ACKED_TRANSITIONS,
   NOTIFY_TYPE,
   EVENT_TIME_STAMPS,
   EVENT_MESSAGE_TEXTS,
   EVENT_MESSAGE_TEXTS_CONFIG,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};

// lightingOutput
//    Updated to 135-2012
static const BACnetPropertyIdentifier lightingOutput_PL[] =
{
   OBJECT_IDENTIFIER,
   OBJECT_NAME,
   OBJECT_TYPE,
   PRESENT_VALUE,
   TRACKING_VALUE,
   LIGHTING_COMMAND,
   IN_PROGRESS,
   DESCRIPTION,
   STATUS_FLAGS,
   RELIABILITY,
   OUT_OF_SERVICE,
   BLINK_WARN_ENABLE,
   EGRESS_TIME,
   EGRESS_ACTIVE,
   DEFAULT_FADE_TIME,
   DEFAULT_RAMP_RATE,
   DEFAULT_STEP_INCREMENT,
   TRANSITION,
   FEEDBACK_VALUE,
   PRIORITY_ARRAY,
   RELINQUISH_DEFAULT,
   POWER,
   INSTANTANEOUS_POWER,
   MIN_ACTUAL_VALUE,
   MAX_ACTUAL_VALUE,
   LIGHTING_COMMAND_DEFAULT_PRIORITY,
   COV_INCREMENT,
   RELIABILITY_EVALUATION_INHIBIT,
   PROPERTY_LIST,
   PROFILE_NAME
};


// Pretty up the code with this macro (also more fool proof)
// eg, expands to "PICS::structuredView_PL, sizeof( PICS::structuredView_PL )"
#define PROPLIST_ARGS(x) pRetval = x, nProps = (sizeof( x ) / sizeof(BACnetPropertyIdentifier))

// Return a pointer to the property table for the specified type, and the number of elements
// Return NULL and 0 for unknown object type
const BACnetPropertyIdentifier* GetObjectTable( BACnetObjectType objType, int &nProps)
{
   const BACnetPropertyIdentifier *pRetval = NULL;
   switch(objType)
   {
   case OBJ_ANALOG_INPUT:
      PROPLIST_ARGS( analogInput_PL ); break;
   case OBJ_ANALOG_OUTPUT:
      PROPLIST_ARGS( analogOutput_PL ); break;
   case OBJ_ANALOG_VALUE:
      PROPLIST_ARGS( analogValue_PL ); break;
   case OBJ_BINARY_INPUT:
      PROPLIST_ARGS( binaryInput_PL ); break;
   case OBJ_BINARY_OUTPUT:
      PROPLIST_ARGS( binaryOutput_PL ); break;
   case OBJ_BINARY_VALUE:
      PROPLIST_ARGS( binaryValue_PL ); break;
   case OBJ_CALENDAR:
      PROPLIST_ARGS( calendar_PL ); break;
   case OBJ_COMMAND:
      PROPLIST_ARGS( command_PL ); break;
   case OBJ_DEVICE:
      PROPLIST_ARGS( device_PL ); break;
   case OBJ_EVENT_ENROLLMENT:
      PROPLIST_ARGS( eventEnrollment_PL ); break;
   case OBJ_FILE:
      PROPLIST_ARGS( file_PL ); break;
   case OBJ_GROUP:
      PROPLIST_ARGS( group_PL ); break;
   case OBJ_LOOP:
      PROPLIST_ARGS( loop_PL ); break;
   case OBJ_MULTI_STATE_INPUT:
      PROPLIST_ARGS( multistateInput_PL ); break;
   case OBJ_MULTI_STATE_OUTPUT:
      PROPLIST_ARGS( multistateOutput_PL ); break;
   case OBJ_NOTIFICATIONCLASS:
      PROPLIST_ARGS( notificationClass_PL ); break;
   case OBJ_PROGRAM:
      PROPLIST_ARGS( program_PL ); break;
   case OBJ_SCHEDULE:
      PROPLIST_ARGS( schedule_PL ); break;
   case OBJ_AVERAGING:
      PROPLIST_ARGS( averaging_PL ); break;
   case OBJ_MULTI_STATE_VALUE:
      PROPLIST_ARGS( multistateValue_PL ); break;
   case OBJ_TREND_LOG:       // 20
      PROPLIST_ARGS( trend_PL ); break;
   case OBJ_LIFE_SAFETY_POINT:
      PROPLIST_ARGS( lifeSafetyPoint_PL ); break;
   case OBJ_LIFE_SAFETY_ZONE:
      PROPLIST_ARGS( lifeSafetyZone_PL ); break;
   case OBJ_ACCUMULATOR:
      PROPLIST_ARGS( accumulator_PL ); break;
   case OBJ_PULSE_CONVERTER:
      PROPLIST_ARGS( pulseConverter_PL ); break;
   case OBJ_EVENT_LOG:
      PROPLIST_ARGS( eventLog_PL ); break;
   case OBJ_GLOBAL_GROUP:
      PROPLIST_ARGS( globalgroup_PL ); break;
   case OBJ_TREND_LOG_MULTIPLE:
      PROPLIST_ARGS( trendLogMultiple_PL ); break;
   case OBJ_LOAD_CONTROL:
      PROPLIST_ARGS( loadControl_PL ); break;
   case OBJ_STRUCTURED_VIEW:
      PROPLIST_ARGS( structuredView_PL ); break;
   case OBJ_ACCESS_DOOR:     // 30
      PROPLIST_ARGS( accessDoor_PL ); break;
   case OBJ_ACCESS_CREDENTIAL:
      PROPLIST_ARGS( accessCredential_PL ); break;
   case OBJ_ACCESS_POINT:
      PROPLIST_ARGS( accessPoint_PL ); break;
   case OBJ_ACCESS_RIGHTS:
      PROPLIST_ARGS( accessRights_PL ); break;
   case OBJ_ACCESS_USER:
      PROPLIST_ARGS( accessUser_PL ); break;
   case OBJ_ACCESS_ZONE:
      PROPLIST_ARGS( accessZone_PL ); break;
   case OBJ_CREDENTIAL_DATA_INPUT:
      PROPLIST_ARGS( credentialDataInput_PL ); break;
   case OBJ_NETWORK_SECURITY:
      PROPLIST_ARGS( networkSecurity_PL ); break;
   case OBJ_BITSTRING_VALUE:
      PROPLIST_ARGS( bitstringValue_PL ); break;
   case OBJ_CHARACTERSTRING_VALUE:
      PROPLIST_ARGS( characterstringValue_PL ); break;

   case OBJ_DATE_PATTERN_VALUE:
   case OBJ_DATE_VALUE:            // All have the same Property List
   case OBJ_OCTETSTRING_VALUE:
   case OBJ_TIME_PATTERN_VALUE:
   case OBJ_TIME_VALUE:
      PROPLIST_ARGS( dateValue_PL ); break;

   case OBJ_DATETIME_PATTERN_VALUE: // Both have the same Property List
   case OBJ_DATETIME_VALUE:
      PROPLIST_ARGS( dateTimeValue_PL ); break;

   case OBJ_INTEGER_VALUE:         // All have the same Property List
   case OBJ_LARGE_ANALOG_VALUE:
   case OBJ_POSITIVE_INTEGER_VALUE:
      PROPLIST_ARGS( integerValue_PL ); break;

   case OBJ_NOTIFICATION_FORWARDER:
      PROPLIST_ARGS( notificationForwarder_PL ); break;
   case OBJ_ALERT_ENROLLMENT:
      PROPLIST_ARGS( alertEnrollment_PL ); break;
   case OBJ_CHANNEL:
      PROPLIST_ARGS( channel_PL ); break;
   case OBJ_LIGHTING_OUTPUT:
      PROPLIST_ARGS( lightingOutput_PL ); break;

   default:
      pRetval = NULL;
      nProps = 0;
      break;
   }

   return pRetval;
}

// Check whether the propertyID is one of the Object's standard properties
bool Check_Obj_Prop(int objType, unsigned int propertyID)
{
   int nElements;
   const BACnetPropertyIdentifier *pProps = GetObjectTable( (BACnetObjectType)objType, nElements );
   if (pProps)
   {
      for (int ix = 0; ix < nElements; ix++)
      {
         if (pProps[ix] == propertyID)
            return true;
      }
   }

   return false;
}
