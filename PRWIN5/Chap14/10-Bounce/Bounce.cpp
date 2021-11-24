/*---------------------------------------
   BOUNCE.C -- Bouncing Ball Program
               (c) Charles Petzold, 1998
  ---------------------------------------*/

#include <windows.h>
#define ID_TIMER    1

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Bounce") ;
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

	hwnd = CreateWindow (szAppName, TEXT ("Bouncing Ball"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
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

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap ;
	static int     cxClient, cyClient, xCenter, yCenter, cxTotal, cyTotal,
		cxRadius, cyRadius, cxMove, cyMove ;
	HBRUSH         hBrush ;
	HDC            hdc, hdcMem ;
	int            iScale ;

	switch (iMsg)
	{
	case WM_CREATE:
		SetTimer (hwnd, ID_TIMER, 50, NULL) ;
		return 0 ;

	case WM_SIZE:
		xCenter = (cxClient = LOWORD (lParam)) / 2 ;
		yCenter = (cyClient = HIWORD (lParam)) / 2 ;

		iScale = min (cxClient, cyClient) / 16 ;

		cxRadius = iScale;
		cyRadius = iScale;

		cxMove = max (1, cxRadius / 2) ; // pixels of each move(step)
		cyMove = max (1, cyRadius / 2) ;

		cxTotal = 2 * (cxRadius + cxMove) ;
		cyTotal = 2 * (cyRadius + cyMove) ;

		if (hBitmap)
			DeleteObject (hBitmap) ;

		hdc = GetDC (hwnd) ;
		hdcMem = CreateCompatibleDC (hdc) ;
		hBitmap = CreateCompatibleBitmap (hdc, cxTotal, cyTotal) ; 
		// only a bit larger than the ball size, the ball size and its surrounding white frame(margin).
		// Painting this margin-surrounding ball at a new position will simultaneously wipe out previous ball figure.
		ReleaseDC (hwnd, hdc) ;

		SelectObject (hdcMem, hBitmap) ;
		Rectangle (hdcMem, -1, -1, cxTotal + 1, cyTotal + 1) ; // paint the ball-bmp background all white

		hBrush = CreateHatchBrush (HS_DIAGCROSS, 0L) ;
		SelectObject (hdcMem, hBrush) ;
		SetBkColor (hdcMem, RGB (255, 0, 255)) ;
		Ellipse (hdcMem, cxMove, cyMove, cxTotal - cxMove, cyTotal - cyMove) ;
		DeleteDC (hdcMem) ;
		DeleteObject (hBrush) ;
		return 0 ;

	case WM_TIMER:
		if (!hBitmap)
			break ;

		hdc = GetDC (hwnd) ;
		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;

		BitBlt (hdc, xCenter - cxTotal / 2,
			yCenter - cyTotal / 2, cxTotal, cyTotal,
			hdcMem, 0, 0, SRCCOPY) ;

		ReleaseDC (hwnd, hdc) ;
		DeleteDC (hdcMem) ;

		xCenter += cxMove ;
		yCenter += cyMove ;

		if ((xCenter + cxRadius >= cxClient) || (xCenter - cxRadius <= 0))
			cxMove = -cxMove ;

		if ((yCenter + cyRadius >= cyClient) || (yCenter - cyRadius <= 0))
			cyMove = -cyMove ;

		return 0 ;

	case WM_DESTROY:
		if (hBitmap)
			DeleteObject (hBitmap) ;

		KillTimer (hwnd, ID_TIMER) ;
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}
