#include <windows.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "c:\dudas\code\vtsapi.h" 
#include "c:\dudas\code\props.h"                 
#include "c:\dudas\code\propid.h"                  
#include "c:\dudas\code\db.h"
#include "c:\dudas\code\dudapi.h"

                                           
// local constants ===========================================================================================

const char* gThisDLLName= "DUDAPI.DLL"; // literal name of this dll

// Param for GetPropValue()

#define SETVAL			0 // Set property value
#define READVAL			1 // Read property value
#define CREATEMSG		2 // Create message


// local variables ===========================================================================================

static HMODULE hMod= 0; // instance handle of this module

static char* gMonthTable[] = 
        { 
          "january",   "february", "march",    "april",    
          "may",       "june",     "july",     "august",
          "september", "october",  "november", "december"
        };  


// local functions declaration ===============================================================================

int  ai(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  ao(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  av(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  bi(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  bo(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  bv(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  cal(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  com(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  dev(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  eve(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  flo(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  gru(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  loo(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  msi(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  mso(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  nofy(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  prg(generic_object far* Obj,dword id,char far* Buffer,long Param);
int  sced(generic_object far* Obj,dword id,char far* Buffer,long Param);

void	ReadBitStream(char far* Buffer, int Flag, int c);
void	ReadOctetBitStream(char far* Buffer, octet Flag, int c);
void	ReadFltStream(char far* Buffer, float fval[], int c);
void	ReadIntStream(char far* Buffer, int dec[], int c);
void	CvtFltStr(double fval, char far* Buffer);
double	CvtStrFlt(char far* Buffer);
void    ReadBACnetDate(char far* Buffer, BACnetDate far* date);
void    ReadBACnetTime(char far* Buffer, BACnetTime far* dtime);
void    ReadBACnetDateRange(char far* Buffer, BACnetDateRange far* range);
void    ReadBACnetWeekNDay(char far* Buffer, BACnetWeekNDay far* weekNday);
void	ReadBACnetDateTime(char far* Buffer, BACnetDateTime far* dtime);
void	ReadBACnetStream(char far* Buffer, int object_type, dword PropId, int pvstr[],int c);
void    SetBACnetDateTime(BACnetDateTime far* dtime, char far* Buffer);

int		far pascal _WEP(int);         
  
  
// export functions ==========================================================================================
           

// This function is used to paint a standard icon on a window. There are no standard icons
// in Visual Basic. Look up "windows.h" for the ID of a specific standard icon.
//
// in:   x, y   logical position of the icon.
//       hw     Windows handle
//       id     id of standard icon (IDI_ASTERISK, IDI_ ...)
void  far pascal __export DrawStdIcon (int x, int y, HWND hw, int id )
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
void far pascal __export CpyGenObj(generic_object far* godst, generic_object far* gosrc)
{ 
  if ((godst==NULL)||(gosrc==NULL)) return;
  _fmemcpy(godst,gosrc,sizeof(generic_object));       
}
                                          

// This function is used to step through the generic_object list. 
//
// in:   obj   generic_object 
// out:  obj   next generic_object
//
// returns: -1 end of list, else 0
int far pascal __export GetNextGenObj(generic_object far* obj)
{ 
  if ((obj==NULL)||(obj->next==NULL)) return(-1);
  _fmemcpy(obj,obj->next,sizeof(generic_object));
  return(0);
}
   
   
// This function is used to step through the standard object list. The function retrieves 
// the pointer to a specified object in the object list. Use FindPicsObj() and GetPropValue()
// to read or write values.
//
// in:       root   points to the root of the object list
//           index  zerobased index for the object you want to obtain the pointer
// returns:  a long pointer to the indexed object.
generic_object far* far pascal __export FindPicsObj(generic_object far* root, int index)
{ 
  while ((index--) && (root!=NULL)) root= root->next;
  return(root);
}

                                          
// This function is used to obtain the name of a Property. 
//
// in:   PropName     points to a buffer to contain the property name
//       objtype      Property for this BACnetObjectType
//       i            zero-based index in property table
//       std_props    bitstream to determine if property is supported/not supported
//
// returns: the PropId (supported), else -1 
dword  far pascal __export GetPropName(char far* PropName, word i, word objtype, octet far std_props[8])
{ octet index= (octet)i;
  dword PropId;

  if ( std_props[index/8] & (1<<(index%8)) )
    {
      PropId= VTSAPIgetpropinfo(objtype,index,PropName,NULL,NULL,NULL,NULL);
      return(PropId);
    }  
  else return((dword)-1); // not supported
}
  
   
   
// This function is used to retrieve the proprty value of an object.
//
// in:
//       Obj        pointer to a specific generic object
//       PropId     Property ID as defined in propid.h
//       Buffer     points to a buffer to contain the string value of the Property
//       Param      1==read, 0==write
//
// returns: 1 if ok, else 0
int  far pascal __export GetPropValue(generic_object far* Obj, dword PropId, char far* Buffer, long Param)
{ dword value; 
  switch( (enum BACnetObjectType) Obj->object_type)  
    { 
      case ANALOG_INPUT:
          value= ai(Obj,PropId,Buffer,Param);
        break;
      case ANALOG_OUTPUT:
          value= ao(Obj,PropId,Buffer,Param);
        break;                                           
      case ANALOG_VALUE:
          value= av(Obj,PropId,Buffer,Param);
        break;   
      case BINARY_INPUT:
          value= bi(Obj,PropId,Buffer,Param);
        break;   
      case BINARY_OUTPUT:
          value= bo(Obj,PropId,Buffer,Param);
        break;   
      case BINARY_VALUE:
          value= bv(Obj,PropId,Buffer,Param);
        break;   
      case CALENDAR:
          value= cal(Obj,PropId,Buffer,Param);
        break;   
      case COMMAND:
          value= com(Obj,PropId,Buffer,Param);
        break;   
      case DEVICE:
          value= dev(Obj,PropId,Buffer,Param);
        break;   
      case EVENT_ENROLLMENT:
          value= eve(Obj,PropId,Buffer,Param);
        break;   
      case FILE_O:
          value= flo(Obj,PropId,Buffer,Param);
        break;   
      case GROUP:
          value= gru(Obj,PropId,Buffer,Param);
        break;   
      case LOOP:
          value= loo(Obj,PropId,Buffer,Param);
        break;   
      case MULTI_STATE_INPUT:
          value= msi(Obj,PropId,Buffer,Param);
        break;   
      case MULTI_STATE_OUTPUT:
          value= mso(Obj,PropId,Buffer,Param);
        break;   
      case NOTIFICATIONCLASS:
          value= nofy(Obj,PropId,Buffer,Param);
        break;   
      case PROGRAM:
          value= prg(Obj,PropId,Buffer,Param);
        break;   
      case SCHEDULE:
          value= sced(Obj,PropId,Buffer,Param);
        break;   
    }     
  if ((long)value !=-1) return(1);  
  else return 0;  
}
  

// This Function is used to obtain the literal name of a BACnetObjectType.
// in:    objtype   enumerated BACnetObjectType
//        Buffer    points to a buffer to contain name
// returns: 1 if ok, else 0
int   far pascal __export GetObjType(word objtype, char far* Buffer)
{ word pet;
  VTSAPIgetpropinfo(objtype,2,Buffer,NULL,NULL,NULL,&pet); // 2 == ObjTypeProperty
  VTSAPIgetenumtable(pet,objtype,NULL,NULL,Buffer);
  return(1);
}


// This function is used to obtain the name of an enumerated property value.
// in:    Name     buffer to contain name of enumeration
//        objtype  property is in this object
//        PropId   ID of the property as defined in propid.h
//        EnumVal  enumerated value for which you want to retrieve name
// returns: 1 if ok, 0 else
int far pascal __export GetEnumName(char far* Name, word objtype, dword PropId, word EnumVal)
{ word pet,i; dword pid, pcount; 

  pcount= VTSAPIgetpropinfo(objtype,0xFFFF,Name,NULL,NULL,NULL,&pet); // how many properties?

  if (pcount==0xFFFFFFFF) { Name[0]= 0; return(0); } // objtype invalid
  i= (word)pcount;
  while ( (--i) >=0 ) // iterate all properties
    { pid= VTSAPIgetpropinfo(objtype,i,Name,NULL,NULL,NULL,&pet);
      if (pid==PropId) break; // check if PropId was found
    }
  if (i<0) return(0); // PropId not found 
  Name[0]= 0; // clear buffer
  i= VTSAPIgetenumtable(pet,EnumVal,NULL,NULL,Name); // get enumeration name
  return (i!=0); // pcount!= 0  success
}                                                   



// This function is used to read a string resource from the resource file. The Function 
// uses the literal name "DUDAPI.DLL" to retrieve the instance handle of this module.
//                                                                                   
// in:		id			ID of the string resource
//    		Buffer		to contain the string resource
//			Size		size of the buffer
int  far pascal __export LoadStringRes(int id, char far* Buffer, int Size)
{ int c;
  if (hMod==0) // first call to function
    hMod= GetModuleHandle(gThisDLLName);  // retrieve instance handle
  c= LoadString(hMod,id,Buffer,Size);     // load string into buffer
  return(c);
}

                                     
                                     
                                     
                   
// ===========================================================================================================
//
// local functions
//
// ===========================================================================================================

  
  
// This function is used to convert an array of float values and to store them into a buffer.
// in:    Buffer    points to a buffer to contain all values
//        flt       array of float values
//        c         number of elements in array
void ReadFltStream(char far* Buffer, float fval[], int c)
{ int i; char far* p;
  p= Buffer;
  for (i=0; i<c; i++) 
    { CvtFltStr(fval[i],p);
      p= p + strlen(p);
      *(p++)= ';';                                  
      *(p++)= ' ';
    }
}


// This function is used to convert an array of int values and to store them into a buffer.
// in:    Buffer    points to a buffer to contain all values
//        flt       array of float values
//        c         number of elements in array
void ReadIntStream(char far* Buffer, int dec[], int c)
{ int i; char far* p;
  p= Buffer;
  for (i=0; i<c; i++) 
    { _itoa(dec[i],p,10);
      p= p + strlen(p);
      *(p++)= ';'; // seperator
      *(p++)= ' '; // seperator
    }
}


// This function is used to convert an octet value to a bitstream, and to store the bits 
// into a string buffer.
// in:	Buffer		points to a buffer to contain all bits, eg. "FTTF", ...
//		Flag		octet bitstream: 0110 0000 == (f,t,t,f, f,f,f,f)
//		c			number of bits you want to be converted (from the left)
void ReadOctetBitStream(char far* Buffer, octet Flag, int count)
{ int i; octet mask= 0x80;
  for (i=0; i<count; i++)
    { if (Flag & mask) Buffer[i]= 'T';
      else Buffer[i]= 'F';
      Flag= Flag << 1;
    }
  Buffer[count]= 0;
}

// This function is used to convert an int value to a bitstream, and to store the bits 
// into a string buffer.
// in:	Buffer		points to a buffer to contain all bits, eg. "FTTF", ...
//		Flag		int bitstream: 0110 0000 0000 0000 == (f,t,t,f, f,f,f,f, f,f,f,f, f,f,f,f)
//		c			number of bits you want to be converted (from the left)
void ReadBitStream(char far* Buffer, int Flag, int count)
{ int i; int mask= 0x8000;
  for (i= 0; i<count; i++)
    { 
      if (Flag & mask) Buffer[i]= 'T';
      else Buffer[i]= 'F';
      Flag= Flag << 1;
    }
  Buffer[count]= 0;
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


// This function is used to convert a string to a BACnetDateTime.
// in:		dtime		value to be converted
//			Buffer		points to a buffer to contain string representation
void SetBACnetDateTime(BACnetDateTime far* dtime, char far* Buffer)
{ BACnetDateTime defdtime;
  memset(&defdtime,dontcare,sizeof(BACnetDateTime));
  *dtime= defdtime;
}
        
        
void ReadBACnetDate(char far* Buffer, BACnetDate far* date)
{ char ch[5];
  if (date->month!=dontcare)
    { _itoa(date->month,ch,10);
      strcpy(Buffer,ch); strcat(Buffer,"/");
    } else strcpy(Buffer,"?/")  ;
  if (date->day_of_month!=dontcare)  
    { _itoa(date->day_of_month,ch,10);
      strcat(Buffer,ch); strcat(Buffer,"/");
    } else strcat(Buffer,"?/")  ;
  if (date->year!=dontcare)  
    { _itoa(date->year,ch,10);
      strcat(Buffer,ch); 
    } else strcat(Buffer,"?")  ;
}


void ReadBACnetTime(char far* Buffer, BACnetTime far* dtime)
{ char ch[5];
  
  if (dtime->hour!=dontcare)
    { _itoa(dtime->hour,ch,10);
      strcpy(Buffer,ch); strcat(Buffer,":");
    } else strcpy(Buffer,"?:")  ;
  if (dtime->minute!=dontcare)  
    { _itoa(dtime->minute,ch,10);
      strcat(Buffer,ch); strcat(Buffer,":");
    } else strcat(Buffer,"?:")  ;
  if (dtime->second!=dontcare)  
    { _itoa(dtime->second,ch,10);
      strcat(Buffer,ch); strcat(Buffer,".");
    } else strcat(Buffer,"?.")  ;
  if (dtime->hundredths!=dontcare)  
    { _itoa(dtime->hundredths,ch,10);
      strcat(Buffer,ch);
    } else strcat(Buffer,"?")  ;
}



void ReadBACnetWeekNDay(char far* Buffer, BACnetWeekNDay far* weekNday)
{ 
  
}


void ReadBACnetDateRange(char far* Buffer, BACnetDateRange far* range) 
{
  ReadBACnetDate(Buffer, &(range->start_date)); 
  strcat(Buffer," - "); 
  Buffer= Buffer + strlen(Buffer);
  ReadBACnetDate(Buffer, &(range->end_date)); 
}


// This function is used to convert a BACnetDateTime into a string representation.
// in:		Buffer		points to a buffer to contain string representation
//			dtime		value to be converted
void ReadBACnetDateTime(char far* Buffer, BACnetDateTime far* dtime)
{
  ReadBACnetTime(Buffer, &(dtime->time) ); strcat(Buffer,";");
  Buffer= Buffer + strlen(Buffer);
  ReadBACnetDate(Buffer, &(dtime->date) );
}


// This function is used to convert BACnetCalendarEntry values into a string representation.
// in:		Buffer		buffer to contain the list of date strings
//			datelist	list of BACnetCalendarEntry dates.
void ReadBACnetCalendarEntry(char far* Buffer,BACnetCalendarEntry far* datelist)
{ BACnetCalendarEntry far* p= datelist;

  while (p)
  { switch (p->choice)
      { 
        case 0: // p->u.date
          ReadBACnetDate(Buffer,&(p->u.date));
          break;
        case 1: // p->u.date_range
          ReadBACnetDateRange(Buffer,&(p->u.date_range));
          break;
        case 2: // p->u.weekNday
          ReadBACnetWeekNDay(Buffer,&(p->u.weekNday));
          break;
      }
    strcat(Buffer,"; ");
    Buffer= Buffer + strlen(Buffer);
    p= p->next;
  }
}


// This function is used to obtain the names of enumerated property values. The names will be
// seperated with a "; "
// in:    Buffer		buffer to contain the names of the enumerated values
//        obj_type		the property is in the object of this type
//        PropId		ID of the property as defined in propid.h
//        EnumArray		enumerated values for which you want to retrieve the names
//	      c				size of EnumArray
void ReadBACnetStream(char far* Buffer, int object_type, dword PropId, int EnumArray[],int c)
{ int i; char far* p;
  p= Buffer;
  for (i=0; i<c; i++) 
    { 
      GetEnumName(p,object_type,PropId,EnumArray[i]);
      p= p + strlen(p);
      *(p++)= ';'; // seperator
      *(p++)= ' '; // seperator
    }
  *p= 0;  
}




// ===========================================================================================================
//
// BACnetStandardObject Functions:
//
// ===========================================================================================================

        
        
int ai(generic_object far* Object,dword id,char far* Buffer,long Param) 
{ 
  ai_obj_type far* Obj= (ai_obj_type far*)Object;  
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,OBJECT_TYPE,Obj->go.object_type);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) CvtFltStr(Obj->pv,Buffer);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case DEVICE_TYPE:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->device_type);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4); 
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,EVENT_STATE,Obj->state);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,RELIABILITY,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,OUT_OF_SERVICE,Obj->out_of_service);
        break;
      case UPDATE_INTERVAL:
        if (Param==READVAL) _itoa(Obj->update_interval,Buffer,10);
        break;
      case UNITS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,UNITS,Obj->units);
        break;
      case MIN_PRES_VALUE:
        if (Param==READVAL) CvtFltStr(Obj->min_pres_value,Buffer);
        break;
      case MAX_PRES_VALUE:
        if (Param==READVAL) CvtFltStr(Obj->max_pres_value,Buffer);
        break;
      case RESOLUTION:
        if (Param==READVAL) CvtFltStr(Obj->resolution,Buffer);
        break;
      case COV_INCREMENT:
        if (Param==READVAL) CvtFltStr(Obj->cov_increment,Buffer);
        break;
      case TIME_DELAY:
        if (Param==READVAL) _itoa(Obj->time_delay,Buffer,10);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _itoa(Obj->notification_class,Buffer,10);
        break;
      case HIGH_LIMIT:
        if (Param==READVAL) CvtFltStr(Obj->high_limit,Buffer);
        break;
      case LOW_LIMIT:
        if (Param==READVAL) CvtFltStr(Obj->low_limit,Buffer);
        break;
      case DEADBAND:	
        if (Param==READVAL) CvtFltStr(Obj->deadband,Buffer);
        break;
      case LIMIT_ENABLE:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->limit_enable,2);
        break;
      case EVENT_ENABLE:	
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->event_enable,3);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->acked_transitions,3);
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,NOTIFY_TYPE,Obj->notify_type);
        break;
    }
  return 1;
}

int ao(generic_object far* Object,dword id,char far* Buffer,long Param)
{ ao_obj_type far* Obj= (ao_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,OBJECT_TYPE,Obj->go.object_type);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) CvtFltStr(Obj->pv,Buffer);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case DEVICE_TYPE:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->device_type);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4); 
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,EVENT_STATE,Obj->state);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,RELIABILITY,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,OUT_OF_SERVICE,Obj->out_of_service);
        break;
      case UNITS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,UNITS,Obj->units);
        break;
      case MIN_PRES_VALUE:
        if (Param==READVAL) CvtFltStr(Obj->min_pres_value,Buffer);
        break;
      case MAX_PRES_VALUE:
        if (Param==READVAL) CvtFltStr(Obj->max_pres_value,Buffer);
        break;
      case RESOLUTION:
        if (Param==READVAL) CvtFltStr(Obj->resolution,Buffer);
        break;
      case PRIORITY_ARRAY:    
        if (Param==READVAL) ReadFltStream(Buffer,Obj->priority_array,16);
        break;
      case RELINQUISH_DEFAULT:
        if (Param==READVAL) CvtFltStr(Obj->relinquish_default,Buffer);
        break;
      case COV_INCREMENT:
        if (Param==READVAL) CvtFltStr(Obj->cov_increment,Buffer);
        break;
      case TIME_DELAY:
        if (Param==READVAL) _itoa(Obj->time_delay,Buffer,10);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _itoa(Obj->notification_class,Buffer,10);
        break;
      case HIGH_LIMIT:
        if (Param==READVAL) CvtFltStr(Obj->high_limit,Buffer);
        break;
      case LOW_LIMIT:
        if (Param==READVAL) CvtFltStr(Obj->low_limit,Buffer);
        break;
      case DEADBAND:	
        if (Param==READVAL) CvtFltStr(Obj->deadband,Buffer);
        break;
      case LIMIT_ENABLE:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->limit_enable,2);
        break;
      case EVENT_ENABLE:	
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->event_enable,3);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->acked_transitions,3);
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,NOTIFY_TYPE,Obj->notify_type);
        break;
    }
  return 1;
}

