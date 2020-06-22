#ifndef __utils_h_
#define __utils_h_

#ifdef __cplusplus
extern"C" {
#endif

#include <tchar.h> // for TCHAR

typedef void *THREAD_HANDLE;
typedef int (*PROC_WinThread)(void *param);
	// Note: This defaults to __cdecl, not __stdcall .
//
THREAD_HANDLE winCreateThread(PROC_WinThread proc, void *param, int stacksize=0, unsigned *pThreadId=0);

int pl(const TCHAR *szfmt, ...); // Print a line.

void pl_need_prefix(const TCHAR *pfx0, bool need_ts, bool need_seq, bool need_thread_id);

/////

//void CLSIDtochar(REFCLSID clsid, char* szCLSID, int length);

bool HKCR_GetValueSZ(const TCHAR* szKey, const TCHAR* itemname, TCHAR* itemvalue, int bufchars);


#ifdef __cplusplus
} // extern"C" {
#endif

#endif
