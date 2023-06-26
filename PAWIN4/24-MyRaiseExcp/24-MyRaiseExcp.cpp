/*
This is Jimm Chen's code to see WinAPI RaiseException() and its behavior
regarding EXCEPTION_NONCONTINUABLE parameter value. 

[2023-06-26] I see that EXCEPTION_NONCONTINUABLE with EXCEPTION_CONTINUE_EXECUTION
will cause *nested* exception.
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef EXCEPTION_NESTED_CALL
#define EXCEPTION_NESTED_CALL 0x10 // Win10SDK has this, but not on VC2010
#endif

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

typedef int ExcpFilterDecision_t;

void dump_erec_simple(const EXCEPTION_RECORD *perec)
{
	const EXCEPTION_RECORD &erec = *perec;
	printf("EXCEPTION_RECORD:\n");
	printf("  .ExceptionCode    = 0x%X\n", erec.ExceptionCode);
	printf("  .ExceptionFlags   = 0x%X\n", erec.ExceptionFlags);

	// Allocate a piece of space on a stack
	void *stackptr = _alloca(0);
	printf("  Stack-pointer(rough) = [%p] \n", stackptr);
}

ExcpFilterDecision_t my_filter(const EXCEPTION_RECORD *perec, 
	bool filter_want_continue, bool filter_check_nested)
{
	static int s_count = 0;
	s_count++;

	dump_erec_simple(perec);

	if(filter_check_nested)
	{
		if(s_count>=5)
		{
			printf("!!! my_filter() nested calling detected. STOP it.\n");
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	if(filter_want_continue)
		return EXCEPTION_CONTINUE_EXECUTION;
	else
		return EXCEPTION_EXECUTE_HANDLER;
}

void my_raise_work(bool raise_no_continue, 
	bool filter_want_continue, bool filter_check_nested)
{
	printf("my_raise_work() Starts.\n");

	EXCEPTION_POINTERS* pep = nullptr;
	__try
	{
		printf("  Will do RaiseException() %s...\n", 
			raise_no_continue ? "(EXCEPTION_NONCONTINUABLE)" : "");
		
		RaiseException(0xE0004444, 
			raise_no_continue ? EXCEPTION_NONCONTINUABLE : 0,
			0, nullptr);

		printf("  OK. Continues after RaiseException()\n");
	}
	__except(my_filter( (GetExceptionInformation())->ExceptionRecord,
		filter_want_continue, filter_check_nested
		))
	{
		printf("[Caught] in my_raise_work() __except {...}\n");
	}

	printf("my_raise_work() Ends.\n");
}

int main(int argc, char* argv[])
{
	bool raise_no_continue = false;
	if(argc>1 && argv[1][0]=='1')
		raise_no_continue = true;

	bool filter_want_continue = false;
	if(argc>2 && argv[2][0]=='1')
		filter_want_continue = true;

	bool filter_check_nested = false;
	if(argc>3 && argv[3][0]=='1')
		filter_check_nested = true;

	my_raise_work(raise_no_continue, filter_want_continue, filter_check_nested);
}
