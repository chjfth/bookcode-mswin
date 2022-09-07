#ifndef __vaDbg_h_
#define __vaDbg_h_

#include <tchar.h>

void vaDbg(const TCHAR *fmt, ...);

void vaSetDlgItemText(HWND hdlg, int ctrlid, const TCHAR *fmt, ...);

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);

void chSETWINDOWICON(HWND hwnd, const TCHAR *icon_resname);

int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...);

TCHAR * charsets_to_codepages_hint(TCHAR *buf, int buflen);


TCHAR *parse_cmdparam_TCHARs(
	const TCHAR *T_cmdline, bool single_param_as_literal,
	TCHAR outbuf[], int outbuflen, int *p_retlen,
	TCHAR out_szliteral[], int out_szliteral_buflen);

#endif
