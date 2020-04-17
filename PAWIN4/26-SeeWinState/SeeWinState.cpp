#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>
#define COUNT(ar) (sizeof(ar)/sizeof(ar[0]))

void dbgprint(const TCHAR *fmt, ...)
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

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("SeeWinState") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName,                  // window class name
		TEXT ("HelloWin - Drag window border to see dirty background."), // window caption
		WS_OVERLAPPEDWINDOW,        // window style
		CW_USEDEFAULT,              // initial x position
		CW_USEDEFAULT,              // initial y position
		CW_USEDEFAULT,              // initial x size
		CW_USEDEFAULT,              // initial y size
		NULL,                       // parent window handle
		NULL,                       // window menu handle
		hInstance,                  // program instance handle
		NULL) ;                     // creation parameters

	ShowWindow (hwnd, iCmdShow) ;
	//UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

VOID CALLBACK myTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	// SetActiveWindow(hwnd);
		// If uncomment this line, awnd will be the value of hwnd.

	// SetActiveWindow(NULL);
		// If this is enable, we will see SeeWinState.exe window flipping
		// between foreground and background back and forth, quite uncanny.
		// -- verified on Win7, Win10.

	HWND awnd = GetActiveWindow();
	HWND f = GetForegroundWindow();
	dbgprint(TEXT("ActiveHWND=0x%X , ForegroundHWND=0x%0X"), awnd, f);
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	RECT        rect;
	LRESULT lret;
	const int R=50;
	static UINT_PTR timerid;

	switch (message)
	{
	case WM_CREATE:
		timerid = SetTimer(hwnd, NULL, 1000, myTimerProc);
		return 0 ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		GetClientRect (hwnd, &rect) ;

		DrawText (hdc, 
			TEXT ("Use DebugView to see timer output of GetActiveWindow() and GetForegroundWindow() result."), 
			-1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_DESTROY:
		KillTimer(hwnd, timerid);
		PostQuitMessage (0) ;
		return 0 ;
	}

	lret = DefWindowProc (hwnd, message, wParam, lParam) ;
	return lret;
}