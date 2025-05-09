#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// In real life, this would be an instance variable
BOOL g_fShown = FALSE;

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg) {
	case WM_INITDIALOG:
		return TRUE;

	case WM_SHOWWINDOW: // wParam==TRUE if the window is being shown
		if (wParam && !g_fShown) {
			g_fShown = TRUE;
			MessageBox(hwnd,
				IsWindowVisible(hwnd) ? TEXT("Visible") : TEXT("Not Visible"),
				TEXT("t3-WM_SHOWWINDOW"), MB_OK);
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
cl /Od /Zi t3-WM_SHOWWINDOW.cpp /link /debug user32.lib gdi32.lib box.res 

*/
