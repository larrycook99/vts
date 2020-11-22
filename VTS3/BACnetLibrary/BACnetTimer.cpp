
#include "stdafx.h"

#include "BACnetTimer.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	The only thing that gets defined in this module is the global pointer to 
//	the time manager to use.  Everything else in the BACnetTimer class is 
//	pure virtual (otherwise known as abstract).
//

BACnetTimerManagerPtr gTimeManager;
