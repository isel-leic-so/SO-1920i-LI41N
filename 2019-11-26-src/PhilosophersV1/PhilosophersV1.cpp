 // PhilosophersV1.cpp : Defines the entry point for the console application.
//
// Pseudo-solução obtendo um "pauzinho" de cada vez..
// pode gerar deadlock!
//
// Descomente as chamadas a SwitchToThread para ver o efeito.
//
// Pode-se pensar em embrulhar numa secção crítica a obtenção dos 2 pauzinhos
// Tal solução não permite o máximo paralelismo pois inibe outros de obter pauzinhos
// se alguem ficar bloqueado na secção crítica!
//
 
#include <Windows.h>
#include <process.h>
#include <stdio.h>

#define NSTICKS 5
CRITICAL_SECTION sticks[NSTICKS];
HANDLE  philosophers[NSTICKS];

UINT WINAPI Philosopher(LPVOID arg) {
	DWORD i = (DWORD)arg;
	/*_tprintf(_T("filosofo  %d\n"), i);*/
	int n = 1;
	while (TRUE) {
		n++;
		if (n % 100 == 0) printf("%d ok!\n", i); 
		// try to eat...
		//printf("Philosopher %d try to eat...\n", i);
		EnterCriticalSection(&sticks[i]);
		//SwitchToThread();
		//Sleep(10);
		EnterCriticalSection(&sticks[(i + 1) % NSTICKS]);

		// eat...
		//printf("Philosofer %d eating...\n", i);
		//Sleep(20+ (rand() % 300));

		// finish eating...
		LeaveCriticalSection(&sticks[i]);
		LeaveCriticalSection(&sticks[(i + 1) % NSTICKS]);

		// thinking ...
		//printf("Philosopher %d thinking...\n", i);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	int i = 0;

	//SetProcessAffinityMask(GetCurrentProcess(), 1);
	/* inicialize sticks */
	for (i = 0; i < NSTICKS; ++i)
		 InitializeCriticalSection(&sticks[i]);

	/* create Philosophers */
	for (i = 0; i < NSTICKS; ++i) {
		philosophers[i] =
			(HANDLE)_beginthreadex(NULL, 0, Philosopher, (LPVOID)i, 
			0, NULL);
	}
		

	getchar();

	for (i = 0; i < NSTICKS; ++i)
		DeleteCriticalSection(&sticks[i]);
	return 0;
}

