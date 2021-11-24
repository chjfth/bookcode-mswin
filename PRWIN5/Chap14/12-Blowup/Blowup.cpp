/*---------------------------------------
   BLOWUP.C -- Video Magnifier Program
               (c) Charles Petzold, 1998
  ---------------------------------------*/

#include <windows.h>
#include <stdlib.h>      // for abs definition
#include "resource.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName [] = TEXT ("Blowup") ;
	HACCEL       hAccel ;
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
	wndclass.lpszMenuName  = szAppName ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Blow-Up Mouse Demo"), 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	hAccel = LoadAccelerators (hInstance, szAppName) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator (hwnd, hAccel, &msg))
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
	}
	return msg.wParam ;
}

void InvertBlock (HWND hwndScr, HWND hwnd, POINT ptBeg, POINT ptEnd)
{
	HDC hdc ;

	hdc = GetDCEx (hwndScr, NULL, DCX_CACHE | DCX_LOCKWINDOWUPDATE) ;
	ClientToScreen (hwnd, &ptBeg) ;
	ClientToScreen (hwnd, &ptEnd) ;
	PatBlt (hdc, ptBeg.x, ptBeg.y, ptEnd.x - ptBeg.x, ptEnd.y - ptBeg.y,
		DSTINVERT) ;
	ReleaseDC (hwndScr, hdc) ;
}

