#ifndef __utils_h_
#define __utils_h_

#include <tchar.h> // for TCHAR

typedef void *THREAD_HANDLE;
typedef int (*PROC_WinThread)(void *param);
	// Note: This defaults to __cdecl, not __stdcall .
//
THREAD_HANDLE winCreateThread(PROC_WinThread proc, void *param, int stacksize=0, unsigned *pThreadId=0);

int pl(const TCHAR *szfmt, ...); // Print a line.

void winPrintfLine_need_prefix(bool need_seq=true, bool need_thread_id=false);

#endif
