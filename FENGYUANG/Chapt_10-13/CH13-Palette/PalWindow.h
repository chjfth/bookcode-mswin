//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : palwindow.h					                                     //
//  Description: Palette window                                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include <windows.h>
#include <windowsx.h>
#include "..\..\include\utils.h"

void AnalyzePalette(PALETTEENTRY entry[], int no, TCHAR mess[], int bufsize);
BOOL Switch8bpp(void);

class KPaletteWnd : public KWindow
{
	HDC			 m_hDC;
	TCHAR		 m_name[MAX_PATH];
	PALETTEENTRY m_Entry[256];
	int			 m_nEntry;
	int			 m_nGeneration;

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch ( uMsg )
		{
			case WM_CREATE:
			{
				m_hWnd   = hWnd;
				m_hDC    = GetDC(m_hWnd);
				m_nEntry = GetSystemPaletteEntries(m_hDC, 0, 256, m_Entry);
				m_nGeneration = 0;
				_tcscpy_s(m_name, ARRAYSIZE(m_name), "Original");
				return 0;
			}

			case WM_MOUSEMOVE:
			{
				int mousex = GET_X_LPARAM(lParam);
				int mousey = GET_Y_LPARAM(lParam);
				int cellx = (mousex-10)/16;
				int celly = (mousey-45)/16;
				int palslot = -1;
				TCHAR str_palslot[10] = {};
				if(cellx>=0 && cellx<16 && celly>=0 && celly<16)
				{
					palslot = celly*16 + cellx;
					_sntprintf_s(str_palslot, ARRAYSIZE(str_palslot), _T("#%02X "), palslot);
				}

				COLORREF cr = GetPixel(m_hDC, LOWORD(lParam), HIWORD(lParam));
				TCHAR temp[32];
				_sntprintf_s(temp, ARRAYSIZE(temp), 
					"%sRGB(%02X, %02X, %02X)        ", str_palslot,
					GetRValue(cr), GetGValue(cr), GetBValue(cr));
				TextOut(m_hDC, 10, 310, temp, _tcslen(temp));
				return 0;
			}

			case WM_PAINT:
			{
				PAINTSTRUCT ps;

				HDC hDC = BeginPaint(hWnd, & ps);

				TextOut(hDC, 10, 10, m_name, _tcslen(m_name));

				Draw_16x16_PaletteArray(hDC, 10, 45, 256, 256);

				TCHAR temp[64];
				AnalyzePalette(m_Entry, m_nEntry, temp, ARRAYSIZE(temp));
				TextOut(hDC, 10, 25, temp, _tcslen(temp));

				wsprintf(temp, "System Palette [%d]", m_nGeneration);
				SetWindowText(hWnd, temp);
				
				EndPaint(hWnd, & ps);
				return 0;
			}

			case WM_PALETTECHANGED:
			{
				PALETTEENTRY m_New[256];

				GetSystemPaletteEntries(m_hDC, 0, 256, m_New);

				int diff = 0;
				for (int i=0; i<m_nEntry; i++)
				{
					if ( memcmp(& m_New[i], & m_Entry[i], sizeof(PALETTEENTRY)) )
						diff ++;
				}

				_sntprintf_s(m_name, ARRAYSIZE(m_name), _T("%d entries changed by HWND(0x%08x) "), diff, wParam);

				int pfxlen = _tcslen(m_name);
				GetWindowModuleFileName((HWND) wParam, m_name+pfxlen, ARRAYSIZE(m_name)-pfxlen);

				memcpy(m_Entry, m_New, sizeof(m_New));
				m_nGeneration ++;

				InvalidateRect(hWnd, NULL, TRUE);
				return 0;
			}

			case WM_NCDESTROY:
				ReleaseDC(m_hWnd, m_hDC);
				return 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

public:

	void CreatePaletteWindow(HINSTANCE hInst)
	{
		if ( ! Switch8bpp() )
			return;

		CreateEx(0, _T("SysPal"), _T("System Palette"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			CW_USEDEFAULT, CW_USEDEFAULT, 290, 370, NULL, NULL, hInst);
			ShowWindow(SW_NORMAL);
			UpdateWindow();
	}
};

