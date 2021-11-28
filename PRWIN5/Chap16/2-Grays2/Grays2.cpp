/*-----------------------------------------------
   GRAYS2.C -- Gray Shades Using Palette Manager
               (c) Charles Petzold, 1998
  -----------------------------------------------*/

#include <windows.h>
#include <tchar.h>
#include "..\set-256color-mode.h"

bool g_use_red = false, g_use_green = false, g_use_blue = false;
bool g_prompt_params = false;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Grays2") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	Set_256ColorMode(szAppName);

	// Chj special: Command-line tells to set "gray" scale of red/green/blue.
	//
	const TCHAR *title_append = _T("");
	CharUpperA(szCmdLine);
	if( szCmdLine[0]=='R')
		g_use_red = true, title_append = _T("(R)");
	else if(szCmdLine[0]=='G')
		g_use_green = true, title_append = _T("(G)");
	else if(szCmdLine[0]=='B')
		g_use_blue = true, title_append = _T("(B)");
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

	TCHAR szTitle[100];
	_sntprintf_s(szTitle, ARRAYSIZE(szTitle), _T("Shades of Gray #2 %s"), title_append);

	hwnd = CreateWindow (szAppName, 
		g_prompt_params
			? TEXT ("Shades of Gray #2 (Hint: pass in param R, G, or B to show colored shades)") 
			: szTitle,
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
	static HPALETTE s_hPalette ;
	static int      cxClient, cyClient ;
	HBRUSH          hBrush ;
	HDC             hdc ;
	int             i ;
	LOGPALETTE    * plp ;
	PAINTSTRUCT     ps ;
	RECT            rect ;

	switch (message)
	{
	case WM_CREATE:
		// Set up a LOGPALETTE structure and create a palette

		plp = (LOGPALETTE*) malloc (sizeof (LOGPALETTE) + 64 * sizeof (PALETTEENTRY)) ;

		plp->palVersion    = 0x0300 ;
		plp->palNumEntries = 65 ;

		for (i = 0 ; i < 65 ; i++)
		{
			plp->palPalEntry[i].peRed   = (BYTE)(g_use_red ? min (255, 4 * i) : 0);
			plp->palPalEntry[i].peGreen = (BYTE)(g_use_green ? min (255, 4 * i) : 0);
			plp->palPalEntry[i].peBlue  = (BYTE)(g_use_blue ? min (255, 4 * i) : 0);
			plp->palPalEntry[i].peFlags = 0 ;
		}
		s_hPalette = CreatePalette (plp) ;
		free (plp) ;
		return 0 ;

	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		// Select and realize the palette in the device context

		SelectPalette (hdc, s_hPalette, FALSE) ;
		RealizePalette (hdc) ;

		// Draw the fountain of grays

		for (i = 0 ; i < 65 ; i++)
		{
			rect.left   = i * cxClient / 64 ;
			rect.top    = 0 ;
			rect.right  = (i + 1) * cxClient / 64 ;
			rect.bottom = cyClient ;

			// Key: To enable palette, use PALETTERGB() instead of RGB()
			hBrush = CreateSolidBrush (PALETTERGB (
				g_use_red ? min (255, 4 * i) : 0, 
				g_use_green ? min (255, 4 * i) : 0, 
				g_use_blue ? min (255, 4 * i) : 0
				)) ;

			FillRect (hdc, &rect, hBrush) ;
			DeleteObject (hBrush) ;
		}
		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_QUERYNEWPALETTE:
		if (!s_hPalette)
			return FALSE ;

		vaDbg(_T("See WM_QUERYNEWPALETTE."));

		hdc = GetDC (hwnd) ;
		SelectPalette (hdc, s_hPalette, FALSE) ;
		RealizePalette (hdc) ;
		InvalidateRect (hwnd, NULL, TRUE) ;

		ReleaseDC (hwnd, hdc) ;
		return TRUE ;

	case WM_PALETTECHANGED:
		if (!s_hPalette)
			break;
		
		if((HWND)wParam == hwnd)
		{
			// Chj: wParam is the HWND that caused system-palette change.
			// So, if it is ourself causing the change, we have called
			// SelectPalette() and RealizePalette() in WM_QUERYNEWPALETTE,
			// then no need to call them again.
			vaDbg(_T("See WM_PALETTECHANGED (self HWND)."));
			break ;
		}

		vaDbg(_T("See WM_PALETTECHANGED (HWND=0x%X)."), (DWORD)wParam);

		hdc = GetDC (hwnd) ;
		SelectPalette (hdc, s_hPalette, FALSE) ;
		RealizePalette (hdc) ;
		UpdateColors (hdc) ;

		ReleaseDC (hwnd, hdc) ;
		break ;

	case WM_DESTROY:
		DeleteObject (s_hPalette) ;
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
