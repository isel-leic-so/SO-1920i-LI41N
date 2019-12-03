// PhilosophersV3.cpp : Defines the entry point for the console application.
//
// Nesta versão tira-se partido do seguinte princípio:
//
// Se numerarmos os recursos de 0 a N-1 (ou qualquer outra numeração crescente)
// não pode haver deadlock se os recursos forem pedidos pela mesma ordem por todos os intervenientes
// Para o conseguir troca-se a lógica de obtenção do filósofo 4 (admitindo que são 5).
//


#include <Windows.h>
#include <stdio.h>
#include <process.h>

#define NSTICKS 5

CRITICAL_SECTION  sticks[NSTICKS];

UINT WINAPI Philosopher(LPVOID arg) {
	DWORD i = (DWORD)arg;
	/*_tprintf(_T("filosofo  %d\n"), i);*/
	int n = 1;
	while (TRUE) {
		n++;
		if (n % 1000 == 0) printf("%d ok!\n", i);
		// try to eat...
		if (i == NSTICKS - 1) {
			EnterCriticalSection(&sticks[0]);
			//Sleep(10);
			EnterCriticalSection(&sticks[i] );
		}
		else {
			EnterCriticalSection(&sticks[i] );
			//Sleep(10);
			EnterCriticalSection(&sticks[(i + 1) % NSTICKS] );
		}

		// eat...

		printf("Philosofer %d eating...\n", i);
		//Sleep( 2 + rand() % 30 /*2000+ (rand() % 3000)*/);
		// finish eating...

		LeaveCriticalSection(&sticks[i]);
		LeaveCriticalSection(&sticks[(i + 1) % NSTICKS]);

		// thinking ...
		printf("Philosofer %d thinking...\n", i);
		//Sleep( 2 + rand() % 30 /*2000+ (rand() % 3000)*/);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	int i = 0;


	/* inicialize sticks */
	for (i = 0; i < NSTICKS; ++i)
		 InitializeCriticalSection(&sticks[i]);


	/* create Philosophers */
	for (i = 0; i < NSTICKS; ++i)
		_beginthreadex(NULL, 0, Philosopher, (LPVOID)i, 0, NULL);

	getchar();

	for (i = 0; i < NSTICKS; ++i)
		DeleteCriticalSection(&sticks[i]);
	return 0;
}


