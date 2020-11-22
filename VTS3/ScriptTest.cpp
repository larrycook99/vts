
#include "stdafx.h"
#include "VTS.h"
#include "ScriptBase.h"

//
//	ScriptTest::ScriptTest
//

ScriptTest::ScriptTest( const CString &number )
//	: ScriptCase( "" ), testFirstPacket(0), testNext(0)
	: ScriptCase( "" ), testFirstCommand(0), testNext(0)
{
	baseType = scriptTest;
	baseLabel = number;

	// set the image list offset and status
	baseImage = 4;
	baseStatus = 0;
}
