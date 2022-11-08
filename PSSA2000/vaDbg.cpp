#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#include <AclUI.h>
#include <PrSht.h>
#include <sddl.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "InterpretConst.h"
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
	_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%02d:%02d:%02d.%03d]"), buf,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
	_sntprintf(buf, bufchars-1, _T("%s%02d:%02d:%02d.%03d]"), buf,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
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

int vaMsgBox(UINT utype, const TCHAR *szfmt, ...)
{
	TCHAR szProgname[256];
	GetModuleFileName(NULL, szProgname, ARRAYSIZE(szProgname));

	va_list args;
	va_start(args, szfmt);

	TCHAR msgtext[4000] = {};
	_vsntprintf_s(msgtext, _TRUNCATE, szfmt, args);

	HWND hwnd = GetActiveWindow();
	int ret = MessageBox(hwnd, msgtext, szProgname, utype);

	va_end(args);
	return ret;
}

const TCHAR *app_WinErrStr(DWORD winerr)
{
	static TCHAR s_retbuf[400] = {};
	TCHAR szWinErr[200] = {};

	s_retbuf[0] = 0;

	if (winerr == (DWORD)-1)
		winerr = GetLastError();

	DWORD retchars = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, 
		winerr,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), // LANGID
		szWinErr, ARRAYSIZE(szWinErr),
		NULL); 

	if(retchars>0)
	{
		_sntprintf_s(s_retbuf, _TRUNCATE, _T("WinErr=%d, %s"), winerr, szWinErr);
	}
	else
	{
		_sntprintf_s(s_retbuf, _TRUNCATE, 
			_T("WinErr=%d (FormatMessage does not known this error-code)"), 
			winerr);
	}

	return s_retbuf;
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

//////////////////////////////////////////////////////////////////////////

#undef _tprintf
#define _tprintf xxx

void CH10_DumpACL( PACL pACL )
{
	// Due to using ITCS(), we cannot use __try{} here.

	if (pACL == NULL){
		vaDbg(TEXT("NULL DACL"));
		return;
	}

	ACL_SIZE_INFORMATION aclSize = {0};
	if (!GetAclInformation(pACL, &aclSize, sizeof(aclSize), AclSizeInformation))
		return;

	vaDbg(TEXT("ACL ACE count: %d"), aclSize.AceCount);

	for (ULONG lIndex = 0;lIndex < aclSize.AceCount;lIndex++)
	{
		ACCESS_ALLOWED_ACE* pACE;
		if (!GetAce(pACL, lIndex, (PVOID*)&pACE))
			return;

		vaDbg(TEXT("ACE #%d/%d"), lIndex+1, aclSize.AceCount);
		vaDbg(TEXT("  ACE Type = %s"), ITCS(pACE->Header.AceType, itc_ACE_TYPE));
		vaDbg(TEXT("  ACE Flags = %s"), ITCS(pACE->Header.AceFlags, itc_ACE_FLAGS));

		TCHAR bitbufs[40] = {};
		ULONG lIndex2 = (ULONG)1<<31;
		for(int i=0; i<32; i++){
			bitbufs[i] = ((pACE->Mask & lIndex2) != 0)?TEXT('1'):TEXT('0');
			lIndex2>>=1;
		}
		vaDbg(TEXT("  ACE Mask (32->0) = %s"), bitbufs);

		TCHAR szName[1024];
		TCHAR szDom[1024];
		PSID pSID = PSIDFromPACE(pACE);
		SID_NAME_USE sidUse = SidTypeUnknown;  
		ULONG lLen1 = 1024, lLen2 = 1024;
			
		if (!LookupAccountSid(NULL, pSID, szName, &lLen1, szDom, &lLen2, &sidUse))
			lstrcpy(szName, TEXT("Unknown"));
			
		PTSTR pszSID = nullptr;
		if (!ConvertSidToStringSid(pSID, &pszSID))
			return;
			
		vaDbg(TEXT("  ACE SID = %s ( %s )"), pszSID, szName);
		LocalFree(pszSID);
	}
}



//////////////////////////////////////////////////////////////////////////



