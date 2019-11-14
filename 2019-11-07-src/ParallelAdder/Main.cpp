#include <Windows.h>
#include <stdio.h>


typedef struct _AdderCtx {
	INT *start;
	INT *end;

	LONG partialResult;
} ADDER_CTX, *PADDER_CTX;


DWORD WINAPI AdderFunc(_In_ LPVOID lpParameter) {
	PADDER_CTX pctx = (PADDER_CTX) lpParameter;

	pctx->partialResult = 0L;
	for (INT* s = pctx->start; s < pctx->end; s++) {
		pctx->partialResult += *s;
	}
	return 0;
}

LONG ParallelAdder(INT vals[], INT size) {
	return 0;
}

int main() {
	return 0;
}