/*------------------------------------------------------------
   GuiHasRc.cpp -- As template for PRWIN5 bookcode projects.
  ------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <windows.h>

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PTSTR szParams, int iCmdShow)
{
	MessageBox(NULL, TEXT("Hello."), TEXT("Hello"), MB_OK);
	return 0;
}
