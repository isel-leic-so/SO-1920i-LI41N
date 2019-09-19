// SyscallCost.cpp : Defines the entry point for the console application.
//

// Evaluate system call cost.
// Why the difference between GetSystemInfo cycle
// and GetCurrentProcessId cycle?
// JM - 2019

// need to include in order to use windows API
#include <windows.h>

void   NtQueryTimerResolution(  ULONG * MinimumResolution,
								ULONG * MaximumResolution,
							    ULONG * CurrentResolution);
								
#define TRIES 10000000
int main() {
	SYSTEM_INFO si;
	int ntries = TRIES;
	ULONG MinimumResolution=0;
	ULONG MaximumResolution=0;
	ULONG CurrentResolution=0;

	
	
	int startTime = GetTickCount();
	while (ntries > 0) {
		GetSystemInfo(&si);
		//NtQueryTimerResolution( &MinimumResolution,&MaximumResolution,&CurrentResolution);
 
		ntries--;
	}
	
	ntries = TRIES;
	double total = GetTickCount() - startTime;
	printf("Total GetSystemInfo calls done in %dms\n", (int) total);
	printf("(%lf micros per call!)\n", (total*1000)/TRIES);

 
	startTime = GetTickCount();
	while (ntries > 0) {
		GetCurrentProcessId();
		ntries--;
	}

	total = GetTickCount() - startTime;
	printf("\nTotal GetTickCount calls done in %dms\n", (int) total);
	printf("(%lf micros per call!)\n", (total*1000)/TRIES);
    return 0;
}

