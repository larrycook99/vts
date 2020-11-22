
#include "stdafx.h"

#define	_BACnetTaskDebug		0

#if _BACnetTaskDebug
#include <iostream>
#endif

#include "BACnet.hpp"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//

BACnetTaskManagerPtr gTaskManager;

//
//	Note that the task is NOT installed.  You can create a task of a specific type 
//	then install it when it needs to be scheduled.
//

BACnetTask::BACnetTask(BACnetTaskType typ, long delay)
{
#if _BACnetTaskDebug
	cout << "(new task " << (int)this << ")" << endl;
#endif
	taskType = typ;
	taskInterval = delay;
	isActive = 0;
}

//
//	When the task goes away, make sure it's not in the task list.
//

BACnetTask::~BACnetTask(void)
{
#if _BACnetTaskDebug
	cout << "(delete task " << (int)this << ")" << endl;
#endif
	if (isActive)
		gTaskManager->SuspendTask( this );
}

//
//	BACnetTask::InstallTask
//

void BACnetTask::InstallTask(void)
{
	gTaskManager->InstallTask( this );
}


// Install, rescheduling if necessary
void BACnetTask::InstallTask( BACnetTaskType theType, long theInterval )
{
	if (isActive)
		gTaskManager->SuspendTask( this );

	taskType = theType;
	taskInterval = theInterval;

	gTaskManager->InstallTask( this );
}

//
//	BACnetTask::SuspendTask
//

void BACnetTask::SuspendTask(void)
{
	gTaskManager->SuspendTask( this );
}

//
//	BACnetTask::ResumeTask
//

void BACnetTask::ResumeTask(void)
{
	gTaskManager->ResumeTask( this );
}
