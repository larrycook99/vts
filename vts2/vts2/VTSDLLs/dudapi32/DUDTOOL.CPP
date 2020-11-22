/*	23-Mar-98 [001]  JN  32-bit version (not all marked) 
						 eg. far pascal __export --> APIENTRY
*/

#include "stdafx.h"									//			***001 Begin
//#include <afx.h>			
//#include <afxwin.h>								//			***001 End

#include <windows.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "db.h"       // contains BACnet datatypes
#include "vtsapi.h" 
#include "bacprim.h"  // contains basic encoding functions

#include "resource.h"   // resource identifiers
#include "dudclass.h" // derived classes 
#include "dudapi.h"   // export interface
#include "dudtool.h"  // tool functions

#include "props.h"    
#include "propid.h"                  


#define nPRIO 16    // size of priority arrays
#define nTEST 3     // number of test values
#define nTESTLIST 4 // number of test values for list properties


// test values for list properties:
BACnetCalendarEntry far* gTestCalist[nTESTLIST];
BACnetReadAccessSpecification far* gTestRaslist[nTESTLIST];
BACnetVTClassList far* gTestVtcl[nTESTLIST];
BACnetObjectPropertyReference far* gTestLopref[nTESTLIST];  
BACnetSessionKey far* gTestSkeys[nTESTLIST];
BACnetAddressBinding far* gTestDabind[nTESTLIST];
BACnetRecipient far* gTestRecip[nTESTLIST];
BACnetDestination far* gTestReciplist[nTESTLIST];
BACnetVTSession far* gTestVtse[nTESTLIST];

octet gTestStavals[nTESTLIST][4]= { {1,2,3,0}, {4,0,0,0}, {0,0,0,0}, {5,0,0,0} };

int 		gTestEnum[nTEST]= {0, 1, 2};
float 		gTestReal[nTEST]= {47.1F, 0.5F, 92.2F};				//					***001

octet 		gTestBits[nTEST]= {0x55, 0x44, 0x33};
int		  	gTestUsedBits= 5;

int			gTestInt[nTEST]=  	{13, 14, 15};

char far*	gTestString[nTEST]= {"abcd", "efgh", "ijkl"};   

BOOL		gTestBool[nTEST]= {TRUE, FALSE, TRUE};

BACnetDateTime gTestDT[nTEST] = { 
									{{96,5,20,2},{9,28,0,0}}, 
									{{96,4,21,3},{9,13,0,0}}, 
									{{96,6,21,2},{8,29,0,0}} 
								};

//																										***001 Begin
float gTestPAF[nTEST][nPRIO]= { {1.1F,2.1F,3.1F,4.1F,5.1F,6.1F,7.1F,8.1F,9.1F,10.1F,11.1F,12.1F,13.1F,14.1F,15.1F,16.1F},
								{1.2F,2.2F,3.2F,4.2F,5.2F,6.2F,7.2F,8.2F,9.2F,10.2F,11.2F,12.2F,13.2F,14.2F,15.2F,16.2F},	
								{1.3F,2.3F,3.3F,4.3F,5.3F,6.3F,7.3F,8.3F,9.3F,10.3F,11.3F,12.3F,13.3F,14.3F,15.3F,16.3F}	
							  };	//																	***001 End

enum BACnetBinaryPV gTestPAB[nTEST][nPRIO]= 
        {
          { INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,
            INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE }, 
          { ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,
            ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE } ,
          { ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,ACTIVE,
            INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE,INACTIVE } 
        };    