static const Enum2Val_st e2v_CSecurityInformation_PropertySheetPageCallback_uMsg[] =
{
	ITC_NAMEPAIR(PSPCB_CREATE),
	ITC_NAMEPAIR(PSPCB_RELEASE),
	ITC_NAMEPAIR(PSPCB_SI_INITDIALOG),
};
CInterpretConst itc_CSecurityInformation_PropertySheetPageCallback_uMsg(
	e2v_CSecurityInformation_PropertySheetPageCallback_uMsg, 
	ARRAYSIZE(e2v_CSecurityInformation_PropertySheetPageCallback_uMsg));
//
static const Enum2Val_st e2v_CSecurityInformation_PropertySheetPageCallback_uPage[] =
{
	ITC_NAMEPAIR(SI_PAGE_PERM),
	ITC_NAMEPAIR(SI_PAGE_ADVPERM),
	ITC_NAMEPAIR(SI_PAGE_AUDIT),
	ITC_NAMEPAIR(SI_PAGE_OWNER),
	ITC_NAMEPAIR(SI_PAGE_EFFECTIVE),
	ITC_NAMEPAIR(SI_PAGE_TAKEOWNERSHIP),
};
CInterpretConst itc_CSecurityInformation_PropertySheetPageCallback_uPage(
	e2v_CSecurityInformation_PropertySheetPageCallback_uPage, 
	ARRAYSIZE(e2v_CSecurityInformation_PropertySheetPageCallback_uPage));
//
static const Bitfield2Val_st b2v_SECURITY_INFORMATION[] =
{
	ITC_NAMEPAIR(OWNER_SECURITY_INFORMATION           ),
	ITC_NAMEPAIR(GROUP_SECURITY_INFORMATION           ),
	ITC_NAMEPAIR(DACL_SECURITY_INFORMATION            ),
	ITC_NAMEPAIR(SACL_SECURITY_INFORMATION            ),
	ITC_NAMEPAIR(LABEL_SECURITY_INFORMATION           ),
	ITC_NAMEPAIR(PROTECTED_DACL_SECURITY_INFORMATION  ),
	ITC_NAMEPAIR(PROTECTED_SACL_SECURITY_INFORMATION  ),
	ITC_NAMEPAIR(UNPROTECTED_DACL_SECURITY_INFORMATION),
	ITC_NAMEPAIR(UNPROTECTED_SACL_SECURITY_INFORMATION),
};
CInterpretConst itc_SECURITY_INFORMATION(
	b2v_SECURITY_INFORMATION, ARRAYSIZE(b2v_SECURITY_INFORMATION),
	_T("0x%08X"));
//
static const Enum2Val_st e2v_SE_OBJECT_TYPE[] =
{
	ITC_NAMEPAIR(SE_UNKNOWN_OBJECT_TYPE),
	ITC_NAMEPAIR(SE_FILE_OBJECT),
	ITC_NAMEPAIR(SE_SERVICE),
	ITC_NAMEPAIR(SE_PRINTER),
	ITC_NAMEPAIR(SE_REGISTRY_KEY),
	ITC_NAMEPAIR(SE_LMSHARE),
	ITC_NAMEPAIR(SE_KERNEL_OBJECT),
	ITC_NAMEPAIR(SE_WINDOW_OBJECT),
	ITC_NAMEPAIR(SE_DS_OBJECT),
	ITC_NAMEPAIR(SE_DS_OBJECT_ALL),
	ITC_NAMEPAIR(SE_PROVIDER_DEFINED_OBJECT),
	ITC_NAMEPAIR(SE_WMIGUID_OBJECT),
	ITC_NAMEPAIR(SE_REGISTRY_WOW64_32KEY),
};
CInterpretConst itc_SE_OBJECT_TYPE(
	e2v_SE_OBJECT_TYPE, ARRAYSIZE(e2v_SE_OBJECT_TYPE)
	);
