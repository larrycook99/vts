
#include "stdafx.h"
#include "resource.h"

#include "WinBACnetTaskManager.hpp"

struct WinTaskEntry {
	WinTaskEntry		*taskNext;
	BACnetTaskPtr		taskPtr;
	unsigned long		taskTime;
	};

typedef WinTaskEntry *WinTaskEntryPtr;

//

WinTaskEntryPtr	gBACnetWinTaskList = 0;
int				gBACnetWinTaskContinue = 0;
CWinThread		*gBACnetWinTaskThread = 0;

UINT WinBACnetTaskThreadFunc( LPVOID pParam );

//
//	WinBACnetTaskManager::WinBACnetTaskManager
//

WinBACnetTaskManager::WinBACnetTaskManager( void )
{
	// allow it to run once it gets started
	gBACnetWinTaskContinue = true;

	// start it suspended
	gBACnetWinTaskThread =
		AfxBeginThread( WinBACnetTaskThreadFunc, 0
			, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED
			);

	// dont let windows delete the thread object
	gBACnetWinTaskThread->m_bAutoDelete = FALSE;

	// now let it run
//	Move it to VTSAPP->InitInstance(), Because gTaskManager hasn't been initialized 
//	Modified by xuyiping-hust
//	gBACnetWinTaskThread->ResumeThread();  

}

//
//	WinBACnetTaskManager::~WinBACnetTaskManager
//

WinBACnetTaskManager::~WinBACnetTaskManager( void )
{
	int		loopCount
	;
	DWORD	dwExitCode
	;

	// see if our thread is still around
	::GetExitCodeThread( gBACnetWinTaskThread->m_hThread, &dwExitCode );
	if (dwExitCode == STILL_ACTIVE) {
		// the thread is still running
		gBACnetWinTaskContinue = false;
		
		// wait for it to go away
		for (loopCount = 0; loopCount < 5; loopCount++)
			if (::WaitForSingleObject( gBACnetWinTaskThread->m_hThread, 1000 ) == WAIT_OBJECT_0)
				break;
		if (loopCount == 5) {
			AfxMessageBox( "WARNING: terminating task manager thread" );
			::TerminateThread( gBACnetWinTaskThread->m_hThread, 0 );
		}
	}
	
	// The thread has terminated. Delete the CWinThread object.
	delete gBACnetWinTaskThread;
}

//
//	WinBACnetTaskThreadFunc
//

UINT WinBACnetTaskThreadFunc( LPVOID )
{
	while (gBACnetWinTaskContinue) {
		gTaskManager->ProcessTasks();
		::Sleep( 1 );
	}

	return 0;
}

//
//	WinBACnetTaskManager::InstallTask
//
//	Installing a task means putting it in the correct place in the queue.  This is 
//	written so that when two tasks are both scheduled for the same time the one that 
//	was scheduled first will be given priority.
//

void WinBACnetTaskManager::InstallTask( BACnetTaskPtr tp )
{
	// see if application is confused
	if (tp->isActive)
	{
		TRACE( "InstallTask %p already active.\n", tp ); 
		return;
	}
	
	CSingleLock	lock( &mgrCS );

	// lock to prevent multiple thread access
	lock.Lock();

	WinTaskEntryPtr	newTask, cur, *prev;
	
	// new entry
	newTask = new WinTaskEntry();
	newTask->taskPtr = tp;
	newTask->taskTime = GetTickCount() + tp->taskInterval;
	
	// add it to the list
	prev = &gBACnetWinTaskList;
	for (cur = gBACnetWinTaskList; cur; cur = cur->taskNext) {
		if (newTask->taskTime < cur->taskTime)
			break;
		prev = &cur->taskNext;
	}
	newTask->taskNext = cur;
	*prev = newTask;
	
	// now it's active
	tp->isActive = true;

	// more activity OK
	lock.Unlock();
}

//
//	WinBACnetTaskManager::SuspendTask
//
//	When a task is suspended it is removed from the task list.  This is pretty typical
//	code for searching and removing an element from a single linked list.  If the 
//	task was never installed in the first place (or the pointer got corrupted somehow)
//	it won't make any difference.
//

void WinBACnetTaskManager::SuspendTask( BACnetTaskPtr tp )
{
	// see if application is confused
	if (!tp->isActive)
		return;
	
	CSingleLock		lock( &mgrCS );

	// lock to prevent multiple thread access
	lock.Lock();

	WinTaskEntryPtr		cur, *prev;
	
	prev = &gBACnetWinTaskList;
	for (cur = gBACnetWinTaskList; cur; cur = cur->taskNext) {
		if (cur->taskPtr == tp) {
			*prev = cur->taskNext;
			break;
		}
		prev = &cur->taskNext;
	}
	
	delete cur;
	
	// tell the task it is no longer scheduled
	tp->isActive = false;

	// ready for more activity
	lock.Unlock();
}

