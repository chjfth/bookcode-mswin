/* 2020.05.07 Chj: This program uses myDynDlgbox() to create the main dialog programatically.

Compile it with:

	cl /Od /Zi t1-PostMessage-DynBox.cpp /link /debug user32.lib gdi32.lib
 
Compared to t1-PostMessage.cpp, the child dialog-box will show "Visible", why?

*/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

INT_PTR CALLBACK
DlgProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uiMsg) {
  case WM_INITDIALOG:
    PostMessage(hwnd, WM_APP, 0, 0);
    return TRUE;
  case WM_APP:
    MessageBox(hwnd,
              IsWindowVisible(hwnd) ? TEXT("Visible") : TEXT("Not Visible"),
              TEXT("Title"), MB_OK);
    break;
  case WM_CLOSE:
   EndDialog(hwnd, 0);
   break;
  }
  return FALSE;
}

INT_PTR myDynDlgbox(HINSTANCE hinstExe)
{
    char memblock[4000];

    // Prepare DLG template header:

    DLGTEMPLATE &dt = *(DLGTEMPLATE*)memblock;
    dt.style = DS_FIXEDSYS | WS_MINIMIZEBOX | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
//    dt.style |= DS_SETFONT;
    dt.dwExtendedStyle = 0;
    dt.cdit = 1; // count of child controls
    dt.x=0, dt.y=0, dt.cx=140, dt.cy=55;

    // menu, class, title

    WORD *pword = (WORD*)(memblock+sizeof(DLGTEMPLATE));
    *pword++ = 0x0000; // this dlgbox no menu

    *pword++ = 0x0000; // use system default dlgbox "window class"

    const WCHAR szTitle[] = L"ABC"; // set title to "ABC"
    wcscpy((wchar_t*)pword, szTitle);
    pword += sizeof(szTitle)/sizeof(szTitle[0]);

    // DLGITEMTEMPLATE elements

    if((INT_PTR)pword & 0x3)
        *pword++ = 0; // must align it on DWORD boundary

    DLGITEMTEMPLATE *pitem = (DLGITEMTEMPLATE*)pword;

    // button
    pitem->style = WS_VISIBLE;
    pitem->dwExtendedStyle = 0;
    pitem->x=49, pitem->y=36, pitem->cx=50, pitem->cy=14;
    pitem->id = IDOK;
    //
    pword = (WORD*)(pitem+1);
    *pword++ = 0xFFFF; *pword++ = 0x0080; // ctrl class is button
    wcscpy((wchar_t*)pword, L"[OK]"); pword+=5; // button text
    *pword++ = 0x0000; // no creation data


    const WCHAR *mystr = L"My private string";
    INT_PTR dlgret =  DialogBoxIndirectParam(hinstExe, &dt, NULL, DlgProc, (LPARAM)mystr);
    return dlgret;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev,
                   LPSTR lpCmdLine, int nShowCmd)
{
    myDynDlgbox(hinst);
    return 0;
}
