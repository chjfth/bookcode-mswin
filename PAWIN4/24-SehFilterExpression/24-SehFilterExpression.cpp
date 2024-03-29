#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

DWORD g_temp = 0;
DWORD g_ExceptionCode = 0;

extern"C" __declspec(noinline)
int seh_continue_search(void*)
{
	return EXCEPTION_CONTINUE_SEARCH;
}

extern"C"
int see_asm_GetExceptionInformation(bool is_abnormal)
{
	int divisor = is_abnormal ? 0 : 1;
	int iret = -1;
	__try {
		iret = 3 / divisor;
	}
	__except (seh_continue_search(GetExceptionInformation())) {
		printf("In __except body GEI.\n");
	}

	return iret;
}

extern"C"
int see_asm_AbnormalTermination(bool is_abnormal)
{
	int iret = -1;
	char array[0x100] = {};
	__try {
		if (is_abnormal)
			*(int*)nullptr = 0x4444;
	}
	__finally {
		iret = AbnormalTermination();
		g_temp = iret; // so that we can always inspect iret's value
	}

	return iret;
}

//////////////////////////////////////////////////////////////////////////

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

void test_GetExceptionCode(int dividend, int divisor)
{
	printf("==== test_GetExceptionCode ====\n");

	int result = 0;
	__try {
		result = dividend / divisor;
		
		((int*)nullptr)[result] = 0x4444;
	}
	__except (CoffeeFilter(GetExceptionCode())) {
		// Handle the exception.
		printf("[A][Caught!] exception code = 0x%X\n", g_ExceptionCode);
	} 

	printf("[A]result = %d\n", result);
	printf("\n");
}

//////////////////////////////////////////////////////////////////////////

void FuncSkunk(int dividend, int divisor) 
{
	// Declare variables that we can use to save [the exception-record 
	// and the context-record] if an exception should occur.
	
	EXCEPTION_RECORD SavedExceptRec;
	CONTEXT SavedContext;

	int result = 0;
	__try {

		result = dividend / divisor;

		((int*)nullptr)[result] = 0x4444;
	}
	__except (
		SavedExceptRec = *(GetExceptionInformation())->ExceptionRecord,
		SavedContext = *(GetExceptionInformation())->ContextRecord,
		EXCEPTION_EXECUTE_HANDLER
		) 
	{
		printf("[B][Caught!] EXCEPTION_RECORD members:\n");
		printf("  .ExceptionCode    = 0x%X\n", SavedExceptRec.ExceptionCode);
		printf("  .ExceptionFlags   = 0x%X\n", SavedExceptRec.ExceptionFlags);
		printf("  .ExceptionRecord  = @[%p]\n", SavedExceptRec.ExceptionRecord);
		printf("  .ExceptionAddress = @[%p]\n", SavedExceptRec.ExceptionAddress);
		printf("  .NumberParameters = %d\n", SavedExceptRec.NumberParameters);
		for(int i=0; i<(int)SavedExceptRec.NumberParameters; i++)
		{
			printf("    [%d] ptr[%p]\n", i, (void*)SavedExceptRec.ExceptionInformation[i]);
		}
	}

	printf("[B]result = %d\n", result);
}

void test_GetExceptionInformation(int dividend, int divisor)
{
	printf("==== test_GetExceptionInformation ====\n");
	
	FuncSkunk(dividend, divisor);

	printf("\n");
}

int main(int argc, char* argv[])
{
	int param = 0;
	if(argc>1)
		param = atoi(argv[1]);

	see_asm_GetExceptionInformation(false);

	__try {
		see_asm_GetExceptionInformation(true);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		// just catch it an go on
	}

	////
	
	see_asm_AbnormalTermination(false);

	__try {
		see_asm_AbnormalTermination(true);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		// just catch it an go on
	}

	////
	
	// p664j: CoffeeFilter
	test_GetExceptionCode(3, param);

	// p666: FuncSkunk()
	test_GetExceptionInformation(3, param);

	return 0;
}

/*
chj@WIN7EVN-PC [2023/06/25 21:06:50.04] d:\test
> D:\gitw\bookcode-mswin\PAWIN4\bin-v100\Win32\Debug\24-SehFilterExpression.exe
==== test_GetExceptionCode ====
[A][Caught!] exception code = 0xC0000094
[A]result = 0

==== test_GetExceptionInformation ====
[B][Caught!] EXCEPTION_RECORD members:
  .ExceptionCode    = 0xC0000094
  .ExceptionFlags   = 0x0
  .ExceptionRecord  = @[00000000]
  .ExceptionAddress = @[0040129E]
  .NumberParameters = 0
[B]result = 0


chj@WIN7EVN-PC [2023/06/25 21:06:55.15] d:\test
> D:\gitw\bookcode-mswin\PAWIN4\bin-v100\Win32\Debug\24-SehFilterExpression.exe 1
==== test_GetExceptionCode ====
[A][Caught!] exception code = 0xC0000005
[A]result = 3

==== test_GetExceptionInformation ====
[B][Caught!] EXCEPTION_RECORD members:
  .ExceptionCode    = 0xC0000005
  .ExceptionFlags   = 0x0
  .ExceptionRecord  = @[00000000]
  .ExceptionAddress = @[004012AD]
  .NumberParameters = 2
    [0] ptr[00000001]
    [1] ptr[0000000C]
[B]result = 3

*/