word gTestPAU[nTEST][nPRIO]= { 
								{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
								{16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1},
								{16,15,14,13,12,11,10,9,1,2,3,4,5,6,7,8}
							 };

BACnetDateRange gTestDTRange[nTEST]= { 
										{ {96,5,20,0xFF}, {96,5,22,0xFF} },
										{ {95,4,19,0xFF}, {97,6,24,0xFF} },
										{ {95,6,21,0xFF}, {95,6,23,0xFF} }
									 };
									 
//																						***001 Begin
BACnetEventParameter gTestEvparm[nTEST]= {
    {OUT_OF_RANGE, {NULL,8,{0,0,0,0}}, NULL, NULL, 0, 0.0, 17, {NULL}, {NULL}, 1.5F, 1.6F, 1.7F, 1.8F, 1.9F },
    {OUT_OF_RANGE, {NULL,8,{0,0,0,0}}, NULL, NULL, 0, 0.0, 17, {NULL}, {NULL}, 1.1F, 1.2F, 1.3F, 1.4F, 1.5F },
    {OUT_OF_RANGE, {NULL,8,{0,0,0,0}}, NULL, NULL, 0, 0.0, 17, {NULL}, {NULL}, 1.3F, 4.2F, 7.3F, 11.4F, 51.5F }
  };								//													***001 End

BACnetObjectIdentifier gTestLObj[nTEST]= { {NULL,4711}, {NULL,4712}, {NULL,4713} };

BACnetTimeValue gTestTimeValue[nTEST][7]= {
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


BACnetTimeValue far* gTestWsched[nTEST][7]; 
static BACnetExceptionSchedule far* gTestXSched[nTEST];

#define nTESTUWNUM 3 								  
word gTestUWArr[nTEST][nTESTUWNUM]= { {1,2,3}, {3,2,1}, {2,3,1} };
								  
char far* gTestActext[nTEST][1]= { {"act1"}, {"act2"}, {"act3"} };

static BACnetActionCommand far* gTestAct[nTEST][1];


static char gBuffer[256];


// ===========================================================================================================
//
// functions 
//
// ===========================================================================================================


// This function is used to allocate dynamic memory and to write test property values
// to the allocated memory.
void CreateTestValues(void)
{ 
 
  // Test value for BACnetActionCommand: --- 

  BACnetActionCommand far* pAct;

  pAct= gTestAct[0][0]= new  BACnetActionCommand;								//			***001
  
  pAct->next= NULL;
  pAct->device_id= 0;
  pAct->object_id= 4711;
  pAct->property_id= 85;
  pAct->pa_index= NotAnArray;
  pAct->value_type= FLT;
  pAct->av.fproperty_value= 1.1F;												//			***001
  pAct->priority= 16;
  pAct->post_delay= 1;
  pAct->quit_on_failure= 0;
  pAct->write_successful= 0;
  
  pAct= gTestAct[1][0]= new  BACnetActionCommand;						//						***001

  pAct->next= NULL;
  pAct->device_id= 0;
  pAct->object_id= 4712;
  pAct->property_id= 85;
  pAct->pa_index= NotAnArray;
  pAct->value_type= FLT;
  pAct->av.fproperty_value= 1.2F;										//						***001
  pAct->priority= 16;
  pAct->post_delay= 2;
  pAct->quit_on_failure= 0;
  pAct->write_successful= 0;

  pAct= gTestAct[2][0]= new  BACnetActionCommand;						//						***001

  pAct->next= NULL;
  pAct->device_id= 0;
  pAct->object_id= 4713;
  pAct->property_id= 85;
  pAct->pa_index= NotAnArray;
  pAct->value_type= FLT;
  pAct->av.fproperty_value= 1.3F;										//						***001
  pAct->priority= 16;          
  pAct->post_delay= 3;
  pAct->quit_on_failure= 0;
  pAct->write_successful= 0;


  // Test Value for BACnetExceptionSchedule: ---

  BACnetExceptionSchedule far* px;
  BACnetSpecialEvent far* pe;
  BACnetTimeValue far* pt;

  px= gTestXSched[0]= new  BACnetExceptionSchedule;					//							***001
  px->size= 1;

  pe= px->special_event= new  BACnetSpecialEvent;					//							***001
  pe->next= NULL;
  pe->choice= 0;
  pe->u.date.year= 96;
  pe->u.date.month= 5;
  pe->u.date.day_of_month= 3;
  pe->u.date.day_of_week= 1;
  pe->event_priority= 16;

  pt= pe->list_of_time_values= new  BACnetTimeValue;				//							***001
  pt->next= NULL;
  pt->time.hour= 7;
  pt->time.minute= 30;
  pt->time.second= 42;
  pt->time.hundredths= 11;
  pt->value_type= UNS;
  pt->av.uproperty_value= 3;
  
  px= gTestXSched[1]= new  BACnetExceptionSchedule;					//							***001
  px->size= 1;

  pe= px->special_event= new  BACnetSpecialEvent;					//							***001
  pe->next= NULL;
  pe->choice= 0;
  pe->u.date.year= 96;
  pe->u.date.month= 4;
  pe->u.date.day_of_month= 2;
  pe->u.date.day_of_week= 2;
  pe->event_priority= 16;

  pt= pe->list_of_time_values= new  BACnetTimeValue;				//							***001
  pt->next= NULL;
  pt->time.hour= 8;
  pt->time.minute= 31;
  pt->time.second= 43;
  pt->time.hundredths= 12;
  pt->value_type= UNS;
  pt->av.uproperty_value= 2;
  

  px= gTestXSched[2]= new  BACnetExceptionSchedule;						//						***001
  px->size= 1;

  pe= px->special_event= new  BACnetSpecialEvent;						//						***001
  pe->next= NULL;
  pe->choice= 0;
  pe->u.date.year= 96;
  pe->u.date.month= 4;
  pe->u.date.day_of_month= 3;
  pe->u.date.day_of_week= 3;
  pe->event_priority= 16;

  pt= pe->list_of_time_values= new  BACnetTimeValue;					//						***001
  pt->next= NULL;
  pt->time.hour= 8;
  pt->time.minute= 33;
  pt->time.second= 44;
  pt->time.hundredths= 13;
  pt->value_type= UNS;
  pt->av.uproperty_value= 3;


  for (int i=0; i<7; i++)
    {
      gTestWsched[0][i]= (BACnetTimeValue far*)&gTestTimeValue[0][i];
      gTestWsched[1][i]= (BACnetTimeValue far*)&gTestTimeValue[1][i];
      gTestWsched[2][i]= (BACnetTimeValue far*)&gTestTimeValue[2][i];
    }
    
    
  // Test Values for ListOf Properties: ---------------------------------------------------------------------
 
  // Date_List (calendar) List of BACnetCalendarEntry
 
  BACnetCalendarEntry far* pCal= gTestCalist[0]= new  BACnetCalendarEntry; // 3 elements		***001

  pCal->choice= 				0;
  pCal->u.date.year= 			96;
  pCal->u.date.month= 			5;
  pCal->u.date.day_of_month= 	20;
  pCal->u.date.day_of_week= 	dontcare;
  pCal->next= 					new  BACnetCalendarEntry;					//					***001
  pCal= pCal->next;

  pCal->choice= 				0;
  pCal->u.date.year= 			96;
  pCal->u.date.month= 			4;
  pCal->u.date.day_of_month= 	19;
  pCal->u.date.day_of_week= 	dontcare;
  pCal->next= 					new  BACnetCalendarEntry;					//					***001
  pCal= pCal->next;

  pCal->choice= 				0;
  pCal->u.date.year= 			96;
  pCal->u.date.month= 			3;
  pCal->u.date.day_of_month= 	18;
  pCal->u.date.day_of_week= 	dontcare;
  pCal->next= NULL;
  
  gTestCalist[1]= gTestCalist[0]->next->next; // 1 element
  gTestCalist[2]= NULL; // empty list
  
  pCal= gTestCalist[3]= new  BACnetCalendarEntry;						//					***001
  pCal->choice= 				0;
  pCal->u.date.year= 			97;
  pCal->u.date.month= 			3;
  pCal->u.date.day_of_month= 	15;
  pCal->u.date.day_of_week= 	dontcare;
  pCal->next= NULL;
  
  // VT_Classes_Supported (device) List of BACnetVTClass

  BACnetVTClassList far* pVtcl= gTestVtcl[0]= new  BACnetVTClassList; // 3 elements				***001
  
  pVtcl->vtclass= DEFAULT_TERMINAL;
  pVtcl->next= new  BACnetVTClassList;										//					***001
  pVtcl= pVtcl->next;
  
  pVtcl->vtclass= ANSI_X34;
  pVtcl->next= new  BACnetVTClassList;										//					***001
  pVtcl= pVtcl->next;

  pVtcl->vtclass= DEC_VT52;
  pVtcl->next= NULL;

  gTestVtcl[1]= gTestVtcl[0]->next->next; // 1 element
  gTestVtcl[2]= NULL; // empty list

  pVtcl= gTestVtcl[3]= new  BACnetVTClassList;								//					***001
  pVtcl->vtclass= IBM_3130;
  pVtcl->next= NULL;
 

  // Active_VT_Sessions (device) List of BACnetVTSession
 
  BACnetVTSession far* pVtse= gTestVtse[0]= new  BACnetVTSession; // 3 elements					***001

  pVtse->local_session_id= 1;
  pVtse->remote_session_id= 1;
  pVtse->remote_address.network_number= 1;
  pVtse->remote_address.mac_address[0]= 'p';
  pVtse->remote_address.address_size= 1; 
  pVtse->next= new  BACnetVTSession;										//					***001
  pVtse= pVtse->next;
  
  pVtse->local_session_id= 2;
  pVtse->remote_session_id= 2;
  pVtse->remote_address.network_number= 2;
  pVtse->remote_address.mac_address[0]= 'o';
  pVtse->remote_address.address_size= 1; 
  pVtse->next= new  BACnetVTSession;										//					***001
  pVtse= pVtse->next;

  pVtse->local_session_id= 3;
  pVtse->remote_session_id= 3;
  pVtse->remote_address.network_number= 3;
  pVtse->remote_address.mac_address[0]= 'e';
  pVtse->remote_address.address_size= 1; 
  pVtse->next= NULL;

  gTestVtse[1]= gTestVtse[0]->next->next; // 1 element
  gTestVtse[2]= NULL; // empty list
 
  pVtse= gTestVtse[3]= new  BACnetVTSession; 								//					***001
  pVtse->local_session_id= 4;
  pVtse->remote_session_id= 4;
  pVtse->remote_address.network_number= 4;
  pVtse->remote_address.mac_address[0]= 'z';
  pVtse->remote_address.address_size= 1; 
  pVtse->next= NULL;
  
  // List_Of_Session_Keys (device) List of BACnetSessionKey
 
  BACnetSessionKey far* pSkeys= gTestSkeys[0]= new  BACnetSessionKey; // 3 elements				***001
  
  pSkeys->session_key[0]= 1; pSkeys->session_key[1]= 2; pSkeys->session_key[2]= 3; pSkeys->session_key[3]= 4;
  pSkeys->session_key[4]= 5; pSkeys->session_key[5]= 6; pSkeys->session_key[6]= 7; pSkeys->session_key[7]= 8;
  pSkeys->peer_address.network_number= 1;
  pSkeys->peer_address.mac_address[0]= 'p';
  pSkeys->peer_address.address_size= 1; 
  pSkeys->next= new  BACnetSessionKey;										//					***001
  pSkeys= pSkeys->next;
  
  pSkeys->session_key[0]= 8; pSkeys->session_key[1]= 6; pSkeys->session_key[2]= 4; pSkeys->session_key[3]= 2;
  pSkeys->session_key[4]= 7; pSkeys->session_key[5]= 5; pSkeys->session_key[6]= 3; pSkeys->session_key[7]= 1;
  pSkeys->peer_address.network_number= 2;
  pSkeys->peer_address.mac_address[0]= 'o';
  pSkeys->peer_address.address_size= 1; 
  pSkeys->next= new  BACnetSessionKey;										//					***001
  pSkeys= pSkeys->next;

  pSkeys->session_key[0]= 1; pSkeys->session_key[1]= 3; pSkeys->session_key[2]= 5; pSkeys->session_key[3]= 7;
  pSkeys->session_key[4]= 2; pSkeys->session_key[5]= 4; pSkeys->session_key[6]= 6; pSkeys->session_key[7]= 8;
  pSkeys->peer_address.network_number= 3;
  pSkeys->peer_address.mac_address[0]= 'e';
  pSkeys->peer_address.address_size= 1; 
  pSkeys->next= NULL;

  gTestSkeys[1]= gTestSkeys[0]->next->next; // 1 element
  gTestSkeys[2]= NULL; // empty list   
  
  pSkeys= gTestSkeys[3]= new  BACnetSessionKey; 							//					***001
  pSkeys->session_key[0]= 1; pSkeys->session_key[1]= 3; pSkeys->session_key[2]= 5; pSkeys->session_key[3]= 7;
  pSkeys->session_key[4]= 2; pSkeys->session_key[5]= 4; pSkeys->session_key[6]= 6; pSkeys->session_key[7]= 8;
  pSkeys->peer_address.network_number= 4;
  pSkeys->peer_address.mac_address[0]= 'z';
  pSkeys->peer_address.address_size= 1; 
  pSkeys->next= NULL;
   
  // Time_Synchronization_Recipients (device)  List of BACnetRecipient
 
  BACnetRecipient far* pRecip= gTestRecip[0]= new  BACnetRecipient; // 3 elements				***001
  
  pRecip->choice= 0;
  pRecip->u.device= 4711;
  pRecip->next= new  BACnetRecipient;										//					***001
  pRecip= pRecip->next;
  
  pRecip->choice= 0;
  pRecip->u.device= 4712;
  pRecip->next= new  BACnetRecipient;										//					***001
  pRecip= pRecip->next;

  pRecip->choice= 0;
  pRecip->u.device= 4713;
  pRecip->next= NULL;

  gTestRecip[1]= gTestRecip[0]->next->next; // 1 element
  gTestRecip[2]= NULL; // empty list   
 
  pRecip= gTestRecip[3]= new  BACnetRecipient; 								//					***001
  pRecip->choice= 0;
  pRecip->u.device= 4714;
  pRecip->next= NULL;
 
  // Device_Address_Binding (device) List of BACnetAddressBinding 
  
  BACnetAddressBinding far* pDabind= gTestDabind[0]= new  BACnetAddressBinding; // 3 elements	***001

  pDabind->device_object_id= 4711;
  pDabind->device_address.network_number= 1;
  pDabind->device_address.mac_address[0]= 'P';
  pDabind->device_address.address_size= 1;
  pDabind->next= new  BACnetAddressBinding;									//					***001
  pDabind= pDabind->next;
  
  pDabind->device_object_id= 4712;
  pDabind->device_address.network_number= 2;
  pDabind->device_address.mac_address[0]= 'O';
  pDabind->device_address.address_size= 1;
  pDabind->next= new  BACnetAddressBinding;									//					***001
  pDabind= pDabind->next;
  
  pDabind->device_object_id= 4713;
  pDabind->device_address.network_number= 3;
  pDabind->device_address.mac_address[0]= 'E';
  pDabind->device_address.address_size= 1;
  pDabind->next= NULL;

  gTestDabind[1]= gTestDabind[0]->next->next; // 1 element
  gTestDabind[2]= NULL; // empty list   

  pDabind= gTestDabind[3]= new  BACnetAddressBinding; 						//					***001
  pDabind->device_object_id= 4714;
  pDabind->device_address.network_number= 4;
  pDabind->device_address.mac_address[0]= 'z';
  pDabind->device_address.address_size= 1;
  pDabind->next= NULL;

  // List_Of_Group_Members (group) List of ReadAccessSpecification
  
  BACnetReadAccessSpecification far* pRaslist= gTestRaslist[0]= new  BACnetReadAccessSpecification; // 3 elements ***001
  
  pRaslist->object_id= 4711;
  pRaslist->list_of_prop_ref= NULL;
  pRaslist->next= new  BACnetReadAccessSpecification;							//					***001
  pRaslist= pRaslist->next;
  
  pRaslist->object_id= 4712;
  pRaslist->list_of_prop_ref= NULL;
  pRaslist->next= new  BACnetReadAccessSpecification;							//					***001
  pRaslist= pRaslist->next;

  pRaslist->object_id= 4713;
  pRaslist->list_of_prop_ref= NULL;
  pRaslist->next= NULL;
 
  gTestRaslist[1]= gTestRaslist[0]->next->next; // 1 element
  gTestRaslist[2]= NULL; // empty list   
  
  pRaslist= gTestRaslist[3]= new  BACnetReadAccessSpecification;				//					***001
  pRaslist->object_id= 4714;
  pRaslist->list_of_prop_ref= NULL;
  pRaslist->next= NULL;

  // Present_Value (group) List of ReadAccessResult


  // Recipient_List (notification class) List of BACnetDestination
  
  BACnetDestination far* pReciplist= gTestReciplist[0]= new  BACnetDestination; // 3 elements***001
  
  pReciplist->valid_days= 0x44;
  pReciplist->from_time.hour= 16;
  pReciplist->from_time.minute= 17;
  pReciplist->from_time.second= dontcare;
  pReciplist->from_time.hundredths= dontcare;
  pReciplist->to_time.hour= 16;
  pReciplist->to_time.minute= 18;
  pReciplist->to_time.second= dontcare;
  pReciplist->to_time.hundredths= dontcare;
  pReciplist->recipient.next= NULL;
  pReciplist->recipient.choice= 0;
  pReciplist->recipient.u.device= 4711;
  pReciplist->process_id= 14;
  pReciplist->notification= 0;
  pReciplist->transitions= 0x55;
  pReciplist->next= new  BACnetDestination;					//					***001
  pReciplist= pReciplist->next;
  
  pReciplist->valid_days= 0x45;
  pReciplist->from_time.hour= 17;
  pReciplist->from_time.minute= 18;
  pReciplist->from_time.second= dontcare;
  pReciplist->from_time.hundredths= dontcare;
  pReciplist->to_time.hour= 17;
  pReciplist->to_time.minute= 19;
  pReciplist->to_time.second= dontcare;
  pReciplist->to_time.hundredths= dontcare;
  pReciplist->recipient.next= NULL;
  pReciplist->recipient.choice= 0;
  pReciplist->recipient.u.device= 4712;
  pReciplist->process_id= 15;
  pReciplist->notification= 0;
  pReciplist->transitions= 0x55;
  pReciplist->next= new  BACnetDestination;								//					***001
  pReciplist= pReciplist->next;

  pReciplist->valid_days= 0x46;
  pReciplist->from_time.hour= 17;
  pReciplist->from_time.minute= 18;
  pReciplist->from_time.second= dontcare;
  pReciplist->from_time.hundredths= dontcare;
  pReciplist->to_time.hour= 19;
  pReciplist->to_time.minute= 18;
  pReciplist->to_time.second= dontcare;
  pReciplist->to_time.hundredths= dontcare;
  pReciplist->recipient.next= NULL;
  pReciplist->recipient.choice= 0;
  pReciplist->recipient.u.device= 4713;
  pReciplist->process_id= 16;
  pReciplist->notification= 0;
  pReciplist->transitions= 0x44;
  pReciplist->next= NULL;

  gTestReciplist[1]= gTestReciplist[0]->next->next; // 1 element
  gTestReciplist[2]= NULL; // empty list   
  
  pReciplist= gTestReciplist[3]= new  BACnetDestination; 					//					***001
  pReciplist->valid_days= 0x47;
  pReciplist->from_time.hour= 18;
  pReciplist->from_time.minute= 16;
  pReciplist->from_time.second= dontcare;
  pReciplist->from_time.hundredths= dontcare;
  pReciplist->to_time.hour= 19;
  pReciplist->to_time.minute= 8;
  pReciplist->to_time.second= dontcare;
  pReciplist->to_time.hundredths= dontcare;
  pReciplist->recipient.next= NULL;
  pReciplist->recipient.choice= 0;
  pReciplist->recipient.u.device= 4714;
  pReciplist->process_id= 17;
  pReciplist->notification= 0;
  pReciplist->transitions= 0x44;
  pReciplist->next= NULL;


  // List_Of_Object_Property_References(schedule) List of BACnetObjectPropertyReference
   
  BACnetObjectPropertyReference far* pLopref= gTestLopref[0]= new  BACnetObjectPropertyReference; // 3 elements***001
  
  pLopref->object_id= 4711;
  pLopref->property_id= 75;
  pLopref->pa_index= NotAnArray;
  pLopref->next= new  BACnetObjectPropertyReference;							//					***001
  pLopref= pLopref->next;
  
  pLopref->object_id= 4712;
  pLopref->property_id= 75;
  pLopref->pa_index= NotAnArray;
  pLopref->next= new  BACnetObjectPropertyReference;							//					***001
  pLopref= pLopref->next;

  pLopref->object_id= 4713;
  pLopref->property_id= 75;
  pLopref->pa_index= NotAnArray;
  pLopref->next= NULL;

  gTestLopref[1]= gTestLopref[0]->next->next; // 1 element
  gTestLopref[2]= NULL; // empty list    

  pLopref= gTestLopref[3]= new  BACnetObjectPropertyReference;							//					***001
  pLopref->object_id= 4714;
  pLopref->property_id= 75;
  pLopref->pa_index= NotAnArray;
  pLopref->next= NULL;

}


// This function is used to release the dynamic allocated memory of the
// test property values.
void DeleteTestValues(void)
{ 
  BACnetExceptionSchedule far* px;
  BACnetSpecialEvent far* pe;
  BACnetTimeValue far* pt;
      
  for (int i= 0; i<nTEST; i++)
    {
      delete gTestAct[i][0];
      
      px= gTestXSched[i];
      pe= px->special_event;
      pt= pe->list_of_time_values;
      
      delete pt;
      delete pe;
      delete px;
    }  
      
  // delete ListOf-Property lists

  BACnetCalendarEntry far* pCal= gTestCalist[0];
  BACnetCalendarEntry far* pCal2;
  while (pCal!=NULL) { pCal2= pCal->next; delete pCal; pCal= pCal2; }
  delete gTestCalist[3];

  BACnetVTClassList far* pVtcl= gTestVtcl[0];
  BACnetVTClassList far* pVtcl2;
  while (pVtcl!=NULL) { pVtcl2= pVtcl->next; delete pVtcl; pVtcl= pVtcl2; }
  delete gTestVtcl[3];

  BACnetVTSession far* pVtse= gTestVtse[0];
  BACnetVTSession far* pVtse2;
  while (pVtse!=NULL) { pVtse2= pVtse->next; delete pVtse; pVtse= pVtse2; }
  delete gTestVtse[3];

  BACnetRecipient far* pRecip= gTestRecip[0];
  BACnetRecipient far* pRecip2;
  while (pRecip!=NULL) { pRecip2= pRecip->next; delete pRecip; pRecip= pRecip2; }
  delete gTestRecip[3];

  BACnetSessionKey far* pSkeys= gTestSkeys[0];
  BACnetSessionKey far* pSkeys2;
  while (pSkeys!=NULL) { pSkeys2= pSkeys->next; delete pSkeys; pSkeys= pSkeys2; }
  delete gTestSkeys[3];

  BACnetAddressBinding far* pDabind= gTestDabind[0];
  BACnetAddressBinding far* pDabind2;
  while (pDabind!=NULL) { pDabind2= pDabind->next; delete pDabind; pDabind= pDabind2; }
  delete gTestDabind[3];

  BACnetReadAccessSpecification far* pRaslist= gTestRaslist[0];
  BACnetReadAccessSpecification far* pRaslist2;
  while (pRaslist!=NULL) { pRaslist2= pRaslist->next; delete pRaslist; pRaslist= pRaslist2; }
  delete gTestRaslist[3];

  BACnetDestination far* pReciplist= gTestReciplist[0];
  BACnetDestination far* pReciplist2;
  while (pReciplist!=NULL) { pReciplist2= pReciplist->next; delete pReciplist; pReciplist= pReciplist2; }
  delete gTestReciplist[3];

  BACnetObjectPropertyReference far* pLopref= gTestLopref[0];
  BACnetObjectPropertyReference far* pLopref2;
  while (pLopref!=NULL) { pLopref2= pLopref->next; delete pLopref; pLopref= pLopref2; }
  delete gTestLopref[3];

}


// encode BACnetDate
octet far* eBACnetDate(octet far* p, const BACnetDate far* pv, int TagNr)
{
  if (pv==NULL) return(p);
  p= eDATESTRING(p,pv->year,pv->month,pv->day_of_month,pv->day_of_week);
  return p;
}

// encode BACnetTime
octet far* eBACnetTime(octet far* p, const BACnetTime far* pv, int TagNr)
{
  if (pv==NULL) return(p);
  p= eTIMESTRING(p,pv->hour,pv->minute,pv->second,pv->hundredths);
  return p;
}

             
// encode BACnetCalendarEntry
octet far* eCALIST(octet far* p, const BACnetCalendarEntry far* pv, int TagNr)
 {
  while (pv!=NULL)
    { 
      switch (pv->choice)
        { case 0: // BACnetDate
            *p++= 0x0E; // opening tag 
              *p++= 0xA4;
              memcpy(p,&pv->u,4); p+= 4;							//					***001
            *p++= 0x0F;  // closing tag 
            break;
          case 1: // BACnetDateRange
            *p++= 0x1E; // opening tag
            p= eDTRANGE(p,(BACnetDateRange far*) &pv->u);
            *p++= 0x1F; // closing tag
            break;
          case 2: // BACnetWeekNDay
            *p++= 0x2E; // opening tag
            p= eOCTETSTRING(p,(octet far*)&pv->u.weekNday,3);
            *p++= 0x2F; // closing tag
            break;
        }    
      pv= pv->next;
    }
  return p;  
}

// encode list of BACnetPropertyReference             
octet far* ePROPREF(octet far* p, const BACnetPropertyReference far* pv, int TagNr)
{ while (pv!=NULL)
    { p= ecUINT(p,0x00,pv->property_id);
      if (pv->pa_index!=NotAnArray) // optional: array index
        { p= ecUINT(p,0x18,pv->pa_index); }
      pv= pv->next;
    }      
  return p;
}

// encode list of BACnetObjectPropertyReference             
octet far* eLOPREF(octet far* p, const BACnetObjectPropertyReference far* pv, int TagNr)
{ while (pv!=NULL)
    { p= etagOBJECTID(p,0x08,pv->object_id);
      p= ecUINT(p,0x18,pv->property_id);
      if (pv->pa_index!=NotAnArray) // optional: array index
        { p= ecUINT(p,0x28,pv->pa_index); }
      pv= pv->next;
    }      
  return p;
}

// encode BACnetReadAccessSpecification             
octet far* eRASLIST(octet far* p, const BACnetReadAccessSpecification far* pv, int TagNr)
{ 
  while (pv!=NULL)
    { // encode object identifier      
      p= etagOBJECTID(p,0x08,pv->object_id);
      // encode property reference list
      *(p++)= 0x1E; // opening tag
      p= ePROPREF(p,pv->list_of_prop_ref);
      *(p++) = 0x1F; // closing tag
      pv= pv->next;
    }
  return p;  
}

// encode BACnetDateTime             
octet far* eDT(octet far* p, const BACnetDateTime far* pv, int TagNr)
{ 
  if (pv==NULL) return p;
  *p++= 0xA4; // application tag
  memcpy(p,&pv->date,4); p+=4;									//					***001
  *p++= 0xB4; // application tag
  memcpy(p,&pv->time,4); p+=4; 									//					***001
  return p;
}

// encode array of object identifier
octet far* eLOBJ(octet far* p, const BACnetObjectIdentifier far* pv, int ArrayIndex, int TagNr)
{ int i;
        
  switch (ArrayIndex)
    {
      case -1: // all elements 
        while (pv!=NULL)
          { // encode object identifier
            p= etagOBJECTID(p,0xC0,pv->object_id);
            pv= pv->next;
          }   
        break;  
        
      case 0: // number of elements
        i= 0;
        while (pv!=NULL)
          { i++;
            pv= pv->next;
          } 
        p= eUINT(p,i);    
        break;  
        
      default: // ArrayIndex element   
        i= ArrayIndex-1;
        while ((pv!=NULL) && (i>0))
          { i--; pv= pv->next; }
        if ((pv==NULL) || (ArrayIndex<0)) return p; // invalid array index  
        p= etagOBJECTID(p,0xC0,pv->object_id);
        break;
    }    
  return p;  
}

// encode float priority array
octet far* ePAF(octet far* p, const float far* pv, int ArrayIndex, int TagNr)
{ int i;
  if (pv==NULL) return p;

  switch(ArrayIndex)
    { 
      case -1: // all elements
        for (i=0; i<nPRIO; i++)
          {   
            if (pv[i]==fpaNULL) 
              p= eNULL(p);
            else 
              p= eREAL(p,pv[i]);
          }
        break;
        
      case 0:  // number of elements
        p= eUINT(p,nPRIO);
        break;
        
      default: // ArrayIndex element     
        if ((ArrayIndex>0)&&(ArrayIndex<=nPRIO))
          {
            if (pv[ArrayIndex-1]==fpaNULL) 
              p= eNULL(p);
            else 
              p= eREAL(p,pv[ArrayIndex-1]);
          }
        break;
    }     
  return p;
}

// encode BACnetBinaryPV
octet far* ePAB(octet far* p, const enum BACnetBinaryPV far* pv, int ArrayIndex, int TagNr)
{ int i;
  if (pv==NULL) return p;
  
  switch(ArrayIndex)                                                           
    { 
      case -1: // all elements
        for (i=0; i<nPRIO; i++)
          { 
            if (pv[i]!=bpaNULL) p= eENUM(p,pv[i]);
            else p= eNULL(p);
          }
        break;  

      case 0:  // number of elements
        p= eUINT(p,nPRIO);
        break;
        
      default: // ArrayIndex element     
        if ((ArrayIndex>0)&&(ArrayIndex<=nPRIO))
          p= eENUM(p,pv[ArrayIndex-1]);
        break;
    }
  return p;
}

// encode uw priority array
octet far* ePAU(octet far* p, const word far* pv, int ArrayIndex, int TagNr)
{ int i;
  if (pv==NULL) return p;

  switch(ArrayIndex)
    { 
      case -1: // all elements
        for (i=0; i<nPRIO; i++)
          { 
            if (pv[i]!=upaNULL) p= eUINT(p,pv[i]);
            else p= eNULL(p);
          }
        break;  
          
      case 0:  // number of elements
        p= eUINT(p,nPRIO);
        break;
        
      default: // ArrayIndex element     
        if ((ArrayIndex>0)&&(ArrayIndex<=nPRIO))
          p= eUINT(p,pv[ArrayIndex-1]);
        break;
    }
  return p;
}

// encode uw array  
octet far* eUWARR(octet far* p, const word far* pv, int num, int ArrayIndex, int TagNr)
{ int i;
  if (pv==NULL) return p;
  switch(ArrayIndex)
    { 
      case -1: // all elements
        for (i=0; i<num; i++) p= eUINT(p,pv[i]);
        break;  

      case 0:  // number of elements
        p= eUINT(p,num);
        break;
        
      default: // ArrayIndex element     
        if ((ArrayIndex>0)&&(ArrayIndex<=num))
          p= eUINT(p,pv[ArrayIndex-1]);
        break;
    }    
  return p;
}


// encode action text array
octet far* eACTEXT(octet far* p,char far** pv, int num, int ArrayIndex, int TagNr)
{ int i; word n;
  if (pv==NULL) return p;

  switch(ArrayIndex)
    { 
      case -1: // all elements
        for (i= 0; i<num; i++)
          { if (pv[i]==NULL) break;
            n= strlen(pv[i]);									//					***001
            p= eCHARSTRING(p,pv[i],0x70,n,ANSI,0);
          }  
        break;    

      case 0:  // number of elements
        p= eUINT(p,num);
        break;
        
      default: // ArrayIndex element     
        if ((ArrayIndex>0)&&(ArrayIndex<=num))
          p= eCHARSTRING(p,pv[ArrayIndex-1],0x70,strlen(pv[ArrayIndex-1]),ANSI,0);		//					***001
        break;
    }    
  return p;  
}


// encode array of BACnetActionCommand
octet far* eACT(octet far* p, BACnetActionCommand far** pv, int num, int ArrayIndex, int TagNr)
{ BACnetActionCommand far* pact;
  int i;
  octet far* s;
  if (pv==NULL) return p;
  
  switch (ArrayIndex)
    { 
      case -1: // all elements -------------------------------------------------------------
        for (i=0; i<num; i++)
          {
            pact= pv[i];
            *p++= 0x0E; // open tag
            while (pact!=NULL)
              {
                if (pact->device_id!=0) // optional
                  { p= etagOBJECTID(p,0x00,pact->device_id); }
                p= etagOBJECTID(p,0x10,pact->object_id);  
                p= ecUINT(p,0x20,pact->property_id);
                if (pact->pa_index!=NotAnArray) // optional: array index
                  { p= ecUINT(p,0x30,pact->pa_index); }
                s= p;  
                switch(pact->value_type)
                  { case BPV:
                      p= eENUM(p,pact->av.bproperty_value);
                      break;
                    case UNS:
                      p= eUINT(p,pact->av.uproperty_value);
                      break;
                    case FLT:
                      p= eREAL(p,pact->av.fproperty_value);
                      break;
                  }
                *s= 0x48 | (*s & 0x07);  
                if ( (pact->priority>=1) && (pact->priority<=16) ) // optional: priority
                  { p= ecUINT(p,0x50,pact->priority); }
                if (pact->post_delay!=0) // optional: post_delay
                  { s= p;
                    p= eUINT(p,pact->post_delay); 
                    *s= 0x68 | (*s & 0x07); // context tag [6]
                  }
                p= ecBOOL(p,0x70,pact->quit_on_failure);  
                p= ecBOOL(p,0x80,pact->write_successful);
                pact= pact->next;
              }
            *p++= 0x0F; // close tag  
          } // end for
        break;
        
      case 0: // number of elements -------------------------------------------------------------
        p= eUINT(p,num);
        break;
          
      default: // ArrayIndex element ------------------------------------------------------------
        if ((ArrayIndex<=0)||(ArrayIndex>num)) return p; // invalid ArrayIndex
        pact= pv[ArrayIndex];
        *p++= 0x0E; // open tag
        while (pact!=NULL)
          {
            if (pact->device_id!=0) // optional
              { p= etagOBJECTID(p,0x00,pact->device_id); }
            p= etagOBJECTID(p,0x10,pact->object_id);  
            p= ecUINT(p,0x20,pact->property_id);
            if (pact->pa_index!=NotAnArray) // optional: array index
              { p= ecUINT(p,0x30,pact->pa_index); }
            s= p;  
            switch(pact->value_type)
              { case BPV:
                  p= eENUM(p,pact->av.bproperty_value);
                  break;
                case UNS:
                  p= eUINT(p,pact->av.uproperty_value);
                  break;
                case FLT:
                  p= eREAL(p,pact->av.fproperty_value);
                  break;
              }
            *s= 0x48 | (*s & 0x07);  
            if ( (pact->priority>=1) && (pact->priority<=16) ) // optional: priority
              { p= ecUINT(p,0x50,pact->priority); }
            if (pact->post_delay!=0) // optional: post_delay
              { s= p;
                p= eUINT(p,pact->post_delay); 
                *s= 0x68 | (*s & 0x07); // context tag [6]
              }
            p= ecBOOL(p,0x70,pact->quit_on_failure);  
            p= ecBOOL(p,0x80,pact->write_successful);
            pact= pact->next;
          }
        *p++= 0x0F; // close tag  
        break;
          
    } // end switch
            
  return p;
}               


// encode array of BACnetTimeValue
octet far* eWSCHED(octet far* p, BACnetTimeValue far** pv, int num, int ArrayIndex, int TagNr)
{ int i;
  if (pv==NULL) return p;
  switch(ArrayIndex)
    { 
      case -1: // all elements
        for (i=0; i<num; i++)
          { *p++= 0x0E; // day-schedule [0]
            p= eTIMEVALUE(p,pv[i]);
            *p++= 0x0F;  
          }    
        break;  

      case 0:  // number of elements
        p= eUINT(p,num);
        break;
        
      default: // ArrayIndex element     
        if ((ArrayIndex>0)&&(ArrayIndex<=num))
          { *p++= 0x0E; // day-schedule [0]
            p= eTIMEVALUE(p,pv[ArrayIndex-1]);
            *p++= 0x0F;  
          }  
        break;
    }      
  return p;
}

// encode array BACnetExceptionSchedule
octet far* eXSCHED(octet far* p, BACnetExceptionSchedule far* pv, int ArrayIndex, int TagNr)
{ int i;
  BACnetSpecialEvent far* pse;
  if (pv==NULL) return p;
  
  switch (ArrayIndex)
    { 
    
      case -1: // all elements -------------------------------------------------------------------
        pse= pv->special_event;
        while (pse!=NULL)
          { // encode period
            switch (pse->choice)
              { case 0: // [0] CalendarEntry [0]
                  *p++= 0x0E;
                    *p++= 0x0E;
                      *p++= 0xA4; 
                      memcpy(p,&pse->u.date,4); p+=4; 					//					***001
                    *p++= 0x0F;
                  *p++= 0x0F;
                  break;
                case 1: // [0] CalendarEntry [1]
                  *p++= 0x0E;
                    *p++= 0x1E;
                      p= eDTRANGE(p,&pse->u.date_range);
                    *p++= 0x1F;
                  *p++= 0x0F;
                  break;
                case 2: // [0] CalendarEntry [2]
                  *p++= 0x0E;
                    *p++= 0x2E;
                      p= eOCTETSTRING(p,(octet far*)&pse->u.weekNday,3);
                    *p++= 0x2F;
                  *p++= 0x0F;
                  break;
                case 3: // [1] CalendarReference 
                  *p++= 0x1E;
                    p= etagOBJECTID(p,0xC0,pse->u.calendar_ref.object_id);
                  *p++= 0x1F;
                  break;
              }
            *p++= 0x2E;  
              p= eTIMEVALUE(p,pse->list_of_time_values); // encode timevalue [2]
            *p++= 0x2F;
            p= ecUINT(p,0x30, pse->event_priority);      // encode eventPriority [3]  
            pse= pse->next;
          }
        break;
        
      case 0: // number of elements --------------------------------------------------------------  
        pse= pv->special_event;
        i= 0;
        while (pse!=NULL)
          { i++;
            pse= pse->next;
          }
        p= eUINT(p,i);
        break;
        
      default: // ArrayIndex element -------------------------------------------------------------
        pse= pv->special_event;
        i= ArrayIndex-1;
        while ((pse!=NULL) && (i>0))
          { i--; pse= pse->next; }
        if ((pse==NULL) || (ArrayIndex<0)) return p; // invalid array index  
        switch (pse->choice)
          { case 0: // [0] CalendarEntry [0]
              *p++= 0x0E;
                 *p++= 0x0E;
                   *p++= 0xA4; 
                   memcpy(p,&pse->u.date,4); p+=4; 					//					***001
                 *p++= 0x0F;
              *p++= 0x0F;
              break;
            case 1: // [0] CalendarEntry [1]
              *p++= 0x0E;
                *p++= 0x1E;
                  p= eDTRANGE(p,&pse->u.date_range);
                *p++= 0x1F;
              *p++= 0x0F;
              break;
            case 2: // [0] CalendarEntry [2]
              *p++= 0x0E;
                *p++= 0x2E;
                  p= eOCTETSTRING(p,(octet far*)&pse->u.weekNday,3);
                *p++= 0x2F;
              *p++= 0x0F;
              break;
            case 3: // [1] CalendarReference 
              *p++= 0x1E;
                p= etagOBJECTID(p,0xC0,pse->u.calendar_ref.object_id);
              *p++= 0x1F;
              break;
          }
        *p++= 0x2E;  
          p= eTIMEVALUE(p,pse->list_of_time_values); // encode timevalue [2]
        *p++= 0x2F;
        p= ecUINT(p,0x30, pse->event_priority);      // encode eventPriority [3]  
        pse= pse->next;
        break;  
    }  
  return p;
}     


// encode BACnetDateRange
octet far* eDTRANGE(octet far* p, const BACnetDateRange far* pv, int TagNr)
{ 
  if (pv==NULL) return p;
  *p++= 0xA4; // application tag
  memcpy(p,&pv->start_date,4); p+= 4;					//					***001
  *p++= 0xA4; // application tag
  memcpy(p,&pv->end_date,4); p+= 4;						//					***001
  return p;
}


// encode a context tagged boolean value
octet far* ecBOOL(octet far* p, octet tag, boolean bval)
{
  *p++= (tag & 0xF0) | 0x09; // tag | context 0x08, length 0x01
  if (bval) *p++= 0x01;
  else *p++= 0x00;
  return p;
}  

//encocde a context tagged property value
octet far* ecUINT(octet far* p, octet tag, dword property_id)
{ octet far* s= p;
  p= eUINT(p,property_id);
  *s= (tag & 0xF0) | 0x08 | (*s & 0x07); // tag nr | context | length
  return p;
  
}

// encode BACnetVTClassList
octet far* eVTCL(octet far* p, const BACnetVTClassList far* pv, int TagNr)
{ 
  while (pv!=NULL)
   { p= eENUM(p,pv->vtclass);
     pv= pv->next;
   }
  return p;
}

// encode BACnetEventParam
octet far* eEVPARM(octet far* p, const BACnetEventParameter far* pv, int TagNr)
{ octet far* s;
  //BACnetListBitstringValue far* pbit;
  BACnetPropertyStates far* ps;
  if (pv==NULL) return p;
  switch (pv->event_type)
    {
      case CHANGE_OF_BITSTRING: // [0]
        *p++= 0x0E;
        	s= p;
        	p= eUINT(p,pv->time_delay);
        	*s= 0x08 | (*s & 0x07); // [0]
        	//p= eBITSTRING(p,(byte far*)&pv->bitmask,0x10,8); // [1]
        	*p++= 0x2E; // [2]
        	//pbit= pv->list_bitstring_value;
        	//    while (pbit!=NULL)
        	//      { p= eBITSTRING(p,&pbit->list_bitstring_value,0x80,8);
        	//        pbit= pbit->next;
        	//      }
        	*p++= 0x2F;
        *p++= 0x0F;
        break;
      case CHANGE_OF_STATE: // [1]
        *p++= 0x1E;
        	s= p;
        	p= eUINT(p,pv->time_delay);
        	*s= 0x08 | (*s & 0x07); // [0]
        	*p++= 0x1E; // [1]
        	  ps= pv->list_of_value;
        	  while (ps!=NULL)
        	    { // enum or bool?
        	      ps= ps->next;
        	    }
        	*p++= 0x1F;
        *p++= 0x1F;
        break;
      case CHANGE_OF_VALUE: // [2]
        *p++= 0x2E;
        	s= p;
        	p= eUINT(p,pv->time_delay);
        	*s= 0x08 | (*s & 0x07); // [0]
        	    *p++= 0x1E; // [1]
        	        s=p;
                    p= eREAL(p,pv->low_diff_limit); // ??? where is the variable in the structure?
                    *s= 0x18 | (*s & 0x07); // [1] REAL 
        	    *p++= 0x1F; 
        *p++= 0x2F;
        break;
      case COMMAND_FAILURE: // [3]
        *p++= 0x3E;
        	s= p;
        	p= eUINT(p,pv->time_delay);
        	*s= 0x08 | (*s & 0x07); // [0]
            *p++= 0x1E; 
                p= eLOPREF(p,&pv->setpoint_ref); // [1] 
            *p++= 0x1F; 
        *p++= 0x3F;
        break;
      case FLOATING_LIMIT: // [4]
        *p++= 0x4E;
            s= p;
            p= eUINT(p,pv->time_delay);
            *s= 0x08 | (*s & 0x07); // [0]
            *p++= 0x1E; 
                p= eLOPREF(p,&pv->setpoint_ref); // [1] 
            *p++= 0x1F; 
            s= p;
            p= eREAL(p,pv->low_diff_limit);
            *s= 0x28 | (*s & 0x07); // [2]
            s= p;
            p= eREAL(p,pv->high_diff_limit);
            *s= 0x38 | (*s & 0x07); // [3]
            s= p;
            p= eREAL(p,pv->deadband);
            *s= 0x48 | (*s & 0x07); // [4]
        *p++= 0x4F;
        break;
      case OUT_OF_RANGE: // [5]
        *p++= 0x5E;
        	s= p;
        	p= eUINT(p,pv->time_delay);
        	*s= 0x08 | (*s & 0x07); // [0]
        	s= p;
        	p= eREAL(p,pv->low_limit);
        	*s= 0x18 | (*s & 0x07); // [1]
        	s= p;
        	p= eREAL(p,pv->high_limit);
        	*s= 0x28 | (*s & 0x07); // [2]
        	s= p;
        	p= eREAL(p,pv->deadband);
        	*s= 0x38 | (*s & 0x07); // [3]
        *p++= 0x5F;
        break;
    }
  return p;
}

// encode BACnetSessionKey
octet far* eSKEYS(octet far* p, const BACnetSessionKey far* pv, int TagNr)  
{ 
  while (pv!=NULL)
    { 
      p= eOCTETSTRING(p,(octet *)(pv->session_key),8);								//				***001
      p= eUINT(p,(pv->peer_address.network_number));
      p= eOCTETSTRING(p,(octet *)(pv->peer_address.mac_address),pv->peer_address.address_size);	//	***001
      pv= pv->next;
    }
  return p;
}
                         
// encode BACnetAddressBinding
octet far* eDABIND(octet far* p, const BACnetAddressBinding far* pv, int TagNr)   
{ 
  while (pv!=NULL)
    { // device object id
      p= etagOBJECTID(p,0xC0,pv->device_object_id);
      // BACnetAddress
      p= eUINT(p,pv->device_address.network_number);
      p= eOCTETSTRING(p,(octet *)(pv->device_address.mac_address),pv->device_address.address_size);	//***001
      pv= pv->next;
    } 
  return p;
}
          
// encode BACnetRecipient
octet far* eRECIP(octet far* p, const BACnetRecipient far* pv, int TagNr)     
{
  while (pv!=NULL)
    { 
      if (pv->choice==0) // BACnetObjectIdentifier
        {
          p= etagOBJECTID(p,0x08,pv->u.device);
        }
      else // BACnetAddress
        { *p++= 0x1E;
          p= eUINT(p,pv->u.address.network_number);
          p= eOCTETSTRING(p,(octet *)(pv->u.address.mac_address),pv->u.address.address_size);	//	***001
          *p++= 0x1F;
        }  
      pv= pv->next;
    }
  return p;
}

// encode BACnetDestination list
octet far* eRECIPLIST(octet far* p, const BACnetDestination far* pv, int TagNr) 
{ 
  while (pv!=NULL)
   {              
     p= eBITSTRING(p,(byte far*)&pv->valid_days,0x80,7);
     *p++= 0xB4;
     memcpy(p,&pv->from_time,4); p+= 4;						//					***001
     *p++= 0xB4;
     memcpy(p,&pv->to_time,4); p+= 4;						//					***001
     p= eRECIP(p,&pv->recipient);
     p= eUINT(p,pv->process_id);
     p= eBOOL(p,pv->notification);
     p= eBITSTRING(p,(byte far*)&pv->transitions,0x80,3);
     pv= pv->next;
   }
  return p;
}


// encode BACnetTimeValue
octet far* eTIMEVALUE(octet far* p, BACnetTimeValue far* ptv, int TagNr)
{
  while (ptv!=NULL)
    { *p++= 0xB4; // application tag
      memcpy(p,&ptv->time,4); p+=4;							//					***001
      switch(ptv->value_type)
        { case BPV:
            p= eENUM(p,ptv->av.bproperty_value);
            break;
          case UNS:
            p= eUINT(p,ptv->av.uproperty_value);
            break;
          case FLT:
            p= eREAL(p,ptv->av.fproperty_value);
            break;
        }
      ptv= ptv->next;
    }
  return p;
}


// encode BACnetSetpointReference
octet far* eSETREF(octet far* p, const BACnetObjectPropertyReference far* pv, int TagNr)    
{
  if (pv==NULL) { p= eNULL(p); return p; } // optional: SetpointReference
  while (pv!=NULL)
    { *p++= 0x0E; // [0] opening tag 
       p= eLOPREF(p,pv);
       *p++= 0x0F;
       pv= pv->next;
    }
  return p;
}

// encode list of unsigned
octet far* eSTAVALS(octet far* p,const octet far* pv, int TagNr)   
{ if (p==NULL) return p;
  for (int i=0; pv[i]>0; i++)
    p= eUINT(p,pv[i]);
  return p;
}
             
// encode BACnetVTSession
octet far* eVTSE(octet far* p, const BACnetVTSession far* pv, int TagNr)      
{ 
  while (pv!=NULL)
    { p= eUINT(p,pv->local_session_id);
      p= eUINT(p,pv->remote_session_id);
      // BACnetAddress
      p= eUINT(p,pv->remote_address.network_number);
      p= eOCTETSTRING(p,(octet *)(pv->remote_address.mac_address),pv->remote_address.address_size);
      pv= pv->next;
    }
  return p;
}
  

// This function is used to encode any property value according to the datatype.
// in:		p	buffer
//		msg	action to perform (test value, property value)
// returns:	buffer_address + number_of_encoded_octets
octet far* eASN_1_AnyType(octet far* p, PVMessage far* msg)
{
  int i= msg->TestNr;

  if ( (msg->Action==RND_ENCODEPROP) && ((msg->TestNr<0)||(msg->TestNr>1)) )
    msg->TestNr= 0;

  switch (msg->pt) //parse type
    {
     case ob_id: // an object identifier -----------------------
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP:     // encode from database
           case RND_ENCODEPROP: // encode a test value (never)
             p= etagOBJECTID(p, 0xC0, *((dword far*)msg->pv) );
             break;  
         }  
       break;
       
     case et: // an enumeration table --------------------------
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eENUM(p, *((word far*)(msg->pv)) ); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eENUM(p, gTestEnum[i]);   
             break;
         }    
       break;                                                   
       
     case flt: // float ----------------------------------------
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eREAL(p, *((float far*)(msg->pv)) ); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eREAL(p, gTestReal[i]);   
             break;
         }    
       break;
       
     case bits: // octet of 1 or 0 flags
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eBITSTRING(p, (byte far*)msg->pv,0x80,msg->UsedBits); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eBITSTRING(p,&gTestBits[i],0x80,gTestUsedBits);
             break;
         }    
       break;          

     case u127: // 1..127 -----------------------------------------------
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eUINT(p, *((octet far*)(msg->pv)) ); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eUINT(p, gTestInt[i]);   
             break;
         }    
       break;

     case u16:  // 1..16
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eUINT(p, *((octet far*)(msg->pv)) ); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eUINT(p, gTestInt[i]);   
             break;
         }    
       break;

     case ud:   // unsigned dword
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eUINT(p, *((dword far*)(msg->pv)) ); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eUINT(p, gTestInt[i]);   
             break;
         }    
       break;                     

     case uw:   // unsigned word
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eUINT(p, *((word far*)(msg->pv)) ); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eUINT(p, gTestInt[i]);   
             break;
         }    
       break;
       
     case sw:   // signed word ------------------------------------------
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eINT(p, *((short far*)(msg->pv)) ); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eINT(p, gTestInt[i]); 
             break;
         }    
       break;
       
     case s10:  // char [10] --------------------------------------------
     case s32:  // char [32]
     case s64:  // char [64]
     case s132: // char [132]
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eCHARSTRING(p,(char far*)msg->pv,0x70,strlen((char far*)msg->pv),ANSI,0); //***001
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eCHARSTRING(p,gTestString[i],0x70,strlen(gTestString[i]),ANSI,0);		//	***001
             break;
         }    
       break;
                                                             
     case ebool: // boolean enumeration ---------------------------------
     
       switch(msg->Action)
         {
           case SET_TRUE:
             p= eBOOL(p, 1); // encode true  value  
             break;
           case SET_FALSE:
             p= eBOOL(p, 0); // encode false value  
             break;  
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eBOOL(p, *((boolean far*)(msg->pv)) ); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eBOOL(p, gTestBool[i]); // random value  
             break;
         }    
       break;
       
     case enull: // null enumeration ------------------------------------
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eNULL(p); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eNULL(p);  
             break;
         }    
       break;

     case ptDate: // date
       p= eBACnetDate(p,(BACnetDate far*)msg->pv);
       break;
     
     case ptTime: // time
       p= eBACnetTime(p,(BACnetTime far*)msg->pv);
       break;
     
     case dt: // date + time
       switch(msg->Action)
         {
           case SETPROP: 
             new CBACnetDateTimeDlg((BACnetDateTime far*)msg->pv,CWnd::FromHandle(msg->hw));
             break;
           case ENCODEPROP: // encode from database
             p= eDT(p,(BACnetDateTime far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eDT(p,&gTestDT[i]);
             break;
         }    
       break;

     case calist: // list of calendarentry
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eCALIST(p,(BACnetCalendarEntry far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eCALIST(p,gTestCalist[i]);
             break;
         }    
       break;

     case paf: // priority array flt
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= ePAF(p,(float far*)msg->pv,msg->ArrayIndex);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= ePAF(p,gTestPAF[i]);
             break;
         }    
       break;

     case pab: // priority array bpv
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= ePAB(p,(enum BACnetBinaryPV far*)msg->pv,msg->ArrayIndex);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= ePAB(p,gTestPAB[i]);
             break;
         }    
       break;

     case pau: // priority array uw
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= ePAU(p,(word far*)msg->pv,msg->ArrayIndex);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= ePAU(p,gTestPAU[i]);
             break;
         }    
       break;

     case dtrange: // range of dates
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eDTRANGE(p,(BACnetDateRange far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eDTRANGE(p,&gTestDTRange[i]);
             break;
         }    
       break;

     case raslist: // list of readaccessspecs
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eRASLIST(p,(BACnetReadAccessSpecification far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eRASLIST(p,gTestRaslist[i]);
             break;
         }    
       break;

     case lobj: // list of objects
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eLOBJ(p,(BACnetObjectIdentifier far*)msg->pv,msg->ArrayIndex);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eLOBJ(p,&gTestLObj[i]);
             break;
         }    
       break;
       
     case act: // action array
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eACT(p,(BACnetActionCommand far**)msg->pv, msg->Num,msg->ArrayIndex);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eACT(p,gTestAct[i],1);
             break;
         }    
       break;

     case statext: // state text array
     case actext:  // action_text array
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eACTEXT(p,(char far**)msg->pv,msg->Num,msg->ArrayIndex);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eACTEXT(p,gTestActext[i],1);
             break;
         }    
       break;

     case vtcl: // vt classes
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eVTCL(p,(BACnetVTClassList far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eVTCL(p,gTestVtcl[i]);
             break;
         }    
       break;

     case evparm: // event parameter
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eEVPARM(p,(BACnetEventParameter far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eEVPARM(p,&gTestEvparm[i]);
             break;
         }    
       break;

     case skeys: // session keys
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eSKEYS(p,(BACnetSessionKey far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eSKEYS(p,gTestSkeys[i]);
             break;
         }    
       break;

     case dabind: // device address bindings
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eDABIND(p,(BACnetAddressBinding far*)msg->pv); 
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eDABIND(p,gTestDabind[i]); 
             break;
         }    
       break;

     case tsrecip: // time synch recipients
     case recip: // recipient
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eRECIP(p,(BACnetRecipient far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eRECIP(p,gTestRecip[i]);
             break;
         }    
       break;

     case reciplist: // list of BACnetDestination
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eRECIPLIST(p,(BACnetDestination far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eRECIPLIST(p,gTestReciplist[i]);
             break;
         }    
       break;

     case xsched:    // exception schedule: array[] of specialevent
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eXSCHED(p,(BACnetExceptionSchedule far*)msg->pv,msg->ArrayIndex);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eXSCHED(p,gTestXSched[i]);
             break;
         }    
       break;

     case wsched: // weekly schedule: array[7] of list of timevalue
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eWSCHED(p,(BACnetTimeValue far**)msg->pv,7,msg->ArrayIndex);
             break;
           case RND_ENCODEPROP: // encode a test value
             p= eWSCHED(p,gTestWsched[i],7); 
             break;
         }    
       break;

     case stavals: // list of unsigned
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eSTAVALS(p,(octet far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eSTAVALS(p,gTestStavals[i]);
             break;
         }    
       break;
     
     case propref: // list of object prop refs
     case lopref:  // list of object prop refs
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eLOPREF(p,(BACnetObjectPropertyReference far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eLOPREF(p,gTestLopref[i]);
             break;
         }    
       break;

     case setref: // setpoint reference
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eSETREF(p,(BACnetObjectPropertyReference far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eSETREF(p,gTestLopref[i]);
             break;
         }    
       break;

     case vtse: // list of active  vt sessions (parse type) 
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eVTSE(p,(BACnetVTSession far*)msg->pv);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eVTSE(p,gTestVtse[i]);
             break;
         }
       break;      

     case uwarr: // array of uw
       switch(msg->Action)
         {
           case SETPROP:
             break;
           case ENCODEPROP: // encode from database
             p= eUWARR(p,(word far*)msg->pv,msg->Num);
             break;
           case RND_ENCODEPROP: // encode a test value 
             p= eUWARR(p,gTestUWArr[i],nTESTUWNUM);
             break;
         }    
       break;

    } // switch    
  return p;   
}


// This function is used to encode any property value and/or service parameters:
//
// 	msg.Action==	GET_PARSE_TYPE:
//		return the parsetype of the property
// 	msg.Action== ASN_1_ANY_PROP:
//		encode the property value
// 	msg.Action== RND_ASN_1_ANY_PROP:
//		encode test property value
// 	any other value for msg.Action:
//		encode ObjectId, PropertyId, ArrayIndex (opt.), 
//		Property value/test value, Priority (opt.)
//
// in:	Buffer		points to a buffer to contain the encoded property value
//		msg			PVMessage, (see struct declaration)
// returns: 0 if ok, else !=0
int EncodeFromDatabase(char far* Buffer, PVMessage far* msg)
{ word object_type; dword object_inst;
  octet far* p; // end of encoded data
  octet far* s; // start of encoded data
  generic_object far* Obj= (generic_object far*) msg->Obj;
  
  p= (octet far*) Buffer;
  msg->BufferLen= 0;
  
  switch (msg->Action)
    {
      case GET_PARSE_TYPE: // return parse type
        break;
    
      case ASN_1_ANY_PROP: // encode Property Value only
        msg->Action= ENCODEPROP;      // new action
        p= eASN_1_AnyType(p,msg);     // encode any property value
        msg->Action= ASN_1_ANY_PROP;  // restore action 
        break;

      case RND_ASN_1_ANY_PROP: // encode test Property Value only
         msg->Action= RND_ENCODEPROP;      // new action
         p= eASN_1_AnyType(p,msg);         // encode any property value
         msg->Action= RND_ASN_1_ANY_PROP;  // restore action 
         break;
         
      default:
        // encode ObjectId ---
        if (msg->Obj==NULL) return(-1); // no object, no property
        object_type= (word)(Obj->object_id>>22);
        object_inst= Obj->object_id & 0x3FFFFF;
        s= p;
        p= eOBJECTID(s,object_type,object_inst); // application tagged
        *s= 0x08 | (*s & 0x07);                  // adjust context tag
        // encode PropId ---
        s= p; 
        p= eUINT(s,msg->PropId);     // application tagged
        *s= 0x18 | (*s & 0x07); // adjust context tag
        // encode ArrayIndex ---
        if (msg->ArrayIndex != -1)
          { s= p;
            p= eUINT(s,msg->ArrayIndex); // application tagged
            *s= 0x28 | (*s & 0x07);      // adjust context tag
          }
        // encode PropVal ---
        *(p++)= 0x3E; // open tag
        p= eASN_1_AnyType(p,msg);
        *(p++)= 0x3F; // closing tag
        // encode Priority ---
        if (msg->Priority != -1)
          { s= p;
            p= eUINT(s,msg->Priority); 
            *s= 0x48 | (*s & 0x07);
          }
        break;
         
    } // switch
  
   *p= '\0'; // end of string
   msg->BufferLen= (char far*)p - (char far*)Buffer; // Buffer length
   return(0); // well done
}                                                             
 
 
int MyRand(int range)
{	  
  srand( (unsigned)time( NULL ) );
  return rand() / (RAND_MAX / range); 
}	  


// This function is used to convert an octet value to a hex representation. 
// in:	Buffer		points to a buffer to contain hex value, eg: 0x3E
//		Flag		octet, eg: 0011 1110
void Hex(char far* Buffer, octet Flag)
{
  Buffer[0]= '0';
  Buffer[1]= 'x';
  Buffer[2]= 30 + (Flag & 0xF0);
  Buffer[3]= 30 + (Flag & 0x0F);
  Buffer[4]= '\0';
}

void HexStr(char far* Buffer, char far* OctetStr, int s, int n)
{ 
  n= s+n;
  int j= 0;
  for (int i= s; i<n; i++)
    {
      Hex(Buffer+j,OctetStr[i]);
      j+= 4;
    }
  Buffer[j]= 0;  
}


// This function is used to convert a double value into a string representation.
// in: flt     value to be converted
//     Buffer  points to a buffer to contain string representation
void CvtFltStr(double fval, char far* Buffer)
{  
  _gcvt(fval,7,Buffer);
}
double CvtStrFlt(char far* Buffer)
{ 
  return( atof(Buffer) );
}


// ===========================================================================================================
//
// BACnetStandardObject Functions:
//
// ===========================================================================================================

        
// analog input
int ai(char far* Buffer,PVMessage far* msg) 
{ 
  ai_obj_type far* Obj= (ai_obj_type far*)msg->Obj;  

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= flt;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case DEVICE_TYPE:
        msg->pv= Obj->device_type;
        msg->pt= s64;
       break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
      case UPDATE_INTERVAL:
        msg->pv= &Obj->update_interval;
        msg->pt= uw;
        break;
      case UNITS:
        msg->pv= &Obj->units;
        msg->pt= et;
        break;
      case MIN_PRES_VALUE:
        msg->pv= &Obj->min_pres_value;
        msg->pt= flt;
        break;
      case MAX_PRES_VALUE:
        msg->pv= &Obj->max_pres_value;
        msg->pt= flt;
        break;
      case RESOLUTION:
        msg->pv= &Obj->resolution;
        msg->pt= flt;
        break;
      case COV_INCREMENT:
        msg->pv= &Obj->cov_increment;
        msg->pt= flt;
        break;
      case TIME_DELAY:
        msg->pv= &Obj->time_delay;
        msg->pt= uw;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case HIGH_LIMIT:
        msg->pv= &Obj->high_limit;
        msg->pt= flt;
        break;
      case LOW_LIMIT:
        msg->pv= &Obj->low_limit;
        msg->pt= flt;
       break;
      case DEADBAND:	
        msg->pv= &Obj->deadband;
        msg->pt= flt;
        break;
      case LIMIT_ENABLE:
        msg->pv= &Obj->limit_enable;
        msg->pt= bits;
        msg->UsedBits= 2;
       break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
    }

  return EncodeFromDatabase(Buffer, msg);
}

               
// analog output               
int ao(char far* Buffer,PVMessage far* msg)
{ ao_obj_type far* Obj= (ao_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= flt;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case DEVICE_TYPE:
        msg->pv= Obj->device_type;
        msg->pt= s64;
       break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
      case UNITS:
        msg->pv= &Obj->units;
        msg->pt= et;
        break;
      case MIN_PRES_VALUE:
        msg->pv= &Obj->min_pres_value;
        msg->pt= flt;
        break;
      case MAX_PRES_VALUE:
        msg->pv= &Obj->max_pres_value;
        msg->pt= flt;
        break;
      case RESOLUTION:
        msg->pv= &Obj->resolution;
        msg->pt= flt;
        break;
      case PRIORITY_ARRAY:    
        msg->pv= Obj->priority_array;
        msg->pt= paf;
        break;
      case RELINQUISH_DEFAULT:
        msg->pv= &Obj->relinquish_default;
        msg->pt= flt;
        break;
      case COV_INCREMENT:
        msg->pv= &Obj->cov_increment;
        msg->pt= flt;
        break;
      case TIME_DELAY:
        msg->pv= &Obj->time_delay;
        msg->pt= uw;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case HIGH_LIMIT:
        msg->pv= &Obj->high_limit;
        msg->pt= flt;
        break;
      case LOW_LIMIT:
        msg->pv= &Obj->low_limit;
        msg->pt= flt;
       break;
      case DEADBAND:	
        msg->pv= &Obj->deadband;
        msg->pt= flt;
        break;
      case LIMIT_ENABLE:
        msg->pv= &Obj->limit_enable;
        msg->pt= bits;
        msg->UsedBits= 2;
        break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
    }

  return EncodeFromDatabase(Buffer, msg);
}




// analog value
int av(char far* Buffer,PVMessage far* msg)
{ av_obj_type far* Obj= (av_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= flt;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:     
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
      case UNITS:
        msg->pv= &Obj->units;
        msg->pt= et;
        break;
      case PRIORITY_ARRAY:    
        msg->pv= Obj->priority_array;
        msg->pt= paf;
        break;
      case RELINQUISH_DEFAULT:
        msg->pv= &Obj->relinquish_default;
        msg->pt= flt;
        break;
      case COV_INCREMENT:
        msg->pv= &Obj->cov_increment;
        msg->pt= flt;
        break;
      case TIME_DELAY:
        msg->pv= &Obj->time_delay;
        msg->pt= uw;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case HIGH_LIMIT:
        msg->pv= &Obj->high_limit;
        msg->pt= flt;
        break;
      case LOW_LIMIT:
        msg->pv= &Obj->low_limit;
        msg->pt= flt;
       break;
      case DEADBAND:	
        msg->pv= &Obj->deadband;
        msg->pt= flt;
        break;
      case LIMIT_ENABLE:
        msg->pv= &Obj->limit_enable;
        msg->pt= bits;
        msg->UsedBits= 2;
        break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
    }

  return EncodeFromDatabase(Buffer, msg);
}


// binary input
int bi(char far* Buffer,PVMessage far* msg)
{ bi_obj_type far* Obj= (bi_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:           
        ultoa(Obj->go.object_id,gBuffer,10);
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case DEVICE_TYPE:
        msg->pv= Obj->device_type;
        msg->pt= s64;
       break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
      case POLARITY:
        msg->pv= &Obj->polarity;
        msg->pt= et;
        break;
      case INACTIVE_TEXT:
        msg->pv= Obj->inactive_text;
        msg->pt= s64;
        break;
      case ACTIVE_TEXT:
        msg->pv= Obj->active_text;
        msg->pt= s64;
        break;
      case CHANGE_OF_STATE_TIME:             
        msg->pv= &Obj->cos_time;
        msg->pt= dt;
        break;
      case CHANGE_OF_STATE_COUNT:
        msg->pv= &Obj->cos_count;
        msg->pt= uw;
        break;
      case TIME_OF_STATE_COUNT_RESET:
        msg->pv= &Obj->time_of_state_count_reset;
        msg->pt= dt;
        break;
      case ELAPSED_ACTIVE_TIME:
        msg->pv= &Obj->elapsed_active_time;
        msg->pt= ud;
        break;
      case TIME_OF_ACTIVE_TIME_RESET:
        msg->pv= &Obj->time_of_active_time_reset;
        msg->pt= dt;
        break;
      case TIME_DELAY:
        msg->pv= &Obj->time_delay;
        msg->pt= uw;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case ALARM_VALUE:
        msg->pv= &Obj->alarm_value;
        msg->pt= et;
        break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
   }

  return EncodeFromDatabase(Buffer, msg);
}

            
//binary output            
int bo(char far* Buffer,PVMessage far* msg)
{ bo_obj_type far* Obj= (bo_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case DEVICE_TYPE:
        msg->pv= Obj->device_type;
        msg->pt= s64;
        break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
      case POLARITY:
        msg->pv= &Obj->polarity;
        msg->pt= et;
        break;
      case INACTIVE_TEXT:
        msg->pv= Obj->inactive_text;
        msg->pt= s64;
        break;
      case ACTIVE_TEXT:
        msg->pv= Obj->active_text;
        msg->pt= s64;
        break;
      case CHANGE_OF_STATE_TIME:             
        msg->pv= &Obj->cos_time;
        msg->pt= dt;
        break;
      case CHANGE_OF_STATE_COUNT:
        msg->pv= &Obj->cos_count;
        msg->pt= uw;
        break;
      case TIME_OF_STATE_COUNT_RESET:
        msg->pv= &Obj->time_of_state_count_reset;
        msg->pt= dt;
        break;
      case ELAPSED_ACTIVE_TIME:
        msg->pv= &Obj->elapsed_active_time;
        msg->pt= ud;
        break;
      case TIME_OF_ACTIVE_TIME_RESET:
        msg->pv= &Obj->time_of_active_time_reset;
        msg->pt= dt;
        break;
      case MINIMUM_OFF_TIME:
        msg->pv= &Obj->min_off_time;
        msg->pt= ud;
        break;
      case MINIMUM_ON_TIME:
        msg->pv= &Obj->min_on_time;
        msg->pt= ud;
        break;                 
      case PRIORITY_ARRAY:    
        msg->pv= Obj->priority_array;
        msg->pt= pab;
        break;
      case RELINQUISH_DEFAULT:
        msg->pv= &Obj->relinquish_default; 
        msg->pt= et;
        break;
      case TIME_DELAY:
        msg->pv= &Obj->time_delay;
        msg->pt= uw;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case FEEDBACK_VALUE:
        msg->pv= &Obj->feedback_value;
        msg->pt= et;
        break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
    }

  return EncodeFromDatabase(Buffer, msg);
}

int bv(char far* Buffer,PVMessage far* msg)
{ bv_obj_type far* Obj= (bv_obj_type far*)msg->Obj;
 
  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
      case INACTIVE_TEXT:
        msg->pv= Obj->inactive_text;
        msg->pt= s64;
        break;
      case ACTIVE_TEXT:
        msg->pv= Obj->active_text;
        msg->pt= s64;
        break;
      case CHANGE_OF_STATE_TIME:             
        msg->pv= &Obj->cos_time;
        msg->pt= dt;
        break;
      case CHANGE_OF_STATE_COUNT:
        msg->pv= &Obj->cos_count;
        msg->pt= uw;
        break;
      case TIME_OF_STATE_COUNT_RESET:
        msg->pv= &Obj->time_of_state_count_reset;
        msg->pt= dt;
        break;
      case ELAPSED_ACTIVE_TIME:
        msg->pv= &Obj->elapsed_active_time;
        msg->pt= ud;
        break;
      case TIME_OF_ACTIVE_TIME_RESET:
        msg->pv= &Obj->time_of_active_time_reset;
        msg->pt= dt;
        break;
      case MINIMUM_OFF_TIME:
        msg->pv= &Obj->min_off_time;
        msg->pt= ud;
        break;
      case MINIMUM_ON_TIME:
        msg->pv= &Obj->min_on_time;
        msg->pt= ud;
        break;
      case PRIORITY_ARRAY:    
        msg->pv= Obj->priority_array;
        msg->pt= pab;
        break;
      case RELINQUISH_DEFAULT:
        msg->pv= &Obj->relinquish_default;
        msg->pt= et;
        break;
      case TIME_DELAY:
        msg->pv= &Obj->time_delay;
        msg->pt= uw;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case ALARM_VALUE:
        msg->pv= &Obj->alarm_value;
        msg->pt= et;
        break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
    }

  return EncodeFromDatabase(Buffer, msg);
}
                     
