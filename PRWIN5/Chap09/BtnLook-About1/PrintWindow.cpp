#include <windows.h>
#include "PrintWindow.h"

void fengyuan_DoPrintWindow(HWND hWnd)
{
	// Sample code from:
	// http://www.fengyuan.com/article/wmprint.html

	BOOL b = FALSE;
	HDC hDCMem = CreateCompatibleDC(NULL);
	// -- NULL : creates a memory DC compatible with the application's current screen
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
		PRF_CHILDREN | 
		PRF_CLIENT | 
		PRF_ERASEBKGND | 
		PRF_NONCLIENT | 
		PRF_OWNED |
		0);

	SelectObject(hDCMem, hOld);
	b = DeleteDC(hDCMem); // DeleteObject(hDCMem);
	// -- Use DeleteDC() according to MSDN. DeleteObject(hDCMem) returns TRUE as well.

	b = OpenClipboard(hWnd);
	b = EmptyClipboard(); 
	HANDLE clipdata = SetClipboardData(CF_BITMAP, hBmp);
	// -- Now, we can use Free Clipboard Viewer to view the bitmap in the clipboard.

	b = CloseClipboard();
}

