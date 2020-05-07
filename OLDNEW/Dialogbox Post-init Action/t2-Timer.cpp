#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

INT_PTR CALLBACK
DlgProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uiMsg) {
  
  case WM_INITDIALOG:
    SetTimer(hwnd, 1, 1, 0);
    Sleep(500); //simulate paging
    return TRUE;
  
  case WM_TIMER:
    if (wParam == 1) {
      KillTimer(hwnd, 1);
      MessageBox(hwnd,
                IsWindowVisible(hwnd) ? TEXT("Visible") : TEXT("Not Visible"),
                TEXT("t2-Timer"), MB_OK);
    }
    break;
  case WM_CLOSE:
   EndDialog(hwnd, 0);
   break;
  }
  return FALSE;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev,
                   LPSTR lpCmdLine, int nShowCmd)
{
    DialogBox(hinst, MAKEINTRESOURCE(1), NULL, DlgProc);
    return 0;
}

/* Compiling commands:

rc box.rc
cl /Od /Zi t2-Timer.cpp /link /debug user32.lib gdi32.lib box.res 

*/
