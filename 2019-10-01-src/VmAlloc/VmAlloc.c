// src.cpp : Defines the entry point for the console application.
//
// Checking the allocation in two separated phases: reserve, then committ
//
// Check system change with task manager and resource monitor.
//
// JM, 2019
//

#include <windows.h>
#include <stdio.h>
#include <assert.h>

// 512 MBytes allocation!
#define MEM_SIZE (512*1024*1024)
 

int main(int argc, char* argv[])
{
	SYSTEM_INFO inf;
	DWORD i;

	// Just show the minimum reserve size (granularity size) end page size
	GetSystemInfo(&inf);
	 
	printf("region minimum size=%X\n", inf.dwAllocationGranularity);
	printf("page size=%X\n", inf.dwPageSize);

	printf("Start: press return to continue..."); getchar();

	LPBYTE buf1 = (LPBYTE) VirtualAlloc(NULL, MEM_SIZE, MEM_RESERVE, 
										PAGE_READWRITE);
	if (buf1 == NULL) {
		printf("Error %d reserving region!\n", GetLastError());
		return 0;
	}

	printf("Reserved: press return to continue...");
	getchar();

	LPBYTE buf2 = (LPBYTE) VirtualAlloc(buf1, MEM_SIZE, MEM_COMMIT, 
		PAGE_READWRITE);
	// buf1 and buf2 must be the same!
	assert(buf1 == buf2);
	printf("buf1=%p, buf2=%p\n", buf1, buf2);
	printf("Commited: press return to continue...");
	getchar();

	for(i=0; i < MEM_SIZE/inf.dwPageSize; ++i)
		buf1[i*inf.dwPageSize] = (BYTE) i;
	printf("Changed: press return to continue...");
	getchar();

	VirtualFree(buf1, 0, MEM_DECOMMIT);
	printf("Decommited: press return to continue...");
	getchar();

	VirtualFree(buf1, 0, MEM_RELEASE);
	printf("Released: press return to continue...");
	getchar();

	return 0;
}


