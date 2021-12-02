//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : fontfamily.cpp					                                     //
//  Description: Font family enumeration                                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\ListView.h"
#include "..\..\include\LogWindow.h"

#include "Resource.h"
#include "TrueType.h"
#include "FontFamily.h"
#include "appshare.h"

typedef struct
{
	unsigned	mask;
	unsigned	flag;
	const TCHAR *name;
}	WordDef;


const WordDef NTM_Flags[]	=
{
	{	0xFFFFFFFF, NTM_ITALIC,			_T("Italic")				},
	{	0xFFFFFFFF, NTM_BOLD,			_T("Bold")					},
	{	0xFFFFFFFF, NTM_REGULAR,		_T("Regular")				},
	{	0xFFFFFFFF, NTM_NONNEGATIVE_AC,	_T("Nonnegative AC")		},
	{	0xFFFFFFFF, NTM_PS_OPENTYPE,	_T("Postscript OpenType")	},
	{	0xFFFFFFFF, NTM_TT_OPENTYPE,	_T("TrueType OpenType")		},
	{	0xFFFFFFFF, NTM_MULTIPLEMASTER,	_T("Multiple Master")		},
	{	0xFFFFFFFF, NTM_TYPE1,			_T("Type 1 Font")			},
	{	0xFFFFFFFF, NTM_DSIG,			_T("Digital Signature")		},
	{	0xFFFFFFFF, 0,					NULL					}
};


const WordDef NTM_Family[] = 
{
	{	0x0F,	DEFAULT_PITCH,		_T("Default Pitch")		},
	{	0x0F,	FIXED_PITCH,		_T("Fixed Pitch")		},
	{	0x0F,	VARIABLE_PITCH,		_T("Variable Pitch")	},
	{	0x0F,	MONO_FONT,			_T("Mono Font")			},
	
	{	0xFF0,	FF_DONTCARE,		_T("Dont Care")			},	
	{	0xFF0,	FF_DECORATIVE,		_T("Decorative")		},
	{	0xFF0,	FF_MODERN,			_T("Modern")			},
	{	0xFF0,	FF_ROMAN,			_T("Roman")				},
	{	0xFF0,	FF_SCRIPT,			_T("Script")			},
	{	0xFF0,	FF_SWISS,			_T("Swiss")				},
	{	0xFF0,	0,					NULL				}
};


void DecodeFlag(unsigned flag, const WordDef * dic, TCHAR * result, int bufsize)
{
	result[0] = 0;

	for (; dic->name; dic ++)
	{
		if ( (flag & dic->mask)==dic->flag )
		{
			if ( result[0] )
				_tcscat_s(result, bufsize, _T(", "));

			_tcscat_s(result, bufsize, dic->name);
		}
	}
}


void ListFonts(KListView * pList)
{
	const TCHAR Key_Fonts[] = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

	HKEY hKey;

	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, Key_Fonts, 0, KEY_READ, & hKey)==ERROR_SUCCESS )
	{
		for (int i=0; ; i++)
		{
			TCHAR szValueName[MAX_PATH];
			BYTE  szValueData[MAX_PATH];

			DWORD nValueNameLen = MAX_PATH;
			DWORD nValueDataLen = MAX_PATH;
			DWORD dwType;

			if ( RegEnumValue(hKey, i, szValueName, & nValueNameLen, NULL,
					& dwType, szValueData, & nValueDataLen) != ERROR_SUCCESS )
				break;

			pList->AddItem(0, szValueName);
		    pList->AddItem(1, (const TCHAR*) szValueData);
		}
		RegCloseKey(hKey);
	}
}


