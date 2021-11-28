#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

const TCHAR *ErrStrDispChange(LONG err)
{
	struct ErrMap_st {
		LONG code;
		const TCHAR *str;
	} errmap[] = {

		{ DISP_CHANGE_RESTART , _T("DISP_CHANGE_RESTART") },
		{ DISP_CHANGE_FAILED  , _T("DISP_CHANGE_FAILED") },
		{ DISP_CHANGE_BADMODE , _T("DISP_CHANGE_BADMODE") },
		{ DISP_CHANGE_NOTUPDATED, _T("DISP_CHANGE_NOTUPDATED") },
		{ DISP_CHANGE_BADFLAGS, _T("DISP_CHANGE_BADFLAGS") },
		{ DISP_CHANGE_BADPARAM, _T("DISP_CHANGE_BADPARAM") },
		{ DISP_CHANGE_BADDUALVIEW, _T("DISP_CHANGE_BADDUALVIEW") },
	};

	int i;
	for(i=0; i<ARRAYSIZE(errmap); i++)
	{
		if(errmap[i].code==err)
			return errmap[i].str;
	}

	static TCHAR errmsg[100];
	_sntprintf_s(errmsg, ARRAYSIZE(errmsg), 
		_T("Unknown DISP_CHANGE_xxx return value %l"), err);
	return errmsg;
}

void Set_256ColorMode(const TCHAR *szAppname)
{
	DEVMODE dm = {sizeof(dm)};
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, & dm); // query current
	if( dm.dmBitsPerPel==8 )
		return; // already in 8-bpp
	
	int mbret = MessageBox(NULL, 
		_T("Current video mode is not 256-color.\r\n")
		_T("\r\n")
		_T("To achieve demonstration purpose, you should change it to 256-color.\r\n")
		_T("\r\n")
		_T("Do it now?\r\n")
		,
		szAppname,
		MB_YESNO
		);
	if(mbret!=IDYES)
		return;

	dm.dmBitsPerPel = 8;
	LONG result = ChangeDisplaySettings(&dm, 0);
	if(result==DISP_CHANGE_SUCCESSFUL)
		return;

	TCHAR errmsg[100];
	_sntprintf_s(errmsg, ARRAYSIZE(errmsg),
		_T("EnumDisplaySettings() returns error: %s"),  ErrStrDispChange(result));

	MessageBox(NULL, errmsg, _T("Fail setting 256-color video mode"), MB_OK|MB_ICONERROR);
	ExitProcess(4);
}

