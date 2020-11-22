
#include "stdafx.h"
#include "VTS.h"
#include "ScriptBase.h"

//
//	ScriptSection::ScriptSection
//

ScriptSection::ScriptSection( const CString &title )
{
	baseType = scriptSection;
	baseLabel = title;

	// set the image list offset and status
	baseImage = 0;
	baseStatus = 0;
}