//calendar                     
int cal(char far* Buffer,PVMessage far* msg)
{ calendar_obj_type far* Obj= (calendar_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= ebool;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case DATE_LIST:
        msg->pv= Obj->date_list;
        msg->pt= calist;
        break;
    }  

  return EncodeFromDatabase(Buffer, msg);
}

//comnmand
int com(char far* Buffer,PVMessage far* msg)
{ command_obj_type far* Obj= (command_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= ud;
        break;
      case IN_PROCESS:
        msg->pv= &Obj->in_process;
        msg->pt= ebool;
        break;
      case ALL_WRITES_SUCCESSFUL:
        msg->pv= &Obj->all_writes_successful;
        msg->pt= ebool;
        break;
      case ACTION:
        msg->pv= Obj->action;
        msg->pt= act;  
        msg->Num= Obj->num_actions;
        break;
      case ACTION_TEXT:
        msg->pv= &Obj->action_text; 
        msg->pt= actext;
        for (msg->Num= 0; (msg->Num < 32) && (Obj->action_text[msg->Num]); msg->Num++);
        break;
    }  

  return EncodeFromDatabase(Buffer, msg);
}

// device
int dev(char far* Buffer,PVMessage far* msg)
{ device_obj_type far* Obj= (device_obj_type far*)msg->Obj;
  
  BACnetTime LocalTime;
  BACnetDate LocalDate;
  
  msg->UsedBits= 0;
  msg->pv= NULL;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case SYSTEM_STATUS:
        msg->pv= &Obj->system_status;
        msg->pt= et;
        break;
      case VENDOR_NAME:
        msg->pv= Obj->vendor_name;
        msg->pt= s64;
        break;
      case VENDOR_IDENTIFIER:
        msg->pv= &Obj->vendor_id;
        msg->pt= uw;
        break;
      case MODEL_NAME:
        msg->pv= Obj->model_name;
        msg->pt= s32;
        break;
      case FIRMWARE_REVISION:
        msg->pv= Obj->firmware_rev;
        msg->pt= s10;
        break;
      case APPLICATION_SOFTWARE_VERSION:
        msg->pv= Obj->application_software_ver;
        msg->pt= s32;
        break;
      case LOCATION:
        msg->pv= Obj->location;
        msg->pt= s64;
        break;
      case PROTOCOL_VERSION:
        msg->pv= &Obj->protocol_ver;
        msg->pt= uw;
        break;
      case PROTOCOL_CONFORMANCE_CLASS:
        msg->pv= &Obj->protocol_conf_class;
        msg->pt= uw;         
        break;
      case PROTOCOL_SERVICES_SUPPORTED:
        msg->pv= Obj->protocol_services_supported;
        msg->pt= bits;
        msg->UsedBits= 35;
        break;
      case PROTOCOL_OBJECT_TYPES_SUPPORTED:
        msg->pv= Obj->object_types_supported;
        msg->pt= bits; 
        msg->UsedBits= 18;
        break;  
      case OBJECT_LIST:
        msg->pv= Obj->object_list;
        msg->pt= lobj;
        break;  
      case MAX_APDU_LENGTH_ACCEPTED:
        msg->pv= &Obj->max_apdu_length_accepted;
        msg->pt= uw;
        break;
      case SEGMENTATION_SUPPORTED:
        msg->pv= &Obj->segmentation_supported;
        msg->pt= et;
        break;
      case VT_CLASSES_SUPPORTED:
        msg->pv= Obj->vt_classes_supported;
        msg->pt= vtcl;
        break;
      case ACTIVE_VT_SESSIONS:
        msg->pv= Obj->active_vt_sessions;
        msg->pt= vtse;
        break;
      case LOCAL_TIME:
        { time_t timer;
          struct tm* ptm;
          tzset();
          time(&timer);
          ptm= localtime(&timer);
          LocalTime.hour=   dontcare; LocalTime.minute=     dontcare; 
          LocalTime.second= dontcare; LocalTime.hundredths= dontcare;
          if (ptm!=NULL)
            { LocalTime.hour=   ptm->tm_hour;  LocalTime.minute=     ptm->tm_min; 
              LocalTime.second= ptm->tm_sec;   LocalTime.hundredths= dontcare;
            }
        }  
        msg->pv= &LocalTime;
        msg->pt= ptTime;
        break;
      case LOCAL_DATE:
        { time_t timer;
          struct tm* ptm;
          tzset();
          time(&timer);
          ptm= localtime(&timer);
          LocalDate.year=         dontcare; LocalDate.month=       dontcare; 
          LocalDate.day_of_month= dontcare; LocalDate.day_of_week= dontcare;
          if (ptm!=NULL)
            { LocalDate.year=         ptm->tm_year; LocalDate.month=       ptm->tm_mon   +1; 
              LocalDate.day_of_month= ptm->tm_mday; LocalDate.day_of_week= ptm->tm_wday  +1;
            }  
        }  
        msg->pv= &LocalDate;
        msg->pt= ptDate;
        break;
      case UTC_OFFSET:
        msg->pv= &Obj->utc_offset;
        msg->pt= flt;
        break;
      case DAYLIGHT_SAVINGS_STATUS:
        msg->pv= &Obj->day_savings_status;
        msg->pt= ebool;
        break;
      case APDU_SEGMENT_TIMEOUT:
        msg->pv= &Obj->apdu_segment_timeout;
        msg->pt= uw;
        break;
      case APDU_TIMEOUT:
        msg->pv= &Obj->apdu_timeout;
        msg->pt= uw;
        break;
      case NUMBER_OF_APDU_RETRIES:
        msg->pv= &Obj->number_apdu_retries;
        msg->pt= uw;
        break;
      case LIST_OF_SESSION_KEYS:
        msg->pv= Obj->list_session_keys;
        msg->pt= skeys;
        break;
      case TIME_SYNCHRONIZATION_RECIPIENTS:
        msg->pv= Obj->time_synch_recipients;
        msg->pt= tsrecip;
        break;
      case MAX_MASTER:
        msg->pv= &Obj->max_master;
        msg->pt= uw;
        break;
      case MAX_INFO_FRAMES:
        msg->pv= &Obj->max_info_frames;
        msg->pt= uw;
        break;
      case DEVICE_ADDRESS_BINDING:
        msg->pv= Obj->device_add_binding;
        msg->pt= dabind;
        break;
    }    

  return EncodeFromDatabase(Buffer, msg);
}
    
