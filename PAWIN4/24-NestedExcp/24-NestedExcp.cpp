/*
This is Jimm Chen's code to see how "nested/recursive exception" behaves.
This is a topic that Jeffrey Richter did not state clearly in his book.

But the result is unlucky, EXCEPTION_RECORD.ExceptionRecord is *always* NULL.

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

typedef int ExceptionCode_t;

void ind(int indent)
{
	if(indent>0)
		printf("%*s", indent, "");
}

void dump_erec(const EXCEPTION_RECORD *perec, int indent=0)
{
	const EXCEPTION_RECORD &erec = *perec;

	ind(indent); printf("EXCEPTION_RECORD members:\n");
	ind(indent); printf("  .ExceptionCode    = 0x%X\n", erec.ExceptionCode);
	ind(indent); printf("  .ExceptionFlags   = 0x%X %s\n", 
		erec.ExceptionFlags,
		erec.ExceptionFlags & EXCEPTION_NESTED_CALL ? "(nested)" : ""
		);
	ind(indent); printf("  .ExceptionRecord  = [%p]\n", erec.ExceptionRecord);
	ind(indent); printf("  .ExceptionAddress = [%p]\n", erec.ExceptionAddress);
	ind(indent); printf("  .NumberParameters = %d\n", erec.NumberParameters);
#ifdef _M_IX86
	void *now_esp = nullptr;
	__asm { mov now_esp, esp }
	ind(indent); printf("  ESP = %p\n", now_esp);
#else
	// Allocate a piece of space on a stack
	void *stackptr = _alloca(0);
	ind(indent); printf("  Stack-pointer(rough) = %p \n", stackptr);
#endif
}


ExceptionCode_t my_filter_rebomb(EXCEPTION_RECORD *perec, int divisor)
{
	static int s_seq = 0;
	s_seq++;

	printf("[%d] my_filter_rebomb() starts.\n", s_seq);
	dump_erec(perec);
	printf("\n");

	if(s_seq>=5)
	{
		printf("my_filter_rebomb() STOPs at s_seq=%d !\n", s_seq);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	// will trigger recursive filter evaluation
	int ret = perec->ExceptionCode / divisor; 
	
	printf("my_filter_rebomb(): will not reach this.\n");
	return EXCEPTION_EXECUTE_HANDLER;
}

int rebomb_work(int divisor)
{
	printf("rebomb_work() Starts.\n");

	int retval = -1;
	__try 
	{
		retval = 3 / divisor;
	}
	__except (my_filter_rebomb( (GetExceptionInformation())->ExceptionRecord, divisor ))
	{
		printf("[Caught] in rebomb_work().\n");
	}

	printf("rebomb_work() Ends, retval=%d.\n", retval);
	return retval;
}

//////////////////////////////////////////////////////////////////////////

int my_inner_work(int divisor)
{
	printf("my_inner_work() Starts.\n");

	EXCEPTION_POINTERS* pep = nullptr;
	int retval = -2;
	__try 
	{
		retval = ((int*)nullptr)[3];
	}
	__except ( pep=GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER )
	{
		printf("[Caught] in my_inner_work().\n");
		dump_erec(pep->ExceptionRecord, 9);

		printf("         Will trigger another exception in this __except {...}\n");
		printf("\n");
		retval = 3 / divisor;
	}

	printf("my_inner_work() Ends, retval=%d.\n", retval);
	return retval;
}

int my_outer_work(int divisor)
{
	printf("my_outer_work() Starts.\n");

	EXCEPTION_POINTERS* pep = nullptr;
	int retval = -1;
	__try
	{
		retval = my_inner_work(divisor);
	}
	__except( pep=GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER )
	{
		printf("[Caught] in my_outer_work().\n");
		dump_erec(pep->ExceptionRecord);
	}

	printf("my_outer_work() Ends. retval=%d\n", retval);
	return retval;
}

int main(int argc, char* argv[])
{
	int divisor = 0;
	if(argc>1)
		divisor = 1;

	printf("\n");
	printf("==== rebomb_work ====\n");
	printf("\n");
	int ret = rebomb_work(divisor);

	printf("\n");
	printf("==== my_outer_work ====\n");
	printf("\n");
	ret = my_outer_work(divisor);

	return ret;
}

