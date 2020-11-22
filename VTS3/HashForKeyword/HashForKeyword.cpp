// HashForKeyword.cpp : Calculate hash code for input keywords
//

#include "stdafx.h"

// HACK ALERT: this code is snipped from the main VTS project.
// Yes, it should use the same code, but a tangle of dependencies follow.
// SOrry
//
//	ScriptToken::HashCode
//
//	This hash function is used to encode a keyword into some unique value 
//	that can be used in switch statements and looked up in tables very 
//	quickly.  There may be two keywords that map to the same hash value,
//	but I haven't found any yet!
//
int HashCode( const char *key )
{ 
#define LARGENUMBER     6293815 
 
	int		index = 1;
	unsigned int sum = 0;
	unsigned int multiple = LARGENUMBER;

	while (*key != '\0') {
		sum += multiple * index++ * (*key++);
		multiple *= LARGENUMBER;
	}

	return sum; 
}


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		printf( "HashForKeyword: generate VTS script hash for specified keywords\n"
			    "usage:\n"
				"  HashForKeyword key1 {key2 ...}\n"
				"Output will be a list of hash values suitable for inclusion in vts ScriptKeywords tables.\n" );
		return -1;
	}

	unsigned int numval = (unsigned int)atoi( argv[1] );
	for (int ix = 2; ix < argc; ix++)
	{
		_strupr( argv[ix] );
		printf( ", { 0x%08x, %u }                   // %s\n", HashCode( argv[ix] ), numval, argv[ix] );
		numval += 1;
	}
	
	return 0;
}

