#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define COUNT(ar) (sizeof(ar)/sizeof(ar[0]))

inline void vaDbg(const TCHAR *fmt, ...)
{
    static int count = 0;
    TCHAR buf[1000] = {0};
   
#if _MSC_VER >= 1400 // VS2005+, avoid warning of deprecated _sntprintf()
    _sntprintf_s(buf, COUNT(buf)-3, _TRUNCATE, TEXT("[%d] "), ++count); // prefix seq
#else
    _sntprintf(buf, COUNT(buf)-3, TEXT("[%d] "), ++count); // prefix seq
#endif
   
    int prefixlen = (int)_tcslen(buf);
   
    va_list args;
    va_start(args, fmt);
#if _MSC_VER >= 1400 // VS2005+
    _vsntprintf_s(buf+prefixlen, COUNT(buf)-3-prefixlen, _TRUNCATE, fmt, args);
    prefixlen = (int)_tcslen(buf);
    _tcsncpy_s(buf+prefixlen, 2, TEXT("\r\n"), _TRUNCATE); // add trailing \r\n
#else
    _vsntprintf(buf+prefixlen, COUNT(buf)-3-prefixlen, fmt, args);
    prefixlen = _tcslen(buf);
    _tcsncpy(buf+prefixlen, TEXT("\r\n"), 2); // add trailing \r\n
#endif
    va_end(args);
   
    OutputDebugString(buf);
}