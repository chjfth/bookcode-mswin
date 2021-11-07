#ifndef __utils_h_
#define __utils_h_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

inline int RectW(const RECT &r){ return r.right - r.left; }
inline int RectH(const RECT &r){ return r.bottom - r.top; }

inline bool IsDisplayMode256color()
{
	HDC hdc = GetDC(NULL);
	int ret = GetDeviceCaps(hdc, SIZEPALETTE);
	ReleaseDC(NULL, hdc);
	return ret==256 ? true: false;
}


void vaSetDlgItemText(HWND hwnd, UINT ctrlid, const TCHAR *fmt, ...);

void vaDbg(const TCHAR *fmt, ...);

void vaMsgBox(HWND hwndParent, const TCHAR *title, const TCHAR *fmt, ...);

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);


enum RgnShape_et {
	RgnNone = ERROR,
	RgnEmpty = NULLREGION,
	RgnOneRect = SIMPLEREGION,
	RgnMultiRect = COMPLEXREGION,
};

enum RandomRgnIndex {
	Rgi_CLIPRGN = 1,
	Rgi_METARGN = 2,
	Rgi_APIRGN = 3,
	Rgi_SYSRGN = SYSRGN,
};

enum RgnExist_et {
	RgnExist_Yes = 1,
	RgnExist_No = 0,
	RgnExist_Fail = -1,
}; // As return value for GetRandomRgn, GetClipRgn

inline const TCHAR *RgnShapeStr(RgnShape_et shape)
{
	switch(shape)
	{
	case RgnEmpty:     return _T("EmptyRgn");
	case RgnOneRect:   return _T("SimplRgn");
	case RgnMultiRect: return _T("CmplxRgn");
	}
	return _T("NoneRgn");
}


RgnExist_et GetRandomRgn_refdc(HDC hDC, HRGN hrgn, int iNum=SYSRGN);

void Draw_16x16_PaletteArray(HDC hdcShow, int x, int y, int width, int height);

#define WEBCOLOR_HOP 51 // or 0x33

#endif
