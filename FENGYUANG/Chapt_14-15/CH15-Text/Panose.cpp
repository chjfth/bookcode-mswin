//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : panose.cpp						                                     //
//  Description: Panose font matching                                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define INITGUID
#define _WIN32_WINNT 0x0500
#define STRICT

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#include <tchar.h>
#include <assert.h>

#include "..\..\include\win.h"
#include "..\..\include\canvas.h"
#include "..\..\include\listview.h"
#include "..\..\include\OutlineMetric.h"

#include "resource.h"

#include "PanMap.h"
#include "Panose.h"


class KFontMapper
{
	IPANOSEMapper	* m_pMapper;
	const PANOSE    * m_pFontList;
	int			      m_nFontNo;

public:
	KFontMapper(void)
	{
		m_pMapper   = NULL;
		m_pFontList = NULL;
		m_nFontNo   = 0;

		CoInitialize(NULL);
		CoCreateInstance(CLSID_PANOSEMapper, NULL, CLSCTX_INPROC_SERVER, IID_IPANOSEMapper, 
			(void **) & m_pMapper);
	}

	void SetFontList(const PANOSE * pFontList, int nFontNo)
	{
		m_pFontList = pFontList;
		m_nFontNo   = nFontNo;
	}

	int PickFonts(const PANOSE * pTarget, unsigned short * pOrder, unsigned short * pScore, int nResult)
	{
		m_pMapper->vPANRelaxThreshold();

		int rslt = m_pMapper->unPANPickFonts(
						pOrder,					// recieves order of fonts, best-to-worst
						pScore,					// receives match result for each font
						(BYTE *) pTarget,		// the PANOSE number we're mapping to
						nResult,				// the number of fonts to return
						(BYTE *) m_pFontList,	// the first font's PANOSE number
						m_nFontNo,				// the number of fonts to compare
						sizeof(PANOSE),			// count of bytes between PANOSE numbers
						pTarget->bFamilyType);	// the PANOSE 'family' to map to

		m_pMapper->bPANRestoreThreshold();

		return rslt;
	}
	
	~KFontMapper()
	{
		if ( m_pMapper )
			m_pMapper->Release();
		
		CoUninitialize();
	}
};

// 'Arial Bold Italic' -> PANOSE
bool GetPANOSE(HDC hDC, const TCHAR * fullname, PANOSE * panose, TCHAR facename[], int bufsize)
{
	TCHAR name[MAX_PATH];

	// remove space before
	while (fullname[0]==' ')
		fullname ++;

	_tcscpy_s(name, ARRAYSIZE(name), fullname);

	// remove space after
	for (int i=_tcslen(name)-1; (i>=0) && (name[i]==' '); i--)
		name[i] = 0;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight  = 100;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfWeight  = FW_REGULAR;

	if ( _tcsstr(name, _T("Italic")) )
		lf.lfItalic = TRUE;

	if ( _tcsstr(name, _T("Bold")) )
		lf.lfWeight = FW_BOLD;

	_tcscpy_s(lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), name);
	
	HFONT hFont = CreateFontIndirect(& lf);

	if ( hFont==NULL )
		return false;

	HGDIOBJ hOld = SelectObject(hDC, hFont);

	{
		KOutlineTextMetric otm(hDC);
	
		if ( otm.GetName(otm.m_pOtm->otmpFaceName) )
		{
			_tcscpy_s(facename, bufsize, otm.GetName(otm.m_pOtm->otmpFaceName) ); 
	
			* panose = otm.m_pOtm->otmPanoseNumber;
		}
		else
			facename[0] = 0;
	}

	SelectObject(hDC, hOld);
	DeleteObject(hFont);

	return facename[0] != 0;
}


const TCHAR * PAN_FAMILY[] = 
{
	_T("Any"),
	_T("No Fit"),
	_T("Text and Display"),
	_T("Script"),
	_T("Decorative"),
	_T("Pictorial")
};

const TCHAR * PAN_SERIF[] = 
{
	_T("Any"),
	_T("No Fit"),
	_T("Cove"),
	_T("Obtuse Cove"),
	_T("Square Cove"),
	_T("Obtuse Square Cove"),
	_T("Square"),
	_T("Thin"),
	_T("Bone"),
	_T("Exaggerated"),
	_T("Triangle"),
	_T("Normal Sans"),
	_T("Obtuse Sans"),
	_T("Prep Sans"),
	_T("Flared"),
	_T("Rounded")
};

