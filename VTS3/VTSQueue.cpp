#include "stdafx.h"
#include "vts.h"
#include "VTSQueue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* test app for VTSQueue template */

#ifdef LOCAL_TEST

#include "VTSQueue.h"
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

char *strings[] = {{"One"},{"Two"},{"Three"},{"Four"},{NULL}};

/* note - only tests the FIFO part, not the multithread safe part */
void main(void)
{
  int index;
  VTSQueue<char *> queue;
  char **pWord;

  index = 0;
  while (strings[index]) {
    queue.Write(&strings[index]);
    index++;
  }

  index = 0;
  do {
    pWord = queue.Read();
    if (pWord)
      assert(strcmp(*pWord,strings[index]) == 0);
    else
      assert(strings[index] == NULL);
    index++;
  } while (pWord);

  return;
}

//#endif



/////////////////////////////////////////////////////////////////////////////
// VTSQueue

//ScriptExecMsgQueue * pbogustemplatetest;


//
//	VTSQueue<T>::VTSQueue
//

template<class T>
	VTSQueue<T>::VTSQueue()
		: qFirst(0), qLast(0)
	{
	}

//
//	VTSQueue<T>::~VTSQueue
//

template<class T>
	VTSQueue<T>::~VTSQueue()
	{
		if (qFirst)
			TRACE0( "Warning: queue not empty\n" );
	}

//
//	VTSQueue<T>::Read
//
//	This function returns a pointer to the first (oldest) element that was
//	added to the queue.  If the queue is empty it returns nil.
//

template<class T>
	T* VTSQueue<T>::Read( void )
	{
		// lock the queue
		qCS.Lock();

		VTSQueueElem	*cur = qFirst
		;
		T				*rslt = 0
		;

		// if the queue not is empty, extract the first element
		if (cur) {
			// set result
			rslt = cur->qElem;

			// remove from list
			qFirst = cur->qNext;
			if (!qFirst)
				qLast = 0;

			// delete wrapper element
			delete cur;
		}

		// unlock the queue
		qCS.Unlock();

		// fini
		return rslt;
	}

//
//	VTSQueue<T>::Write
//
//	This function adds a pointer to an element to the end of the queue.
//

template<class T>
	void VTSQueue<T>::Write( T* tp )
	{
		// lock the queue
		qCS.Lock();

		VTSQueueElem	*cur = new VTSQueueElem()
		;

		cur->qElem = tp;
		cur->qNext = 0;

		if (qFirst)
			qLast->qNext = cur;
		else
			qFirst = cur;

		qLast = cur;

		// unlock the queue
		qCS.Unlock();
	}


template<class T>
	void VTSQueue<T>::Fire( T* tp )
	{
		Write(tp);
		::PostThreadMessage( AfxGetApp()->m_nThreadID, WM_VTS_EXECMSG, (WPARAM)0, (LPARAM)0	);
	}

#endif