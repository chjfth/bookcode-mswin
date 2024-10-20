#ifndef __utils_h_
#define __utils_h_

#include <tchar.h>
#include <windows.h>
#include <Shlwapi.h>

#include "..\..\vaDbg.h"
#include "..\..\BeginPaint_NoFlicker.h"


#define APPNAME "DigClock2"


const TCHAR *GetExeFilename();

void Hwnd_SetAlwaysOnTop(HWND hwnd, bool istop);

void Hwnd_ShowTitle(HWND hwnd, bool istitle, int cli_width=-1, int cli_height=-1);

bool Is_MouseInClientRect(HWND hwnd);

void MoveWindow_byOffset(HWND hwnd, int offsetx, int offsety);

const TCHAR* Seconds_to_HMS(int seconds);
int HMS_to_Seconds(const TCHAR *szHMS);


#endif
