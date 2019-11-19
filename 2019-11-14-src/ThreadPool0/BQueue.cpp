#include "stdafx.h"
#include <stdlib.h>
#include <assert.h>
#include <crtdbg.h>

#include "Include/BQueue.h"

typedef struct {
	LIST_ENTRY link;
	LPVOID item;
} QNODE, *PQNODE;

static PQNODE createNode(LPVOID item) {
	PQNODE n = (PQNODE)malloc(sizeof(QNODE));
	n->item = item;
	return n;
}

static VOID destroyNode(PQNODE n) {
	free(n);
}

#define MAXITEMS 20000

UTILS_API VOID BQ_Init(PBQUEUE q) {
	InitializeListHead(&q->list);
	q->mutex = CreateMutex(NULL, FALSE, NULL);
	q->hasItems = CreateEvent(NULL, FALSE, FALSE, NULL);
	q->hasSpace = CreateEvent(NULL, FALSE, TRUE, NULL);
	// .... to complete
}

UTILS_API VOID BQ_Put(PBQUEUE q, LPVOID item) {
	// to complete
}

UTILS_API LPVOID BQ_Get(PBQUEUE q) {
	 
	WaitForSingleObject(q->mutex, INFINITE);

	while (BQ_IsEmpty(q)) {
		ReleaseMutex(q->mutex); // release the mutex since we are enter wait state
		WaitForSingleObject(q->hasItems, INFINITE);
		WaitForSingleObject(q->mutex, INFINITE); // reenter critical section
	}
	PQNODE n = (PQNODE) RemoveHeadList(&q->list);
	assert(&n->link != &q->list);
	ReleaseMutex(q->mutex);

	LPVOID item = n->item;
	destroyNode(n);
	
	SetEvent(q->hasSpace);

	return item;
}

BOOL BQ_IsEmpty(PBQUEUE q) {
	return IsListEmpty(&q->list);
}