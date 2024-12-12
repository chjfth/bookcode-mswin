/*-------------------------------------------------
   CHECKER4.C -- Mouse Hit-Test Demo Program No. 4
                 (c) Charles Petzold, 1998

[2024-12-12] Chj: Add debug code to observe rob-focus behavior. (p288)
When child #0 has focus and user clicks on child #1, child #0 will
rob focus back to child #0. In such situation, we observe how 
WM_SETFOCUS and WM_KILLFOCUS behave.

Concise: A SetFocus() call triggers a pair of nested WM_KILLFOCUS/WM_SETFOCUS
callback. But, if WinMan finds that:
    After WM_KILLFOCUS, GetFocus() has diverged.
Then, WinMan will drop out(=NOT send) that trailing WM_SETFOCUS.

Key code: ChildWndProc WM_SETFOCUS, WM_KILLFOCUS processing and debug message.

Evernote timetag: 20241212.1
  -------------------------------------------------*/

#include <windows.h>
#include "vaDbg.h"

#define DIVISIONS 5

LRESULT CALLBACK MainWndProc   (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK ChildWndProc (HWND, UINT, WPARAM, LPARAM) ;

int   g_idFocus = 0 ;
TCHAR szChildClass[] = TEXT ("Checker4_Child") ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Checker4") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = MainWndProc ;
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

	hwnd = CreateWindow (szAppName, TEXT ("Checker4-RobFocus"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		400, 300,
		NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	vaDbgTs(_T("Checker4-RobFocus main-hwnd: 0x%08X"), hwnd);

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return (int)msg.wParam ;
}

void mainwin_WM_SETFOCUS(HWND hwndMain, HWND hwndWhoLoseFocus)
{
	vaDbgTs(_T("Main-window got WM_SETFOCUS. Losing-focus hwnd: 0x%08X"), hwndWhoLoseFocus);

	HWND hwndChild = GetDlgItem(hwndMain, g_idFocus);

	vaDbgTs(_T("> Main-window SetFocus(0x%08X) to child #%d"), hwndChild, g_idFocus);
	SetFocus (GetDlgItem (hwndMain, g_idFocus)) ;
	vaDbgTs(_T("< Main-window SetFocus() done."));
}

void mainwin_WM_KILLFOCUS(HWND hwndMain, HWND hwndWhoGainFocus)
{
	vaDbgTs(_T("Main-window got WM_KILLFOCUS. Gaining-focus hwnd: 0x%08X"), hwndWhoGainFocus);
}

LRESULT CALLBACK MainWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		mainwin_WM_SETFOCUS(hwnd, (HWND)wParam);
		return 0 ;

	case WM_KILLFOCUS:
		mainwin_WM_KILLFOCUS(hwnd, (HWND)wParam);
		return 0;

	// On key-down message, possibly change the focus window

	case WM_KEYDOWN:
		x = g_idFocus & 0xFF ;
		y = g_idFocus >> 8 ;

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

		g_idFocus = y << 8 | x ;

		{
			HWND hwndChildFocus = GetDlgItem(hwnd, g_idFocus);
			int idChildFocus = GetDlgCtrlID(hwndChildFocus);

			vaDbgTs(_T(">>> Keydown SetFocus(0x%08X) to child #%d"), hwndChildFocus, idChildFocus);
			SetFocus(hwndChildFocus);
			vaDbgTs(_T("<<< Keydown SetFocus(0x%08X) to child #%d"), hwndChildFocus, idChildFocus);
		}

		return 0 ;

	case WM_DESTROY :
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

inline int childid_to_seq(int childid)
{
	return (childid>>8)*DIVISIONS + (childid&0xff);
}

int cfseq() // Get Current focused Child-seq (0~24)
{
	HWND hwnd = GetFocus();
	int childid = GetDlgCtrlID(hwnd);
	return childid_to_seq(childid);
}

void Test_RobBackFocus(HWND hwndLoseFocus, HWND hwndGainFocus)
{
	// Chj special: If hwndLoseFocus is child #0 and hwndGainFocus is child #1, 
	// Rob focus back to #0.

	int cidLoseFocus = GetDlgCtrlID(hwndLoseFocus);
	int cidGainFocus = GetDlgCtrlID(hwndGainFocus);

	if(cidLoseFocus==0 && cidGainFocus==1)
	{
		vaDbgTs(_T(">> Child #0 SetFocus(0x%08X) rob back focus. (*%d)"), hwndLoseFocus, cfseq());
		SetFocus(hwndLoseFocus); // rob focus back
		vaDbgTs(_T("<< Child #0 SetFocus(0x%08X) rob back focus. (*%d)"), hwndLoseFocus, cfseq());
	}
}


LRESULT CALLBACK ChildWndProc (HWND hwnd, UINT message, 
	WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	RECT        rect ;

	int childid = GetDlgCtrlID(hwnd);
	int childseq = childid_to_seq(childid);

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

		;;;;;;;;
		// For Return and Space, fall through to toggle the square
		;;;;;;;;

	case WM_LBUTTONDOWN :
		SetWindowLongPtr (hwnd, 0, 1 ^ GetWindowLongPtr (hwnd, 0)) ;
		
		vaDbgTs(_T(">>> Child #%d SetFocus(0x%08X) to self."), childseq, hwnd);
		SetFocus (hwnd) ;
		vaDbgTs(_T("<<< Child #%d SetFocus(0x%08X) done."), childseq, hwnd);
		
		InvalidateRect (hwnd, NULL, FALSE) ;
		return 0 ;

	case WM_SETFOCUS:
		
		vaDbgTs(_T("Child #%d got WM_SETFOCUS. (*%d)"), childseq, cfseq());

		g_idFocus = childid; // GetWindowLongPtr (hwnd, GWLP_ID) ;

		InvalidateRect (hwnd, NULL, TRUE); // need repaint
		return 0 ;

	case WM_KILLFOCUS:

		vaDbgTs(_T("Child #%d got WM_KILLFOCUS. (*%d)"), childseq, cfseq());

		Test_RobBackFocus(hwnd, (HWND)wParam);

		InvalidateRect (hwnd, NULL, TRUE); // need repaint
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
