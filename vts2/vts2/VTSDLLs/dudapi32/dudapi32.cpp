/*	09-Jun-98 [002] JN  fix to CheckFunctionalGroup
						fix integer->short
    23-Mar-98 [001] JN  32-bit version (not all marked) 
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

#include "db.h" 
#include "vtsapi.h" 
#include "bacprim.h"

#include "resource.h"   // resource identifiers
#include "dudclass.h" // derived classes 
#include "dudapi.h"   // export interface
#include "dudtool.h"  // tool functions

#include "props.h"                 
#include "propid.h"                  

const char* gThisDLLName= "DUDAPI32.DLL"; // literal name of this dll			***001
static HMODULE hMod= 0; // instance handle of this module						***001

//int		far pascal _WEP(int);					//			***001        
int     GetPropIndex(word object_type, dword PropId);
int     CheckTableProp(generic_object far* pObj, TApplServ far* pApplServ);
int     CheckClass(int n, TApplServ far* pApplServ, char far ApplServ[35], char far Result[35], 
                  generic_object far* root, TObjProp far resObj[64], short far* eol);	//***002


// conformance classes: -------------------------------------------------------------------------------------


#define nCC1 1
TApplServ gCC1_Table[nCC1]= // Conformance Class 1
        { 
             { asReadProperty, ssExecute, DEVICE, -1 }, 
        };  

#define nCC2 1
TApplServ gCC2_Table[nCC2]= // Conformance Class 2
        {
             { asWriteProperty, ssExecute, -1, -1 }
        };

#define nCC3 6
TApplServ gCC3_Table[nCC3]= // Conformance Class 3
        { 
             { asI_Am,                  ssInitiate, -1, -1 },
             { asI_Have,                ssInitiate, -1, -1 },
             { asReadPropertyMultiple,  ssExecute,  -1, -1 },
             { asWritePropertyMultiple, ssExecute,  -1, -1 },
             { asWho_Has,               ssExecute,  -1, -1 },
             { asWho_Is,                ssExecute,  -1, -1 }
        };

#define nCC4 6
TApplServ gCC4_Table[nCC4]= // Conformance Class 4
        { 
             { asAddListElement,        ssInitiate|ssExecute, -1, -1 },
             { asRemoveListElement,     ssInitiate|ssExecute, -1, -1 },
             { asReadProperty,          ssInitiate,           -1, -1 },
             { asReadPropertyMultiple,  ssInitiate,           -1, -1 },
             { asWriteProperty,         ssInitiate,           -1, -1 },
             { asWritePropertyMultiple, ssInitiate,           -1, -1 }
        };

#define nCC5 5
TApplServ gCC5_Table[nCC5]= // Conformance Class 5
        { 
             { asCreateObject,            ssExecute,  -1, -1 },
             { asDeleteObject,            ssExecute,  -1, -1 },
             { asReadPropertyConditional, ssExecute,  -1, -1 },
             { asWho_Has,                 ssInitiate, -1, -1 },
             { asWho_Is,                  ssInitiate, -1, -1 }
        };


// functional groups: ----------------------------------------------------------------------------------------

#define nFgClock 4
TApplServ gFgClock[nFgClock]=
   {
      { asTimeSynchronization, ssExecute, DEVICE, LOCAL_TIME },
      { asTimeSynchronization, ssExecute, DEVICE, LOCAL_DATE },
      { asTimeSynchronization, ssExecute, DEVICE, UTC_OFFSET },
      { asTimeSynchronization, ssExecute, DEVICE, DAYLIGHT_SAVINGS_STATUS }
   };

#define nFgHHWS 6
TApplServ gFgHHWS[nFgHHWS]=
        { 
             { asWho_Is,              ssInitiate|ssExecute, -1, -1 },
             { asI_Am,                ssInitiate,           -1, -1 },
             { asReadProperty,        ssInitiate,           -1, -1 },
             { asWriteProperty,       ssInitiate,           -1, -1 },
             { asReinitializeDevice,  ssInitiate,           -1, -1 },
             { asTimeSynchronization, ssInitiate,           -1, -1 }
        };
        
#define nFgPCWS 15
TApplServ gFgPCWS[nFgPCWS]=
        {
             { asWho_Is,                  ssInitiate|ssExecute, -1, -1 },
             { asI_Am,                    ssInitiate,           -1, -1 },
             { asReadProperty,            ssInitiate,           -1, -1 },
             { asReadPropertyConditional, ssInitiate,           -1, -1 },
             { asReadPropertyMultiple,    ssInitiate,           -1, -1 },
             { asWriteProperty,           ssInitiate,           -1, -1 },
             { asWritePropertyMultiple,   ssInitiate,           -1, -1 },
             { asReinitializeDevice,      ssInitiate,           -1, -1 },
             { asTimeSynchronization,     ssInitiate,           -1, -1 },
             { asCreateObject,            ssInitiate,           -1, -1 },
             { asDeleteObject,            ssInitiate,           -1, -1 },   
             { asAddListElement,          ssInitiate,           -1, -1 },
             { asRemoveListElement,       ssInitiate,           -1, -1 },
             { asAtomicReadFile,          ssInitiate,           -1, -1 },
             { asAtomicWriteFile,         ssInitiate,           -1, -1 }
        };

#define nFgEventInit 4
TApplServ gFgEventInit[nFgEventInit]=
        { 
             { asAcknowledgeAlarm,              ssExecute,  -1, -1 },
             { asConfirmedEventNotification,    ssInitiate, -1, -1 },
             { asGetAlarmSummary,               ssExecute,  -1, -1 },
             { asUnconfirmedEventNotification,  ssInitiate, -1, -1 }
        };

#define nFgEventResponse 2
TApplServ gFgEventResponse[nFgEventResponse]=
        { 
             { asAcknowledgeAlarm,              ssInitiate, -1, -1 },
             { asConfirmedEventNotification,    ssExecute,  -1, -1 },
        };        


#define nFgCOVInit 2
TApplServ gFgCOVInit[nFgCOVInit]=        
        {
             { asSubscribeCOV,             ssExecute,  -1, -1 },
             { asConfirmedCOVNotification, ssInitiate, -1, -1 } 
        };

        
#define nFgCOVResponse 2
TApplServ gFgCOVResponse[nFgCOVResponse]=        
        {
             { asSubscribeCOV,             ssInitiate, -1, -1 },
             { asConfirmedCOVNotification, ssExecute,  -1, -1 } 
        };
        

#define nFgFiles 3
TApplServ gFgFiles[nFgFiles]=
        { 
             { asAtomicReadFile,  ssExecute, -1, -1 },
             { asAtomicWriteFile, ssExecute, -1, -1 },
             { asNN, -1, FILE_O, -1 }
        };


#define nFgReinitialize 1
TApplServ gFgReinitialize[nFgReinitialize]=
        {
             { asReinitializeDevice,      ssExecute,        -1, -1 },
        };

#define nFgVO 3
TApplServ gFgVO[nFgVO]=
        { 
             { asVT_Open,  ssInitiate,            -1, -1 },
             { asVT_Close, ssInitiate|ssExecute,  -1, -1 },
             { asVT_Data,  ssInitiate|ssExecute,  -1, -1 }
             
        };        

#define nFgVT 3
TApplServ gFgVT[nFgVT]=
        { 
             { asVT_Open,             ssExecute,  -1, -1 },
             { asVT_Close, ssInitiate|ssExecute,  -1, -1 },
             { asVT_Data,  ssInitiate|ssExecute,  -1, -1 }
        };        

#define nFgDevCom 1
TApplServ gFgDevCom[nFgDevCom]=
        { 
             { asDeviceCommunicationControl, ssExecute, -1, -1 }
        };        
        
#define nFgTimeMaster 5
TApplServ gFgTimeMaster[nFgTimeMaster]=
  { 
      { asTimeSynchronization, ssExecute, DEVICE, LOCAL_TIME },
      { asTimeSynchronization, ssExecute, DEVICE, LOCAL_DATE },
      { asTimeSynchronization, ssExecute, DEVICE, UTC_OFFSET },
      { asTimeSynchronization, ssExecute, DEVICE, DAYLIGHT_SAVINGS_STATUS },
      { asTimeSynchronization, ssExecute, DEVICE, TIME_SYNCHRONIZATION_RECIPIENTS } 
  };        


// local variables -------------------------------------------------------------------------------------------

static char gBuffer[256];
static int  gInitServer= 0;



// ===========================================================================================================
//
// export functions
//
// ===========================================================================================================
           


// This function is used to paint a standard icon on a window. There are no standard icons
// in Visual Basic. Look up "windows.h" for the ID of a specific standard icon.
//
// in:   x, y   logical position of the icon.
//       hw     Windows handle
//       id     id of standard icon (IDI_ASTERISK, IDI_ ...)
extern "C"
void  APIENTRY DrawStdIcon (int x, int y, HWND hw, int id )
{ 

  HICON icon;
  HDC dc;
  int oldmode;
  
  dc = GetDC(hw);    
  oldmode= GetMapMode(dc);
  SetMapMode(dc, MM_TEXT);
  icon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ASTERISK));
  DrawIcon(dc, x, y, icon);
  SetMapMode(dc,oldmode);
  ReleaseDC(hw, dc);
  
}


// This function is used to copy an generic_object. Need for visual basic.
extern "C"
void APIENTRY CpyGenObj(generic_object far* godst, generic_object far* gosrc)
{ 
  if ((godst==NULL)||(gosrc==NULL)) return;
  memcpy(godst,gosrc,sizeof(generic_object));			//			***001
}
                                          

// This function is used to step through the generic_object list. 
//
// in:   obj   generic_object 
// out:  obj   next generic_object
//
// returns: -1 end of list, else 0
extern "C"
short APIENTRY GetNextGenObj(generic_object far* obj)	//		***002
{ 
  if ((obj==NULL)||(obj->next==NULL)) return(-1);
  memcpy(obj,obj->next,sizeof(generic_object));		//			***001
  return(0);
}
   
   
// This function is used to step through the standard object list. The function retrieves 
// the pointer to a specified object in the object list. Use FindPicsObj() and GetPropValue()
// to read or write values.
//
// in:       root   points to the root of the object list
//           index  zerobased index for the object you want to obtain the pointer
// returns:  a long pointer to the indexed object.
extern "C"
generic_object far* APIENTRY FindGenObj(generic_object far* root, int index)
{ 
  while ((index--) && (root!=NULL)) root= (generic_object far*) root->next;
  return(root);
}

                                          
// This function is used to obtain the name of a Property. 
//
// in:   PropName     points to a buffer to contain the property name
//       objtype      property for this BACnetObjectType
//       i            zero-based index in property table
//       propFlags    bitstream to determine if property is supported/not supported   
// out:  PropId       property ID
//
// returns: >0 if property supported, -1 if invalid property index, 0 if not supported 
extern "C"
short APIENTRY GetPropNameSupported(char far* PropName, word i, word objtype, octet far propFlags[64], dword far* PropId) //***002
{ 
  if ( (propFlags[i] & 1) == 1 )
    {                                   
      *PropId= VTSAPIgetpropinfo(objtype,i,PropName,NULL,NULL,NULL,NULL);
      if (*PropId == -1) return(-1); // invalid property index
      else return( strlen(PropName) );			//						***001
    }  
  else return(0); // not supported
}



// This function is used to find a generic_object identified by its object id.
// in:		root		root of object list
//			ObjectId	object being sought
// returns: pointer to matching object, or NULL
extern "C"
generic_object far* APIENTRY GetpObj(generic_object far* root, dword ObjectId)  
{ 
  while (root!=NULL)
    { 
      if (root->object_id == ObjectId) // gotcha!
        break;
      root= (generic_object far*) root->next;
    }
  return root;
}

   
// This function is used to retrieve the property value of an object.
//
// in:
//       Buffer     points to a buffer to contain the string value of the Property
//       msg		PVMessage
//
// returns: 0 if ok, else !=0
extern "C"
short APIENTRY GetPropValue(char far* Buffer, PVMessage far* msg)	//***002
{ 
  generic_object far* Obj= msg->Obj;
  int value; 
  if (Obj==NULL) return(-1); // no generic_object
  
  switch( (enum BACnetObjectType) Obj->object_type)  
    { 
      case ANALOG_INPUT:
          value= ai(Buffer,msg);
        break;
      case ANALOG_OUTPUT:
          value= ao(Buffer,msg);
        break;
      case ANALOG_VALUE:
          value= av(Buffer,msg);
        break;   
      case BINARY_INPUT:
          value= bi(Buffer,msg);
        break;   
      case BINARY_OUTPUT:
          value= bo(Buffer,msg);
        break;   
      case BINARY_VALUE:
          value= bv(Buffer,msg);
        break;   
      case CALENDAR:
          value= cal(Buffer,msg);
        break;   
      case COMMAND:
          value= com(Buffer,msg);
        break;   
      case DEVICE:
          value= dev(Buffer,msg);
        break;   
      case EVENT_ENROLLMENT:
          value= eve(Buffer,msg);
        break;   
      case FILE_O:
          value= flo(Buffer,msg);
        break;   
      case GROUP:
          value= gru(Buffer,msg);
        break;   
      case LOOP:
          value= loo(Buffer,msg);
        break;   
      case MULTI_STATE_INPUT:
          value= msi(Buffer,msg);
        break;       
      case MULTI_STATE_OUTPUT:
          value= mso(Buffer,msg);
        break;   
      case NOTIFICATIONCLASS:
          value= nofy(Buffer,msg);
        break;   
      case PROGRAM:
          value= prg(Buffer,msg);
        break;   
      case SCHEDULE:
          value= sced(Buffer,msg);
        break;   
    }   
  return (short)value;						//						***002
}
  

// This Function is used to obtain the literal name of a BACnetObjectType.
// in:    objtype   enumerated BACnetObjectType
//        Buffer    points to a buffer to contain name
// returns: >0 if ok, else 0
extern "C"
short APIENTRY GetObjType(word objtype, char far* Buffer)	//			***002
{ word pet;
  VTSAPIgetpropinfo(objtype,2,Buffer,NULL,NULL,NULL,&pet); // 2 == ObjTypeProperty
  VTSAPIgetenumtable(pet,objtype,NULL,NULL,Buffer);
  return( strlen(Buffer) );									//			***001
}                                                         


// This function is used to obtain the name of an enumerated property value.
// in:    Name     buffer to contain name of enumeration
//        objtype  property is in this object
//        PropId   ID of the property as defined in propid.h
//        EnumVal  enumerated value for which you want to retrieve name
// returns: >0 if ok, 0 else
extern "C"
short APIENTRY GetEnumName(char far* Name, word objtype, dword PropId, word EnumVal) //***002
{ word pet,i; dword pid, pcount; 

  pcount= VTSAPIgetpropinfo(objtype,0xFFFF,Name,NULL,NULL,NULL,&pet); // how many properties?

  if (pcount==0xFFFFFFFF) { Name[0]= 0; return(0); } // objtype invalid
  i= (word)pcount;
  while ( (--i) >=0 ) // iterate all properties
    { pid= VTSAPIgetpropinfo(objtype,i,Name,NULL,NULL,NULL,&pet);
      if (pid==PropId) break; // check if PropId was found
    }
  if (i<0) return(0); // PropId not found 
  Name[0]= 0; // reset buffer
  i= VTSAPIgetenumtable(pet,EnumVal,NULL,NULL,Name); // get enumeration name
  
  return ( strlen(Name) );							//			***001
}                                                   



// This function is used to obtain the name of a property PropID.
// in:    PropId   ID of the property as defined in propid.h
//        PropName buffer to contain name of property
// returns: >0 if ok, 0 else
extern "C"
short APIENTRY GetPropName(dword PropId, char far* PropName)	//	***002
{ 
  PropName[0]= 0;
  if ((PropId>=0)&&(PropId<stPropIDs.nes)) 
    { strcpy(PropName,stPropIDs.estrings[PropId]);		//			***001
      return(strlen(PropName));							//			***001
    }
  else return(0);  
}                                                   



// This function is used to read a string resource from the resource file. The Function 
// uses the literal name "DUDAPI32.DLL" to retrieve the instance handle of this module.
//                                                                                   
// in:		id			ID of the string resource
//    		Buffer		to contain the string resource
//			Size		size of the buffer >0
extern "C"
short APIENTRY LoadStringRes(int id, char far* Buffer, int Size)	//	***002
/*{																		***001 Begin
  TRY
    { 
      CString s; int slen, max;
	  s.LoadString(id);
      slen= s.GetLength();
      (Size>slen)?(max= slen):(max= Size);
      strncpy(Buffer, (const char*)s , max);				//			***001
      Buffer[max]= 0;
      return(max); 
    }
  CATCH_ALL(e)
    {
      // a failure caused an exception.
      Buffer[0]= 0;
      return 0;
    }
  END_CATCH_ALL 
                       
}*/
{ short c;													//			***002
  if (hMod==0) // first call to function
    hMod= GetModuleHandle(gThisDLLName);  // retrieve instance handle
  c= (short)LoadString(hMod,id,Buffer,Size);     // load string into buffer ***002
  return(c);
}											//							***001 End

