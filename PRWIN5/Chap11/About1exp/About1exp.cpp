/*------------------------------------------
About1exp.C -- About Box Demo Program No. 1
(c) Charles Petzold, 1998

[2024-12-23] Add Chj experiment code.

In About1exp.rc, the ABOUTBOX DIALOG's style is *only* DS_MODALFRAME,
This makes DlgMan erroneously calculate the window size, whose client area
is *not* high enough to hold the whole dialog height.

Key func:
exp_DbgDialogboxDimension()
------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include "vaDbg.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("About1exp");
	MSG          msg;
	HWND         hwnd;
	WNDCLASS     wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, szAppName);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = szAppName;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("About1 exp"),
		WS_OVERLAPPEDWINDOW,
		360, 200, // CW_USEDEFAULT, CW_USEDEFAULT,
		360, 200, // CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	static bool s_initdone = false;

	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_APP_ABOUT:
			DialogBox(hInstance, TEXT("AboutBox"), hwnd, AboutDlgProc);
			break;
		}
		return 0;

	case WM_WINDOWPOSCHANGED:
		if(!s_initdone)
		{
			s_initdone = true;

			// Purpose: When main UI appears, open the About box automatically.
			// Ref: https://devblogs.microsoft.com/oldnewthing/20060925-02/?p=29603
			SendMessage(hwnd, WM_COMMAND, IDM_APP_ABOUT, 0);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void exp_DbgDialogboxDimension(HWND hdlg)
{
	// Find the dlgbox resource template, and extract its 
	// original dimension value in dialog base units.

	HINSTANCE hInstance = GetWindowInstance(hdlg);

	HRSRC hRes = FindResource(hInstance, _T("ABOUTBOX"), RT_DIALOG);
	assert(hRes);

	HGLOBAL hGlobal = LoadResource(hInstance, hRes);
	assert(hGlobal);

	DLGTEMPLATE* dlgTemplate = (DLGTEMPLATE*)hGlobal;

	// Access the original dialog template dimensions
	int x = dlgTemplate->x;
	int y = dlgTemplate->y;
	int width = dlgTemplate->cx;
	int height = dlgTemplate->cy;

	vaDbgS(_T("DLGTEMPLATE position(dlgunit): LT[%d,%d] WH[%d,%d] RB[%d,%d]"), 
		x,y,   width,height,      x+width,y+height);

	RECT rc = {x, y, x+width, y+height};
	MapDialogRect(hdlg, &rc);
	vaDbgS(_T("  Converted to pixel position: LT[%d,%d] WH[%d,%d] RB[%d,%d]"), 
		rc.left, rc.top, 
		rc.right-rc.left , rc.bottom-rc.top,
		rc.right, rc.bottom);
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:

		exp_DbgDialogboxDimension(hDlg); // Chj

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
