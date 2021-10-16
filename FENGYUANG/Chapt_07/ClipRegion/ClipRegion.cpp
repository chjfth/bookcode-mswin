//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : clipregion.cpp					                                     //
//  Description: Clip/meta region demo program, Chapter 7                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>

#include "..\..\include\win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\Status.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\LogWindow.h"
#include "..\..\include\utils.h"

#include "Resource.h"

#ifndef SYSRGN
#define SYSRGN 4
__declspec(dllimport) extern "C" BOOL WINAPI GetRandomRgn(HDC hDC, HRGN hRgn, int which);
#endif


enum Rgntype_et {
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


class KMyCanvas : public KCanvas
{
	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int				m_nRepaint;	
	
	int				m_Red, m_Green, m_Blue;
	HRGN			m_hRegion;
	KLogWindow		m_Log;
	DWORD			m_Redraw;
	bool            m_bValid[SYSRGN+1]; // [5]
	HRGN            m_hRandomRgn[SYSRGN+1];
	int				m_test;

public:

	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	void DumpRegions(HDC hDC);
	void DrawRegions(HDC hDC);
	void TestClipMeta(HDC hDC, const RECT & rect);

	KMyCanvas()
	{
		m_nRepaint = 0;
		m_hRegion  = CreateRectRgn(0, 0, 1, 1);
		
		m_Red	   = 0x4F;
		m_Green    = 0x8F;
		m_Blue     = 0xCF;
		m_Redraw   = 0;
		m_test     = IDM_TEST_DEFAULT;

		for (int i=1; i<=4; i++)
		{
			m_hRandomRgn[i] = CreateRectRgn(0, 0, 1, 1);
			m_bValid[i]     = false;
		}
	}
};


BOOL KMyCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT cmdid = LOWORD(wParam);

	switch ( cmdid )
	{
		case IDM_VIEW_HREDRAW:
		case IDM_VIEW_VREDRAW:
			{
				HMENU hMenu = GetMenu(GetParent(m_hWnd));

				MENUITEMINFO mii;
				
				memset(& mii, 0, sizeof(mii));
				mii.cbSize = sizeof(mii);
				mii.fMask  = MIIM_STATE;
				
				if ( GetMenuState(hMenu, cmdid, MF_BYCOMMAND) & MF_CHECKED )
					mii.fState = MF_UNCHECKED;
				else
					mii.fState = MF_CHECKED;
				
				SetMenuItemInfo(hMenu, cmdid, FALSE, & mii);
				
				if ( cmdid==IDM_VIEW_HREDRAW )
					m_Redraw ^= WVR_HREDRAW;
				else
					m_Redraw ^= WVR_VREDRAW;
			}
			return TRUE;

		case IDM_TEST_DEFAULT:
		case IDM_TEST_SETCLIP:
		case IDM_TEST_SETMETA:
		case IDM_TEST_SETMETACLIP:
			m_test = cmdid;
			InvalidateRect(m_hWnd, NULL, TRUE);
			::UpdateWindow(m_hWnd);
			break;

		case IDM_FILE_EXIT:
			DestroyWindow(GetParent(m_hWnd));
			return TRUE;
	}

	return FALSE;	// not processed
}

 
LRESULT KMyCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr;

	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd = hWnd;
			m_Log.Create(m_hInst, _T("ClipRegion"), LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH)));
			return 0;

		case WM_NCCALCSIZE:
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);

			if ( wParam )
			{
				lr &= ~ (WVR_HREDRAW | WVR_VREDRAW);
				lr |= m_Redraw;
			}
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps; 

				HDC hDC = BeginPaint(m_hWnd, &ps);

			//	HRGN hRgn1 = CreateRectRgn(0, 0, 100, 100);
			//	HRGN hRgn2 = CreateRectRgn(50, 50, 300, 300);
			//	int value = CombineRgn(hRgn1, hRgn1, NULL, RGN_AND);
			//	DeleteObject(hRgn1);
			//	DeleteObject(hRgn2);
				
				OnDraw(hDC, &ps.rcPaint);

				EndPaint(m_hWnd, &ps);

				hDC = GetDC(m_hWnd);
				
				DrawRegions(hDC);

				ReleaseDC(m_hWnd, hDC);
			}
			return 0;

		default:
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return lr;
}

