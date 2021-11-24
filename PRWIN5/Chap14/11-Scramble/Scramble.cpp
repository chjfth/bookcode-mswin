/*------------------------------------------------
   SCRAMBLE.C -- Scramble (and Unscramble) Screen
                 (c) Charles Petzold, 1998
  ------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM 300

void dbgprint(const TCHAR *fmt, ...)
{
	static int count = 0;
	TCHAR buf[1000] = {0};

	_sntprintf_s(buf, ARRAYSIZE(buf)-3, _TRUNCATE, TEXT("[%d] "), ++count); // prefix seq
	int prefixlen = (int)_tcslen(buf);

	va_list args;
	va_start(args, fmt);
	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);
	prefixlen = (int)_tcslen(buf);
	_tcsncpy_s(buf+prefixlen, 2, TEXT("\r\n"), _TRUNCATE); // add trailing \r\n
	va_end(args);

	OutputDebugString(buf);
}

void dbgBlt(int i, int j, int k, BOOL is_succ)
{
	dbgprint(_T("BitBlt #%d.%d.%d %s"), i, j, k, is_succ?_T("succ"):_T("fail"));
}

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static int iKeep [NUM][4] ;
	HDC        hdcScr, hdcMem ;
	int        cx, cy ;
	HBITMAP    hBitmap ;
	HWND       hwnd ;
	int        i, j, x1, y1, x2, y2 ;
	BOOL succ;

	if (LockWindowUpdate (hwnd = GetDesktopWindow ()))
	{
		hdcScr  = GetDCEx (hwnd, NULL, DCX_CACHE | DCX_LOCKWINDOWUPDATE) ;
		hdcMem  = CreateCompatibleDC (hdcScr) ;
		cx      = GetSystemMetrics (SM_CXSCREEN) / 10 ;
		cy      = GetSystemMetrics (SM_CYSCREEN) / 10 ;
		hBitmap = CreateCompatibleBitmap (hdcScr, cx, cy) ;

		SelectObject (hdcMem, hBitmap) ;

		srand ((int) GetCurrentTime ()) ;

		for (i = 0 ; i < 2   ; i++)
		for (j = 0 ; j < NUM ; j++)
		{
			if (i == 0)
			{
				iKeep [j] [0] = x1 = cx * (rand () % 10) ;
				iKeep [j] [1] = y1 = cy * (rand () % 10) ;
				iKeep [j] [2] = x2 = cx * (rand () % 10) ;
				iKeep [j] [3] = y2 = cy * (rand () % 10) ;
			}
			else
			{
				x1 = iKeep [NUM - 1 - j] [0] ;
				y1 = iKeep [NUM - 1 - j] [1] ;
				x2 = iKeep [NUM - 1 - j] [2] ;
				y2 = iKeep [NUM - 1 - j] [3] ;
			}
			
			succ = BitBlt (hdcMem,  0,  0, cx, cy, hdcScr, x1, y1, SRCCOPY) ;
			dbgBlt(i, j, 0, succ);
			succ = BitBlt (hdcScr, x1, y1, cx, cy, hdcScr, x2, y2, SRCCOPY) ;
			dbgBlt(i, j, 1, succ);
			succ = BitBlt (hdcScr, x2, y2, cx, cy, hdcMem,  0,  0, SRCCOPY) ;
			dbgBlt(i, j, 2, succ);

			Sleep (10) ;
		}

		DeleteDC (hdcMem) ;
		ReleaseDC (hwnd, hdcScr) ;
		DeleteObject (hBitmap) ;

		LockWindowUpdate (NULL) ;
	}
	return FALSE ;
}
