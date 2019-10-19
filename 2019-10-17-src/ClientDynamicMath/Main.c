#include <Windows.h>
#include <stdio.h>
#include "../Include/MathDll.h"

typedef int(*AddFunc)(int i1, int i2);


int main() {
	HMODULE mathDll = LoadLibrary("MathDll.dll");
	printf("mathDll mapped at %p\n", mathDll);

	if (mathDll == NULL) {
		printf("error %d loading dll!\n", GetLastError());
		return 1;
	}
	AddFunc add = (AddFunc) GetProcAddress(mathDll, "Add");
	if (add == NULL) {
		printf("error %d getting name on dll!\n", GetLastError());
		return 1;
	}
	int res = add(3, 5);
	printf("3+5=%d\n", res);
	return 0;
}