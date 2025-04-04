#ifndef __vaDbg_h_
#define __vaDbg_h_

#include <tchar.h>
#include <stdarg.h>

DWORD TrueGetMillisec(); // more accurate than GetTickCount();

void vaDbgTs(const TCHAR *fmt, ...); // with seq+timestamp prefix
void vlDbgTs(const TCHAR *fmt, va_list args);

void vaDbgS(const TCHAR *fmt, ...);  // with sequence-only prefix
void vlDbgS(const TCHAR *fmt, va_list args);

BOOL vaSetWindowText(HWND hwnd, const TCHAR *fmt, ...);
BOOL vaSetDlgItemText(HWND hdlg, int ctrlid, const TCHAR *fmt, ...);

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);

void chSETWINDOWICON(HWND hwnd, const TCHAR *icon_resname);

int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...);

TCHAR * charsets_to_codepages_hint(TCHAR *buf, int buflen);


TCHAR *parse_cmdparam_TCHARs(
	const TCHAR *T_cmdline, TCHAR outbuf[], int outbuflen, 
	int *p_retlen=nullptr, // optional
	bool *p_prefer_hexinput=nullptr // optional
	);

#endif
