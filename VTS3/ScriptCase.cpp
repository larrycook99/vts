
#include "stdafx.h"
#include "VTS.h"
#include "ScriptBase.h"

//
//	ScriptCase::ScriptCase
//

ScriptCase::ScriptCase( const CString &number )
	: caseGroup(0), caseSubgroup(0)
{
	// set the base information
	baseType = scriptCase;
	baseLabel = number;

	// set the image list offset and status
	baseImage = 17;
	baseStatus = 0;
}