static const TCHAR *rgi2text(int iNum)
{
	if(iNum==1) return _T("CLIPRGN");
	else if(iNum==2) return _T("METARGN");
	else if(iNum==3) return _T("APIRGN");
	else if(iNum==4) return _T("SYSRGN");
	else return _T("Bad num");
}

void KMyCanvas::DumpRegions(HDC hDC)
{
	TCHAR tmpstr[200] = {};
	for (int i=1; i<=4; i++)
	{
		m_bValid[i] = false;
		int rslt = GetRandomRgn(hDC, m_hRandomRgn[i], i);

		switch ( rslt )
		{
			case 1:
				_sntprintf_s(tmpstr, ARRAYSIZE(tmpstr), _T("RandomRgn(%d=%s)"), i, rgi2text(i));
				m_bValid[i] = true;
				m_Log.DumpRegion(tmpstr, m_hRandomRgn[i], false);
				break;

			case -1:
				m_Log.Log("RandomRgn(%d=%s) Error\r\n", i, rgi2text(i));
				break;

			case 0:
				m_Log.Log("RandomRgn(%d=%s) no region\r\n", i, rgi2text(i));
				break;

			default:
				m_Log.Log("Unexpected\r\n");
		}
	}
}


void KMyCanvas::DrawRegions(HDC hDC)
{
	// Chj: This is better named: MarkRandomRegionsLocation()

	HBRUSH hBrush;

	SetBkMode(hDC, TRANSPARENT);

	if ( m_bValid[Rgi_APIRGN] )
	{
		// Chj: we'll draw HS_DIAGCROSS before HS_VERTICAL/HS_HORIZONTAL for better visual effect
		hBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(0xFF, 0, 0));
		FillRgn(hDC, m_hRandomRgn[3], hBrush);
		DeleteObject(hBrush);
	}

	if ( m_bValid[Rgi_CLIPRGN] )
	{
		hBrush = CreateHatchBrush(HS_VERTICAL, RGB(0xFF, 0xE2, 0x66));
		FillRgn(hDC, m_hRandomRgn[1], hBrush);
		DeleteObject(hBrush);
	}

	if ( m_bValid[Rgi_METARGN] )
	{
		hBrush = CreateHatchBrush(HS_HORIZONTAL, RGB(0, 0xFF, 0xFF));
		FillRgn(hDC, m_hRandomRgn[2], hBrush);
		DeleteObject(hBrush);
	}
}


void KMyCanvas::OnDraw(HDC hDC, const RECT * rcPaint)
{
	RECT rect;
	TCHAR mess[64];
	
	GetClientRect(m_hWnd, & rect);
	
	GetRandomRgn(hDC, m_hRegion, SYSRGN);

	POINT Origin;
	GetDCOrgEx(hDC, & Origin);

	if ( ((unsigned) hDC) & 0xFFFF0000 )
	{
		// [CH5.5] It is a 32-bit HDC, so we're running on WinNT.
		// The m_hRegion on NT is expressed in screen coordinate,
		// and we convert it to be client-area coordinate here.
		// Verified, it is a must on Windows 7.
		OffsetRgn(m_hRegion, - Origin.x, - Origin.y);
	}

	m_nRepaint ++;

	switch ( m_nRepaint % 3 )
	{
		case 0: m_Red  = (m_Red   + 0x31) & 0xFF; break;
		case 1: m_Green= (m_Green + 0x31) & 0xFF; break;
		case 2: m_Blue = (m_Blue  + 0x31) & 0xFF; break;
	}

	SetTextAlign(hDC, TA_TOP | TA_CENTER);

	int size = GetRegionData(m_hRegion, 0, NULL);
	int rectcount = 0;

	if ( size )
	{
		RGNDATA * pRegion = (RGNDATA *) new char[size];
		GetRegionData(m_hRegion, size, pRegion);

		const RECT * pRect = (const RECT *) & pRegion->Buffer;
		rectcount = pRegion->rdh.nCount;

		TEXTMETRIC tm;
		GetTextMetrics(hDC, & tm);
		int lineheight = tm.tmHeight + tm.tmExternalLeading; 

		for (int i=0; i<rectcount; i++)
		{
			int x = (pRect[i].left + pRect[i].right)/2;
			int y = (pRect[i].top + pRect[i].bottom)/2;

			wsprintf(mess, "WM_PAINT #%d, rects=%d", m_nRepaint, i+1);
			::TextOut(hDC, x, y - lineheight, mess, _tcslen(mess));

			wsprintf(mess, "(%d, %d, %d, %d)", pRect[i].left, pRect[i].top, pRect[i].right, pRect[i].bottom);
			::TextOut(hDC, x, y, mess, _tcslen(mess));

			m_Log.Log(_T("SYSRGN-rect#%d (%d,%d, %d,%d) [%dx%d]"), i+1,
				pRect[i].left, pRect[i].top, pRect[i].right, pRect[i].bottom,
				RectW(pRect[i]), RectH(pRect[i]));
		}

		delete [] (char *) pRegion;
	}

	HBRUSH hBrush = CreateSolidBrush(RGB(m_Red, m_Green, m_Blue));

	FrameRgn(hDC, m_hRegion, hBrush, 4, 4);
	FrameRgn(hDC, m_hRegion, (HBRUSH) GetStockObject(WHITE_BRUSH), 1, 1);

	DeleteObject(hBrush);

	TestClipMeta(hDC, rect);
}