// event enrollment    
int eve(char far* Buffer,PVMessage far* msg)
{ ee_obj_type far* Obj= (ee_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case EVENT_TYPE:
        msg->pv= &Obj->parameter_list.event_type;
        msg->pt= et;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
      case EVENT_PARAMETERS: 
        msg->pv= &Obj->parameter_list;
        msg->pt= evparm;
        break;
      case OBJECT_PROPERTY_REFERENCE: 
        msg->pv= &Obj->obj_prop_ref;
        msg->pt= lopref;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
     case RECIPIENT:     
        msg->pv= &Obj->recipient;
        msg->pt= recip;
        break;
      case PROCESS_IDENTIFIER:
        msg->pv= &Obj->process_id;
        msg->pt= uw;
        break;
      case PRIORITY:
        msg->pv= &Obj->priority;
        msg->pt= uw;
        break;
      case ISSUE_CONFIRMED_NOTIFICATIONS:
        msg->pv= &Obj->issue_conf_notifications;
        msg->pt= et;
        break;
    }    

  return EncodeFromDatabase(Buffer, msg);
}

// file 
int flo(char far* Buffer,PVMessage far* msg)
{ file_obj_type far* Obj= (file_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case FILE_TYPE:
        msg->pv= Obj->file_type;
        msg->pt= s32;
        break;
      case FILE_SIZE:
        msg->pv= &Obj->file_size;
        msg->pt= ud;
        break;
      case MODIFICATION_DATE: 
        msg->pv= &Obj->mod_date;
        msg->pt= dt;
        break;
      case ARCHIVE:
        msg->pv= &Obj->archive;
        msg->pt= ebool;
      case READ_ONLY:
        msg->pv= &Obj->read_only;
        msg->pt= ebool;
        break;
      case FILE_ACCESS_METHOD: 
        msg->pv= &Obj->access_method;
        msg->pt= et;
        break;
    }    

  return EncodeFromDatabase(Buffer, msg);
}

