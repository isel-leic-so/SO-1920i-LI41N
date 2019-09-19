// SyscallCost.cpp : Defines the entry point for the console application.
//

// Evaluate system call cost.
// Why the difference between GetSystemInfo cycle
// and GetTickCount cycle?
// JM - 2018

// need to include in order to use windows API
#include <windows.h>

#define TRIES 10000000
int main() {
	SYSTEM_INFO si;
	int ntries = TRIES;

 
	int startTime = GetTickCount();
	while (ntries > 0) {
		GetSystemInfo(&si);
	 
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