int av(generic_object far* Object,dword id,char far* Buffer,long Param)
{ av_obj_type far* Obj= (av_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,OBJECT_TYPE,Obj->go.object_type);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) CvtFltStr(Obj->pv,Buffer);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4); 
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,EVENT_STATE,Obj->state);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,RELIABILITY,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,OUT_OF_SERVICE,Obj->out_of_service);
        break;
      case UNITS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,UNITS,Obj->units);
        break;
      case PRIORITY_ARRAY:    
        if (Param==READVAL) ReadFltStream(Buffer,Obj->priority_array,16);
        break;
      case RELINQUISH_DEFAULT:
        if (Param==READVAL) CvtFltStr(Obj->relinquish_default,Buffer);
        break;
      case COV_INCREMENT:
        if (Param==READVAL) CvtFltStr(Obj->cov_increment,Buffer);
        break;
      case TIME_DELAY:
        if (Param==READVAL) _itoa(Obj->time_delay,Buffer,10);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _itoa(Obj->notification_class,Buffer,10);
        break;
      case HIGH_LIMIT:
        if (Param==READVAL) CvtFltStr(Obj->high_limit,Buffer);
        break;
      case LOW_LIMIT:
        if (Param==READVAL) CvtFltStr(Obj->low_limit,Buffer);
        break;
      case DEADBAND:	
        if (Param==READVAL) CvtFltStr(Obj->deadband,Buffer);
        break;
      case LIMIT_ENABLE:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->limit_enable,2);
        break;
      case EVENT_ENABLE:	
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->event_enable,3);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->acked_transitions,3);
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,NOTIFY_TYPE,Obj->notify_type);
        break;
    }
  return 1;
}

