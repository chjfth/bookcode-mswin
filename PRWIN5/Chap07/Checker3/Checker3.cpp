/*-------------------------------------------------
   CHECKER3.C -- Mouse Hit-Test Demo Program No. 3
                 (c) Charles Petzold, 1998

[2024-12-10] Chj special: 
	I set up a 10-second timer for WndProc, that reports GetFocus() result.
	In the timer-callback, I also check if client area size has changed. 
	* If changed to landscape rect, I'll SetFocus to main-window.
	* If changed to portrait rect , I'll SetFocus to the centric child window.
	These help observe WM_SETFOCUS/WM_KILLFOCUS behavior, especially 
	the interaction among main-window and child-window..
				 
-------------------------------------------------*/

#include <windows.h>
#include "vaDbg.h"

#define MY_TIMER_ID 10

int g_orig_clix, g_orig_cliy; // original client-area width/height

#define DIVISIONS 5

LRESULT CALLBACK WndProc   (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK ChildWndProc (HWND, UINT, WPARAM, LPARAM) ;

TCHAR szChildClass[] = TEXT ("Checker3_Child") ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Checker3") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon(hInstance, TEXT("MYPROGRAM"));
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

	wndclass.lpfnWndProc   = ChildWndProc ;
	wndclass.cbWndExtra    = sizeof (long); // Reserve a `long` value for a window instance
	wndclass.hIcon         = NULL ;
	wndclass.lpszClassName = szChildClass ;

	RegisterClass (&wndclass) ;

	hwnd = CreateWindow (szAppName, TEXT ("Checker3 Mouse Hit-Test Demo"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		400, 400,
		NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	vaDbgTs(_T("Checker3 main-hwnd: 0x%08X"), hwnd);
	static int s_msgcount = 0;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		s_msgcount++;
		vaDbgTs(_T("#%d Got message %d (hwnd=0x%08X) >>>"), s_msgcount, msg.message, msg.hwnd);

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;

		vaDbgTs(_T("#%d Got message %d (hwnd=0x%08X) <<<"), s_msgcount, msg.message, msg.hwnd);
	}
	return (int)msg.wParam ;
}

void CALLBACK ChjTimer(HWND hwnd, UINT msg, UINT_PTR timerId, DWORD tick)
{
	(void)hwnd; (void)msg; (void)tick;
	if(timerId==MY_TIMER_ID)
	{
		RECT rc = {};
		GetClientRect(hwnd, &rc);

		HWND hwndNowFocus = GetFocus();
		vaDbgTs(_T("GetFocus() = 0x%08X"), hwndNowFocus);

		if(rc.right==g_orig_clix && rc.bottom==g_orig_cliy)
		{
			// window size not changed yet, do nothing more
			return; 
		}

		if(rc.right > rc.bottom) // landscape window
		{
			vaDbgTs(_T(">>> SetFocus() to main window 0x%08X."), hwnd);
			HWND prevFocus = SetFocus( hwnd );
			vaDbgTs(_T("<<< SetFocus() reports prev-focus: 0x%08X."), prevFocus);
		}

		if(rc.right < rc.bottom) // portrait window
		{
			HWND hCentric = GetDlgItem(hwnd, (2<<8)|2 );
			vaDbgTs(_T(">>> SetFocus() to centric child 0x%08X."), hCentric);
			HWND prevFocus = SetFocus( hCentric );
			vaDbgTs(_T("<<< SetFocus() reports prev-focus: 0x%08X."), prevFocus);
		}
	}
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndChild[DIVISIONS][DIVISIONS] ;
	int         cxBlock, cyBlock, x, y ;
	RECT rc = {};

	switch (message)
	{
	case WM_CREATE :
		for (x = 0 ; x < DIVISIONS ; x++)
		{
			for (y = 0 ; y < DIVISIONS ; y++)
			{
				hwndChild[x][y] = CreateWindow (szChildClass, NULL,
					WS_CHILDWINDOW | WS_VISIBLE,
					0, 0, 0, 0,
					hwnd, (HMENU) (y << 8 | x),
					(HINSTANCE) GetWindowLongPtr (hwnd, GWLP_HINSTANCE),
					NULL) ;

				vaDbgS(_T("Created child window: [%d,%d] hwnd=0x%08X"), x, y, hwndChild[x][y]);
			}
		}

		// Chj special:
		SetTimer(hwnd, MY_TIMER_ID, 10*1000, ChjTimer);
		GetClientRect(hwnd, &rc);
		g_orig_clix = rc.right, g_orig_cliy = rc.bottom;

		return 0 ;

	case WM_SIZE :
		cxBlock = LOWORD (lParam) / DIVISIONS ;
		cyBlock = HIWORD (lParam) / DIVISIONS ;

		for (x = 0 ; x < DIVISIONS ; x++)
		{
			for (y = 0 ; y < DIVISIONS ; y++)
			{
				MoveWindow (hwndChild[x][y],
					x * cxBlock, y * cyBlock,
					cxBlock, cyBlock, TRUE) ;
			}
		}
		return 0 ;

	case WM_LBUTTONDOWN :
		MessageBeep (0) ;
		return 0 ;

	case WM_DESTROY :
		KillTimer(hwnd, MY_TIMER_ID);
		PostQuitMessage (0) ;
		return 0 ;

	case WM_SETFOCUS:
		vaDbgTs(_T("WndProc.WM_SETFOCUS, Losing-focus hwnd=0x%08X"), wParam);
		return 0;
	case WM_KILLFOCUS:
		vaDbgTs(_T("WndProc.WM_KILLFOCUS, Gaining-focus hwnd=0x%08X"), wParam);
		return 0;
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

LRESULT CALLBACK ChildWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	RECT        rect ;

	switch (message)
	{
	case WM_CREATE :
		SetWindowLongPtr (hwnd, 0, 0) ; // Set the extra `long` flag: on/off
		return 0 ;

	case WM_LBUTTONDOWN :
		SetWindowLongPtr (hwnd, 0, 1^GetWindowLongPtr (hwnd, 0)); // Toggle the extra `long`
		InvalidateRect (hwnd, NULL, FALSE) ;
		return 0 ;

	case WM_PAINT :
		hdc = BeginPaint (hwnd, &ps) ;

		GetClientRect (hwnd, &rect) ;
		Rectangle (hdc, 0, 0, rect.right, rect.bottom) ;

		if (GetWindowLongPtr (hwnd, 0))
		{
			MoveToEx (hdc, 0,          0, NULL) ;
			LineTo   (hdc, rect.right, rect.bottom) ;
			MoveToEx (hdc, 0,          rect.bottom, NULL) ;
			LineTo   (hdc, rect.right, 0) ;
		}

		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_SETFOCUS:
		vaDbgTs(_T("ChildWndProc.WM_SETFOCUS, Losing-focus hwnd=0x%08X"), wParam);
		return 0;
	case WM_KILLFOCUS:
		vaDbgTs(_T("ChildWndProc.WM_KILLFOCUS, Gaining-focus hwnd=0x%08X"), wParam);
		return 0;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
