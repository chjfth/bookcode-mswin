#include "utils.h"

bool g_isDbg = false;
double g_sysdpiScaling = 1.0;

void myDbg(const TCHAR *fmt, ...)
{
	if(g_isDbg)
	{
		va_list args;
		va_start(args, fmt);
		vlDbgTs(fmt, args);
		va_end(args);
	}
}

const TCHAR *GetExeFilename()
{
	static TCHAR exepath[MAX_PATH] = _T("Unknown exepath");
	GetModuleFileName(NULL, exepath, ARRAYSIZE(exepath));

	const TCHAR *pfilename = StrRChr(exepath, NULL, _T('\\'));
	if(pfilename && pfilename[1])
		pfilename++;
	else
		pfilename = exepath;

	return pfilename;
}

void Hwnd_SetAlwaysOnTop(HWND hwnd, bool istop)
{
	SetWindowPos(hwnd, 
		istop? HWND_TOPMOST : HWND_NOTOPMOST,
		0,0,0,0, SWP_NOMOVE|SWP_NOSIZE
		);
}

void MySaveSysDpiScaling()
{
	HDC hdc = GetDC(NULL);
	int sysdpi = GetDeviceCaps(hdc, LOGPIXELSX); // or GetDeviceCaps
	BOOL succ = ReleaseDC(NULL, hdc);
	
	g_sysdpiScaling = (double)sysdpi / 96;
	// -- This will not change without user sign-out/sign-in, even on Win10.1607+.
}

void MyAdjustClientSize(HWND hwnd, bool istitle, int cli_width, int cli_height, 
	bool isDpiScaling)
{
	// cli_width, cli_height: 
	// * If >0, This function will adjust window size so that
	//   hwnd's client area has exactly that width & height (pixels)
	// * If <=0, it keeps current client size, and only istitle counts.
	//
	// isDpiScaling: Will increase cli_width & cli_height by System DPI scaling.

	struct StyleBits 
	{ 
		DWORD bits_on; DWORD bits_on_ex; 
	} 
	twownds[2] =
	{
		{ WS_POPUPWINDOW|WS_THICKFRAME , WS_EX_DLGMODALFRAME }, // style bits for no-title window
		{ WS_OVERLAPPEDWINDOW , WS_EX_TOOLWINDOW }, // style bits for has-title window
	};

	int cliw_inc = 0, clih_inc = 0;

	if(isDpiScaling)
	{
		cli_width  = int(cli_width * g_sysdpiScaling);
		cli_height = int(cli_height* g_sysdpiScaling);
	}

	// Save original client-area absolute position first.
	//
	RECT rectAbsCli = {}; // client-area absolute position(screen coordinate)
	GetClientRect(hwnd, &rectAbsCli); // interim result
	//
	if(cli_width>0)
		cliw_inc = cli_width - rectAbsCli.right;
	if(cli_height>0)
		clih_inc = cli_height - rectAbsCli.bottom;
	//
	MapWindowPoints(hwnd, HWND_DESKTOP, (POINT*)&rectAbsCli, 2);

	DWORD winstyle = (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE);
	winstyle &= (~twownds[!istitle].bits_on);
	winstyle |= twownds[istitle].bits_on;
	SetWindowLongPtr(hwnd, GWL_STYLE, winstyle);

	DWORD winstyleEx = (DWORD)GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	winstyleEx &= (~twownds[!istitle].bits_on_ex);
	winstyleEx |= twownds[istitle].bits_on_ex;

	SetWindowLongPtr(hwnd, GWL_EXSTYLE, winstyleEx);

	// (must) Repaint the window frame, so that we can calculate its *new* border size.
	SetWindowPos(hwnd, NULL, 0,0,0,0, 
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	// Now move the window so to keep client-area position & size intact.
	// We determine final whole-window position by adding window-border size to rectCliAbs.
	//
	RECT rectNewFrame = {};
	GetWindowRect(hwnd, &rectNewFrame); // interim result
	//RECT rectCliInFrame = GetClientAreaPosiz(hwnd);
	RECT rectNewCli = {};
	GetClientRect(hwnd, &rectNewCli); // interim result
	MapWindowPoints(hwnd, HWND_DESKTOP, (POINT*)&rectNewCli, 2);
	//
	rectNewFrame.left += (rectAbsCli.left - rectNewCli.left);
	rectNewFrame.top += (rectAbsCli.top - rectNewCli.top);
	rectNewFrame.right += (rectAbsCli.right - rectNewCli.right);
	rectNewFrame.bottom += (rectAbsCli.bottom - rectNewCli.bottom);

	SetWindowPos(hwnd, NULL, 
		rectNewFrame.left, 
		rectNewFrame.top, 
		rectNewFrame.right-rectNewFrame.left + cliw_inc, 
		rectNewFrame.bottom-rectNewFrame.top + clih_inc,
		SWP_NOZORDER | SWP_FRAMECHANGED
		);
}

bool Is_MouseInClientRect(HWND hwnd)
{
	POINT mpt = {};
	GetCursorPos(&mpt);
	ScreenToClient(hwnd, &mpt);

	RECT rccli = {};
	GetClientRect(hwnd, &rccli);
	if(PtInRect(&rccli, mpt))
		return true;
	else 
		return false;

}

void MoveWindow_byOffset(HWND hwnd, int offsetx, int offsety)
{
	RECT oldrect = {};
	GetWindowRect(hwnd, &oldrect);
	MoveWindow(hwnd, oldrect.left+offsetx, oldrect.top+offsety, 
		oldrect.right-oldrect.left, oldrect.bottom-oldrect.top, TRUE);
}

const TCHAR* Seconds_to_HMS(int seconds)
{
	// Turn 63 seconds into "00:01:03"

	static TCHAR szHMS[40];

	int zSeconds = seconds % 60;
	int tmp = seconds / 60;
	int zMinutes = tmp % 60;
	int zHours = (tmp / 60) % 100;

	_sntprintf_s(szHMS, _TRUNCATE, _T("%02d:%02d:%02d"), zHours, zMinutes, zSeconds);
	return szHMS;
}

int HMS_to_Seconds(const TCHAR *szHMS)
{
	// Strip leading spaces.
	const TCHAR *pszHMS = szHMS;
	while(*pszHMS==' ')
		pszHMS++;

	// Turn "00:01:03" into 63 seconds.
	// -1 on error.
	if(! (pszHMS[2]==':' && pszHMS[5]==':') )
	{
		vaMsgBox(NULL, MB_OK|MB_ICONWARNING, _T(APPNAME),
			_T("Time format error:\r\n\r\n%s"), pszHMS);
		return -1;
	}

	int zHours=0, zMinutes=0, zSeconds=0;
	_stscanf_s(pszHMS, _T("%02d:%02d:%02d"), &zHours, &zMinutes, &zSeconds);

	int seconds = (zHours*60+zMinutes) * 60 + zSeconds;
	return seconds;
}



////////////////////////////////////////////////////////////////////

#define MY_FILTER_MSG0(hwnd, message, fn)    \
    case (message): \
	{ \
		MsgRelay_et is_relay = FILTER_##message((hwnd), (wParam), (lParam), (fn)); \
		if(is_relay) \
			break; \
		else \
			return 0; \
	}

enum MsgRelay_et { Relay_no=0, Relay_yes=1 };

/* MsgRelay_et Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) */
#define FILTER_WM_KEYDOWN(hwnd, wParam, lParam, fn) \
    (fn)((hwnd), (UINT)(wParam), TRUE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam))

