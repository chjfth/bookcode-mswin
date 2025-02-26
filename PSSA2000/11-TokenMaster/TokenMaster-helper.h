#pragma once

#include "../chjutils/ch10-DumpSD.h"

extern HWND g_hwndProcessCombo;
extern HWND g_hwndThreadCombo;
extern HWND g_hwndToken;
extern HWND g_hwndStatus;
extern HWND g_hwndEnablePrivileges;
extern HWND g_hwndEnableGroups;
extern HWND g_hwndDeletedPrivileges;
extern HWND g_hwndDisabledSids;
extern HWND g_hwndRestrictedSids;
extern HWND g_hwndLogonTypes;
extern HWND g_hwndLogonProviders;
extern HWND g_hwndImpersonationLevels;
extern HWND g_hwndTokenTypes;

typedef DWORD WinError_t;

BOOL myEnablePrivilege(PTSTR szPriv, BOOL fEnabled);

HANDLE myOpenSystemProcess();

BOOL myModifySecurity(HANDLE hProc, DWORD dwAccess,
	FUNC_InterpretRights procItr=nullptr, void *userctx=0);

HANDLE myGetLSAToken();

BOOL myRunAsUser(PTSTR pszEXE, PTSTR pszUserName, PTSTR pszPassword, PTSTR pszDesktop);

BOOL TryRelaunch();

BOOL myGetUserSID(PSID psid, BOOL fAllowImpersonate, PDWORD pdwSize);


void RefreshStatus(PCTSTR szStatus, DWORD dwLastError);

PVOID myAllocateTokenInfo(HANDLE hToken, TOKEN_INFORMATION_CLASS tokenClass);

void guiUpdatePrivileges();

void guiUpdateGroups();

void RefreshSnapShot();

void guiPopulateProcessCombo();

void guiPopulateThreadCombo();

void guiPopulateStaticCombos();

void guiGetToken(HWND hwnd);

BOOL GetAccountName(HWND hwnd, PTSTR szBuf, DWORD dwSize, BOOL fAllowGroups,
	BOOL fAllowUsers);