const TCHAR * PAN_WEIGHT[] =
{
	_T("Any"),
	_T("No Fit"),
	_T("Very Light"),
	_T("Light"),
	_T("Thin"),
	_T("Book"),
	_T("Medium"),
	_T("Demi"),
	_T("Bold"),
	_T("Heavy"),
	_T("Black"),
	_T("Nord")
};

const TCHAR * PAN_PROP[] =
{
	_T("Any"),
	_T("Not Fit"),
	_T("Old Style"),
	_T("Modern"),
	_T("Even Width"),
	_T("Expanded"),
	_T("Condensed"),
	_T("Very Expanded"),
	_T("Very Condensed"),
	_T("Monospaced")
};

const TCHAR * PAN_CONTRAST[] =
{
	_T("Any"),
	_T("Not Fit"),
	_T("None"),
	_T("Very Low"),
	_T("Low"),
	_T("Medium Low"),
	_T("Medium"),
	_T("Mediim High"),
	_T("High"),
	_T("Very High")
};

const TCHAR * PAN_STROKE [] =
{
	_T("Any"),
	_T("Not Fit"),
	_T("Gradual/Diagonal"),
	_T("Gradual/Transitional"),
	_T("Gradual/Vertical"),
	_T("Gradual/Horizontal"),
	_T("Rapid/Vertical"),
	_T("Rapid/Horizontal"),
	_T("Instant/Vertical")
};

const TCHAR * PAN_ARM [] =
{
	_T("Any"),
	_T("Not Fit"),
	_T("Straight Arms/Horizontal"),
	_T("Straight Arms/Wedge"),
	_T("Straight Arms/Vertical"),
	_T("Straight Arms/Single-Serif"),
	_T("Straight Arms/Double-Serif"),
	_T("Non-Straight Arms/Horizontal"),
	_T("Non-Straight Arms/Wedge"),
	_T("Non-Straight Arms/Vertical"),
	_T("Non-Straight Arms/Single-Serif"),
	_T("Non-Straight Arms/Double-Serif")
};

const TCHAR * PAN_LETT [] =
{
	_T("Any"),
	_T("Not Fit"),
	_T("Normal/Contact"),
	_T("Normal/Weighted"),
	_T("Normal/Boxed"),
	_T("Normal/Flattened"),
	_T("Normal/Rounded"),
	_T("Normal/Off Center"),
	_T("Normal/Square"),
	_T("Oblique/Contact"),
	_T("Oblique/Weighted"),
	_T("Oblique/Boxed"),
	_T("Oblique/Flattened"),
	_T("Oblique/Rounded"),
	_T("Oblique/Off Center"),
	_T("Oblique/Square")
};

const TCHAR * PAN_MIDLINE [] =
{
	_T("Any"),
	_T("Not Fit"),
	_T("Standard/Trimmed"),
	_T("Standard/Pointed"),
	_T("Standard/Serifed"),
	_T("High/Trimmed"),
	_T("High/Pointed"),
	_T("High/Serifed"),
	_T("Constant/Trimmed"),
	_T("Constant/Pointed"),
	_T("Constant/Serifed"),
	_T("Low/Trimmed"),
	_T("Low/Pointed"),
	_T("Low/Serifed")
};

const TCHAR * PAN_XHEIGHT [] = 
{
	_T("Any"),
	_T("Not Fit"),
	_T("Constant/Small"),
	_T("Constant/Standard"),
	_T("Constant/Large"),
	_T("Ducking/Small"),
	_T("Ducking/Standard"),
	_T("Ducking/Large")
};


