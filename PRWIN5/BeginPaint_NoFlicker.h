#ifndef __BeginPaint_NoFlicker_h_
#define __BeginPaint_NoFlicker_h_

HDC BeginPaint_NoFlicker(HWND hwnd, PAINTSTRUCT *lpPaint);

BOOL EndPaint_NoFlicker(HWND hwnd, PAINTSTRUCT *lpPaint);


#endif
