#include <CHHI_DEBUG.h>

#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <Shlwapi.h>

#include <_MINMAX_.h>
#include <RECTxy.h>
#include <mswin/utils_wingui.h>

#include "utils.h"

#include "iversion.h"

#include <CHHI_vaDBG_is_vaDbgTs.h>

#ifndef DigClock2_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


WindowShaker g_winshaker;

ChimePlay g_chimeplay;


void ShowHelp(HWND hwndParent)
{
	static TCHAR *s_help_fmt =
		_T("DigClock2 by Jimm Chen. (version %d.%d.%d)\r\n")
		_T("\r\n")
		_T("This clock program works in wall-time mode or countdown mode.\r\n")
		_T("Clock digits drawing code is written by Charles Petzold [PRWIN5] Chap08.\r\n")

		_T("\r\n")
		_T("To Move the clock window:\r\n")
		_T("(1) Click and drag with mouse left button.\r\n")
		_T("(2) Use keyboard arrow keys, pixel by pixel. Press Ctrl key to accelerate.\r\n")
		_T("\r\n")
		_T("To change digit color: \r\n")
		_T("(1) Left click on the clock for next color.\r\n")
		_T("(2) Shift+click to cycle back.\r\n")
		_T("\r\n")
		_T("In countdown mode, you can use keyboard Up/Down to adjust time values.\r\n")
		_T("\r\n")
		_T("User settings are saved to %s.ini .\r\n")
		_T("\r\n")
		_T("Compiled on: ") _T(__DATE__) _T(", ") _T(__TIME__)
		;
	vaMsgBox(hwndParent, MB_OK | MB_ICONINFORMATION, _T("Help"),
		s_help_fmt, 
		THISEXE_VMAJOR, THISEXE_VMINOR, THISEXE_VBUILD,
		GetIniStemName());
}


void Hwnd_SetAlwaysOnTop(HWND hwnd, bool istop)
{
	SetWindowPos(hwnd, 
		istop? HWND_TOPMOST : HWND_NOTOPMOST,
		0,0,0,0, SWP_NOMOVE|SWP_NOSIZE
		);
}

/*
void MySaveSysDpiScaling()
{
	HDC hdc = GetDC(NULL);
	int sysdpi = GetDeviceCaps(hdc, LOGPIXELSX); // or GetDeviceCaps
	BOOL succ = ReleaseDC(NULL, hdc);
	
	g_sysdpiScaling = (double)sysdpi / 96;
	// -- This will not change without user sign-out/sign-in, even on Win10.1607+.
}
*/

int AfterDpiScale(int input)
{
	static double s_sysdpiScaling = 0;
	if (s_sysdpiScaling == 0)
	{
		HDC hdc = GetDC(NULL);
		int sysdpi = GetDeviceCaps(hdc, LOGPIXELSX); // or GetDeviceCaps
		BOOL succ = ReleaseDC(NULL, hdc);

		s_sysdpiScaling = (double)sysdpi / 96;
	}

	return (int)(input * s_sysdpiScaling);
}

#if 0
// v2.0: Instead use MoveWindow_byClientRect() and my_AdjustClientRect()
void MyAdjustClientSize(HWND hwnd, bool istitle, int cli_width, int cli_height, 
	bool isDpiScaling)
{
	// cli_width, cli_height: 
	// * If >0, This function will adjust window size so that
	//   hwnd's client area has exactly that width & height (pixels)
	// * If <=0, it keeps current client size, and only istitle counts.
	//
	// isDpiScaling: Will increase cli_width & cli_height by System DPI scaling.

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

	if(isDpiScaling)
	{
		cli_width  = int(cli_width * g_sysdpiScaling);
		cli_height = int(cli_height* g_sysdpiScaling);
	}

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

	// [2026-03-25] Need this two lines, otherwise:
	// When switching ShowDate=Yes/Timezone, the bottom bar is NOT refreshed immediately. Why?
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
}
#endif

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

Sdring Seconds_to_HMS(int seconds)
{
	// Example: Turn 63 seconds into "00:01:03"

	TCHAR szHMS[40];

	int zSeconds = seconds % 60;
	int tmp = seconds / 60;
	int zMinutes = tmp % 60;
	int zHours = (tmp / 60) % 100;

	_sntprintf_s(szHMS, _TRUNCATE, _T("%02d:%02d:%02d"), zHours, zMinutes, zSeconds);
	return szHMS;
}

int HMS_to_Seconds(const TCHAR *szHMS, bool error_msgbox)
{
	// Strip leading spaces.
	const TCHAR *pszHMS = szHMS;
	while(*pszHMS==' ')
		pszHMS++;

	// Turn "00:01:03" into 63 seconds.
	// -1 on error.
	if(! (pszHMS[2]==':' && pszHMS[5]==':' && pszHMS[8]=='\0') )
	{
		if(error_msgbox)
		{
			vaMsgBox(NULL, MB_OK | MB_ICONWARNING, _T(APPNAME),
				_T("Time format error:\r\n\r\n%s"), pszHMS);
		}
		return -1;
	}

	int zHours=0, zMinutes=0, zSeconds=0;
	_stscanf_s(pszHMS, _T("%02d:%02d:%02d"), &zHours, &zMinutes, &zSeconds);

	int seconds = (zHours*60+zMinutes) * 60 + zSeconds;
	return seconds;
}