// This function is used to get the property index of a property.        
int GetPropIndex(word object_type, dword PropId)
{ dword c; dword id;
  c= VTSAPIgetpropinfo(object_type,0xFFFF,NULL,NULL,NULL,NULL,NULL);
  for (int i=0; (dword)i<c; i++)
    { id= VTSAPIgetpropinfo(object_type,i,gBuffer,NULL,NULL,NULL,NULL);
      if (id==PropId) return(i);
    }
  return -1;
}        
 
           
           
// This function is used to check a conformance class or a functional group.
// in:		n				number of lines in table
//			pApplServ		Table containing class requirements
//			ApplServ		appl services supported by the given device
//		    root			object list (objects supported)
// out:     Result  		appl services missing to complete the class
//			resObj  		objects and properties missing (int ObjType, int PropIndex...)
int CheckClass(int n, TApplServ far* pApplServ, char far ApplServ[35], char far Result[35], 
                           generic_object far* root, TObjProp far resObj[64], short far* eol)
{
  int line; enum BACnetApplServ serv; 
  int allChecked, obj_supported;
  char InitExec;

  int rval= 1; // default: class supported

  for (line= 0; line<n; line++) // for all lines in table:
    { 
      serv= pApplServ[line].ApplServ; // required application service 0..34
      if (serv>=0)
        { InitExec= pApplServ[line].InitExec;
          if ( (ApplServ[serv] & InitExec) != pApplServ[line].InitExec )
            { Result[serv]= Result[serv] | ( (InitExec ^ ApplServ[serv]) & InitExec ); 
              rval= 0;
            } 
        }
    } // application services checked!           
      
  int iProp;       // property index
  generic_object far* pObj= root;
  while (pObj!=NULL) // step through object list, check required objects/properties
    { 
      obj_supported= 1;
      for (line= 0; line<n; line++)
	  {
          if (pApplServ[line].object_type == (word)pObj->object_type) // check this object:
            { 
              if (pApplServ[line].property_id!=-0xFFFF) // property required	***002
                {
                  iProp= GetPropIndex(pObj->object_type,pApplServ[line].property_id);
                  if ((pObj->propflags[iProp] & 1)==1) // property supported
                    pApplServ[line].object_type= 0xFFFE;  // temporary mark		***002
                  else
                    obj_supported= 0; // not supported  
                }
              else // object supported
                pApplServ[line].object_type= 0xFFFE; // temporary mark			***002
            }
        }
      
      allChecked= 1;  
      for (line= 0; line<n; line++)   
        { 
          if (pApplServ[line].object_type==0xFFFE)					//			***002
            if (obj_supported) pApplServ[line].object_type= 0xFFFF; // obj supports requirement!***002
            else pApplServ[line].object_type= pObj->object_type;	// write back object type
            
          if (pApplServ[line].object_type!=0xFFFF) allChecked= 0;	//			***002
        }
      if (allChecked) break; // all lines in table checked 
      pObj= (generic_object far*)pObj->next;
      
    } // while        
  
  short iObjProp= *eol; // index in result array					//			***002
    
  for (line= 0; line<n; line++) // collect all lines not supported:
    {
      if (pApplServ[line].object_type!=0xFFFF) // not supported		//			***002
        { iObjProp++;
          if (iObjProp>63) break;
          resObj[iObjProp].object_type= pApplServ[line].object_type;
          resObj[iObjProp].property_id= pApplServ[line].property_id;
        }
    }
      
  *eol= iObjProp;
             
  if (iObjProp>0) rval= 0; // class not supported
  return(rval);  
    
}          