int bi(generic_object far* Object,dword id,char far* Buffer,long Param)
{ bi_obj_type far* Obj= (bi_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->pv);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case DEVICE_TYPE:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4);
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->state);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->out_of_service);
        break;
      case POLARITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->polarity);
        break;
      case INACTIVE_TEXT:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->inactive_text);
        break;
      case ACTIVE_TEXT:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->active_text);
        break;
      case CHANGE_OF_STATE_TIME:
        if (Param==READVAL) ReadBACnetDateTime(Buffer,&(Obj->cos_time));
        break;
      case CHANGE_OF_STATE_COUNT:
        if (Param==READVAL) _itoa(Obj->cos_count,Buffer,10);
        break;
      case TIME_OF_STATE_COUNT_RESET:
        if (Param==READVAL) ReadBACnetDateTime(Buffer,&(Obj->time_of_state_count_reset));
        break;
      case ELAPSED_ACTIVE_TIME:
        if (Param==READVAL) _ltoa(Obj->elapsed_active_time,Buffer,10);
        break;
      case TIME_OF_ACTIVE_TIME_RESET:
        if (Param==READVAL) ReadBACnetDateTime(Buffer,&(Obj->time_of_active_time_reset));
        break;
      case TIME_DELAY:
        if (Param==READVAL) _itoa(Obj->time_delay,Buffer,10);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _itoa(Obj->notification_class,Buffer,10);
        break;
      case ALARM_VALUE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,ALARM_VALUE,Obj->alarm_value);
        break;
      case EVENT_ENABLE:	
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->event_enable,3);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->acked_transitions,3);
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,NOTIFY_TYPE,Obj->notify_type);
        break;
    }
  return(1);
}

