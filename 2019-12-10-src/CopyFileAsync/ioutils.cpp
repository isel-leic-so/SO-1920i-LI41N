#include <windows.h>
#include "ioutils.h"

/*
* Auxiliary functions
*/

HANDLE OpenOrCreateWithAsyncModeAccess(LPCSTR fileName, DWORD creationMode) {
	HANDLE hFile = CreateFile(fileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		creationMode,
		FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	return hFile;
}

HANDLE OpenAsync(LPCSTR fileName) {
	return OpenOrCreateWithAsyncModeAccess(fileName, OPEN_EXISTING);
}

HANDLE CreateAsync(LPCSTR fileName) {
	return OpenOrCreateWithAsyncModeAccess(fileName, CREATE_ALWAYS);
}

