#ifndef __vaDbg_h_
#define __vaDbg_h_

#include <tchar.h>

void vaDbg(const TCHAR *fmt, ...);

void vaSetDlgItemText(HWND hdlg, int ctrlid, const TCHAR *fmt, ...);

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);

void chSETWINDOWICON(HWND hwnd, const TCHAR *icon_resname);


TCHAR *parse_cmdparam_TCHARs(TCHAR outbuf[], int outbuflen, int *p_retlen=nullptr);

#endif