/* MsgRelay_et Edit_OnMouseMove(HWND hEdit, int x, int y, UINT keyFlags) */
#define FILTER_WM_MOUSEMOVE(hwnd, wParam, lParam, fn) \
	(fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam))

/* MsgRelay_et Cls_OnNCDestroy(HWND hwnd) */
#define FILTER_WM_NCDESTROY(hwnd, wParam, lParam, fn) \
    (fn)(hwnd)


////////////////////////////////////////////////////////////////////

enum { HEXMAGIC = 0x20241024 };
enum { SZFMT_MAX_= 8 };
enum { MaxUpDownDigits = 6 };

struct EditCustProp_st
{
	UINT hexmagic;
	WNDPROC wndproc_was;
	int min_val;
	int max_val;
	bool is_wrap_around;
	bool is_pad_0; // "5" becomes "05" or "005" etc, according to hot length

	bool is_cleanup_ready;
	bool is_mouse_hidden;

	EditCustProp_st(WNDPROC wndproc_was,
		int min_val, int max_val, bool is_wrap_around, bool is_pad_0)
	{
		this->hexmagic = HEXMAGIC;
		this->wndproc_was = wndproc_was;
		this->min_val = min_val;
		this->max_val = max_val;
		this->is_wrap_around = is_wrap_around;
		this->is_pad_0 = is_pad_0;

		this->is_cleanup_ready = false;
		this->is_mouse_hidden = false;
	}

	void HideMouse()
	{
		// When user starts adjusting numbers with keyboard, I'll hide the mouse 
		// temporarily so that mouse cursor does not obscure the ticking numbers.
		if(! is_mouse_hidden) {
			ShowCursor(FALSE);
			is_mouse_hidden = true;
		}
	}