// This Function is used to check, whether a BACnet device supports all required application services
// in order to belong to a particular conformance class.  
// in:		ConfClass		conformance class (1..5)
//			ApplServ		appl services supported by the given device
//		    root			object list supported
// out:     Result  		appl services missing to complete the conformance class
//			resObj  		objects and properties missing (int ObjType, int PropIndex...)
// retunrs  1 if supported, 0 else			
extern "C"
short APIENTRY CheckConfClass(word ConfClass, char far ApplServ[35], char far Result[35], 
                                   generic_object far* root, TObjProp far resObj[64], short far* eol)
{  short rval= 1; // default: ConfClass supported					***002
  
  memset(Result, 0, 35);						//					***001
  memset(resObj,0,64*sizeof(TObjProp));			//					***001
  *eol= -1;
  
  switch (ConfClass)
    { 
      case 6:  
        //if ( CheckClass(nFgClock,gFgClock,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        //if ( CheckClass(nFgPCWS,gFgPCWS,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        //if ( CheckClass(nFgEventInit,gFgEventInit,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        //if ( CheckClass(nFgEventResponse,gFgEventResponse,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        //if ( CheckClass(nFgFiles,gFgFiles,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        break;
      case 5:  
        if ( CheckClass(nCC5,gCC5_Table,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        break;
      case 4:
        if ( CheckClass(nCC4,gCC4_Table,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        break;
      case 3:
        if ( CheckClass(nCC3,gCC3_Table,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        break;
      case 2:
        if ( CheckClass(nCC2,gCC2_Table,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        break;
      case 1:
      default:
        if ( CheckClass(nCC1,gCC1_Table,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
        break;
    }    
    
  return(rval);  
}
  

extern "C"
short APIENTRY CheckFunctionalGroup(dword FuncGroup, char far ApplServ[35], char far Result[35], 
                                          generic_object far* root, TObjProp far resObj[64], short far* eol)
{ 
   short rval= 1; // default: ConfClass supported					***002
  
  memset(Result, 0, 35);						//					***001
  memset(resObj,0,64*sizeof(TObjProp));			//					***001
  *eol= -1;
  
  switch (FuncGroup)
    { 
   		case fgHHWS:				
          if ( CheckClass(nFgHHWS,gFgHHWS,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgPCWS:				
          if ( CheckClass(nFgPCWS,gFgPCWS,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgCOVEventInitiation:	
          if ( CheckClass(nFgCOVInit,gFgCOVInit,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgCOVEventResponse:	
          if ( CheckClass(nFgCOVResponse,gFgCOVResponse,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgEventInitiation:		
          if ( CheckClass(nFgEventInit,gFgEventInit,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgEventResponse:		
          if ( CheckClass(nFgEventResponse,gFgEventResponse,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgClock:				
          if ( CheckClass(nFgClock,gFgClock,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgDeviceCommunications:
          if ( CheckClass(nFgDevCom,gFgDevCom,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgFiles:		
          if ( CheckClass(nFgFiles,gFgFiles,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgTimeMaster:	
          if ( CheckClass(nFgTimeMaster,gFgTimeMaster,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgVirtualOPI:	
          if ( CheckClass(nFgVO,gFgVO,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgReinitialize:
          if ( CheckClass(nFgReinitialize,gFgReinitialize,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
   		case fgVirtualTerminal:
          if ( CheckClass(nFgVT,gFgVT,ApplServ,Result,root,resObj,eol)== 0 ) rval= 0;
   		  break;
    }
    
  return (rval);  
}



extern "C"
short APIENTRY DevApplServCheck(char far ApplServ[35], generic_object far* root, 
              					char far resApplServ[35])					//***002
{ device_obj_type far* pdev= NULL; 
  while (root!=NULL)
    { 
      if (root->object_type==DEVICE)
        { pdev= (device_obj_type far*)root;
          break;
        } 
      root= (generic_object far*)root->next;  
    }
  if (pdev==NULL) return(0);  
  
  int iAppl=  0x80;
  int iOctet= 0;
  short rval=   1;						//							***002
  memset(resApplServ, 0, 35);			//							***001

  for (int i= 0; i<35; i++)
    { 
      if (  ((ApplServ[i] & ssExecute) == ssExecute) &&
            ( (pdev->protocol_services_supported[iOctet] & iAppl) != iAppl ) // not supported!
         )   
        {
          resApplServ[i]= ssExecute | 0x04; // missing in protocol_services_supported
          rval= 0;
        }
      
      if (  ((pdev->protocol_services_supported[iOctet] & iAppl) == iAppl) &&
            ( (ApplServ[i] & ssExecute) != ssExecute ) // not supported!
         )
        {
          resApplServ[i]= ssExecute | 0x08;  // missing in application services supported (PICS)
          rval= 0;
        }
        
      iAppl/= 2;
      if (iAppl==0) { iAppl= 0x80; iOctet++; }
    }         
    
  return rval;  
  
}              													 


extern "C"
BACnetObjectIdentifier far* APIENTRY GetObjIdRoot(generic_object far* pdbRoot)
{ // find the device object
  device_obj_type far* pdev= NULL; 
  while (pdbRoot!=NULL)
    { if (pdbRoot->object_type==DEVICE)
        { pdev= (device_obj_type far*)pdbRoot;
          break;
        } 
      pdbRoot= (generic_object far*)pdbRoot->next;  
    }
  if (pdev==NULL) return(NULL);  
  return(pdev->object_list); // list of object IDs
 }


// This function is used to check, if an element of object_list 
// (BACnetObjectIdentifier far*) is present in the PICS Database. Afterwards,
// the pointer to the element of object_list will point to the next element in 
// object_list.
// in:			pdbRoot		root of PICS Database
//			pid		pointer to a long value. This long value is the 
//					  address of the object_list element to be checked.
// out:			pid		address of next element in list (or NULL= end of 
//					  list)
//			ObjId		object id of element 
// returns:	1 if element is in PICS Database, 0 else.
extern "C"
short APIENTRY pIDinList(generic_object far* pdbRoot, long far* pid, dword far* ObjId)	//***002
{ 
  BACnetObjectIdentifier far* p= (BACnetObjectIdentifier far*) *pid;
  if (p==NULL) { *ObjId= 0; return(0); }
  *ObjId= p->object_id;
  while (pdbRoot!=NULL)
    { if (pdbRoot->object_id==p->object_id) break;
      pdbRoot= (generic_object far*)pdbRoot->next;
    }

 *pid= (long)p->next; // next pid
 if (pdbRoot!=NULL) 
   return(1);         // pid in pdb list
 else 
   return(0);         // pid not in pdb list  
}


// This function is used to check, if a particular object of the PICS Database is present 
// in the list of property Object_List of the Device object.
// in:			pidRoot		points to Object_List
//			pdb		points to a long value. This long value contains the 
//					  address of the object to be checked.
// out:			pdb		address of next object
//			ObjId		object id of object
// returns:	1 if object is in Object_List, 0 else
extern "C"
short APIENTRY pDBinList(BACnetObjectIdentifier far* pidRoot, long far* pdb, dword far* ObjId) //***002
{ 
  generic_object far* p= (generic_object far*) *pdb;
  if (p==NULL) { *ObjId= 0; return(0); }
  *ObjId= p->object_id;
  while (pidRoot!=NULL)
    { if (pidRoot->object_id==p->object_id) break;
      pidRoot= (BACnetObjectIdentifier far*)pidRoot->next;
    }
  *pdb= (long)p->next; // next pdb 
  if (pidRoot!=NULL) 
    return(1);         // pid in pdb list
  else 
    return(0);         // pid not in pdb list  
}                                                                      


// This function is used to check the object types supported in:
//   the PICS "object types supported"
//   property Object_Types_Supported of the Device object
// in:		StdObj			array 1..18 indicating supported object types
//			pdbRoot			root of PICS Database
// out:		resObjDev[18]	missing object types in Device object (indicated by 1)
//			resObjPICS[18]	missing object types in PICS (indicated by 1)
// returns: 0 if at least one object type is missing, 1 else
extern "C"
short APIENTRY CheckObjTypeDevPics(char far* StdObj, generic_object far* pdbRoot,
                                          octet far resObjDev[18], octet far resObjPICS[18])	//***002
{ // find the device object
  device_obj_type far* pdev= NULL; 
  while (pdbRoot!=NULL)
    { if (pdbRoot->object_type==DEVICE)
        { pdev= (device_obj_type far*)pdbRoot;
          break;
        } 
      pdbRoot= (generic_object far*)pdbRoot->next;  
    }
  if (pdev==NULL) return(0);  
  
  int iOctet= 0;
  int iBit= 0x80;
  short rval= 1; // default: no object type missing					***002

  memset(resObjDev, 0, 18);				//							***001
  memset(resObjPICS, 0, 18);			//							***001
  
  for (int i=0; i<18; i++)
    { 
      if ( (StdObj[i]>0) && ((pdev->object_types_supported[iOctet] & iBit) == 0) )     // not in Device object
        { resObjDev[i]= 1; rval= 0; }
      if ( (StdObj[i]==0) && ((pdev->object_types_supported[iOctet] & iBit) == iBit) ) // not in PICS section
        { resObjPICS[i]= 1; rval= 0; }
      
      iBit/= 2;
      if (iBit==0) { iOctet++; iBit= 0x80; }
    }          
    
  return(rval);  
}                                          


// This function is used to extract object type and object instance from an object identifier.
// in: 		ObjId		object identifier
// out:		ObjType		buffer to contain object type
//			ObjInst		buffer to contain object instance
extern "C"
void APIENTRY SplitObjectId(dword ObjId, word far* ObjType, dword far* ObjInst)
{
  if (ObjType!=NULL) *ObjType= (word) (ObjId>>22); 
  if (ObjInst!=NULL) *ObjInst= ObjId & 0x3FFFFF;
}


// This function is used to convert 2 octets to an integer value. The first
// octet is the least significant byte of the integer value.
// in:		sInt		octet string (low byte, high byte)
// returns	integer value	
extern "C"
short APIENTRY CMyInt(unsigned char far* sInt)						//***002
{
  return ( sInt[0] + sInt[1]*256 );  // so easy! (you can't do this in VB)
}


// This function is used to convert 4 octets to an long value. The first
// octet is the most significant byte of the long value.
// in:		sLong		octet string 
// returns	long value	
extern "C"
long APIENTRY CMyBigLong(unsigned char far* sLong)
{ 
  long f=256; long l;
  
  l=  sLong[3]; 
  l+= sLong[2]* f; f= f*256;
  l+= sLong[1]* f; f= f*256;
  l+= sLong[0]* f; 
  
  return (l);
}


// This function is used to convert 2 octets to an integer value. The first
// octet is the most significant byte of the integer value.
// in:		sInt		octet string (high byte, low byte)
// returns	integer value	
extern "C"
short APIENTRY CMyBigInt(unsigned char far* sInt)
{ 
  return ( sInt[1] + sInt[0]*256 );  // so easy! (you can't do this in VB)
}


// This function is used to remove the complete database.
// in:		root		root of object list.
extern "C"
void APIENTRY MyDeletePICSObject(generic_object far* root)
{ generic_object far* p;
  while (root!=NULL)
	{ p= root;
	  root= (generic_object far*)root->next;
	  DeletePICSObject(p);
	}
}


// Every client of dudapi32.dll has to call this function first, in order to create
// test values (dynamic allocated memeory). The dynamic data will be created only
// once (first client)
extern "C"
void APIENTRY InitDudapi(void)
{
  if (gInitServer==0) 
    {
      CreateTestValues();
    }  

  gInitServer++;

}


// Every client of dudapi32.dll has to call this function, in order to release
// dynamic data. The data will be released only once (last client).
extern "C"
void APIENTRY CloseDudapi(void)
{
  if (gInitServer>0) gInitServer--;
  
  if (gInitServer==0) // no active clients
    { 
      DeleteTestValues();
    }  
}
								
