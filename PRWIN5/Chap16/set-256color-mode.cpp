#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <varargs.h>

const TCHAR *ErrStrDispChange(LONG err)
{
	struct ErrMap_st {
		LONG code;
		const TCHAR *str;
	} errmap[] = {

		{ DISP_CHANGE_RESTART , _T("DISP_CHANGE_RESTART") },
		{ DISP_CHANGE_FAILED  , _T("DISP_CHANGE_FAILED") },
		{ DISP_CHANGE_BADMODE , _T("DISP_CHANGE_BADMODE") },
		{ DISP_CHANGE_NOTUPDATED, _T("DISP_CHANGE_NOTUPDATED") },
		{ DISP_CHANGE_BADFLAGS, _T("DISP_CHANGE_BADFLAGS") },
		{ DISP_CHANGE_BADPARAM, _T("DISP_CHANGE_BADPARAM") },
		{ DISP_CHANGE_BADDUALVIEW, _T("DISP_CHANGE_BADDUALVIEW") },
	};

	int i;
	for(i=0; i<ARRAYSIZE(errmap); i++)
	{
		if(errmap[i].code==err)
			return errmap[i].str;
	}

	static TCHAR errmsg[100];
	_sntprintf_s(errmsg, ARRAYSIZE(errmsg), 
		_T("Unknown DISP_CHANGE_xxx return value %l"), err);
	return errmsg;
}

void Set_256ColorMode(const TCHAR *szAppname)
{
	// Check 256-color video mode using either way:
	// 1. EnumDisplaySettings(), DEVMODE.dmBitsPerPel should be 8.
	// 2. GetDeviceCaps(, SIZEPALETTE), should return 256.

	DEVMODE dm = {sizeof(dm)};
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, & dm); // query current
	if( dm.dmBitsPerPel==8 )
		return; // already in 8-bpp
	
	int mbret = MessageBox(NULL, 
		_T("Current video mode is not 256-color.\r\n")
		_T("\r\n")
		_T("To achieve demonstration purpose, you should change it to 256-color.\r\n")
		_T("\r\n")
		_T("Also note that, 256-color mode on Windows 8+ is not possible. So you should run this on Win7/XP.\r\n")
		_T("\r\n")
		_T("Do it now?\r\n")
		,
		szAppname,
		MB_YESNO
		);
	if(mbret!=IDYES)
		return;

	dm.dmBitsPerPel = 8;
	LONG result = ChangeDisplaySettings(&dm, 0);
	if(result==DISP_CHANGE_SUCCESSFUL)
		return;

	TCHAR errmsg[100];
	_sntprintf_s(errmsg, ARRAYSIZE(errmsg),
		_T("EnumDisplaySettings() returns error: %s"),  ErrStrDispChange(result));

	MessageBox(NULL, errmsg, _T("Fail setting 256-color video mode"), MB_OK|MB_ICONERROR);
	ExitProcess(4);
}

void vaDbg(const TCHAR *fmt, ...)
{
	static int count = 0;
	TCHAR buf[1000] = {0};

#if _MSC_VER >= 1400 // VS2005+, avoid warning of deprecated _sntprintf()
	_sntprintf_s(buf, ARRAYSIZE(buf)-3, _TRUNCATE, TEXT("[%d] "), ++count); // prefix seq
#else
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
