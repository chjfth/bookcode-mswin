#ifndef __utils_h_
#define __utils_h_

#include "targetver.h"
#include <windows.h>
#include <tchar.h>
#include <sdring.h>
#include <commdefs.h>

#include <vaDbgTs.h>
#include <vaDbgTs_util.h>
using namespace vaDbgTs_util;
#include <utils_env.h>
#include <CHwndTimer.h>
#include <mswin/FlexiInfobox.h>

#include <InterpretConst.h>
using namespace itc;

#include <mswin/IPlaySound_mswin.h>
#include <mswin/Tooltip-helper.h>

#include <DataXIni.h>
#include <mswin/MenuTracker.h>

#define APPNAME "DigClock2"


Sdring Seconds_to_HMS(int seconds);
int HMS_to_Seconds(const TCHAR *szHMS, bool error_msgbox = false);


enum ClockMode_et { CM_WallTime = 0, CM_Countdown = 1 };

struct Format_int_as_HHMMSS {};
struct Format_COLORREF_as_RGB {};

#include "datax.h" // should place it after `ClockMode_et` definition

extern const CInterpretConst& itc__ClockMode();

//extern Sdrings g_chime_filepaths;

//extern int g_seconds_remain;

//extern CTooltipSimple g_tooltip;

//////////////////////////////////////////////////////////////////////////

void ShowHelp(HWND hwndParent);

void Hwnd_SetAlwaysOnTop(HWND hwnd, bool istop);

//void MySaveSysDpiScaling();
int AfterDpiScale(int input);

//void MyAdjustClientSize(HWND hwnd, bool istitle, int cli_width=-1, int cli_height=-1,
//	bool isDpiScaling=false);

bool Is_MouseInClientRect(HWND hwnd);

void MoveWindow_byOffset(HWND hwnd, int offsetx, int offsety);

inline const TCHAR* GetIniStemName()
{
	return GetExeStemname();
}


class WindowShaker : public CHwndTimer
{
public:
	void ShakeStart(HWND hwnd, int nudge_max, int interval_millisec, int duration_millisec);
	void ShakeStop();

protected:
	virtual void TimerCallback() cxx11_override;
	virtual void TimerOffCallback() cxx11_override;

private:
	HWND m_hwnd;
	RECT m_rcOrigWin; // original window position
	int m_nudge_max;
};


class ChimePlay  // singleton
{
public:
	enum Purpose_et
	{
		None = 0,
		TimeDue = 1,
		SndPreview = 2,
	};

	ChimePlay()
	{
		m_playsound = IPlaySound::CreateObj();
	}

	~ChimePlay()
	{
		delete m_playsound;
	}

	// DigClock2 UI code should call it first.
	UINT SetDefaultChime(const void *ptrWavBin, int nBytes, HWND hwndToNotify); 

	IPlaySound::ReCode_et PlayOnce(Purpose_et purpose, const TCHAR *pszSoundFile);
	// -- If pszSoundFile==nullptr, then play the default chime.

	bool IsPlaying();

	void RepeatOnce();

	void PlayStop();

	Purpose_et GetPurpose() { return m_purpose; }

private:
	IPlaySound *m_playsound = nullptr;

	Purpose_et m_purpose = None;

	Sdring m_soundfile;

	const void *m_ptrWavBin = nullptr;
	int m_nbWaveBin = 0;
};

//extern ChimePlay g_chimeplay;


//Sdrings chime_list_GetValue(); // get value from g_chime_list
//void chime_list_SetValue(const Sdrings& chime_list); // set value to g_chime_list

//void AddNewFiles_to_ChimeList_SingleDir(HWND hwnd, const Sdrings &ss);

//void AddNewFiles_to_ChimeList(HWND hwnd, const Sdrings &ss);

extern const TCHAR * const g_audio_extnames[];
extern const int n_audio_extnames;

//extern CMenuTracker g_menu_tracker;


class CAppWin;          // Defined in AppWin.h
class CDlgCountdownCfg; // Defined in DlgCountdownCfg.h

//void CAppWin_StartCountdownSeconds(CAppWin *appwin); // todo modify name,param

Sdring CAppWin_GetCountDownCfg(); // Returns sth like "00:01:00"
bool CAppWin_StartCountDown(const TCHAR *cfghms); // cfghms like "00:01:00"


struct GlobalPtrs_st
{
	CAppWin *appwin;
	DataXIni *xini;

	HWND hdlgFloatbar;

	ChimePlay *chimeplay;
};
extern GlobalPtrs_st gp;


#endif