HBITMAP CopyBitmap (HBITMAP hBitmapSrc)
{
	BITMAP  bitmap ;
	HBITMAP hBitmapDst ;
	HDC     hdcSrc, hdcDst ;

	GetObject (hBitmapSrc, sizeof (BITMAP), &bitmap) ;
	hBitmapDst = CreateBitmapIndirect (&bitmap) ;

	hdcSrc = CreateCompatibleDC (NULL) ;
	hdcDst = CreateCompatibleDC (NULL) ;

	SelectObject (hdcSrc, hBitmapSrc) ;
	SelectObject (hdcDst, hBitmapDst) ;

	BitBlt (hdcDst, 0, 0, bitmap.bmWidth, bitmap.bmHeight,
		hdcSrc, 0, 0, SRCCOPY) ;

	DeleteDC (hdcSrc) ;
	DeleteDC (hdcDst) ;

	return hBitmapDst ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL    s_bCapturing, s_bBlocking ;
	static HBITMAP s_hBitmap ;
	static HWND    s_hwndScr ;
	static POINT   s_ptBeg, s_ptEnd ;
	BITMAP         bm ;
	HBITMAP        hBitmapClip ;
	HDC            hdc, hdcMem ;
	int            iEnable ;
	PAINTSTRUCT    ps ;
	RECT           rect ;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		if (!s_bCapturing)
		{
			if (LockWindowUpdate (s_hwndScr = GetDesktopWindow ()))
			{
				s_bCapturing = TRUE ;
				SetCapture (hwnd) ;
				SetCursor (LoadCursor (NULL, IDC_CROSS)) ;
			}
			else
				MessageBeep (0) ;
		}
		return 0 ;

	case WM_RBUTTONDOWN:
		if (s_bCapturing)
		{
			s_bBlocking = TRUE ;
			s_ptBeg.x = LOWORD (lParam) ;
			s_ptBeg.y = HIWORD (lParam) ;
			s_ptEnd = s_ptBeg ;
			InvertBlock (s_hwndScr, hwnd, s_ptBeg, s_ptEnd) ;
		}
		return 0 ;

	case WM_MOUSEMOVE:
		if (s_bBlocking)
		{
			InvertBlock (s_hwndScr, hwnd, s_ptBeg, s_ptEnd) ;
			s_ptEnd.x = LOWORD (lParam) ;
			s_ptEnd.y = HIWORD (lParam) ;
			InvertBlock (s_hwndScr, hwnd, s_ptBeg, s_ptEnd) ;
		}
		return 0 ;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		if (s_bBlocking)
		{
			InvertBlock (s_hwndScr, hwnd, s_ptBeg, s_ptEnd) ;
			s_ptEnd.x = LOWORD (lParam) ;
			s_ptEnd.y = HIWORD (lParam) ;

			if (s_hBitmap)
			{
				DeleteObject (s_hBitmap) ;
				s_hBitmap = NULL ;
			}

			hdc = GetDC (hwnd) ;
			hdcMem = CreateCompatibleDC (hdc) ;
			s_hBitmap = CreateCompatibleBitmap (hdc, 
				abs (s_ptEnd.x - s_ptBeg.x),
				abs (s_ptEnd.y - s_ptBeg.y)) ;

			SelectObject (hdcMem, s_hBitmap) ;

			StretchBlt (hdcMem, 0, 0, abs (s_ptEnd.x - s_ptBeg.x),
				abs (s_ptEnd.y - s_ptBeg.y), 
				hdc, s_ptBeg.x, s_ptBeg.y, s_ptEnd.x - s_ptBeg.x, 
				s_ptEnd.y - s_ptBeg.y, SRCCOPY) ;

			DeleteDC (hdcMem) ;
			ReleaseDC (hwnd, hdc) ;
			InvalidateRect (hwnd, NULL, TRUE) ;
		}
		if (s_bBlocking || s_bCapturing)
		{
			s_bBlocking = s_bCapturing = FALSE ;
			SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
			ReleaseCapture () ;
			LockWindowUpdate (NULL) ;
		}
		return 0 ;

	case WM_INITMENUPOPUP:
		iEnable = IsClipboardFormatAvailable(CF_BITMAP) ? MF_ENABLED : MF_GRAYED ;

		EnableMenuItem ((HMENU) wParam, IDM_EDIT_PASTE, iEnable) ;

		iEnable = s_hBitmap ? MF_ENABLED : MF_GRAYED ;

		EnableMenuItem ((HMENU) wParam, IDM_EDIT_CUT,    iEnable) ;
		EnableMenuItem ((HMENU) wParam, IDM_EDIT_COPY,   iEnable) ;
		EnableMenuItem ((HMENU) wParam, IDM_EDIT_DELETE, iEnable) ;
		return 0 ;

	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
			if (s_hBitmap)
			{
				hBitmapClip = CopyBitmap (s_hBitmap) ;
				OpenClipboard (hwnd) ;
				EmptyClipboard () ;
				SetClipboardData (CF_BITMAP, hBitmapClip) ;
			}
			if (LOWORD (wParam) == IDM_EDIT_COPY)
				return 0 ;
			// fall through for IDM_EDIT_CUT
		case IDM_EDIT_DELETE:
			if (s_hBitmap)
			{
				DeleteObject (s_hBitmap) ;
				s_hBitmap = NULL ;
			}
			InvalidateRect (hwnd, NULL, TRUE) ;
			return 0 ;

		case IDM_EDIT_PASTE:
			if (s_hBitmap)
			{
				DeleteObject (s_hBitmap) ;
				s_hBitmap = NULL ;
			}
			OpenClipboard (hwnd) ;
			hBitmapClip = (HBITMAP)GetClipboardData (CF_BITMAP) ;

			if (hBitmapClip)
				s_hBitmap = CopyBitmap (hBitmapClip) ;

			CloseClipboard () ;
			InvalidateRect (hwnd, NULL, TRUE) ;
			return 0 ;
		}
		break ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		if (s_hBitmap)
		{
			GetClientRect (hwnd, &rect) ;

			hdcMem = CreateCompatibleDC (hdc) ;
			SelectObject (hdcMem, s_hBitmap) ;
			GetObject (s_hBitmap, sizeof (BITMAP), (PSTR) &bm) ;
			SetStretchBltMode (hdc, COLORONCOLOR) ;

			StretchBlt (hdc,    0, 0, rect.right, rect.bottom,
				hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY) ;

			DeleteDC (hdcMem) ;
		}
		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_DESTROY:
		if (s_hBitmap)
			DeleteObject (s_hBitmap) ;

		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