// group
int gru(char far* Buffer,PVMessage far* msg)
{ group_obj_type far* Obj= (group_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case LIST_OF_GROUP_MEMBERS: 
        msg->pv= Obj->list_of_group_members;
        msg->pt= raslist;
        break;
    }    

  return EncodeFromDatabase(Buffer, msg);
}

// loop
int loo(char far* Buffer,PVMessage far* msg)
{ loop_obj_type far* Obj= (loop_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= flt;
        break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
      case UPDATE_INTERVAL:
        msg->pv= &Obj->update_interval;
        msg->pt= uw;
        break;
      case OUTPUT_UNITS: 
        msg->pv= &Obj->output_units;
        msg->pt= et;
        break;
      case MANIPULATED_VARIABLE_REFERENCE:
        msg->pv= &Obj->man_var_ref;
        msg->pt= lopref;
        break;
      case CONTROLLED_VARIABLE_REFERENCE:
        msg->pv= &Obj->cont_var_ref;
        msg->pt= lopref;
        break;
      case CONTROLLED_VARIABLE_VALUE:  
        msg->pv= &Obj->cont_var_value;
        msg->pt= flt;
        break;
      case CONTROLLED_VARIABLE_UNITS:
        msg->pv= &Obj->cont_var_units;
        msg->pt= et;
        break;
      case SETPOINT_REFERENCE:
        msg->pv= Obj->setpoint_ref;
        msg->pt= setref;
        break;
      case SETPOINT:     
        msg->pv= &Obj->setpoint;
        msg->pt= flt;
        break;
      case ACTION:
        msg->pv= &Obj->action;
        msg->pt= et;
        break;
      case PROPORTIONAL_CONSTANT:
        msg->pv= &Obj->proportional_const;
        msg->pt= flt;
        break;
      case PROPORTIONAL_CONSTANT_UNITS:
        msg->pv= &Obj->proportional_const_units;
        msg->pt= et;
        break;
      case INTEGRAL_CONSTANT:
        msg->pv= &Obj->integral_const;
        msg->pt= flt;
        break;
      case INTEGRAL_CONSTANT_UNITS:  
        msg->pv= &Obj->integral_const_units;
        msg->pt= et;
        break;
      case DERIVATIVE_CONSTANT:  
        msg->pv= &Obj->derivative_const;
        msg->pt= flt;
        break;
      case DERIVATIVE_CONSTANT_UNITS:  
        msg->pv= &Obj->derivative_const_units;
        msg->pt= et;
        break;
      case BIAS:
        msg->pv= &Obj->bias;
        msg->pt= flt;
        break;
      case MAXIMUM_OUTPUT:
        msg->pv= &Obj->max_output;
        msg->pt= flt;
        break;
      case MINIMUM_OUTPUT:
        msg->pv= &Obj->min_output;
        msg->pt= flt;
        break;
      case PRIORITY_FOR_WRITING:
        msg->pv= &Obj->priority_for_writing;
        msg->pt= uw;
        break;
      case COV_INCREMENT:
        msg->pv= &Obj->cov_increment;
        msg->pt= flt;
        break;
      case TIME_DELAY:
        msg->pv= &Obj->time_delay;
        msg->pt= uw;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case ERROR_LIMIT:
        msg->pv= &Obj->error_limit;
        msg->pt= flt;
        break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
    }    

  return EncodeFromDatabase(Buffer, msg);
}
 

