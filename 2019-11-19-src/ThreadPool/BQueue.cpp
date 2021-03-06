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

UTILS_API VOID BQ_Init(PBQUEUE q, INT capacity) {
	InitializeListHead(&q->list);
	q->mutex = CreateMutex(NULL, FALSE, NULL);
	q->hasItems = CreateSemaphore(NULL, 0, capacity, NULL);
	q->hasSpace = CreateSemaphore(NULL, capacity, capacity, NULL);
	q->maxSize = capacity;

}

UTILS_API VOID BQ_Put(PBQUEUE q, LPVOID item) {
	PQNODE pnode = createNode(item);

	WaitForSingleObject(q->hasSpace, INFINITE);

	WaitForSingleObject(q->mutex, INFINITE);

	InsertTailList(&q->list, &pnode->link);
	ReleaseMutex(q->mutex);

	ReleaseSemaphore(q->hasItems, 1, NULL /* to get previous count*/);
}

UTILS_API LPVOID BQ_Get(PBQUEUE q) {
	 
	
	WaitForSingleObject(q->hasItems, INFINITE);

	// entrada em exclus�o para obter o elemento � cabe�a da fila
	WaitForSingleObject(q->mutex, INFINITE);

	PQNODE n = (PQNODE) RemoveHeadList(&q->list);
	assert(&n->link != &q->list);
	
	ReleaseMutex(q->mutex);

	ReleaseSemaphore(q->hasSpace, 1, NULL);

	LPVOID item = n->item;
	destroyNode(n);
	
	 
	return item;
}

BOOL BQ_IsEmpty(PBQUEUE q) {
	return IsListEmpty(&q->list);
}