int KEnumFontFamily::EnumProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType)
{
	if ( (FontType & m_nType)==0 )
		return TRUE;

	if ( m_nLogFont < MAX_LOGFONT )
		m_LogFont[m_nLogFont ++] = lpelfe->elfLogFont;

	m_pList->AddItem(0, (const TCHAR*) lpelfe->elfFullName);
    m_pList->AddItem(1, (const TCHAR*) lpelfe->elfScript);
	m_pList->AddItem(2, (const TCHAR*) lpelfe->elfStyle);
	m_pList->AddItem(3, (const TCHAR*) lpelfe->elfLogFont.lfFaceName);

	m_pList->AddItem(4, lpelfe->elfLogFont.lfHeight);
	m_pList->AddItem(5, lpelfe->elfLogFont.lfWidth);
	m_pList->AddItem(6, lpelfe->elfLogFont.lfWeight);

	TCHAR Result[MAX_PATH];

	DecodeFlag(lpntme->ntmTm.ntmFlags, NTM_Flags, Result, ARRAYSIZE(Result));
	m_pList->AddItem(7, Result);

	DecodeFlag(lpelfe->elfLogFont.lfPitchAndFamily, NTM_Family, Result, ARRAYSIZE(Result));
	m_pList->AddItem(8, Result);

	return TRUE;
}


void KEnumFontFamily::EnumFontFamilies(HDC hdc, KListView * pList, BYTE charset, 
									   TCHAR * FaceName, unsigned type)
{
	m_pList	   = pList;
	m_nLogFont = 0;
	m_nType    = type;

	LOGFONT lf;
	memset(& lf, 0, sizeof(lf));
	lf.lfCharSet		= charset;
	lf.lfFaceName[0]	= 0;

	if ( FaceName )
		_tcscpy_s(lf.lfFaceName, sizeof(lf.lfFaceName), FaceName);

	lf.lfPitchAndFamily = 0;

    EnumFontFamiliesEx(hdc, & lf, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) this, 0); 
}


void KListViewCanvas::DecodeFontFile(const TCHAR * fontfile)
{
	TCHAR fullname[MAX_PATH];

	if ( _tcschr(fontfile, ':') )
		_tcscpy_s(fullname, ARRAYSIZE(fullname), fontfile);
	else
	{
		GetWindowsDirectory(fullname, MAX_PATH);
		_tcscat_s(fullname, ARRAYSIZE(fullname), _T("\\Fonts\\"));
		_tcscat_s(fullname, ARRAYSIZE(fullname), fontfile);
	}

	// BUG! BUGGY CODE below: We should only pass .FON to WM_USER_RasterFontView.

	// ask frame window to create a new MDI child window to decode a font
	SendMessage(m_hFrame, WM_USER_RasterFontView, 0, (LPARAM) fullname); 
}


void UnicodeRange(LOGFONT * pLogFont, HINSTANCE hInstance)
{
	HFONT hFont  = CreateFontIndirect(pLogFont);
	HDC   hDC	 = GetDC(NULL);
	HGDIOBJ hOld = SelectObject(hDC, hFont);

	// query for size
	DWORD size = GetFontUnicodeRanges(hDC, NULL);

	GLYPHSET * pGlyphSet = (GLYPHSET *) new BYTE[size];

	// get real data
	pGlyphSet->cbThis = size;
	size = GetFontUnicodeRanges(hDC, pGlyphSet);

	KLogWindow * pLog = new KLogWindow;

	assert(pLog);

	pLog->Create(hInstance, _T("UNICODE Range"));

	pLog->Log(_T("%s \r\n"), pLogFont->lfFaceName);

	pLog->Log(_T("cbSize   %d\r\n"), pGlyphSet->cbThis);
	pLog->Log(_T("flAccel  %d\r\n"), pGlyphSet->flAccel);
	pLog->Log(_T("cGlyphsSupported %d\r\n"), pGlyphSet->cGlyphsSupported);
	pLog->Log(_T("cRanges          %d\r\n"), pGlyphSet->cRanges);

	for (unsigned i=0; i<pGlyphSet->cRanges; i++)
	{
		pLog->Log(_T("%3d %04x..%04x (%d)\r\n"), i, 
			pGlyphSet->ranges[i].wcLow, 
			pGlyphSet->ranges[i].wcLow + pGlyphSet->ranges[i].cGlyphs -1,
			pGlyphSet->ranges[i].cGlyphs);
	}

	WORD gi[10];
	size = GetGlyphIndices(hDC, _T("A Quick Brown Fox"), 10, gi, GGI_MARK_NONEXISTING_GLYPHS);

	delete [] (BYTE *) pGlyphSet;

	SelectObject(hDC, hOld);
	ReleaseDC(NULL, hDC);
	DeleteObject(hFont);
}


