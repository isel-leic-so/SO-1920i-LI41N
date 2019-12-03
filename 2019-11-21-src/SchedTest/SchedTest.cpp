// SchedTest.cpp : Defines the entry point for the console application.
//
#include <stdio.h>

#include <Windows.h>
#include <process.h>

typedef  unsigned(__stdcall *thread_func)(void *);

#define CACHE_ALIGN 256
volatile  __int64 __declspec(align(CACHE_ALIGN))  LowPriorityCounter;
 
volatile __int64 __declspec(align(CACHE_ALIGN)) HighPriorityCounter1;

volatile __int64 __declspec(align(CACHE_ALIGN)) HighPriorityCounter2;
 
volatile BOOL __declspec(align(CACHE_ALIGN)) testEnd;

HANDLE CreateSuspendedThreadAtPriority(thread_func func, DWORD priority, LPVOID arg)
{
	HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, func,
		arg, CREATE_SUSPENDED, NULL);
	if (thread != NULL) 
		SetThreadPriority(thread, priority);
	
	return thread;

}
 

UINT WINAPI LowPriorityThread(LPVOID arg) {
	while (!testEnd) {
		LowPriorityCounter++;
	
	}
	return 0;
}

UINT WINAPI HighPriorityThread(LPVOID arg)
{
	int id = (INT)arg;
	while (!testEnd)  {
		if (id == 1) HighPriorityCounter1++;
		else HighPriorityCounter2++;
		printf("High Priority (%d)!\n", GetThreadPriority(GetCurrentThread()));
	}
	 
	return 0;
}

int main(int argc, char* argv[])
{
#define NTHREADS 3	 
	HANDLE threads[NTHREADS];

	
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	SetProcessAffinityMask(GetCurrentProcess(), 1);

	

	threads[0] =
		CreateSuspendedThreadAtPriority(LowPriorityThread, THREAD_PRIORITY_LOWEST, NULL);
	 
	threads[1] =
		CreateSuspendedThreadAtPriority(HighPriorityThread, THREAD_PRIORITY_HIGHEST,
			(LPVOID) 2);
 
	threads[2] =
		CreateSuspendedThreadAtPriority(HighPriorityThread, THREAD_PRIORITY_HIGHEST,
			(LPVOID)1);

	
	for(int i=0; i < NTHREADS; ++i)
		ResumeThread(threads[i]);
	 
	
	Sleep(3000);
	testEnd = TRUE;
	
	WaitForMultipleObjects(NTHREADS, threads, TRUE, INFINITE);
 
	for (int i = 0; i < NTHREADS; ++i)
		CloseHandle(threads[i]);

	printf("LowPriorityCounter: %I64d, HighPriorityCounters: (%I64d, %I64d)\n",
		LowPriorityCounter, HighPriorityCounter1, HighPriorityCounter2);
	return 0;
}

