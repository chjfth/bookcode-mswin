#ifndef __utils_h_
#define __utils_h_

#include "targetver.h"
#include <windows.h>
#include <tchar.h>
#include <sdring.h>
#include <commdefs.h>

#include <vaDbgTs.h>
#include <vaDbgTs_util.h>
using namespace vaDbgTs_util;
#include <utils_env.h>
#include <CHwndTimer.h>

#include <InterpretConst.h>
using namespace itc;

enum ClockMode_et { CM_WallTime = 0, CM_Countdown = 1 };

extern const CInterpretConst& itc__ClockMode();


#define APPNAME "DigClock2"


void ShowHelp(HWND hwndParent);

void Hwnd_SetAlwaysOnTop(HWND hwnd, bool istop);

//void MySaveSysDpiScaling();
int AfterDpiScale(int input);

//void MyAdjustClientSize(HWND hwnd, bool istitle, int cli_width=-1, int cli_height=-1,
//	bool isDpiScaling=false);

bool Is_MouseInClientRect(HWND hwnd);

void MoveWindow_byOffset(HWND hwnd, int offsetx, int offsety);

Sdring Seconds_to_HMS(int seconds);
int HMS_to_Seconds(const TCHAR *szHMS, bool error_msgbox=false);

inline const TCHAR* GetIniStemName()
{
	return GetExeStemname();
}


class WindowShaker : public CHwndTimer
{
public:
	void ShakeStart(HWND hwnd, int nudge_max, int interval_millisec, int duration_millisec);
	void ShakeStop();

protected:
	virtual void TimerCallback() cxx11_override;
	virtual void TimerOffCallback() cxx11_override;

private:
	HWND m_hwnd;
	RECT m_rcOrigWin; // original window position
	int m_nudge_max;
};

extern WindowShaker g_winshaker;

#endif
