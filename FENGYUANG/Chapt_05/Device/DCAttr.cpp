//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dcattr.cpp		   	                                                 //
//  Description: KDCAttributes class                                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define NOCRYPT
#define _WIN32_WINNT 0x0500
#define WINVER       0x0500

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>

#include "resource.h"

#include "..\..\include\property.h"
#include "..\..\include\listview.h"
#include "..\..\include\JULayout.h"

#include "DCAttr.h"


BOOL KDCAttributes::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
		case WM_INITDIALOG:
			m_hWnd = hWnd;
			{ 
				HWND hOK = GetDlgItem(hWnd, IDOK);

				// [2021-10-05] Chj: A mysterious SetWindowRgn() experiment from original author. 
				// I find some very weird behavior from Windows: 
				// * Even if the OK button gets SetWindowRgn, the non-window-region area(the
				//   button's four corners) can still be painted on screen.
				// * If we drag to enlarge the dlgbox, we see that Elliptic window-region
				//   takes effect, but in a very strange way. See it yourself.
				RECT rect;
				GetWindowRect(hOK, & rect);
				HRGN hRgn = CreateEllipticRgn(0, 0, (rect.right - rect.left), (rect.bottom - rect.top));

				SetWindowRgn(hOK, hRgn, TRUE);
			}

			m_List.FromDlgItem(hWnd, IDC_DCATTRIBUTES);

			return OnInitDialog();

		case WM_COMMAND:
			switch ( wParam )
			{
				case IDOK:
				case IDCANCEL:
				{
					EndDialog(hWnd, TRUE);
					break;
				}

				case MAKELONG(IDC_CREATEDC, BN_SETFOCUS):
					DumpDC(m_hDC);
					break;

				case MAKELONG(IDC_GETDC, BN_SETFOCUS):
					{
						HDC hDC = GetDC(hWnd);

						DumpDC(hDC);
				
						ReleaseDC(hWnd, hDC);
					}
					break;

				case MAKELONG(IDC_GETDCPARENT, BN_SETFOCUS):
					{
						HDC hDC = GetDC(GetParent(hWnd));

						DumpDC(hDC);

						ReleaseDC(GetParent(hWnd), hDC);
					}
					break;

				case MAKELONG(IDC_GETDCOK, BN_SETFOCUS):
					{
						HDC hDC = GetDC(GetDlgItem(hWnd, IDOK));

						DumpDC(hDC);

						ReleaseDC(GetDlgItem(hWnd, IDOK), hDC);
					}
					break;

				case MAKELONG(IDC_MEMDC, BN_SETFOCUS):
					{
						HDC hDC = CreateCompatibleDC(NULL);

						DumpDC(hDC);

						DeleteDC(hDC);
					}
					break;

				case MAKELONG(IDC_MEMDC2, BN_SETFOCUS):
					{
						HDC hDC = CreateCompatibleDC(NULL);

						HBITMAP hBmp = CreateCompatibleBitmap(hDC, 100, 100);
						
						SelectObject(hDC, hBmp);
						DumpDC(hDC);
						
						DeleteDC(hDC);
						DeleteObject(hBmp);
					}
					break;

				case MAKELONG(IDC_WMF, BN_SETFOCUS):
					{
						HDC hDC = CreateMetaFile(_T("c:\\t.wmf"));

						DumpDC(hDC);
						
						DeleteDC(hDC);
					}
					break;

				case MAKELONG(IDC_EMF, BN_SETFOCUS):
					{
						RECT rect = { 100, 100, 200, 300 };

						HDC hDC = CreateEnhMetaFile(GetDC(GetDesktopWindow()), _T("c:\\t.emf"), 
							& rect, _T("Testing"));

						DumpDC(hDC);
						
						DeleteDC(hDC);
					}
					break;

				default:
					return FALSE;
			}
			return TRUE;
	}
		
	return FALSE;
}


