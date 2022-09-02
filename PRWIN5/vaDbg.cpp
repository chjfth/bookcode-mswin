#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

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

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd)
{
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	buf[0]=_T('['); buf[1]=_T('\0'); buf[bufchars-1] = _T('\0');
	if(ymd) {
#if _MSC_VER >= 1400 // VS2005+
		_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%04d-%02d-%02d "), buf, 
			st.wYear, st.wMonth, st.wDay);
#else
		_sntprintf(buf, bufchars-1, _T("%s%04d-%02d-%02d "), buf, 
			st.wYear, st.wMonth, st.wDay);
#endif
	}
#if _MSC_VER >= 1400 // VS2005+
	_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%02d:%02d:%02d]"), buf,
		st.wHour, st.wMinute, st.wSecond);
#else
	_sntprintf(buf, bufchars-1, _T("%s%02d:%02d:%02d]"), buf,
		st.wHour, st.wMinute, st.wSecond);
#endif
	return buf;
}

//////////////////////////////////////////////////////////////////////////

TCHAR *my_parse_cmdparams(TCHAR outbuf[], int outbufchars)
{
/* Implicit input: GetCommandLineA()/GetCommandLineW()

	If TCHAR is char (ANSI version), user will get a char array on return.
	If TCHAR is WCHAR (Unicode version), user will get a WCHAR array on return.

	If user pass more than one parameter, like this:

		EXENAME 41 42 7535 43 44

	Then each parameter will be considered a TCHAR in hex representation, 
	and user gets 5 TCHARs on return.

	So in ANSI version, user will get sth. equivalent to:

		const char *outbuf = "\x41\x42\x35\x43\x44";

	In Unicode version, user will get sth. equivalent to:
		
		const WCHAR *outbuf = "\x0041\x0042\x0043\x0044\x0045";

	If only one parameter is given, like this:

		EXENAME "AB cde"

	The string of "AB cde" (6 TCHARs) will be returned.
*/
	TCHAR *T_cmdline = GetCommandLine();
	WCHAR *W_cmdline = nullptr;

#ifdef UNICODE
	W_cmdline = T_cmdline;
#else
	WCHAR W_buf[1024] = {};
	W_cmdline = W_buf;
	MultiByteToWideChar(CP_ACP, 0, T_cmdline, -1, W_buf, ARRAYSIZE(W_buf));		
#endif

	int argc = 0;
	WCHAR **argv = CommandLineToArgvW(W_cmdline, &argc);
	// -- argv[0] is exepath itself.

	if(argc==2)
	{
#ifdef UNICODE
		_tcscpy_s(outbuf, outbufchars, argv[1]);
#else
		WideCharToMultiByte(CP_ACP, 0, argv[1], -1, outbuf, outbufchars, NULL, NULL);
#endif
		LocalFree(argv);
		return outbuf;
	}

	int cycles = min(outbufchars-1, argc-1);

	int i;
	for(i=1; i<=cycles; i++)
	{
		outbuf[i-1] = (TCHAR)wcstoul(argv[i], nullptr, 16);
	}

	outbuf[cycles] = 0;

	LocalFree(argv);

	return outbuf;
}
