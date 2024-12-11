/*-------------------------------------------------
   CHECKER4.C -- Mouse Hit-Test Demo Program No. 4
                 (c) Charles Petzold, 1998
  -------------------------------------------------*/

#include <windows.h>
#include "vaDbg.h"

#define DIVISIONS 5

LRESULT CALLBACK WndProc   (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK ChildWndProc (HWND, UINT, WPARAM, LPARAM) ;

int   idFocus = 0 ;
TCHAR szChildClass[] = TEXT ("Checker4_Child") ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Checker4") ;
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
	wndclass.cbWndExtra    = sizeof (long) ;
	wndclass.hIcon         = NULL ;
	wndclass.lpszClassName = szChildClass ;

	RegisterClass (&wndclass) ;

	hwnd = CreateWindow (szAppName, TEXT ("Checker4 Mouse Hit-Test Demo"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		400, 300,
		NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	vaDbgTs(_T("Checker4 main-hwnd: 0x%08X"), hwnd);

	while (GetMessage (&msg, NULL, 0, 0))
	{
		if(msg.message==WM_KEYDOWN)
		{
			vaDbgTs(_T("GetMessage() sees WM_KEYDOWN, target-hwnd=0x%08X"), msg.hwnd);
		}

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return (int)msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndChild[DIVISIONS][DIVISIONS] ;
	int         cxBlock, cyBlock, x, y ;

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
			}
		}
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

	// On set-focus message, set focus to child window

	case WM_SETFOCUS:
		vaDbgTs(_T("Main-window WM_SETFOCUS. Losing-focus hwnd: 0x%08X"), wParam);
		
		SetFocus (GetDlgItem (hwnd, idFocus)) ;
		return 0 ;

	case WM_KILLFOCUS:
		vaDbgTs(_T("Main-window WM_KILLFOCUS. Gaining-focus hwnd: 0x%08X"), wParam);
		
		return 0;

	// On key-down message, possibly change the focus window

	case WM_KEYDOWN:
		x = idFocus & 0xFF ;
		y = idFocus >> 8 ;

		switch (wParam)
		{
		case VK_UP:    y-- ;                    break ;
		case VK_DOWN:  y++ ;                    break ;
		case VK_LEFT:  x-- ;                    break ;
		case VK_RIGHT: x++ ;                    break ;
		case VK_HOME:  x = y = 0 ;              break ;
		case VK_END:   x = y = DIVISIONS - 1 ;  break ;
		default:       return 0 ;
		}

		x = (x + DIVISIONS) % DIVISIONS ;
		y = (y + DIVISIONS) % DIVISIONS ;

		idFocus = y << 8 | x ;

		SetFocus (GetDlgItem (hwnd, idFocus)) ;
		return 0 ;

	case WM_DESTROY :
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

LRESULT CALLBACK ChildWndProc (HWND hwnd, UINT message, 
	WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	RECT        rect ;

	switch (message)
	{
	case WM_CREATE :
		SetWindowLongPtr (hwnd, 0, 0) ;       // on/off flag
		return 0 ;

	case WM_KEYDOWN:
		// Send most key presses to the parent window

		if (wParam != VK_RETURN && wParam != VK_SPACE)
		{
			SendMessage (GetParent (hwnd), message, wParam, lParam) ;
			return 0 ;
		}
		// For Return and Space, fall through to toggle the square

	case WM_LBUTTONDOWN :
		SetWindowLongPtr (hwnd, 0, 1 ^ GetWindowLongPtr (hwnd, 0)) ;
		SetFocus (hwnd) ;
		InvalidateRect (hwnd, NULL, FALSE) ;
		return 0 ;

		// For focus messages, invalidate the window for repaint

	case WM_SETFOCUS:
		idFocus = GetWindowLongPtr (hwnd, GWLP_ID) ;

		// Fall through

	case WM_KILLFOCUS:
		InvalidateRect (hwnd, NULL, TRUE) ;
		return 0 ;

	case WM_PAINT :
		hdc = BeginPaint (hwnd, &ps) ;

		GetClientRect (hwnd, &rect) ;
		Rectangle (hdc, 0, 0, rect.right, rect.bottom) ;

		// Draw the "x" mark

		if (GetWindowLongPtr (hwnd, 0))
		{
			MoveToEx (hdc, 0,          0, NULL) ;
			LineTo   (hdc, rect.right, rect.bottom) ;
			MoveToEx (hdc, 0,          rect.bottom, NULL) ;
			LineTo   (hdc, rect.right, 0) ;
		}

		// Draw the "focus" rectangle

		if (hwnd == GetFocus ())
		{
			rect.left   += rect.right / 10 ;
			rect.right  -= rect.left ;
			rect.top    += rect.bottom / 10 ;
			rect.bottom -= rect.top ;

			SelectObject (hdc, GetStockObject (NULL_BRUSH)) ;
			SelectObject (hdc, CreatePen (PS_DASH, 0, 0)) ;
			Rectangle (hdc, rect.left, rect.top, rect.right, rect.bottom) ;
			DeleteObject (SelectObject (hdc, GetStockObject (BLACK_PEN))) ;
		}

		EndPaint (hwnd, &ps) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
