#ifndef __dbgprint_h_20151218_
#define __dbgprint_h_20151218_

#include <tchar.h>


#ifdef __cplusplus
extern"C"{
#endif

void dbgprint(const TCHAR *fmt, ...);

const TCHAR * get_exename();

void vaMsgBox(HWND hwndParent, const TCHAR *fmt, ...);

void vaMsgBoxWinErr(HWND hwndParent, const TCHAR *fmt, ...);

#ifdef __cplusplus
} // extern"C"{
#endif


#endif