	void ShowMouse()
	{
		if(is_mouse_hidden) {
			ShowCursor(TRUE);
			is_mouse_hidden = false;
		}
	}
};

// Key used to store the original WndProc in the window's property-list
const TCHAR* MYEDITBOX_PROP_STR = _T("WndProc_before_EnableUpDownKeyAdjustNumber");

static bool is_all_number_digits(const TCHAR *p, int nchars)
{
	int i;
	for(i=0; i<nchars; i++)
	{
		if(! isdigit(p[i]))
			return false;
	}
	return true;
}

static MsgRelay_et Edit_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	HWND hEdit = hwnd;

	// We only process WM_KEYDOWN of Up/Down key.
	// If current caret is on a number, we will increase/decrease the number,
	// otherwise, do nothing and relay the message(to its old wndproc).
	
	if(!fDown)
		return Relay_yes;

	bool is_inc = false;
	if(vk==VK_UP)
		is_inc = true;
	else if(vk==VK_DOWN)
		is_inc = false;
	else
		return Relay_yes;

	// Get editbox text length.
	
	const int TBUFSIZE = 100;
	TCHAR szText[TBUFSIZE] = {};
	Edit_GetText(hEdit, szText, TBUFSIZE-1);
	int textlen = (int)_tcslen(szText);
	if(textlen<=0)
		return Relay_yes;
	
	// Send EM_GETSEL to retrieve the selection range (or caret position if no selection)
	int startPos = 0, endPos = 0;
	SendMessage(hEdit, EM_GETSEL, (WPARAM)&startPos, (LPARAM)&endPos);

	myDbg(_T("hEdit 0x%08X: [#%d~%d) %s | %.*s"), hEdit, startPos, endPos, szText, 
		endPos-startPos, szText+startPos // the substring after |
		);

	if(startPos>endPos) // then swap, not seen this case yet
	{
		DWORD tmp = startPos;
		startPos = endPos;
		endPos = tmp;
	}

	// Now, we check two cases.
	// Case 1: If user has explicitly select(higlighted) some numbered text,
	//         we process only that selected text.
	// Case 2: If user has selected nothing, we find a whole number string
	//         around the caret and process that whole number.

	int startHot = startPos, endHot_ = endPos; // The hot section is what we operate.
	
	if(startPos<endPos) // user has explicit selection
	{
		if(! is_all_number_digits(szText+startPos, endPos-startPos))
		{
			return Relay_no;
		}
	}
	else // find number around caret
	{
		// caret pos or pos[-1] needs to be a digit
		if(! (
			isdigit(szText[startPos]) || (startPos>0 && isdigit(szText[startPos-1]))
			) ) 
		{
			myDbg(_T("Caret pos NOT on a digit, do nothing."));
			return Relay_yes;
		}
		
		// Looking left-side:
		while(startHot>0 && isdigit(szText[startHot-1]))
			startHot--;

		// Looking right-size:
		while(endHot_<textlen && isdigit(szText[endHot_]))
			endHot_++;
	}

	int hotlen = (int)(endHot_-startHot);

	TCHAR szHot[TBUFSIZE] = {};
	_sntprintf_s(szHot, _TRUNCATE, _T("%.*s"), hotlen, szText+startHot);
	
	if(hotlen<=MaxUpDownDigits)
	{
		myDbg(_T("hEdit 0x%08X: hot [@%d~%d) %s"), hEdit,	startHot, endHot_, szHot);
	}
	else
	{
		myDbg(_T("hEdit 0x%08X: bad [@%d~%d) %s (exceed %d)"), hEdit, startHot, endHot_, szHot, MaxUpDownDigits);
		return Relay_yes;
	}

	// Now we increase/decrease the hot number.

	EditCustProp_st *myprop = (EditCustProp_st*)GetProp(hEdit, MYEDITBOX_PROP_STR);
	
	int numHot = _tcstoul(szHot, 0, 10);
	int newHot = is_inc ? numHot+1 : numHot-1;
	if(newHot > myprop->max_val)
	{
		newHot = myprop->is_wrap_around ? myprop->min_val : myprop->max_val;
	}
	else if(newHot < myprop->min_val)
	{
		newHot = myprop->is_wrap_around ? myprop->max_val : myprop->min_val;
	}

	assert(hotlen>0);
	TCHAR szFmt[20] = _T("%d");
	if(myprop->is_pad_0)
		_sntprintf_s(szFmt, _TRUNCATE, _T("%%0%ud"), hotlen);
	
	TCHAR szNewHot[TBUFSIZE] = {};
	const TCHAR *pszNewHot = szNewHot; // may adjust
	_sntprintf_s(szNewHot, _TRUNCATE, szFmt, newHot);
	int hotlenv = (int)_tcslen(szNewHot); // v: (this len could be) verbose

	if(hotlenv > hotlen)
	{
		// For example, there is "52" at carent, but user select only "5"(hotlen==1) and then increase it.
		// Then, when hotstring goes from 5,6,7... and reaches "10", we should chop off the "1"
		// and preserve only the "0", bcz strlen("10") has exceeded hotlen.
		pszNewHot = szNewHot + hotlenv - hotlen;
	}

	myDbg(_T("    %s : %s -> %s"), is_inc?_T("INC"):_T("DEC"), szHot, pszNewHot);

	TCHAR szNewText[TBUFSIZE] = {};
	
	_sntprintf_s(szNewText, _TRUNCATE, _T("%.*s%s%s"), 
		startHot, szText,
		pszNewHot,
		szText + endHot_);

	Edit_SetText(hEdit, szNewText);
	Edit_SetSel(hEdit, startHot, endHot_);
	
	if(myprop->is_cleanup_ready)
	{
		myprop->HideMouse();
	}
	
	return Relay_no;
}

