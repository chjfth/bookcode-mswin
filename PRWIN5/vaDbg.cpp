#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "vaDbg.h"

static int s_dbgcount = 0;

void vaDbgTs(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlDbgTs(fmt, args);
	va_end(args);
}

void vlDbgTs(const TCHAR *fmt, va_list args)
{
	// Note: Each calling outputs one line, with timestamp prefix.
	// A '\n' will be added automatically at end.

	static int count = 0;
	static DWORD s_prev_msec = GetTickCount();

	DWORD now_msec = GetTickCount();

	TCHAR buf[4000] = {0};

	// Print timestamp to show that time has elapsed for more than one second.
	DWORD delta_msec = now_msec - s_prev_msec;
	if(delta_msec>=1000)
	{
		OutputDebugString(_T(".\n"));
	}

	TCHAR timebuf[40] = {};
	now_timestr(timebuf, ARRAYSIZE(timebuf));

	_sntprintf_s(buf, _TRUNCATE, _T("[%d]%s (+%3u.%03us) "), 
		++s_dbgcount,
		timebuf, 
		delta_msec/1000, delta_msec%1000);

	int prefixlen = (int)_tcslen(buf);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);

	// add trailing \n
	int slen = (int)_tcslen(buf);
	if(slen==ARRAYSIZE(buf)-1)
		--slen;

	buf[slen] = '\n';
	buf[slen+1] = '\0';

	OutputDebugString(buf);

	s_prev_msec = now_msec;
}

void vaDbgS(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlDbgS(fmt, args);
	va_end(args);
}

void vlDbgS(const TCHAR *fmt, va_list args)
{
	// This only has Sequential prefix.

	TCHAR buf[1000] = {0};

	_sntprintf_s(buf, ARRAYSIZE(buf)-3, _TRUNCATE, TEXT("[%d] "), ++s_dbgcount); // prefix seq

	int prefixlen = (int)_tcslen(buf);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);

	prefixlen = (int)_tcslen(buf);
	_tcsncpy_s(buf+prefixlen, 2, TEXT("\r\n"), _TRUNCATE); // add trailing \r\n

	// add trailing \n
	int slen = (int)_tcslen(buf);
	if(slen==ARRAYSIZE(buf)-1)
		--slen;

	buf[slen] = '\n';
	buf[slen+1] = '\0';

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

// Sets the dialog box icons
void chSETWINDOWICON(HWND hwnd, const TCHAR *icon_resname) {
	SendMessage(hwnd, WM_SETICON, TRUE,  (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), icon_resname)
		);
	SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), icon_resname)
		);
}

int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);

	TCHAR msgtext[4000] = {};
	_vsntprintf_s(msgtext, _TRUNCATE, szfmt, args);

	int ret = MessageBox(hwnd, msgtext, szTitle, utype);

	va_end(args);
	return ret;
}

TCHAR * charsets_to_codepages_hint(TCHAR *buf, int buflen)
{
	const TCHAR *ar_charset_prefix[] =
	{
		TEXT ("0   = ANSI or West European"),
		TEXT ("1   = Default"),
		TEXT ("2   = Symbol"),
		TEXT ("128 = Shift-JIS (Japanese)"),
		TEXT ("129 = Hangul (Korean)"),
		TEXT ("130 = Johab (Korean, obsolete)"),
		TEXT ("134 = GB2312 - GBK (Simplified Chinese)"),
		TEXT ("136 = Big5 (Traditional Chinese)"),
		TEXT ("177 = Hebrew"),
		TEXT ("178 = Arabic"),
		TEXT ("161 = Greek"),
		TEXT ("162 = Turkish"),
		TEXT ("163 = Vietnamese"),
		TEXT ("204 = Russian"),
		TEXT ("222 = Thai"),
		TEXT ("238 = East European"),
		TEXT ("255 = OEM/DOS")
	};

	buf[0] = '\0';

	for(int i=0; i<ARRAYSIZE(ar_charset_prefix); i++)
	{
		TCHAR tmp[80] = {};
		int charset = _ttoi(ar_charset_prefix[i]);

		CHARSETINFO csi = {};
		BOOL succ = TranslateCharsetInfo((DWORD*)(DWORD_PTR)charset, &csi, TCI_SRCCHARSET);
		if(succ)
		{
			_sntprintf_s(tmp, _TRUNCATE, _T("%-40.40s , ANSI-codepage: %u\n"), ar_charset_prefix[i], csi.ciACP);
		}
		else
		{
			_sntprintf_s(tmp, _TRUNCATE, _T("%s\n"), ar_charset_prefix[i]);
		}
		_tcscat_s(buf, buflen, tmp);
	}

	return buf;
}

//////////////////////////////////////////////////////////////////////////

TCHAR *parse_cmdparam_TCHARs(
	const TCHAR *T_cmdline, bool single_param_as_literal,
	TCHAR outbuf[], int outbuflen, int *p_retlen,
	TCHAR out_szliteral[], int out_szliteral_buflen)
{
/*  T_cmdline should points to string from GetCommandLineA()/GetCommandLineW().

	If TCHAR is char (ANSI version), outbuf[] will be a char array.
	If TCHAR is WCHAR (Unicode version), outbuf[] will be a WCHAR array.

	Actual output TCHAR count will be returned in *p_retlen.

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
	//const TCHAR *T_cmdline = GetCommandLine();
	const WCHAR *W_cmdline = nullptr;

	if(out_szliteral)
		out_szliteral[0] = '\0';

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

	if(single_param_as_literal && argc==2)
	{
		// not hexform
#ifdef UNICODE
		_tcscpy_s(outbuf, outbuflen, argv[1]);
		int retlen = (int)wcslen(argv[1]);
#else
		int retlen = WideCharToMultiByte(CP_ACP, 0, argv[1], -1, outbuf, outbuflen, NULL, NULL);
#endif
		if(p_retlen)
			*p_retlen = retlen;

		if(out_szliteral)
			_tcscpy_s(out_szliteral, out_szliteral_buflen, outbuf);

		LocalFree(argv);
		return outbuf;
	}

	int cycles = min(outbuflen-1, argc-1);

	int i;
	for(i=1; i<=cycles; i++)
	{
		outbuf[i-1] = (TCHAR)wcstoul(argv[i], nullptr, 16);
	}

	// To make it caller friendly, we always append a NULL char, but don't count it.
	outbuf[cycles] = 0;

	if(p_retlen)
		*p_retlen = cycles;

	LocalFree(argv);
	return outbuf;
}

