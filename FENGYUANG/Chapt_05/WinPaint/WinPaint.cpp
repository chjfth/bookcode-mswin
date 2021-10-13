//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : winpaint.cpp         	                                             //
//  Description: Visualize WM_PAINT message                                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//
//  [2021-10-12] Chj: Add code to demonstrate child-window WM_NCCALCSIZE processing.
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <assert.h>
#include <stdio.h>
#include <tchar.h>

#include "..\..\include\win.h"
#include "..\..\include\Toolbar.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\Status.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\LogWindow.h"

#include "Resource.h"

#define NCBORDER_SIZE 20

class KMyCanvas : public KCanvas
{
	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int				m_nRepaint;	
	
	int				m_Red, m_Green, m_Blue;
	HRGN			m_hRegion;
	KLogWindow		m_Log;   // define a KLogWindow instance directly
	DWORD			m_Redraw;

	int m_ncborder; // canvas non-client border pixels(just like a top-level windows's frame)
	bool m_is_nccenter; // in WM_NCCALCSIZE, whether center client-area to new window pos.

public:

	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	KMyCanvas()
	{
		m_nRepaint = 0;
		m_hRegion  = CreateRectRgn(0, 0, 1, 1);
		
		m_Red	   = 0x4F;
		m_Green    = 0x8F;
		m_Blue     = 0xCF;
		m_Redraw   = 0;

		m_ncborder = 0; // no border, this is most common for a child window
		m_is_nccenter = false;
	}
};


BOOL KMyCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT cmdid = GET_WM_COMMAND_ID(wParam, lParam);

	switch ( cmdid )
	{
		case IDM_VIEW_HREDRAW:
		case IDM_VIEW_VREDRAW:
		case IDM_CENTER_CLIENT_AREA:
		case IDM_CANVAS_ADD_NONCLIENT_BORDER:
		{
			// Check for conflict
			if( cmdid==IDM_CENTER_CLIENT_AREA && (m_Redraw & (WVR_HREDRAW|WVR_VREDRAW))!=0 )
			{
				MessageBox(m_hWnd, _T("With WVR_HREDRAW or WVR_VREDRAW turn on, we cannot enable center-client feature."), _T("Error"), MB_OK);
				return 0;
			}
			if( (cmdid==IDM_VIEW_HREDRAW || cmdid==IDM_VIEW_VREDRAW) && m_is_nccenter )
			{
				MessageBox(m_hWnd, _T("With center-client feature on, we cannot enable WVR_VREDRAW or WVR_VREDRAW."), _T("Error"), MB_OK);
				return 0;
			}

			HMENU hMenu = GetMenu(GetParent(m_hWnd));

			MENUITEMINFO mii = { sizeof(mii) };
			mii.fMask  = MIIM_STATE;
			
			// toggle menu-item check-state.
			if ( GetMenuState(hMenu, cmdid, MF_BYCOMMAND) & MF_CHECKED )
				mii.fState = MF_UNCHECKED;
			else
				mii.fState = MF_CHECKED;
				
			SetMenuItemInfo(hMenu, cmdid, FALSE, &mii);
				
			if ( cmdid==IDM_VIEW_HREDRAW )
				m_Redraw ^= WVR_HREDRAW; // toggle WVR_HREDRAW bit
			else if(cmdid==IDM_VIEW_VREDRAW)
				m_Redraw ^= WVR_VREDRAW; // toggle WVR_VREDRAW bit
			else if(cmdid==IDM_CENTER_CLIENT_AREA)
				m_is_nccenter = !m_is_nccenter;
			else if(cmdid==IDM_CANVAS_ADD_NONCLIENT_BORDER)
			{
				m_ncborder = m_ncborder==0 ? NCBORDER_SIZE : 0;
		
				SetWindowPos(m_hWnd, NULL, 0,0,0,0, 
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
				// -- MSDN: SWP_FRAMECHANGED causes the system to send WM_NCCALCSIZE
				// to our m_hWnd, which is what we need to refresh the display.
				InvalidateRect(m_hWnd, NULL, TRUE);
			}

			return TRUE;
		}

		case IDM_FILE_INVALIDATE_RGN:
		{
			// Invalidate a circle region, and we'll see that GetRegionData()
			// represents this as a lot of scan-lines(thin rectangles).

			HRGN hrgnCircle = CreateEllipticRgn(0,0, 100,100);
			InvalidateRgn(m_hWnd, hrgnCircle, FALSE);
			DeleteObject(hrgnCircle);
			return 0;
		}

		case IDM_FILE_EXIT:
			DestroyWindow(GetParent(m_hWnd));
			return TRUE;
	}

	return FALSE;	// not processed
}

static POINT GetRectCenter(RECT &r)
{
	POINT pt = {(r.left+r.right)/2, (r.top+r.bottom)/2};
	return pt;
}