int bo(generic_object far* Object,dword id,char far* Buffer,long Param)
{ bo_obj_type far* Obj= (bo_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->pv);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case DEVICE_TYPE:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->device_type);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4);
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->state);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->out_of_service);
        break;
      case POLARITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->polarity);
        break;
      case INACTIVE_TEXT:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->inactive_text);
        break;
      case ACTIVE_TEXT:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->active_text);
        break;
      case CHANGE_OF_STATE_TIME:
        if (Param==READVAL) ReadBACnetDateTime(Buffer,&(Obj->cos_time));
        break;
      case CHANGE_OF_STATE_COUNT:
        if (Param==READVAL) _itoa(Obj->cos_count,Buffer,10);
        break;
      case TIME_OF_STATE_COUNT_RESET:
        if (Param==READVAL) ReadBACnetDateTime(Buffer,&(Obj->time_of_state_count_reset));
        break;
      case ELAPSED_ACTIVE_TIME:
        if (Param==READVAL) _ltoa(Obj->elapsed_active_time,Buffer,10);
        break;
      case TIME_OF_ACTIVE_TIME_RESET:
        if (Param==READVAL) ReadBACnetDateTime(Buffer,&(Obj->time_of_active_time_reset));
        break;
      case MINIMUM_OFF_TIME:
        if (Param==READVAL) _ltoa(Obj->min_off_time,Buffer,10);
        break;
      case MINIMUM_ON_TIME:
        if (Param==READVAL) _ltoa(Obj->min_on_time,Buffer,10);
        break;
      case PRIORITY_ARRAY:    
        if (Param==READVAL) ReadBACnetStream(Buffer,Obj->go.object_type,PRIORITY_ARRAY,Obj->priority_array,16);
        break;
      case RELINQUISH_DEFAULT:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->relinquish_default);
        break;
      case TIME_DELAY:
        if (Param==READVAL) _itoa(Obj->time_delay,Buffer,10);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _itoa(Obj->notification_class,Buffer,10);
        break;
      case FEEDBACK_VALUE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->feedback_value);
        break;
      case EVENT_ENABLE:	
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->event_enable,3);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->acked_transitions,3);
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->notify_type);
        break;
    }
  return 1;
}

