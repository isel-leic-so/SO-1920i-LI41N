#include <Windows.h>
#include <stdio.h>

#define BUF_SIZE (64*1024)

BOOL CopySync(LPCSTR inName, LPCSTR outName) {
	BYTE buf[BUF_SIZE];

	HANDLE fin = NULL, fout = NULL;

	if ((fin = CreateFile(
		inName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL)) == INVALID_HANDLE_VALUE) return NULL;

	if ((fout = CreateFile(
		outName,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL)) == INVALID_HANDLE_VALUE) {
		CloseHandle(fin);
		return NULL;
	}

	DWORD read, written;

	// do copy
	BOOL res;
	while ((res=ReadFile(fin, buf, BUF_SIZE, &read, NULL)) && read > 0) {
		WriteFile(fout, buf, read, &written, NULL);
	}

	CloseHandle(fin);
	CloseHandle(fout);

	return TRUE;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("usage: asyncopy <fin> <fout>\n");
		return 1;
	}

	LONGLONG start = GetTickCount64();
	if (CopySync(argv[1], argv[2])) {
		printf("Done in %I64ums!\n", GetTickCount64() - start);
	}
	return 0;
}