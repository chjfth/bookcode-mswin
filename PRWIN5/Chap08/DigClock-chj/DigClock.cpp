/*-----------------------------------------
DIGCLOCK.c -- Digital Clock(c) Charles Petzold, 1998

DigClock.cpp -- Updated by Chj, 2021.11.
* Thin window border.
* Drag window by clicking anywhere inside the window.
* Move window by pressing arrow keys, Ctrl to accelerate.
* Right-click context menu, toggle always on top.
* Select different color by mouse left/right clicking.
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

void dbgprint(const TCHAR *fmt, ...)
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

#define ID_TIMER    1

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("DigClock") ;
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
		MessageBox (NULL, TEXT ("Program requires Windows NT!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindowEx (WS_EX_DLGMODALFRAME,
		szAppName, TEXT ("Digital Clock"),
		WS_POPUPWINDOW|WS_THICKFRAME,  // WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		160, 60, // CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL) ;

	SendMessage(hwnd, WM_SETICON, TRUE, (LPARAM)LoadIcon(hInstance, _T("MYPROGRAM")));

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

void DisplayDigit (HDC hdc, int iNumber)
{
	static BOOL  fSevenSegment [10][7] = {
		1, 1, 1, 0, 1, 1, 1,     // 0
		0, 0, 1, 0, 0, 1, 0,     // 1
		1, 0, 1, 1, 1, 0, 1,     // 2
		1, 0, 1, 1, 0, 1, 1,     // 3
		0, 1, 1, 1, 0, 1, 0,     // 4
		1, 1, 0, 1, 0, 1, 1,     // 5
		1, 1, 0, 1, 1, 1, 1,     // 6
		1, 0, 1, 0, 0, 1, 0,     // 7
		1, 1, 1, 1, 1, 1, 1,     // 8
		1, 1, 1, 1, 0, 1, 1 
	} ;  // 9

	static POINT ptSegment [7][6] = {
		7,  6,  11,  2,  31,  2,  35,  6,  31, 10,  11, 10,
		6,  7,  10, 11,  10, 31,   6, 35,   2, 31,   2, 11,
		36,  7,  40, 11,  40, 31,  36, 35,  32, 31,  32, 11,
		7, 36,  11, 32,  31, 32,  35, 36,  31, 40,  11, 40,
		6, 37,  10, 41,  10, 61,   6, 65,   2, 61,   2, 41,
		36, 37,  40, 41,  40, 61,  36, 65,  32, 61,  32, 41,
		7, 66,  11, 62,  31, 62,  35, 66,  31, 70,  11, 70 
	} ;

	int          iSeg ;

	for (iSeg = 0 ; iSeg < 7 ; iSeg++)
	{
		if (fSevenSegment [iNumber][iSeg])
			Polygon (hdc, ptSegment [iSeg], 6) ;
	}
}

void DisplayTwoDigits (HDC hdc, int iNumber, BOOL fSuppress)
{
	if (!fSuppress || (iNumber / 10 != 0))
		DisplayDigit (hdc, iNumber / 10) ;

	OffsetWindowOrgEx (hdc, -42, 0, NULL) ;
	DisplayDigit (hdc, iNumber % 10) ;
	OffsetWindowOrgEx (hdc, -42, 0, NULL) ;
}

void DisplayColon (HDC hdc)
{
	POINT ptColon [2][4] = { 2,  21,  6,  17,  10, 21,  6, 25,
		2,  51,  6,  47,  10, 51,  6, 55 } ;

	Polygon (hdc, ptColon [0], 4) ;
	Polygon (hdc, ptColon [1], 4) ;

	OffsetWindowOrgEx (hdc, -12, 0, NULL) ;
}

void DisplayTime (HDC hdc, BOOL f24Hour, BOOL fSuppress)
{
	SYSTEMTIME st = {};
	GetLocalTime (&st) ;

	if (f24Hour)
		DisplayTwoDigits (hdc, st.wHour, fSuppress) ;
	else
		DisplayTwoDigits (hdc, (st.wHour %= 12) ? st.wHour : 12, fSuppress) ;

	DisplayColon (hdc) ;
	DisplayTwoDigits (hdc, st.wMinute, FALSE) ;
	DisplayColon (hdc) ;
	DisplayTwoDigits (hdc, st.wSecond, FALSE) ;
}

void MoveWindow_byOffset(HWND hwnd, int offsetx, int offsety)
{
	RECT oldrect = {};
	GetWindowRect(hwnd, &oldrect);
	MoveWindow(hwnd, oldrect.left+offsetx, oldrect.top+offsety, 
		oldrect.right-oldrect.left, oldrect.bottom-oldrect.top, TRUE);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL   f24Hour, fSuppress ;
	static HBRUSH hBrushRed ;
	static int    cxClient, cyClient ;
	HDC           hdc ;
	PAINTSTRUCT   ps ;
	TCHAR         szBuffer [2] ;

	static POINT s_pos_mousedown; // client-area position
	static bool s_is_dragging = false;

	switch (message)
	{{
	case WM_CREATE:
		hBrushRed = CreateSolidBrush (RGB (0, 160, 0)) ;
		SetTimer (hwnd, ID_TIMER, 1000, NULL) ;

		// fall through

	case WM_SETTINGCHANGE:
		GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2) ;
		f24Hour = (szBuffer[0] == '1') ;

		GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2) ;
		fSuppress = (szBuffer[0] == '0') ;

		InvalidateRect (hwnd, NULL, TRUE) ;
		return 0 ;

	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;

	case WM_TIMER:
		InvalidateRect (hwnd, NULL, TRUE) ;
		return 0 ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		SetMapMode (hdc, MM_ISOTROPIC) ;
		SetWindowExtEx (hdc, 276, 72, NULL) ;
		SetViewportExtEx (hdc, cxClient, cyClient, NULL) ;

		SetWindowOrgEx (hdc, 138, 36, NULL) ;
		SetViewportOrgEx (hdc, cxClient / 2, cyClient / 2, NULL) ;

		SelectObject (hdc, GetStockObject (NULL_PEN)) ;
		SelectObject (hdc, hBrushRed) ;

		DisplayTime (hdc, f24Hour, fSuppress) ;

		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_LBUTTONDOWN:
	{
		s_is_dragging = true;
		SetCapture(hwnd);

		s_pos_mousedown.x = GET_X_LPARAM(lParam);
		s_pos_mousedown.y = GET_Y_LPARAM(lParam);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if(s_is_dragging==false)
			break;
		
		// Set new window pos

		int offsetx = GET_X_LPARAM(lParam) - s_pos_mousedown.x;
		int offsety = GET_Y_LPARAM(lParam) - s_pos_mousedown.y;

		MoveWindow_byOffset(hwnd, offsetx, offsety);
		
		return 0; // this return is a must!
	}
	case WM_LBUTTONUP:
	{
		s_is_dragging = false;
		ReleaseCapture();

		return 0;
	}
	case WM_KEYDOWN:
	{
		bool isCtrl = GetKeyState(VK_CONTROL)<0;
		int scale = isCtrl ? 10 : 1;

		int offsetx = 0, offsety = 0;
		if(wParam==VK_UP)
			offsety = -1 * scale;
		else if(wParam==VK_DOWN)
			offsety = 1 * scale;
		else if(wParam==VK_LEFT)
			offsetx = -1 * scale;
		else if(wParam==VK_RIGHT)
			offsetx = 1 * scale;

		MoveWindow_byOffset(hwnd, offsetx, offsety);

		return 0;
	}
	
	case WM_DESTROY:
		KillTimer (hwnd, ID_TIMER) ;
		DeleteObject (hBrushRed) ;
		PostQuitMessage (0) ;
		return 0 ;
	}}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