int bv(generic_object far* Object,dword id,char far* Buffer,long Param)
{ bv_obj_type far* Obj= (bv_obj_type far*)Object;
 
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->pv);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4);
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->state);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->out_of_service);
        break;
      case INACTIVE_TEXT:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->inactive_text);
        break;
      case ACTIVE_TEXT:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->active_text);
        break;
      case CHANGE_OF_STATE_TIME:
        if (Param==READVAL) ReadBACnetDateTime(Buffer,&(Obj->cos_time));
        break;
      case CHANGE_OF_STATE_COUNT:
        if (Param==READVAL) _itoa(Obj->cos_count,Buffer,10);
        break;
      case TIME_OF_STATE_COUNT_RESET:
        if (Param==READVAL) ReadBACnetDateTime(Buffer,&(Obj->time_of_state_count_reset));
        break;
      case ELAPSED_ACTIVE_TIME:
        if (Param==READVAL) _ltoa(Obj->elapsed_active_time,Buffer,10);
        break;
      case TIME_OF_ACTIVE_TIME_RESET:
        if (Param==READVAL) ReadBACnetDateTime(Buffer,&(Obj->time_of_active_time_reset));
        break;
      case MINIMUM_OFF_TIME:
        if (Param==READVAL) _ltoa(Obj->min_off_time,Buffer,10);
        break;
      case MINIMUM_ON_TIME:
        if (Param==READVAL) _ltoa(Obj->min_on_time,Buffer,10);
        break;
      case PRIORITY_ARRAY:    
        if (Param==READVAL) ReadBACnetStream(Buffer,Obj->go.object_type,PRIORITY_ARRAY,Obj->priority_array,16);
        break;
      case RELINQUISH_DEFAULT:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->relinquish_default);
        break;
      case TIME_DELAY:
        if (Param==READVAL) _itoa(Obj->time_delay,Buffer,10);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _itoa(Obj->notification_class,Buffer,10);
        break;
      case ALARM_VALUE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,ALARM_VALUE,Obj->alarm_value);
        break;
      case EVENT_ENABLE:	
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->event_enable,3);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->acked_transitions,3);
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->notify_type);
        break;
    }
  return 1;
}

