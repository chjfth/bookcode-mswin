#pragma once 

#include "..\CmnHdr.h"           /* See Appendix A. */

#include <WindowsX.h>
#include <Shlobj.h> // for IsUserAnAdmin()
#include <AclAPI.h>
#include <AclUI.h>
#include <ObjBase.h>
#include <ObjSel.h>
#include <TlHelp32.h>
#include <intsafe.h>

#include "..\ClassLib\PrintBuf.h"      // See Appendix B.

#include <mswin/WinError.itc.h>
using namespace itc;

#include "DumpInfo.h"

#define DIVIDERL  TEXT("********************************************************************************\r\n")

#define DIVIDERS  TEXT("--------------------------------------------------------------------------------\r\n")


extern HANDLE g_hSnapShot;
extern HANDLE g_hToken;

//extern HWND g_hwndToken;


//BOOL GetTextualSid(PSID pSid, PTSTR TextualSid, PDWORD pdwBufferLen);

