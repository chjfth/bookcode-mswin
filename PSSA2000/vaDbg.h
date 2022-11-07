#ifndef __vaDbg_h_
#define __vaDbg_h_

#include <tchar.h>

void vaDbg(const TCHAR *fmt, ...);

void vaSetDlgItemText(HWND hdlg, int ctrlid, const TCHAR *fmt, ...);

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);

void chSETWINDOWICON(HWND hwnd, const TCHAR *icon_resname);

int vaMsgBox(UINT utype, const TCHAR *szfmt, ...);

const TCHAR *app_WinErrStr(DWORD winerr);

TCHAR *parse_cmdparam_TCHARs(
	const TCHAR *T_cmdline, bool single_param_as_literal,
	TCHAR outbuf[], int outbuflen, int *p_retlen,
	TCHAR out_szliteral[], int out_szliteral_buflen);

//// PSSA2000 specific:

#define PSIDFromPACE(pACE) ((PSID)(&((pACE)->SidStart))) // CH10.2

void CH10_DumpACL( PACL pACL );

//////////////////////////////////////////////////////////////////////////

#include "InterpretConst.h"

extern CInterpretConst itc_CSecurityInformation_PropertySheetPageCallback_uMsg;
extern CInterpretConst itc_CSecurityInformation_PropertySheetPageCallback_uPage;
extern CInterpretConst itc_SECURITY_INFORMATION;
extern CInterpretConst itc_SE_OBJECT_TYPE;
extern CInterpretConst itc_ACE_TYPE;
extern CInterpretConst itc_ACE_FLAGS;

#endif
