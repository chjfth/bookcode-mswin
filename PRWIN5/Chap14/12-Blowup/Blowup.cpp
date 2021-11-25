/*---------------------------------------
   BLOWUP.C -- Video Magnifier Program
               (c) Charles Petzold, 1998
  ---------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
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
	// Input `hwnd` is only used for ClientToScreen coord translation.

	HDC hdc ;

	hdc = GetDCEx (hwndScr, NULL, DCX_CACHE | DCX_LOCKWINDOWUPDATE) ;
	ClientToScreen (hwnd, &ptBeg) ;
	ClientToScreen (hwnd, &ptEnd) ;
	
	PatBlt (hdc, ptBeg.x, ptBeg.y, ptEnd.x - ptBeg.x, ptEnd.y - ptBeg.y,
		DSTINVERT) ; // invert screen color

	ReleaseDC (hwndScr, hdc) ;
}

HBITMAP CopyBitmap (HBITMAP hBitmapSrc)
{
	BITMAP  bitmap = {};
	HBITMAP hBitmapDst ;
	HDC     hdcSrc, hdcDst ;

	int bytes_required = GetObject (hBitmapSrc, 0, NULL); // query buffer size required
	assert(bytes_required==sizeof(BITMAP));

	int bytes_copied = GetObject (hBitmapSrc, sizeof (bitmap), &bitmap);
	//
	// [2021-11-25] Chj memo: A strange Windows XP bug?
	// If we run the program inside a WinXP VM, a weird symptom happens:  
	// Operations:
	// * On Host PC(Win10), press Alt+PrintScreen to copy a window bitmap.
	// * Switch to the VM, so the clipboard content(with a bitmap) is transferred to the 
	//   clipboard inside the WinXP VM(automatically).
	// * In VM, open mspaint.exe, press Ctrl+V, we can verify that the bitmap is pasted.
	// * Now, switch to Blowup program, Ctrl+V paste, and this CopyBitmap function
	//   is triggered.
	// * Weird things happen here: 
	//   - bytes_required gets value 24, which is exactly sizeof(BITMAP).
	//   - bytes_copied is 0, which means GetObject() fails to return BITMAP struct.
	// WHY?

	if(bytes_copied!=bytes_required)
	{
		TCHAR errtext[200] = {};
		_sntprintf_s(errtext, ARRAYSIZE(errtext),
			_T("Error: GetObject() returns %d for HBITMAP from clipboard (should be %d).\r\n")
			_T("\r\n")
			_T("This may happen on WinXP, but should not happen on Win7.\r\n")
			,
			bytes_copied, bytes_required);

		MessageBox(NULL, errtext, _T("Unexpected Error"), MB_OK|MB_ICONWARNING);
		return NULL;
	}
	
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
			s_ptBeg.x = GET_X_LPARAM(lParam) ;
			s_ptBeg.y = GET_Y_LPARAM(lParam) ;
			s_ptEnd = s_ptBeg ;
			InvertBlock (s_hwndScr, hwnd, s_ptBeg, s_ptEnd) ;
		}
		return 0 ;

	case WM_MOUSEMOVE:
		if (s_bBlocking)
		{
			InvertBlock (s_hwndScr, hwnd, s_ptBeg, s_ptEnd) ;
			s_ptEnd.x = GET_X_LPARAM(lParam) ;
			s_ptEnd.y = GET_Y_LPARAM(lParam) ;
			InvertBlock (s_hwndScr, hwnd, s_ptBeg, s_ptEnd) ;
		}
		return 0 ;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		if (s_bBlocking)
		{
			InvertBlock (s_hwndScr, hwnd, s_ptBeg, s_ptEnd) ; // turn inverted figure back to normal

			s_ptEnd.x = GET_X_LPARAM(lParam) ;
			s_ptEnd.y = GET_Y_LPARAM(lParam) ;

			if (s_hBitmap)
			{
				DeleteObject (s_hBitmap) ;
				s_hBitmap = NULL ;
			}

			int cx_diff = s_ptEnd.x - s_ptBeg.x; // can be negative
			int cy_diff = s_ptEnd.y - s_ptBeg.y; // can be negative
			int abs_width = abs(cx_diff);
			int abs_height = abs(cy_diff);

			hdc = GetDC (hwnd) ;
			hdcMem = CreateCompatibleDC (hdc) ;
			s_hBitmap = CreateCompatibleBitmap (hdc, abs_width, abs_height);
			SelectObject (hdcMem, s_hBitmap) ;

			// note: s_ptBeg, s_ptEnd is relative to hwnd(hdc),
			// and the POINT .x, .y values can go beyond the hwnd area.

			StretchBlt (hdcMem, 0, 0, abs_width, abs_height,
				hdc, s_ptBeg.x, s_ptBeg.y, cx_diff, cy_diff, 
				SRCCOPY) ;

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

			iEnable =  IsClipboardFormatAvailable(CF_BITMAP);
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
