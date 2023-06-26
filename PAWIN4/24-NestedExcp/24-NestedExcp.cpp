#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

typedef int ExceptionCode_t;

void dump_erec(const EXCEPTION_RECORD *perec)
{
	const EXCEPTION_RECORD &erec = *perec;

	printf("EXCEPTION_RECORD members:\n");
	printf("  .ExceptionCode    = 0x%X\n", erec.ExceptionCode);
	printf("  .ExceptionFlags   = 0x%X\n", erec.ExceptionFlags);
	printf("  .ExceptionRecord  = [%p]\n", erec.ExceptionRecord);
	printf("  .ExceptionAddress = [%p]\n", erec.ExceptionAddress);
	printf("  .NumberParameters = %d\n", erec.NumberParameters);
#ifdef _M_IX86
	void *now_esp = nullptr;
	__asm { mov now_esp, esp }
	printf("  ESP = %p\n", now_esp);
#else
	// Allocate a piece of space on a stack
	void *stackptr = _alloca(0);
	printf("  Stack-pointer(rough) = %p \n", stackptr);
#endif
	printf("\n");
}


ExceptionCode_t my_filter_rebomb(EXCEPTION_RECORD *perec, int divisor)
{
	static int s_seq = 0;
	s_seq++;

	printf("[%d] my_filter_rebomb() starts.\n", s_seq);
	dump_erec(perec);

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
/*
ExceptionCode_t my_outer_filter(EXCEPTION_RECORD *erec)
{

	return EXCEPTION_EXECUTE_HANDLER;
}

int my_outer_work(int divisor)
{
	printf("my_outer_work() Starts.\n");

	int retval = -1;
	__try
	{
		retval = my_inner_work(divisor);
	}
	__except(my_outer_filter( (GetExceptionInformation())->ExceptionRecord ))
	{
		printf("[Caught] in my_outer_work().\n");
	}

	printf("my_outer_work() Ends. retval=%d\n", retval);
	return retval;
}
*/
int main(int argc, char* argv[])
{
	int divisor = 0;
	if(argc>1)
		divisor = 1;

	printf("\n");
	printf("==== rebomb_work ====\n");
	printf("\n");
	int ret = rebomb_work(divisor);

	return ret;
}