int cal(generic_object far* Object,dword id,char far* Buffer,long Param)
{ calendar_obj_type far* Obj= (calendar_obj_type far*)Object;

  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->pv);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case DATE_LIST:
        if (Param==READVAL) ReadBACnetCalendarEntry(Buffer,Obj->date_list);
        break;
    }  
  return 1;
}

int com(generic_object far* Object,dword id,char far* Buffer,long Param)
{ command_obj_type far* Obj= (command_obj_type far*)Object;

  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) _ultoa((unsigned long)Obj->pv,Buffer,10);
        break;
      case IN_PROCESS:
        break;
      case ALL_WRITES_SUCCESSFUL:
        break;
      case ACTION:
        break;
      case ACTION_TEXT:
        break;
    }  
  return 1;
}

int dev(generic_object far* Object,dword id,char far* Buffer,long Param)
{ device_obj_type far* Obj= (device_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case SYSTEM_STATUS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->system_status);
        break;
      case VENDOR_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->vendor_name);
        break;
      case VENDOR_IDENTIFIER:
        if (Param==READVAL) _ultoa((unsigned long)Obj->vendor_id,Buffer,10);
        break;
      case MODEL_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->model_name);
        break;
      case FIRMWARE_REVISION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->firmware_rev);
        break;
      case APPLICATION_SOFTWARE_VERSION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->application_software_ver);
        break;
      case LOCATION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->location);
        break;
      case PROTOCOL_VERSION:
        if (Param==READVAL) _ultoa((unsigned long)Obj->protocol_ver,Buffer,10);
        break;
      case PROTOCOL_CONFORMANCE_CLASS:
        if (Param==READVAL) _ultoa((unsigned long)Obj->protocol_conf_class,Buffer,10);
        break;
      case PROTOCOL_SERVICES_SUPPORTED:
        break;
      case PROTOCOL_OBJECT_TYPES_SUPPORTED:
        break;
      case OBJECT_LIST:
        break;
      case MAX_APDU_LENGTH_ACCEPTED:
        if (Param==READVAL) _ultoa((unsigned long)Obj->max_apdu_length_accepted,Buffer,10);
        break;
      case SEGMENTATION_SUPPORTED:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->segmentation_supported);
        break;
      case VT_CLASSES_SUPPORTED:
        break;
      case ACTIVE_VT_SESSIONS:
        break;
      case LOCAL_TIME:
        break;
      case LOCAL_DATE:
        break;
      case UTC_OFFSET:
        if (Param==READVAL) CvtFltStr(Obj->utc_offset,Buffer);
        break;
      case DAYLIGHT_SAVINGS_STATUS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->day_savings_status);
        break;
      case APDU_SEGMENT_TIMEOUT:
        if (Param==READVAL) _ultoa(Obj->apdu_segment_timeout,Buffer,10);
        break;
      case APDU_TIMEOUT:
        if (Param==READVAL) _ultoa(Obj->apdu_timeout,Buffer,10);
        break;
      case NUMBER_OF_APDU_RETRIES:
        if (Param==READVAL) _ultoa((unsigned long)Obj->number_apdu_retries,Buffer,10);
        break;
      case LIST_OF_SESSION_KEYS:
        break;
      case TIME_SYNCHRONIZATION_RECIPIENTS:
        break;
      case MAX_MASTER:
        if (Param==READVAL) _ultoa((unsigned long)Obj->max_master,Buffer,10);
        break;
      case MAX_INFO_FRAMES:
        if (Param==READVAL) _ultoa((unsigned long)Obj->max_info_frames,Buffer,10);
        break;
      case DEVICE_ADDRESS_BINDING:
        break;
    }    
  return 1;
}

