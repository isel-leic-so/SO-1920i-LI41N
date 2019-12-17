// CopyFileAsync.cpp : Defines the entry point for the console application.
//
// Assynchronous file copy with overlapped I/O and multiple buffers with event synchronization
//
// JM, 2015

#include <windows.h>
#include <stdio.h>

#include "ioutils.h"

#define BUFFER_SIZE (4096*16)
#define NOPERS 4

/* I/O operation type */
typedef enum OperType { READ, WRITE } OperType;

typedef struct Operation {
	OVERLAPPED overlapped;
	OperType type;
	BYTE buffer[BUFFER_SIZE];
} Operation;


/*
* Copy global state
*/

typedef struct copy_ctx {

	/* file handles */
	HANDLE fIn;
	HANDLE fOut;

	/* Input file size */
	LARGE_INTEGER inFileSize;
	LARGE_INTEGER currInPos;

	/* pending operations */
	int readOpers, writeOpers;
	
	/* total operations */
	int nOpers;

	/* parallel operations */
	Operation opers[NOPERS];


	/* array of handles for WaitForMultipleObjects */
	HANDLE waitOn[NOPERS];
} CopyCtx, *PCopyCtx;




/*
* Start I/O operations
* Mantain the copy process while operations are done synchronously
*/
BOOL DoIO(PCopyCtx ctx, int i, DWORD nBytes) {
	DWORD res;
	DWORD nTransfer;
	
	ctx->nOpers++;
	while (TRUE)  {

		switch (ctx->opers[i].type) {
		case READ:
			if (ctx->currInPos.QuadPart >= ctx->inFileSize.QuadPart)
				return TRUE;


			ctx->readOpers++;
			ctx->opers[i].overlapped.Offset = ctx->currInPos.LowPart;
			ctx->opers[i].overlapped.OffsetHigh = ctx->currInPos.HighPart;

			ctx->currInPos.QuadPart += nBytes; // adjust input position
			if (!ReadFile(ctx->fIn, ctx->opers[i].buffer, nBytes, 
				&nTransfer, &ctx->opers[i].overlapped)) {
				if ((res = GetLastError()) != ERROR_IO_PENDING) {
					printf("Error %d reading file at offset %I64d!\n", res, ctx->currInPos.QuadPart);
					return FALSE;
				}
				return TRUE;
			}

			printf("Synchronous read!\n");
			ctx->readOpers--;
			ctx->opers[i].type = WRITE;

			break;
		case WRITE:
			ctx->writeOpers++;
			if (!WriteFile(ctx->fOut, ctx->opers[i].buffer, nBytes,
				&nTransfer, &ctx->opers[i].overlapped)) {
				if ((res = GetLastError()) != ERROR_IO_PENDING) {
					printf("Error %d writing file at ofsfet %I64d!\n", res, ctx->currInPos.QuadPart);
					return FALSE;
				}
				return TRUE;
			}
			ctx->writeOpers--;
			printf("Synchronous write!\n");
			ctx->opers[i].type = READ;
			break;
		}

	}
}

/*
  Called when an input operation is concluded
*/
BOOL AsyncReadDone(PCopyCtx ctx, int i) {
	// DWORD used for the number of bytes transfered in I/O operation.
	DWORD nTransfered;

	ctx->readOpers--;

	if (!GetOverlappedResult(ctx->fIn, &ctx->opers[i].overlapped,
		&nTransfered, FALSE)) {
		printf("Error %d getting read result!\n", GetLastError());
		return FALSE;
	}
	ctx->opers[i].type = WRITE;
	return DoIO(ctx, i, nTransfered);
}

/*
Called when an output operation is concluded
*/
BOOL AsyncWriteDone(PCopyCtx ctx, int i) {
	DWORD nTransfered;

	if (!GetOverlappedResult(ctx->fOut, &ctx->opers[i].overlapped, &nTransfered, FALSE)) {
		printf("Error %d getting write result!\n", GetLastError());
		return FALSE;
	}

	ctx->writeOpers--;
	ctx->opers[i].type = READ;
	DoIO(ctx, i, BUFFER_SIZE);
	return TRUE;
}

VOID OperInit(Operation *oper) {
	ZeroMemory(&oper->overlapped, sizeof(OVERLAPPED));
	oper->overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	oper->type = READ;
}

VOID StartCopy(PCopyCtx ctx, HANDLE fin, HANDLE fout, LARGE_INTEGER finSize) {
	ctx->fIn = fin;
	ctx->fOut = fout;
	ctx->nOpers = 0;
	ctx->inFileSize = finSize;

	ctx->currInPos.QuadPart = 0;

	ctx->readOpers = ctx->writeOpers = 0;
	
	// Start copy
	for (int i = 0; i < NOPERS; ++i) {
		OperInit(ctx->opers + i);
		ctx->waitOn[i] = ctx->opers[i].overlapped.hEvent;
		DoIO(ctx, i,  BUFFER_SIZE);
	}

}

VOID EndCopy(PCopyCtx ctx) {
	for (int i = 0; i < NOPERS; ++i) {
		CloseHandle(ctx->opers[i].overlapped.hEvent);
	}
}

//
// AsyncCopy implementation
//
BOOL CopyAsync(LPCSTR inName, LPCSTR outName) {
	
	HANDLE fin = NULL, fout = NULL;
	CopyCtx ctx;
	LARGE_INTEGER finSize;

	// Open the input file for asynchronous file I/O.
	fin = OpenAsync(inName);
	if (fin == INVALID_HANDLE_VALUE) {
		printf("Error %d opening file!\n", GetLastError());
		return FALSE;
	}

	// Create the output file for asynchronous file I/O.
	fout = CreateAsync(outName);
	if (fout == INVALID_HANDLE_VALUE) {
		printf("Error %d creating file!\n", GetLastError());
		CloseHandle(fin);
		return 1;
	}

	if (!GetFileSizeEx(fin, &finSize)) {
		printf("Error %d getting input file size!\n", GetLastError());
		return FALSE;
	}

	 
	if (!SetFilePointerEx(fout, finSize, NULL, FILE_BEGIN))
	{
		printf("Error %d setting output file size!\n", GetLastError());
		return FALSE;
	}

	SetEndOfFile(fout);
	 


	StartCopy(&ctx, fin, fout, finSize);
	

	// Start reading the data asynchronously.
	do {
		DWORD res = WaitForMultipleObjects(NOPERS, ctx.waitOn, 
			FALSE, INFINITE);
	 
		if (res < WAIT_OBJECT_0 || 
			res >= WAIT_OBJECT_0 + NOPERS) {
			printf("wait error %d copying file!\n", GetLastError());
			break;
		}

		res -= WAIT_OBJECT_0;
		if (ctx.opers[res].type == READ) { // partial input done
			//printf("partial read done at %d!\n", overlappedIn[res].Offset);
			if (!AsyncReadDone(&ctx, res))
				break;
		}
		else { // partial write done
			//printf("partial write done at %d!\n", overlappedOut[res].Offset);
			if (!AsyncWriteDone(&ctx, res))
				break;
		}
	} while (ctx.writeOpers > 0 || ctx.readOpers > 0);
	
	CloseHandle(fin);
	CloseHandle(fout);

	EndCopy(&ctx);
	return TRUE;
}


int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("usage: asyncopy <fin> <fout>\n");
		return 1;
	}

	LONGLONG start = GetTickCount64();
	if (CopyAsync(argv[1], argv[2])) {
		printf("Done in %I64ums!\n", GetTickCount64() - start);
	}
	return 0;
}


