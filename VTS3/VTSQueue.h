// VTSQueue.h: interface for the VTSQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSQUEUE_H__AC6F4361_C02A_11D4_BEFB_00A0C95A9812__INCLUDED_)
#define AFX_VTSQUEUE_H__AC6F4361_C02A_11D4_BEFB_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
  #pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include "stdafx.h"
#include "VTS.h"

//
//	VTSQueue
//
//	A VTSQueue is a FIFO queue that is thread safe.  It is used to transfer 
//	messages from the script executor to the main application thread, as well 
//	as transfer messages from the IO thread to the BACnetTaskManager thread.
//
//	It is a simple linked list.  Pointers to elements (rather than elements 
//	themselves) are added and removed from the list.  If dynamic elements are 
//	created and added to the list it is up to the reader to dispose of them.
//

template<class T>
	class VTSQueue {
		private:
			struct VTSQueueElem {
				T				*qElem;
				VTSQueueElem	*qNext;
				};

			CCriticalSection	qCS;			// read/write lock

			VTSQueueElem		*qFirst;		// first element
			VTSQueueElem		*qLast;			// last element

		public:
			VTSQueue();
			virtual ~VTSQueue();

			T* Read( void );					// nil if empty
			void Write( T* t );					// add a pointer to an element
			void Fire( T* t );					// write to the queue then fire to waiting thread
		};

class ScriptExecMsg;
typedef VTSQueue<ScriptExecMsg> ScriptExecMsgQueue;

#endif // !defined(AFX_VTSQUEUE_H__AC6F4361_C02A_11D4_BEFB_00A0C95A9812__INCLUDED_)
