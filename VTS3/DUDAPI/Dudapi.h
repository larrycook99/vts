// DUDAPI.H

#ifndef __DUDAPI_H_INCLUDED
#define __DUDAPI_H_INCLUDED


// Action for GetPropValue()
#define SETPROP			   0  // Set property value
#define ENCODEPROP		   1  // Encode ObjID, PropID, [ArrayIndex], PropVal, [Priority]
#define RND_ENCODEPROP     2  // Encode test value
#define RND_ASN_1_ANY_PROP 3  // Encode any test Property Value
#define ASN_1_ANY_PROP     4  // Encode any Property Value
#define GET_PARSE_TYPE     5  // Get the parse type of any property
#define SET_TRUE           6  // Encode an application tagged boolean TRUE
#define SET_FALSE          7  // Encode an application tagged boolean FALSE

//standard application services (BACnetServicesSupported)
enum BACnetApplServ 
{
   asNN= -1,
   asAcknowledgeAlarm,              //0
   asConfirmedCOVNotification,      //1
   asConfirmedEventNotification,    //2
   asGetAlarmSummary,               //3
   asGetEnrollmentSummary,          //4
   asSubscribeCOV,                  //5
   asAtomicReadFile,                //6
   asAtomicWriteFile,               //7
   asAddListElement,                //8 
   asRemoveListElement,             //9 
   asCreateObject,                  //10
   asDeleteObject,                  //11
   asReadProperty,                  //12
   asReadPropertyConditional,       //13
   asReadPropertyMultiple,          //14
   asWriteProperty,                 //15
   asWritePropertyMultiple,         //16
   asDeviceCommunicationControl,    //17 
   asConfirmedPrivateTransfer,      //18
   asConfirmedTextMessage,          //19
   asReinitializeDevice,            //20
   asVT_Open,                       //21
   asVT_Close,                      //22
   asVT_Data,                       //23
   asAuthenticate,                  //24
   asRequestKey,                    //25
   asI_Am,                          //26
   asI_Have,                        //27
   asUnconfirmedCOVNotification,    //28
   asUnconfirmedEventNotification,  //29
   asUnconfirmedPrivateTransfer,    //30
   asUnconfirmedTextMessage,        //31
   asTimeSynchronization,           //32
   asWho_Has,                       //33
   asWho_Is,                        //34
   asReadRange,                     //35
   asUTCTimeSynchronization,        //36
   asLifeSafetyOperation,           //37
   asSubscribeCOVProperty,          //38
   asGetEventInformation,           //39
   asWriteGroup,                    //40  // Last in 135-2012 (rev 14)
 };



// object type, property id
typedef struct tagTObjProp
{
   word object_type; // only standard objects
   char property_id; // only standard properties
} TObjProp;

// type for application service tables
typedef struct tagTApplServ
{
   enum                  BACnetApplServ ApplServ;       // number of application service
   char                  InitExec;                      // flag
   word                  object_type;                   // required object type
   char                  property_id;                   // required property
} TApplServ;



// message structure for function GetPropValue()
typedef struct tagPVMessage 
  { // in:
    HWND     			hw;         	// parent window
    long     			Action;     	// action 
    generic_object far* Obj;  			// pointer to object     
    dword    			PropId;			// property ID
    int      			ArrayIndex;  	// Property array index
    int      			Priority;    	// Priority (for WriteProperty Service)
    int 				TagNr;			// TagNr to encode (optional)
    int                 TestNr;	        // Number of TestValue	
    // in|out:
    int      			BufferLen;   	// length of encoded data
    // out:                     
    void far*			pv;				// pointer to property value
    int					pt;				// parse type
    int      			UsedBits;		// number of used bits (bitstring property only)
    int					Num;			// number of array elements
  } PVMessage;



#ifdef __cplusplus 
extern "C" {
#endif // __cplusplus

                            
// export functions declaration

void APIENTRY MyDeletePICSObject(generic_object far* root);

generic_object far* APIENTRY GetpObj(generic_object far* root, dword ObjectId);  
generic_object far* APIENTRY GetpDeviceObj(generic_object far* root);  

short APIENTRY GetPropNameSupported(char far* PropName, word i, word objtype,		//***002
                                    octet far propFlags[64], dword far* PropId, word * pParseType);

short APIENTRY GetPropValue(char far* Buffer, PVMessage far* msg);					//***002
short APIENTRY GetPropIndex(word object_type, dword PropId);

void APIENTRY SplitObjectId(dword ObjId, word far* ObjType, dword far* ObjInst);

#ifdef __cplusplus
}
#endif // cplusplus


#endif // __DUDAPI_H_INCLUDED

