#include <windows.h>
#include <shellapi.h>

#include <AclUI.h>
#include <PrSht.h>
#include <sddl.h>
#include <Dbt.h> // DBT_DEVICEARRIVAL 

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>

#include <vaDbg.h>

#include "chjutils.h"

struct Const2Str_st
{
	int Const;
	const TCHAR *Str;
};

#define ITEM_Const2Str(macroname) { macroname, _T( #macroname ) }

template<size_t arsize>
const TCHAR *Const2Str(
	const Const2Str_st (&armap)[arsize], int Const, bool ret_known=false)
{
	for(int i=0; i<arsize; i++)
	{
		if(armap[i].Const==Const)
			return armap[i].Str;
	}

	return ret_known ? _T("Unknown") : NULL;
}

////

#include "winerrs.partial.cpp"

const TCHAR * Winerr2Str(DWORD winerr)
{
	return Const2Str(gar_Winerr2Str, winerr, true);
}

const TCHAR *WinerrStr(DWORD winerr)
{
	static TCHAR s_retbuf[80] = {};
	s_retbuf[0] = 0;

	if (winerr==0 || winerr==(DWORD)-1)
		winerr = GetLastError();

	const TCHAR *errstr = Const2Str(gar_Winerr2Str, winerr, false);

	if(errstr)
	{
		_sntprintf_s(s_retbuf, _TRUNCATE, _T("WinErr=%d (%s)"), winerr, errstr);
	}
	else
	{
		TCHAR szErrDesc[200] = {};
		DWORD retchars = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			winerr,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), // LANGID
			szErrDesc, ARRAYSIZE(szErrDesc)-1,
			NULL); 

		if(retchars>0)
		{
			_sntprintf_s(s_retbuf, _TRUNCATE, _T("WinErr=%d, %s"), winerr, szErrDesc);
		}
		else
		{
			_sntprintf_s(s_retbuf, _TRUNCATE, 
				_T("WinErr=%d (FormatMessage does not known this error-code)"), 
				winerr);
		}
	}

	return s_retbuf;
}

const TCHAR *app_WinErrStr(DWORD winerr)
{
	return WinerrStr(winerr);
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
		
		const WCHAR *outbuf = "\x0041\x0042\x7535\x0043\x0044";

	If only one parameter is given, like this:

		EXENAME "AB cde"

	The string of "AB cde" (6 TCHARs) will be returned.

	// [2025-01-24] out_szliteral[] looks stupid, forgot why I once design that.

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

	// To make it caller friendly, we always append a NUL char, but don't count it.
	outbuf[cycles] = '\0';

	if(p_retlen)
		*p_retlen = cycles;

	LocalFree(argv);
	return outbuf;
}