void KDCAttributes::Add(LPCTSTR pszAttribute, LPCTSTR pszFormat, ...)
{
    va_list ap;
	TCHAR temp[MAX_PATH];

	va_start(ap, pszFormat);
	
	_vstprintf_s(temp, MAX_PATH, pszFormat, ap);
	
	va_end(ap);

	m_List.AddItem(0, pszAttribute);
	m_List.AddItem(1, temp);
}

void KDCAttributes::Add(BOOL is_succ, LPCTSTR pszAttribute, LPCTSTR pszFormat, ...)
{
	if(!is_succ)
	{
		AddWinError(pszAttribute);
		return;
	}

	va_list ap;
	TCHAR temp[MAX_PATH];

	va_start(ap, pszFormat);

	_vstprintf_s(temp, MAX_PATH, pszFormat, ap);

	va_end(ap);

	m_List.AddItem(0, pszAttribute);
	m_List.AddItem(1, temp);
}

void KDCAttributes::AddWinError(LPCTSTR pszAttribute)
{
	DWORD winerr = GetLastError();

	m_List.AddItem(0, pszAttribute);
	
	TCHAR temp[MAX_PATH]={}, errstr[MAX_PATH]={};

	DWORD retchars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, winerr, 
		0, // LANGID
		errstr, MAX_PATH,
		NULL); // A trailing \r\n has been filled.

	_sntprintf_s(temp, MAX_PATH, _TRUNCATE, _T("Error. WinErr=%d, %s"), winerr, errstr);

	m_List.AddItem(1, temp);
}


BOOL KDCAttributes::OnInitDialog(void)
{
	m_List.AddColumn(0, 160, _T("Attribute"));
	m_List.AddColumn(1, 400, _T("Value"));

	SendDlgItemMessage(m_hWnd, IDC_CREATEDC, BM_SETCHECK, BST_CHECKED, 0);
	
	DumpDC(m_hDC);

	////

	JULayout *jul = JULayout::EnableJULayout(m_hWnd);
	jul->AnchorControl(0,0, 100,100, IDC_DCATTRIBUTES);
	jul->AnchorControl(50,100, 50,100, IDOK);

	return TRUE;
}