LRESULT KMyCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = 0;

	switch( uMsg )
	{
		case WM_CREATE:
		{
			m_hWnd = hWnd;
			m_Log.Create(m_hInst, _T("WinPaint"), LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH)));
			m_Log.Log(_T("WM_CREATE\r\n"));

			// Set a dynamic menu-item text.
			//
			HMENU hMenu = GetMenu(GetParent(m_hWnd));
			TCHAR mitext[100];
			_sntprintf_s(mitext, ARRAYSIZE(mitext), _TRUNCATE, _T("Canvas add non-client border (%d px)"), NCBORDER_SIZE);
			BOOL succ = ModifyMenu(hMenu, IDM_CANVAS_ADD_NONCLIENT_BORDER, 
				MF_BYCOMMAND, IDM_CANVAS_ADD_NONCLIENT_BORDER, mitext);
			assert(succ);

			return 0;
		}

		case WM_NCCALCSIZE:
		{
			m_Log.Log(_T("WM_NCCALCSIZE\r\n"));

			if(wParam==0)
			{
				// This is first WM_NCCALCSIZE callback, no window-moving yet.
				// Here, we determine client-area position for current HWND.

				RECT ircClient = *(RECT*)lParam;  // this is *screen* coord
				RECT &orcClient = *(RECT*)lParam; // also *screen* coord

				InflateRect(&orcClient, -m_ncborder, -m_ncborder); 
				lr = 0; // placeholder to set breakpoint
			}
			else
			{
				NCCALCSIZE_PARAMS *pNccs = (NCCALCSIZE_PARAMS*)lParam;

				//  Give names to these things
				RECT ircWndPosNow = pNccs->rgrc[0]; // input naming, rela-to hWnd's parent
				RECT ircWndPosWas = pNccs->rgrc[1]; // input naming, rela-to hWnd's parent
				RECT ircClientAreaWas = pNccs->rgrc[2]; // input naming, rela-to hWnd's parent
				//
				RECT &orcClientNew = pNccs->rgrc[0]; // output naming, rela-to hWnd's parent
				RECT &orcValidDst  = pNccs->rgrc[1]; // output naming, rela-to hWnd's parent
				RECT &orcValidSrc  = pNccs->rgrc[2]; // output naming, rela-to hWnd's parent

				int oldcliw = ircClientAreaWas.right - ircClientAreaWas.left; // old client-area width
				int oldclih = ircClientAreaWas.bottom - ircClientAreaWas.top; // old client-area height

				int newcliw = ircWndPosNow.right - ircWndPosNow.left - m_ncborder*2; // new client-area width
				int newclih = ircWndPosNow.bottom - ircWndPosNow.top - m_ncborder*2; // new client-area height

				if(m_ncborder>0)
				{
					// Now we change rgrc[0] to be the coordinate of hWnd's client area location.
					//
					SetRect(&orcClientNew, 
						ircWndPosNow.left + m_ncborder, ircWndPosNow.top + m_ncborder, 
						ircWndPosNow.right - m_ncborder, ircWndPosNow.bottom - m_ncborder);
				}

				if(m_is_nccenter)
				{
					POINT oldcenter = GetRectCenter(ircClientAreaWas);
					POINT newcenter = GetRectCenter(ircWndPosNow);

					int copycliw = min(oldcliw, newcliw);
					int copyclih = min(oldclih, newclih);

					// Tell Windows to move old client area content to *center* of new client area.
					//
					SetRect(&orcValidSrc,
						oldcenter.x - copycliw/2, oldcenter.y - copyclih/2, 
						oldcenter.x + copycliw/2, oldcenter.y + copyclih/2
						);
					//
					SetRect(&orcValidDst, 
						newcenter.x - copycliw/2, newcenter.y - copyclih/2, 
						newcenter.x + copycliw/2, newcenter.y + copyclih/2
						);
					lr |= WVR_VALIDRECTS;
					// -- MSDN: This flag cannot be combined with any other flags. 
				}
			} // wParam==1

			if ( wParam && (lr & WVR_VALIDRECTS)==0 )
			{
				lr &= ~ (WVR_HREDRAW | WVR_VREDRAW);
				lr |= m_Redraw;
			}

			m_Log.Log("WM_NCCALCSIZE returns 0x%x", lr);

			break;
		}
		case WM_NCPAINT:
		{
			m_Log.Log("WM_NCPAINT HRGN %0x\r\n", (HRGN) wParam);
			
			HRGN hUpdateRgn = (HRGN)wParam;
			(void)hUpdateRgn;

			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
			
			m_Log.Log("WN_NCPAINT.Def returns\r\n");
			break;
		}

		case WM_ERASEBKGND:
		{
			m_Log.Log("WM_ERASEBKGND HDC %0x\r\n", (HDC) wParam);
			
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
			
			m_Log.Log("WM_ERASEBKGND.Def returns\r\n");
			break;
		}

		case WM_SIZE:
			m_Log.Log("WM_SIZE type %d, width %d, height %d\r\n", wParam, LOWORD(lParam), HIWORD(lParam));
			
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
			
			m_Log.Log("WM_SIZE.Def returns\r\n");
			break;

		case WM_PAINT:
		{
			m_nRepaint ++;
			switch ( m_nRepaint % 3 )
			{
			case 0: m_Red  = (m_Red   + 0x52) & 0xFF; break;
			case 1: m_Green= (m_Green + 0x52) & 0xFF; break;
			case 2: m_Blue = (m_Blue  + 0x52) & 0xFF; break;
			}

			int tmp = m_Red; m_Red=m_Green; m_Green=m_Blue; m_Blue=tmp;
			// -- Chj: to make adjacent colors distinct


			PAINTSTRUCT ps; 

			m_Log.Log("WM_PAINT\r\n");
				
			m_Log.Log("BeginPaint\r\n");
			HDC hDC = BeginPaint(m_hWnd, &ps);
			DWORD objtype = GetObjectType(hDC);
			m_Log.Log("BeginPaint returns HDC 0x%08x (objtype=%d)\r\n", hDC, objtype);

			OnDraw(hDC, &ps.rcPaint);

			m_Log.Log("EndPaint\r\n");
			EndPaint(m_hWnd, &ps);
			m_Log.Log("EndPaint returns GetObjectType(0x%08x)=%d\r\n", hDC, GetObjectType(hDC));
				
			m_Log.Log("WM_PAINT returns\r\n");
			return 0;
		}

		default:
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return lr;
}


