#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

#include "utils.h"


void vaSetDlgItemText(HWND hwnd, UINT ctrlid, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	TCHAR buf[1000] = _T("");
	_vsnprintf_s(buf, ARRAYSIZE(buf), fmt, args);

	SetDlgItemText(hwnd, ctrlid, buf);

	va_end(args);
}

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd)
{
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	buf[0]=_T('['); buf[1]=_T('\0'); buf[bufchars-1] = _T('\0');
	if(ymd) {
		_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%04d-%02d-%02d "), buf, 
			st.wYear, st.wMonth, st.wDay);
	}

	_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%02d:%02d:%02d.%03d]"), buf,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	return buf;
}

RgnExist_et GetRandomRgn_refdc(HDC hDC, HRGN hrgn, int iNum)
{
	// Call GetRandomRgn() and convert the coord inside the region
	// to be relative to hDC's origin.

	POINT Origin;
	BOOL succ = GetDCOrgEx(hDC, &Origin);
	if(!succ)
		return RgnExist_Fail;

	int rr = GetRandomRgn(hDC, hrgn, iNum);
	if(rr==-1)
		return RgnExist_Fail;
	else if(rr==0)
		return RgnExist_No;

	if ( iNum==SYSRGN && ((unsigned) hDC) & 0xFFFF0000 )
	{
		// [CH5.5] It is a 32-bit HDC, so we're running on WinNT.
		// The m_hRegion on NT is expressed in screen coordinate,
		// and we convert it to be client-area coordinate here.
		// Verified on Windows 7 & XP.
		OffsetRgn(hrgn, - Origin.x, - Origin.y);
	}
	
	return RgnExist_Yes;
}

void Draw_16x16_PaletteArray(HDC hdcShow, int x, int y, int width, int height)
{
	// Used by CH13 Palette.exe sample code.

	HDC hMemDC = CreateCompatibleDC(hdcShow);

	BYTE data[80][80]; // 80=16*5

	for (int i=0; i<80; i++)
	{
		for (int j=0; j<80; j++)
		{
			// Chj: every "cell" is 5 pixels
			data[i][j] = (i/5) * 16 + (j/5);

			// Chj: Left-most and top-most pixels are white, to act as gap
			if ( ((i%5)==0) || ((j%5)==0) )
				data[i][j] = 255;
		}
	}

	HBITMAP hBitmap = CreateBitmap(80, 80, 1, 8, data);

	HGDIOBJ hOld = SelectObject(hMemDC, hBitmap);

	StretchBlt(hdcShow, x,y,width,height, hMemDC,0,0,80,80, SRCCOPY);

	SelectObject(hMemDC, hOld); // restore old hBitmap
	DeleteObject(hBitmap);

	DeleteObject(hMemDC);
}