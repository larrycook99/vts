
#include "stdafx.h"
#include "VTS.h"
#include "ScriptBase.h"

//
//	ScriptReference::ScriptReference
//

ScriptReference::ScriptReference( const CString &clause )
{
	baseType = scriptReference;
	baseLabel = clause;

	// set the image list offset and status
	baseImage = 12;
	baseStatus = 0;
}
