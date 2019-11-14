// Shell.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <windows.h>


HANDLE LaunchProcess(PCHAR cmdLine) {
	PROCESS_INFORMATION pi;
	STARTUPINFOA si = { sizeof(si)  };
	si.lpTitle = "myshell";
	if (!CreateProcess(
		NULL, 
		cmdLine, 
		NULL, 
		NULL, 
		FALSE, 
		0, // CREATE_NEW_CONSOLE, 
		NULL, 
		NULL, 
		&si, 
		&pi)) {
		return NULL;
	}
	
	CloseHandle(pi.hThread);
	return pi.hProcess;
}

int main(int argc, char* argv[]) {
	char line[128];

	printf("> ");
	while (gets_s(line, 128) != NULL) {
		if (line[0] != 0)  {
			if (strcmp(line, "exit") == 0)
				break;
			HANDLE p = LaunchProcess(line);
			if (p == NULL)
				printf("Error %d: execing %s\n", GetLastError(), line); 
			else {
			
				DWORD exitCode;

				WaitForSingleObject(p, INFINITE);
				GetExitCodeProcess(p, &exitCode);
				if (exitCode != 0) printf("Process returned %d\n", exitCode);
				
				CloseHandle(p);   
			}
		}
		printf("> ");
	}
	return 0;
}

