#include <windows.h>
#include "PrintWindow.h"

void fengyuan_DoPrintWindow(HWND hWnd)
{
	// Sample code from:
	// http://www.fengyuan.com/article/wmprint.html

	HDC hDCMem = CreateCompatibleDC(NULL);
	RECT rect;
	GetWindowRect(hWnd, & rect);

	HBITMAP hBmp = NULL;
	{
		HDC hDC = GetDC(hWnd);
		hBmp = CreateCompatibleBitmap(hDC, rect.right - rect.left, rect.bottom - rect.top);
		ReleaseDC(hWnd, hDC);
	}

	HGDIOBJ hOld = SelectObject(hDCMem, hBmp);
	SendMessage(hWnd, WM_PRINT, (WPARAM) hDCMem, 
		PRF_CHILDREN | PRF_CLIENT | PRF_ERASEBKGND | PRF_NONCLIENT | PRF_OWNED);

	SelectObject(hDCMem, hOld);
	DeleteObject(hDCMem);

	OpenClipboard(hWnd);

	EmptyClipboard(); 
	SetClipboardData(CF_BITMAP, hBmp);
	CloseClipboard();
}

