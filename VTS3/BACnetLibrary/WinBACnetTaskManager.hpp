#ifndef _WinBACnetTaskManager
#define _WinBACnetTaskManager

#include <afxmt.h>

#include "BACnet.hpp"

//
//	WinBACnetTaskManager
//

class WinBACnetTaskManager : public BACnetTaskManager {
	protected:
		bool				mgrResched;		// task rescheduled itself
		CCriticalSection	mgrCS;			// read/write lock

	public:
		WinBACnetTaskManager( void );
		virtual ~WinBACnetTaskManager( void );

		virtual void InstallTask( BACnetTaskPtr tp );
		virtual void SuspendTask( BACnetTaskPtr tp );
		virtual void ResumeTask( BACnetTaskPtr tp );
		
		virtual void ProcessTasks( void );
	};

typedef WinBACnetTaskManager *WinBACnetTaskManagerPtr;

#endif