void KMyCanvas::TestClipMeta(HDC hDC, const RECT & rect)
{
	/////////////////////////////////
	// Play with clip and meta region
	HRGN hRgn = NULL;

	switch ( m_test )
	{
		case IDM_TEST_DEFAULT:
			break;

		case IDM_TEST_SETCLIP:
			hRgn = CreateEllipticRgn(0, 0, rect.right*3/4, rect.bottom);
			SelectClipRgn(hDC, hRgn);
			DeleteObject(hRgn);
			break;

		case IDM_TEST_SETMETA:
			hRgn = CreateEllipticRgn(0, 0, rect.right, rect.bottom*3/4);
			SelectClipRgn(hDC, hRgn);
			SetMetaRgn(hDC);
			break;

		case IDM_TEST_SETMETACLIP:
			hRgn = CreateEllipticRgn(0, 0, rect.right, rect.bottom*3/4);
			SelectClipRgn(hDC, hRgn);
			SetMetaRgn(hDC);
			DeleteObject(hRgn);
			hRgn = CreateEllipticRgn(0, 0, rect.right*3/4, rect.bottom);
			SelectClipRgn(hDC, hRgn);
			break;
	}
	
	if(hRgn)
		DeleteObject(hRgn);

	// with meta and clip region selected, only the
	// intersection of system region, meta region, clip region can be painted
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0xFF));
	FillRect(hDC, & rect, hBrush);
	DeleteObject(hBrush);

	DumpRegions(hDC);

	char mess[64] = {};
	//
	strcat_s(mess, ARRAYSIZE(mess), "Clip: ");
	if ( m_bValid[1] ) 
		strcat_s(mess, ARRAYSIZE(mess), "Y"); 
	else 
		strcat_s(mess, ARRAYSIZE(mess), "N");
	//	
	strcat_s(mess, ARRAYSIZE(mess), ",  Meta: ");
	if ( m_bValid[2] ) 
		strcat_s(mess, ARRAYSIZE(mess), "Y"); 
	else 
		strcat_s(mess, ARRAYSIZE(mess), "N");
	//
	strcat_s(mess, ARRAYSIZE(mess), ", API: ");
	if ( m_bValid[3] ) 
		strcat_s(mess, ARRAYSIZE(mess), "Y"); 
	else 
		strcat_s(mess, ARRAYSIZE(mess), "N");
	//
	m_pStatus->SetText(0, mess);
}

class KLogoFrame : public KFrame
{
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:
	KLogoFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus) :
			KFrame(hInstance, pButtons, nCount, pToolbar, pCanvas, pStatus)
	{
	}

};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KMyCanvas     canvas;
	KStatusWindow status;

	KLogoFrame frame(hInst, NULL, 0, NULL, & canvas, & status);

	frame.CreateEx(0, _T("ClipRegion"), _T("Regions in a Device Context: System/Meta/Clip Regions"),
		WS_OVERLAPPEDWINDOW,
	    200, 200, 
		600, 400, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
