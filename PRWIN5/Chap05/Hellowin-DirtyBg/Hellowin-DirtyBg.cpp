/*------------------------------------------------------------
   HELLOWIN.C -- Displays "Hello, Windows 98!" in client area
                 (c) Charles Petzold, 1998
  ------------------------------------------------------------*/

#include <windows.h>

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
	static TCHAR szAppName[] = TEXT ("HelloWin") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = NULL; //hInstance ;
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
		400,              // x size
		300,              // y size
		NULL,                       // parent window handle
		NULL,                       // window menu handle
		NULL, // hInstance,         // program instance handle
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

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	RECT        rect, rugn ;
	LRESULT lret;
	BOOL b;
	const int R=50;
	int iret;
	HRGN hrAll=0, hrHole=0, hrFinal=0;

	switch (message)
	{
	case WM_CREATE:
		return 0 ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		GetClientRect (hwnd, &rect) ;

		SetBkMode(hdc, TRANSPARENT); 
			// Use transparent text drawing mode, so that we can more easily 
			// see the dirty background on dragging the window.
		DrawText (hdc, TEXT ("Hello, Windows 98!"), -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_ERASEBKGND:
		hdc = (HDC)wParam;
		GetClientRect (hwnd, &rect) ;
		b = GetUpdateRect(hwnd, &rugn, false); //COMPLEXREGION
		if(b)
		{
			dbgprint(TEXT("GetUpdateRect[%d(+%d),%d(+%d) : %d(-%d),%d(-%d)]"), 
				rect.left, rugn.left-rect.left , rect.top, rugn.top-rect.top, 
				rect.right, rect.right-rugn.right , rect.bottom, rect.bottom-rugn.bottom);
			// On Windows 7, I always see (+0)(+0) : (-0)(-0)
		}
		hrAll = CreateRectRgn(0, 0, 1, 1); // create a null region
		iret = GetUpdateRgn(hwnd, hrAll, FALSE);

		hrHole = CreateEllipticRgn(rect.right/2-R, rect.bottom/2-R, rect.right/2+R, rect.bottom/2+R);

		hrFinal = CreateRectRgn(0, 0, 1, 1); // create a null region
		iret = CombineRgn(hrFinal, hrAll, hrHole, RGN_DIFF);
			// iret will be COMPLEXREGION(3)
		
		iret = SelectClipRgn(hdc, hrFinal); 

		// Now, the clipping region of hdc is a whole-window rectangle 
		// with a hole at its center, the hole will make the background dirty.

		DeleteObject(hrAll);
		DeleteObject(hrHole);
		DeleteObject(hrFinal);

		break; // so to have DefWindowProc() repaint (most) background for us


	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}

	lret = DefWindowProc (hwnd, message, wParam, lParam) ;

	if(message==WM_ERASEBKGND)
		lret=lret;	

	return lret;
}