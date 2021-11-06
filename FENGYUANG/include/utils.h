#ifndef __utils_h_
#define __utils_h_

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

inline int RectW(const RECT &r){ return r.right - r.left; }
inline int RectH(const RECT &r){ return r.bottom - r.top; }


void vaSetDlgItemText(HWND hwnd, UINT ctrlid, const TCHAR *fmt, ...);

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


#endif