// multi state input 
int msi(char far* Buffer,PVMessage far* msg)
{ mi_obj_type far* Obj= (mi_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt=ud;
        break;
      case DEVICE_TYPE:
        msg->pv= Obj->device_type;
        msg->pt= s64;
       break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
      case NUMBER_OF_STATES:
        msg->pv= &Obj->num_of_states;
        msg->pt= uw;
        break;
      case STATE_TEXT:
        msg->pv= &Obj->state_text;
        msg->pt= statext;
        msg->Num= 32;
        break;
      case TIME_DELAY:
        msg->pv= &Obj->time_delay;
        msg->pt= uw;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case ALARM_VALUES:
        msg->pv= Obj->alarm_values;
        msg->pt= stavals;
        break;
      case FAULT_VALUES:
        msg->pv= Obj->fault_values;
        msg->pt= stavals;
        break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
   }    

  return EncodeFromDatabase(Buffer, msg);
}

// multi state output
int mso(char far* Buffer,PVMessage far* msg)
{ mo_obj_type far* Obj= (mo_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        msg->pt= uw;
        break;
      case DEVICE_TYPE:
        msg->pv= Obj->device_type;
        msg->pt= s64;
       break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case EVENT_STATE:
        msg->pv= &Obj->state;
        msg->pt= et;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
      case NUMBER_OF_STATES:
        msg->pv= &Obj->num_of_states;
        msg->pt= uw;
        break;
      case STATE_TEXT:
        msg->pv= &Obj->state_text;
        msg->pt= statext; 
        msg->Num= 32;
        break;
      case PRIORITY_ARRAY:
        msg->pv= Obj->priority_array;
        msg->pt= pau;
        break;
      case RELINQUISH_DEFAULT:
        msg->pv= &Obj->relinquish_default;
        msg->pt= uw;
        break;
      case TIME_DELAY:
        msg->pv= &Obj->time_delay;
        msg->pt= uw;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case FEEDBACK_VALUE:
        msg->pv= &Obj->feedback_value;
        msg->pt= et;
        break;
      case EVENT_ENABLE:	
        msg->pv= &Obj->event_enable;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case ACKED_TRANSITIONS:
        msg->pv= &Obj->acked_transitions;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case NOTIFY_TYPE:
        msg->pv= &Obj->notify_type;
        msg->pt= et;
        break;
    }    

  return EncodeFromDatabase(Buffer, msg);
}