void KPanoseView::AddFont(const TCHAR * fullname, HDC hDC)
{
	PANOSE panose;
	TCHAR  facename[MAX_PATH];

	if ( GetPANOSE(hDC, fullname, & panose, facename, ARRAYSIZE(facename)) )
	{
		assert(m_nFonts<MAX_FONTS);

		if ( m_nFonts < MAX_FONTS )
		{
			_tcscpy_s(m_TypeFace[m_nFonts], ARRAYSIZE(m_TypeFace[m_nFonts]), facename);
			m_Panose[m_nFonts] = panose;

			m_nFonts ++;
		}
		
		m_Fonts.AddItem( 0, fullname);
		m_Fonts.AddItem( 1, facename);
		
		m_Fonts.AddItem( 2, PAN_FAMILY  [panose.bFamilyType]);
		m_Fonts.AddItem( 3, PAN_SERIF   [panose.bSerifStyle]);
		m_Fonts.AddItem( 4, PAN_WEIGHT  [panose.bWeight]);
		m_Fonts.AddItem( 5, PAN_PROP    [panose.bProportion]);
		m_Fonts.AddItem( 6, PAN_CONTRAST[panose.bContrast]);
		m_Fonts.AddItem( 7, PAN_STROKE  [panose.bStrokeVariation]);
		m_Fonts.AddItem( 8, PAN_ARM     [panose.bArmStyle]);
		m_Fonts.AddItem( 9, PAN_LETT    [panose.bLetterform]);
		m_Fonts.AddItem(10, PAN_MIDLINE [panose.bMidline]);
		m_Fonts.AddItem(11, PAN_XHEIGHT [panose.bXHeight]);
	}
	else
		assert(false);
}


void KPanoseView::ListPANOSE(void)
{
	const TCHAR Key_Fonts[] = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

	HKEY hKey;

	HDC hDC = GetDC(NULL);

	m_nFonts = 0;

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

			// szValueName font name
		    // szValueData file name

			TCHAR * p = _tcschr(szValueName, '(');
			if ( p && p[1]=='T' ) // (TrueType)
			{
				* p = 0;

				p = _tcschr(szValueName, '&'); // Font collection: A & B

				if ( p )
				{
					*p=0;
					AddFont(szValueName, hDC);
					AddFont(p+1, hDC);
				}
				else
					AddFont(szValueName, hDC);
			}
		}
		
		RegCloseKey(hKey);
	}

	ReleaseDC(NULL, hDC);
}


// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Shared Tools\Panose
void Map(void)
{
/*	KFontMapper mapper;

	mapper.SetFontList(s_panfonts, NUM_PANFONTS);

	unsigned short Order[3];
	unsigned short Score[3];

	int result = mapper.PickFonts(& s_panfonts[1], Order, Score, 3);

	int i = 0;
*/
}


bool KPanoseView::Initialize(HINSTANCE hInstance, KStatusWindow * pStatus, HWND hWndFrame)
{
	m_hFrame    = hWndFrame;
	m_hInst     = hInstance;
	m_pStatus   = pStatus;

	RegisterClass(_T("PanoseViewClass"), hInstance);
		
	return true;
}


LRESULT KPanoseView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd		= hWnd;
			m_hViewMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_PANOSE));

			{
				RECT rect;

				GetClientRect(m_hWnd, & rect);
				m_Fonts.Create(hWnd, 101, 0, 0, rect.right, rect.bottom, m_hInst);
			}

			m_Fonts.AddColumn( 0, 100, _T("Original"));
			m_Fonts.AddColumn( 1, 100, _T("Typeface"));

			m_Fonts.AddColumn( 2, 100, _T("Family"));
			m_Fonts.AddColumn( 3, 100, _T("Serif"));
			m_Fonts.AddColumn( 4, 100, _T("Weight"));
			m_Fonts.AddColumn( 5, 100, _T("Proportion"));
			m_Fonts.AddColumn( 6, 100, _T("Contract"));
			m_Fonts.AddColumn( 7, 100, _T("Stroke"));
			m_Fonts.AddColumn( 8, 100, _T("Arm"));
			m_Fonts.AddColumn( 9, 100, _T("Letterform"));
			m_Fonts.AddColumn(10, 100, _T("Midline"));
			m_Fonts.AddColumn(11, 100, _T("XHeight"));

			ListPANOSE();
			return 0;

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

					if ( id==IDM_PANOSE_MATCH )
					{
						TCHAR fontname[MAX_PATH];

						m_Fonts.GetItemText(pInfo->iItem, 1, fontname, MAX_PATH);

						KFontMapper mapper;

						mapper.SetFontList(m_Panose, m_nFonts);

						unsigned short Order[10];
						unsigned short Score[10];

						int result = mapper.PickFonts(& m_Panose[pInfo->iItem], Order, Score, 10);

						TCHAR temp[MAX_PATH] = {};
						for (int i=0; i<result; i++)
							wsprintf( temp + _tcslen(temp), _T("%d\t%d\t%s\n"), i, Score[i], m_TypeFace[Order[i]]);

						MyMessageBox(NULL, temp, fontname, MB_OK, IDI_TEXT);

						return TRUE;
					}

				}
			}

		default:
			return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 3);
	}
}
