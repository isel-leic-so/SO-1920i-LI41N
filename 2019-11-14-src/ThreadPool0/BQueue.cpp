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
	q->hasItems = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	q->size = 0;
}

UTILS_API VOID BQ_Put(PBQUEUE q, LPVOID item) {
	// to complete
}

UTILS_API LPVOID BQ_Get(PBQUEUE q) {
	 
	WaitForSingleObject(q->mutex, INFINITE);

	while (BQ_IsEmpty(q)) {
		ReleaseMutex(q->mutex); 
		// release the mutex since we are enter wait state
		// It is possible that two or more "getters" are in this place exactly
		// before entering on wait for event "hasItems"
		// Since we leaved the mutex it is possible that,say, two "elems"
		// are added into the queue at this exactly moment.
		// If this is the case, only the first "getter" can proceed immediatly.
		// The others, even on the presence of items in the list, will block, since
		// the event memorize only one signal ( the signalization state is binary)
		// Because of this necesssary we cannot use events to synchronize queue access.
		// Enter semaphores...
		WaitForSingleObject(q->hasItems, INFINITE);
		WaitForSingleObject(q->mutex, INFINITE); // reenter critical section
	}
	PQNODE n = (PQNODE) RemoveHeadList(&q->list);
	assert(&n->link != &q->list);

	q->size--;
	ReleaseMutex(q->mutex);

	LPVOID item = n->item;
	destroyNode(n);
	
	return item;
}

BOOL BQ_IsEmpty(PBQUEUE q) {
	return IsListEmpty(&q->list);
}

UTILS_API VOID BQ_Destroy(PBQUEUE q) {
	CloseHandle(q->mutex);
	CloseHandle(q->hasItems);
}