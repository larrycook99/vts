
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
//	BACnetClient::BACnetClient
//

BACnetClient::BACnetClient( BACnetDevicePtr dp )
	: clientTSM( dp ), clientNext(0)
{
	// bind to our internal TSM
	Bind( this, &clientTSM );
	
	// now bind to the device
	clientTSM.tsmDevice->Bind( this );
}

//
//	BACnetClient::~BACnetClient
//

BACnetClient::~BACnetClient( void )
{
	// unbind from the device
	clientTSM.tsmDevice->Unbind( this );
	
	// unbind from our internal TSM
	Unbind( this, &clientTSM );
}
