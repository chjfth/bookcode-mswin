#define WIN32_LEAN_AND_MEAN // would omit REFCLSID 

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <process.h>
#include <objbase.h>
#include <assert.h>

#include <utils.h>

//////////////////////////////////////////////////////////////////////////

struct SwThreadParam
{
	PROC_WinThread proc;
	void *param;
};

static unsigned __stdcall
_WinThreadWrapper(void * param)
{
	SwThreadParam *pw = (SwThreadParam*)param;

	int ret = pw->proc(pw->param);
	delete pw;

	return ret;
}

THREAD_HANDLE
winCreateThread(PROC_WinThread proc, void *param, int stacksize, unsigned *pThreadId)
{
	HANDLE hThread = NULL;
	SwThreadParam *pwp = new SwThreadParam;
	if(!pwp)
		return NULL;

	pwp->proc = proc, pwp->param = param;

	unsigned tid;
	hThread = (HANDLE)_beginthreadex(NULL, stacksize>0 ? stacksize : 64000 ,
		_WinThreadWrapper, pwp ,0, &tid);

	if(pThreadId)
		*pThreadId = tid;

	return hThread;
}

//////////////////////////////////////////////////////////////////////////

#define COUNT(ar) (sizeof(ar)/sizeof(ar[0]))

static const TCHAR *s_prefix0 = TEXT("");
static int s_prefix_seq = 0;
static bool s_is_print_ts = false; // need timestamp
static bool s_is_print_seq = false;
static bool s_is_print_tid = false;

static int get_offset_msec()
{
	static DWORD s_msec_origin = GetTickCount();

	DWORD now_msec = GetTickCount();
	return now_msec - s_msec_origin;
}

int pl(const TCHAR *fmt, ...)
{
	// User calls this function once to printf ONE LINE of text.
	// And \n will be automatically added. 
	// Here, we organize the whole text into one string and call system printf ONCE,
	// so that different thread's output will not get half-line intermixed.

	TCHAR buf[1000] = {0}, tsbuf[20] = {0};
	DWORD tid = GetCurrentThreadId();

	if(s_is_print_ts)
	{
		int offset_msec = get_offset_msec();
		_sntprintf_s(tsbuf, COUNT(tsbuf)-1, _TRUNCATE, TEXT("+%03d.%03d"),
			offset_msec/1000, offset_msec%1000);
	}

	if(s_is_print_seq && s_is_print_tid) {
		_sntprintf_s(buf, COUNT(buf)-3, _TRUNCATE, TEXT("%s%s[%d:%u] "), s_prefix0, tsbuf, ++s_prefix_seq, tid);
	}
	else if(s_is_print_seq && !s_is_print_tid) {
		_sntprintf_s(buf, COUNT(buf)-3, _TRUNCATE, TEXT("%s%s[%d] "), s_prefix0, tsbuf, ++s_prefix_seq);
	}
	else if(!s_is_print_seq && s_is_print_tid) {
		_sntprintf_s(buf, COUNT(buf)-3, _TRUNCATE, TEXT("%s%s[:%u] "), s_prefix0, tsbuf, tid);
	}

	int prefixlen = (int)_tcslen(buf);

	va_list args;
	va_start(args, fmt);
	_vsntprintf_s(buf+prefixlen, COUNT(buf)-3-prefixlen, _TRUNCATE, fmt, args);
	prefixlen = (int)_tcslen(buf);
	_tcsncpy_s(buf+prefixlen, 2, TEXT("\n"), _TRUNCATE); // add trailing \r\n
	va_end(args);

	_tprintf(TEXT("%s"), buf);
	return 0;
}

void pl_need_prefix(const TCHAR *pfx0, bool need_ts, bool need_seq, bool need_thread_id)
{
	s_prefix0 = pfx0;
	s_is_print_ts = need_ts;
	s_is_print_seq = need_seq;
	s_is_print_tid = need_thread_id;
}

//////////////////////////////////////////////////////////////////////////

// Open a registry key and get REG_SZ value.
bool HKCR_GetValueSZ(const TCHAR* szKey, const TCHAR* itemname, TCHAR* itemvalue, int bufchars)
{
	HKEY hKey = NULL;

	long lResult = RegOpenKeyEx(HKEY_CLASSES_ROOT, szKey, 0, KEY_QUERY_VALUE, &hKey);
	if(lResult != ERROR_SUCCESS)
		return FALSE;

	DWORD bufbytes = bufchars*sizeof(TCHAR);
	DWORD rettype = 0;
	lResult = RegGetValue(hKey, _T(""), itemname, RRF_RT_REG_SZ, &rettype, itemvalue, &bufbytes);

	RegCloseKey(hKey);
	return lResult==ERROR_SUCCESS ? true : false;
}