//
static const Enum2Val_st e2v_ACE_TYPE[] =
{
	ITC_NAMEPAIR(ACCESS_ALLOWED_ACE_TYPE                 ),
	ITC_NAMEPAIR(ACCESS_DENIED_ACE_TYPE                  ),
	ITC_NAMEPAIR(SYSTEM_AUDIT_ACE_TYPE                   ),
	ITC_NAMEPAIR(SYSTEM_ALARM_ACE_TYPE                   ),
	ITC_NAMEPAIR(ACCESS_ALLOWED_COMPOUND_ACE_TYPE        ),
	ITC_NAMEPAIR(ACCESS_ALLOWED_OBJECT_ACE_TYPE          ),
	ITC_NAMEPAIR(ACCESS_DENIED_OBJECT_ACE_TYPE           ),
	ITC_NAMEPAIR(SYSTEM_AUDIT_OBJECT_ACE_TYPE            ),
	ITC_NAMEPAIR(SYSTEM_ALARM_OBJECT_ACE_TYPE            ),
	ITC_NAMEPAIR(ACCESS_ALLOWED_CALLBACK_ACE_TYPE        ),
	ITC_NAMEPAIR(ACCESS_DENIED_CALLBACK_ACE_TYPE         ),
	ITC_NAMEPAIR(ACCESS_ALLOWED_CALLBACK_OBJECT_ACE_TYPE ),
	ITC_NAMEPAIR(ACCESS_DENIED_CALLBACK_OBJECT_ACE_TYPE  ),
	ITC_NAMEPAIR(SYSTEM_AUDIT_CALLBACK_ACE_TYPE          ),
	ITC_NAMEPAIR(SYSTEM_ALARM_CALLBACK_ACE_TYPE          ),
	ITC_NAMEPAIR(SYSTEM_AUDIT_CALLBACK_OBJECT_ACE_TYPE   ),
	ITC_NAMEPAIR(SYSTEM_ALARM_CALLBACK_OBJECT_ACE_TYPE   ),
	ITC_NAMEPAIR(SYSTEM_MANDATORY_LABEL_ACE_TYPE         ),
};
CInterpretConst itc_ACE_TYPE(e2v_ACE_TYPE, ARRAYSIZE(e2v_ACE_TYPE),
	_T("0x%04X"));
//
static const Bitfield2Val_st b2v_ACE_FLAGS[] =
{
	ITC_NAMEPAIR(OBJECT_INHERIT_ACE         ),
	ITC_NAMEPAIR(CONTAINER_INHERIT_ACE      ),
	ITC_NAMEPAIR(NO_PROPAGATE_INHERIT_ACE   ),
	ITC_NAMEPAIR(INHERIT_ONLY_ACE           ),
	ITC_NAMEPAIR(INHERITED_ACE              ),
	ITC_NAMEPAIR(SUCCESSFUL_ACCESS_ACE_FLAG ),
	ITC_NAMEPAIR(FAILED_ACCESS_ACE_FLAG     ),
};
CInterpretConst itc_ACE_FLAGS(b2v_ACE_FLAGS, ARRAYSIZE(b2v_ACE_FLAGS),
	_T("0x%02X"));
//
static const Bitfield2Val_st b2v_SECURITY_DESCRIPTOR_CONTROL[] =
{
	ITC_NAMEPAIR(SE_OWNER_DEFAULTED      ),
	ITC_NAMEPAIR(SE_GROUP_DEFAULTED      ),
	ITC_NAMEPAIR(SE_DACL_PRESENT         ),
	ITC_NAMEPAIR(SE_DACL_DEFAULTED       ),
	ITC_NAMEPAIR(SE_SACL_PRESENT         ),
	ITC_NAMEPAIR(SE_SACL_DEFAULTED       ),
	ITC_NAMEPAIR(SE_DACL_AUTO_INHERIT_REQ),
	ITC_NAMEPAIR(SE_SACL_AUTO_INHERIT_REQ),
	ITC_NAMEPAIR(SE_DACL_AUTO_INHERITED  ),
	ITC_NAMEPAIR(SE_SACL_AUTO_INHERITED  ),
	ITC_NAMEPAIR(SE_DACL_PROTECTED       ),
	ITC_NAMEPAIR(SE_SACL_PROTECTED       ),
	ITC_NAMEPAIR(SE_RM_CONTROL_VALID     ),
	ITC_NAMEPAIR(SE_SELF_RELATIVE        ),
};
CInterpretConst itc_SECURITY_DESCRIPTOR_CONTROL(
	b2v_SECURITY_DESCRIPTOR_CONTROL, ARRAYSIZE(b2v_SECURITY_DESCRIPTOR_CONTROL),
	_T("0x%04X"));