LRESULT KListViewCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
		{
			m_hWnd		= hWnd;
			m_hViewMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_DIBVIEW));

			RECT rect;

			GetClientRect(m_hWnd, & rect);
			m_Fonts.Create(hWnd, 101, 0, 0, rect.right, rect.bottom, m_hInst);

//			m_Fonts.AddIcon(LVSIL_SMALL, m_hInst, IDI_EMPTY);
//          m_Fonts.AddIcon(LVSIL_SMALL, m_hInst, IDI_EQUAL);
//          m_Fonts.AddIcon(LVSIL_SMALL, m_hInst, IDI_CHANGE);

			if ( m_bShowFontFamily )
			{
				m_Fonts.AddColumn(0, 100, _T("Full Name"));
				m_Fonts.AddColumn(1, 100, _T("Script"));
				m_Fonts.AddColumn(2, 100, _T("Style"));
	
			    m_Fonts.AddColumn(3, 80,  _T("Face Name"));
		        m_Fonts.AddColumn(4, 60,  _T("Height"));
				m_Fonts.AddColumn(5, 60,  _T("Width"));
				m_Fonts.AddColumn(6, 60,  _T("Weight"));
				m_Fonts.AddColumn(7, 130, _T("Flags"));
				m_Fonts.AddColumn(8, 130, _T("Family"));

				HDC hdc = GetDC(NULL);
				enumfont.EnumFontFamilies(hdc, & m_Fonts, DEFAULT_CHARSET, NULL);
				ReleaseDC(NULL, hdc);
			}
			else 
			{
				m_Fonts.AddColumn(0, 100, _T("Name"));
				m_Fonts.AddColumn(1, 100, _T("File"));

				ListFonts(& m_Fonts);
			}

			return 0;
		}

		case WM_SIZE:
			MoveWindow(m_Fonts.GetHWND(), 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);	
			return 0;

		case WM_NOTIFY:
			if (wParam == 101)
			{
				NM_LISTVIEW * pInfo = (NM_LISTVIEW *) lParam;
				
				if ( (pInfo->hdr.code == NM_RCLICK) && (pInfo->iItem != -1) ) 
				{					
					POINT pt = pInfo->ptAction;
					
					ClientToScreen(m_hWnd, & pt);
					
					HMENU hMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_POPUP));

					int id = TrackPopupMenu(GetSubMenu(hMenu, 0), TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
								 pt.x, pt.y, 0, m_hWnd, NULL);

					DestroyMenu(hMenu);

					if ( id==IDM_POP_DECODE )
					{
						if ( m_bShowFontFamily )
						{
							// ask frame window to create a new MDI child window to decode a font
							SendMessage(m_hFrame, WM_USER_TrueTypeFontView, 0, (LPARAM) & enumfont.m_LogFont[pInfo->iItem]);
						}
						else
						{
							TCHAR fontname[MAX_PATH];

							m_Fonts.GetItemText(pInfo->iItem, 1, fontname, MAX_PATH);

							DecodeFontFile(fontname);
						}

						return TRUE;
					}

					if ( id==IDM_POP_UNICODERANGE )
					{
						if ( m_bShowFontFamily )
							UnicodeRange(& enumfont.m_LogFont[pInfo->iItem], m_hInst);
					
						return TRUE;
					}
				}
			}

		default:
			return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 3);
	}
}
