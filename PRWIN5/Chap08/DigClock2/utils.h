#ifndef __utils_h_
#define __utils_h_

#include "targetver.h"
#include <windows.h>
#include <tchar.h>
#include <sdring.h>

#include <vaDbgTs.h>
#include <vaDbgTs_util.h>
using namespace vaDbgTs_util;
#include <utils_env.h>

#include <InterpretConst.h>
using namespace itc;

enum ClockMode_et { CM_WallTime = 0, CM_Countdown = 1 };

extern CInterpretConst& get_itcClockMode();


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


#endif
