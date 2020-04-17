#include "..\CmnHdr.h"     /* See Appendix A. */
#include <tchar.h>

// The demo code from p658 with some additional info

TCHAR g_szBuffer[100];

LONG OilFilter1(DWORD ecode, TCHAR **ppchBuffer) 
{
	if (*ppchBuffer == NULL) {
		*ppchBuffer = g_szBuffer;
			// Chj: NOTE on x86 and x64, the C statement 
			//		*pchBuffer = TEXT('J');
			// will expand to two(x86) and three(x64) instructions, 
			// and the final one(the one triggering exception) is something like
			//		move byte ptr [eax], 4Ah
			// i.e. pchBuffer's value has been cached in eax,
			// so, changing the RAM cell of *ppchBuffer does not fix the 
			// problematic eax content, so, return(EXCEPTION_CONTINUE_EXECUTION)
			// will definitely causes a second 0xC0000005 exception to occur.

		return(EXCEPTION_CONTINUE_EXECUTION); // 2
	}
	else {
		// In fact, the second 0xC0000005 will send the flow here,
		// so the program will not be trapped in infinite loop.
		return(EXCEPTION_EXECUTE_HANDLER); // 4
	}
}

void FunclinRoosevelt1() 
{
	int x = 0;
	TCHAR *pchBuffer = NULL;
	__try {
		*pchBuffer = TEXT('J'); // 1
		g_szBuffer[0] = TEXT('*');
		x = 5 / x;		// -3- (WILL NOT execute this)
	}
	__except (OilFilter1(GetExceptionCode(), &pchBuffer)) {
		MessageBox(NULL, TEXT("An exception occurred"), NULL, MB_OK); // 5
	}
	
	MessageBox(NULL, TEXT("Function completed"), NULL, MB_OK); // 6
} 



int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int)
{
	FunclinRoosevelt1();
	return(0);
}


//////////////////////////////// End of File //////////////////////////////////
