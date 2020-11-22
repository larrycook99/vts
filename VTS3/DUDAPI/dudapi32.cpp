/* 29-Jan-01 [003] JJB revise for C++, added PICS namespace and typecasts
   09-Jun-98 [002] JN  fix to CheckFunctionalGroup
                  fix integer->short
    23-Mar-98 [001] JN  32-bit version (not all marked) 
                   eg. far pascal __export --> APIENTRY
*/

#include "stdafx.h"

#include <windows.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "StringTables.h"

namespace PICS {

#define _DoStaticPropDescriptors 1

#include "db.h" 
#include "stdobj.h"
#include "stdobjpr.h"
#include "vtsapi.h" 
#include "bacprim.h"

#include "dudapi.h"   // export interface

#include "props.h"
#include "propid.h"
#include "VTS.h"

// local variables -------------------------------------------------------------------------------------------

static char gBuffer[256];

// ===========================================================================================================
//
// export functions
//
// ===========================================================================================================


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
short APIENTRY GetPropNameSupported(char far* PropName, word i, word objtype, octet far propFlags[64], dword far* PropId, word * pParseType)
{
  if ( (propFlags[i] & 1) == 1 )
    {
      *PropId= VTSAPIgetpropinfo(objtype,i,PropName,pParseType,NULL,NULL,NULL);
      if (*PropId == -1) return(-1); // invalid property index
      else return( strlen(PropName) );
    }  
  else return(0); // not supported
}



// This function is used to find a generic_object identified by its object id.
// in:      root     root of object list
//       ObjectId object being sought
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

// This function is used to find a generic_object identified by its object id.
// in:      root     root of object list
//       ObjectId object being sought
// returns: pointer to matching object, or NULL
extern "C"
generic_object far* APIENTRY GetpDeviceObj(generic_object far* root)
{
  while (root!=NULL)
  {
    if (root->object_type == OBJ_DEVICE) 
      break;
    root= (generic_object far*) root->next;
  }
  return root;
}

// This function is used to get the property index of a property.
extern "C"
short APIENTRY GetPropIndex(word object_type, dword PropId)
{
  dword c; dword id;
  c= VTSAPIgetpropinfo(object_type,0xFFFF,NULL,NULL,NULL,NULL,NULL);
  for (short i=0; (dword)i<c; i++)
  {
    id= VTSAPIgetpropinfo(object_type,i,gBuffer,NULL,NULL,NULL,NULL);
    if (id==PropId) return(i);
  }
  return -1;
}

// This function is used to extract object type and object instance from an object identifier.
// in:      ObjId    object identifier
// out:     ObjType     buffer to contain object type
//       ObjInst     buffer to contain object instance
extern "C"
void APIENTRY SplitObjectId(dword ObjId, word far* ObjType, dword far* ObjInst)
{
  if (ObjType!=NULL) *ObjType= (word) (ObjId>>22);
  if (ObjInst!=NULL) *ObjInst= ObjId & 0x3FFFFF;
}


// This function is used to remove the complete database.
// in:      root     root of object list.
extern "C"
void APIENTRY MyDeletePICSObject(generic_object far* root)
{
  generic_object far* p;
  while (root!=NULL)
  {
    p= root;
    root= (generic_object far*)root->next;
    DeletePICSObject(p);
  }
}

}
