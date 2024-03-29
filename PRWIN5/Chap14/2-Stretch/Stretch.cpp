/*----------------------------------------
   STRETCH.C -- StretchBlt Demonstration
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#include <windows.h>

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName [] = TEXT ("Stretch") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_INFORMATION) ;
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

	hwnd = CreateWindow (szAppName, TEXT ("StretchBlt Demo"), 
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

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int  cxClient, cyClient, cxSource, cySource ;
	HDC         hdcClient, hdcWindow ;
	PAINTSTRUCT ps ;

	switch (message)
	{{
	case WM_CREATE:
	{
		int cxsizeframe = GetSystemMetrics (SM_CXSIZEFRAME);
		int cxsize = GetSystemMetrics (SM_CXSIZE);
		cxSource = cxsizeframe + cxsize;

		int cysizeframe = GetSystemMetrics (SM_CYSIZEFRAME);
		int cycaption = GetSystemMetrics (SM_CYCAPTION);
		cySource = cysizeframe + cycaption;
		return 0 ;
	}

	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;

	case WM_PAINT:
	{
		hdcClient = BeginPaint (hwnd, &ps) ;
		hdcWindow = GetWindowDC (hwnd) ;

#if 0 // try raster operation
		SelectObject (hdcClient, CreateHatchBrush (HS_DIAGCROSS, RGB (0, 0, 0)));
		StretchBlt (hdcClient, 0, 0, cxClient, cyClient,
			hdcWindow, 0, 0, cxSource, cySource, MERGECOPY) ;
		DeleteObject (SelectObject(hdcClient, GetStockObject (WHITE_BRUSH))) ;
#else
		StretchBlt (hdcClient, 0, 0, cxClient, cyClient,
			hdcWindow, 0, 0, cxSource, cySource, MERGECOPY) ;
#endif 

		ReleaseDC (hwnd, hdcWindow) ;
		EndPaint (hwnd, &ps) ;
		return 0 ;
	}
	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
