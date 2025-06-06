/*------------------------------------------
About2exp.C -- About Box Demo Program No. 2
(c) Charles Petzold, 1998

Chj: 
* Apply JULayout, so that the dlgbox can be resized. 
  IDC_PAINT will grow with the dlgbox.
------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "vaDbg.h"

#define JULAYOUT_IMPL
#include "JULayout2.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

int iCurrentColor = IDC_BLACK,
iCurrentFigure = IDC_RECT;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("About2exp");
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

	hwnd = CreateWindow(szAppName, TEXT("About2exp"),
		WS_OVERLAPPEDWINDOW,
		400, 200, // CW_USEDEFAULT, CW_USEDEFAULT,
		400, 200, // CW_USEDEFAULT, CW_USEDEFAULT,
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

void PaintWindow(HWND hwnd, int iColor, int iFigure)
{
	// Chj: `hwnd` can be the program's main window, or a control.

	static COLORREF crColor[8] = { RGB(0,   0, 0), RGB(0,   0, 255),
		RGB(0, 255, 0), RGB(0, 255, 255),
		RGB(255,   0, 0), RGB(255,   0, 255),
		RGB(255, 255, 0), RGB(255, 255, 255) };

	HBRUSH          hBrush;
	HDC             hdc;
	RECT            rect;

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	hBrush = CreateSolidBrush(crColor[iColor - IDC_BLACK]);
	hBrush = (HBRUSH)SelectObject(hdc, hBrush);

	if (iFigure == IDC_RECT)
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	else
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);

	DeleteObject(SelectObject(hdc, hBrush));
	ReleaseDC(hwnd, hdc);
}

void PaintTheBlock(HWND hCtrl, int iColor, int iFigure)
{
	InvalidateRect(hCtrl, NULL, TRUE);
	UpdateWindow(hCtrl);
	PaintWindow(hCtrl, iColor, iFigure);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	PAINTSTRUCT      ps;

	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

		// Chj: Let the color selection dialog pops out automatically.
		PostMessage(hwnd, WM_COMMAND, GET_WM_COMMAND_MPS(IDM_APP_ABOUT, 0, 0));
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_APP_ABOUT:
			if (DialogBox(hInstance, TEXT("AboutBox"), hwnd, AboutDlgProc))
				InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}
		break;

	case WM_PAINT:
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);

		PaintWindow(hwnd, iCurrentColor, iCurrentFigure);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

static void MyDlg_EnableJulayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(50,0, 50,0, IDC_LBL_About2exp1);
	jul->AnchorControl(50,0, 50,0, IDC_LBL_About2exp2);

	jul->AnchorControl(0,0, 0,100, IDC_GBX_Color);

	jul->AnchorControl(0,0, 100,100, IDC_PAINT);

	jul->AnchorControl(0,100, 100,100, IDC_GBX_Figure);
	jul->AnchorControls(0,100, 0,100, IDC_RECT, IDC_ELLIPSE, -1);

	jul->AnchorControl(50,100, 50,100, IDOK);
	jul->AnchorControl(50,100, 50,100, IDCANCEL);
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	static HWND hCtrlBlock;
	static int  iColor, iFigure;

	switch (message)
	{
	case WM_INITDIALOG:
		iColor = iCurrentColor;
		iFigure = iCurrentFigure;

		CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, iColor);
		CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, iFigure);

		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);

		MyDlg_EnableJulayout(hDlg);

		SetFocus(GetDlgItem(hDlg, iColor));
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			iCurrentColor = iColor;
			iCurrentFigure = iFigure;
			EndDialog(hDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;

		case IDC_BLACK:
		case IDC_RED:
		case IDC_GREEN:
		case IDC_YELLOW:
		case IDC_BLUE:
		case IDC_MAGENTA:
		case IDC_CYAN:
		case IDC_WHITE:
			iColor = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return TRUE;

		case IDC_RECT:
		case IDC_ELLIPSE:
			iFigure = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return TRUE;
		}
		break;

	case WM_PAINT:

		vaDbgTs(_T("AboutDlgProc.WM_PAINT"));

		PaintTheBlock(hCtrlBlock, iColor, iFigure);
		break;
	}
	return FALSE;
}