//
//	WinBACnetTaskManager::ResumeTask
//
//	Called to resume a task that has been suspended.  If the taskTime has already
//	expired it will fire very soon, otherwise it will fire when it's supposed to.
//

void WinBACnetTaskManager::ResumeTask( BACnetTaskPtr tp )
{
	InstallTask( tp );
}

//
//	WinBACnetTaskManager::ProcessTasks
//
//	The task manager is fairly simple.  See if the first task installed in the list 
//	needs to fire and fire it.  If the task needs to be re-installed, figure out when 
//	the next interval time will be and call the InstallTask method.  If the interval 
//	is zero, save it in a temporary list until finished processing all of the tasks
//	and then put these special guys back in the front of the list.
//
//	This code is written so a recurring task can schedule itself for the first time 
//	it should fire and then repeat after that.  For example, the first time a task 
//	should fire is 1000 ticks away but then should fire every 15 ticks after that.
//
//	A task may change its type during processing, but it's kinda ugly.  This is for 
//	one shots that would like another chance (?).  The isActive flag remains true 
//	while the task is running in case it uses a function that needs to know but may 
//	be called while it is not running.  A task may call SuspendTask() to keep itself 
//	from being re-queued.
//
//	This is slightly more complicated in the transition to make it thread safe.  The 
//	manager only locks when it is the process of futzing with pointers.
//

void WinBACnetTaskManager::ProcessTasks( void )
{
	unsigned long			now
	;
	WinTaskEntryPtr			curTask, *prevTask, firstZero, lastZero, i
	;
	CSingleLock				lock( &mgrCS )
	;
	
	firstZero = lastZero = 0;
	
	for (;;) {
		now = GetTickCount();
		
		// lock to prevent multiple thread access
		lock.Lock();

		curTask = gBACnetWinTaskList;
		if (!curTask || (curTask->taskTime > now)) {
			lock.Unlock();
			break;
		}

		// strip it off the front of the list
		gBACnetWinTaskList = curTask->taskNext;
		curTask->taskNext = 0;
		
		// let non-recurring tasks know they are NOT scheduled
		if (curTask->taskPtr->taskType != BACnetTask::recurringTask)
		{
			curTask->taskPtr->isActive = 0;
		}

		// ok for other threads (and this one) to change the list
		lock.Unlock();

		// give the task a chance to do something
#ifdef _DEBUG
		// During debugging, this lets us SEE who blew cookies
		curTask->taskPtr->ProcessTask();
#else
		try
		{
			curTask->taskPtr->ProcessTask();
		}
		catch (...) {
			AfxMessageBox( IDS_ERR_TASKERROR, MB_ICONHAND);

			// Set task type to oneShotTask so this will kill itself
			curTask->taskPtr->taskType = BACnetTask::oneShotTask;
		}
#endif		
		// lock it back down
		lock.Lock();

		// reschedule it
		switch (curTask->taskPtr->taskType) {
			case BACnetTask::oneShotTask:
				delete curTask;
				break;
			
			case BACnetTask::oneShotDeleteTask:
				delete curTask->taskPtr;
				delete curTask;
				break;
			
			case BACnetTask::recurringTask:
				// could have been suspended
				if (!curTask->taskPtr->isActive) {
					delete curTask;
					break;
				}
				
				// positive recurring interval
				if (curTask->taskPtr->taskInterval > 0) {
					// compute next running time
					curTask->taskTime += curTask->taskPtr->taskInterval;
					
					// reinstall it
					prevTask = &gBACnetWinTaskList;
					for (i = gBACnetWinTaskList; i; i = i->taskNext) {
						if (curTask->taskTime < i->taskTime)
							break;
						prevTask = &i->taskNext;
					}
					curTask->taskNext = i;
					*prevTask = curTask;
				} else {
					if (!firstZero)
						firstZero = lastZero = curTask;
					else {
						lastZero->taskNext = curTask;
						lastZero = curTask;
					}
				}
				break;
		}

		// ok again
		lock.Unlock();
	}
	
	// if there are any zero wait tasks, put them on the front of the list
	if (firstZero) {
		// lock it back down
		lock.Lock();

		lastZero->taskNext = gBACnetWinTaskList;
		gBACnetWinTaskList = firstZero;

		// ok again
		lock.Unlock();
	}
}
