#ifndef _BACnetTimer
#define _BACnetTimer

#include "BACnet.hpp"

//
//	BACnetTimer
//

class BACnetTimer {
	protected:
		BACnetTimer( void ) { }
		~BACnetTimer( void ) { }
	};

typedef BACnetTimer *BACnetTimerPtr;

//
//	BACnetTimerManager
//

class BACnetTimerManager {
	public:
		virtual BACnetTimerPtr NewTimer( void ) = 0;
		virtual void DeleteTimer( BACnetTimerPtr tp ) = 0;
		
//		virtual BACnetTimerPtr SetTime( const BACnetDateTime &when ) = 0;
		virtual void SetTimeDelay( BACnetTimerPtr tp, unsigned long delta ) = 0;
		
		virtual int Compare( BACnetTimerPtr tp1, BACnetTimerPtr tp2 ) = 0;
//		virtual unsigned long TimeUntilExpired( BACnetTimerPtr tp ) = 0;
//		virtual unsigned long TimeElapsed( BACnetTimerPtr tp ) = 0;
	};

typedef BACnetTimerManager *BACnetTimerManagerPtr;

extern BACnetTimerManagerPtr gTimeManager;

#endif
