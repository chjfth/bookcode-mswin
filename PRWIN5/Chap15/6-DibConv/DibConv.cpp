/*----------------------------------------
   DIBCONV.C -- Converts a DIB to a DDB
                (c) Charles Petzold, 1998
  
  Chj: This program does something similar to SetDIBitsToDevice(),
  but, this program dissect SetDIBitsToDevice's work into two steps,
  first, call CreateDIBitmap() to get a DDB, second, BitBlt the DDB to screen.
  ----------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include "resource.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

TCHAR szAppName[] = TEXT ("DibConv") ;

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

	hwnd = CreateWindow (szAppName, TEXT ("DIB to DDB Conversion (Chj Exp)"),
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

HBITMAP __stdcall CreateDIBitmap_Petzold (
	HDC hdc, CONST BITMAPINFOHEADER * pbmih,
	DWORD fInit, CONST VOID * pBits,
	CONST BITMAPINFO * pbmi, UINT fUsage)
{
	HBITMAP hBitmap ;
	int cx, cy; //, iBitCount ;
	if (pbmih->biSize == sizeof (BITMAPCOREHEADER))
	{
		cx = ((PBITMAPCOREHEADER) pbmih)->bcWidth ;
		cy = ((PBITMAPCOREHEADER) pbmih)->bcHeight ;
		// iBitCount = ((PBITMAPCOREHEADER) pbmih)->bcBitCount ;
	}
	else
	{
		cx = pbmih->biWidth ;
		cy = pbmih->biHeight ;
		// iBitCount = pbmih->biBitCount ; // useless
	} 
	
	if (hdc)
		hBitmap = CreateCompatibleBitmap (hdc, cx, cy) ;
	else
		hBitmap = CreateBitmap (cx, cy, 1, 1, NULL) ;
	
	if (fInit == CBM_INIT)
	{
		HDC hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;
		SetDIBitsToDevice (hdcMem, 0, 0, cx, cy, 0, 0, 0, cy,
			pBits, pbmi, fUsage) ;
		DeleteDC (hdcMem) ;
	}

	return hBitmap;
}


HBITMAP CreateBitmapObjectFromDibFile (HDC hdc, PTSTR szFileName, bool is_petzold_way=false)
{
	BITMAPFILEHEADER * pbmfh ;
	BOOL               bSuccess ;
	DWORD              dwFileSize, dwHighSize, dwBytesRead ;
	HANDLE             hFile ;
	HBITMAP            hBitmap ;

	// Open the file: read access, prohibit write access

	hFile = CreateFile (szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
		return NULL ;

	// Read in the whole file

	dwFileSize = GetFileSize (hFile, &dwHighSize) ;

	if (dwHighSize)
	{
		CloseHandle (hFile) ;
		return NULL ;
	}

	pbmfh = (BITMAPFILEHEADER*) malloc (dwFileSize) ;

	if (!pbmfh)
	{
		CloseHandle (hFile) ;
		return NULL ;
	}

	bSuccess = ReadFile (hFile, pbmfh, dwFileSize, &dwBytesRead, NULL) ;
	CloseHandle (hFile) ;

	// Verify the file

	if (!bSuccess || (dwBytesRead != dwFileSize)         
		|| (pbmfh->bfType != * (WORD *) "BM") 
		|| (pbmfh->bfSize != dwFileSize))
	{
		free (pbmfh) ;
		return NULL ;
	}

	// Create the DDB 

	BITMAPINFOHEADER bmiheader1 = *(BITMAPINFOHEADER *) (pbmfh + 1);
	// -- Chj Experiment: Use a separate BITMAPINFOHEADER to see how it behaves
	// when bmi1 and bmi2 contradict. (We need VS IDE live debugger to try it.)
	// Result: only bmiheader1 .biWidth and .biHeight is used.

	auto fnCreateDIBitmap = is_petzold_way ? CreateDIBitmap_Petzold : CreateDIBitmap;
	// -- `auto` is supported on VS2010 SP1.

	hBitmap = fnCreateDIBitmap (hdc,              
		&bmiheader1, // bmi1
		CBM_INIT,
		(BYTE *) pbmfh + pbmfh->bfOffBits,
		(BITMAPINFO *) (pbmfh + 1), // bmi2
		DIB_RGB_COLORS) ;

	free (pbmfh) ;

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
	{{
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
	{
		int cmdid = LOWORD (wParam);
		switch (cmdid)
		{
		case IDM_FILE_OPEN:
		case IDM_FILE_OPEN_PETZOLD:

			// Show the File Open dialog box

			if (!GetOpenFileName (&ofn))
				return 0 ;

			// If there's an existing DIB, delete it

			if (s_hBitmap)
			{
				DeleteObject (s_hBitmap) ;
				s_hBitmap = NULL ;
			}
			// Create the DDB from the DIB

			SetCursor (LoadCursor (NULL, IDC_WAIT)) ;
			ShowCursor (TRUE) ;

			hdc = GetDC (hwnd) ;
			s_hBitmap = CreateBitmapObjectFromDibFile (hdc, szFileName,
				cmdid==IDM_FILE_OPEN_PETZOLD ? true : false
				) ;
			ReleaseDC (hwnd, hdc) ;

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
	}

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
	}}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