// notification class
int nofy(char far* Buffer,PVMessage far* msg)
{ nc_obj_type far* Obj= (nc_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case NOTIFICATION_CLASS:
        msg->pv= &Obj->notification_class;
        msg->pt= uw;
        break;
      case PRIORITY:
        msg->pv= Obj->priority;
        msg->pt= uwarr;
        msg->Num= 3;
        break;
      case ACK_REQUIRED:
        msg->pv= &Obj->ack_required;
        msg->pt= bits;
        msg->UsedBits= 3;
        break;
      case RECIPIENT_LIST:
        msg->pv= Obj->recipient_list;
        msg->pt= reciplist;
        break;
    }    

  return EncodeFromDatabase(Buffer, msg);
}

// program
int prg(char far* Buffer,PVMessage far* msg)
{ program_obj_type far* Obj= (program_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case PROGRAM_STATE:
        msg->pv= &Obj->prog_state;
        msg->pt= et;
        break;
      case PROGRAM_CHANGE:
        msg->pv= &Obj->prog_change;
        msg->pt= et;
        break;
      case REASON_FOR_HALT:
        msg->pv= &Obj->reason_for_halt;
        msg->pt= et;
        break;
      case DESCRIPTION_OF_HALT:
        msg->pv= Obj->description_of_halt;
        msg->pt= s64;
        break;
      case PROGRAM_LOCATION:
        msg->pv= Obj->prog_location;
        msg->pt= s64;
        break;
      case INSTANCE_OF:
        msg->pv= Obj->instance_of;
        msg->pt= s64;
        break;
      case STATUS_FLAGS:
        msg->pv= &Obj->status_flags;
        msg->pt= bits;
        msg->UsedBits= 4;
        break;
      case RELIABILITY:
        msg->pv= &Obj->reliability;
        msg->pt= et;
        break;
      case OUT_OF_SERVICE:
        msg->pv= &Obj->out_of_service;
        msg->pt= ebool;
        break;
    }    

  return EncodeFromDatabase(Buffer, msg);
}

