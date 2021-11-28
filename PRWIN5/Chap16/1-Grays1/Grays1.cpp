/*---------------------------------------
   GRAYS1. -- Gray Shades
               (c) Charles Petzold, 1998
  ---------------------------------------*/

#include <windows.h>
#include "..\set-256color-mode.h"

bool g_use_red = false, g_use_green = false, g_use_blue = false;
bool g_prompt_params = false;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Grays1") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	Set_256ColorMode(szAppName);

	// Chj special: Command-line tells to set "gray" scale of red/green/blue.
	//
	CharUpperA(szCmdLine);
	if( szCmdLine[0]=='R')
		g_use_red = true;
	else if(szCmdLine[0]=='G')
		g_use_green = true;
	else if(szCmdLine[0]=='B')
		g_use_blue = true;
	else {
		g_use_red = g_use_green = g_use_blue = true;
		g_prompt_params = true;
	}

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

	hwnd = CreateWindow (szAppName, 
		g_prompt_params
			? TEXT ("Shades of Gray #1 (Hint: pass in param R, G, or B to show colored shades)") 
			: TEXT ("Shades of Gray #1"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		648, 300, // CW_USEDEFAULT, CW_USEDEFAULT,             
		NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int  cxClient, cyClient ;
	HBRUSH      hBrush ;
	HDC         hdc ;
	int         i ;
	PAINTSTRUCT ps ;
	RECT        rect ;

	switch (message)
	{
	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		// Draw the fountain of grays

		for (i = 0 ; i < 65 ; i++)
		{
			rect.left   = i * cxClient / 65 ;
			rect.top    = 0 ;
			rect.right  = (i + 1) * cxClient / 65 ;
			rect.bottom = cyClient ;

			hBrush = CreateSolidBrush (RGB (
				g_use_red ? min (255, 4 * i) : 0, 
				g_use_green ? min (255, 4 * i) : 0, 
				g_use_blue ? min (255, 4 * i) : 0
				)) ;

			FillRect (hdc, &rect, hBrush) ;
			DeleteObject (hBrush) ;
		}
		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
