#ifndef __utils_h_
#define __utils_h_

#include "targetver.h"
#include <windows.h>
#include <tchar.h>

#include <vaDbgTs.h>
#include <vaDbgTs_util.h>
using namespace vaDbgTs_util;


#define APPNAME "DigClock2"

#ifdef __cplusplus
extern"C"{
#endif


void ShowHelp(HWND hwndParent);

void Hwnd_SetAlwaysOnTop(HWND hwnd, bool istop);

//void MySaveSysDpiScaling();
int AfterDpiScale(int input);

//void MyAdjustClientSize(HWND hwnd, bool istitle, int cli_width=-1, int cli_height=-1,
//	bool isDpiScaling=false);

bool Is_MouseInClientRect(HWND hwnd);

void MoveWindow_byOffset(HWND hwnd, int offsetx, int offsety);

const TCHAR* Seconds_to_HMS(int seconds);
int HMS_to_Seconds(const TCHAR *szHMS);



#ifdef __cplusplus
} // extern"C"{
#endif

#endif
