#ifndef __vaDbg_h_
#define __vaDbg_h_

#include <tchar.h>

void vaDbg(const TCHAR *fmt, ...);

void vaSetDlgItemText(HWND hdlg, int ctrlid, const TCHAR *fmt, ...);

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);


#endif
