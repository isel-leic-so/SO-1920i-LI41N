#include <windows.h>

#include "cdl.h"


// unchecked the wanted alternative

// #define WITH_MUTEX
#define WITH_EVENT
// #define WITH_INTERLOCKED

/// signal (decrease) the countdownlatch.
/// alternative implementations based on mutex, event e atomic decrement
/// (InterlockedDecrement) 
VOID CDL_signal(PCDL cdl) {
	int curr;

#if defined(WITH_MUTEX) || defined(WITH_EVENT)
	// enter critical section 
	WaitForSingleObject(cdl->lock, INFINITE);
	curr = cdl->counter;
	curr--;
	cdl->counter = curr;
#else
	curr = InterlockedDecrement(&cdl->counter);
#endif

	
#ifdef WITH_MUTEX
	// leave critical section 
	ReleaseMutex(cdl->lock);
#else
#ifdef WITH_EVENT
	// leave critical section  
	SetEvent(cdl->lock);
#endif
#endif
	// signal done "event" when "curr" reaches 0	
	if (curr == 0)
		SetEvent(cdl->done);
}

VOID CDL_init(PCDL cdl, int initialCount) {
	cdl->counter = initialCount;
	cdl->done = CreateEvent(NULL, TRUE, FALSE, NULL);
#ifdef WITH_MUTEX
	cdl->lock = CreateMutex(NULL, FALSE, NULL);
#else
#ifdef WITH_EVENT
	cdl->lock = CreateEvent(NULL, FALSE, TRUE, NULL);
#endif
#endif
}

VOID CDL_wait(PCDL cdl) {
	if (cdl->counter > 0)
		WaitForSingleObject(cdl->done, INFINITE);
}

VOID CDL_destroy(PCDL cdl) {
	CloseHandle(cdl->done);

#if defined(WITH_MUTEX) || defined(WITH_EVENT)
	CloseHandle(cdl->lock);
#endif
}