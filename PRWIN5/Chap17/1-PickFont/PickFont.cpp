/*-----------------------------------------
PICKFONT.C -- Create Logical Font
       (c) Charles Petzold, 1998

Enhancements by Jimm Chen.
[2022-09-04] v2.0, can be compiled with VC2010, not supporting VC6 any more.
-----------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "..\..\vaDbg.h"
#include "resource.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define VERSION "2.3"

// Formatting for BCHAR fields of TEXTMETRIC structure

#ifdef UNICODE
#define BCHARFORM TEXT ("0x%04X")
#else
#define BCHARFORM TEXT ("0x%02X")
#endif

#ifdef UNICODE
#define MAKE_TCHAR_UNSIGNED(c) (c)
#else
#define MAKE_TCHAR_UNSIGNED(c) ((unsigned char)(c))
#endif


// Structure shared between main window and dialog box

typedef struct
{
	int        iDevice, iMapMode ;
	BOOL       fMatchAspect ;
	BOOL       fAdvGraphics ;
	LOGFONT    lf ;
	TEXTMETRIC tm ;
	TCHAR      szFaceName [LF_FULLFACESIZE] ;
}
DLGPARAMS ;

#define RELOAD_SAMPLE_TEXT 5

#define literal_BUFMAX 100
#define hexform_BUFMAX (literal_BUFMAX*5)

struct DlgSampleText_st
{
	bool usehex;
	TCHAR literal[literal_BUFMAX+1];
	TCHAR hexform[hexform_BUFMAX+1];
};

// Global variables

HWND  g_hdlg ;
TCHAR szAppName[] = TEXT ("PickFont") ;

TCHAR gar_sample_text[literal_BUFMAX+1] = {0};
int g_sample_text_len = 0; // in TCHARs

DlgSampleText_st g_dlgsamp = {0};

int g_refreshcount = 0;

HINSTANCE g_hInstanceExe = NULL;

// Forward declarations of functions

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
INT_PTR    CALLBACK DlgProc (HWND, UINT, WPARAM, LPARAM) ;
void SetLogFontFromFields    (HWND hdlg, DLGPARAMS * pdp) ;
void SetFieldsFromTextMetric (HWND hdlg, DLGPARAMS * pdp) ;
void MySetMapMode (HDC hdc, int iMapMode) ;

void prepare_cmd_params()
{
	static TCHAR  szText_stock[] = TEXT ("\x41\x42\x43\x44\x45 ")
		TEXT ("\x61\x62\x63\x64\x65 ")

		TEXT ("\xC0\xC1\xC2\xC3\xC4\xC5 ")
		TEXT ("\xE0\xE1\xE2\xE3\xE4\xE5 ") 
#ifdef UNICODE
		TEXT ("\x0390\x0391\x0392\x0393\x0394\x0395 ")
		TEXT ("\x03B0\x03B1\x03B2\x03B3\x03B4\x03B5 ")

		TEXT ("\x0410\x0411\x0412\x0413\x0414\x0415 ")
		TEXT ("\x0430\x0431\x0432\x0433\x0434\x0435 ")

		TEXT ("\x7535\x8111\x771F\x6709\x8DA3") // chj: meaningful Chinese text
#endif
		;

	parse_cmdparam_TCHARs(
		GetCommandLine(), true,
		gar_sample_text, ARRAYSIZE(gar_sample_text), &g_sample_text_len,
		g_dlgsamp.literal, ARRAYSIZE(g_dlgsamp.literal)
		);

	if(gar_sample_text[0]=='\0')
	{
		// Use stock sample-text

		_tcscpy_s(gar_sample_text, ARRAYSIZE(gar_sample_text), szText_stock);
		g_sample_text_len = (int)_tcslen(gar_sample_text);

		_tcscpy_s(g_dlgsamp.literal, ARRAYSIZE(g_dlgsamp.literal), szText_stock);

		g_dlgsamp.usehex = false;
	}
	else
	{
		if(g_dlgsamp.literal[0])
			g_dlgsamp.usehex = false;
		else
			g_dlgsamp.usehex = true;
	}


	// Make a hexdump of g_sample_text[] into g_dlg.samp.hexform[], so that user can later edit it.

	TCHAR *hexbuf = g_dlgsamp.hexform;
	const int hexbuf_maxlen = ARRAYSIZE(g_dlgsamp.hexform);

	int hexbuf_used = 0;
	int i;
	for(i=0; i<g_sample_text_len; i++)
	{
		if(hexbuf_used >= hexbuf_maxlen-1)
			break;

		_sntprintf_s(hexbuf+hexbuf_used, hexbuf_maxlen-1-hexbuf_used, _TRUNCATE,
			_T("%02X "), // "%02X" so that "<256" WCHAR costs only a width of two 
			MAKE_TCHAR_UNSIGNED(gar_sample_text[i]));

		int thislen = (int)_tcslen(hexbuf+hexbuf_used);
		hexbuf_used += thislen;
	}
}

void reload_sample_text()
{
	// Input: g_dlgsamp
	// Output gar_sample_text[], g_sample_text_len

	if(!g_dlgsamp.usehex)
	{
		// g_dlgsamp.literal[] => sample-text

		_tcscpy_s(gar_sample_text, ARRAYSIZE(gar_sample_text), g_dlgsamp.literal);
		g_sample_text_len = (int)_tcslen(gar_sample_text);
	}
	else
	{
		// g_dlgsamp.hexform[] => sample-text

		TCHAR cmdline[10+hexform_BUFMAX]={};
		_sntprintf_s(cmdline, _TRUNCATE, _T("EXENAME %s"), g_dlgsamp.hexform);

		parse_cmdparam_TCHARs(cmdline, false,
			gar_sample_text, ARRAYSIZE(gar_sample_text), &g_sample_text_len,
			NULL, 0);
	}
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	prepare_cmd_params();

	g_hInstanceExe = hInstance;

	HWND     hwnd ;
	MSG      msg ;
	WNDCLASS wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //(HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = szAppName ; 
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	const TCHAR *wintitle_prefix =
#ifdef UNICODE
	TEXT ("(Unicode) ") 
#else
	TEXT ("(ANSI) ") 
#endif
	TEXT ("PickFont: Create Logical Font");

	TCHAR szTitle[100]={};
	_sntprintf_s(szTitle, _TRUNCATE, _T("%s - v%s , GetACP=%u"),
		wintitle_prefix, TEXT(VERSION), GetACP()
		);

	hwnd = CreateWindow (szAppName, 
		szTitle,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL) ;

	chSETWINDOWICON(hwnd, MAKEINTRESOURCE(IDI_ICON1));

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		if (g_hdlg == 0 || !IsDialogMessage (g_hdlg, &msg))
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
	}
	return (int)msg.wParam ;
}

void Set_WindowSizeByClientSize(HWND hwnd, int clicx, int clicy)
{
	RECT rowin, rocli;
	GetWindowRect(hwnd, &rowin);
	GetClientRect(hwnd, &rocli);
	int nccx = (rowin.right-rowin.left) - (rocli.right-rocli.left);
	int nccy = (rowin.bottom-rowin.top) - (rocli.bottom-rocli.top); // nc: non-client

	MoveWindow(hwnd, rowin.left, rowin.top, clicx+nccx, clicy+nccy, TRUE);
}

int TextOut_hexdump(HDC hdc, const TCHAR *pText, int Textlen, int xDraw, int yDraw)
{
	const int BUFSIZE_Hexdump = ARRAYSIZE(gar_sample_text)*(sizeof(TCHAR)*2+1);
	TCHAR szHexdmp[BUFSIZE_Hexdump+1]={};
	
	const int steplen = sizeof(TCHAR)*2+1; //  +1 for the "." separator
	int usedlen = 0;
	int i;
	for(i=0; i<Textlen; i++, usedlen = steplen*i)
	{
		if(usedlen>=BUFSIZE_Hexdump)
			break;

		_sntprintf_s(szHexdmp+usedlen, BUFSIZE_Hexdump-usedlen, _TRUNCATE,
			sizeof(TCHAR)==2 ? _T("%04X.") : _T("%02X."),
			MAKE_TCHAR_UNSIGNED(pText[i]));
	}

	SIZE drawsize = {};
	LOGFONT lf = {};
	lf.lfHeight = -12;
	static HFONT s_font = CreateFontIndirect(&lf);

	HFONT prevfont = SelectFont(hdc, s_font);
	COLORREF prevcolor = SetTextColor(hdc, RGB(128, 128, 128));
	
	TextOut(hdc, xDraw, yDraw, szHexdmp, usedlen);
	BOOL succ = GetTextExtentPoint32(hdc, szHexdmp, usedlen, &drawsize);

	// restore DC attr
	SelectFont(hdc, prevfont);
	SetTextColor(hdc, prevcolor); 

	return succ ? drawsize.cy : 32; // 32 is arbitrary
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static DLGPARAMS dp ;
	const TCHAR *pText = gar_sample_text;
	int Textlen = g_sample_text_len;;
	HDC              hdc ;
	PAINTSTRUCT      ps ;
	RECT             rect ;

	switch (message)
	{{
	case WM_CREATE:
	{
		dp.iDevice = IDM_DEVICE_SCREEN ;

		g_hdlg = CreateDialogParam (((LPCREATESTRUCT) lParam)->hInstance, 
			szAppName, hwnd, DlgProc, (LPARAM) &dp) ;

		// Chj: adjust main window size accordingly.
		RECT rcli;
		GetClientRect (g_hdlg, &rcli) ;
		Set_WindowSizeByClientSize(hwnd, rcli.right+16, rcli.bottom+64);
		return 0 ;
	}

	case WM_SETFOCUS:
		SetFocus (g_hdlg) ;
		return 0 ;

	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDM_DEVICE_SCREEN:
		case IDM_DEVICE_PRINTER:
			CheckMenuItem (GetMenu (hwnd), dp.iDevice, MF_UNCHECKED) ;
			dp.iDevice = LOWORD (wParam) ;
			CheckMenuItem (GetMenu (hwnd), dp.iDevice, MF_CHECKED) ;
			SendMessage (hwnd, WM_COMMAND, IDOK, 0) ;
			return 0 ;
		}
		break ;

	case WM_PAINT:
	{
		hdc = BeginPaint (hwnd, &ps) ;

		// Set graphics mode so escapement works in Windows NT

		SetGraphicsMode (hdc, dp.fAdvGraphics ? GM_ADVANCED : GM_COMPATIBLE) ;

		// Set the mapping mode and the mapper flag

		MySetMapMode (hdc, dp.iMapMode) ;
		SetMapperFlags (hdc, dp.fMatchAspect) ;

		// Find the point to begin drawing text

		GetClientRect (g_hdlg, &rect) ;
		rect.bottom += 1 ;
		DPtoLP (hdc, (PPOINT) &rect, 2) ;

		int xText = rect.left;
		int yText = rect.bottom;

		// Create and select the font; display the text

		SelectObject (hdc, CreateFontIndirect (&dp.lf)) ;

		int hexdump_height = TextOut_hexdump(hdc, pText, Textlen, xText, yText);

		// Display our sample-text
		TextOut (hdc, xText, yText+hexdump_height, pText, Textlen) ;

		// Write some debug info
		//
		SIZE rsize = {0};
		BOOL succ = GetTextExtentPoint32(hdc, pText, Textlen, &rsize);
		vaDbg(TEXT("PickFont sample text dimension: %dx%d"), rsize.cx, rsize.cy);

		DeleteObject (SelectObject (hdc, GetStockObject (SYSTEM_FONT))) ;
		EndPaint (hwnd, &ps) ;
		return 0 ;
	}

	case WM_SYSCOLORCHANGE :
	{
		InvalidateRect (hwnd, NULL, TRUE) ;
		break ;
	}
	case WM_DESTROY:
	{	PostQuitMessage (0) ;
		return 0 ;
	}
	}}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

void DlgRefresh_ChangeSampleText(HWND hdlg, bool usehex)
{
	if(usehex)
	{
		CheckDlgButton(hdlg, IDC_RADIO_USE_HEXFORM, TRUE);
		Edit_Enable(GetDlgItem(hdlg, IDC_EDIT_HEXFORM), TRUE);
		Edit_Enable(GetDlgItem(hdlg, IDC_EDIT_LITERAL), FALSE);
	}
	else
	{
		CheckDlgButton(hdlg, IDC_RADIO_USE_LITERAL, TRUE);
		Edit_Enable(GetDlgItem(hdlg, IDC_EDIT_LITERAL), TRUE);
		Edit_Enable(GetDlgItem(hdlg, IDC_EDIT_HEXFORM), FALSE);
	}
}

INT_PTR CALLBACK DlgProc_ChangeSampleText(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool s_usehex = false;
	static const TCHAR *s_helptext=
		_T("Hint: You can pass in sample-text from command line.\r\n")
		_T("If only one parameter is given to the command-line, typically wrapped with a double-quotes, it is considered literal.\r\n")
		_T("If more than one parameters are given, each one is in hexform represent a TCHAR.")
		;

	switch (message)
	{{
	case WM_INITDIALOG:
	{
		SetDlgItemText(hdlg, IDC_HELPTEXT, s_helptext);

		SetDlgItemText(hdlg, IDC_EDIT_HEXFORM, g_dlgsamp.hexform);
		SetDlgItemText(hdlg, IDC_EDIT_LITERAL, g_dlgsamp.literal);
		s_usehex = g_dlgsamp.usehex;

		DlgRefresh_ChangeSampleText(hdlg, s_usehex);
		return TRUE;
	}
	case WM_COMMAND:
	{
		int id = GET_WM_COMMAND_ID(wParam, lParam);
		HWND hctrl = GET_WM_COMMAND_HWND(wParam, lParam);

		if(id==IDC_RADIO_USE_HEXFORM || id==IDC_RADIO_USE_LITERAL)
		{
			s_usehex = (id==IDC_RADIO_USE_HEXFORM);
			DlgRefresh_ChangeSampleText(hdlg, s_usehex);
		}

		if(id==IDOK)
		{
			GetDlgItemText(hdlg, IDC_EDIT_HEXFORM, g_dlgsamp.hexform, ARRAYSIZE(g_dlgsamp.hexform));
			GetDlgItemText(hdlg, IDC_EDIT_LITERAL, g_dlgsamp.literal, ARRAYSIZE(g_dlgsamp.literal));				

			g_dlgsamp.usehex = s_usehex;

			if(s_usehex && !g_dlgsamp.hexform[0])
			{
				MessageBox(hdlg, _T("Hexform editbox is empty!"), _T("Input Error"), MB_ICONEXCLAMATION);
				return TRUE;
			}
			if(!s_usehex && !g_dlgsamp.literal[0])
			{
				MessageBox(hdlg, _T("Literal editbox is empty!"), _T("Input Error"), MB_ICONEXCLAMATION);
				return TRUE;
			}

			reload_sample_text();

			EndDialog(hdlg, RELOAD_SAMPLE_TEXT);
		}

 		if(id==ID_MYCANCEL)
 		{
			// I do NOT check id==IDCANCEL, bcz I do NOT want ESC to close the dialog-box.
 			EndDialog(hdlg, 3);
 		}

		return TRUE;

	} // WM_COMMAND
		
	case WM_SYSCOMMAND:
	{
		if(wParam==SC_CLOSE)
		{
			// So user can use Alt+F4 or window-title Close button to close it.
			EndDialog(hdlg, 3);
		}
	}

	}}

	return FALSE;
}

INT_PTR CALLBACK DlgProc (HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static DLGPARAMS * pdp ;
	static PRINTDLG    pd = { sizeof (PRINTDLG) } ;
	HDC                hdcDevice ;
	HFONT              hFont ;

	switch (message)
	{{
	case WM_INITDIALOG:
		// Save pointer to dialog-parameters structure in WndProc

		pdp = (DLGPARAMS *) lParam ;

		SendDlgItemMessage (hdlg, IDC_LF_FACENAME, EM_LIMITTEXT, 
			LF_FACESIZE - 1, 0) ;

		CheckRadioButton (hdlg, IDC_OUT_DEFAULT, IDC_OUT_OUTLINE,
			IDC_OUT_DEFAULT) ;

		CheckRadioButton (hdlg, IDC_DEFAULT_QUALITY, IDC_PROOF_QUALITY,
			IDC_DEFAULT_QUALITY) ;

		CheckRadioButton (hdlg, IDC_DEFAULT_PITCH, IDC_VARIABLE_PITCH,
			IDC_DEFAULT_PITCH) ;

		CheckRadioButton (hdlg, IDC_FF_DONTCARE, IDC_FF_DECORATIVE,
			IDC_FF_DONTCARE) ;

		CheckRadioButton (hdlg, IDC_MM_TEXT, IDC_MM_LOGTWIPS,
			IDC_MM_TEXT) ;

		SendMessage (hdlg, WM_COMMAND, IDOK, 0) ;

		// fall through
	case WM_SETFOCUS:
		SetFocus (GetDlgItem (hdlg, IDC_LF_HEIGHT)) ;
		return FALSE ;

	case WM_COMMAND:
		switch (LOWORD (wParam))
		{{
		case IDC_CHARSET_HELP:
		{
			TCHAR info_charsets[1024]={};
			vaMsgBox(hdlg, MB_ICONINFORMATION, TEXT("LOGFONT.lfCharSet meaning"), 
				_T("%s\n%s"),

				charsets_to_codepages_hint(info_charsets, ARRAYSIZE(info_charsets)),

				TEXT ("Chj: Does Charset=0 mean ANSI or West European? MSDN is misleading on this.\n")
				TEXT ("The fact is: It depends on your font height, and the mapping rule is weird.\n")
				TEXT ("Please be aware, when it acts as ANSI, it is same as Default (from codepage perspective).\n")

				);
			return TRUE ;

			// These radio buttons set the lfOutPrecision field
		}
		case IDC_OUT_DEFAULT:   
			pdp->lf.lfOutPrecision = OUT_DEFAULT_PRECIS ;  
			return TRUE ;

		case IDC_OUT_STRING:
			pdp->lf.lfOutPrecision = OUT_STRING_PRECIS ;  
			return TRUE ;

		case IDC_OUT_CHARACTER:
			pdp->lf.lfOutPrecision = OUT_CHARACTER_PRECIS ;  
			return TRUE ;

		case IDC_OUT_STROKE:
			pdp->lf.lfOutPrecision = OUT_STROKE_PRECIS ;  
			return TRUE ;

		case IDC_OUT_TT:
			pdp->lf.lfOutPrecision = OUT_TT_PRECIS ;  
			return TRUE ;

		case IDC_OUT_DEVICE:
			pdp->lf.lfOutPrecision = OUT_DEVICE_PRECIS ;  
			return TRUE ;

		case IDC_OUT_RASTER:
			pdp->lf.lfOutPrecision = OUT_RASTER_PRECIS ;  
			return TRUE ;

		case IDC_OUT_TT_ONLY:
			pdp->lf.lfOutPrecision = OUT_TT_ONLY_PRECIS ;  
			return TRUE ;

		case IDC_OUT_OUTLINE:
			pdp->lf.lfOutPrecision = OUT_OUTLINE_PRECIS ;  
			return TRUE ;

			// These three radio buttons set the lfQuality field

		case IDC_DEFAULT_QUALITY:
			pdp->lf.lfQuality = DEFAULT_QUALITY ;   
			return TRUE ;

		case IDC_DRAFT_QUALITY:
			pdp->lf.lfQuality = DRAFT_QUALITY ;  
			return TRUE ;

		case IDC_PROOF_QUALITY:
			pdp->lf.lfQuality = PROOF_QUALITY ;  
			return TRUE ;

			// These three radio buttons set the lower nibble
			//   of the lfPitchAndFamily field

		case IDC_DEFAULT_PITCH:
			pdp->lf.lfPitchAndFamily = (BYTE)
				((0xF0 & pdp->lf.lfPitchAndFamily) | DEFAULT_PITCH) ; 
			return TRUE ;

		case IDC_FIXED_PITCH:
			pdp->lf.lfPitchAndFamily = (BYTE)
				((0xF0 & pdp->lf.lfPitchAndFamily) | FIXED_PITCH) ; 
			return TRUE ;

		case IDC_VARIABLE_PITCH:
			pdp->lf.lfPitchAndFamily = (BYTE)
				((0xF0 & pdp->lf.lfPitchAndFamily) | VARIABLE_PITCH) ;  
			return TRUE ;

			// These six radio buttons set the upper nibble
			//   of the lpPitchAndFamily field

		case IDC_FF_DONTCARE:
			pdp->lf.lfPitchAndFamily = (BYTE)
				((0x0F & pdp->lf.lfPitchAndFamily) | FF_DONTCARE) ;  
			return TRUE ;

		case IDC_FF_ROMAN:
			pdp->lf.lfPitchAndFamily = (BYTE)
				((0x0F & pdp->lf.lfPitchAndFamily) | FF_ROMAN) ;  
			return TRUE ;

		case IDC_FF_SWISS:
			pdp->lf.lfPitchAndFamily = (BYTE)
				((0x0F & pdp->lf.lfPitchAndFamily) | FF_SWISS) ;  
			return TRUE ;

		case IDC_FF_MODERN:
			pdp->lf.lfPitchAndFamily = (BYTE)
				((0x0F & pdp->lf.lfPitchAndFamily) | FF_MODERN) ;  
			return TRUE ;

		case IDC_FF_SCRIPT:
			pdp->lf.lfPitchAndFamily = (BYTE)
				((0x0F & pdp->lf.lfPitchAndFamily) | FF_SCRIPT) ;  
			return TRUE ;

		case IDC_FF_DECORATIVE:
			pdp->lf.lfPitchAndFamily = (BYTE)
				((0x0F & pdp->lf.lfPitchAndFamily) | FF_DECORATIVE) ;  
			return TRUE ;

			// Mapping mode:

		case IDC_MM_TEXT:
		case IDC_MM_LOMETRIC:
		case IDC_MM_HIMETRIC:
		case IDC_MM_LOENGLISH:
		case IDC_MM_HIENGLISH:
		case IDC_MM_TWIPS:
		case IDC_MM_LOGTWIPS:
			pdp->iMapMode = LOWORD (wParam) ;
			return TRUE ;

			// OK button pressed
			// -----------------

		case IDOK:
		{
			// Get LOGFONT structure

			SetLogFontFromFields (hdlg, pdp) ;

			// Set Match-Aspect and Advanced Graphics flags

			pdp->fMatchAspect = IsDlgButtonChecked (hdlg, IDC_MATCH_ASPECT) ;
			pdp->fAdvGraphics = IsDlgButtonChecked (hdlg, IDC_ADV_GRAPHICS) ;

			// Get Information Context

			if (pdp->iDevice == IDM_DEVICE_SCREEN)
			{
				hdcDevice = CreateIC (TEXT ("DISPLAY"), NULL, NULL, NULL) ;
			}
			else
			{
				pd.hwndOwner = hdlg ;
				pd.Flags = PD_RETURNDEFAULT | PD_RETURNIC ;
				pd.hDevNames = NULL ;
				pd.hDevMode = NULL ;

				PrintDlg (&pd) ;

				hdcDevice = pd.hDC ;
			}
			// Set the mapping mode and the mapper flag

			MySetMapMode (hdcDevice, pdp->iMapMode) ;
			SetMapperFlags (hdcDevice, pdp->fMatchAspect) ;

			// Create font and select it into IC

			hFont = CreateFontIndirect (&pdp->lf) ;
			SelectObject (hdcDevice, hFont) ;

			// Get the text metrics and face name

			GetTextMetrics (hdcDevice, &pdp->tm) ;
			GetTextFace (hdcDevice, LF_FULLFACESIZE, pdp->szFaceName) ;
			
			// acquire some debug-info >>>

			FONTSIGNATURE fontsig = {}; 
			int charset2 = GetTextCharsetInfo(hdcDevice, &fontsig, 0);
			if(fontsig.fsUsb[0]==0)
			{
				CHARSETINFO csi = {};
				BOOL succ = TranslateCharsetInfo((DWORD*)(pdp->lf.lfCharSet), &csi, TCI_SRCCHARSET);
				vaDbg(TEXT("Used TranslateCharsetInfo(TCI_SRCCHARSET) for fontface \"%s\", return %d."), pdp->szFaceName, succ);
			}

			if(charset2==pdp->tm.tmCharSet)
			{
				vaDbg(TEXT("GetTextCharsetInfo() returns CharSet=%d (match tm.tmCharSet)"), charset2);

				if(fontsig.fsUsb[0])
				{
					vaDbg(TEXT("\"%s\" Unicode-subranges: %04X.%04X.%04X.%04X , Codepages: %04X.%04X"),
						pdp->szFaceName,
						fontsig.fsUsb[3], fontsig.fsUsb[2], fontsig.fsUsb[1], fontsig.fsUsb[0],
						fontsig.fsCsb[1], fontsig.fsCsb[0]);
				}
			}
			else
			{
				vaMsgBox(hdlg, MB_ICONEXCLAMATION, TEXT("UNEXPECT!"),
					TEXT("GetTextCharsetInfo() returns CharSet=%d.\r\n")
					TEXT("Does NOT match tm.tmCharSet=%d.\r\n")
					,
					charset2, pdp->tm.tmCharSet);
			}			

			// acquire some debug-info <<<

			DeleteDC (hdcDevice) ;
			DeleteObject (hFont) ;

			// Update dialog fields and invalidate main window

			SetFieldsFromTextMetric (hdlg, pdp) ;
			InvalidateRect (GetParent (hdlg), NULL, TRUE) ;
			return TRUE ;
		} // WM_COMMAND.ID_OK
		case IDC_BTN_CHANGE_SAMPLE_TEXT:
			{
				INT_PTR ret = DialogBox(g_hInstanceExe, MAKEINTRESOURCE(IDD_CHANGE_SAMPLE_TEXT), hdlg, 
					DlgProc_ChangeSampleText);

				if(ret==RELOAD_SAMPLE_TEXT)
				{
					SendMessage (hdlg, WM_COMMAND, IDOK, 0) ;
				}
				return 0;
			}

		}} // WM_COMMAND.switch done
		break ;

	case WM_KEYUP:
	{
		// I can NOT get here, why?
		if(VK_F1==wParam)
		{	
			SendMessage (hdlg, WM_COMMAND, IDOK, 0) ;
			return 0;
		}
	}

	}}
	return FALSE ;
}
void SetLogFontFromFields (HWND hdlg, DLGPARAMS * pdp)
{
	pdp->lf.lfHeight      = GetDlgItemInt (hdlg, IDC_LF_HEIGHT,  NULL, TRUE) ;
	pdp->lf.lfWidth       = GetDlgItemInt (hdlg, IDC_LF_WIDTH,   NULL, TRUE) ;
	pdp->lf.lfEscapement  = GetDlgItemInt (hdlg, IDC_LF_ESCAPE,  NULL, TRUE) ;
	pdp->lf.lfOrientation = GetDlgItemInt (hdlg, IDC_LF_ORIENT,  NULL, TRUE) ;
	pdp->lf.lfWeight      = GetDlgItemInt (hdlg, IDC_LF_WEIGHT,  NULL, TRUE) ;
	pdp->lf.lfCharSet     = GetDlgItemInt (hdlg, IDC_LF_CHARSET, NULL, FALSE) ;

	pdp->lf.lfItalic = 
		IsDlgButtonChecked (hdlg, IDC_LF_ITALIC) == BST_CHECKED ;
	pdp->lf.lfUnderline = 
		IsDlgButtonChecked (hdlg, IDC_LF_UNDER)  == BST_CHECKED ;
	pdp->lf.lfStrikeOut = 
		IsDlgButtonChecked (hdlg, IDC_LF_STRIKE) == BST_CHECKED ;

	GetDlgItemText (hdlg, IDC_LF_FACENAME, pdp->lf.lfFaceName, LF_FACESIZE) ;
}

void SetFieldsFromTextMetric (HWND hdlg, DLGPARAMS * pdp) 
{
	TCHAR   szBuffer [10] ;
	TCHAR * szYes = TEXT ("Yes") ; 
	TCHAR * szNo  = TEXT ("No") ;
	TCHAR * szFamily [] = { TEXT ("Don't Know"), TEXT ("Roman"),
		TEXT ("Swiss"),      TEXT ("Modern"),
		TEXT ("Script"),     TEXT ("Decorative"), 
		TEXT ("Undefined") } ;

	SetDlgItemInt (hdlg, IDC_TM_HEIGHT,   pdp->tm.tmHeight,           TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_ASCENT,   pdp->tm.tmAscent,           TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_DESCENT,  pdp->tm.tmDescent,          TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_INTLEAD,  pdp->tm.tmInternalLeading,  TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_EXTLEAD,  pdp->tm.tmExternalLeading,  TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_AVECHAR,  pdp->tm.tmAveCharWidth,     TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_MAXCHAR,  pdp->tm.tmMaxCharWidth,     TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_WEIGHT,   pdp->tm.tmWeight,           TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_OVERHANG, pdp->tm.tmOverhang,         TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_DIGASPX,  pdp->tm.tmDigitizedAspectX, TRUE) ;
	SetDlgItemInt (hdlg, IDC_TM_DIGASPY,  pdp->tm.tmDigitizedAspectY, TRUE) ;

	wsprintf (szBuffer, BCHARFORM, pdp->tm.tmFirstChar) ;
	SetDlgItemText (hdlg, IDC_TM_FIRSTCHAR, szBuffer) ;

	wsprintf (szBuffer, BCHARFORM, pdp->tm.tmLastChar) ;
	SetDlgItemText (hdlg, IDC_TM_LASTCHAR, szBuffer) ;

	wsprintf (szBuffer, BCHARFORM, pdp->tm.tmDefaultChar) ;
	SetDlgItemText (hdlg, IDC_TM_DEFCHAR, szBuffer) ;

	wsprintf (szBuffer, BCHARFORM, pdp->tm.tmBreakChar) ;
	SetDlgItemText (hdlg, IDC_TM_BREAKCHAR, szBuffer) ;

	SetDlgItemText (hdlg, IDC_TM_ITALIC, pdp->tm.tmItalic     ? szYes : szNo) ;
	SetDlgItemText (hdlg, IDC_TM_UNDER,  pdp->tm.tmUnderlined ? szYes : szNo) ;
	SetDlgItemText (hdlg, IDC_TM_STRUCK, pdp->tm.tmStruckOut  ? szYes : szNo) ;

	SetDlgItemText (hdlg, IDC_TM_VARIABLE, 
		TMPF_FIXED_PITCH & pdp->tm.tmPitchAndFamily ? szYes : szNo) ;

	SetDlgItemText (hdlg, IDC_TM_VECTOR, 
		TMPF_VECTOR & pdp->tm.tmPitchAndFamily ? szYes : szNo) ;

	SetDlgItemText (hdlg, IDC_TM_TRUETYPE, 
		TMPF_TRUETYPE & pdp->tm.tmPitchAndFamily ? szYes : szNo) ;

	SetDlgItemText (hdlg, IDC_TM_DEVICE, 
		TMPF_DEVICE & pdp->tm.tmPitchAndFamily ? szYes : szNo) ;

	SetDlgItemText (hdlg, IDC_TM_FAMILY, 
		szFamily [min (6, pdp->tm.tmPitchAndFamily >> 4)]) ;

	SetDlgItemInt  (hdlg, IDC_TM_CHARSET,   pdp->tm.tmCharSet, FALSE) ;
	SetDlgItemText (hdlg, IDC_TM_FACENAME, pdp->szFaceName) ;

	vaSetDlgItemText(hdlg, IDC_REFRESH_COUNT, _T("#%d"), ++g_refreshcount);
}

void MySetMapMode (HDC hdc, int iMapMode)
{
	switch (iMapMode)
	{
	case IDC_MM_TEXT:       SetMapMode (hdc, MM_TEXT) ;       break ;
	case IDC_MM_LOMETRIC:   SetMapMode (hdc, MM_LOMETRIC) ;   break ;
	case IDC_MM_HIMETRIC:   SetMapMode (hdc, MM_HIMETRIC) ;   break ;
	case IDC_MM_LOENGLISH:  SetMapMode (hdc, MM_LOENGLISH) ;  break ;
	case IDC_MM_HIENGLISH:  SetMapMode (hdc, MM_HIENGLISH) ;  break ;
	case IDC_MM_TWIPS:      SetMapMode (hdc, MM_TWIPS) ;      break ;
	
	case IDC_MM_LOGTWIPS:
		SetMapMode (hdc, MM_ANISOTROPIC) ;
		SetWindowExtEx (hdc, 1440, 1440, NULL) ;
		SetViewportExtEx (hdc, 
			GetDeviceCaps (hdc, LOGPIXELSX), // typical: 96 or 120
			GetDeviceCaps (hdc, LOGPIXELSY), // typical: 96 or 120
			NULL) ;
		// -- Chj memo: 
		// When system DPI-scaling is 100%,  "96 pixels" is considered one inch in user's eye.
		// When system DPI-scaling is 125%, "120 pixels" is considered one inch in user's eye.
		break ;
	}
}