// schedule
int sced(char far* Buffer,PVMessage far* msg)
{ schedule_obj_type far* Obj= (schedule_obj_type far*)msg->Obj;

  msg->UsedBits= 0;

  switch ( (enum BACnetPropertyIdentifier) msg->PropId )
    {
      case OBJECT_IDENTIFIER:
        msg->pv= &Obj->go.object_id;
        msg->pt= ob_id;             
        break;                                     
      case OBJECT_NAME:
        msg->pv= Obj->go.object_name;
        msg->pt= s64;
        break;
      case OBJECT_TYPE:
        msg->pv= &Obj->go.object_type;
        msg->pt= et;
        break;
      case DESCRIPTION:
        msg->pv= Obj->go.description;
        msg->pt= s132;
        break;
      case PRESENT_VALUE:
        msg->pv= &Obj->pv;
        switch (Obj->value_type)
          { case 0:
              msg->pt= et;
              break;
            case 1:
              msg->pt= uw;
              break;
            case 2:
              msg->pt= flt;
              break;
          }
        break;
      case EFFECTIVE_PERIOD:
        msg->pv= &Obj->effective_period;
        msg->pt= dtrange;
        break;
      case WEEKLY_SCHEDULE:
        msg->pv= Obj->weekly_schedule;
        msg->pt= wsched;
        break;
      case EXCEPTION_SCHEDULE:
        msg->pv= &Obj->exception_schedule;
        msg->pt= xsched;                    
        break;
      case LIST_OF_OBJECT_PROPERTY_REFERENCES:
        msg->pv= Obj->list_obj_prop_ref;
        msg->pt= lopref;
        break;
      case PRIORITY_FOR_WRITING:
        msg->pv= &Obj->priority_for_writing;
        msg->pt= uw;
        break;
    }    

  return EncodeFromDatabase(Buffer, msg);
}


  