#ifndef __utils_h_
#define __utils_h_

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

void vaSetDlgItemText(HWND hwnd, UINT ctrlid, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	TCHAR buf[1000] = _T("");
	_vsnprintf_s(buf, ARRAYSIZE(buf), fmt, args);

	SetDlgItemText(hwnd, ctrlid, buf);

	va_end(args);
}

#endif
