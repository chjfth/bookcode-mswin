#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

DWORD g_ExceptionCode = 0;

LONG CoffeeFilter (DWORD dwExceptionCode) 
{
	g_ExceptionCode = dwExceptionCode;

	return (dwExceptionCode == 
			EXCEPTION_ACCESS_VIOLATION      // 0xC0000005
			|| EXCEPTION_INT_DIVIDE_BY_ZERO // 0xC0000094
			) 
		? EXCEPTION_EXECUTE_HANDLER 
		: EXCEPTION_CONTINUE_SEARCH;
}

void test_GetExpressionCode(int dividend, int divisor)
{
	int result1 = 0;
	__try {
		result1 = dividend / divisor;
		
		((int*)nullptr)[result1] = 0x4444;
	}
	__except (CoffeeFilter(GetExceptionCode())) {
		// Handle the exception.
		printf("[Caught1!] exception code = 0x%X\n", g_ExceptionCode);
	} 

	printf("result1 = %d\n", result1);
}

int main(int argc, char* argv[])
{
	int param = 0;
	if(argc>1)
		param = atoi(argv[1]);

	// p664j: CoffeeFilter
	test_GetExpressionCode(3, param);
	/*
	> D:\...\24-SehFilterExpression.exe
	[Caught1!] exception code = 0xC0000094

	> D:\...\24-SehFilterExpression.exe 1
	[Caught1!] exception code = 0xC0000005
	*/

	return 0;
}