const CInterpretConst& itc__ClockMode()
{
	// This function returns the single s_itcClockMode object.
	//
	// Note: I cannot define s_itcClockMode as real C++ global object, because
	// another global object(DataXString_AutoSaveIni<bool> g_ClockMode)'s ctor refers
	// to s_itcClockMode. C++ global-object initializer cannot guarantee which one
	// in constructed first.

	static const Enum2Val_st _e2v_ClockMode[] =
	{
		ITC_NAMEPAIR(CM_WallTime),
		ITC_NAMEPAIR(CM_Countdown),
	};
	static CInterpretConst s_itcClockMode(_e2v_ClockMode, ITCF_SINT);
	return s_itcClockMode;
}

//////////////////////////////////////////////////////////////////////////

void WindowShaker::TimerCallback()
{
	// Generate random value between [-nudge_max , +nudge_max]

	int nudgeX = m_nudge_max - rand() % (m_nudge_max * 2);
	int nudgeY = m_nudge_max - rand() % (m_nudge_max * 2);

	RECT rcnew = m_rcOrigWin;
	OffsetRect(&rcnew, nudgeX, nudgeY);

	MoveWindow(m_hwnd, RECT_disXYWH(rcnew), FALSE);
}

void WindowShaker::TimerOffCallback()
{
	MoveWindow(m_hwnd, RECT_disXYWH(m_rcOrigWin), FALSE);
}


void WindowShaker::ShakeStart(HWND hwnd, int nudge_max, 
	int interval_millisec, int duration_millisec)
{
	nudge_max = _MAX_(1, nudge_max);
	interval_millisec = _MAX_(10, interval_millisec);
	if(duration_millisec>=0)
		duration_millisec = _MAX_(100, duration_millisec);

	m_hwnd = hwnd;
	m_nudge_max = nudge_max;

	GetWindowRect(hwnd, &m_rcOrigWin);

	if(duration_millisec>0)
		StartPeriodicWorkT(hwnd, interval_millisec, false, duration_millisec);
	else if(duration_millisec<0)
		StartPeriodicWorkN(hwnd, interval_millisec, false, -1);
}

void WindowShaker::ShakeStop()
{
	StopTimer();
}

//////////////////////////////////////////////////////////////////////////

UINT ChimePlay::SetDefaultChime(const void *ptrWavBin, int nBytes, HWND hwndToNotify)
{
// 	if (!hwndToNotify)  // mere test code
// 	{
// 		IPlaySound_RegisterHwndNotify(m_playsound, hwndToNotify);
// 		return 0;
// 	}

	assert(ptrWavBin && nBytes);
	m_ptrWavBin = ptrWavBin;
	m_nbWaveBin = nBytes;

	return IPlaySound_RegisterHwndNotify(m_playsound, hwndToNotify);
}

IPlaySound::ReCode_et 
ChimePlay::PlayOnce(Purpose_et purpose, const TCHAR *pszSoundFile)
{
	// First check if pszSoundFile repeats last call

	if(Sdring::str_match(pszSoundFile, m_soundfile))
	{
		RepeatOnce();
		return IPlaySound::E_Success;
	}
	else
	{
		IPlaySound::ReCode_et pserr = m_playsound->OpenSoundFile(pszSoundFile);
		if (pserr)
		{
			vaDBG1(_T("[DigClock2] ChimePlay::PlayOnce(): OpenSoundFile(\"%s\") fails with %s"),
				pszSoundFile, ITCS(pserr, itc::IPlaySound_ReCode));
			return pserr;
		}

		pserr = m_playsound->PlayOnce();
		if (pserr)
		{
			vaDBG1(_T("[DigClock2] ChimePlay::PlayOnce(): PlayOnce(\"%s\") fails with %s"),
				pszSoundFile, ITCS(pserr, itc::IPlaySound_ReCode));
			return pserr;
		}

		m_purpose = purpose;
		m_soundfile = pszSoundFile;

		return IPlaySound::E_Success;
	}
}

void ChimePlay::RepeatOnce()
{
	if (!m_playsound->IsOpened())
	{
		assert(m_ptrWavBin);
		if(!m_ptrWavBin)
			return;

		IPlaySound::ReCode_et pserr = 
			m_playsound->OpenWavBin(m_ptrWavBin, m_nbWaveBin);

		if (pserr)
		{
			vaDBG1(_T("[DigClock2] ChimePlay::RepeatOnce(): OpenSoundBin() fails with %s"),
				ITCS(pserr, itc::IPlaySound_ReCode));
			return;
		}
	}

	m_playsound->PlayOnce();
}

void ChimePlay::PlayStop()
{
	m_playsound->Stop();

	m_purpose = None;
}




#ifndef DigClock2_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif
