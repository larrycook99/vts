// WPRPList.cpp: implementation of the WPRPList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "WPRPList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//Xiao Shiyuan 2002-12-2

WPMRPMListList glWPMRPMListList[5];
int glWPMRPMHistoryCount = 0;
int glCurWPMRPMHistory = 0;


//madanner 9/04, fixed massive memory leaks :(  by adding destructor
WPMRPMList::~WPMRPMList()
{
	DeleteAllObjects();
}

void WPMRPMList::DeleteAllObjects(void)
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
	{
		delete GetNext( pos );
	}

	RemoveAll();
}


void WPMRPMList::KillAll(void)
{
	DeleteAllObjects();
}


WPMRPMListList::~WPMRPMListList()
{
	DeleteAllObjects();
}

void WPMRPMListList::DeleteAllObjects(void)
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
	{
		delete GetNext( pos );
	}

	RemoveAll();
}


void WPMRPMListList::KillAll(void)
{
	DeleteAllObjects();
}