void KDCAttributes::DumpDC(HDC hDC)
{
	POINT pnt;
	SIZE  size;
	BOOL succ;

	m_List.DeleteAll();

	if(!hDC)
	{
		Add(_T("N/A"), _T("Because passed in HDC is NULL."));
		return;
	}

	Add(_T("Technology"),  _T("%d"), GetDeviceCaps(hDC, TECHNOLOGY));
	Add(_T("width (HORZRES)"),	   _T("%d"), GetDeviceCaps(hDC, HORZRES));
	Add(_T("height (VERTRES)"),	   _T("%d"), GetDeviceCaps(hDC, VERTRES));

	succ = GetDCOrgEx(hDC, & pnt); 
	if(succ) {
		Add(_T("GetDCOrgEx"), _T("{ %d, %d }"), pnt.x, pnt.y);
	}
	else {
		// For metafile, we'll get ERROR_INVALID_HANDLE(6)
		AddWinError(_T("GetDCOrgEx"));
	}

	TCHAR szTitle[MAX_PATH];

	szTitle[0] = 0;

	GetWindowText(WindowFromDC(hDC), szTitle, MAX_PATH);
	Add(_T("WindowFromDC & GetWindowText"),    _T("0x%X \"%s\""), WindowFromDC(hDC), szTitle);

	Add(_T("GetCurrentObject(OBJ_BITMAP)"),  _T("0x%X"), GetCurrentObject(hDC, OBJ_BITMAP));

	Add(_T("GetGraphicsMode"), _T("%d"), GetGraphicsMode(hDC));
	Add(_T("GetMapMode (Mapping Mode)"),  _T("%d"), GetMapMode(hDC));

	succ = GetViewportExtEx(hDC, & size);
	Add(succ, _T("GetViewportExtEx"), _T("{ %d, %d }"), size.cx, size.cy);
	
	succ = GetViewportOrgEx(hDC, & pnt);
	Add(succ, _T("GetViewportOrgEx"), _T("{ %d, %d }"), pnt.x, pnt.y);

	succ = GetWindowExtEx(hDC, & size);
	Add(succ, _T("GetWindowExtEx"), _T("{ %d, %d }"), size.cx, size.cy);
	
	succ = GetWindowOrgEx(hDC, & pnt);
	Add(succ, _T("GetWindowOrgEx"), _T("{ %d, %d }"), pnt.x, pnt.y);

	XFORM xform;
	succ = GetWorldTransform(hDC, & xform);

	Add(succ, _T("GetWorldTransform"), _T("{ %f, %f, %f, %f, %f, %f }"),
		xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);

	// transformation

	Add(_T("GetBkColor"), _T("0x%X"), GetBkColor(hDC));
	Add(_T("GetTextColor"), _T("0x%X"), GetTextColor(hDC));
	Add(_T("GetCurrentObject(OBJ_PAL) (Palette)"), _T("0x%X"), GetCurrentObject(hDC, OBJ_PAL));

	{
		COLORADJUSTMENT ca;
		succ = GetColorAdjustment(hDC, & ca);
	
		Add(succ, _T("GetColorAdjustment"), _T("{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d }"), 
			ca.caSize, ca.caFlags, ca.caIlluminantIndex,
			ca.caRedGamma, ca.caGreenGamma, ca.caBlueGamma, 
			ca.caReferenceBlack, ca.caReferenceWhite,
			ca.caContrast, ca.caBrightness, ca.caColorfulness, ca.caRedGreenTint);
	}

	Add(_T("GetColorSpace"), _T("0x%X"), GetColorSpace(hDC));
	Add(_T("SetICMMode(ICM_QUERY)"),    _T("%d"),   SetICMMode(hDC, ICM_QUERY));

	{
		TCHAR szProfile[MAX_PATH];
		DWORD dwSize = MAX_PATH;

		szProfile[0] = 0;
		succ = GetICMProfile(hDC, & dwSize, szProfile);

		Add(succ, _T("GetICMProfile"), _T("%s"), szProfile);
	}

	succ = GetCurrentPositionEx(hDC, & pnt);
	Add(succ, _T("GetCurrentPositionEx"), _T("{ %d, %d }"), pnt.x, pnt.y);

	Add(_T("GetROP2"),	_T("%d"),	GetROP2(hDC));
	Add(_T("GetBkMode (Background Mode)"),	_T("%d"),	GetBkMode(hDC));
	Add(_T("GetCurrentObject(OBJ_PEN) (Logical Pen)"),	_T("0x%X"), GetCurrentObject(hDC, OBJ_PEN));
	Add(_T("GetDCPenColor"),    _T("0x%X"), GetDCPenColor(hDC));
	Add(_T("GetArcDirection"),	_T("%d"),	GetArcDirection(hDC));

	FLOAT miter;
	succ = GetMiterLimit(hDC, & miter);
	Add(succ, _T("GetMiterLimit"),		_T("%f"),	miter);
	
	Add(_T("GetCurrentObject(OBJ_BRUSH) (Logical Brush)"),    _T("0x%X"), GetCurrentObject(hDC, OBJ_BRUSH));
	Add(_T("GetDCBrushColor"),   _T("0x%X"), GetDCBrushColor(hDC));

	succ = GetBrushOrgEx(hDC, & pnt);
	Add(succ, _T("GetBrushOrgEx"),     _T("{ %d, %d }"), pnt.x, pnt.y);

	Add(_T("GetPolyFillMode"),   _T("%d"), GetPolyFillMode(hDC));
	Add(_T("GetStretchBltMode"), _T("%d"), GetStretchBltMode(hDC));
	Add(_T("GetCurrentObject(OBJ_FONT) (Logical Font)"),
		_T("0x%X"), GetCurrentObject(hDC, OBJ_FONT));
	
	int iret = GetTextCharacterExtra(hDC);
	if(iret>=0)
		Add(_T("GetTextCharacterExtra (Inter-character spacing)"), _T("%d"), iret);
	else
		AddWinError(_T("GetTextCharacterExtra"));

	DWORD flag = SetMapperFlags(hDC, 0);
	SetMapperFlags(hDC, flag);
	Add(_T("SetMapperFlags (Font Mapper Flags)"),       _T("0x%X"), flag);

	Add(_T("GetTextAlign"),		   _T("0x%X"), GetTextAlign(hDC));

	Add(_T("Text Justification"),  _T("write only"), 0);

	Add(_T("GetLayout"),           _T("%d"), GetLayout(hDC));

	Add(_T("GetPath"),			   _T("%d points"), GetPath(hDC, NULL, NULL, 0));

	RECT rect;
	int typ = GetClipBox(hDC, & rect);

	Add(typ, _T("GetClipBox"),	_T("type %d clip box { %d, %d, %d, %d }"), 
		typ, rect.left, rect.top, rect.right, rect.bottom);

	HRGN hRgn = CreateRectRgn(0, 0, 1, 1);
	DWORD reqbytes = GetRegionData(hRgn, 0, NULL); // always 48 bytes

	iret = GetClipRgn(hDC, hRgn);
	// -- If the function succeeds and there is no clipping region for the given device context, the return value is 0. 
	// -- If the function succeeds and there is a clipping region for the given device context, the return value is 1. 
	//
	if(iret==1)
	{
		reqbytes = GetRegionData(hRgn, 0, NULL);
		Add(_T("GetRegionData"), _T("%d bytes"), reqbytes);
	}
	else if(iret==0)
	{
		Add(_T("GetRegionData"), _T("No region in this DC."));
	}
	else
	{
		assert(iret==-1); // the case for MetaFile
		Add(_T("GetRegionData"), _T("Invalid operation on this DC."));
	}
	
	succ = GetMetaRgn(hDC, hRgn);
	if(succ)
	{
		succ = GetRgnBox(hRgn, & rect);

		Add(succ, _T("GetMetaRgn and GetRgnBox"), 
			_T("GetRegionData()=%d bytes, rgn box { %d, %d, %d, %d }"), 
			GetRegionData(hRgn, 0, NULL), 
			rect.left, rect.top, rect.right, rect.bottom);
	}
	else
	{
		AddWinError(_T("GetMetaRgn"));
	}

	// Chj: MSDN 2008 says GetRandomRgn()'s iNum param must be SYSRGN(4).
	// But: CH7.2 reveals the meaning of 1~3.
	// CLIPRGN(1), METARGN(2), APIRGN(3), SYSRGN(4)
	//
	for (int i=1; i<=5; i++)
	{
		TCHAR strApicall[40];
		_sntprintf_s(strApicall, ARRAYSIZE(strApicall), _TRUNCATE, _T("GetRandomRgn(%d)"), i);

		int rslt = GetRandomRgn(hDC, hRgn, i);

		if ( rslt==1 )
		{
			GetRgnBox(hRgn, & rect);
			
			Add(strApicall,
				_T("GetRegionData()=%d bytes, rgn box { %d, %d, %d, %d }"), 
				GetRegionData(hRgn, 0, NULL), 
				rect.left, rect.top, rect.right, rect.bottom);
		}
		else if ( rslt==0 )
			Add(strApicall, _T("NULL"), 0);
		else
			AddWinError(strApicall);
	}
	DeleteObject(hRgn);

	iret = GetBoundsRect(hDC, & rect, 0);
	Add(iret, _T("GetBoundsRect()"), 
		_T("{ %d, %d, %d, %d }"), 
		rect.left, rect.top, rect.right, rect.bottom);
}
