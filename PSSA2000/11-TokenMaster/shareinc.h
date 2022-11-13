#pragma once 

#include "..\CmnHdr.h"           /* See Appendix A. */

#include <WindowsX.h>
#include <AclAPI.h>
#include <AclUI.h>
#include <ObjBase.h>
#include <ObjSel.h>
#include <TlHelp32.h>

#include "..\ClassLib\PrintBuf.h"      // See Appendix B.

#include "DumpInfo.h"

#define DIVIDERL  TEXT("****************************************") \
	TEXT("****************************************\r\n")

#define DIVIDERS  TEXT("----------------------------------------") \
	TEXT("----------------------------------------\r\n")


extern HANDLE g_hSnapShot;
extern HANDLE g_hToken;
extern HWND g_hwndToken;

extern HWND g_hwndRestrictedSids;


void UpdatePrivileges();
void UpdateGroups();

PVOID AllocateTokenInfo(HANDLE hToken, TOKEN_INFORMATION_CLASS tokenClass);

BOOL GetTextualSid(PSID pSid, PTSTR TextualSid, PDWORD pdwBufferLen);

