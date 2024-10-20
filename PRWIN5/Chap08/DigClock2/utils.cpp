#include "utils.h"

const TCHAR *GetExeFilename()
{
	static TCHAR exepath[MAX_PATH] = _T("Unknown exepath");
	GetModuleFileName(NULL, exepath, ARRAYSIZE(exepath));

	const TCHAR *pfilename = StrRChr(exepath, NULL, _T('\\'));
	if(pfilename && pfilename[1])
		pfilename++;
	else
		pfilename = exepath;

	return pfilename;
}

void Hwnd_SetAlwaysOnTop(HWND hwnd, bool istop)
{
	SetWindowPos(hwnd, 
		istop? HWND_TOPMOST : HWND_NOTOPMOST,
		0,0,0,0, SWP_NOMOVE|SWP_NOSIZE
		);
}

void Hwnd_ShowTitle(HWND hwnd, bool istitle, int cli_width, int cli_height)
{
	// cli_width, cli_height: If >0, This function will adjust window size so that
	// hwnd's client area has exactly that width & height (pixels)

	struct StyleBits 
	{ 
		DWORD bits_on; DWORD bits_on_ex; 
	} 
	twownds[2] =
	{
		{ WS_POPUPWINDOW|WS_THICKFRAME , WS_EX_DLGMODALFRAME }, // style bits for no-title window
		{ WS_OVERLAPPEDWINDOW , WS_EX_TOOLWINDOW }, // style bits for has-title window
	};

	int cliw_inc = 0, clih_inc = 0;

	// Save original client-area absolute position first.
	//
	RECT rectAbsCli = {}; // client-area absolute position(screen coordinate)
	GetClientRect(hwnd, &rectAbsCli); // interim result
	//
	if(cli_width>0)
		cliw_inc = cli_width - rectAbsCli.right;
	if(cli_height>0)
		clih_inc = cli_height - rectAbsCli.bottom;
	//
	MapWindowPoints(hwnd, HWND_DESKTOP, (POINT*)&rectAbsCli, 2);

	DWORD winstyle = (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE);
	winstyle &= (~twownds[!istitle].bits_on);
	winstyle |= twownds[istitle].bits_on;
	SetWindowLongPtr(hwnd, GWL_STYLE, winstyle);

	DWORD winstyleEx = (DWORD)GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	winstyleEx &= (~twownds[!istitle].bits_on_ex);
	winstyleEx |= twownds[istitle].bits_on_ex;

	SetWindowLongPtr(hwnd, GWL_EXSTYLE, winstyleEx);

	// (must) Repaint the window frame, so that we can calculate its *new* border size.
	SetWindowPos(hwnd, NULL, 0,0,0,0, 
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	// Now move the window so to keep client-area position & size intact.
	// We determine final whole-window position by adding window-border size to rectCliAbs.
	//
	RECT rectNewFrame = {};
	GetWindowRect(hwnd, &rectNewFrame); // interim result
	//RECT rectCliInFrame = GetClientAreaPosiz(hwnd);
	RECT rectNewCli = {};
	GetClientRect(hwnd, &rectNewCli); // interim result
	MapWindowPoints(hwnd, HWND_DESKTOP, (POINT*)&rectNewCli, 2);
	//
	rectNewFrame.left += (rectAbsCli.left - rectNewCli.left);
	rectNewFrame.top += (rectAbsCli.top - rectNewCli.top);
	rectNewFrame.right += (rectAbsCli.right - rectNewCli.right);
	rectNewFrame.bottom += (rectAbsCli.bottom - rectNewCli.bottom);

	SetWindowPos(hwnd, NULL, 
		rectNewFrame.left, 
		rectNewFrame.top, 
		rectNewFrame.right-rectNewFrame.left + cliw_inc, 
		rectNewFrame.bottom-rectNewFrame.top + clih_inc,
		SWP_NOZORDER | SWP_FRAMECHANGED
		);
}

bool Is_MouseInClientRect(HWND hwnd)
{
	POINT mpt = {};
	GetCursorPos(&mpt);
	ScreenToClient(hwnd, &mpt);

	RECT rccli = {};
	GetClientRect(hwnd, &rccli);
	if(PtInRect(&rccli, mpt))
		return true;
	else 
		return false;

}

void MoveWindow_byOffset(HWND hwnd, int offsetx, int offsety)
{
	RECT oldrect = {};
	GetWindowRect(hwnd, &oldrect);
	MoveWindow(hwnd, oldrect.left+offsetx, oldrect.top+offsety, 
		oldrect.right-oldrect.left, oldrect.bottom-oldrect.top, TRUE);
}

const TCHAR* Seconds_to_HMS(int seconds)
{
	// Turn 63 seconds into "00:01:03"

	static TCHAR szHMS[40];

	int zSeconds = seconds % 60;
	int tmp = seconds / 60;
	int zMinutes = tmp % 60;
	int zHours = (tmp / 60) % 100;

	_sntprintf_s(szHMS, _TRUNCATE, _T("%02d:%02d:%02d"), zHours, zMinutes, zSeconds);
	return szHMS;
}

int HMS_to_Seconds(const TCHAR *szHMS)
{
	// Strip leading spaces.
	const TCHAR *pszHMS = szHMS;
	while(*pszHMS==' ')
		pszHMS++;

	// Turn "00:01:03" into 63 seconds.
	// -1 on error.
	if(! (pszHMS[2]==':' && pszHMS[5]==':') )
	{
		vaMsgBox(NULL, MB_OK|MB_ICONWARNING, _T(APPNAME),
			_T("Time format error:\r\n\r\n%s"), pszHMS);
		return -1;
	}

	int zHours=0, zMinutes=0, zSeconds=0;
	_stscanf_s(pszHMS, _T("%02d:%02d:%02d"), &zHours, &zMinutes, &zSeconds);

	int seconds = (zHours*60+zMinutes) * 60 + zSeconds;
	return seconds;
}

