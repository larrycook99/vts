
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "BACnet.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//
//	BACnetServer::BACnetServer
//

BACnetServer::BACnetServer( BACnetDevicePtr dp )
	: serverTSM( dp ), serverNext(0)
{
	// bind to our internal TSM
	Bind( &serverTSM, this );
	
	// bind to the device
	serverTSM.tsmDevice->Bind( this );
}

//
//	BACnetServer::~BACnetServer
//

BACnetServer::~BACnetServer( void )
{
	// unbind from the device
	serverTSM.tsmDevice->Unbind( this );
	
	// unbind from our internal TSM
	Unbind( &serverTSM, this );
}
