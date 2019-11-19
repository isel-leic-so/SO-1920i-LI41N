#include <Windows.h>
#include "Chrono.h"
#include "cdl.h"
#include <stdio.h>

typedef LONG(*Adder)(int vals[], int size);

typedef struct _AdderCtx {
	INT *start;
	INT *end;

	LONG partialResult;

	HANDLE workDone;
	//int dummy[128];
	PCDL cdl;
} ADDER_CTX, *PADDER_CTX;
 

DWORD WINAPI AdderFunc(_In_ LPVOID lpParameter) {
	PADDER_CTX pctx = (PADDER_CTX) lpParameter;
	LONG sum = 0;
	pctx->partialResult = 0L;
	for (INT* s = pctx->start; s < pctx->end; s++) {
		sum += *s;
	 
	}
	pctx->partialResult = sum;
	return 0;
}

DWORD WINAPI AdderFuncTP(_In_ LPVOID lpParameter) {
	PADDER_CTX pctx = (PADDER_CTX)lpParameter;
	LONG sum = 0;
	pctx->partialResult = 0L;
	for (INT* s = pctx->start; s < pctx->end; s++) {
		sum += *s;

	}
	pctx->partialResult = sum;
	SetEvent(pctx->workDone);
	return 0;
}

DWORD WINAPI AdderFuncTP_CDL(_In_ LPVOID lpParameter) {
	PADDER_CTX pctx = (PADDER_CTX)lpParameter;
	LONG sum = 0;
	pctx->partialResult = 0L;
	for (INT* s = pctx->start; s < pctx->end; s++) {
		sum += *s;

	}
	pctx->partialResult = sum;

	CDL_signal(pctx->cdl); // work done!

	return 0;
}

#define VALS_SIZE (10000*1000)
INT vals[VALS_SIZE];


VOID InitVals(INT vals[], int size) {
	for (int i = 0; i < size; ++i) vals[i] = 1;
}

VOID CtxInit(PADDER_CTX ctx, INT * start, INT *end) {
	ctx->start = start;
	ctx->end = end;
	ctx->partialResult = 0;
}

VOID CtxInitTP(PADDER_CTX ctx, INT * start, INT *end) {
	ctx->start = start;
	ctx->end = end;
	ctx->partialResult = 0;
	ctx->workDone = CreateEvent(NULL, FALSE, FALSE, NULL);
}

VOID CtxInitTP_CDL(PADDER_CTX ctx, INT * start, INT *end, PCDL cdl) {
	ctx->start = start;
	ctx->end = end;
	ctx->partialResult = 0;
	ctx->cdl = cdl;
}


LONG SequentialAdder(INT *vals, INT size) {
	LONG res = 0;
	for (int i = 0; i < size; ++i) {
		res += vals[i]; 
	}
	return res;
}

#define MAX_CORES 64
LONG ParallelAdder(INT vals[], INT size) {
	SYSTEM_INFO si;
	HANDLE threads[MAX_CORES];
	ADDER_CTX ctx[MAX_CORES];

	GetSystemInfo(&si);

	int ncores = si.dwNumberOfProcessors;

	int partialSize = size / ncores;

	int *currStart = vals;
	for (int i = 0; i < ncores; ++i) {
		 
		int *end = (i < ncores - 1) ?
			currStart + partialSize :
			vals + size;

		CtxInit(ctx +i, currStart, end);

		threads[i] = CreateThread(NULL, 0, AdderFunc, ctx + i, 0, NULL);
		currStart += partialSize;
	}

	// collect results
	LONG sum = 0;
	for (int i = 0; i < ncores; ++i) {
		WaitForSingleObject(threads[i], INFINITE);
		sum += ctx[i].partialResult;
		CloseHandle(threads[i]);
	}
	return sum;
}

LONG ParallelAdderTP(INT vals[], INT size) {
	SYSTEM_INFO si;
	ADDER_CTX ctx[MAX_CORES];

	GetSystemInfo(&si);

	int ncores = si.dwNumberOfProcessors;

	int partialSize = size / ncores;

	int *currStart = vals;
	for (int i = 0; i < ncores; ++i) {

		int *end = (i < ncores - 1) ?
			currStart + partialSize :
			vals + size;

		CtxInitTP(ctx + i, currStart, end);

		//threads[i] = CreateThread(NULL, 0, AdderFunc, ctx + i, 0, NULL);
		QueueUserWorkItem(AdderFuncTP, ctx + i, 0);

		currStart += partialSize;
	}


	// collect results
	LONG sum = 0;
	for (int i = 0; i < ncores; ++i) {
		WaitForSingleObject(ctx[i].workDone, INFINITE);
		sum += ctx[i].partialResult;
		CloseHandle(ctx[i].workDone);
	}
	return sum;
}

LONG ParallelAdderTP_CDL(INT vals[], INT size) {
	SYSTEM_INFO si;
	ADDER_CTX ctx[MAX_CORES];
	CDL cdl;

	GetSystemInfo(&si);

	//int ncores = si.dwNumberOfProcessors;
	int ncores = 50;

	int partialSize = size / ncores;

	CDL_init(&cdl, ncores);

	int *currStart = vals;
	for (int i = 0; i < ncores; ++i) {

		int *end = (i < ncores - 1) ?
			currStart + partialSize :
			vals + size;

		CtxInitTP_CDL(ctx + i, currStart, end, &cdl);

		//threads[i] = CreateThread(NULL, 0, AdderFunc, ctx + i, 0, NULL);
		QueueUserWorkItem(AdderFuncTP_CDL, ctx + i, 0);

		currStart += partialSize;
	}


	// collect results
	CDL_wait(&cdl); // wait for them

	LONG sum = 0;
	for (int i = 0; i < ncores; ++i) {
		sum += ctx[i].partialResult;
	}

	CDL_destroy(&cdl);

	return sum;
}
#define NTRIES 8
LONG Test(Adder adder, const char *msg, INT vals[], DWORD size) {
	printf("Test of: %s\n", msg);
	Chronometer chrono;
	__int64 minTime = LLONG_MAX;
	LONG res;

	
	for (int t = 0; t < NTRIES; ++t) {
		
		chrono.Start();

		res = adder(vals, size);
		chrono.Stop();
		if (chrono.GetMicros() < minTime)
			minTime = chrono.GetMicros();
	}

	printf("Total=%ld, Done in %I64d micros!\n\n", res, minTime);
	return res;
}

int main() {
 
	InitVals(vals, VALS_SIZE);

	Test(SequentialAdder, "Sequential Adder", vals, VALS_SIZE);
	
	Test(ParallelAdder, "Parallel Adder", vals, VALS_SIZE);
 
	Test(ParallelAdderTP, "Parallel Adder TP", vals, VALS_SIZE);

	Test(ParallelAdderTP_CDL, "Parallel Adder TP with CDL", vals, VALS_SIZE);

}