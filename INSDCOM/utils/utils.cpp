#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <process.h>

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

static int s_prefix_seq = 0;
static bool s_is_print_seq = false;
static bool s_is_print_tid = false;

int pl(const TCHAR *fmt, ...)
{
	// User calls this function once to printf ONE LINE of text.
	// And \n will be automatically added. 
	// Here, we organize the whole text into one string and call system printf ONCE,
	// so that different thread's output will not get half-line intermixed.

	TCHAR buf[1000] = {0};
	DWORD tid = GetCurrentThreadId();

	if(s_is_print_seq && s_is_print_tid) {
		_sntprintf_s(buf, COUNT(buf)-3, _TRUNCATE, TEXT("[%d.%u] "), ++s_prefix_seq, tid);
	}
	else if(s_is_print_seq && !s_is_print_tid) {
		_sntprintf_s(buf, COUNT(buf)-3, _TRUNCATE, TEXT("[%d] "), ++s_prefix_seq);
	}
	else if(!s_is_print_seq && s_is_print_tid) {
		_sntprintf_s(buf, COUNT(buf)-3, _TRUNCATE, TEXT("[.%u] "), tid);
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

void winPrintfLine_need_prefix(bool need_seq, bool need_thread_id)
{
	s_is_print_seq = need_seq;
	s_is_print_tid = need_thread_id;
}
