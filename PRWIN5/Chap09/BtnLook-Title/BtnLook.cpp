/*----------------------------------------
   Origin: BTNLOOK.C -- Button Look Program
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#include <windows.h>
#include <shlwapi.h>
#include <CommCtrl.h>
#include "resource.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

struct
{
	int     iStyle ;
	TCHAR * szText ;
}
button[] =
{
	BS_PUSHBUTTON,      TEXT ("PUSHBUTTON"),
	BS_DEFPUSHBUTTON,   TEXT ("DEFPUSHBUTTON"),
	BS_CHECKBOX,        TEXT ("CHECKBOX"), 
	BS_AUTOCHECKBOX,    TEXT ("AUTOCHECKBOX"),
	BS_RADIOBUTTON,     TEXT ("RADIOBUTTON"),
	BS_3STATE,          TEXT ("3STATE"),
	BS_AUTO3STATE,      TEXT ("AUTO3STATE"),
	BS_GROUPBOX,        TEXT ("GROUPBOX"),
	BS_AUTORADIOBUTTON, TEXT ("AUTORADIO"),
	BS_OWNERDRAW,       TEXT ("OWNERDRAW")
} ;

#define NUM (sizeof button / sizeof button[0])

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	static TCHAR szAppName[] = TEXT ("BtnLook") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, 
		TEXT("BtnLook"), // will modify later
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		540, 420,
		NULL, NULL, hInstance, NULL) ;

	// Set window title to be EXE name and HWND value).
	TCHAR szTitle[MAX_PATH] = {0};
	GetModuleFileName(NULL, szTitle, MAX_PATH);
	PathStripPath(szTitle);
	PathRemoveExtension(szTitle);

	wsprintf(szTitle, TEXT("%s (#%08X)"), szTitle, (UINT)hwnd);
	SetWindowText(hwnd, szTitle);

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return (int)msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND  hwndButton[NUM] ;
	static RECT  rect ;
	static TCHAR szTop[]    = TEXT ("message            wParam       lParam"),
	    szUnd[]    = TEXT ("_______            ______       ______"),
		szFormat[] = TEXT ("%-16s%04X-%04X    %04X-%04X"),
		szBuffer[50] ;
	static int   cxChar, cyChar ;
	HDC          hdc ;
	PAINTSTRUCT  ps ;
	int          i ;

	switch (message)
	{
	case WM_CREATE :

		cxChar = LOWORD (GetDialogBaseUnits ()) ;
		cyChar = HIWORD (GetDialogBaseUnits ()) ;

		for (i = 0 ; i < NUM ; i++)
		{
			hwndButton[i] = CreateWindow ( TEXT("button"), 
				button[i].szText,
				WS_CHILD | WS_VISIBLE | button[i].iStyle,
				cxChar, cyChar * (1 + 2 * i),
				20 * cxChar, 7 * cyChar / 4,
				hwnd, 
				(HMENU) i,
				((LPCREATESTRUCT) lParam)->hInstance, 
				NULL) ;
		}
		return 0 ;

	case WM_SIZE :
		rect.left   = 24 * cxChar ;
		rect.top    =  2 * cyChar ;
		rect.right  = LOWORD (lParam) ;
		rect.bottom = HIWORD (lParam) ;
		return 0 ;

	case WM_PAINT :
		InvalidateRect (hwnd, &rect, TRUE) ;

		hdc = BeginPaint (hwnd, &ps) ;
		SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
		SetBkMode (hdc, TRANSPARENT) ;
          
		TextOut (hdc, 24 * cxChar, cyChar, szTop, lstrlen (szTop)) ;
		TextOut (hdc, 24 * cxChar, cyChar, szUnd, lstrlen (szUnd)) ;
		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_DRAWITEM :
	case WM_COMMAND :

		ScrollWindow (hwnd, 0, -cyChar, &rect, &rect) ;

		hdc = GetDC (hwnd) ;
		SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

		TextOut (hdc, 24 * cxChar, cyChar * (rect.bottom / cyChar - 1),
			szBuffer,
			wsprintf (szBuffer, szFormat,
			message == WM_DRAWITEM ? TEXT ("WM_DRAWITEM") : 
			TEXT ("WM_COMMAND"),
			HIWORD (wParam), LOWORD (wParam),
			HIWORD (lParam), LOWORD (lParam))) ;

		ReleaseDC (hwnd, hdc) ;
		ValidateRect (hwnd, &rect) ;
		break ;

	case WM_DESTROY :
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}