#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

DWORD g_dwProtectedData = 0;

void FuncOStimpy1() 
{
	void FuncORen1();

	// 1. Do any processing here.
	printf("1. [FuncOStimpy1] Starts.\n");


	__try {
		// 2. Call another function.
		printf("2. [FuncOStimpy1] calling FuncORen1().\n");

		FuncORen1();

		// Code here never executes.
	}

	__except ( /* 6. Evaluate filter. */ EXCEPTION_EXECUTE_HANDLER) {
		
		printf("8. [FuncOStimpy1] After the unwind, the exception handler executes.\n");
		
	}

	// 9. Exception handled -- continue execution.
	printf("9. [FuncOStimpy1] Exception handled -- continue execution\n");
}

void FuncORen1() 
{
	DWORD dwTemp = 0;

	// 3. Do any processing here.
	printf("3. [FuncORen1] Starts.\n");

	__try { 
		// 4. Request permission to access protected data.
		
		printf("4. [FuncORen1] WaitForSingleObject(g_hSem, INFINITE);\n");

		// 5. Modify the data.
		//    An exception is generated here.
		printf("5. [FuncORen1] Triggering divide-by-zero exception.\n");

		g_dwProtectedData = 5 / dwTemp;
	}
	__finally {
		// 7. Global unwind occurs because filter evaluated
		//    to EXCEPTION_EXECUTE_HANDLER.

		// Allow others to use protected data.
		printf("7. [FuncORen1] ReleaseSemaphore(g_hSem, 1, NULL);\n");
	}

	// Continue processing -- never executes.
}

void test_EXCEPTION_EXECUTE_HANDLER()
{
	// c*p650
	printf("==== FuncOStimpy1 ====\n");
	FuncOStimpy1();
	printf("\n");
}

//////////////////////////////////////////////////////////////////////////

void FuncPheasant() 
{
	printf("[FuncPheasant] Starts.\n");

	__try {
		printf("[FuncPheasant] In __try{...}, triggerring nullptr exception)\n");
		strcpy(NULL, NULL);
	}
	__finally {
		printf("[FuncPheasant] In __finally{...}, executing 'return'.\n");
		printf("               This will halt global unwinds.\n");
		return;
	}
}

void FuncFish() 
{
	printf("[FuncFish] Starts.\n");

	FuncPheasant();

	printf("[FuncFish] Ends.\n"); // This WILL execute.
} 

void FuncMonkey() 
{
	printf("[FuncMonkey] Starts.\n");

	__try {
		FuncFish();
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		// This will not execute.
		MessageBeep(0); 
	}	

	printf("[FuncMonkey] Ends.\n");
} 


void test_finally_return()
{
	printf("==== Halting Global Unwinds ====\n");

	FuncMonkey(); // => FuncFish() => FuncPheasant()

	printf("\n");
}


int main(int argc, char* argv[])
{
	test_EXCEPTION_EXECUTE_HANDLER();

	test_finally_return();

	return 0;
}

