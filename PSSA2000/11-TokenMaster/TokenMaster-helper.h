#pragma once

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

BOOL EnablePrivilege(PTSTR szPriv, BOOL fEnabled);

HANDLE OpenSystemProcess();

BOOL ModifySecurity(HANDLE hProc, DWORD dwAccess);

HANDLE GetLSAToken();

BOOL RunAsUser(PTSTR pszEXE, PTSTR pszUserName, PTSTR pszPassword, PTSTR pszDesktop);

BOOL TryRelaunch();

BOOL GetUserSID(PSID psid, BOOL fAllowImpersonate, PDWORD pdwSize);


void Status(PTSTR szStatus, DWORD dwLastError);

PVOID AllocateTokenInfo(HANDLE hToken, TOKEN_INFORMATION_CLASS tokenClass);

void UpdatePrivileges();

void UpdateGroups();

void RefreshSnapShot();

void PopulateProcessCombo();

void PopulateThreadCombo();

void PopulateStaticCombos();

void GetToken(HWND hwnd);

BOOL GetAccountName(HWND hwnd, PTSTR szBuf, DWORD dwSize, BOOL fAllowGroups,
	BOOL fAllowUsers);



