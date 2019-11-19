#pragma once


typedef struct _cdl {
	LONG counter;
	HANDLE done;
	HANDLE lock;
} CDL, *PCDL;

VOID CDL_signal(PCDL cdl);

VOID CDL_init(PCDL cdl, int initialCount);

VOID CDL_wait(PCDL cdl);
 
VOID CDL_destroy(PCDL cdl);
	 