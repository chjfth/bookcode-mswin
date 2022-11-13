#pragma once

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

void DumpSID(PSID psid, CPrintBuf* pbufToken);

void DumpSIDAttributes(DWORD dwAttrib, CPrintBuf* pbufToken);

BOOL DumpTokenGroups(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenPrivileges(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenOwner(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenPrimaryGroup(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenDefaultDacl(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenSource(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenImpersonationLevel(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenType(HANDLE hToken, CPrintBuf* pbufToken);

BOOL DumpTokenRestrictedSids(HANDLE hToken, CPrintBuf* pbufToken);

void DumpToken();




