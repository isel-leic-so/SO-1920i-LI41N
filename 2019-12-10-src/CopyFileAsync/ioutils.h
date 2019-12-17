#pragma once

HANDLE OpenOrCreateWithAsyncModeAccess(LPCSTR fileName, DWORD creationMode);
	 
HANDLE OpenAsync(LPCSTR fileName);
	 

HANDLE CreateAsync(LPCSTR fileName);
	 