#include <Windows.h>
#include <stdio.h>
#include "../Include/MathDll.h"

int main() {
	LONG res;
	res = Add(3, 5);
	printf("3+5=%ld\n", res);

	
	return 0;
}