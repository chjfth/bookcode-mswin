/*--------------------------------------------------------
   DIBSECT.C -- Displays a DIB Section in the client area
                (c) Charles Petzold, 1998
  --------------------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <assert.h>
#include "resource.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

TCHAR szAppName[] = TEXT ("DibSect") ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	HWND     hwnd ;
	MSG      msg ;
	WNDCLASS wndclass ;

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

	hwnd = CreateWindow (szAppName, TEXT ("DIB Section Display"),
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

HBITMAP CreateDibSectionFromDibFile (PTSTR szFileName)
{
	BITMAPFILEHEADER bmfh ;
	BITMAPINFO     * pbmi ;
	BYTE           * pBits ;
	BOOL             bSuccess ;
	DWORD            dwInfoSize, dwBytesRead ;
	HANDLE           hFile ;
	HBITMAP          hBitmap ;

	// Open the file: read access, prohibit write access

	hFile = CreateFile (szFileName, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
		return NULL ;

	// Read in the BITMAPFILEHEADER

	bSuccess = ReadFile (hFile, &bmfh, sizeof (BITMAPFILEHEADER), 
		&dwBytesRead, NULL) ;

	if (!bSuccess || (dwBytesRead != sizeof (BITMAPFILEHEADER))         
		|| (bmfh.bfType != * (WORD *) "BM"))
	{
		CloseHandle (hFile) ;
		return NULL ;
	}

	// Allocate memory for the BITMAPINFO structure & read it in

	dwInfoSize = bmfh.bfOffBits - sizeof (BITMAPFILEHEADER) ;

	pbmi = (BITMAPINFO*) malloc (dwInfoSize) ;

	bSuccess = ReadFile (hFile, pbmi, dwInfoSize, &dwBytesRead, NULL) ;

	if (!bSuccess || (dwBytesRead != dwInfoSize))
	{
		free (pbmi) ;
		CloseHandle (hFile) ;
		return NULL ;
	}

	// Create the DIB Section

	hBitmap = CreateDIBSection (NULL, pbmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0) ;

	if (hBitmap == NULL)
	{
		free (pbmi) ;
		CloseHandle (hFile) ;
		return NULL ;
	}

	// Read in the bitmap bits

	DWORD bytes_to_read = bmfh.bfSize - bmfh.bfOffBits;
	// -- Chj: What if the bmp file content is malformed, and bytes_to_read exceeds
	// what CreateDIBSection allocates for us? 
	// Well, DIBSECTION.dsBmih.biSizeImage tells the true buffer size.

	DIBSECTION dibsec_info = {};
	int gotbytes = GetObject(hBitmap, sizeof(dibsec_info), &dibsec_info);
	assert(bytes_to_read==dibsec_info.dsBmih.biSizeImage);

	BOOL succ = ReadFile (hFile, pBits, bytes_to_read, &dwBytesRead, NULL) ;
	assert(succ && dwBytesRead==bytes_to_read);

	free (pbmi) ;
	CloseHandle (hFile) ;

	return hBitmap ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP      s_hBitmap ;
	static int          cxClient, cyClient ;
	static OPENFILENAME ofn ;
	static TCHAR        szFileName [MAX_PATH], szTitleName [MAX_PATH] ;
	static TCHAR        szFilter[] = TEXT ("Bitmap Files (*.BMP)\0*.bmp\0")
		TEXT ("All Files (*.*)\0*.*\0\0") ;
	BITMAP              bitmap ;
	HDC                 hdc, hdcMem ;
	PAINTSTRUCT         ps ;

	switch (message)
	{
	case WM_CREATE:
		ofn.lStructSize       = sizeof (OPENFILENAME) ;
		ofn.hwndOwner         = hwnd ;
		ofn.hInstance         = NULL ;
		ofn.lpstrFilter       = szFilter ;
		ofn.lpstrCustomFilter = NULL ;
		ofn.nMaxCustFilter    = 0 ;
		ofn.nFilterIndex      = 0 ;
		ofn.lpstrFile         = szFileName ;
		ofn.nMaxFile          = MAX_PATH ;
		ofn.lpstrFileTitle    = szTitleName ;
		ofn.nMaxFileTitle     = MAX_PATH ;
		ofn.lpstrInitialDir   = NULL ;
		ofn.lpstrTitle        = NULL ;
		ofn.Flags             = 0 ;
		ofn.nFileOffset       = 0 ;
		ofn.nFileExtension    = 0 ;
		ofn.lpstrDefExt       = TEXT ("bmp") ;
		ofn.lCustData         = 0 ;
		ofn.lpfnHook          = NULL ;
		ofn.lpTemplateName    = NULL ;

		return 0 ;

	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;

	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDM_FILE_OPEN:

			// Show the File Open dialog box

			if (!GetOpenFileName (&ofn))
				return 0 ;

			// If there's an existing bitmap, delete it

			if (s_hBitmap)
			{
				DeleteObject (s_hBitmap) ;
				s_hBitmap = NULL ;
			}
			// Create the DIB Section from the DIB file

			SetCursor (LoadCursor (NULL, IDC_WAIT)) ;
			ShowCursor (TRUE) ;

			s_hBitmap = CreateDibSectionFromDibFile (szFileName) ;

			ShowCursor (FALSE) ;
			SetCursor (LoadCursor (NULL, IDC_ARROW)) ;

			// Invalidate the client area for later update

			InvalidateRect (hwnd, NULL, TRUE) ;

			if (s_hBitmap == NULL)
			{
				MessageBox (hwnd, TEXT ("Cannot load DIB file"), 
					szAppName, MB_OK | MB_ICONEXCLAMATION) ;
			}
			return 0 ;
		}
		break ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		if (s_hBitmap)
		{
			GetObject (s_hBitmap, sizeof (BITMAP), &bitmap) ;

			hdcMem = CreateCompatibleDC (hdc) ;
			SelectObject (hdcMem, s_hBitmap) ;

			BitBlt (hdc,    0, 0, bitmap.bmWidth, bitmap.bmHeight, 
				hdcMem, 0, 0, SRCCOPY) ;

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