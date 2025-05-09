#ifndef __utils_h_
#define __utils_h_

#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <Shlwapi.h>

#include "vaDbg.h"
#include "..\..\BeginPaint_NoFlicker.h"


#define APPNAME "DigClock2"

#ifdef __cplusplus
extern"C"{
#endif


#define HANDLE_dlgMSG(hwnd, message, fn) \
  case (message): \
  return SetDlgMsgResult( hwnd, message, HANDLE_##message((hwnd), (wParam), (lParam), (fn)) );
	// For message processing in a WinAPI user's dialog-procedure, we need a further step
	// beyond that of windowsx.h's HANDLE_MSG(). This HANDLE_dlgMSG() applies that further step.
	// Ref: Raymond Chen https://devblogs.microsoft.com/oldnewthing/20031107-00/?p=41923


const TCHAR *GetExeFilename();

void Hwnd_SetAlwaysOnTop(HWND hwnd, bool istop);

void MySaveSysDpiScaling();

void MyAdjustClientSize(HWND hwnd, bool istitle, int cli_width=-1, int cli_height=-1,
	bool isDpiScaling=false);

bool Is_MouseInClientRect(HWND hwnd);

void MoveWindow_byOffset(HWND hwnd, int offsetx, int offsety);

const TCHAR* Seconds_to_HMS(int seconds);
int HMS_to_Seconds(const TCHAR *szHMS);


#ifdef __cplusplus
} // extern"C"{
#endif

#endif