void KMyCanvas::OnDraw(HDC hDC, const RECT * rcPaint)
{
	RECT rect;

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

	TCHAR mess[64];

	wsprintf(mess, _T("HDC 0x%X, Org(%d, %d)"), hDC, Origin.x, Origin.y); 
	if ( m_pStatus )
		m_pStatus->SetText(pane_1, mess);

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

			wsprintf(mess, "WM_PAINT #%d, rect %d", m_nRepaint, i+1);
			::TextOut(hDC, x, y - lineheight, mess, _tcslen(mess));

			wsprintf(mess, "(%d, %d, %d, %d)", pRect[i].left, pRect[i].top, pRect[i].right, pRect[i].bottom);
			::TextOut(hDC, x, y, mess, _tcslen(mess));

			// >>> Chj: draw dotted border around each rect, for better teaching purpose
			HBRUSH oldbrush = SelectBrush(hDC, GetStockObject(HOLLOW_BRUSH));
			HPEN oldpen = SelectPen(hDC, CreatePen(PS_DOT, 1, RGB(m_Red, m_Green, m_Blue)));
			Rectangle(hDC, pRect[i].left, pRect[i].top, pRect[i].right, pRect[i].bottom);
			DeleteObject(SelectPen(hDC, oldpen));
			SelectBrush(hDC, oldbrush);
			// <<<

			m_Log.Log(_T("Rgn-rect#%d (%d,%d, %d,%d)"), i+1,
				pRect[i].left, pRect[i].top, pRect[i].right, pRect[i].bottom);
		}

		delete [] (char *) pRegion;
	}

	wsprintf(mess, _T("WM_PAINT message #%d: %d rects in sysrgn"), m_nRepaint, rectcount);
	if ( m_pStatus )
		m_pStatus->SetText(pane_2, mess);


	HBRUSH hBrush = CreateSolidBrush(RGB(m_Red, m_Green, m_Blue));

	FrameRgn(hDC, m_hRegion, hBrush, 4, 4);
	FrameRgn(hDC, m_hRegion, (HBRUSH) GetStockObject(WHITE_BRUSH), 1, 1);

	DeleteObject(hBrush);
}


class KMyFrame : public KFrame
{
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);
		wc.hIcon  = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:
	KMyFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
			KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus) :
		KFrame(hInstance, pButtons, nCount, pToolbar, pCanvas, pStatus)
	{
	}

};

const TBBUTTON tbButtons[] =
{
	{ STD_FILENEW,	 IDM_FILE_EXIT,   TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_EXIT,   0 },
	{ STD_CUT, IDM_FILE_INVALIDATE_RGN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_DO_INVALIDATE_RGN, 0 }
	// -- BUG: iBitmap value STD_xxx is inconsistent with actual display. (to investigate)
};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KToolbar      toolbar;
	KMyCanvas     canvas;
	KStatusWindow status;

	KMyFrame frame(hInst, tbButtons, 2, &toolbar, &canvas, &status);

	frame.CreateEx(0, _T("ClassName"), _T("WinPaint (chjmod)"),
		WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, CW_USEDEFAULT, 
		400+16, 200+80+37, // [Win7] 16: left+right window frame; 37: toolbar height 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
