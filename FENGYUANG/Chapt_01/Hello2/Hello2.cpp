//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : hello2.cpp						                                     //
//  Description: Hello World Demo 2, full screen display, Chapter 1                 //
//  Version    : 1.00.001, July 26, 2000                                             //
//-----------------------------------------------------------------------------------//

/* Chj memo:

Command line compiling:

    cl hello2.cpp /UNICODE /Od /Zi /GS- /link /subsystem:windows kernel32.lib user32.lib gdi32.lib

On VS2015+, append extra lib: libucrt.lib

*/

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <assert.h>

void CenterText(HDC hDC, int x, int y, LPCTSTR szFace, LPCTSTR szMessage, int point)
{
    HFONT hFont = CreateFont(- point * GetDeviceCaps(hDC, LOGPIXELSY) / 72,
							 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE, 
							 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
							 PROOF_QUALITY, VARIABLE_PITCH, szFace);

	int oldlimit = GdiSetBatchLimit(1); // 1=No Batch
	// -- Chj: With no-batch enabled, We will see that TextOut() causes the big blue text
	// to appear on screen immediately as soon as TextOut is called. Otherwise, the blue
	// text appearance is delayed until DeleteObject(hFont) is called.
	// 2021.11.10, verified on Windows XP. Remote debugging exhibits this clearly.

    HGDIOBJ hOld = SelectObject(hDC, hFont);

    SetTextAlign(hDC, TA_CENTER | TA_BASELINE);

    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 0, 0xFF));
    TextOut(hDC, x, y, szMessage, _tcslen(szMessage));

    SelectObject(hDC, hOld);
    DeleteObject(hFont);

	GdiSetBatchLimit(oldlimit);
}

const TCHAR szMessage[] = _T("Hello, World");
const TCHAR szFace[]    = _T("Times New Roman");

#pragma comment(linker, "-merge:.rdata=.text")

#if _MSC_VER<1600
// [2021-11-10] Chj: On VC2010(_MSC_VER==1600), setting -align:512 causes this
// generated Win32 fail to launch, reported as "not a valid Win32 application" on run.
#pragma comment(linker, "-align:512")
#endif

extern "C" void WinMainCRTStartup()
{
    HDC hDC = GetDC(NULL);

    CenterText(hDC, GetSystemMetrics(SM_CXSCREEN) / 2,
		    GetSystemMetrics(SM_CYSCREEN) / 2,
		    szFace, szMessage, 72);
    
    ReleaseDC(NULL, hDC);
    ExitProcess(0);
}
