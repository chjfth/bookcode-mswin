#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "vaDbg.h"

void vaDbg(const TCHAR *fmt, ...)
{
	static int count = 0;
	TCHAR buf[1000] = {0};

#if _MSC_VER >= 1400 // VS2005+, avoid warning of deprecated _sntprintf()
	_sntprintf_s(buf, ARRAYSIZE(buf)-3, _TRUNCATE, TEXT("[%d] "), ++count); // prefix seq
#else
#define ARRAYSIZE(ar) (sizeof(ar)/sizeof(ar[0]))
	_sntprintf(buf, ARRAYSIZE(buf)-3, TEXT("[%d] "), ++count); // prefix seq
#endif

	int prefixlen = (int)_tcslen(buf);

	va_list args;
	va_start(args, fmt);
#if _MSC_VER >= 1400 // VS2005+
	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);
	prefixlen = (int)_tcslen(buf);
	_tcsncpy_s(buf+prefixlen, 2, TEXT("\r\n"), _TRUNCATE); // add trailing \r\n
#else
	_vsntprintf(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, fmt, args);
	prefixlen = _tcslen(buf);
	_tcsncpy(buf+prefixlen, TEXT("\r\n"), 2); // add trailing \r\n
#endif
	va_end(args);

	OutputDebugString(buf);
}

void vaSetDlgItemText(HWND hdlg, int ctrlid, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	TCHAR buf[1000] = {0};
#if _MSC_VER >= 1400 // VS2005+
	_vsntprintf_s(buf, ARRAYSIZE(buf), fmt, args);
#else
	_vsntprintf(buf, ARRAYSIZE(buf), fmt, args);
#endif

	SetDlgItemText(hdlg, ctrlid, buf);

	va_end(args);
}
