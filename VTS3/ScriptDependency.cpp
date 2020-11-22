
#include "stdafx.h"
#include "VTS.h"
#include "ScriptBase.h"

//
//	ScriptDependency::ScriptDependency
//

ScriptDependency::ScriptDependency( const CString &number )
{
	baseType = scriptDependency;
	baseLabel = number;

	// set the image list offset and status
	baseImage = 8;
	baseStatus = 0;
}