static MsgRelay_et Edit_OnMouseMove(HWND hEdit, int x, int y, UINT keyFlags)
{
	EditCustProp_st *myprop = (EditCustProp_st*)GetProp(hEdit, MYEDITBOX_PROP_STR);

	if(! myprop->is_cleanup_ready)
	{
		// Establish WM_MOUSELEAVE tracking.
		TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hEdit};
		TrackMouseEvent(&tme);

		myprop->is_cleanup_ready = true;
	}

	return Relay_yes;
}

static MsgRelay_et Edit_OnNCDestroy(HWND hEdit)
{
	bool succ = Editbox_DisableUpDownKeyAdjustNumber(hEdit);
    assert(succ);
	return Relay_yes;
}

// Custom window procedure for the edit control
static LRESULT CALLBACK Edit_MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hEdit = hwnd;
	
    // Retrieve the original WndProc from the window's properties
	//
	EditCustProp_st *myprop = (EditCustProp_st*)GetProp(hEdit, MYEDITBOX_PROP_STR);
    WNDPROC oldWndProc = myprop->wndproc_was;
	bool succ = false;

    switch (uMsg)
	{
		MY_FILTER_MSG0(hwnd, WM_KEYDOWN, Edit_OnKey);
		MY_FILTER_MSG0(hwnd, WM_MOUSEMOVE, Edit_OnMouseMove);
    	MY_FILTER_MSG0(hwnd, WM_NCDESTROY, Edit_OnNCDestroy);
	
	case WM_MOUSELEAVE: 
		myprop->ShowMouse();
		myprop->is_cleanup_ready = false;
		break;
	}

    // Call the original WndProc for default processing
    return CallWindowProc(oldWndProc, hEdit, uMsg, wParam, lParam);
}


bool Editbox_EnableUpDownKeyAdjustNumber(HWND hEdit,
	int min_val, int max_val, bool is_wrap_around, bool is_pad_0)
{
	// Purpose: When the caret is a number, user pressing Up will increase the number,
	// pressing Down will decrease the number. This is convenient for adjusting
	// date/time value on an editbox.
		
	EditCustProp_st *myprop = new EditCustProp_st(nullptr,
		min_val, max_val, is_wrap_around, is_pad_0);
	if(!myprop)
		return false;

    WNDPROC oldWndProc = SubclassWindow(hEdit, Edit_MyWndProc);

	myprop->wndproc_was = oldWndProc;
	
    BOOL succ = SetProp(hEdit, MYEDITBOX_PROP_STR, myprop); // Store custom props into hEdit.
	if(succ)
		return true;
	else
	{	// restore old state
		SubclassWindow(hEdit, oldWndProc);
		delete myprop;
		return false;
	}

	return true;
}

bool Editbox_DisableUpDownKeyAdjustNumber(HWND hEdit)
{
	WNDPROC currentWndProc = (WNDPROC)GetWindowLongPtr(hEdit, GWLP_WNDPROC);
	EditCustProp_st *myprop = (EditCustProp_st*)GetProp(hEdit, MYEDITBOX_PROP_STR);

	if(!myprop)
		return false;

	if(currentWndProc != Edit_MyWndProc)
	{
		// Someone else has further subclassed this hEdit, so we should not unsubclass it.
		return false;
	}

	assert(myprop->hexmagic==HEXMAGIC);
	assert(myprop->wndproc_was);

	// Restore original WndProc for editbox
	SubclassWindow(hEdit, myprop->wndproc_was);

	// Remove our custom wnd-prop
	RemoveProp(hEdit, MYEDITBOX_PROP_STR);

	// Destroy our custom struct
	delete myprop;
	
	return true;
}