int eve(generic_object far* Object,dword id,char far* Buffer,long Param)
{ ee_obj_type far* Obj= (ee_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case EVENT_TYPE:
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->notify_type);
        break;
      case EVENT_PARAMETERS:
        break;
      case OBJECT_PROPERTY_REFERENCE:
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->state);
        break;
      case EVENT_ENABLE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->event_enable);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->acked_transitions);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _itoa(Obj->notification_class,Buffer,10);
        break;
      case RECIPIENT:
        break;
      case PROCESS_IDENTIFIER:
        break;
      case PRIORITY:
        break;
      case ISSUE_CONFIRMED_NOTIFICATIONS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->issue_conf_notifications);
        break;
    }    
  return 1;
}

int flo(generic_object far* Object,dword id,char far* Buffer,long Param)
{ file_obj_type far* Obj= (file_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case FILE_TYPE:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->file_type);
        break;
      case FILE_SIZE:
        if (Param==READVAL) _ultoa(Obj->file_size,Buffer,10);
        break;
      case MODIFICATION_DATE:
        break;
      case ARCHIVE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->archive);
        break;
      case READ_ONLY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->read_only);
        break;
      case FILE_ACCESS_METHOD:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->access_method);
        break;
    }    
  return 1;
}

int gru(generic_object far* Object,dword id,char far* Buffer,long Param)
{ group_obj_type far* Obj= (group_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case LIST_OF_GROUP_MEMBERS:
        break;
      case PRESENT_VALUE:
        break;
    }    
  return 1;
}

int loo(generic_object far* Object,dword id,char far* Buffer,long Param)
{ loop_obj_type far* Obj= (loop_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) CvtFltStr(Obj->pv,Buffer);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4);
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->state);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->out_of_service);
        break;
      case UPDATE_INTERVAL:
        if (Param==READVAL) _ultoa(Obj->update_interval,Buffer,10);
      case OUTPUT_UNITS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->output_units);
        break;
      case MANIPULATED_VARIABLE_REFERENCE:
        break;
      case CONTROLLED_VARIABLE_REFERENCE:
        break;
      case CONTROLLED_VARIABLE_VALUE:
        if (Param==READVAL) CvtFltStr(Obj->cont_var_value,Buffer);
        break;
      case CONTROLLED_VARIABLE_UNITS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->cont_var_units);
        break;
      case SETPOINT_REFERENCE:
        break;
      case SETPOINT:
        if (Param==READVAL) CvtFltStr(Obj->setpoint,Buffer);
        break;
      case ACTION:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->action);
        break;
      case PROPORTIONAL_CONSTANT:
        if (Param==READVAL) CvtFltStr(Obj->proportional_const,Buffer);
        break;
      case PROPORTIONAL_CONSTANT_UNITS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->proportional_const_units);
        break;
      case INTEGRAL_CONSTANT:
        if (Param==READVAL) CvtFltStr(Obj->integral_const,Buffer);
        break;
      case INTEGRAL_CONSTANT_UNITS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->integral_const_units);
        break;
      case DERIVATIVE_CONSTANT:
        if (Param==READVAL) CvtFltStr(Obj->derivative_const,Buffer);
        break;
      case DERIVATIVE_CONSTANT_UNITS:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->derivative_const_units);
        break;
      case BIAS:
        if (Param==READVAL) CvtFltStr(Obj->bias,Buffer);
        break;
      case MAXIMUM_OUTPUT:
        if (Param==READVAL) CvtFltStr(Obj->max_output,Buffer);
        break;
      case MINIMUM_OUTPUT:
        if (Param==READVAL) CvtFltStr(Obj->min_output,Buffer);
        break;
      case PRIORITY_FOR_WRITING:
        if (Param==READVAL) _ultoa(Obj->priority_for_writing,Buffer,10);
        break;
      case COV_INCREMENT:
        if (Param==READVAL) CvtFltStr(Obj->cov_increment,Buffer);
        break;
      case TIME_DELAY:
        if (Param==READVAL) _ultoa(Obj->time_delay,Buffer,10);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _ultoa(Obj->notification_class,Buffer,10);
        break;
      case ERROR_LIMIT:
        if (Param==READVAL) CvtFltStr(Obj->error_limit,Buffer);
        break;
      case EVENT_ENABLE:	
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->event_enable,3);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->acked_transitions,3);
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->notify_type);
        break;
    }    
  return 1;
}

int msi(generic_object far* Object,dword id,char far* Buffer,long Param)
{ mi_obj_type far* Obj= (mi_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) _itoa(Obj->pv,Buffer,10);
        break;
      case DEVICE_TYPE:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->device_type);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4);
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->state);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->out_of_service);
        break;
      case NUMBER_OF_STATES:
        if (Param==READVAL) _itoa(Obj->num_of_states,Buffer,10);
        break;
      case STATE_TEXT:
        break;
      case TIME_DELAY:
        if (Param==READVAL) _ultoa(Obj->time_delay,Buffer,10);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _ultoa(Obj->notification_class,Buffer,10);
        break;
      case ALARM_VALUES:
        break;
      case FAULT_VALUES:
        break;
      case EVENT_ENABLE:	
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->event_enable,3);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->acked_transitions,3);
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->notify_type);
        break;
    }    
  return 1;
}

int mso(generic_object far* Object,dword id,char far* Buffer,long Param)
{ mo_obj_type far* Obj= (mo_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case PRESENT_VALUE:
        if (Param==READVAL) _itoa(Obj->pv,Buffer,10);
        break;
      case DEVICE_TYPE:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->device_type);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4);
        break;
      case EVENT_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->state);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->out_of_service);
        break;
      case NUMBER_OF_STATES:
        if (Param==READVAL) _itoa(Obj->num_of_states,Buffer,10);
        break;
      case STATE_TEXT:
        break;
      case PRIORITY_ARRAY:
        break;
      case RELINQUISH_DEFAULT:
        break;
      case TIME_DELAY:
        if (Param==READVAL) _ultoa(Obj->time_delay,Buffer,10);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _ultoa(Obj->notification_class,Buffer,10);
        break;
      case FEEDBACK_VALUE:
        break;
      case EVENT_ENABLE:	
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->event_enable,3);
        break;
      case ACKED_TRANSITIONS:
        if (Param==READVAL) ReadOctetBitStream(Buffer,Obj->acked_transitions,3);
        break;
      case NOTIFY_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->notify_type);
        break;
    }    
  return 1;
}

int nofy(generic_object far* Object,dword id,char far* Buffer,long Param)
{ nc_obj_type far* Obj= (nc_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case NOTIFICATION_CLASS:
        if (Param==READVAL) _ultoa(Obj->notification_class,Buffer,10);
        break;
      case PRIORITY:
        break;
      case ACK_REQUIRED:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->ack_required);
        break;
      case RECIPIENT_LIST:
        break;
    }    
  return 1;
}

int prg(generic_object far* Object,dword id,char far* Buffer,long Param)
{ program_obj_type far* Obj= (program_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case PROGRAM_STATE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->prog_state);
        break;
      case PROGRAM_CHANGE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->prog_change);
        break;
      case REASON_FOR_HALT:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->reason_for_halt);
        break;
      case DESCRIPTION_OF_HALT:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->description_of_halt);
        break;
      case PROGRAM_LOCATION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->prog_location);
        break;
      case INSTANCE_OF:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->instance_of);
        break;
      case STATUS_FLAGS:
        if (Param==READVAL) ReadOctetBitStream(Buffer, Obj->status_flags,4);
        break;
      case RELIABILITY:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->reliability);
        break;
      case OUT_OF_SERVICE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->out_of_service);
        break;
    }    
  return 1;
}

int sced(generic_object far* Object,dword id,char far* Buffer,long Param)
{ schedule_obj_type far* Obj= (schedule_obj_type far*)Object;
  switch ( (enum BACnetPropertyIdentifier) id )
    {
      case OBJECT_IDENTIFIER:
        if (Param==READVAL) _ultoa(Obj->go.object_id & 0x003FFFFF,Buffer,10);
        break;
      case OBJECT_NAME:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.object_name);
        break;
      case OBJECT_TYPE:
        if (Param==READVAL) GetEnumName(Buffer,Obj->go.object_type,id,Obj->go.object_type);
        break;
      case DESCRIPTION:
        if (Param==READVAL) _fstrcpy(Buffer,Obj->go.description);
        break;
      case PRESENT_VALUE:
        break;
      case EFFECTIVE_PERIOD:
        break;
      case WEEKLY_SCHEDULE:
        break;
      case EXCEPTION_SCHEDULE:
        break;
      case LIST_OF_OBJECT_PROPERTY_REFERENCES:
        break;
      case PRIORITY_FOR_WRITING:
        break;
    }    
  return 1;
}




int far pascal _WEP(int i)
{ i= 1;
  /* Your WEP functionality goes here */
  return i;
}
  
  